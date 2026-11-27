// port/sdk/d3d: the Direct3D 12 backend of the GPU seam (gpu_backend.h), Windows only. Host C++ (halo3_gpu): no
// exceptions, RTTI or dynamic initializers; the C library through gpu_crt.h; Direct3D 12 and DXGI through d3d12_mini.h,
// with d3d12.dll and dxgi.dll opened at run time (no import of either: a machine without them starts, and the registry
// falls back to the other renderer).
//
// What it maps the seam onto:
//   - the universal Xenos layout = the shader pack's root signature (shader_pack_xenos_root_signature): bind group 0 the
//     six root CBVs b0-b5 (dynamic offsets are GPU virtual address offsets), bind group 1 the SRV table t0-t127 and the
//     sampler table s128-s319 (space1), gpu_cmd_xenos_bind_memory the raw-buffer tables t0-t15 / u0-u3 (space2);
//   - descriptors: views and samplers are made once in CPU-only heaps; a bind group's SRVs are copied into 128-slot
//     ranges of one shader-visible CBV/SRV/UAV heap (cached groups in chunks that outlive frames, frame groups and the
//     per-draw tables in a linear region per frame slot); its samplers are placed into the 2048-slot shader-visible
//     sampler heap by content, sharing slots (see s_sampler_heap);
//   - resource states: every texture's state is tracked and changed by explicit barriers, batched before the command
//     that needs them; a default-heap buffer starts each command list in COMMON (buffers decay after every
//     ExecuteCommandLists), is promoted implicitly on its first use in the list and changed by barriers after that;
//   - pipelines: D3D12_GRAPHICS / COMPUTE_PIPELINE_STATE_DESC from the neutral state, kept in an ID3D12PipelineLibrary
//     that persists as pipeline_cache_d3d12.bin (gpu_get_pipeline_cache);
//   - what Direct3D 12 cannot do with a copy command (scaled or converting blits, partial depth / stencil copies, the
//     letterboxed present) through d3d12_copy.hlsl's graphics passes, and the six compute helpers through their DXIL.
// Diagnostics: HALO3_D3D12_DEBUG=1 turns the debug layer on (2: also GPU-based validation) and logs its messages;
// HALO3_D3D12_DEVICE=<n> picks an adapter; HALO3_D3D12_WARP=1 allows the WARP software adapter.
#ifdef _WIN32

#include "gpu_backend.h"
#include "gpu_crt.h"
#include "d3d12_mini.h"
#include "../../shaders/shader_pack.h"
#include "../../shaders/xenos_spirv_abi.h"
#include "../kernel/win32_host.h"
#include "../runtime/port_runtime.h"

#include "edram_7e3_dxil.h"
#include "pwl_convert_dxil.h"
#include "depth_pack_dxil.h"
#include "display_gamma_dxil.h"
#include "resolve_scale_dxil.h"
#include "resolve_downscale_dxil.h"
#include "d3d12_copy_dxil.h"

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(int, FreeLibrary, 4, (void *module));
PORT_HOST_IMPORT(void *, CreateEventA, 16, (void *attributes, int manual_reset, int initial_state, const char *name));
PORT_HOST_IMPORT(uint32_t, WaitForSingleObject, 8, (void *handle, uint32_t milliseconds));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void, AcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(void, ReleaseSRWLockExclusive, 4, (void **lock));

namespace {

using namespace d3d12;

// ---- sizes ----

const uint32_t k_none = 0xffffffffu;
const uint32_t k_group_srvs = XE_BINDING_SAMPLER;                                // t0-t127 of bind group 1
const uint32_t k_group_samplers = 6u * XE_SAMPLER_VARIANT_STRIDE;                // s128-s319
const uint32_t k_memory_views = XE_D3D12_VERTEX_STREAMS + XE_D3D12_MEMEXPORT_BUFFERS; // tables 8 and 9
const uint32_t k_cached_groups = 1024;           // the frontend's cached texture sets at most (k_xe_texture_set_cap)
const uint32_t k_frame_descriptors = 131072;     // per frame slot: frame groups, memory tables, pass tables
const uint32_t k_static_descriptors = 256;        // the null tables bound where nothing was (static_null_tables)
const uint32_t k_sampler_heap_size = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;
const uint32_t k_occlusion_queries = 4096;        // per frame: the heap's queries (a record takes one a rendering)
const uint32_t k_occlusion_records = 1024;        // per frame: gpu_occlusion_begin's records
const uint32_t k_occlusion_record_queries = 8;    // the renderings one record counts (more go uncounted)
const uint32_t k_staging_ring_bytes = 16u << 20;  // per frame: deferred uploads
const uint32_t k_max_texture_images = 96;         // images one texture group samples (the frontend's write limit)
const uint32_t k_back_buffers_max = GPU_FRAMES_MAX + 1u; // the swap chain: one more than the frames in flight
const uint32_t k_rtv_ring = 64;                   // transient render target / depth views (consumed when recorded)

// ---- small containers (plain data) ----

template <typename T> struct s_array {
    T *items;
    uint32_t count, capacity;
    T *push()
    {
        if (count == capacity) {
            const uint32_t grown = capacity != 0 ? capacity * 2u : 16u;
            T *more = (T *)realloc(items, grown * sizeof(T));
            if (more == 0)
                return 0;
            items = more;
            capacity = grown;
        }
        memset(&items[count], 0, sizeof(T));
        return &items[count++];
    }
    void release()
    {
        free(items);
        items = 0;
        count = capacity = 0;
    }
};

// ---- formats ----

enum e_aspect {
    k_aspect_color = 1,
    k_aspect_depth = 2,
    k_aspect_stencil = 4,
};

struct s_format {
    DXGI_FORMAT format;   // the view / target format (UNKNOWN: not an image format here)
    DXGI_FORMAT resource; // the resource's (typeless for sampled depth / stencil)
    DXGI_FORMAT depth;    // depth plane SRV
    DXGI_FORMAT stencil;  // stencil plane SRV
    uint8_t mapping[4];   // which DXGI component holds the format's R, G, B, A (the packed 16-bit formats)
    uint8_t aspect;
};

#define F_COLOR(f) { f, f, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, { 0, 1, 2, 3 }, k_aspect_color }
#define F_NONE { DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, { 0, 1, 2, 3 }, 0 }
const s_format k_formats[] = {
    F_NONE,
    F_COLOR(DXGI_FORMAT_R8G8B8A8_UNORM), F_COLOR(DXGI_FORMAT_R8G8B8A8_SNORM), F_NONE, F_NONE,
    F_COLOR(DXGI_FORMAT_R8G8B8A8_UINT), F_COLOR(DXGI_FORMAT_R8G8B8A8_SINT), F_COLOR(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
    F_COLOR(DXGI_FORMAT_B8G8R8A8_UNORM), F_NONE, F_NONE, F_NONE, F_NONE, F_NONE, F_COLOR(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB),
    F_COLOR(DXGI_FORMAT_R10G10B10A2_UNORM), F_NONE, F_NONE, F_NONE, F_COLOR(DXGI_FORMAT_R10G10B10A2_UINT), F_NONE,
    F_COLOR(DXGI_FORMAT_R16G16_UNORM), F_COLOR(DXGI_FORMAT_R16G16_SNORM), F_NONE, F_NONE,
    F_COLOR(DXGI_FORMAT_R16G16_UINT), F_COLOR(DXGI_FORMAT_R16G16_SINT), F_COLOR(DXGI_FORMAT_R16G16_FLOAT),
    F_COLOR(DXGI_FORMAT_R16G16B16A16_UNORM), F_COLOR(DXGI_FORMAT_R16G16B16A16_SNORM), F_NONE, F_NONE,
    F_COLOR(DXGI_FORMAT_R16G16B16A16_UINT), F_COLOR(DXGI_FORMAT_R16G16B16A16_SINT),
    F_COLOR(DXGI_FORMAT_R16G16B16A16_FLOAT),
    F_COLOR(DXGI_FORMAT_R32_UINT), F_COLOR(DXGI_FORMAT_R32_SINT), F_COLOR(DXGI_FORMAT_R32_FLOAT),
    F_COLOR(DXGI_FORMAT_R32G32_UINT), F_COLOR(DXGI_FORMAT_R32G32_SINT), F_COLOR(DXGI_FORMAT_R32G32_FLOAT),
    F_COLOR(DXGI_FORMAT_R32G32B32_UINT), F_COLOR(DXGI_FORMAT_R32G32B32_SINT), F_COLOR(DXGI_FORMAT_R32G32B32_FLOAT),
    F_COLOR(DXGI_FORMAT_R32G32B32A32_UINT), F_COLOR(DXGI_FORMAT_R32G32B32A32_SINT),
    F_COLOR(DXGI_FORMAT_R32G32B32A32_FLOAT),
    F_COLOR(DXGI_FORMAT_R8_UNORM), F_COLOR(DXGI_FORMAT_R8_SNORM),
    F_COLOR(DXGI_FORMAT_R8G8_UNORM), F_COLOR(DXGI_FORMAT_R8G8_SNORM),
    F_COLOR(DXGI_FORMAT_R16_UNORM), F_COLOR(DXGI_FORMAT_R16_SNORM), F_COLOR(DXGI_FORMAT_R16_FLOAT),
    // the packed 16-bit formats name their components from the most significant bit (Vulkan's), DXGI's from the
    // least: b4g4r4a4 is DXGI_FORMAT_B4G4R4A4_UNORM with R = its G, G = its R, B = its A, A = its B; b5g6r5 is
    // DXGI_FORMAT_B5G6R5_UNORM with R and B exchanged; a1r5g5b5 is DXGI_FORMAT_B5G5R5A1_UNORM bit for bit
    { DXGI_FORMAT_B4G4R4A4_UNORM, DXGI_FORMAT_B4G4R4A4_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, { 1, 0, 3, 2 },
      k_aspect_color },
    { DXGI_FORMAT_B5G6R5_UNORM, DXGI_FORMAT_B5G6R5_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, { 2, 1, 0, 3 },
      k_aspect_color },
    F_COLOR(DXGI_FORMAT_B5G5R5A1_UNORM),
    F_COLOR(DXGI_FORMAT_BC1_UNORM), F_COLOR(DXGI_FORMAT_BC1_UNORM_SRGB),
    F_COLOR(DXGI_FORMAT_BC2_UNORM), F_COLOR(DXGI_FORMAT_BC2_UNORM_SRGB),
    F_COLOR(DXGI_FORMAT_BC3_UNORM), F_COLOR(DXGI_FORMAT_BC3_UNORM_SRGB),
    F_COLOR(DXGI_FORMAT_BC4_UNORM), F_COLOR(DXGI_FORMAT_BC5_UNORM), F_COLOR(DXGI_FORMAT_BC5_SNORM),
    { DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
      DXGI_FORMAT_X24_TYPELESS_G8_UINT, { 0, 0, 0, 0 }, k_aspect_depth | k_aspect_stencil },
    { DXGI_FORMAT_D32_FLOAT_S8X24_UINT, DXGI_FORMAT_R32G8X24_TYPELESS, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
      DXGI_FORMAT_X32_TYPELESS_G8X24_UINT, { 0, 0, 0, 0 }, k_aspect_depth | k_aspect_stencil },
};
#undef F_COLOR
#undef F_NONE
static_assert(sizeof(k_formats) / sizeof(k_formats[0]) == k_gpu_format_count, "a DXGI format per e_gpu_format");

const s_format &format_of(uint32_t format)
{
    return k_formats[format < k_gpu_format_count ? format : 0u];
}

bool format_is_bc(uint32_t format) { return format >= k_gpu_format_bc1_rgba_unorm && format <= k_gpu_format_bc5_snorm; }

// The format a block-compressed image is kept in when the device cannot make it at its size (unaligned top level and
// no UnalignedBlockTexturesSupported): its blocks are decoded on upload (decode_bc_blocks).
uint32_t bc_decoded_format(uint32_t format)
{
    switch (format) {
    case k_gpu_format_bc1_rgba_srgb:
    case k_gpu_format_bc2_srgb:
    case k_gpu_format_bc3_srgb: return k_gpu_format_r8g8b8a8_srgb;
    case k_gpu_format_bc4_unorm: return k_gpu_format_r8_unorm;
    case k_gpu_format_bc5_unorm: return k_gpu_format_r8g8_unorm;
    case k_gpu_format_bc5_snorm: return k_gpu_format_r8g8_snorm;
    default: return k_gpu_format_r8g8b8a8_unorm;
    }
}

// ---- objects ----

struct s_d3d12;

// A CPU-only descriptor heap allocator (views, samplers, render target and depth views): pages of descriptors, a free
// list of indices. Descriptors written here are copied into the shader-visible heaps (bind groups) or consumed when a
// command is recorded (render targets).
struct s_descriptor_pool {
    D3D12_DESCRIPTOR_HEAP_TYPE type;
    uint32_t page_size;
    uint32_t increment;
    s_array<ID3D12DescriptorHeap *> pages;
    s_array<D3D12_CPU_DESCRIPTOR_HANDLE> starts;
    s_array<uint32_t> free_list;
    uint32_t next; // the next never-used index
};

// Buffer states per command-list kind: a buffer decays to COMMON when each ExecuteCommandLists finishes, so its state
// is the one in the list being recorded of that kind (the frame's list, the frame's upload list, an immediate list).
enum e_list_kind {
    k_list_frame = 0,
    k_list_upload = 1,
    k_list_immediate = 2,
    k_list_kinds = 3,
};

struct s_buffer {
    ID3D12Resource *resource;
    D3D12_GPU_VIRTUAL_ADDRESS address;
    uint32_t size;       // as asked for
    uint32_t allocated;  // the resource's
    uint32_t usage;      // e_gpu_buffer_usage
    uint32_t memory;     // e_gpu_memory_usage
    uint8_t *mapped;     // upload / readback: persistently mapped
    uint32_t state[k_list_kinds];
    uint32_t serial[k_list_kinds]; // the list (s_d3d12::list_serial) the state belongs to
    uint32_t uploads;
    uint32_t upload_serial;   // the frame whose upload list last filled it (its later uploads go there too)
    uint32_t srv, uav;   // raw views (CPU descriptor indices), k_none
    uint32_t pool_frame; // in the pool of retired buffers since this frame serial
};

struct s_image;

// A shader resource view (gpu_view): its CPU descriptor and the image it reads.
struct s_view {
    s_image *image;
    uint32_t descriptor;
};

// Image states. A texture keeps its state across command lists (no decay: every change is an explicit barrier), so
// two are tracked: `state`, what it is after the commands recorded so far (the frame's and its upload list's), and
// `submitted`, what it is after everything submitted (what a one-off submission, which runs before the recording
// frame, finds and must leave it in).
struct s_image {
    ID3D12Resource *resource;
    s_gpu_image_desc desc;    // as made (defaults filled in)
    uint32_t internal_format; // the e_gpu_format the resource holds (desc.format, or bc_decoded_format)
    uint32_t decode_bc;       // uploads decode its blocks into internal_format
    uint32_t aspect;          // e_aspect
    uint32_t flags;           // D3D12_RESOURCE_FLAGS
    uint32_t state;           // D3D12_RESOURCE_STATES (all subresources together)
    uint32_t submitted;
    uint32_t touched_serial;  // the frame whose lists changed `state` (in its s_frame::touched)
    uint32_t used;            // a command has used it (an upload can no longer run ahead of the frame)
    uint32_t upload_serial;   // the frame whose upload list last filled it (its later uploads go there too)
    s_view view;              // its own view (desc.view_type, desc.swizzle)
    s_view sampled_view;      // depth images: the depth plane, RRRR
    s_view stencil_view;      // depth images: the stencil plane in .x
    s_view raw_view;          // a 2D view of level 0 layer 0 without swizzle (blit and helper sources)
    s_view array_view;        // made on first use
    s_view swizzled_view;     // made on first use
    uint32_t swizzled_swizzle;
    uint32_t rtv, dsv, uav;   // CPU descriptors of level 0 layer 0 (k_none when not allowed)
};

struct s_sampler {
    uint32_t descriptor; // CPU descriptor index
    uint32_t id;         // unique for the backend's life (sampler heap placement compares these)
};

struct s_shader {
    const void *code;
    size_t size;
    uint32_t digest[4]; // the container's (container_digest): pipeline keys name shaders by it
};

enum e_layout_kind {
    k_layout_xenos = 0,
    k_layout_helper = 1,
    k_layout_copy = 2,
};

enum e_bind_layout_kind {
    k_bind_constants = 0,
    k_bind_textures = 1,
};

struct s_bind_layout {
    uint32_t kind;
};

struct s_pipeline_layout {
    ID3D12RootSignature *root;
    uint32_t kind; // e_layout_kind
};

struct s_pso_entry;

struct s_pipeline {
    s_pso_entry *entry;         // its pipeline state object (shared: pso_get)
    s_pipeline_layout *layout;
    uint32_t bind_point;        // e_gpu_bind_point
    D3D_PRIMITIVE_TOPOLOGY topology;
    uint32_t stencil_reference;
    uint32_t uses_blend_factor; // OMSetBlendFactor matters
    uint32_t alpha_splat;       // constant alpha through the blend factor: its alpha in every component
    float blend_factor[4];
};

// A bind group: group 0 (constants) keeps its six buffers; group 1 (textures) a 128-slot range of the shader-visible
// view heap and the samplers placed into the sampler heap at bind time.
struct s_group {
    uint32_t kind;        // e_bind_layout_kind
    uint32_t lifetime;    // e_gpu_bind_lifetime
    uint32_t frame_serial;
    s_buffer *buffers[XE_BINDING_VERTEX_FETCH + 1];
    uint32_t offsets[XE_BINDING_VERTEX_FETCH + 1];
    uint32_t srv_base;    // index in the view heap (k_none: a transient group not copied into this frame yet)
    uint32_t chunk;       // its chunk (cached / persistent texture groups), k_none
    uint32_t transient;   // a cached / persistent group that got no chunk: its views are copied into the region of
                          // each frame that binds it (frame_serial: the last), from sources
    uint32_t sources[k_group_srvs]; // the view written to each slot (its CPU pool descriptor), k_none
    uint32_t image_count;
    s_image *images[k_max_texture_images];
    uint32_t sampler_count;
    uint8_t sampler_slot[k_max_texture_images];
    uint32_t sampler_id[k_max_texture_images];
    uint32_t sampler_descriptor[k_max_texture_images];
    uint32_t sampler_generation; // the sampler heap generation it is placed in (0: none)
    uint32_t sampler_base;
};

// The shader-visible sampler heap: 2048 slots, filled by content. A group's samplers go to slots base + slot (the
// table s128-s319 starts at base); a slot holding another group's sampler of the same id is shared, an empty one is
// written. Slots are never overwritten while the heap is current (a copied descriptor stays valid after its sampler is
// destroyed: ids are never reused), so every placed group stays valid. When a group fits nowhere, the next heap
// becomes current (a new generation: SetDescriptorHeaps on the recording list) and groups placed in the old one are
// placed again when bound; the old heap is reused once the frames that bound it are done.
struct s_sampler_heap {
    ID3D12DescriptorHeap *heap;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu;
    uint32_t generation;
    uint32_t used_serial;  // the last frame that may read it
    uint32_t filled;
    uint32_t ids[D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE];
};

enum e_retired_kind {
    k_retired_buffer = 0,
    k_retired_image,
    k_retired_group,   // a bind group (its descriptor chunk freed with it)
    k_retired_sampler, // a sampler's CPU descriptor (index)
    k_retired_object,  // any other COM object (released)
};

struct s_retired {
    uint32_t kind;
    void *object;
    uint32_t index;
};

// A record counts the samples of its draws in every rendering it spans: a query of the frame's heap each, summed.
struct s_occlusion_record {
    uint32_t queries[k_occlusion_record_queries];
    uint32_t query_count;
};

struct s_frame {
    ID3D12CommandAllocator *allocator;
    ID3D12GraphicsCommandList *list;
    ID3D12CommandAllocator *upload_allocator;
    ID3D12GraphicsCommandList *upload_list;
    uint32_t upload_recording;
    uint32_t serial;          // the frame's serial; the frame fence reaches it when the frame is done
    s_array<s_retired> retired;
    s_array<s_group *> groups; // frame-lifetime groups
    s_array<s_image *> touched; // images whose state its lists changed (s_image::submitted at submission)
    uint32_t view_base, view_used;
    // memory tables (gpu_cmd_xenos_bind_memory) made this frame, by content: draws binding the same buffers share one
    uintptr_t memory_keys[256][k_memory_views];
    uint32_t memory_bases[256];
    s_buffer *staging;        // the deferred uploads' ring (upload heap)
    uint32_t staging_used;
    // occlusion queries
    ID3D12QueryHeap *occlusion_heap;
    s_buffer *occlusion_readback;
    s_occlusion_record *records; // k_occlusion_records
    uint32_t occlusion_records;
    uint32_t occlusion_queries;  // the heap's queries used
    uint32_t results_serial;
    uint32_t result_count;
    uint64_t *results;
    // timestamps
    ID3D12QueryHeap *timestamp_heap;
    s_buffer *timestamp_readback;
    uint32_t timestamps_written;
    uint32_t acquired_image;
};

// What one bind point (graphics, compute) of the recording list has, and what the frontend asked for.
struct s_bind_state {
    ID3D12RootSignature *root;       // set on the list
    const s_pipeline *pipeline;      // the frontend's
    D3D12_GPU_VIRTUAL_ADDRESS cbv[XE_BINDING_VERTEX_FETCH + 1];      // wanted
    D3D12_GPU_VIRTUAL_ADDRESS cbv_set[XE_BINDING_VERTEX_FETCH + 1];  // on the list (0: unknown)
    uint64_t table[4];               // wanted: root parameters 6..9 (GPU handles), 0: none yet
    uint64_t table_set[4];
    const s_group *textures;         // the texture group bound (images to check before draws)
    s_buffer *streams[XE_D3D12_VERTEX_STREAMS];
    s_buffer *exports[XE_D3D12_MEMEXPORT_BUFFERS];
    uint32_t memory_base;            // the last memory table (view heap index), k_none
    uint32_t memory_frame;
};

struct s_copy_pass {
    s_pso_entry *entry;
    uint32_t key; // copy_pass_key: the pass, its target format and stencil bit
};

enum e_helper {
    k_helper_7e3 = 0,
    k_helper_pwl_decode,
    k_helper_pwl_encode,
    k_helper_depth_20e4,
    k_helper_depth_unorm24,
    k_helper_display_gamma,
    k_helper_resolve_scale,
    k_helper_downscale,
    k_helper_count,
};

struct s_d3d12 {
    s_gpu_backend base; // first: the seam's s_gpu_backend * is this object
    gpu_log_function log;
    void *log_context;
    void *d3d12_module, *dxgi_module, *user32_module;
    PFN_D3D12_CREATE_DEVICE create_device;
    PFN_D3D12_GET_DEBUG_INTERFACE get_debug_interface;
    PFN_CREATE_DXGI_FACTORY2 create_factory;
    int(__stdcall *get_client_rect)(HWND window, RECT *rect);
    IDXGIFactory6 *factory;  // as IDXGIFactory4 (only its methods are called through it)
    IDXGIFactory6 *factory5; // QueryInterface'd: CheckFeatureSupport; NULL without
    IDXGIFactory6 *factory6; // QueryInterface'd: EnumAdapterByGpuPreference; NULL without
    IDXGIAdapter1 *adapter;
    ID3D12Device *device;
    ID3D12Device1 *device1;
    ID3D12InfoQueue *info_queue;
    uint32_t debug;          // HALO3_D3D12_DEBUG
    ID3D12CommandQueue *queue;
    ID3D12Fence *frame_fence;     // reaches each frame's serial when it is done
    ID3D12Fence *immediate_fence; // one-off submissions
    uint64_t immediate_value;
    void *event;
    ID3D12CommandAllocator *immediate_allocator;
    ID3D12GraphicsCommandList *immediate_list;
    uint32_t immediate_recording;
    // features
    uint32_t unaligned_bc;
    uint32_t unrestricted_copy_pitch;
    uint32_t alpha_blend_factor;
    uint32_t tearing;
    uint64_t timestamp_frequency;
    // descriptors
    s_descriptor_pool views, samplers, rtvs, dsvs;
    ID3D12DescriptorHeap *view_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE view_cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE view_gpu;
    uint32_t view_increment, sampler_increment, rtv_increment, dsv_increment;
    uint32_t view_heap_size;
    uint32_t chunk_base;                // first descriptor of the group chunks
    s_array<uint32_t> free_chunks;
    uint32_t next_chunk;
    uint32_t group_chunks;              // chunks in the view heap (create_device)
    s_array<s_sampler_heap *> sampler_heaps;
    s_sampler_heap *sampler_heap;       // the current generation
    uint32_t sampler_generation;
    uint32_t sampler_ids;
    uint32_t null_srv_buffer, null_uav_buffer, null_srv_2d; // CPU view descriptors
    uint32_t null_rtv;
    uint32_t rtv_ring_first, rtv_ring_next; // k_rtv_ring render target views made at record time (transient_rtv)
    uint32_t depth_format;              // e_gpu_format of depth / stencil targets (caps.depth_stencil_format)
    s_buffer *zero_buffer;              // root CBVs nothing has set (zeros)
    // frames
    s_frame frames[GPU_FRAMES_MAX];
    uint32_t frame_count, frame_index, frame_serial;
    uint32_t recording;
    uint32_t list_serial[k_list_kinds];
    // the recording list's state
    s_bind_state bind[2];
    ID3D12PipelineState *pso_set;
    D3D_PRIMITIVE_TOPOLOGY topology_set;
    uint32_t stencil_ref_set;
    float blend_factor_set[4];
    uint32_t dynamic_valid;             // the three above are known
    uint32_t stencil_wanted;            // what the bound graphics pipeline / state asks for
    float blend_wanted[4];
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissor;
    uint32_t viewport_valid, scissor_valid;
    s_buffer *index_buffer;
    uint32_t index_offset, index_type, index_valid;
    s_array<D3D12_RESOURCE_BARRIER> barriers;
    // rendering
    uint32_t rendering, rendering_width, rendering_height;
    s_image *colors[GPU_MAX_COLOR_ATTACHMENTS];
    uint32_t color_count;
    s_image *depth;
    // occlusion
    uint32_t occlusion_active;          // record + 1 of the open record
    uint32_t occlusion_running;         // query + 1 of its query in the current rendering
    // objects
    s_array<s_buffer *> buffer_pool;
    s_array<s_group *> cached_groups;   // k_gpu_bind_cached_textures groups, until bind_groups_reset_cached
    s_array<s_pso_entry *> psos; // the pipeline state objects made, shared by content (pso_get)
    uint32_t format_support[k_gpu_format_count]; // D3D12_FORMAT_SUPPORT1 of each format (its view / target format)
    s_array<s_shader *> modules;        // per pack module (pack_xenos_module), until pack_release
    s_pipeline_layout xenos_layout;
    s_bind_layout bind_layouts[2];
    uint32_t xenos_made;
    s_pipeline_layout helper_layout, copy_layout;
    s_pso_entry *helpers[k_helper_count];
    uint32_t helper_failed[k_helper_count];
    s_array<s_copy_pass> copy_passes;
    // pipeline library (pipeline_cache_d3d12.bin)
    ID3D12PipelineLibrary *library;
    void *library_blob;
    void *library_lock;                 // SRWLOCK
    uint32_t pipelines_created;
    // stats
    uint32_t view_epoch;
    s_gpu_stats stats;
    uint32_t reported;                  // bits: one-time diagnostics
    // presentation
    HWND window;
    IDXGISwapChain3 *swapchain;
    ID3D12Resource *back_buffers[k_back_buffers_max];
    uint32_t back_buffer_states[k_back_buffers_max];
    uint32_t back_buffer_count;
    uint32_t swapchain_width, swapchain_height;
    uint32_t swapchain_flags;
    uint32_t vsync;
    uint32_t present_image;
    gpu_checkpoint_describe_function describe;
    void *describe_context;
};

s_d3d12 *self_of(s_gpu_backend *gpu) { return (s_d3d12 *)gpu; }
const s_d3d12 *self_of(const s_gpu_backend *gpu) { return (const s_d3d12 *)gpu; }

struct s_gpu_handle_value {
    uintptr_t raw;
    operator uint64_t() const { return (uint64_t)raw; }
    template <typename H> operator H *() const { return (H *)raw; }
};
template <typename T> s_gpu_handle_value handle_of(T *pointer) { return {(uintptr_t)pointer}; }
template <typename T, typename H> T *pointer_of(H handle) { return (T *)(uintptr_t)handle; }

template <typename T> void release(T *&object)
{
    if (object != 0) {
        object->lpVtbl->Release(object);
        object = 0;
    }
}

// ---- logging ----

void log_text(s_d3d12 *d, const char *format, ...)
{
    char text[1024];
    __builtin_va_list arguments;
    __builtin_va_start(arguments, format);
    int length = _vsnprintf(text, sizeof(text) - 2, format, arguments);
    __builtin_va_end(arguments);
    if (length < 0 || (size_t)length > sizeof(text) - 2)
        length = (int)(sizeof(text) - 2);
    text[length] = '\n';
    text[length + 1] = '\0';
    d->log(d->log_context, text);
}

void default_log(void *, const char *text) { port_write_error(text); }

// Logs a diagnostic once for the backend's life (bit: its bit in s_d3d12::reported).
bool report_once(s_d3d12 *d, uint32_t bit)
{
    if ((d->reported & (1u << bit)) != 0)
        return false;
    d->reported |= 1u << bit;
    return true;
}

enum e_report {
    k_report_fan = 0,
    k_report_point_fill,
    k_report_attachment_sampled,
    k_report_stream_export_alias,
    k_report_descriptors,
    k_report_sample_count,
    k_report_constant_alpha,
    k_report_restart_index,
    k_report_v2,
    k_report_library,
    k_report_copy_format,
    k_report_depth_copy,
    k_report_storage_view,
    k_report_vertex_buffer,
    k_report_layout,
    k_report_chunks,
};

const char *hresult_text(HRESULT result)
{
    switch ((uint32_t)result) {
    case 0x80070057u: return "E_INVALIDARG";
    case 0x8007000Eu: return "E_OUTOFMEMORY";
    case 0x80004005u: return "E_FAIL";
    case 0x80004002u: return "E_NOINTERFACE";
    case 0x887A0005u: return "DXGI_ERROR_DEVICE_REMOVED";
    case 0x887A0006u: return "DXGI_ERROR_DEVICE_HUNG";
    case 0x887A0007u: return "DXGI_ERROR_DEVICE_RESET";
    case 0x887A0020u: return "DXGI_ERROR_DRIVER_INTERNAL_ERROR";
    case 0x887A0004u: return "DXGI_ERROR_UNSUPPORTED";
    case 0x887A0002u: return "DXGI_ERROR_NOT_FOUND";
    case 0x887E0001u: return "D3D12_ERROR_ADAPTER_NOT_FOUND";
    case 0x887E0002u: return "D3D12_ERROR_DRIVER_VERSION_MISMATCH";
    default: return "HRESULT";
    }
}

gpu_result result_of(HRESULT result)
{
    if (SUCCEEDED(result))
        return k_gpu_ok;
    switch ((uint32_t)result) {
    case 0x8007000Eu: return k_gpu_out_of_memory;
    case 0x887A0005u:
    case 0x887A0006u:
    case 0x887A0007u:
    case 0x887A0020u: return k_gpu_device_lost;
    case 0x887A0004u: return k_gpu_unsupported;
    default: return k_gpu_error;
    }
}

// Drains the debug layer's stored messages into the log (HALO3_D3D12_DEBUG).
void drain_debug_messages(s_d3d12 *d)
{
    if (d->info_queue == 0)
        return;
    ID3D12InfoQueue *queue = d->info_queue;
    const uint64_t count = queue->lpVtbl->GetNumStoredMessages(queue);
    static const char *const k_severity[] = { "corruption", "error", "warning", "info", "message" };
    for (uint64_t i = 0; i < count && i < 256u; i++) {
        SIZE_T length = 0;
        if (FAILED(queue->lpVtbl->GetMessage(queue, i, 0, &length)) || length == 0 || length > 65536u)
            continue;
        D3D12_MESSAGE *message = (D3D12_MESSAGE *)malloc(length);
        if (message == 0)
            continue;
        if (SUCCEEDED(queue->lpVtbl->GetMessage(queue, i, message, &length)))
            log_text(d, "d3d12 debug layer: %s %u: %s",
                     (uint32_t)message->Severity < 5u ? k_severity[message->Severity] : "?", (uint32_t)message->ID,
                     message->pDescription != 0 ? message->pDescription : "");
        free(message);
    }
    queue->lpVtbl->ClearStoredMessages(queue);
}

void report_device_removed(s_d3d12 *d, const char *where)
{
    const HRESULT reason = d->device != 0 ? d->device->lpVtbl->GetDeviceRemovedReason(d->device) : E_FAIL;
    log_text(d, "d3d12: the device was removed (%s; reason %s %08x)", where, hresult_text(reason), (uint32_t)reason);
    drain_debug_messages(d);
}

// ---- descriptors ----

bool pool_init(s_d3d12 *d, s_descriptor_pool *pool, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t page_size)
{
    memset(pool, 0, sizeof(*pool));
    pool->type = type;
    pool->page_size = page_size;
    pool->increment = d->device->lpVtbl->GetDescriptorHandleIncrementSize(d->device, type);
    return pool->increment != 0;
}

uint32_t pool_alloc(s_d3d12 *d, s_descriptor_pool *pool)
{
    if (pool->free_list.count != 0)
        return pool->free_list.items[--pool->free_list.count];
    if (pool->next == pool->pages.count * pool->page_size) {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = pool->type;
        desc.NumDescriptors = pool->page_size;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ID3D12DescriptorHeap *heap = 0;
        HRESULT result = d->device->lpVtbl->CreateDescriptorHeap(d->device, &desc, IID_ID3D12DescriptorHeap, (void **)&heap);
        ID3D12DescriptorHeap **page = SUCCEEDED(result) ? pool->pages.push() : 0;
        D3D12_CPU_DESCRIPTOR_HANDLE *start = page != 0 ? pool->starts.push() : 0;
        if (start == 0) {
            if (page != 0)
                pool->pages.count--;
            release(heap);
            log_text(d, "d3d12: a CPU descriptor heap (type %u) could not be made: %s %08x", (uint32_t)pool->type,
                     hresult_text(result), (uint32_t)result);
            return k_none;
        }
        *page = heap;
        heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(heap, start);
    }
    return pool->next++;
}

void pool_free(s_descriptor_pool *pool, uint32_t index)
{
    if (index == k_none)
        return;
    if (uint32_t *slot = pool->free_list.push())
        *slot = index;
}

D3D12_CPU_DESCRIPTOR_HANDLE pool_cpu(const s_descriptor_pool *pool, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = pool->starts.items[index / pool->page_size];
    handle.ptr += (SIZE_T)(index % pool->page_size) * pool->increment;
    return handle;
}

void pool_destroy(s_descriptor_pool *pool)
{
    for (uint32_t i = 0; i < pool->pages.count; i++)
        release(pool->pages.items[i]);
    pool->pages.release();
    pool->starts.release();
    pool->free_list.release();
}

D3D12_CPU_DESCRIPTOR_HANDLE view_heap_cpu(const s_d3d12 *d, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = d->view_cpu;
    handle.ptr += (SIZE_T)index * d->view_increment;
    return handle;
}

uint64_t view_heap_gpu(const s_d3d12 *d, uint32_t index)
{
    return d->view_gpu.ptr + (uint64_t)index * d->view_increment;
}

void copy_view(s_d3d12 *d, uint32_t destination, uint32_t source)
{
    d->device->lpVtbl->CopyDescriptorsSimple(d->device, 1, view_heap_cpu(d, destination), pool_cpu(&d->views, source),
                                             D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// `count` descriptors of the recording frame's region; k_none (logged once) when it is full.
uint32_t frame_descriptors(s_d3d12 *d, uint32_t count)
{
    s_frame &frame = d->frames[d->frame_index];
    if (!d->recording || frame.view_used + count > k_frame_descriptors) {
        if (report_once(d, k_report_descriptors))
            log_text(d, "d3d12: a frame used all %u of its shader-visible descriptors (draws skipped)",
                     k_frame_descriptors);
        return k_none;
    }
    const uint32_t base = frame.view_base + frame.view_used;
    frame.view_used += count;
    return base;
}

// A 128-descriptor chunk of the group region (texture groups that outlive a frame).
uint32_t chunk_alloc(s_d3d12 *d)
{
    if (d->free_chunks.count != 0)
        return d->free_chunks.items[--d->free_chunks.count];
    if (d->next_chunk < d->group_chunks)
        return d->next_chunk++;
    if (report_once(d, k_report_chunks))
        log_text(d, "d3d12: all %u texture group ranges are in use: cached groups are copied into each frame that "
                    "binds them", d->group_chunks);
    return k_none;
}

uint32_t chunk_base(const s_d3d12 *d, uint32_t chunk) { return d->chunk_base + chunk * k_group_srvs; }

void retire(s_d3d12 *d, uint32_t kind, void *object, uint32_t index = 0);
void occlusion_resume(s_d3d12 *d, ID3D12GraphicsCommandList *list);
void occlusion_suspend(s_d3d12 *d, ID3D12GraphicsCommandList *list);

// ---- the sampler heap ----

s_sampler_heap *sampler_heap_make(s_d3d12 *d)
{
    // a heap no recording or unfinished frame may read
    const uint64_t done = d->frame_fence->lpVtbl->GetCompletedValue(d->frame_fence);
    for (uint32_t i = 0; i < d->sampler_heaps.count; i++) {
        s_sampler_heap *heap = d->sampler_heaps.items[i];
        if (heap != d->sampler_heap && heap->used_serial <= done) {
            memset(heap->ids, 0, sizeof(heap->ids));
            heap->filled = 0;
            heap->generation = ++d->sampler_generation;
            return heap;
        }
    }
    s_sampler_heap *heap = (s_sampler_heap *)calloc(1, sizeof(s_sampler_heap));
    s_sampler_heap **slot = heap != 0 ? d->sampler_heaps.push() : 0;
    if (slot == 0) {
        free(heap);
        return 0;
    }
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    desc.NumDescriptors = k_sampler_heap_size;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    const HRESULT result =
        d->device->lpVtbl->CreateDescriptorHeap(d->device, &desc, IID_ID3D12DescriptorHeap, (void **)&heap->heap);
    if (FAILED(result)) {
        log_text(d, "d3d12: a shader-visible sampler heap could not be made: %s %08x", hresult_text(result),
                 (uint32_t)result);
        d->sampler_heaps.count--;
        free(heap);
        return 0;
    }
    heap->heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(heap->heap, &heap->cpu);
    heap->heap->lpVtbl->GetGPUDescriptorHandleForHeapStart(heap->heap, &heap->gpu);
    heap->generation = ++d->sampler_generation;
    *slot = heap;
    return heap;
}

void set_descriptor_heaps(s_d3d12 *d, ID3D12GraphicsCommandList *list)
{
    ID3D12DescriptorHeap *heaps[2] = { d->view_heap, d->sampler_heap->heap };
    list->lpVtbl->SetDescriptorHeaps(list, 2, heaps);
}

ID3D12GraphicsCommandList *frame_list(s_d3d12 *d) { return d->frames[d->frame_index].list; }

// Places a group's samplers in the current sampler heap (a new generation when they fit nowhere). False when no heap
// can be had.
bool place_samplers(s_d3d12 *d, s_group *group)
{
    if (group->sampler_generation == d->sampler_heap->generation)
        return true;
    for (int attempt = 0; attempt < 2; attempt++) {
        s_sampler_heap *heap = d->sampler_heap;
        heap->used_serial = d->frame_serial;
        for (uint32_t base = 0; base + k_group_samplers <= k_sampler_heap_size; base++) {
            bool fits = true;
            for (uint32_t i = 0; i < group->sampler_count && fits; i++) {
                const uint32_t id = heap->ids[base + group->sampler_slot[i]];
                fits = id == 0 || id == group->sampler_id[i];
            }
            if (!fits)
                continue;
            for (uint32_t i = 0; i < group->sampler_count; i++) {
                const uint32_t slot = base + group->sampler_slot[i];
                if (heap->ids[slot] != 0)
                    continue;
                heap->ids[slot] = group->sampler_id[i];
                heap->filled++;
                D3D12_CPU_DESCRIPTOR_HANDLE destination = heap->cpu;
                destination.ptr += (SIZE_T)slot * d->sampler_increment;
                d->device->lpVtbl->CopyDescriptorsSimple(d->device, 1, destination,
                                                         pool_cpu(&d->samplers, group->sampler_descriptor[i]),
                                                         D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
            }
            group->sampler_generation = heap->generation;
            group->sampler_base = base;
            return true;
        }
        // full for this group: the next generation (the frames that bound this heap keep it until they are done)
        s_sampler_heap *next = sampler_heap_make(d);
        if (next == 0)
            return false;
        d->sampler_heap = next;
        next->used_serial = d->frame_serial;
        if (d->recording) {
            set_descriptor_heaps(d, frame_list(d));
            // the tables bound so far point into the old heaps' generation: bound again before the next draw
            for (s_bind_state &b : d->bind)
                memset(b.table_set, 0, sizeof(b.table_set));
        }
    }
    return false;
}

// ---- barriers ----

const uint32_t k_state_shader_read =
    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
const uint32_t k_read_states = D3D12_RESOURCE_STATE_GENERIC_READ | D3D12_RESOURCE_STATE_DEPTH_READ;

bool is_read_state(uint32_t state) { return state != 0 && (state & ~k_read_states) == 0; }

void barrier_transition(s_d3d12 *d, ID3D12Resource *resource, uint32_t before, uint32_t after)
{
    D3D12_RESOURCE_BARRIER *b = d->barriers.push();
    if (b == 0)
        return;
    b->Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    b->Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    b->Transition.pResource = resource;
    b->Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b->Transition.StateBefore = (D3D12_RESOURCE_STATES)before;
    b->Transition.StateAfter = (D3D12_RESOURCE_STATES)after;
}

void barrier_uav(s_d3d12 *d, ID3D12Resource *resource)
{
    D3D12_RESOURCE_BARRIER *b = d->barriers.push();
    if (b == 0)
        return;
    b->Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    b->Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    b->UAV.pResource = resource;
}

// Records the barriers queued from `mark` on into `list`. The frame's list takes them all (mark 0). An upload or
// one-off list takes only those its operation queued (mark: the count before): any before it are the recording
// frame's, from an operation that stopped between queueing and recording them, and stay for its next flush.
void flush_barriers(s_d3d12 *d, ID3D12GraphicsCommandList *list, uint32_t mark = 0)
{
    if (d->barriers.count <= mark)
        return;
    list->lpVtbl->ResourceBarrier(list, d->barriers.count - mark, d->barriers.items + mark);
    d->stats.barriers++;
    d->barriers.count = mark;
}

// Notes that the recording frame's lists change an image's state (its submitted state follows at frame_end).
void touch(s_d3d12 *d, s_image *image)
{
    image->used = 1;
    if (!d->recording || image->touched_serial == d->frame_serial)
        return;
    image->touched_serial = d->frame_serial;
    if (s_image **slot = d->frames[d->frame_index].touched.push())
        *slot = image;
}

// An image into `state` (whole resource) in the recording frame's lists. A read state the image is in already needs
// nothing; UNORDERED_ACCESS to itself is a UAV barrier (the writes before and after are ordered).
void image_state(s_d3d12 *d, s_image *image, uint32_t state)
{
    touch(d, image);
    if (is_read_state(state) && is_read_state(image->state))
        state |= image->state; // one command may read it two ways (sampled and depth-tested, say): both
    if (image->state == state) {
        if (state == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
            barrier_uav(d, image->resource);
        else
            d->stats.transitions_skipped++;
        return;
    }
    barrier_transition(d, image->resource, image->state, state);
    image->state = state;
}

// A default-heap buffer into `state` in the list of `kind` being recorded: its first use there is an implicit
// promotion from COMMON (no barrier), later changes are barriers. Upload and readback buffers keep their heap's state.
// UNORDERED_ACCESS to itself needs nothing here: writes one draw or dispatch makes for a later one to read are ordered
// by gpu_cmd_memory_barrier, as on Vulkan.
void buffer_state(s_d3d12 *d, s_buffer *buffer, uint32_t kind, uint32_t state)
{
    if (buffer == 0 || buffer->memory != k_gpu_memory_gpu)
        return;
    if (buffer->serial[kind] != d->list_serial[kind]) {
        buffer->serial[kind] = d->list_serial[kind];
        buffer->state[kind] = state;
        return;
    }
    const uint32_t now = buffer->state[kind];
    if (is_read_state(state) && is_read_state(now))
        state |= now; // one command may read it two ways (indices and a vertex stream, say): both
    if (now == state)
        return;
    barrier_transition(d, buffer->resource, now, state);
    buffer->state[kind] = state;
}

// ---- submission ----

bool wait_fence(s_d3d12 *d, ID3D12Fence *fence, uint64_t value, const char *where)
{
    uint64_t done = fence->lpVtbl->GetCompletedValue(fence);
    if (done == ~0ull) {
        report_device_removed(d, where);
        return false;
    }
    if (done >= value)
        return true;
    if (FAILED(fence->lpVtbl->SetEventOnCompletion(fence, value, d->event)))
        return false;
    host_WaitForSingleObject(d->event, 0xffffffffu);
    done = fence->lpVtbl->GetCompletedValue(fence);
    if (done == ~0ull) {
        report_device_removed(d, where);
        return false;
    }
    return done >= value;
}

ID3D12GraphicsCommandList *immediate_begin(s_d3d12 *d)
{
    if (!d->immediate_recording) {
        if (FAILED(d->immediate_allocator->lpVtbl->Reset(d->immediate_allocator)) ||
            FAILED(d->immediate_list->lpVtbl->Reset(d->immediate_list, d->immediate_allocator, 0)))
            return 0;
        d->immediate_recording = 1;
        d->list_serial[k_list_immediate]++;
    }
    return d->immediate_list;
}

// Submits the immediate list and waits for it (after every earlier submission: a frame being recorded is not seen).
gpu_result immediate_submit(s_d3d12 *d)
{
    ID3D12GraphicsCommandList *list = d->immediate_list; // its operations flushed their barriers (from their mark)
    d->immediate_recording = 0;
    HRESULT result = list->lpVtbl->Close(list);
    if (SUCCEEDED(result)) {
        ID3D12CommandList *lists[1] = { (ID3D12CommandList *)list };
        d->queue->lpVtbl->ExecuteCommandLists(d->queue, 1, lists);
        result = d->queue->lpVtbl->Signal(d->queue, d->immediate_fence, ++d->immediate_value);
    }
    d->stats.immediate_submits++;
    if (FAILED(result)) {
        log_text(d, "d3d12: a one-off submission failed: %s %08x", hresult_text(result), (uint32_t)result);
        return result_of(result);
    }
    return wait_fence(d, d->immediate_fence, d->immediate_value, "a one-off submission") ? k_gpu_ok : k_gpu_device_lost;
}

// The recording frame's upload list, begun on first use; NULL outside a frame. It runs before the frame's own list,
// so only commands no earlier command of the frame can observe belong in it (a buffer's first upload, an image no
// command has used).
ID3D12GraphicsCommandList *upload_begin(s_d3d12 *d)
{
    if (!d->recording)
        return 0;
    s_frame &frame = d->frames[d->frame_index];
    if (!frame.upload_recording) {
        if (FAILED(frame.upload_allocator->lpVtbl->Reset(frame.upload_allocator)) ||
            FAILED(frame.upload_list->lpVtbl->Reset(frame.upload_list, frame.upload_allocator, 0)))
            return 0;
        frame.upload_recording = 1;
        d->list_serial[k_list_upload]++;
    }
    d->stats.deferred_uploads++;
    return frame.upload_list;
}

// Waits until the queue has run everything submitted.
bool queue_idle(s_d3d12 *d)
{
    if (d->queue == 0 || d->immediate_fence == 0)
        return true;
    if (FAILED(d->queue->lpVtbl->Signal(d->queue, d->immediate_fence, ++d->immediate_value)))
        return false;
    return wait_fence(d, d->immediate_fence, d->immediate_value, "waiting for the GPU");
}

uint32_t align_up(uint32_t value, uint32_t alignment) { return (value + alignment - 1u) / alignment * alignment; }

// ---- deferred destruction ----

const uint32_t k_buffer_pool_max = 64;            // retired buffers kept for reuse
const uint32_t k_buffer_pool_bytes = 4u << 20;    // larger ones are released
const uint32_t k_buffer_pool_frames = 120;        // pooled longer than this: released

void buffer_release(s_d3d12 *d, s_buffer *buffer)
{
    if (buffer == 0)
        return;
    pool_free(&d->views, buffer->srv);
    pool_free(&d->views, buffer->uav);
    if (buffer->mapped != 0)
        buffer->resource->lpVtbl->Unmap(buffer->resource, 0, 0);
    release(buffer->resource);
    free(buffer);
}

// A buffer no submitted command uses any more: into the pool of reusable ones, or released.
void buffer_recycle(s_d3d12 *d, s_buffer *buffer)
{
    if (buffer->allocated <= k_buffer_pool_bytes && d->buffer_pool.count < k_buffer_pool_max) {
        if (s_buffer **slot = d->buffer_pool.push()) {
            buffer->pool_frame = d->frame_serial;
            *slot = buffer;
            return;
        }
    }
    buffer_release(d, buffer);
}

void buffer_pool_trim(s_d3d12 *d, bool all)
{
    uint32_t kept = 0;
    for (uint32_t i = 0; i < d->buffer_pool.count; i++) {
        s_buffer *buffer = d->buffer_pool.items[i];
        if (all || buffer->pool_frame + k_buffer_pool_frames < d->frame_serial)
            buffer_release(d, buffer);
        else
            d->buffer_pool.items[kept++] = buffer;
    }
    d->buffer_pool.count = kept;
}

void view_release(s_d3d12 *d, s_view *view)
{
    pool_free(&d->views, view->descriptor);
    view->descriptor = k_none;
}

void image_release(s_d3d12 *d, s_image *image)
{
    if (image == 0)
        return;
    if (d->recording && image->touched_serial == d->frame_serial) {
        s_array<s_image *> &touched = d->frames[d->frame_index].touched;
        for (uint32_t i = 0; i < touched.count; i++) {
            if (touched.items[i] == image) {
                touched.items[i] = touched.items[--touched.count];
                break;
            }
        }
    }
    view_release(d, &image->view);
    view_release(d, &image->sampled_view);
    view_release(d, &image->stencil_view);
    view_release(d, &image->raw_view);
    view_release(d, &image->array_view);
    view_release(d, &image->swizzled_view);
    pool_free(&d->rtvs, image->rtv);
    pool_free(&d->dsvs, image->dsv);
    pool_free(&d->views, image->uav);
    release(image->resource);
    free(image);
}

void group_release(s_d3d12 *d, s_group *group)
{
    if (group->chunk != k_none) {
        if (uint32_t *slot = d->free_chunks.push())
            *slot = group->chunk;
    }
    free(group);
}

void destroy_one(s_d3d12 *d, const s_retired &r)
{
    switch (r.kind) {
    case k_retired_buffer: buffer_recycle(d, (s_buffer *)r.object); break;
    case k_retired_image: image_release(d, (s_image *)r.object); break;
    case k_retired_group: group_release(d, (s_group *)r.object); break;
    case k_retired_sampler: pool_free(&d->samplers, r.index); break;
    default: {
        IUnknown *object = (IUnknown *)r.object;
        if (object != 0)
            object->lpVtbl->Release(object);
        break;
    }
    }
}

// Destroys what a frame slot retired; the slot's fence has been waited for.
void destroy_retired(s_d3d12 *d, s_frame &frame)
{
    for (uint32_t i = 0; i < frame.retired.count; i++)
        destroy_one(d, frame.retired.items[i]);
    frame.retired.count = 0;
}

// Destroyed once the frames that may use it are done: kept by the frame slot recording now (or last recorded), whose
// fence covers every submission so far.
void retire(s_d3d12 *d, uint32_t kind, void *object, uint32_t index)
{
    s_retired r;
    r.kind = kind;
    r.object = object;
    r.index = index;
    s_retired *slot = d->frames[d->frame_index].retired.push();
    if (slot == 0) {
        // no room to defer it: the GPU's work first, then at once
        queue_idle(d);
        destroy_one(d, r);
        return;
    }
    *slot = r;
}

// ---- buffers ----

D3D12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE type)
{
    D3D12_HEAP_PROPERTIES heap = {};
    heap.Type = type;
    heap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    return heap;
}

D3D12_RESOURCE_DESC buffer_desc(uint32_t size, uint32_t flags)
{
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = (D3D12_RESOURCE_FLAGS)flags;
    return desc;
}

// usage: e_gpu_buffer_usage; memory: e_gpu_memory_usage. Storage buffers get raw views (an SRV; a UAV in device memory),
// which the memory tables copy (cmd_xenos_bind_memory). Sizes are kept multiples of 256 (root CBV offsets, raw views).
s_buffer *buffer_make(s_d3d12 *d, uint32_t size, uint32_t usage, uint32_t memory)
{
    const uint32_t allocated = align_up(size != 0 ? size : 4u, 256u);
    if (memory == k_gpu_memory_gpu)
        usage |= k_gpu_buffer_transfer_dst;
    for (uint32_t i = 0; i < d->buffer_pool.count; i++) {
        s_buffer *pooled = d->buffer_pool.items[i];
        if (pooled->memory == memory && pooled->usage == usage && pooled->allocated >= allocated &&
            pooled->allocated / 2u <= allocated) {
            d->buffer_pool.items[i] = d->buffer_pool.items[--d->buffer_pool.count];
            pooled->size = size;
            pooled->uploads = 0;
            pooled->upload_serial = 0;
            memset(pooled->serial, 0, sizeof(pooled->serial));
            d->stats.buffers_reused++;
            return pooled;
        }
    }
    s_buffer *buffer = (s_buffer *)calloc(1, sizeof(s_buffer));
    if (buffer == 0)
        return 0;
    buffer->size = size;
    buffer->allocated = allocated;
    buffer->usage = usage;
    buffer->memory = memory;
    buffer->srv = buffer->uav = k_none;
    const bool device = memory == k_gpu_memory_gpu;
    const bool storage = (usage & k_gpu_buffer_storage) != 0;
    const D3D12_HEAP_PROPERTIES heap = heap_properties(device                        ? D3D12_HEAP_TYPE_DEFAULT
                                                       : memory == k_gpu_memory_upload ? D3D12_HEAP_TYPE_UPLOAD
                                                                                       : D3D12_HEAP_TYPE_READBACK);
    const D3D12_RESOURCE_DESC desc =
        buffer_desc(allocated, device && storage ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE);
    const uint32_t initial = device                          ? D3D12_RESOURCE_STATE_COMMON
                             : memory == k_gpu_memory_upload ? D3D12_RESOURCE_STATE_GENERIC_READ
                                                             : D3D12_RESOURCE_STATE_COPY_DEST;
    HRESULT result = d->device->lpVtbl->CreateCommittedResource(d->device, &heap, D3D12_HEAP_FLAG_NONE, &desc,
                                                                (D3D12_RESOURCE_STATES)initial, 0, IID_ID3D12Resource,
                                                                (void **)&buffer->resource);
    if (SUCCEEDED(result) && !device) {
        const D3D12_RANGE nothing = { 0, 0 };
        void *mapped = 0;
        result = buffer->resource->lpVtbl->Map(buffer->resource, 0,
                                               memory == k_gpu_memory_upload ? &nothing : 0, &mapped);
        buffer->mapped = (uint8_t *)mapped;
    }
    if (FAILED(result)) {
        log_text(d, "d3d12: a %u-byte buffer could not be made: %s %08x", size, hresult_text(result), (uint32_t)result);
        if (result == DXGI_ERROR_DEVICE_REMOVED)
            report_device_removed(d, "making a buffer");
        buffer->mapped = 0;
        release(buffer->resource);
        free(buffer);
        return 0;
    }
    buffer->address = buffer->resource->lpVtbl->GetGPUVirtualAddress(buffer->resource);
    d->stats.memory_allocations++;
    if (storage && memory != k_gpu_memory_readback) {
        buffer->srv = pool_alloc(d, &d->views);
        if (buffer->srv != k_none) {
            D3D12_SHADER_RESOURCE_VIEW_DESC view = {};
            view.Format = DXGI_FORMAT_R32_TYPELESS;
            view.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            view.Buffer.NumElements = allocated / 4u;
            view.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
            d->device->lpVtbl->CreateShaderResourceView(d->device, buffer->resource, &view,
                                                        pool_cpu(&d->views, buffer->srv));
        }
        if (device) {
            buffer->uav = pool_alloc(d, &d->views);
            if (buffer->uav != k_none) {
                D3D12_UNORDERED_ACCESS_VIEW_DESC view = {};
                view.Format = DXGI_FORMAT_R32_TYPELESS;
                view.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
                view.Buffer.NumElements = allocated / 4u;
                view.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
                d->device->lpVtbl->CreateUnorderedAccessView(d->device, buffer->resource, 0, &view,
                                                             pool_cpu(&d->views, buffer->uav));
            }
        }
    }
    return buffer;
}

// `size` bytes at a multiple of `alignment` in the recording frame's staging ring (upload heap), for a copy recorded
// into its upload list; NULL when the ring has no room left.
s_buffer *staging_ring_alloc(s_d3d12 *d, uint32_t size, uint32_t alignment, uint32_t *out_offset)
{
    s_frame &frame = d->frames[d->frame_index];
    if (!d->recording || size > k_staging_ring_bytes)
        return 0;
    if (frame.staging == 0) {
        frame.staging = buffer_make(d, k_staging_ring_bytes, k_gpu_buffer_transfer_src, k_gpu_memory_upload);
        if (frame.staging == 0)
            return 0;
    }
    const uint32_t start = align_up(frame.staging_used, alignment);
    if (start > k_staging_ring_bytes || size > k_staging_ring_bytes - start)
        return 0;
    frame.staging_used = start + size;
    *out_offset = start;
    return frame.staging;
}

// ---- images ----

// The SRV component mapping of a view: the view's swizzle (e_gpu_swizzle per component, GPU_SWIZZLE) through the
// format's own component order (s_format::mapping).
UINT compose_mapping(const s_format &format, uint32_t swizzle)
{
    UINT c[4];
    for (uint32_t i = 0; i < 4; i++) {
        const uint32_t s = (swizzle >> (8u * i)) & 0xffu;
        if (s == k_gpu_swizzle_zero)
            c[i] = D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0;
        else if (s == k_gpu_swizzle_one)
            c[i] = D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1;
        else
            c[i] = format.mapping[s >= k_gpu_swizzle_r && s <= k_gpu_swizzle_a ? s - k_gpu_swizzle_r : i];
    }
    return d3d12_shader_4_component_mapping(c[0], c[1], c[2], c[3]);
}

// Makes an SRV of an image in the CPU view pool: view_type (e_gpu_view_type), its format, mapping and plane.
bool make_view(s_d3d12 *d, s_image *image, s_view *out, uint32_t view_type, DXGI_FORMAT format, UINT mapping,
               uint32_t plane)
{
    out->image = image;
    out->descriptor = pool_alloc(d, &d->views);
    if (out->descriptor == k_none)
        return false;
    D3D12_SHADER_RESOURCE_VIEW_DESC view = {};
    view.Format = format;
    view.Shader4ComponentMapping = mapping;
    const uint32_t levels = image->desc.mip_levels;
    switch (view_type) {
    case k_gpu_view_3d:
        view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
        view.Texture3D.MipLevels = levels;
        break;
    case k_gpu_view_cube:
        view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        view.TextureCube.MipLevels = levels;
        break;
    case k_gpu_view_2d_array:
        view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        view.Texture2DArray.MipLevels = levels;
        view.Texture2DArray.ArraySize = image->desc.array_layers;
        view.Texture2DArray.PlaneSlice = plane;
        break;
    default:
        if (image->desc.array_layers > 1) {
            // a 2D view of a layered image sees its first layer (a Vulkan 2D view's single layer)
            view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            view.Texture2DArray.MipLevels = levels;
            view.Texture2DArray.ArraySize = 1;
            view.Texture2DArray.PlaneSlice = plane;
        } else {
            view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            view.Texture2D.MipLevels = levels;
            view.Texture2D.PlaneSlice = plane;
        }
        break;
    }
    d->device->lpVtbl->CreateShaderResourceView(d->device, image->resource, &view, pool_cpu(&d->views, out->descriptor));
    return true;
}

// The image's 2D view of level 0 / layer 0 targets (render target, depth / stencil, storage) in `slot` of their pools.
void make_target_views(s_d3d12 *d, s_image *image)
{
    const s_format &format = format_of(image->internal_format);
    if ((image->flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0) {
        image->rtv = pool_alloc(d, &d->rtvs);
        if (image->rtv != k_none) {
            D3D12_RENDER_TARGET_VIEW_DESC view = {};
            view.Format = format.format;
            if (image->desc.view_type == k_gpu_view_3d) {
                view.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                view.Texture3D.WSize = 1;
            } else if (image->desc.array_layers > 1) {
                view.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                view.Texture2DArray.ArraySize = 1;
            } else {
                view.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            }
            d->device->lpVtbl->CreateRenderTargetView(d->device, image->resource, &view, pool_cpu(&d->rtvs, image->rtv));
        }
    }
    if ((image->flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0) {
        image->dsv = pool_alloc(d, &d->dsvs);
        if (image->dsv != k_none) {
            D3D12_DEPTH_STENCIL_VIEW_DESC view = {};
            view.Format = format.format;
            if (image->desc.array_layers > 1) {
                view.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                view.Texture2DArray.ArraySize = 1;
            } else {
                view.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            }
            d->device->lpVtbl->CreateDepthStencilView(d->device, image->resource, &view, pool_cpu(&d->dsvs, image->dsv));
        }
    }
    if ((image->flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0) {
        image->uav = pool_alloc(d, &d->views);
        if (image->uav != k_none) {
            D3D12_UNORDERED_ACCESS_VIEW_DESC view = {};
            view.Format = format.format;
            if (image->desc.view_type == k_gpu_view_3d) {
                view.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
                view.Texture3D.WSize = image->desc.depth;
            } else if (image->desc.array_layers > 1) {
                view.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                view.Texture2DArray.ArraySize = 1;
            } else {
                view.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            }
            d->device->lpVtbl->CreateUnorderedAccessView(d->device, image->resource, 0, &view,
                                                         pool_cpu(&d->views, image->uav));
        }
    }
}

// Whether a format can be a render target, and one the backend's draws write by value (its components in order: the
// packed formats with another component order cannot take a blit's output as is).
bool format_renderable(const s_d3d12 *d, uint32_t format);
bool format_has_support(const s_d3d12 *d, uint32_t format, uint32_t support1);

s_image *image_make(s_d3d12 *d, const s_gpu_image_desc *in)
{
    const s_format &format = format_of(in->format);
    if (format.format == DXGI_FORMAT_UNKNOWN || in->width == 0 || in->height == 0) {
        log_text(d, "d3d12: an image of format %s (%ux%u) cannot be made here", gpu_format_name(in->format), in->width,
                 in->height);
        return 0;
    }
    s_image *image = (s_image *)calloc(1, sizeof(s_image));
    if (image == 0)
        return 0;
    image->desc = *in;
    s_gpu_image_desc &desc = image->desc;
    if (desc.usage == 0)
        desc.usage = k_gpu_image_sampled | k_gpu_image_transfer_dst;
    if (desc.view_type == k_gpu_view_none)
        desc.view_type = k_gpu_view_2d;
    if (desc.depth == 0 || desc.view_type != k_gpu_view_3d)
        desc.depth = 1;
    if (desc.mip_levels == 0)
        desc.mip_levels = 1;
    if (desc.array_layers == 0 || desc.view_type == k_gpu_view_3d)
        desc.array_layers = desc.view_type == k_gpu_view_cube ? 6u : 1u;
    image->internal_format = desc.format;
    image->aspect = format.aspect;
    image->view.descriptor = image->sampled_view.descriptor = image->stencil_view.descriptor = k_none;
    image->raw_view.descriptor = image->array_view.descriptor = image->swizzled_view.descriptor = k_none;
    image->rtv = image->dsv = image->uav = k_none;
    // a block-compressed image whose top level is not whole blocks needs UnalignedBlockTexturesSupported: without it
    // its blocks are decoded on upload into an uncompressed format
    if (format_is_bc(desc.format) && ((desc.width & 3u) != 0 || (desc.height & 3u) != 0) && !d->unaligned_bc) {
        image->internal_format = bc_decoded_format(desc.format);
        image->decode_bc = 1;
    }
    const s_format &internal = format_of(image->internal_format);
    uint32_t flags = D3D12_RESOURCE_FLAG_NONE;
    if ((format.aspect & k_aspect_depth) != 0) {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // every depth image: copies into it draw (SV_Depth)
    } else {
        if ((desc.usage & k_gpu_image_color_attachment) != 0) {
            if (!format_has_support(d, image->internal_format, D3D12_FORMAT_SUPPORT1_RENDER_TARGET)) {
                log_text(d, "d3d12: format %s cannot be a render target here", gpu_format_name(desc.format));
                free(image);
                return 0;
            }
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        } else if ((desc.usage & k_gpu_image_transfer_dst) != 0 && format_renderable(d, image->internal_format)) {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // a scaling or converting copy into it draws
        }
        if ((desc.usage & k_gpu_image_storage) != 0) {
            if (!format_has_support(d, image->internal_format, D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW)) {
                log_text(d, "d3d12: format %s cannot be a storage image here", gpu_format_name(desc.format));
                free(image);
                return 0;
            }
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
    }
    image->flags = flags;
    D3D12_RESOURCE_DESC resource = {};
    resource.Dimension = desc.view_type == k_gpu_view_3d ? D3D12_RESOURCE_DIMENSION_TEXTURE3D
                                                         : D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resource.Width = desc.width;
    resource.Height = desc.height;
    resource.DepthOrArraySize = (UINT16)(desc.view_type == k_gpu_view_3d ? desc.depth : desc.array_layers);
    resource.MipLevels = (UINT16)desc.mip_levels;
    resource.Format = internal.resource;
    resource.SampleDesc.Count = 1;
    resource.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resource.Flags = (D3D12_RESOURCE_FLAGS)flags;
    const D3D12_HEAP_PROPERTIES heap = heap_properties(D3D12_HEAP_TYPE_DEFAULT);
    const HRESULT result = d->device->lpVtbl->CreateCommittedResource(d->device, &heap, D3D12_HEAP_FLAG_NONE, &resource,
                                                                      D3D12_RESOURCE_STATE_COMMON, 0, IID_ID3D12Resource,
                                                                      (void **)&image->resource);
    if (FAILED(result)) {
        log_text(d, "d3d12: a %ux%ux%u %s image (%u levels, %u layers) could not be made: %s %08x", desc.width,
                 desc.height, desc.depth, gpu_format_name(desc.format), desc.mip_levels, desc.array_layers,
                 hresult_text(result), (uint32_t)result);
        if (result == DXGI_ERROR_DEVICE_REMOVED)
            report_device_removed(d, "making an image");
        free(image);
        return 0;
    }
    image->state = image->submitted = D3D12_RESOURCE_STATE_COMMON;
    d->stats.memory_allocations++;
    bool ok;
    if ((format.aspect & k_aspect_depth) != 0) {
        const UINT depth = d3d12_shader_4_component_mapping(0, 0, 0, 0); // depth in every component (Vulkan's RRRR)
        const UINT stencil = d3d12_shader_4_component_mapping(1, 1, 1, 1); // the stencil plane's value is its G
        ok = make_view(d, image, &image->view, desc.view_type, format.depth, depth, 0) &&
             make_view(d, image, &image->sampled_view, desc.view_type, format.depth, depth, 0) &&
             make_view(d, image, &image->stencil_view, k_gpu_view_2d, format.stencil, stencil, 1) &&
             make_view(d, image, &image->raw_view, k_gpu_view_2d, format.depth, depth, 0);
    } else {
        ok = make_view(d, image, &image->view, desc.view_type, internal.format, compose_mapping(internal, desc.swizzle),
                       0) &&
             make_view(d, image, &image->raw_view, desc.view_type == k_gpu_view_3d ? k_gpu_view_3d : k_gpu_view_2d,
                       internal.format, compose_mapping(internal, 0), 0);
    }
    if (!ok) {
        image_release(d, image);
        return 0;
    }
    make_target_views(d, image);
    return image;
}

// Expands a 5:6:5 color.
void bc_color565(uint32_t value, uint8_t out[4])
{
    const uint32_t r = (value >> 11) & 31u, g = (value >> 5) & 63u, b = value & 31u;
    out[0] = (uint8_t)(r << 3 | r >> 2);
    out[1] = (uint8_t)(g << 2 | g >> 4);
    out[2] = (uint8_t)(b << 3 | b >> 2);
    out[3] = 255;
}

// The 16 texels of a BC1 color block (8 bytes) as RGBA; `four` forces the four-color mode (BC2, BC3).
void bc_decode_color(const uint8_t *block, bool four, uint8_t texels[16][4])
{
    const uint32_t c0 = (uint32_t)block[0] | (uint32_t)block[1] << 8, c1 = (uint32_t)block[2] | (uint32_t)block[3] << 8;
    uint8_t palette[4][4];
    bc_color565(c0, palette[0]);
    bc_color565(c1, palette[1]);
    for (uint32_t c = 0; c < 3; c++) {
        if (four || c0 > c1) {
            palette[2][c] = (uint8_t)((2u * palette[0][c] + palette[1][c] + 1u) / 3u);
            palette[3][c] = (uint8_t)((palette[0][c] + 2u * palette[1][c] + 1u) / 3u);
        } else {
            palette[2][c] = (uint8_t)((palette[0][c] + palette[1][c] + 1u) / 2u);
            palette[3][c] = 0;
        }
    }
    palette[2][3] = 255;
    palette[3][3] = four || c0 > c1 ? 255 : 0; // BC1's three-color mode: index 3 is transparent black
    const uint32_t indices = (uint32_t)block[4] | (uint32_t)block[5] << 8 | (uint32_t)block[6] << 16 |
                             (uint32_t)block[7] << 24;
    for (uint32_t i = 0; i < 16; i++)
        memcpy(texels[i], palette[(indices >> (2u * i)) & 3u], 4);
}

// The 16 values of a BC4 block (8 bytes: two endpoints, 3-bit indices); signed: SNORM endpoints, values -127..127.
void bc_decode_channel(const uint8_t *block, bool is_signed, int32_t values[16])
{
    int32_t a0 = block[0], a1 = block[1];
    if (is_signed) {
        a0 = (int8_t)block[0] < -127 ? -127 : (int8_t)block[0];
        a1 = (int8_t)block[1] < -127 ? -127 : (int8_t)block[1];
    }
    int32_t palette[8];
    palette[0] = a0;
    palette[1] = a1;
    if (a0 > a1) {
        for (int32_t i = 1; i < 7; i++)
            palette[i + 1] = (a0 * (7 - i) + a1 * i + (a0 * (7 - i) + a1 * i >= 0 ? 3 : -3)) / 7;
    } else {
        for (int32_t i = 1; i < 5; i++)
            palette[i + 1] = (a0 * (5 - i) + a1 * i + (a0 * (5 - i) + a1 * i >= 0 ? 2 : -2)) / 5;
        palette[6] = is_signed ? -127 : 0;
        palette[7] = is_signed ? 127 : 255;
    }
    uint64_t indices = 0;
    for (uint32_t i = 0; i < 6; i++)
        indices |= (uint64_t)block[2 + i] << (8u * i);
    for (uint32_t i = 0; i < 16; i++)
        values[i] = palette[(indices >> (3u * i)) & 7u];
}

// Decodes a level's blocks (`rows` rows of blocks, `row_pitch` apart) into texels of bc_decoded_format's size per texel,
// `out_pitch` bytes a row, width x height texels.
void decode_bc_blocks(uint32_t format, const uint8_t *data, uint32_t row_pitch, uint32_t width, uint32_t height,
                      uint8_t *out, uint32_t out_pitch)
{
    const uint32_t texel_bytes = format == k_gpu_format_bc4_unorm ? 1u
                                 : format == k_gpu_format_bc5_unorm || format == k_gpu_format_bc5_snorm ? 2u
                                                                                                         : 4u;
    const uint32_t block_bytes = format <= k_gpu_format_bc1_rgba_srgb || format == k_gpu_format_bc4_unorm ? 8u : 16u;
    for (uint32_t by = 0; by * 4u < height; by++) {
        for (uint32_t bx = 0; bx * 4u < width; bx++) {
            const uint8_t *block = data + (size_t)by * row_pitch + (size_t)bx * block_bytes;
            uint8_t texels[16][4];
            int32_t channel[2][16];
            switch (format) {
            case k_gpu_format_bc1_rgba_unorm:
            case k_gpu_format_bc1_rgba_srgb: bc_decode_color(block, false, texels); break;
            case k_gpu_format_bc2_unorm:
            case k_gpu_format_bc2_srgb:
                bc_decode_color(block + 8, true, texels);
                for (uint32_t i = 0; i < 16; i++) {
                    const uint32_t alpha = (block[i / 2u] >> (4u * (i & 1u))) & 15u;
                    texels[i][3] = (uint8_t)(alpha * 17u);
                }
                break;
            case k_gpu_format_bc3_unorm:
            case k_gpu_format_bc3_srgb:
                bc_decode_color(block + 8, true, texels);
                bc_decode_channel(block, false, channel[0]);
                for (uint32_t i = 0; i < 16; i++)
                    texels[i][3] = (uint8_t)channel[0][i];
                break;
            case k_gpu_format_bc4_unorm:
                bc_decode_channel(block, false, channel[0]);
                for (uint32_t i = 0; i < 16; i++)
                    texels[i][0] = (uint8_t)channel[0][i];
                break;
            default: // BC5
                bc_decode_channel(block, format == k_gpu_format_bc5_snorm, channel[0]);
                bc_decode_channel(block + 8, format == k_gpu_format_bc5_snorm, channel[1]);
                for (uint32_t i = 0; i < 16; i++) {
                    texels[i][0] = (uint8_t)channel[0][i];
                    texels[i][1] = (uint8_t)channel[1][i];
                }
                break;
            }
            for (uint32_t y = 0; y < 4u && by * 4u + y < height; y++)
                for (uint32_t x = 0; x < 4u && bx * 4u + x < width; x++)
                    memcpy(out + (size_t)(by * 4u + y) * out_pitch + (size_t)(bx * 4u + x) * texel_bytes,
                           texels[y * 4u + x], texel_bytes);
        }
    }
}

// One subresource's copy of an upload: where its texels go in the staging buffer.
struct s_upload_copy {
    uint32_t subresource;
    uint32_t region;      // the s_gpu_image_region
    uint32_t element;     // layer of the region
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    uint32_t rows;        // rows of blocks (texels when decoded) per slice
};

gpu_result image_upload(s_d3d12 *d, s_image *image, const s_gpu_image_region *regions, uint32_t region_count,
                        const void *data, uint32_t size)
{
    uint32_t block_width, block_height, block_bytes;
    if (!gpu_format_block_info(image->desc.format, &block_width, &block_height, &block_bytes)) {
        log_text(d, "d3d12: image upload: format %s has no block size", gpu_format_name(image->desc.format));
        return k_gpu_unsupported;
    }
    if (image->aspect != k_aspect_color) {
        log_text(d, "d3d12: image upload into a depth / stencil image (%s)", gpu_format_name(image->desc.format));
        return k_gpu_unsupported;
    }
    uint32_t texel_bytes = block_bytes, unit_width = block_width, unit_height = block_height;
    if (image->decode_bc) {
        gpu_format_block_info(image->internal_format, &unit_width, &unit_height, &texel_bytes);
        unit_width = unit_height = 1;
    }
    const s_format &internal = format_of(image->internal_format);
    uint32_t copy_count = 0;
    for (uint32_t i = 0; i < region_count; i++)
        copy_count += regions[i].layer_count != 0 ? regions[i].layer_count : 1u;
    s_upload_copy *copies = (s_upload_copy *)calloc(copy_count != 0 ? copy_count : 1u, sizeof(s_upload_copy));
    if (copies == 0)
        return k_gpu_out_of_memory;
    uint32_t staging_size = 0, n = 0;
    for (uint32_t i = 0; i < region_count; i++) {
        const s_gpu_image_region &region = regions[i];
        const uint32_t row_bytes = (region.width + block_width - 1u) / block_width * block_bytes;
        const uint32_t row_pitch = region.row_pitch != 0 ? region.row_pitch : row_bytes;
        const uint32_t rows = (region.height + block_height - 1u) / block_height;
        const uint32_t slice_pitch = region.slice_pitch != 0 ? region.slice_pitch : row_pitch * rows;
        const uint32_t layers = region.layer_count != 0 ? region.layer_count : 1u;
        const uint32_t depth = region.depth != 0 ? region.depth : 1u;
        const uint64_t end = (uint64_t)region.offset + (uint64_t)slice_pitch * (depth * layers - 1u) +
                             (uint64_t)row_pitch * (rows - 1u) + row_bytes;
        const uint32_t level_width = image->desc.width >> region.mip_level != 0 ? image->desc.width >> region.mip_level : 1u;
        const uint32_t level_height =
            image->desc.height >> region.mip_level != 0 ? image->desc.height >> region.mip_level : 1u;
        const uint32_t level_depth = image->desc.depth >> region.mip_level != 0 ? image->desc.depth >> region.mip_level : 1u;
        if (region.width == 0 || region.height == 0 || end > size || row_pitch < row_bytes ||
            region.mip_level >= image->desc.mip_levels || region.width > level_width || region.height > level_height ||
            depth > level_depth || region.base_layer + layers > image->desc.array_layers) {
            log_text(d, "d3d12: image upload region %u (level %u, %ux%ux%u, layers %u+%u at offset %u) is outside the "
                        "%ux%ux%u image of %u levels and %u layers or the %u bytes of data", i, region.mip_level,
                     region.width, region.height, depth, region.base_layer, layers, region.offset, image->desc.width,
                     image->desc.height, image->desc.depth, image->desc.mip_levels, image->desc.array_layers, size);
            free(copies);
            return k_gpu_error;
        }
        const uint32_t width = align_up(region.width, unit_width), height = align_up(region.height, unit_height);
        const uint32_t out_rows = height / unit_height;
        const uint32_t pitch = align_up(width / unit_width * texel_bytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        for (uint32_t layer = 0; layer < layers; layer++) {
            s_upload_copy &copy = copies[n++];
            copy.subresource = region.mip_level + (region.base_layer + layer) * image->desc.mip_levels;
            copy.region = i;
            copy.element = layer;
            copy.rows = out_rows;
            staging_size = align_up(staging_size, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
            copy.footprint.Offset = staging_size;
            copy.footprint.Footprint.Format = internal.format;
            copy.footprint.Footprint.Width = width;
            copy.footprint.Footprint.Height = height;
            copy.footprint.Footprint.Depth = depth;
            copy.footprint.Footprint.RowPitch = pitch;
            staging_size += pitch * out_rows * depth;
        }
    }
    // an image no command has used may be filled ahead of the frame (the upload list), staged in its ring when it fits;
    // so is one this frame filled that way already (its uploads stay in order: a one-off list would run first)
    const bool pending = image->upload_serial == d->frame_serial && d->recording;
    ID3D12GraphicsCommandList *list = !image->used || pending ? upload_begin(d) : 0;
    const bool deferred = list != 0;
    uint32_t staging_base = 0;
    s_buffer *ring = deferred ? staging_ring_alloc(d, staging_size, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, &staging_base)
                              : 0;
    s_buffer *staging = ring != 0 ? ring : buffer_make(d, staging_size, k_gpu_buffer_transfer_src, k_gpu_memory_upload);
    if (staging == 0) {
        free(copies);
        return k_gpu_out_of_memory;
    }
    for (uint32_t c = 0; c < n; c++) {
        s_upload_copy &copy = copies[c];
        const s_gpu_image_region &region = regions[copy.region];
        const uint32_t row_bytes = (region.width + block_width - 1u) / block_width * block_bytes;
        const uint32_t row_pitch = region.row_pitch != 0 ? region.row_pitch : row_bytes;
        const uint32_t rows = (region.height + block_height - 1u) / block_height;
        const uint32_t slice_pitch = region.slice_pitch != 0 ? region.slice_pitch : row_pitch * rows;
        const uint32_t depth = copy.footprint.Footprint.Depth;
        copy.footprint.Offset += staging_base;
        for (uint32_t z = 0; z < depth; z++) {
            const uint8_t *source = (const uint8_t *)data + region.offset +
                                    (size_t)slice_pitch * (copy.element * depth + z);
            uint8_t *target = staging->mapped + copy.footprint.Offset +
                              (size_t)copy.footprint.Footprint.RowPitch * copy.rows * z;
            if (image->decode_bc) {
                decode_bc_blocks(image->desc.format, source, row_pitch, region.width, region.height, target,
                                 copy.footprint.Footprint.RowPitch);
            } else {
                for (uint32_t y = 0; y < rows; y++)
                    memcpy(target + (size_t)copy.footprint.Footprint.RowPitch * y, source + (size_t)row_pitch * y,
                           row_bytes);
            }
        }
    }
    if (!deferred)
        list = immediate_begin(d);
    if (list == 0) {
        if (ring == 0)
            buffer_release(d, staging);
        free(copies);
        return k_gpu_error;
    }
    // a one-off submission runs before the recording frame: it starts from the image's submitted state, and leaves
    // the image in it again when the recording frame's commands have used the image (they expect that state)
    // (an upload list after a deferred upload: that one left the image ready for sampling)
    const bool in_frame = d->recording && image->touched_serial == d->frame_serial;
    const uint32_t before = !deferred ? image->submitted : pending ? k_state_shader_read : image->state;
    const uint32_t mark = d->barriers.count;
    if (before != D3D12_RESOURCE_STATE_COPY_DEST)
        barrier_transition(d, image->resource, before, D3D12_RESOURCE_STATE_COPY_DEST);
    flush_barriers(d, list, mark);
    for (uint32_t c = 0; c < n; c++) {
        D3D12_TEXTURE_COPY_LOCATION destination = {}, source = {};
        destination.pResource = image->resource;
        destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        destination.SubresourceIndex = copies[c].subresource;
        source.pResource = staging->resource;
        source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        source.PlacedFootprint = copies[c].footprint;
        list->lpVtbl->CopyTextureRegion(list, &destination, 0, 0, 0, &source, 0);
    }
    free(copies);
    const uint32_t after = !deferred && in_frame ? image->submitted : k_state_shader_read;
    if (after != D3D12_RESOURCE_STATE_COPY_DEST)
        barrier_transition(d, image->resource, D3D12_RESOURCE_STATE_COPY_DEST, after);
    flush_barriers(d, list, mark);
    if (deferred) {
        if (!pending) // else the frame's commands track the state from the first one's
            image->state = after;
        image->upload_serial = d->frame_serial;
        touch(d, image);
        if (ring == 0)
            retire(d, k_retired_buffer, staging);
        return k_gpu_ok;
    }
    image->used = 1;
    if (!in_frame)
        image->state = image->submitted = after;
    const gpu_result result = immediate_submit(d);
    buffer_release(d, staging);
    return result;
}

// ---- samplers ----

uint32_t sampler_make(s_d3d12 *d, const s_gpu_sampler_desc *in)
{
    D3D12_SAMPLER_DESC desc = {};
    const uint32_t reduction = in->compare_enable ? 1u : 0u;
    if (in->max_anisotropy > 1.0f) {
        desc.Filter = reduction != 0 ? D3D12_FILTER_COMPARISON_ANISOTROPIC : D3D12_FILTER_ANISOTROPIC;
        desc.MaxAnisotropy = in->max_anisotropy >= 16.0f ? 16u : (UINT)(in->max_anisotropy + 0.5f);
    } else {
        desc.Filter = (D3D12_FILTER)d3d12_basic_filter(in->min_filter == k_gpu_filter_linear ? 1u : 0u,
                                                       in->mag_filter == k_gpu_filter_linear ? 1u : 0u,
                                                       in->mipmap_mode == k_gpu_filter_linear ? 1u : 0u, reduction);
        desc.MaxAnisotropy = 1;
    }
    const uint32_t modes[3] = { in->address_u, in->address_v, in->address_w };
    D3D12_TEXTURE_ADDRESS_MODE address[3];
    for (uint32_t i = 0; i < 3; i++)
        address[i] = modes[i] <= k_gpu_address_mirror_clamp_to_edge ? (D3D12_TEXTURE_ADDRESS_MODE)(modes[i] + 1u)
                                                                    : D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.AddressU = address[0];
    desc.AddressV = address[1];
    desc.AddressW = address[2];
    desc.MipLODBias = in->mip_lod_bias < -16.0f ? -16.0f : in->mip_lod_bias > 15.99f ? 15.99f : in->mip_lod_bias;
    desc.ComparisonFunc = in->compare_enable && in->compare_op <= k_gpu_compare_always
                              ? (D3D12_COMPARISON_FUNC)(in->compare_op + 1u)
                              : D3D12_COMPARISON_FUNC_NEVER;
    const float alpha = in->border_color == k_gpu_border_transparent_black ? 0.0f : 1.0f;
    const float color = in->border_color == k_gpu_border_opaque_white ? 1.0f : 0.0f;
    desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = color;
    desc.BorderColor[3] = alpha;
    desc.MinLOD = in->min_lod;
    desc.MaxLOD = in->max_lod;
    const uint32_t descriptor = pool_alloc(d, &d->samplers);
    if (descriptor != k_none)
        d->device->lpVtbl->CreateSampler(d->device, &desc, pool_cpu(&d->samplers, descriptor));
    return descriptor;
}

// ---- formats ----

bool format_has_support(const s_d3d12 *d, uint32_t format, uint32_t support1)
{
    return format < k_gpu_format_count && format_of(format).format != DXGI_FORMAT_UNKNOWN &&
           (d->format_support[format] & support1) == support1;
}

bool format_renderable(const s_d3d12 *d, uint32_t format)
{
    const s_format &f = format_of(format);
    return f.aspect == k_aspect_color && !format_is_bc(format) && f.mapping[0] == 0 && f.mapping[1] == 1 &&
           f.mapping[2] == 2 && f.mapping[3] == 3 && format_has_support(d, format, D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
}

// Integer formats: their texels are not floats (a converting draw cannot read or write them).
bool format_is_integer(uint32_t format)
{
    switch (format) {
    case k_gpu_format_r8g8b8a8_uint: case k_gpu_format_r8g8b8a8_sint: case k_gpu_format_a2b10g10r10_uint:
    case k_gpu_format_r16g16_uint: case k_gpu_format_r16g16_sint: case k_gpu_format_r16g16b16a16_uint:
    case k_gpu_format_r16g16b16a16_sint: case k_gpu_format_r32_uint: case k_gpu_format_r32_sint:
    case k_gpu_format_r32g32_uint: case k_gpu_format_r32g32_sint: case k_gpu_format_r32g32b32_uint:
    case k_gpu_format_r32g32b32_sint: case k_gpu_format_r32g32b32a32_uint: case k_gpu_format_r32g32b32a32_sint:
        return true;
    default: return false;
    }
}

void query_format_support(s_d3d12 *d)
{
    for (uint32_t format = 0; format < k_gpu_format_count; format++) {
        const s_format &f = format_of(format);
        d->format_support[format] = 0;
        if (f.format == DXGI_FORMAT_UNKNOWN)
            continue;
        D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {};
        support.Format = f.format;
        if (SUCCEEDED(d->device->lpVtbl->CheckFeatureSupport(d->device, D3D12_FEATURE_FORMAT_SUPPORT, &support,
                                                             sizeof(support))))
            d->format_support[format] = support.Support1;
    }
}

// ---- shaders, root signatures ----

// A DXBC container's digest (bytes 4-19, which DXC's validator computes over the container).
void container_digest(const void *code, size_t size, uint32_t out[4])
{
    memset(out, 0, 16);
    if (size >= 20)
        memcpy(out, (const uint8_t *)code + 4, 16);
    if ((out[0] | out[1] | out[2] | out[3]) == 0) // not signed: a hash of the bytes instead
        out[0] = gpu_hash_bytes(2166136261u, code, size), out[1] = (uint32_t)size;
}

s_shader *shader_wrap(const void *code, size_t size)
{
    s_shader *shader = (s_shader *)calloc(1, sizeof(s_shader));
    if (shader == 0)
        return 0;
    shader->code = code;
    shader->size = size;
    container_digest(code, size, shader->digest);
    return shader;
}

ID3D12RootSignature *root_signature_make(s_d3d12 *d, const void *code, size_t size, const char *what)
{
    ID3D12RootSignature *root = 0;
    const HRESULT result =
        d->device->lpVtbl->CreateRootSignature(d->device, 0, code, size, IID_ID3D12RootSignature, (void **)&root);
    if (FAILED(result)) {
        log_text(d, "d3d12: the %s root signature could not be made: %s %08x", what, hresult_text(result),
                 (uint32_t)result);
        return 0;
    }
    return root;
}

// ---- pipeline state objects ----

enum e_pso_kind {
    k_pso_graphics = 0,
    k_pso_compute = 1,
};

// What makes a pipeline state object: its description with the pointers taken out (the root signature by its layout,
// each shader by its container digest). The whole key, padding zeroed, is compared: one object serves every pipeline
// whose key is equal (the frontend keys on stencil reference and blend constants too, which Direct3D 12 sets on the
// command list).
struct s_pso_key {
    uint32_t kind;
    uint32_t layout;
    uint32_t digests[5][4]; // VS, PS, DS, HS, GS (compute: [0] the CS)
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics;
};

struct s_pso_entry {
    s_pso_key key;
    uint32_t hash;
    uint32_t references;
    ID3D12PipelineState *pso;
};

void lock(s_d3d12 *d) { host_AcquireSRWLockExclusive(&d->library_lock); }
void unlock(s_d3d12 *d) { host_ReleaseSRWLockExclusive(&d->library_lock); }

// The pipeline library's name of a key: 16 hex digits of two hashes of it.
void pso_name(const s_pso_key &key, uint32_t hash, WCHAR name[24])
{
    static const char k_hex[] = "0123456789abcdef";
    const uint32_t second = gpu_hash_bytes(0x9e3779b9u, &key, sizeof(key));
    for (uint32_t i = 0; i < 8; i++) {
        name[i] = (WCHAR)k_hex[(hash >> (28u - 4u * i)) & 15u];
        name[8 + i] = (WCHAR)k_hex[(second >> (28u - 4u * i)) & 15u];
    }
    name[16] = 0;
}

// The pipeline state object of a key (desc: its description with the pointers in), shared: from the objects made
// already, else the pipeline library (pipeline_cache_d3d12.bin), else made (and stored in the library). Any thread.
s_pso_entry *pso_get(s_d3d12 *d, const s_pso_key &key, const void *desc)
{
    const uint32_t hash = gpu_hash_bytes(2166136261u, &key, sizeof(key));
    lock(d);
    for (uint32_t i = 0; i < d->psos.count; i++) {
        s_pso_entry *entry = d->psos.items[i];
        if (entry->hash == hash && memcmp(&entry->key, &key, sizeof(key)) == 0) {
            entry->references++;
            unlock(d);
            return entry;
        }
    }
    WCHAR name[24];
    pso_name(key, hash, name);
    ID3D12PipelineState *pso = 0;
    if (d->library != 0) {
        const HRESULT loaded =
            key.kind == k_pso_graphics
                ? d->library->lpVtbl->LoadGraphicsPipeline(d->library, name,
                                                           (const D3D12_GRAPHICS_PIPELINE_STATE_DESC *)desc,
                                                           IID_ID3D12PipelineState, (void **)&pso)
                : d->library->lpVtbl->LoadComputePipeline(d->library, name,
                                                          (const D3D12_COMPUTE_PIPELINE_STATE_DESC *)desc,
                                                          IID_ID3D12PipelineState, (void **)&pso);
        if (FAILED(loaded))
            pso = 0;
    }
    unlock(d);
    bool made = false;
    if (pso == 0) {
        const HRESULT result =
            key.kind == k_pso_graphics
                ? d->device->lpVtbl->CreateGraphicsPipelineState(d->device,
                                                                 (const D3D12_GRAPHICS_PIPELINE_STATE_DESC *)desc,
                                                                 IID_ID3D12PipelineState, (void **)&pso)
                : d->device->lpVtbl->CreateComputePipelineState(d->device,
                                                                (const D3D12_COMPUTE_PIPELINE_STATE_DESC *)desc,
                                                                IID_ID3D12PipelineState, (void **)&pso);
        if (FAILED(result)) {
            log_text(d, "d3d12: a %s pipeline state object could not be made: %s %08x",
                     key.kind == k_pso_graphics ? "graphics" : "compute", hresult_text(result), (uint32_t)result);
            if (result == DXGI_ERROR_DEVICE_REMOVED)
                report_device_removed(d, "making a pipeline");
            drain_debug_messages(d);
            return 0;
        }
        made = true;
    }
    s_pso_entry *entry = (s_pso_entry *)calloc(1, sizeof(s_pso_entry));
    lock(d);
    // another thread may have made the same one meanwhile: share the first
    for (uint32_t i = 0; i < d->psos.count; i++) {
        s_pso_entry *other = d->psos.items[i];
        if (other->hash == hash && memcmp(&other->key, &key, sizeof(key)) == 0) {
            other->references++;
            unlock(d);
            free(entry);
            pso->lpVtbl->Release(pso);
            return other;
        }
    }
    s_pso_entry **slot = entry != 0 ? d->psos.push() : 0;
    if (slot == 0) {
        unlock(d);
        free(entry);
        pso->lpVtbl->Release(pso);
        return 0;
    }
    entry->key = key;
    entry->hash = hash;
    entry->references = 1;
    entry->pso = pso;
    *slot = entry;
    if (made) {
        d->pipelines_created++;
        if (d->library != 0) {
            const HRESULT stored = d->library->lpVtbl->StorePipeline(d->library, name, pso);
            if (FAILED(stored) && stored != E_INVALIDARG && report_once(d, k_report_library))
                log_text(d, "d3d12: the pipeline library did not take a pipeline: %s %08x (not cached)",
                         hresult_text(stored), (uint32_t)stored);
        }
    }
    unlock(d);
    return entry;
}

// Drops a reference; the object goes once the frames that may use it are done.
void pso_put(s_d3d12 *d, s_pso_entry *entry)
{
    if (entry == 0)
        return;
    lock(d);
    const bool last = --entry->references == 0;
    if (last) {
        for (uint32_t i = 0; i < d->psos.count; i++) {
            if (d->psos.items[i] == entry) {
                d->psos.items[i] = d->psos.items[--d->psos.count];
                break;
            }
        }
    }
    unlock(d);
    if (!last)
        return;
    if (d->pso_set == entry->pso)
        d->pso_set = 0;
    retire(d, k_retired_object, entry->pso);
    free(entry);
}

D3D12_BLEND blend_factor(const s_d3d12 *d, uint32_t factor, bool alpha, bool *uses_constant, bool *uses_alpha_constant)
{
    switch (factor) {
    case k_gpu_blend_zero: return D3D12_BLEND_ZERO;
    case k_gpu_blend_one: return D3D12_BLEND_ONE;
    case k_gpu_blend_src_color: return alpha ? D3D12_BLEND_SRC_ALPHA : D3D12_BLEND_SRC_COLOR;
    case k_gpu_blend_one_minus_src_color: return alpha ? D3D12_BLEND_INV_SRC_ALPHA : D3D12_BLEND_INV_SRC_COLOR;
    case k_gpu_blend_dst_color: return alpha ? D3D12_BLEND_DEST_ALPHA : D3D12_BLEND_DEST_COLOR;
    case k_gpu_blend_one_minus_dst_color: return alpha ? D3D12_BLEND_INV_DEST_ALPHA : D3D12_BLEND_INV_DEST_COLOR;
    case k_gpu_blend_src_alpha: return D3D12_BLEND_SRC_ALPHA;
    case k_gpu_blend_one_minus_src_alpha: return D3D12_BLEND_INV_SRC_ALPHA;
    case k_gpu_blend_dst_alpha: return D3D12_BLEND_DEST_ALPHA;
    case k_gpu_blend_one_minus_dst_alpha: return D3D12_BLEND_INV_DEST_ALPHA;
    case k_gpu_blend_constant_color:
        *uses_constant = true;
        return D3D12_BLEND_BLEND_FACTOR;
    case k_gpu_blend_one_minus_constant_color:
        *uses_constant = true;
        return D3D12_BLEND_INV_BLEND_FACTOR;
    case k_gpu_blend_constant_alpha:
    case k_gpu_blend_one_minus_constant_alpha: {
        const bool inverse = factor == k_gpu_blend_one_minus_constant_alpha;
        // an alpha slot reads the blend factor's alpha already (as constant color does there)
        if (alpha) {
            *uses_constant = true;
            return inverse ? D3D12_BLEND_INV_BLEND_FACTOR : D3D12_BLEND_BLEND_FACTOR;
        }
        // a color slot, the factor's alpha alone: D3D12_BLEND_ALPHA_FACTOR where the device has it (OPTIONS13), else
        // the blend factor with its alpha in every component (alpha_splat), which constant color in the same pipeline
        // cannot share
        if (d->alpha_blend_factor)
            return inverse ? D3D12_BLEND_INV_ALPHA_FACTOR : D3D12_BLEND_ALPHA_FACTOR;
        *uses_alpha_constant = true;
        return inverse ? D3D12_BLEND_INV_BLEND_FACTOR : D3D12_BLEND_BLEND_FACTOR;
    }
    case k_gpu_blend_src_alpha_saturate: return D3D12_BLEND_SRC_ALPHA_SAT;
    default: return D3D12_BLEND_ONE;
    }
}

D3D12_STENCIL_OP stencil_op(uint32_t op) { return op <= k_gpu_stencil_decrement_wrap ? (D3D12_STENCIL_OP)(op + 1u) : D3D12_STENCIL_OP_KEEP; }
D3D12_COMPARISON_FUNC compare_func(uint32_t op)
{
    return op <= k_gpu_compare_always ? (D3D12_COMPARISON_FUNC)(op + 1u) : D3D12_COMPARISON_FUNC_ALWAYS;
}

D3D_PRIMITIVE_TOPOLOGY topology_of(uint32_t topology, uint32_t patch_control_points)
{
    switch (topology) {
    case k_gpu_topology_point_list: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    case k_gpu_topology_line_list: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case k_gpu_topology_line_strip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case k_gpu_topology_triangle_strip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case k_gpu_topology_patch_list:
        return (D3D_PRIMITIVE_TOPOLOGY)(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST +
                                        (patch_control_points >= 1u && patch_control_points <= 32u
                                             ? patch_control_points - 1u : 0u));
    default: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // lists (and fans: the frontend makes them lists)
    }
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE topology_type_of(uint32_t topology)
{
    switch (topology) {
    case k_gpu_topology_point_list: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case k_gpu_topology_line_list:
    case k_gpu_topology_line_strip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case k_gpu_topology_patch_list: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    default: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    }
}

// The fixed-function part of a graphics pipeline's description, from the neutral state.
void graphics_desc(s_d3d12 *d, const s_gpu_graphics_state &state, D3D12_GRAPHICS_PIPELINE_STATE_DESC *desc,
                   s_pipeline *pipeline)
{
    bool uses_constant = false, uses_alpha_constant = false;
    D3D12_BLEND_DESC &blend = desc->BlendState;
    blend.AlphaToCoverageEnable = state.alpha_to_coverage != 0;
    blend.IndependentBlendEnable = 1;
    const uint32_t colors = state.color_attachment_count <= GPU_MAX_COLOR_ATTACHMENTS ? state.color_attachment_count
                                                                                       : GPU_MAX_COLOR_ATTACHMENTS;
    for (uint32_t i = 0; i < 8; i++) {
        D3D12_RENDER_TARGET_BLEND_DESC &target = blend.RenderTarget[i];
        target.SrcBlend = target.SrcBlendAlpha = D3D12_BLEND_ONE;
        target.DestBlend = target.DestBlendAlpha = D3D12_BLEND_ZERO;
        target.BlendOp = target.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        target.LogicOp = D3D12_LOGIC_OP_NOOP;
        target.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        if (i >= colors)
            continue;
        const s_gpu_blend_attachment &b = state.blend[i];
        target.RenderTargetWriteMask = (UINT8)(b.write_mask & 15u);
        if (!b.enable)
            continue;
        target.BlendEnable = 1;
        target.SrcBlend = blend_factor(d, b.src_color, false, &uses_constant, &uses_alpha_constant);
        target.DestBlend = blend_factor(d, b.dst_color, false, &uses_constant, &uses_alpha_constant);
        target.BlendOp = b.color_op <= k_gpu_blend_op_max ? (D3D12_BLEND_OP)(b.color_op + 1u) : D3D12_BLEND_OP_ADD;
        target.SrcBlendAlpha = blend_factor(d, b.src_alpha, true, &uses_constant, &uses_alpha_constant);
        target.DestBlendAlpha = blend_factor(d, b.dst_alpha, true, &uses_constant, &uses_alpha_constant);
        target.BlendOpAlpha = b.alpha_op <= k_gpu_blend_op_max ? (D3D12_BLEND_OP)(b.alpha_op + 1u) : D3D12_BLEND_OP_ADD;
    }
    if (uses_constant && uses_alpha_constant && report_once(d, k_report_constant_alpha))
        log_text(d, "d3d12: a pipeline blends with the constant color and its alpha alone, which this device cannot "
                    "tell apart (no AlphaBlendFactorSupported): the alpha is used for both");
    desc->SampleMask = 0xffffffffu;
    D3D12_RASTERIZER_DESC &raster = desc->RasterizerState;
    raster.FillMode = state.polygon_mode == k_gpu_polygon_line ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
    if (state.polygon_mode == k_gpu_polygon_point && report_once(d, k_report_point_fill))
        log_text(d, "d3d12: point fill mode is drawn solid (Direct3D 12 has no point fill)");
    raster.CullMode = state.cull_mode == k_gpu_cull_front  ? D3D12_CULL_MODE_FRONT
                      : state.cull_mode == k_gpu_cull_back ? D3D12_CULL_MODE_BACK
                                                           : D3D12_CULL_MODE_NONE;
    raster.FrontCounterClockwise = state.front_face == k_gpu_front_face_counter_clockwise;
    if (state.depth_bias_enable) {
        const float bias = state.depth_bias_constant;
        raster.DepthBias = (INT)(bias >= 0.0f ? bias + 0.5f : bias - 0.5f);
        raster.SlopeScaledDepthBias = state.depth_bias_slope;
    }
    raster.DepthClipEnable = 1;
    const uint32_t samples = state.sample_count != 0 ? state.sample_count : 1u;
    raster.MultisampleEnable = samples > 1u;
    raster.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    D3D12_DEPTH_STENCIL_DESC &depth = desc->DepthStencilState;
    const bool has_depth = state.depth_format != k_gpu_format_undefined || state.stencil_format != k_gpu_format_undefined;
    depth.DepthEnable = has_depth && state.depth_test != 0;
    depth.DepthWriteMask = state.depth_write ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    depth.DepthFunc = compare_func(state.depth_compare);
    depth.StencilEnable = has_depth && state.stencil_test != 0;
    depth.StencilReadMask = (UINT8)state.stencil_front.compare_mask;
    depth.StencilWriteMask = (UINT8)state.stencil_front.write_mask;
    const s_gpu_stencil_face *faces[2] = { &state.stencil_front, &state.stencil_back };
    D3D12_DEPTH_STENCILOP_DESC *ops[2] = { &depth.FrontFace, &depth.BackFace };
    for (uint32_t f = 0; f < 2; f++) {
        ops[f]->StencilFailOp = stencil_op(faces[f]->fail_op);
        ops[f]->StencilDepthFailOp = stencil_op(faces[f]->depth_fail_op);
        ops[f]->StencilPassOp = stencil_op(faces[f]->pass_op);
        ops[f]->StencilFunc = compare_func(faces[f]->compare_op);
    }
    const bool strip = state.topology == k_gpu_topology_line_strip || state.topology == k_gpu_topology_triangle_strip;
    desc->IBStripCutValue = state.primitive_restart && strip ? D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF
                                                             : D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    desc->PrimitiveTopologyType = topology_type_of(state.topology);
    desc->NumRenderTargets = colors;
    for (uint32_t i = 0; i < colors; i++)
        desc->RTVFormats[i] = format_of(state.color_formats[i]).format;
    desc->DSVFormat = format_of(state.depth_format != k_gpu_format_undefined ? state.depth_format
                                                                              : state.stencil_format).format;
    desc->SampleDesc.Count = samples;
    if (pipeline != 0) {
        pipeline->stencil_reference = state.stencil_front.reference & 0xffu;
        pipeline->uses_blend_factor = uses_constant || uses_alpha_constant;
        pipeline->alpha_splat = uses_alpha_constant;
        memcpy(pipeline->blend_factor, state.blend_constants, sizeof(pipeline->blend_factor));
    }
}

s_pipeline *pipeline_wrap(s_d3d12 *d, s_pso_entry *entry, s_pipeline_layout *layout, uint32_t bind_point)
{
    s_pipeline *pipeline = (s_pipeline *)calloc(1, sizeof(s_pipeline));
    if (pipeline == 0) {
        pso_put(d, entry);
        return 0;
    }
    pipeline->entry = entry;
    pipeline->layout = layout;
    pipeline->bind_point = bind_point;
    pipeline->topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    return pipeline;
}

// A graphics pipeline of the Xenos layout (or the backend's own) from shader stages and the neutral state.
s_pipeline *graphics_pipeline(s_d3d12 *d, const s_gpu_shader_stage *stages, uint32_t stage_count,
                              s_pipeline_layout *layout, const s_gpu_graphics_state &state, uint32_t patch_control_points)
{
    if (layout == 0 || layout->root == 0) {
        log_text(d, "d3d12: a graphics pipeline without its layout");
        return 0;
    }
    s_pso_key key;
    memset(&key, 0, sizeof(key));
    key.kind = k_pso_graphics;
    key.layout = layout->kind;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC &desc = key.graphics;
    // the stages: VS, PS, DS, HS, GS slots of the key's digests
    const s_shader *shaders[5] = {};
    for (uint32_t i = 0; i < stage_count; i++) {
        const s_shader *shader = pointer_of<s_shader>(stages[i].module);
        if (shader == 0)
            continue;
        const uint32_t slot = stages[i].stage == k_gpu_stage_vertex         ? 0u
                              : stages[i].stage == k_gpu_stage_fragment     ? 1u
                              : stages[i].stage == k_gpu_stage_tess_eval    ? 2u
                              : stages[i].stage == k_gpu_stage_tess_control ? 3u
                              : stages[i].stage == k_gpu_stage_geometry     ? 4u
                                                                            : 5u;
        if (slot < 5)
            shaders[slot] = shader;
    }
    if (shaders[0] == 0) {
        log_text(d, "d3d12: a graphics pipeline without a vertex shader");
        return 0;
    }
    s_pipeline staged = {};
    graphics_desc(d, state, &desc, &staged);
    const bool tessellated = shaders[3] != 0;
    if (tessellated)
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    for (uint32_t s = 0; s < 5; s++)
        if (shaders[s] != 0)
            memcpy(key.digests[s], shaders[s]->digest, 16);
    D3D12_GRAPHICS_PIPELINE_STATE_DESC full = desc;
    full.pRootSignature = layout->root;
    D3D12_SHADER_BYTECODE *slots[5] = { &full.VS, &full.PS, &full.DS, &full.HS, &full.GS };
    for (uint32_t s = 0; s < 5; s++) {
        if (shaders[s] != 0) {
            slots[s]->pShaderBytecode = shaders[s]->code;
            slots[s]->BytecodeLength = shaders[s]->size;
        }
    }
    s_pso_entry *entry = pso_get(d, key, &full);
    if (entry == 0)
        return 0;
    s_pipeline *pipeline = pipeline_wrap(d, entry, layout, k_gpu_bind_graphics);
    if (pipeline == 0)
        return 0;
    pipeline->topology = topology_of(tessellated ? (uint32_t)k_gpu_topology_patch_list : state.topology,
                                     tessellated ? patch_control_points : 0u);
    pipeline->stencil_reference = staged.stencil_reference;
    pipeline->uses_blend_factor = staged.uses_blend_factor;
    pipeline->alpha_splat = staged.alpha_splat;
    memcpy(pipeline->blend_factor, staged.blend_factor, sizeof(pipeline->blend_factor));
    if (state.topology == k_gpu_topology_triangle_fan && report_once(d, k_report_fan))
        log_text(d, "d3d12: a triangle fan pipeline (drawn as a list: the frontend expands fans, caps.triangle_fans)");
    return pipeline;
}

s_pso_entry *compute_pso(s_d3d12 *d, const s_shader *shader, s_pipeline_layout *layout)
{
    if (shader == 0 || layout == 0 || layout->root == 0)
        return 0;
    s_pso_key key;
    memset(&key, 0, sizeof(key));
    key.kind = k_pso_compute;
    key.layout = layout->kind;
    memcpy(key.digests[0], shader->digest, 16);
    D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
    desc.pRootSignature = layout->root;
    desc.CS.pShaderBytecode = shader->code;
    desc.CS.BytecodeLength = shader->size;
    return pso_get(d, key, &desc);
}

// ---- bind groups ----

s_group *group_make(s_d3d12 *d, const s_bind_layout *layout, uint32_t lifetime)
{
    s_group *group = (s_group *)calloc(1, sizeof(s_group));
    if (group == 0)
        return 0;
    group->kind = layout->kind;
    group->lifetime = lifetime;
    group->chunk = k_none;
    group->frame_serial = d->frame_serial;
    memset(group->sources, 0xff, sizeof(group->sources));
    const bool frame = lifetime == k_gpu_bind_frame || lifetime == k_gpu_bind_frame_textures;
    if (group->kind == k_bind_textures) {
        if (frame) {
            group->srv_base = frame_descriptors(d, k_group_srvs);
            if (group->srv_base == k_none) {
                free(group);
                return 0;
            }
        } else {
            group->chunk = chunk_alloc(d);
            if (group->chunk != k_none) {
                group->srv_base = chunk_base(d, group->chunk);
            } else {
                group->transient = 1; // copied into the frame region when bound (group_frame_copy)
                group->srv_base = d->recording ? frame_descriptors(d, k_group_srvs) : k_none;
            }
        }
    }
    if (frame) {
        if (!d->recording) {
            log_text(d, "d3d12: a frame bind group outside a frame");
            group_release(d, group);
            return 0;
        }
        s_group **slot = d->frames[d->frame_index].groups.push();
        if (slot == 0) {
            group_release(d, group);
            return 0;
        }
        *slot = group;
    } else if (lifetime == k_gpu_bind_cached_textures) {
        s_group **slot = d->cached_groups.push();
        if (slot == 0) {
            group_release(d, group);
            return 0;
        }
        *slot = group;
    }
    return group;
}

void group_add_image(s_group *group, s_image *image)
{
    for (uint32_t i = 0; i < group->image_count; i++)
        if (group->images[i] == image)
            return;
    if (group->image_count < k_max_texture_images)
        group->images[group->image_count++] = image;
}

void group_write_image(s_d3d12 *d, s_group *group, uint32_t slot, const s_view *view)
{
    if (group->kind != k_bind_textures || slot >= k_group_srvs || view == 0 || view->descriptor == k_none)
        return;
    group->sources[slot] = view->descriptor;
    if (group->srv_base != k_none)
        copy_view(d, group->srv_base + slot, view->descriptor);
    group_add_image(group, view->image);
}

// A transient group (no chunk) into the recording frame's region, once a frame: false when the region is full.
bool group_frame_copy(s_d3d12 *d, s_group *group)
{
    if (!group->transient || (group->srv_base != k_none && group->frame_serial == d->frame_serial))
        return true;
    const uint32_t base = frame_descriptors(d, k_group_srvs);
    if (base == k_none)
        return false;
    for (uint32_t slot = 0; slot < k_group_srvs; slot++)
        if (group->sources[slot] != k_none)
            copy_view(d, base + slot, group->sources[slot]);
    group->srv_base = base;
    group->frame_serial = d->frame_serial;
    return true;
}

void group_write_sampler(s_d3d12 *d, s_group *group, uint32_t slot, const s_sampler *sampler)
{
    if (group->kind != k_bind_textures || slot < k_group_srvs || slot >= k_group_srvs + k_group_samplers ||
        sampler == 0 || sampler->descriptor == k_none)
        return;
    const uint8_t index = (uint8_t)(slot - k_group_srvs);
    group->sampler_generation = 0; // placed again when bound
    for (uint32_t i = 0; i < group->sampler_count; i++) {
        if (group->sampler_slot[i] == index) {
            group->sampler_id[i] = sampler->id;
            group->sampler_descriptor[i] = sampler->descriptor;
            return;
        }
    }
    if (group->sampler_count >= k_max_texture_images) {
        if (report_once(d, k_report_descriptors))
            log_text(d, "d3d12: a texture bind group with more than %u samplers", k_max_texture_images);
        return;
    }
    group->sampler_slot[group->sampler_count] = index;
    group->sampler_id[group->sampler_count] = sampler->id;
    group->sampler_descriptor[group->sampler_count] = sampler->descriptor;
    group->sampler_count++;
}

// ---- frames ----

// The recording frame's list for a command, or NULL (logged) outside a frame.
ID3D12GraphicsCommandList *recording_list(s_d3d12 *d, const char *caller)
{
    if (!d->recording) {
        log_text(d, "d3d12: %s outside gpu_frame_begin / gpu_frame_end", caller);
        return 0;
    }
    return d->frames[d->frame_index].list;
}

// What the recording list has bound is unknown from here on (a pass of the backend's own bound something else).
void forget_bindings(s_d3d12 *d, uint32_t bind_point)
{
    s_bind_state &b = d->bind[bind_point];
    b.root = 0;
    memset(b.cbv_set, 0, sizeof(b.cbv_set));
    memset(b.table_set, 0, sizeof(b.table_set));
    if (bind_point == k_gpu_bind_graphics) {
        d->dynamic_valid = 0;
        d->viewport_valid = d->scissor_valid = 0;
        d->topology_set = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}

// A record's samples, once its frame is done: the sum of its queries (one a rendering it spanned).
uint64_t occlusion_sum(const s_frame &frame, const s_occlusion_record &record)
{
    if (frame.occlusion_readback == 0)
        return 0;
    const uint64_t *samples = (const uint64_t *)frame.occlusion_readback->mapped;
    uint64_t total = 0;
    for (uint32_t i = 0; i < record.query_count; i++)
        total += samples[record.queries[i]];
    return total;
}

gpu_result frame_begin(s_d3d12 *d)
{
    if (d->recording) {
        log_text(d, "d3d12: gpu_frame_begin inside a frame");
        return k_gpu_error;
    }
    d->frame_index = (d->frame_index + 1u) % d->frame_count;
    s_frame &frame = d->frames[d->frame_index];
    if (!wait_fence(d, d->frame_fence, frame.serial, "waiting for a frame slot"))
        return k_gpu_device_lost;
    destroy_retired(d, frame);
    buffer_pool_trim(d, false);
    // the slot's frame is done: its occlusion results and GPU time, before its queries are used again
    for (uint32_t r = 0; r < frame.occlusion_records; r++)
        frame.results[r] = occlusion_sum(frame, frame.records[r]);
    frame.result_count = frame.occlusion_records;
    frame.results_serial = frame.serial;
    if (frame.timestamps_written && frame.timestamp_readback != 0 && d->timestamp_frequency != 0) {
        const uint64_t *stamps = (const uint64_t *)frame.timestamp_readback->mapped;
        if (stamps[1] >= stamps[0])
            d->stats.gpu_frame_ms = (float)((double)(stamps[1] - stamps[0]) * 1000.0 / (double)d->timestamp_frequency);
    }
    frame.timestamps_written = 0;
    for (uint32_t i = 0; i < frame.groups.count; i++)
        group_release(d, frame.groups.items[i]);
    frame.groups.count = 0;
    frame.view_used = 0;
    frame.staging_used = 0;
    for (uint32_t i = 0; i < 256; i++)
        frame.memory_bases[i] = k_none;
    HRESULT result = frame.allocator->lpVtbl->Reset(frame.allocator);
    if (SUCCEEDED(result))
        result = frame.list->lpVtbl->Reset(frame.list, frame.allocator, 0);
    if (FAILED(result)) {
        log_text(d, "d3d12: a frame's command list could not be begun: %s %08x", hresult_text(result), (uint32_t)result);
        return result_of(result);
    }
    frame.upload_recording = 0;
    frame.serial = ++d->frame_serial;
    d->recording = 1;
    d->list_serial[k_list_frame]++;
    memset(d->bind, 0, sizeof(d->bind));
    for (uint32_t i = 0; i < 2; i++)
        d->bind[i].memory_base = k_none;
    d->pso_set = 0;
    d->topology_set = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    d->dynamic_valid = 0;
    d->viewport_valid = d->scissor_valid = 0;
    d->index_buffer = 0;
    d->index_valid = 0;
    d->rendering = 0;
    frame.occlusion_records = 0;
    frame.occlusion_queries = 0;
    d->occlusion_active = 0;
    d->occlusion_running = 0;
    frame.acquired_image = k_none;
    d->sampler_heap->used_serial = frame.serial;
    set_descriptor_heaps(d, frame.list);
    if (frame.timestamp_heap != 0)
        frame.list->lpVtbl->EndQuery(frame.list, frame.timestamp_heap, D3D12_QUERY_TYPE_TIMESTAMP, 0);
    return k_gpu_ok;
}

void end_rendering(s_d3d12 *d);

gpu_result frame_end(s_d3d12 *d, uint32_t wait)
{
    if (!d->recording) {
        log_text(d, "d3d12: gpu_frame_end outside a frame");
        return k_gpu_error;
    }
    s_frame &frame = d->frames[d->frame_index];
    ID3D12GraphicsCommandList *list = frame.list;
    if (d->rendering)
        end_rendering(d);
    occlusion_suspend(d, list); // end_rendering did, unless a query began outside a rendering
    d->occlusion_active = 0;
    if (frame.occlusion_queries != 0)
        list->lpVtbl->ResolveQueryData(list, frame.occlusion_heap, D3D12_QUERY_TYPE_OCCLUSION, 0,
                                       frame.occlusion_queries, frame.occlusion_readback->resource, 0);
    if (frame.timestamp_heap != 0) {
        list->lpVtbl->EndQuery(list, frame.timestamp_heap, D3D12_QUERY_TYPE_TIMESTAMP, 1);
        list->lpVtbl->ResolveQueryData(list, frame.timestamp_heap, D3D12_QUERY_TYPE_TIMESTAMP, 0, 2,
                                       frame.timestamp_readback->resource, 0);
        frame.timestamps_written = 1;
    }
    flush_barriers(d, list);
    d->recording = 0;
    // the upload list first, in a submission of its own (buffers decay to COMMON between them)
    HRESULT result = S_OK;
    if (frame.upload_recording) {
        frame.upload_recording = 0; // its operations flushed their barriers (from their mark)
        result = frame.upload_list->lpVtbl->Close(frame.upload_list);
        if (SUCCEEDED(result)) {
            ID3D12CommandList *lists[1] = { (ID3D12CommandList *)frame.upload_list };
            d->queue->lpVtbl->ExecuteCommandLists(d->queue, 1, lists);
        }
    }
    const HRESULT closed = list->lpVtbl->Close(list);
    if (SUCCEEDED(result) && SUCCEEDED(closed)) {
        ID3D12CommandList *lists[1] = { (ID3D12CommandList *)list };
        d->queue->lpVtbl->ExecuteCommandLists(d->queue, 1, lists);
    }
    // signalled whatever happened, so the slot can be begun again
    const HRESULT signalled = d->queue->lpVtbl->Signal(d->queue, d->frame_fence, frame.serial);
    for (uint32_t i = 0; i < frame.touched.count; i++)
        frame.touched.items[i]->submitted = frame.touched.items[i]->state;
    frame.touched.count = 0;
    d->present_image = frame.acquired_image;
    if (FAILED(result) || FAILED(closed) || FAILED(signalled)) {
        const HRESULT failure = FAILED(result) ? result : FAILED(closed) ? closed : signalled;
        log_text(d, "d3d12: frame submission failed: %s %08x", hresult_text(failure), (uint32_t)failure);
        drain_debug_messages(d);
        if (failure == DXGI_ERROR_DEVICE_REMOVED)
            report_device_removed(d, "submitting a frame");
        return result_of(failure);
    }
    if (d->info_queue != 0)
        drain_debug_messages(d);
    if (wait && !wait_fence(d, d->frame_fence, frame.serial, "waiting for the frame"))
        return k_gpu_device_lost;
    return k_gpu_ok;
}

gpu_result frame_wait(s_d3d12 *d, uint32_t slot)
{
    if (slot >= d->frame_count)
        return k_gpu_error;
    if (d->recording && slot == d->frame_index)
        return k_gpu_not_ready; // not submitted yet
    return wait_fence(d, d->frame_fence, d->frames[slot].serial, "waiting for a frame") ? k_gpu_ok : k_gpu_device_lost;
}

// ---- offscreen render targets ----

struct s_render_target {
    uint32_t width, height;
    uint32_t row_pitch; // of the readback buffer's rows
    s_image *color;
    s_image *depth;
    s_buffer *readback;
};

void render_target_destroy(s_d3d12 *d, s_render_target *target)
{
    if (target == 0)
        return;
    image_release(d, target->color);
    image_release(d, target->depth);
    buffer_release(d, target->readback);
    free(target);
}

s_render_target *render_target_make(s_d3d12 *d, uint32_t width, uint32_t height, uint32_t color_format, uint32_t flags)
{
    uint32_t block_width, block_height, block_bytes;
    if (!gpu_format_block_info(color_format, &block_width, &block_height, &block_bytes) || block_width != 1 ||
        format_of(color_format).aspect != k_aspect_color) {
        log_text(d, "d3d12: render target: format %s is not a color format", gpu_format_name(color_format));
        return 0;
    }
    s_render_target *target = (s_render_target *)calloc(1, sizeof(s_render_target));
    if (target == 0)
        return 0;
    target->width = width;
    target->height = height;
    target->row_pitch = d->unrestricted_copy_pitch ? width * block_bytes
                                                   : align_up(width * block_bytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
    s_gpu_image_desc desc = {};
    desc.view_type = k_gpu_view_2d;
    desc.format = color_format;
    desc.width = width;
    desc.height = height;
    desc.depth = 1;
    desc.mip_levels = 1;
    desc.array_layers = 1;
    desc.usage = k_gpu_image_color_attachment | k_gpu_image_transfer_src | k_gpu_image_transfer_dst | k_gpu_image_sampled;
    if ((flags & k_gpu_render_target_storage) != 0)
        desc.usage |= k_gpu_image_storage;
    target->color = image_make(d, &desc);
    if (target->color != 0 && (flags & k_gpu_render_target_depth) != 0) {
        desc.format = d->depth_format;
        desc.usage = k_gpu_image_depth_stencil_attachment;
        target->depth = image_make(d, &desc);
    }
    target->readback = buffer_make(d, target->row_pitch * height, k_gpu_buffer_transfer_dst, k_gpu_memory_readback);
    if (target->color == 0 || ((flags & k_gpu_render_target_depth) != 0 && target->depth == 0) || target->readback == 0) {
        render_target_destroy(d, target);
        return 0;
    }
    return target;
}

// ---- rendering ----

D3D12_CPU_DESCRIPTOR_HANDLE null_rtv(const s_d3d12 *d) { return pool_cpu(&d->rtvs, d->null_rtv); }

// A render target view made now for one level / layer of an image (or a back buffer), valid for commands recorded
// until k_rtv_ring more are made (a view is read when OMSetRenderTargets / Clear is recorded).
D3D12_CPU_DESCRIPTOR_HANDLE transient_rtv(s_d3d12 *d, ID3D12Resource *resource, DXGI_FORMAT format, uint32_t level,
                                          uint32_t layer, bool volume)
{
    const uint32_t index = d->rtv_ring_first + d->rtv_ring_next;
    d->rtv_ring_next = (d->rtv_ring_next + 1u) % k_rtv_ring;
    D3D12_RENDER_TARGET_VIEW_DESC view = {};
    view.Format = format;
    if (volume) {
        view.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
        view.Texture3D.MipSlice = level;
        view.Texture3D.FirstWSlice = layer;
        view.Texture3D.WSize = 1;
    } else {
        view.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        view.Texture2DArray.MipSlice = level;
        view.Texture2DArray.FirstArraySlice = layer;
        view.Texture2DArray.ArraySize = 1;
    }
    const D3D12_CPU_DESCRIPTOR_HANDLE handle = pool_cpu(&d->rtvs, index);
    d->device->lpVtbl->CreateRenderTargetView(d->device, resource, &view, handle);
    return handle;
}

void set_viewport(s_d3d12 *d, float x, float y, float width, float height, float min_depth, float max_depth)
{
    d->viewport.TopLeftX = x;
    d->viewport.TopLeftY = y;
    d->viewport.Width = width;
    d->viewport.Height = height;
    d->viewport.MinDepth = min_depth;
    d->viewport.MaxDepth = max_depth;
    d->viewport_valid = 0;
}

void set_scissor(s_d3d12 *d, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    d->scissor.left = x;
    d->scissor.top = y;
    d->scissor.right = x + (int32_t)width;
    d->scissor.bottom = y + (int32_t)height;
    d->scissor_valid = 0;
}

void end_rendering(s_d3d12 *d)
{
    // Direct3D 12 has no render pass to end: the attachments stay bound until the next begin_rendering, and draws are
    // only recorded inside one. The open occlusion query ends here and the next rendering begins another (Vulkan's
    // rule), so the backend's own copy and present passes, drawn between renderings, are never counted
    occlusion_suspend(d, d->frames[d->frame_index].list);
    d->rendering = 0;
}

void begin_rendering(s_d3d12 *d, s_gpu_image *const *colors_in, uint32_t color_count, s_gpu_image *depth_in,
                     const s_gpu_clear *clear)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_begin_rendering");
    if (list == 0)
        return;
    if (d->rendering)
        end_rendering(d);
    if (color_count > GPU_MAX_COLOR_ATTACHMENTS)
        color_count = GPU_MAX_COLOR_ATTACHMENTS;
    while (color_count != 0 && colors_in[color_count - 1u] == 0)
        color_count--;
    const uint32_t flags = clear != 0 ? clear->flags : 0u;
    // the render area is what every attachment covers (Xenos surfaces of different sizes can be bound together)
    uint32_t width = 0, height = 0;
    bool sized = false;
    D3D12_CPU_DESCRIPTOR_HANDLE targets[GPU_MAX_COLOR_ATTACHMENTS];
    for (uint32_t i = 0; i < color_count; i++) {
        s_image *image = (s_image *)colors_in[i];
        d->colors[i] = image;
        targets[i] = null_rtv(d);
        if (image == 0)
            continue;
        if (image->rtv == k_none) {
            log_text(d, "d3d12: gpu_cmd_begin_rendering: color attachment %u is not a render target", i);
            d->colors[i] = 0;
            continue;
        }
        if (!sized || image->desc.width < width)
            width = image->desc.width;
        if (!sized || image->desc.height < height)
            height = image->desc.height;
        sized = true;
        image_state(d, image, D3D12_RESOURCE_STATE_RENDER_TARGET);
        targets[i] = pool_cpu(&d->rtvs, image->rtv);
    }
    s_image *depth = (s_image *)depth_in;
    if (depth != 0 && depth->dsv == k_none) {
        log_text(d, "d3d12: gpu_cmd_begin_rendering: the depth attachment is not a depth / stencil image");
        depth = 0;
    }
    if (depth != 0) {
        if (!sized || depth->desc.width < width)
            width = depth->desc.width;
        if (!sized || depth->desc.height < height)
            height = depth->desc.height;
        sized = true;
        image_state(d, depth, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }
    flush_barriers(d, list);
    if (!sized) {
        log_text(d, "d3d12: begin rendering without attachments");
        return;
    }
    const D3D12_CPU_DESCRIPTOR_HANDLE dsv = depth != 0 ? pool_cpu(&d->dsvs, depth->dsv) : D3D12_CPU_DESCRIPTOR_HANDLE();
    list->lpVtbl->OMSetRenderTargets(list, color_count, color_count != 0 ? targets : 0, 0, depth != 0 ? &dsv : 0);
    const D3D12_RECT area = { 0, 0, (int32_t)width, (int32_t)height };
    if ((flags & k_gpu_clear_color) != 0) {
        for (uint32_t i = 0; i < color_count; i++)
            if (d->colors[i] != 0 && (clear->color_mask & (1u << i)) != 0)
                list->lpVtbl->ClearRenderTargetView(list, targets[i], clear->color, 1, &area);
    }
    if (depth != 0) {
        uint32_t clears = (flags & k_gpu_clear_depth) != 0 ? (uint32_t)D3D12_CLEAR_FLAG_DEPTH : 0u;
        if ((flags & k_gpu_clear_stencil) != 0 && (depth->aspect & k_aspect_stencil) != 0)
            clears |= D3D12_CLEAR_FLAG_STENCIL;
        if (clears != 0)
            list->lpVtbl->ClearDepthStencilView(list, dsv, (D3D12_CLEAR_FLAGS)clears, clear->depth,
                                                (UINT8)clear->stencil, 1, &area);
    }
    d->rendering = 1;
    occlusion_resume(d, list);
    d->rendering_width = width;
    d->rendering_height = height;
    d->color_count = color_count;
    d->depth = depth;
    set_viewport(d, 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f);
    set_scissor(d, 0, 0, width, height);
}

void clear_attachments(s_d3d12 *d, const s_gpu_clear *clear, const int32_t *rects, uint32_t rect_count)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_clear_attachments");
    if (list == 0 || clear == 0)
        return;
    if (!d->rendering) {
        log_text(d, "d3d12: gpu_cmd_clear_attachments outside rendering");
        return;
    }
    D3D12_RECT local[16];
    D3D12_RECT *areas = rect_count <= 16u ? local : (D3D12_RECT *)malloc(rect_count * sizeof(D3D12_RECT));
    if (areas == 0)
        return;
    uint32_t count = 0;
    if (rects == 0 || rect_count == 0) {
        areas[count++] = D3D12_RECT{ 0, 0, (int32_t)d->rendering_width, (int32_t)d->rendering_height };
    } else {
        for (uint32_t i = 0; i < rect_count; i++) {
            const int32_t x0 = rects[4 * i] < 0 ? 0 : rects[4 * i];
            const int32_t y0 = rects[4 * i + 1] < 0 ? 0 : rects[4 * i + 1];
            const int32_t x1 = rects[4 * i + 2] > (int32_t)d->rendering_width ? (int32_t)d->rendering_width : rects[4 * i + 2];
            const int32_t y1 =
                rects[4 * i + 3] > (int32_t)d->rendering_height ? (int32_t)d->rendering_height : rects[4 * i + 3];
            if (x1 > x0 && y1 > y0)
                areas[count++] = D3D12_RECT{ x0, y0, x1, y1 };
        }
    }
    if (count != 0) {
        if ((clear->flags & k_gpu_clear_color) != 0) {
            for (uint32_t i = 0; i < d->color_count; i++)
                if (d->colors[i] != 0 && (clear->color_mask & (1u << i)) != 0)
                    list->lpVtbl->ClearRenderTargetView(list, pool_cpu(&d->rtvs, d->colors[i]->rtv), clear->color,
                                                        count, areas);
        }
        uint32_t clears = 0;
        if ((clear->flags & k_gpu_clear_depth) != 0 && d->depth != 0)
            clears |= D3D12_CLEAR_FLAG_DEPTH;
        if ((clear->flags & k_gpu_clear_stencil) != 0 && d->depth != 0 && (d->depth->aspect & k_aspect_stencil) != 0)
            clears |= D3D12_CLEAR_FLAG_STENCIL;
        if (clears != 0)
            list->lpVtbl->ClearDepthStencilView(list, pool_cpu(&d->dsvs, d->depth->dsv), (D3D12_CLEAR_FLAGS)clears,
                                                clear->depth, (UINT8)clear->stencil, count, areas);
    }
    if (areas != local)
        free(areas);
}

// ---- binding and drawing ----

void bind_pipeline(s_d3d12 *d, const s_pipeline *pipeline, const s_gpu_graphics_state *state)
{
    if (recording_list(d, "gpu_cmd_bind_pipeline") == 0 || pipeline == 0)
        return;
    s_bind_state &b = d->bind[pipeline->bind_point == k_gpu_bind_compute ? 1 : 0];
    if (b.pipeline == pipeline)
        d->stats.pipelines_skipped++;
    else
        d->stats.pipelines++;
    b.pipeline = pipeline;
    if (pipeline->bind_point == k_gpu_bind_graphics) {
        d->stencil_wanted = state != 0 ? state->stencil_front.reference & 0xffu : pipeline->stencil_reference;
        memcpy(d->blend_wanted, state != 0 ? state->blend_constants : pipeline->blend_factor, sizeof(d->blend_wanted));
    }
}

void bind_group(s_d3d12 *d, const s_pipeline *pipeline, uint32_t set, gpu_bind_group handle, uint32_t offset_count,
                const uint32_t *offsets)
{
    if (recording_list(d, "gpu_cmd_bind_group") == 0 || pipeline == 0)
        return;
    s_group *group = pointer_of<s_group>(handle);
    if (group == 0)
        return;
    s_bind_state &b = d->bind[pipeline->bind_point == k_gpu_bind_compute ? 1 : 0];
    d->stats.sets++;
    if (group->kind == k_bind_constants) {
        // the dynamic offsets in binding order (every constant block of the layout is a dynamic uniform buffer)
        uint32_t next = 0;
        for (uint32_t i = 0; i <= XE_BINDING_VERTEX_FETCH; i++) {
            if (group->buffers[i] == 0) {
                b.cbv[i] = 0;
                continue;
            }
            const uint32_t dynamic = next < offset_count ? offsets[next] : 0u;
            next++;
            b.cbv[i] = group->buffers[i]->address + group->offsets[i] + dynamic;
        }
        (void)set;
    } else {
        b.textures = group;
    }
}

// The draw's or dispatch's bindings onto the recording list: root signature, pipeline, the root arguments that
// changed, the resource states its descriptors need. NULL when it cannot be drawn.
ID3D12GraphicsCommandList *prepare(s_d3d12 *d, uint32_t bind_point, const char *caller)
{
    ID3D12GraphicsCommandList *list = recording_list(d, caller);
    if (list == 0)
        return 0;
    s_bind_state &b = d->bind[bind_point];
    const s_pipeline *pipeline = b.pipeline;
    if (pipeline == 0 || pipeline->entry == 0) {
        log_text(d, "d3d12: %s without a pipeline bound", caller);
        return 0;
    }
    const bool graphics = bind_point == k_gpu_bind_graphics;
    ID3D12RootSignature *root = pipeline->layout->root;
    if (b.root != root) {
        if (graphics)
            list->lpVtbl->SetGraphicsRootSignature(list, root);
        else
            list->lpVtbl->SetComputeRootSignature(list, root);
        b.root = root;
        memset(b.cbv_set, 0, sizeof(b.cbv_set));
        memset(b.table_set, 0, sizeof(b.table_set));
    }
    ID3D12PipelineState *pso = pipeline->entry->pso;
    if (d->pso_set != pso) {
        list->lpVtbl->SetPipelineState(list, pso);
        d->pso_set = pso;
    }
    if (pipeline->layout->kind == k_layout_xenos) {
        for (uint32_t i = 0; i <= XE_BINDING_VERTEX_FETCH; i++) {
            const D3D12_GPU_VIRTUAL_ADDRESS address = b.cbv[i] != 0 ? b.cbv[i] : d->zero_buffer->address;
            if (b.cbv_set[i] == address)
                continue;
            if (graphics)
                list->lpVtbl->SetGraphicsRootConstantBufferView(list, XE_D3D12_ROOT_SYSTEM + i, address);
            else
                list->lpVtbl->SetComputeRootConstantBufferView(list, XE_D3D12_ROOT_SYSTEM + i, address);
            b.cbv_set[i] = address;
        }
        s_group *textures = (s_group *)b.textures;
        if (textures != 0 && (textures->lifetime == k_gpu_bind_frame || textures->lifetime == k_gpu_bind_frame_textures) &&
            textures->frame_serial != d->frame_serial)
            textures = 0; // a frame group of an earlier frame is gone
        if (textures != 0 && (!group_frame_copy(d, textures) || !place_samplers(d, textures)))
            return 0; // (barriers it queued stay for the frame list's next flush)
        uint64_t tables[4];
        tables[0] = view_heap_gpu(d, textures != 0 ? textures->srv_base : 0u);
        tables[1] = d->sampler_heap->gpu.ptr +
                    (uint64_t)(textures != 0 ? textures->sampler_base : 0u) * d->sampler_increment;
        const uint32_t memory = b.memory_base != k_none && b.memory_frame == d->frame_serial ? b.memory_base : k_group_srvs;
        tables[2] = view_heap_gpu(d, memory);
        tables[3] = view_heap_gpu(d, memory + XE_D3D12_VERTEX_STREAMS);
        for (uint32_t t = 0; t < 4; t++) {
            if (b.table_set[t] == tables[t])
                continue;
            D3D12_GPU_DESCRIPTOR_HANDLE table;
            table.ptr = tables[t];
            if (graphics)
                list->lpVtbl->SetGraphicsRootDescriptorTable(list, XE_D3D12_ROOT_TEXTURES + t, table);
            else
                list->lpVtbl->SetComputeRootDescriptorTable(list, XE_D3D12_ROOT_TEXTURES + t, table);
            b.table_set[t] = tables[t];
        }
        if (textures != 0) {
            for (uint32_t i = 0; i < textures->image_count; i++) {
                s_image *image = textures->images[i];
                if (d->rendering && (image->state == D3D12_RESOURCE_STATE_RENDER_TARGET ||
                                     image->state == D3D12_RESOURCE_STATE_DEPTH_WRITE)) {
                    bool attached = image == d->depth;
                    for (uint32_t c = 0; c < d->color_count && !attached; c++)
                        attached = image == d->colors[c];
                    if (attached) {
                        if (report_once(d, k_report_attachment_sampled))
                            log_text(d, "d3d12: a draw samples an image it renders to (left as an attachment: its "
                                        "texels read are undefined)");
                        continue;
                    }
                }
                image_state(d, image, k_state_shader_read);
            }
        }
        if (b.memory_base != k_none && b.memory_frame == d->frame_serial) {
            for (uint32_t s = 0; s < XE_D3D12_VERTEX_STREAMS; s++)
                buffer_state(d, b.streams[s], k_list_frame, k_state_shader_read);
            for (uint32_t e = 0; e < XE_D3D12_MEMEXPORT_BUFFERS; e++) {
                s_buffer *exported = b.exports[e];
                if (exported == 0)
                    continue;
                for (uint32_t s = 0; s < XE_D3D12_VERTEX_STREAMS; s++) {
                    if (b.streams[s] == exported && report_once(d, k_report_stream_export_alias))
                        log_text(d, "d3d12: a draw reads a vertex stream from the buffer it exports to (bound for "
                                    "the export: the stream reads are undefined)");
                }
                buffer_state(d, exported, k_list_frame, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            }
        }
    }
    if (graphics) {
        if (d->topology_set != pipeline->topology) {
            list->lpVtbl->IASetPrimitiveTopology(list, pipeline->topology);
            d->topology_set = pipeline->topology;
        }
        if (!d->dynamic_valid || d->stencil_ref_set != d->stencil_wanted) {
            list->lpVtbl->OMSetStencilRef(list, d->stencil_wanted);
            d->stencil_ref_set = d->stencil_wanted;
        }
        if (pipeline->uses_blend_factor) {
            float factor[4];
            memcpy(factor, d->blend_wanted, sizeof(factor));
            if (pipeline->alpha_splat)
                factor[0] = factor[1] = factor[2] = factor[3];
            if (!d->dynamic_valid || memcmp(d->blend_factor_set, factor, sizeof(factor)) != 0) {
                list->lpVtbl->OMSetBlendFactor(list, factor);
                memcpy(d->blend_factor_set, factor, sizeof(factor));
            }
        } else if (!d->dynamic_valid) {
            static const float k_zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            list->lpVtbl->OMSetBlendFactor(list, k_zero);
            memset(d->blend_factor_set, 0, sizeof(d->blend_factor_set));
        }
        d->dynamic_valid = 1;
        if (!d->viewport_valid) {
            list->lpVtbl->RSSetViewports(list, 1, &d->viewport);
            d->viewport_valid = 1;
        }
        if (!d->scissor_valid) {
            list->lpVtbl->RSSetScissorRects(list, 1, &d->scissor);
            d->scissor_valid = 1;
        }
    }
    flush_barriers(d, list);
    return list;
}

void bind_index_buffer(s_d3d12 *d, const s_buffer *buffer, uint32_t offset, uint32_t index_type)
{
    if (recording_list(d, "gpu_cmd_bind_index_buffer") == 0 || buffer == 0)
        return;
    if (d->index_valid && d->index_buffer == buffer && d->index_offset == offset && d->index_type == index_type) {
        d->stats.index_buffers_skipped++;
        return;
    }
    d->index_buffer = (s_buffer *)buffer;
    d->index_offset = offset;
    d->index_type = index_type;
    d->index_valid = 0;
    d->stats.index_buffers++;
}

void draw_indexed(s_d3d12 *d, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                  int32_t vertex_offset, uint32_t first_instance)
{
    if (!d->rendering) {
        log_text(d, "d3d12: gpu_cmd_draw_indexed outside rendering");
        return;
    }
    if (d->index_buffer == 0) {
        log_text(d, "d3d12: gpu_cmd_draw_indexed without an index buffer");
        return;
    }
    buffer_state(d, d->index_buffer, k_list_frame, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    ID3D12GraphicsCommandList *list = prepare(d, k_gpu_bind_graphics, "gpu_cmd_draw_indexed");
    if (list == 0)
        return;
    if (!d->index_valid) {
        D3D12_INDEX_BUFFER_VIEW view;
        view.BufferLocation = d->index_buffer->address + d->index_offset;
        view.SizeInBytes = d->index_buffer->allocated > d->index_offset ? d->index_buffer->allocated - d->index_offset : 0u;
        view.Format = d->index_type == k_gpu_index_uint16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        list->lpVtbl->IASetIndexBuffer(list, &view);
        d->index_valid = 1;
    }
    list->lpVtbl->DrawIndexedInstanced(list, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void draw(s_d3d12 *d, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    if (!d->rendering) {
        log_text(d, "d3d12: gpu_cmd_draw outside rendering");
        return;
    }
    if (ID3D12GraphicsCommandList *list = prepare(d, k_gpu_bind_graphics, "gpu_cmd_draw"))
        list->lpVtbl->DrawInstanced(list, vertex_count, instance_count, first_vertex, first_instance);
}

void dispatch(s_d3d12 *d, uint32_t x, uint32_t y, uint32_t z)
{
    if (ID3D12GraphicsCommandList *list = prepare(d, k_gpu_bind_compute, "gpu_cmd_dispatch"))
        list->lpVtbl->Dispatch(list, x, y, z);
}

// The vertex-stream and memexport views of the next draws or dispatch: a 20-descriptor table of the frame (raw buffer
// views; null views where nothing is bound), shared with every draw of the frame binding the same buffers.
void xenos_bind_memory(s_d3d12 *d, const s_pipeline *pipeline, s_buffer *const *streams, s_buffer *const *exports)
{
    if (recording_list(d, "gpu_cmd_xenos_bind_memory") == 0 || pipeline == 0)
        return;
    s_bind_state &b = d->bind[pipeline->bind_point == k_gpu_bind_compute ? 1 : 0];
    uintptr_t key[k_memory_views];
    for (uint32_t s = 0; s < XE_D3D12_VERTEX_STREAMS; s++) {
        b.streams[s] = streams != 0 ? streams[s] : 0;
        key[s] = (uintptr_t)b.streams[s];
    }
    for (uint32_t e = 0; e < XE_D3D12_MEMEXPORT_BUFFERS; e++) {
        b.exports[e] = exports != 0 ? exports[e] : 0;
        key[XE_D3D12_VERTEX_STREAMS + e] = (uintptr_t)b.exports[e];
    }
    s_frame &frame = d->frames[d->frame_index];
    const uint32_t slot = gpu_hash_bytes(2166136261u, key, sizeof(key)) & 255u;
    if (frame.memory_bases[slot] != k_none && memcmp(frame.memory_keys[slot], key, sizeof(key)) == 0) {
        b.memory_base = frame.memory_bases[slot];
        b.memory_frame = d->frame_serial;
        return;
    }
    const uint32_t base = frame_descriptors(d, k_memory_views);
    b.memory_base = base;
    b.memory_frame = d->frame_serial;
    if (base == k_none)
        return;
    for (uint32_t s = 0; s < XE_D3D12_VERTEX_STREAMS; s++) {
        const s_buffer *buffer = b.streams[s];
        if (buffer != 0 && buffer->srv == k_none && report_once(d, k_report_storage_view))
            log_text(d, "d3d12: a vertex stream in a buffer without storage use (read as zeros)");
        copy_view(d, base + s, buffer != 0 && buffer->srv != k_none ? buffer->srv : d->null_srv_buffer);
    }
    for (uint32_t e = 0; e < XE_D3D12_MEMEXPORT_BUFFERS; e++) {
        const s_buffer *buffer = b.exports[e];
        if (buffer != 0 && buffer->uav == k_none && report_once(d, k_report_storage_view))
            log_text(d, "d3d12: a memexport into a buffer without storage use in device memory (dropped)");
        copy_view(d, base + XE_D3D12_VERTEX_STREAMS + e,
                  buffer != 0 && buffer->uav != k_none ? buffer->uav : d->null_uav_buffer);
    }
    memcpy(frame.memory_keys[slot], key, sizeof(key));
    frame.memory_bases[slot] = base;
}

// ---- copies ----

// The subresource index of a level, layer and plane.
UINT subresource(const s_image *image, uint32_t level, uint32_t layer, uint32_t plane)
{
    const uint32_t layers = image->desc.view_type == k_gpu_view_3d ? 1u : image->desc.array_layers;
    return level + layer * image->desc.mip_levels + plane * image->desc.mip_levels * layers;
}

D3D12_TEXTURE_COPY_LOCATION texture_location(const s_image *image, UINT index)
{
    D3D12_TEXTURE_COPY_LOCATION location = {};
    location.pResource = image->resource;
    location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    location.SubresourceIndex = index;
    return location;
}

D3D12_TEXTURE_COPY_LOCATION buffer_location(const s_buffer *buffer, uint32_t offset, DXGI_FORMAT format, uint32_t width,
                                            uint32_t height, uint32_t row_pitch)
{
    D3D12_TEXTURE_COPY_LOCATION location = {};
    location.pResource = buffer->resource;
    location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    location.PlacedFootprint.Offset = offset;
    location.PlacedFootprint.Footprint.Format = format;
    location.PlacedFootprint.Footprint.Width = width;
    location.PlacedFootprint.Footprint.Height = height;
    location.PlacedFootprint.Footprint.Depth = 1;
    location.PlacedFootprint.Footprint.RowPitch = row_pitch;
    return location;
}

// Whether a buffer footprint at `offset` with `row_pitch` is one CopyTextureRegion takes as is.
bool footprint_placeable(const s_d3d12 *d, uint32_t offset, uint32_t row_pitch, uint32_t block_bytes)
{
    if (d->unrestricted_copy_pitch)
        return offset % 4u == 0 && offset % block_bytes == 0 && row_pitch % block_bytes == 0;
    return offset % D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT == 0 && row_pitch % D3D12_TEXTURE_DATA_PITCH_ALIGNMENT == 0;
}

void copy_buffer(s_d3d12 *d, s_buffer *source, uint32_t source_offset, s_buffer *destination,
                 uint32_t destination_offset, uint32_t size)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_copy_buffer");
    if (list == 0 || source == 0 || destination == 0 || size == 0)
        return;
    if (d->rendering)
        end_rendering(d);
    if (source == destination) {
        // one resource cannot be the copy's source and destination at once: through a scratch buffer
        s_buffer *scratch = buffer_make(d, size, k_gpu_buffer_transfer_src, k_gpu_memory_gpu);
        if (scratch == 0)
            return;
        buffer_state(d, source, k_list_frame, D3D12_RESOURCE_STATE_COPY_SOURCE);
        buffer_state(d, scratch, k_list_frame, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        list->lpVtbl->CopyBufferRegion(list, scratch->resource, 0, source->resource, source_offset, size);
        buffer_state(d, scratch, k_list_frame, D3D12_RESOURCE_STATE_COPY_SOURCE);
        buffer_state(d, destination, k_list_frame, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        list->lpVtbl->CopyBufferRegion(list, destination->resource, destination_offset, scratch->resource, 0, size);
        retire(d, k_retired_buffer, scratch);
        return;
    }
    buffer_state(d, source, k_list_frame, D3D12_RESOURCE_STATE_COPY_SOURCE);
    buffer_state(d, destination, k_list_frame, D3D12_RESOURCE_STATE_COPY_DEST);
    flush_barriers(d, list);
    list->lpVtbl->CopyBufferRegion(list, destination->resource, destination_offset, source->resource, source_offset, size);
}

void copy_buffer_to_image(s_d3d12 *d, s_buffer *buffer, uint32_t offset, uint32_t row_length, s_image *image,
                          uint32_t width, uint32_t height)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_copy_buffer_to_image");
    if (list == 0 || buffer == 0 || image == 0 || width == 0 || height == 0)
        return;
    uint32_t block_width, block_height, block_bytes;
    if (image->decode_bc || image->aspect != k_aspect_color ||
        !gpu_format_block_info(image->desc.format, &block_width, &block_height, &block_bytes)) {
        log_text(d, "d3d12: gpu_cmd_copy_buffer_to_image into a %s image it cannot copy to",
                 gpu_format_name(image->desc.format));
        return;
    }
    if (d->rendering)
        end_rendering(d);
    const uint32_t texels = row_length != 0 ? row_length : width;
    const uint32_t pitch = (texels + block_width - 1u) / block_width * block_bytes;
    const uint32_t rows = (height + block_height - 1u) / block_height;
    const DXGI_FORMAT format = format_of(image->internal_format).format;
    const uint32_t copy_width = align_up(width, block_width), copy_height = align_up(height, block_height);
    image_state(d, image, D3D12_RESOURCE_STATE_COPY_DEST);
    D3D12_TEXTURE_COPY_LOCATION destination = texture_location(image, 0);
    if (footprint_placeable(d, offset, pitch, block_bytes)) {
        buffer_state(d, buffer, k_list_frame, D3D12_RESOURCE_STATE_COPY_SOURCE);
        flush_barriers(d, list);
        const D3D12_TEXTURE_COPY_LOCATION source = buffer_location(buffer, offset, format, copy_width, copy_height, pitch);
        list->lpVtbl->CopyTextureRegion(list, &destination, 0, 0, 0, &source, 0);
    } else {
        // the rows moved to a footprint Direct3D 12 can copy from (256-byte row pitch at offset 0)
        const uint32_t aligned = align_up(pitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        s_buffer *scratch = buffer_make(d, aligned * rows, k_gpu_buffer_transfer_src, k_gpu_memory_gpu);
        if (scratch == 0)
            return;
        buffer_state(d, buffer, k_list_frame, D3D12_RESOURCE_STATE_COPY_SOURCE);
        buffer_state(d, scratch, k_list_frame, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        const uint32_t row_bytes = (width + block_width - 1u) / block_width * block_bytes;
        for (uint32_t y = 0; y < rows; y++)
            list->lpVtbl->CopyBufferRegion(list, scratch->resource, (UINT64)aligned * y, buffer->resource,
                                           (UINT64)offset + (UINT64)pitch * y, row_bytes);
        buffer_state(d, scratch, k_list_frame, D3D12_RESOURCE_STATE_COPY_SOURCE);
        flush_barriers(d, list);
        const D3D12_TEXTURE_COPY_LOCATION source = buffer_location(scratch, 0, format, copy_width, copy_height, aligned);
        list->lpVtbl->CopyTextureRegion(list, &destination, 0, 0, 0, &source, 0);
        retire(d, k_retired_buffer, scratch);
    }
    image_state(d, image, k_state_shader_read);
    flush_barriers(d, list);
}

void copy_image_to_buffer(s_d3d12 *d, s_image *image, uint32_t level, uint32_t layer, int32_t x, int32_t y,
                          uint32_t width, uint32_t height, s_buffer *buffer, uint32_t offset)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_copy_image_to_buffer");
    if (list == 0 || image == 0 || buffer == 0 || width == 0 || height == 0)
        return;
    uint32_t block_width, block_height, block_bytes;
    if (image->aspect != k_aspect_color ||
        !gpu_format_block_info(image->internal_format, &block_width, &block_height, &block_bytes)) {
        log_text(d, "d3d12: gpu_cmd_copy_image_to_buffer from a %s image it cannot copy from",
                 gpu_format_name(image->desc.format));
        return;
    }
    if (d->rendering)
        end_rendering(d);
    const uint32_t before = image->state;
    image_state(d, image, D3D12_RESOURCE_STATE_COPY_SOURCE);
    const uint32_t pitch = (width + block_width - 1u) / block_width * block_bytes;
    const uint32_t rows = (height + block_height - 1u) / block_height;
    const DXGI_FORMAT format = format_of(image->internal_format).format;
    const uint32_t copy_width = align_up(width, block_width), copy_height = align_up(height, block_height);
    const D3D12_TEXTURE_COPY_LOCATION source = texture_location(image, subresource(image, level, layer, 0));
    const D3D12_BOX box = { (UINT)x, (UINT)y, 0, (UINT)x + copy_width, (UINT)y + copy_height, 1 };
    if (footprint_placeable(d, offset, pitch, block_bytes)) {
        buffer_state(d, buffer, k_list_frame, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        const D3D12_TEXTURE_COPY_LOCATION destination =
            buffer_location(buffer, offset, format, copy_width, copy_height, pitch);
        list->lpVtbl->CopyTextureRegion(list, &destination, 0, 0, 0, &source, &box);
    } else {
        // tight rows at any offset (as Vulkan writes them): through a scratch buffer of 256-byte rows
        const uint32_t aligned = align_up(pitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        s_buffer *scratch = buffer_make(d, aligned * rows, k_gpu_buffer_transfer_src, k_gpu_memory_gpu);
        if (scratch == 0)
            return;
        buffer_state(d, scratch, k_list_frame, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        const D3D12_TEXTURE_COPY_LOCATION destination =
            buffer_location(scratch, 0, format, copy_width, copy_height, aligned);
        list->lpVtbl->CopyTextureRegion(list, &destination, 0, 0, 0, &source, &box);
        buffer_state(d, scratch, k_list_frame, D3D12_RESOURCE_STATE_COPY_SOURCE);
        buffer_state(d, buffer, k_list_frame, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        for (uint32_t row = 0; row < rows; row++)
            list->lpVtbl->CopyBufferRegion(list, buffer->resource, (UINT64)offset + (UINT64)pitch * row,
                                           scratch->resource, (UINT64)aligned * row, pitch);
        retire(d, k_retired_buffer, scratch);
    }
    // back to where it was (a resolved texture stays ready for sampling)
    if (before != D3D12_RESOURCE_STATE_COMMON)
        image_state(d, image, before);
    flush_barriers(d, list);
}

void readback(s_d3d12 *d, s_render_target *target)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_readback");
    if (list == 0 || target == 0)
        return;
    if (d->rendering)
        end_rendering(d);
    image_state(d, target->color, D3D12_RESOURCE_STATE_COPY_SOURCE);
    flush_barriers(d, list);
    const D3D12_TEXTURE_COPY_LOCATION source = texture_location(target->color, 0);
    const D3D12_TEXTURE_COPY_LOCATION destination =
        buffer_location(target->readback, 0, format_of(target->color->internal_format).format, target->width,
                        target->height, target->row_pitch);
    list->lpVtbl->CopyTextureRegion(list, &destination, 0, 0, 0, &source, 0);
}

// ---- copies by drawing (d3d12_copy.hlsl) ----

enum e_copy_pass {
    k_copy_color = 0,
    k_copy_present,
    k_copy_depth,
    k_copy_stencil,
};

// The pipeline of a copy pass into targets of `format` (the render target's, or the depth / stencil view's), the
// stencil pass writing bit `bit`.
s_pso_entry *copy_pass(s_d3d12 *d, uint32_t pass, DXGI_FORMAT format, uint32_t bit)
{
    const uint32_t key_value = pass << 24 | bit << 16 | (uint32_t)format;
    for (uint32_t i = 0; i < d->copy_passes.count; i++)
        if (d->copy_passes.items[i].key == key_value)
            return d->copy_passes.items[i].entry;
    struct s_code {
        const uint8_t *code;
        size_t size;
    };
    static const s_code k_pixel[4] = {
        { k_d3d12_copy_color_dxil, sizeof(k_d3d12_copy_color_dxil) },
        { k_d3d12_copy_present_dxil, sizeof(k_d3d12_copy_present_dxil) },
        { k_d3d12_copy_depth_dxil, sizeof(k_d3d12_copy_depth_dxil) },
        { k_d3d12_copy_stencil_dxil, sizeof(k_d3d12_copy_stencil_dxil) },
    };
    s_pso_key key;
    memset(&key, 0, sizeof(key));
    key.kind = k_pso_graphics;
    key.layout = k_layout_copy;
    container_digest(k_d3d12_copy_vs_dxil, sizeof(k_d3d12_copy_vs_dxil), key.digests[0]);
    container_digest(k_pixel[pass].code, k_pixel[pass].size, key.digests[1]);
    D3D12_GRAPHICS_PIPELINE_STATE_DESC &desc = key.graphics;
    for (uint32_t i = 0; i < 8; i++) {
        desc.BlendState.RenderTarget[i].SrcBlend = desc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
        desc.BlendState.RenderTarget[i].DestBlend = desc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
        desc.BlendState.RenderTarget[i].BlendOp = desc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        desc.BlendState.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;
        desc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }
    desc.SampleMask = 0xffffffffu;
    desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    desc.RasterizerState.DepthClipEnable = 1;
    desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.SampleDesc.Count = 1;
    if (pass == k_copy_color || pass == k_copy_present) {
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = format;
    } else {
        desc.DSVFormat = format;
        D3D12_DEPTH_STENCIL_DESC &depth = desc.DepthStencilState;
        if (pass == k_copy_depth) {
            depth.DepthEnable = 1;
            depth.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        } else {
            depth.StencilEnable = 1;
            depth.StencilReadMask = 0xff;
            depth.StencilWriteMask = (UINT8)(1u << bit);
            D3D12_DEPTH_STENCILOP_DESC *faces[2] = { &depth.FrontFace, &depth.BackFace };
            for (uint32_t f = 0; f < 2; f++) {
                faces[f]->StencilFailOp = D3D12_STENCIL_OP_KEEP;
                faces[f]->StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
                faces[f]->StencilPassOp = D3D12_STENCIL_OP_REPLACE;
                faces[f]->StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            }
        }
        for (uint32_t f = 0; f < 2; f++) {
            D3D12_DEPTH_STENCILOP_DESC &face = f == 0 ? depth.FrontFace : depth.BackFace;
            if (face.StencilFunc == 0) {
                face.StencilFailOp = face.StencilDepthFailOp = face.StencilPassOp = D3D12_STENCIL_OP_KEEP;
                face.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            }
        }
    }
    if (pass == k_copy_color || pass == k_copy_present) {
        D3D12_DEPTH_STENCILOP_DESC *faces[2] = { &desc.DepthStencilState.FrontFace, &desc.DepthStencilState.BackFace };
        for (uint32_t f = 0; f < 2; f++) {
            faces[f]->StencilFailOp = faces[f]->StencilDepthFailOp = faces[f]->StencilPassOp = D3D12_STENCIL_OP_KEEP;
            faces[f]->StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        }
    }
    D3D12_GRAPHICS_PIPELINE_STATE_DESC full = desc;
    full.pRootSignature = d->copy_layout.root;
    full.VS.pShaderBytecode = k_d3d12_copy_vs_dxil;
    full.VS.BytecodeLength = sizeof(k_d3d12_copy_vs_dxil);
    full.PS.pShaderBytecode = k_pixel[pass].code;
    full.PS.BytecodeLength = k_pixel[pass].size;
    s_pso_entry *entry = pso_get(d, key, &full);
    if (entry == 0)
        return 0;
    s_copy_pass *slot = d->copy_passes.push();
    if (slot == 0) {
        pso_put(d, entry);
        return 0;
    }
    slot->entry = entry;
    slot->key = key_value;
    return entry;
}

// One draw of a copy pass into the targets bound (OMSetRenderTargets): `rect` its destination rectangle (viewport and
// scissor), `constants` d3d12_copy.hlsl's copy_constants, `source` t0 and `stencil` t1.
bool copy_draw(s_d3d12 *d, ID3D12GraphicsCommandList *list, s_pso_entry *pass, const s_view *source,
               const s_view *stencil, const int32_t constants[12], const D3D12_RECT &rect)
{
    if (pass == 0)
        return false;
    const uint32_t base = frame_descriptors(d, 2);
    if (base == k_none)
        return false;
    copy_view(d, base, source != 0 && source->descriptor != k_none ? source->descriptor : d->null_srv_2d);
    copy_view(d, base + 1u, stencil != 0 && stencil->descriptor != k_none ? stencil->descriptor : d->null_srv_2d);
    s_bind_state &b = d->bind[k_gpu_bind_graphics];
    if (b.root != d->copy_layout.root) {
        list->lpVtbl->SetGraphicsRootSignature(list, d->copy_layout.root);
        forget_bindings(d, k_gpu_bind_graphics);
        b.root = d->copy_layout.root;
    }
    if (d->pso_set != pass->pso) {
        list->lpVtbl->SetPipelineState(list, pass->pso);
        d->pso_set = pass->pso;
    }
    if (d->topology_set != D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
        list->lpVtbl->IASetPrimitiveTopology(list, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        d->topology_set = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    const D3D12_VIEWPORT viewport = { (float)rect.left, (float)rect.top, (float)(rect.right - rect.left),
                                      (float)(rect.bottom - rect.top), 0.0f, 1.0f };
    list->lpVtbl->RSSetViewports(list, 1, &viewport);
    list->lpVtbl->RSSetScissorRects(list, 1, &rect);
    d->viewport_valid = d->scissor_valid = 0; // the frontend's go back before its next draw
    list->lpVtbl->SetGraphicsRoot32BitConstants(list, 0, 12, constants, 0);
    D3D12_GPU_DESCRIPTOR_HANDLE table;
    table.ptr = view_heap_gpu(d, base);
    list->lpVtbl->SetGraphicsRootDescriptorTable(list, 1, table);
    list->lpVtbl->DrawInstanced(list, 3, 1, 0, 0);
    return true;
}

// Rectangles of a depth / stencil image into another of its format, depth and (where both have it) stencil: whole
// subresources by CopyResource, others by drawing (Direct3D 12 copies depth / stencil only whole): the depth by
// SV_Depth, the stencil cleared and then set bit by bit.
void depth_copy_rects(s_d3d12 *d, ID3D12GraphicsCommandList *list, s_image *source, s_image *destination,
                      const s_gpu_copy_rect *rects, uint32_t count)
{
    const s_gpu_copy_rect &first = rects[0];
    const bool whole = count == 1 && first.source_x == 0 && first.source_y == 0 && first.destination_x == 0 &&
                       first.destination_y == 0 && first.width == source->desc.width &&
                       first.height == source->desc.height && source->desc.width == destination->desc.width &&
                       source->desc.height == destination->desc.height &&
                       source->desc.mip_levels == destination->desc.mip_levels &&
                       source->desc.array_layers == destination->desc.array_layers;
    if (whole) {
        image_state(d, source, D3D12_RESOURCE_STATE_COPY_SOURCE);
        image_state(d, destination, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        list->lpVtbl->CopyResource(list, destination->resource, source->resource);
        return;
    }
    if (destination->dsv == k_none)
        return;
    image_state(d, source, k_state_shader_read);
    image_state(d, destination, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    flush_barriers(d, list);
    const D3D12_CPU_DESCRIPTOR_HANDLE dsv = pool_cpu(&d->dsvs, destination->dsv);
    list->lpVtbl->OMSetRenderTargets(list, 0, 0, 0, &dsv);
    const DXGI_FORMAT format = format_of(destination->internal_format).format;
    D3D12_RECT local[16];
    D3D12_RECT *areas = count <= 16u ? local : (D3D12_RECT *)malloc(count * sizeof(D3D12_RECT));
    if (areas == 0)
        return;
    uint32_t n = 0;
    s_pso_entry *depth_pass = copy_pass(d, k_copy_depth, format, 0);
    for (uint32_t i = 0; i < count; i++) {
        const s_gpu_copy_rect &r = rects[i];
        if (r.width == 0 || r.height == 0)
            continue;
        const int32_t constants[12] = { r.source_x, r.source_y, (int32_t)r.width, (int32_t)r.height,
                                        r.destination_x, r.destination_y, (int32_t)r.width, (int32_t)r.height };
        areas[n] = D3D12_RECT{ r.destination_x, r.destination_y, r.destination_x + (int32_t)r.width,
                               r.destination_y + (int32_t)r.height };
        copy_draw(d, list, depth_pass, &source->raw_view, 0, constants, areas[n]);
        n++;
    }
    if (n != 0 && (source->aspect & destination->aspect & k_aspect_stencil) != 0) {
        list->lpVtbl->ClearDepthStencilView(list, dsv, D3D12_CLEAR_FLAG_STENCIL, 0.0f, 0, n, areas);
        list->lpVtbl->OMSetStencilRef(list, 0xffu);
        d->dynamic_valid = 0; // the draws' own stencil reference goes back before the next draw
        for (uint32_t bit = 0; bit < 8; bit++) {
            s_pso_entry *stencil_pass = copy_pass(d, k_copy_stencil, format, bit);
            uint32_t a = 0;
            for (uint32_t i = 0; i < count; i++) {
                const s_gpu_copy_rect &r = rects[i];
                if (r.width == 0 || r.height == 0)
                    continue;
                const int32_t constants[12] = { r.source_x, r.source_y, (int32_t)r.width, (int32_t)r.height,
                                                r.destination_x, r.destination_y, (int32_t)r.width,
                                                (int32_t)r.height, (int32_t)bit };
                copy_draw(d, list, stencil_pass, 0, &source->stencil_view, constants, areas[a++]);
            }
        }
    }
    if (areas != local)
        free(areas);
}

void copy_image(s_d3d12 *d, s_image *source, int32_t source_x, int32_t source_y, uint32_t width, uint32_t height,
                s_image *destination, uint32_t level, uint32_t layer, int32_t destination_x, int32_t destination_y,
                uint32_t destination_width, uint32_t destination_height)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_copy_image");
    if (list == 0 || source == 0 || destination == 0 || width == 0 || height == 0)
        return;
    if (d->rendering)
        end_rendering(d);
    const uint32_t out_width = destination_width != 0 ? destination_width : width;
    const uint32_t out_height = destination_height != 0 ? destination_height : height;
    const bool same_size = out_width == width && out_height == height;
    if (source->aspect != k_aspect_color || destination->aspect != k_aspect_color) {
        if (source->desc.format == destination->desc.format && same_size && level == 0 && layer == 0 &&
            source != destination) {
            const s_gpu_copy_rect rect = { source_x, source_y, destination_x, destination_y, width, height };
            depth_copy_rects(d, list, source, destination, &rect, 1);
            image_state(d, destination, k_state_shader_read);
            flush_barriers(d, list);
        } else if (report_once(d, k_report_depth_copy)) {
            log_text(d, "d3d12: a scaled or converting depth / stencil blit (skipped)");
        }
        return;
    }
    if (same_size && source->internal_format == destination->internal_format && source != destination) {
        // an exact copy (a blit of equal sizes and formats)
        image_state(d, source, D3D12_RESOURCE_STATE_COPY_SOURCE);
        image_state(d, destination, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        const D3D12_TEXTURE_COPY_LOCATION to = texture_location(destination, subresource(destination, level, layer, 0));
        const D3D12_TEXTURE_COPY_LOCATION from = texture_location(source, 0);
        const D3D12_BOX box = { (UINT)source_x, (UINT)source_y, 0, (UINT)(source_x + (int32_t)width),
                                (UINT)(source_y + (int32_t)height), 1 };
        list->lpVtbl->CopyTextureRegion(list, &to, (UINT)destination_x, (UINT)destination_y, 0, &from, &box);
    } else {
        // scaled (nearest) or converted by value: a draw into the destination
        if ((destination->flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) == 0 ||
            !format_renderable(d, destination->internal_format) || format_is_integer(source->internal_format) ||
            format_is_integer(destination->internal_format) || source == destination) {
            if (report_once(d, k_report_copy_format))
                log_text(d, "d3d12: a blit from %s into %s it cannot draw (skipped)",
                         gpu_format_name(source->desc.format), gpu_format_name(destination->desc.format));
            return;
        }
        image_state(d, source, k_state_shader_read);
        image_state(d, destination, D3D12_RESOURCE_STATE_RENDER_TARGET);
        flush_barriers(d, list);
        const DXGI_FORMAT format = format_of(destination->internal_format).format;
        const D3D12_CPU_DESCRIPTOR_HANDLE rtv =
            level == 0 && layer == 0 && destination->rtv != k_none
                ? pool_cpu(&d->rtvs, destination->rtv)
                : transient_rtv(d, destination->resource, format, level, layer,
                                destination->desc.view_type == k_gpu_view_3d);
        list->lpVtbl->OMSetRenderTargets(list, 1, &rtv, 0, 0);
        const int32_t constants[12] = { source_x, source_y, (int32_t)width, (int32_t)height, destination_x,
                                        destination_y, (int32_t)out_width, (int32_t)out_height };
        const D3D12_RECT rect = { destination_x, destination_y, destination_x + (int32_t)out_width,
                                  destination_y + (int32_t)out_height };
        copy_draw(d, list, copy_pass(d, k_copy_color, format, 0), &source->raw_view, 0, constants, rect);
    }
    image_state(d, destination, k_state_shader_read);
    flush_barriers(d, list);
}

void copy_depth(s_d3d12 *d, s_image *source, int32_t source_x, int32_t source_y, uint32_t width, uint32_t height,
                s_image *destination, int32_t destination_x, int32_t destination_y)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_copy_depth");
    if (list == 0 || source == 0 || destination == 0 || width == 0 || height == 0)
        return;
    if ((source->aspect & k_aspect_depth) == 0 || source->desc.format != destination->desc.format ||
        source == destination) {
        log_text(d, "d3d12: gpu_cmd_copy_depth needs two depth images of one format (%s, %s)",
                 gpu_format_name(source->desc.format), gpu_format_name(destination->desc.format));
        return;
    }
    if (d->rendering)
        end_rendering(d);
    const s_gpu_copy_rect rect = { source_x, source_y, destination_x, destination_y, width, height };
    depth_copy_rects(d, list, source, destination, &rect, 1);
    image_state(d, destination, k_state_shader_read);
    flush_barriers(d, list);
}

void copy_rects(s_d3d12 *d, s_image *source, s_image *destination, const s_gpu_copy_rect *rects, uint32_t count)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_copy_rects");
    if (list == 0 || source == 0 || destination == 0 || source == destination || count == 0)
        return;
    // what the resources hold: a BC image without OPTIONS8 may hold its blocks decoded (internal_format)
    if (source->desc.format != destination->desc.format || source->internal_format != destination->internal_format) {
        log_text(d, "d3d12: gpu_cmd_copy_rects needs two images of one format (%s as %s, %s as %s)",
                 gpu_format_name(source->desc.format), gpu_format_name(source->internal_format),
                 gpu_format_name(destination->desc.format), gpu_format_name(destination->internal_format));
        return;
    }
    bool any = false;
    for (uint32_t i = 0; i < count && !any; i++)
        any = rects[i].width != 0 && rects[i].height != 0;
    if (!any)
        return;
    if (d->rendering)
        end_rendering(d);
    if (source->aspect != k_aspect_color) {
        depth_copy_rects(d, list, source, destination, rects, count);
    } else {
        image_state(d, source, D3D12_RESOURCE_STATE_COPY_SOURCE);
        image_state(d, destination, D3D12_RESOURCE_STATE_COPY_DEST);
        flush_barriers(d, list);
        const D3D12_TEXTURE_COPY_LOCATION to = texture_location(destination, 0), from = texture_location(source, 0);
        for (uint32_t i = 0; i < count; i++) {
            const s_gpu_copy_rect &r = rects[i];
            if (r.width == 0 || r.height == 0)
                continue;
            const D3D12_BOX box = { (UINT)r.source_x, (UINT)r.source_y, 0, (UINT)(r.source_x + (int32_t)r.width),
                                    (UINT)(r.source_y + (int32_t)r.height), 1 };
            list->lpVtbl->CopyTextureRegion(list, &to, (UINT)r.destination_x, (UINT)r.destination_y, 0, &from, &box);
        }
    }
    // a texture is left for sampling; an attachment or a scratch image as it is (its next use moves it)
    if ((destination->desc.usage & k_gpu_image_sampled) != 0)
        image_state(d, destination, k_state_shader_read);
    flush_barriers(d, list);
}

// ---- the compute helper passes (helper_binding.hlsli: t0, t1, u0 in a table, b0 a root CBV) ----

s_pso_entry *helper_pso(s_d3d12 *d, uint32_t helper, const char *name)
{
    if (d->helpers[helper] != 0)
        return d->helpers[helper];
    if (d->helper_failed[helper])
        return 0;
    struct s_code {
        const uint8_t *code;
        size_t size;
    };
    static const s_code k_code[k_helper_count] = {
        { k_edram_7e3_dxil, sizeof(k_edram_7e3_dxil) },
        { k_pwl_decode_dxil, sizeof(k_pwl_decode_dxil) },
        { k_pwl_encode_dxil, sizeof(k_pwl_encode_dxil) },
        { k_depth_pack_20e4_dxil, sizeof(k_depth_pack_20e4_dxil) },
        { k_depth_pack_unorm24_dxil, sizeof(k_depth_pack_unorm24_dxil) },
        { k_display_gamma_dxil, sizeof(k_display_gamma_dxil) },
        { k_resolve_scale_dxil, sizeof(k_resolve_scale_dxil) },
        { k_resolve_downscale_dxil, sizeof(k_resolve_downscale_dxil) },
    };
    s_shader shader = {};
    shader.code = k_code[helper].code;
    shader.size = k_code[helper].size;
    container_digest(shader.code, shader.size, shader.digest);
    d->helpers[helper] = compute_pso(d, &shader, &d->helper_layout);
    if (d->helpers[helper] == 0) {
        log_text(d, "d3d12: the %s pipeline could not be made", name);
        d->helper_failed[helper] = 1;
    }
    return d->helpers[helper];
}

// One helper dispatch: 8x8 groups over width x height of the destination's storage view.
bool helper_pass(s_d3d12 *d, uint32_t helper, const char *name, s_image *source, const s_view *t0, const s_view *t1,
                 s_image *destination, D3D12_GPU_VIRTUAL_ADDRESS constants, uint32_t width, uint32_t height)
{
    ID3D12GraphicsCommandList *list = recording_list(d, name);
    if (list == 0 || source == 0 || destination == 0)
        return false;
    if (destination->uav == k_none) {
        log_text(d, "d3d12: %s into an image without storage use", name);
        return false;
    }
    s_pso_entry *pass = helper_pso(d, helper, name);
    if (pass == 0)
        return false;
    const uint32_t base = frame_descriptors(d, 3);
    if (base == k_none)
        return false;
    copy_view(d, base, t0 != 0 && t0->descriptor != k_none ? t0->descriptor : d->null_srv_2d);
    copy_view(d, base + 1u, t1 != 0 && t1->descriptor != k_none ? t1->descriptor : d->null_srv_2d);
    copy_view(d, base + 2u, destination->uav);
    if (d->rendering)
        end_rendering(d);
    image_state(d, source, k_state_shader_read);
    image_state(d, destination, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    flush_barriers(d, list);
    s_bind_state &b = d->bind[k_gpu_bind_compute];
    if (b.root != d->helper_layout.root) {
        list->lpVtbl->SetComputeRootSignature(list, d->helper_layout.root);
        forget_bindings(d, k_gpu_bind_compute);
        b.root = d->helper_layout.root;
    }
    if (d->pso_set != pass->pso) {
        list->lpVtbl->SetPipelineState(list, pass->pso);
        d->pso_set = pass->pso;
    }
    D3D12_GPU_DESCRIPTOR_HANDLE table;
    table.ptr = view_heap_gpu(d, base);
    list->lpVtbl->SetComputeRootDescriptorTable(list, 0, table);
    list->lpVtbl->SetComputeRootConstantBufferView(list, 1, constants != 0 ? constants : d->zero_buffer->address);
    list->lpVtbl->Dispatch(list, (width + 7u) / 8u, (height + 7u) / 8u, 1);
    return true;
}

// The resolve exponent bias pass takes these float formats (Vulkan: rgba16f, and the others without a storage format).
bool scale_format(uint32_t format)
{
    return format == k_gpu_format_r16g16b16a16_sfloat || format == k_gpu_format_r16g16_sfloat ||
           format == k_gpu_format_r32g32_sfloat || format == k_gpu_format_r32g32b32a32_sfloat;
}

// ---- occlusion queries ----

// The open record takes a query of the frame's heap for the current rendering (begin_rendering, occlusion_begin).
void occlusion_resume(s_d3d12 *d, ID3D12GraphicsCommandList *list)
{
    s_frame &frame = d->frames[d->frame_index];
    if (d->occlusion_active == 0 || d->occlusion_running != 0 || frame.occlusion_heap == 0)
        return;
    s_occlusion_record &record = frame.records[d->occlusion_active - 1u];
    if (frame.occlusion_queries >= k_occlusion_queries || record.query_count >= k_occlusion_record_queries)
        return; // the rest of its draws go uncounted
    const uint32_t query = frame.occlusion_queries++;
    record.queries[record.query_count++] = query;
    list->lpVtbl->BeginQuery(list, frame.occlusion_heap, D3D12_QUERY_TYPE_OCCLUSION, query);
    d->occlusion_running = query + 1u;
}

void occlusion_suspend(s_d3d12 *d, ID3D12GraphicsCommandList *list)
{
    if (d->occlusion_running == 0)
        return;
    list->lpVtbl->EndQuery(list, d->frames[d->frame_index].occlusion_heap, D3D12_QUERY_TYPE_OCCLUSION,
                           d->occlusion_running - 1u);
    d->occlusion_running = 0;
}

int occlusion_begin(s_d3d12 *d, uint32_t *out_serial, uint32_t *out_record)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_occlusion_begin");
    s_frame &frame = d->frames[d->frame_index];
    if (list == 0 || frame.occlusion_heap == 0 || frame.occlusion_records >= k_occlusion_records)
        return 0;
    occlusion_suspend(d, list); // the open record ends
    const uint32_t index = frame.occlusion_records++;
    frame.records[index].query_count = 0;
    d->occlusion_active = index + 1u;
    if (d->rendering)
        occlusion_resume(d, list);
    *out_serial = frame.serial;
    *out_record = index;
    return 1;
}

void occlusion_end(s_d3d12 *d)
{
    if (d->occlusion_active == 0)
        return;
    if (ID3D12GraphicsCommandList *list = recording_list(d, "gpu_occlusion_end"))
        occlusion_suspend(d, list);
    d->occlusion_active = 0;
}

int occlusion_result(s_d3d12 *d, uint32_t serial, uint32_t record, uint64_t *out_samples)
{
    *out_samples = 0;
    for (uint32_t i = 0; i < d->frame_count; i++) {
        const s_frame &frame = d->frames[i];
        if (frame.results_serial == serial && frame.serial != serial)
            return record < frame.result_count ? (*out_samples = frame.results[record], 1) : 1;
        if (frame.serial != serial)
            continue;
        if (d->recording && i == d->frame_index)
            return 0; // still being recorded
        if (d->frame_fence->lpVtbl->GetCompletedValue(d->frame_fence) < serial)
            return 0; // submitted, not finished
        if (record < frame.occlusion_records)
            *out_samples = occlusion_sum(frame, frame.records[record]);
        return 1;
    }
    return 1; // older than every frame slot: nothing to report
}

// ---- presentation ----

void swapchain_release_buffers(s_d3d12 *d)
{
    for (uint32_t i = 0; i < d->back_buffer_count; i++)
        release(d->back_buffers[i]);
    d->back_buffer_count = 0;
}

void swapchain_destroy(s_d3d12 *d)
{
    if (d->swapchain == 0)
        return;
    queue_idle(d);
    swapchain_release_buffers(d);
    release(d->swapchain);
    d->swapchain_width = d->swapchain_height = 0;
}

void surface_destroy(s_d3d12 *d)
{
    swapchain_destroy(d);
    d->window = 0;
}

gpu_result surface_create(s_d3d12 *d, uint32_t window_system, uintptr_t window)
{
    if (window_system != k_gpu_window_system_win32 || window == 0) {
        log_text(d, "d3d12: presents to Win32 windows only (window system %u)", window_system);
        return k_gpu_unsupported;
    }
    surface_destroy(d);
    d->window = (HWND)window;
    // the port's window handles fullscreen itself (borderless): no DXGI Alt+Enter mode switch
    d->factory->lpVtbl->MakeWindowAssociation(d->factory, d->window, DXGI_MWA_NO_ALT_ENTER);
    return k_gpu_ok;
}

// The window's client area; false when it is not known (or empty: minimized).
bool client_size(s_d3d12 *d, uint32_t *out_width, uint32_t *out_height)
{
    RECT rect = {};
    if (d->window == 0 || d->get_client_rect == 0 || !d->get_client_rect(d->window, &rect))
        return false;
    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return false;
    *out_width = (uint32_t)(rect.right - rect.left);
    *out_height = (uint32_t)(rect.bottom - rect.top);
    return true;
}

const DXGI_FORMAT k_back_buffer_format = DXGI_FORMAT_B8G8R8A8_UNORM;

gpu_result swapchain_create(s_d3d12 *d, uint32_t width, uint32_t height, uint32_t vsync)
{
    if (d->window == 0) {
        log_text(d, "d3d12: gpu_swapchain_create without a surface");
        return k_gpu_surface_lost;
    }
    // the window's size (the swapchain follows it: swapchain_acquire reports a change), else the one asked for
    client_size(d, &width, &height);
    if (width == 0 || height == 0)
        width = height = 8;
    queue_idle(d); // the back buffers may still be in use
    swapchain_release_buffers(d);
    d->vsync = vsync;
    const uint32_t count = d->frame_count + 1u < 2u ? 2u : d->frame_count + 1u > k_back_buffers_max ? k_back_buffers_max
                                                                                         : d->frame_count + 1u;
    HRESULT result;
    if (d->swapchain != 0) {
        result = d->swapchain->lpVtbl->ResizeBuffers(d->swapchain, count, width, height, k_back_buffer_format,
                                                     d->swapchain_flags);
    } else {
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.Format = k_back_buffer_format;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = count;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        desc.Flags = d->tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
        d->swapchain_flags = desc.Flags;
        IDXGISwapChain3 *first = 0;
        result = d->factory->lpVtbl->CreateSwapChainForHwnd(d->factory, (IUnknown *)d->queue, d->window, &desc, 0, 0,
                                                             &first);
        if (SUCCEEDED(result)) {
            // CreateSwapChainForHwnd gives an IDXGISwapChain1: the IDXGISwapChain3 of it
            result = first->lpVtbl->QueryInterface(first, IID_IDXGISwapChain3, (void **)&d->swapchain);
            first->lpVtbl->Release(first);
        }
    }
    if (FAILED(result)) {
        log_text(d, "d3d12: the swapchain (%ux%u) could not be made: %s %08x", width, height, hresult_text(result),
                 (uint32_t)result);
        release(d->swapchain);
        return result_of(result);
    }
    for (uint32_t i = 0; i < count; i++) {
        result = d->swapchain->lpVtbl->GetBuffer(d->swapchain, i, IID_ID3D12Resource, (void **)&d->back_buffers[i]);
        if (FAILED(result)) {
            log_text(d, "d3d12: swapchain buffer %u: %s %08x", i, hresult_text(result), (uint32_t)result);
            swapchain_release_buffers(d);
            return result_of(result);
        }
        d->back_buffer_states[i] = D3D12_RESOURCE_STATE_PRESENT;
        d->back_buffer_count = i + 1u;
    }
    d->swapchain_width = width;
    d->swapchain_height = height;
    return k_gpu_ok;
}

gpu_result swapchain_acquire(s_d3d12 *d, uint32_t *out_image_index)
{
    if (!d->recording || d->swapchain == 0) {
        log_text(d, "d3d12: gpu_swapchain_acquire needs a swapchain and a frame being recorded");
        return k_gpu_error;
    }
    s_frame &frame = d->frames[d->frame_index];
    if (frame.acquired_image != k_none) {
        if (out_image_index != 0)
            *out_image_index = frame.acquired_image;
        return k_gpu_ok;
    }
    // a resized window: the swapchain again (a minimized one keeps it)
    uint32_t width, height;
    if (client_size(d, &width, &height) && (width != d->swapchain_width || height != d->swapchain_height))
        return k_gpu_out_of_date;
    const uint32_t index = d->swapchain->lpVtbl->GetCurrentBackBufferIndex(d->swapchain);
    if (index >= d->back_buffer_count)
        return k_gpu_out_of_date;
    frame.acquired_image = index;
    if (out_image_index != 0)
        *out_image_index = index;
    return k_gpu_ok;
}

void blit_to_swapchain(s_d3d12 *d, s_render_target *target)
{
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_blit_to_swapchain");
    if (list == 0 || target == 0)
        return;
    const uint32_t image = d->frames[d->frame_index].acquired_image;
    if (image == k_none) {
        log_text(d, "d3d12: gpu_cmd_blit_to_swapchain before gpu_swapchain_acquire");
        return;
    }
    if (d->rendering)
        end_rendering(d);
    ID3D12Resource *back_buffer = d->back_buffers[image];
    image_state(d, target->color, k_state_shader_read);
    if (d->back_buffer_states[image] != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        barrier_transition(d, back_buffer, d->back_buffer_states[image], D3D12_RESOURCE_STATE_RENDER_TARGET);
        d->back_buffer_states[image] = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    flush_barriers(d, list);
    const uint32_t sw = d->swapchain_width, sh = d->swapchain_height;
    const uint32_t tw = target->width != 0 ? target->width : 1u, th = target->height != 0 ? target->height : 1u;
    // the display's aspect ratio kept in a resized window (vulkan_backend.cpp vulkan_cmd_blit_to_swapchain): 1:1 when
    // the window matches the target, else centred 16:9 (a 9:5 target, Halo 3's 1152x640) or the target's own aspect
    // with black bars
    uint32_t dst_x = 0, dst_y = 0, dst_w = sw, dst_h = sh;
    if (sw != tw || sh != th) {
        const uint64_t aspect_num = ((uint64_t)tw * 5u == (uint64_t)th * 9u) ? 16u : (uint64_t)tw;
        const uint64_t aspect_den = ((uint64_t)tw * 5u == (uint64_t)th * 9u) ? 9u : (uint64_t)th;
        if ((uint64_t)sw * aspect_den > (uint64_t)sh * aspect_num) {
            dst_h = sh;
            dst_w = (uint32_t)(((uint64_t)sh * aspect_num + aspect_den / 2u) / aspect_den);
            dst_w = dst_w == 0 ? 1u : dst_w > sw ? sw : dst_w;
            dst_x = (sw - dst_w) / 2u;
        } else if ((uint64_t)sw * aspect_den < (uint64_t)sh * aspect_num) {
            dst_w = sw;
            dst_h = (uint32_t)(((uint64_t)sw * aspect_den + aspect_num / 2u) / aspect_num);
            dst_h = dst_h == 0 ? 1u : dst_h > sh ? sh : dst_h;
            dst_y = (sh - dst_h) / 2u;
        }
    }
    const D3D12_CPU_DESCRIPTOR_HANDLE rtv = transient_rtv(d, back_buffer, k_back_buffer_format, 0, 0, false);
    list->lpVtbl->OMSetRenderTargets(list, 1, &rtv, 0, 0);
    if (dst_w < sw || dst_h < sh) {
        static const float k_black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        list->lpVtbl->ClearRenderTargetView(list, rtv, k_black, 0, 0);
    }
    const int32_t constants[12] = { 0, 0, (int32_t)tw, (int32_t)th, (int32_t)dst_x, (int32_t)dst_y, (int32_t)dst_w,
                                    (int32_t)dst_h };
    const D3D12_RECT rect = { (int32_t)dst_x, (int32_t)dst_y, (int32_t)(dst_x + dst_w), (int32_t)(dst_y + dst_h) };
    copy_draw(d, list, copy_pass(d, k_copy_present, k_back_buffer_format, 0), &target->color->raw_view, 0, constants,
              rect);
    barrier_transition(d, back_buffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    d->back_buffer_states[image] = D3D12_RESOURCE_STATE_PRESENT;
    flush_barriers(d, list);
}

gpu_result swapchain_present(s_d3d12 *d)
{
    const uint32_t image = d->present_image;
    if (image == k_none || d->swapchain == 0) {
        log_text(d, "d3d12: gpu_swapchain_present without an acquired image in the last frame");
        return k_gpu_error;
    }
    d->present_image = k_none;
    const UINT flags = !d->vsync && d->tearing ? DXGI_PRESENT_ALLOW_TEARING : 0u;
    const HRESULT result = d->swapchain->lpVtbl->Present(d->swapchain, d->vsync ? 1u : 0u, flags);
    if (result == DXGI_STATUS_OCCLUDED)
        return k_gpu_ok;
    if (FAILED(result)) {
        log_text(d, "d3d12: present failed: %s %08x", hresult_text(result), (uint32_t)result);
        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET)
            report_device_removed(d, "presenting");
        return result_of(result);
    }
    return k_gpu_ok;
}

// ---- the operations (gpu_backend.h) ----

s_buffer *buffer_of(s_gpu_buffer *buffer) { return (s_buffer *)buffer; }
const s_buffer *buffer_of(const s_gpu_buffer *buffer) { return (const s_buffer *)buffer; }
s_image *image_of(s_gpu_image *image) { return (s_image *)image; }
const s_image *image_of(const s_gpu_image *image) { return (const s_image *)image; }
s_render_target *target_of(s_gpu_render_target *target) { return (s_render_target *)target; }
const s_render_target *target_of(const s_gpu_render_target *target) { return (const s_render_target *)target; }
const s_pipeline *pipeline_of(const s_gpu_pipeline *pipeline) { return (const s_pipeline *)pipeline; }

void backend_destroy(s_d3d12 *d);

void op_destroy(s_gpu_backend *gpu) { backend_destroy(self_of(gpu)); }
gpu_result op_wait_idle(s_gpu_backend *gpu) { return queue_idle(self_of(gpu)) ? k_gpu_ok : k_gpu_device_lost; }

gpu_result op_get_pipeline_cache(s_gpu_backend *gpu, void *data, size_t *size)
{
    s_d3d12 *d = self_of(gpu);
    if (d->library == 0) {
        *size = 0;
        return k_gpu_ok;
    }
    lock(d);
    const size_t needed = d->library->lpVtbl->GetSerializedSize(d->library);
    gpu_result result = k_gpu_ok;
    if (data == 0) {
        *size = needed;
    } else if (*size < needed) {
        result = k_gpu_incomplete; // a library serializes whole only
        *size = 0;
    } else {
        const HRESULT serialized = d->library->lpVtbl->Serialize(d->library, data, needed);
        if (FAILED(serialized)) {
            log_text(d, "d3d12: the pipeline library could not be serialized: %s %08x", hresult_text(serialized),
                     (uint32_t)serialized);
            result = result_of(serialized);
            *size = 0;
        } else {
            *size = needed;
        }
    }
    unlock(d);
    return result;
}

uint32_t op_pipelines_created(const s_gpu_backend *gpu) { return self_of(gpu)->pipelines_created; }
uint32_t op_frame_index(const s_gpu_backend *gpu) { return self_of(gpu)->frame_index; }
uint32_t op_view_epoch(const s_gpu_backend *gpu) { return self_of(gpu)->view_epoch; }
void op_get_stats(const s_gpu_backend *gpu, s_gpu_stats *out) { *out = self_of(gpu)->stats; }

int op_rendering(const s_gpu_backend *gpu, uint32_t *out_width, uint32_t *out_height)
{
    const s_d3d12 *d = self_of(gpu);
    if (out_width != 0)
        *out_width = d->rendering_width;
    if (out_height != 0)
        *out_height = d->rendering_height;
    return d->rendering != 0;
}

int op_occlusion_active(const s_gpu_backend *gpu) { return self_of(gpu)->occlusion_active != 0; }

void op_set_checkpoint_describe(s_gpu_backend *gpu, gpu_checkpoint_describe_function describe, void *context)
{
    s_d3d12 *d = self_of(gpu);
    d->describe = describe;
    d->describe_context = context;
}

uint32_t op_format_storage(s_gpu_backend *gpu, uint32_t format)
{
    return format_has_support(self_of(gpu), format, D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) ? 1u : 0u;
}

uint32_t op_format_blendable_filterable(s_gpu_backend *gpu, uint32_t format)
{
    return format_has_support(self_of(gpu), format, D3D12_FORMAT_SUPPORT1_BLENDABLE | D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE)
               ? 1u : 0u;
}

// buffers

s_gpu_buffer *op_buffer_create(s_gpu_backend *gpu, uint32_t size, uint32_t usage, uint32_t memory_usage)
{
    if (memory_usage > k_gpu_memory_readback)
        return 0;
    return (s_gpu_buffer *)buffer_make(self_of(gpu), size, usage, memory_usage);
}

void op_buffer_destroy(s_gpu_backend *gpu, s_gpu_buffer *buffer) { buffer_release(self_of(gpu), buffer_of(buffer)); }

void op_buffer_retire(s_gpu_backend *gpu, s_gpu_buffer *buffer)
{
    if (buffer != 0)
        retire(self_of(gpu), k_retired_buffer, buffer_of(buffer));
}

void *op_buffer_mapped(const s_gpu_backend *, const s_gpu_buffer *buffer) { return buffer_of(buffer)->mapped; }

// Readback heaps are coherent once the fence a frame signals has been waited for: nothing to invalidate.
void op_buffer_invalidate(s_gpu_backend *, s_gpu_buffer *) {}

gpu_result op_buffer_download(s_gpu_backend *gpu, s_gpu_buffer *handle, uint32_t offset, void *data, uint32_t size)
{
    s_d3d12 *d = self_of(gpu);
    s_buffer *buffer = buffer_of(handle);
    if (offset > buffer->size || size > buffer->size - offset) {
        log_text(d, "d3d12: buffer download of %u bytes at %u overruns the %u-byte buffer", size, offset, buffer->size);
        return k_gpu_error;
    }
    if (size == 0)
        return k_gpu_ok;
    if (buffer->mapped != 0) {
        memcpy(data, buffer->mapped + offset, size);
        return k_gpu_ok;
    }
    s_buffer *staging = buffer_make(d, size, k_gpu_buffer_transfer_dst, k_gpu_memory_readback);
    ID3D12GraphicsCommandList *list = staging != 0 ? immediate_begin(d) : 0;
    if (list == 0) {
        buffer_release(d, staging);
        return k_gpu_out_of_memory;
    }
    const uint32_t mark = d->barriers.count;
    buffer_state(d, buffer, k_list_immediate, D3D12_RESOURCE_STATE_COPY_SOURCE);
    flush_barriers(d, list, mark);
    list->lpVtbl->CopyBufferRegion(list, staging->resource, 0, buffer->resource, offset, size);
    const gpu_result result = immediate_submit(d);
    if (result == k_gpu_ok)
        memcpy(data, staging->mapped, size);
    buffer_release(d, staging);
    return result;
}

gpu_result op_buffer_upload(s_gpu_backend *gpu, s_gpu_buffer *handle, uint32_t offset, const void *data, uint32_t size)
{
    s_d3d12 *d = self_of(gpu);
    s_buffer *buffer = buffer_of(handle);
    if (offset > buffer->size || size > buffer->size - offset) {
        log_text(d, "d3d12: buffer upload of %u bytes at %u overruns the %u-byte buffer", size, offset, buffer->size);
        return k_gpu_error;
    }
    if (size == 0)
        return k_gpu_ok;
    if (buffer->memory == k_gpu_memory_upload) {
        memcpy(buffer->mapped + offset, data, size);
        return k_gpu_ok;
    }
    if (buffer->memory != k_gpu_memory_gpu) {
        log_text(d, "d3d12: buffer upload into a readback buffer");
        return k_gpu_error;
    }
    // a buffer's first upload: no command can have read it yet, so it may run ahead of the frame (the upload list),
    // its bytes staged in the frame's staging ring when they fit
    // so may one whose first upload went there this frame (its uploads stay in order: a one-off list would run first)
    const bool pending = buffer->upload_serial == d->frame_serial && d->recording;
    ID3D12GraphicsCommandList *deferred = buffer->uploads++ == 0 || pending ? upload_begin(d) : 0;
    uint32_t ring_offset = 0;
    s_buffer *ring = deferred != 0 ? staging_ring_alloc(d, size, 4u, &ring_offset) : 0;
    s_buffer *staging = ring != 0 ? ring : buffer_make(d, size, k_gpu_buffer_transfer_src, k_gpu_memory_upload);
    if (staging == 0)
        return k_gpu_out_of_memory;
    memcpy(staging->mapped + ring_offset, data, size);
    ID3D12GraphicsCommandList *list = deferred != 0 ? deferred : immediate_begin(d);
    if (list == 0) {
        if (ring == 0)
            buffer_release(d, staging);
        return k_gpu_error;
    }
    const uint32_t mark = d->barriers.count;
    buffer_state(d, buffer, deferred != 0 ? k_list_upload : k_list_immediate, D3D12_RESOURCE_STATE_COPY_DEST);
    flush_barriers(d, list, mark);
    list->lpVtbl->CopyBufferRegion(list, buffer->resource, offset, staging->resource, ring_offset, size);
    if (deferred != 0) {
        buffer->upload_serial = d->frame_serial;
        if (ring == 0)
            retire(d, k_retired_buffer, staging);
        return k_gpu_ok;
    }
    const gpu_result result = immediate_submit(d);
    buffer_release(d, staging);
    return result;
}

void op_buffer_device_address(const s_gpu_backend *, const s_gpu_buffer *, uint32_t *out_lo, uint32_t *out_hi)
{
    *out_lo = *out_hi = 0; // caps.buffer_device_address = 0: the memory tables instead
}

// images, samplers

s_gpu_image *op_image_create(s_gpu_backend *gpu, const s_gpu_image_desc *desc)
{
    return (s_gpu_image *)image_make(self_of(gpu), desc);
}

void op_image_destroy(s_gpu_backend *gpu, s_gpu_image *image)
{
    if (image == 0)
        return;
    s_d3d12 *d = self_of(gpu);
    d->view_epoch++;
    image_release(d, image_of(image));
}

void op_image_retire(s_gpu_backend *gpu, s_gpu_image *image)
{
    if (image == 0)
        return;
    s_d3d12 *d = self_of(gpu);
    d->view_epoch++; // texture groups holding its views must not be bound again (gpu_bind_groups_reset_cached)
    retire(d, k_retired_image, image_of(image));
}

void op_image_get_desc(const s_gpu_backend *, const s_gpu_image *image, s_gpu_image_desc *out)
{
    *out = image_of(image)->desc;
}

gpu_view op_image_view(const s_gpu_backend *, const s_gpu_image *image)
{
    if (image == 0)
        return GPU_NULL_HANDLE;
    return handle_of(&image_of(image)->view);
}

gpu_view op_image_sampled_view(const s_gpu_backend *, const s_gpu_image *handle)
{
    if (handle == 0)
        return GPU_NULL_HANDLE;
    const s_image *image = image_of(handle);
    return handle_of(image->sampled_view.descriptor != k_none ? &image->sampled_view : &image->view);
}

gpu_view op_image_array_view(s_gpu_backend *gpu, s_gpu_image *handle)
{
    s_d3d12 *d = self_of(gpu);
    s_image *image = image_of(handle);
    if (image == 0)
        return GPU_NULL_HANDLE;
    if (image->desc.view_type == k_gpu_view_2d_array)
        return op_image_sampled_view(gpu, handle);
    if (image->desc.view_type != k_gpu_view_2d)
        return GPU_NULL_HANDLE;
    if (image->array_view.descriptor == k_none) {
        const s_format &format = format_of(image->internal_format);
        const bool depth = (image->aspect & k_aspect_depth) != 0;
        if (!make_view(d, image, &image->array_view, k_gpu_view_2d_array, depth ? format.depth : format.format,
                       depth ? d3d12_shader_4_component_mapping(0, 0, 0, 0) : compose_mapping(format, image->desc.swizzle),
                       0))
            return GPU_NULL_HANDLE;
    }
    return handle_of(&image->array_view);
}

gpu_view op_image_swizzled_view(s_gpu_backend *gpu, s_gpu_image *handle, uint32_t swizzle)
{
    s_d3d12 *d = self_of(gpu);
    s_image *image = image_of(handle);
    if (image == 0 || image->aspect != k_aspect_color)
        return GPU_NULL_HANDLE;
    if (swizzle == image->desc.swizzle)
        return handle_of(&image->view);
    if (image->swizzled_view.descriptor != k_none) {
        if (image->swizzled_swizzle == swizzle)
            return handle_of(&image->swizzled_view);
        // one other swizzle for the image's life (draws recorded with a view may still use it)
        log_text(d, "d3d12: gpu_image_swizzled_view: a second swizzle %08x of an image viewed as %08x", swizzle,
                 image->swizzled_swizzle);
        return GPU_NULL_HANDLE;
    }
    const s_format &format = format_of(image->internal_format);
    if (!make_view(d, image, &image->swizzled_view, image->desc.view_type, format.format, compose_mapping(format, swizzle),
                   0))
        return GPU_NULL_HANDLE;
    image->swizzled_swizzle = swizzle;
    return handle_of(&image->swizzled_view);
}

gpu_result op_image_upload(s_gpu_backend *gpu, s_gpu_image *image, const s_gpu_image_region *regions,
                           uint32_t region_count, const void *data, uint32_t size)
{
    if (image == 0 || regions == 0 || region_count == 0)
        return k_gpu_error;
    return image_upload(self_of(gpu), image_of(image), regions, region_count, data, size);
}

gpu_sampler op_sampler_create(s_gpu_backend *gpu, const s_gpu_sampler_desc *desc)
{
    s_d3d12 *d = self_of(gpu);
    s_sampler *sampler = (s_sampler *)calloc(1, sizeof(s_sampler));
    if (sampler == 0)
        return GPU_NULL_HANDLE;
    sampler->descriptor = sampler_make(d, desc);
    if (sampler->descriptor == k_none) {
        free(sampler);
        return GPU_NULL_HANDLE;
    }
    sampler->id = ++d->sampler_ids;
    return handle_of(sampler);
}

void op_sampler_destroy(s_gpu_backend *gpu, gpu_sampler handle)
{
    s_sampler *sampler = pointer_of<s_sampler>(handle);
    if (sampler == 0)
        return;
    s_d3d12 *d = self_of(gpu);
    d->view_epoch++;
    // its descriptor stays until the frames that may place it (a group bound later this frame) are done
    retire(d, k_retired_sampler, 0, sampler->descriptor);
    free(sampler);
}

// shader pack modules and pipelines

gpu_shader op_pack_shader_module(s_gpu_backend *gpu, s_shader_pack *, uint32_t, const char **out_entry_point)
{
    s_d3d12 *d = self_of(gpu);
    if (out_entry_point != 0)
        *out_entry_point = 0;
    if (report_once(d, k_report_v2))
        log_text(d, "d3d12: version 2 shader pack modules are SPIR-V (the Direct3D 12 renderer takes the version 4 "
                    "DXIL pack)");
    return GPU_NULL_HANDLE;
}

gpu_shader op_pack_xenos_module(s_gpu_backend *gpu, s_shader_pack *pack, uint32_t module_index)
{
    s_d3d12 *d = self_of(gpu);
    if (shader_pack_module_format(pack) != SHADER_PACK_MODULE_FORMAT_DXIL) {
        if (report_once(d, k_report_v2))
            log_text(d, "d3d12: the shader pack's modules are not DXIL");
        return GPU_NULL_HANDLE;
    }
    const uint32_t count = shader_pack_module_count(pack);
    if (module_index >= count)
        return GPU_NULL_HANDLE;
    if (d->modules.count < count) {
        while (d->modules.count < count)
            if (d->modules.push() == 0)
                return GPU_NULL_HANDLE;
    }
    s_shader *&shader = d->modules.items[module_index];
    if (shader == 0) {
        size_t size = 0;
        const uint32_t *code = shader_pack_module_code(pack, module_index, &size);
        if (code == 0 || size == 0) {
            log_text(d, "d3d12: shader pack module %u did not load: %s", module_index, shader_pack_last_error());
            return GPU_NULL_HANDLE;
        }
        shader = shader_wrap(code, size);
    }
    return handle_of(shader);
}

void op_pack_release(s_gpu_backend *gpu)
{
    s_d3d12 *d = self_of(gpu);
    for (uint32_t i = 0; i < d->modules.count; i++)
        free(d->modules.items[i]);
    d->modules.release();
}

s_gpu_pipeline *op_pack_graphics_pipeline(s_gpu_backend *gpu, s_shader_pack *pack, uint32_t, uint32_t,
                                          const s_xenos_vertex_declaration *, const s_gpu_graphics_state *)
{
    op_pack_shader_module(gpu, pack, 0, 0);
    return 0;
}

s_gpu_pipeline *op_pack_compute_pipeline(s_gpu_backend *gpu, s_shader_pack *pack, uint32_t)
{
    op_pack_shader_module(gpu, pack, 0, 0);
    return 0;
}

// The universal Xenos layout: the pack's root signature (its root signature module), bind group 0 its six root CBVs,
// bind group 1 its texture and sampler tables.
int op_xenos_layout(s_gpu_backend *gpu, s_shader_pack *pack, s_gpu_xenos_layout *out)
{
    s_d3d12 *d = self_of(gpu);
    if (!d->xenos_made) {
        const uint32_t module = shader_pack_xenos_root_signature(pack);
        size_t size = 0;
        const uint32_t *code = module != SHADER_PACK_NOT_FOUND ? shader_pack_module_code(pack, module, &size) : 0;
        if (code == 0) {
            log_text(d, "d3d12: the shader pack has no root signature module (a version 4 pack carries one)");
            return 0;
        }
        d->xenos_layout.root = root_signature_make(d, code, size, "Xenos");
        if (d->xenos_layout.root == 0)
            return 0;
        d->xenos_layout.kind = k_layout_xenos;
        d->bind_layouts[k_bind_constants].kind = k_bind_constants;
        d->bind_layouts[k_bind_textures].kind = k_bind_textures;
        d->xenos_made = 1;
    }
    out->layout = handle_of(&d->xenos_layout);
    out->constants = handle_of(&d->bind_layouts[k_bind_constants]);
    out->textures = handle_of(&d->bind_layouts[k_bind_textures]);
    return 1;
}

s_gpu_pipeline *op_graphics_pipeline_create_stages(s_gpu_backend *gpu, const s_gpu_shader_stage *stages,
                                                   uint32_t stage_count, gpu_pipeline_layout layout,
                                                   const s_gpu_graphics_state *state, uint32_t patch_control_points)
{
    return (s_gpu_pipeline *)graphics_pipeline(self_of(gpu), stages, stage_count,
                                               pointer_of<s_pipeline_layout>(layout), *state, patch_control_points);
}

gpu_pipeline_handle op_compute_pipeline_create_handle(s_gpu_backend *gpu, gpu_shader module, const char *,
                                                      gpu_pipeline_layout layout)
{
    return handle_of(compute_pso(self_of(gpu), pointer_of<s_shader>(module), pointer_of<s_pipeline_layout>(layout)));
}

s_gpu_pipeline *op_pipeline_adopt(s_gpu_backend *gpu, gpu_pipeline_handle handle, gpu_pipeline_layout layout,
                                  uint32_t bind_point)
{
    s_pso_entry *entry = pointer_of<s_pso_entry>(handle);
    if (entry == 0)
        return 0;
    return (s_gpu_pipeline *)pipeline_wrap(self_of(gpu), entry, pointer_of<s_pipeline_layout>(layout), bind_point);
}

void op_pipeline_destroy(s_gpu_backend *gpu, s_gpu_pipeline *handle)
{
    s_pipeline *pipeline = (s_pipeline *)handle;
    if (pipeline == 0)
        return;
    s_d3d12 *d = self_of(gpu);
    for (s_bind_state &b : d->bind)
        if (b.pipeline == pipeline)
            b.pipeline = 0;
    pso_put(d, pipeline->entry);
    free(pipeline);
}

gpu_bind_layout op_pipeline_bind_layout(const s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, uint32_t set)
{
    const s_d3d12 *d = self_of(gpu);
    if (pipeline == 0 || pipeline_of(pipeline)->layout != &d->xenos_layout || set > XE_SET_TEXTURES)
        return GPU_NULL_HANDLE;
    return handle_of(&d->bind_layouts[set]);
}

uint32_t op_pipeline_vertex_streams(const s_gpu_backend *, const s_gpu_pipeline *) { return 0; }

// Pipeline library parts: Direct3D 12 has none (caps.pipeline_library = 0), so the frontend never asks for them.
gpu_pipeline_handle library_unsupported(s_d3d12 *d)
{
    if (report_once(d, k_report_library))
        log_text(d, "d3d12: pipeline library parts are not supported (caps.pipeline_library = 0)");
    return GPU_NULL_HANDLE;
}

gpu_pipeline_handle op_pipeline_library_vertex_input(s_gpu_backend *gpu, const s_gpu_graphics_state *, uint32_t)
{
    return library_unsupported(self_of(gpu));
}

gpu_pipeline_handle op_pipeline_library_pre_rasterization(s_gpu_backend *gpu, const s_gpu_shader_stage *, uint32_t,
                                                          gpu_pipeline_layout, const s_gpu_graphics_state *, uint32_t)
{
    return library_unsupported(self_of(gpu));
}

gpu_pipeline_handle op_pipeline_library_fragment_shader(s_gpu_backend *gpu, gpu_shader, gpu_pipeline_layout, uint32_t)
{
    return library_unsupported(self_of(gpu));
}

gpu_pipeline_handle op_pipeline_library_fragment_output(s_gpu_backend *gpu, const s_gpu_graphics_state *)
{
    return library_unsupported(self_of(gpu));
}

// A bare handle (compute_pipeline_create_handle) never adopted: its reference.
void op_pipeline_library_destroy(s_gpu_backend *gpu, gpu_pipeline_handle handle)
{
    pso_put(self_of(gpu), pointer_of<s_pso_entry>(handle));
}

s_gpu_pipeline *op_pipeline_link(s_gpu_backend *gpu, const gpu_pipeline_handle *, gpu_pipeline_layout,
                                 const s_gpu_graphics_state *, uint32_t)
{
    library_unsupported(self_of(gpu));
    return 0;
}

gpu_pipeline_handle op_pipeline_link_handle(s_gpu_backend *gpu, const gpu_pipeline_handle *, gpu_pipeline_layout,
                                            uint32_t)
{
    return library_unsupported(self_of(gpu));
}

void op_pipeline_replace(s_gpu_backend *gpu, s_gpu_pipeline *, gpu_pipeline_handle handle)
{
    library_unsupported(self_of(gpu));
    pso_put(self_of(gpu), pointer_of<s_pso_entry>(handle));
}

// Without pipeline libraries a pipeline's whole state is its key.
void op_graphics_state_static_key(const s_gpu_backend *, const s_gpu_graphics_state *state, s_gpu_graphics_state *out)
{
    *out = *state;
}

// bind groups

gpu_bind_group op_bind_group_allocate(s_gpu_backend *gpu, gpu_bind_layout layout, uint32_t lifetime)
{
    const s_bind_layout *bind_layout = pointer_of<s_bind_layout>(layout);
    if (bind_layout == 0)
        return GPU_NULL_HANDLE;
    return handle_of(group_make(self_of(gpu), bind_layout, lifetime));
}

void op_bind_group_free(s_gpu_backend *gpu, gpu_bind_group handle)
{
    s_group *group = pointer_of<s_group>(handle);
    if (group == 0)
        return;
    s_d3d12 *d = self_of(gpu);
    if (group->lifetime == k_gpu_bind_frame || group->lifetime == k_gpu_bind_frame_textures)
        return; // its frame slot recycles it
    if (group->lifetime == k_gpu_bind_cached_textures) {
        for (uint32_t i = 0; i < d->cached_groups.count; i++) {
            if (d->cached_groups.items[i] == group) {
                d->cached_groups.items[i] = d->cached_groups.items[--d->cached_groups.count];
                break;
            }
        }
    }
    for (s_bind_state &b : d->bind)
        if (b.textures == group)
            b.textures = 0;
    retire(d, k_retired_group, group);
}

void op_bind_groups_reset_cached(s_gpu_backend *gpu)
{
    s_d3d12 *d = self_of(gpu);
    for (uint32_t i = 0; i < d->cached_groups.count; i++) {
        for (s_bind_state &b : d->bind)
            if (b.textures == d->cached_groups.items[i])
                b.textures = 0;
        retire(d, k_retired_group, d->cached_groups.items[i]);
    }
    d->cached_groups.count = 0;
}

void op_bind_write_buffer(s_gpu_backend *gpu, gpu_bind_group handle, uint32_t binding, uint32_t array_element,
                          uint32_t type, s_gpu_buffer *buffer, uint32_t offset, uint32_t)
{
    s_d3d12 *d = self_of(gpu);
    s_group *group = pointer_of<s_group>(handle);
    if (group == 0)
        return;
    if (group->kind != k_bind_constants || binding > XE_BINDING_VERTEX_FETCH || array_element != 0 ||
        (type != k_gpu_binding_uniform_buffer && type != k_gpu_binding_uniform_buffer_dynamic)) {
        if (report_once(d, k_report_layout))
            log_text(d, "d3d12: a buffer binding (%u, type %u) outside the Xenos layout's constant blocks", binding, type);
        return;
    }
    group->buffers[binding] = buffer_of(buffer);
    group->offsets[binding] = offset;
}

void op_bind_write_image(s_gpu_backend *gpu, gpu_bind_group handle, uint32_t binding, uint32_t array_element,
                         uint32_t type, gpu_view view)
{
    s_d3d12 *d = self_of(gpu);
    s_group *group = pointer_of<s_group>(handle);
    if (group == 0)
        return;
    if (type != k_gpu_binding_sampled_image) {
        if (report_once(d, k_report_layout))
            log_text(d, "d3d12: an image binding of type %u outside the Xenos layout", type);
        return;
    }
    group_write_image(d, group, binding + array_element, pointer_of<s_view>(view));
}

void op_bind_write_sampler(s_gpu_backend *gpu, gpu_bind_group handle, uint32_t binding, uint32_t array_element,
                           gpu_sampler sampler)
{
    s_group *group = pointer_of<s_group>(handle);
    if (group != 0)
        group_write_sampler(self_of(gpu), group, binding + array_element, pointer_of<s_sampler>(sampler));
}

void op_bind_write_textures(s_gpu_backend *gpu, gpu_bind_group handle, uint32_t count, const uint32_t *binding,
                            const uint64_t *handles, uint32_t first_sampler)
{
    s_d3d12 *d = self_of(gpu);
    s_group *group = pointer_of<s_group>(handle);
    if (group == 0)
        return;
    for (uint32_t i = 0; i < count; i++) {
        if (binding[i] < first_sampler)
            group_write_image(d, group, binding[i], pointer_of<s_view>(handles[i]));
        else
            group_write_sampler(d, group, binding[i], pointer_of<s_sampler>(handles[i]));
    }
}

// frames

gpu_result op_frame_begin(s_gpu_backend *gpu) { return frame_begin(self_of(gpu)); }
gpu_result op_frame_end(s_gpu_backend *gpu, uint32_t wait) { return frame_end(self_of(gpu), wait); }
gpu_result op_frame_wait(s_gpu_backend *gpu, uint32_t slot) { return frame_wait(self_of(gpu), slot); }

// offscreen render targets

s_gpu_render_target *op_render_target_create(s_gpu_backend *gpu, uint32_t width, uint32_t height, uint32_t color_format,
                                             uint32_t flags)
{
    return (s_gpu_render_target *)render_target_make(self_of(gpu), width, height, color_format, flags);
}

void op_render_target_destroy(s_gpu_backend *gpu, s_gpu_render_target *target)
{
    s_d3d12 *d = self_of(gpu);
    d->view_epoch++;
    render_target_destroy(d, target_of(target));
}

void op_render_target_get_info(const s_gpu_backend *, const s_gpu_render_target *handle, s_gpu_render_target_info *out)
{
    const s_render_target *target = target_of(handle);
    out->width = target->width;
    out->height = target->height;
    out->color_format = target->color->desc.format;
    out->depth_format = target->depth != 0 ? target->depth->desc.format : (uint32_t)k_gpu_format_undefined;
    out->readback_row_pitch = target->row_pitch;
}

s_gpu_image *op_render_target_color_image(s_gpu_backend *, s_gpu_render_target *target)
{
    return target != 0 ? (s_gpu_image *)target_of(target)->color : 0;
}

const void *op_render_target_read(s_gpu_backend *, s_gpu_render_target *target)
{
    return target_of(target)->readback->mapped;
}

// commands

void op_cmd_begin_rendering(s_gpu_backend *gpu, s_gpu_image *const *colors, uint32_t color_count, s_gpu_image *depth,
                            const s_gpu_clear *clear)
{
    begin_rendering(self_of(gpu), colors, color_count, depth, clear);
}

void op_cmd_end_rendering(s_gpu_backend *gpu) { end_rendering(self_of(gpu)); }

void op_cmd_clear_attachments(s_gpu_backend *gpu, const s_gpu_clear *clear, const int32_t *rects, uint32_t rect_count)
{
    clear_attachments(self_of(gpu), clear, rects, rect_count);
}

void op_cmd_copy_image(s_gpu_backend *gpu, s_gpu_image *source, int32_t source_x, int32_t source_y, uint32_t width,
                       uint32_t height, s_gpu_image *destination, uint32_t destination_level,
                       uint32_t destination_layer, int32_t destination_x, int32_t destination_y,
                       uint32_t destination_width, uint32_t destination_height)
{
    copy_image(self_of(gpu), image_of(source), source_x, source_y, width, height, image_of(destination),
               destination_level, destination_layer, destination_x, destination_y, destination_width, destination_height);
}

void op_cmd_copy_depth(s_gpu_backend *gpu, s_gpu_image *source, int32_t source_x, int32_t source_y, uint32_t width,
                       uint32_t height, s_gpu_image *destination, int32_t destination_x, int32_t destination_y)
{
    copy_depth(self_of(gpu), image_of(source), source_x, source_y, width, height, image_of(destination), destination_x,
               destination_y);
}

void op_cmd_copy_rects(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, const s_gpu_copy_rect *rects,
                       uint32_t count)
{
    copy_rects(self_of(gpu), image_of(source), image_of(destination), rects, count);
}

void op_cmd_set_viewport(s_gpu_backend *gpu, float x, float y, float width, float height, float min_depth,
                         float max_depth)
{
    s_d3d12 *d = self_of(gpu);
    if (d->viewport_valid && d->viewport.TopLeftX == x && d->viewport.TopLeftY == y && d->viewport.Width == width &&
        d->viewport.Height == height && d->viewport.MinDepth == min_depth && d->viewport.MaxDepth == max_depth)
        return; // what the list has
    set_viewport(d, x, y, width, height, min_depth, max_depth);
}

void op_cmd_set_scissor(s_gpu_backend *gpu, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    s_d3d12 *d = self_of(gpu);
    if (d->scissor_valid && d->scissor.left == x && d->scissor.top == y && d->scissor.right == x + (int32_t)width &&
        d->scissor.bottom == y + (int32_t)height)
        return;
    set_scissor(d, x, y, width, height);
}

void op_cmd_bind_pipeline(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, const s_gpu_graphics_state *state)
{
    bind_pipeline(self_of(gpu), pipeline_of(pipeline), state);
}

void op_cmd_bind_group(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, uint32_t set, gpu_bind_group group,
                       uint32_t offset_count, const uint32_t *offsets)
{
    bind_group(self_of(gpu), pipeline_of(pipeline), set, group, offset_count, offsets);
}

void op_cmd_bind_group_pair(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, gpu_bind_group group0,
                            uint32_t offset_count0, const uint32_t *offsets0, gpu_bind_group group1)
{
    bind_group(self_of(gpu), pipeline_of(pipeline), 0, group0, offset_count0, offsets0);
    bind_group(self_of(gpu), pipeline_of(pipeline), 1, group1, 0, 0);
}

// Version 2 packs' fixed-function vertex input (never on Direct3D 12, whose pack fetches its own vertices).
void op_cmd_bind_vertex_buffer(s_gpu_backend *gpu, uint32_t binding, const s_gpu_buffer *handle, uint32_t offset,
                               uint32_t stride)
{
    s_d3d12 *d = self_of(gpu);
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_bind_vertex_buffer");
    const s_buffer *buffer = buffer_of(handle);
    if (list == 0 || buffer == 0 || binding >= 32u)
        return;
    buffer_state(d, (s_buffer *)buffer, k_list_frame, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    flush_barriers(d, list);
    D3D12_VERTEX_BUFFER_VIEW view;
    view.BufferLocation = buffer->address + offset;
    view.SizeInBytes = buffer->allocated > offset ? buffer->allocated - offset : 0u;
    view.StrideInBytes = stride;
    list->lpVtbl->IASetVertexBuffers(list, binding, 1, &view);
    if (report_once(d, k_report_vertex_buffer))
        log_text(d, "d3d12: fixed-function vertex buffers are bound (a version 2 pack's draws are not drawn here)");
}

void op_cmd_bind_index_buffer(s_gpu_backend *gpu, const s_gpu_buffer *buffer, uint32_t offset, uint32_t index_type)
{
    bind_index_buffer(self_of(gpu), buffer_of(buffer), offset, index_type);
}

void op_cmd_xenos_bind_memory(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, s_gpu_buffer *const *streams,
                              s_gpu_buffer *const *exports)
{
    xenos_bind_memory(self_of(gpu), pipeline_of(pipeline), (s_buffer *const *)streams, (s_buffer *const *)exports);
}

void op_cmd_draw(s_gpu_backend *gpu, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                 uint32_t first_instance)
{
    draw(self_of(gpu), vertex_count, instance_count, first_vertex, first_instance);
}

void op_cmd_draw_indexed(s_gpu_backend *gpu, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                         int32_t vertex_offset, uint32_t first_instance)
{
    draw_indexed(self_of(gpu), index_count, instance_count, first_index, vertex_offset, first_instance);
}

void op_cmd_dispatch(s_gpu_backend *gpu, uint32_t groups_x, uint32_t groups_y, uint32_t groups_z)
{
    dispatch(self_of(gpu), groups_x, groups_y, groups_z);
}

// Between all earlier and later commands: unordered-access writes are ordered by a UAV barrier on every resource; every
// other access is ordered by the state transitions the backend records before it.
void op_cmd_memory_barrier(s_gpu_backend *gpu)
{
    s_d3d12 *d = self_of(gpu);
    ID3D12GraphicsCommandList *list = recording_list(d, "gpu_cmd_memory_barrier");
    if (list == 0)
        return;
    barrier_uav(d, 0);
    flush_barriers(d, list);
}

void op_cmd_copy_buffer(s_gpu_backend *gpu, s_gpu_buffer *source, uint32_t source_offset, s_gpu_buffer *destination,
                        uint32_t destination_offset, uint32_t size)
{
    copy_buffer(self_of(gpu), buffer_of(source), source_offset, buffer_of(destination), destination_offset, size);
}

void op_cmd_copy_buffer_to_image(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, uint32_t row_length,
                                 s_gpu_image *image, uint32_t width, uint32_t height)
{
    copy_buffer_to_image(self_of(gpu), buffer_of(buffer), offset, row_length, image_of(image), width, height);
}

void op_cmd_copy_image_to_buffer(s_gpu_backend *gpu, s_gpu_image *image, uint32_t level, uint32_t layer, int32_t x,
                                 int32_t y, uint32_t width, uint32_t height, s_gpu_buffer *buffer, uint32_t offset)
{
    copy_image_to_buffer(self_of(gpu), image_of(image), level, layer, x, y, width, height, buffer_of(buffer), offset);
}

void op_cmd_readback(s_gpu_backend *gpu, s_gpu_render_target *target) { readback(self_of(gpu), target_of(target)); }

// Device-loss checkpoints are not kept here (caps.checkpoints = 0): GetDeviceRemovedReason and, with
// HALO3_D3D12_DEBUG, the debug layer's messages say what happened.
void op_cmd_checkpoint(s_gpu_backend *, const void *) {}

void op_cmd_reinterpret_7e3(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination)
{
    s_image *to = image_of(destination);
    if (to == 0 || source == 0)
        return;
    helper_pass(self_of(gpu), k_helper_7e3, "gpu_cmd_reinterpret_7e3", image_of(source), &image_of(source)->view, 0, to,
                0, to->desc.width, to->desc.height);
}

uint32_t op_cmd_convert_pwl(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, uint32_t encode)
{
    s_image *to = image_of(destination);
    if (to == 0 || source == 0 || to->uav == k_none)
        return 0;
    return helper_pass(self_of(gpu), encode ? k_helper_pwl_encode : k_helper_pwl_decode, "gpu_cmd_convert_pwl",
                       image_of(source), &image_of(source)->view, 0, to, 0, to->desc.width, to->desc.height)
               ? 1u : 0u;
}

uint32_t op_cmd_downscale_image(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination)
{
    s_image *to = image_of(destination);
    if (to == 0 || source == 0 || to->uav == k_none)
        return 0;
    return helper_pass(self_of(gpu), k_helper_downscale, "gpu_cmd_downscale_image", image_of(source),
                       &image_of(source)->view, 0, to, 0, to->desc.width, to->desc.height)
               ? 1u : 0u;
}

uint32_t op_cmd_pack_depth_stencil(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                   uint32_t float_depth)
{
    s_d3d12 *d = self_of(gpu);
    s_image *from = image_of(source), *to = image_of(destination);
    if (from == 0 || to == 0)
        return 0;
    if ((from->aspect & k_aspect_stencil) == 0 || from->sampled_view.descriptor == k_none ||
        to->desc.format != k_gpu_format_r8g8b8a8_unorm || to->uav == k_none || to->desc.swizzle != 0) {
        log_text(d, "d3d12: gpu_cmd_pack_depth_stencil needs a sampled depth/stencil image and an R8G8B8A8_UNORM "
                    "storage image with the identity view");
        return 0;
    }
    if (!helper_pass(d, float_depth ? k_helper_depth_20e4 : k_helper_depth_unorm24, "gpu_cmd_pack_depth_stencil", from,
                     &from->sampled_view, &from->stencil_view, to, 0, to->desc.width, to->desc.height))
        return 0;
    image_state(d, to, k_state_shader_read);
    flush_barriers(d, frame_list(d));
    return 1;
}

void op_cmd_apply_display_gamma(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, s_gpu_buffer *ramp,
                                uint32_t ramp_offset)
{
    s_d3d12 *d = self_of(gpu);
    s_image *to = image_of(destination);
    if (to == 0 || source == 0 || ramp == 0)
        return;
    if (to->desc.format != k_gpu_format_r8g8b8a8_unorm) {
        log_text(d, "d3d12: gpu_cmd_apply_display_gamma into an image that is not R8G8B8A8_UNORM");
        return;
    }
    helper_pass(d, k_helper_display_gamma, "gpu_cmd_apply_display_gamma", image_of(source), &image_of(source)->view, 0,
                to, buffer_of(ramp)->address + ramp_offset, to->desc.width, to->desc.height);
}

uint32_t op_cmd_scale_image_rect(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                 s_gpu_buffer *parameters, uint32_t parameters_offset, uint32_t width, uint32_t height)
{
    s_image *to = image_of(destination);
    if (source == 0 || to == 0 || parameters == 0 || to->uav == k_none || !scale_format(to->desc.format))
        return 0;
    return helper_pass(self_of(gpu), k_helper_resolve_scale, "gpu_cmd_scale_image_rect", image_of(source),
                       &image_of(source)->view, 0, to, buffer_of(parameters)->address + parameters_offset, width,
                       height)
               ? 1u : 0u;
}

int op_occlusion_begin(s_gpu_backend *gpu, uint32_t *out_serial, uint32_t *out_record)
{
    return occlusion_begin(self_of(gpu), out_serial, out_record);
}

void op_occlusion_end(s_gpu_backend *gpu) { occlusion_end(self_of(gpu)); }

int op_occlusion_result(s_gpu_backend *gpu, uint32_t serial, uint32_t record, uint64_t *out_samples)
{
    return occlusion_result(self_of(gpu), serial, record, out_samples);
}

gpu_result op_surface_create(s_gpu_backend *gpu, uint32_t window_system, void *, uintptr_t window)
{
    return surface_create(self_of(gpu), window_system, window);
}

void op_surface_destroy(s_gpu_backend *gpu) { surface_destroy(self_of(gpu)); }

gpu_result op_swapchain_create(s_gpu_backend *gpu, uint32_t width, uint32_t height, uint32_t vsync)
{
    return swapchain_create(self_of(gpu), width, height, vsync);
}

void op_swapchain_destroy(s_gpu_backend *gpu) { swapchain_destroy(self_of(gpu)); }

gpu_result op_swapchain_acquire(s_gpu_backend *gpu, uint32_t *out_image_index)
{
    return swapchain_acquire(self_of(gpu), out_image_index);
}

void op_cmd_blit_to_swapchain(s_gpu_backend *gpu, s_gpu_render_target *target)
{
    blit_to_swapchain(self_of(gpu), target_of(target));
}

gpu_result op_swapchain_present(s_gpu_backend *gpu) { return swapchain_present(self_of(gpu)); }

const s_gpu_backend_ops k_ops = {
    op_destroy,
    op_wait_idle,
    op_get_pipeline_cache,
    op_pipelines_created,
    op_frame_index,
    op_view_epoch,
    op_get_stats,
    op_rendering,
    op_occlusion_active,
    op_set_checkpoint_describe,
    op_format_storage,
    op_format_blendable_filterable,
    op_buffer_create,
    op_buffer_destroy,
    op_buffer_retire,
    op_buffer_mapped,
    op_buffer_invalidate,
    op_buffer_download,
    op_buffer_upload,
    op_buffer_device_address,
    op_image_create,
    op_image_destroy,
    op_image_retire,
    op_image_get_desc,
    op_image_view,
    op_image_sampled_view,
    op_image_array_view,
    op_image_swizzled_view,
    op_image_upload,
    op_sampler_create,
    op_sampler_destroy,
    op_pack_shader_module,
    op_pack_xenos_module,
    op_pack_release,
    op_pack_graphics_pipeline,
    op_pack_compute_pipeline,
    op_xenos_layout,
    op_graphics_pipeline_create_stages,
    op_compute_pipeline_create_handle,
    op_pipeline_adopt,
    op_pipeline_destroy,
    op_pipeline_bind_layout,
    op_pipeline_vertex_streams,
    op_pipeline_library_vertex_input,
    op_pipeline_library_pre_rasterization,
    op_pipeline_library_fragment_shader,
    op_pipeline_library_fragment_output,
    op_pipeline_library_destroy,
    op_pipeline_link,
    op_pipeline_link_handle,
    op_pipeline_replace,
    op_graphics_state_static_key,
    op_bind_group_allocate,
    op_bind_group_free,
    op_bind_groups_reset_cached,
    op_bind_write_buffer,
    op_bind_write_image,
    op_bind_write_sampler,
    op_bind_write_textures,
    op_frame_begin,
    op_frame_end,
    op_frame_wait,
    op_render_target_create,
    op_render_target_destroy,
    op_render_target_get_info,
    op_render_target_color_image,
    op_render_target_read,
    op_cmd_begin_rendering,
    op_cmd_end_rendering,
    op_cmd_clear_attachments,
    op_cmd_copy_image,
    op_cmd_copy_depth,
    op_cmd_copy_rects,
    op_cmd_set_viewport,
    op_cmd_set_scissor,
    op_cmd_bind_pipeline,
    op_cmd_bind_group,
    op_cmd_bind_group_pair,
    op_cmd_bind_vertex_buffer,
    op_cmd_bind_index_buffer,
    op_cmd_xenos_bind_memory,
    op_cmd_draw,
    op_cmd_draw_indexed,
    op_cmd_dispatch,
    op_cmd_memory_barrier,
    op_cmd_copy_buffer,
    op_cmd_copy_buffer_to_image,
    op_cmd_copy_image_to_buffer,
    op_cmd_readback,
    op_cmd_checkpoint,
    op_cmd_reinterpret_7e3,
    op_cmd_convert_pwl,
    op_cmd_downscale_image,
    op_cmd_pack_depth_stencil,
    op_cmd_apply_display_gamma,
    op_cmd_scale_image_rect,
    op_occlusion_begin,
    op_occlusion_end,
    op_occlusion_result,
    op_surface_create,
    op_surface_destroy,
    op_swapchain_create,
    op_swapchain_destroy,
    op_swapchain_acquire,
    op_cmd_blit_to_swapchain,
    op_swapchain_present,
};
static_assert(sizeof(k_ops) == 106 * sizeof(void *), "every operation of s_gpu_backend_ops");

// ---- creation ----

// An environment variable as a decimal number; `fallback` when unset or not a number.
int32_t environment_number(const char *name, int32_t fallback)
{
    const char *text = getenv(name);
    if (text == 0 || *text < '0' || *text > '9')
        return fallback;
    int32_t value = 0;
    for (; *text >= '0' && *text <= '9' && value < 100000; text++)
        value = value * 10 + (*text - '0');
    return value;
}

template <typename T> T load_symbol(void *module, const char *name)
{
    T function;
    void *address = module != 0 ? host_GetProcAddress(module, name) : 0;
    memcpy(&function, &address, sizeof(function));
    return function;
}

// The adapter's description as ASCII (caps.device_name).
void adapter_name(const DXGI_ADAPTER_DESC1 &desc, char *out, size_t size)
{
    size_t n = 0;
    for (; n + 1 < size && n < 128 && desc.Description[n] != 0; n++)
        out[n] = desc.Description[n] < 128 ? (char)desc.Description[n] : '?';
    out[n] = '\0';
}

gpu_result create_device(s_d3d12 *d, const s_gpu_backend_create_info *info)
{
    d->d3d12_module = host_LoadLibraryA("d3d12.dll");
    d->dxgi_module = host_LoadLibraryA("dxgi.dll");
    d->create_device = load_symbol<PFN_D3D12_CREATE_DEVICE>(d->d3d12_module, "D3D12CreateDevice");
    d->get_debug_interface = load_symbol<PFN_D3D12_GET_DEBUG_INTERFACE>(d->d3d12_module, "D3D12GetDebugInterface");
    d->create_factory = load_symbol<PFN_CREATE_DXGI_FACTORY2>(d->dxgi_module, "CreateDXGIFactory2");
    if (d->create_device == 0 || d->create_factory == 0) {
        log_text(d, "d3d12: Direct3D 12 is not available (%s)",
                 d->d3d12_module == 0 ? "no d3d12.dll" : d->dxgi_module == 0 ? "no dxgi.dll" : "no DXGI 1.4 factory");
        return k_gpu_unsupported;
    }
    d->user32_module = host_LoadLibraryA("user32.dll");
    d->get_client_rect = load_symbol<int(__stdcall *)(HWND, RECT *)>(d->user32_module, "GetClientRect");
    // the debug layer (HALO3_D3D12_DEBUG=1; 2: also GPU-based validation; or the launch's validation request)
    const int32_t debug = environment_number("HALO3_D3D12_DEBUG", info->validation ? 1 : 0);
    if (debug > 0 && d->get_debug_interface != 0) {
        ID3D12Debug1 *debug1 = 0;
        ID3D12Debug *layer = 0;
        if (SUCCEEDED(d->get_debug_interface(IID_ID3D12Debug, (void **)&layer))) {
            layer->lpVtbl->EnableDebugLayer(layer);
            d->debug = 1;
            if (debug >= 2 && SUCCEEDED(layer->lpVtbl->QueryInterface(layer, IID_ID3D12Debug1, (void **)&debug1))) {
                debug1->lpVtbl->SetEnableGPUBasedValidation(debug1, 1);
                d->debug = 2;
                release(debug1);
            }
            release(layer);
        } else {
            log_text(d, "d3d12: HALO3_D3D12_DEBUG: the debug layer is not installed (Graphics Tools)");
        }
    }
    HRESULT result = d->create_factory(d->debug ? DXGI_CREATE_FACTORY_DEBUG : 0u, IID_IDXGIFactory4, (void **)&d->factory);
    if (FAILED(result) && d->debug)
        result = d->create_factory(0, IID_IDXGIFactory4, (void **)&d->factory);
    if (FAILED(result)) {
        log_text(d, "d3d12: CreateDXGIFactory2 failed: %s %08x", hresult_text(result), (uint32_t)result);
        return k_gpu_unsupported;
    }
    if (FAILED(d->factory->lpVtbl->QueryInterface(d->factory, IID_IDXGIFactory5, (void **)&d->factory5)))
        d->factory5 = 0;
    if (FAILED(d->factory->lpVtbl->QueryInterface(d->factory, IID_IDXGIFactory6, (void **)&d->factory6)))
        d->factory6 = 0;
    if (d->factory5 != 0) {
        BOOL tearing = 0;
        if (SUCCEEDED(d->factory5->lpVtbl->CheckFeatureSupport(d->factory5, DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing,
                                                               sizeof(tearing))))
            d->tearing = tearing != 0;
    }
    // the adapter: the one asked for (device_index, HALO3_D3D12_DEVICE), else the first hardware one that makes a
    // device, high performance first; WARP only with HALO3_D3D12_WARP=1
    const int32_t wanted = info->device_index >= 0 ? info->device_index : environment_number("HALO3_D3D12_DEVICE", -1);
    const bool warp = environment_number("HALO3_D3D12_WARP", 0) != 0;
    for (uint32_t i = 0; d->device == 0; i++) {
        IDXGIAdapter1 *adapter = 0;
        result = d->factory6 != 0 ? d->factory6->lpVtbl->EnumAdapterByGpuPreference(
                                        d->factory6, i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_IDXGIAdapter1,
                                        (void **)&adapter)
                                  : d->factory->lpVtbl->EnumAdapters1(d->factory, i, &adapter);
        if (FAILED(result))
            break; // DXGI_ERROR_NOT_FOUND: no more
        DXGI_ADAPTER_DESC1 desc = {};
        adapter->lpVtbl->GetDesc1(adapter, &desc);
        const bool software = (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0;
        if ((wanted >= 0 && (int32_t)i != wanted) || (wanted < 0 && software && !warp)) {
            release(adapter);
            continue;
        }
        result = d->create_device((IUnknown *)adapter, D3D_FEATURE_LEVEL_11_0, IID_ID3D12Device, (void **)&d->device);
        if (SUCCEEDED(result)) {
            d->adapter = adapter;
            adapter_name(desc, d->base.caps.device_name, sizeof(d->base.caps.device_name));
        } else {
            char name[256];
            adapter_name(desc, name, sizeof(name));
            log_text(d, "d3d12: adapter %u (%s) makes no device: %s %08x", i, name, hresult_text(result),
                     (uint32_t)result);
            d->device = 0;
            release(adapter);
        }
    }
    if (d->device == 0 && warp) {
        IDXGIAdapter1 *adapter = 0;
        if (SUCCEEDED(d->factory->lpVtbl->EnumWarpAdapter(d->factory, IID_IDXGIAdapter1, (void **)&adapter))) {
            DXGI_ADAPTER_DESC1 desc = {};
            adapter->lpVtbl->GetDesc1(adapter, &desc);
            if (SUCCEEDED(d->create_device((IUnknown *)adapter, D3D_FEATURE_LEVEL_11_0, IID_ID3D12Device,
                                           (void **)&d->device))) {
                d->adapter = adapter;
                adapter_name(desc, d->base.caps.device_name, sizeof(d->base.caps.device_name));
            } else {
                d->device = 0;
                release(adapter);
            }
        }
    }
    if (d->device == 0) {
        log_text(d, wanted >= 0 ? "d3d12: adapter %d makes no Direct3D 12 device" : "d3d12: no adapter makes a Direct3D "
                                                                                     "12 device (feature level 11_0)",
                 wanted);
        return k_gpu_unsupported;
    }
    // what the Xenos layout needs: 192 samplers a stage (resource binding tier 2)
    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
    d->device->lpVtbl->CheckFeatureSupport(d->device, D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));
    if (options.ResourceBindingTier < D3D12_RESOURCE_BINDING_TIER_2) {
        log_text(d, "d3d12: %s has resource binding tier %u; the Xenos layout needs tier 2 (192 samplers a stage)",
                 d->base.caps.device_name, (uint32_t)options.ResourceBindingTier);
        return k_gpu_unsupported;
    }
    // the pack's and the backend's DXIL is shader model 6.0 and their root signatures version 1.1 (descriptor range
    // flags): a runtime or driver without either would make the device and then fail every pipeline. A runtime that
    // predates the query refuses it (E_INVALIDARG), which says no as well
    D3D12_FEATURE_DATA_SHADER_MODEL shader_model = {};
    shader_model.HighestShaderModel = D3D_SHADER_MODEL_6_0;
    if (FAILED(d->device->lpVtbl->CheckFeatureSupport(d->device, D3D12_FEATURE_SHADER_MODEL, &shader_model,
                                                      sizeof(shader_model))) ||
        shader_model.HighestShaderModel < D3D_SHADER_MODEL_6_0) {
        log_text(d, "d3d12: %s does not run shader model 6.0 (DXIL); a newer driver may", d->base.caps.device_name);
        return k_gpu_unsupported;
    }
    D3D12_FEATURE_DATA_ROOT_SIGNATURE root_signature = {};
    root_signature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(d->device->lpVtbl->CheckFeatureSupport(d->device, D3D12_FEATURE_ROOT_SIGNATURE, &root_signature,
                                                      sizeof(root_signature))) ||
        root_signature.HighestVersion < D3D_ROOT_SIGNATURE_VERSION_1_1) {
        log_text(d, "d3d12: the Direct3D 12 runtime takes no version 1.1 root signature (Windows 10 1607 or later does)");
        return k_gpu_unsupported;
    }
    D3D12_FEATURE_DATA_D3D12_OPTIONS8 options8 = {};
    if (SUCCEEDED(d->device->lpVtbl->CheckFeatureSupport(d->device, D3D12_FEATURE_D3D12_OPTIONS8, &options8,
                                                         sizeof(options8))))
        d->unaligned_bc = options8.UnalignedBlockTexturesSupported != 0;
    D3D12_FEATURE_DATA_D3D12_OPTIONS13 options13 = {};
    if (SUCCEEDED(d->device->lpVtbl->CheckFeatureSupport(d->device, D3D12_FEATURE_D3D12_OPTIONS13, &options13,
                                                         sizeof(options13)))) {
        d->unrestricted_copy_pitch = options13.UnrestrictedBufferTextureCopyPitchSupported != 0;
        d->alpha_blend_factor = options13.AlphaBlendFactorSupported != 0;
    }
    static const D3D_FEATURE_LEVEL k_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_12_0,
                                                  D3D_FEATURE_LEVEL_12_1 };
    D3D12_FEATURE_DATA_FEATURE_LEVELS levels = {};
    levels.NumFeatureLevels = sizeof(k_levels) / sizeof(k_levels[0]);
    levels.pFeatureLevelsRequested = k_levels;
    levels.MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_11_0;
    d->device->lpVtbl->CheckFeatureSupport(d->device, D3D12_FEATURE_FEATURE_LEVELS, &levels, sizeof(levels));
    d->base.caps.api_major = (uint32_t)levels.MaxSupportedFeatureLevel >> 12;
    d->base.caps.api_minor = ((uint32_t)levels.MaxSupportedFeatureLevel >> 8) & 15u;
    if (d->debug && SUCCEEDED(d->device->lpVtbl->QueryInterface(d->device, IID_ID3D12InfoQueue, (void **)&d->info_queue))) {
        // keep what matters: corruption, errors, warnings
        D3D12_MESSAGE_SEVERITY denied[2] = { D3D12_MESSAGE_SEVERITY_INFO, D3D12_MESSAGE_SEVERITY_MESSAGE };
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumSeverities = 2;
        filter.DenyList.pSeverityList = denied;
        d->info_queue->lpVtbl->PushStorageFilter(d->info_queue, &filter);
    }
    // the pipeline library (pipeline_cache_d3d12.bin): the blob must outlive it
    if (SUCCEEDED(d->device->lpVtbl->QueryInterface(d->device, IID_ID3D12Device1, (void **)&d->device1))) {
        if (info->pipeline_cache_data != 0 && info->pipeline_cache_size != 0) {
            d->library_blob = malloc(info->pipeline_cache_size);
            if (d->library_blob != 0) {
                memcpy(d->library_blob, info->pipeline_cache_data, info->pipeline_cache_size);
                result = d->device1->lpVtbl->CreatePipelineLibrary(d->device1, d->library_blob, info->pipeline_cache_size,
                                                                   IID_ID3D12PipelineLibrary, (void **)&d->library);
                if (FAILED(result)) {
                    // another driver or device (D3D12_ERROR_DRIVER_VERSION_MISMATCH / ADAPTER_NOT_FOUND), or damaged
                    log_text(d, "d3d12: the pipeline cache was not taken (%s %08x): starting empty", hresult_text(result),
                             (uint32_t)result);
                    d->library = 0;
                    free(d->library_blob);
                    d->library_blob = 0;
                }
            }
        }
        if (d->library == 0 &&
            FAILED(d->device1->lpVtbl->CreatePipelineLibrary(d->device1, 0, 0, IID_ID3D12PipelineLibrary,
                                                             (void **)&d->library)))
            d->library = 0;
    }
    D3D12_COMMAND_QUEUE_DESC queue = {};
    queue.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    result = d->device->lpVtbl->CreateCommandQueue(d->device, &queue, IID_ID3D12CommandQueue, (void **)&d->queue);
    if (SUCCEEDED(result))
        result = d->device->lpVtbl->CreateFence(d->device, 0, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence,
                                                (void **)&d->frame_fence);
    if (SUCCEEDED(result))
        result = d->device->lpVtbl->CreateFence(d->device, 0, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence,
                                                (void **)&d->immediate_fence);
    if (SUCCEEDED(result))
        result = d->device->lpVtbl->CreateCommandAllocator(d->device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                           IID_ID3D12CommandAllocator, (void **)&d->immediate_allocator);
    if (SUCCEEDED(result))
        result = d->device->lpVtbl->CreateCommandList(d->device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                      d->immediate_allocator, 0, IID_ID3D12GraphicsCommandList,
                                                      (void **)&d->immediate_list);
    if (SUCCEEDED(result))
        result = d->immediate_list->lpVtbl->Close(d->immediate_list);
    d->event = host_CreateEventA(0, 0, 0, 0);
    if (FAILED(result) || d->event == 0) {
        log_text(d, "d3d12: the queue, fences or command lists could not be made: %s %08x", hresult_text(result),
                 (uint32_t)result);
        return result_of(FAILED(result) ? result : E_FAIL);
    }
    if (FAILED(d->queue->lpVtbl->GetTimestampFrequency(d->queue, &d->timestamp_frequency)))
        d->timestamp_frequency = 0;
    d->stats.gpu_frame_ms = -1.0f;
    // descriptors
    if (!pool_init(d, &d->views, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4096) ||
        !pool_init(d, &d->samplers, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 2048) ||
        !pool_init(d, &d->rtvs, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 512) ||
        !pool_init(d, &d->dsvs, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 256))
        return k_gpu_error;
    d->view_increment = d->views.increment;
    d->sampler_increment = d->samplers.increment;
    d->frame_count = info->frames_in_flight == 0 ? 2u : info->frames_in_flight > GPU_FRAMES_MAX ? GPU_FRAMES_MAX
                                                                                                : info->frames_in_flight;
    d->chunk_base = k_static_descriptors;
    // chunks for texture groups that outlive a frame: the frontend's cached sets, as many retired by resets in the
    // frames in flight, and one reset more; at most what the frame regions leave of the tier 2 limit (past it a group
    // is transient: group_frame_copy)
    const uint32_t chunks_wanted = k_cached_groups * (d->frame_count + 2u);
    const uint32_t budget = (D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2 - k_static_descriptors -
                             d->frame_count * k_frame_descriptors) / k_group_srvs;
    d->group_chunks = chunks_wanted < budget ? chunks_wanted : budget;
    d->view_heap_size = k_static_descriptors + d->group_chunks * k_group_srvs + d->frame_count * k_frame_descriptors;
    D3D12_DESCRIPTOR_HEAP_DESC heap = {};
    heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heap.NumDescriptors = d->view_heap_size;
    heap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    result = d->device->lpVtbl->CreateDescriptorHeap(d->device, &heap, IID_ID3D12DescriptorHeap, (void **)&d->view_heap);
    if (FAILED(result)) {
        log_text(d, "d3d12: the shader-visible descriptor heap (%u) could not be made: %s %08x", d->view_heap_size,
                 hresult_text(result), (uint32_t)result);
        return result_of(result);
    }
    d->view_heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(d->view_heap, &d->view_cpu);
    d->view_heap->lpVtbl->GetGPUDescriptorHandleForHeapStart(d->view_heap, &d->view_gpu);
    d->sampler_heap = sampler_heap_make(d);
    if (d->sampler_heap == 0)
        return k_gpu_error;
    // the null tables (static_null_tables): textures by dimension, vertex streams, memexport
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC view = {};
        view.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        for (uint32_t i = 0; i < k_group_srvs; i++) {
            memset(&view.Texture2DArray, 0, sizeof(view.Texture2DArray));
            view.ViewDimension = i < XE_BINDING_TEX_3D     ? D3D12_SRV_DIMENSION_TEXTURE2D
                                 : i < XE_BINDING_TEX_CUBE ? D3D12_SRV_DIMENSION_TEXTURE3D
                                 : i < XE_BINDING_TEX_STACKED ? D3D12_SRV_DIMENSION_TEXTURECUBE
                                                              : D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            view.Texture2D.MipLevels = 1; // the same field in every texture view's union member
            if (view.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2DARRAY)
                view.Texture2DArray.ArraySize = 1;
            d->device->lpVtbl->CreateShaderResourceView(d->device, 0, &view, view_heap_cpu(d, i));
        }
        D3D12_SHADER_RESOURCE_VIEW_DESC raw = {};
        raw.Format = DXGI_FORMAT_R32_TYPELESS;
        raw.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        raw.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        raw.Buffer.NumElements = 1;
        raw.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
        D3D12_UNORDERED_ACCESS_VIEW_DESC raw_uav = {};
        raw_uav.Format = DXGI_FORMAT_R32_TYPELESS;
        raw_uav.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        raw_uav.Buffer.NumElements = 1;
        raw_uav.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
        for (uint32_t s = 0; s < XE_D3D12_VERTEX_STREAMS; s++)
            d->device->lpVtbl->CreateShaderResourceView(d->device, 0, &raw, view_heap_cpu(d, k_group_srvs + s));
        for (uint32_t e = 0; e < XE_D3D12_MEMEXPORT_BUFFERS; e++)
            d->device->lpVtbl->CreateUnorderedAccessView(d->device, 0, 0, &raw_uav,
                                                         view_heap_cpu(d, k_group_srvs + XE_D3D12_VERTEX_STREAMS + e));
        // and the same nulls in the CPU pool, for the tables copied together (memory tables, pass tables)
        d->null_srv_buffer = pool_alloc(d, &d->views);
        d->null_uav_buffer = pool_alloc(d, &d->views);
        d->null_srv_2d = pool_alloc(d, &d->views);
        if (d->null_srv_buffer == k_none || d->null_uav_buffer == k_none || d->null_srv_2d == k_none)
            return k_gpu_error;
        d->device->lpVtbl->CreateShaderResourceView(d->device, 0, &raw, pool_cpu(&d->views, d->null_srv_buffer));
        d->device->lpVtbl->CreateUnorderedAccessView(d->device, 0, 0, &raw_uav, pool_cpu(&d->views, d->null_uav_buffer));
        memset(&view.Texture2DArray, 0, sizeof(view.Texture2DArray));
        view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        view.Texture2D.MipLevels = 1;
        d->device->lpVtbl->CreateShaderResourceView(d->device, 0, &view, pool_cpu(&d->views, d->null_srv_2d));
        d->null_rtv = pool_alloc(d, &d->rtvs);
        if (d->null_rtv == k_none)
            return k_gpu_error;
        D3D12_RENDER_TARGET_VIEW_DESC target = {};
        target.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        target.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        d->device->lpVtbl->CreateRenderTargetView(d->device, 0, &target, pool_cpu(&d->rtvs, d->null_rtv));
        d->rtv_ring_first = d->rtvs.next;
        for (uint32_t i = 0; i < k_rtv_ring; i++)
            if (pool_alloc(d, &d->rtvs) != d->rtv_ring_first + i)
                return k_gpu_error;
    }
    // frames
    for (uint32_t i = 0; i < d->frame_count; i++) {
        s_frame &frame = d->frames[i];
        frame.view_base = d->chunk_base + d->group_chunks * k_group_srvs + i * k_frame_descriptors;
        frame.acquired_image = k_none;
        result = d->device->lpVtbl->CreateCommandAllocator(d->device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                           IID_ID3D12CommandAllocator, (void **)&frame.allocator);
        if (SUCCEEDED(result))
            result = d->device->lpVtbl->CreateCommandList(d->device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, frame.allocator, 0,
                                                          IID_ID3D12GraphicsCommandList, (void **)&frame.list);
        if (SUCCEEDED(result))
            result = frame.list->lpVtbl->Close(frame.list);
        if (SUCCEEDED(result))
            result = d->device->lpVtbl->CreateCommandAllocator(d->device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                               IID_ID3D12CommandAllocator, (void **)&frame.upload_allocator);
        if (SUCCEEDED(result))
            result = d->device->lpVtbl->CreateCommandList(d->device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                          frame.upload_allocator, 0, IID_ID3D12GraphicsCommandList,
                                                          (void **)&frame.upload_list);
        if (SUCCEEDED(result))
            result = frame.upload_list->lpVtbl->Close(frame.upload_list);
        D3D12_QUERY_HEAP_DESC queries = {};
        queries.Type = D3D12_QUERY_HEAP_TYPE_OCCLUSION;
        queries.Count = k_occlusion_queries;
        if (SUCCEEDED(result))
            result = d->device->lpVtbl->CreateQueryHeap(d->device, &queries, IID_ID3D12QueryHeap,
                                                        (void **)&frame.occlusion_heap);
        queries.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
        queries.Count = 2;
        if (SUCCEEDED(result) && d->timestamp_frequency != 0 &&
            FAILED(d->device->lpVtbl->CreateQueryHeap(d->device, &queries, IID_ID3D12QueryHeap,
                                                      (void **)&frame.timestamp_heap)))
            frame.timestamp_heap = 0; // no GPU frame time
        if (FAILED(result)) {
            log_text(d, "d3d12: frame slot %u could not be made: %s %08x", i, hresult_text(result), (uint32_t)result);
            return result_of(result);
        }
        frame.occlusion_readback = buffer_make(d, k_occlusion_queries * 8u, k_gpu_buffer_transfer_dst,
                                               k_gpu_memory_readback);
        frame.records = (s_occlusion_record *)calloc(k_occlusion_records, sizeof(s_occlusion_record));
        frame.results = (uint64_t *)calloc(k_occlusion_records, sizeof(uint64_t));
        if (frame.timestamp_heap != 0)
            frame.timestamp_readback = buffer_make(d, 16, k_gpu_buffer_transfer_dst, k_gpu_memory_readback);
        if (frame.occlusion_readback == 0 || frame.records == 0 || frame.results == 0 ||
            (frame.timestamp_heap != 0 && frame.timestamp_readback == 0))
            return k_gpu_out_of_memory;
    }
    d->frame_index = d->frame_count - 1u; // the first frame_begin takes slot 0
    // the backend's own layouts (their root signatures embedded in the DXIL)
    d->helper_layout.root = root_signature_make(d, k_edram_7e3_dxil, sizeof(k_edram_7e3_dxil), "helper");
    d->helper_layout.kind = k_layout_helper;
    d->copy_layout.root = root_signature_make(d, k_d3d12_copy_vs_dxil, sizeof(k_d3d12_copy_vs_dxil), "copy");
    d->copy_layout.kind = k_layout_copy;
    if (d->helper_layout.root == 0 || d->copy_layout.root == 0)
        return k_gpu_error;
    // constant blocks nothing set read zeros (a root CBV reads up to 64 KiB)
    d->zero_buffer = buffer_make(d, 65536, k_gpu_buffer_uniform, k_gpu_memory_upload);
    if (d->zero_buffer == 0)
        return k_gpu_out_of_memory;
    memset(d->zero_buffer->mapped, 0, 65536);
    query_format_support(d);
    d->depth_format = format_has_support(d, k_gpu_format_d32_sfloat_s8_uint, D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL)
                          ? (uint32_t)k_gpu_format_d32_sfloat_s8_uint
                          : (uint32_t)k_gpu_format_d24_unorm_s8_uint;
    return k_gpu_ok;
}

void backend_destroy(s_d3d12 *d)
{
    if (d == 0)
        return;
    if (d->queue != 0 && d->frame_fence != 0)
        queue_idle(d);
    surface_destroy(d);
    for (uint32_t i = 0; i < GPU_FRAMES_MAX; i++) {
        s_frame &frame = d->frames[i];
        destroy_retired(d, frame);
        for (uint32_t g = 0; g < frame.groups.count; g++)
            group_release(d, frame.groups.items[g]);
        frame.groups.release();
        frame.retired.release();
        frame.touched.release();
        release(frame.list);
        release(frame.allocator);
        release(frame.upload_list);
        release(frame.upload_allocator);
        release(frame.occlusion_heap);
        release(frame.timestamp_heap);
        buffer_release(d, frame.occlusion_readback);
        buffer_release(d, frame.timestamp_readback);
        buffer_release(d, frame.staging);
        free(frame.records);
        free(frame.results);
    }
    for (uint32_t i = 0; i < d->cached_groups.count; i++)
        group_release(d, d->cached_groups.items[i]);
    d->cached_groups.release();
    d->copy_passes.release();
    for (uint32_t i = 0; i < d->psos.count; i++) {
        release(d->psos.items[i]->pso);
        free(d->psos.items[i]);
    }
    d->psos.release();
    // what retiring them queued (pso_put of the last references) after the frames were emptied
    for (uint32_t i = 0; i < GPU_FRAMES_MAX; i++) {
        destroy_retired(d, d->frames[i]);
        d->frames[i].retired.release();
    }
    buffer_pool_trim(d, true);
    d->buffer_pool.release();
    for (uint32_t i = 0; i < d->modules.count; i++)
        free(d->modules.items[i]);
    d->modules.release();
    release(d->xenos_layout.root);
    release(d->helper_layout.root);
    release(d->copy_layout.root);
    buffer_release(d, d->zero_buffer);
    for (uint32_t i = 0; i < d->sampler_heaps.count; i++) {
        release(d->sampler_heaps.items[i]->heap);
        free(d->sampler_heaps.items[i]);
    }
    d->sampler_heaps.release();
    d->free_chunks.release();
    d->barriers.release();
    release(d->view_heap);
    pool_destroy(&d->views);
    pool_destroy(&d->samplers);
    pool_destroy(&d->rtvs);
    pool_destroy(&d->dsvs);
    release(d->library);
    free(d->library_blob);
    release(d->immediate_list);
    release(d->immediate_allocator);
    release(d->immediate_fence);
    release(d->frame_fence);
    if (d->event != 0)
        host_CloseHandle(d->event);
    release(d->queue);
    drain_debug_messages(d);
    release(d->info_queue);
    release(d->device1);
    release(d->device);
    release(d->adapter);
    release(d->factory6);
    release(d->factory5);
    release(d->factory);
    if (d->user32_module != 0)
        host_FreeLibrary(d->user32_module);
    if (d->dxgi_module != 0)
        host_FreeLibrary(d->dxgi_module);
    if (d->d3d12_module != 0)
        host_FreeLibrary(d->d3d12_module);
    free(d);
}

} // namespace

extern "C" gpu_result d3d12_gpu_backend_create(const s_gpu_backend_create_info *info, s_gpu_backend **out_backend)
{
    *out_backend = 0;
    s_d3d12 *d = (s_d3d12 *)calloc(1, sizeof(s_d3d12));
    if (d == 0)
        return k_gpu_out_of_memory;
    d->log = info->log != 0 ? info->log : default_log;
    d->log_context = info->log != 0 ? info->log_context : 0;
    d->base.ops = &k_ops;
    const gpu_result result = create_device(d, info);
    if (result != k_gpu_ok) {
        backend_destroy(d);
        return result;
    }
    s_gpu_caps &caps = d->base.caps;
    caps.kind = k_gpu_backend_d3d12;
    caps.name = "d3d12";
    caps.api_name = "Direct3D 12";
    caps.shader_module_format = SHADER_PACK_MODULE_FORMAT_DXIL;
    caps.pipeline_cache_file = gpu_backend_pipeline_cache_file(k_gpu_backend_d3d12);
    caps.frames_in_flight = d->frame_count;
    caps.uniform_offset_alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    caps.depth_stencil_format = d->depth_format;
    caps.validation = d->debug != 0;
    caps.checkpoints = 0;
    caps.buffer_device_address = 0;         // vertex fetch and memexport through the memory tables (space2)
    caps.vertex_id_excludes_base_vertex = 1; // SV_VertexID counts from 0
    caps.point_sprites = 0;                 // points through the pack's point geometry shader
    caps.triangle_fans = 0;                 // the frontend draws fans as lists
    caps.clip_distance = 1;
    caps.geometry = 1;
    caps.tessellation = 1;
    caps.full_draw_index = 1;
    caps.sampler_mirror_clamp_to_edge = 1;  // D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
    caps.pipeline_library = 0;
    caps.pipeline_library_fast_link = 0;
    caps.max_stage_samplers = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE; // resource binding tier 2
    caps.max_stage_sampled_images = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2;
    caps.max_stage_uniform_buffers = 14;
    caps.max_dynamic_uniform_buffers = XE_BINDING_VERTEX_FETCH + 1; // the root CBVs of the Xenos layout
    caps.max_bind_groups = 2;
    log_text(d, "d3d12: %s, feature level %u_%u%s%s%s", caps.device_name, caps.api_major, caps.api_minor,
             d->debug ? ", debug layer" : "", d->library != 0 ? ", pipeline library" : "",
             d->tearing ? ", tearing" : "");
    *out_backend = &d->base;
    return k_gpu_ok;
}

#endif /* _WIN32 */
