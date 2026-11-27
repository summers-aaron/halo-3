// port/sdk/d3d: the PC renderer behind the Xbox D3D device (vulkan_device.h): its Xenos GPU frontend. Host code
// (halo3_gpu): the D3D device (null_device.cpp, built against the XDK) describes its state in vulkan_device.h's plain
// structures; this file turns it into GPU work (render targets and EDRAM aliasing, resolves, textures, constants, vertex
// fetch, memexport, pipelines from the shader pack, presentation) and owns the frontend caches. The GPU seam selects
// Vulkan or Direct3D 12. Vulkan supplies the native graphics bind and cropped depth resolve paths.
#include "vulkan_device.h"

#include "gpu_backend.h"
#include "vulkan_internal.h"
#include "gpu_crt.h"
#include "gpu_host.h"
#include "xenos_vertex.h"
#include "../../shaders/shader_pack.h"
#include "../../shaders/xenos_spirv_abi.h"
#include "../../data/geometry_swap.h"
#include "../../data/texture_convert.h"
#include "../runtime/port_config.h"
#include "../runtime/port_runtime.h"
#include "../runtime/port_window.h"
#include "../kernel/storage.h"
extern "C" {
#include "../kernel/storage_host.h"
}

// The SDK's performance counter (port/sdk/kernel/time_*.c): frame timing only (HALO3_FRAME_STATS).
extern "C" int PORT_MS_ABI QueryPerformanceCounter(long long *count);
extern "C" int PORT_MS_ABI QueryPerformanceFrequency(long long *frequency);
// The SDK's thread id (port/sdk/kernel/kernel_linux.c; the forwarded Win32 call on Windows).
extern "C" unsigned long PORT_MS_ABI GetCurrentThreadId(void);
extern "C" void PORT_MS_ABI Sleep(unsigned long milliseconds);
extern "C" PORT_MS_ABI unsigned int port_config_anisotropy(void);

void vulkan_cmd_flush_memexports(s_vulkan_backend *backend);
struct s_memexport_sync_range {
    uint64_t base, end;
    bool write;
};
void vulkan_cmd_dispatch_memexport(s_vulkan_backend *backend, uint32_t groups,
                                   const s_memexport_sync_range *ranges, uint32_t count);

static_assert(sizeof(void *) != 8 || sizeof(s_xenos_texture_desc) == 28, "ABI size mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_d3d_texture_source) == 72 &&
    offsetof(s_port_d3d_texture_source, base) == 0 && offsetof(s_port_d3d_texture_source, mip) == 8 &&
    offsetof(s_port_d3d_texture_source, desc) == 16 && offsetof(s_port_d3d_texture_source, identifier) == 44 &&
    offsetof(s_port_d3d_texture_source, fetch_constant) == 48), "s_port_d3d_texture_source 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_surface) == 40 &&
    offsetof(s_port_vk_surface, key) == 0 && offsetof(s_port_vk_surface, msaa_samples) == 32), "s_port_vk_surface 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_texture) == 88 &&
    offsetof(s_port_vk_texture, key) == 0 && offsetof(s_port_vk_texture, generation) == 8 &&
    offsetof(s_port_vk_texture, source) == 16), "s_port_vk_texture 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || sizeof(s_port_vk_sampler_state) == 52, "s_port_vk_sampler_state size mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_sampler) == 144 &&
    offsetof(s_port_vk_sampler, texture) == 0 && offsetof(s_port_vk_sampler, state) == 88 &&
    offsetof(s_port_vk_sampler, bound) == 140), "s_port_vk_sampler 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || sizeof(s_port_vk_render_state) == 148, "s_port_vk_render_state size mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_stream) == 32 &&
    offsetof(s_port_vk_stream, key) == 0 && offsetof(s_port_vk_stream, data) == 8), "s_port_vk_stream 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_indices) == 32 &&
    offsetof(s_port_vk_indices, key) == 0 && offsetof(s_port_vk_indices, data) == 8), "s_port_vk_indices 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_draw) == 4776 &&
    offsetof(s_port_vk_draw, state) == 24 && offsetof(s_port_vk_draw, streams) == 216 &&
    offsetof(s_port_vk_draw, indices) == 728 && offsetof(s_port_vk_draw, samplers) == 760 &&
    offsetof(s_port_vk_draw, clip_plane_enable) == 4504 && offsetof(s_port_vk_draw, point_size_max) == 4620 &&
    offsetof(s_port_vk_draw, samplers_any_gen) == 4748 && offsetof(s_port_vk_draw, vp_gen) == 4752 &&
    offsetof(s_port_vk_draw, vs_chunk_gen) == 4760 && offsetof(s_port_vk_draw, ps_chunk_gen) == 4768),
    "s_port_vk_draw 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_geometry) == 56 &&
    offsetof(s_port_vk_geometry, stream0) == 0 && offsetof(s_port_vk_geometry, indices) == 32 &&
    offsetof(s_port_vk_geometry, declaration) == 40 && offsetof(s_port_vk_geometry, stream_mask) == 48),
    "s_port_vk_geometry 64-bit ABI mismatch");
static_assert(PORT_VK_SAMPLER_COUNT == 26 && PORT_VK_DIRTY_SAMPLER(25) == (1ull << 63),
    "draw dirty mask ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_compute_resource) == 24 &&
    offsetof(s_port_vk_compute_resource, storage) == 0 && offsetof(s_port_vk_compute_resource, binding) == 8),
    "s_port_vk_compute_resource 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_compute_constants) == 24 &&
    offsetof(s_port_vk_compute_constants, data) == 8), "s_port_vk_compute_constants 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_compute_dispatch) == 40 &&
    offsetof(s_port_vk_compute_dispatch, resources) == 0 && offsetof(s_port_vk_compute_dispatch, constants) == 8 &&
    offsetof(s_port_vk_compute_dispatch, explicit_index) == 16), "s_port_vk_compute_dispatch 64-bit ABI mismatch");

uint32_t g_port_vk_resource_touch_epoch = 1; // atomic: the pager and render thread share it

namespace {

// Vulkan pipeline wrappers read the layout table. Let workers compile in parallel and exclude operations that can
// add a layout (including lazy image passes). Library compilation and optimization use only raw handles.
s_port_gpu_monitor *g_xe_link_layout_monitor;
uint32_t g_xe_layout_readers;
uint32_t g_xe_layout_writers;
bool g_vk_display_gamma_initialized, g_vk_reinterpret_7e3_initialized;

struct s_xe_layout_lock {
    bool read_only;
    explicit s_xe_layout_lock(bool read = false) : read_only(read)
    {
        if (g_xe_link_layout_monitor == 0)
            return;
        port_gpu_monitor_lock(g_xe_link_layout_monitor);
        if (read_only) {
            while (g_xe_layout_writers != 0)
                port_gpu_monitor_wait(g_xe_link_layout_monitor);
            g_xe_layout_readers++;
            port_gpu_monitor_unlock(g_xe_link_layout_monitor);
        } else {
            g_xe_layout_writers++;
            while (g_xe_layout_readers != 0)
                port_gpu_monitor_wait(g_xe_link_layout_monitor);
        }
    }
    ~s_xe_layout_lock()
    {
        if (g_xe_link_layout_monitor == 0)
            return;
        if (read_only) {
            port_gpu_monitor_lock(g_xe_link_layout_monitor);
            if (--g_xe_layout_readers == 0)
                port_gpu_monitor_broadcast(g_xe_link_layout_monitor);
        } else if (--g_xe_layout_writers == 0) {
            port_gpu_monitor_broadcast(g_xe_link_layout_monitor);
        }
        port_gpu_monitor_unlock(g_xe_link_layout_monitor);
    }
};

template <typename F, typename... T> auto xe_layout_call(F op, T... args) -> decltype(op(args...))
{
    s_xe_layout_lock lock;
    return op(args...);
}

// On valid input these Vulkan image passes create their layout once or remember a permanent failure.
template <typename F, typename... T>
void xe_layout_call_once(bool &initialized, bool valid, F op, s_gpu_backend *gpu, T... args)
{
    if (initialized) {
        op(gpu, args...);
        return;
    }
    xe_layout_call(op, gpu, args...);
    initialized = valid && gpu->caps.kind == k_gpu_backend_vulkan;
}

// ---- small containers ----

// Open-addressing map from a 32-bit key (a host address or a hash; 0 is not a key) to a 32-bit value.
struct s_map {
    uint32_t *keys;
    uint32_t *values;
    uint32_t capacity; // a power of two
    uint32_t count;
};

uint32_t mix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x7feb352du;
    h ^= h >> 15;
    h *= 0x846ca68bu;
    h ^= h >> 16;
    return h;
}

bool map_grow(s_map *map)
{
    s_map grown = {};
    grown.capacity = map->capacity != 0 ? map->capacity * 2u : 64u;
    grown.keys = (uint32_t *)calloc(grown.capacity, sizeof(uint32_t));
    grown.values = (uint32_t *)calloc(grown.capacity, sizeof(uint32_t));
    if (grown.keys == 0 || grown.values == 0) {
        free(grown.keys);
        free(grown.values);
        return false;
    }
    for (uint32_t i = 0; i < map->capacity; i++) {
        if (map->keys[i] == 0)
            continue;
        uint32_t s = mix32(map->keys[i]) & (grown.capacity - 1u);
        while (grown.keys[s] != 0)
            s = (s + 1u) & (grown.capacity - 1u);
        grown.keys[s] = map->keys[i];
        grown.values[s] = map->values[i];
        grown.count++;
    }
    free(map->keys);
    free(map->values);
    *map = grown;
    return true;
}

uint32_t *map_slot(s_map *map, uint32_t key, bool insert)
{
    if (map->capacity == 0 || (insert && (map->count + 1u) * 2u > map->capacity)) {
        if (!insert && map->capacity == 0)
            return 0;
        if (!map_grow(map))
            return 0;
    }
    uint32_t s = mix32(key) & (map->capacity - 1u);
    while (map->keys[s] != 0) {
        if (map->keys[s] == key)
            return &map->values[s];
        s = (s + 1u) & (map->capacity - 1u);
    }
    if (!insert)
        return 0;
    map->keys[s] = key;
    map->values[s] = 0;
    map->count++;
    return &map->values[s];
}

bool map_find(const s_map *map, uint32_t key, uint32_t *out)
{
    uint32_t *v = map_slot((s_map *)map, key, false);
    if (v == 0)
        return false;
    *out = *v;
    return true;
}

void map_set(s_map *map, uint32_t key, uint32_t value)
{
    uint32_t *v = map_slot(map, key, true);
    if (v != 0)
        *v = value;
}

void map_free(s_map *map)
{
    free(map->keys);
    free(map->values);
    *map = s_map();
}

uint32_t key_of(const void *p) { return (uint32_t)(uintptr_t)p; }

// A growable array of T (trivially copyable).
template <typename T> struct t_list {
    T *items;
    uint32_t count;
    uint32_t capacity;

    T *push()
    {
        if (count == capacity) {
            uint32_t grown = capacity != 0 ? capacity * 2u : 16u;
            T *items_grown = (T *)realloc(items, grown * sizeof(T));
            if (items_grown == 0)
                return 0;
            items = items_grown;
            capacity = grown;
        }
        T *item = &items[count++];
        memset(item, 0, sizeof(T));
        return item;
    }
    void release()
    {
        free(items);
        items = 0;
        count = capacity = 0;
    }
};

// ---- state ----

// PC-only cbuffer members the renderer fills (write_pc_members).
enum e_pc_member {
    k_pc_member_clip_plane,
    k_pc_member_dynamic_light_clip_plane,
    k_pc_member_always_true,
    k_pc_member_specular_enabled,
    k_pc_member_viewport_res,
    k_pc_member_viewport_res_multipliers,
    k_pc_member_viewport_bounds_uv,
    k_pc_member_render_resolution,
    k_pc_member_render_pixel_size,
    // the uint2 index range of a DX11 GPU-effect shader (particle / beam / contrail / light volume / ripple): the
    // records a compute dispatch updates, and the base a render vertex shader adds to its vertex index
    k_pc_member_particle_index_range,
    k_pc_member_beam_index_range,
    k_pc_member_contrail_index_range,
    k_pc_member_light_volume_index_range,
    k_pc_member_ripple_index_range,
    k_pc_member_count
};
const uint32_t k_pc_member_first_index_range = k_pc_member_particle_index_range;

struct s_shader_entry {
    const void *object;
    uint32_t stage;
    uint32_t microcode;     // microcode table index, SHADER_PACK_NOT_FOUND if the pack does not know the microcode
    uint32_t shader_index;  // pack shader, SHADER_PACK_NOT_FOUND without one
    uint32_t xenos;         // version 3 packs: its translation (xenos table index), SHADER_PACK_NOT_FOUND without one
    uint32_t reported;
    uint32_t clips;         // pixel shaders: 0 not looked at yet, 1 the SPIR-V discards fragments, 2 it never does
    uint32_t hash_lo, hash_hi; // its microcode hash (shader_pack_hash_microcode), for reports
    // where its PC-only cbuffer members are (scan_pc_members, e_pc_member order); binding ~0u when absent
    uint32_t pc_scanned, pc_any;
    uint32_t pc_binding[k_pc_member_count];
    uint32_t pc_offset[k_pc_member_count];
    // vertex shaders: 0 not looked at yet, 1 the SPIR-V reads SV_InstanceID (gl_InstanceIndex), 2 it does not
    uint32_t reads_instance;
    // the sampler bindings its SPIR-V compares through (SampleCmp: OpImage*Dref*), set << 16 | binding
    // (shader_compare_samplers; compare_scanned 0 until looked at)
    uint32_t compare_scanned, compare_count;
    uint32_t compare_binding[4];
    // Cached version-3 Xenos shader metadata and deduplicated literals (populated once on first draw).
    uint8_t xenos_cached;   // 0 unchecked, 1 valid, 2 missing
    uint32_t dedup_index;   // 0 unchecked, 1 no literals, otherwise index in g_xe_dedup_literals + 2
    s_shader_pack_xenos xenos_meta;
    s_gpu_pipeline *xenos_compute;
    uint32_t shell_local_fetches; // unnormalized point fetches of the current framebuffer row
    uint32_t shell_loop_mask;
    bool shell_coordinates;
    bool shell_uses_origin;
};

struct s_target_entry {
    const void *key;
    uint32_t width, height, d3d_format, host_format, depth;
    uint32_t host_width, host_height;
    uint32_t edram_base;    // s_port_vk_surface::edram_base
    int32_t color_exp_bias; // s_port_vk_surface::color_exp_bias: draws scale their color output by 2^bias
    uint32_t content;       // the EDRAM contents the image holds (g_edram_serial when they were drawn)
    // the EDRAM contents written by a color draw that sampled a depth resolve of this same EDRAM (the Xbox restoring a
    // depth buffer by drawing its resolved bits back as color): index + 1 of that texture entry, valid while
    // restored_content == content (edram_sync carries it into the depth surface)
    uint32_t restored_texture;
    uint32_t restored_content;
    s_gpu_image *image;  // owned unless back_buffer
    uint32_t snapshot_texture; // while nonzero the RT's pixels live in this texture (index + 1), image is its spare
    s_gpu_image *shell_image; // spare backing of the other height (same guest header and EDRAM pitch)
    uint32_t shell_height, strip_height;
    uint32_t back_buffer;
    uint32_t scale;         // host pixels per guest pixel (target_res_scale: 1 for post-process targets)
    uint32_t msaa;          // s_port_vk_surface::msaa_samples: the image holds one sample per pixel, sample 0 of a
                            // multisampled view over another surface's EDRAM (edram_gather_samples)
    // Deferred full-target clear recorded when not inside a render pass: folded into VK_ATTACHMENT_LOAD_OP_CLEAR when
    // ensure_pass() opens for a draw, or flushed if another operation reads this target first.
    uint32_t pending_clear_flags;
    float pending_clear_color[4];
    float pending_clear_depth;
    uint32_t pending_clear_stencil;
};

struct s_texture_entry {
    const void *key;
    const void *base, *mip;
    uint32_t d3d_format, dimension, width, height, depth, levels;
    uint32_t generation;
    uint32_t resolved;      // the image holds what a Resolve wrote
    uint32_t resolved_scale;// host pixels per guest pixel of a resolved texture (1 or g_res_scale)
    uint32_t depth_edram;   // a depth resolve: the EDRAM base of the depth surface + 1 (0: a color resolve)
    uint32_t depth_float;   // a depth resolve of a 24_8_FLOAT (D24FS8) surface: EDRAM depth is 20e4 float, else unorm24
    uint32_t as_array;      // a 3D texture uploaded as a 2D array (texture_view)
    // CPU reads of resolved memory (port_vk_texture_lock): once the game has locked it, every resolve into it is also
    // copied into `readback`, which the next lock writes into the texture's memory, as the console's resolve writes it
    uint32_t cpu_read;
    s_gpu_buffer *readback;
    uint32_t readback_size;
    // HALO3_RES_SCALE > 1: the guest-size image the scaled rectangle is sampled down into before the buffer copy
    s_gpu_image *readback_image;
    uint32_t readback_image_w, readback_image_h;
    uint32_t readback_serial; // g_frame_serial of the frame that copied (0: none yet), in slot readback_slot
    uint32_t readback_slot;
    uint32_t readback_level, readback_layer, readback_x, readback_y, readback_w, readback_h;
    uint32_t host_format;
    s_gpu_image *image;
    // a color resolve: whether the console's memory holds the PWL codes of the linear light this image holds (a gamma
    // target resolved into a gamma texture: resolve_keeps_gamma_codes) rather than the values the image holds, and how
    // many resolves have written it
    uint32_t linear_storage;
    uint32_t resolve_serial;
    uint32_t snapshot_target; // reciprocal of snapshot_texture, by index so table growth is harmless
    // the image read through the other color space (resolved_view): a fetch with other signs than the storage's
    // [0]: decode (r16g16b16a16_unorm), [1]: encode (r8g8b8a8_unorm)
    s_gpu_image *converted[2];
    uint32_t converted_serial[2];
    // a depth resolve read through a 32-bit color header (packed_depth_view): the 24_8 words its memory holds, as that
    // header's 8_8_8_8 components, packed from the depth and stencil of resolve packed_serial; sampled through the
    // header's swizzle packed_swizzle
    s_gpu_image *packed;
    uint32_t packed_serial;
    uint32_t packed_swizzle;
    // HALO3_RES_SCALE: the image sampled by draws into guest-resolution targets (resolved_at_draw_scale), box-filtered
    // down to guest size from guest_source as it was at resolve guest_serial
    s_gpu_image *guest;
    const s_gpu_image *guest_source;
    uint32_t guest_serial;
    // HALO3_TEXTURE_RECHECK: a hash of the source texels at conversion (texture_view compares it on every bind)
    uint64_t source_hash;
    uint32_t source_checked;  // g_presented when it was last compared
    // Deferred 7e3 resolve (port_vk_resolve -> port_vk_present): when a 7e3 target is resolved directly into a
    // 2_10_10_10 texture (usually the front buffer), defer gpu_cmd_reinterpret_7e3 so port_vk_present can fuse
    // float_to_7e3 into display_gamma.hlsl. Materialized before owner is overwritten or entry is sampled/released.
    uint32_t pending_7e3_owner;   // g_targets index + 1, or 0
    uint32_t pending_7e3_content; // owner->content when deferred
};

struct s_buffer_entry {
    const void *key;
    uint32_t signature;     // declaration hash ^ stride for vertex data, index size for indices
    const void *data;       // the guest bytes it was converted from (a header the pager placed again moves them)
    uint32_t size;
    uint32_t generation;
    uint32_t dynamic;       // the data changed after its first upload: copied into the frame ring from then on
    s_gpu_buffer *buffer;
};

struct s_declaration_entry {
    const void *key;
    uint32_t hash;
    s_xenos_vertex_declaration declaration;
};

struct s_pipeline_key {
    uint32_t vertex_shader, pixel_shader, declaration_hash;
    s_gpu_graphics_state state;
};

struct s_pipeline_entry {
    s_pipeline_key key;
    uint32_t hash;
    s_gpu_pipeline *pipeline; // NULL: creation failed (not retried)
};

struct s_compute_entry {
    uint32_t shader_index;       // pack shader (version 3: the Xenos module)
    s_gpu_pipeline *pipeline; // NULL: creation failed (not retried)
};

struct s_sampler_entry {
    s_gpu_sampler_desc desc;
    gpu_sampler sampler;
    s_vulkan_bindless_token bindless;
};

struct s_ring {
    s_gpu_buffer *buffer;
    uint8_t *mapped;
    uint32_t cursor;
};

const uint32_t k_ring_size = 64u << 20;
const uint32_t k_unknown_cbuffer_size = 16384;

s_gpu_backend *g_gpu;
s_vulkan_backend *g_vk;
bool g_xe_abi6;
s_shader_pack *g_pack;
s_gpu_render_target *g_back_buffer;
extern "C" int g_port_vulkan_device_started;
int g_port_vulkan_device_started; // 0: not tried, 1: up, -1: off or failed
int &g_started = g_port_vulkan_device_started;

s_port_vk_shadows g_shadows;
uint32_t g_frame_open;
uint32_t g_frame_serial;     // frames begun
uint32_t g_frame_thread;     // GetCurrentThreadId of the thread that began the last frame: the one recording them
uint32_t g_ring_splits;     // frames submitted early to free the upload ring (ring_ensure_headroom)
uint32_t g_presented;
s_ring g_rings[GPU_FRAMES_MAX];
uint32_t g_uniform_alignment = 256;
const uint32_t k_storage_alignment = 256; // >= every device's minStorageBufferOffsetAlignment (the spec's maximum)

// The *_index_range PC member of the shaders being bound (write_pc_members): (first record, end) for a memexport
// dispatch, (0, vertices per instance) for an instanced GPU-effect draw.
uint32_t g_index_range[2];

// ---- memory export (D3DDevice_BeginExport .. EndExport) ----
//
// The Xbox updates its GPU effects (particles, contrails, beams, light volumes, water ripples) with export-only vertex
// shaders: between BeginExport and EndExport of a vertex buffer the game draws a point list whose vertex i reads record
// i with vfetch and writes it with memexport (eA = a stream constant, eM# = the data), and later draws read the
// records back with vfetch from the same buffer. Bungie's DX11 build replaced each update with a compute shader writing
// RWStructuredBuffer<s_raw_*> u0, and the render vertex shaders read it as StructuredBuffer t16; the pack's microcode
// table maps the retail update vertex shader to that compute shader (port/tools/shaderpack microcode.odin). Both index
// records by the Xbox vertex index (beam_update, particle_update: index = vertex index; the render shaders compute the
// same buffer_index), so each exported guest buffer gets one device-local storage buffer of the pack's record stride,
// kept across frames: the effect state lives only there, as on the console it lives only in GPU-written memory (the
// CPU never reads it back).
struct s_export_buffer {
    const void *base;       // the guest memory (a D3DVertexBuffer's data)
    uint32_t guest_size;
    uint32_t stride;        // bytes per PC record (the storage buffer binding's stride)
    uint32_t records;
    s_port_vk_storage *storage;
};
t_list<s_export_buffer> g_export_buffers;
s_map g_export_index;       // key_of(base) -> index
struct s_export_slot {
    const void *base;
    uint32_t size, xbox_stride;
    uint32_t texture;       // port_vk_begin_texture_export: the memory is a texture level the CPU reads in host order
    uint32_t endian;        // that texture format's GPUENDIAN (s_xe_export::endian)
};
s_export_slot g_export;     // the open BeginExport (D3D has one export slot the game uses, index 0)

s_export_buffer *export_buffer_find(const void *base);
s_export_buffer *export_buffer_for(const void *base, uint32_t guest_size, uint32_t xbox_stride, uint32_t stride);
// Geometry can be borrowed from an UP draw or from the CPU-expanded legacy water path.
struct s_draw_geometry {
    const s_port_vk_draw *packet;
    const s_port_vk_geometry *override;

    const s_port_vk_draw *operator->() const { return packet; }
    uint32_t stream_mask() const { return override ? override->stream_mask : packet->stream_mask; }
    const s_port_vk_stream &stream(uint32_t slot) const
    {
        return override && slot == 0 ? override->stream0 : packet->streams[slot];
    }
    const s_port_vk_indices &indices() const
    {
        return override && override->indices ? *override->indices : packet->indices;
    }
    const void *declaration() const
    {
        return override && override->declaration ? override->declaration : packet->declaration;
    }
};

bool memexport_draw(s_draw_geometry draw, s_shader_entry *vs, uint32_t start, uint32_t count);
bool water_draw(s_draw_geometry draw, const s_shader_pack_shader_info &vs_info, uint32_t primitive_type,
                uint32_t start, uint32_t count, uint32_t indexed);
void water_draw_patches(s_draw_geometry draw, uint32_t first, uint32_t patches, const float *factors,
                        float minimum, float maximum);
const float *water_factors_for(const void *data, uint32_t size);
s_gpu_buffer *storage_buffer_of(const s_port_vk_storage *storage, uint32_t *out_size);

// A version 3 pack (translated Xenos microcode) is open: draws, memexport and tessellation take the "shader pack
// version 3" path below instead of the DX11 shaders'.
bool g_xenos;
bool xenos_startup();
void xenos_shutdown();
// pipeline libraries and their prewarm (shader pack version 3)
void prewarm_start();
void prewarm_stop();
void prewarm_frame();
void xe_prewarm_shader(uint32_t xenos);
void xenos_draw(s_draw_geometry draw, uint64_t dirty_mask, uint32_t primitive_type, uint32_t start, uint32_t count, int base_vertex,
                uint32_t indexed, s_shader_entry *vs_entry);
void xenos_draw_tessellated(s_draw_geometry draw, uint64_t dirty_mask, uint32_t primitive_type, int base_patch, uint32_t start_index,
                            uint32_t index_count, uint32_t mode, float minimum_level, float maximum_level);
int xenos_export_read(const void *base, uint32_t record, uint32_t count, void *out, uint32_t *out_stride);
void xe_exports_forget(const void *base, uint32_t size);
bool xe_exported_texture_view(const s_port_vk_texture *texture, uint32_t image_dim, gpu_view *out,
                              s_gpu_image **out_image);
void xe_export_lock(const s_port_d3d_texture_source &source, uint32_t level, uint32_t layer);
bool xe_buffer_lock(const void *base, uint32_t size, bool read_only);
void finish_frame();
bool xe_v6_allocate_arenas(s_vulkan_backend *vk);
#ifdef HALO3_64BIT
s_gpu_buffer *g_xe_readback;
uint32_t g_xe_readback_capacity;
void xe_readback_prefetch();
void xe_readback_prefetch_failed();
void xe_readback_shutdown();
#endif
uint32_t g_xe_texture_exports; // exports a texture's BeginExport made (0: texture binds skip the export lookup)
void xe_apply_placements();

// port_vk_memory_placed runs on the thread that fixes up the game's resources (the main thread),
// port_vk_memory_streamed on the resource pager's, the draws on the render thread: the ranges wait here, under a spin
// lock, until the render thread applies them (xe_apply_placements) before its next use of the exports.
struct s_placed_range {
    const void *base;
    uint32_t size;
    uint32_t streamed; // 0: a buffer header placed over the memory; 1: the pager rewrote it
};
const uint32_t k_placed_capacity = 512;
s_placed_range g_placed[k_placed_capacity];
uint32_t g_placed_count;    // pending ranges (read without the lock only as a hint)
uint32_t g_placed_overflow; // more ranges than fit: every export is forgotten
int g_placed_lock;

void placed_lock()
{
    while (__atomic_exchange_n(&g_placed_lock, 1, __ATOMIC_ACQUIRE) != 0) {
    }
}

void placed_unlock() { __atomic_store_n(&g_placed_lock, 0, __ATOMIC_RELEASE); }

// window and dumps
int g_window;
int g_window_closed;
uint32_t g_window_width, g_window_height;
// the swapchain's vsync (FIFO) as it was made, and port_config_revision then: the vsync setting and the frame rate cap
// apply live (Guide > Settings), so a present remakes the swapchain once they ask for the other present mode
uint32_t g_swapchain_fifo;
unsigned long g_swapchain_revision;
// HALO3_RES_SCALE: the internal resolution scale N (1..3). Every EDRAM surface (render targets, depth buffers, the back
// buffer) and every resolved texture is N x N host pixels per guest pixel, as Xenia's draw_resolution_scale scales its
// EDRAM and resolve targets. Guest coordinates (viewport, scissor, clear and resolve rectangles) are scaled on the way
// in, translated shaders get pixel_scale = 1/N so VPOS stays in guest pixels, resolves the CPU reads back are sampled
// down to guest size, and occlusion query counts are divided by N^2. Pack version 3 only (the DX11 shaders read host
// SV_Position for their screen-space lookups).
uint32_t g_res_scale = 1;
uint32_t g_back_buffer_width, g_back_buffer_height; // guest size of the back buffer
// The last draw's PA_SU_VTX_CNTL.PIX_CENTER was D3D zero (a half-pixel offset): with HALO3_RES_SCALE, a resolve then
// fills the host pixels the offset leaves uncovered at its left and top edges (xenia's
// resolve_resolution_scale_fill_half_pixel_offset).
bool g_half_pixel_offset;
const char *g_dump_directory;
int32_t g_present_resolve = -1;       // HALO3_PRESENT_RESOLVE
s_gpu_image *g_frame_resolves[64]; // this frame's Resolve destinations, in order
uint32_t g_frame_resolve_count;
uint8_t *g_last_frame;
uint32_t g_last_width, g_last_height;
uint32_t g_dump_resolves_first = 1, g_dump_resolves_last = 0; // HALO3_DUMP_RESOLVES
uint32_t g_dump_every = 1; // HALO3_DUMP_EVERY: with HALO3_DUMP_FRAMES, every Nth presented frame is written
// HALO3_TRACE_FRAME=N: one line per draw, clear and resolve of presented frame N on stderr (which pass wrote what).
uint32_t g_trace_frame = 0xffffffffu;
// HALO3_TEXTURE_RECHECK=1: hash each texture's source texels on every bind and report (and convert again) when the
// memory changed under an unchanged header, base and generation. Diagnostic only: it reads every bound texture.
bool g_texture_recheck = false;
bool tracing() { return g_presented == g_trace_frame; }
const char *pack_name(const s_shader_entry *entry)
{
    s_shader_pack_shader_info info;
    if (entry == 0)
        return "-";
    s_shader_pack_xenos xenos; // a version 3 pack names its translations (the shader tag paths)
    if (entry->xenos != SHADER_PACK_NOT_FOUND && shader_pack_get_xenos(g_pack, entry->xenos, &xenos) && xenos.name != 0)
        return xenos.name;
    return entry->shader_index != SHADER_PACK_NOT_FOUND && shader_pack_get_shader(g_pack, entry->shader_index, &info) ? info.name : "?";
}
void trace_target(char *text, size_t size, const char *label, const s_target_entry *t)
{
    const size_t n = strlen(text);
    if (t == 0)
        snprintf(text + n, size - n, " %s -", label);
    else if (g_res_scale > 1) // HALO3_RES_SCALE: /xN, the target's host pixels per guest pixel (target_res_scale)
        snprintf(text + n, size - n, " %s %p:%ux%u/f%x/%s/e%x/b%d/x%u", label, t->key, t->width, t->height,
                 t->d3d_format, gpu_format_name(t->host_format), t->edram_base, t->color_exp_bias, t->scale != 0 ? t->scale : g_res_scale);
    else
        snprintf(text + n, size - n, " %s %p:%ux%u/f%x/%s/e%x/b%d", label, t->key, t->width, t->height, t->d3d_format,
                 gpu_format_name(t->host_format),
                 t->edram_base, t->color_exp_bias);
    if (t != 0 && t->msaa != 0) { // a multisampled view: /m1 two samples, /m2 four (s_port_vk_surface::msaa_samples)
        const size_t m = strlen(text);
        snprintf(text + m, size - m, "/m%u", t->msaa);
    }
}
// One Resolve of a dumped frame: its pixels (buffer, a color resolve of an 8-bit-per-texel-block format) and what the
// frame manifest lists about it (every resolve, depth ones included).
struct s_resolve_dump {
    s_gpu_buffer *buffer; // 0: listed only (a depth resolve, a format the dump cannot read, nothing copied)
    uint32_t width, height, host_format, index;
    uint32_t sequence;    // the frame's resolves so far, depth ones included
    uint32_t draw_serial; // g_draw_serial at the resolve: the draws before it in the trace
    uint32_t flags, source_index, depth, linear_storage;
    const void *source_key;
    uint32_t source_edram_base, source_d3d_format, source_host_format, source_width, source_height;
    int32_t source_exp_bias, resolve_exp_bias;
    int32_t rect[4];
    const void *dest_key;
    uint32_t dest_d3d_format, dest_width, dest_height, dest_x, dest_y, dest_level, dest_slice;
};
s_resolve_dump g_resolve_dumps[64];
uint32_t g_resolve_dump_count;
uint32_t g_resolve_dump_index;    // the frame's color resolves so far (the rKK of the file names)
uint32_t g_resolve_sequence;      // the frame's resolves so far
const char *g_resolve_directory; // HALO3_DUMP_RESOLVES_DIR, else HALO3_DUMP_FRAMES

// ---- frame timing (HALO3_FRAME_STATS=N) ----
//
// Every N presented frames, one line on stderr: the wall time from one present to the next (average and its FPS,
// median, 99th percentile, worst), the part of it spent in the frame's submission, GPU wait and swapchain present
// (gpu_frame_end and gpu_swapchain_present), and the backend's synchronous one-off submissions over those frames
// (each a CPU stall on a fence: first-use uploads, readbacks). The counter is the SDK's own (the port's
// QueryPerformanceCounter, port/sdk/kernel/time_*.c).
uint32_t g_frame_stats_window; // N, 0: off
struct s_frame_stats {
    long long frequency;
    long long last_present;      // counter at the previous present's start, 0: none yet
    uint32_t count;              // intervals collected in this window
    uint32_t first_frame;        // g_presented at the window's first interval
    uint32_t immediate_base;     // the backend's immediate_submits at the window's start (s_gpu_stats)
    uint32_t deferred_base;      // its deferred_uploads
    double submit_ms;            // the window's time in frame end + present
    uint64_t converted_base[4];  // g_converted_bytes at the window's start
    uint32_t allocation_base;    // its memory_allocations
    uint32_t reuse_base;         // its buffers_reused
    double gpu_ms;               // the window's GPU frame times (timestamps; frames that had one)
    uint32_t gpu_frames;
    float *intervals;            // [g_frame_stats_window] milliseconds
};
s_frame_stats g_frame_stats;
// Vertex bytes converted on the CPU, by path: user-pointer streams, rewritten (dynamic) buffers, first uploads of
// static buffers, rectangle lists (HALO3_FRAME_STATS reports them per frame).
enum { k_converted_user_pointer, k_converted_dynamic, k_converted_static, k_converted_rectangles };
uint64_t g_converted_bytes[4];
// Pipelines the draw thread had to create before it could record a draw (a cache miss of pipeline_for, xe_pipeline
// or xe_compute_pipeline, module loading included): each is a stall of the frame being recorded (HALO3_FRAME_STATS
// reports them per window and for the whole run, with the seconds since the device started).
struct s_pipeline_stats {
    long long started;           // counter when the device started
    uint32_t count, over_2ms;    // the run's
    double total_ms, max_ms;
    uint32_t window_count;       // this window's
    double window_ms, window_max_ms;
};
s_pipeline_stats g_pipeline_stats;
const char *pipeline_prewarm_status(); // the background pipeline work so far, for the stats line
// HALO3_FRAME_TRACE=<ms> (with HALO3_FRAME_STATS): one stderr line per presented frame whose present-to-present
// interval is at least <ms> (0: every frame), with where the frame's time went: the device frame's CPU span (its
// first command to its present), gpu_frame_begin (the wait for the slot's fence and the destruction of what it
// retired), the GPU time of the last finished frame (timestamps), and this frame's pipeline creations, synchronous
// uploads, memory allocations, texture conversions, memexport copies made and draws, and the backend's binds (recorded,
// skipped as redundant), image barrier commands (and transitions that needed none), texture sets written and exports
// read back for a CPU lock of their memory.
struct s_frame_trace {
    bool on;
    double threshold;
    long long frame_begun;          // counter when begin_frame opened the frame
    double begin_ms;
    uint32_t pipelines, textures, exports, draws;
    double pipeline_ms;
    double texture_ms, texture_convert_ms, texture_upload_ms; // conversions: all, source extent + convert, upload
    s_gpu_stats base;               // the backend's counters when the frame began (submits, allocations, binds)
    uint32_t texture_sets;          // texture descriptor sets written (xe_texture_set: made, not found cached)
    uint32_t readbacks;             // exports read back for a CPU lock of their memory (xe_buffer_lock, xe_export_lock)
};
s_frame_trace g_frame_trace;

long long frame_stats_now()
{
    long long count = 0;
    QueryPerformanceCounter(&count);
    return count;
}

double frame_stats_ms(long long from, long long to)
{
    return g_frame_stats.frequency > 0 ? (double)(to - from) * 1000.0 / (double)g_frame_stats.frequency : 0.0;
}

// Same-frame recording. Only the HAL device owner publishes to the ring and allocates
// from the slab. Other SDK threads rendezvous with the worker through a separate mailbox.
constexpr uint32_t k_record_slots = 8192;
constexpr size_t k_record_slab_size = 16u << 20;
template<class T> struct s_record_optional {
    T value;
    bool present;
    const T *get() const { return present ? &value : nullptr; }
};
template<class T> s_record_optional<T> record_optional(const T *p)
{
    return { p ? *p : T{}, p != nullptr };
}
template<class T> struct s_record_quad {
    T values[4];
    bool present;
    const T *get() const { return present ? values : nullptr; }
};
template<class T> s_record_quad<T> record_quad(const T *p)
{
    s_record_quad<T> copy = {};
    copy.present = p != nullptr;
    if (p) memcpy(copy.values, p, sizeof(copy.values));
    return copy;
}
struct s_record_range {
    const void *base;
    uint32_t size;

    bool overlaps(const s_record_range &other) const
    {
        const uintptr_t a = (uintptr_t)base, b = (uintptr_t)other.base;
        return base && other.base && size && other.size && (a <= b ? b - a < size : a - b < other.size);
    }
};
struct s_record_command {
    void (*run)(const void *);
    const void *data;
    long long published_at;
    const s_record_range *reads = nullptr;
    uint32_t read_count = 0;
    uint32_t texture_count = 0;
    const s_record_range *texture_reads = nullptr;
};
struct s_record_request {
    s_record_command command;
    uint64_t after;
    s_record_request *next;
    bool done;
    bool asynchronous = false;
};
struct s_record_queue {
    alignas(64) uint64_t published;
    alignas(64) uint64_t consumed;
    alignas(64) uint32_t sleeping, waiters, read_scanners;
    uint32_t owner, enabled;
    bool stopping, servicing_request, trace;
    s_port_gpu_monitor *monitor;
    void *thread;
    s_record_request *first, *last;
    uint32_t requests;
    uint8_t *slab;
    uint8_t *slabs[2];
    uint32_t slab_index;
    size_t used;
    const s_record_range *texture_reads; // immutable in this slab until a texture binding changes
    uint32_t texture_count;
    uint64_t prev_present_seq;
    struct { s_record_command cmd; alignas(16) uint8_t capture[192]; } slots[k_record_slots];
};
s_record_queue g_record;
thread_local bool g_record_worker, g_record_reading;
// Mailbox callers may still be leaving the condition variable when shutdown joins
// the worker. Close admission before teardown, then retire those callers as well.
constexpr uint32_t k_record_closing = 1u << 31;
uint32_t g_record_clients = k_record_closing;
struct s_record_client {
    bool admitted;
    s_record_client() : admitted(!(__atomic_fetch_add(&g_record_clients, 1u, __ATOMIC_ACQ_REL) & k_record_closing)) {}
    ~s_record_client() { __atomic_fetch_sub(&g_record_clients, 1u, __ATOMIC_RELEASE); }
};
s_port_vk_shadows g_live_shadows;
const s_port_vk_draw *g_record_source; // producer's delta baseline

bool record_remote()
{
    return __atomic_load_n(&g_record.enabled, __ATOMIC_ACQUIRE) && !g_record_worker && !g_record_reading;
}

bool record_owner()
{
    return (uint32_t)GetCurrentThreadId() == __atomic_load_n(&g_record.owner, __ATOMIC_ACQUIRE);
}

uint64_t record_head() { return __atomic_load_n(&g_record.published, __ATOMIC_ACQUIRE); }
uint64_t record_tail() { return __atomic_load_n(&g_record.consumed, __ATOMIC_ACQUIRE); }

void record_notify()
{
    port_gpu_monitor_lock(g_record.monitor);
    port_gpu_monitor_broadcast(g_record.monitor);
    port_gpu_monitor_unlock(g_record.monitor);
}

void record_trace(const char *kind, uint64_t seq, long long publish, long long start, long long end)
{
    char line[160];
    snprintf(line, sizeof(line), "vk-record %s %llu %lld %lld %lld\n", kind,
             (unsigned long long)seq, publish, start, end);
    port_write_error(line);
}

void record_run(const s_record_command &cmd, uint64_t seq)
{
    const long long start = g_record.trace ? frame_stats_now() : 0;
    cmd.run(cmd.data);
    if (g_record.trace)
        record_trace("packet", seq, cmd.published_at, start, frame_stats_now());
}

void record_worker_main(void *)
{
    g_record_worker = true;
    __atomic_store_n(&g_frame_thread, (uint32_t)GetCurrentThreadId(), __ATOMIC_RELEASE);
    uint64_t tail = record_tail();
    unsigned spins = 0;
    for (;;) {
        if (__atomic_load_n(&g_record.requests, __ATOMIC_ACQUIRE)) {
            port_gpu_monitor_lock(g_record.monitor);
            s_record_request *r = g_record.first;
            if (r && r->after <= tail) {
                g_record.servicing_request = true;
                g_record.first = r->next;
                if (!r->next) g_record.last = nullptr;
                __atomic_store_n(&g_record.requests, g_record.first != nullptr, __ATOMIC_RELEASE);
                port_gpu_monitor_unlock(g_record.monitor);
                record_run(r->command, tail);
                port_gpu_monitor_lock(g_record.monitor);
                g_record.servicing_request = false;
                r->done = true;
                port_gpu_monitor_broadcast(g_record.monitor);
                port_gpu_monitor_unlock(g_record.monitor);
                continue;
            }
            port_gpu_monitor_unlock(g_record.monitor);
        }
        if (tail != record_head()) {
            record_run(g_record.slots[tail % k_record_slots].cmd, tail + 1);
            // Pair each idle/wait flag with a second sequence check under the monitor.
            __atomic_store_n(&g_record.consumed, ++tail, __ATOMIC_SEQ_CST);
            if (__atomic_load_n(&g_record.waiters, __ATOMIC_SEQ_CST)) record_notify();
            spins = 60000;
            continue;
        }
        if (spins) {
            --spins;
            __builtin_ia32_pause();
            continue;
        }
        port_gpu_monitor_lock(g_record.monitor);
        __atomic_store_n(&g_record.sleeping, 1, __ATOMIC_SEQ_CST);
        while (tail == __atomic_load_n(&g_record.published, __ATOMIC_SEQ_CST) &&
               !g_record.first && !g_record.stopping)
            port_gpu_monitor_wait(g_record.monitor);
        __atomic_store_n(&g_record.sleeping, 0, __ATOMIC_RELAXED);
        const bool stop = g_record.stopping && tail == record_head() && !g_record.first;
        port_gpu_monitor_unlock(g_record.monitor);
        if (stop) break;
    }
    g_record_worker = false;
}

void record_wait(uint64_t seq, const char *reason)
{
    if (record_tail() >= seq) return;
    const long long start = g_record.trace ? frame_stats_now() : 0;
    for (unsigned i = 0; i < 16000 && record_tail() < seq; ++i) __builtin_ia32_pause();
    if (record_tail() < seq) {
        port_gpu_monitor_lock(g_record.monitor);
        __atomic_add_fetch(&g_record.waiters, 1u, __ATOMIC_SEQ_CST);
        while (__atomic_load_n(&g_record.consumed, __ATOMIC_SEQ_CST) < seq)
            port_gpu_monitor_wait(g_record.monitor);
        __atomic_sub_fetch(&g_record.waiters, 1u, __ATOMIC_RELAXED);
        port_gpu_monitor_unlock(g_record.monitor);
    }
    if (g_record.trace) record_trace(reason, seq, 0, start, frame_stats_now());
}

struct s_record_resource_guard {
    s_record_resource_guard() { placed_lock(); }
    ~s_record_resource_guard() { placed_unlock(); }
};


uint64_t record_publish(s_record_command cmd, size_t capture_size = 0)
{
    const uint64_t head = record_head();
    if (head - record_tail() == k_record_slots)
        record_wait(head - k_record_slots + 1, "full");
    if (g_record.trace) cmd.published_at = frame_stats_now();
    auto publish_slot = [&] {
        auto &slot = g_record.slots[head % k_record_slots];
        if (capture_size)
            cmd.data = memcpy(slot.capture, cmd.data, capture_size);
        slot.cmd = cmd;
        __atomic_store_n(&g_record.published, head + 1, __ATOMIC_SEQ_CST);
    };
    if (__builtin_expect(__atomic_load_n(&g_record.read_scanners, __ATOMIC_ACQUIRE) != 0, 0)) {
        s_record_resource_guard guard;
        publish_slot();
    } else {
        publish_slot();
    }
    if (__atomic_load_n(&g_record.sleeping, __ATOMIC_SEQ_CST)) record_notify();
    return head + 1;
}

// Oversize payloads borrow their caller's storage through a synchronous worker call.
uint8_t *record_reserve(size_t bytes)
{
    if (bytes > k_record_slab_size) return nullptr;
    size_t at = (g_record.used + 15) & ~size_t(15);
    if (bytes > k_record_slab_size - at) {
        record_wait(record_head(), "slab");
        s_record_resource_guard guard; // retire any resource scan of the old slab before reuse
        at = 0;
        g_record.texture_reads = nullptr;
    }
    g_record.used = at + bytes;
    return g_record.slab + at;
}

template<class F> s_record_command record_command(const F &f)
{
    return { [](const void *p) { (*static_cast<const F *>(p))(); }, &f, 0 };
}

template<class F> void record_post(const F &f)
{
    static_assert(__is_trivially_copyable(F), "recorded closures must own trivial values");
    static_assert(sizeof(F) <= sizeof(g_record.slots[0].capture) && alignof(F) <= 16, "packet exceeds slot");
    record_publish(record_command(f), sizeof(F));
}

// The caller holds the monitor. Ready polls can run at the next packet boundary.
void record_enqueue_request(s_record_request *r)
{
    if (g_record.trace) r->command.published_at = frame_stats_now();
    if (r->asynchronous && g_record.first && r->after < g_record.first->after) {
        r->next = g_record.first;
        g_record.first = r;
    } else {
        if (g_record.last) g_record.last->next = r;
        else g_record.first = r;
        g_record.last = r;
    }
    __atomic_store_n(&g_record.requests, 1u, __ATOMIC_RELEASE);
    port_gpu_monitor_broadcast(g_record.monitor);
}

template<class F> void record_sync(const F &f)
{
    auto cmd = record_command(f);
    if (record_owner()) return record_wait(record_publish(cmd), "drain");
    s_record_client client;
    if (!client.admitted) return;
    port_gpu_monitor_lock(g_record.monitor);
    s_record_request r = {cmd, record_head(), nullptr, false};
    record_enqueue_request(&r);
    while (!r.done) port_gpu_monitor_wait(g_record.monitor);
    port_gpu_monitor_unlock(g_record.monitor);
}

// Foreign SDK threads use the mailbox. Only the HAL owner can publish ring slots.
template<class F> void record_dispatch(const F &f)
{
    if (record_owner()) record_post(f);
    else record_sync(f);
}

// Copy variable-length arguments before the caller can reuse them.
template<class T, class F> void record_array(const T *source, size_t count, const F &run)
{
    if (!record_owner() || (source && count > k_record_slab_size / sizeof(T)))
        return record_sync([&] { run(source); });
    const size_t bytes = count * sizeof(T);
    const auto *copy = source && bytes ? (const T *)memcpy(record_reserve(bytes), source, bytes) : nullptr;
    record_post([=] { run(copy); });
}

template<class F> auto record_result(const F &f) -> decltype(f())
{
    decltype(f()) result{};
    record_sync([&] { result = f(); });
    return result;
}

// Shell preflight may inspect hundreds of shaders consecutively. Once the owner's
// ring is drained it can read the immutable bindings directly. Against mailbox
// mutations the monitor is held until the read completes.
template<class F> auto record_read(const F &f) -> decltype(f())
{
    if (!record_owner()) return record_result(f);
    record_wait(record_head(), "read");
    port_gpu_monitor_lock(g_record.monitor);
    while (g_record.first || g_record.servicing_request)
        port_gpu_monitor_wait(g_record.monitor);
    g_record_reading = true;
    const auto value = f();
    g_record_reading = false;
    port_gpu_monitor_unlock(g_record.monitor);
    return value;
}

// Declarations are compared by pointer in the renderer. Intern their contents so a
// transient declaration has both immutable bytes and a stable cache identity.
struct s_record_decl {
    s_record_decl *next;
    size_t bytes;
    s_xenos_d3d_vertex_element elements[XENOS_VERTEX_MAX_ELEMENTS + 1];
};
s_record_decl *g_record_decls[128];

const void *record_declaration(const void *pointer)
{
    if (!pointer) return nullptr;
    const auto *elements = (const s_xenos_d3d_vertex_element *)pointer;
    unsigned n = 0;
    while (n < XENOS_VERTEX_MAX_ELEMENTS && elements[n].stream != 0xff) ++n;
    const size_t bytes = (n + 1) * sizeof(*elements);
    auto &bucket = g_record_decls[gpu_hash_bytes(2166136261u, elements, bytes) % 128];
    for (auto *d = bucket; d; d = d->next)
        if (d->bytes == bytes && !memcmp(d->elements, elements, bytes)) return d->elements;
    auto *d = (s_record_decl *)malloc(sizeof(s_record_decl));
    if (!d) return nullptr;
    d->next = bucket;
    d->bytes = bytes;
    memcpy(d->elements, elements, bytes);
    bucket = d;
    return d->elements;
}

void xe_invalidate_draw_caches();

struct s_record_bank {
    float floats[256 * 4];
    uint32_t chunks[64];
};
// Changes since the preceding published draw packet. A cache may use a mask
// only if its generation matches that baseline (a draw can return before binding).
struct s_record_changes {
    uint32_t float_gen[2], sampler_gen, stream_gen, flow_gen;
    uint64_t chunks[2];
    uint32_t sampler_slots, stream_slots, flow_rows;
};
// Isolate the decoded packet's hot generation counters from the producer's baseline.
struct alignas(64) {
    s_port_vk_draw draw;
    alignas(64) s_record_bank banks[2];
    // Contiguous XDK Flow layout, with 16 zero registers for legacy full 5-bit pixel loop indices.
    uint32_t flow[8 + 32 + 16];
} g_decoded;
uint32_t g_record_revision;

struct alignas(64) s_record_seen {
    uint32_t float_gen[2], flow_gen, stream_gen;
    uint32_t revision, published_stream_gen, sampler_gen, stream_mask;
    uint8_t flow_sources;
    const uint32_t *chunk_source[2];
    const float *float_source[2];
    float clip_planes[6][4];
    s_port_vk_stream streams[PORT_VK_MAX_STREAMS];
    uint32_t mapped_streams, bools[2][4], ints[2][16];
};
s_record_seen g_record_seen;

struct s_record_draw_args {
    uint32_t primitive, start, count;
    int base;
    uint32_t indexed, tess_mode;
    float minimum, maximum;
    bool tessellated;
};
struct s_record_delta {
    s_record_draw_args args;
    uint64_t dirty;
    s_record_changes changes;
    uint32_t streams;
    uint32_t slice_mask;
    uint8_t float_sources, chunk_sources, flow_sources, changed_flow, uniform_stamps;
    bool flow, reseed, clip_planes;
    const s_port_vk_geometry *geometry;
    struct s_slice { const uint8_t *bytes; uint32_t first, size; } *slices;
};

constexpr unsigned k_index_slice = PORT_VK_MAX_STREAMS;
thread_local const s_record_delta *g_record_packet; // valid only while executing this draw

const s_record_changes *record_changes_for(const s_port_vk_draw *draw)
{
    return draw == &g_decoded.draw && g_record_packet ? &g_record_packet->changes : nullptr;
}

// CPU reads use the packet copy. Export lookups and stream cache identities keep
// using the guest address, even when a pending export first becomes live on this draw.
const uint8_t *vertex_bytes(const s_port_vk_stream &stream, unsigned slot, size_t offset)
{
    if (g_record_packet && (g_record_packet->slice_mask & (1u << slot)) && &stream == &g_decoded.draw.streams[slot]) {
        const auto &slice = g_record_packet->slices[slot];
        return slice.bytes + (offset - slice.first);
    }
    return (const uint8_t *)stream.data + offset;
}
const uint8_t *index_bytes(const s_port_vk_indices &indices, size_t offset)
{
    if (g_record_packet && (g_record_packet->slice_mask & (1u << k_index_slice)) && &indices == &g_decoded.draw.indices) {
        const auto &slice = g_record_packet->slices[k_index_slice];
        return slice.bytes + (offset - slice.first);
    }
    return (const uint8_t *)indices.data + offset;
}

// A registered ordinary Xenos VS uses xe_stream's vertex range. Compute, legacy
// shaders and cache misses can fetch indirectly and need the whole bound stream.
struct s_record_vs_hint { const void *shader; unsigned pending; };
s_record_vs_hint g_record_linear_vs[4096];
auto &record_linear_vs_slot(const void *shader)
{
    return g_record_linear_vs[mix32(key_of(shader)) % 4096];
}

void record_texture_ranges(const s_port_d3d_texture_source &source, s_record_range ranges[2])
{
    struct s_tex_range_cache {
        s_xenos_texture_desc desc;
        uint32_t base_size, mip_size;
        uint8_t valid;
    };
    static thread_local s_tex_range_cache cache[256];
    const uint32_t *w = (const uint32_t *)&source.desc;
    const uint32_t h = (w[0] ^ (w[1] * 2654435761u) ^ (w[2] * 2246822519u) ^ (w[3] * 3266489917u));
    auto &e = cache[(h ^ (h >> 8) ^ (h >> 16)) & 255u];
    if (!e.valid || memcmp(&e.desc, &source.desc, sizeof(e.desc)) != 0) {
        s_xenos_texture_layout layout;
        const bool known = xenos_texture_get_layout(&source.desc, &layout) != 0;
        e.desc = source.desc;
        e.base_size = known ? layout.base_size : UINT32_MAX;
        e.mip_size = known ? layout.mip_size : UINT32_MAX;
        e.valid = 1;
    }
    ranges[0] = {source.base, e.base_size};
    ranges[1] = {source.mip, e.mip_size};
}

// A BeginExport can still be in the ring when the CPU asks for the memory.
// Remember its exact range at publication. Resolve aliases use their header or base.
struct s_record_target {
    s_record_range range;
    const void *key; // null for an export
    uint32_t pending, live_size;
    bool live, held;
};
t_list<s_record_target> g_record_targets;
s_map g_record_written_buffers;
s_record_range g_record_open_export;
bool g_record_targets_incomplete;
uint32_t g_record_invalidations, g_record_borrowers;

// The caller holds the resource lock. Indices stay stable while a command is pending.
s_record_target *record_target(s_record_range range, const void *key)
{
    s_record_target *vacant = nullptr;
    for (uint32_t i = 0; i < g_record_targets.count; ++i) {
        auto &t = g_record_targets.items[i];
        if (t.range.base == range.base && t.key == key) {
            if (range.size > t.range.size) t.range.size = range.size;
            return &t;
        }
        if (!t.pending && !t.live) vacant = &t;
    }
    auto *t = vacant ? vacant : g_record_targets.push();
    if (t) *t = {range, key, 0, 0, false, false};
    else g_record_targets_incomplete = true;
    return t;
}

template<class F> void record_target_command(s_record_range range, const void *key, const F &run)
{
    if (!range.base && !key) return record_dispatch(run);
    uint32_t index = UINT32_MAX;
    {
        s_record_resource_guard guard;
        if (auto *t = record_target(range, key)) {
            ++t->pending;
            index = uint32_t(t - g_record_targets.items);
        }
    }
    record_dispatch([=] {
        run();
        s_record_resource_guard guard;
        g_record_open_export = {g_export.base, g_export.size};
        if (index != UINT32_MAX) {
            auto &t = g_record_targets.items[index];
            if (!--t.pending) t.range.size = t.live_size;
        }
    });
}

bool record_gpu_write(s_record_range range, const void *key)
{
    s_record_resource_guard guard;
    if (g_record_targets_incomplete || range.overlaps(g_record_open_export)) return true;
    for (uint32_t i = 0; i < g_record_targets.count; ++i) {
        const auto &t = g_record_targets.items[i];
        if (!t.live && !t.pending) continue;
        if (t.key) {
            if (key && (key == t.key || (range.base && range.base == t.range.base))) return true;
        } else if (range.overlaps(t.range)) {
            // An existing CPU-owned export can become GPU-owned again through
            // an earlier export's stream constant, even without a new BeginExport.
            if (t.pending || t.held || record_head() != record_tail() ||
                __atomic_load_n(&g_record_borrowers, __ATOMIC_ACQUIRE)) return true;
        }
    }
    return false;
}

void record_resolved(const void *key, const void *base, bool live)
{
    if (!g_record_worker || !key) return;
    s_record_resource_guard guard;
    for (uint32_t i = 0; i < g_record_targets.count; ++i) {
        auto &t = g_record_targets.items[i];
        if (t.key == key) {
            t.live = live && t.range.base == base;
            t.live_size = t.live ? 1 : 0;
        }
    }
}

// Suppress copy-elision while a queued operation can change which copy holds the bytes.
void record_drop_export_hints()
{
    s_record_resource_guard guard;
    ++g_record_invalidations;
}

void record_finish_export_invalidation()
{
    s_record_resource_guard guard;
    --g_record_invalidations;
}

void record_export_held(s_record_range range, bool held, bool live = true)
{
    if (!g_record_worker) return;
    s_record_resource_guard guard;
    if (auto *t = record_target(range, nullptr)) {
        t->live = live;
        t->held = held;
        t->live_size = live ? range.size : 0;
        if (!t->pending) t->range.size = t->live_size;
    }
}

bool record_held(const void *base)
{
    if (g_record_invalidations || g_placed_count || g_placed_overflow || g_record_targets_incomplete) return false;
    bool held = false;
    for (uint32_t i = 0; i < g_record_targets.count; ++i) {
        const auto &t = g_record_targets.items[i];
        const s_record_range live = {t.range.base, t.live_size};
        if (t.key || !t.live || !live.overlaps({base, 1})) continue;
        if (!t.held) return false; // an overlapping CPU-owned copy can shadow this one
        held = true;
    }
    return held;
}

s_record_range g_record_textures[PORT_VK_SAMPLER_COUNT][2];
s_xenos_texture_desc g_record_texture_desc[PORT_VK_SAMPLER_COUNT];
uint32_t g_record_bound_textures;

void record_wait_borrowers()
{
    if (!__atomic_load_n(&g_record_borrowers, __ATOMIC_ACQUIRE)) return;
    port_gpu_monitor_lock(g_record.monitor);
    while (__atomic_load_n(&g_record_borrowers, __ATOMIC_ACQUIRE))
        port_gpu_monitor_wait(g_record.monitor);
    port_gpu_monitor_unlock(g_record.monitor);
}

// Readers only inspect published packets. Publication and slab reclamation use
// the same lock so a foreign Lock can walk those packets while the owner records.
void record_wait_reads(const s_record_range &range, const char *reason = "resource")
{
    if (!range.base || !range.size) return;
    record_wait_borrowers();
    if (record_head() == record_tail()) return;
    uint64_t dependency = 0;
    __atomic_fetch_add(&g_record.read_scanners, 1u, __ATOMIC_SEQ_CST);
    {
        s_record_resource_guard guard;
        const uint64_t tail = record_tail();
        for (uint64_t seq = record_head(); seq > tail && !dependency; --seq) {
            const auto &cmd = g_record.slots[(seq - 1) % k_record_slots].cmd;
            for (unsigned i = 0; i < cmd.read_count; ++i)
                if (range.overlaps(cmd.reads[i])) { dependency = seq; break; }
            for (unsigned i = 0; !dependency && i < cmd.texture_count; ++i)
                if (range.overlaps(cmd.texture_reads[i])) { dependency = seq; break; }
        }
    }
    __atomic_fetch_sub(&g_record.read_scanners, 1u, __ATOMIC_RELEASE);
    if (dependency) record_wait(dependency, reason);
}

struct s_delta_writer {
    uint8_t *p;
    void bytes(const void *src, size_t n) { memcpy(p, src, n); p += n; }
    void stamps(const uint32_t *src, unsigned count, uint32_t uniform)
    {
        if (!uniform) bytes(src, count * sizeof(*src));
    }
};
struct s_delta_reader {
    const uint8_t *p;
    void bytes(void *dst, size_t n) { memcpy(dst, p, n); p += n; }
    void stamps(uint32_t *dst, unsigned count, uint32_t uniform)
    {
        if (uniform) {
            for (unsigned i = 0; i < count; ++i) dst[i] = uniform;
        } else bytes(dst, count * sizeof(*dst));
    }
};

using t_chunk_stamps = uint32_t __attribute__((vector_size(16)));
inline t_chunk_stamps load_chunk_stamps(const uint32_t *p)
{
    t_chunk_stamps v;
    __builtin_memcpy(&v, p, sizeof(v));
    return v;
}

template<class Predicate>
inline uint64_t chunk_stamp_mask(const Predicate &changed)
{
    using t_i16 = int16_t __attribute__((vector_size(16)));
    using t_i8 = char __attribute__((vector_size(16)));
    uint64_t mask = 0;
    for (unsigned i = 0; i < 64; i += 16) {
        const t_i16 lo = __builtin_ia32_packssdw128(changed(i), changed(i + 4));
        const t_i16 hi = __builtin_ia32_packssdw128(changed(i + 8), changed(i + 12));
        const t_i8 bytes = (t_i8)__builtin_ia32_packsswb128(lo, hi);
        mask |= uint64_t(uint32_t(__builtin_ia32_pmovmskb128(bytes))) << i;
    }
    return mask;
}

// Each changed chunk carries the bank generation of its last write. A reseed,
// an unversioned bank or a wrap needs the complete bank.
inline uint64_t changed_float_chunks(const uint32_t *stamps, uint32_t previous, uint32_t current)
{
    if (!stamps || !previous || current < previous)
        return UINT64_MAX;
    return chunk_stamp_mask([&](unsigned i) {
        return (load_chunk_stamps(stamps + i) - previous - 1u) <= (current - previous - 1u);
    });
}

// Both directions walk precisely the same fields. Sections are byte-packed (including
// pointers), so the worker never dereferences a potentially unaligned packet member.
template<class IO, class Draw>
void record_draw_fields(IO &io, Draw &d, const s_record_delta &delta)
{
    const uint64_t bits = delta.dirty;
    io.bytes(&d.state_gen, 5 * sizeof(uint32_t));
    io.bytes(&d.samplers_any_gen, 2 * sizeof(uint32_t));
    if (!(bits & ~(PORT_VK_DIRTY_VS_CONST | PORT_VK_DIRTY_PS_CONST | PORT_VK_DIRTY_RT | PORT_VK_DIRTY_VDECL)))
        return;
    auto &rs = d.state;
    if (bits & PORT_VK_DIRTY_VS) io.bytes(&d.vertex_shader, sizeof(d.vertex_shader));
    if (bits & PORT_VK_DIRTY_PS) io.bytes(&d.pixel_shader, sizeof(d.pixel_shader));
    if (bits & PORT_VK_DIRTY_BLEND) {
        io.bytes(rs.blend_control, sizeof(rs.blend_control));
        io.bytes(rs.color_write_enable, sizeof(rs.color_write_enable));
        io.bytes(&rs.blend_factor, sizeof(rs.blend_factor));
    }
    if (bits & PORT_VK_DIRTY_DS) {
        io.bytes(&rs.z_enable, 3 * sizeof(uint32_t));
        io.bytes(&rs.stencil_enable, offsetof(s_port_vk_render_state, depth_bias) -
                                     offsetof(s_port_vk_render_state, stencil_enable));
    }
    if (bits & PORT_VK_DIRTY_RAST) {
        io.bytes(&rs.cull_mode, 2 * sizeof(uint32_t));
        io.bytes(&rs.depth_bias, offsetof(s_port_vk_render_state, blend_factor) -
                                 offsetof(s_port_vk_render_state, depth_bias));
        io.bytes(&d.clip_plane_enable, sizeof(d.clip_plane_enable));
        if (delta.clip_planes)
            io.bytes(d.clip_planes, sizeof(d.clip_planes));
        io.bytes(&d.point_size_min, 2 * sizeof(float));
    }
    if (bits & PORT_VK_DIRTY_VP) {
        io.bytes(d.viewport, sizeof(d.viewport) + sizeof(d.scissor));
        io.bytes(&rs.scissor_test_enable, sizeof(rs.scissor_test_enable));
        io.bytes(d.fragment_offset, sizeof(d.fragment_offset) + sizeof(d.pixel_center_offset));
    }
    if (bits & PORT_VK_DIRTY_STREAMS) {
        io.bytes(&d.stream_mask, sizeof(d.stream_mask));
        for (uint32_t m = delta.streams; m; m &= m - 1)
            io.bytes(&d.streams[__builtin_ctz(m)], sizeof(s_port_vk_stream));
    }
    if (bits & PORT_VK_DIRTY_IB) io.bytes(&d.indices, sizeof(d.indices));
    constexpr uint32_t slots = (1u << PORT_VK_SAMPLER_COUNT) - 1;
    const uint32_t textures = (bits >> 12) & slots, samplers = bits >> 38;
    for (uint32_t m = textures | samplers; m; m &= m - 1) {
        const unsigned slot = __builtin_ctz(m);
        auto &t = d.samplers[slot];
        const uint32_t bit = 1u << slot;
        if (textures & bit) {
            io.bytes(&t.texture, sizeof(t.texture));
            io.bytes(&t.bound, sizeof(t.bound));
        }
        if (samplers & bit)
            io.bytes(&t.state, sizeof(t.state));
        io.bytes(&d.sampler_gen[slot], sizeof(d.sampler_gen[slot]));
    }
}

// One stamp span followed by one float span per run. Full-bank seeds take two
// copies, while an isolated dirty chunk retains its fixed-size inline copies.
template<class IO, class Stamps, class Floats>
void record_chunk_runs(IO &io, uint64_t mask, Stamps *stamps, Floats *floats, uint32_t uniform_stamp)
{
    while (mask) {
        const unsigned first = __builtin_ctzll(mask);
        const uint64_t run = mask >> first;
        if (!(run & 2u)) {
            if (stamps) io.stamps(stamps + first, 1, uniform_stamp);
            if (floats) io.bytes(floats + first * 16u, 64);
            mask &= mask - 1u;
        } else {
            const uint64_t rest = (run + 1u) & run;
            const unsigned count = run == UINT64_MAX ? 64 : __builtin_ctzll(~run);
            if (stamps) io.stamps(stamps + first, count, uniform_stamp);
            if (floats) {
                if (count == 2) io.bytes(floats + first * 16u, 128);
                else io.bytes(floats + first * 16u, count * 64u);
            }
            mask = rest << first;
        }
    }
}

void execute_record_draw(const s_port_vk_draw *draw, uint64_t dirty, const s_record_draw_args &a,
                         const s_port_vk_geometry *geometry)
{
    if (a.tessellated)
        port_vk_draw_tessellated(draw, dirty, a.primitive, a.base, a.start, a.count, a.tess_mode, a.minimum, a.maximum);
    else
        port_vk_draw(draw, dirty, a.primitive, a.start, a.count, a.base, a.indexed, geometry);
}

void apply_record_delta(const s_record_delta &delta)
{
    auto &draw = g_decoded.draw;
    s_delta_reader in = { (const uint8_t *)(&delta + 1) };
    if (delta.reseed) xe_invalidate_draw_caches();
    if (delta.dirty & PORT_VK_DIRTY_VDECL) in.bytes(&draw.declaration, sizeof(draw.declaration));
    record_draw_fields(in, draw, delta);
    if (!(delta.changes.chunks[0] | delta.changes.chunks[1]) && !delta.flow)
        return;
    for (unsigned s = 0; s < 2; ++s) {
        if (delta.changes.chunks[s]) {
            // A change of either source always seeds all chunks of this bank.
            auto &bank = g_decoded.banks[s];
            auto *floats = delta.float_sources & (1u << s) ? bank.floats : nullptr;
            auto *stamps = delta.chunk_sources & (1u << s) ? bank.chunks : nullptr;
            (s ? g_shadows.pixel_float : g_shadows.vertex_float) = floats;
            (s ? draw.ps_chunk_gen : draw.vs_chunk_gen) = stamps;
            record_chunk_runs(in, delta.changes.chunks[s], stamps, floats,
                              delta.uniform_stamps & (1u << s) ? (s ? draw.ps_float_gen : draw.vs_float_gen) : 0u);
        }
        if (delta.flow) {
            const bool have_bools = delta.flow_sources & (1u << s);
            const bool have_ints = delta.flow_sources & (4u << s);
            auto &bools = s ? g_shadows.pixel_bool : g_shadows.vertex_bool;
            auto &ints = s ? g_shadows.pixel_int : g_shadows.vertex_int;
            bools = have_bools ? g_decoded.flow + 4 * s : nullptr;
            ints = have_ints ? g_decoded.flow + 8 + 16 * s : nullptr;
            if (delta.changed_flow & (1u << s)) in.bytes(g_decoded.flow + 4 * s, 4 * sizeof(uint32_t));
            if (delta.changed_flow & (4u << s)) in.bytes(g_decoded.flow + 8 + 16 * s, 16 * sizeof(uint32_t));
        }
    }
}

// Exceptionally large UPs and calls from outside the current HAL owner borrow the
// live state only while the caller waits. The next delta starts a new baseline.
void record_borrow_draw(const s_port_vk_draw *draw, uint64_t dirty, const s_record_draw_args &args,
                        const s_port_vk_geometry *geometry)
{
    s_record_client client;
    if (!client.admitted) return;
    __atomic_add_fetch(&g_record_borrowers, 1u, __ATOMIC_RELEASE);
    record_sync([&] {
        const auto saved = g_shadows;
        g_shadows = g_live_shadows;
        xe_invalidate_draw_caches();
        execute_record_draw(draw, dirty, args, geometry);
        g_shadows = saved;
        xe_invalidate_draw_caches();
    });
    if (__atomic_sub_fetch(&g_record_borrowers, 1u, __ATOMIC_ACQ_REL) == 0)
        record_notify();
}

void record_draw(const s_port_vk_draw *draw, uint64_t dirty, const s_record_draw_args &args,
                 const s_port_vk_geometry *geometry)
{
    if (!record_owner()) {
        record_borrow_draw(draw, dirty, args, geometry);
        __atomic_add_fetch(&g_record_revision, 1u, __ATOMIC_RELEASE);
        return;
    }
    // Reserve a worst-case delta, then commit only the bytes actually serialized.
    // All pieces of this draw must share one reservation so rollover cannot split it.
    const size_t vmax = geometry && geometry->stream0.data ? geometry->stream0.size : 0;
    const size_t imax = geometry && geometry->indices && geometry->indices->data ? geometry->indices->size : 0;
    const uint32_t revision = __atomic_load_n(&g_record_revision, __ATOMIC_ACQUIRE);
    const bool reset = g_record_source != draw || revision != g_record_seen.revision ||
                       draw->vs_float_gen < g_record_seen.float_gen[0] || draw->ps_float_gen < g_record_seen.float_gen[1] ||
                       draw->bool_loop_gen < g_record_seen.flow_gen;
    const bool seed = reset || dirty == UINT64_MAX;
    g_record_seen.revision = revision;
    if (seed) dirty = UINT64_MAX;
    const void *decl = (dirty & PORT_VK_DIRTY_VDECL) ? record_declaration(draw->declaration) : nullptr;
    const void *override_decl = geometry ? record_declaration(geometry->declaration) : nullptr;
    auto &seen = g_record_seen;
    uint32_t changed_streams = 0;
    if ((dirty & PORT_VK_DIRTY_STREAMS) && (seed || !draw->stream_gen || seen.stream_gen != draw->stream_gen)) {
        seen.mapped_streams = 0;
        for (unsigned i = 0; i < PORT_VK_MAX_STREAMS; ++i) {
            const auto &stream = draw->streams[i];
            if (stream.data && !stream.key) seen.mapped_streams |= 1u << i;
            if (seed || memcmp(&stream, &seen.streams[i], sizeof(stream))) {
                seen.streams[i] = stream;
                changed_streams |= 1u << i;
            }
        }
        seen.stream_gen = draw->stream_gen;
    }
    const uint32_t bound = geometry ? geometry->stream_mask & ~1u : draw->stream_mask;
    uint32_t mapped = bound & seen.mapped_streams;
    const bool indexed = args.indexed && !(geometry && geometry->indices);
    bool mapped_ib = indexed && draw->indices.data && !draw->indices.key;
    if (g_xenos && (mapped || (mapped_ib && args.tessellated))) {
        s_record_resource_guard guard;
        for (uint32_t m = mapped; m; m &= m - 1) {
            const unsigned i = __builtin_ctz(m);
            if (record_held(draw->streams[i].data)) mapped &= ~(1u << i);
        }
        if (mapped_ib && args.tessellated && record_held(draw->indices.data)) mapped_ib = false;
    }
    struct s_slice_range { uint32_t first, size; };
    [[clang::uninitialized]] s_slice_range slices[k_index_slice + 1];
    uint64_t payload = uint64_t(vmax) + imax;
    const bool linear = mapped && !args.indexed && draw->vertex_shader &&
        __atomic_load_n(&record_linear_vs_slot(draw->vertex_shader).shader, __ATOMIC_ACQUIRE) == draw->vertex_shader;
    for (uint32_t m = mapped; m; m &= m - 1) {
        const unsigned i = __builtin_ctz(m);
        const auto &v = draw->streams[i];
        if (v.offset >= v.size) {
            slices[i] = {};
            continue;
        }
        const uint32_t bytes = v.size - v.offset;
        uint32_t first = 0, end = bytes;
        if (linear && args.start == 0 && v.stride) {
            const uint32_t vertices = bytes / v.stride;
            const uint64_t last = uint64_t(args.count);
            if (last < vertices) end = uint32_t(last) * v.stride;
        }
        if (end - first > 16384u) {
            mapped &= ~(1u << i);
            continue;
        }
        slices[i] = {v.offset + first, end - first};
        payload += end - first;
    }
    if (mapped_ib) {
        const auto &ib = draw->indices;
        const unsigned stride = args.tessellated ? 12u : ib.index_size;
        if (stride == 2 || stride == 4 || stride == 12) {
            const uint64_t end = (uint64_t(args.start) + args.count) * stride;
            if (end <= ib.size && uint64_t(args.count) * stride <= 16384u)
                slices[k_index_slice] = {args.start * stride, args.count * stride};
            else
                mapped_ib = false;
        } else {
            mapped_ib = false;
        }
        if (mapped_ib) payload += slices[k_index_slice].size;
    }
    if (dirty & PORT_VK_DIRTY_TEXTURES) g_record.texture_reads = nullptr;
    for (uint32_t m = (dirty & PORT_VK_DIRTY_TEXTURES) >> 12; m; m &= m - 1) {
        const unsigned i = __builtin_ctz(m);
        if (draw->samplers[i].bound) {
            const auto &source = draw->samplers[i].texture.source;
            auto &ranges = g_record_textures[i];
            if (memcmp(&source.desc, &g_record_texture_desc[i], sizeof(source.desc)) != 0) {
                record_texture_ranges(source, ranges);
                g_record_texture_desc[i] = source.desc;
            } else {
                ranges[0].base = source.base;
                ranges[1].base = source.mip;
            }
            g_record_bound_textures |= 1u << i;
        } else g_record_bound_textures &= ~(1u << i);
    }
    constexpr size_t fixed = sizeof(s_record_delta) + sizeof(s_port_vk_draw) + 2 * sizeof(s_record_bank) +
        sizeof(g_decoded.flow) + sizeof(s_port_vk_geometry) + sizeof(s_port_vk_indices) +
        (k_index_slice + 1) * sizeof(s_record_delta::s_slice) +
        (PORT_VK_MAX_STREAMS + 1 + 2 * PORT_VK_SAMPLER_COUNT) * sizeof(s_record_range) + 96;
    // If the owned geometry alone fits, keep the delta and pin an oversize mapped source.
    const bool borrow = payload > k_record_slab_size - fixed;
    if (borrow) payload = uint64_t(vmax) + imax;
    uint8_t *storage = payload <= k_record_slab_size - fixed ? record_reserve(fixed + size_t(payload)) : nullptr;
    if (!storage || ((dirty & PORT_VK_DIRTY_VDECL) && draw->declaration && !decl) ||
        (geometry && geometry->declaration && !override_decl)) {
        g_record_source = nullptr;
        record_borrow_draw(draw, dirty, args, geometry);
        return;
    }
    auto *delta = (s_record_delta *)storage;
    *delta = {};
    delta->args = args;
    delta->dirty = dirty;
    delta->reseed = reset;
    g_record_source = draw;
    if ((dirty & PORT_VK_DIRTY_RAST) && (seed || memcmp(seen.clip_planes, draw->clip_planes, sizeof(seen.clip_planes)))) {
        memcpy(seen.clip_planes, draw->clip_planes, sizeof(seen.clip_planes));
        delta->clip_planes = true;
    }
    delta->streams = changed_streams;
    // Publish the baseline along with its changes. The worker can consult it in place
    // throughout this draw, including when the preceding draw skipped a bind.
    auto &changes = delta->changes;
    changes.float_gen[0] = seen.float_gen[0];
    changes.float_gen[1] = seen.float_gen[1];
    changes.sampler_gen = seen.sampler_gen;
    changes.stream_gen = seen.published_stream_gen;
    changes.flow_gen = seen.flow_gen;
    changes.sampler_slots = ((dirty & PORT_VK_DIRTY_TEXTURES) >> 12) | (dirty >> 38);
    changes.stream_slots = changed_streams;
    if (dirty & PORT_VK_DIRTY_STREAMS) {
        changes.stream_slots |= seen.stream_mask ^ draw->stream_mask;
        seen.stream_mask = draw->stream_mask;
    }
    seen.sampler_gen = draw->samplers_any_gen;
    seen.published_stream_gen = draw->stream_gen;
    s_delta_writer out = { (uint8_t *)(delta + 1) };
    if (dirty & PORT_VK_DIRTY_VDECL) out.bytes(&decl, sizeof(decl));
    record_draw_fields(out, *draw, *delta);
    delta->flow = seed || !draw->bool_loop_gen || seen.flow_gen != draw->bool_loop_gen;
    seen.flow_gen = draw->bool_loop_gen;
    for (unsigned s = 0; s < 2; ++s) {
        const float *floats = s ? g_live_shadows.pixel_float : g_live_shadows.vertex_float;
        const uint32_t *chunks = s ? draw->ps_chunk_gen : draw->vs_chunk_gen;
        const uint32_t gen = s ? draw->ps_float_gen : draw->vs_float_gen;
        if (floats) delta->float_sources |= 1u << s;
        if (chunks) delta->chunk_sources |= 1u << s;
        const bool new_source = seen.chunk_source[s] != chunks || seen.float_source[s] != floats;
        // Both guest sources map onto the same decoded bank address on the worker.
        if (new_source)
            delta->reseed = true;
        const bool all = seed || !gen || new_source || gen < seen.float_gen[s];
        if (all || gen != seen.float_gen[s]) {
            delta->changes.chunks[s] = all ? UINT64_MAX : changed_float_chunks(chunks, seen.float_gen[s], gen);
            // With exactly one bank write between packets every changed stamp equals gen.
            const bool uniform = chunks && !all && seen.float_gen[s] && gen - seen.float_gen[s] == 1u;
            if (uniform) delta->uniform_stamps |= 1u << s;
            record_chunk_runs(out, delta->changes.chunks[s], chunks, floats, uniform ? gen : 0u);
        }
        seen.float_gen[s] = gen;
        seen.chunk_source[s] = chunks;
        seen.float_source[s] = floats;
        if (delta->flow) {
            const auto *bools = s ? g_live_shadows.pixel_bool : g_live_shadows.vertex_bool;
            const auto *ints = s ? g_live_shadows.pixel_int : g_live_shadows.vertex_int;
            if (bools) {
                delta->flow_sources |= 1u << s;
                if (seed || memcmp(bools, seen.bools[s], sizeof(seen.bools[s]))) {
                    delta->changed_flow |= 1u << s;
                    memcpy(seen.bools[s], bools, sizeof(seen.bools[s]));
                    out.bytes(bools, sizeof(seen.bools[s]));
                }
            }
            if (ints) {
                delta->flow_sources |= 4u << s;
                if (seed || memcmp(ints, seen.ints[s], sizeof(seen.ints[s]))) {
                    delta->changed_flow |= 4u << s;
                    memcpy(seen.ints[s], ints, sizeof(seen.ints[s]));
                    out.bytes(ints, sizeof(seen.ints[s]));
                }
            }
        }
    }
    if (delta->flow) {
        const uint32_t changed = delta->changed_flow | (seen.flow_sources ^ delta->flow_sources);
        changes.flow_rows = (changed & 3u) | ((changed & 4u) ? 0x3cu : 0u) | ((changed & 8u) ? 0x3c0u : 0u);
        seen.flow_sources = delta->flow_sources;
    }
    if (geometry) {
        auto *geo = (s_port_vk_geometry *)(((uintptr_t)out.p + 15) & ~uintptr_t(15));
        *geo = *geometry;
        geo->declaration = override_decl;
        out.p = (uint8_t *)(geo + 1);
        if (geometry->indices) {
            auto *indices = (s_port_vk_indices *)out.p;
            *indices = *geometry->indices;
            geo->indices = indices;
            out.p = (uint8_t *)(indices + 1);
            if (imax) { indices->data = out.p; out.bytes(geometry->indices->data, imax); }
        }
        out.p = (uint8_t *)(((uintptr_t)out.p + 15) & ~uintptr_t(15));
        if (vmax) { geo->stream0.data = out.p; out.bytes(geometry->stream0.data, vmax); }
        delta->geometry = geo;
    }
    if (!borrow && (mapped || mapped_ib)) {
        out.p = (uint8_t *)(((uintptr_t)out.p + 15) & ~uintptr_t(15));
        delta->slices = (s_record_delta::s_slice *)out.p;
        delta->slice_mask = mapped | (uint32_t(mapped_ib) << k_index_slice);
        // Only set bits are read. Reserve through the last such slot, then fill
        // those descriptors directly in the packet alongside their immutable bytes.
        out.p += (32u - __builtin_clz(delta->slice_mask)) * sizeof(*delta->slices);
        for (uint32_t m = delta->slice_mask; m; m &= m - 1) {
            const unsigned i = __builtin_ctz(m);
            const void *base = i == k_index_slice ? draw->indices.data : draw->streams[i].data;
            delta->slices[i] = {out.p, slices[i].first, slices[i].size};
            if (slices[i].size) out.bytes((const uint8_t *)base + slices[i].first, slices[i].size);
        }
    }
    out.p = (uint8_t *)(((uintptr_t)out.p + 15) & ~uintptr_t(15));
    auto *reads = (s_record_range *)out.p;
    unsigned n = 0;
    for (uint32_t m = bound & ~(borrow ? 0u : mapped); m; m &= m - 1) {
        const auto &v = draw->streams[__builtin_ctz(m)];
        reads[n++] = {v.data, v.size};
    }
    if (indexed && (borrow || !mapped_ib)) reads[n++] = {draw->indices.data, draw->indices.size};
    auto *end = reads + n;
    if (!g_record.texture_reads) {
        g_record.texture_reads = end;
        for (uint32_t m = g_record_bound_textures; m; m &= m - 1) {
            const auto &ranges = g_record_textures[__builtin_ctz(m)];
            for (const auto &r : ranges) if (r.base && r.size) *end++ = r;
        }
        g_record.texture_count = uint32_t(end - g_record.texture_reads);
    }
    g_record.used = (uint8_t *)end - g_record.slab;
    const uint64_t seq = record_publish({ [](const void *data) {
        const auto &packet = *(const s_record_delta *)data;
        apply_record_delta(packet);
        g_record_packet = &packet;
        execute_record_draw(&g_decoded.draw, packet.dirty, packet.args, packet.geometry);
        g_record_packet = nullptr;
    }, delta, 0, reads, n, g_record.texture_count, g_record.texture_reads });
    if (borrow) record_wait(seq, "mapped");
}

struct s_record_query {
    const void *key;
    s_record_query *next;
    s_record_request poll;
    uint64_t issue, generation, polling_generation;
    uint32_t samples;
    bool ready;
};
s_record_query *g_record_queries[256];
bool g_record_missed_issue;

// Only the monitor protects these records. GPU query state stays on the worker.
s_record_query *record_query(const void *key, bool create)
{
    auto &bucket = g_record_queries[mix32(key_of(key)) % 256];
    for (auto *q = bucket; q; q = q->next)
        if (q->key == key) return q;
    if (!create) return nullptr;
    auto *q = (s_record_query *)calloc(1, sizeof(s_record_query));
    if (q) {
        q->key = key;
        q->poll.done = true;
        q->next = bucket;
        bucket = q;
    }
    return q;
}

template<class F> void record_issue(const void *query, const F &f)
{
    s_record_client client;
    if (!client.admitted) return;
    const bool owner = record_owner();
    port_gpu_monitor_lock(g_record.monitor);
    auto *q = record_query(query, true);
    uint64_t generation = 0;
    if (q) {
        generation = ++q->generation;
        q->issue = owner ? record_head() + 1 : UINT64_MAX;
        q->ready = false;
    } else g_record_missed_issue = true;
    port_gpu_monitor_unlock(g_record.monitor);
    if (owner) return record_post(f);
    record_sync(f);
    port_gpu_monitor_lock(g_record.monitor);
    if (q && q->generation == generation) q->issue = 0;
    port_gpu_monitor_unlock(g_record.monitor);
}

int record_poll_query(const void *key, uint32_t *samples)
{
    *samples = 0;
    s_record_client client;
    if (!client.admitted) return 0;
    port_gpu_monitor_lock(g_record.monitor);
    auto *q = record_query(key, g_record_missed_issue);
    int ready = !q && !g_record_missed_issue; // never issued: complete with zero samples
    if (q && q->issue <= record_tail() && (q->generation || record_tail() == record_head())) {
        if (q->ready) {
            ready = 1;
        } else if (record_owner() && record_tail() == record_head() && !g_record.first && !g_record.servicing_request) {
            g_record_reading = true;
            ready = port_vk_query_result(key, &q->samples);
            g_record_reading = false;
            q->ready = ready != 0;
        } else if (q->poll.done) {
            q->polling_generation = q->generation;
            q->poll = {{[](const void *data) {
                auto &q = *(s_record_query *)data;
                uint32_t count = 0;
                const bool ready = port_vk_query_result(q.key, &count) != 0;
                port_gpu_monitor_lock(g_record.monitor);
                if (q.generation == q.polling_generation) {
                    q.samples = count;
                    q.ready = ready;
                }
                port_gpu_monitor_unlock(g_record.monitor);
            }, q, 0}, q->issue, nullptr, false, true};
            record_enqueue_request(&q->poll);
        }
        if (ready) *samples = q->samples;
    }
    port_gpu_monitor_unlock(g_record.monitor);
    return ready;
}

void record_start()
{
    const char *setting = getenv("HALO3_VK_THREAD");
    if (!g_vk || (setting && strcmp(setting, "0") == 0)) return;
    g_record.monitor = port_gpu_monitor_create();
    g_record.slabs[0] = (uint8_t *)malloc(k_record_slab_size);
    g_record.slabs[1] = (uint8_t *)malloc(k_record_slab_size);
    g_record.slab_index = 0;
    g_record.slab = g_record.slabs[0];
    g_record.prev_present_seq = 0;
    __atomic_store_n(&g_record.owner, (uint32_t)GetCurrentThreadId(), __ATOMIC_RELEASE);
    g_record.stopping = false;
    g_record.used = 0;
    g_record.texture_reads = nullptr;
    memset(g_record_linear_vs, 0, sizeof(g_record_linear_vs));
    g_record_open_export = {};
    g_record_invalidations = 0;
    g_record_targets_incomplete = false;
    g_record_missed_issue = false;
    const char *trace = getenv("HALO3_VK_THREAD_TRACE");
    g_record.trace = trace && strcmp(trace, "0") != 0;
    if (g_record.trace) {
        long long frequency = 0;
        QueryPerformanceFrequency(&frequency);
        record_trace("clock-hz", 0, frequency, 0, 0);
    }
    if (g_record.monitor && g_record.slabs[0] && g_record.slabs[1])
        g_record.thread = port_gpu_thread_start(record_worker_main, nullptr);
    if (!g_record.thread) {
        if (g_record.monitor) port_gpu_monitor_destroy(g_record.monitor);
        free(g_record.slabs[0]);
        free(g_record.slabs[1]);
        g_record.monitor = nullptr;
        g_record.slabs[0] = nullptr;
        g_record.slabs[1] = nullptr;
        g_record.slab = nullptr;
        port_write_error("Vulkan recording worker unavailable, using synchronous recording\n");
        return;
    }
    __atomic_fetch_and(&g_record_clients, ~k_record_closing, __ATOMIC_RELEASE);
    g_live_shadows = g_shadows;
    g_record_source = nullptr;
    __atomic_store_n(&g_record.enabled, 1u, __ATOMIC_RELEASE);
}

void record_stop(s_record_command terminal)
{
    if (!g_record.thread) return;
    __atomic_fetch_or(&g_record_clients, k_record_closing, __ATOMIC_ACQ_REL);
    while (__atomic_load_n(&g_record_clients, __ATOMIC_ACQUIRE) != k_record_closing)
        Sleep(1);
    record_wait(record_publish(terminal), "shutdown");
    port_gpu_monitor_lock(g_record.monitor);
    g_record.stopping = true;
    port_gpu_monitor_broadcast(g_record.monitor);
    port_gpu_monitor_unlock(g_record.monitor);
    port_gpu_thread_join(g_record.thread);
    __atomic_store_n(&g_started, 0, __ATOMIC_RELEASE);
    __atomic_store_n(&g_record.enabled, 0u, __ATOMIC_RELEASE);
    port_gpu_monitor_destroy(g_record.monitor);
    free(g_record.slabs[0]);
    free(g_record.slabs[1]);
    {
        s_record_resource_guard guard;
        g_record_targets.release();
        map_free(&g_record_written_buffers);
    }
    for (auto &bucket : g_record_queries)
        while (bucket) {
            auto *q = bucket;
            bucket = q->next;
            free(q);
        }
    for (auto &bucket : g_record_decls) {
        while (bucket) {
            auto *d = bucket;
            bucket = d->next;
            free(d);
        }
    }
    g_record.monitor = nullptr;
    g_record.slabs[0] = nullptr;
    g_record.slabs[1] = nullptr;
    g_record.slab = nullptr;
    g_record.prev_present_seq = 0;
    g_record.thread = nullptr;
    g_record_source = nullptr;
    g_shadows = {};
    g_live_shadows = {};
}

// Pipeline creation or a replay wait on the draw thread: counter from _begin (0 when stats are off) to _end.
long long pipeline_stats_begin()
{
    return g_frame_stats_window != 0 ? frame_stats_now() : 0;
}

void pipeline_stats_end(long long started)
{
    if (started == 0)
        return;
    const double ms = frame_stats_ms(started, frame_stats_now());
    s_pipeline_stats &p = g_pipeline_stats;
    p.count++;
    p.over_2ms += ms > 2.0 ? 1u : 0u;
    p.total_ms += ms;
    p.max_ms = ms > p.max_ms ? ms : p.max_ms;
    p.window_count++;
    p.window_ms += ms;
    p.window_max_ms = ms > p.window_max_ms ? ms : p.window_max_ms;
    g_frame_trace.pipelines++;
    g_frame_trace.pipeline_ms += ms;
}

// The present starting now: the frame's trace line (HALO3_FRAME_TRACE), and the next frame's counters.
void frame_trace_present(long long now)
{
    s_frame_trace &t = g_frame_trace;
    static long long last_present;
    if (!t.on)
        return;
    const double interval = last_present != 0 ? frame_stats_ms(last_present, now) : 0.0;
    static double last_cpu;
    const double cpu = port_gpu_thread_cpu_ms();
    const double cpu_ms = cpu >= 0.0 && last_cpu > 0.0 ? cpu - last_cpu : -1.0;
    last_cpu = cpu;
    last_present = now;
    s_gpu_stats b;
    gpu_get_stats(g_gpu, &b);
    if (interval >= t.threshold) {
        const s_gpu_stats &a = t.base;
        char text[400];
        snprintf(text, sizeof(text),
                 "ftrace: frame %u interval %.2f span %.2f begin %.2f gpu %.2f pso %u %.2f sync %u alloc %u "
                 "tex %u %.2f exports %u draws %u reused %u texparts %.2f %.2f cpu %.2f binds pipe %u %u dyn %u "
                 "sets %u %u index %u %u barriers %u %u texsets %u readbacks %u\n",
                 g_presented, interval, t.frame_begun != 0 ? frame_stats_ms(t.frame_begun, now) : 0.0, t.begin_ms,
                 (double)b.gpu_frame_ms, t.pipelines, t.pipeline_ms,
                 b.immediate_submits - a.immediate_submits, b.memory_allocations - a.memory_allocations,
                 t.textures, t.texture_ms, t.exports, t.draws,
                 b.buffers_reused - a.buffers_reused, t.texture_convert_ms, t.texture_upload_ms, cpu_ms,
                 b.pipelines - a.pipelines, b.pipelines_skipped - a.pipelines_skipped, b.dynamic_states - a.dynamic_states,
                 b.sets - a.sets, b.sets_skipped - a.sets_skipped, b.index_buffers - a.index_buffers,
                 b.index_buffers_skipped - a.index_buffers_skipped, b.barriers - a.barriers,
                 b.transitions_skipped - a.transitions_skipped, t.texture_sets, t.readbacks);
        port_write_error(text);
    }
    t.frame_begun = 0;
    t.begin_ms = t.pipeline_ms = t.texture_ms = t.texture_convert_ms = t.texture_upload_ms = 0.0;
    t.pipelines = t.textures = t.exports = t.draws = 0;
    t.texture_sets = t.readbacks = 0;
    t.base = b;
}

// A texture conversion's time (HALO3_FRAME_TRACE), from construction to the end of the scope.
struct s_texture_trace_timer {
    long long started;
    s_texture_trace_timer() : started(g_frame_trace.on ? frame_stats_now() : 0) {}
    ~s_texture_trace_timer()
    {
        if (started != 0) {
            g_frame_trace.textures++;
            g_frame_trace.texture_ms += frame_stats_ms(started, frame_stats_now());
        }
    }
};

// The present starting now: closes the previous frame's interval, and reports a full window.
void frame_stats_present_begin(long long now)
{
    s_frame_stats &s = g_frame_stats;
    if (g_frame_stats_window == 0 || s.intervals == 0)
        return;
    s_gpu_stats stats;
    gpu_get_stats(g_gpu, &stats);
    if (s.last_present != 0) {
        if (s.count == 0) {
            s.first_frame = g_presented;
            s.immediate_base = stats.immediate_submits;
            s.deferred_base = stats.deferred_uploads;
            for (int k = 0; k < 4; k++)
                s.converted_base[k] = g_converted_bytes[k];
            s.allocation_base = stats.memory_allocations;
            s.reuse_base = stats.buffers_reused;
        }
        s.intervals[s.count++] = (float)frame_stats_ms(s.last_present, now);
        if (stats.gpu_frame_ms >= 0.0f) {
            s.gpu_ms += stats.gpu_frame_ms;
            s.gpu_frames++;
        }
    }
    s.last_present = now;
    if (s.count < g_frame_stats_window)
        return;
    // sorted copy for the percentiles (insertion sort: one window every N frames; no C library qsort on Windows)
    float *sorted = s.intervals;
    double total = 0.0;
    for (uint32_t i = 0; i < s.count; i++) {
        const float v = sorted[i];
        total += v;
        uint32_t j = i;
        for (; j > 0 && sorted[j - 1] > v; j--)
            sorted[j] = sorted[j - 1];
        sorted[j] = v;
    }
    const double average = total / (double)s.count;
    const uint32_t p99 = (uint32_t)((double)(s.count - 1) * 0.99 + 0.5);
    char text[576];
    snprintf(text, sizeof(text),
             "frame stats: frames %u..%u: %.2f ms/frame (%.1f FPS), median %.2f, p99 %.2f, worst %.2f ms; "
             "submit+GPU wait+present %.2f ms/frame; %u sync uploads (%.1f/frame), %u batched; vertices converted "
             "%.0f/%.0f/%.0f/%.0f KB/frame (user pointer/dynamic/static/rectangles)\n",
             s.first_frame, s.first_frame + s.count - 1u, average, average > 0.0 ? 1000.0 / average : 0.0,
             (double)sorted[s.count / 2u], (double)sorted[p99], (double)sorted[s.count - 1u], s.submit_ms / (double)s.count,
             stats.immediate_submits - s.immediate_base,
             (double)(stats.immediate_submits - s.immediate_base) / (double)s.count,
             stats.deferred_uploads - s.deferred_base,
             (double)(g_converted_bytes[0] - s.converted_base[0]) / 1024.0 / s.count,
             (double)(g_converted_bytes[1] - s.converted_base[1]) / 1024.0 / s.count,
             (double)(g_converted_bytes[2] - s.converted_base[2]) / 1024.0 / s.count,
             (double)(g_converted_bytes[3] - s.converted_base[3]) / 1024.0 / s.count);
    port_write_error(text);
    snprintf(text, sizeof(text),
             "frame stats: GPU %.2f ms/frame (timestamps); %u memory allocations (%.1f/frame), %u buffers reused\n",
             s.gpu_frames != 0 ? s.gpu_ms / (double)s.gpu_frames : 0.0,
             stats.memory_allocations - s.allocation_base,
             (double)(stats.memory_allocations - s.allocation_base) / (double)s.count,
             stats.buffers_reused - s.reuse_base);
    port_write_error(text);
    s_pipeline_stats &p = g_pipeline_stats;
    snprintf(text, sizeof(text),
             "frame stats: pipeline creations and waits on the draw thread: %u (%.1f ms, worst %.2f ms) in these frames; %u "
             "(%.1f ms, worst %.2f ms, %u over 2 ms) in %.1f s since the device started%s\n",
             p.window_count, p.window_ms, p.window_max_ms, p.count, p.total_ms, p.max_ms, p.over_2ms,
             frame_stats_ms(p.started, now) / 1000.0, pipeline_prewarm_status());
    port_write_error(text);
    p.window_count = 0;
    p.window_ms = p.window_max_ms = 0.0;
    s.count = 0;
    s.submit_ms = 0.0;
    s.gpu_ms = 0.0;
    s.gpu_frames = 0;
}

// The display: the console's display controller scans the front buffer out through its gamma LUT
// (port_vk_set_display_gamma). Each presented frame goes through that LUT into a display image of the source's
// size (R8G8B8A8_UNORM, storage), which the window and the frame dumps show as they are.
struct s_display_target {
    s_gpu_render_target *target;
    uint32_t width, height;
};
s_display_target g_display_targets[4];
// scratch images for exponent-biased resolves (scaled_resolve_source), one per source size
struct s_scratch_image {
    s_gpu_image *image;
    uint32_t width, height, format;
};
s_scratch_image g_resolve_scratch[8];
uint32_t g_display_gamma[4u * 257u]; // GPU_DISPLAY_GAMMA_RAMP_SIZE: header (.x the mode), table[256]

// caches
t_list<s_shader_entry> g_shaders;
s_map g_shader_index;
t_list<s_target_entry> g_targets;
s_map g_target_index;
t_list<s_texture_entry> g_textures;
s_map g_texture_index;
uint32_t g_pending_7e3_texture; // g_textures index + 1 with a deferred 7e3 resolve, or 0
void materialize_pending_7e3(s_texture_entry *entry);
void materialize_pending_7e3_for_owner(const s_target_entry *owner);
// D3DQUERYTYPE_OCCLUSION queries (port_vk_query_*): the backend occlusion records each Issue(BEGIN) .. Issue(END) made;
// a tiled query has one per tile, and GetData sums them (CreateQueryTiled).
struct s_query_entry {
    const void *key;
    uint32_t count;
    uint32_t serial[8], record[8];
    uint64_t completed_samples;
};
const void *g_active_query;
t_list<s_query_entry> g_queries;
s_map g_query_index;
t_list<s_buffer_entry> g_buffers;
s_map g_buffer_index;
s_map g_index_buffer_index; // D3DIndexBuffer -> g_buffers index (upload_indices; its own map: keys never meet vertex keys)
t_list<s_declaration_entry> g_declarations;
s_map g_declaration_index;
t_list<s_pipeline_entry> g_pipelines;
s_map g_pipeline_index;     // hash -> first entry index + 1 (collisions: linear scan of equal hashes)
t_list<s_sampler_entry> g_samplers;
s_map g_sampler_index; // sampler_desc_hash -> g_samplers index (the last sampler made with that hash)
// sampler_for's answer for recent states (it is asked for every texture fetch of every draw, mostly the same few states):
// entry by a hash of the state, valid while its sampler is not null (cleared when the samplers are destroyed)
const uint32_t k_sampler_state_words = sizeof(s_port_vk_sampler_state) / 4u;
struct s_sampler_memo {
    uint32_t state[k_sampler_state_words]; // the s_port_vk_sampler_state's words
    uint32_t compare;
    gpu_sampler sampler;
    uint32_t entry_index; // index + 1, stable across g_samplers growth
};
const uint32_t k_sampler_memo_entries = 256;
s_sampler_memo g_sampler_memo[k_sampler_memo_entries];
static uint32_t g_anisotropy_override = 0;
static uint32_t g_anisotropy_config_rev = ~0u;
void xe_invalidate_sampler_caches();
t_list<s_compute_entry> g_compute_pipelines;
s_map g_touches;            // resource key -> generation
// An Xbox texture header is a view of memory: two headers over the same base address name the same texels (a
// caller may resolve into a texture through one header and pass another over the same memory to D3DDevice_Swap).
s_map g_resolved_memory;    // texture base address -> index + 1 of the entry a Resolve last wrote there
// Most sampled textures have never been a resolve destination. Track headers and
// aliases independently: a header can still own an image after its base changes.
// Only the render thread adds keys. Invalidations can leave false positives.
struct s_resolve_filter {
    uint64_t words[16];
    static uint32_t bit(const void *key) { return (key_of(key) * 0x9e3779b1u) >> 22; }
    void add(const void *key) {
        const uint32_t i = bit(key);
        const uint64_t mask = 1ull << (i & 63u);
        if (!(__atomic_load_n(&words[i >> 6], __ATOMIC_RELAXED) & mask))
            __atomic_fetch_or(&words[i >> 6], mask, __ATOMIC_RELEASE);
    }
    bool contains(const void *key) const {
        const uint32_t i = bit(key);
        return (__atomic_load_n(&words[i >> 6], __ATOMIC_ACQUIRE) >> (i & 63u)) & 1u;
    }
    void clear() {
        for (auto &word : words) __atomic_store_n(&word, 0ull, __ATOMIC_RELEASE);
    }
};
s_resolve_filter g_resolved_headers, g_resolved_bases;
// Texture memory the CPU wrote through a lock (port_vk_texture_written): its texels are in host order
s_map g_host_order_memory;  // texture base or mip address -> 1
// Both are records about memory, which the game's resource pager rewrites on its own thread (port_vk_memory_streamed)
// while the render thread converts and binds textures: every use of the two maps holds this spin lock.
int g_memory_records_lock;
struct s_memory_records_lock {
    s_memory_records_lock()
    {
        while (__atomic_exchange_n(&g_memory_records_lock, 1, __ATOMIC_ACQUIRE) != 0) {
        }
    }
    ~s_memory_records_lock() { __atomic_store_n(&g_memory_records_lock, 0, __ATOMIC_RELEASE); }
};
// EDRAM: base -> index + 1 of the target last drawn into it (edram_written), and the serial of its contents
s_map g_edram_owner;
uint32_t g_edram_serial;
// EDRAM clears (port_vk_clear_edram) not yet applied to every surface at their base: one per base, the newest
struct s_edram_clear {
    uint32_t base;          // EDRAM base + 1 (0: free)
    uint32_t serial;        // g_edram_serial when it was issued
    uint32_t depth;         // a depth / stencil clear (else color)
    s_gpu_clear clear;
    int32_t rect[4];
};
s_edram_clear g_edram_clears[16];
int g_storage_a2b10g10r10 = -1; // whether A2B10G10R10 targets can be written by gpu_cmd_reinterpret_7e3

// bound state
s_target_entry *g_bound_color[4];
s_target_entry *g_bound_depth;
s_gpu_image *g_pass_color[4];
s_gpu_image *g_pass_depth;
uint32_t g_pass_depth_format;
uint32_t g_pass_depth_width;
uint32_t g_pass_depth_height;
bool g_pass_color_aliases_depth;
uint32_t g_depth_resolve_edram[16];
uint32_t g_depth_resolve_edram_count;
bool g_depth_resolve_edram_overflow;
void record_depth_resolve_edram(uint32_t depth_edram)
{
    if (depth_edram == 0 || g_depth_resolve_edram_overflow)
        return;
    for (uint32_t i = 0; i < g_depth_resolve_edram_count; i++)
        if (g_depth_resolve_edram[i] == depth_edram)
            return;
    if (g_depth_resolve_edram_count < sizeof(g_depth_resolve_edram) / sizeof(g_depth_resolve_edram[0]))
        g_depth_resolve_edram[g_depth_resolve_edram_count++] = depth_edram;
    else
        g_depth_resolve_edram_overflow = true;
}
uint32_t g_pass_format_epoch = 1;
void pass_format_changed()
{
    if (++g_pass_format_epoch == 0)
        g_pass_format_epoch = 1;
}

void clear_pass_depth()
{
    g_pass_depth = 0;
    g_pass_depth_format = k_gpu_format_undefined;
    g_pass_depth_width = 0;
    g_pass_depth_height = 0;
}

void clear_pass_attachments()
{
    for (s_gpu_image *&color : g_pass_color)
        color = 0;
    g_pass_color_aliases_depth = false;
    clear_pass_depth();
}

void end_pass()
{
    gpu_cmd_end_rendering(g_gpu);
    clear_pass_attachments();
}

bool begin_pass(s_gpu_image *const *colors, uint32_t count, const s_target_entry *depth_target,
                const s_gpu_clear *clear)
{
    s_gpu_image *depth = depth_target != 0 ? depth_target->image : 0;
    gpu_cmd_begin_rendering(g_gpu, colors, count, depth, clear);
    pass_format_changed();
    clear_pass_attachments();
    if (!gpu_rendering(g_gpu, 0, 0))
        return false;
    for (uint32_t i = 0; i < count; i++)
        g_pass_color[i] = colors[i];
    if (depth != 0) {
        g_pass_depth = depth;
        g_pass_depth_format = depth_target->host_format;
        g_pass_depth_width = depth_target->host_width;
        g_pass_depth_height = depth_target->host_height;
    }
    return true;
}

void resource_touch_changed()
{
    if (__atomic_add_fetch(&g_port_vk_resource_touch_epoch, 1u, __ATOMIC_ACQ_REL) == 0)
        __atomic_add_fetch(&g_port_vk_resource_touch_epoch, 1u, __ATOMIC_ACQ_REL);
}
struct s_viewport_scissor_cache {
    uint32_t frame;
    uint32_t vp_gen;
    uint32_t pass_epoch;
    VkCommandBuffer command_buffer;
    uint32_t rendering_epoch;
};
s_viewport_scissor_cache g_vp_cache;

// fallbacks for bindings without a texture
s_gpu_image *g_dummy_2d, *g_dummy_3d, *g_dummy_cube, *g_dummy_array;
s_gpu_image *g_dummy_flat_normal; // 2D, (0, 0, 0, 0): the placeholder of a normal map (dummy_view_for)
gpu_sampler g_default_sampler;
// MCC's colour grading (the DX11 final composite looks its result up in two 3D LUTs the Xbox game never binds):
// an identity LUT and the linear, clamped sampler it is read with stand in for them (color_grading_binding)
s_gpu_image *g_identity_lut;
gpu_sampler g_lut_sampler;

// ---- logging ----

// The name the renderer's log lines start with: the backend's (caps.name: "vulkan", "d3d12"; the Vulkan lines read as
// they did before the backend seam), before one is up the one being started.
const char *g_log_name = "vulkan";

// "<name>: " then the text (no newline added).
void log_text(const char *text)
{
    port_write_error(g_log_name);
    port_write_error(": ");
    port_write_error(text);
}

// "<name> device: " then the text: the frontend's own problems (a draw it skipped, a texture it could not convert).
void device_log_text(const char *text)
{
    port_write_error(g_log_name);
    port_write_error(" device: ");
    port_write_error(text);
}

void write_uint(unsigned int value)
{
    char text[12];
    int n = 11;
    text[n] = '\0';
    do {
        text[--n] = (char)('0' + value % 10);
        value /= 10;
    } while (value && n);
    port_write_error(text + n);
}

void write_hex(unsigned int value)
{
    char text[9];
    for (int i = 7; i >= 0; i--) {
        text[i] = "0123456789abcdef"[value & 15u];
        value >>= 4;
    }
    text[8] = '\0';
    port_write_error(text);
}

// One line per distinct problem (by a caller-chosen id), so a frame loop does not flood the log.
s_map g_reported;
bool first_report(uint32_t id)
{
    uint32_t seen;
    if (map_find(&g_reported, id | 1u, &seen))
        return false;
    map_set(&g_reported, id | 1u, 1);
    return true;
}

void report(uint32_t id, const char *text, const char *detail = 0)
{
    if (!first_report(id))
        return;
    device_log_text("");
    port_write_error(text);
    if (detail != 0) {
        port_write_error(": ");
        port_write_error(detail);
    }
    port_write_error("\n");
}

uint32_t hash_text(const char *text)
{
    return gpu_hash_bytes(2166136261u, text, strlen(text));
}

// ---- frames and the ring ----

bool begin_frame()
{
    if (g_frame_open)
        return true;
    const long long begun = g_frame_trace.on ? frame_stats_now() : 0;
    if (gpu_frame_begin(g_gpu) != k_gpu_ok)
        return false;
    if (begun != 0) {
        g_frame_trace.frame_begun = begun;
        g_frame_trace.begin_ms = frame_stats_ms(begun, frame_stats_now());
    }
    g_frame_open = 1;
    __atomic_store_n(&g_frame_thread, (uint32_t)GetCurrentThreadId(), __ATOMIC_RELAXED);
    if (++g_frame_serial == 0) {
        g_frame_serial = 1;
        xe_invalidate_draw_caches();
    }
    g_rings[gpu_frame_index(g_gpu)].cursor = 0;
    clear_pass_attachments();
    const uint32_t aniso_rev = (uint32_t)port_config_revision();
    if (aniso_rev != g_anisotropy_config_rev) {
        g_anisotropy_config_rev = aniso_rev;
        const uint32_t next_aniso = port_config_anisotropy();
        if (next_aniso != g_anisotropy_override) {
            g_anisotropy_override = next_aniso;
            memset(g_sampler_memo, 0, sizeof(g_sampler_memo));
            xe_invalidate_sampler_caches();
        }
    }
    return true;
}

// Space in the current frame's ring: returns the mapped pointer; *out_offset is its offset in ring_buffer().
void *ring_alloc(uint32_t size, uint32_t alignment, uint32_t *out_offset)
{
    s_ring &ring = g_rings[gpu_frame_index(g_gpu)];
    const uint32_t top = ring.cursor + alignment - 1u;
    const uint32_t offset = (alignment & (alignment - 1u)) == 0 ? top & -alignment : top / alignment * alignment;
    if (ring.buffer == 0 || (uint64_t)offset + size > k_ring_size) {
        report(0x72696e67u, "the frame's upload ring is full (draw skipped)");
        return 0;
    }
    ring.cursor = offset + size;
    *out_offset = offset;
    return ring.mapped + offset;
}

// ring_alloc at an offset of at least `minimum` (a binding that starts that many bytes before the space).
void *ring_alloc_after(uint32_t size, uint32_t alignment, uint32_t minimum, uint32_t *out_offset)
{
    s_ring &ring = g_rings[gpu_frame_index(g_gpu)];
    if (ring.cursor < minimum) {
        if (minimum > k_ring_size) {
            report(0x72696e67u, "the frame's upload ring is full (draw skipped)");
            return 0;
        }
        ring.cursor = minimum;
    }
    return ring_alloc(size, alignment, out_offset);
}

s_gpu_buffer *ring_buffer() { return g_rings[gpu_frame_index(g_gpu)].buffer; }

template<uint32_t N>
inline void copy_upload_part(uint8_t *&dst, const uint8_t *&src)
{
    __builtin_memcpy_inline(dst, src, N);
    dst += N;
    src += N;
}

// libc's short unaligned memcpy writes overlapping head and tail vectors. For
// packet-sized uploads, write each byte of the mapped destination exactly once.
inline void copy_upload_bytes(uint8_t *dst, const uint8_t *src, uint32_t bytes)
{
    if (bytes > 512) {
        memcpy(dst, src, bytes);
        return;
    }
    while (bytes >= 64) {
        copy_upload_part<64>(dst, src);
        bytes -= 64;
    }
    if (bytes & 32) copy_upload_part<32>(dst, src);
    if (bytes & 16) copy_upload_part<16>(dst, src);
    if (bytes & 8)  copy_upload_part<8>(dst, src);
    if (bytes & 4)  copy_upload_part<4>(dst, src);
    if (bytes & 2)  copy_upload_part<2>(dst, src);
    if (bytes & 1)  *dst = *src;
}

// The upload ring takes every draw's data until the frame is presented. A frame that draws far more than a shown one
// fills it: the tiled screenshot (main_screenshot_start_capture) renders 3840x2176 as dozens of tiles in one frame, each
// the whole scene, and with a full ring every later draw was skipped (the screenshot's own memexport draw among them).
// Such a frame is submitted once its ring has less than k_ring_headroom left and recording goes on in the next slot,
// with its own ring and descriptor sets, as after any mid-frame sync (finish_frame) but without waiting. Called at a
// draw's start, before it allocates or records anything, so each draw stays in one command buffer; not while an
// occlusion query is open (its count must come from one frame).
const uint32_t k_ring_headroom = 16u << 20;
void ring_ensure_headroom()
{
    if (!g_frame_open || g_rings[gpu_frame_index(g_gpu)].cursor <= k_ring_size - k_ring_headroom ||
        gpu_occlusion_active(g_gpu))
        return;
    end_pass();
    gpu_frame_end(g_gpu, 0u);
    g_frame_open = 0;
    g_ring_splits++;
    report(0x72737074u, "a frame filled most of the upload ring (the tiled screenshot): submitted, continued in the next "
                        "frame slot");
    begin_frame();
}

// ---- render targets ----

void unlink_color_snapshot(s_target_entry *target)
{
    if (target->snapshot_texture) {
        g_textures.items[target->snapshot_texture - 1u].snapshot_target = 0;
        target->snapshot_texture = 0;
    }
}

void exchange_resolve_images(s_target_entry *target, s_texture_entry *texture)
{
    s_gpu_image *old = target->image;
    target->image = texture->image;
    texture->image = old;
}

// Releasing or fully replacing the texture returns its original backing to EDRAM.
// Each entry always owns exactly one image, including between frames and at shutdown.
void return_color_snapshot(s_texture_entry *texture)
{
    if (texture->snapshot_target) {
        auto *target = &g_targets.items[texture->snapshot_target - 1u];
        exchange_resolve_images(target, texture);
        unlink_color_snapshot(target);
    }
}

// The ordinary Vulkan copy leaves its destination shader-readable. Do the same for a
// handoff, and for a sampled snapshot after a transfer has used it as a copy source.
void color_snapshot_sampleable(s_gpu_image *image)
{
    auto *vk_image = reinterpret_cast<s_vulkan_image *>(image);
    if (vk_image->layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        return;
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
    VkPipelineStageFlags stages = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    if (vk_image->layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    } else if (vk_image->layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        stages = VK_PIPELINE_STAGE_TRANSFER_BIT;
        barrier.srcAccessMask = 0;
    } else if (vk_image->layout == VK_IMAGE_LAYOUT_GENERAL) {
        stages = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    }
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = (VkImageLayout)vk_image->layout;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex = barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vk_image->image;
    barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    VkCommandBuffer cmd = vulkan_recording::recording_command_buffer(g_vk, "color snapshot");
    if (!cmd) return;
    g_vk->vk.vkCmdPipelineBarrier(cmd, stages, (VkPipelineStageFlags)g_vk->shader_stages,
                                  0, 0, nullptr, 0, nullptr, 1, &barrier);
    vk_image->layout = barrier.newLayout;
    vk_image->attachment_dirty = 0;
    ++g_vk->bind_counters.barriers;
}

// The conversion passes leave attachment-capable storage images in GENERAL.
void finish_resolve_conversion(s_gpu_image *image)
{
    if (g_vk && g_frame_open && reinterpret_cast<s_vulkan_image *>(image)->layout == VK_IMAGE_LAYOUT_GENERAL &&
        !gpu_rendering(g_gpu, 0, 0))
        color_snapshot_sampleable(image);
}

// A partial write or load needs the original pixels in the RT's private backing.
void restore_color_snapshot(s_target_entry *target)
{
    if (!target || !target->snapshot_texture)
        return;
    s_gpu_image *snapshot = g_textures.items[target->snapshot_texture - 1u].image;
    gpu_cmd_copy_image(g_gpu, snapshot, 0, 0, target->host_width, target->host_height,
                       target->image, 0, 0, 0, 0, target->host_width, target->host_height);
    color_snapshot_sampleable(snapshot);
    clear_pass_attachments();
    unlink_color_snapshot(target);
}

// The texels a D3DFORMAT names, as far as memory goes: a ..._AS_16_16_16_16 format holds its base format's texels
// (fetched at 16-bit precision) and a ..._EXPAND format its 16-bit format's, so headers over one memory that agree
// on this read the same texels whatever their swizzle, sign or filtering precision.
uint32_t texel_layout(uint32_t d3d_format)
{
    const uint32_t format = xenos_d3dformat_texture_format(d3d_format);
    switch (format) {
    case 50: return k_xenos_format_8_8_8_8;        // 8_8_8_8_AS_16_16_16_16
    case 51: return k_xenos_format_dxt1;           // DXT1_AS_16_16_16_16
    case 52: return k_xenos_format_dxt2_3;         // DXT2_3_AS_16_16_16_16
    case 53: return k_xenos_format_dxt4_5;         // DXT4_5_AS_16_16_16_16
    case 54: return k_xenos_format_2_10_10_10;     // 2_10_10_10_AS_16_16_16_16
    case 55: return 16u;                           // 10_11_11_AS_16_16_16_16
    case 56: return 17u;                           // 11_11_10_AS_16_16_16_16
    case 27: return k_xenos_format_16;             // 16_EXPAND
    case 28: return k_xenos_format_16_16;          // 16_16_EXPAND
    case 29: return k_xenos_format_16_16_16_16;    // 16_16_16_16_EXPAND
    default: return format;
    }
}

// An 8_8_8_8 surface or texture header whose X, Y and Z are GPUSIGN_GAMMA (D3DFMT_*_GAMMA: `| 0x7e00`). As a render
// target the Xenos stores the PWL gamma codes of the linear colors the shader writes; as a texture it degammas them.
bool format_is_gamma(uint32_t d3d_format)
{
    return texel_layout(d3d_format) == k_xenos_format_8_8_8_8 && ((d3d_format >> 9) & 0x3fu) == 0x3fu;
}

// Whether a color resolve from a surface of source_format into a texture of dest_format leaves the PWL gamma codes in
// memory. The Xenos resolve of an 8_8_8_8_GAMMA EDRAM target decodes the codes to linear before averaging the samples
// and writes linear values (xenia-edge resolve.xesli XeResolveDecodePWLGammaSource / ResolveEdramInfo
// decode_pwl_gamma). D3DDevice_Resolve (retail 0x82177870; at 0x821782a0) re-aliases the source as plain 8_8_8_8 in
// RB_COLOR_INFO, so that the codes are copied as they are, only when the destination's SignX is GPUSIGN_GAMMA: a gamma
// texture then degammas the codes when fetched, and a plain texture (the texture camera's 0x18280186 albedo and final
// image under a 0x18287f86 target) holds the linear light itself.
bool resolve_keeps_gamma_codes(uint32_t source_format, uint32_t dest_format)
{
    return format_is_gamma(source_format) && ((dest_format >> 9) & 3u) == 3u;
}

// The Xenos 16-bit fixed-point EDRAM formats (16_16_EDRAM, 16_16_16_16_EDRAM: signed fixed point in [-32, 32), xenia
// xenos.h) as the Xbox units themselves, in 32-bit float targets: Halo 3 clears its distortion target to 16.0, the
// distortion draws add Xbox-unit offsets onto it, and its resolve's exponent bias of -5 brings the neutral 16.0 to 0.5 in
// the fixed-point texture the displacement pass reads (scaled_resolve_source). 32-bit floats keep the 1/2048 steps of
// the 16-bit fixed point at 16.0 (float16 steps there by 1/64); a device that cannot blend or filter them gets float16.
uint32_t edram_fixed16_host_format(bool four_channels)
{
    static int blendable[2] = { -1, -1 };
    const uint32_t wide = four_channels ? k_gpu_format_r32g32b32a32_sfloat : k_gpu_format_r32g32_sfloat;
    int &known = blendable[four_channels ? 1 : 0];
    if (known < 0) {
        known = gpu_format_blendable_filterable(g_gpu, wide) ? 1 : 0;
        if (!known)
            log_text("32-bit float render targets cannot be blended or filtered here: the 16-bit "
                     "fixed-point EDRAM targets use float16 (coarser at the distortion target's 16.0)\n");
    }
    return known ? wide : four_channels ? k_gpu_format_r16g16b16a16_sfloat : k_gpu_format_r16g16_sfloat;
}

uint32_t target_host_format(uint32_t d3d_format, uint32_t depth)
{
    if (depth)
        return g_gpu->caps.depth_stencil_format;
    // a gamma target keeps the linear colors at 16 bits (xenia gamma_render_target_as_unorm16): blending is linear,
    // and the darks keep the precision their gamma codes have (codes 0-63 are 16 linear 8-bit levels)
    if (format_is_gamma(d3d_format))
        return k_gpu_format_r16g16b16a16_unorm;
    switch (texel_layout(d3d_format)) {
    case k_xenos_format_2_10_10_10:
        return k_gpu_format_a2b10g10r10_unorm; // X in the low bits, as the Xenos lays it out
    case k_xenos_format_2_10_10_10_float:
    case k_xenos_format_16_16_16_16_float:
    case k_xenos_format_16_16_16_16:
        return k_gpu_format_r16g16b16a16_sfloat;
    case k_xenos_format_16_16_float:
    case k_xenos_format_16_16:
        return k_gpu_format_r16g16_sfloat;
    case k_xenos_format_16_16_edram:
    case k_xenos_format_16_16_16_16_edram:
        return edram_fixed16_host_format(texel_layout(d3d_format) == k_xenos_format_16_16_16_16_edram);
    case k_xenos_format_32_float:
        return k_gpu_format_r32_sfloat;
    case k_xenos_format_32_32_float:
        return k_gpu_format_r32g32_sfloat;
    case k_xenos_format_8:
        return k_gpu_format_r8_unorm;
    case k_xenos_format_8_8:
        return k_gpu_format_r8g8_unorm;
    default:
        return k_gpu_format_b8g8r8a8_unorm;
    }
}

// HALO3_RES_SCALE: host pixels per guest pixel of a render target's image. Halo 3's reduced-resolution post-process
// surfaces (k_render_surface_group_descriptions: the bloom and exposure chain at 1/4 .. 1/64 and 1x1, the 1/4
// 8_8_8_8_GAMMA blur behind translucent UI (c_screen_postprocess::blur_display), the 1/16 8_8_8_8, the 4-player
// level of the depth-of-field target) run fixed-tap kernels whose offsets are guest texels (c1 = 1 / the guest size:
// downsample_4x4_block +-1 texel, kernel_5 / blur_11 taps 1.8 texels apart). At N x N host texels per guest texel the
// taps cover a 1/N footprint with zero-weight gaps between them, so a blur becomes N shifted copies (the boxy shapes
// behind the lobby cards, a screen-door halo around bright HDR). Those surfaces stay at guest resolution, as xenia's
// draw_resolution_scale_threshold keeps small offscreen surfaces native. A color surface at most a quarter of the back
// buffer each way is one: Halo 3 draws no geometry into one that small (the 4-player split-screen views and the
// distortion target are 576x320, the texture camera 320x192), so its depth buffers never pair with one.
// Shadow maps, water ripple simulation buffers, and dynamic square render targets stay at guest resolution too: they
// are not screen-space. Every screen-space view in Halo 3 (main back buffer, split-screen views, texture camera,
// distortion) is rectangular; the only square surfaces are the 512x512 shadow depth map (surface 2), the 800x800
// world-space water ripple height and slope-height buffers (surfaces 56 and 57), the 384x384 dynamic render targets
// (whose depth buffer is also 384x384), and 1x1 exposure targets. Shadow maps use percentage-closer kernels placed in
// guest texels (+-0.5 .. +-1.5 texels, depth biases of sqrt(2) / size); at N x N host texels per guest texel the taps
// read every Nth host texel and shadow edges break into steps and speckle. Water ripple buffers (800x800 covering a
// fixed 20x20 world-unit window) rely on implicit mip selection over the 9-level tex_ripple_pattern in ripple_apply_ps
// and a guest-texel (1/800) finite difference in ripple_slope_ps; scaling them to 800*N shifts mip selection by
// log2(N) sharper levels and amplifies finite-difference slopes ~3x-4x into noisy, clipped spikes at 4K.
uint32_t target_res_scale(const s_port_vk_surface *surface)
{
    if (surface->width == surface->height)
        return 1u;
    if (surface->depth)
        return g_res_scale;
    if (g_back_buffer_width == 0 || g_back_buffer_height == 0)
        return g_res_scale;
    if (surface->width <= g_back_buffer_width / 4u && surface->height <= g_back_buffer_height / 4u)
        return 1u;
    return g_res_scale;
}

// The target table may grow while other surfaces are bound.
s_target_entry *push_target()
{
    if (g_targets.count < g_targets.capacity)
        return g_targets.push();
    uint32_t color_indices[4], depth_index = 0;
    for (uint32_t i = 0; i < 4; i++)
        color_indices[i] = g_bound_color[i] != 0 ? (uint32_t)(g_bound_color[i] - g_targets.items) + 1u : 0u;
    if (g_bound_depth != 0)
        depth_index = (uint32_t)(g_bound_depth - g_targets.items) + 1u;
    s_target_entry *entry = g_targets.push();
    if (entry != 0) {
        for (uint32_t i = 0; i < 4; i++)
            g_bound_color[i] = color_indices[i] != 0 ? &g_targets.items[color_indices[i] - 1u] : 0;
        if (depth_index != 0)
            g_bound_depth = &g_targets.items[depth_index - 1u];
    }
    return entry;
}

s_target_entry *target_for(const s_port_vk_surface *surface)
{
    if (surface == 0 || surface->key == 0)
        return 0;
    uint32_t index;
    s_target_entry *entry = 0;
    if (map_find(&g_target_index, key_of(surface->key), &index))
        entry = &g_targets.items[index];
    if (entry != 0 && entry->back_buffer)
        return entry;
    const uint32_t host_format = target_host_format(surface->d3d_format, surface->depth);
    if (entry != 0)
        entry->color_exp_bias = surface->color_exp_bias;
    if (entry != 0 && entry->width == surface->width &&
        (entry->strip_height ? entry->strip_height : entry->height) == surface->height &&
        entry->host_format == host_format && entry->msaa == surface->msaa_samples) {
        if (entry->edram_base != surface->edram_base) {
            entry->edram_base = surface->edram_base;
            entry->content = 0;
            for (int i = 0; i < 4; i++)
                if (g_pass_color[i] == entry->image)
                    g_pass_color_aliases_depth = true;
        }
        return entry;
    }
    if (entry == 0) {
        entry = push_target();
        if (entry == 0)
            return 0;
        map_set(&g_target_index, key_of(surface->key), g_targets.count - 1u);
    } else if (entry->image != 0) {
        // the surface was redefined: the old image goes once no frame can use it
        materialize_pending_7e3_for_owner(entry);
        unlink_color_snapshot(entry);
        bool attached = g_pass_depth == entry->image;
        for (const s_gpu_image *color : g_pass_color)
            attached |= color == entry->image;
        if (attached)
            end_pass();
        pass_format_changed();
        gpu_image_retire(g_gpu, entry->image);
        if (entry->shell_image)
            gpu_image_retire(g_gpu, entry->shell_image);
        entry->shell_image = 0;
        entry->image = 0;
    }
    entry->key = surface->key;
    entry->width = surface->width;
    entry->height = surface->height;
    entry->strip_height = entry->shell_height = 0;
    entry->d3d_format = surface->d3d_format;
    entry->host_format = host_format;
    entry->depth = surface->depth;
    entry->edram_base = surface->edram_base;
    entry->color_exp_bias = surface->color_exp_bias;
    entry->msaa = surface->msaa_samples;
    entry->content = 0;
    entry->pending_clear_flags = 0;
    const uint32_t scale = target_res_scale(surface);
    entry->scale = scale;
    s_gpu_image_desc desc = {};
    desc.view_type = k_gpu_view_2d;
    desc.format = host_format;
    // HALO3_RES_SCALE: N x N host pixels per guest pixel
    desc.width = (surface->width != 0 ? surface->width : 1u) * scale;
    desc.height = (surface->height != 0 ? surface->height : 1u) * scale;
    desc.depth = 1;
    desc.mip_levels = 1;
    desc.array_layers = 1;
    // a depth surface is also a copy destination: what its EDRAM holds is copied in (edram_sync_owner: a depth restore,
    // a multisampled view over a single-sampled surface)
    desc.usage = surface->depth ? (uint32_t)(k_gpu_image_depth_stencil_attachment | k_gpu_image_transfer_src |
                                             k_gpu_image_transfer_dst)
                                : (uint32_t)(k_gpu_image_color_attachment | k_gpu_image_transfer_src |
                                             k_gpu_image_transfer_dst | k_gpu_image_sampled);
    if (host_format == k_gpu_format_a2b10g10r10_unorm) {
        // a 2_10_10_10 surface may read a 7e3 surface's EDRAM (edram_sync)
        if (g_storage_a2b10g10r10 < 0)
            g_storage_a2b10g10r10 = gpu_format_storage(g_gpu, k_gpu_format_a2b10g10r10_unorm) ? 1 : 0;
        if (g_storage_a2b10g10r10 > 0)
            desc.usage |= k_gpu_image_storage;
    }
    if (!surface->depth && texel_layout(surface->d3d_format) == k_xenos_format_8_8_8_8 &&
        gpu_format_storage(g_gpu, host_format)) {
        // an 8_8_8_8 surface may read the EDRAM of one with the other color space (edram_sync, gpu_cmd_convert_pwl)
        desc.usage |= k_gpu_image_storage;
    }
    entry->host_width = desc.width;
    entry->host_height = desc.height;
    // The float bloom targets and their single-level resolve textures can exchange
    // backings if both support storage. Keep the original RT allocation as fallback.
    const uint32_t rt_usage = desc.usage;
    if (g_vk && !surface->depth && !surface->msaa_samples &&
        texel_layout(surface->d3d_format) == k_xenos_format_16_16_16_16_float &&
        gpu_format_storage(g_gpu, host_format))
        desc.usage |= k_gpu_image_storage;
    entry->image = gpu_image_create(g_gpu, &desc);
    if (!entry->image && desc.usage != rt_usage) {
        desc.usage = rt_usage;
        entry->image = gpu_image_create(g_gpu, &desc);
    }
    if (entry->image == 0)
        report(key_of(surface->key) ^ 0x5254u, "render target image creation failed");
    return entry;
}

uint32_t bound_res_scale()
{
    for (int i = 0; i < 4; i++)
        if (g_bound_color[i] != 0 && g_bound_color[i]->scale != 0)
            return g_bound_color[i]->scale;
    if (g_bound_depth != 0 && g_bound_depth->scale != 0)
        return g_bound_depth->scale;
    return g_res_scale;
}

// ---- EDRAM aliasing ----

// On the Xbox, surfaces at one EDRAM base share their memory: a render target and the header the game resolves it
// through (another format over the same bits: a 2_10_10_10_FLOAT target read as 2_10_10_10), or two passes' targets
// placed over each other. Here every surface is its own image, so the image last drawn into owns the range, and
// another surface over the same range and size is brought up to date before it is drawn into or resolved: copied,
// or its bits reinterpreted where the formats differ as EDRAM would read them. Partial overlaps are not modelled.
uint32_t edram_key(uint32_t base) { return 0x45440000u ^ (base * 2654435761u); }

void edram_written(s_target_entry *entry)
{
    if (entry == 0 || entry->back_buffer || entry->edram_base == 0xffffffffu || entry->image == 0)
        return;
    materialize_pending_7e3_for_owner(entry);
    entry->content = ++g_edram_serial;
    map_set(&g_edram_owner, edram_key(entry->edram_base), (uint32_t)(entry - g_targets.items) + 1u);
}

void edram_sync_owner(s_target_entry *entry);
void edram_apply_clear(s_target_entry *entry);

// A surface's EDRAM row pitch in tiles: its row of samples (XGSurfaceSize: 4x multisampling doubles the width) in tiles
// of 80 32-bit samples, or 40 of a 64-bit format (xenia-edge xenos.h).
uint32_t edram_pitch_tiles(const s_target_entry *t)
{
    const uint32_t format = xenos_d3dformat_texture_format(t->d3d_format);
    const bool wide = !t->depth && (format == k_xenos_format_16_16_16_16_edram ||
                                    format == k_xenos_format_16_16_16_16_float || format == k_xenos_format_32_32_float);
    const uint32_t tile = wide ? 40u : 80u;
    return (t->width * (t->msaa == 2u ? 2u : 1u) + tile - 1u) / tile;
}

// The single-sampled pixel column (row) under 4x sample 0 of column (row) c of a view (edram_gather_samples).
uint32_t edram_sample0(uint32_t c) { return ((c & ~1u) << 1) | (c & 1u); }

s_gpu_image *g_gather_scratch; // edram_gather_samples' columns, grown as needed

// A 4x multisampled view over EDRAM a surface of fewer samples drew. Halo 3 resolves the scene depth for the sniper
// rifle's scope screen and the Ghost's display through one (player_view_resolve_depth_of_field_source: surface 5, a
// 576x320 4x view of the 1152x640 scene depth; surface 6 over a split-screen view's 2x depth), and its distortion
// particles depth-test against another (surface 4). The Xenos keeps the samples of every view of an EDRAM range where
// a single-sampled view of it has its pixels (XGSurfaceSize: 2x doubles the height, 4x the width too), arranged in
// 4x4-sample blocks; per xenia-edge edram.xesli (XeEdramOffsetBytes), sample s of 4x pixel (x, y) is at
//   u = ((x & ~1) << 1) | (x & 1) | ((s & 1) << 1),   v = ((y & ~1) << 1) | (y & 1) | (s & 2)
// and 2x pixel (x, y) sample s at u = (x & ~2) | (s << 1), v = ((y & ~1) << 1) | (y & 1) | (x & 2), for views of
// the same pitch in tiles. So 4x sample 0 of (x, y) is single-sampled pixel (u0(x), u0(y)) and 2x pixel (u0(x), y)
// sample 0 (u0 = edram_sample0). The images here keep one sample per pixel: the view's gets sample 0 (what a
// FRAGMENT0 or depth resolve copies and what its draws test against), the surface's holds its pixels' one value. A
// pair of the view's columns (rows) is a pair of adjacent columns (rows) of the surface, so the copy goes by runs:
// columns, then (over a single-sampled surface) rows out of a scratch image. A 4x view over a surface of another
// format, depth or pitch is reported and left to edram_sync_owner's other cases. Returns whether it copied.
bool edram_gather_samples(s_target_entry *owner, s_target_entry *entry)
{
    const uint32_t os = owner->scale != 0 ? owner->scale : g_res_scale;
    const uint32_t n = entry->scale != 0 ? entry->scale : g_res_scale;
    if (owner->msaa > 1u || entry->msaa != 2u || owner->depth != entry->depth || owner->host_format != entry->host_format ||
        os != n || edram_pitch_tiles(owner) != edram_pitch_tiles(entry)) {
        char detail[160];
        snprintf(detail, sizeof(detail), "%ux%u/m%u/f%08x over %ux%u/m%u/f%08x at edram %x", entry->width,
                 entry->height, entry->msaa, entry->d3d_format, owner->width, owner->height, owner->msaa,
                 owner->d3d_format, entry->edram_base);
        report(owner->msaa * 0x101u ^ entry->msaa * 0x10001u ^ owner->host_format * 7u ^ entry->host_format * 13u ^
                   0x6d736161u,
               "a 4x multisampled view over EDRAM of a surface of another format or pitch (its samples not gathered; "
               "a 4x view over a 1x or 2x surface of the same format and pitch is)", detail);
        return false;
    }
    const bool rows_too = owner->msaa == 0; // over a 2x surface the view's rows are its rows
    // the view's columns and rows whose sample 0 lies inside the surface (edram_sample0 grows with c)
    uint32_t w = 0, h = 0;
    while (w < entry->width && edram_sample0(w) < owner->width)
        w++;
    while (h < entry->height && (rows_too ? edram_sample0(h) : h) < owner->height)
        h++;
    if (w == 0 || h == 0)
        return false;
#ifdef HALO3_64BIT
    if (owner->depth && owner->host_format == k_gpu_format_d32_sfloat_s8_uint) {
        uint32_t offset = 0;
        const uint32_t parameters[4] = { w * n, h * n, n, rows_too ? 1u : 0u };
        void *data = ring_alloc(sizeof(parameters), g_uniform_alignment, &offset);
        if (data != 0) {
            memcpy(data, parameters, sizeof(parameters));
            if (xe_layout_call(gpu_cmd_gather_depth, g_gpu, owner->image, entry->image, ring_buffer(), offset, w * n, h * n))
                return true;
        }
    }
#endif
    const uint32_t span = rows_too ? edram_sample0(h - 1u) + 1u : h; // the surface's rows the view's rows come from
    if (rows_too) {
        s_gpu_image_desc have = {};
        if (g_gather_scratch != 0)
            gpu_image_get_desc(g_gpu, g_gather_scratch, &have);
        if (g_gather_scratch == 0 || have.format != owner->host_format || have.width < w * n || have.height < span * n) {
            if (g_gather_scratch != 0)
                gpu_image_retire(g_gpu, g_gather_scratch); // copies recorded this frame may still use it
            const bool grow = have.format == owner->host_format;
            s_gpu_image_desc desc = {};
            desc.view_type = k_gpu_view_2d;
            desc.format = owner->host_format;
            desc.width = grow && have.width > w * n ? have.width : w * n;
            desc.height = grow && have.height > span * n ? have.height : span * n;
            desc.depth = 1;
            desc.mip_levels = 1;
            desc.array_layers = 1;
            desc.usage = k_gpu_image_transfer_src | k_gpu_image_transfer_dst;
            g_gather_scratch = gpu_image_create(g_gpu, &desc);
            if (g_gather_scratch == 0) {
                report(0x67617468u, "the scratch image of a multisampled EDRAM view could not be made (not carried)");
                return false;
            }
        }
    }
    const uint32_t most = ((w > h ? w : h) + 1u) / 2u;
    s_gpu_copy_rect *rects = (s_gpu_copy_rect *)calloc(most, sizeof(s_gpu_copy_rect));
    if (rects == 0)
        return false;
    // HALO3_RES_SCALE: the arrangement is the guest pixels'; each carries its N x N host pixels along
    uint32_t count = 0;
    for (uint32_t x = 0; x < w; x += 2u) {
        s_gpu_copy_rect &r = rects[count++];
        r.source_x = (int32_t)(edram_sample0(x) * n);
        r.source_y = 0;
        r.destination_x = (int32_t)(x * n);
        r.destination_y = 0;
        r.width = (x + 1u < w ? 2u : 1u) * n;
        r.height = span * n;
    }
    gpu_cmd_copy_rects(g_gpu, owner->image, rows_too ? g_gather_scratch : entry->image, rects, count);
    if (rows_too) {
        count = 0;
        for (uint32_t y = 0; y < h; y += 2u) {
            s_gpu_copy_rect &r = rects[count++];
            r.source_x = 0;
            r.source_y = (int32_t)(edram_sample0(y) * n);
            r.destination_x = 0;
            r.destination_y = (int32_t)(y * n);
            r.width = w * n;
            r.height = (y + 1u < h ? 2u : 1u) * n;
        }
        gpu_cmd_copy_rects(g_gpu, g_gather_scratch, entry->image, rects, count);
    }
    free(rects);
    if (tracing()) {
        char text[160];
        snprintf(text, sizeof(text), "trace edram samples %ux%u (4x sample 0) of %p (%ux) into %p, edram %x\n", w, h,
                 owner->key, owner->msaa == 0 ? 1u : 2u, entry->key, entry->edram_base);
        port_write_error(text);
    }
    return true;
}

#ifdef HALO3_64BIT
s_vulkan_pipeline *g_cropped_depth_pipeline;

struct s_cropped_depth_parameters {
    uint32_t width, height, source_x, source_y, gather_rows;
};

// Crop variant of depth_gather.hlsl, built with dxc -O3 -spirv -fspv-target-env=vulkan1.3.
// At scale 1, sample_zero(c) is c + (c & ~1u). Add the crop origin before mapping either axis.
// Each invocation writes up to four floats and one packed stencil word, using the cropped row pitch.
// The third and fourth uniform words are source_x/y, followed by gather_rows.
static const uint32_t k_cropped_depth_spirv[] = {
    0x07230203u, 0x00010600u, 0x000e0000u, 0x0000007cu, 0x00000000u, 0x00020011u, 0x00000001u, 0x0006000bu,
    0x00000001u, 0x4c534c47u, 0x6474732eu, 0x3035342eu, 0x00000000u, 0x0003000eu, 0x00000000u, 0x00000001u,
    0x000a000fu, 0x00000005u, 0x00000002u, 0x6e69616du, 0x00000000u, 0x00000003u, 0x00000004u, 0x00000005u,
    0x00000006u, 0x00000007u, 0x00060010u, 0x00000002u, 0x00000011u, 0x00000008u, 0x00000008u, 0x00000001u,
    0x00040047u, 0x00000003u, 0x0000000bu, 0x0000001cu, 0x00040047u, 0x00000004u, 0x00000022u, 0x00000000u,
    0x00040047u, 0x00000004u, 0x00000021u, 0x00000000u, 0x00040047u, 0x00000005u, 0x00000022u, 0x00000000u,
    0x00040047u, 0x00000005u, 0x00000021u, 0x00000001u, 0x00040047u, 0x00000006u, 0x00000022u, 0x00000000u,
    0x00040047u, 0x00000006u, 0x00000021u, 0x00000002u, 0x00040047u, 0x00000007u, 0x00000022u, 0x00000000u,
    0x00040047u, 0x00000007u, 0x00000021u, 0x00000003u, 0x00040047u, 0x00000008u, 0x00000006u, 0x00000004u,
    0x00050048u, 0x00000009u, 0x00000000u, 0x00000023u, 0x00000000u, 0x00030047u, 0x00000009u, 0x00000002u,
    0x00050048u, 0x0000000au, 0x00000000u, 0x00000023u, 0x00000000u, 0x00050048u, 0x0000000au, 0x00000001u,
    0x00000023u, 0x00000004u, 0x00050048u, 0x0000000au, 0x00000002u, 0x00000023u, 0x00000008u, 0x00050048u,
    0x0000000au, 0x00000003u, 0x00000023u, 0x0000000cu, 0x00050048u, 0x0000000au, 0x00000004u, 0x00000023u,
    0x00000010u, 0x00030047u, 0x0000000au, 0x00000002u, 0x00040015u, 0x0000000bu, 0x00000020u, 0x00000001u,
    0x0004002bu, 0x0000000bu, 0x0000000cu, 0x00000000u, 0x00040015u, 0x0000000du, 0x00000020u, 0x00000000u,
    0x0004002bu, 0x0000000du, 0x0000000eu, 0x00000004u, 0x00020014u, 0x0000000fu, 0x00030029u, 0x0000000fu,
    0x00000010u, 0x0004002bu, 0x0000000bu, 0x00000011u, 0x00000001u, 0x0004002bu, 0x0000000du, 0x00000012u,
    0x00000003u, 0x0004002bu, 0x0000000bu, 0x00000013u, 0x00000003u, 0x0004002bu, 0x0000000bu, 0x00000014u,
    0x00000004u, 0x0004002bu, 0x0000000du, 0x00000015u, 0x00000000u, 0x0004002bu, 0x0000000bu, 0x00000016u,
    0x00000002u, 0x0004002bu, 0x0000000du, 0x00000017u, 0x00000002u, 0x0004002bu, 0x0000000du, 0x00000018u,
    0x000000ffu, 0x0004002bu, 0x0000000du, 0x00000019u, 0x00000008u, 0x0004002bu, 0x0000000du, 0x0000001au,
    0x0000001fu, 0x0004002bu, 0x0000000du, 0x0000001bu, 0x00000001u, 0x00030016u, 0x0000001cu, 0x00000020u,
    0x00090019u, 0x0000001du, 0x0000001cu, 0x00000001u, 0x00000002u, 0x00000000u, 0x00000000u, 0x00000001u,
    0x00000000u, 0x00040020u, 0x0000001eu, 0x00000000u, 0x0000001du, 0x00090019u, 0x0000001fu, 0x0000000du,
    0x00000001u, 0x00000002u, 0x00000000u, 0x00000000u, 0x00000001u, 0x00000000u, 0x00040020u, 0x00000020u,
    0x00000000u, 0x0000001fu, 0x0003001du, 0x00000008u, 0x0000000du, 0x0003001eu, 0x00000009u, 0x00000008u,
    0x00040020u, 0x00000021u, 0x0000000cu, 0x00000009u, 0x0007001eu, 0x0000000au, 0x0000000du, 0x0000000du,
    0x0000000du, 0x0000000du, 0x0000000du, 0x00040020u, 0x00000022u, 0x00000002u, 0x0000000au, 0x00040017u,
    0x00000023u, 0x0000000du, 0x00000003u, 0x00040020u, 0x00000024u, 0x00000001u, 0x00000023u, 0x00020013u,
    0x00000025u, 0x00030021u, 0x00000026u, 0x00000025u, 0x00040017u, 0x00000027u, 0x0000000bu, 0x00000003u,
    0x00040020u, 0x00000028u, 0x00000002u, 0x0000000du, 0x00040017u, 0x00000029u, 0x0000000bu, 0x00000002u,
    0x00040017u, 0x0000002au, 0x0000001cu, 0x00000004u, 0x00040020u, 0x0000002bu, 0x0000000cu, 0x0000000du,
    0x00040017u, 0x0000002cu, 0x0000000du, 0x00000004u, 0x0004003bu, 0x0000001eu, 0x00000004u, 0x00000000u,
    0x0004003bu, 0x00000020u, 0x00000005u, 0x00000000u, 0x0004003bu, 0x00000021u, 0x00000006u, 0x0000000cu,
    0x0004003bu, 0x00000022u, 0x00000007u, 0x00000002u, 0x0004003bu, 0x00000024u, 0x00000003u, 0x00000001u,
    0x0004002bu, 0x0000000du, 0x0000002du, 0xfffffffcu, 0x0004002bu, 0x0000000du, 0x0000002eu, 0xfffffffeu,
    0x00050036u, 0x00000025u, 0x00000002u, 0x00000000u, 0x00000026u, 0x000200f8u, 0x0000002fu, 0x0004003du,
    0x00000023u, 0x00000030u, 0x00000003u, 0x000300f7u, 0x00000031u, 0x00000000u, 0x000300fbu, 0x00000015u,
    0x00000032u, 0x000200f8u, 0x00000032u, 0x00050051u, 0x0000000du, 0x00000033u, 0x00000030u, 0x00000000u,
    0x00050084u, 0x0000000du, 0x00000034u, 0x00000033u, 0x0000000eu, 0x00050041u, 0x00000028u, 0x00000035u,
    0x00000007u, 0x0000000cu, 0x0004003du, 0x0000000du, 0x00000036u, 0x00000035u, 0x000500b0u, 0x0000000fu,
    0x00000037u, 0x00000034u, 0x00000036u, 0x000300f7u, 0x00000038u, 0x00000000u, 0x000400fau, 0x00000037u,
    0x00000039u, 0x00000038u, 0x000200f8u, 0x00000039u, 0x00050051u, 0x0000000du, 0x0000003au, 0x00000030u,
    0x00000001u, 0x00050041u, 0x00000028u, 0x0000003bu, 0x00000007u, 0x00000011u, 0x0004003du, 0x0000000du,
    0x0000003cu, 0x0000003bu, 0x000500aeu, 0x0000000fu, 0x0000003du, 0x0000003au, 0x0000003cu, 0x000200f9u,
    0x00000038u, 0x000200f8u, 0x00000038u, 0x000700f5u, 0x0000000fu, 0x0000003eu, 0x00000010u, 0x00000032u,
    0x0000003du, 0x00000039u, 0x000300f7u, 0x0000003fu, 0x00000000u, 0x000400fau, 0x0000003eu, 0x00000040u,
    0x0000003fu, 0x000200f8u, 0x00000040u, 0x000200f9u, 0x00000031u, 0x000200f8u, 0x0000003fu, 0x00050080u,
    0x0000000du, 0x00000041u, 0x00000036u, 0x00000012u, 0x000500c7u, 0x0000000du, 0x00000042u, 0x00000041u,
    0x0000002du, 0x00050051u, 0x0000000du, 0x00000043u, 0x00000030u, 0x00000001u, 0x00050084u, 0x0000000du,
    0x00000044u, 0x00000043u, 0x00000042u, 0x00050080u, 0x0000000du, 0x00000045u, 0x00000044u, 0x00000034u,
    0x00050041u, 0x00000028u, 0x00000046u, 0x00000007u, 0x00000013u, 0x0004003du, 0x0000000du, 0x00000047u,
    0x00000046u, 0x00050080u, 0x0000000du, 0x00000048u, 0x00000047u, 0x00000043u, 0x00050041u, 0x00000028u,
    0x00000049u, 0x00000007u, 0x00000014u, 0x0004003du, 0x0000000du, 0x0000004au, 0x00000049u, 0x000500abu,
    0x0000000fu, 0x0000004bu, 0x0000004au, 0x00000015u, 0x000300f7u, 0x0000004cu, 0x00000000u, 0x000400fau,
    0x0000004bu, 0x0000004du, 0x0000004eu, 0x000200f8u, 0x0000004du, 0x000500c7u, 0x0000000du, 0x0000004fu,
    0x00000048u, 0x0000002eu, 0x00050080u, 0x0000000du, 0x00000050u, 0x00000048u, 0x0000004fu, 0x000200f9u,
    0x0000004cu, 0x000200f8u, 0x0000004eu, 0x000200f9u, 0x0000004cu, 0x000200f8u, 0x0000004cu, 0x000700f5u,
    0x0000000du, 0x00000051u, 0x00000050u, 0x0000004du, 0x00000048u, 0x0000004eu, 0x00050082u, 0x0000000du,
    0x00000052u, 0x00000036u, 0x00000034u, 0x0007000cu, 0x0000000du, 0x00000053u, 0x00000001u, 0x00000026u,
    0x00000052u, 0x0000000eu, 0x000200f9u, 0x00000054u, 0x000200f8u, 0x00000054u, 0x000700f5u, 0x0000000du,
    0x00000055u, 0x00000015u, 0x0000004cu, 0x00000056u, 0x00000057u, 0x000700f5u, 0x0000000du, 0x00000058u,
    0x00000015u, 0x0000004cu, 0x00000059u, 0x00000057u, 0x000500b0u, 0x0000000fu, 0x0000005au, 0x00000058u,
    0x00000053u, 0x000400f6u, 0x0000005bu, 0x00000057u, 0x00000002u, 0x000400fau, 0x0000005au, 0x00000057u,
    0x0000005bu, 0x000200f8u, 0x00000057u, 0x00050041u, 0x00000028u, 0x0000005cu, 0x00000007u, 0x00000016u,
    0x0004003du, 0x0000000du, 0x0000005du, 0x0000005cu, 0x00050080u, 0x0000000du, 0x0000005eu, 0x0000005du,
    0x00000034u, 0x00050080u, 0x0000000du, 0x0000005fu, 0x0000005eu, 0x00000058u, 0x000500c7u, 0x0000000du,
    0x00000060u, 0x0000005fu, 0x0000002eu, 0x00050080u, 0x0000000du, 0x00000061u, 0x0000005fu, 0x00000060u,
    0x0004007cu, 0x0000000bu, 0x00000062u, 0x00000061u, 0x0004007cu, 0x0000000bu, 0x00000063u, 0x00000051u,
    0x00060050u, 0x00000027u, 0x00000064u, 0x00000062u, 0x00000063u, 0x0000000cu, 0x00050080u, 0x0000000du,
    0x00000065u, 0x00000045u, 0x00000058u, 0x00050084u, 0x0000000du, 0x00000066u, 0x00000065u, 0x0000000eu,
    0x000500c2u, 0x0000000du, 0x00000067u, 0x00000066u, 0x00000017u, 0x0007004fu, 0x00000029u, 0x00000068u,
    0x00000064u, 0x00000064u, 0x00000000u, 0x00000001u, 0x0004003du, 0x0000001du, 0x00000069u, 0x00000004u,
    0x0007005fu, 0x0000002au, 0x0000006au, 0x00000069u, 0x00000068u, 0x00000002u, 0x0000000cu, 0x00050051u,
    0x0000001cu, 0x0000006bu, 0x0000006au, 0x00000000u, 0x0004007cu, 0x0000000du, 0x0000006cu, 0x0000006bu,
    0x00060041u, 0x0000002bu, 0x0000006du, 0x00000006u, 0x00000015u, 0x00000067u, 0x0003003eu, 0x0000006du,
    0x0000006cu, 0x0004003du, 0x0000001fu, 0x0000006eu, 0x00000005u, 0x0007005fu, 0x0000002cu, 0x0000006fu,
    0x0000006eu, 0x00000068u, 0x00000002u, 0x0000000cu, 0x00050051u, 0x0000000du, 0x00000070u, 0x0000006fu,
    0x00000000u, 0x000500c7u, 0x0000000du, 0x00000071u, 0x00000070u, 0x00000018u, 0x00050084u, 0x0000000du,
    0x00000072u, 0x00000058u, 0x00000019u, 0x000500c7u, 0x0000000du, 0x00000073u, 0x00000072u, 0x0000001au,
    0x000500c4u, 0x0000000du, 0x00000074u, 0x00000071u, 0x00000073u, 0x000500c5u, 0x0000000du, 0x00000056u,
    0x00000055u, 0x00000074u, 0x00050080u, 0x0000000du, 0x00000059u, 0x00000058u, 0x0000001bu, 0x000200f9u,
    0x00000054u, 0x000200f8u, 0x0000005bu, 0x00050041u, 0x00000028u, 0x00000075u, 0x00000007u, 0x00000011u,
    0x0004003du, 0x0000000du, 0x00000076u, 0x00000075u, 0x00050084u, 0x0000000du, 0x00000077u, 0x00000042u,
    0x00000076u, 0x00050084u, 0x0000000du, 0x00000078u, 0x00000077u, 0x0000000eu, 0x00050080u, 0x0000000du,
    0x00000079u, 0x00000078u, 0x00000045u, 0x000500c2u, 0x0000000du, 0x0000007au, 0x00000079u, 0x00000017u,
    0x00060041u, 0x0000002bu, 0x0000007bu, 0x00000006u, 0x00000015u, 0x0000007au, 0x0003003eu, 0x0000007bu,
    0x00000055u, 0x000200f9u, 0x00000031u, 0x000200f8u, 0x00000031u, 0x000100fdu, 0x00010038u,
};

s_vulkan_pipeline *cropped_depth_pipeline(s_vulkan_backend *backend)
{
    if (g_cropped_depth_pipeline != 0)
        return g_cropped_depth_pipeline;
    s_xe_layout_lock lock;
    const s_vulkan_layout_binding bindings[] = {
        { 0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
    };
    const VkDescriptorSetLayout set = vulkan_descriptor_set_layout_get(backend, bindings, 4);
    const VkPipelineLayout layout = set ? vulkan_pipeline_layout_get(backend, &set, 1) : VK_NULL_HANDLE;
    if (!layout)
        return 0;
    const VkShaderModule module = vulkan_shader_module_create(backend, k_cropped_depth_spirv,
                                                               sizeof(k_cropped_depth_spirv));
    if (module) {
        g_cropped_depth_pipeline = vulkan_compute_pipeline_create(backend, module, "main", layout);
        vulkan_shader_module_destroy(backend, module);
    }
    return g_cropped_depth_pipeline;
}

VkImageView cropped_depth_stencil_view(s_vulkan_backend *backend, s_vulkan_image *image)
{
    if (!image->stencil_view) {
        VkImageViewCreateInfo view = {};
        view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view.image = image->image;
        view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view.format = (VkFormat)image->desc.format;
        view.subresourceRange = { VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1 };
        if (backend->vk.vkCreateImageView(backend->device, &view, 0, &image->stencil_view) != VK_SUCCESS)
            return VK_NULL_HANDLE;
    }
    return image->stencil_view;
}

// Synchronize the gather with earlier attachment/transfer writes and subsequent readers of both depth aspects.
void cropped_depth_transition(s_vulkan_backend *backend, VkCommandBuffer command_buffer, s_vulkan_image *image,
                              VkImageLayout layout, VkPipelineStageFlags2 stages, VkAccessFlags2 access,
                              const VkMemoryBarrier2 *memory)
{
    VkDependencyInfo dependency = {};
    dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency.memoryBarrierCount = memory != 0 ? 1u : 0u;
    dependency.pMemoryBarriers = memory;
    VkImageMemoryBarrier2 barrier = {};
    if (image->layout != (uint32_t)layout || layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        barrier.dstStageMask = stages;
        barrier.dstAccessMask = access;
        barrier.oldLayout = (VkImageLayout)image->layout;
        barrier.newLayout = layout;
        barrier.srcQueueFamilyIndex = barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image->image;
        barrier.subresourceRange = { image->aspect, 0, image->desc.mip_levels, 0, image->desc.array_layers };
        dependency.imageMemoryBarrierCount = 1;
        dependency.pImageMemoryBarriers = &barrier;
        image->layout = layout;
        image->attachment_dirty = 0;
    }
    backend->vk.vkCmdPipelineBarrier2(command_buffer, &dependency);
    backend->bind_counters.barriers++;
}

bool dispatch_cropped_depth(s_target_entry *owner, s_gpu_image *destination,
                            const s_cropped_depth_parameters &params, uint32_t dest_x, uint32_t dest_y)
{
    s_vulkan_backend *backend = g_vk;
    if (backend == 0)
        return false;
    const VkCommandBuffer cmd = vulkan_recording::recording_command_buffer(backend, "cropped_depth_gather");
    if (!cmd)
        return false;
    s_vulkan_image *src = reinterpret_cast<s_vulkan_image *>(owner->image);
    s_vulkan_image *dst = reinterpret_cast<s_vulkan_image *>(destination);
    const uint32_t pitch = (params.width + 3u) & ~3u;
    const uint64_t pixels = (uint64_t)pitch * params.height;
    if (!src->sampled_view || (dst->desc.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0 ||
        (uint64_t)dest_x + params.width > dst->desc.width || (uint64_t)dest_y + params.height > dst->desc.height ||
        pixels * 5u > backend->properties.limits.maxStorageBufferRange)
        return false;
    const VkImageView stencil = cropped_depth_stencil_view(backend, src);
    const s_vulkan_pipeline *pipeline = stencil ? cropped_depth_pipeline(backend) : 0;
    if (!pipeline)
        return false;

    // Full and cropped gathers share the device-local scratch buffer and its fence retirement.
    const uint32_t size = (uint32_t)(pixels * 5u);
    s_vulkan_buffer *&buffer = backend->depth_gather_buffer;
    if (!buffer || buffer->size < size) {
        s_vulkan_buffer *grown = vulkan_buffer_create(backend, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT, k_vulkan_memory_gpu);
        if (!grown)
            return false;
        vulkan_buffer_retire(backend, buffer);
        buffer = grown;
    }
    uint32_t offset;
    void *uniform = ring_alloc(sizeof(params), g_uniform_alignment, &offset);
    if (!uniform)
        return false;
    memcpy(uniform, &params, sizeof(params));
    const VkDescriptorSet set = vulkan_descriptor_set_allocate(backend, vulkan_pipeline_set_layout(pipeline, 0), 1);
    if (!set)
        return false;

    const VkDescriptorImageInfo images[] = {
        { VK_NULL_HANDLE, src->sampled_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
        { VK_NULL_HANDLE, stencil, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
    };
    const VkDescriptorBufferInfo buffers[] = {
        { buffer->buffer, 0, size },
        { vulkan_gpu_native_buffer(ring_buffer())->buffer, offset, sizeof(params) },
    };
    VkWriteDescriptorSet writes[4] = {};
    for (uint32_t i = 0; i < 4; i++) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = set;
        writes[i].dstBinding = i;
        writes[i].descriptorCount = 1;
    }
    writes[0].descriptorType = writes[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    writes[0].pImageInfo = &images[0];
    writes[1].pImageInfo = &images[1];
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[2].pBufferInfo = &buffers[0];
    writes[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[3].pBufferInfo = &buffers[1];
    backend->vk.vkUpdateDescriptorSets(backend->device, 4, writes, 0, 0);

    end_pass();
    vulkan_cmd_flush_memexports(backend);
    const VkMemoryBarrier2 reuse = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2, 0,
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_ACCESS_2_SHADER_WRITE_BIT };
    cropped_depth_transition(backend, cmd, src, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              backend->shader_stages, VK_ACCESS_2_SHADER_READ_BIT, &reuse);
    vulkan_cmd_bind_pipeline_native(backend, pipeline);
    vulkan_recording::record_descriptor_sets(backend, cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->layout, 0, 1,
                                             &set, 0, 0);
    vulkan_cmd_dispatch_inline(backend, (params.width + 31u) / 32u, (params.height + 7u) / 8u, 1);

    const VkMemoryBarrier2 transfer = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2, 0,
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_ACCESS_2_SHADER_WRITE_BIT,
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_READ_BIT };
    cropped_depth_transition(backend, cmd, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, &transfer);
    VkBufferImageCopy copies[2] = {};
    copies[0].bufferRowLength = pitch;
    copies[0].imageSubresource = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0, 1 };
    copies[0].imageOffset = { (int32_t)dest_x, (int32_t)dest_y, 0 };
    copies[0].imageExtent = { params.width, params.height, 1 };
    copies[1] = copies[0];
    copies[1].bufferOffset = pixels * sizeof(float);
    copies[1].imageSubresource.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    backend->vk.vkCmdCopyBufferToImage(cmd, buffer->buffer, dst->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 2, copies);
    cropped_depth_transition(backend, cmd, dst, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              backend->shader_stages, VK_ACCESS_2_SHADER_READ_BIT, 0);
    return true;
}

bool resolve_cropped_depth(s_target_entry *source, s_gpu_image *destination, const int rect[4], uint32_t dest_x,
                           uint32_t dest_y, uint32_t w, uint32_t h)
{
    if (g_gpu->caps.kind != k_gpu_backend_vulkan || source->back_buffer || source->edram_base == 0xffffffffu ||
        source->msaa != 2u || source->host_format != k_gpu_format_d32_sfloat_s8_uint || source->pending_clear_flags != 0 ||
        source->image == destination || rect[0] < 0 || rect[1] < 0 ||
        (uint64_t)(uint32_t)rect[0] + w != (uint32_t)rect[2] ||
        (uint64_t)(uint32_t)rect[1] + h != (uint32_t)rect[3])
        return false;
    uint32_t index;
    if (!map_find(&g_edram_owner, edram_key(source->edram_base), &index) || index == 0 || index > g_targets.count)
        return false;
    s_target_entry *owner = &g_targets.items[index - 1u];
    if (owner == source || owner->image == 0 || owner->image == destination || owner->edram_base != source->edram_base ||
        owner->content == 0 || owner->content == source->content || owner->pending_clear_flags != 0 ||
        !owner->depth || owner->msaa > 1u || owner->host_format != source->host_format ||
        (owner->scale != 0 ? owner->scale : g_res_scale) != 1u || edram_pitch_tiles(owner) != edram_pitch_tiles(source) ||
        edram_sample0((uint32_t)rect[2] - 1u) >= owner->width ||
        (owner->msaa == 0 ? edram_sample0((uint32_t)rect[3] - 1u) : (uint32_t)rect[3] - 1u) >= owner->height)
        return false;
    for (const s_edram_clear &pending : g_edram_clears)
        if (pending.base == source->edram_base + 1u && pending.depth && pending.serial > source->content)
            return false;
    const s_cropped_depth_parameters params = { w, h, (uint32_t)rect[0], (uint32_t)rect[1], owner->msaa == 0 };
    // The original view still needs a full gather if it is later used as an attachment.
    return dispatch_cropped_depth(owner, destination, params, dest_x, dest_y);
}
#endif

void materialize_target_contents(s_target_entry *entry)
{
    if (entry == 0 || entry->image == 0)
        return;
    if (!entry->pending_clear_flags) {
        restore_color_snapshot(entry);
        return;
    }
    s_gpu_clear clear = {};
    clear.flags = entry->pending_clear_flags;
    clear.color_mask = entry->depth ? 0u : 1u;
    memcpy(clear.color, entry->pending_clear_color, sizeof(clear.color));
    clear.depth = entry->pending_clear_depth;
    clear.stencil = entry->pending_clear_stencil;
    entry->pending_clear_flags = 0;
    s_gpu_image *image = entry->image;
    if (entry->depth)
        begin_pass(0, 0, entry, &clear);
    else if (begin_pass(&image, 1, 0, &clear))
        unlink_color_snapshot(entry); // one color attachment, the load clear covers the whole image
    end_pass();
    restore_color_snapshot(entry); // only still linked if the standalone clear could not begin
}

// Sampling and EDRAM reads can use the immutable snapshot without filling the
// spare. A transfer reader must put the snapshot back into its sampled layout.
s_gpu_image *target_read_image(s_target_entry *target)
{
    if (target->pending_clear_flags) materialize_target_contents(target);
    return target->snapshot_texture ? g_textures.items[target->snapshot_texture - 1u].image : target->image;
}

void materialize_pending_7e3(s_texture_entry *entry)
{
    if (entry == 0 || entry->pending_7e3_owner == 0)
        return;
    const uint32_t owner_idx = entry->pending_7e3_owner;
    const uint32_t content = entry->pending_7e3_content;
    entry->pending_7e3_owner = 0;
    entry->pending_7e3_content = 0;
    if (g_pending_7e3_texture == (uint32_t)(entry - g_textures.items) + 1u)
        g_pending_7e3_texture = 0;
    if (owner_idx > g_targets.count || entry->image == 0)
        return;
    s_target_entry *owner = &g_targets.items[owner_idx - 1u];
    if (owner->image == 0 || owner->content != content)
        return;
    xe_layout_call_once(g_vk_reinterpret_7e3_initialized, g_frame_open != 0,
                        gpu_cmd_reinterpret_7e3, g_gpu, target_read_image(owner), entry->image);
    finish_resolve_conversion(entry->image);
    if (!gpu_rendering(g_gpu, 0, 0))
        clear_pass_attachments();
}

void materialize_pending_7e3_for_owner(const s_target_entry *owner)
{
    if (owner == 0 || g_pending_7e3_texture == 0 || g_pending_7e3_texture > g_textures.count)
        return;
    s_texture_entry *pending = &g_textures.items[g_pending_7e3_texture - 1u];
    if (pending->pending_7e3_owner == (uint32_t)(owner - g_targets.items) + 1u)
        materialize_pending_7e3(pending);
}

void edram_sync(s_target_entry *entry)
{
    edram_sync_owner(entry);
    edram_apply_clear(entry);
}

void edram_sync_owner(s_target_entry *entry)
{
    if (entry == 0 || entry->back_buffer || entry->edram_base == 0xffffffffu || entry->image == 0)
        return;
    uint32_t index = 0;
    if (!map_find(&g_edram_owner, edram_key(entry->edram_base), &index) || index == 0 || index > g_targets.count)
        return;
    s_target_entry *owner = &g_targets.items[index - 1u];
    if (owner == entry || owner->image == 0 || owner->edram_base != entry->edram_base || owner->content == 0 ||
        entry->content == owner->content)
        return;
    s_gpu_image *from = target_read_image(owner);
    entry->content = owner->content;
    if (entry->depth && !owner->depth && owner->restored_texture != 0 && owner->restored_content == owner->content) {
        // a color surface over this depth buffer's EDRAM, drawn from a depth resolve of it (restore_ldr_hdr_depth
        // after predicated tiling): its bits are that depth and stencil again. The two surfaces share the EDRAM
        // pitch (widths in whole 80-sample tiles) and height, so texel (x, y) is the same sample in both.
        s_texture_entry *texture = owner->restored_texture <= g_textures.count ? &g_textures.items[owner->restored_texture - 1u] : 0;
        if (texture != 0 && texture->image != 0 && texture->depth_edram == entry->edram_base + 1u &&
            texture->host_format == entry->host_format) {
            s_gpu_image_desc desc = {};
            gpu_image_get_desc(g_gpu, texture->image, &desc);
            const uint32_t es = entry->scale != 0 ? entry->scale : g_res_scale;
            const uint32_t w = desc.width < entry->width * es ? desc.width : entry->width * es;
            const uint32_t h = desc.height < entry->height * es ? desc.height : entry->height * es;
            gpu_cmd_copy_depth(g_gpu, texture->image, 0, 0, w, h, entry->image, 0, 0);
            if (tracing()) {
                char text[160];
                snprintf(text, sizeof(text), "trace depth restore %ux%u into edram %x\n", w, h, entry->edram_base);
                port_write_error(text);
            }
        } else {
            report(owner->host_format * 0x10001u ^ entry->host_format ^ 0x72737464u,
                   "a depth restore from a texture of another format or surface (not carried)");
        }
        if (!gpu_rendering(g_gpu, 0, 0))
            clear_pass_attachments();
        return;
    }
    // a 4x view over a 1x or 2x surface's EDRAM: its samples (other pairings of sample counts go on as surfaces of
    // one sample count, below)
    if (entry->msaa == 2u && owner->msaa != 2u) {
        restore_color_snapshot(owner);
        from = owner->image;
        if (edram_gather_samples(owner, entry)) {
            if (!gpu_rendering(g_gpu, 0, 0))
                clear_pass_attachments();
            return;
        }
    }
    if (owner->width != entry->width || owner->height != entry->height || owner->depth != entry->depth)
        return;
    const uint32_t os = owner->scale != 0 ? owner->scale : g_res_scale;
    const uint32_t es = entry->scale != 0 ? entry->scale : g_res_scale;
    uint32_t owner_bw = 0, owner_bh = 0, owner_bytes = 0, entry_bw = 0, entry_bh = 0, entry_bytes = 0;
    gpu_format_block_info(owner->host_format, &owner_bw, &owner_bh, &owner_bytes);
    gpu_format_block_info(entry->host_format, &entry_bw, &entry_bh, &entry_bytes);
    if (owner->host_format == entry->host_format) {
        gpu_cmd_copy_image(g_gpu, from, 0, 0, owner->width * os, owner->height * os,
                           entry->image, 0, 0, 0, 0, entry->width * es, entry->height * es);
        unlink_color_snapshot(entry);
    } else if (texel_layout(owner->d3d_format) == k_xenos_format_2_10_10_10_float &&
               entry->host_format == k_gpu_format_a2b10g10r10_unorm && g_storage_a2b10g10r10 > 0) {
        restore_color_snapshot(entry);
        xe_layout_call_once(g_vk_reinterpret_7e3_initialized, g_frame_open != 0, gpu_cmd_reinterpret_7e3, g_gpu,
                            from, entry->image);
    } else if (!owner->depth && !entry->depth && texel_layout(owner->d3d_format) == k_xenos_format_8_8_8_8 &&
               texel_layout(entry->d3d_format) == k_xenos_format_8_8_8_8 &&
               format_is_gamma(owner->d3d_format) != format_is_gamma(entry->d3d_format)) {
        // One EDRAM through the other color space (Halo 3's water pass switches a base between 8_8_8_8_GAMMA and
        // 8_8_8_8): EDRAM holds the gamma target's codes, so the linear surface over a gamma owner's bits reads them
        // as plain unorm (encode), and a gamma surface over plain bits holds their degamma (decode). The direction is
        // what xenon-pc-comp 98d8188 and Halo3Rex 118dfa2 had to fix.
        restore_color_snapshot(entry);
        if (!xe_layout_call(gpu_cmd_convert_pwl, g_gpu, from, entry->image, format_is_gamma(owner->d3d_format) ? 1u : 0u))
            report(owner->host_format * 0x10001u ^ entry->host_format ^ 0x70776cu,
                   "an EDRAM alias between gamma and linear 8_8_8_8 the device cannot convert (not carried)");
    } else if (!owner->depth && !entry->depth && owner_bytes == entry_bytes && owner_bytes != 0) {
        report(owner->host_format * 0x10001u ^ entry->host_format, "an EDRAM alias between two formats copied by value");
        gpu_cmd_copy_image(g_gpu, from, 0, 0, owner->width * os, owner->height * os,
                           entry->image, 0, 0, 0, 0, entry->width * es, entry->height * es);
        unlink_color_snapshot(entry);
    } else {
        report(owner->host_format * 0x10001u ^ entry->host_format,
               "an EDRAM alias between depth formats or texels of other sizes (not carried)");
    }
    if (!gpu_rendering(g_gpu, 0, 0)) {
        if (owner->snapshot_texture) color_snapshot_sampleable(from);
        clear_pass_attachments();
    }
}

// A pending EDRAM clear at the entry's base newer than its contents: the rectangle is cleared in this surface (the
// tile-size surface that sits on that EDRAM now), which then owns the base, so the clear is applied once.
void edram_apply_clear(s_target_entry *entry)
{
    if (entry == 0 || entry->back_buffer || entry->edram_base == 0xffffffffu || entry->image == 0)
        return;
    const uint32_t es = entry->scale != 0 ? entry->scale : g_res_scale;
    for (s_edram_clear &pending : g_edram_clears) {
        if (pending.base != entry->edram_base + 1u || pending.serial <= entry->content || (pending.depth != 0) != (entry->depth != 0))
            continue;
        materialize_target_contents(entry);
        int32_t rect[4] = { pending.rect[0], pending.rect[1], pending.rect[2], pending.rect[3] };
        rect[2] = rect[2] > (int32_t)entry->width ? (int32_t)entry->width : rect[2];
        rect[3] = rect[3] > (int32_t)entry->height ? (int32_t)entry->height : rect[3];
        for (int i = 0; i < 4; i++)
            rect[i] *= (int32_t)es; // guest pixels -> host pixels (HALO3_RES_SCALE)
        if (rect[2] > rect[0] && rect[3] > rect[1]) {
            s_gpu_image *image = entry->image;
            const bool begun = entry->depth ? begin_pass(0, 0, entry, 0) : begin_pass(&image, 1, 0, 0);
            if (begun)
                gpu_cmd_clear_attachments(g_gpu, &pending.clear, rect, 1);
            end_pass();
            if (tracing()) {
                char text[160];
                snprintf(text, sizeof(text), "trace edram clear %x (%d,%d,%d,%d) into %p\n", entry->edram_base, rect[0], rect[1],
                         rect[2], rect[3], entry->key);
                port_write_error(text);
            }
        }
        entry->content = pending.serial;
        map_set(&g_edram_owner, edram_key(entry->edram_base), (uint32_t)(entry - g_targets.items) + 1u);
    }
}

// Marks `entry`'s prior EDRAM contents as discarded (about to be 100% overwritten by a full-target clear) so
// edram_sync_owner / edram_apply_clear will not waste GPU work copying or reinterpreting stale contents into it.
void edram_discard_contents(s_target_entry *entry)
{
    if (entry == 0)
        return;
    materialize_pending_7e3_for_owner(entry);
    entry->pending_clear_flags = 0;
    if (entry->back_buffer || entry->edram_base == 0xffffffffu || entry->image == 0)
        return;
    uint32_t index = 0;
    if (map_find(&g_edram_owner, edram_key(entry->edram_base), &index) && index != 0 && index <= g_targets.count) {
        s_target_entry *owner = &g_targets.items[index - 1u];
        if (owner != entry && owner->edram_base == entry->edram_base && owner->content != 0)
            entry->content = owner->content;
    }
    for (s_edram_clear &pending : g_edram_clears) {
        if (pending.base == entry->edram_base + 1u && (pending.depth != 0) == (entry->depth != 0))
            pending.base = 0;
    }
}

bool edram_needs_sync(const s_target_entry *entry)
{
    if (entry == 0 || entry->back_buffer || entry->edram_base == 0xffffffffu || entry->image == 0)
        return false;
    uint32_t index = 0;
    if (map_find(&g_edram_owner, edram_key(entry->edram_base), &index) && index != 0 && index <= g_targets.count) {
        const s_target_entry *owner = &g_targets.items[index - 1u];
        if (owner != entry && owner->image != 0 && owner->edram_base == entry->edram_base && owner->content != 0 &&
            entry->content != owner->content)
            return true;
    }
    for (const s_edram_clear &pending : g_edram_clears) {
        if (pending.base == entry->edram_base + 1u && pending.serial > entry->content &&
            (pending.depth != 0) == (entry->depth != 0))
            return true;
    }
    return false;
}

// If `entry` would need edram_sync_owner via gpu_cmd_reinterpret_7e3 from `owner->image` (and has no pending
// edram_apply_clear), returns `owner`; else NULL. Used by port_vk_resolve to reinterpret directly into the resolve
// texture without an intermediate copy through `entry->image`.
s_target_entry *edram_7e3_owner(s_target_entry *entry)
{
    if (entry == 0 || entry->back_buffer || entry->edram_base == 0xffffffffu || entry->image == 0 || entry->depth)
        return 0;
    uint32_t index = 0;
    if (!map_find(&g_edram_owner, edram_key(entry->edram_base), &index) || index == 0 || index > g_targets.count)
        return 0;
    s_target_entry *owner = &g_targets.items[index - 1u];
    if (owner == entry || owner->image == 0 || owner->edram_base != entry->edram_base || owner->content == 0 ||
        entry->content == owner->content)
        return 0;
    if (owner->depth || (entry->msaa == 2u && owner->msaa != 2u))
        return 0;
    if (owner->width != entry->width || owner->height != entry->height)
        return 0;
    if (owner->host_format == entry->host_format)
        return 0;
    if (g_storage_a2b10g10r10 < 0)
        g_storage_a2b10g10r10 = gpu_format_storage(g_gpu, k_gpu_format_a2b10g10r10_unorm) ? 1 : 0;
    if (texel_layout(owner->d3d_format) != k_xenos_format_2_10_10_10_float ||
        entry->host_format != k_gpu_format_a2b10g10r10_unorm || g_storage_a2b10g10r10 <= 0)
        return 0;
    for (const s_edram_clear &pending : g_edram_clears) {
        if (pending.base == entry->edram_base + 1u && pending.serial > entry->content && pending.depth == 0)
            return 0;
    }
    return owner;
}

// If `entry` would need edram_sync_owner via gpu_cmd_convert_pwl from `owner->image` (and has no pending
// edram_apply_clear), returns `owner` and writes `*out_encode`; else NULL. Used by port_vk_resolve to convert
// directly into the resolve texture without an intermediate copy through `entry->image`.
s_target_entry *edram_pwl_owner(s_target_entry *entry, uint32_t *out_encode)
{
    if (entry == 0 || entry->back_buffer || entry->edram_base == 0xffffffffu || entry->image == 0 || entry->depth)
        return 0;
    uint32_t index = 0;
    if (!map_find(&g_edram_owner, edram_key(entry->edram_base), &index) || index == 0 || index > g_targets.count)
        return 0;
    s_target_entry *owner = &g_targets.items[index - 1u];
    if (owner == entry || owner->image == 0 || owner->edram_base != entry->edram_base || owner->content == 0 ||
        entry->content == owner->content)
        return 0;
    if (owner->depth || (entry->msaa == 2u && owner->msaa != 2u))
        return 0;
    if (owner->width != entry->width || owner->height != entry->height)
        return 0;
    if (owner->host_format == entry->host_format)
        return 0;
    if (texel_layout(owner->d3d_format) != k_xenos_format_8_8_8_8 ||
        texel_layout(entry->d3d_format) != k_xenos_format_8_8_8_8 ||
        format_is_gamma(owner->d3d_format) == format_is_gamma(entry->d3d_format))
        return 0;
    for (const s_edram_clear &pending : g_edram_clears) {
        if (pending.base == entry->edram_base + 1u && pending.serial > entry->content && pending.depth == 0)
            return 0;
    }
    if (out_encode != 0)
        *out_encode = format_is_gamma(owner->d3d_format) ? 1u : 0u;
    return owner;
}

// Whether a draw reads or writes the depth surface (depth test, or the stencil test, which writes it too).
bool touches_depth(const s_port_vk_render_state &rs)
{
    return rs.z_enable != 0 || rs.stencil_enable != 0;
}

// The depth surface a pass attaches: the bound one, unless it is smaller than a bound color target and the draw or
// clear does not touch depth or stencil. The Xenos binds surfaces as EDRAM ranges each of its own size, so a draw that
// leaves depth alone covers its whole color target whatever depth surface is bound; a host render area lies within
// every attachment, so attaching the smaller one would clip it. (The split-screen composite,
// screen_postprocess_blit_player_view, copies each 864x320 view into the 1152x640 frame with a view's 864x320 depth
// surface still bound and Z off.) A later draw that does touch depth begins the pass again with it.
s_target_entry *pass_depth(bool touches)
{
    if (g_bound_depth == 0 || g_bound_depth->image == 0 || touches)
        return g_bound_depth;
    const uint32_t dw = g_bound_depth->host_width;
    const uint32_t dh = g_bound_depth->host_height;
    for (uint32_t i = 0; i < 4; i++) {
        if (g_bound_color[i] == 0 || g_bound_color[i]->image == 0)
            continue;
        if (g_bound_color[i]->host_width > dw || g_bound_color[i]->host_height > dh)
            return 0;
    }
    return g_bound_depth;
}

// Whether two bound color targets are one EDRAM range: the same surface, or two surfaces at one ColorBase (a surface
// over a texture level, edram_base 0xffffffff, and the back buffer are only ever the same as themselves).
bool same_edram(const s_target_entry *a, const s_target_entry *b)
{
    return a == b ||
           (a->edram_base != 0xffffffffu && a->edram_base == b->edram_base && !a->back_buffer && !b->back_buffer);
}

// The color slots a draw writes (bit i: render target i): the outputs its pixel shader exports (`outputs`, bit i: oCi)
// whose target has a component write enabled (D3DRS_COLORWRITEENABLEi).
uint32_t written_color_slots(const s_port_vk_render_state &rs, uint32_t outputs)
{
    uint32_t written = 0;
    for (uint32_t i = 0; i < 4; i++)
        if ((outputs & (1u << i)) != 0 && (rs.color_write_enable[i] & 0xfu) != 0)
            written |= 1u << i;
    return written;
}

// The color slots a draw or clear attaches (bit i: g_bound_color[i]); `written`: the slots it writes. The Xbox D3D
// lets one surface (or two at one EDRAM base) be bound as two render targets, and Halo 3's dynamic lights do so: they
// bind the LDR accumulation surface as both targets (render_objects_build_lighting gets surface 13 for surface0 and
// surface1; rasterizer_set_light_render_targets) for a shader that writes the light to oC0 and its HDR copy, 1/128 as
// bright, to oC1. Attached twice to one host rendering, the image is written by both attachments unordered
// (undefined; on NVIDIA each 8x8 tile kept one of the two sums: Arrival's cinematic lights shone on characters in a
// screen door of 8x8 blocks). As in Xenia (RenderTargetCache::Update, from the same Halo 3 lighting pass), a target is
// attached once, at the lowest of its slots the operation writes (the lowest of them when it writes none), and its
// other slots are left out.
uint32_t attached_color_slots(uint32_t written)
{
    uint32_t slots = 0;
    for (uint32_t i = 0; i < 4; i++) {
        const s_target_entry *target = g_bound_color[i];
        if (target == 0 || target->image == 0)
            continue;
        uint32_t keeper = i; // the slot of target's EDRAM that is attached
        for (uint32_t j = 0; j < 4; j++) {
            const s_target_entry *other = g_bound_color[j];
            if (j == i || other == 0 || other->image == 0 || !same_edram(target, other))
                continue;
            const uint32_t wk = (written >> keeper) & 1u, wj = (written >> j) & 1u;
            if (wj > wk || (wj == wk && j < keeper))
                keeper = j;
        }
        if (keeper == i) {
            slots |= 1u << i;
        } else if (tracing()) {
            char text[160];
            snprintf(text, sizeof(text), "trace   rt%u %p is rt%u's EDRAM (base %x): not attached\n", i, target->key,
                     keeper, target->edram_base);
            port_write_error(text);
        }
    }
    return slots;
}

// Makes the bound targets of `slots` (attached_color_slots) the current rendering (begun anew only when they changed);
// `touches`: whether what is about to be drawn or cleared touches depth or stencil (pass_depth).
// When `out_slots != 0`, unwritten color slots that would require an edram_sync are dropped before opening a new pass
// so no wasted convert_pwl/reinterpret_7e3 runs and the untouched target is not falsely marked as EDRAM owner.
bool ensure_pass(bool touches, uint32_t slots, uint32_t written = 0xfu, uint32_t *out_slots = 0)
{
    s_gpu_image *colors[4];
    s_target_entry *targets[4];
    uint32_t count = 0;
    auto gather_colors = [&]() {
        count = 0;
        for (uint32_t i = 0; i < 4; i++) {
            targets[i] = (slots & (1u << i)) != 0 ? g_bound_color[i] : 0;
            colors[i] = targets[i] != 0 ? targets[i]->image : 0;
            if (colors[i] != 0)
                count = i + 1u;
        }
    };
    gather_colors();
    s_target_entry *depth_target = pass_depth(touches);
    s_gpu_image *depth = depth_target != 0 ? depth_target->image : 0;
    if (count == 0 && depth == 0) {
        report(0x6e6f7274u, "a draw or clear with no render target bound (skipped)");
        return false;
    }
    auto pass_matches = [&]() -> bool {
        bool colors_same = gpu_rendering(g_gpu, 0, 0);
        for (uint32_t i = 0; i < 4 && colors_same; i++)
            colors_same = g_pass_color[i] == colors[i];
        if (colors_same && g_pass_depth == depth)
            return true;
        // A draw that does not touch depth/stencil can keep the currently bound depth attachment as long as its
        // dimensions cover every bound color target, avoiding a needless vkCmdEndRendering -> vkCmdBeginRendering split.
        if (!touches && colors_same && g_pass_depth != 0 && count != 0 &&
            (g_xenos || (depth != 0 && g_pass_depth_format == depth_target->host_format))) {
            bool fits = true;
            for (uint32_t i = 0; i < 4 && fits; i++) {
                if (colors[i] == 0)
                    continue;
                fits = g_pass_depth_width >= targets[i]->host_width && g_pass_depth_height >= targets[i]->host_height;
            }
            if (fits)
                return true;
        }
        return false;
    };
    if (pass_matches()) {
        if (out_slots != 0)
            *out_slots = slots;
        return true;
    }
    if (out_slots != 0 && (slots & ~written) != 0) {
        uint32_t filtered = slots;
        for (uint32_t i = 0; i < 4; i++) {
            if ((filtered & (1u << i)) != 0 && (written & (1u << i)) == 0 && targets[i] != 0 &&
                edram_needs_sync(targets[i]) && ((filtered & ~(1u << i)) != 0 || depth != 0))
                filtered &= ~(1u << i);
        }
        if (filtered != slots) {
            slots = filtered;
            gather_colors();
            if (pass_matches()) {
                *out_slots = slots;
                return true;
            }
        }
    }
    // the targets take up what their EDRAM holds, then own it
    for (uint32_t i = 0; i < 4; i++) {
        materialize_pending_7e3_for_owner(targets[i]);
        edram_sync(targets[i]);
    }
    materialize_pending_7e3_for_owner(depth_target);
    edram_sync(depth_target);
    // Load operations cover only the render area common to all attachments. Preserve
    // the old pixels outside that area even when this target has a pending full clear.
    uint32_t discard_snapshots = 0;
    for (uint32_t i = 0; i < count; ++i) {
        auto *t = targets[i];
        if (!t || !t->snapshot_texture) continue;
        bool full_clear = (t->pending_clear_flags & k_gpu_clear_color) &&
            (!depth_target || (depth_target->host_width >= t->host_width && depth_target->host_height >= t->host_height));
        for (uint32_t j = 0; full_clear && j < count; ++j)
            if (targets[j] && (targets[j]->host_width < t->host_width || targets[j]->host_height < t->host_height))
                full_clear = false;
        if (full_clear) discard_snapshots |= 1u << i;
        else restore_color_snapshot(t);
    }
    s_gpu_clear pass_clear = {};
    bool have_pass_color = false;
    uint32_t extra_clear_mask = 0;
    float extra_clear_colors[4][4];
    for (uint32_t i = 0; i < count; i++) {
        if (targets[i] != 0 && (targets[i]->pending_clear_flags & k_gpu_clear_color) != 0) {
            if (!have_pass_color) {
                pass_clear.flags |= k_gpu_clear_color;
                pass_clear.color_mask |= 1u << i;
                memcpy(pass_clear.color, targets[i]->pending_clear_color, sizeof(pass_clear.color));
                have_pass_color = true;
            } else if (memcmp(pass_clear.color, targets[i]->pending_clear_color, sizeof(pass_clear.color)) == 0) {
                pass_clear.color_mask |= 1u << i;
            } else {
                extra_clear_mask |= 1u << i;
                memcpy(extra_clear_colors[i], targets[i]->pending_clear_color, sizeof(extra_clear_colors[i]));
            }
            targets[i]->pending_clear_flags &= ~k_gpu_clear_color;
        }
    }
    if (depth_target != 0 && (depth_target->pending_clear_flags & (k_gpu_clear_depth | k_gpu_clear_stencil)) != 0) {
        if ((depth_target->pending_clear_flags & k_gpu_clear_depth) != 0) {
            pass_clear.flags |= k_gpu_clear_depth;
            pass_clear.depth = depth_target->pending_clear_depth;
        }
        if ((depth_target->pending_clear_flags & k_gpu_clear_stencil) != 0) {
            pass_clear.flags |= k_gpu_clear_stencil;
            pass_clear.stencil = depth_target->pending_clear_stencil;
        }
        depth_target->pending_clear_flags &= ~(k_gpu_clear_depth | k_gpu_clear_stencil);
    }
    if (!begin_pass(colors, count, depth != 0 ? depth_target : 0, pass_clear.flags != 0 ? &pass_clear : 0))
        return false;
    if (extra_clear_mask != 0) {
        for (uint32_t i = 0; i < count; i++) {
            if ((extra_clear_mask & (1u << i)) == 0)
                continue;
            s_gpu_clear extra = {};
            extra.flags = k_gpu_clear_color;
            extra.color_mask = 1u << i;
            memcpy(extra.color, extra_clear_colors[i], sizeof(extra.color));
            gpu_cmd_clear_attachments(g_gpu, &extra, 0, 0);
        }
    }
    bool aliases_depth = false;
    for (uint32_t i = 0; i < 4; i++) {
        if (discard_snapshots & (1u << i)) unlink_color_snapshot(targets[i]);
        edram_written(targets[i]);
        if (!aliases_depth && targets[i] != 0 && !targets[i]->depth && targets[i]->edram_base != 0xffffffffu) {
            if (g_depth_resolve_edram_overflow) {
                aliases_depth = true;
            } else {
                const uint32_t key = targets[i]->edram_base + 1u;
                for (uint32_t j = 0; j < g_depth_resolve_edram_count; j++) {
                    if (g_depth_resolve_edram[j] == key) {
                        aliases_depth = true;
                        break;
                    }
                }
            }
        }
    }
    g_pass_color_aliases_depth = aliases_depth;
    edram_written(depth_target);
    if (out_slots != 0)
        *out_slots = slots;
    return true;
}

// ---- textures ----

uint32_t touch_generation(const void *key)
{
    uint32_t g = 0;
    map_find(&g_touches, key_of(key), &g);
    return g;
}

// The texture converter's output formats (port/data/texture_convert.h e_pc_texture_format, numbered as Vulkan's formats)
// as the backends' (e_gpu_format); undefined for any other.
uint32_t gpu_format_of_pc(uint32_t pc_format)
{
    switch (pc_format) {
    case k_pc_format_b4g4r4a4_unorm_pack16: return k_gpu_format_b4g4r4a4_unorm_pack16;
    case k_pc_format_b5g6r5_unorm_pack16: return k_gpu_format_b5g6r5_unorm_pack16;
    case k_pc_format_a1r5g5b5_unorm_pack16: return k_gpu_format_a1r5g5b5_unorm_pack16;
    case k_pc_format_r8_unorm: return k_gpu_format_r8_unorm;
    case k_pc_format_r8_snorm: return k_gpu_format_r8_snorm;
    case k_pc_format_r8g8_unorm: return k_gpu_format_r8g8_unorm;
    case k_pc_format_r8g8_snorm: return k_gpu_format_r8g8_snorm;
    case k_pc_format_r8g8b8a8_unorm: return k_gpu_format_r8g8b8a8_unorm;
    case k_pc_format_r8g8b8a8_snorm: return k_gpu_format_r8g8b8a8_snorm;
    case k_pc_format_b8g8r8a8_unorm: return k_gpu_format_b8g8r8a8_unorm;
    case k_pc_format_r16_unorm: return k_gpu_format_r16_unorm;
    case k_pc_format_r16_snorm: return k_gpu_format_r16_snorm;
    case k_pc_format_r16_sfloat: return k_gpu_format_r16_sfloat;
    case k_pc_format_r16g16_unorm: return k_gpu_format_r16g16_unorm;
    case k_pc_format_r16g16_snorm: return k_gpu_format_r16g16_snorm;
    case k_pc_format_r16g16_sfloat: return k_gpu_format_r16g16_sfloat;
    case k_pc_format_r16g16b16a16_unorm: return k_gpu_format_r16g16b16a16_unorm;
    case k_pc_format_r16g16b16a16_snorm: return k_gpu_format_r16g16b16a16_snorm;
    case k_pc_format_r16g16b16a16_sfloat: return k_gpu_format_r16g16b16a16_sfloat;
    case k_pc_format_r32_sfloat: return k_gpu_format_r32_sfloat;
    case k_pc_format_r32g32_sfloat: return k_gpu_format_r32g32_sfloat;
    case k_pc_format_r32g32b32a32_sfloat: return k_gpu_format_r32g32b32a32_sfloat;
    case k_pc_format_bc1_rgba_unorm: return k_gpu_format_bc1_rgba_unorm;
    case k_pc_format_bc1_rgba_srgb: return k_gpu_format_bc1_rgba_srgb;
    case k_pc_format_bc2_unorm: return k_gpu_format_bc2_unorm;
    case k_pc_format_bc2_srgb: return k_gpu_format_bc2_srgb;
    case k_pc_format_bc3_unorm: return k_gpu_format_bc3_unorm;
    case k_pc_format_bc3_srgb: return k_gpu_format_bc3_srgb;
    case k_pc_format_bc4_unorm: return k_gpu_format_bc4_unorm;
    case k_pc_format_bc5_unorm: return k_gpu_format_bc5_unorm;
    case k_pc_format_bc5_snorm: return k_gpu_format_bc5_snorm;
    case k_pc_format_a2b10g10r10_unorm_pack32: return k_gpu_format_a2b10g10r10_unorm;
    default: return k_gpu_format_undefined;
    }
}

uint32_t swizzle_of(uint8_t xenos)
{
    switch (xenos) {
    case k_xenos_swizzle_x: return k_gpu_swizzle_r;
    case k_xenos_swizzle_y: return k_gpu_swizzle_g;
    case k_xenos_swizzle_z: return k_gpu_swizzle_b;
    case k_xenos_swizzle_w: return k_gpu_swizzle_a;
    case k_xenos_swizzle_0: return k_gpu_swizzle_zero;
    default: return k_gpu_swizzle_one;
    }
}

uint32_t view_type_of(uint32_t dimension)
{
    switch (dimension) {
    case k_xenos_dimension_3d: return k_gpu_view_3d;
    case k_xenos_dimension_cube: return k_gpu_view_cube;
    case k_xenos_dimension_array: return k_gpu_view_2d_array;
    default: return k_gpu_view_2d;
    }
}

s_texture_entry *texture_entry(const void *key, bool create)
{
    uint32_t index;
    if (map_find(&g_texture_index, key_of(key), &index))
        return &g_textures.items[index];
    if (!create)
        return 0;
    s_texture_entry *entry = g_textures.push();
    if (entry == 0)
        return 0;
    entry->key = key;
    map_set(&g_texture_index, key_of(key), g_textures.count - 1u);
    return entry;
}

// The image a Resolve left in this texture's memory: its own entry's, or the one written through another header
// over the same base address with the same texels (texel_layout) and size; 0 when the memory holds no resolve.
s_texture_entry *resolved_entry(const void *key, const s_port_d3d_texture_source &source)
{
    if (g_resolved_headers.contains(key)) {
        s_texture_entry *own = texture_entry(key, false);
        if (own != 0 && own->resolved && own->image != 0)
            return own;
    }
    if (!source.base || !g_resolved_bases.contains(source.base))
        return 0;
    uint32_t index = 0;
    {
        s_memory_records_lock lock;
        if (!map_find(&g_resolved_memory, key_of(source.base), &index))
            index = 0;
    }
    if (index == 0 || index > g_textures.count)
        return 0;
    s_texture_entry *alias = &g_textures.items[index - 1u];
    if (!alias->resolved || alias->image == 0 || alias->base != source.base || alias->width != source.desc.width ||
        alias->height != source.desc.height)
        return 0;
    const uint32_t layout = texel_layout(source.desc.d3d_format);
    if (texel_layout(alias->d3d_format) == layout)
        return alias;
    // A depth resolve read through a 32-bit color header (restore_ldr_hdr_depth samples the 24_8 bits as 8_8_8_8 to
    // write them back into EDRAM): the PC shaders read that texture as depth, which the depth image's view gives.
    if (alias->depth_edram != 0 && (layout == k_xenos_format_8_8_8_8 || layout == k_xenos_format_2_10_10_10 ||
                                    layout == 22u /* 24_8 */ || layout == 23u /* 24_8_FLOAT */ ||
                                    layout == k_xenos_format_32_float))
        return alias;
    return 0;
}

// resolved_entry of each sampler slot, kept from draw to draw: note_depth_restore and every texture fetch of
// xe_texture_set ask for it, a lock and two map lookups per bound texture per draw. It is looked up again when the
// slot's texture (header, base, size, format) differs or the resolve state changed: g_resolve_epoch advances with
// everything that changes what resolved_entry finds (a Resolve, an image released, a texture the game writes, memory
// the pager streams over; resolve_state_changed). The entry is kept by index: a texture conversion may grow g_textures.
uint32_t g_resolve_epoch = 1; // atomic: the pager streams in on the main thread
struct s_slot_resolved {
    uint32_t epoch; // g_resolve_epoch when it was looked up (0: never)
    const void *key, *base;
    uint32_t width, height, d3d_format;
    uint32_t texture; // g_textures index + 1, 0: no resolve
};
s_slot_resolved g_slot_resolved[PORT_VK_SAMPLER_COUNT];

void resolve_state_changed()
{
    if (__atomic_add_fetch(&g_resolve_epoch, 1u, __ATOMIC_ACQ_REL) == 0)
        __atomic_add_fetch(&g_resolve_epoch, 1u, __ATOMIC_ACQ_REL); // 0 means never looked up
}

s_texture_entry *slot_resolved(const s_port_vk_draw *draw, uint32_t slot)
{
    const s_port_vk_texture &texture = draw->samplers[slot].texture;
    s_slot_resolved &kept = g_slot_resolved[slot];
    const uint32_t epoch = __atomic_load_n(&g_resolve_epoch, __ATOMIC_ACQUIRE); // before the lookup it validates
    if (kept.epoch != epoch || kept.key != texture.key || kept.base != texture.source.base ||
        kept.width != texture.source.desc.width || kept.height != texture.source.desc.height ||
        kept.d3d_format != texture.source.desc.d3d_format || kept.texture > g_textures.count) {
        s_texture_entry *resolved = resolved_entry(texture.key, texture.source);
        kept.epoch = epoch;
        kept.key = texture.key;
        kept.base = texture.source.base;
        kept.width = texture.source.desc.width;
        kept.height = texture.source.desc.height;
        kept.d3d_format = texture.source.desc.d3d_format;
        kept.texture = resolved != 0 ? (uint32_t)(resolved - g_textures.items) + 1u : 0u;
    }
    return kept.texture != 0 ? &g_textures.items[kept.texture - 1u] : 0;
}

void release_texture_image(s_texture_entry *entry)
{
    if (entry->image == 0)
        return;
    resolve_state_changed(); // a resolved image resolved_entry may have found
    return_color_snapshot(entry);
    // draws already recorded, or frames in flight, may still sample it
    gpu_image_retire(g_gpu, entry->image);
    for (int i = 0; i < 2; i++) {
        if (entry->converted[i] != 0)
            gpu_image_retire(g_gpu, entry->converted[i]);
        entry->converted[i] = 0;
        entry->converted_serial[i] = 0;
    }
    if (entry->guest != 0)
        gpu_image_retire(g_gpu, entry->guest);
    entry->guest = 0;
    entry->guest_source = 0;
    if (entry->packed != 0)
        gpu_image_retire(g_gpu, entry->packed);
    entry->packed = 0;
    entry->depth_edram = 0;
    entry->depth_float = 0;
    entry->linear_storage = 0;
    entry->pending_7e3_owner = 0;
    entry->pending_7e3_content = 0;
    if (g_pending_7e3_texture == (uint32_t)(entry - g_textures.items) + 1u)
        g_pending_7e3_texture = 0;
    entry->image = 0;
}

// A resolved image sampled through a header of the other color space than its storage (an 8_8_8_8 header with or
// without GPUSIGN_GAMMA): the console's memory holds 8-bit values, a gamma fetch degammas them, a plain fetch reads
// them as they are. A gamma target resolved into a gamma texture keeps its PWL codes in memory (linear_storage) while
// its image here is linear, so a plain fetch of it reads the PWL encode; any other resolve's memory holds what its
// image holds (a gamma target into a plain texture: the decoded linear light), so a gamma fetch of it reads the decode.
// Converted into a side image once per resolve (one per direction so flipping never retires images or bumps view_epoch).
// A version 3 pack degammas a gamma fetch in the shader (io.odin fetch_gamma), so every fetch reads the memory's values
// there — except when sampling a linear_storage resolve through a gamma header (out_skip_gamma != 0), where we sample
// the 16-bit linear image directly and clear derived[n][0] so the shader skips pwl_gamma_to_linear().
s_gpu_image *resolved_image(s_texture_entry *resolved, uint32_t fetch_format, bool *out_skip_gamma = 0,
                            bool *cacheable = 0)
{
    if (out_skip_gamma != 0)
        *out_skip_gamma = false;
    materialize_pending_7e3(resolved);
    if (resolved->depth_edram != 0 || texel_layout(fetch_format) != k_xenos_format_8_8_8_8 ||
        texel_layout(resolved->d3d_format) != k_xenos_format_8_8_8_8)
        return resolved->image;
    if (g_xenos && out_skip_gamma != 0 && resolved->linear_storage != 0 && format_is_gamma(fetch_format)) {
        *out_skip_gamma = true;
        return resolved->image;
    }
    const bool fetch_gamma = !g_xenos && format_is_gamma(fetch_format);
    if (fetch_gamma == (resolved->linear_storage != 0))
        return resolved->image;
    const uint32_t encode = resolved->linear_storage != 0 ? 1u : 0u;
    s_gpu_image_desc desc = {};
    gpu_image_get_desc(g_gpu, resolved->image, &desc);
    if (desc.mip_levels != 1 || desc.array_layers != 1 || desc.view_type != k_gpu_view_2d) {
        report(resolved->d3d_format ^ fetch_format ^ 0x70776376u,
               "a resolve fetched through the other color space with levels or layers (not converted)");
        return resolved->image;
    }
    if (resolved->converted[encode] != 0 && resolved->converted_serial[encode] == resolved->resolve_serial)
        return resolved->converted[encode];
    if (resolved->converted[encode] == 0) {
        s_gpu_image_desc converted = desc;
        converted.format = encode ? k_gpu_format_r8g8b8a8_unorm : k_gpu_format_r16g16b16a16_unorm;
        converted.usage = k_gpu_image_sampled | k_gpu_image_storage;
        converted.swizzle = 0;
        resolved->converted[encode] = gpu_image_create(g_gpu, &converted);
    }
    if (resolved->converted[encode] == 0 ||
        !xe_layout_call(gpu_cmd_convert_pwl, g_gpu, resolved->image, resolved->converted[encode], encode)) {
        if (cacheable != 0)
            *cacheable = false;
        if (!gpu_rendering(g_gpu, 0, 0))
            clear_pass_attachments();
        report(resolved->d3d_format ^ fetch_format ^ 0x70776366u,
               "a resolve fetched through the other color space the device cannot convert (sampled as stored)");
        return resolved->image;
    }
    resolved->converted_serial[encode] = resolved->resolve_serial;
    clear_pass_attachments();
    return resolved->converted[encode];
}

// HALO3_RES_SCALE: a color resolve with more host texels per guest texel than the draw's render target (a scaled
// resolve sampled by a guest-resolution post-process pass, target_res_scale) is read through its box average at the
// target's scale, made once per resolve (resolve_downscale.hlsl). Those passes place their taps for guest texels
// (downsample_4x4_block's four bilinear taps +-1 texel cover a 4 x 4 block: at 3x, 16 of its 144 host texels), so the
// full image aliases into them (edges and highlights crawl in the lobby blur and the bloom); the copy holds what the
// console's own resolve would, supersampled. Depth resolves keep their texels (an average depth is no surface's).
s_gpu_image *resolved_at_draw_scale(s_texture_entry *resolved, s_gpu_image *image, bool *cacheable = 0)
{
    const uint32_t texture_scale = resolved->resolved_scale != 0 ? resolved->resolved_scale : g_res_scale;
    const uint32_t draw_scale = bound_res_scale();
    if (image == 0 || resolved->depth_edram != 0 || texture_scale <= draw_scale || texture_scale % draw_scale != 0)
        return image;
    const uint32_t n = texture_scale / draw_scale;
    s_gpu_image_desc desc = {};
    gpu_image_get_desc(g_gpu, image, &desc);
    if (desc.mip_levels != 1 || desc.array_layers != 1 || desc.view_type != k_gpu_view_2d ||
        desc.width % n != 0 || desc.height % n != 0) {
        report(resolved->d3d_format ^ 0x67737473u, "a scaled resolve with levels or layers read at guest resolution "
                                                    "(sampled at host resolution)");
        return image;
    }
    if (resolved->guest != 0 && resolved->guest_source == image && resolved->guest_serial == resolved->resolve_serial)
        return resolved->guest;
    s_gpu_image_desc guest = desc;
    guest.width = desc.width / n;
    guest.height = desc.height / n;
    guest.usage = k_gpu_image_sampled | k_gpu_image_storage;
    guest.swizzle = 0; // the pass reads the source through its view's swizzle
    if (resolved->guest != 0) {
        s_gpu_image_desc current = {};
        gpu_image_get_desc(g_gpu, resolved->guest, &current);
        if (current.width != guest.width || current.height != guest.height || current.format != guest.format) {
            gpu_image_retire(g_gpu, resolved->guest);
            resolved->guest = 0;
        }
    }
    if (resolved->guest == 0 && gpu_format_storage(g_gpu, guest.format))
        resolved->guest = gpu_image_create(g_gpu, &guest);
    if (resolved->guest == 0 || !xe_layout_call(gpu_cmd_downscale_image, g_gpu, image, resolved->guest)) {
        if (cacheable != 0)
            *cacheable = false;
        if (!gpu_rendering(g_gpu, 0, 0))
            clear_pass_attachments();
        report(desc.format ^ 0x67737466u, "a scaled resolve the device cannot box-filter to guest resolution (sampled "
                                          "at host resolution)");
        return image;
    }
    resolved->guest_source = image;
    resolved->guest_serial = resolved->resolve_serial;
    if (tracing()) {
        char text[160];
        snprintf(text, sizeof(text), "trace guest copy %ux%u -> %ux%u (format %u) of tex %p\n", desc.width, desc.height,
                 guest.width, guest.height, desc.format, resolved->key);
        port_write_error(text);
    }
    clear_pass_attachments();
    return resolved->guest;
}

// A depth resolve fetched through a 32-bit color header (k_8_8_8_8) by a version 3 pack's retail shader. The Xenos
// resolve leaves the 24_8 words (depth << 8) | stencil in memory; the fetch splits each into X = the stencil and Y, Z,
// W = the depth's bytes from the low one, then swizzles them as the header says (A8R8G8B8 0x18280186: R = Z, G = Y,
// B = X). Halo 3's sniper_scope pass reads the stencil so: its stencil_sampler is surface 54, an A8R8G8B8 header over
// surface 53's memory, the scope screens' depth resolve (player_view_resolve_depth_of_field_source), and .z >= 64/255
// marks the units that glow on the sniper rifle's scope screen and the Ghost's display. The depth image's sampled view
// has the depth in every channel instead (what a version 2 pack's DX11 shaders read), so the words are packed into an
// R8G8B8A8 image (depth_pack.hlsl) once per resolve and sampled through the header's swizzle.
gpu_view packed_depth_view(s_texture_entry *resolved, uint32_t fetch_format, bool *cacheable,
                            s_gpu_image **out_image)
{
    const uint32_t swizzle = GPU_SWIZZLE(swizzle_of((uint8_t)xenos_d3dformat_swizzle(fetch_format, 0)),
                                         swizzle_of((uint8_t)xenos_d3dformat_swizzle(fetch_format, 1)),
                                         swizzle_of((uint8_t)xenos_d3dformat_swizzle(fetch_format, 2)),
                                         swizzle_of((uint8_t)xenos_d3dformat_swizzle(fetch_format, 3)));
    s_gpu_image_desc depth = {};
    gpu_image_get_desc(g_gpu, resolved->image, &depth);
    if (resolved->packed != 0) {
        s_gpu_image_desc have = {};
        gpu_image_get_desc(g_gpu, resolved->packed, &have);
        if (have.width != depth.width || have.height != depth.height || resolved->packed_swizzle != swizzle) {
            gpu_image_retire(g_gpu, resolved->packed); // draws recorded this frame may still sample it
            resolved->packed = 0;
        }
    }
    if (resolved->packed == 0) {
        s_gpu_image_desc desc = {};
        desc.view_type = k_gpu_view_2d;
        desc.format = k_gpu_format_r8g8b8a8_unorm;
        desc.width = depth.width;
        desc.height = depth.height;
        desc.depth = 1;
        desc.mip_levels = 1;
        desc.array_layers = 1;
        desc.usage = k_gpu_image_sampled | k_gpu_image_storage;
        resolved->packed = gpu_image_create(g_gpu, &desc);
        resolved->packed_serial = 0;
        resolved->packed_swizzle = swizzle;
    }
    if (resolved->packed != 0 && resolved->packed_serial != resolved->resolve_serial) {
        if (xe_layout_call(gpu_cmd_pack_depth_stencil, g_gpu, resolved->image, resolved->packed, resolved->depth_float)) {
            resolved->packed_serial = resolved->resolve_serial;
        } else {
            gpu_image_retire(g_gpu, resolved->packed);
            resolved->packed = 0;
        }
        if (!gpu_rendering(g_gpu, 0, 0))
            clear_pass_attachments();
        if (tracing()) {
            char text[160];
            snprintf(text, sizeof(text), "trace depth pack %ux%u (%s) of tex %p for format %08x\n", depth.width,
                     depth.height, resolved->depth_float ? "20e4" : "unorm24", resolved->key, fetch_format);
            port_write_error(text);
        }
    }
    const gpu_view view = resolved->packed != 0 ? gpu_image_swizzled_view(g_gpu, resolved->packed, swizzle)
                                                   : GPU_NULL_HANDLE;
    if (view == GPU_NULL_HANDLE) {
        if (cacheable != 0)
            *cacheable = false;
        report(fetch_format ^ 0x64706b76u, "a depth resolve read through a 32-bit color header the device could not "
                                           "pack (its depth read in every channel instead)");
        if (out_image)
            *out_image = resolved->image;
        return gpu_image_sampled_view(g_gpu, resolved->image);
    }
    if (out_image)
        *out_image = resolved->packed;
    return view;
}

gpu_view resolved_view(s_texture_entry *resolved, uint32_t fetch_format, bool *out_skip_gamma = 0, bool *cacheable = 0,
                        s_gpu_image **out_image = 0)
{
    if (out_skip_gamma != 0)
        *out_skip_gamma = false;
    if (g_xenos && resolved->depth_edram != 0 && texel_layout(fetch_format) == k_xenos_format_8_8_8_8)
        return packed_depth_view(resolved, fetch_format, cacheable, out_image);
    s_gpu_image *image = resolved_image(resolved, fetch_format, out_skip_gamma, cacheable);
    image = resolved_at_draw_scale(resolved, image, cacheable);
    if (out_image)
        *out_image = image;
    return gpu_image_sampled_view(g_gpu, image);
}

// FNV-1a over a texture's source texels (HALO3_TEXTURE_RECHECK)
uint64_t texture_source_hash(const s_port_vk_texture *texture)
{
    uint32_t base_extent = 0, mip_extent = 0;
    if (!xenos_texture_get_source_extent(&texture->source.desc, &base_extent, &mip_extent))
        return 0;
    uint64_t h = 0xcbf29ce484222325ull;
    const unsigned char *spans[2] = { (const unsigned char *)texture->source.base, (const unsigned char *)texture->source.mip };
    const uint32_t sizes[2] = { texture->source.base != 0 ? base_extent : 0u, texture->source.mip != 0 ? mip_extent : 0u };
    for (int s = 0; s < 2; s++)
        for (uint32_t i = 0; spans[s] != 0 && i < sizes[s]; i++)
            h = (h ^ spans[s][i]) * 0x100000001b3ull;
    return h;
}

// The view of a texture as the game set it: converted from its Xenos layout (cached), or the image a Resolve wrote.
// texture_view's converted texels (the draw thread's): one buffer kept from texture to texture and grown as needed,
// so a conversion writes pages already mapped in instead of a fresh allocation's (a large texture's took a page fault
// every 4 KB). A texture over 64 MB gets its own allocation, released with texture_scratch_release.
const uint32_t k_texture_scratch_limit = 64u << 20;
void *g_texture_scratch;
uint32_t g_texture_scratch_size;

void *texture_scratch(uint32_t size)
{
    if (size > k_texture_scratch_limit)
        return malloc(size);
    if (size > g_texture_scratch_size) {
        free(g_texture_scratch);
        g_texture_scratch = malloc(size);
        g_texture_scratch_size = g_texture_scratch != 0 ? size : 0u;
    }
    return g_texture_scratch;
}

void texture_scratch_release(void *pixels, uint32_t size)
{
    if (size > k_texture_scratch_limit)
        free(pixels);
}

// `resolved`: resolved_entry of the texture (slot_resolved for a draw's sampler).
gpu_view texture_view_resolved(const s_port_vk_texture *texture, uint32_t image_dim, s_texture_entry *resolved,
                               bool *out_skip_gamma = 0, bool *cacheable = 0, s_gpu_image **out_image = 0)
{
    if (out_skip_gamma != 0)
        *out_skip_gamma = false;
    if (out_image)
        *out_image = 0;
    auto view_of = [&](s_gpu_image *img) -> gpu_view {
        if (out_image)
            *out_image = img;
        if (img == 0)
            return GPU_NULL_HANDLE;
        return image_dim == k_shader_pack_image_2d_array ? gpu_image_array_view(g_gpu, img)
                                                         : gpu_image_view(g_gpu, img);
    };
    // memory a texture export wrote (version 3 pack): sampled from the export's copy, where the GPU's writes are
    gpu_view exported = GPU_NULL_HANDLE;
    if (g_xe_texture_exports != 0 && xe_exported_texture_view(texture, image_dim, &exported, out_image))
        return exported;
    if (resolved != 0) {
        if (image_dim == k_shader_pack_image_2d_array) {
            materialize_pending_7e3(resolved);
            return view_of(resolved->image);
        }
        return resolved_view(resolved, texture->source.desc.d3d_format, out_skip_gamma, cacheable, out_image);
    }
    s_texture_entry *entry = texture_entry(texture->key, true);
    if (entry == 0)
        return GPU_NULL_HANDLE;
    if (entry->resolved && entry->image != 0) {
        if (image_dim == k_shader_pack_image_2d_array) {
            materialize_pending_7e3(entry);
            return view_of(entry->image);
        }
        return resolved_view(entry, texture->source.desc.d3d_format, out_skip_gamma, cacheable, out_image);
    }
    const s_xenos_texture_desc &desc = texture->source.desc;
    const uint32_t generation = touch_generation(texture->key) + texture->generation;
    // A 3D texture the DX11 shader declares as a Texture2DArray (the lightmaps: lightprobe_texture_array and
    // dominant_light_intensity_map, which the Xbox shader fetches slice by slice at z = (i + 0.5) / depth, so
    // never filters between slices): each slice becomes a layer. Only the first level (3D levels halve in depth).
    const uint32_t as_array = desc.dimension == k_xenos_dimension_3d && image_dim == k_shader_pack_image_2d_array ? 1u : 0u;
    if (entry->image != 0 && entry->base == texture->source.base && entry->mip == texture->source.mip &&
        entry->d3d_format == desc.d3d_format && entry->dimension == desc.dimension && entry->width == desc.width &&
        entry->height == desc.height && entry->depth == desc.depth && entry->levels == desc.levels &&
        entry->generation == generation && entry->as_array == as_array) {
        if (!g_texture_recheck || g_presented - entry->source_checked < 30u)
            return view_of(entry->image);
        entry->source_checked = g_presented;
        if (texture_source_hash(texture) == entry->source_hash)
            return view_of(entry->image);
        char detail[160];
        snprintf(detail, sizeof(detail), "header %p format %08x %ux%ux%u levels %u base %p mip %p", texture->key,
                 desc.d3d_format, desc.width, desc.height, desc.depth, desc.levels, entry->base, entry->mip);
        device_log_text("texture memory changed under a cached conversion: ");
        port_write_error(detail);
        port_write_error("\n");
    }
    s_texture_trace_timer trace_timer;
    if (entry->image != 0 && entry->as_array != as_array)
        report(key_of(texture->key) ^ 0x33646172u, "a 3D texture bound both as 3D and as a 2D array (converted again)");
    entry->as_array = as_array;
    release_texture_image(entry);
    entry->base = texture->source.base;
    entry->mip = texture->source.mip;
    entry->d3d_format = desc.d3d_format;
    entry->dimension = desc.dimension;
    entry->width = desc.width;
    entry->height = desc.height;
    entry->depth = desc.depth;
    entry->levels = desc.levels;
    entry->generation = generation;
    entry->source_hash = g_texture_recheck ? texture_source_hash(texture) : 0;
    entry->source_checked = g_presented;
    if (texture->source.base == 0 && texture->source.mip == 0) {
        report(key_of(texture->key) ^ 0x6e6f6261u, "a texture without data is bound (dummy texture used)");
        return GPU_NULL_HANDLE;
    }
    // texels the CPU wrote (port_vk_texture_written) are in host order: no GPUENDIAN swap to undo
    s_xenos_texture_desc convert_desc = desc;
    uint32_t host_order = 0;
    const void *first_texels = texture->source.base != 0 ? texture->source.base : texture->source.mip;
    {
        s_memory_records_lock lock;
        if (!map_find(&g_host_order_memory, key_of(first_texels), &host_order))
            host_order = 0;
    }
    if (host_order != 0)
        convert_desc.d3d_format &= ~(3u << 6);
    // A version 3 pack degammas GPUSIGN_GAMMA components in the shader, after filtering, as Xenia does (io.odin
    // fetch_gamma): the image keeps the raw codes, so those channels convert as unsigned.
    if (g_xenos)
        for (uint32_t c = 0; c < 4; c++)
            if (xenos_d3dformat_sign(convert_desc.d3d_format, c) == 3u)
                convert_desc.d3d_format &= ~(3u << (9u + 2u * c));
    s_pc_texture_info info;
    uint32_t base_extent = 0, mip_extent = 0;
    const long long convert_started = g_frame_trace.on ? frame_stats_now() : 0;
    if (!xenos_texture_get_pc_info(&convert_desc, &info) ||
        !xenos_texture_get_source_extent(&convert_desc, &base_extent, &mip_extent)) {
        report(desc.d3d_format ^ 0x666d74u, "unsupported texture format (dummy texture used)");
        return GPU_NULL_HANDLE;
    }
    const uint32_t pixel_bytes = info.total_size ? info.total_size : 1u;
    void *pixels = texture_scratch(pixel_bytes);
    if (pixels == 0)
        return GPU_NULL_HANDLE;
    if (!xenos_texture_convert(&convert_desc, texture->source.base, base_extent, texture->source.mip,
                               texture->source.mip != 0 ? mip_extent : 0, pixels, info.total_size, &info)) {
        texture_scratch_release(pixels, pixel_bytes);
        report(key_of(texture->key) ^ 0x636f6e76u, "texture conversion failed (dummy texture used)");
        return GPU_NULL_HANDLE;
    }
    if (convert_started != 0)
        g_frame_trace.texture_convert_ms += frame_stats_ms(convert_started, frame_stats_now());
    // GPUSIGN_GAMMA channels the converter could not make linear (xenos_texture_get_pc_info: 8-bit channels and BC1-3
    // RGB are): the shader reads them gamma-encoded where the Xenos fetch would have linearized them
    if ((info.gamma_channels & ~info.gamma_mask) != 0)
        report(desc.d3d_format ^ 0x67616d61u, "a GPUSIGN_GAMMA texture channel the converter does not linearize");
    const bool volume = desc.dimension == k_xenos_dimension_3d && !as_array;
    // A texture without its base level (MinMipLevel 1: the high-resolution level is not loaded) keeps its level
    // numbers, as Xenia's texture cache does: the converted levels start at image level 1, image level 0 is never
    // sampled (sampler_state_of clamps minLod to 1), so explicit LODs (sampleCUBElod of the dynamic environment maps)
    // address the levels the Xenos would.
    const uint32_t first_level = texture->source.base == 0 && !as_array ? 1u : 0u;
    const uint32_t levels = as_array ? 1u : info.levels;
    s_gpu_image_desc image_desc = {};
    image_desc.view_type = as_array ? (uint32_t)k_gpu_view_2d_array : view_type_of(desc.dimension);
    image_desc.format = gpu_format_of_pc(info.pc_format);
    image_desc.width = first_level != 0 ? desc.width : info.level[0].width;
    image_desc.height = first_level != 0 ? desc.height : info.level[0].height;
    image_desc.depth = volume ? (first_level != 0 ? desc.depth : info.level[0].depth) : 1u;
    image_desc.mip_levels = first_level + levels;
    image_desc.array_layers = as_array ? info.level[0].depth : info.layers;
    image_desc.usage = k_gpu_image_sampled | k_gpu_image_transfer_dst;
    image_desc.swizzle = GPU_SWIZZLE(swizzle_of(info.swizzle[0]), swizzle_of(info.swizzle[1]),
                                     swizzle_of(info.swizzle[2]), swizzle_of(info.swizzle[3]));
    entry->image = gpu_image_create(g_gpu, &image_desc);
    entry->host_format = image_desc.format;
    if (entry->image != 0) {
        s_gpu_image_region regions[16];
        uint32_t region_count = 0;
        for (uint32_t l = 0; l < levels && first_level + l < 16; l++) {
            const s_pc_texture_level &level = info.level[l];
            s_gpu_image_region &r = regions[region_count++];
            memset(&r, 0, sizeof(r));
            r.mip_level = first_level + l;
            r.base_layer = 0;
            r.layer_count = as_array ? level.depth : info.layers; // a volume's slices are the array's layers
            r.width = level.width;
            r.height = level.height;
            r.depth = volume ? level.depth : 1u;
            r.offset = level.offset;
            r.row_pitch = level.row_pitch;
            r.slice_pitch = desc.dimension == k_xenos_dimension_3d ? level.slice_pitch : level.layer_pitch;
        }
        // uploads go through their own submission; the image is new, so no frame in flight uses it
        const long long upload_started = g_frame_trace.on ? frame_stats_now() : 0;
        if (gpu_image_upload(g_gpu, entry->image, regions, region_count, pixels, info.total_size) != k_gpu_ok) {
            gpu_image_destroy(g_gpu, entry->image);
            entry->image = 0;
        }
        if (upload_started != 0)
            g_frame_trace.texture_upload_ms += frame_stats_ms(upload_started, frame_stats_now());
    }
    texture_scratch_release(pixels, pixel_bytes);
    return view_of(entry->image);
}

gpu_view texture_view(const s_port_vk_texture *texture, uint32_t image_dim)
{
    return texture_view_resolved(texture, image_dim, resolved_entry(texture->key, texture->source));
}

// The sampler state of a bound fetch, with the most detailed level it may use: D3DSAMP_MAXMIPLEVEL, the header's
// MinMipLevel (fetch constant dword 4 bits 2-5) and level 1 for a texture without base data, whose image starts at
// level 1 (texture_view); Xenia's GetSubresourcesFromFetchConstant clamps the same way.
s_port_vk_sampler_state sampler_state_of(const s_port_vk_sampler &slot)
{
    s_port_vk_sampler_state state = slot.state;
    if (!slot.bound)
        return state;
    const s_port_vk_texture &texture = slot.texture;
    uint32_t minimum = (texture.source.fetch_constant[4] >> 2) & 0xfu;
    if (texture.source.base == 0 && texture.source.mip != 0)
        minimum = minimum > 1u ? minimum : 1u;
    if (state.min_mip_level < minimum)
        state.min_mip_level = minimum;
    const uint32_t last = texture.source.desc.levels > 0 ? texture.source.desc.levels - 1u : 0u;
    if (state.min_mip_level > last)
        state.min_mip_level = last;
    return state;
}

// The key of a sampler description in g_sampler_index (never 0: not a key).
uint32_t sampler_desc_hash(const s_gpu_sampler_desc &desc)
{
    static_assert(sizeof(s_gpu_sampler_desc) % 4u == 0, "whole words");
    uint32_t words[sizeof(s_gpu_sampler_desc) / 4u], hash = 2166136261u;
    memcpy(words, &desc, sizeof(words));
    for (uint32_t word : words)
        hash = mix32(hash ^ word);
    return hash != 0 ? hash : 1u;
}

gpu_sampler sampler_of_desc(const s_port_vk_sampler_state &state, bool compare, uint32_t *entry_index);

// `compare`: a sampler the shader depth-compares through (shader_compares_through): compareEnable with LESS_OR_EQUAL,
// the Xbox's own test (shadow_generate.fx's 2007 PCF: step(reference, stored depth), lit where reference <= depth;
// D3D11 SampleCmp and the host APIs compare reference OP texel alike).
gpu_sampler sampler_for(const s_port_vk_sampler_state &state, bool compare = false, uint32_t *entry_index = 0)
{
    static_assert(sizeof(s_port_vk_sampler_state) % 4u == 0, "whole words");
    // the words weighted by distinct odd constants (independent multiplies), mixed once: an entry's index only (the
    // words are compared)
    static const uint32_t k_weights[k_sampler_state_words] = { 0x9e3779b1u, 0x85ebca77u, 0xc2b2ae3du, 0x27d4eb2fu,
                                                               0x165667b1u, 0xd3a2646du, 0xfd7046c5u, 0xb55a4f09u,
                                                               0x7feb352du, 0x846ca68bu, 0x2545f491u, 0x68e31da5u,
                                                               0x1b873593u };
    static_assert(sizeof(k_weights) == sizeof(s_port_vk_sampler_state), "a weight a word");
    uint32_t words[k_sampler_state_words];
    memcpy(words, &state, sizeof(words));
    const uint32_t flag = compare ? 1u : 0u;
    uint32_t state_hash = flag;
    for (uint32_t i = 0; i < k_sampler_state_words; i++)
        state_hash += words[i] * k_weights[i];
    s_sampler_memo &memo = g_sampler_memo[mix32(state_hash) % k_sampler_memo_entries];
    uint32_t differ = memo.compare ^ flag;
    for (uint32_t i = 0; i < k_sampler_state_words; i++)
        differ |= memo.state[i] ^ words[i];
    if (differ == 0 && memo.sampler != GPU_NULL_HANDLE) {
        if (entry_index)
            *entry_index = memo.entry_index;
        return memo.sampler;
    }
    uint32_t index = 0;
    const gpu_sampler sampler = sampler_of_desc(state, compare, &index);
    if (entry_index)
        *entry_index = index;
    if (sampler != g_default_sampler) { // not a failure's fallback: asked again next time
        memcpy(memo.state, words, sizeof(words));
        memo.compare = flag;
        memo.sampler = sampler;
        memo.entry_index = index;
    }
    return sampler;
}

// sampler_for without its memo: the device sampler for the state (made once per description).
gpu_sampler sampler_of_desc(const s_port_vk_sampler_state &state, bool compare, uint32_t *entry_index)
{
    *entry_index = 0;
    s_gpu_sampler_desc desc = {};
    if (compare) {
        desc.compare_enable = 1;
        desc.compare_op = k_gpu_compare_less_equal;
    }
    desc.mag_filter = state.mag_filter == 0 ? k_gpu_filter_nearest : k_gpu_filter_linear;
    desc.min_filter = state.min_filter == 0 ? k_gpu_filter_nearest : k_gpu_filter_linear;
    desc.mipmap_mode = state.mip_filter == 1 ? k_gpu_filter_linear : k_gpu_filter_nearest;
    const uint32_t address[3] = { state.address_u, state.address_v, state.address_w };
    uint32_t *out[3] = { &desc.address_u, &desc.address_v, &desc.address_w };
    for (int i = 0; i < 3; i++) {
        switch (address[i]) {
        case 1: *out[i] = k_gpu_address_mirrored_repeat; break;
        case 2: *out[i] = k_gpu_address_clamp_to_edge; break;
        // MIRRORONCE: mirror-clamp-to-edge where the device has it (caps); without it the nearest mode
        case 3: *out[i] = g_gpu->caps.sampler_mirror_clamp_to_edge ? k_gpu_address_mirror_clamp_to_edge
                                                                  : k_gpu_address_clamp_to_edge; break;
        case 6: *out[i] = k_gpu_address_clamp_to_border; break;
        default: *out[i] = k_gpu_address_repeat; break;
        }
    }
    desc.mip_lod_bias = state.lod_bias;
    float aniso = (state.min_filter == 4 || state.mag_filter == 4) ? (float)(state.max_anisotropy ? state.max_anisotropy : 1u) : 1.0f;
    if (g_anisotropy_override > 1u && !compare &&
        state.min_filter != 0 && state.mag_filter != 0 && state.mip_filter != 2) {
        if ((float)g_anisotropy_override > aniso)
            aniso = (float)g_anisotropy_override;
    }
    desc.max_anisotropy = aniso;
    desc.min_lod = (float)state.min_mip_level;
    desc.max_lod = state.mip_filter == 2 ? desc.min_lod : 1000.0f;
    desc.border_color = state.border_color == 2 ? k_gpu_border_opaque_white
                        : state.border_color == 1 ? k_gpu_border_opaque_black
                                                  : k_gpu_border_transparent_black;
    // every texture fetch of every draw asks: found by its hash (a linear search through the hundreds made took a
    // share of the frame), the list searched only for a sampler whose hash another one took over
    const uint32_t hash = sampler_desc_hash(desc);
    uint32_t index;
    if (map_find(&g_sampler_index, hash, &index) && index < g_samplers.count &&
        memcmp(&g_samplers.items[index].desc, &desc, sizeof(desc)) == 0) {
        *entry_index = index + 1u;
        return g_samplers.items[index].sampler;
    }
    for (uint32_t i = 0; i < g_samplers.count; i++) {
        if (memcmp(&g_samplers.items[i].desc, &desc, sizeof(desc)) == 0) {
            *entry_index = i + 1u;
            return g_samplers.items[i].sampler;
        }
    }
    gpu_sampler sampler = gpu_sampler_create(g_gpu, &desc);
    if (sampler == GPU_NULL_HANDLE)
        return g_default_sampler;
    s_sampler_entry *entry = g_samplers.push();
    if (entry != 0) {
        entry->desc = desc;
        entry->sampler = sampler;
        *entry_index = g_samplers.count;
        map_set(&g_sampler_index, hash, g_samplers.count - 1u);
    }
    return sampler;
}

s_gpu_image *make_dummy(uint32_t view_type, uint32_t texel)
{
    s_gpu_image_desc desc = {};
    desc.view_type = view_type;
    desc.format = k_gpu_format_r8g8b8a8_unorm;
    desc.width = 1;
    desc.height = 1;
    desc.depth = 1;
    desc.mip_levels = 1;
    desc.array_layers = view_type == k_gpu_view_cube ? 6u : 1u;
    s_gpu_image *image = gpu_image_create(g_gpu, &desc);
    if (image == 0)
        return 0;
    const uint32_t texels[6] = { texel, texel, texel, texel, texel, texel };
    s_gpu_image_region region = {};
    region.layer_count = desc.array_layers;
    region.width = region.height = region.depth = 1;
    region.row_pitch = 4;
    region.slice_pitch = 4;
    gpu_image_upload(g_gpu, image, &region, 1, texels, sizeof(texels));
    return image;
}

// A 16x16x16 LUT whose every texel holds its own coordinates: the colour grading of no colour grading (16 entries
// a side, as final_composite's lookup assumes; 17 * index is exact in 8 bits).
s_gpu_image *make_identity_lut()
{
    const uint32_t size = 16;
    s_gpu_image_desc desc = {};
    desc.view_type = k_gpu_view_3d;
    desc.format = k_gpu_format_r8g8b8a8_unorm;
    desc.width = desc.height = desc.depth = size;
    desc.mip_levels = 1;
    desc.array_layers = 1;
    s_gpu_image *image = gpu_image_create(g_gpu, &desc);
    if (image == 0)
        return 0;
    uint32_t *texels = (uint32_t *)malloc(size * size * size * 4u);
    if (texels == 0)
        return image;
    for (uint32_t b = 0; b < size; b++)
        for (uint32_t g = 0; g < size; g++)
            for (uint32_t r = 0; r < size; r++)
                texels[(b * size + g) * size + r] = (17u * r) | (17u * g) << 8 | (17u * b) << 16 | 0xff000000u;
    s_gpu_image_region region = {};
    region.layer_count = 1;
    region.width = region.height = region.depth = size;
    region.row_pitch = size * 4u;
    region.slice_pitch = size * size * 4u;
    gpu_image_upload(g_gpu, image, &region, 1, texels, size * size * size * 4u);
    free(texels);
    return image;
}

// Whether a pack binding is one of MCC's colour grading LUTs or their samplers (final_composite_base.hlsl
// color_grading0 / color_grading1): resources of the PC shaders the Xbox game has no counterpart for.
bool color_grading_binding(const s_shader_pack_binding &binding)
{
    return binding.name != 0 && (strcmp(binding.name, "LocalTexture_color_grading0") == 0 ||
                                 strcmp(binding.name, "LocalTexture_color_grading1") == 0 ||
                                 strcmp(binding.name, "LocalSampler_color_grading0") == 0 ||
                                 strcmp(binding.name, "LocalSampler_color_grading1") == 0);
}

// A pixel draw sampling a depth resolve while a color target over that same depth buffer's EDRAM is bound writes
// the depth bits back into EDRAM as color: the depth surface takes them up when it is next used (edram_sync).
void note_depth_restore(const s_port_vk_draw *draw)
{
    if (!g_pass_color_aliases_depth)
        return;
    for (uint32_t s = 0; s < PORT_VK_VERTEX_SAMPLER_BASE; s++) {
        const s_port_vk_sampler &slot = draw->samplers[s];
        if (!slot.bound || slot.texture.key == 0)
            continue;
        // resolved_entry (memoized): the key's own entry when it is a resolve with an image, else an alias; none
        // means neither is a resolve to look at (the own entry is missing, not resolved or without an image)
        s_texture_entry *texture = slot_resolved(draw, s);
        if (texture == 0 || !texture->resolved || texture->image == 0 || texture->depth_edram == 0)
            continue;
        for (uint32_t i = 0; i < 4; i++) {
            s_target_entry *target = g_pass_color[i] != 0 ? g_bound_color[i] : 0; // attached (ensure_pass)
            if (target != 0 && !target->depth && target->edram_base + 1u == texture->depth_edram) {
                target->restored_texture = (uint32_t)(texture - g_textures.items) + 1u;
                target->restored_content = target->content;
            }
        }
    }
}

gpu_view dummy_view(uint32_t image_dim)
{
    switch (image_dim) {
    case k_shader_pack_image_3d: return g_dummy_3d ? gpu_image_view(g_gpu, g_dummy_3d) : GPU_NULL_HANDLE;
    case k_shader_pack_image_cube: return g_dummy_cube ? gpu_image_view(g_gpu, g_dummy_cube) : GPU_NULL_HANDLE;
    case k_shader_pack_image_2d_array: return g_dummy_array ? gpu_image_view(g_gpu, g_dummy_array) : GPU_NULL_HANDLE;
    default: return g_dummy_2d ? gpu_image_view(g_gpu, g_dummy_2d) : GPU_NULL_HANDLE;
    }
}

// The placeholder of a pack texture binding the game leaves unbound: a texture the DX11 shader reads but the retail
// shader never fetches (the Xbox compiler dropped the fetch, so the game has nothing to bind there). It must stand for
// what the retail shader computed without it (validate_all_maps, 02_all_maps_unresolved_gather.tsv):
//   - a normal map (bump_map, detail bumps: the 18 bump-mapped additive/transparent shader templates) is flat:
//     (0, 0) gives bump_mapping.fx's z = 1, the Xbox's untouched normal; white would tilt it 45 degrees;
//   - everything else keeps white: a colour or mask multiplies by 1, an alpha_test_map passes (the Xbox shader does
//     not alpha test), and terrain's blend_map (the 12 terrain templates with a single active material) normalizes
//     to a weight of 1 for that material (black would divide 0 by 0).
bool text_starts_with(const char *text, const char *prefix)
{
    for (; *prefix != 0; text++, prefix++)
        if (*text != *prefix)
            return false;
    return true;
}

bool normal_map_binding(const s_shader_pack_binding &binding)
{
    if (binding.name == 0)
        return false;
    for (const char *c = binding.name; *c != 0; c++)
        if (text_starts_with(c, "bump") || text_starts_with(c, "normal_map"))
            return true;
    return false;
}

gpu_view dummy_view_for(const s_shader_pack_binding &binding)
{
    if (binding.image_dim == k_shader_pack_image_2d && g_dummy_flat_normal != 0 && normal_map_binding(binding))
        return gpu_image_view(g_gpu, g_dummy_flat_normal);
    return dummy_view(binding.image_dim);
}

// ---- shaders and declarations ----

s_shader_entry *shader_entry(const void *object)
{
    uint32_t index;
    if (object == 0 || !map_find(&g_shader_index, key_of(object), &index))
        return 0;
    return &g_shaders.items[index];
}

// D3D's alpha test (D3DRS_ALPHATESTENABLE, ALPHAFUNC, ALPHAREF) is fixed function on the Xbox and has no host API
// counterpart. The pack's shaders are Bungie's DX11 ones, written for an API without it: every pixel shader that
// alpha tests discards its own fragments (alpha_test.fx calc_alpha_test_on_ps: clip(alpha - 0.5), decorators
// k_decorator_alpha_test_threshold, the shadow generators), so the render state needs nothing from the pipeline.
// What is checked is that it holds: a draw with the alpha test on whose pixel shader cannot discard (no OpKill,
// OpTerminateInvocation or OpDemoteToHelperInvocation in its SPIR-V) is reported, once per shader.
bool shader_clips(s_shader_entry *entry)
{
    if (entry->clips == 0) {
        entry->clips = 2;
        s_shader_pack_shader_info info;
        size_t size = 0;
        const uint32_t *code = shader_pack_get_shader(g_pack, entry->shader_index, &info)
                                   ? shader_pack_module_code(g_pack, info.module_index, &size) : 0;
        for (size_t word = 5; code != 0 && word < size / 4u;) {
            const uint32_t opcode = code[word] & 0xffffu, length = code[word] >> 16;
            if (opcode == 252u || opcode == 4416u || opcode == 5380u) {
                entry->clips = 1;
                break;
            }
            word += length != 0 ? length : 1u;
        }
    }
    return entry->clips == 1;
}

// PC-only cbuffer members: MCC's DX11 shaders read values no Xbox register carries, so the pack's gathers leave them
// zero. The renderer writes what the Xbox build does in their place; their cbuffer binding and byte offset come from
// the module's SPIR-V (OpMemberName -> OpMemberDecorate Offset of that struct member -> the Uniform OpVariable of the
// struct -> its Binding).
//   - user clip planes: fixed function on the Xbox (PA_CL_UCP, D3DRS_CLIPPLANEENABLE); the DX11 vertex shaders clip
//     for themselves, SV_ClipDistance = dot(clip-space position, plane), from clip_plane.fx v_clip_plane (ViewVS, the
//     object clip plane) and dynamic_light_clip.fx v_dynamic_light_clip_plane[6] (DynamicLightClipVS, light geometry);
//   - deform.fx v_always_true (ViewVS): "always true - poor man's isolate", the branch around every vertex shader's
//     deform (vertex decompression, rigid and skinned transforms); zero skips it;
//   - MiscPS p_shader_pc_specular_enabled (1: the Xbox always lights specular) and p_shader_pc_albedo_lighting (0: a
//     PC debug view);
//   - GlobalPS (global_texture_sampling.fx): the viewport of split-screen rendering into one full-size target, which
//     the *_IN_VIEWPORT samplers map their screen texture coordinates into. The Xbox renders a view into targets of
//     its own size: the viewport is the whole render resolution (the identity mapping); zero maps every coordinate to
//     (0, 0);
//   - ps_total_time / vs_total_time (MCC's texture scrolling) and WaterPS k_is_camera_underwater stay zero (not set
//     by the Xbox build).
// (MiscPS LDR_gamma2 / HDR_gamma2 are not PC-only: they are the 2007 ldr_gamma2 b14 / hdr_gamma2 b15 the game sets
// per pass, and the pack gathers them.)
const char *const k_pc_member_names[k_pc_member_count] = {
    "v_clip_plane", "v_dynamic_light_clip_plane", "v_always_true", "p_shader_pc_specular_enabled",
    "ps_global_viewport_res", "ps_global_viewport_res_multipliers", "ps_global_viewport_bounds_uv",
    "ps_global_render_resolution", "ps_global_render_pixel_size", "particle_index_range", "beam_index_range",
    "contrail_index_range", "light_volume_index_range", "ripple_index_range",
};

bool spirv_string_equals(const uint32_t *words, size_t count, const char *text)
{
    const size_t length = strlen(text);
    if ((length + 4u) / 4u > count)
        return false;
    return memcmp(words, text, length + 1u) == 0; // the SPIR-V string's NUL included
}

void scan_pc_members(s_shader_entry *entry)
{
    if (entry->pc_scanned)
        return;
    entry->pc_scanned = 1;
    entry->pc_any = 0;
    uint32_t struct_id[k_pc_member_count] = {}, member[k_pc_member_count] = {};
    for (uint32_t k = 0; k < k_pc_member_count; k++) {
        entry->pc_binding[k] = ~0u;
        entry->pc_offset[k] = 0;
    }
    s_shader_pack_shader_info info;
    size_t size = 0;
    const uint32_t *code = shader_pack_get_shader(g_pack, entry->shader_index, &info)
                               ? shader_pack_module_code(g_pack, info.module_index, &size) : 0;
    const size_t words = code != 0 ? size / 4u : 0;
    // OpMemberName (6): struct, member, name
    for (size_t w = 5; w < words;) {
        const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
        if (length == 0 || w + length > words)
            break;
        if (opcode == 6u && length > 3u) {
            for (uint32_t k = 0; k < k_pc_member_count; k++) {
                if (struct_id[k] == 0 && spirv_string_equals(code + w + 3, length - 3u, k_pc_member_names[k])) {
                    struct_id[k] = code[w + 1];
                    member[k] = code[w + 2];
                }
            }
        }
        w += length;
    }
    for (uint32_t k = 0; k < k_pc_member_count; k++) {
        if (struct_id[k] == 0)
            continue;
        uint32_t offset = ~0u, pointer = 0, variable = 0, binding = ~0u;
        for (size_t w = 5; w < words;) { // OpMemberDecorate (72) Offset (35); OpTypePointer (32) Uniform (2)
            const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
            if (opcode == 72u && length >= 5u && code[w + 1] == struct_id[k] && code[w + 2] == member[k] &&
                code[w + 3] == 35u)
                offset = code[w + 4];
            if (opcode == 32u && length >= 4u && code[w + 2] == 2u && code[w + 3] == struct_id[k])
                pointer = code[w + 1];
            w += length;
        }
        for (size_t w = 5; pointer != 0 && w < words;) { // OpVariable (59): type, id, storage
            const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
            if (opcode == 59u && length >= 4u && code[w + 1] == pointer)
                variable = code[w + 2];
            w += length;
        }
        for (size_t w = 5; variable != 0 && w < words;) { // OpDecorate (71) Binding (33)
            const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
            if (opcode == 71u && length >= 4u && code[w + 1] == variable && code[w + 2] == 33u)
                binding = code[w + 3];
            w += length;
        }
        if (offset != ~0u && binding != ~0u) {
            entry->pc_binding[k] = binding;
            entry->pc_offset[k] = offset;
            entry->pc_any = 1;
        }
    }
}

// Whether a vertex shader's SPIR-V reads SV_InstanceID (OpDecorate (71) <id> BuiltIn (11) InstanceIndex (43)): the DX11
// GPU-effect render shaders (beam.fx, light_volume.fx, particle_render.hlsl) split the Xbox vertex index into
// (SV_InstanceID, SV_VertexID) and are drawn instanced (port_vk_draw).
bool shader_reads_instance(s_shader_entry *entry)
{
    if (entry->reads_instance == 0) {
        entry->reads_instance = 2;
        s_shader_pack_shader_info info;
        size_t size = 0;
        const uint32_t *code = shader_pack_get_shader(g_pack, entry->shader_index, &info)
                                   ? shader_pack_module_code(g_pack, info.module_index, &size) : 0;
        const size_t words = code != 0 ? size / 4u : 0;
        for (size_t w = 5; w < words;) {
            const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
            if (length == 0 || w + length > words)
                break;
            if (opcode == 71u && length >= 4u && code[w + 2] == 11u && code[w + 3] == 43u)
                entry->reads_instance = 1;
            w += length;
        }
    }
    return entry->reads_instance == 1;
}

// Whether a shader compares depth through the sampler at (set, binding): MCC's DX11 shadow lookups (shadow_generate.fx
// sample_PCF_hardware, texture.fx texCmp2D_offset: SamplerComparisonState + SampleCmp) become OpImage*Dref* on an
// OpSampledImage of that sampler. Vulkan leaves a depth-compare sample through a sampler made without compareEnable
// undefined, so such a binding gets a comparison sampler (sampler_for). The SPIR-V is walked once a shader: OpDecorate
// DescriptorSet / Binding of the variables, OpLoad of a variable, OpSampledImage (image, sampler), then the Dref
// instructions' sampled image back to the variable.
bool shader_compares_through(s_shader_entry *entry, uint32_t set, uint32_t binding)
{
    if (entry->compare_scanned == 0) {
        entry->compare_scanned = 1;
        s_shader_pack_shader_info info;
        size_t size = 0;
        const uint32_t *code = shader_pack_get_shader(g_pack, entry->shader_index, &info)
                                   ? shader_pack_module_code(g_pack, info.module_index, &size) : 0;
        const size_t words = code != 0 && size >= 20u && code[0] == 0x07230203u ? size / 4u : 0;
        const uint32_t bound = words != 0 ? code[3] : 0u;
        // per id: the variable an OpLoad read (+1), the sampler an OpSampledImage took (+1), DescriptorSet / Binding
        uint32_t *loaded = bound != 0 && bound < (1u << 22) ? (uint32_t *)calloc(bound, sizeof(uint32_t)) : 0;
        uint32_t *sampler_of = loaded != 0 ? (uint32_t *)calloc(bound, sizeof(uint32_t)) : 0;
        uint32_t *decorated = sampler_of != 0 ? (uint32_t *)calloc(bound, sizeof(uint32_t)) : 0;
        for (size_t w = 5; decorated != 0 && w < words;) {
            const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
            if (length == 0 || w + length > words)
                break;
            const uint32_t *op = code + w;
            if (opcode == 71u && length >= 4u && op[1] < bound && (op[2] == 33u || op[2] == 34u)) {
                // OpDecorate Binding (33) / DescriptorSet (34): bit 31 marks a binding seen
                if (op[2] == 33u)
                    decorated[op[1]] = (decorated[op[1]] & 0xffff0000u) | (op[3] & 0xffffu) | 0x80000000u;
                else
                    decorated[op[1]] = (decorated[op[1]] & 0x8000ffffu) | (op[3] & 0x7fffu) << 16;
            } else if (opcode == 61u && length >= 4u && op[2] < bound && op[3] < bound) {
                loaded[op[2]] = op[3] + 1u; // OpLoad result <- pointer
            } else if (opcode == 86u && length >= 5u && op[2] < bound && op[4] < bound) {
                sampler_of[op[2]] = op[4] + 1u; // OpSampledImage result <- (image, sampler)
            } else if ((opcode == 89u || opcode == 90u || opcode == 93u || opcode == 94u || opcode == 97u ||
                        opcode == 307u || opcode == 308u || opcode == 311u || opcode == 312u || opcode == 315u) &&
                       length >= 4u && op[3] < bound && sampler_of[op[3]] != 0) {
                // OpImage[Sparse]Sample[Proj]Dref{Implicit,Explicit}Lod, OpImage[Sparse]DrefGather: operand 3 is
                // the sampled image
                const uint32_t sampler = sampler_of[op[3]] - 1u;
                const uint32_t variable = loaded[sampler] != 0 ? loaded[sampler] - 1u : sampler;
                const uint32_t d = variable < bound ? decorated[variable] : 0u;
                if ((d & 0x80000000u) != 0) {
                    const uint32_t key = d & 0x7fffffffu;
                    bool known = false;
                    for (uint32_t i = 0; i < entry->compare_count; i++)
                        known |= entry->compare_binding[i] == key;
                    if (!known && entry->compare_count < 4u)
                        entry->compare_binding[entry->compare_count++] = key;
                }
            }
            w += length;
        }
        free(loaded);
        free(sampler_of);
        free(decorated);
    }
    const uint32_t key = (set & 0x7fffu) << 16 | (binding & 0xffffu);
    for (uint32_t i = 0; i < entry->compare_count; i++)
        if (entry->compare_binding[i] == key)
            return true;
    return false;
}

// Writes `bytes` at a member's offset when it lives in this cbuffer binding.
void write_pc_member(const s_shader_entry *entry, uint32_t k, uint32_t binding, void *data, uint32_t size,
                     const void *value, uint32_t bytes)
{
    if (entry->pc_binding[k] == binding && entry->pc_offset[k] + bytes <= size)
        memcpy((char *)data + entry->pc_offset[k], value, bytes);
}

// Fills the PC-only members of one cbuffer of a draw's shader (after its gathers).
void write_pc_members(s_shader_entry *entry, uint32_t binding, const s_port_vk_draw *draw, void *data, uint32_t size)
{
    scan_pc_members(entry);
    if (!entry->pc_any)
        return;
    // user clip planes: v_clip_plane carries one plane, the lowest enabled one; v_dynamic_light_clip_plane[i] carries
    // plane i. A disabled plane stays zero (distance 0, never clipped).
    for (uint32_t i = 0; i < 6 && draw->clip_plane_enable != 0; i++) {
        if ((draw->clip_plane_enable & (1u << i)) != 0) {
            write_pc_member(entry, k_pc_member_clip_plane, binding, data, size, draw->clip_planes[i], 16);
            break;
        }
    }
    if (entry->pc_binding[k_pc_member_dynamic_light_clip_plane] == binding) {
        for (uint32_t i = 0; i < 6; i++) {
            const uint32_t at = entry->pc_offset[k_pc_member_dynamic_light_clip_plane] + 16u * i;
            if ((draw->clip_plane_enable & (1u << i)) != 0 && at + 16u <= size)
                memcpy((char *)data + at, draw->clip_planes[i], 16);
        }
    }
    const uint32_t always_true = 1;
    write_pc_member(entry, k_pc_member_always_true, binding, data, size, &always_true, 4);
    const float specular_enabled = 1.0f;
    write_pc_member(entry, k_pc_member_specular_enabled, binding, data, size, &specular_enabled, 4);
    s_gpu_render_target_info screen;
    gpu_render_target_get_info(g_gpu, g_back_buffer, &screen);
    const float resolution[2] = { (float)screen.width, (float)screen.height };
    const float pixel_size[2] = { 1.0f / resolution[0], 1.0f / resolution[1] };
    const float multipliers[2] = { 1.0f, 1.0f };
    const float bounds_uv[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    write_pc_member(entry, k_pc_member_viewport_res, binding, data, size, resolution, 8);
    write_pc_member(entry, k_pc_member_viewport_res_multipliers, binding, data, size, multipliers, 8);
    write_pc_member(entry, k_pc_member_viewport_bounds_uv, binding, data, size, bounds_uv, 16);
    write_pc_member(entry, k_pc_member_render_resolution, binding, data, size, resolution, 8);
    write_pc_member(entry, k_pc_member_render_pixel_size, binding, data, size, pixel_size, 8);
    for (uint32_t k = k_pc_member_first_index_range; k < k_pc_member_count; k++)
        write_pc_member(entry, k, binding, data, size, g_index_range, 8);
}

const s_xenos_vertex_declaration *declaration_for(const void *elements, uint32_t *out_hash)
{
    if (elements == 0)
        return 0;
    uint32_t index;
    if (map_find(&g_declaration_index, key_of(elements), &index)) {
        *out_hash = g_declarations.items[index].hash;
        return &g_declarations.items[index].declaration;
    }
    s_declaration_entry *entry = g_declarations.push();
    if (entry == 0)
        return 0;
    entry->key = elements;
    if (!xenos_vertex_declaration_from_d3d((const s_xenos_d3d_vertex_element *)elements, &entry->declaration)) {
        g_declarations.count--;
        report(key_of(elements), "unusable vertex declaration (draw skipped)");
        return 0;
    }
    entry->hash = gpu_hash_bytes(2166136261u, &entry->declaration, sizeof(entry->declaration));
    map_set(&g_declaration_index, key_of(elements), g_declarations.count - 1u);
    *out_hash = entry->hash;
    return &entry->declaration;
}

// ---- vertex and index data ----

// Where a stream's (or the index) data is for this draw: a cached device buffer, or a copy in the frame ring.
struct s_data_binding {
    s_gpu_buffer *buffer;
    uint32_t offset;
};

// What a stream's converted copy depends on: the declaration's elements in that stream (xenos_vertex_convert_stream
// reads no others). One vertex buffer drawn under several declarations that agree on it shares one copy.
uint32_t stream_layout_hash(const s_xenos_vertex_declaration *declaration, uint32_t s)
{
    uint32_t hash = 2166136261u;
    for (uint32_t e = 0; e < declaration->element_count; e++) {
        const s_xenos_d3d_vertex_element &element = declaration->elements[e];
        if (element.stream != s)
            continue;
        const uint32_t words[2] = { element.offset, element.type };
        hash = gpu_hash_bytes(hash, words, sizeof(words));
    }
    return hash;
}

// A stream's vertices as the pipeline reads them: converted in place, or (PRT ambient) one vertex per packed value.
void convert_vertices(const s_xenos_vertex_declaration *declaration, uint32_t s, const s_port_vk_stream &stream,
                      bool packed_prt, const uint8_t *source, void *dst, uint32_t vertices)
{
    if (packed_prt)
        xenos_vertex_expand_packed_prt(source, dst, vertices, stream.big_endian);
    else
        xenos_vertex_convert_stream(declaration, s, source, dst, vertices, stream.stride, stream.big_endian);
}

// The vertices a draw reads, [first, end), in its streams' numbering (the vertex index after base_vertex). Indexed
// draws learn it from their indices only when a stream needs it (vertex_range_resolve).
struct s_vertex_range {
    bool known;
    bool whole;           // the draw may read any vertex (instance-rate streams, decorators): convert the stream whole
    uint32_t first, end;
    const s_port_vk_indices *indices; // indexed: the index data, and the draw's [index_first, + index_count)
    uint32_t index_first, index_count;
    int base_vertex;
};

// The index loops below run over every index a draw copies into the ring (dynamic and user-pointer index data) or
// scans for its vertex range: one loop per index size and byte order, without branches in the loop body, so the
// compiler vectorizes them (SSE2 on i686).
inline uint16_t swap16(uint16_t v) { return (uint16_t)((v >> 8) | (v << 8)); }
inline uint32_t swap32(uint32_t v) { return (v >> 24) | ((v >> 8) & 0xff00u) | ((v << 8) & 0xff0000u) | (v << 24); }

template <bool k_swap>
void widen_indices16(const uint8_t *__restrict src, uint32_t count, uint32_t *__restrict dst)
{
    for (uint32_t i = 0; i < count; i++) {
        uint16_t h;
        memcpy(&h, src + (size_t)i * 2u, 2);
        h = k_swap ? swap16(h) : h;
        dst[i] = h == 0xffffu ? 0xffffffffu : (uint32_t)h; // the 16-bit restart index stays the restart index
    }
}

template <bool k_swap> void copy_indices32(const uint8_t *__restrict src, uint32_t count, uint32_t *__restrict dst)
{
    for (uint32_t i = 0; i < count; i++) {
        uint32_t v;
        memcpy(&v, src + (size_t)i * 4u, 4);
        dst[i] = k_swap ? swap32(v) : v;
    }
}

// The lowest and highest index of [src, + count) other than the restart index (*low > *high when there is none).
template <bool k_swap> void index_bounds16(const uint8_t *__restrict src, uint32_t count, uint32_t *low, uint32_t *high)
{
    // 16-bit accumulators: the loop vectorizes in 16-bit lanes (eight indices a vector; SSE2 has no unsigned 16-bit
    // min / max, which LLVM makes of saturating subtractions), not widened to 32-bit ones
    uint16_t l = 0xffffu, h = 0; // the restart index 0xffff is above every vertex index: it never lowers `l`
    for (uint32_t i = 0; i < count; i++) {
        uint16_t v;
        memcpy(&v, src + (size_t)i * 2u, 2);
        const uint16_t index = k_swap ? swap16(v) : v;
        l = index < l ? index : l;
        const uint16_t counted = index == 0xffffu ? (uint16_t)0u : index;
        h = counted > h ? counted : h;
    }
    *low = l == 0xffffu ? 0xffffffffu : l;
    *high = h;
}

template <bool k_swap> void index_bounds32(const uint8_t *__restrict src, uint32_t count, uint32_t *low, uint32_t *high)
{
    uint32_t l = 0xffffffffu, h = 0;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t v;
        memcpy(&v, src + (size_t)i * 4u, 4);
        const uint32_t index = k_swap ? swap32(v) : v;
        l = index < l ? index : l;
        const uint32_t counted = index == 0xffffffffu ? 0u : index;
        h = counted > h ? counted : h;
    }
    *low = l;
    *high = h;
}

// Index i of [src, ...) as a 32-bit index (a 16-bit restart index becomes the 32-bit one).
uint32_t read_index(const s_port_vk_indices &indices, const uint8_t *src, uint32_t i)
{
    if (indices.index_size == 2) {
        uint16_t h;
        memcpy(&h, src + (size_t)i * 2u, 2);
        h = indices.big_endian ? swap16(h) : h;
        return h == 0xffffu ? 0xffffffffu : h;
    }
    uint32_t v;
    memcpy(&v, src + (size_t)i * 4u, 4);
    return indices.big_endian ? swap32(v) : v;
}

void vertex_range_resolve(s_vertex_range *range)
{
    if (range->known || range->whole)
        return;
    range->known = true;
    const s_port_vk_indices &indices = *range->indices;
    const uint8_t *src = index_bytes(indices, (size_t)range->index_first * indices.index_size);
    uint32_t low = 0xffffffffu, high = 0;
    if (indices.index_size == 2) {
        if (indices.big_endian)
            index_bounds16<true>(src, range->index_count, &low, &high);
        else
            index_bounds16<false>(src, range->index_count, &low, &high);
    } else if (indices.big_endian) {
        index_bounds32<true>(src, range->index_count, &low, &high);
    } else {
        index_bounds32<false>(src, range->index_count, &low, &high);
    }
    if (low > high) {
        range->first = range->end = 0;
        return;
    }
    const int64_t first = (int64_t)range->base_vertex + low, end = (int64_t)range->base_vertex + high + 1;
    if (first < 0 || end > 0xffffffffll) {
        range->whole = true; // vertices before the buffer's start: leave it to the whole-stream path
        return;
    }
    range->first = (uint32_t)first;
    range->end = (uint32_t)end;
}

// A stream's copy for this draw only, in the frame ring: the vertices the draw reads (or the stream from the vertex
// at its offset to its end when that is not known), bound so the draw's own vertex numbers address them.
bool upload_vertices_to_ring(const s_port_vk_stream &stream, const s_xenos_vertex_declaration *declaration, uint32_t s,
                             bool packed_prt, uint32_t vertices, s_vertex_range *range, s_data_binding *out)
{
    const uint8_t *source = vertex_bytes(stream, s, stream.offset);
    uint32_t first = 0, end = vertices;
    if (range != 0 && !packed_prt) {
        vertex_range_resolve(range);
        if (!range->whole) {
            first = range->first < vertices ? range->first : vertices;
            end = range->end < vertices ? range->end : vertices;
            end = end > first ? end : first;
        }
    }
    // the copy holds vertices [first, end); the binding starts `first` vertices before it, so it must not be placed
    // nearer the ring's start than that (vertex attribute addresses keep the source's alignment: the copy is 16-aligned)
    const uint32_t lead = first * stream.stride;
    void *dst = ring_alloc_after((end - first) * stream.stride, 16, lead, &out->offset);
    if (dst == 0)
        return false;
    convert_vertices(declaration, s, stream, packed_prt, source + (size_t)first * stream.stride, dst, end - first);
    g_converted_bytes[stream.key == 0 ? k_converted_user_pointer : k_converted_dynamic] += (uint64_t)(end - first) * stream.stride;
    out->offset -= lead;
    out->buffer = ring_buffer();
    return true;
}

bool upload_vertices(const s_port_vk_stream &stream, const s_xenos_vertex_declaration *declaration, uint32_t s,
                     s_vertex_range *range, s_data_binding *out)
{
    if (stream.data == 0 || stream.stride == 0 || stream.size <= stream.offset)
        return false;
    const uint32_t bytes = stream.size - stream.offset;
    // PRT ambient packs four vertices a dword (xenos_vertex_stream_is_packed_prt): the buffer holds one value a byte,
    // the copy one 4-byte vertex a value, so vertex v reads its own value at the stride the game set
    const bool packed_prt = xenos_vertex_stream_is_packed_prt(declaration, s, stream.stride) != 0;
    const uint32_t vertices = packed_prt ? bytes : bytes / stream.stride;
    const uint32_t used = vertices * stream.stride;
    const uint8_t *source = vertex_bytes(stream, s, stream.offset);
    if (stream.key == 0) // user-pointer data (DrawVerticesUP): this draw only
        return upload_vertices_to_ring(stream, declaration, s, packed_prt, vertices, range, out);
    const uint32_t key = key_of(stream.key) ^ mix32(stream.offset * 31u + s);
    const uint32_t signature =
        stream_layout_hash(declaration, s) ^ mix32(stream.stride) ^ (stream.big_endian ? 0x80000000u : 0u);
    const uint32_t generation = touch_generation(stream.key);
    uint32_t index;
    s_buffer_entry *entry = 0;
    if (map_find(&g_buffer_index, key, &index))
        entry = &g_buffers.items[index];
    if (entry != 0 && entry->signature == signature && entry->data == source && entry->size == used && !entry->dynamic &&
        entry->generation == generation && entry->buffer != 0) {
        out->buffer = entry->buffer;
        out->offset = 0;
        return true;
    }
    if (entry != 0 && entry->buffer != 0 && entry->signature == signature && entry->data == source && entry->size == used) {
        // rewritten since it was uploaded: from now on the frame ring carries it (a device buffer could still be
        // read by frames in flight)
        entry->dynamic = 1;
        entry->generation = generation;
        return upload_vertices_to_ring(stream, declaration, s, packed_prt, vertices, range, out);
    }
    if (entry == 0) {
        entry = g_buffers.push();
        if (entry == 0)
            return false;
        map_set(&g_buffer_index, key, g_buffers.count - 1u);
    } else if (entry->buffer != 0) {
        // draws already recorded, or frames in flight, may still read it
        gpu_buffer_retire(g_gpu, entry->buffer);
        entry->buffer = 0;
    }
    entry->key = stream.key;
    entry->signature = signature;
    entry->data = source;
    entry->size = used;
    entry->generation = generation;
    entry->dynamic = 0;
    void *converted = malloc(used ? used : 1u);
    if (converted == 0)
        return false;
    convert_vertices(declaration, s, stream, packed_prt, source, converted, vertices);
    g_converted_bytes[k_converted_static] += used;
    entry->buffer = gpu_buffer_create(g_gpu, used ? used : 16u, k_gpu_buffer_vertex, k_gpu_memory_gpu);
    bool ok = entry->buffer != 0 && gpu_buffer_upload(g_gpu, entry->buffer, 0, converted, used) == k_gpu_ok;
    free(converted);
    if (!ok)
        return false;
    out->buffer = entry->buffer;
    out->offset = 0;
    return true;
}

// Converts indices [first, first + count) to 32-bit (cache-file data swapped; a 16-bit 0xffff, the restart index,
// becomes 0xffffffff), expanding quads to two triangles each when `quads`. Returns the indices written.
uint32_t convert_indices(const s_port_vk_indices &indices, uint32_t first, uint32_t count, bool quads, uint32_t *dst)
{
    const uint8_t *src = index_bytes(indices, (size_t)first * indices.index_size);
    if (!quads) {
        if (indices.index_size == 2) {
            if (indices.big_endian)
                widen_indices16<true>(src, count, dst);
            else
                widen_indices16<false>(src, count, dst);
        } else if (indices.big_endian) {
            copy_indices32<true>(src, count, dst);
        } else {
            copy_indices32<false>(src, count, dst);
        }
        return count;
    }
    uint32_t n = 0;
    for (uint32_t q = 0; q + 4u <= count; q += 4u) {
        const uint32_t v0 = read_index(indices, src, q), v1 = read_index(indices, src, q + 1u),
                       v2 = read_index(indices, src, q + 2u), v3 = read_index(indices, src, q + 3u);
        dst[n++] = v0;
        dst[n++] = v1;
        dst[n++] = v2;
        dst[n++] = v0;
        dst[n++] = v2;
        dst[n++] = v3;
    }
    return n;
}

// The 32-bit indices [first, first + count) of a draw, expanding quads to triangles when `quads`. A D3DIndexBuffer the
// game has not rewritten since its first use is converted whole, once, into a device buffer (as upload_vertices keeps
// vertex buffers), and the draw binds it at the first index's offset; user-pointer indices, a quad list that does not
// start on a quad, and a buffer rewritten after its first use (from then on: dynamic) are copied into the frame ring
// for this draw only.
bool upload_indices(const s_port_vk_indices &indices, uint32_t first, uint32_t count, bool quads, s_data_binding *out,
                    uint32_t *out_count)
{
    if (indices.data == 0 || (indices.index_size != 2 && indices.index_size != 4))
        return false;
    if (((uint64_t)first + count) * indices.index_size > indices.size) {
        report(0x69647872u, "an indexed draw reads past its index buffer (skipped)");
        return false;
    }
    const uint32_t out_count_value = quads ? count / 4u * 6u : count;
    s_buffer_entry *entry = 0;
    if (indices.key != 0 && (!quads || (first & 3u) == 0)) {
        const uint32_t total = indices.size / indices.index_size;
        const uint32_t converted_count = quads ? total / 4u * 6u : total;
        const uint32_t signature = indices.index_size | (indices.big_endian ? 0x100u : 0u) | (quads ? 0x200u : 0u);
        const uint32_t key = key_of(indices.key) ^ mix32(signature);
        const uint32_t generation = touch_generation(indices.key);
        uint32_t index;
        if (map_find(&g_index_buffer_index, key, &index))
            entry = &g_buffers.items[index];
        if (entry != 0 && entry->key == indices.key && entry->signature == signature && entry->data == indices.data &&
            entry->size == indices.size && entry->generation == generation && !entry->dynamic && entry->buffer != 0) {
            out->buffer = entry->buffer;
            out->offset = (quads ? first / 4u * 6u : first) * 4u;
            *out_count = out_count_value;
            return true;
        }
        if (entry != 0 && entry->buffer != 0 && entry->key == indices.key && entry->signature == signature &&
            entry->data == indices.data && entry->size == indices.size) {
            // rewritten since it was converted: the ring from now on (frames in flight may read the device copy)
            entry->dynamic = 1;
            entry->generation = generation;
        } else if (converted_count != 0) {
            if (entry == 0) {
                entry = g_buffers.push();
                if (entry == 0)
                    return false;
                map_set(&g_index_buffer_index, key, g_buffers.count - 1u);
            } else if (entry->buffer != 0) {
                gpu_buffer_retire(g_gpu, entry->buffer);
                entry->buffer = 0;
            }
            entry->key = indices.key;
            entry->signature = signature;
            entry->data = indices.data;
            entry->size = indices.size;
            entry->generation = generation;
            entry->dynamic = 0;
            uint32_t *converted = (uint32_t *)malloc((size_t)converted_count * 4u);
            if (converted == 0)
                return false;
            convert_indices(indices, 0, quads ? total / 4u * 4u : total, quads, converted);
            entry->buffer = gpu_buffer_create(g_gpu, converted_count * 4u, k_gpu_buffer_index,
                                              k_gpu_memory_gpu);
            const bool ok = entry->buffer != 0 &&
                            gpu_buffer_upload(g_gpu, entry->buffer, 0, converted, converted_count * 4u) == k_gpu_ok;
            free(converted);
            if (ok) {
                out->buffer = entry->buffer;
                out->offset = (quads ? first / 4u * 6u : first) * 4u;
                *out_count = out_count_value;
                return true;
            }
            if (entry->buffer != 0)
                gpu_buffer_retire(g_gpu, entry->buffer);
            entry->buffer = 0;
        }
    }
    uint32_t *dst = (uint32_t *)ring_alloc(out_count_value * 4u + 4u, 16, &out->offset);
    if (dst == 0)
        return false;
    *out_count = convert_indices(indices, first, count, quads, dst);
    out->buffer = ring_buffer();
    return true;
}

// A triangle fan as the list of its triangles, for a backend that draws no fans (caps.triangle_fans = 0: Direct3D 12):
// triangle i of a fan is (i + 1, i + 2, 0), the order Vulkan draws a fan's triangles in (the provoking vertex first, the
// winding kept), and with `restart` a restart index (0xffffffff once converted) ends a fan, the next index beginning
// another. Indexed: the draw's indices [first, first + count) converted to 32 bits (convert_indices); else the vertices
// first .. first + count - 1. Written into the frame ring; false when the fan has no triangle to draw.
bool fan_indices(const s_port_vk_indices &indices, bool indexed, uint32_t first, uint32_t count, bool restart,
                 s_data_binding *out, uint32_t *out_count)
{
    if (count < 3u)
        return false;
    uint32_t *source = 0;
    if (indexed) {
        if (indices.data == 0 || (indices.index_size != 2 && indices.index_size != 4))
            return false;
        if (((uint64_t)first + count) * indices.index_size > indices.size) {
            report(0x69647872u, "an indexed draw reads past its index buffer (skipped)");
            return false;
        }
        source = (uint32_t *)malloc((size_t)count * 4u);
        if (source == 0)
            return false;
        convert_indices(indices, first, count, false, source);
    }
    uint32_t *dst = (uint32_t *)ring_alloc((count - 2u) * 12u + 4u, 16, &out->offset);
    if (dst == 0) {
        free(source);
        return false;
    }
    uint32_t n = 0, hub = 0, previous = 0, seen = 0;
    for (uint32_t i = 0; i < count; i++) {
        const uint32_t v = source != 0 ? source[i] : first + i;
        if (restart && source != 0 && v == 0xffffffffu) {
            seen = 0;
            continue;
        }
        if (seen == 0) {
            hub = v;
        } else if (seen >= 2u) {
            dst[n++] = previous;
            dst[n++] = v;
            dst[n++] = hub;
        }
        previous = v;
        seen++;
    }
    free(source);
    out->buffer = ring_buffer();
    *out_count = n;
    return n != 0;
}

// An indexed draw of `index_count` 32-bit indices at `binding` (upload_indices). The buffer is bound from its start
// and the binding's offset becomes the first index (its 32-bit indices are 4-aligned in a cached index buffer and
// 16-aligned in the ring), so draws from one buffer (every ring copy of the frame, the parts of one mesh) share one
// index buffer bind, which the backend then skips.
void draw_indexed_from(const s_data_binding &binding, uint32_t index_count, int32_t vertex_offset,
                       uint32_t instance_count, uint32_t first_instance, bool bind_at_offset = false)
{
    const bool direct_offset = bind_at_offset || (binding.offset & 3u) != 0;
    const uint32_t offset = direct_offset ? binding.offset : 0u;
    const uint32_t first = direct_offset ? 0u : binding.offset / 4u;
    if (g_vk) {
        vulkan_cmd_bind_index_buffer_native(g_vk, vulkan_gpu_native_buffer(binding.buffer), offset, VK_INDEX_TYPE_UINT32);
        vulkan_cmd_draw_indexed_inline(g_vk, index_count, instance_count, first, vertex_offset, first_instance);
    } else {
        gpu_cmd_bind_index_buffer(g_gpu, binding.buffer, offset, k_gpu_index_uint32);
        gpu_cmd_draw_indexed(g_gpu, index_count, instance_count, first, vertex_offset, first_instance);
    }
}

inline void record_draw(uint32_t count, uint32_t instances, uint32_t start, uint32_t first_instance)
{
    if (g_vk)
        vulkan_cmd_draw_inline(g_vk, count, instances, start, first_instance);
    else
        gpu_cmd_draw(g_gpu, count, instances, start, first_instance);
}

// ---- pipeline state ----

uint32_t blend_factor(uint32_t d3d)
{
    switch (d3d) {
    case 0: return k_gpu_blend_zero;
    case 1: return k_gpu_blend_one;
    case 4: return k_gpu_blend_src_color;
    case 5: return k_gpu_blend_one_minus_src_color;
    case 6: return k_gpu_blend_src_alpha;
    case 7: return k_gpu_blend_one_minus_src_alpha;
    case 8: return k_gpu_blend_dst_color;
    case 9: return k_gpu_blend_one_minus_dst_color;
    case 10: return k_gpu_blend_dst_alpha;
    case 11: return k_gpu_blend_one_minus_dst_alpha;
    case 12: return k_gpu_blend_constant_color;
    case 13: return k_gpu_blend_one_minus_constant_color;
    case 14: return k_gpu_blend_constant_alpha;
    case 15: return k_gpu_blend_one_minus_constant_alpha;
    case 16: return k_gpu_blend_src_alpha_saturate;
    default: return k_gpu_blend_one;
    }
}

uint32_t blend_op(uint32_t d3d)
{
    switch (d3d) {
    case 1: return k_gpu_blend_op_subtract;
    case 2: return k_gpu_blend_op_min;
    case 3: return k_gpu_blend_op_max;
    case 4: return k_gpu_blend_op_reverse_subtract;
    default: return k_gpu_blend_op_add;
    }
}

float float_bits(uint32_t bits)
{
    float f;
    memcpy(&f, &bits, 4);
    return f;
}

// The depth/stencil and rasterization fields Vulkan pipeline libraries take dynamically.
void build_raster_depth_state(const s_port_vk_render_state &rs, uint32_t scale, s_gpu_graphics_state &state)
{
    state.cull_mode = rs.cull_mode == 2 ? k_gpu_cull_front : rs.cull_mode == 6 ? k_gpu_cull_back : k_gpu_cull_none;
    state.front_face = k_gpu_front_face_clockwise;
    const bool depth = g_pass_depth != 0;
    state.depth_test = depth && rs.z_enable;
    state.depth_write = state.depth_test && rs.z_write_enable;
    state.depth_compare = depth ? rs.z_func & 7u : (uint32_t)k_gpu_compare_less_equal;
    const float bias = depth ? float_bits(rs.depth_bias) : 0.0f;
    const float slope = depth ? float_bits(rs.slope_scale_depth_bias) : 0.0f;
    state.depth_bias_enable = bias != 0.0f || slope != 0.0f;
    // Convert the constant bias to 24-bit depth units and the slope to host-pixel units.
    state.depth_bias_constant = state.depth_bias_enable ? bias * 16777216.0f : 0.0f;
    state.depth_bias_slope = state.depth_bias_enable ? slope * (float)scale : 0.0f;
    state.stencil_test = depth && rs.stencil_enable && state.stencil_format != k_gpu_format_undefined;
    const s_gpu_stencil_face keep = { k_gpu_stencil_keep, k_gpu_stencil_keep, k_gpu_stencil_keep, k_gpu_compare_always,
                                      0xff, 0xff, 0 };
    state.stencil_front = keep;
    state.stencil_back = keep;
    if (state.stencil_test) {
        state.stencil_front = { rs.stencil_fail & 7u, rs.stencil_pass & 7u, rs.stencil_z_fail & 7u, rs.stencil_func & 7u,
                               rs.stencil_mask & 0xffu, rs.stencil_write_mask & 0xffu, rs.stencil_ref & 0xffu };
        state.stencil_back = rs.two_sided_stencil_mode
            ? s_gpu_stencil_face { rs.ccw_stencil_fail & 7u, rs.ccw_stencil_pass & 7u, rs.ccw_stencil_z_fail & 7u,
                                   rs.ccw_stencil_func & 7u, rs.stencil_mask & 0xffu, rs.stencil_write_mask & 0xffu,
                                   rs.stencil_ref & 0xffu }
            : state.stencil_front;
    }
}

// `slots`: the color slots the draw's rendering attaches (attached_color_slots, as given to ensure_pass).
void build_state(const s_port_vk_render_state &rs, uint32_t topology, bool restart, uint32_t slots,
                 s_gpu_graphics_state *state)
{
    // ensure_pass may retain the previous depth attachment across a draw with depth and stencil disabled.
    gpu_graphics_state_default(state, k_gpu_format_undefined, g_pass_depth_format);
    state->topology = topology;
    state->primitive_restart = restart ? 1u : 0u;
    state->polygon_mode = rs.fill_mode == 0x25 ? k_gpu_polygon_line : rs.fill_mode == 1 ? k_gpu_polygon_point : k_gpu_polygon_fill;
    uint32_t count = 0;
    bool reads_blend_factor = false;
    for (uint32_t i = 0; i < 4; i++) {
        const bool bound = g_bound_color[i] != 0 && g_bound_color[i]->image != 0;
        if (bound && (slots & (1u << i)) == 0) {
            // a slot left out for another bound to the same EDRAM (attached_color_slots): no attachment, no writes
            state->color_formats[i] = k_gpu_format_undefined;
            state->color_exp_bias[i] = 0;
            state->blend[i] = s_gpu_blend_attachment {};
            continue;
        }
        state->color_formats[i] = bound ? g_bound_color[i]->host_format : (uint32_t)k_gpu_format_undefined;
        // RB_COLORi_INFO.ColorExpBias: the target scales what the pixel shader writes, before blending
        state->color_exp_bias[i] = state->color_formats[i] != k_gpu_format_undefined ? g_bound_color[i]->color_exp_bias : 0;
        if (state->color_formats[i] != k_gpu_format_undefined)
            count = i + 1u;
        // RB_BLENDCONTROLi: each render target blends by its own register. ONE, ADD, ZERO for color and alpha is the
        // Xenos encoding of no blending (what D3DRS_ALPHABLENDENABLE FALSE writes); it leaves blending off here, so
        // the source is written unchanged (a blend would compute 0 * NaN for a NaN destination).
        const uint32_t control = rs.blend_control[i];
        s_gpu_blend_attachment &b = state->blend[i];
        b.enable = (control & 0x1fff1fffu) != 0x00010001u ? 1u : 0u;
        b.src_color = blend_factor(control & 0x1fu);
        b.color_op = blend_op((control >> 5) & 7u);
        b.dst_color = blend_factor((control >> 8) & 0x1fu);
        b.src_alpha = blend_factor((control >> 16) & 0x1fu);
        b.alpha_op = blend_op((control >> 21) & 7u);
        b.dst_alpha = blend_factor((control >> 24) & 0x1fu);
        if (!b.enable)
            b.src_color = b.dst_color = b.src_alpha = b.dst_alpha = b.color_op = b.alpha_op = 0;
        b.write_mask = rs.color_write_enable[i] & 0xfu; // D3DCOLORWRITEENABLE_RED.. = GPU_COLOR_WRITE_R..
        const uint32_t factors[4] = { b.src_color, b.dst_color, b.src_alpha, b.dst_alpha };
        for (uint32_t f = 0; f < 4 && b.enable; f++)
            reads_blend_factor |= factors[f] >= k_gpu_blend_constant_color &&
                                  factors[f] <= k_gpu_blend_one_minus_constant_alpha;
    }
    // D3DRS_BLENDFACTOR: D3DDevice_SetRenderState_BlendFactor (d3d9/state.obj, 0x8216f008) writes RB_BLEND_RED..ALPHA,
    // the D3DCOLOR's bytes times 1/255. Set only when a target reads it (the decals' multiplier, the albedo pass's
    // motion blur mask: alpha = source alpha * CONSTANTALPHA), so the pipelines that do not keep one key.
    if (reads_blend_factor) {
        const uint32_t c = rs.blend_factor;
        state->blend_constants[0] = (float)((c >> 16) & 0xffu) * (1.0f / 255.0f);
        state->blend_constants[1] = (float)((c >> 8) & 0xffu) * (1.0f / 255.0f);
        state->blend_constants[2] = (float)(c & 0xffu) * (1.0f / 255.0f);
        state->blend_constants[3] = (float)(c >> 24) * (1.0f / 255.0f);
    }
    state->color_attachment_count = count;
    build_raster_depth_state(rs, g_pass_depth && rs.slope_scale_depth_bias ? bound_res_scale() : 1u, *state);
}

s_gpu_pipeline *pipeline_for(uint32_t vs, uint32_t ps, const s_xenos_vertex_declaration *declaration,
                             uint32_t declaration_hash, const s_gpu_graphics_state &state)
{
    s_pipeline_key key;
    memset(&key, 0, sizeof(key));
    key.vertex_shader = vs;
    key.pixel_shader = ps;
    key.declaration_hash = declaration_hash;
    key.state = state;
    uint32_t hash = gpu_hash_bytes(2166136261u, &key, sizeof(key));
    if (hash == 0)
        hash = 1;
    uint32_t first;
    if (map_find(&g_pipeline_index, hash, &first)) {
        for (uint32_t i = first; i < g_pipelines.count; i++) {
            s_pipeline_entry &e = g_pipelines.items[i];
            if (e.hash == hash && memcmp(&e.key, &key, sizeof(key)) == 0)
                return e.pipeline;
        }
    }
    const long long started = pipeline_stats_begin();
    s_gpu_pipeline *pipeline = xe_layout_call(gpu_pack_graphics_pipeline, g_gpu, g_pack, vs, ps, declaration, &state);
    pipeline_stats_end(started);
    s_pipeline_entry *entry = g_pipelines.push();
    if (entry != 0) {
        entry->key = key;
        entry->hash = hash;
        entry->pipeline = pipeline;
        if (!map_find(&g_pipeline_index, hash, &first))
            map_set(&g_pipeline_index, hash, g_pipelines.count - 1u);
    }
    if (pipeline == 0)
        report(hash, "pipeline creation failed (draws with it are skipped)");
    return pipeline;
}

// ---- descriptor sets ----

const s_port_vk_sampler *sampler_slot(const s_port_vk_draw *draw, uint32_t stage, uint32_t xbox_sampler)
{
    const uint32_t slot = stage == PORT_VK_STAGE_VERTEX ? PORT_VK_VERTEX_SAMPLER_BASE + xbox_sampler : xbox_sampler;
    return slot < PORT_VK_SAMPLER_COUNT ? &draw->samplers[slot] : 0;
}

// One stage's set: cbuffers gathered from the register shadows, textures and samplers from the Xbox sampler slots.
bool write_stage_set(s_draw_geometry draw, const s_gpu_pipeline *pipeline, uint32_t stage,
                     s_shader_entry *shader, gpu_bind_group *out_set)
{
    *out_set = GPU_NULL_HANDLE;
    s_shader_pack_shader_info info;
    if (!shader_pack_get_shader(g_pack, shader->shader_index, &info))
        return false;
    if (info.binding_count == 0)
        return true;
    gpu_bind_layout layout = gpu_pipeline_bind_layout(g_gpu, pipeline, stage);
    if (layout == GPU_NULL_HANDLE)
        return true;
    gpu_bind_group set = gpu_bind_group_allocate(g_gpu, layout, k_gpu_bind_frame);
    if (set == GPU_NULL_HANDLE)
        return false;
    s_shader_pack_microcode micro = {};
    if (shader->microcode != SHADER_PACK_NOT_FOUND)
        shader_pack_get_microcode(g_pack, shader->microcode, &micro);
    const float *floats = stage == PORT_VK_STAGE_VERTEX ? g_shadows.vertex_float : g_shadows.pixel_float;
    const uint32_t *bools = stage == PORT_VK_STAGE_VERTEX ? g_shadows.vertex_bool : g_shadows.pixel_bool;
    const uint32_t *ints = stage == PORT_VK_STAGE_VERTEX ? g_shadows.vertex_int : g_shadows.pixel_int;
    static const float k_zero_floats[1024] = {};
    static const uint32_t k_zero_dwords[32] = {};
    if (floats == 0)
        floats = k_zero_floats;
    if (bools == 0)
        bools = k_zero_dwords;
    if (ints == 0)
        ints = k_zero_dwords;

    for (uint32_t b = 0; b < info.binding_count; b++) {
        s_shader_pack_binding binding;
        if (!shader_pack_get_binding(g_pack, shader->shader_index, b, &binding))
            continue;
        const uint32_t count = binding.count != 0 ? binding.count : 1u;
        switch (binding.descriptor_type) {
        case k_shader_pack_descriptor_uniform_buffer: {
            uint32_t size = k_unknown_cbuffer_size;
            const s_shader_pack_file_gather *gathers = 0;
            uint32_t gather_count = 0, buffer_id = SHADER_PACK_GATHER_MATERIAL_PARAMETERS;
            if (binding.role == k_shader_pack_role_material_parameters) {
                size = info.material_parameters_size != 0 ? info.material_parameters_size : 16u;
            } else {
                s_shader_pack_cbuffer cbuffer;
                if (shader_pack_find_cbuffer(g_pack, binding.name_hash, &cbuffer)) {
                    size = cbuffer.size != 0 ? cbuffer.size : 16u;
                    gathers = cbuffer.gathers;
                    gather_count = cbuffer.gather_count;
                    buffer_id = cbuffer.table_index;
                } else {
                    report(binding.name_hash, "a cbuffer the pack has no gather table for (left zero)", binding.name);
                    buffer_id = 0xfffeu;
                }
            }
            uint32_t offset;
            void *data = ring_alloc(size, g_uniform_alignment, &offset);
            if (data == 0)
                return false;
            memset(data, 0, size);
            if (gathers != 0)
                shader_pack_apply_gathers(gathers, gather_count, buffer_id, data, size, floats, bools, ints);
            // this retail shader's own gathers: MaterialParameters, and its register overrides for this cbuffer
            if (micro.gather_count != 0)
                shader_pack_apply_gathers(micro.gathers, micro.gather_count, buffer_id, data, size, floats, bools, ints);
            write_pc_members(shader, binding.binding, draw.packet, data, size);
            if (tracing() && stage != PORT_VK_STAGE_VERTEX && binding.role == k_shader_pack_role_material_parameters) {
                char t[2000];
                int len = snprintf(t, sizeof(t), "trace   mp");
                s_shader_pack_parameter parameter;
                for (uint32_t q = 0; shader_pack_get_parameter(g_pack, shader->shader_index, q, &parameter) && len < 1900; q++) {
                    const float *v = (const float *)((const uint8_t *)data + parameter.offset);
                    len += snprintf(t + len, sizeof(t) - (size_t)len, " %s=%g,%g,%g,%g", parameter.name, v[0], v[1], v[2], v[3]);
                }
                snprintf(t + len, sizeof(t) - (size_t)len, "\n");
                port_write_error(t);
            }
            gpu_bind_write_buffer(g_gpu, set, binding.binding, 0, k_gpu_binding_uniform_buffer,
                                  ring_buffer(), offset, size);
            break;
        }
        case k_shader_pack_descriptor_sampled_image:
        case k_shader_pack_descriptor_sampler: {
            const s_port_vk_sampler *slot = 0;
            for (uint32_t m = 0; m < micro.sampler_count; m++) {
                if (micro.samplers[m].binding_index == b) {
                    slot = sampler_slot(draw.packet, stage, micro.samplers[m].sampler);
                    break;
                }
            }
            // MCC's colour grading LUTs, never bound by the game: the identity, read linearly and clamped
            const bool neutral_lut = color_grading_binding(binding) && (slot == 0 || !slot->bound) &&
                                     g_identity_lut != 0 && g_lut_sampler != GPU_NULL_HANDLE;
            for (uint32_t element = 0; element < count; element++) {
                if (binding.descriptor_type == k_shader_pack_descriptor_sampler) {
                    // a sampler the shader compares through (SampleCmp) needs compareEnable; without an Xbox sampler
                    // of its name (MCC's shared TempSamplerComparison__), the default state with linear filtering
                    const bool compare = shader_compares_through(shader, binding.set, binding.binding);
                    s_port_vk_sampler_state compare_default = {};
                    compare_default.mag_filter = compare_default.min_filter = 1;
                    compare_default.address_u = compare_default.address_v = compare_default.address_w = 2;
                    gpu_sampler sampler = neutral_lut ? g_lut_sampler
                                        : slot != 0 ? sampler_for(sampler_state_of(*slot), compare)
                                        : compare ? sampler_for(compare_default, true) : g_default_sampler;
                    gpu_bind_write_sampler(g_gpu, set, binding.binding, element, sampler);
                } else {
                    gpu_view view = slot != 0 && slot->bound ? texture_view(&slot->texture, binding.image_dim) : GPU_NULL_HANDLE;
                    if (tracing() && stage != PORT_VK_STAGE_VERTEX) {
                        char text[200];
                        snprintf(text, sizeof(text), "trace   bind %s <- s%d dim %u\n", binding.name ? binding.name : "?",
                                 slot != 0 ? (int)(slot - draw->samplers) : -1, binding.image_dim);
                        port_write_error(text);
                    }
                    if (view == GPU_NULL_HANDLE && neutral_lut)
                        view = gpu_image_view(g_gpu, g_identity_lut);
                    if (view == GPU_NULL_HANDLE)
                        view = dummy_view_for(binding);
                    gpu_bind_write_image(g_gpu, set, binding.binding, element,
                                         k_gpu_binding_sampled_image, view);
                }
            }
            break;
        }
        case k_shader_pack_descriptor_storage_buffer: {
            // StructuredBuffer t16 of a GPU-effect render shader (vs_particle_state_buffer,
            // vs_beam_profile_state_buffer, vs_contrail_profile_state_buffer, vs_light_volume_profile_state_buffer,
            // vs_ripple_buffer): the records the Xbox shader vfetches from its vertex stream, i.e. the storage of the
            // exported buffer that stream reads
            if (binding.binding != 116u) {
                // mesh_vertices t17 (particle_model): the model's own vertices, a converted copy of its stream; not
                // filled yet (REPORT_W2.md, follow-up)
                report(binding.name_hash ^ 0x6d657368u, "a storage buffer binding the D3D device does not fill (draw skipped)",
                       binding.name);
                return false;
            }
            const s_export_buffer *source = 0;
            for (uint32_t s = 0; s < PORT_VK_MAX_STREAMS && source == 0; s++)
                if ((draw.stream_mask() & (1u << s)) != 0 && draw.stream(s).data != 0)
                    source = export_buffer_find(draw.stream(s).data);
            uint32_t storage_size = 0;
            s_gpu_buffer *buffer = source != 0 ? storage_buffer_of(source->storage, &storage_size) : 0;
            if (buffer == 0) {
                // nothing was exported into the stream yet (an effect drawn before its first update): skip, as an
                // unwritten buffer would draw garbage
                report(binding.name_hash ^ 0x73747262u, "a draw reading a storage buffer no export wrote (skipped)",
                       binding.name);
                return false;
            }
            gpu_bind_write_buffer(g_gpu, set, binding.binding, 0, k_gpu_binding_storage_buffer, buffer,
                                  0, storage_size);
            break;
        }
        default:
            report(binding.descriptor_type * 0x1003u + 7u, "a shader binding type the D3D device cannot fill (draw skipped)",
                   binding.name);
            return false;
        }
    }
    *out_set = set;
    return true;
}

// ---- presentation ----

// HALO3_DUMP_RESOLVES=<first>:<last> (with HALO3_DUMP_FRAMES): every Resolve of frames first..last is also written as
// <dir>/frame_NNNN_rKK.ppm (8-bit, values clamped to [0, 1]; diagnostics for the intermediate targets).
// HALO3_DUMP_RESOLVES_FRAME=<N> is HALO3_DUMP_RESOLVES=N:N; HALO3_DUMP_RESOLVES_DIR=<dir> writes the resolve dumps there
// (HALO3_DUMP_FRAMES is then not needed). Each dumped frame also gets <dir>/frame_NNNN_resolves.tsv: one row per
// Resolve in issue order (depth ones included), with the draws before it (HALO3_TRACE_FRAME's serials), the source
// surface (EDRAM base, Xenos and host formats, size, ColorExpBias), the rectangle, the destination texture (format,
// size, point, level, slice), the resolve's exponent bias and the files written. Float targets are also written as
// <dir>/frame_NNNN_rKK.pfm (32-bit float RGB, unclamped: HDR values above 1 survive); a gamma target, which the port
// stores as linear 16-bit, also as frame_NNNN_rKK_pwl.ppm: the PWL codes the console's resolve writes to memory.
// Targets with a W channel also get frame_NNNN_rKK_a.pgm, their alpha plane (the albedo target's specular mask).
float half_to_float(uint16_t half)
{
    const uint32_t sign = (uint32_t)(half & 0x8000u) << 16, exponent = (half >> 10) & 0x1fu, mantissa = half & 0x3ffu;
    uint32_t bits;
    if (exponent == 0) {
        if (mantissa == 0) {
            bits = sign;
        } else { // denormal: renormalize
            uint32_t e = 113, m = mantissa;
            while ((m & 0x400u) == 0) {
                m <<= 1;
                e--;
            }
            bits = sign | e << 23 | (m & 0x3ffu) << 13;
        }
    } else if (exponent == 31) {
        bits = sign | 0x7f800000u | mantissa << 13;
    } else {
        bits = sign | (exponent + 112u) << 23 | mantissa << 13;
    }
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

uint8_t unit_to_byte(float value)
{
    return !(value > 0.0f) ? 0 : value >= 1.0f ? 255 : (uint8_t)(value * 255.0f + 0.5f);
}

bool resolve_dumping()
{
    return g_resolve_directory != 0 && g_presented >= g_dump_resolves_first && g_presented <= g_dump_resolves_last;
}

// Lists the resolve `note` describes in the frame's manifest and, with an image, queues the copy of its pixels
// (level / layer, the rectangle at x, y of note.width x note.height, in note.host_format).
void queue_resolve_dump(const s_resolve_dump &note, s_gpu_image *image, uint32_t level, uint32_t layer, uint32_t x,
                        uint32_t y)
{
    if (!resolve_dumping() || g_resolve_dump_count >= sizeof(g_resolve_dumps) / sizeof(g_resolve_dumps[0]))
        return;
    s_resolve_dump &dump = g_resolve_dumps[g_resolve_dump_count++];
    dump = note;
    dump.buffer = 0;
    uint32_t block_width = 0, block_height = 0, bytes = 0;
    if (image == 0 || note.width == 0 || note.height == 0 ||
        !gpu_format_block_info(note.host_format, &block_width, &block_height, &bytes) || block_width != 1)
        return;
    dump.buffer =
        gpu_buffer_create(g_gpu, note.width * note.height * bytes, k_gpu_buffer_transfer_dst, k_gpu_memory_readback);
    if (dump.buffer != 0)
        gpu_cmd_copy_image_to_buffer(g_gpu, image, level, layer, (int32_t)x, (int32_t)y, note.width, note.height,
                                     dump.buffer, 0);
}

// Xenos linear -> PWL gamma code (pwl_convert.hlsl linear_to_pwl, xenia LinearToPWLGamma), for a gamma target's dump
uint8_t linear_to_pwl_byte(float value)
{
    value = !(value > 0.0f) ? 0.0f : value > 1.0f ? 1.0f : value;
    float scale, offset;
    if (value >= 128.0f / 1023.0f) {
        if (value >= 512.0f / 1023.0f)
            scale = 1023.0f / 8.0f, offset = 128.0f;
        else
            scale = 1023.0f / 4.0f, offset = 64.0f;
    } else {
        if (value >= 64.0f / 1023.0f)
            scale = 1023.0f / 2.0f, offset = 32.0f;
        else
            scale = 1023.0f, offset = 0.0f;
    }
    const float code = (float)(int)(value * scale + 1.0f / 64.0f) + offset;
    return code >= 255.0f ? 255 : (uint8_t)code;
}

// <dir>/frame_NNNN<suffix>: the path, or false when it does not fit
bool resolve_dump_path(char *path, size_t size, uint32_t frame, const char *suffix)
{
    const int n = snprintf(path, size, "%s/frame_%04u%s", g_resolve_directory, frame, suffix);
    return n > 0 && (size_t)n < size - 1;
}

// Writes rows of 32-bit float RGB, bottom row first (the PFM convention), little-endian (scale -1)
bool write_pfm(const char *path, const float *rgb, uint32_t width, uint32_t height)
{
    FILE *file = fopen(path, "wb");
    if (file == 0)
        return false;
    char header[64];
    const int n = snprintf(header, sizeof(header), "PF\n%u %u\n-1.0\n", width, height);
    bool ok = n > 0 && fwrite(header, 1, (size_t)n, file) == (size_t)n;
    for (uint32_t y = height; ok && y-- > 0;)
        ok = fwrite(rgb + (size_t)y * width * 3u, sizeof(float), (size_t)width * 3u, file) == (size_t)width * 3u;
    return fclose(file) == 0 && ok;
}

// An 8-bit grey PGM of one channel: a resolve dump's alpha plane (the albedo target's specular mask, the normal
// target's 2-bit W), which the RGB dumps leave out.
bool write_pgm(const char *path, const uint8_t *grey, uint32_t width, uint32_t height)
{
    FILE *file = fopen(path, "wb");
    if (file == 0)
        return false;
    char header[64];
    const int n = snprintf(header, sizeof(header), "P5\n%u %u\n255\n", width, height);
    bool ok = n > 0 && fwrite(header, 1, (size_t)n, file) == (size_t)n &&
              fwrite(grey, 1, (size_t)width * height, file) == (size_t)width * height;
    return fclose(file) == 0 && ok;
}

// After the frame has finished: writes and frees the frame's resolve dumps, and its manifest.
void write_resolve_dumps(uint32_t frame)
{
    char path[512];
    FILE *manifest = resolve_dump_path(path, sizeof(path), frame, "_resolves.tsv") ? fopen(path, "wb") : 0;
    if (manifest != 0) {
        static const char k_header[] =
            "# seq\tr\tdraw_serial\tflags\tkind\tsource\tsrc_edram_base\tsrc_d3d_format\tsrc_host_format\tsrc_size\t"
            "src_exp_bias\trect\tdest\tdest_d3d_format\tdest_size\tdest_point\tlevel\tslice\tresolve_exp_bias\thost_format\t"
            "storage\tfiles\n";
        fwrite(k_header, 1, sizeof(k_header) - 1u, manifest);
    }
    for (uint32_t d = 0; d < g_resolve_dump_count; d++) {
        s_resolve_dump &dump = g_resolve_dumps[d];
        const uint32_t count = dump.width * dump.height;
        const bool is_float = dump.host_format == k_gpu_format_r16g16b16a16_sfloat || dump.host_format == k_gpu_format_r16g16_sfloat ||
                              dump.host_format == k_gpu_format_r32_sfloat;
        const bool is_pwl = dump.linear_storage != 0 && dump.host_format == k_gpu_format_r16g16b16a16_unorm;
        char files[160] = "-";
        const uint8_t *source = 0;
        uint8_t *pixels = 0, *codes = 0, *alpha = 0;
        float *rgb = 0;
        if (dump.buffer != 0) {
            gpu_buffer_invalidate(g_gpu, dump.buffer);
            source = (const uint8_t *)gpu_buffer_mapped(g_gpu, dump.buffer);
            pixels = (uint8_t *)malloc((size_t)count * 4u);
            rgb = is_float ? (float *)malloc((size_t)count * 3u * sizeof(float)) : 0;
            codes = is_pwl ? (uint8_t *)malloc((size_t)count * 4u) : 0;
        }
        // the formats with a W channel also get their alpha plane
        const bool has_alpha = dump.host_format == k_gpu_format_b8g8r8a8_unorm || dump.host_format == k_gpu_format_r8g8b8a8_unorm ||
                               dump.host_format == k_gpu_format_a2b10g10r10_unorm ||
                               dump.host_format == k_gpu_format_r16g16b16a16_sfloat || dump.host_format == k_gpu_format_r16g16b16a16_unorm;
        if (source != 0 && has_alpha)
            alpha = (uint8_t *)malloc((size_t)count);
        if (source != 0 && pixels != 0) {
            for (uint32_t i = 0; i < count; i++) {
                float c[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                switch (dump.host_format) {
                case k_gpu_format_b8g8r8a8_unorm:
                    c[0] = source[i * 4 + 2] / 255.0f, c[1] = source[i * 4 + 1] / 255.0f, c[2] = source[i * 4] / 255.0f;
                    c[3] = source[i * 4 + 3] / 255.0f;
                    break;
                case k_gpu_format_r8g8b8a8_unorm:
                    c[0] = source[i * 4] / 255.0f, c[1] = source[i * 4 + 1] / 255.0f, c[2] = source[i * 4 + 2] / 255.0f;
                    c[3] = source[i * 4 + 3] / 255.0f;
                    break;
                case k_gpu_format_a2b10g10r10_unorm: {
                    uint32_t v;
                    memcpy(&v, source + i * 4, 4);
                    c[0] = (v & 1023u) / 1023.0f, c[1] = ((v >> 10) & 1023u) / 1023.0f, c[2] = ((v >> 20) & 1023u) / 1023.0f;
                    c[3] = (float)(v >> 30) / 3.0f;
                    break;
                }
                case k_gpu_format_r16g16b16a16_sfloat:
                case k_gpu_format_r16g16_sfloat: {
                    const uint32_t n = dump.host_format == k_gpu_format_r16g16_sfloat ? 2u : 4u;
                    for (uint32_t k = 0; k < n; k++) {
                        uint16_t h;
                        memcpy(&h, source + (size_t)i * n * 2u + k * 2u, 2);
                        c[k] = half_to_float(h);
                    }
                    break;
                }
                case k_gpu_format_r32_sfloat:
                    memcpy(&c[0], source + i * 4, 4);
                    c[1] = c[2] = c[0];
                    break;
                case k_gpu_format_r16g16b16a16_unorm: // a gamma target's linear storage (format_is_gamma)
                    for (uint32_t k = 0; k < 4; k++) {
                        uint16_t u;
                        memcpy(&u, source + (size_t)i * 8u + k * 2u, 2);
                        c[k] = u / 65535.0f;
                    }
                    break;
                default:
                    break;
                }
                if (alpha != 0)
                    alpha[i] = unit_to_byte(c[3]);
                pixels[i * 4 + 0] = unit_to_byte(c[2]);
                pixels[i * 4 + 1] = unit_to_byte(c[1]);
                pixels[i * 4 + 2] = unit_to_byte(c[0]);
                pixels[i * 4 + 3] = 255;
                if (rgb != 0)
                    rgb[i * 3 + 0] = c[0], rgb[i * 3 + 1] = c[1], rgb[i * 3 + 2] = c[2];
                if (codes != 0) {
                    codes[i * 4 + 0] = linear_to_pwl_byte(c[2]);
                    codes[i * 4 + 1] = linear_to_pwl_byte(c[1]);
                    codes[i * 4 + 2] = linear_to_pwl_byte(c[0]);
                    codes[i * 4 + 3] = 255;
                }
            }
            char suffix[32];
            size_t at = 0;
            snprintf(suffix, sizeof(suffix), "_r%02u.ppm", dump.index);
            if (resolve_dump_path(path, sizeof(path), frame, suffix) &&
                gpu_write_ppm(path, pixels, dump.width, dump.height, dump.width * 4u, k_gpu_format_b8g8r8a8_unorm))
                at += (size_t)snprintf(files + at, sizeof(files) - at, "%s", suffix + 1);
            snprintf(suffix, sizeof(suffix), "_r%02u.pfm", dump.index);
            if (rgb != 0 && resolve_dump_path(path, sizeof(path), frame, suffix) && write_pfm(path, rgb, dump.width, dump.height))
                at += (size_t)snprintf(files + at, sizeof(files) - at, "%s%s", at != 0 ? "," : "", suffix + 1);
            snprintf(suffix, sizeof(suffix), "_r%02u_pwl.ppm", dump.index);
            if (codes != 0 && resolve_dump_path(path, sizeof(path), frame, suffix) &&
                gpu_write_ppm(path, codes, dump.width, dump.height, dump.width * 4u, k_gpu_format_b8g8r8a8_unorm))
                at += (size_t)snprintf(files + at, sizeof(files) - at, "%s%s", at != 0 ? "," : "", suffix + 1);
            snprintf(suffix, sizeof(suffix), "_r%02u_a.pgm", dump.index);
            if (alpha != 0 && resolve_dump_path(path, sizeof(path), frame, suffix) && write_pgm(path, alpha, dump.width, dump.height))
                at += (size_t)snprintf(files + at, sizeof(files) - at, "%s%s", at != 0 ? "," : "", suffix + 1);
            if (at == 0)
                files[0] = '-', files[1] = '\0';
        }
        if (manifest != 0) {
            char row[640];
            const int n = snprintf(
                row, sizeof(row),
                "%u\t%d\t%u\t%08x\t%s\t%s%u\t%08x\t%08x\t%s\t%ux%u\t%d\t%d,%d,%d,%d\t%p\t%08x\t%ux%u\t%u,%u\t%u\t%u\t%d\t%s\t%s\t%s\n",
                dump.sequence, dump.depth ? -1 : (int)dump.index, dump.draw_serial, dump.flags, dump.depth ? "depth" : "color",
                dump.source_key == 0 ? "none" : dump.depth ? "depth" : "rt", dump.depth ? 0u : dump.source_index,
                dump.source_edram_base, dump.source_d3d_format, gpu_format_name(dump.source_host_format), dump.source_width, dump.source_height,
                dump.source_exp_bias, dump.rect[0], dump.rect[1], dump.rect[2], dump.rect[3], dump.dest_key,
                dump.dest_d3d_format, dump.dest_width, dump.dest_height, dump.dest_x, dump.dest_y, dump.dest_level,
                dump.dest_slice, dump.resolve_exp_bias, gpu_format_name(dump.host_format),
                dump.linear_storage ? "linear16(gamma)" : is_float ? "float" : "unorm", files);
            if (n > 0)
                fwrite(row, 1, (size_t)n, manifest);
        }
        free(pixels);
        free(rgb);
        free(codes);
        free(alpha);
        if (dump.buffer != 0)
            gpu_buffer_destroy(g_gpu, dump.buffer);
        dump.buffer = 0;
    }
    if (manifest != 0)
        fclose(manifest);
    g_resolve_dump_count = 0;
}

void dump_frame(const void *pixels, uint32_t width, uint32_t height)
{
    if (g_dump_directory == 0)
        return;
    char path[512];
    size_t n = strlen(g_dump_directory);
    if (n > sizeof(path) - 32)
        return;
    memcpy(path, g_dump_directory, n);
    const char prefix[] = "/frame_";
    memcpy(path + n, prefix, sizeof(prefix) - 1);
    n += sizeof(prefix) - 1;
    uint32_t number = g_presented;
    char digits[10];
    int d = 0;
    do {
        digits[d++] = (char)('0' + number % 10u);
        number /= 10u;
    } while (number != 0 || d < 4);
    while (d > 0)
        path[n++] = digits[--d];
    memcpy(path + n, ".ppm", 5);
    if (!gpu_write_ppm(path, pixels, width, height, width * 4u, k_gpu_format_b8g8r8a8_unorm))
        report(0x64756d70u, "cannot write frame dumps", path);
}

// Whether the swapchain presents with vsync (FIFO): at the console's 30 frames a second; above it the frames are paced
// by the D3D device's vertical blanks (port_vk_frame_rate) and presented without waiting for the display.
// HALO3_VSYNC=0 / 1 decides either way; without it, halo3.ini's vsync (auto, 1, 0), which the Guide changes live.
uint32_t swapchain_fifo()
{
    int vsync = -1;
    if (port_config_locked(PORT_SETTING_VSYNC)) {
        const char *text = getenv("HALO3_VSYNC");
        if (text != 0 && (text[0] == '0' || text[0] == '1'))
            vsync = text[0] == '1';
    } else {
        vsync = port_config_vsync();
    }
    return vsync >= 0 ? (uint32_t)vsync : port_vk_frame_rate() == 30u ? 1u : 0u;
}

void create_swapchain()
{
    g_swapchain_revision = port_config_revision();
    const uint32_t fifo = swapchain_fifo();
    g_swapchain_fifo = fifo;
    gpu_result result = gpu_swapchain_create(g_gpu, g_window_width, g_window_height, fifo);
    if (result != k_gpu_ok) {
        device_log_text("swapchain creation failed: ");
        port_write_error(gpu_result_string(result));
        port_write_error("\n");
        g_window = 0;
    }
}

// ---- the persistent pipeline cache ----

// <data directory>/<the backend's file> (port_storage_data_file, gpu_backend_pipeline_cache_file: pipeline_cache.bin
// for Vulkan): the backend's pipeline cache blob (gpu_get_pipeline_cache) behind a header of its own, so a file cut
// short (the process killed while writing it) or from another build is ignored rather than handed to the driver. The
// driver checks its own header (vendor, device, cache UUID) and the backend starts empty when the driver refuses the
// data. HALO3_PIPELINE_CACHE=0 neither reads nor writes it (cold-start measurements).
struct s_pipeline_cache_header {
    uint32_t magic;   // k_pipeline_cache_magic
    uint32_t version; // k_pipeline_cache_version
    uint32_t size;    // bytes of cache data that follow
    uint32_t hash;    // FNV-1a of those bytes
};
const uint32_t k_pipeline_cache_magic = 0x43503348u; // "H3PC"
const uint32_t k_pipeline_cache_version = 1;
const uint32_t k_pipeline_cache_quiet_frames = 90; // presented frames without a new pipeline before it is saved
char g_pipeline_cache_path[PORT_STORAGE_PATH];
uint32_t g_pipeline_cache_saved;   // gpu_pipelines_created when the file was last written (or read)
uint32_t g_pipeline_cache_seen;    // ... at the last present
uint32_t g_pipeline_cache_quiet;   // presents since that count last changed
uint32_t g_pipeline_cache_written_size, g_pipeline_cache_written_hash; // what the file holds (pipeline_cache_write)

uint32_t pipeline_cache_hash(const uint8_t *data, uint32_t size)
{
    uint32_t h = 2166136261u;
    for (uint32_t i = 0; i < size; i++)
        h = (h ^ data[i]) * 16777619u;
    return h;
}

// The cache data the file holds (malloc'ed, the caller frees it), or 0. Also decides the path for pipeline_cache_save.
void *pipeline_cache_load(const char *file_name, uint32_t *out_size)
{
    *out_size = 0;
    g_pipeline_cache_path[0] = '\0';
    g_pipeline_cache_written_size = g_pipeline_cache_written_hash = 0;
    const char *setting = getenv("HALO3_PIPELINE_CACHE");
    if ((setting != 0 && strcmp(setting, "0") == 0) || !port_storage_data_file(file_name, g_pipeline_cache_path))
        return 0;
    FILE *file = fopen(g_pipeline_cache_path, "rb");
    if (file == 0)
        return 0;
    s_pipeline_cache_header header = {};
    void *data = 0;
    long length = fseek(file, 0, 2) == 0 ? ftell(file) : -1;
    if (length >= (long)sizeof(header) && fseek(file, 0, 0) == 0 && fread(&header, sizeof(header), 1, file) == 1 &&
        header.magic == k_pipeline_cache_magic && header.version == k_pipeline_cache_version &&
        (long)header.size == length - (long)sizeof(header) && header.size != 0) {
        data = malloc(header.size);
        if (data != 0 && (fread(data, 1, header.size, file) != header.size ||
                          pipeline_cache_hash((const uint8_t *)data, header.size) != header.hash)) {
            free(data);
            data = 0;
        }
    }
    fclose(file);
    if (data == 0) {
        report(0x70636c64u, "the pipeline cache file is damaged or of another version (starting empty)",
               g_pipeline_cache_path);
        return 0;
    }
    *out_size = header.size;
    g_pipeline_cache_written_size = header.size; // what the file already holds
    g_pipeline_cache_written_hash = header.hash;
    return data;
}

// The backend's pipeline cache written to the file unless its contents are what was last written (*last_size,
// *last_hash, updated). Any thread: the cache is internally synchronized, and this touches no other device state.
void pipeline_cache_write(uint32_t *last_size, uint32_t *last_hash)
{
    size_t size = 0;
    if (gpu_get_pipeline_cache(g_gpu, 0, &size) != k_gpu_ok || size == 0 || size > 0x7fffffffu)
        return;
    uint8_t *data = (uint8_t *)malloc(size);
    if (data == 0)
        return;
    gpu_result result = gpu_get_pipeline_cache(g_gpu, data, &size);
    const uint32_t hash = pipeline_cache_hash(data, (uint32_t)size);
    if ((result == k_gpu_ok || result == k_gpu_incomplete) && (size != *last_size || hash != *last_hash)) {
        s_pipeline_cache_header header = { k_pipeline_cache_magic, k_pipeline_cache_version, (uint32_t)size, hash };
        FILE *file = fopen(g_pipeline_cache_path, "wb");
        if (file == 0 || fwrite(&header, sizeof(header), 1, file) != 1 || fwrite(data, 1, size, file) != size)
            device_log_text("cannot write the pipeline cache\n");
        else
            *last_size = (uint32_t)size, *last_hash = hash;
        if (file != 0)
            fclose(file);
    }
    free(data);
}

bool prewarm_save_pipeline_cache(bool wait);
void xe_key_cache_save(bool wait);

// Writes the backend's pipeline cache when it gained pipelines since the file was last written: on a prewarm worker
// when there is one (tens of megabytes: not on the render thread), else here. wait: here and now, after any write in
// progress (shutdown).
void pipeline_cache_save(bool wait = false)
{
    if (wait)
        xe_key_cache_save(true);
    if (g_gpu == 0 || g_pipeline_cache_path[0] == '\0')
        return;
    const uint32_t created = gpu_pipelines_created(g_gpu);
    if (created == g_pipeline_cache_saved)
        return;
    g_pipeline_cache_saved = created;
    if (!prewarm_save_pipeline_cache(wait))
        pipeline_cache_write(&g_pipeline_cache_written_size, &g_pipeline_cache_written_hash);
}

// Once per present: the cache is written when pipelines stop being created for a while (a new map or menu settled),
// so a run that ends without a clean shutdown keeps what it compiled.
void pipeline_cache_present()
{
    if (g_presented % k_pipeline_cache_quiet_frames == 0)
        xe_key_cache_save(false);
    const uint32_t created = gpu_pipelines_created(g_gpu);
    if (created != g_pipeline_cache_seen) {
        g_pipeline_cache_seen = created;
        g_pipeline_cache_quiet = 0;
        return;
    }
    if (++g_pipeline_cache_quiet == k_pipeline_cache_quiet_frames)
        pipeline_cache_save();
}

const char *module_format_name(uint32_t format)
{
    return format == SHADER_PACK_MODULE_FORMAT_DXIL ? "DXIL"
         : format == SHADER_PACK_MODULE_FORMAT_SPIRV ? "SPIR-V" : "unknown";
}

int fail(const char *what, const char *why)
{
    log_text("");
    port_write_error(what);
    port_write_error(": ");
    port_write_error(why);
    port_write_error("; staying on the null device\n");
    port_vulkan_device_shutdown();
    __atomic_store_n(&g_started, -1, __ATOMIC_RELEASE);
    return 0;
}

bool parse_resolution(const char *text, uint32_t *w, uint32_t *h)
{
    if (text == 0)
        return false;
    uint32_t a = 0, b = 0;
    const char *p = text;
    while (*p >= '0' && *p <= '9')
        a = a * 10u + (uint32_t)(*p++ - '0');
    if (*p != 'x' && *p != 'X')
        return false;
    p++;
    while (*p >= '0' && *p <= '9')
        b = b * 10u + (uint32_t)(*p++ - '0');
    if (*p != '\0' || a == 0 || b == 0 || a > 16384 || b > 16384)
        return false;
    *w = a;
    *h = b;
    return true;
}

// A draw's viewport (the half-pixel offset on its origin) and scissor rectangle, clamped to the rendering area. Both
// are in guest pixels: HALO3_RES_SCALE multiplies them (the half-pixel offset too, as xenia's GetHostViewportInfo
// scales it with the draw resolution).
void set_viewport_scissor(const s_port_vk_draw *draw)
{
    if (draw->vp_gen != 0 && g_vp_cache.frame == g_frame_serial &&
        g_vp_cache.vp_gen == draw->vp_gen && g_vp_cache.pass_epoch == g_pass_format_epoch &&
        (!g_vk || (vulkan_recording::command_state_known(g_vk) && g_vk->viewport_valid && g_vk->scissor_valid &&
                   g_vp_cache.command_buffer == g_vk->command_buffer && g_vp_cache.rendering_epoch == g_vk->rendering_epoch)))
        return;
    const float *vp = draw->viewport;
    const uint32_t res_scale = bound_res_scale();
    const float scale = (float)res_scale;
    const float center = draw->pixel_center_offset * scale; // xenia: on the viewport origin, after the window offset
    uint32_t width = 0, height = 0;
    if (g_vk) {
        width = g_vk->rendering_width;
        height = g_vk->rendering_height;
    } else {
        gpu_rendering(g_gpu, &width, &height);
    }
    const auto viewport = [&](float x, float y, float w, float h, float min_depth, float max_depth) {
        if (g_vk)
            vulkan_cmd_set_viewport_inline(g_vk, x, y, w, h, min_depth, max_depth);
        else
            gpu_cmd_set_viewport(g_gpu, x, y, w, h, min_depth, max_depth);
    };
    const auto scissor = [&](int32_t x, int32_t y, uint32_t w, uint32_t h) {
        if (g_vk)
            vulkan_cmd_set_scissor_inline(g_vk, x, y, w, h);
        else
            gpu_cmd_set_scissor(g_gpu, x, y, w, h);
    };
    if (vp[2] > 0.0f && vp[3] > 0.0f)
        viewport(vp[0] * scale + center, vp[1] * scale + center, vp[2] * scale, vp[3] * scale,
                             vp[4], vp[5]);
    else
        viewport(center, center, (float)width, (float)height, 0.0f, 1.0f);
    if (draw->state.scissor_test_enable) {
        const int n = (int)res_scale;
        int x0 = draw->scissor[0] < 0 ? 0 : draw->scissor[0] * n, y0 = draw->scissor[1] < 0 ? 0 : draw->scissor[1] * n;
        int x1 = draw->scissor[2] * n, y1 = draw->scissor[3] * n;
        if (x1 > (int)width)
            x1 = (int)width;
        if (y1 > (int)height)
            y1 = (int)height;
        scissor(x0, y0, x1 > x0 ? (uint32_t)(x1 - x0) : 0u, y1 > y0 ? (uint32_t)(y1 - y0) : 0u);
    } else {
        scissor(0, 0, width, height);
    }
    g_vp_cache.frame = g_frame_serial;
    g_vp_cache.vp_gen = draw->vp_gen;
    g_vp_cache.pass_epoch = g_pass_format_epoch;
    if (g_vk) {
        g_vp_cache.command_buffer = g_vk->command_buffer;
        g_vp_cache.rendering_epoch = g_vk->rendering_epoch;
    }
}

#ifdef HALO3_64BIT
// Track just the vertical component that changes between strip and frame coordinates. Each component is
// ordinary data, signed PsParamGen Y, abs(Y), c108.y, or their sum. A conditional write can lose a proof.
// Unknown control flow, addressing or use of an unadjusted Y leaves the ordinary replay in charge.
bool shell_pixel_coordinates(s_shader_entry &entry, const s_shader_pack_xenos &ps,
                             const uint32_t *code, uint32_t dwords, bool host, bool dynamic_loops)
{
    enum row { ordinary, signed_y, local_y, origin, frame_y, uncertain };
    row regs[64][4] = {};
    if (ps.param_gen < 64)
        regs[ps.param_gen][1] = signed_y;
    const auto dependent = [](row r) { return r != ordinary && r != frame_y; };
    const auto join = [&](row a, row b) {
        return a == b ? a : dependent(a) || dependent(b) ? uncertain : ordinary;
    };
    const auto word = [&](uint32_t i) { return host ? code[i] : __builtin_bswap32(code[i]); };
    uint32_t sampled = 0, not_local = 0, first_branch = ~0u, loop_mask = 0;
    bool check_loops = false;
    // After a branch, only admit instructions that consume invariant values. Forget tracked frame rows
    // there (a preceding write could have been skipped), and never introduce a dependent register.
    // This also proves loop back edges without depending on an execution count.
    bool literal108 = false;
    for (uint32_t d = 0; d < ps.def_count; ++d)
        literal108 |= ps.defs[d].reg == 108;
    uint32_t limit = dwords / 3;
    for (uint32_t pc = 0; pc < limit * 2; ++pc) {
        const uint32_t i = pc / 2 * 3;
        const uint32_t cf = pc & 1 ? (word(i + 1) >> 16) | (word(i + 2) << 16) : word(i);
        const uint32_t op = pc & 1 ? word(i + 2) >> 28 : (word(i + 1) >> 12) & 15;
        if (!op || op == 12 || op == 15)
            continue;
        if (op >= 7 && op <= 11) {
            if (first_branch == ~0u) {
                first_branch = pc;
                for (auto &reg : regs)
                    for (auto &r : reg)
                        if (r == frame_y) r = ordinary;
            }
            if (op == 7 || op == 8)
                loop_mask |= 1u << ((cf >> 16) & 31);
            if (op == 9 || op == 10 || (cf & 8191) < first_branch || (op == 7 && (cf & 8192)))
                return false;
            continue;
        }
        const uint32_t start = cf & 4095, count = (cf >> 12) & 7;
        if (start + count > dwords / 3)
            return false;
        if (count && start < limit)
            limit = start;
        row before[64][4];
        if (op > 2)
            memcpy(before, regs, sizeof(regs));
        for (uint32_t k = 0; k < count; ++k) {
            const uint32_t at = (start + k) * 3;
            const uint32_t a = word(at), b = word(at + 1), c = word(at + 2);
            if ((cf >> (16 + 2 * k)) & 1) {
                if ((a & 31) != 1 || (a & ((1u << 11) | (1u << 18))))
                    return false;
                const row *src = regs[(a >> 5) & 63];
                const uint32_t dim = (c >> 14) & 3;
                for (uint32_t axis = 0; axis < (dim < 2 ? dim + 1 : 3); ++axis)
                    if (dependent(src[(a >> (26 + axis * 2)) & 3]))
                        return false;
                sampled |= 1u << ((a >> 20) & 31);
                if (dim != 1 || !(a & (1u << 25)) || src[(a >> 28) & 3] != frame_y || ((c >> 21) & 31))
                    not_local |= 1u << ((a >> 20) & 31);
                row *dst = regs[(a >> 12) & 63];
                for (uint32_t j = 0; j < 4; ++j)
                    if (first_branch == ~0u && ((b >> (j * 3)) & 7) != 7)
                        dst[j] = b >> 31 ? join(dst[j], ordinary) : ordinary;
                continue;
            }
            if (!(a & 0x8000u) && (a & 0x4040u)) // indirect temporary destination
                return false;
            const uint32_t vec = (c >> 24) & 31, scalar = a >> 26;
            const bool predicated = (b & (1u << 28)) != 0, output = (a & 0x8000) != 0;
            const uint32_t vm = (a >> 16) & 15, sm = (a >> 20) & 15;
            const auto indirect_origin = [&](uint32_t s, uint32_t reg) {
                const bool first = s == 0 || (s == 1 ? c >> 31 : (c >> 30) == 3);
                if (!(b & (1u << (first ? 31 : 30)))) return false;
                check_loops = true;
                // aL starts at zero. For these lighting loops, check every possible value at the draw.
                return (b & (1u << 29)) || !dynamic_loops || (reg <= 108 && 108 - reg <= 63);
            };
            const auto operand = [&](uint32_t s, uint32_t lane) {
                const uint32_t shift = 16 - s * 8, reg = (c >> shift) & 255;
                lane = (lane + (b >> (shift + lane * 2))) & 3;
                row r = ordinary;
                if (c & (1u << (31 - s))) {
                    if (reg & 64)
                        return uncertain;
                    r = regs[reg & 63][lane];
                    if ((reg & 128) && r == signed_y)
                        r = local_y;
                } else if (lane == 1) {
                    if (indirect_origin(s, reg)) return uncertain;
                    if (!literal108 && reg == 108) r = origin;
                }
                if ((b & (1u << (26 - s))) && r != ordinary)
                    return dependent(r) ? uncertain : ordinary;
                return r;
            };
            static const uint8_t nargs[] = {2,2,2,2,2,2,2,2,1,1,1,3,3,3,3,2,2,3,2,1,2,2,2,2,2,2,2,2,2,2,0,0};
            row result[4] = {};
            for (uint32_t j = 0; j < 4; ++j) {
                if (!(vm & (1u << j)) && vec < 20 && !(vec >= 15 && vm))
                    continue;
                const row x = operand(0, j), y = operand(1, j);
                if (vec == 0 && ((x == local_y && y == origin) || (x == origin && y == local_y))) {
                    if (first_branch != ~0u) return false;
                    result[j] = frame_y;
                    entry.shell_uses_origin = true;
                } else if ((vec == 2 || vec == 3) && x == y) { // mov = max(src,src)
                    result[j] = x;
                } else {
                    for (uint32_t s = 0; s < nargs[vec]; ++s)
                        if (dependent(operand(s, j)))
                            return false;
                }
                if (a & (1u << 24)) {
                    if (dependent(result[j]))
                        return false;
                    result[j] = ordinary;
                }
                if ((output || first_branch != ~0u) && dependent(result[j]))
                    return false;
            }
            if (scalar >= 42 && scalar <= 47) {
                const uint32_t temp = (scalar & 1) | ((c >> 28) & 2) | (b & 60);
                if (dependent(regs[temp][b & 3]) || ((3 + (b >> 6)) % 4 == 1 &&
                    (indirect_origin(2, c & 255) || ((c & 255) == 108 && !literal108))))
                    return false;
            } else if (scalar != 50 && scalar != 33) {
                const bool two = scalar == 0 || scalar == 2 || (scalar >= 4 && scalar <= 6) ||
                                 (scalar >= 23 && scalar <= 25);
                if (scalar > 50 || scalar == 41 || dependent(operand(2, 3)) ||
                    (two && dependent(operand(2, nargs[vec] == 3 ? 2 : 0))))
                    return false;
            }
            if (!output && first_branch == ~0u) {
                for (uint32_t j = 0; j < 4; ++j) {
                    row &v = regs[a & 63][j], &s = regs[(a >> 8) & 63][j];
                    if (vm & (1u << j))
                        v = predicated ? join(v, result[j]) : result[j];
                    if (sm & (1u << j))
                        s = predicated ? join(s, ordinary) : ordinary;
                }
            }
        }
        if (op > 2)
            for (uint32_t r = 0; r < 64; ++r)
                for (uint32_t c = 0; c < 4; ++c)
                    regs[r][c] = join(before[r][c], regs[r][c]);
    }
    entry.shell_loop_mask = check_loops ? loop_mask : 0;
    entry.shell_local_fetches = sampled & ~not_local;
    return true;
}
#endif

} // namespace

extern "C" {

int PORT_MS_ABI port_vulkan_device_startup(unsigned int back_buffer_width, unsigned int back_buffer_height)
{
    if (g_started)
        return g_started > 0;
    const char *enabled = getenv("HALO3_VULKAN");
    if (!enabled || enabled[0] != '1' || enabled[1] != '\0') {
        __atomic_store_n(&g_started, -1, __ATOMIC_RELEASE);
        return 0;
    }
    // the renderer first (HALO3_RENDERER, halo3.ini [display] renderer), before a window opens: the backend asked
    // for, else (unless HALO3_RENDERER_FALLBACK=0) the next that starts and takes the pack's module format. The log
    // lines until one is up carry the first one's name.
    uint32_t kinds[2];
    const uint32_t kind_count = gpu_backend_candidates(gpu_renderer_requested(), kinds);
    if (kind_count == 0) {
        g_log_name = "renderer";
        return fail("HALO3_RENDERER", "no renderer can start (see above)");
    }
    g_log_name = gpu_backend_name(kinds[0]);
    const char *window = getenv("HALO3_WINDOW");
    g_window_width = back_buffer_width;
    g_window_height = back_buffer_height;
    if (getenv("HALO3_RES") != 0 && !parse_resolution(getenv("HALO3_RES"), &g_window_width, &g_window_height))
        log_text("HALO3_RES is not WxH; using the back buffer size\n");
    g_window = window != 0 && window[0] == '1' && window[1] == '\0';
    if (g_window && !port_window_open("Halo 3", g_window_width, g_window_height)) {
        log_text("no window (see above); rendering offscreen\n");
        g_window = 0;
    }
    const char *dump = getenv("HALO3_DUMP_FRAMES");
    g_dump_directory = dump != 0 && dump[0] != '\0' ? dump : 0;
    if (const char *every = getenv("HALO3_DUMP_EVERY")) {
        uint32_t n = 0;
        for (const char *c = every; *c >= '0' && *c <= '9'; c++)
            n = n * 10u + (uint32_t)(*c - '0');
        g_dump_every = n != 0 ? n : 1u;
    }
    if (const char *range = getenv("HALO3_DUMP_RESOLVES")) {
        uint32_t first = 0, last = 0;
        const char *c = range;
        for (; *c >= '0' && *c <= '9'; c++)
            first = first * 10u + (uint32_t)(*c - '0');
        if (*c == ':')
            for (c++; *c >= '0' && *c <= '9'; c++)
                last = last * 10u + (uint32_t)(*c - '0');
        else
            last = first;
        g_dump_resolves_first = first;
        g_dump_resolves_last = last;
    }
    if (const char *frame = getenv("HALO3_DUMP_RESOLVES_FRAME")) {
        uint32_t n = 0;
        for (const char *c = frame; *c >= '0' && *c <= '9'; c++)
            n = n * 10u + (uint32_t)(*c - '0');
        g_dump_resolves_first = g_dump_resolves_last = n;
    }
    const char *resolve_directory = getenv("HALO3_DUMP_RESOLVES_DIR");
    g_resolve_directory = resolve_directory != 0 && resolve_directory[0] != '\0' ? resolve_directory : g_dump_directory;
    if (const char *trace = getenv("HALO3_TRACE_FRAME")) {
        uint32_t n = 0;
        for (const char *c = trace; *c >= '0' && *c <= '9'; c++)
            n = n * 10u + (uint32_t)(*c - '0');
        g_trace_frame = n;
    }
    if (const char *stats = getenv("HALO3_FRAME_STATS")) {
        uint32_t n = 0;
        for (const char *c = stats; *c >= '0' && *c <= '9' && n < 100000u; c++)
            n = n * 10u + (uint32_t)(*c - '0');
        g_frame_stats.intervals = n != 0 ? (float *)malloc(n * sizeof(float)) : 0;
        g_frame_stats_window = g_frame_stats.intervals != 0 ? n : 0u;
        QueryPerformanceFrequency(&g_frame_stats.frequency);
        memset(&g_pipeline_stats, 0, sizeof(g_pipeline_stats));
        g_pipeline_stats.started = frame_stats_now();
        // HALO3_FRAME_TRACE=<ms>: frames at least that long (decimal milliseconds; 0: every frame)
        if (const char *trace = getenv("HALO3_FRAME_TRACE")) {
            double ms = 0.0, scale = 1.0;
            bool fraction = false;
            for (const char *c = trace; (*c >= '0' && *c <= '9') || (*c == '.' && !fraction); c++) {
                if (*c == '.')
                    fraction = true;
                else if (fraction)
                    ms += (double)(*c - '0') * (scale *= 0.1);
                else
                    ms = ms * 10.0 + (double)(*c - '0');
            }
            g_frame_trace.on = g_frame_stats.frequency > 0;
            g_frame_trace.threshold = ms;
        }
    }
    g_texture_recheck = getenv("HALO3_TEXTURE_RECHECK") != 0 && getenv("HALO3_TEXTURE_RECHECK")[0] == '1';
    if (const char *present_resolve = getenv("HALO3_PRESENT_RESOLVE")) {
        g_present_resolve = 0;
        for (const char *c = present_resolve; *c >= '0' && *c <= '9'; c++)
            g_present_resolve = g_present_resolve * 10 + (*c - '0');
    }

    s_gpu_backend_create_info create = {};
    create.application_name = "halo3";
    create.window_system = g_window ? (uint32_t)port_window_system() : (uint32_t)k_gpu_window_system_none;
    create.device_index = -1;
    const char *why = "no renderer";
    const char *what = "gpu_backend_create";
    for (uint32_t i = 0; i < kind_count && g_gpu == 0; i++) {
        const uint32_t kind = kinds[i];
        g_log_name = gpu_backend_name(kind);
        const char *path = gpu_backend_shader_pack(kind);
        if (!path) {
            const char *explicit_pack = getenv("HALO3_SHADER_PACK");
            if (explicit_pack && *explicit_pack)
                path = explicit_pack;
        }
        const bool discover_abi6 = kind == k_gpu_backend_vulkan && path == 0;
        for (uint32_t attempt = 0; attempt < (discover_abi6 ? 2u : 1u) && !g_gpu; ++attempt) {
            shader_pack_close(g_pack);
            g_pack = 0;
            if (discover_abi6 && attempt == 0) {
                char installed[PORT_STORAGE_PATH];
                const char *candidates[] = { "shaders/halo3_shaders.abi6.pack", installed,
                                             "play/halo3_shaders.abi6.pack" };
                installed[0] = '\0';
                port_storage_data_file(candidates[0], installed);
                for (const char *candidate : candidates) {
                    if (!*candidate)
                        continue;
                    g_pack = shader_pack_open(candidate);
                    if (shader_pack_xenos_abi_version(g_pack) == XE_ABI_VERSION_BINDLESS)
                        break;
                    shader_pack_close(g_pack);
                    g_pack = 0;
                }
                if (!g_pack)
                    continue;
            } else {
                g_pack = shader_pack_open(path);
                if (!g_pack && !path && kind == k_gpu_backend_vulkan) {
                    char installed[PORT_STORAGE_PATH];
                    if (port_storage_data_file("shaders/halo3_shaders.pack", installed))
                        g_pack = shader_pack_open(installed);
                }
            }
            if (!g_pack) {
                why = shader_pack_last_error();
                what = "shader pack";
                continue;
            }
            if (shader_pack_xenos_abi_version(g_pack) == XE_ABI_VERSION_BINDLESS && discover_abi6 && attempt != 0) {
                why = "the fallback shader pack does not contain ABI 5";
                what = "shader pack";
                continue;
            }
            what = "gpu_backend_create";
            g_xenos = shader_pack_is_xenos(g_pack) != 0;
            const uint32_t abi = shader_pack_xenos_abi_version(g_pack);
            const bool abi6 = abi == XE_ABI_VERSION_BINDLESS;
            create.xenos_features = g_xenos ? 1u : 0u;
            const char *cache_file = gpu_backend_pipeline_cache_file(kind);
            char xenos_cache_file[64];
            if (kind == k_gpu_backend_vulkan && g_xenos) {
                snprintf(xenos_cache_file, sizeof(xenos_cache_file), "pipeline_cache.abi%u.%08x.bin",
                         abi, shader_pack_table_crc(g_pack));
                cache_file = xenos_cache_file;
            }
            uint32_t pipeline_cache_size = 0;
            void *pipeline_cache_data = pipeline_cache_load(cache_file, &pipeline_cache_size);
            create.pipeline_cache_data = pipeline_cache_data;
            create.pipeline_cache_size = pipeline_cache_size;
            const gpu_result result = gpu_backend_create(kind, &create, &g_gpu);
            free(pipeline_cache_data);
            if (result != k_gpu_ok) {
                why = gpu_result_string(result);
                g_gpu = 0;
                continue;
            }
            s_vulkan_backend *vk = vulkan_gpu_native_backend(g_gpu);
            if (g_gpu->caps.shader_module_format != shader_pack_module_format(g_pack)) {
                log_text("the shader pack holds ");
                port_write_error(module_format_name(shader_pack_module_format(g_pack)));
                port_write_error(" modules, this renderer takes ");
                port_write_error(module_format_name(g_gpu->caps.shader_module_format));
                port_write_error(" (HALO3_SHADER_PACK or HALO3_SHADER_PACK_DXIL)\n");
                why = "the shader pack is for another renderer";
            } else if (abi6 && !vulkan_backend_supports_xenos_abi6(vk)) {
                why = "the device lacks ABI 6 buffer address or descriptor indexing support";
            } else {
                s_gpu_xenos_layout layout;
                if (!abi6 || (g_gpu->caps.clip_distance && gpu_xenos_layout(g_gpu, g_pack, &layout) &&
                              xe_v6_allocate_arenas(vk))) {
                    g_vk = vk;
                    g_xe_abi6 = abi6;
                    break;
                }
                why = "the device could not create the ABI 6 Xenos layout";
            }
            gpu_backend_destroy(g_gpu);
            g_gpu = 0;
            if (discover_abi6 && attempt == 0)
                log_text("ABI 6 unavailable on this device, trying the fallback shader pack\n");
        }
        if (g_gpu == 0 && i + 1u < kind_count) {
            log_text("could not start (");
            port_write_error(why);
            port_write_error("); trying ");
            port_write_error(gpu_backend_name(kinds[i + 1u]));
            port_write_error("\n");
        }
    }
    g_pipeline_cache_saved = g_pipeline_cache_seen = g_pipeline_cache_quiet = 0;
    if (g_gpu == 0)
        return fail(what, why);
    g_log_name = g_gpu->caps.name;
    if (!g_xenos && shader_pack_microcode_count(g_pack) == 0)
        log_text("the shader pack has no microcode table: retail shaders will not resolve\n");
    if (g_xenos && shader_pack_xenos_count(g_pack) == 0)
        log_text("the Xenos shader pack has no shaders: retail shaders will not resolve\n");
    // the pack's modules must all load on this device before anything draws with them (version 3: on first use, by
    // gpu_pack_xenos_module; a module that does not load fails only the pipelines made with it)
    const unsigned int modules = shader_pack_module_count(g_pack);
    for (unsigned int i = 0; i < modules && !g_xenos; i++) {
        if (gpu_pack_shader_module(g_gpu, g_pack, i, 0) == GPU_NULL_HANDLE)
            return fail("shader pack", "a module did not load (see above)");
    }
    __atomic_store_n(&g_res_scale, 1, __ATOMIC_RELAXED);
    if (const char *scale = getenv("HALO3_RES_SCALE")) {
        const uint32_t n = scale[0] >= '1' && scale[0] <= '3' && scale[1] == '\0' ? (uint32_t)(scale[0] - '0') : 0u;
        if (n == 0)
            log_text("HALO3_RES_SCALE is not 1, 2 or 3; rendering at 1x\n");
        else if (n > 1 && !g_xenos)
            log_text("HALO3_RES_SCALE needs the version 3 (Xenos) shader pack; rendering at 1x\n");
        else
            __atomic_store_n(&g_res_scale, n, __ATOMIC_RELAXED);
    }
    g_back_buffer_width = back_buffer_width;
    g_back_buffer_height = back_buffer_height;
    g_back_buffer = gpu_render_target_create(g_gpu, back_buffer_width * g_res_scale,
                                             back_buffer_height * g_res_scale, k_gpu_format_b8g8r8a8_unorm, 0);
    if (!g_back_buffer)
        return fail("back buffer", "render target creation failed");
    const uint32_t alignment = g_gpu->caps.uniform_offset_alignment;
    if (alignment > g_uniform_alignment && alignment <= 4096u)
        g_uniform_alignment = alignment;
    for (uint32_t i = 0; i < g_gpu->caps.frames_in_flight; i++) {
        g_rings[i].buffer = gpu_buffer_create(g_gpu, k_ring_size,
                                              k_gpu_buffer_vertex | k_gpu_buffer_index |
                                                  k_gpu_buffer_uniform | k_gpu_buffer_storage,
                                              k_gpu_memory_upload);
        if (g_rings[i].buffer == 0)
            return fail("frame ring", "buffer creation failed");
        g_rings[i].mapped = (uint8_t *)gpu_buffer_mapped(g_gpu, g_rings[i].buffer);
    }
    g_dummy_2d = make_dummy(k_gpu_view_2d, 0xffffffffu);
    g_dummy_3d = make_dummy(k_gpu_view_3d, 0xffffffffu);
    g_dummy_cube = make_dummy(k_gpu_view_cube, 0xffffffffu);
    g_dummy_array = make_dummy(k_gpu_view_2d_array, 0xffffffffu);
    g_dummy_flat_normal = make_dummy(k_gpu_view_2d, 0u);
    s_gpu_sampler_desc sampler = {};
    sampler.mag_filter = sampler.min_filter = k_gpu_filter_linear;
    sampler.mipmap_mode = k_gpu_filter_linear;
    sampler.max_lod = 1000.0f;
    sampler.max_anisotropy = 1.0f;
    g_default_sampler = gpu_sampler_create(g_gpu, &sampler);
    g_identity_lut = make_identity_lut();
    sampler.mipmap_mode = k_gpu_filter_nearest;
    sampler.address_u = sampler.address_v = sampler.address_w = k_gpu_address_clamp_to_edge;
    sampler.max_lod = 0.0f;
    g_lut_sampler = gpu_sampler_create(g_gpu, &sampler);
    if (g_dummy_2d == 0 || g_default_sampler == GPU_NULL_HANDLE)
        return fail("fallback texture", "creation failed");
    if (g_xenos && !xenos_startup())
        return fail("Xenos shader pack", "the device cannot run the translated shaders (see above)");
    if (g_window) {
        const gpu_result surface = gpu_surface_create(g_gpu, (uint32_t)port_window_system(), port_window_native_display(),
                                                      (uintptr_t)port_window_native_window());
        if (surface != k_gpu_ok) {
            log_text("window surface creation failed; rendering offscreen\n");
            g_window = 0;
        } else {
            create_swapchain();
        }
    }

    const s_gpu_caps &caps = g_gpu->caps;
    log_text("up on ");
    port_write_error(caps.device_name);
    port_write_error(" (");
    port_write_error(caps.api_name);
    port_write_error(" ");
    write_uint(caps.api_major);
    port_write_error(".");
    write_uint(caps.api_minor);
    if (g_xenos) {
        port_write_error("), Xenos shader pack (version ");
        write_uint(shader_pack_version(g_pack));
        port_write_error(", ABI ");
        write_uint(shader_pack_xenos_abi_version(g_pack));
        port_write_error(") ");
        write_uint(shader_pack_xenos_count(g_pack));
        port_write_error(" translated shaders / ");
        write_uint(modules);
        port_write_error(" modules, back buffer ");
    } else {
        port_write_error("), shader pack ");
        write_uint(shader_pack_shader_count(g_pack));
        port_write_error(" shaders / ");
        write_uint(modules);
        port_write_error(" modules / ");
        write_uint(shader_pack_microcode_count(g_pack));
        port_write_error(" retail microcode entries, back buffer ");
    }
    write_uint(back_buffer_width);
    port_write_error("x");
    write_uint(back_buffer_height);
    if (g_res_scale > 1) {
        port_write_error(" rendered at ");
        write_uint(g_res_scale);
        port_write_error("x (");
        write_uint(back_buffer_width * g_res_scale);
        port_write_error("x");
        write_uint(back_buffer_height * g_res_scale);
        port_write_error(")");
    }
    if (g_window) {
        port_write_error(", window ");
        write_uint(g_window_width);
        port_write_error("x");
        write_uint(g_window_height);
    }
    if (g_dump_directory != 0) {
        port_write_error(", dumping frames to ");
        port_write_error(g_dump_directory);
    }
    port_write_error("\n");
    record_start();
    __atomic_store_n(&g_started, 1, __ATOMIC_RELEASE);
    return 1;
}

int PORT_MS_ABI port_vulkan_device_active(void)
{
    return port_vulkan_device_active_fast();
}

void PORT_MS_ABI port_vulkan_device_shutdown(void)
{
    const bool threaded = record_remote();
    if (threaded) {
        record_stop({ [](const void *) { finish_frame(); gpu_wait_idle(g_gpu); }, nullptr, 0 });
    }
    if (g_gpu == 0) {
        // the pack opens before the backend (its version picks the device features): it may be all there is
        if (g_pack)
            shader_pack_close(g_pack);
        g_pack = 0;
        g_xenos = false;
        __atomic_store_n(&g_started, 0, __ATOMIC_RELEASE);
        return;
    }
    if (!threaded) {
        finish_frame();
        gpu_wait_idle(g_gpu);
    }
#ifdef HALO3_64BIT
    if (g_cropped_depth_pipeline) {
        vulkan_pipeline_destroy(g_vk, g_cropped_depth_pipeline);
        g_cropped_depth_pipeline = 0;
    }
#endif
    pipeline_cache_save(true);
    xenos_shutdown();
    for (uint32_t i = 0; i < g_pipelines.count; i++)
        gpu_pipeline_destroy(g_gpu, g_pipelines.items[i].pipeline);
    for (uint32_t i = 0; i < g_compute_pipelines.count; i++)
        if (g_compute_pipelines.items[i].pipeline != 0)
            gpu_pipeline_destroy(g_gpu, g_compute_pipelines.items[i].pipeline);
    for (uint32_t i = 0; i < g_samplers.count; i++)
        gpu_sampler_destroy(g_gpu, g_samplers.items[i].sampler);
    for (uint32_t i = 0; i < g_textures.count; i++) {
        if (g_textures.items[i].image != 0)
            gpu_image_destroy(g_gpu, g_textures.items[i].image);
        if (g_textures.items[i].readback != 0)
            gpu_buffer_destroy(g_gpu, g_textures.items[i].readback);
        if (g_textures.items[i].readback_image != 0)
            gpu_image_destroy(g_gpu, g_textures.items[i].readback_image);
        if (g_textures.items[i].packed != 0)
            gpu_image_destroy(g_gpu, g_textures.items[i].packed);
        for (int c = 0; c < 2; c++)
            if (g_textures.items[i].converted[c] != 0)
                gpu_image_destroy(g_gpu, g_textures.items[i].converted[c]);
    }
    for (uint32_t i = 0; i < g_targets.count; i++) {
        if (g_targets.items[i].shell_image)
            gpu_image_destroy(g_gpu, g_targets.items[i].shell_image);
        if (!g_targets.items[i].back_buffer && g_targets.items[i].image != 0)
            gpu_image_destroy(g_gpu, g_targets.items[i].image);
    }
    for (uint32_t i = 0; i < g_buffers.count; i++)
        gpu_buffer_destroy(g_gpu, g_buffers.items[i].buffer);
    for (uint32_t i = 0; i < GPU_FRAMES_MAX; i++) {
        if (g_rings[i].buffer != 0)
            gpu_buffer_destroy(g_gpu, g_rings[i].buffer);
        g_rings[i].buffer = 0;
        g_rings[i].mapped = 0;
    }
    gpu_image_destroy(g_gpu, g_dummy_2d);
    gpu_image_destroy(g_gpu, g_dummy_3d);
    gpu_image_destroy(g_gpu, g_dummy_cube);
    gpu_image_destroy(g_gpu, g_dummy_array);
    gpu_image_destroy(g_gpu, g_dummy_flat_normal);
    g_dummy_2d = g_dummy_3d = g_dummy_cube = g_dummy_array = g_dummy_flat_normal = 0;
    if (g_default_sampler != GPU_NULL_HANDLE)
        gpu_sampler_destroy(g_gpu, g_default_sampler);
    g_default_sampler = GPU_NULL_HANDLE;
    gpu_image_destroy(g_gpu, g_identity_lut);
    g_identity_lut = 0;
    gpu_image_destroy(g_gpu, g_gather_scratch);
    g_gather_scratch = 0;
    if (g_lut_sampler != GPU_NULL_HANDLE)
        gpu_sampler_destroy(g_gpu, g_lut_sampler);
    g_lut_sampler = GPU_NULL_HANDLE;
    if (g_window) {
        gpu_swapchain_destroy(g_gpu);
        gpu_surface_destroy(g_gpu);
        port_window_close();
        g_window = 0;
    }
    for (s_scratch_image &scratch : g_resolve_scratch) {
        gpu_image_destroy(g_gpu, scratch.image);
        scratch = s_scratch_image();
    }
    for (s_display_target &display : g_display_targets) {
        if (display.target != 0)
            gpu_render_target_destroy(g_gpu, display.target);
        display = s_display_target();
    }
    if (g_back_buffer)
        gpu_render_target_destroy(g_gpu, g_back_buffer);
    gpu_pack_release(g_gpu);
    if (g_pack)
        shader_pack_close(g_pack);
    gpu_backend_destroy(g_gpu);
    g_back_buffer = 0;
    g_pack = 0;
    g_gpu = 0;
    g_vk = 0;
    g_xe_abi6 = false;
    g_active_query = 0;
    g_pipelines.release();
    g_compute_pipelines.release();
    g_samplers.release();
    map_free(&g_sampler_index);
    memset(g_sampler_memo, 0, sizeof(g_sampler_memo));
    g_textures.release();
    g_targets.release();
    g_pending_7e3_texture = 0;
    g_buffers.release();
    g_shaders.release();
    g_declarations.release();
    map_free(&g_pipeline_index);
    map_free(&g_texture_index);
    map_free(&g_target_index);
    map_free(&g_buffer_index);
    map_free(&g_index_buffer_index);
    map_free(&g_shader_index);
    map_free(&g_declaration_index);
    map_free(&g_touches);
    {
        s_memory_records_lock lock;
        map_free(&g_resolved_memory);
        g_resolved_headers.clear();
        g_resolved_bases.clear();
        resolve_state_changed(); // the textures go (slot_resolved)
        map_free(&g_host_order_memory);
    }
    map_free(&g_edram_owner);
    g_edram_serial = 0;
    g_storage_a2b10g10r10 = -1;
    for (int i = 0; i < 4; i++)
        g_bound_color[i] = 0;
    g_bound_depth = 0;
    free(g_last_frame);
    free(g_texture_scratch);
    g_texture_scratch = 0;
    g_texture_scratch_size = 0;
    g_last_frame = 0;
    g_vk_display_gamma_initialized = false;
    g_vk_reinterpret_7e3_initialized = false;
    __atomic_store_n(&g_started, 0, __ATOMIC_RELEASE);
}

void PORT_MS_ABI port_vk_record_drain(void)
{
    if (!record_remote()) return;
    if (record_owner()) return record_wait(record_head(), "drain");
    s_record_client client;
    if (client.admitted) record_wait(record_head(), "drain");
}

// Final release and BlockUntilNotBusy only fence packets that can still use
// the resource's guest storage. All other resource descriptors are packet-owned.
void PORT_MS_ABI port_vk_record_resource_idle(const void *texture, const void *base, uint32_t size)
{
    if (!record_remote()) return;
    s_record_client client;
    if (!client.admitted) return;
    s_record_range ranges[2] = {{base, size}, {}};
    if (texture) {
        s_port_d3d_texture_source source = {};
        if (!port_d3d_texture_get_source(texture, &source)) return;
        record_texture_ranges(source, ranges);
    }
    if (record_gpu_write(ranges[0], texture) || (ranges[1].base && record_gpu_write(ranges[1], nullptr))) {
        record_wait_borrowers();
        return record_wait(record_head(), "resource");
    }
    for (const auto &r : ranges) record_wait_reads(r, "res-idle");
}

void PORT_MS_ABI port_vk_record_acquire(void)
{
    // The HAL ownership handoff serializes the two producers. Packets and the
    // slab stay live through their own sequence numbers across that handoff.
    if (record_remote())
        __atomic_exchange_n(&g_record.owner, (uint32_t)GetCurrentThreadId(), __ATOMIC_ACQ_REL);
}

void PORT_MS_ABI port_vk_record_release(void)
{
    if (record_remote() && record_owner())
        __atomic_store_n(&g_record.owner, 0u, __ATOMIC_RELEASE);
}

// A CPU writer can reuse a movie plane or a dynamic buffer after its D3D
// fence. The original GPU lifetime is already handled by the backend's slot fences.
static uint64_t g_record_fences[k_record_slots];

void PORT_MS_ABI port_vk_record_fence(uint32_t fence)
{
    if (record_remote())
        __atomic_store_n(&g_record_fences[fence % k_record_slots],
                         (uint64_t(fence) << 32) | uint32_t(record_head()), __ATOMIC_RELEASE);
}

int PORT_MS_ABI port_vk_record_fence_pending(uint32_t fence, int wait)
{
    if (!fence || !record_remote()) return 0;
    s_record_client client;
    if (!client.admitted) return 0;
    const uint64_t head = record_head();
    const uint64_t stamp = __atomic_load_n(&g_record_fences[fence % k_record_slots], __ATOMIC_ACQUIRE);
    // The ring never has more than 4096 outstanding commands. An overwritten
    // fence slot is rare and may conservatively order through all published work.
    const uint64_t seq = uint32_t(stamp >> 32) == fence ? head - uint32_t(head - uint32_t(stamp)) : head;
    if (seq > head || seq <= record_tail()) return 0;
    if (wait) record_wait(seq, "fence");
    return !wait;
}

void PORT_MS_ABI port_vk_set_shadows(const s_port_vk_shadows *shadows)
{
    if (record_remote() && shadows) {
        // A foreign synchronous draw can be using the previous live shadows.
        record_sync([&] {
            g_live_shadows = *shadows;
            __atomic_add_fetch(&g_record_revision, 1u, __ATOMIC_RELEASE);
        });
        return;
    }
    if (shadows != 0)
        g_shadows = *shadows;
}

void PORT_MS_ABI port_vk_shader_register(const void *object, uint32_t stage, const void *microcode, uint32_t size_bytes,
                             uint32_t words_in_host_order)
{
    if (record_remote()) {
        auto &hint = record_linear_vs_slot(object);
        if (stage == PORT_VK_STAGE_VERTEX) {
            s_record_resource_guard guard;
            ++hint.pending;
            __atomic_store_n(&hint.shader, (const void *)nullptr, __ATOMIC_RELEASE);
        }
        return record_array((const uint8_t *)microcode, size_bytes, [=](const uint8_t *copy) {
            port_vk_shader_register(object, stage, copy, size_bytes, words_in_host_order);
            if (stage == PORT_VK_STAGE_VERTEX) {
                const auto *entry = shader_entry(object);
                s_shader_pack_xenos meta;
                const bool linear = g_xenos && entry && shader_pack_get_xenos(g_pack, entry->xenos, &meta) &&
                                    meta.model == k_shader_pack_xenos_vertex;
                s_record_resource_guard guard;
                auto &slot = record_linear_vs_slot(object);
                if (!--slot.pending)
                    __atomic_store_n(&slot.shader, linear ? object : nullptr, __ATOMIC_RELEASE);
            }
        });
    }
    if (g_started <= 0 || object == 0)
        return;
    uint32_t index;
    s_shader_entry *entry;
    if (map_find(&g_shader_index, key_of(object), &index)) {
        entry = &g_shaders.items[index];
    } else {
        entry = g_shaders.push();
        if (entry == 0)
            return;
        map_set(&g_shader_index, key_of(object), g_shaders.count - 1u);
    }
    entry->object = object;
    entry->stage = stage;
    entry->microcode = SHADER_PACK_NOT_FOUND;
    entry->xenos = SHADER_PACK_NOT_FOUND;
    entry->shader_index = SHADER_PACK_NOT_FOUND;
    entry->xenos_cached = 0;
    entry->dedup_index = 0;
    entry->xenos_compute = 0;
    entry->reported = 0;
    entry->clips = 0;
    entry->pc_scanned = 0;
    entry->reads_instance = 0;
    entry->compare_scanned = 0;
    entry->compare_count = 0;
    if (microcode == 0 || size_bytes == 0 || (size_bytes & 3u) != 0)
        return;
    uint32_t lo, hi;
    shader_pack_hash_microcode(microcode, size_bytes, (int)words_in_host_order, &lo, &hi);
    entry->hash_lo = lo;
    entry->hash_hi = hi;
    s_shader_pack_microcode m;
    entry->microcode = shader_pack_find_microcode(g_pack, stage, lo, hi, &m);
    if (entry->microcode != SHADER_PACK_NOT_FOUND)
        entry->shader_index = m.shader_index;
#ifdef HALO3_64BIT
    // The legacy pack has no translation for the executable's Bink pixel shader.
    // Its yuv_to_rgb shader uses the same four texture slots and c3-c6 conversion constants.
    if (!g_xenos && stage == PORT_VK_STAGE_PIXEL && hi == 0xb8be9f32u && lo == 0xf317135bu &&
        entry->shader_index == SHADER_PACK_NOT_FOUND) {
        const uint32_t fallback = shader_pack_find_microcode(g_pack, stage, 0xd49b09f5u, 0x1b880144u, &m);
        if (fallback != SHADER_PACK_NOT_FOUND && m.shader_index != SHADER_PACK_NOT_FOUND) {
            entry->microcode = fallback;
            entry->shader_index = m.shader_index;
        }
    }
#endif
    if (g_xenos) {
        s_shader_pack_xenos x;
        entry->xenos = shader_pack_find_xenos(g_pack, stage, lo, hi, &x);
#ifdef HALO3_64BIT
        entry->shell_uses_origin = false;
        entry->shell_coordinates = false;
        entry->shell_loop_mask = 0;
        entry->shell_local_fetches = 0;
#endif
        // its pipeline library compiled now, on a worker, rather than when a draw first needs it
        if (entry->xenos != SHADER_PACK_NOT_FOUND)
            xe_prewarm_shader(entry->xenos);
    }
    // HALO3_DUMP_UNKNOWN_SHADERS=<dir>: the microcode of each shader the pack does not know, as the Xbox holds it
    // (big-endian words), as <dir>/<hash>.vs|ps.bin, to find where it comes from
    static const char *unknown_directory = getenv("HALO3_DUMP_UNKNOWN_SHADERS");
    if ((g_xenos ? entry->xenos : entry->microcode) == SHADER_PACK_NOT_FOUND && unknown_directory != 0 &&
        unknown_directory[0] != 0) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%08x%08x.%s.bin", unknown_directory, hi, lo,
                 stage == PORT_VK_STAGE_VERTEX ? "vs" : "ps");
        if (FILE *file = fopen(path, "wb")) {
            const uint32_t *words = (const uint32_t *)microcode;
            for (uint32_t w = 0; w < size_bytes / 4u; w++) {
                const uint32_t big = words_in_host_order ? __builtin_bswap32(words[w]) : words[w];
                fwrite(&big, 4, 1, file);
            }
            fclose(file);
        }
    }
}

void PORT_MS_ABI port_vk_shader_forget(const void *object)
{
    if (record_remote()) {
        return record_dispatch([=] { port_vk_shader_forget(object); });
    }
    uint32_t index;
    if (g_started > 0 && map_find(&g_shader_index, key_of(object), &index)) {
        g_shaders.items[index].shader_index = SHADER_PACK_NOT_FOUND;
        g_shaders.items[index].xenos = SHADER_PACK_NOT_FOUND;
    }
}

uint32_t PORT_MS_ABI port_vk_shader_pack_index(const void *object)
{
    if (record_remote()) {
        return record_read([&] { return port_vk_shader_pack_index(object); });
    }
    const s_shader_entry *entry = shader_entry(object);
    return entry != 0 ? entry->shader_index : SHADER_PACK_NOT_FOUND;
}

void PORT_MS_ABI port_vk_set_back_buffer(const s_port_vk_surface *surface)
{
    if (record_remote()) {
        const auto copy = record_optional(surface);
        return record_dispatch([=] { port_vk_set_back_buffer(copy.get()); });
    }
    if (g_started <= 0 || surface == 0)
        return;
    uint32_t index;
    s_target_entry *entry;
    if (map_find(&g_target_index, key_of(surface->key), &index)) {
        entry = &g_targets.items[index];
    } else {
        entry = push_target();
        if (entry == 0)
            return;
        map_set(&g_target_index, key_of(surface->key), g_targets.count - 1u);
    }
    s_gpu_render_target_info info;
    gpu_render_target_get_info(g_gpu, g_back_buffer, &info);
    if (entry->snapshot_texture) {
        gpu_image_retire(g_gpu, entry->image); // this former RT's spare
        unlink_color_snapshot(entry);
    }
    entry->key = surface->key;
    entry->width = g_back_buffer_width; // guest pixels (the image is g_res_scale times as large: HALO3_RES_SCALE)
    entry->height = g_back_buffer_height;
    entry->d3d_format = surface->d3d_format;
    entry->host_format = info.color_format;
    entry->depth = 0;
    entry->back_buffer = 1;
    entry->scale = g_res_scale;
    entry->host_width = entry->width * g_res_scale;
    entry->host_height = entry->height * g_res_scale;
    s_gpu_image *image = gpu_render_target_color_image(g_gpu, g_back_buffer);
    if (entry->image != image) {
        entry->image = image;
        pass_format_changed();
    }
    entry->pending_clear_flags = 0;
}

unsigned int PORT_MS_ABI port_vk_render_resolution_scale(void)
{
    return __atomic_load_n(&g_res_scale, __ATOMIC_RELAXED);
}

uint32_t PORT_MS_ABI port_vk_config_revision(void)
{
    return (uint32_t)port_config_revision();
}

void PORT_MS_ABI port_vk_resize_back_buffer(unsigned int width, unsigned int height, unsigned int res_scale)
{
    if (record_remote()) {
        __atomic_add_fetch(&g_record_revision, 1u, __ATOMIC_RELEASE);
        return record_dispatch([=] { port_vk_resize_back_buffer(width, height, res_scale); });
    }
    if (res_scale == 0u) {
        res_scale = 1u;
        char scale_str[32];
        if (port_config_value(PORT_SETTING_RES_SCALE, scale_str, sizeof(scale_str)) > 0 &&
            scale_str[0] >= '1' && scale_str[0] <= '3' && scale_str[1] == '\0') {
            res_scale = (unsigned int)(scale_str[0] - '0');
        } else if (const char *scale_env = getenv("HALO3_RES_SCALE")) {
            if (scale_env[0] >= '1' && scale_env[0] <= '3' && scale_env[1] == '\0')
                res_scale = (unsigned int)(scale_env[0] - '0');
        }
    }
    if (res_scale < 1u || res_scale > 3u || !g_xenos)
        res_scale = 1u;
    if (g_gpu == 0) {
        g_back_buffer_width = width;
        g_back_buffer_height = height;
        __atomic_store_n(&g_res_scale, res_scale, __ATOMIC_RELAXED);
        return;
    }
    finish_frame();
    gpu_wait_idle(g_gpu);
    for (uint32_t i = 0; i < 4; i++)
        g_bound_color[i] = 0;
    g_bound_depth = 0;
    clear_pass_attachments();
    for (s_display_target &display : g_display_targets) {
        if (display.target != 0)
            gpu_render_target_destroy(g_gpu, display.target);
        display = s_display_target();
    }
    for (s_scratch_image &scratch : g_resolve_scratch) {
        if (scratch.image != 0)
            gpu_image_destroy(g_gpu, scratch.image);
        scratch = s_scratch_image();
    }
    if (g_gather_scratch != 0) {
        gpu_image_destroy(g_gpu, g_gather_scratch);
        g_gather_scratch = 0;
    }
    for (uint32_t i = 0; i < g_targets.count; i++) {
        if (g_targets.items[i].shell_image)
            gpu_image_destroy(g_gpu, g_targets.items[i].shell_image);
        if (!g_targets.items[i].back_buffer && g_targets.items[i].image != 0)
            gpu_image_destroy(g_gpu, g_targets.items[i].image);
    }
    g_targets.release();
    g_pending_7e3_texture = 0;
    map_free(&g_target_index);
    map_free(&g_edram_owner);
    memset(g_edram_clears, 0, sizeof(g_edram_clears));
    g_edram_serial = 0;
    for (uint32_t i = 0; i < g_textures.count; i++) {
        s_texture_entry *entry = &g_textures.items[i];
        entry->snapshot_target = 0;
        entry->pending_7e3_owner = 0;
        entry->pending_7e3_content = 0;
        if (!entry->resolved)
            continue;
        if (entry->image != 0)
            gpu_image_destroy(g_gpu, entry->image);
        for (int c = 0; c < 2; c++) {
            if (entry->converted[c] != 0)
                gpu_image_destroy(g_gpu, entry->converted[c]);
            entry->converted[c] = 0;
            entry->converted_serial[c] = 0;
        }
        if (entry->guest != 0)
            gpu_image_destroy(g_gpu, entry->guest);
        if (entry->packed != 0)
            gpu_image_destroy(g_gpu, entry->packed);
        if (entry->readback != 0)
            gpu_buffer_destroy(g_gpu, entry->readback);
        if (entry->readback_image != 0)
            gpu_image_destroy(g_gpu, entry->readback_image);
        entry->image = 0;
        entry->guest = 0;
        entry->guest_source = 0;
        entry->guest_serial = 0;
        entry->packed = 0;
        entry->packed_serial = 0;
        entry->packed_swizzle = 0;
        entry->readback = 0;
        entry->readback_size = 0;
        entry->readback_image = 0;
        entry->readback_image_w = 0;
        entry->readback_image_h = 0;
        entry->readback_serial = 0;
        if (entry->resolved) record_resolved(entry->key, entry->base, false);
        entry->resolved = 0;
        entry->resolved_scale = 0;
        entry->depth_edram = 0;
        entry->depth_float = 0;
        entry->linear_storage = 0;
        entry->resolve_serial = 0;
    }
    {
        s_memory_records_lock lock;
        map_free(&g_resolved_memory);
        g_resolved_headers.clear();
        g_resolved_bases.clear();
        resolve_state_changed();
    }
    if (g_back_buffer != 0)
        gpu_render_target_destroy(g_gpu, g_back_buffer);
    g_back_buffer_width = width;
    g_back_buffer_height = height;
    __atomic_store_n(&g_res_scale, res_scale, __ATOMIC_RELAXED);
    g_back_buffer = gpu_render_target_create(g_gpu, width * g_res_scale, height * g_res_scale,
                                             k_gpu_format_b8g8r8a8_unorm, 0);
    pass_format_changed();
}

void PORT_MS_ABI port_vk_set_render_target(uint32_t index, const s_port_vk_surface *surface)
{
    if (record_remote()) {
        if (!record_owner())
            return record_sync([&] { port_vk_set_render_target(index, surface); });
        const bool bound = surface != nullptr;
        const auto copy = bound ? *surface : s_port_vk_surface{};
        return record_post([=] { port_vk_set_render_target(index, bound ? &copy : nullptr); });
    }
    if (g_started <= 0 || index >= 4)
        return;
    s_target_entry *t = target_for(surface);
    if (g_bound_color[index] != t) {
        g_bound_color[index] = t;
        pass_format_changed();
    }
}

#ifdef HALO3_64BIT
void PORT_MS_ABI port_vk_shell_analyze_shader(const void *object, const uint32_t *code, uint32_t words, int host, int dynamic_loops)
{
    if (record_remote()) {
        return record_array(code, words, [=](const uint32_t *copy) {
            port_vk_shell_analyze_shader(object, copy, words, host, dynamic_loops);
        });
    }
    s_shader_entry *entry = shader_entry(object);
    s_shader_pack_xenos ps;
    if (g_vk && entry && shader_pack_get_xenos(g_pack, entry->xenos, &ps))
        entry->shell_coordinates = shell_pixel_coordinates(*entry, ps, code, words, host != 0, dynamic_loops != 0);
}

void PORT_MS_ABI port_vk_shell_trace(int single, unsigned draws, unsigned resolves, unsigned op, const void *pixel)
{
    if (record_remote()) {
        return record_dispatch([=] { port_vk_shell_trace(single, draws, resolves, op, pixel); });
    }
    static const bool enabled = getenv("HALO3_FRAME_STATS") || getenv("HALO3_GPU_PASS_TRACE");
    if (!enabled) return;
    static int previous = -1;
    if (previous == single) return;
    previous = single;
    const auto *ps = shader_entry(pixel);
    char line[192];
    snprintf(line, sizeof(line), "shell tiling executions=%d preflight_draws=%u resolve_groups=%u op=%u ps=%08x%08x\n",
             single ? 1 : 4, draws, resolves, op, ps ? ps->hash_hi : 0, ps ? ps->hash_lo : 0);
    port_write_error(line);
}

int PORT_MS_ABI port_vk_shell_available()
{
    if (record_remote()) {
        return record_read([&] { return port_vk_shell_available(); });
    }
    return g_vk && g_xenos && !g_active_query && !g_export.base && g_present_resolve < 0;
}

// A shadow projection uses normalized point samples at (abs(VPOS) + c108 + 0.5) / frame size
// for depth and normals. Its filtered shadow-map samples have independent caster coordinates.
int PORT_MS_ABI port_vk_shell_draw(const void *vertex, const void *pixel, uint32_t scene_samples,
                                   uint32_t forbidden_samples, uint32_t point_samples, int origin,
                                   int local_view, const float *screen_texel, uint32_t width, uint32_t height,
                                   const uint32_t *loops, int first_band)
{
    if (record_remote()) {
        return record_read([&] { return port_vk_shell_draw(vertex, pixel, scene_samples, forbidden_samples, point_samples, origin, local_view, screen_texel, width, height, loops, first_band); });
    }
    s_shader_entry *v = shader_entry(vertex), *p = shader_entry(pixel);
    s_shader_pack_xenos vs, ps;
    if (!v || !shader_pack_get_xenos(g_pack, v->xenos, &vs) || vs.model != k_shader_pack_xenos_vertex ||
        (vs.flags & k_shader_pack_xenos_memexport))
        return 0;
    for (uint32_t i = 0; i < vs.texture_count; ++i)
        if ((scene_samples | forbidden_samples) & (1u << vs.textures[i].fetch))
            return 0;
    if (!pixel)
        return !first_band;
    if (!p || !shader_pack_get_xenos(g_pack, p->xenos, &ps) || ps.model != k_shader_pack_xenos_fragment ||
        (ps.flags & k_shader_pack_xenos_memexport))
        return 0;
    const uint64_t hash = ((uint64_t)p->hash_hi << 32) | p->hash_lo;
    const bool projected = hash == 0x465b6b88a89982b0ull || hash == 0x5807637a52253da9ull ||
                           hash == 0x976088e5dff7478dull || hash == 0xa44ffcf65780e30bull ||
                           hash == 0xacd294913419838cull;
    uint32_t local_fetches = p->shell_local_fetches;
    if (first_band) {
        const uint64_t vertex_hash = ((uint64_t)v->hash_hi << 32) | v->hash_lo;
        if (g_res_scale != 1 || vertex_hash != 0x2167141e54bf4353ull || hash != 0x56518515a8d18d79ull)
            return 0;
        // pixel_copy passes screen POSITION0 through with w=1. All covered rows have the band-zero VPOS.
        local_fetches = 0;
    } else if (projected && !local_view) {
        if (!origin || g_res_scale != 1 || screen_texel[0] != 1.0f / width ||
            screen_texel[1] != 1.0f / height || screen_texel[2] != 0.5f / width ||
            screen_texel[3] != 0.5f / height)
            return 0;
        local_fetches = 5; // tf0 = scene depth, tf2 = scene normal
    } else if (hash == 0x4ce428fe998a454dull) {
        // shield_impact: c108..110 are material colors, and its noise UVs are interpolated. A common
        // material write must have replaced the four different c108 values before accepting this draw.
        if (origin)
            return 0;
        local_fetches = 0;
    } else if (!p->shell_coordinates || (p->shell_uses_origin && (!origin || local_view || g_res_scale != 1))) {
        return 0;
    }
    for (uint32_t used = p->shell_loop_mask; used; used &= used - 1) {
        const uint32_t loop = loops[__builtin_ctz(used)];
        const int n = loop & 255, start = (loop >> 8) & 255, step = (int8_t)(loop >> 16);
        const int last = start + (n ? n - 1 : 0) * step;
        if (n && (start > 63 || last < 0 || last > 63))
            return 0;
    }
    for (uint32_t i = 0; i < ps.texture_count; ++i) {
        const auto &use = ps.textures[i];
        const uint32_t bit = 1u << use.fetch;
        if (forbidden_samples & bit)
            return 0;
        if ((scene_samples & bit) && (!(local_fetches & bit) || local_view || use.dim != 0 ||
            !(use.sampler == XE_SAMPLER_POINT || use.sampler == XE_SAMPLER_POINT_MIP_POINT ||
              (use.sampler == 0 && (point_samples & bit)))))
            return 0;
    }
    return 1;
}

// Prepare every allocation before switching any attachment. An allocation failure can still replay the strips.
int PORT_MS_ABI port_vk_shell_begin(const s_port_vk_surface *surfaces, uint32_t count, uint32_t frame_height)
{
    if (record_remote()) {
        return record_result([&] { return port_vk_shell_begin(surfaces, count, frame_height); });
    }
    // A redefined header can retire its old image and materialize pending resolves in target_for.
    // Leave all of that to normal replay if any header changed since its last bind.
    for (uint32_t i = 0; i < count; ++i) {
        const auto &surface = surfaces[i];
        if (!surface.depth) {
            for (uint32_t j = 0; j < i; ++j) {
                const auto &alias = surfaces[j];
                if (alias.depth || alias.edram_base != surface.edram_base || alias.d3d_format == surface.d3d_format)
                    continue;
                uint32_t bw, bh, bytes, alias_bytes;
                gpu_format_block_info(target_host_format(surface.d3d_format, 0), &bw, &bh, &bytes);
                gpu_format_block_info(target_host_format(alias.d3d_format, 0), &bw, &bh, &alias_bytes);
                // These views must be able to carry every band's contents in either direction.
                if (!bytes || bytes != alias_bytes || alias.width != surface.width) return 0;
            }
        }
        uint32_t index;
        if (!map_find(&g_target_index, key_of(surface.key), &index)) continue;
        const auto &t = g_targets.items[index];
        if (t.back_buffer || t.strip_height || t.width != surface.width || t.height != surface.height ||
            t.d3d_format != surface.d3d_format || t.depth != surface.depth || t.msaa != surface.msaa_samples ||
            t.edram_base != surface.edram_base)
            return 0;
    }
    for (uint32_t i = 0; i < count; ++i) {
        s_target_entry *t = target_for(&surfaces[i]);
        if (!t || !t->image || t->back_buffer || t->strip_height)
            return 0;
        if (!t->shell_image || t->shell_height != frame_height) {
            s_gpu_image_desc desc = {};
            gpu_image_get_desc(g_gpu, t->image, &desc);
            desc.height = frame_height * t->scale;
            s_gpu_image *image = gpu_image_create(g_gpu, &desc);
            if (!image)
                return 0;
            if (t->shell_image)
                gpu_image_retire(g_gpu, t->shell_image);
            t->shell_image = image;
            t->shell_height = frame_height;
        }
    }
    if (!begin_frame())
        return 0;
    end_pass();
    // Import band zero through the ordinary strip views, including any prior clear, alias conversion or
    // sample-0 gather. Finish all imports before changing the dimensions of any of their EDRAM owners.
    for (uint32_t i = 0; i < count; ++i) {
        s_target_entry *t = target_for(&surfaces[i]);
        materialize_pending_7e3_for_owner(t);
        edram_sync(t);
        materialize_target_contents(t);
        if (t->depth)
            gpu_cmd_copy_depth(g_gpu, t->image, 0, 0, t->host_width, t->host_height, t->shell_image, 0, 0);
        else
            gpu_cmd_copy_image(g_gpu, t->image, 0, 0, t->host_width, t->host_height, t->shell_image,
                               0, 0, 0, 0, t->host_width, t->host_height);
    }
    for (uint32_t i = 0; i < count; ++i) {
        s_target_entry *t = target_for(&surfaces[i]);
        s_gpu_image *strip = t->image;
        t->image = t->shell_image;
        t->shell_image = strip;
        t->strip_height = t->height;
        t->height = frame_height;
        t->host_height = frame_height * t->scale;
    }
    clear_pass_attachments();
    pass_format_changed();
    return 1;
}

void PORT_MS_ABI port_vk_shell_apply_begin_clear(const s_port_vk_surface *surfaces, uint32_t count)
{
    if (record_remote()) {
        return record_array(surfaces, count, [=](const s_port_vk_surface *copy) {
            port_vk_shell_apply_begin_clear(copy, count);
        });
    }
    // The begin bindings can still be the guest's ordinary (configuration 0) headers. Apply each
    // pending base clear to its first promoted view, before a color alias can take depth ownership.
    bool color = false, depth = false;
    for (uint32_t i = 0; i < count; ++i) {
        s_target_entry *t = target_for(&surfaces[i]);
        bool &done = t->depth ? depth : color;
        const auto *bound = t->depth ? g_bound_depth : g_bound_color[0];
        if (!done && bound && bound->edram_base == t->edram_base) {
            edram_apply_clear(t);
            done = true;
        }
    }
}

void PORT_MS_ABI port_vk_shell_end(const s_port_vk_surface *surfaces, uint32_t count, uint32_t last_y, const void *setup_color)
{
    if (record_remote()) {
        return record_array(surfaces, count, [=](const s_port_vk_surface *copy) {
            port_vk_shell_end(copy, count, last_y, setup_color);
        });
    }
    end_pass();
    for (uint32_t i = 0; i < count; ++i) {
        s_target_entry *t = target_for(&surfaces[i]);
        materialize_pending_7e3_for_owner(t);
        materialize_target_contents(t);
        const uint32_t h = t->strip_height * t->scale;
        // Legacy leaves the last band's physical EDRAM behind. Preserve that for later alias binds and the
        // next bracket (including a fallback), independently of the four assembled resolve textures.
        const uint32_t y = t->key == setup_color ? 0 : last_y * t->scale;
        if (t->depth)
            gpu_cmd_copy_depth(g_gpu, t->image, 0, y, t->host_width, h, t->shell_image, 0, 0);
        else
            gpu_cmd_copy_image(g_gpu, t->image, 0, y, t->host_width, h, t->shell_image,
                               0, 0, 0, 0, t->host_width, h);
        s_gpu_image *full = t->image;
        t->image = t->shell_image;
        t->shell_image = full;
        t->height = t->strip_height;
        t->strip_height = 0;
        t->host_height = h;
    }
    clear_pass_attachments();
    pass_format_changed();
}
#endif

void PORT_MS_ABI port_vk_set_depth_stencil(const s_port_vk_surface *surface)
{
    if (record_remote()) {
        if (!record_owner())
            return record_sync([&] { port_vk_set_depth_stencil(surface); });
        const bool bound = surface != nullptr;
        const auto copy = bound ? *surface : s_port_vk_surface{};
        return record_post([=] { port_vk_set_depth_stencil(bound ? &copy : nullptr); });
    }
    if (g_started <= 0)
        return;
    s_target_entry *t = surface != 0 ? target_for(surface) : 0;
    if (g_bound_depth != t) {
        // The current pass keeps its actual depth attachment until ensure_pass decides whether to replace it.
        g_bound_depth = t;
        pass_format_changed();
    }
}

// A range for the render thread's next xe_apply_placements (any thread).
void queue_placed(const void *base, uint32_t size, uint32_t streamed)
{
    placed_lock();
    if (g_placed_count < k_placed_capacity) {
        g_placed[g_placed_count].base = base;
        g_placed[g_placed_count].size = size;
        g_placed[g_placed_count].streamed = streamed;
        __atomic_store_n(&g_placed_count, g_placed_count + 1u, __ATOMIC_RELAXED);
    } else {
        __atomic_store_n(&g_placed_overflow, 1u, __ATOMIC_RELAXED);
    }
    placed_unlock();
}

void PORT_MS_ABI port_vk_memory_placed(const void *base, uint32_t size)
{
    if (record_remote()) {
        resource_touch_changed();
        record_drop_export_hints();
        return record_dispatch([=] {
            port_vk_memory_placed(base, size);
            record_finish_export_invalidation();
        });
    }
    if (g_started <= 0 || !g_xenos || base == 0 || size == 0)
        return;
    queue_placed(base, size, 0);
    resource_touch_changed(); // the render thread drops the exports it replaces at its next draw
}

int PORT_MS_ABI port_vk_buffer_lock(const void *base, uint32_t size, int read_only)
{
    if (record_remote()) {
        s_record_client client;
        if (!client.admitted) return 0;
        if (!record_gpu_write({base, size}, nullptr)) {
            if (!read_only) record_wait_reads({base, size}, "buf-lock");
            return 0;
        }
        record_drop_export_hints();
        const int changed = record_result([&] { return port_vk_buffer_lock(base, size, read_only); });
        record_finish_export_invalidation();
        return changed;
    }
    if (g_started <= 0 || !g_xenos || base == 0 || size == 0)
        return 0;
    // The recording worker runs threaded readbacks. In synchronous mode, other
    // SDK threads still take the ordinary CPU lock without entering renderer state.
    if ((uint32_t)GetCurrentThreadId() != __atomic_load_n(&g_frame_thread, __ATOMIC_RELAXED))
        return 0;
    return xe_buffer_lock(base, size, read_only != 0) ? 1 : 0;
}

int PORT_MS_ABI port_vk_buffer_was_mapped(const void *key, const void *base)
{
    if (!record_remote()) return 0;
    s_record_resource_guard guard;
    uint32_t mapped;
    return map_find(&g_record_written_buffers, key_of(key), &mapped) && mapped == key_of(base);
}

void PORT_MS_ABI port_vk_buffer_written(const void *key, const void *base)
{
    if (record_remote()) {
        s_record_resource_guard guard;
        map_set(&g_record_written_buffers, key_of(key), key_of(base));
        resource_touch_changed();
    }
    port_vk_resource_touched(key);
}

void PORT_MS_ABI port_vk_resource_touched(const void *key)
{
    if (record_remote()) {
        return record_dispatch([=] { port_vk_resource_touched(key); });
    }
    if (g_started <= 0 || key == 0)
        return;
    uint32_t *g = map_slot(&g_touches, key_of(key), true);
    if (g != 0)
        (*g)++;
    resource_touch_changed();
    // a texture the game writes is no longer what a Resolve left in it
    s_texture_entry *entry = texture_entry(key, false);
    if (entry != 0 && entry->base != 0) {
        // the memory is the game's again, for every header over it
        s_memory_records_lock lock;
        uint32_t *resolved = map_slot(&g_resolved_memory, key_of(entry->base), false);
        if (resolved != 0 && *resolved != 0) {
            *resolved = 0;
            resolve_state_changed(); // slot_resolved
        }
    }
    if (entry != 0 && entry->resolved) {
        record_resolved(entry->key, entry->base, false);
        entry->resolved = 0;
        resolve_state_changed();
        release_texture_image(entry);
    }
}

static void texture_written(const void *texture_header, const s_port_d3d_texture_source &source)
{
    if (g_started <= 0 || !texture_header) return;
    port_vk_resource_touched(texture_header);
    s_memory_records_lock lock;
    if (source.base != 0)
        map_set(&g_host_order_memory, key_of(source.base), 1);
    if (source.mip != 0)
        map_set(&g_host_order_memory, key_of(source.mip), 1);
}

void PORT_MS_ABI port_vk_texture_written(const void *texture_header)
{
    s_port_d3d_texture_source source = {};
    if (texture_header) port_d3d_texture_get_source(texture_header, &source);
    if (record_remote())
        return record_dispatch([=] { texture_written(texture_header, source); });
    texture_written(texture_header, source);
}

void PORT_MS_ABI port_vk_texels_written(const void *base, uint32_t size)
{
    if (record_remote()) {
        return record_dispatch([=] { port_vk_texels_written(base, size); });
    }
    if (g_started <= 0 || base == 0 || size == 0)
        return;
    const uintptr_t begin = (uintptr_t)base, end = begin + size;
    {
        s_memory_records_lock lock;
        map_set(&g_host_order_memory, key_of(base), 1);
    }
    for (uint32_t i = 0; i < g_textures.count; i++) {
        const s_texture_entry &entry = g_textures.items[i];
        const uintptr_t level0 = (uintptr_t)entry.base, mips = (uintptr_t)entry.mip;
        if (entry.key != 0 && ((level0 >= begin && level0 < end) || (mips >= begin && mips < end))) {
            s_port_d3d_texture_source source = {};
            source.base = entry.base;
            source.mip = entry.mip;
            texture_written(entry.key, source);
        }
    }
}

// Both maps record addresses (a texture's first texels): those inside the rewritten range stop holding. A value of 0
// is "no record" for both (an open-addressing key is never removed). A few dozen records, a few pages a second. The
// device copies of memory exports over the range stop holding it too (the render thread drops them at its next draw,
// xe_apply_placements): the page is new data, wherever its buffers' headers are placed next.
void PORT_MS_ABI port_vk_memory_streamed(const void *base, uint32_t size)
{
    if (record_remote()) {
        resource_touch_changed();
        record_drop_export_hints();
        return record_dispatch([=] {
            port_vk_memory_streamed(base, size);
            record_finish_export_invalidation();
        });
    }
    if (g_started <= 0 || base == 0 || size == 0)
        return;
    if (g_xenos)
        queue_placed(base, size, 1);
    resource_touch_changed();
    const uint32_t begin = key_of(base);
    const uint32_t last = begin + (size - 1u);
    s_memory_records_lock lock;
    s_map *maps[2] = { &g_host_order_memory, &g_resolved_memory };
    for (s_map *map : maps)
        for (uint32_t i = 0; i < map->capacity; i++)
            if (map->keys[i] >= begin && map->keys[i] <= last && map->values[i] != 0) {
                map->values[i] = 0;
                if (map == &g_resolved_memory)
                    resolve_state_changed(); // slot_resolved
            }
}

// ---- device-loss diagnostics (HALO3_VULKAN_CHECKPOINTS, gpu_cmd_checkpoint) ----

// The recent draws, by serial: a checkpoint before each draw carries its serial, and a lost device's last
// checkpoints are described from here.
struct s_draw_record {
    uint32_t serial;
    uint32_t kind;          // 0 a draw, 1 a clear (flags in count), 2 a resolve (flags in count, source in start), 3 a present
    const void *vertex_shader, *pixel_shader;
    uint32_t vs_index, ps_index;
    uint32_t primitive_type, start, count, indexed, stream_mask;
};
const uint32_t k_draw_record_count = 1u << 14;
s_draw_record *g_draw_records;
uint32_t g_draw_serial;

void describe_record(uint32_t serial, const char *lead)
{
    const s_draw_record *r = g_draw_records != 0 ? &g_draw_records[serial & (k_draw_record_count - 1u)] : 0;
    if (r == 0 || serial == 0 || r->serial != serial) {
        char text[64];
        snprintf(text, sizeof(text), "    %scommand %u (not recorded)\n", lead, serial);
        log_text(text);
        return;
    }
    if (r->kind != 0) {
        static const char *const k_kinds[] = { "draw", "clear", "resolve", "present" };
        char text[96];
        snprintf(text, sizeof(text), "    %scommand %u: %s, flags 0x%x, source %u\n", lead, serial,
                 k_kinds[r->kind < 4u ? r->kind : 0u], r->count, r->start);
        log_text(text);
        return;
    }
    s_shader_pack_shader_info vs = {}, ps = {};
    shader_pack_get_shader(g_pack, r->vs_index, &vs);
    shader_pack_get_shader(g_pack, r->ps_index, &ps);
    char text[512];
    snprintf(text, sizeof(text),
             "    %sdraw %u: primitive %u, %s %u from %u, streams 0x%x; vs %p #%u %s (%u); ps %p #%u %s\n", lead, serial,
             r->primitive_type, r->indexed ? "indices" : "vertices", r->count, r->start, r->stream_mask, r->vertex_shader,
             r->vs_index, vs.name != 0 ? vs.name : "?", (uint32_t)vs.vertex_type, r->pixel_shader, r->ps_index,
             ps.name != 0 ? ps.name : "?");
    log_text(text);
}

// A checkpoint the queue reached, and the commands recorded after it (the first of which it did not finish).
void describe_checkpoint(void *context, const void *marker)
{
    (void)context;
    const uint32_t serial = (uint32_t)(size_t)marker;
    describe_record(serial, "");
    for (uint32_t next = serial + 1u; next <= serial + 3u && next <= g_draw_serial; next++)
        describe_record(next, "then ");
}

s_draw_record *checkpoint_record(uint32_t kind)
{
    if (!g_gpu->caps.checkpoints)
        return 0;
    if (g_draw_records == 0) {
        g_draw_records = (s_draw_record *)calloc(k_draw_record_count, sizeof(s_draw_record));
        if (g_draw_records == 0)
            return 0;
        gpu_set_checkpoint_describe(g_gpu, describe_checkpoint, 0);
    }
    if (++g_draw_serial == 0)
        g_draw_serial = 1;
    s_draw_record *record = &g_draw_records[g_draw_serial & (k_draw_record_count - 1u)];
    memset(record, 0, sizeof(*record));
    record->serial = g_draw_serial;
    record->kind = kind;
    gpu_cmd_checkpoint(g_gpu, (const void *)(size_t)g_draw_serial);
    return record;
}

void checkpoint_draw(s_draw_geometry draw, const s_shader_entry *vs, const s_shader_entry *ps,
                     uint32_t primitive_type, uint32_t start, uint32_t count, uint32_t indexed)
{
    s_draw_record *record = checkpoint_record(0);
    if (record == 0)
        return;
    s_draw_record &r = *record;
    r.vertex_shader = draw->vertex_shader;
    r.pixel_shader = draw->pixel_shader;
    r.vs_index = vs->shader_index;
    r.ps_index = ps != 0 ? ps->shader_index : SHADER_PACK_NOT_FOUND;
    r.primitive_type = primitive_type;
    r.start = start;
    r.count = count;
    r.indexed = indexed;
    r.stream_mask = draw.stream_mask();
}

// ---- rectangle lists ----

// D3DPT_RECTLIST: every three vertices are three corners of a screen-aligned rectangle, and the Xenos makes the
// fourth by mirroring the right-angle corner across the diagonal, the longest of the three edges (xenia's
// kRectangleList geometry shader, src/xenia/gpu/vulkan/vulkan_pipeline_cache.cc). The renderer has no geometry
// stage, so the fourth vertex is made from the vertex data instead: every attribute extrapolated as
// a + b - corner, and the rectangle drawn as a quad (corner, a, fourth, b). That is what the hardware draws when the
// vertex shader is affine in its inputs with a constant w, which is what rectangles are drawn with (screen quads:
// positions and texture coordinates passed through or transformed by a matrix). The diagonal is found from the
// position element (POSITION 0, before the shader transforms it).

uint8_t *g_rect_scratch;
uint32_t g_rect_scratch_size;

uint8_t *rect_scratch(uint32_t size)
{
    if (size > g_rect_scratch_size) {
        uint8_t *grown = (uint8_t *)realloc(g_rect_scratch, size);
        if (grown == 0)
            return 0;
        g_rect_scratch = grown;
        g_rect_scratch_size = size;
    }
    return g_rect_scratch;
}

int32_t clamp_i32(int64_t v, int64_t lo, int64_t hi) { return (int32_t)(v < lo ? lo : v > hi ? hi : v); }

// One element of the fourth vertex, a + b - c, in the converted (host order) data. Components of the packed
// formats are not extrapolated; equal elements (a color or texture index shared by the corners) are copied as is.
void rect_extrapolate_element(uint32_t type, const uint8_t *a, const uint8_t *b, const uint8_t *c, uint8_t *out,
                              uint32_t size)
{
    if (memcmp(a, b, size) == 0 && memcmp(a, c, size) == 0) {
        memcpy(out, a, size);
        return;
    }
    const uint32_t format = type & 0x3fu;
    const bool is_signed = ((type >> 8) & 1u) != 0;
    switch (format) {
    case k_xenos_vertex_format_32_float:
    case k_xenos_vertex_format_32_32_float:
    case k_xenos_vertex_format_32_32_32_float:
    case k_xenos_vertex_format_32_32_32_32_float:
        for (uint32_t i = 0; i + 4u <= size; i += 4u) {
            float fa, fb, fc;
            memcpy(&fa, a + i, 4);
            memcpy(&fb, b + i, 4);
            memcpy(&fc, c + i, 4);
            const float r = fa + fb - fc;
            memcpy(out + i, &r, 4);
        }
        return;
    case k_xenos_vertex_format_8_8_8_8:
        for (uint32_t i = 0; i < size; i++) {
            const int64_t r = is_signed ? (int64_t)(int8_t)a[i] + (int8_t)b[i] - (int8_t)c[i]
                                        : (int64_t)a[i] + b[i] - c[i];
            out[i] = (uint8_t)clamp_i32(r, is_signed ? -128 : 0, is_signed ? 127 : 255);
        }
        return;
    case k_xenos_vertex_format_16_16:
    case k_xenos_vertex_format_16_16_16_16:
        for (uint32_t i = 0; i + 2u <= size; i += 2u) {
            uint16_t ua, ub, uc;
            memcpy(&ua, a + i, 2);
            memcpy(&ub, b + i, 2);
            memcpy(&uc, c + i, 2);
            const int64_t r = is_signed ? (int64_t)(int16_t)ua + (int16_t)ub - (int16_t)uc : (int64_t)ua + ub - uc;
            const uint16_t v = (uint16_t)clamp_i32(r, is_signed ? -32768 : 0, is_signed ? 32767 : 65535);
            memcpy(out + i, &v, 2);
        }
        return;
    case k_xenos_vertex_format_32:
    case k_xenos_vertex_format_32_32:
    case k_xenos_vertex_format_32_32_32_32:
        for (uint32_t i = 0; i + 4u <= size; i += 4u) {
            uint32_t ua, ub, uc;
            memcpy(&ua, a + i, 4);
            memcpy(&ub, b + i, 4);
            memcpy(&uc, c + i, 4);
            const uint32_t v = ua + ub - uc;
            memcpy(out + i, &v, 4);
        }
        return;
    default:
        report(type ^ 0x72656374u, "a rectangle list with a packed vertex element that differs per corner "
                                   "(the fourth corner takes the first's)");
        memcpy(out, a, size);
        return;
    }
}

uint32_t vertex_element_size(uint32_t type)
{
    switch (type & 0x3fu) {
    case k_xenos_vertex_format_8_8_8_8:
    case k_xenos_vertex_format_2_10_10_10:
    case k_xenos_vertex_format_10_11_11:
    case k_xenos_vertex_format_11_11_10:
    case k_xenos_vertex_format_16_16:
    case k_xenos_vertex_format_16_16_float:
    case k_xenos_vertex_format_32:
    case k_xenos_vertex_format_32_float: return 4u;
    case k_xenos_vertex_format_16_16_16_16:
    case k_xenos_vertex_format_16_16_16_16_float:
    case k_xenos_vertex_format_32_32:
    case k_xenos_vertex_format_32_32_float: return 8u;
    case k_xenos_vertex_format_32_32_32_float: return 12u;
    case k_xenos_vertex_format_32_32_32_32:
    case k_xenos_vertex_format_32_32_32_32_float: return 16u;
    default: return 0u;
    }
}

// The draws of one decorator group (port_vk_draw, after its pipeline, streams and sets are bound): `count` vertices =
// index_count (VS c242.x) per instance. Each instance record (stream 1, 16 bytes: USHORT4 position, BYTE4N quaternion,
// D3DCOLOR) names its mesh variant in position.w's high byte; variant t is stream 0's vertices t * index_count ..
// (t + 1) * index_count - 1 (unindexed). One instanced vkCmdDraw per run of equal variant: firstVertex selects the
// variant, firstInstance the run's records.
static void draw_decorator_instances(s_draw_geometry draw, uint32_t start, uint32_t count)
{
    const float per_instance_float = g_shadows.vertex_float != 0 ? g_shadows.vertex_float[242 * 4] : 0.0f;
    const uint32_t per_instance = per_instance_float >= 1.0f ? (uint32_t)per_instance_float : 0u;
    const s_port_vk_stream &mesh = draw.stream(0);
    const s_port_vk_stream &records = draw.stream(1);
    if (start != 0 || per_instance == 0 || count % per_instance != 0 || records.data == 0 || records.stride < 8 ||
        records.size <= records.offset || mesh.stride == 0 || mesh.size <= mesh.offset) {
        report(0x64656372u, "a decorator draw the device cannot split into instances (skipped)");
        return;
    }
    const uint32_t instances = count / per_instance;
    const uint32_t record_count = (records.size - records.offset) / records.stride;
    const uint32_t mesh_vertices = (mesh.size - mesh.offset) / mesh.stride;
    if (instances > record_count) {
        report(0x64656373u, "a decorator draw with more instances than its instance buffer holds (skipped)");
        return;
    }
    const uint8_t *base = vertex_bytes(records, 1, records.offset);
    if (tracing()) {
        char text[200];
        int n = snprintf(text, sizeof(text), "trace decorator draw: %u instances of %u vertices, %s, record 0:", instances,
                         per_instance, records.big_endian ? "big-endian" : "host order");
        for (uint32_t b = 0; b < 16 && b < records.stride && n > 0 && n < (int)sizeof(text) - 4; b++)
            n += snprintf(text + n, sizeof(text) - n, " %02x", base[b]);
        port_write_error(text);
        port_write_error("\n");
    }
    uint32_t run_start = 0, run_type = ~0u;
    for (uint32_t i = 0; i <= instances; i++) {
        uint32_t type = ~0u;
        if (i < instances) {
            const uint8_t *w = base + (size_t)i * records.stride + 6; // position.w (USHORT4 component 3)
            type = records.big_endian ? w[0] : w[1];                   // its high byte
        }
        if (type == run_type)
            continue;
        if (i > run_start) {
            if ((run_type + 1u) * per_instance <= mesh_vertices)
                record_draw(per_instance, i - run_start, run_type * per_instance, run_start);
            else
                report(0x64656374u, "a decorator instance naming a mesh variant past its vertex buffer (not drawn)");
        }
        run_start = i;
        run_type = type;
    }
}

// Expands `rects` rectangles from vertex `first` into quads of four vertices for every stream the declaration reads,
// each into the frame ring. False if a stream cannot be read or the declaration advances a stream per instance.
bool expand_rectangles(s_draw_geometry draw, const s_xenos_vertex_declaration *declaration, uint32_t first,
                       uint32_t rects, s_data_binding *out_streams)
{
    if (declaration->instance_stream_mask != 0) {
        report(0x72656331u, "an instanced rectangle list (skipped)");
        return false;
    }
    // the converted corners of every stream, side by side in the scratch buffer
    uint32_t scratch_offset[PORT_VK_MAX_STREAMS] = {};
    uint32_t total = 0;
    for (uint32_t s = 0; s < PORT_VK_MAX_STREAMS; s++) {
        if ((declaration->stream_mask & (1u << s)) == 0)
            continue;
        const s_port_vk_stream &stream = draw.stream(s);
        if ((draw.stream_mask() & (1u << s)) == 0 || stream.data == 0 || stream.stride == 0 ||
            stream.size < stream.offset || (stream.size - stream.offset) / stream.stride < first + 3u * rects) {
            report(0x7374726du + s, "a draw whose vertex stream is not set or not readable (skipped)");
            return false;
        }
        scratch_offset[s] = total;
        total += 3u * rects * stream.stride;
    }
    uint8_t *scratch = rect_scratch(total ? total : 1u);
    if (scratch == 0)
        return false;
    for (uint32_t s = 0; s < PORT_VK_MAX_STREAMS; s++) {
        if ((declaration->stream_mask & (1u << s)) == 0)
            continue;
        const s_port_vk_stream &stream = draw.stream(s);
        const uint8_t *source = vertex_bytes(stream, s, stream.offset + size_t(first) * stream.stride);
        xenos_vertex_convert_stream(declaration, s, source, scratch + scratch_offset[s], 3u * rects, stream.stride,
                                    stream.big_endian);
        g_converted_bytes[k_converted_rectangles] += (uint64_t)3u * rects * stream.stride;
    }
    // the position element: its stream, offset and whether it can be read as floats
    const s_xenos_d3d_vertex_element *position = 0;
    for (uint32_t e = 0; e < declaration->element_count; e++) {
        const s_xenos_d3d_vertex_element &el = declaration->elements[e];
        if (el.usage == k_xenos_declusage_position && el.usage_index == 0) {
            position = &el;
            break;
        }
    }
    const uint32_t position_format = position != 0 ? position->type & 0x3fu : 0u;
    const bool float_position = position_format == k_xenos_vertex_format_32_32_float ||
                                position_format == k_xenos_vertex_format_32_32_32_float ||
                                position_format == k_xenos_vertex_format_32_32_32_32_float;
    // the corner of each rectangle (0, 1 or 2)
    uint8_t *corners = (uint8_t *)malloc(rects ? rects : 1u);
    if (corners == 0)
        return false;
    for (uint32_t r = 0; r < rects; r++) {
        corners[r] = 0;
        if (!float_position)
            continue;
        const s_port_vk_stream &stream = draw.stream(position->stream);
        float xy[3][2];
        for (uint32_t v = 0; v < 3; v++)
            memcpy(xy[v], scratch + scratch_offset[position->stream] + (3u * r + v) * stream.stride + position->offset, 8);
        float edge[3]; // [0] 12, [1] 20, [2] 01: the edge opposite vertex i
        for (uint32_t i = 0; i < 3; i++) {
            const float *p = xy[(1u + i) % 3u], *q = xy[(2u + i) % 3u];
            edge[i] = (q[0] - p[0]) * (q[0] - p[0]) + (q[1] - p[1]) * (q[1] - p[1]);
        }
        corners[r] = edge[0] > edge[1] && edge[0] > edge[2] ? 0u : edge[1] > edge[2] ? 1u : 2u;
    }
    bool ok = true;
    for (uint32_t s = 0; s < PORT_VK_MAX_STREAMS && ok; s++) {
        if ((declaration->stream_mask & (1u << s)) == 0)
            continue;
        const uint32_t stride = draw.stream(s).stride;
        uint8_t *dst = (uint8_t *)ring_alloc(4u * rects * stride, 16, &out_streams[s].offset);
        if (dst == 0) {
            ok = false;
            break;
        }
        out_streams[s].buffer = ring_buffer();
        for (uint32_t r = 0; r < rects; r++) {
            const uint8_t *rect = scratch + scratch_offset[s] + 3u * r * stride;
            const uint32_t k = corners[r];
            const uint8_t *c = rect + k * stride;
            const uint8_t *a = rect + ((k + 1u) % 3u) * stride;
            const uint8_t *b = rect + ((k + 2u) % 3u) * stride;
            uint8_t *quad = dst + 4u * r * stride;
            memcpy(quad, c, stride);
            memcpy(quad + stride, a, stride);
            memcpy(quad + 2u * stride, a, stride); // bytes no element covers, then the elements over them
            memcpy(quad + 3u * stride, b, stride);
            for (uint32_t e = 0; e < declaration->element_count; e++) {
                const s_xenos_d3d_vertex_element &el = declaration->elements[e];
                const uint32_t size = vertex_element_size(el.type);
                if (el.stream != s || size == 0 || el.offset + size > stride)
                    continue;
                rect_extrapolate_element(el.type, a + el.offset, b + el.offset, c + el.offset,
                                         quad + 2u * stride + el.offset, size);
            }
        }
    }
    free(corners);
    return ok;
}

// The first draw per (vertex shader, stream) whose stream cannot be read: which stream, why, and the shader's
// microcode hash and pack name, so the caller can be found (the old report said only that some draw had one).
static void report_stream(s_draw_geometry draw, const s_shader_entry *vs, uint32_t s)
{
    if (!first_report(0x7374726du ^ (key_of(draw->vertex_shader) * 31u + s)))
        return;
    const s_port_vk_stream &stream = draw.stream(s);
    const char *why = (draw.stream_mask() & (1u << s)) == 0 ? "not set"
                      : stream.data == 0                    ? "no data"
                      : stream.stride == 0                  ? "stride 0"
                                                            : "offset past the data";
    s_shader_pack_shader_info info;
    const char *name = shader_pack_get_shader(g_pack, vs->shader_index, &info) ? info.name : "?";
    char text[160];
    snprintf(text, sizeof(text), "stream %u %s (data %p, %u bytes, offset %u, stride %u), vertex shader %08x%08x %s", s,
             why, stream.data, stream.size, stream.offset, stream.stride, vs->hash_hi, vs->hash_lo, name);
    device_log_text("a draw whose vertex stream is not set or not readable (skipped): ");
    port_write_error(text);
    port_write_error("\n");
}

void PORT_MS_ABI port_vk_draw(const s_port_vk_draw *packet, uint64_t dirty_mask, uint32_t primitive_type,
                              uint32_t start, uint32_t count, int base_vertex, uint32_t indexed,
                              const s_port_vk_geometry *geometry)
{
    if (record_remote() && packet) {
        record_draw(packet, dirty_mask, { primitive_type, start, count, base_vertex, indexed, 0, 0, 0, false }, geometry);
        return;
    }
    if (g_started <= 0 || packet == 0 || count == 0)
        return;
    const s_draw_geometry draw = { packet, geometry };
    g_frame_trace.draws++;
    g_half_pixel_offset = draw->pixel_center_offset != 0.0f;
    ring_ensure_headroom(); // before the draw records anything
    s_shader_entry *vs = shader_entry(draw->vertex_shader);
    if (tracing()) {
        char text[640];
        snprintf(text, sizeof(text),
                 "trace draw %u: vs %s ps %s pt %u start %u count %u%s vp %g,%g %gx%g z %g..%g sc %d,%d,%d,%d(%u) "
                 "tile %d,%d z %u/%u/%u blend %08x cw %x stencil %u",
                 g_draw_serial, pack_name(vs), draw->pixel_shader != 0 ? pack_name(shader_entry(draw->pixel_shader)) : "NULL",
                 primitive_type, start, count, indexed ? " indexed" : "", draw->viewport[0], draw->viewport[1],
                 draw->viewport[2], draw->viewport[3], draw->viewport[4], draw->viewport[5], draw->scissor[0],
                 draw->scissor[1], draw->scissor[2], draw->scissor[3], draw->state.scissor_test_enable,
                 draw->fragment_offset[0], draw->fragment_offset[1], draw->state.z_enable, draw->state.z_write_enable,
                 draw->state.z_func, draw->state.blend_control[0], draw->state.color_write_enable[0],
                 draw->state.stencil_enable);
        trace_target(text, sizeof(text), "rt0", g_bound_color[0]);
        trace_target(text, sizeof(text), "rt1", g_bound_color[1]);
        trace_target(text, sizeof(text), "ds", g_bound_depth);
        port_write_error(text);
        port_write_error("\n");
        // a few registers every lit draw reads: ps c0 g_exposure, c210..212 the lightmap compression scales, vs c233..238
        // the atmosphere constants
        if (g_shadows.pixel_float != 0 && g_shadows.vertex_float != 0) {
            const float *p = g_shadows.pixel_float, *v = g_shadows.vertex_float;
            snprintf(text, sizeof(text),
                     "trace   ps c0 %g %g %g %g c210 %g %g %g %g c211 %g %g %g %g c212 %g %g %g %g | vs c233 %g %g %g %g "
                     "c234 %g %g %g %g c235 %g %g %g %g",
                     p[0], p[1], p[2], p[3], p[840], p[841], p[842], p[843], p[844], p[845], p[846], p[847], p[848],
                     p[849], p[850], p[851], v[932], v[933], v[934], v[935], v[936], v[937], v[938], v[939], v[940],
                     v[941], v[942], v[943]);
            port_write_error(text);
            port_write_error("\n");
            // the postprocess registers (postprocess_registers.h): c1 pixel size, c2 scale, c3 the first extra constant
            snprintf(text, sizeof(text), "trace   ps c1 %g %g %g %g c2 %g %g %g %g c3 %g %g %g %g\n", p[4], p[5], p[6], p[7], p[8],
                     p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
            port_write_error(text);
            // an object's lighting (hlsl_constant_persist.h): c4..c10 the rest of p_lighting_constant_0..9, c15
            // dynamic_environment_blend
            for (uint32_t c = 4; c <= 15; c += 4) {
                snprintf(text, sizeof(text), "trace   ps c%u %g %g %g %g c%u %g %g %g %g c%u %g %g %g %g c%u %g %g %g %g\n",
                         c, p[4 * c], p[4 * c + 1], p[4 * c + 2], p[4 * c + 3], c + 1, p[4 * c + 4], p[4 * c + 5],
                         p[4 * c + 6], p[4 * c + 7], c + 2, p[4 * c + 8], p[4 * c + 9], p[4 * c + 10], p[4 * c + 11],
                         c + 3, p[4 * c + 12], p[4 * c + 13], p[4 * c + 14], p[4 * c + 15]);
                port_write_error(text);
            }
        }
        for (uint32_t s = 0; s < PORT_VK_SAMPLER_COUNT; s++) {
            const s_port_vk_sampler &slot = draw->samplers[s];
            if (!slot.bound)
                continue;
            const s_xenos_texture_desc &d = slot.texture.source.desc;
            const uint32_t *fc = slot.texture.source.fetch_constant;
            snprintf(text, sizeof(text),
                     "trace   sampler %u: tex %p fmt %x dim %u %ux%ux%u levels %u base %p mip %p id %x fetch %08x %08x "
                     "%08x %08x %08x %08x min level %u",
                     s, slot.texture.key, d.d3d_format, d.dimension, d.width, d.height, d.depth, d.levels,
                     slot.texture.source.base, slot.texture.source.mip, slot.texture.source.identifier, fc[0], fc[1],
                     fc[2], fc[3], fc[4], fc[5], slot.state.min_mip_level);
            port_write_error(text);
            port_write_error("\n");
        }
    }
    // a version 3 pack: the shaders' own microcode, translated (xenos_draw)
    if (g_xenos) {
        xenos_draw(draw, dirty_mask, primitive_type, start, count, base_vertex, indexed, vs);
        return;
    }
    // a memexport update: the pack's compute shader for it (its pixel shader never runs on the Xbox either)
    s_shader_pack_shader_info vs_info = {};
    if (vs != 0 && vs->shader_index != SHADER_PACK_NOT_FOUND && shader_pack_get_shader(g_pack, vs->shader_index, &vs_info) &&
        vs_info.stage == k_shader_pack_stage_compute) {
        memexport_draw(draw, vs, start, count);
        return;
    }
    // Halo 3's water: the factor pass and the triangle-list shading pass (water_draw; the tessellated pass comes through
    // port_vk_draw_tessellated)
    if (vs != 0 && vs->shader_index != SHADER_PACK_NOT_FOUND && vs_info.vertex_type == k_shader_pack_vertex_water &&
        water_draw(draw, vs_info, primitive_type, start, count, indexed))
        return;
    // any other draw inside BeginExport / EndExport whose vertex shader has no compute translation: on the Xbox it
    // only exports (no position), so it rasterizes nothing
    if (g_export.base != 0) {
        report(key_of(draw->vertex_shader) ^ 0x78707274u,
               "a draw inside BeginExport / EndExport whose vertex shader has no compute translation (skipped)");
        return;
    }
    // SetPixelShader(NULL): depth and stencil only (lens flare occlusion queries, the hierarchical Z rebuild, shadow
    // stencil volumes); the pipeline has no fragment stage
    const bool depth_only = draw->pixel_shader == 0;
    s_shader_entry *ps = depth_only ? 0 : shader_entry(draw->pixel_shader);
    if (vs == 0 || (!depth_only && ps == 0) || vs->shader_index == SHADER_PACK_NOT_FOUND ||
        (!depth_only && ps->shader_index == SHADER_PACK_NOT_FOUND)) {
        s_shader_entry *bad = vs == 0 || vs->shader_index == SHADER_PACK_NOT_FOUND ? vs : ps;
        if (draw->vertex_shader == 0) {
            report(0x6e6f7673u, "a draw without a vertex shader (skipped)");
        } else if (bad != 0 && !bad->reported) {
            bad->reported = 1;
            device_log_text("a ");
            port_write_error(bad->stage == PORT_VK_STAGE_VERTEX ? "vertex" : "pixel");
            char hash[64] = " shader whose microcode (hash ";
            size_t n = strlen(hash);
            for (int k = 0; k < 16; k++)
                hash[n++] = "0123456789abcdef"[((k < 8 ? bad->hash_hi : bad->hash_lo) >> (28 - 4 * (k & 7))) & 15u];
            hash[n] = 0;
            port_write_error(hash);
            port_write_error(") the pack does not know (its draws are skipped)\n");
        } else if (bad == 0) {
            // a shader object no CreateVertexShader / CreatePixelShader / XGRegister*Shader registered
            report(vs == 0 ? 0x6e6f7368u : 0x6e6f7370u, vs == 0 ? "a draw with an unregistered vertex shader (skipped)"
                                                                 : "a draw with an unregistered pixel shader (skipped)");
        }
        return;
    }
    const uint32_t ps_index = depth_only ? SHADER_PACK_NOT_FOUND : ps->shader_index;
    if (depth_only && draw->state.z_enable != 0 && (draw->state.z_func & 7u) == 0u /* D3DCMP_NEVER */ &&
        draw->state.stencil_enable == 0)
        return; // touches nothing but the Xbox's hierarchical Z (rasterizer_rebuild_hierarchical_z_buffer)
    uint32_t declaration_hash = 0;
    const s_xenos_vertex_declaration *declaration = declaration_for(draw.declaration(), &declaration_hash);
    if (declaration == 0) {
        report(0x6e6f6463u, "a draw without a vertex declaration (skipped)");
        return;
    }
    // a version 2 pack records no pixel shader outputs: a pixel shader is taken to write all four targets
    const uint32_t slots = attached_color_slots(written_color_slots(draw->state, depth_only ? 0u : 0xfu));
    if (!begin_frame() || !ensure_pass(touches_depth(draw->state), slots))
        return;
    if (!shader_pack_get_shader(g_pack, vs->shader_index, &vs_info))
        return;
    // Decorators (s_decorator_vertex, render_decorators): one auto-indexed DrawVertices of index_count * instances
    // vertices per decorator group, a triangle strip (g_primitive_types[5]). The Xenos shader splits the vertex index
    // itself (decorators.hlsl, !pc): instance = index / index_count (VS c242.x), the instance record from stream 1, and
    // the mesh vertex index + (type_index - instance) * index_count from stream 0, type_index being the record's
    // position.w >> 8. The DX11 shader reads stream 1 as per-instance input instead, so the device draws it instanced:
    // stream 1 advances per instance (only for this vertex type: every other draw keeps its per-vertex declaration),
    // one draw per run of equal type_index, each instance its own strip.
    s_xenos_vertex_declaration instanced_declaration;
    const bool decorator = vs_info.vertex_type == k_shader_pack_vertex_decorator;
    if (decorator) {
        if (indexed || (primitive_type != PORT_VK_PT_TRIANGLESTRIP && primitive_type != PORT_VK_PT_TRIANGLELIST)) {
            report(0x64656331u ^ primitive_type, "a decorator draw that is not an auto-indexed strip or list (skipped)");
            return;
        }
        instanced_declaration = *declaration;
        instanced_declaration.instance_stream_mask = 1u << 1;
        // The instance color is a D3DCOLOR (the game's own buffer table, g_rasterizer_vertex_buffer_declarations
        // declaration 20: RGB light and an exponent in A) though the D3D declaration says UBYTE4N; the DX11 shader reads
        // it as MCC fed it, red from the third byte. Read as UBYTE4N the decorators were lit blue.
        for (uint32_t e = 0; e < instanced_declaration.element_count; e++) {
            s_xenos_d3d_vertex_element &element = instanced_declaration.elements[e];
            if (element.stream == 1 && element.usage == k_xenos_declusage_color && element.usage_index == 1 &&
                element.type == k_xenos_decltype_ubyte4n)
                element.type = k_xenos_decltype_d3dcolor;
        }
        declaration = &instanced_declaration;
        declaration_hash ^= 0x64656331u; // its own pipelines: the vertex input differs from the per-vertex layout
    }
    // The DX11 GPU-effect render shaders split the Xbox's one vertex index into (SV_InstanceID, SV_VertexID): a quad
    // list becomes 4-vertex strips, one instance per quad (particle_render.hlsl: index = instance * 4 + corner +
    // particle_index_range.x; light_volume.fx: profile = instance; corner = vertex_id ^ ((vertex_id & 2) >> 1) undoes
    // the strip order), and each strip one instance (beam.fx: strip = instance, profile = vertex_id / 2). The
    // instance numbers carry the Xbox draw's start: firstInstance = the start quad / strip, vertex numbers from 0.
    uint32_t instance_count = 1, first_instance = 0;
    g_index_range[0] = g_index_range[1] = 0;
    if (!indexed && shader_reads_instance(vs)) {
        if (primitive_type == PORT_VK_PT_QUADLIST && start % 4u == 0 && count % 4u == 0) {
            instance_count = count / 4u;
            first_instance = start / 4u;
            start = 0;
            count = 4;
            primitive_type = PORT_VK_PT_TRIANGLESTRIP;
            g_index_range[1] = 4;
        } else if (primitive_type == PORT_VK_PT_TRIANGLESTRIP && start % count == 0) {
            first_instance = start / count;
            start = 0;
            g_index_range[1] = count;
        } else {
            report(primitive_type * 0x9e37u + 0x696e73u,
                   "an instanced GPU-effect draw the device cannot split into instances (skipped)");
            return;
        }
    }

    uint32_t topology;
    bool quads = false, rects = false;
    switch (primitive_type) {
    case PORT_VK_PT_POINTLIST: topology = k_gpu_topology_point_list; break;
    case PORT_VK_PT_LINELIST: topology = k_gpu_topology_line_list; break;
    case PORT_VK_PT_LINESTRIP: topology = k_gpu_topology_line_strip; break;
    case PORT_VK_PT_TRIANGLELIST: topology = k_gpu_topology_triangle_list; break;
    case PORT_VK_PT_TRIANGLEFAN: topology = k_gpu_topology_triangle_fan; break;
    case PORT_VK_PT_TRIANGLESTRIP: topology = k_gpu_topology_triangle_strip; break;
    case PORT_VK_PT_QUADLIST:
        topology = k_gpu_topology_triangle_list;
        quads = true;
        break;
    case PORT_VK_PT_RECTLIST:
        if (indexed) {
            report(0x72656332u, "an indexed rectangle list (skipped)");
            return;
        }
        topology = k_gpu_topology_triangle_list;
        quads = rects = true;
        break;
    default:
        report(primitive_type * 0x9e37u + 3u, "a primitive type the renderer does not draw (skipped)");
        return;
    }
    const bool strip = topology == k_gpu_topology_triangle_strip || topology == k_gpu_topology_line_strip ||
                       topology == k_gpu_topology_triangle_fan;
    s_gpu_graphics_state state;
    build_state(draw->state, topology, indexed && strip && draw->state.primitive_reset_enable, slots, &state);
    state.fragcoord_offset[0] = draw->fragment_offset[0];
    state.fragcoord_offset[1] = draw->fragment_offset[1];
    if (draw->state.alpha_test_enable && (draw->state.alpha_func & 7u) != 7u /* D3DCMP_ALWAYS */ && !depth_only && !shader_clips(ps))
        report(key_of(draw->pixel_shader) ^ 0x61746573u, "D3D alpha test on with a pixel shader that never discards");
    if (draw->clip_plane_enable != 0) {
        scan_pc_members(vs);
        if (vs->pc_binding[k_pc_member_clip_plane] == ~0u &&
            vs->pc_binding[k_pc_member_dynamic_light_clip_plane] == ~0u)
            report(key_of(draw->vertex_shader) ^ 0x75637031u,
                   "user clip planes on with a vertex shader that has no clip-plane cbuffer member (not clipped)");
    }
    s_gpu_pipeline *pipeline = pipeline_for(vs->shader_index, ps_index, declaration, declaration_hash, state);
    if (pipeline == 0)
        return;
    // the streams the vertex shader reads: a declared stream no input reads need not be bound (the decorator
    // declaration's stream 2, the Xbox shader's own index fetch); a shader without vertex inputs (the DX11 GPU-effect
    // shaders read their records from a StructuredBuffer by SV_VertexID / SV_InstanceID) needs none
    const uint32_t used_streams = vs_info.input_count != 0 ? gpu_pipeline_vertex_streams(g_gpu, pipeline) : 0u;

    // vertex streams (a rectangle list's expanded to four vertices a rectangle, drawn as quads from vertex 0)
    s_data_binding streams[PORT_VK_MAX_STREAMS];
    if (rects) {
        if (count < 3u || !expand_rectangles(draw, declaration, start, count / 3u, streams))
            return;
        count = count / 3u * 4u;
        start = 0;
    }
    // the vertices the draw reads, for the streams copied into the ring for this draw (dynamic and user-pointer data:
    // converting the whole buffer from its offset on every draw made the menu's many small draws from one dynamic
    // buffer cost most of the frame)
    s_vertex_range range = {};
    range.whole = decorator || declaration->instance_stream_mask != 0 || instance_count != 1 || first_instance != 0;
    if (indexed) {
        range.indices = &draw.indices();
        range.index_first = start;
        range.index_count = count;
        range.base_vertex = base_vertex;
        if (draw.indices().data == 0 || (draw.indices().index_size != 2 && draw.indices().index_size != 4) ||
            ((uint64_t)start + count) * draw.indices().index_size > draw.indices().size)
            range.whole = true; // upload_indices reports it
    } else {
        range.known = true;
        range.first = start;
        range.end = start + count;
    }
    for (uint32_t s = 0; s < PORT_VK_MAX_STREAMS && !rects; s++) {
        if ((used_streams & (1u << s)) == 0)
            continue;
        if ((draw.stream_mask() & (1u << s)) == 0 ||
            !upload_vertices(draw.stream(s), declaration, s, &range, &streams[s])) {
            report_stream(draw, vs, s);
            return;
        }
    }
    // indices (quad lists are drawn as indexed triangle lists)
    s_data_binding index_binding = {};
    uint32_t index_count = 0;
    if (indexed) {
        if (!upload_indices(draw.indices(), start, count, quads, &index_binding, &index_count))
            return;
    } else if (quads) {
        index_count = count / 4u * 6u;
        uint32_t *dst = (uint32_t *)ring_alloc(index_count * 4u + 4u, 16, &index_binding.offset);
        if (dst == 0)
            return;
        for (uint32_t q = 0; q < count / 4u; q++) {
            const uint32_t b = start + 4u * q;
            const uint32_t list[6] = { b, b + 1u, b + 2u, b, b + 2u, b + 3u };
            memcpy(dst + 6u * q, list, sizeof(list));
        }
        index_binding.buffer = ring_buffer();
    }

    gpu_bind_group vs_set = GPU_NULL_HANDLE, ps_set = GPU_NULL_HANDLE;
    if (!write_stage_set(draw, pipeline, PORT_VK_STAGE_VERTEX, vs, &vs_set) ||
        (!depth_only && !write_stage_set(draw, pipeline, PORT_VK_STAGE_PIXEL, ps, &ps_set)))
        return;
    // a texture read through the other color space is converted by a compute pass (resolved_view), which ends the
    // rendering: begin it again on the same targets
    if (!gpu_rendering(g_gpu, 0, 0) && !ensure_pass(touches_depth(draw->state), slots))
        return;
    // after the conversions above may have begun the rendering again (edram_written): a depth restore names the
    // targets' content as this draw leaves it
    if (!depth_only)
        note_depth_restore(draw.packet);

    checkpoint_draw(draw, vs, ps, primitive_type, start, count, indexed);
    if (g_vk) {
        const auto *native = vulkan_gpu_native_pipeline(pipeline);
        vulkan_cmd_bind_pipeline_native(g_vk, native);
        if (vs_set != GPU_NULL_HANDLE && ps_set != GPU_NULL_HANDLE) {
            vulkan_cmd_bind_group_pair_native(g_vk, native, vulkan_gpu_native_bind_group(vs_set), 0, 0,
                                               vulkan_gpu_native_bind_group(ps_set));
        } else if (vs_set != GPU_NULL_HANDLE) {
            vulkan_cmd_bind_group_native(g_vk, native, 0, vulkan_gpu_native_bind_group(vs_set), 0, 0);
        } else if (ps_set != GPU_NULL_HANDLE) {
            vulkan_cmd_bind_group_native(g_vk, native, 1, vulkan_gpu_native_bind_group(ps_set), 0, 0);
        }
        for (uint32_t s = 0; s < PORT_VK_MAX_STREAMS; s++)
            if ((used_streams & (1u << s)) != 0)
                vulkan_cmd_bind_vertex_buffer_native(g_vk, s, vulkan_gpu_native_buffer(streams[s].buffer),
                                                     streams[s].offset, draw.stream(s).stride);
    } else {
        gpu_cmd_bind_pipeline(g_gpu, pipeline);
        if (vs_set != GPU_NULL_HANDLE)
            gpu_cmd_bind_group(g_gpu, pipeline, 0, vs_set);
        if (ps_set != GPU_NULL_HANDLE)
            gpu_cmd_bind_group(g_gpu, pipeline, 1, ps_set);
        for (uint32_t s = 0; s < PORT_VK_MAX_STREAMS; s++)
            if ((used_streams & (1u << s)) != 0)
                gpu_cmd_bind_vertex_buffer(g_gpu, s, streams[s].buffer, streams[s].offset, draw.stream(s).stride);
    }
    set_viewport_scissor(draw.packet);
    if (decorator) {
        draw_decorator_instances(draw, start, count);
    } else if (indexed || quads) {
        draw_indexed_from(index_binding, index_count, indexed ? base_vertex : 0, 1, 0);
    } else {
        record_draw(count, instance_count, start, first_instance);
    }
}

void PORT_MS_ABI port_vk_clear(uint32_t flags, const float color[4], float z, uint32_t stencil, const int *rects,
                   uint32_t rect_count)
{
    if (record_remote()) {
        if (!record_owner())
            return record_sync([&] { port_vk_clear(flags, color, z, stencil, rects, rect_count); });
        const auto rgba = record_quad(color);
        const int *copy = nullptr;
        if (rects && rect_count) {
            if (rect_count > k_record_slab_size / (4 * sizeof(int)))
                return record_sync([&] { port_vk_clear(flags, color, z, stencil, rects, rect_count); });
            const size_t n = rect_count * 4 * sizeof(int);
            copy = (const int *)memcpy(record_reserve(n), rects, n);
        }
        return record_post([=] { port_vk_clear(flags, rgba.get(), z, stencil, copy, rect_count); });
    }
    if (g_started <= 0)
        return;
    // the targets cleared: one bound to two slots is attached at the lower cleared one (attached_color_slots)
    const uint32_t slots = attached_color_slots(flags & PORT_VK_CLEAR_TARGETS);
    const bool touches = (flags & (PORT_VK_CLEAR_ZBUFFER | PORT_VK_CLEAR_STENCIL)) != 0;
    if (!begin_frame())
        return;
    // When clearing a full target (all channels for color, or both Z and stencil for depth), discard its prior EDRAM
    // alias contents BEFORE ensure_pass() so edram_sync_owner / edram_apply_clear do not copy or reinterpret pixels
    // that the clear is about to overwrite 100%. When not inside a render pass, full-target clears are deferred into
    // pending_clear_flags and folded into VK_ATTACHMENT_LOAD_OP_CLEAR when ensure_pass() opens for a draw (avoiding
    // 0-draw clear-only render passes and preventing eager edram_sync on uncleared sibling targets like rt1 before a
    // back-to-back TARGET1 clear).
    {
        s_target_entry *depth_target = pass_depth(touches);
        uint32_t min_w = 0xffffffffu, min_h = 0xffffffffu;
        for (uint32_t i = 0; i < 4; i++) {
            s_target_entry *t = (slots & (1u << i)) != 0 ? g_bound_color[i] : 0;
            if (t != 0 && t->image != 0) {
                if (t->width < min_w) min_w = t->width;
                if (t->height < min_h) min_h = t->height;
            }
        }
        if (depth_target != 0 && depth_target->image != 0) {
            if (depth_target->width < min_w) min_w = depth_target->width;
            if (depth_target->height < min_h) min_h = depth_target->height;
        }
        auto covers_target = [&](const s_target_entry *t) -> bool {
            if (t == 0 || t->image == 0 || t->width > min_w || t->height > min_h)
                return false;
            if (rect_count == 0 || rects == 0)
                return true;
            return rect_count == 1 && rects[0] <= 0 && rects[1] <= 0 &&
                   rects[2] >= (int)t->width && rects[3] >= (int)t->height;
        };
        const uint32_t clear_color_slots = flags & PORT_VK_CLEAR_TARGETS & slots;
        const bool clear_z = (flags & PORT_VK_CLEAR_ZBUFFER) != 0;
        const bool clear_s = (flags & PORT_VK_CLEAR_STENCIL) != 0;
        const bool rendering = gpu_rendering(g_gpu, 0, 0);
        bool pass_matches = rendering;
        if (pass_matches) {
            for (uint32_t i = 0; i < 4 && pass_matches; i++) {
                s_target_entry *t = (slots & (1u << i)) != 0 ? g_bound_color[i] : 0;
                pass_matches = g_pass_color[i] == (t != 0 ? t->image : 0);
            }
            if (pass_matches)
                pass_matches = g_pass_depth == (depth_target != 0 ? depth_target->image : 0);
        }
        bool all_full = true;
        bool any_target = false;
        for (uint32_t i = 0; i < 4; i++) {
            if ((clear_color_slots & (1u << i)) == 0)
                continue;
            s_target_entry *t = g_bound_color[i];
            if (t == 0 || t->image == 0)
                continue;
            any_target = true;
            if (covers_target(t))
                edram_discard_contents(t);
            else
                all_full = false;
        }
        if (clear_z || clear_s) {
            if (depth_target != 0 && depth_target->image != 0) {
                any_target = true;
                if (covers_target(depth_target)) {
                    if (clear_z && clear_s)
                        edram_discard_contents(depth_target);
                } else {
                    all_full = false;
                }
            }
        }
        const bool can_defer = all_full && any_target && (!rendering || !pass_matches);
        if (can_defer) {
            if (rendering)
                end_pass();
            for (uint32_t i = 0; i < 4; i++) {
                if ((clear_color_slots & (1u << i)) == 0)
                    continue;
                s_target_entry *t = g_bound_color[i];
                if (t == 0 || t->image == 0)
                    continue;
                t->pending_clear_flags = k_gpu_clear_color;
                if (color != 0)
                    memcpy(t->pending_clear_color, color, sizeof(t->pending_clear_color));
                else
                    memset(t->pending_clear_color, 0, sizeof(t->pending_clear_color));
                edram_written(t);
            }
            if ((clear_z || clear_s) && depth_target != 0 && depth_target->image != 0) {
                if (clear_z) {
                    depth_target->pending_clear_flags |= k_gpu_clear_depth;
                    depth_target->pending_clear_depth = z;
                }
                if (clear_s) {
                    depth_target->pending_clear_flags |= k_gpu_clear_stencil;
                    depth_target->pending_clear_stencil = stencil;
                }
                if (clear_z && clear_s)
                    edram_written(depth_target);
            }
            if (s_draw_record *record = checkpoint_record(1))
                record->count = flags;
            if (tracing()) {
                char text[400];
                snprintf(text, sizeof(text), "trace clear (deferred): flags %x color %g,%g,%g,%g z %g stencil %u rects %u (%d,%d,%d,%d)",
                         flags, color != 0 ? color[0] : 0.0f, color != 0 ? color[1] : 0.0f, color != 0 ? color[2] : 0.0f,
                         color != 0 ? color[3] : 0.0f, z, stencil, rect_count, rect_count ? rects[0] : 0, rect_count ? rects[1] : 0,
                         rect_count ? rects[2] : 0, rect_count ? rects[3] : 0);
                trace_target(text, sizeof(text), "rt0", g_bound_color[0]);
                trace_target(text, sizeof(text), "rt1", g_bound_color[1]);
                trace_target(text, sizeof(text), "ds", g_bound_depth);
                port_write_error(text);
                port_write_error("\n");
            }
            return;
        }
    }
    if (!ensure_pass(touches, slots))
        return;
    if (s_draw_record *record = checkpoint_record(1))
        record->count = flags;
    if (tracing()) {
        char text[400];
        snprintf(text, sizeof(text), "trace clear: flags %x color %g,%g,%g,%g z %g stencil %u rects %u (%d,%d,%d,%d)",
                 flags, color != 0 ? color[0] : 0.0f, color != 0 ? color[1] : 0.0f, color != 0 ? color[2] : 0.0f,
                 color != 0 ? color[3] : 0.0f, z, stencil, rect_count, rect_count ? rects[0] : 0, rect_count ? rects[1] : 0,
                 rect_count ? rects[2] : 0, rect_count ? rects[3] : 0);
        trace_target(text, sizeof(text), "rt0", g_bound_color[0]);
        trace_target(text, sizeof(text), "rt1", g_bound_color[1]);
        trace_target(text, sizeof(text), "ds", g_bound_depth);
        port_write_error(text);
        port_write_error("\n");
    }
    // D3DCLEAR_TARGETn clears render target n only (the lighting pass clears its HDR target, TARGET1, and keeps what
    // the albedo pass left in the LDR target's EDRAM)
    s_gpu_clear clear = {};
    if ((flags & PORT_VK_CLEAR_TARGETS) != 0)
        clear.flags |= k_gpu_clear_color;
    clear.color_mask = flags & PORT_VK_CLEAR_TARGETS & slots;
    if ((flags & PORT_VK_CLEAR_ZBUFFER) != 0)
        clear.flags |= k_gpu_clear_depth;
    if ((flags & PORT_VK_CLEAR_STENCIL) != 0)
        clear.flags |= k_gpu_clear_stencil;
    if (color != 0)
        memcpy(clear.color, color, sizeof(clear.color));
    clear.depth = z;
    clear.stencil = stencil;
    const uint32_t res_scale = bound_res_scale();
    if (res_scale > 1 && rect_count != 0 && rects != 0) {
        // guest rectangles (x0, y0, x1, y1) -> host pixels, in chunks
        int32_t scaled[4 * 16];
        for (uint32_t first = 0; first < rect_count; first += 16) {
            const uint32_t n = rect_count - first < 16u ? rect_count - first : 16u;
            for (uint32_t i = 0; i < 4u * n; i++)
                scaled[i] = rects[4u * first + i] * (int32_t)res_scale;
            gpu_cmd_clear_attachments(g_gpu, &clear, scaled, n);
        }
        return;
    }
    gpu_cmd_clear_attachments(g_gpu, &clear, rects, rect_count);
}

void PORT_MS_ABI port_vk_clear_edram(uint32_t flags, const float color[4], float z, uint32_t stencil, const int rect[4])
{
    if (record_remote()) {
        if (!record_owner())
            return record_sync([&] { port_vk_clear_edram(flags, color, z, stencil, rect); });
        const auto rgba = record_quad(color);
        const auto area = record_quad(rect);
        return record_post([=] { port_vk_clear_edram(flags, rgba.get(), z, stencil, area.get()); });
    }
    if (g_started <= 0 || !begin_frame())
        return;
    for (int t = 0; t < 5; t++) {
        s_target_entry *target = t < 4 ? g_bound_color[t] : g_bound_depth;
        const bool depth = t == 4;
        if (target == 0 || target->back_buffer || target->edram_base == 0xffffffffu)
            continue;
        s_gpu_clear clear = {};
        if (!depth && (flags & (1u << t)) != 0)
            clear.flags |= k_gpu_clear_color;
        clear.color_mask = 1u; // edram_apply_clear renders into the one surface
        if (depth && (flags & PORT_VK_CLEAR_ZBUFFER) != 0)
            clear.flags |= k_gpu_clear_depth;
        if (depth && (flags & PORT_VK_CLEAR_STENCIL) != 0)
            clear.flags |= k_gpu_clear_stencil;
        if (clear.flags == 0)
            continue;
        if (color != 0)
            memcpy(clear.color, color, sizeof(clear.color));
        clear.depth = z;
        clear.stencil = stencil;
        // the base's record, else a free one, else the oldest
        s_edram_clear *slot = 0;
        for (s_edram_clear &pending : g_edram_clears)
            if (pending.base == target->edram_base + 1u)
                slot = &pending;
        for (s_edram_clear &pending : g_edram_clears)
            if (slot == 0 && pending.base == 0)
                slot = &pending;
        for (s_edram_clear &pending : g_edram_clears)
            if (slot == 0 || (slot->base != target->edram_base + 1u && slot->base != 0 && pending.serial < slot->serial))
                slot = &pending;
        slot->base = target->edram_base + 1u;
        slot->serial = ++g_edram_serial;
        slot->depth = depth ? 1u : 0u;
        slot->clear = clear;
        memcpy(slot->rect, rect, sizeof(slot->rect));
        if (tracing()) {
            char text[200];
            snprintf(text, sizeof(text), "trace edram clear issued: base %x flags %x rect %d,%d,%d,%d\n", target->edram_base,
                     clear.flags, rect[0], rect[1], rect[2], rect[3]);
            port_write_error(text);
        }
    }
}

static s_query_entry *query_entry(const void *query, bool create)
{
    uint32_t index;
    if (map_find(&g_query_index, key_of(query), &index))
        return &g_queries.items[index];
    if (!create)
        return 0;
    s_query_entry *entry = g_queries.push();
    if (entry == 0)
        return 0;
    memset(entry, 0, sizeof(*entry));
    entry->key = query;
    map_set(&g_query_index, key_of(query), g_queries.count - 1u);
    return entry;
}

void PORT_MS_ABI port_vk_query_begin(const void *query, uint32_t accumulate)
{
    if (record_remote()) {
        return record_issue(query, [=] { port_vk_query_begin(query, accumulate); });
    }
    if (g_started <= 0 || !begin_frame())
        return;
    s_query_entry *entry = query_entry(query, true);
    if (entry == 0)
        return;
    if (!accumulate) {
        entry->count = 0;
        entry->completed_samples = 0;
    }
    uint32_t serial = 0, record = 0;
    if (entry->count < 8 && gpu_occlusion_begin(g_gpu, &serial, &record)) {
        entry->serial[entry->count] = serial;
        entry->record[entry->count] = record;
        entry->count++;
        g_active_query = query;
    } else {
        report(0x6f636371u, "an occlusion query the device could not begin (it reports the samples it has)");
    }
}

void PORT_MS_ABI port_vk_query_end(const void *query)
{
    if (record_remote()) {
        return record_issue(query, [=] { port_vk_query_end(query); });
    }
    (void)query;
    if (g_started <= 0)
        return;
    gpu_occlusion_end(g_gpu);
    g_active_query = 0;
}

int PORT_MS_ABI port_vk_query_result(const void *query, uint32_t *out_samples)
{
    if (record_remote()) return record_poll_query(query, out_samples);
    *out_samples = 0;
    s_query_entry *entry = g_started > 0 ? query_entry(query, false) : 0;
    if (entry == 0)
        return 1; // never issued: done, no samples
    uint64_t total = entry->completed_samples;
    for (uint32_t i = 0; i < entry->count; i++) {
        uint64_t samples = 0;
        if (!gpu_occlusion_result(g_gpu, entry->serial[i], entry->record[i], &samples))
            return 0;
        total += samples;
    }
    // HALO3_RES_SCALE: N^2 host samples per guest pixel. Rounded up, so a sliver that is visible counts as visible.
    const uint64_t per_pixel = (uint64_t)g_res_scale * g_res_scale;
    total = (total + per_pixel - 1u) / per_pixel;
    *out_samples = total > 0xffffffffull ? 0xffffffffu : (uint32_t)total;
    return 1;
}

int PORT_MS_ABI port_vk_query_result_flush(const void *query, uint32_t *samples)
{
    if (record_remote()) return record_result([&] { return port_vk_query_result(query, samples); });
    return port_vk_query_result(query, samples);
}

// A resolve into memory the CPU reads (s_texture_entry::cpu_read): the rectangle also goes into the entry's readback
// buffer, for port_vk_texture_lock.
static void record_readback(s_texture_entry *entry, uint32_t level, uint32_t layer, uint32_t x, uint32_t y, uint32_t w,
                            uint32_t h)
{
    uint32_t block_width = 0, block_height = 0, bytes = 0;
    if (!gpu_format_block_info(entry->host_format, &block_width, &block_height, &bytes) || block_width != 1 ||
        block_height != 1)
        return;
    const uint32_t size = w * h * bytes;
    if (entry->readback == 0 || entry->readback_size < size) {
        if (entry->readback != 0)
            gpu_buffer_retire(g_gpu, entry->readback);
        entry->readback = gpu_buffer_create(g_gpu, size, k_gpu_buffer_transfer_dst, k_gpu_memory_readback);
        entry->readback_size = entry->readback != 0 ? size : 0;
        if (entry->readback == 0)
            return;
    }
    const uint32_t rs = entry->resolved_scale != 0 ? entry->resolved_scale : g_res_scale;
    if (rs > 1) {
        // the image holds N x N host texels per guest texel: the CPU reads guest texels, each the host texel at its
        // centre (a NEAREST blit of the scaled rectangle into a guest-size image)
        if (level != 0 || layer != 0) {
            report(0x72627363u, "a CPU read of a scaled resolve into a level or slice other than the first (not read)");
            return;
        }
        if (entry->readback_image == 0 || entry->readback_image_w < w || entry->readback_image_h < h) {
            if (entry->readback_image != 0)
                gpu_image_retire(g_gpu, entry->readback_image);
            s_gpu_image_desc desc = {};
            desc.view_type = k_gpu_view_2d;
            desc.format = entry->host_format;
            desc.width = w;
            desc.height = h;
            desc.depth = 1;
            desc.mip_levels = 1;
            desc.array_layers = 1;
            desc.usage = k_gpu_image_sampled | k_gpu_image_transfer_dst | k_gpu_image_transfer_src;
            entry->readback_image = gpu_image_create(g_gpu, &desc);
            entry->readback_image_w = entry->readback_image != 0 ? w : 0u;
            entry->readback_image_h = entry->readback_image != 0 ? h : 0u;
            if (entry->readback_image == 0)
                return;
        }
        gpu_cmd_copy_image(g_gpu, entry->image, (int32_t)(x * rs), (int32_t)(y * rs),
                           w * rs, h * rs, entry->readback_image, 0, 0, 0, 0, w, h);
        gpu_cmd_copy_image_to_buffer(g_gpu, entry->readback_image, 0, 0, 0, 0, w, h, entry->readback, 0);
    } else {
        gpu_cmd_copy_image_to_buffer(g_gpu, entry->image, level, layer, (int32_t)x, (int32_t)y, w, h,
                                     entry->readback, 0);
    }
    entry->readback_serial = g_frame_serial;
    entry->readback_slot = gpu_frame_index(g_gpu);
    entry->readback_level = level;
    entry->readback_layer = layer;
    entry->readback_x = x;
    entry->readback_y = y;
    entry->readback_w = w;
    entry->readback_h = h;
}

// A resolve's exponent bias (D3DRESOLVE_EXPONENTBIAS, a 6-bit signed field at bit 26 of the flags): the colors leave
// EDRAM scaled by 2^bias. The source rectangle is scaled into a scratch image of the source's size and format, which the
// resolve then copies from; NULL when it cannot be (the resolve copies unscaled, reported). `clamp`: the destination
// is fixed point (RB_COPY_DEST_INFO 8_8_8_8 / 2_10_10_10 / 16_16 / 16_16_16_16), so the resolve clamps the float colors
// to the destination's range on the way out ([low, high]: [0, 1] unsigned, [-1, 1] signed), as the console's resolve
// hardware does; the scratch image keeps the source's float format, so the clamp is applied here.
static bool scaled_resolve_into(const s_target_entry *source, s_gpu_image *destination, int32_t bias, int x, int y,
                                uint32_t w, uint32_t h, bool clamp, float low, float high)
{
    uint32_t offset = 0;
    int32_t *parameters = (int32_t *)ring_alloc(GPU_SCALE_IMAGE_PARAMETERS_SIZE, g_uniform_alignment, &offset);
    if (parameters == 0)
        return false;
    const int32_t rect[4] = { x, y, (int32_t)w, (int32_t)h };
    // .x 2^bias (bias in -32..31), .y non-zero: clamp to [.z, .w] (resolve_scale.hlsl)
    uint32_t factor[4] = { (uint32_t)(127 + bias) << 23, clamp ? 0x3f800000u : 0u, 0, 0 };
    memcpy(&factor[2], &low, 4);
    memcpy(&factor[3], &high, 4);
    memcpy(parameters, rect, sizeof(rect));
    memcpy(parameters + 4, factor, sizeof(factor));
    if (!xe_layout_call(gpu_cmd_scale_image_rect, g_gpu, source->image, destination, ring_buffer(), offset, w, h)) {
        report(0x72657864u ^ source->host_format, "a resolve exponent bias the device cannot apply to this format (not "
                                               "applied: shaderStorageImageWriteWithoutFormat)");
        return false;
    }
    return true;
}

static s_gpu_image *scaled_resolve_source(const s_target_entry *source, int32_t bias, int x, int y, uint32_t w,
                                          uint32_t h, bool clamp, float low, float high)
{
    if (source->host_format != k_gpu_format_r16g16b16a16_sfloat && source->host_format != k_gpu_format_r16g16_sfloat &&
        source->host_format != k_gpu_format_r32g32_sfloat && source->host_format != k_gpu_format_r32g32b32a32_sfloat) {
        report(0x72657862u ^ source->host_format, "a resolve exponent bias from a target that is not float (not applied)");
        return 0;
    }
    const uint32_t ss = source->scale != 0 ? source->scale : g_res_scale;
    s_scratch_image *slot = 0;
    for (s_scratch_image &scratch : g_resolve_scratch) {
        if (scratch.image != 0 && scratch.width == source->width * ss &&
            scratch.height == source->height * ss &&
            scratch.format == source->host_format) {
            slot = &scratch;
            break;
        }
        if (scratch.image == 0 && slot == 0)
            slot = &scratch;
    }
    if (slot == 0) {
        report(0x72657863u, "more than 8 sizes or formats of exponent-biased resolve sources (bias not applied)");
        return 0;
    }
    if (slot->image == 0) {
        s_gpu_image_desc desc = {};
        desc.view_type = k_gpu_view_2d;
        desc.format = source->host_format;
        desc.width = source->width * ss; // the source image's size (HALO3_RES_SCALE)
        desc.height = source->height * ss;
        desc.depth = 1;
        desc.mip_levels = 1;
        desc.array_layers = 1;
        desc.usage = k_gpu_image_sampled | k_gpu_image_storage | k_gpu_image_transfer_src;
        slot->image = gpu_image_create(g_gpu, &desc);
        slot->width = source->width * ss;
        slot->height = source->height * ss;
        slot->format = source->host_format;
        if (slot->image == 0)
            return 0;
    }
    return scaled_resolve_into(source, slot->image, bias, x, y, w, h, clamp, low, high) ? slot->image : 0;
}

// Whether a resolve from a float render target into a texture of this format clamps (its layout is fixed point), and
// to what: [0, 1], or [-1, 1] when the destination's first component is GPUSIGN_SIGNED. The texture format itself
// decides, not texel_layout: a 16_EXPAND / 16_16_EXPAND / 16_16_16_16_EXPAND texture has the texel size of its 16-bit
// fixed-point namesake but holds float16 values (xenia texture_info.h: k_16_16_16_16_EXPAND's base format is
// k_16_16_16_16_FLOAT). Halo 3 resolves its exposure luminance chain (288x160 .. 1x1) into 16_16_16_16_EXPAND
// textures, and camera_fx_settings reads the 1x1's log luminance back on the CPU: clamped to [-1, 1] it was wrong by
// several stops (gameplay exposure 0.94 instead of 6.5 on Arrival).
static bool resolve_clamp_range(uint32_t source_host_format, uint32_t dest_d3d_format, float *low, float *high)
{
    if (source_host_format != k_gpu_format_r16g16b16a16_sfloat && source_host_format != k_gpu_format_r16g16_sfloat &&
        source_host_format != k_gpu_format_r32g32_sfloat && source_host_format != k_gpu_format_r32g32b32a32_sfloat)
        return false;
    switch (xenos_d3dformat_texture_format(dest_d3d_format)) {
    case k_xenos_format_8_8_8_8:
    case 50u: // 8_8_8_8_AS_16_16_16_16
    case k_xenos_format_2_10_10_10:
    case 54u: // 2_10_10_10_AS_16_16_16_16
    case k_xenos_format_16_16:
    case k_xenos_format_16_16_16_16:
        *low = xenos_d3dformat_sign(dest_d3d_format, 0) == 1u ? -1.0f : 0.0f;
        *high = 1.0f;
        return true;
    default:
        return false;
    }
}

// The texture takes the actual, shader-readable RT backing. Its old allocation
// stays with the target. Until a write separates them, the reciprocal indices
// identify the exact same pixels, including on another resolve to this texture.
static bool handoff_color_snapshot(s_target_entry *source, s_texture_entry *entry, const s_gpu_image_desc &destination)
{
    if (!g_vk || !(destination.usage & k_gpu_image_color_attachment) || source->depth || source->msaa ||
        source->back_buffer || source->edram_base == UINT32_MAX || !source->content || source->color_exp_bias ||
        source->pending_clear_flags || entry->cpu_read || g_present_resolve >= 0 || resolve_dumping() ||
        target_host_format(entry->d3d_format, 0) != source->host_format || edram_needs_sync(source))
        return false;
    const uint32_t texture = uint32_t(entry - g_textures.items) + 1u;
    if (source->snapshot_texture && source->snapshot_texture != texture)
        return false;
    s_gpu_image_desc rt;
    gpu_image_get_desc(g_gpu, source->image, &rt);
    if (memcmp(&destination, &rt, sizeof(rt)) ||
        (!source->snapshot_texture && reinterpret_cast<s_vulkan_image *>(source->image)->layout == VK_IMAGE_LAYOUT_UNDEFINED))
        return false;
    materialize_pending_7e3_for_owner(source);
    end_pass();
    vulkan_cmd_flush_memexports(g_vk);
    if (!source->snapshot_texture) {
        return_color_snapshot(entry); // its old target keeps the pixels we're about to replace
        exchange_resolve_images(source, entry);
        source->snapshot_texture = texture;
        entry->snapshot_target = uint32_t(source - g_targets.items) + 1u;
    }
    color_snapshot_sampleable(entry->image);
    return true;
}

void PORT_MS_ABI port_vk_resolve(uint32_t flags, uint32_t source_index, const int *source_rect,
                     const s_port_vk_texture *destination, uint32_t dest_x, uint32_t dest_y, uint32_t dest_level,
                     uint32_t dest_slice, const float clear_color[4], float clear_z, uint32_t clear_stencil)
{
    if (record_remote()) {
        const auto rgba = record_quad(clear_color);
        const auto area = record_quad(source_rect);
        const auto texture = record_optional(destination);
        return record_target_command({destination ? destination->source.base : nullptr, 1},
                                     destination ? destination->key : nullptr, [=] {
            port_vk_resolve(flags, source_index, area.get(), texture.get(), dest_x, dest_y, dest_level, dest_slice,
                            rgba.get(), clear_z, clear_stencil);
        });
    }
    if (g_started <= 0 || !begin_frame())
        return;
    resolve_state_changed(); // what resolved_entry finds changes below (slot_resolved)
    if (s_draw_record *record = checkpoint_record(2)) {
        record->count = flags;
        record->start = source_index;
    }
    s_target_entry *source = (flags & 0x4u) != 0 ? g_bound_depth : (source_index < 4 ? g_bound_color[source_index] : 0);
    // The samples a resolve of a multisampled surface copies (D3DRESOLVE_FRAGMENT0..3 0x10..0x40, FRAGMENTS01 / 23 /
    // 0123 0x50..0x70 averaged, ALLFRAGMENTS 0: all averaged; a depth resolve copies one sample of each pixel, xenia-edge
    // draw_util.cc SanitizeCopySampleSelect). The device keeps one sample per pixel: sample 0 of a view over another
    // surface's EDRAM (edram_gather_samples), the one sample a draw into the surface writes; Halo 3 resolves FRAGMENT0.
    if (source != 0 && source->msaa != 0) {
        const uint32_t select = (flags >> 4) & 7u;
        if (select == 2u || select == 3u || select == 4u || select == 6u)
            report(0x66726167u ^ select, "a resolve of samples other than sample 0 of a multisampled surface (sample 0 "
                                         "resolved: the device keeps one sample per pixel)");
    }
    int rect[4] = { 0, 0, source != 0 ? (int)source->width : 0, source != 0 ? (int)source->height : 0 };
    if (source_rect != 0)
        memcpy(rect, source_rect, sizeof(rect));
    if (tracing()) {
        char text[400];
        snprintf(text, sizeof(text), "trace resolve %u: flags %x rect %d,%d,%d,%d -> tex %p (fmt %x %ux%u base %p) at %u,%u level %u slice %u",
                 g_resolve_dump_index, flags, rect[0], rect[1], rect[2], rect[3], destination != 0 ? destination->key : 0,
                 destination != 0 ? destination->source.desc.d3d_format : 0u, destination != 0 ? destination->source.desc.width : 0u,
                 destination != 0 ? destination->source.desc.height : 0u, destination != 0 ? destination->source.base : (const void *)0, dest_x, dest_y, dest_level, dest_slice);
        trace_target(text, sizeof(text), "src", source);
        port_write_error(text);
        port_write_error("\n");
    }
    // the frame manifest's row (HALO3_DUMP_RESOLVES_FRAME); the copy below fills in what it wrote
    s_resolve_dump note = {};
    note.sequence = g_resolve_sequence++;
    note.draw_serial = g_draw_serial;
    note.flags = flags;
    note.source_index = source_index;
    note.depth = (flags & 0x4u) != 0 ? 1u : 0u;
    note.resolve_exp_bias = (int32_t)(((flags >> 26) ^ 0x20u) & 0x3fu) - 0x20;
    memcpy(note.rect, rect, sizeof(note.rect));
    if (source != 0) {
        note.source_key = source->key;
        note.source_edram_base = source->edram_base;
        note.source_d3d_format = source->d3d_format;
        note.source_host_format = source->host_format;
        note.source_width = source->width;
        note.source_height = source->height;
        note.source_exp_bias = source->color_exp_bias;
    }
    if (destination != 0) {
        note.dest_key = destination->key;
        note.dest_d3d_format = destination->source.desc.d3d_format;
        note.dest_width = destination->source.desc.width;
        note.dest_height = destination->source.desc.height;
    }
    note.dest_x = dest_x;
    note.dest_y = dest_y;
    note.dest_level = dest_level;
    note.dest_slice = dest_slice;
    bool noted = false;
    if (source != 0 && source->image != 0 && destination != 0 && destination->key != 0) {
        // A depth resolve (D3DRESOLVE_DEPTHSTENCIL) copies the depth buffer into a texture of the depth image's own
        // format, which shaders sample through its depth-only view: scene depth in [0, 1] in the red channel, as the
        // DX11 shaders read it (soft particles, depth of field, restore_ldr_hdr_depth, water). One level and slice.
        const bool depth = (flags & 0x4u) != 0;
        {
            const s_xenos_texture_desc &desc = destination->source.desc;
            s_texture_entry *entry = texture_entry(destination->key, true);
            const uint32_t levels = depth ? 1u : desc.levels != 0 ? desc.levels : 1u;
            const uint32_t layers = depth ? 1u : desc.dimension == k_xenos_dimension_cube ? 6u
                                    : desc.dimension == k_xenos_dimension_array ? (desc.depth != 0 ? desc.depth : 1u) : 1u;
            s_gpu_image_desc current = {};
            if (entry != 0 && entry->image != 0)
                gpu_image_get_desc(g_gpu, entry->image, &current);
            // HALO3_RES_SCALE: the image is as many times larger as its source surface's
            const uint32_t ss = source->scale != 0 ? source->scale : g_res_scale;
            const uint32_t image_w = (desc.width != 0 ? desc.width : 1u) * ss;
            const uint32_t image_h = (desc.height != 0 ? desc.height : 1u) * ss;
            if (entry != 0 && (!entry->resolved || entry->image == 0 || current.width != image_w ||
                               current.height != image_h || current.format != source->host_format ||
                               current.mip_levels != levels || current.array_layers != layers)) {
                release_texture_image(entry);
                s_gpu_image_desc image_desc = {};
                image_desc.view_type = depth ? (uint32_t)k_gpu_view_2d
                                             : view_type_of(desc.dimension == k_xenos_dimension_3d ? k_xenos_dimension_2d : desc.dimension);
                image_desc.format = source->host_format; // the render target's own format: the copy is exact
                image_desc.width = image_w;
                image_desc.height = image_h;
                image_desc.depth = 1;
                image_desc.mip_levels = levels;
                image_desc.array_layers = layers;
                image_desc.usage = k_gpu_image_sampled | k_gpu_image_transfer_dst | k_gpu_image_transfer_src;
                if (!depth && image_desc.view_type == k_gpu_view_2d && levels == 1 && layers == 1 &&
                    (source->host_format == k_gpu_format_r16g16b16a16_sfloat ||
                     source->host_format == k_gpu_format_r16g16_sfloat ||
                     source->host_format == k_gpu_format_r32g32_sfloat ||
                     source->host_format == k_gpu_format_r32g32b32a32_sfloat ||
                     source->host_format == k_gpu_format_a2b10g10r10_unorm ||
                     source->host_format == k_gpu_format_b8g8r8a8_unorm ||
                     source->host_format == k_gpu_format_r8g8b8a8_unorm ||
                     source->host_format == k_gpu_format_r16g16b16a16_unorm) &&
                    gpu_format_storage(g_gpu, source->host_format))
                    image_desc.usage |= k_gpu_image_storage;
                // Only recycle an image the RT can render into and the texture can
                // sample with every usage it would ordinarily have (including storage).
                current = image_desc;
                if (g_vk && !depth && !source->back_buffer && !source->msaa && !entry->cpu_read &&
                    desc.dimension == k_xenos_dimension_2d && levels == 1 && layers == 1 &&
                    source->host_width == image_w && source->host_height == image_h) {
                    s_gpu_image_desc rt;
                    gpu_image_get_desc(g_gpu, source->image, &rt);
                    if ((rt.usage & image_desc.usage) == image_desc.usage)
                        current.usage |= k_gpu_image_color_attachment;
                }
                entry->image = gpu_image_create(g_gpu, &current);
                if (!entry->image && current.usage != image_desc.usage) {
                    current = image_desc;
                    entry->image = gpu_image_create(g_gpu, &current);
                }
                g_resolved_headers.add(destination->key);
                entry->resolved = 1;
                entry->resolved_scale = ss;
                entry->host_format = source->host_format;
            }
            if (entry != 0 && entry->image != 0) {
                // the texels now live in this memory, whichever header names it next
                record_resolved(destination->key, destination->source.base, true);
                entry->base = destination->source.base;
                entry->mip = destination->source.mip;
                entry->resolved_scale = ss;
                entry->depth_edram = depth ? source->edram_base + 1u : 0u; // 0xffffffff (no EDRAM): 0
                if (entry->depth_edram != 0)
                    record_depth_resolve_edram(entry->depth_edram);
                entry->depth_float = depth && texel_layout(source->d3d_format) == 23u /* 24_8_FLOAT */ ? 1u : 0u;
                entry->linear_storage = !depth && resolve_keeps_gamma_codes(source->d3d_format, desc.d3d_format) ? 1u : 0u;
                entry->resolve_serial++;
                entry->d3d_format = desc.d3d_format;
                entry->width = desc.width;
                entry->height = desc.height;
                if (entry->pending_7e3_owner != 0) {
                    entry->pending_7e3_owner = 0;
                    entry->pending_7e3_content = 0;
                    if (g_pending_7e3_texture == (uint32_t)(entry - g_textures.items) + 1u)
                        g_pending_7e3_texture = 0;
                }
                if (destination->source.base != 0) {
                    s_memory_records_lock lock;
                    map_set(&g_resolved_memory, key_of(destination->source.base), (uint32_t)(entry - g_textures.items) + 1u);
                    g_resolved_bases.add(destination->source.base);
                    // the GPU's texels: a later CPU read gets them through write_back, a later sample from the image
                    map_set(&g_host_order_memory, key_of(destination->source.base), 0);
                }
            }
            if (entry != 0 && entry->image != 0) {
                int x0 = rect[0] < 0 ? 0 : rect[0], y0 = rect[1] < 0 ? 0 : rect[1];
                int x1 = rect[2] > (int)source->width ? (int)source->width : rect[2];
                int y1 = rect[3] > (int)source->height ? (int)source->height : rect[3];
                uint32_t level_w = desc.width >> dest_level, level_h = desc.height >> dest_level;
                level_w = level_w != 0 ? level_w : 1u;
                level_h = level_h != 0 ? level_h : 1u;
                uint32_t w = x1 > x0 ? (uint32_t)(x1 - x0) : 0u, h = y1 > y0 ? (uint32_t)(y1 - y0) : 0u;
                if (dest_x + w > level_w)
                    w = dest_x < level_w ? level_w - dest_x : 0u;
                if (dest_y + h > level_h)
                    h = dest_y < level_h ? level_h - dest_y : 0u;
                // the rectangle so far is in guest pixels; the images are ss times as large (HALO3_RES_SCALE)
                const int n = (int)ss;
                const uint32_t hw = w * ss, hh = h * ss;
                // HALO3_RES_SCALE with the half-pixel offset: a draw covering the guest rectangle exactly leaves the first
                // N >> 1 host columns and rows uncovered (the offset is N / 2 host pixels); as xenia's resolves do
                // (resolve_resolution_scale_fill_half_pixel_offset), they take the first surely covered column and row.
                const uint32_t half = ss >> 1;
                const uint32_t fill = ss > 1 && g_half_pixel_offset && hw > half && hh > half ? half : 0u;
                if (depth && (dest_level != 0 || dest_slice != 0)) {
                    report(0x7a72736cu, "a depth resolve into a texture level or slice other than the first (skipped)");
                } else if (depth && w != 0 && h != 0) {
                    if (entry->snapshot_target) restore_color_snapshot(&g_targets.items[entry->snapshot_target - 1u]);
                    bool gathered = false;
#ifdef HALO3_64BIT
                    if (ss == 1)
                        gathered = resolve_cropped_depth(source, entry->image, rect, dest_x, dest_y, w, h);
#endif
                    if (!gathered) {
                        edram_sync(source);
                        materialize_target_contents(source);
                        gpu_cmd_copy_depth(g_gpu, source->image, x0 * n, y0 * n, hw, hh, entry->image, (int)dest_x * n,
                                           (int)dest_y * n);
                        for (uint32_t i = 0; i < fill; i++) {
                            const int f = (int)fill, sx = x0 * n, sy = y0 * n, dx = (int)dest_x * n, dy = (int)dest_y * n;
                            gpu_cmd_copy_depth(g_gpu, source->image, sx + f, sy, 1, hh, entry->image, dx + (int)i, dy);
                            gpu_cmd_copy_depth(g_gpu, source->image, sx, sy + f, hw, 1, entry->image, dx, dy + (int)i);
                            for (uint32_t j = 0; j < fill; j++)
                                gpu_cmd_copy_depth(g_gpu, source->image, sx + f, sy + f, 1, 1, entry->image, dx + (int)i,
                                                   dy + (int)j);
                        }
                    }
                } else if (!depth && w != 0 && h != 0) {
                    const int32_t bias = (int32_t)(((flags >> 26) ^ 0x20u) & 0x3fu) - 0x20;
                    float low = 0.0f, high = 1.0f;
                    const bool clamp = resolve_clamp_range(source->host_format, desc.d3d_format, &low, &high);
                    const bool full_copy = !bias && !clamp && !fill && !dest_level && !dest_slice &&
                        !x0 && !y0 && !dest_x && !dest_y && w == source->width && h == source->height &&
                        w == desc.width && h == desc.height && hw == source->host_width && hh == source->host_height &&
                        desc.dimension == k_xenos_dimension_2d && levels == 1 && layers == 1;
                    bool direct_edram = full_copy && handoff_color_snapshot(source, entry, current);
                    if (!direct_edram && entry->snapshot_target)
                        restore_color_snapshot(&g_targets.items[entry->snapshot_target - 1u]);
                    if (!direct_edram && source->pending_clear_flags == 0 && bias == 0 && !clamp && fill == 0 &&
                        dest_level == 0 && dest_slice == 0 && x0 == 0 && y0 == 0 && dest_x == 0 && dest_y == 0 &&
                        w == source->width && h == source->height) {
                        uint32_t pwl_encode = 0;
                        s_target_entry *owner_7e3 = edram_7e3_owner(source);
                        s_target_entry *owner_pwl = owner_7e3 == 0 ? edram_pwl_owner(source, &pwl_encode) : 0;
                        if (owner_7e3 != 0 && entry->image != source->image && entry->image != owner_7e3->image) {
                            const uint32_t os = owner_7e3->scale != 0 ? owner_7e3->scale : g_res_scale;
                            s_gpu_image_desc dst_desc = {};
                            gpu_image_get_desc(g_gpu, entry->image, &dst_desc);
                            if ((dst_desc.usage & k_gpu_image_storage) != 0 &&
                                dst_desc.format == k_gpu_format_a2b10g10r10_unorm &&
                                dst_desc.view_type == k_gpu_view_2d && dst_desc.mip_levels == 1 &&
                                dst_desc.array_layers == 1 &&
                                dst_desc.width == owner_7e3->width * os && dst_desc.height == owner_7e3->height * os) {
                                s_gpu_image *from = target_read_image(owner_7e3);
                                if (!entry->cpu_read && !resolve_dumping() && g_present_resolve < 0) {
                                    if (g_pending_7e3_texture != 0 && g_pending_7e3_texture <= g_textures.count)
                                        materialize_pending_7e3(&g_textures.items[g_pending_7e3_texture - 1u]);
                                    entry->pending_7e3_owner = (uint32_t)(owner_7e3 - g_targets.items) + 1u;
                                    entry->pending_7e3_content = owner_7e3->content;
                                    g_pending_7e3_texture = (uint32_t)(entry - g_textures.items) + 1u;
                                } else {
                                    xe_layout_call_once(g_vk_reinterpret_7e3_initialized, g_frame_open != 0,
                                                        gpu_cmd_reinterpret_7e3, g_gpu, from, entry->image);
                                    finish_resolve_conversion(entry->image);
                                }
                                direct_edram = true;
                            }
                        } else if (owner_pwl != 0 && entry->image != source->image && entry->image != owner_pwl->image) {
                            const uint32_t os = owner_pwl->scale != 0 ? owner_pwl->scale : g_res_scale;
                            s_gpu_image_desc dst_desc = {};
                            gpu_image_get_desc(g_gpu, entry->image, &dst_desc);
                            if ((dst_desc.usage & k_gpu_image_storage) != 0 &&
                                dst_desc.format == source->host_format &&
                                dst_desc.view_type == k_gpu_view_2d && dst_desc.mip_levels == 1 &&
                                dst_desc.array_layers == 1 &&
                                dst_desc.width == owner_pwl->width * os && dst_desc.height == owner_pwl->height * os) {
                                if (xe_layout_call(gpu_cmd_convert_pwl, g_gpu, target_read_image(owner_pwl),
                                                   entry->image, pwl_encode)) {
                                    finish_resolve_conversion(entry->image);
                                    direct_edram = true;
                                }
                            }
                        }
                    }
                    if (!direct_edram) {
                        edram_sync(source);
                        if (bias || clamp) materialize_target_contents(source);
                        s_gpu_image *pixels = target_read_image(source);
                        bool direct_scaled = false;
                        if ((bias != 0 || clamp) && fill == 0 && dest_level == 0 && dest_slice == 0 &&
                            (int)dest_x == x0 && (int)dest_y == y0 && entry->image != source->image) {
                            s_gpu_image_desc dst_desc = {};
                            gpu_image_get_desc(g_gpu, entry->image, &dst_desc);
                            if ((dst_desc.usage & k_gpu_image_storage) != 0 && dst_desc.format == source->host_format &&
                                dst_desc.view_type == k_gpu_view_2d && dst_desc.mip_levels == 1 &&
                                dst_desc.array_layers == 1)
                                direct_scaled = scaled_resolve_into(source, entry->image, bias, x0 * n, y0 * n, hw, hh,
                                                                    clamp, low, high);
                        }
                        if (!direct_scaled) {
                            s_gpu_image *from =
                                bias != 0 || clamp
                                    ? scaled_resolve_source(source, bias, x0 * n, y0 * n, hw, hh, clamp, low, high)
                                    : 0;
                            gpu_cmd_copy_image(g_gpu, from != 0 ? from : pixels, x0 * n, y0 * n, hw, hh,
                                               entry->image, dest_level, dest_slice < layers ? dest_slice : 0u,
                                               (int)dest_x * n, (int)dest_y * n, hw, hh);
                            if (fill != 0) {
                                s_gpu_image *src = from != 0 ? from : pixels;
                                const uint32_t layer = dest_slice < layers ? dest_slice : 0u;
                                const int f = (int)fill, sx = x0 * n, sy = y0 * n, dx = (int)dest_x * n, dy = (int)dest_y * n;
                                gpu_cmd_copy_image(g_gpu, src, sx + f, sy, 1, hh, entry->image, dest_level, layer, dx, dy,
                                                   fill, hh);
                                gpu_cmd_copy_image(g_gpu, src, sx, sy + f, hw, 1, entry->image, dest_level, layer, dx, dy,
                                                   hw, fill);
                                gpu_cmd_copy_image(g_gpu, src, sx + f, sy + f, 1, 1, entry->image, dest_level, layer, dx,
                                                   dy, fill, fill);
                            }
                            if (source->snapshot_texture) color_snapshot_sampleable(pixels);
                        }
                    }
                    note.index = g_resolve_dump_index;
                    note.width = hw;
                    note.height = hh;
                    note.host_format = entry->host_format;
                    note.linear_storage = entry->linear_storage;
                    queue_resolve_dump(note, entry->image, dest_level, dest_slice < layers ? dest_slice : 0u,
                                       dest_x * ss, dest_y * ss);
                    noted = true;
                    g_resolve_dump_index++;
                    if (entry->cpu_read)
                        record_readback(entry, dest_level, dest_slice < layers ? dest_slice : 0u, dest_x, dest_y, w, h);
                }
                if (g_frame_resolve_count < sizeof(g_frame_resolves) / sizeof(g_frame_resolves[0]))
                    g_frame_resolves[g_frame_resolve_count++] = entry->image;
                if (!gpu_rendering(g_gpu, 0, 0))
                    clear_pass_attachments();
            }
        }
    } else if (destination != 0) {
        report(0x72736c76u, "a resolve without a bound source surface");
    }
    if (!noted) // a depth resolve, an empty rectangle, a resolve without source or destination: listed only
        queue_resolve_dump(note, 0, 0, 0, 0, 0);
    const bool clears_color = (flags & 0x100u) != 0 && (flags & 0x4u) == 0 && source_index < 4;
    if ((flags & 0x300u) != 0) {
        const uint32_t clear_slots = attached_color_slots(clears_color ? 1u << source_index : 0u);
        const bool clears_depth = (flags & 0x200u) != 0;
        const bool full_color = clears_color && source != 0 && source->image != 0 &&
                                (clear_slots & (1u << source_index)) != 0 &&
                                rect[0] <= 0 && rect[1] <= 0 &&
                                rect[2] >= (int)source->width && rect[3] >= (int)source->height;
        const bool full_depth = clears_depth && g_bound_depth != 0 && g_bound_depth->image != 0 &&
                                rect[0] <= 0 && rect[1] <= 0 &&
                                rect[2] >= (int)g_bound_depth->width && rect[3] >= (int)g_bound_depth->height;
        if (!gpu_rendering(g_gpu, 0, 0) &&
            (!clears_color || full_color) && (!clears_depth || full_depth) &&
            (full_color || full_depth)) {
            if (full_color) {
                edram_discard_contents(source);
                source->pending_clear_flags = k_gpu_clear_color;
                if (clear_color != 0)
                    memcpy(source->pending_clear_color, clear_color, sizeof(source->pending_clear_color));
                else
                    memset(source->pending_clear_color, 0, sizeof(source->pending_clear_color));
                edram_written(source);
            }
            if (full_depth) {
                edram_discard_contents(g_bound_depth);
                g_bound_depth->pending_clear_flags = k_gpu_clear_depth | k_gpu_clear_stencil;
                g_bound_depth->pending_clear_depth = clear_z;
                g_bound_depth->pending_clear_stencil = clear_stencil;
                edram_written(g_bound_depth);
            }
        } else {
            if (full_color)
                edram_discard_contents(source);
            if (full_depth)
                edram_discard_contents(g_bound_depth);
            if (ensure_pass(clears_depth, clear_slots)) {
                s_gpu_clear clear = {};
                if (clears_color) {
                    // the resolved surface only, as the Xenos copy clears it (a depth resolve clears no color)
                    clear.flags |= k_gpu_clear_color;
                    clear.color_mask = 1u << source_index;
                    if (clear_color != 0)
                        memcpy(clear.color, clear_color, sizeof(clear.color));
                }
                if (clears_depth)
                    clear.flags |= k_gpu_clear_depth | k_gpu_clear_stencil;
                clear.depth = clear_z;
                clear.stencil = clear_stencil;
                const uint32_t cs = bound_res_scale();
                int32_t host_rect[4];
                for (int i = 0; i < 4; i++)
                    host_rect[i] = rect[i] * (int32_t)cs; // HALO3_RES_SCALE
                gpu_cmd_clear_attachments(g_gpu, &clear, host_rect, 1);
            }
        }
    }
}

// Writes what the entry's last readback holds into the texture's memory, laid out as its header says (tiled or linear,
// each texel in host order: the order the CPU reads it in, as the resolve's endian swap gives the Xbox's CPU).
static void write_back(const s_texture_entry *entry, const s_port_d3d_texture_source &source)
{
    s_xenos_texture_layout layout;
    uint32_t block_width = 0, block_height = 0, bytes = 0;
    if (!xenos_texture_get_layout(&source.desc, &layout) || layout.block_width != 1 || layout.block_height != 1 ||
        !gpu_format_block_info(entry->host_format, &block_width, &block_height, &bytes) ||
        bytes != layout.bytes_per_block || entry->readback_level >= 16) {
        report(0x6c6f636bu ^ entry->host_format, "a CPU read of a resolve whose texels differ in size from the texture's "
                                              "(not written back)");
        return;
    }
    const s_xenos_level_layout &level = layout.levels[entry->readback_level];
    uint8_t *memory = (uint8_t *)(level.from_mip_address ? source.mip : source.base);
    if (memory == 0)
        return;
    memory += level.offset + entry->readback_layer * level.slice_stride;
    const bool tiled = xenos_d3dformat_tiled(source.desc.d3d_format) != 0;
    const uint32_t pitch_blocks = level.row_pitch_bytes / bytes;
    const uint32_t log2_bytes = bytes == 1 ? 0u : bytes == 2 ? 1u : bytes == 4 ? 2u : bytes == 8 ? 3u : 4u;
    gpu_buffer_invalidate(g_gpu, entry->readback);
    const uint8_t *texels = (const uint8_t *)gpu_buffer_mapped(g_gpu, entry->readback);
    if (texels == 0)
        return;
    for (uint32_t y = 0; y < entry->readback_h; y++) {
        for (uint32_t x = 0; x < entry->readback_w; x++) {
            const uint32_t px = entry->readback_x + x + level.packed_x_blocks;
            const uint32_t py = entry->readback_y + y + level.packed_y_blocks;
            const uint32_t offset = tiled ? xenos_tiled_offset_2d(px, py, pitch_blocks, log2_bytes)
                                          : py * level.row_pitch_bytes + px * bytes;
            memcpy(memory + offset, texels + ((size_t)y * entry->readback_w + x) * bytes, bytes);
        }
    }
}

void PORT_MS_ABI port_vk_texture_lock_mode(const void *texture, uint32_t level, uint32_t layer, int read_only)
{
    if (record_remote()) {
        s_record_client client;
        if (!client.admitted) return;
        s_port_d3d_texture_source source = {};
        if (!texture || !port_d3d_texture_get_source(texture, &source)) return;
        s_record_range ranges[2];
        record_texture_ranges(source, ranges);
        if (record_gpu_write(ranges[0], texture) || (ranges[1].base && record_gpu_write(ranges[1], nullptr))) {
            record_drop_export_hints();
            record_sync([&] { port_vk_texture_lock(texture, level, layer); });
            record_finish_export_invalidation();
        } else if (!read_only) {
            for (const auto &r : ranges) record_wait_reads(r, "tex-lock");
        }
        return;
    }
    if (g_started <= 0 || texture == 0)
        return;
    s_port_d3d_texture_source source = {};
    if (!port_d3d_texture_get_source(texture, &source))
        return;
    if (g_xenos)
        xe_export_lock(source, level, layer); // what memory exports wrote into it
    s_texture_entry *entry = resolved_entry(texture, source);
    if (entry == 0)
        return; // the memory holds what the CPU wrote
    if (!entry->cpu_read) {
        entry->cpu_read = 1; // resolves into it are read back from now on
        // and this first read gets the resolve the image already holds: the whole level copied back now and waited
        // for (once per resolved memory), as the console's memory would hold it
        if (level >= 16 || level >= source.desc.levels || !begin_frame())
            return;
        materialize_pending_7e3(entry);
        const uint32_t w = source.desc.width >> level, h = source.desc.height >> level;
        record_readback(entry, level, layer, 0, 0, w != 0 ? w : 1u, h != 0 ? h : 1u);
        if (entry->readback_serial != g_frame_serial)
            return; // not a format the CPU can read back (record_readback)
        finish_frame();
        write_back(entry, source);
        return;
    }
    if (entry->readback_serial == 0 || entry->readback_level != level || entry->readback_layer != layer)
        return;
    // the frame that copied it: finished once its slot was begun again; otherwise wait for it, unless it is the frame
    // being recorded (the memory then keeps the previous resolve's texels, as a console read before the GPU got there)
    if (entry->readback_serial + g_gpu->caps.frames_in_flight > g_frame_serial &&
        gpu_frame_wait(g_gpu, entry->readback_slot) != k_gpu_ok)
        return;
    write_back(entry, source);
}

void PORT_MS_ABI port_vk_texture_lock(const void *texture, uint32_t level, uint32_t layer)
{
    port_vk_texture_lock_mode(texture, level, layer, 0);
}

// The display image for a source of this size (made on first use, kept: the front buffer's size does not change).
static s_gpu_render_target *display_target(uint32_t width, uint32_t height)
{
    for (s_display_target &display : g_display_targets) {
        if (display.target != 0 && display.width == width && display.height == height)
            return display.target;
    }
    for (s_display_target &display : g_display_targets) {
        if (display.target != 0)
            continue;
        display.target = gpu_render_target_create(g_gpu, width, height, k_gpu_format_r8g8b8a8_unorm,
                                                  k_gpu_render_target_storage);
        if (display.target == 0)
            return 0;
        display.width = width;
        display.height = height;
        return display.target;
    }
    report(0x64697370u, "more than 4 front buffer sizes: frames of a fifth size are not presented");
    return 0;
}

void PORT_MS_ABI port_vk_display_gamma_query(uint32_t *type, float *power)
{
    *type = 2;
    *power = 2.22222233f;
    if (const char *v = getenv("HALO3_DISPLAY_GAMMA")) {
        if (*v >= '0' && *v <= '9')
            *type = (uint32_t)(*v - '0');
        // ",<power>": a decimal number (the Windows CRT the device links has no atof)
        const char *c = v;
        while (*c != 0 && *c != ',')
            c++;
        if (*c == ',') {
            double value = 0.0, scale = 1.0;
            bool fraction = false;
            for (c++; (*c >= '0' && *c <= '9') || (*c == '.' && !fraction); c++) {
                if (*c == '.')
                    fraction = true;
                else if (fraction)
                    value += (double)(*c - '0') * (scale *= 0.1);
                else
                    value = value * 10.0 + (double)(*c - '0');
            }
            if (value > 0.0)
                *power = (float)value;
        }
    }
}

void PORT_MS_ABI port_vk_set_display_gamma(uint32_t mode, const uint16_t *words)
{
    if (record_remote()) {
        if (!record_owner())
            return record_sync([&] { port_vk_set_display_gamma(mode, words); });
        const uint16_t *copy = nullptr;
        if (words && (mode == k_port_vk_display_gamma_ramp || mode == k_port_vk_display_gamma_pwl))
            copy = (uint16_t *)memcpy(record_reserve(768 * sizeof(uint16_t)), words, 768 * sizeof(uint16_t));
        return record_post([=] { port_vk_set_display_gamma(mode, copy); });
    }
    uint32_t table[4u * 257u] = {};
    if (words != 0 && mode == k_port_vk_display_gamma_ramp) {
        for (uint32_t i = 0; i < 256; i++) {
            for (uint32_t c = 0; c < 3; c++)
                table[4u + i * 4u + c] = words[c * 256u + i];
        }
    } else if (words != 0 && mode == k_port_vk_display_gamma_pwl) {
        for (uint32_t i = 0; i < 128; i++) {
            for (uint32_t c = 0; c < 3; c++) {
                const uint16_t *entry = words + (c * 128u + i) * 2u; // D3DPWLGAMMAENTRY { Base, Delta }
                table[4u + i * 4u + c] = (uint32_t)entry[0] | (uint32_t)entry[1] << 16;
            }
        }
    } else {
        mode = k_port_vk_display_gamma_none;
    }
    table[0] = mode;
    memcpy(g_display_gamma, table, sizeof(table));
}

static void pump_present_window()
{
    if (g_window && !g_window_closed && !port_window_pump()) {
        g_window_closed = 1;
        device_log_text("the window was closed; exiting\n");
        port_window_close();
        port_exit_flush();
        port_exit(0);
    }
}

// Pixel dimensions of the image fitted to the client area (including internal resolution scaling).
static uint32_t g_presented_extent;

void PORT_MS_ABI port_vk_presented_extent(int *width, int *height)
{
    const uint32_t extent = __atomic_load_n(&g_presented_extent, __ATOMIC_RELAXED);
    if (extent) {
        *width = (int)(extent & 0xffffu);
        *height = (int)(extent >> 16);
    }
}

void PORT_MS_ABI port_vk_present(const void *front_buffer)
{
    if (record_remote()) {
        pump_present_window();
        if (record_owner()) {
            if (g_record.prev_present_seq)
                record_wait(g_record.prev_present_seq, "prev-present");
            const auto cmd_fn = [front_buffer] { port_vk_present(front_buffer); };
            const uint64_t seq = record_publish(record_command(cmd_fn), sizeof(cmd_fn));
            g_record.prev_present_seq = seq;
            {
                s_record_resource_guard guard;
                g_record.slab_index ^= 1u;
                g_record.slab = g_record.slabs[g_record.slab_index];
                g_record.used = 0;
                g_record.texture_reads = nullptr;
            }
            if (g_record.trace) record_trace("submit-post", seq, 0, 0, frame_stats_now());
            return;
        }
        record_wait(record_head(), "present-drain");
        record_sync([&] { port_vk_present(front_buffer); });
        if (g_record.trace) record_trace("submit-ack", record_head(), 0, 0, frame_stats_now());
        return;
    }
    if (g_started <= 0 || !begin_frame())
        return;
    checkpoint_record(3);
    frame_stats_present_begin(g_frame_stats_window != 0 ? frame_stats_now() : 0);
    frame_trace_present(g_frame_trace.on ? frame_stats_now() : 0);
    end_pass();
    for (uint32_t i = 0; i < g_targets.count; i++) {
        if (g_targets.items[i].back_buffer && g_targets.items[i].pending_clear_flags != 0)
            materialize_target_contents(&g_targets.items[i]);
    }
    // What the display scans out: the front buffer a Resolve wrote (Swap), or the back buffer (Present).
    s_gpu_image *scanout = gpu_render_target_color_image(g_gpu, g_back_buffer);
    bool gamma = true;
    bool fused_7e3 = false;
    if (front_buffer != 0) {
        s_port_d3d_texture_source source = {};
        s_texture_entry *front = port_d3d_texture_get_source(front_buffer, &source) ? resolved_entry(front_buffer, source)
                                                                                     : texture_entry(front_buffer, false);
        s_texture_entry chosen = {};
        if (g_present_resolve >= 0 && (uint32_t)g_present_resolve < g_frame_resolve_count) {
            chosen.image = g_frame_resolves[g_present_resolve];
            chosen.resolved = 1;
            front = &chosen;
            gamma = false; // an intermediate target, shown as it is
        }
        if (front != 0 && front->resolved && front->image != 0) {
            if (front != &chosen && front->pending_7e3_owner != 0 && front->pending_7e3_owner <= g_targets.count) {
                s_target_entry *owner = &g_targets.items[front->pending_7e3_owner - 1u];
                if (owner->image != 0 && owner->content == front->pending_7e3_content) {
                    scanout = target_read_image(owner);
                    fused_7e3 = true;
                }
                front->pending_7e3_owner = 0;
                front->pending_7e3_content = 0;
                if (g_pending_7e3_texture == (uint32_t)(front - g_textures.items) + 1u)
                    g_pending_7e3_texture = 0;
            }
            if (!fused_7e3)
                scanout = front->image;
        } else {
            report(0x66726f6eu, "D3DDevice_Swap of a front buffer no Resolve wrote: presenting the back buffer");
        }
    }
    s_gpu_image_desc scanout_desc;
    gpu_image_get_desc(g_gpu, scanout, &scanout_desc);
    s_gpu_render_target *display = display_target(scanout_desc.width, scanout_desc.height);
    uint32_t ramp_offset = 0;
    void *ramp = display != 0 ? ring_alloc(GPU_DISPLAY_GAMMA_RAMP_SIZE, g_uniform_alignment, &ramp_offset) : 0;
    if (ramp != 0) {
        memcpy(ramp, g_display_gamma, GPU_DISPLAY_GAMMA_RAMP_SIZE);
        if (!gamma)
            *(uint32_t *)ramp = k_port_vk_display_gamma_none;
        ((uint32_t *)ramp)[1] = fused_7e3 ? 1u : 0u;
        s_gpu_image *display_image = gpu_render_target_color_image(g_gpu, display);
        s_gpu_buffer *ramp_buffer = ring_buffer();
        xe_layout_call_once(g_vk_display_gamma_initialized, scanout != 0 && display_image != 0 && ramp_buffer != 0,
                            gpu_cmd_apply_display_gamma, g_gpu, scanout, display_image, ramp_buffer, ramp_offset);
    } else {
        display = 0;
    }
    const bool capture = g_dump_directory != 0 && display != 0 && g_presented % g_dump_every == 0;
    if (capture)
        gpu_cmd_readback(g_gpu, display);
    bool acquired = false;
    if (!g_record_worker) pump_present_window();
    if (g_window && !g_window_closed) {
        // a live setting changed (Guide > Settings): the swapchain again when it asks for the other present mode
        const unsigned long revision = port_config_revision();
        if (revision != g_swapchain_revision) {
            g_swapchain_revision = revision;
            if (swapchain_fifo() != g_swapchain_fifo)
                create_swapchain();
        }
        gpu_result result = g_window ? gpu_swapchain_acquire(g_gpu, 0) : k_gpu_surface_lost;
        if (result == k_gpu_out_of_date) {
            create_swapchain();
            result = g_window ? gpu_swapchain_acquire(g_gpu, 0) : result;
        }
        acquired = result == k_gpu_ok || result == k_gpu_suboptimal;
        if (acquired) {
            const uint32_t w = display ? scanout_desc.width : g_back_buffer_width * g_res_scale;
            const uint32_t h = display ? scanout_desc.height : g_back_buffer_height * g_res_scale;
            __atomic_store_n(&g_presented_extent, (h << 16) | w, __ATOMIC_RELAXED);
            gpu_cmd_blit_to_swapchain(g_gpu, display != 0 ? display : g_back_buffer);
        }
    }
    const long long submit_start = g_frame_stats_window != 0 ? frame_stats_now() : 0;
#ifdef HALO3_64BIT
    xe_readback_prefetch();
    if (gpu_frame_end(g_gpu, capture || g_resolve_dump_count != 0 ? 1u : 0u) != k_gpu_ok)
        xe_readback_prefetch_failed();
#else
    gpu_frame_end(g_gpu, capture || g_resolve_dump_count != 0 ? 1u : 0u);
#endif
    g_frame_open = 0;
    if (acquired) {
        gpu_result result = gpu_swapchain_present(g_gpu);
        if (result == k_gpu_out_of_date || result == k_gpu_suboptimal)
            create_swapchain();
    }
    if (g_frame_stats_window != 0)
        g_frame_stats.submit_ms += frame_stats_ms(submit_start, frame_stats_now());
    if (capture) {
        s_gpu_render_target_info info;
        gpu_render_target_get_info(g_gpu, display, &info);
        const uint8_t *pixels = (const uint8_t *)gpu_render_target_read(g_gpu, display);
        if (pixels != 0) {
            const uint32_t bytes = info.width * info.height * 4u;
            if (g_last_frame == 0 || g_last_width != info.width || g_last_height != info.height) {
                free(g_last_frame);
                g_last_frame = (uint8_t *)malloc(bytes);
            }
            if (g_last_frame != 0) {
                // kept as B8G8R8A8 (port_vk_last_frame)
                for (uint32_t y = 0; y < info.height; y++) {
                    const uint8_t *row = pixels + (size_t)y * info.readback_row_pitch;
                    uint8_t *out = g_last_frame + (size_t)y * info.width * 4u;
                    for (uint32_t x = 0; x < info.width; x++) {
                        out[x * 4u + 0] = row[x * 4u + 2];
                        out[x * 4u + 1] = row[x * 4u + 1];
                        out[x * 4u + 2] = row[x * 4u + 0];
                        out[x * 4u + 3] = row[x * 4u + 3];
                    }
                }
                g_last_width = info.width;
                g_last_height = info.height;
                dump_frame(g_last_frame, info.width, info.height);
            }
        }
    }
    if (g_resolve_dump_count != 0)
        write_resolve_dumps(g_presented);
    g_resolve_dump_index = 0;
    g_resolve_sequence = 0;
    __atomic_add_fetch(&g_presented, 1u, __ATOMIC_RELEASE);
    if (g_xenos)
        prewarm_frame(); // optimized pipelines from the workers swapped in
    pipeline_cache_present();
    g_frame_resolve_count = 0;
}

uint32_t PORT_MS_ABI port_vk_frame_count(void)
{
    return __atomic_load_n(&g_presented, __ATOMIC_ACQUIRE);
}

// HALO3_FPS, parsed once: 30 (default), 31..1000, or 0 (uncapped); 0xffffffff until parsed. Unless the launch set
// HALO3_FPS, halo3.ini's fps_cap then follows the Guide's Settings page live (port_config_fps_cap): the vertical blanks,
// the frame blanks and the interpolation below all follow the rate as it is now.
uint32_t g_frame_rate = 0xffffffffu;
unsigned long g_frame_rate_revision; // port_config_revision when g_frame_rate was decided

uint32_t PORT_MS_ABI port_vk_frame_rate(void)
{
    uint32_t rate = __atomic_load_n(&g_frame_rate, __ATOMIC_ACQUIRE);
    const unsigned long revision = port_config_revision();
    if (rate == 0xffffffffu) {
        rate = 30u;
        if (const char *text = getenv("HALO3_FPS")) {
            uint32_t n = 0;
            const char *c = text;
            for (; *c >= '0' && *c <= '9' && n <= 100000u; c++)
                n = n * 10u + (uint32_t)(*c - '0');
            if (c != text && *c == 0 && (n == 0 || (n >= 30u && n <= 1000u)))
                rate = n;
            else
                log_text("HALO3_FPS is not 0 (uncapped) or 30..1000; keeping 30\n");
        }
        __atomic_store_n(&g_frame_rate_revision, revision, __ATOMIC_RELEASE);
        __atomic_store_n(&g_frame_rate, rate, __ATOMIC_RELEASE);
    } else if (revision != __atomic_load_n(&g_frame_rate_revision, __ATOMIC_ACQUIRE) &&
               !port_config_locked(PORT_SETTING_FPS_CAP)) {
        // one thread at a time (the vertical blank thread, the game's, the presenting one all ask): the revision read
        // again under the lock, before the cap, so a rate decided from an older revision never lands after a newer one
        static volatile char deciding;
        while (__atomic_test_and_set(&deciding, __ATOMIC_ACQUIRE))
            __builtin_ia32_pause();
        const unsigned long current = port_config_revision();
        if (current == __atomic_load_n(&g_frame_rate_revision, __ATOMIC_ACQUIRE)) { // another thread decided it
            rate = __atomic_load_n(&g_frame_rate, __ATOMIC_ACQUIRE);
            __atomic_clear(&deciding, __ATOMIC_RELEASE);
            return rate;
        }
        rate = (uint32_t)port_config_fps_cap();
        const uint32_t before = __atomic_exchange_n(&g_frame_rate, rate, __ATOMIC_ACQ_REL);
        __atomic_store_n(&g_frame_rate_revision, current, __ATOMIC_RELEASE);
        __atomic_clear(&deciding, __ATOMIC_RELEASE);
        if (before != rate) {
            char digits[8] = "0";
            for (uint32_t n = rate, i = n >= 1000u ? 4u : n >= 100u ? 3u : n >= 10u ? 2u : 1u; i > 0; n /= 10u)
                digits[--i] = (char)('0' + n % 10u);
            log_text("frame rate cap now ");
            port_write_error(rate == 0u ? "off" : digits);
            port_write_error(" (halo3.ini fps_cap)\n");
        }
    }
    return rate;
}

uint32_t PORT_MS_ABI port_vk_vertical_blank_rate(void)
{
    const uint32_t rate = port_vk_frame_rate();
    return rate == 30u ? 60u : rate == 0u ? 1000u : rate;
}

uint32_t PORT_MS_ABI port_vk_frame_interpolation(void)
{
    static int off = -1; // HALO3_INTERPOLATE=0, read once; -1: not read yet
    if (off < 0) {
        const char *text = getenv("HALO3_INTERPOLATE");
        off = text != 0 && text[0] == '0' && text[1] == 0 ? 1 : 0;
    }
    return port_vk_frame_rate() != 30u && !off ? 1u : 0u;
}

uint32_t PORT_MS_ABI port_vk_frame_blanks(void)
{
    static uint32_t forced = 0xffffffffu; // HALO3_FRAME_BLANKS 1..4 (0: not set), read once
    if (forced == 0xffffffffu) {
        const char *text = getenv("HALO3_FRAME_BLANKS");
        forced = text != 0 && text[0] >= '1' && text[0] <= '4' && text[1] == 0 ? (uint32_t)(text[0] - '0') : 0u;
    }
    return forced != 0u ? forced : port_vk_frame_rate() == 30u ? 2u : 1u;
}

const void * PORT_MS_ABI port_vk_last_frame(uint32_t *out_width, uint32_t *out_height)
{
    if (record_remote()) {
        return record_result([&] { return port_vk_last_frame(out_width, out_height); });
    }
    if (out_width != 0)
        *out_width = g_last_width;
    if (out_height != 0)
        *out_height = g_last_height;
    return g_last_frame;
}

// ---- explicit compute shaders ----
//
// The Xbox runs the particle, contrail, beam, light volume and ripple updates as memexport vertex shaders; the pack
// has Bungie's DX11 compute shaders for them (rasterizer_globals explicit slots, stage compute). Their bindings
// follow the pack's register spaces: b# cbuffers (gathered from the vertex shader register shadows, where the Xbox
// updates read their constants, then the caller's values for PC-only members such as the *_index_range of each
// update), t# structured buffers at 100 + #, u# read-write structured buffers at 300 + #. The buffers are
// port_vulkan_storage objects (device-local, kept across frames).

} // extern "C"

struct s_port_vk_storage {
    s_gpu_buffer *buffer;
    uint32_t size;
};

namespace {

// Ends and submits the frame being recorded and waits for it (a sync point for reading GPU results back).
gpu_result finish_frame_wait()
{
    if (!g_frame_open)
        return k_gpu_ok;
    end_pass();
    const gpu_result result = gpu_frame_end(g_gpu, 1u);
    g_frame_open = 0;
    return result;
}

void finish_frame()
{
    finish_frame_wait();
}

s_gpu_pipeline *compute_pipeline_for(uint32_t shader_index)
{
    for (uint32_t i = 0; i < g_compute_pipelines.count; i++)
        if (g_compute_pipelines.items[i].shader_index == shader_index)
            return g_compute_pipelines.items[i].pipeline;
    s_gpu_pipeline *pipeline = xe_layout_call(gpu_pack_compute_pipeline, g_gpu, g_pack, shader_index);
    s_compute_entry *entry = g_compute_pipelines.push();
    if (entry != 0) {
        entry->shader_index = shader_index;
        entry->pipeline = pipeline;
    }
    if (pipeline == 0)
        report(shader_index ^ 0x63706970u, "compute pipeline creation failed (its dispatches are skipped)");
    return pipeline;
}

// ---- memory export: storage of exported buffers, compute dispatches of memexport draws ----

s_export_buffer *export_buffer_find(const void *base)
{
    uint32_t index;
    if (base == 0 || !map_find(&g_export_index, key_of(base), &index) || index >= g_export_buffers.count)
        return 0;
    return &g_export_buffers.items[index];
}

s_gpu_buffer *storage_buffer_of(const s_port_vk_storage *storage, uint32_t *out_size)
{
    if (storage == 0)
        return 0;
    *out_size = storage->size;
    return storage->buffer;
}

// The storage of an exported guest buffer, made (zero-filled) at its first export: one record of `stride` bytes per
// Xbox record of `xbox_stride` bytes (16, the smallest GPU-effect record, when the export's stream stride is not
// known: never fewer records than the guest buffer holds).
s_export_buffer *export_buffer_for(const void *base, uint32_t guest_size, uint32_t xbox_stride, uint32_t stride)
{
    if (base == 0 || guest_size == 0 || stride == 0)
        return 0;
    const uint32_t records = guest_size / (xbox_stride != 0 ? xbox_stride : 16u);
    s_export_buffer *entry = export_buffer_find(base);
    // a known record size sizes the storage; without one (a spawn: the export target is not one of its streams) the
    // storage an update already sized is kept
    if (entry != 0 && entry->stride == stride && entry->storage != 0 && (xbox_stride == 0 || entry->records >= records))
        return entry;
    if (entry != 0 && entry->storage != 0) {
        // the game reused the memory for another effect type, or a bigger guest buffer: its records do not carry over
        report(key_of(base) ^ 0x72656d6bu, "an exported buffer changed its record layout (its storage is re-created)");
        port_vulkan_storage_destroy(entry->storage);
        entry->storage = 0;
    }
    if (entry == 0) {
        entry = g_export_buffers.push();
        if (entry == 0)
            return 0;
        map_set(&g_export_index, key_of(base), g_export_buffers.count - 1u);
    }
    entry->base = base;
    entry->guest_size = guest_size;
    entry->stride = stride;
    entry->records = records != 0 ? records : 1u;
    entry->storage = port_vulkan_storage_create(entry->records * stride, 0);
    if (entry->storage == 0)
        report(key_of(base) ^ 0x7374636bu, "export storage creation failed");
    return entry->storage != 0 ? entry : 0;
}

// The workgroup size of a compute module (OpExecutionMode (16) <entry> LocalSize (17) x y z); 64 when absent.
uint32_t compute_local_size_x(uint32_t shader_index)
{
    s_shader_pack_shader_info info;
    size_t size = 0;
    const uint32_t *code = shader_pack_get_shader(g_pack, shader_index, &info)
                               ? shader_pack_module_code(g_pack, info.module_index, &size) : 0;
    const size_t words = code != 0 ? size / 4u : 0;
    for (size_t w = 5; w < words;) {
        const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
        if (length == 0 || w + length > words)
            break;
        if (opcode == 16u && length >= 6u && code[w + 2] == 17u)
            return code[w + 3] != 0 ? code[w + 3] : 64u;
        w += length;
    }
    return 64u;
}

const s_xenos_d3d_vertex_element *find_element(const s_xenos_vertex_declaration *declaration, uint32_t usage,
                                               uint32_t usage_index)
{
    for (uint32_t e = 0; e < declaration->element_count; e++)
        if (declaration->elements[e].usage == usage && declaration->elements[e].usage_index == usage_index)
            return &declaration->elements[e];
    return 0;
}

// One vertex of a draw's stream in host order (each element swapped in its GPUENDIAN unit when the buffer is in
// Xenos order), `stride` bytes into dst.
void read_vertex(const s_port_vk_stream &stream, const s_xenos_vertex_declaration *declaration, uint32_t s,
                 uint32_t vertex, uint8_t *dst)
{
    memcpy(dst, vertex_bytes(stream, s, stream.offset + size_t(vertex) * stream.stride), stream.stride);
    if (!stream.big_endian)
        return;
    for (uint32_t e = 0; e < declaration->element_count; e++) {
        const s_xenos_d3d_vertex_element &el = declaration->elements[e];
        if (el.stream == s && el.offset < stream.stride)
            port_geometry_swap_element(el.type, dst + el.offset);
    }
}

// The packed spawn address of DX11 particle_spawn / contrail_spawn (StructuredBuffer<uint> t4: x | y << 16, the
// cs shader's out_index = x + y * 16) from the Xbox vertex's float2 address : TEXCOORD1 (hlsl_vertex_types.fx).
bool element_address(const s_xenos_d3d_vertex_element &el, const uint8_t *vertex, uint32_t *out)
{
    const uint8_t *p = vertex + el.offset;
    uint32_t x, y;
    switch (el.type) {
    case k_xenos_decltype_float2: {
        float f[2];
        memcpy(f, p, 8);
        x = (uint32_t)(f[0] + 0.5f);
        y = (uint32_t)(f[1] + 0.5f);
        break;
    }
    case k_xenos_decltype_short2:
    case k_xenos_decltype_ushort2: {
        uint16_t h[2];
        memcpy(h, p, 4);
        x = h[0];
        y = h[1];
        break;
    }
    case k_xenos_decltype_float16_2: {
        uint16_t h[2];
        memcpy(h, p, 4);
        x = (uint32_t)(half_to_float(h[0]) + 0.5f);
        y = (uint32_t)(half_to_float(h[1]) + 0.5f);
        break;
    }
    default:
        return false;
    }
    *out = (x & 0xffffu) | (y << 16);
    return true;
}

// A StructuredBuffer input of a memexport compute shader from the CPU-written vertex streams of its draw (the spawn
// queues: t4 the addresses, t5 the records), vertices [start, start + count) as elements 0..count-1, in the frame ring.
// Records keep the Xbox vertex's bytes: the DX11 s_raw_* structs list the Xbox memexport fields in the same order and
// formats (particle: pos 4x32f, vel 4x16f, rnd / rnd2 / rot 4x16un, time 4x16f, anm 2x16f, anm2 2x16un, axis dec3n,
// col / col2 argb8: 76 bytes, then the struct's 4 bytes of padding).
bool upload_spawn_input(s_draw_geometry draw, const s_xenos_vertex_declaration *declaration,
                        const s_shader_pack_binding &binding, uint32_t start, uint32_t count, uint32_t *out_offset,
                        uint32_t *out_size)
{
    const s_xenos_d3d_vertex_element *address = find_element(declaration, k_xenos_declusage_texcoord, 1);
    const bool want_address = binding.stride == 4u;
    if (want_address && address == 0)
        return false;
    // the address stream, or the first other stream the declaration reads
    uint32_t s = want_address ? address->stream : PORT_VK_MAX_STREAMS;
    for (uint32_t i = 0; i < PORT_VK_MAX_STREAMS && s == PORT_VK_MAX_STREAMS; i++)
        if ((declaration->stream_mask & (1u << i)) != 0 && (address == 0 || i != address->stream))
            s = i;
    if (s >= PORT_VK_MAX_STREAMS || (draw.stream_mask() & (1u << s)) == 0)
        return false;
    const s_port_vk_stream &stream = draw.stream(s);
    if (stream.data == 0 || stream.stride == 0 || stream.size < stream.offset ||
        (stream.size - stream.offset) / stream.stride < start + count)
        return false;
    const uint32_t size = count * binding.stride;
    uint8_t *dst = (uint8_t *)ring_alloc(size != 0 ? size : 4u, k_storage_alignment, out_offset);
    uint8_t vertex[256];
    if (dst == 0 || stream.stride > sizeof(vertex))
        return false;
    memset(dst, 0, size);
    for (uint32_t v = 0; v < count; v++) {
        read_vertex(stream, declaration, s, start + v, vertex);
        // Xenos float16 is extended range: exponent 31 is finite (up to 131008, xenia spirv_shader_translator.h), and the
        // CPU packs overflow as 0x7fff (vmx128.cpp float_to_xbox_half: a temporary particle's inverse lifespan 1e10,
        // particle_block.cpp). The DX11 shaders unpack with f16tof32, for which that is NaN (age += NaN * 0 on the first
        // update): such halves become the largest IEEE half
        for (uint32_t e = 0; e < declaration->element_count; e++) {
            const s_xenos_d3d_vertex_element &el = declaration->elements[e];
            const uint32_t halves = el.type == k_xenos_decltype_float16_2 ? 2u : el.type == k_xenos_decltype_float16_4 ? 4u : 0u;
            for (uint32_t h = 0; el.stream == s && h < halves && el.offset + 2u * h + 2u <= stream.stride; h++) {
                uint16_t half;
                memcpy(&half, vertex + el.offset + 2u * h, 2);
                if ((half & 0x7c00u) == 0x7c00u)
                    half = (uint16_t)((half & 0x8000u) | 0x7bffu);
                memcpy(vertex + el.offset + 2u * h, &half, 2);
            }
        }
        if (want_address) {
            uint32_t packed;
            if (!element_address(*address, vertex, &packed))
                return false;
            memcpy(dst + 4u * v, &packed, 4);
        } else {
            memcpy(dst + v * binding.stride, vertex, stream.stride < binding.stride ? stream.stride : binding.stride);
        }
    }
    *out_size = size;
    return true;
}

// Records one compute dispatch: cbuffers from the vertex register shadows (the cbuffer tables, then `micro`'s own
// gathers when a retail memexport shader runs it), PC-only members, `api` constants; storage buffers from `api`'s
// resources, else (a memexport draw) u# = the open export's storage and t# = the draw's exported streams or its
// CPU-written spawn streams; textures and samplers from the vertex sampler slots (`micro`).
bool record_compute(uint32_t shader_index, uint32_t groups_x, uint32_t groups_y, uint32_t groups_z,
                    const s_shader_pack_microcode *micro, s_draw_geometry draw, s_shader_entry *entry,
                    uint32_t start, uint32_t count, const s_port_vk_compute_dispatch *api)
{
    s_shader_pack_shader_info info;
    if (!shader_pack_get_shader(g_pack, shader_index, &info) || !begin_frame())
        return false;
    s_gpu_pipeline *pipeline = compute_pipeline_for(shader_index);
    if (pipeline == 0)
        return false;
    gpu_bind_group set = GPU_NULL_HANDLE;
    gpu_bind_layout layout = gpu_pipeline_bind_layout(g_gpu, pipeline, 0);
    if (info.binding_count != 0 && layout != GPU_NULL_HANDLE) {
        set = gpu_bind_group_allocate(g_gpu, layout, k_gpu_bind_frame);
        if (set == GPU_NULL_HANDLE)
            return false;
    }
    const s_xenos_vertex_declaration *declaration = 0;
    if (draw.packet != 0) {
        uint32_t declaration_hash;
        declaration = declaration_for(draw.declaration(), &declaration_hash);
    }
    static const float k_zero_floats[1024] = {};
    static const uint32_t k_zero_dwords[32] = {};
    const float *floats = g_shadows.vertex_float != 0 ? g_shadows.vertex_float : k_zero_floats;
    const uint32_t *bools = g_shadows.vertex_bool != 0 ? g_shadows.vertex_bool : k_zero_dwords;
    const uint32_t *ints = g_shadows.vertex_int != 0 ? g_shadows.vertex_int : k_zero_dwords;
    for (uint32_t b = 0; b < info.binding_count; b++) {
        s_shader_pack_binding binding;
        if (!shader_pack_get_binding(g_pack, shader_index, b, &binding))
            continue;
        switch (binding.descriptor_type) {
        case k_shader_pack_descriptor_uniform_buffer: {
            s_shader_pack_cbuffer cbuffer;
            const bool known = shader_pack_find_cbuffer(g_pack, binding.name_hash, &cbuffer) != 0;
            const uint32_t size = known && cbuffer.size != 0 ? cbuffer.size : k_unknown_cbuffer_size;
            uint32_t offset;
            uint8_t *data = (uint8_t *)ring_alloc(size, g_uniform_alignment, &offset);
            if (data == 0)
                return false;
            memset(data, 0, size);
            if (known)
                shader_pack_apply_gathers(cbuffer.gathers, cbuffer.gather_count, cbuffer.table_index, data, size,
                                          floats, bools, ints);
            if (known && micro != 0 && micro->gather_count != 0)
                shader_pack_apply_gathers(micro->gathers, micro->gather_count, cbuffer.table_index, data, size, floats,
                                          bools, ints);
            if (entry != 0 && draw.packet != 0)
                write_pc_members(entry, binding.binding, draw.packet, data, size);
            for (uint32_t c = 0; api != 0 && c < api->constant_count; c++) {
                const s_port_vk_compute_constants &constants = api->constants[c];
                if (constants.name_hash == binding.name_hash && constants.data != 0 &&
                    constants.offset <= size && constants.size <= size - constants.offset)
                    memcpy(data + constants.offset, constants.data, constants.size);
            }
            gpu_bind_write_buffer(g_gpu, set, binding.binding, 0, k_gpu_binding_uniform_buffer,
                                  ring_buffer(), offset, size);
            break;
        }
        case k_shader_pack_descriptor_storage_buffer: {
            s_gpu_buffer *buffer = 0;
            uint32_t offset = 0, range = 0;
            for (uint32_t r = 0; api != 0 && r < api->resource_count && buffer == 0; r++) {
                if (api->resources[r].binding == binding.binding && api->resources[r].storage != 0) {
                    buffer = api->resources[r].storage->buffer;
                    offset = api->resources[r].offset;
                    range = api->resources[r].range != 0 ? api->resources[r].range
                                                         : api->resources[r].storage->size - offset;
                }
            }
            if (buffer == 0 && draw.packet != 0 && binding.binding >= 300u) {
                // u#: the memory this draw exports into
                if (s_export_buffer *target = export_buffer_for(g_export.base, g_export.size, g_export.xbox_stride,
                                                                binding.stride))
                    buffer = storage_buffer_of(target->storage, &range);
            } else if (buffer == 0 && draw.packet != 0) {
                // t#: a stream an earlier export wrote, else the CPU-written spawn queue
                for (uint32_t s = 0; s < PORT_VK_MAX_STREAMS && buffer == 0; s++) {
                    const s_export_buffer *source = (draw.stream_mask() & (1u << s)) != 0
                                                        ? export_buffer_find(draw.stream(s).data) : 0;
                    if (source != 0 && source->base != g_export.base && source->stride == binding.stride)
                        buffer = storage_buffer_of(source->storage, &range);
                }
                if (buffer == 0 && declaration != 0 &&
                    upload_spawn_input(draw, declaration, binding, start, count, &offset, &range))
                    buffer = ring_buffer();
            }
            if (buffer == 0) {
                report(binding.name_hash ^ 0x73746f72u, "a compute dispatch without its storage buffer (skipped)",
                       binding.name);
                return false;
            }
            gpu_bind_write_buffer(g_gpu, set, binding.binding, 0, k_gpu_binding_storage_buffer, buffer,
                                  offset, range);
            break;
        }
        case k_shader_pack_descriptor_sampled_image:
        case k_shader_pack_descriptor_sampler: {
            // particle_update's weather occlusion: an Xbox vertex sampler of the update shader
            const s_port_vk_sampler *slot = 0;
            for (uint32_t m = 0; micro != 0 && draw.packet != 0 && m < micro->sampler_count && slot == 0; m++)
                if (micro->samplers[m].binding_index == b)
                    slot = sampler_slot(draw.packet, PORT_VK_STAGE_VERTEX, micro->samplers[m].sampler);
            if (binding.descriptor_type == k_shader_pack_descriptor_sampler) {
                gpu_bind_write_sampler(g_gpu, set, binding.binding, 0,
                                       slot != 0 ? sampler_for(sampler_state_of(*slot)) : g_default_sampler);
            } else {
                gpu_view view = slot != 0 && slot->bound ? texture_view(&slot->texture, binding.image_dim) : GPU_NULL_HANDLE;
                gpu_bind_write_image(g_gpu, set, binding.binding, 0, k_gpu_binding_sampled_image,
                                     view != GPU_NULL_HANDLE ? view : dummy_view_for(binding));
            }
            break;
        }
        default:
            report(binding.descriptor_type * 0x1003u + 11u, "a compute shader binding the device cannot fill (skipped)",
                   binding.name);
            return false;
        }
    }
    // outside any rendering; draws begin their pass again afterwards
    end_pass();
    gpu_cmd_memory_barrier(g_gpu); // earlier draws and dispatches that wrote its buffers
    if (g_vk)
        vulkan_cmd_bind_pipeline_native(g_vk, vulkan_gpu_native_pipeline(pipeline));
    else
        gpu_cmd_bind_pipeline(g_gpu, pipeline);
    if (set != GPU_NULL_HANDLE)
        gpu_cmd_bind_group(g_gpu, pipeline, 0, set);
    if (g_vk)
        vulkan_cmd_dispatch_inline(g_vk, groups_x, groups_y, groups_z);
    else
        gpu_cmd_dispatch(g_gpu, groups_x, groups_y, groups_z);
    gpu_cmd_memory_barrier(g_gpu); // its writes, for the draws that read them
    return true;
}

// A draw with a memexport vertex shader (its pack shader is the slot's compute shader): Xbox vertex i of the point
// list [start, start + count) is DX11 thread i, index = i + *_index_range.x.
bool memexport_draw(s_draw_geometry draw, s_shader_entry *vs, uint32_t start, uint32_t count)
{
    if (g_export.base == 0) {
        report(key_of(draw->vertex_shader) ^ 0x6d656d78u, "a memexport draw outside BeginExport / EndExport (skipped)");
        return false;
    }
    s_shader_pack_microcode micro = {};
    if (vs->microcode != SHADER_PACK_NOT_FOUND)
        shader_pack_get_microcode(g_pack, vs->microcode, &micro);
    // the spawn shaders index only their inputs (uploaded from `start`), the updates the exported records
    bool spawn = false;
    s_shader_pack_shader_info info;
    if (shader_pack_get_shader(g_pack, vs->shader_index, &info)) {
        for (uint32_t b = 0; b < info.binding_count && !spawn; b++) {
            s_shader_pack_binding binding;
            spawn = shader_pack_get_binding(g_pack, vs->shader_index, b, &binding) &&
                    binding.descriptor_type == k_shader_pack_descriptor_storage_buffer && binding.binding < 300u;
        }
    }
    g_index_range[0] = spawn ? 0u : start;
    g_index_range[1] = spawn ? count : start + count;
    const uint32_t local = compute_local_size_x(vs->shader_index);
    return record_compute(vs->shader_index, (count + local - 1u) / local, 1, 1, &micro, draw, vs, start, count, 0);
}

// ---- Halo 3 water: the tessellation factors (water_tessellation) and the tessellated patches (water_shading) ----
//
// On the Xbox (tu/graphicslibxbox/render_water.cpp c_water_renderer::render_water_part) a water part takes three
// passes of one render method:
//   - entry point water_tessellation (rasterizer_render_water, under BeginExport of the part's factor buffer, an
//     INDEX32 index buffer): a point list, one vertex per triangle, whose vertex shader (water_tessellation.fx, !pc)
//     vfetches the triangle's (mesh, water) vertex indices from stream 2 (COLOR0..2) at `index + c131.x`, the three
//     positions from stream 0, and memexports one level per edge: edge_tess_level(c132 camera position, edge, c134.x
//     the view diagonal's sine) times cone_face_testing(c133 camera forward), all 15 when b104 (tiled/screenshot);
//   - entry point static_per_pixel (refraction on, render_shading): D3DDevice_DrawIndexedTessellatedVertices of
//     D3DTPT_TRIPATCH (17) in D3DTM_ADAPTIVE (2) with levels 1..15, the factor buffer as its "index buffer"; the
//     vertex shader gets the patch index and barycentrics and vfetches the three corners itself;
//   - or entry point static_per_vertex (refraction off): an auto-indexed triangle list, vertex = 3 * triangle + corner.
// Bungie's DX11 build has no vertex fetch and no tessellator here: water_tessellation_vs (pc) exports nothing, and both
// water_shading vertex shaders read PC_WATER_TESSELLATION vertices (water.fx s_vertex_type_water_shading): the three
// corners' position, texcoord, tangent, binormal and lightmap texcoord, their local_info (POSITION1) and base texcoord
// (POSITION3), packed in that order as 57 floats (POSITION0..7, TEXCOORD0..1, NORMAL0..4), then the barycentric
// weights `bc` (TEXCOORD2: corner 0's weight in z, corner 2's in x, water_shading.fx barycentric_interpolate). The device
// does the rest: it computes the factor pass's levels on the CPU into the factor buffer (the memory the Xbox GPU
// writes), and expands each patch into those vertices.
//
// Tessellation: the Xenos clamps level + 1 to [min + 1, max + 1] (xenia d3d12_command_processor.cc, "plus 1.0") and
// splits with fractional_even partitioning; edge 0 is corner 0 -> 1, edge 1 is 1 -> 2, edge 2 is 2 -> 0 (xenia
// adaptive_triangle.hs.hlsl). Here an edge with level L gets ceil(L + 1) equal segments, the interior a uniform grid of
// the largest edge's count, and each edge's grid vertices are snapped to that edge's segments: a vertex on an edge
// depends only on the edge's two corners and its own segment count, so neighbouring patches meet without cracks
// (the extra grid vertices on a coarser edge collapse into degenerate triangles). The winding is the triangle's.

const s_xenos_d3d_vertex_element k_water_patch_elements[] = {
    { 0, 0, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_position, 0 },
    { 0, 16, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_position, 1 },
    { 0, 32, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_position, 2 },
    { 0, 48, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_position, 3 },
    { 0, 64, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_position, 4 },
    { 0, 80, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_position, 5 },
    { 0, 96, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_position, 6 },
    { 0, 112, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_position, 7 },
    { 0, 128, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_texcoord, 0 },
    { 0, 144, (uint32_t)k_xenos_decltype_float3, 0, k_xenos_declusage_texcoord, 1 },
    { 0, 156, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_normal, 0 },
    { 0, 172, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_normal, 1 },
    { 0, 188, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_normal, 2 },
    { 0, 204, (uint32_t)k_xenos_decltype_float4, 0, k_xenos_declusage_normal, 3 },
    { 0, 220, (uint32_t)k_xenos_decltype_float2, 0, k_xenos_declusage_normal, 4 },
    { 0, 228, (uint32_t)k_xenos_decltype_float3, 0, k_xenos_declusage_texcoord, 2 },
    { 0xff, 0, (uint32_t)k_xenos_decltype_unused, 0, 0, 0 },
};
const uint32_t k_water_patch_floats = 60;          // 240 bytes a vertex: the elements above
const uint32_t k_water_patch_corner_floats = 57;   // ... of which the three corners (the rest is bc)
const uint32_t k_water_vertex_budget = 1u << 16;   // 15 MB of the 64 MB frame ring at most
const float k_water_max_level = 15.0f;              // water_tessellation.fx MAX_TESS_LEVEL

// One element of vertex `vertex` as the Xenos fetch returns it (missing components 0, 0, 1); false for an element
// outside its stream or of a type the water declarations do not use.
bool fetch_element(const s_port_vk_stream &stream, const s_xenos_d3d_vertex_element *element, uint32_t vertex,
                   float out[4])
{
    out[0] = out[1] = out[2] = 0.0f;
    out[3] = 1.0f;
    if (element == 0 || stream.data == 0 || stream.stride == 0)
        return false;
    uint32_t swap_unit;
    const uint32_t size = port_geometry_decltype_size(element->type, &swap_unit);
    const uint64_t at = (uint64_t)stream.offset + (uint64_t)vertex * stream.stride + element->offset;
    if (size == 0 || size > 16 || at + size > stream.size)
        return false;
    uint8_t b[16];
    memcpy(b, vertex_bytes(stream, element->stream, at), size);
    if (stream.big_endian)
        port_geometry_swap_element(element->type, b);
    uint16_t h[2];
    uint32_t w;
    switch (element->type) {
    case k_xenos_decltype_float1:
    case k_xenos_decltype_float2:
    case k_xenos_decltype_float3:
    case k_xenos_decltype_float4:
        memcpy(out, b, size);
        return true;
    case k_xenos_decltype_ushort2:
        memcpy(h, b, 4);
        out[0] = (float)h[0], out[1] = (float)h[1];
        return true;
    case k_xenos_decltype_ushort2n:
        memcpy(h, b, 4);
        out[0] = (float)h[0] / 65535.0f, out[1] = (float)h[1] / 65535.0f;
        return true;
    case k_xenos_decltype_dhen3n: {
        // 11_11_10: X bits 0..9, Y 10..20, Z 21..31, signed normalized (x / 511, y and z / 1023, clamped to -1)
        memcpy(&w, b, 4);
        const int32_t x = (int32_t)(w << 22) >> 22, y = (int32_t)(w << 11) >> 21, z = (int32_t)w >> 21;
        out[0] = x < -511 ? -1.0f : (float)x / 511.0f;
        out[1] = y < -1023 ? -1.0f : (float)y / 1023.0f;
        out[2] = z < -1023 ? -1.0f : (float)z / 1023.0f;
        return true;
    }
    default:
        return false;
    }
}

const s_xenos_d3d_vertex_element *water_element(const s_xenos_vertex_declaration *declaration, uint32_t usage,
                                                uint32_t usage_index)
{
    for (uint32_t e = 0; e < declaration->element_count; e++)
        if (declaration->elements[e].usage == usage && declaration->elements[e].usage_index == usage_index)
            return &declaration->elements[e];
    return 0;
}

// The device links no libm (gpu_crt.h): a square root by Newton steps, ample for lengths and sines.
float water_sqrt(float x)
{
    if (!(x > 0.0f))
        return 0.0f;
    uint32_t bits;
    memcpy(&bits, &x, 4);
    bits = 0x1fbd1df5u + (bits >> 1); // the exponent halved: within 4 % of the root
    float r;
    memcpy(&r, &bits, 4);
    for (int i = 0; i < 4; i++)
        r = 0.5f * (r + x / r);
    return r;
}

float water_length3(const float v[3]) { return water_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }

// water_tessellation.fx edge_tess_level and cone_face_testing, operation for operation.
float water_edge_level(const float camera[3], const float *p0, const float *p1, float camera_sin)
{
    // the corners in one order, so both triangles of an edge round its level alike (segments are whole numbers here,
    // where the Xenos' fractional_even split hides such differences)
    if (p1[0] < p0[0] || (p1[0] == p0[0] && (p1[1] < p0[1] || (p1[1] == p0[1] && p1[2] < p0[2])))) {
        const float *t = p0;
        p0 = p1;
        p1 = t;
    }
    float center[3], d[3];
    for (int i = 0; i < 3; i++)
        center[i] = (p0[i] + p1[i]) * 0.5f;
    for (int i = 0; i < 3; i++)
        d[i] = p0[i] - center[i];
    const float radius = water_length3(d);
    for (int i = 0; i < 3; i++)
        d[i] = center[i] - camera[i];
    float distance = water_length3(d);
    distance = distance > 0.001f ? distance : 0.001f;
    if (distance < 5.0f / camera_sin || radius > distance)
        return k_water_max_level;
    const float sin_theta = radius / distance;
    float coefficient = 1.0f - distance / 40.0f;
    coefficient = coefficient < 0.0f ? 0.0f : coefficient > 1.0f ? 1.0f : coefficient;
    coefficient *= coefficient;
    const float level = sin_theta * (100.0f * coefficient) / camera_sin;
    return level < k_water_max_level ? level : k_water_max_level;
}

bool water_face_visible(const float camera[3], const float forward[3], float diagonal_sin, const float *p0,
                        const float *p1, const float *p2)
{
    float center[3], d[3];
    for (int i = 0; i < 3; i++)
        center[i] = (p0[i] + p1[i] + p2[i]) * 0.33333f;
    const float *corners[3] = { p0, p1, p2 };
    float radius = 0.0f;
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < 3; i++)
            d[i] = corners[k][i] - center[i];
        const float r = water_length3(d);
        radius = r > radius ? r : radius;
    }
    for (int i = 0; i < 3; i++)
        d[i] = center[i] - camera[i];
    const float distance = water_length3(d);
    if (radius > distance)
        return true;
    const float sph_sin = radius / distance, sph_cos = water_sqrt(1.0f - sph_sin * sph_sin);
    const float cent_cos = (d[0] * forward[0] + d[1] * forward[1] + d[2] * forward[2]) / distance;
    const float cent_sin = water_sqrt(1.0f - cent_cos * cent_cos);
    const float bund_sin = cent_sin * sph_cos - cent_cos * sph_sin, bund_cos = cent_cos * sph_cos + cent_sin * sph_sin;
    if (bund_sin < 0.0f)
        return true;
    if (bund_cos < 0.0f)
        return false;
    return !(bund_sin > diagonal_sin);
}

// The corner indices of water triangle t: (mesh vertex, water vertex) from stream 2's COLOR0..2.
bool water_triangle(s_draw_geometry draw, const s_xenos_vertex_declaration *declaration, uint32_t t,
                    uint32_t mesh[3], uint32_t water[3])
{
    for (uint32_t k = 0; k < 3; k++) {
        const s_xenos_d3d_vertex_element *element = water_element(declaration, k_xenos_declusage_color, k);
        float v[4];
        if (element == 0 || (draw.stream_mask() & (1u << element->stream)) == 0 ||
            !fetch_element(draw.stream(element->stream), element, t, v))
            return false;
        mesh[k] = (uint32_t)v[0];
        water[k] = (uint32_t)v[1];
    }
    return true;
}

bool water_fetch(s_draw_geometry draw, const s_xenos_d3d_vertex_element *element, uint32_t vertex, float out[4])
{
    out[0] = out[1] = out[2] = 0.0f, out[3] = 1.0f; // an element the declaration lacks reads as the null buffer
    return element != 0 && (draw.stream_mask() & (1u << element->stream)) != 0 &&
           fetch_element(draw.stream(element->stream), element, vertex, out);
}

// The factors of this frame's factor passes, by the guest factor buffer they stand for. The Xbox GPU writes them into
// that buffer (tag resource memory); the device keeps them beside it instead, so resource pages keep the bytes their
// page hashes were taken of (resource_swap.h).
struct s_water_factor_buffer {
    const uint8_t *base;    // the factor buffer (D3DIndexBuffer Address)
    uint32_t size;          // its bytes
    uint32_t frame;         // g_frame_serial of the pass that wrote it
    float *factors;         // size / 4 floats
    uint32_t capacity;      // bytes allocated
};
s_water_factor_buffer g_water_factor_buffers[16];

// The factors written this frame for `size` bytes of a factor buffer from `data`, or 0.
const float *water_factors_for(const void *data, uint32_t size)
{
    const uint8_t *p = (const uint8_t *)data;
    for (const s_water_factor_buffer &buffer : g_water_factor_buffers)
        if (buffer.base != 0 && buffer.factors != 0 && buffer.frame == g_frame_serial && p >= buffer.base &&
            size <= buffer.size && (uint32_t)(p - buffer.base) <= buffer.size - size && ((p - buffer.base) & 3) == 0)
            return buffer.factors + (p - buffer.base) / 4;
    return 0;
}

// The host factors of the factor buffer `base` (the same buffer again, else one not written this frame).
s_water_factor_buffer *water_factor_buffer(const void *base, uint32_t size)
{
    s_water_factor_buffer *slot = 0;
    for (s_water_factor_buffer &buffer : g_water_factor_buffers) {
        if (buffer.base == (const uint8_t *)base) {
            slot = &buffer;
            break;
        }
        if (slot == 0 && buffer.frame != g_frame_serial)
            slot = &buffer;
    }
    if (slot == 0)
        return 0;
    if (slot->capacity < size) {
        float *grown = (float *)realloc(slot->factors, size);
        if (grown == 0)
            return 0;
        slot->factors = grown;
        slot->capacity = size;
    }
    slot->base = (const uint8_t *)base;
    slot->size = size;
    slot->frame = g_frame_serial;
    return slot;
}

// The water_tessellation draw (inside BeginExport of the factor buffer): `count` triangles from `start`, the levels of
// each as floats 3 * (start + i + c131.x) + edge of the export (kept by the device: water_factor_buffer).
void water_tessellation_factors(s_draw_geometry draw, uint32_t start, uint32_t count)
{
    uint32_t declaration_hash = 0;
    const s_xenos_vertex_declaration *declaration = declaration_for(draw.declaration(), &declaration_hash);
    const float *c = g_shadows.vertex_float;
    if (declaration == 0 || c == 0 || g_export.base == 0) {
        report(0x77746631u, "a water tessellation pass without its declaration or export (skipped)");
        return;
    }
    const s_xenos_d3d_vertex_element *position = water_element(declaration, k_xenos_declusage_position, 0);
    const float *camera = c + 4 * 132, *forward = c + 4 * 133;
    const float camera_sin = c[4 * 134];
    const int32_t offset = (int32_t)c[4 * 131];
    // k_is_under_screenshot (b104, rasterizer_render_water: tiling on): every edge at the maximum
    const bool maximum = g_shadows.vertex_bool != 0 && (g_shadows.vertex_bool[104 / 32] >> (104 % 32) & 1u) != 0;
    s_water_factor_buffer *buffer = water_factor_buffer(g_export.base, g_export.size);
    if (buffer == 0) {
        report(0x77746637u, "no room for the factors of a water tessellation pass (skipped)");
        return;
    }
    float *out = buffer->factors;
    const uint32_t capacity = g_export.size / 4u;
    uint32_t failed = 0;
    for (uint32_t i = 0; i < count; i++) {
        const int64_t t = (int64_t)start + i + offset;
        if (t < 0 || (uint64_t)t * 3u + 3u > capacity)
            break;
        float level[3] = { k_water_max_level, k_water_max_level, k_water_max_level };
        uint32_t mesh[3], water[3];
        float p[3][4];
        if (!maximum) {
            if (!water_triangle(draw, declaration, (uint32_t)t, mesh, water) || !water_fetch(draw, position, mesh[0], p[0]) ||
                !water_fetch(draw, position, mesh[1], p[1]) || !water_fetch(draw, position, mesh[2], p[2])) {
                failed++;
            } else if (camera_sin > 0.0f) {
                const float visible = water_face_visible(camera, forward, camera_sin, p[0], p[1], p[2]) ? 1.0f : 0.0f;
                for (int k = 0; k < 3; k++)
                    level[k] = water_edge_level(camera, p[k], p[(k + 1) % 3], camera_sin) * visible;
            }
        }
        memcpy(out + 3 * t, level, sizeof(level));
    }
    if (failed != 0)
        report(0x77746632u, "a water tessellation pass whose triangles cannot be read (those edges at the maximum)");
}

uint32_t water_segments(float level, float minimum, float maximum, float scale)
{
    // the Xenos factor (level + 1, clamped), its fractional_even split rounded up to whole segments
    float factor = level == level ? level + 1.0f : maximum + 1.0f;
    factor = factor < minimum + 1.0f ? minimum + 1.0f : factor > maximum + 1.0f ? maximum + 1.0f : factor;
    const float scaled = factor * scale;
    const uint32_t n = (uint32_t)scaled + ((float)(uint32_t)scaled < scaled ? 1u : 0u);
    return n < 1u ? 1u : n > 16u ? 16u : n;
}

uint32_t water_grid_index(uint32_t n, uint32_t b, uint32_t c) { return c * (n + 1u) - c * (c - 1u) / 2u + b; }

// Draws `patches` water triangles from triangle `first` as PC_WATER_TESSELLATION vertices. factors: three floats a patch
// (the tessellated draw), or 0 for one segment an edge (the triangle-list draw).
void water_draw_patches(s_draw_geometry draw, uint32_t first, uint32_t patches, const float *factors,
                        float minimum, float maximum)
{
    uint32_t declaration_hash = 0;
    const s_xenos_vertex_declaration *declaration = declaration_for(draw.declaration(), &declaration_hash);
    if (declaration == 0 || patches == 0)
        return;
    const s_xenos_d3d_vertex_element *position = water_element(declaration, k_xenos_declusage_position, 0);
    const s_xenos_d3d_vertex_element *texcoord = water_element(declaration, k_xenos_declusage_texcoord, 0);
    const s_xenos_d3d_vertex_element *tangent = water_element(declaration, k_xenos_declusage_tangent, 0);
    const s_xenos_d3d_vertex_element *binormal = water_element(declaration, k_xenos_declusage_binormal, 0);
    const s_xenos_d3d_vertex_element *lightmap = water_element(declaration, k_xenos_declusage_texcoord, 1);
    const s_xenos_d3d_vertex_element *local_info = water_element(declaration, k_xenos_declusage_position, 1);
    const s_xenos_d3d_vertex_element *base_texcoord = water_element(declaration, k_xenos_declusage_position, 3);
    // segments per edge, scaled down evenly (each edge's count still a function of its own level) to the budget
    float scale = 1.0f;
    for (int pass = 0; pass < 2; pass++) {
        uint64_t vertices = 0;
        for (uint32_t p = 0; p < patches; p++) {
            uint32_t n = 1;
            for (uint32_t e = 0; factors != 0 && e < 3; e++) {
                const uint32_t s = water_segments(factors[3 * p + e], minimum, maximum, scale);
                n = s > n ? s : n;
            }
            vertices += (uint64_t)(n + 1u) * (n + 2u) / 2u;
        }
        if (vertices <= k_water_vertex_budget)
            break;
        scale *= water_sqrt((float)k_water_vertex_budget / (float)vertices) * 0.95f;
        if (pass == 1) {
            report(0x77746633u, "water patches over the vertex budget (one segment an edge)");
            factors = 0;
        }
    }
    static float *vertex_data;
    static uint32_t *index_data;
    static uint32_t vertex_capacity, index_capacity;
    uint32_t vertex_count = 0, index_count = 0, unreadable = 0;
    for (uint32_t p = 0; p < patches; p++) {
        uint32_t mesh[3], water[3];
        if (!water_triangle(draw, declaration, first + p, mesh, water)) {
            unreadable++;
            continue;
        }
        float corner[k_water_patch_corner_floats];
        float *o = corner;
        for (uint32_t k = 0; k < 3; k++) {
            float v[4];
            water_fetch(draw, position, mesh[k], v), memcpy(o, v, 12), o += 3;
            water_fetch(draw, texcoord, mesh[k], v), memcpy(o, v, 8), o += 2;
            water_fetch(draw, tangent, mesh[k], v), memcpy(o, v, 12), o += 3;
            water_fetch(draw, binormal, mesh[k], v), memcpy(o, v, 12), o += 3;
            water_fetch(draw, lightmap, mesh[k], v), memcpy(o, v, 8), o += 2;
        }
        for (uint32_t k = 0; k < 3; k++) {
            float v[4];
            water_fetch(draw, local_info, water[k], v), memcpy(o, v, 12), o += 3;
            water_fetch(draw, base_texcoord, water[k], v), memcpy(o, v, 12), o += 3;
        }
        uint32_t edge[3] = { 1, 1, 1 }, n = 1;
        for (uint32_t e = 0; factors != 0 && e < 3; e++) {
            edge[e] = water_segments(factors[3 * p + e], minimum, maximum, scale);
            n = edge[e] > n ? edge[e] : n;
        }
        const uint32_t grid = (n + 1u) * (n + 2u) / 2u, triangles = n * n;
        if (vertex_count + grid > vertex_capacity || index_count + 3u * triangles > index_capacity) {
            const uint32_t vc = (vertex_count + grid) * 2u, ic = (index_count + 3u * triangles) * 2u;
            float *nv = (float *)realloc(vertex_data, (size_t)vc * k_water_patch_floats * 4u);
            if (nv != 0)
                vertex_data = nv, vertex_capacity = vc;
            uint32_t *ni = (uint32_t *)realloc(index_data, (size_t)ic * 4u);
            if (ni != 0)
                index_data = ni, index_capacity = ic;
            if (nv == 0 || ni == 0)
                break;
        }
        // grid vertex (b, c): weights (a, b, c) / n of corners 0, 1, 2 (a = n - b - c), snapped on the edges
        for (uint32_t c = 0; c <= n; c++) {
            for (uint32_t b = 0; b + c <= n; b++) {
                const uint32_t a = n - b - c;
                float w0 = (float)a / (float)n, w1 = (float)b / (float)n, w2 = (float)c / (float)n;
                if (c == 0) {        // edge 0: corner 0 -> 1
                    const uint32_t k = (uint32_t)((float)b * (float)edge[0] / (float)n + 0.5f); // round half up (non-negative)
                    w1 = (float)k / (float)edge[0], w0 = (float)(edge[0] - k) / (float)edge[0], w2 = 0.0f;
                } else if (a == 0) { // edge 1: corner 1 -> 2
                    const uint32_t k = (uint32_t)((float)c * (float)edge[1] / (float)n + 0.5f); // round half up (non-negative)
                    w2 = (float)k / (float)edge[1], w1 = (float)(edge[1] - k) / (float)edge[1], w0 = 0.0f;
                } else if (b == 0) { // edge 2: corner 2 -> 0
                    const uint32_t k = (uint32_t)((float)a * (float)edge[2] / (float)n + 0.5f); // round half up (non-negative)
                    w0 = (float)k / (float)edge[2], w2 = (float)(edge[2] - k) / (float)edge[2], w1 = 0.0f;
                }
                float *v = vertex_data + (size_t)(vertex_count + water_grid_index(n, b, c)) * k_water_patch_floats;
                memcpy(v, corner, sizeof(corner));
                v[k_water_patch_corner_floats + 0] = w2; // bc: corner 0's weight in z (barycentric_interpolate)
                v[k_water_patch_corner_floats + 1] = w1;
                v[k_water_patch_corner_floats + 2] = w0;
            }
        }
        for (uint32_t c = 0; c < n; c++) {
            for (uint32_t b = 0; b + c < n; b++) {
                uint32_t *i = index_data + index_count;
                i[0] = vertex_count + water_grid_index(n, b, c);
                i[1] = vertex_count + water_grid_index(n, b + 1u, c);
                i[2] = vertex_count + water_grid_index(n, b, c + 1u);
                index_count += 3u;
                if (b + c + 1u < n) {
                    i[3] = vertex_count + water_grid_index(n, b + 1u, c);
                    i[4] = vertex_count + water_grid_index(n, b + 1u, c + 1u);
                    i[5] = vertex_count + water_grid_index(n, b, c + 1u);
                    index_count += 3u;
                }
            }
        }
        vertex_count += grid;
    }
    if (unreadable != 0)
        report(0x77746634u, "water triangles whose vertex indices cannot be read (skipped)");
    if (vertex_count == 0 || index_count == 0)
        return;
    const s_port_vk_indices indices = { 0, index_data, index_count * 4u, 4u, 0, 0 };
    const s_port_vk_geometry geometry = { { 0, vertex_data, vertex_count * k_water_patch_floats * 4u, 0,
                                            k_water_patch_floats * 4u, 0 }, &indices, k_water_patch_elements, 1u, 0 };
    port_vk_draw(draw.packet, PORT_VK_DIRTY_STREAMS | PORT_VK_DIRTY_IB | PORT_VK_DIRTY_VDECL,
                 PORT_VK_PT_TRIANGLELIST, 0, index_count, 0, 1, &geometry);
}

// The water vertex shaders' draws (vertex type water; port_vk_draw). True when handled here.
bool water_draw(s_draw_geometry draw, const s_shader_pack_shader_info &vs_info, uint32_t primitive_type,
                uint32_t start, uint32_t count, uint32_t indexed)
{
    if (draw.declaration() == (const void *)k_water_patch_elements)
        return false; // the expanded draw itself
    if (vs_info.entry_point == k_shader_pack_entry_water_tessellation) {
        if (g_export.base != 0)
            water_tessellation_factors(draw, start, count);
        else
            report(0x77746635u, "a water tessellation pass outside BeginExport (skipped: it only exports)");
        return true;
    }
    if (primitive_type == PORT_VK_PT_TRIANGLELIST && !indexed) {
        water_draw_patches(draw, start / 3u, count / 3u, 0, 1.0f, 1.0f); // vertex = 3 * triangle + corner
        return true;
    }
    report(0x77746636u ^ primitive_type, "a water draw that is neither a factor pass nor a triangle list (skipped)");
    return true;
}

// ---- shader pack version 3: translated Xenos microcode (port/shaders/xenos_spirv_abi.h) ----
//
// With a version 3 pack (shader_pack_is_xenos) every retail shader is its own microcode translated to SPIR-V
// (port/tools/xenos_spirv), so the device does what the Xbox D3D runtime and the Xenos do for it instead of what the
// DX11 path needs: one universal pipeline layout; the D3DDevice's register shadows copied verbatim (no gathers, PC
// members or sampler maps), the shader's literal constants written over them as the runtime does when it binds a
// shader; the vertex declaration and stream sources as the vfetch patch (xe_vertex_fetch) over the raw guest bytes by
// buffer device address (no per-element conversion, no vertex input state); the fetch constants' textures at the
// fetch constant's bindings; memexport vertex shaders as compute dispatches writing the exported memory's device copy
// by address, which later draws read back the same way; the tessellated water through the tessellator with the
// factor buffer as its index buffer; rectangle lists through a geometry shader. Everything else (render targets,
// EDRAM aliasing, resolves, clears, presentation, textures' conversion and index data) is the device's common path.

enum e_xe_block {
    k_xe_block_system,
    k_xe_block_float_vs,
    k_xe_block_float_ps,
    k_xe_block_bool_loop,
    k_xe_block_fetch,
    k_xe_block_vertex_fetch,
    k_xe_block_count
};
constexpr uint32_t k_xe_block_size[k_xe_block_count] = { sizeof(xe_system), 4096, 4096, sizeof(xe_bool_loop), sizeof(xe_fetch),
                                                     sizeof(xe_vertex_fetch) };
static_assert(sizeof(xe_system) == 288, "xe_system is the translator's 288-byte XeSystem (abi.odin SYS_SIZE)");
static_assert(sizeof(xe_bool_loop) == 160, "xe_bool_loop is XeBoolLoop");
static_assert(sizeof(xe_vertex_fetch) == 1792, "xe_vertex_fetch is XeVertexFetch");
static_assert(sizeof(xe_fetch) == 1280, "xe_fetch is XeFetch");

// The 16-byte rows of a set 0 block (the float registers' 4096 bytes are the most: 256 rows), as a bit set.
const uint32_t k_xe_block_rows = 256;
struct s_xe_rows {
    uint32_t bits[k_xe_block_rows / 32u];
    uint64_t chunk_mask; // bit g (0..63): any row in [4*g .. 4*g+3] is set in bits[]
};

struct s_xe_v6_float_layout {
    uint64_t overlay_mask;
    uint8_t table_lines; // bit 0/1: the shader reads IDs in the low/high 64 bytes
    uint8_t first_def[64]; // first sorted literal definition in each overlaid chunk
    // Immutable table last used by this layout, even if another shader has
    // since replaced the bank's working set of chunk IDs.
    mutable uint64_t table_epoch;
    mutable uint32_t table_gen, table_offset;
    mutable uint64_t resident_literals;
    mutable uint16_t literal_ids[64]; // chunks with no live shadow rows
};

struct s_xe_row_run { uint8_t first, count; };

// The live rows and literals for a pipeline's shaders and helper stages.
struct s_xe_rows_memo {
    uint32_t next; // next interned layout with the same hash, index + 1
    uint32_t kind, vs, ps, extra;
    s_xe_rows rows[k_xe_block_count];
    uint32_t literals[2];
    uint64_t shadow_chunks[2]; // chunks with at least one live float row supplied by the register shadow
    s_xe_v6_float_layout v6_float[2];
    uint32_t fetch_slots;
    // Alternating live/dead rows give the maximum number of runs per non-float block.
    s_xe_row_run runs[(sizeof(xe_system) + 16) / 32 + (sizeof(xe_bool_loop) + 16) / 32 +
                      (sizeof(xe_fetch) + 16) / 32 + (sizeof(xe_vertex_fetch) + 16) / 32];
    uint8_t first_run[k_xe_block_count + 1];
};

static_assert(sizeof(xe_system) % 16u == 0 && sizeof(xe_bool_loop) % 16u == 0 && sizeof(xe_fetch) % 16u == 0 &&
                  sizeof(xe_vertex_fetch) % 16u == 0 && sizeof(xe_vertex_fetch) <= 16u * k_xe_block_rows,
              "set 0's blocks are whole rows");

inline uint64_t xe_rows_compute_chunk_mask(const uint32_t bits[k_xe_block_rows / 32u],
                                           uint32_t words = k_xe_block_rows / 32u)
{
    uint64_t mask = 0;
    for (uint32_t w = 0; w < words; w++) {
        uint32_t v = bits[w];
        if (v == 0)
            continue;
        v = (v | (v >> 2)) & 0x33333333u;
        v = (v | (v >> 1)) & 0x11111111u;
        v = (v | (v >> 3)) & 0x03030303u;
        v = (v | (v >> 6)) & 0x000f000fu;
        v = (v | (v >> 12)) & 0xffu;
        mask |= (uint64_t)v << (w * 8u);
    }
    return mask;
}

// The last copy of each block put in this frame's ring: a draw whose shaders read the same values reuses it. Only the
// rows the shaders that made it read were written (xe_block): `valid` are those, `data` their values.
struct s_xe_block_cache {
    uint32_t frame;           // g_frame_serial of the copy, 0: none
    uint32_t offset;
    uint32_t last_gen;        // generation of `data` when the copy was made (0: unversioned)
    uint32_t literals_id;     // SHADER_PACK_NOT_FOUND when no literals applied, else literals->index
    uint32_t data_valid;      // nonzero when `data` holds the cached rows for memcmp fallback (gen == 0)
    uint32_t chunk_gen_valid; // nonzero when `chunk_gen[]` matches every chunk in `valid.chunk_mask`
    const s_xe_rows *covered_rows; // immutable xe_pipeline_rows layout already covered by this upload
    s_xe_rows valid;
    uint32_t chunk_gen[64];
    uint8_t data[4096];
};

// Precomputed deduplicated literal constant list per Xenos shader (the last literal of a register wins).
struct s_xe_dedup_def {
    uint32_t reg;
    uint32_t value[4];
};
struct s_xe_dedup_literals {
    uint32_t xenos_index;
    s_xe_rows mask;
    uint32_t count;
    s_xe_dedup_def *defs;
};
t_list<s_xe_dedup_literals> g_xe_dedup_literals;
s_map g_xe_dedup_literals_index; // (xenos_index + 1) -> entry index
uint32_t xe_float_literal_index(s_shader_entry *entry, const s_shader_pack_xenos *meta);

const s_xe_dedup_literals *xe_literals_at(uint32_t index)
{
    return index >= 2u ? &g_xe_dedup_literals.items[index - 2u] : 0;
}

constexpr uint32_t k_xe_v6_arena_bytes = 16u << 20;
constexpr uint32_t k_xe_v6_image_keys = 4u * 32u;
constexpr uint32_t k_xe_v6_sampler_keys = 6u * 32u;
static_assert(XE_V6_TABLE_START % 64u == 0 && XE_V6_TABLE_BYTES % 64u == 0, "whole cache lines in the arena");
constexpr uint32_t k_xe_v6_max_draw_tables =
    2u * (XE_V6_TABLE_BYTES + (k_xe_v6_image_keys + k_xe_v6_sampler_keys) * sizeof(uint32_t));

struct s_xe_v6_chunk {
    uint32_t gen;
    uint16_t id;
};

struct s_xe_v6_overlay_chunk {
    s_xe_v6_chunk chunk;
    uint32_t literal_id;
};

struct s_xe_v6_bank {
    uint32_t last_table_off, gen, literal_id, table_lines, last_shader;
    uint64_t pure, dirty, overlay_mask, epoch;
    const float *source;
    const uint32_t *chunk_gen;
    // The shader extracts the low/high 16 bits of each little-endian table word.
    alignas(64) uint16_t ids[64];
    s_xe_v6_chunk chunk[64];
    s_xe_v6_overlay_chunk overlay[64];
};
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ && sizeof(s_xe_v6_bank::ids) == XE_V6_TABLE_BYTES,
              "ABI 6 packs two chunk IDs per little-endian word");

struct s_xe_v6_snapshot {
    s_vulkan_buffer *buffer;
    s_xe_v6_snapshot *next;
};

struct s_xe_v6_arena {
    s_vulkan_buffer *buffer;
    s_xe_v6_snapshot *retired;
    uint8_t *mapped; // write-only upload memory
    uint64_t address, recording, epoch;
    uint32_t next_chunk, tables;
    s_xe_v6_bank banks[2];
};
s_xe_v6_arena g_xe_v6_arenas[VULKAN_MAX_FRAMES];
uint64_t g_xe_v6_epoch;
uint64_t g_xe_v6_bank_epoch;

struct alignas(64) s_xe_v6_texture_tables {
    xe_push_v6 push;
    uint64_t arena_epoch;
    uint32_t dirty_tables; // bits 0,1: VS/PS images; bits 2,3: VS/PS samplers
    uint32_t images[2][k_xe_v6_image_keys];
    uint32_t samplers[2][k_xe_v6_sampler_keys];
};

struct s_xe_draw_textures {
    gpu_bind_group set;
    s_xe_v6_texture_tables *tables;
};

bool xe_v6_allocate_arena(s_vulkan_backend *vk, s_xe_v6_arena &arena)
{
    // The Vulkan upload allocator requires HOST_COHERENT. Each mapped arena is writable only while
    // its frame slot is owned by the CPU, so submission publishes the writes without a mapped flush.
    s_xe_v6_snapshot *old = 0;
    if (arena.buffer) {
        old = (s_xe_v6_snapshot *)malloc(sizeof(*old));
        if (!old)
            return false;
    }
    s_vulkan_buffer *buffer = vulkan_buffer_create(vk, k_xe_v6_arena_bytes + 63u, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                   k_vulkan_memory_upload);
    uint32_t lo, hi;
    vulkan_buffer_device_address(vk, buffer, &lo, &hi);
    const uint64_t address = (uint64_t)lo | ((uint64_t)hi << 32);
    if (!buffer || !address || !buffer->mapped || !buffer->coherent) {
        free(old);
        vulkan_buffer_destroy(vk, buffer);
        return false;
    }
    if (old) {
        *old = { arena.buffer, arena.retired };
        arena.retired = old;
    }
    const uint32_t align = (uint32_t)(-address & 63u);
    arena.buffer = buffer;
    arena.mapped = (uint8_t *)buffer->mapped + align;
    arena.address = address + align;
    memset(arena.mapped, 0, XE_V6_CHUNK_BYTES);
    memset(arena.mapped + XE_V6_ZERO_TABLE, 0, XE_V6_TABLE_BYTES);
    return true;
}

void xe_v6_release_snapshots(s_vulkan_backend *vk, s_xe_v6_arena &arena)
{
    while (arena.retired) {
        s_xe_v6_snapshot *old = arena.retired;
        arena.retired = old->next;
        vulkan_buffer_destroy(vk, old->buffer);
        free(old);
    }
}

void xe_v6_release_arenas(s_vulkan_backend *vk)
{
    for (auto &arena : g_xe_v6_arenas) {
        xe_v6_release_snapshots(vk, arena);
        vulkan_buffer_destroy(vk, arena.buffer);
        arena = {};
    }
}

bool xe_v6_allocate_arenas(s_vulkan_backend *vk)
{
    for (uint32_t i = 0; i < vk->frame_count; ++i) {
        if (!xe_v6_allocate_arena(vk, g_xe_v6_arenas[i])) {
            xe_v6_release_arenas(vk);
            return false;
        }
    }
    return true;
}

void xe_v6_reset_banks(s_xe_v6_arena &arena)
{
    memset(arena.banks, 0, sizeof(arena.banks));
    for (auto &bank : arena.banks) {
        bank.epoch = ++g_xe_v6_bank_epoch;
        bank.last_table_off = XE_V6_ZERO_TABLE;
        bank.table_lines = 3u;
    }
}

s_xe_v6_arena *xe_v6_prepare_arena()
{
    auto &arena = g_xe_v6_arenas[g_vk->frame_index];
    if (arena.recording == g_vk->bindless_serial) {
        if (arena.next_chunk <= XE_V6_CHUNK_COUNT - 128u &&
            arena.tables <= k_xe_v6_arena_bytes - k_xe_v6_max_draw_tables)
            return &arena;
        // Reserve a whole draw's worst case before writing anything. Retire the old snapshot with
        // this submission so every recorded address stays valid through its last use.
        if (!xe_v6_allocate_arena(g_vk, arena))
            return 0;
    } else {
        // Frame begin has waited for this slot's fence.
        xe_v6_release_snapshots(g_vk, arena);
    }
    arena.recording = g_vk->bindless_serial;
    arena.epoch = ++g_xe_v6_epoch;
    arena.next_chunk = 1;
    arena.tables = XE_V6_TABLE_START;
    xe_v6_reset_banks(arena);
    return &arena;
}

template<size_t N>
uint32_t xe_v6_write_table(s_xe_v6_arena &arena, const uint32_t (&ids)[N], uint32_t count)
{
    if (count == 0)
        return XE_V6_ZERO_TABLE;
    // These CPU arrays and the arena reservation both cover the last whole line.
    // The shader only indexes the first count words of its snapshot.
    static_assert(N % 16 == 0, "whole cache lines in the descriptor table");
    const uint32_t offset = arena.tables;
    const uint32_t bytes = ((count + 15u) & ~15u) * sizeof(uint32_t);
    for (uint32_t i = 0; i < bytes; i += 64)
        __builtin_memcpy_inline(arena.mapped + offset + i, ids + i / sizeof(*ids), 64);
    arena.tables += bytes;
    return offset;
}

uint32_t xe_v6_chunk_table(s_xe_v6_arena &arena, uint32_t stage, const s_port_vk_draw &draw,
                           const s_xe_rows_memo &live, const s_shader_pack_xenos *shader)
{
    s_xe_v6_bank &bank = arena.banks[stage];
    const uint64_t active = live.rows[k_xe_block_float_vs + stage].chunk_mask;
    const uint32_t gen = stage ? draw.ps_float_gen : draw.vs_float_gen;
    if (!active && gen)
        return XE_V6_ZERO_TABLE;
    const float *source = stage ? g_shadows.pixel_float : g_shadows.vertex_float;
    const uint32_t *chunk_gen = stage ? draw.ps_chunk_gen : draw.vs_chunk_gen;
    if (bank.source != source || bank.chunk_gen != chunk_gen || gen < bank.gen) {
        memset(bank.chunk, 0, sizeof(bank.chunk));
        memset(bank.overlay, 0, sizeof(bank.overlay));
        bank.source = source;
        bank.chunk_gen = chunk_gen;
        bank.pure = 0;
        bank.overlay_mask = 0;
        bank.dirty = UINT64_MAX;
        bank.gen = 0;
        bank.epoch = ++g_xe_v6_bank_epoch;
    }
    if (!active)
        return XE_V6_ZERO_TABLE;
    // ABI 6 float rows and literals belong to this shader alone, including when
    // the other stage or a pipeline helper changes.
    const uint32_t shader_index = stage ? live.ps : live.vs;
    if (gen && bank.gen == gen && bank.last_shader == shader_index)
        return bank.last_table_off;
    const auto &layout = live.v6_float[stage];
    const uint64_t shadow_chunks = live.shadow_chunks[stage];
    const auto *changes = chunk_gen == g_decoded.banks[stage].chunks ? record_changes_for(&draw) : nullptr;
    if (bank.gen != gen) {
        const bool consecutive = bank.gen && changes && bank.gen == changes->float_gen[stage];
        bank.dirty |= consecutive ? changes->chunks[stage] : changed_float_chunks(chunk_gen, bank.gen, gen);
        bank.gen = gen;
        bank.last_shader = SHADER_PACK_NOT_FOUND;
    }
    if (gen && layout.table_epoch == bank.epoch && layout.table_gen &&
        (layout.table_gen == gen || !source || !shadow_chunks ||
         (changes && layout.table_gen == changes->float_gen[stage] && !(changes->chunks[stage] & shadow_chunks)))) {
        layout.table_gen = gen;
        return layout.table_offset;
    }
    bank.last_shader = shader_index;
    // Inactive IDs can stay resident: only the layout's active chunks are read. A pure
    // shadow chunk remains usable across shader switches until its generation changes.
    const uint64_t overlay = layout.overlay_mask;
    const uint32_t literal_id = live.literals[stage];
    const uint64_t pure = active & ~overlay;
    const uint64_t dirty = gen || !source ? bank.dirty : UINT64_MAX;
    const uint64_t installed = bank.literal_id == literal_id ? bank.overlay_mask : 0u;
    uint64_t patch = (pure & ~bank.pure) | (overlay & ~installed) | (dirty & active & shadow_chunks);
    const bool covered = !(layout.table_lines & ~bank.table_lines);
    if (layout.table_epoch != bank.epoch)
        layout.resident_literals = 0;
    auto remember_table = [&] {
        layout.table_epoch = bank.epoch;
        layout.table_gen = gen;
        layout.table_offset = bank.last_table_off;
        return bank.last_table_off;
    };
    if (!patch && covered)
        return remember_table();
    bool changed = false;
    while (patch) {
        const uint32_t c = (uint32_t)__builtin_ctzll(patch);
        patch &= patch - 1u;
        const uint64_t bit = 1ull << c;
        uint16_t id = 0;
        const bool shadow = source && (shadow_chunks & bit);
        const uint32_t stamp = shadow ? (chunk_gen ? chunk_gen[c] : gen) : 0u;
        const bool unversioned = shadow && !gen;
        if (layout.resident_literals & bit) {
            id = layout.literal_ids[c];
        } else if (overlay & bit) {
            auto &entry = bank.overlay[c];
            if (unversioned || entry.literal_id != literal_id || entry.chunk.gen != stamp) {
                id = (uint16_t)arena.next_chunk++;
                // Finish the overlay in ordinary RAM, then write the upload cache line once.
                alignas(16) [[clang::uninitialized]] uint8_t bytes[XE_V6_CHUNK_BYTES];
                if (shadow)
                    __builtin_memcpy(bytes, source + c * 16u, sizeof(bytes));
                else
                    __builtin_memset(bytes, 0, sizeof(bytes));
                for (uint32_t i = layout.first_def[c]; i < shader->def_count && (shader->defs[i].reg >> 2) == c; ++i) {
                    const auto &def = shader->defs[i];
                    __builtin_memcpy(bytes + (def.reg & 3u) * 16u, def.value, 16u);
                }
                __builtin_memcpy(arena.mapped + id * XE_V6_CHUNK_BYTES, bytes, sizeof(bytes));
                entry = unversioned ? s_xe_v6_overlay_chunk {} : s_xe_v6_overlay_chunk { { stamp, id }, literal_id };
            } else {
                id = entry.chunk.id;
            }
            if (!(shadow_chunks & bit)) {
                layout.literal_ids[c] = id;
                layout.resident_literals |= bit;
            }
        } else if (shadow) {
            auto &entry = bank.chunk[c];
            if (unversioned || !entry.id || entry.gen != stamp) {
                id = (uint16_t)arena.next_chunk++;
                __builtin_memcpy(arena.mapped + id * XE_V6_CHUNK_BYTES, source + c * 16u, XE_V6_CHUNK_BYTES);
                entry = unversioned ? s_xe_v6_chunk {} : s_xe_v6_chunk { stamp, id };
            } else {
                id = entry.id;
            }
        }
        changed |= bank.ids[c] != id;
        bank.ids[c] = id;
    }
    bank.pure = (bank.pure | pure) & ~overlay;
    bank.dirty &= ~(active & shadow_chunks);
    if (overlay) {
        bank.literal_id = literal_id;
        bank.overlay_mask = overlay;
    } else {
        bank.overlay_mask &= ~pure;
    }
    if (changed || !covered) {
        // Publish an immutable table. A later shader needing an unwritten line
        // gets a new snapshot even if every ID it needs is still installed.
        const uint32_t offset = arena.tables;
        uint8_t *dst = arena.mapped + offset;
        if (layout.table_lines & 1u)
            __builtin_memcpy_inline(dst, bank.ids, 64);
        if (layout.table_lines & 2u)
            __builtin_memcpy_inline(dst + 64, bank.ids + 32, 64);
        bank.table_lines = layout.table_lines;
        bank.last_table_off = offset;
        arena.tables = offset + sizeof(bank.ids);
    }
    return remember_table();
}


// Cached xe_system block + lightweight key so consecutive draws with unchanged system state skip fill + memcmp.
struct s_xe_sys_cache {
    uint32_t valid;
    uint32_t gen;
    uint32_t res_scale;
    uint32_t alpha_packed;
    int32_t color_exp_bias[4];
    float point_min, point_max;
    float viewport_w, viewport_h;
    xe_system sys;
};
s_xe_sys_cache g_xe_sys_cache;

struct s_xe_pipeline_key {
    uint32_t vertex, pixel, kind, extra; // xenos entries (pixel SHADER_PACK_NOT_FOUND: depth only), e_xe_pipeline_kind
    s_gpu_graphics_state state;
};
enum e_xe_pipeline_kind {
    k_xe_pipeline_vertex = 0,       // VS (+ FS)
    k_xe_pipeline_rectangles = 1,   // VS + rectangle-list GS (+ FS); extra: the GS module
    k_xe_pipeline_tessellated = 2,  // helper VS + helper TCS + translated TES (+ FS)
    k_xe_pipeline_points = 3,       // VS + the pack's point-sprite GS (+ FS); extra: the GS module (caps.point_sprites 0)
};
// The kinds with a geometry stage (`extra`, the module).
inline bool xe_kind_has_geometry(uint32_t kind)
{
    return kind == k_xe_pipeline_rectangles || kind == k_xe_pipeline_points;
}
struct s_xe_pipeline_entry {
    s_xe_pipeline_key key;
    uint32_t hash;
    uint32_t next; // hash collision chain, index + 1 (0: end)
    s_gpu_pipeline *pipeline; // NULL: creation failed (not retried)
};

// A vertex buffer's raw guest bytes in a device buffer (by its D3DVertexBuffer, as long as the game does not rewrite
// it: from then on its draws copy what they read into the ring).
struct s_xe_buffer {
    const void *key;
    const void *data;
    uint32_t size, generation, dynamic;
    s_gpu_buffer *buffer;
    uint32_t address[2];
};

// The device copy of memory memexport writes (s_export_buffer's counterpart): the guest bytes as the Xenos would
// leave them (formats and byte order of the stream constant), at a device address the shaders write and read.
// The memory is also the CPU's, which reads and writes it in host order: `endian` is the swap between the two (a
// texture export's, port_vk_begin_texture_export: the texture format's GPUENDIAN; a vertex or index buffer export's:
// the Endian128 of the stream constants its draws export with, xe_stream_endian). The copy is made from the memory with
// the swap applied; a CPU lock writes it back with the swap undone (xe_export_lock, xe_buffer_lock), and once the CPU
// holds the whole copy so (cpu_current) the memory is its contents again: the CPU may rewrite it
// (main_screenshot_start_capture clears the buffer before each screenshot; the water writes its interaction events
// into the buffer its previous ripple export wrote), so fetches read the memory (xe_export_holding) and the next export
// of it starts from the memory (xe_export_for).
struct s_xe_export {
    const void *base;
    uint32_t size;
    uint32_t gpu_dword;  // GPU_CONVERT_CPU_TO_GPU_ADDRESS(base) >> 2: the stream constants' BaseAddress
    s_gpu_buffer *buffer;
    uint32_t address[2];
    uint32_t texture;    // made by a texture's BeginExport: texture fetches of the memory sample the copy
    uint32_t endian;
    uint32_t cpu_current;
    uint32_t written;    // g_xe_export_serial when the copy last changed (made, grown, or bound for an export draw)
};

// A texture over the memory of a texture export, sampled from the export's copy (xe_exported_texture_view): the raw
// copy bytes of its level 0 in an R8G8B8A8 image, whose view swizzle undoes the GPUENDIAN swap and applies the format's.
struct s_xe_export_view {
    const void *memory;  // the level's first byte
    uint32_t d3d_format, width, height;
    uint32_t written;    // the export's `written` the image was copied at
    s_gpu_image *image;
};
const uint32_t k_xe_export_views = 4;
s_xe_export_view g_xe_export_views[k_xe_export_views];
uint32_t g_xe_export_view_next; // the slot a new view replaces
uint32_t g_xe_export_serial;

// A texture set made this frame: the descriptors it holds, so a draw needing the same ones binds it again.
const uint32_t k_xe_max_texture_writes = 96;
struct s_xe_texture_set {
    uint32_t hash;
    uint32_t next; // hash collision chain, index + 1 (0: end)
    uint32_t count;
    uint32_t binding[k_xe_max_texture_writes];
    uint64_t handle[k_xe_max_texture_writes];
    gpu_bind_group set;
};

gpu_bind_layout g_xe_set_layout[2];
gpu_pipeline_layout g_xe_layout;
gpu_bind_group g_xe_constant_set[GPU_FRAMES_MAX];
uint32_t g_xe_ring_address[GPU_FRAMES_MAX][2];
// The translated shaders reach vertex streams and memexport destinations by device address (caps.buffer_device_address:
// Vulkan), else (Direct3D 12) by byte offsets into the buffers each draw binds as views (xe_location).
bool g_xe_device_addresses;
s_xe_block_cache *g_xe_blocks; // [k_xe_block_count]
t_list<s_xe_pipeline_entry> g_xe_pipelines;
s_map g_xe_pipeline_index;
// The entry of the last pipeline xe_pipeline returned (index + 1, 0: none): consecutive draws of one material ask for
// the same pipeline, which a compare with its key finds without hashing the key again.
uint32_t g_xe_last_entry;

// FNV-1a over 32-bit words (from `seed`), each step mixed down and the result finalized (murmur3's fmix32), so that
// fields differing only in their high bits (floats) still spread over the map's slots: per draw, a quarter of the
// byte loop's multiplies (gpu_hash_bytes).
uint32_t xe_hash_words(const void *data, size_t size, uint32_t seed = 2166136261u)
{
    const uint32_t *words = (const uint32_t *)data;
    uint32_t hash = seed;
    for (size_t i = 0; i < size / 4u; i++) {
        hash = (hash ^ words[i]) * 16777619u;
        hash ^= hash >> 15;
    }
    hash ^= hash >> 16;
    hash *= 0x85ebca6bu;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35u;
    hash ^= hash >> 16;
    return hash;
}
static_assert(sizeof(s_xe_pipeline_key) % 4u == 0, "xe_hash_words hashes whole words");
t_list<s_compute_entry> g_xe_compute;
t_list<s_xe_buffer> g_xe_buffers;
s_map g_xe_buffer_index;
t_list<s_xe_export> g_xe_exports;
t_list<s_xe_texture_set> g_xe_texture_sets;
s_map g_xe_texture_set_index; // hash -> head of collision chain (index + 1)
uint32_t g_xe_texture_set_epoch; // the backend view epoch the cached sets were made at (gpu_view_epoch)
// cached sets before they are all dropped
const uint32_t k_xe_texture_set_cap = 4096;

// Per-sampler-slot resolved view/type/sampler cache (keyed by draw->sampler_gen[s] and global epochs).
struct s_xe_slot_cache {
    uint32_t gen;
    uint32_t sampler_gen;
    uint32_t resolve_epoch;
    uint32_t view_epoch;
    uint32_t touch_epoch;
    uint32_t export_serial;
    uint32_t draw_scale;
    uint32_t actual_type;
    uint32_t resolved_index; // g_textures index + 1, or 0
    uint8_t view_valid;
    uint8_t array_view_valid;
    uint8_t skip_gamma;
    uint8_t sampler_valid_mask;
    uint8_t last_state_valid;
    s_port_vk_sampler_state last_state;
    gpu_view view;
    gpu_view array_view;
    gpu_sampler sampler[6];
    s_vulkan_bindless_token *view_token, *array_view_token;
    uint32_t sampler_entry[6];
};
s_xe_slot_cache g_xe_slot_cache[PORT_VK_SAMPLER_COUNT];

// 256-entry xe_texture_set() cache: short-circuits in O(1) when the bound shader pair's fetched slots did not change.
struct alignas(64) s_xe_last_texture_set {
    uint32_t valid;
    uint32_t meta_count;
    uint32_t meta_index[2];
    uint32_t fetch_mask;
    uint32_t samplers_any_gen;
    uint32_t resolve_epoch;
    uint32_t view_epoch;
    uint32_t touch_epoch;
    uint32_t export_serial;
    uint32_t draw_scale;
    uint32_t linear_gamma_mask;
    uint64_t bindless_recording, bindless_revision;
    gpu_bind_group set;
    uint32_t slot_gen[PORT_VK_SAMPLER_COUNT];
};
const uint32_t k_xe_tex_set_cache_size = 256u;
s_xe_last_texture_set g_xe_last_tex_set[k_xe_tex_set_cache_size];
s_xe_v6_texture_tables g_xe_v6_texture_tables[k_xe_tex_set_cache_size];

void xe_invalidate_sampler_caches()
{
    memset(g_xe_slot_cache, 0, sizeof(g_xe_slot_cache));
    memset(g_xe_last_tex_set, 0, sizeof(g_xe_last_tex_set));
}

// 64-entry (vs_index, declaration) template cache + per-template last static stream_gen cache for xe_vertex_fetch_fill().
// Each entry precomputes the stream-slot mapping, element unpack descriptors, and denominators once per (vs, decl,
// be_mask) and keeps `data` pre-zeroed so draws only update the 1-3 active `data.stream[]` entries (or hit directly
// with zero work when `stream_gen` also matches).
struct s_xe_vf_template {
    uint32_t valid;
    uint32_t vs_index;
    const void *declaration;
    uint32_t be_mask;           // bit s: stream s is big_endian or memexport (only bits in used_streams)
    uint32_t used_streams;      // bitmask of D3D streams 0..15 referenced by this (vs, decl)
    uint32_t slot_count;        // number of (stream, endian) slots (0..16)
    uint32_t export_slots;      // slots fetching from a memexport buffer
    uint8_t slot_stream[16];    // D3D stream index for each slot
    uint8_t first_stream_slot[16]; // indexed by D3D stream, shared upload for all its endian views in this draw
    uint8_t slot_endian[16];    // endian (0..3) for each slot
    uint32_t stream_valid;      // 1 if data.stream[] / views[] match stream_gen below
    uint32_t stream_gen;
    uint32_t export_serial;
    uint32_t touch_epoch;
    uint32_t vf_gen;
    struct {
        uint32_t frame, gen, offset;
        uint32_t rows[4];
    } ring;
    xe_vertex_fetch data;
    s_gpu_buffer *views[XE_D3D12_VERTEX_STREAMS];
};
const uint32_t k_xe_vf_template_size = 64u;
s_xe_vf_template g_xe_vf_templates[k_xe_vf_template_size];
uint32_t g_xe_vf_gen_counter = 1;
s_xe_vf_template *g_xe_last_vf_template;

// Exact compact static key. Blend controls contain only the bits consumed by build_state.
struct s_xe_static_state {
    uint32_t color_formats[4], depth_format;
    uint32_t blend[4], blend_factor, modes;

    bool operator==(const s_xe_static_state &rhs) const
    {
        return modes == rhs.modes && blend_factor == rhs.blend_factor && depth_format == rhs.depth_format &&
               !memcmp(blend, rhs.blend, sizeof(blend)) && !memcmp(color_formats, rhs.color_formats, sizeof(color_formats));
    }
};

// Hash the stable draw identity, then validate the pipeline's static state and refresh dynamic state separately.
struct s_xe_pipeline_fast_cache {
    const s_xe_rows_memo *live;
    uint32_t topology, restart, slots;
    uint32_t dynamic_gen, scale, pass_epoch;
    s_xe_static_state static_state;
    s_gpu_pipeline *pipeline;
    s_gpu_graphics_state state;
    s_vulkan_dynamic_state dynamic_state;
};
const uint32_t k_xe_pipe_fast_size = 256u;
s_xe_pipeline_fast_cache g_xe_pipe_fast[k_xe_pipe_fast_size];

inline s_xe_pipeline_fast_cache &xe_pipe_fast_slot(const s_xe_rows_memo &live, uint32_t topology, uint32_t slots,
                                                   bool restart)
{
    const uint32_t h = live.vs * 2654435761u ^ live.ps * 2246822519u ^ live.kind * 3266489917u ^
                       live.extra * 668265263u ^ (topology * 32u + slots * 2u + restart) * 0x165667b1u;
    return g_xe_pipe_fast[(h ^ (h >> 16)) & (k_xe_pipe_fast_size - 1u)];
}

void xe_bind_graphics_pipeline(const s_xe_pipeline_fast_cache &graphics)
{
    if (g_vk)
        vulkan_cmd_bind_pipeline_native(g_vk, vulkan_gpu_native_pipeline(graphics.pipeline), &graphics.dynamic_state);
    else
        gpu_cmd_bind_pipeline_state(g_gpu, graphics.pipeline, &graphics.state);
}
uint32_t g_xe_bool_loop_gen;
uint32_t g_xe_fetch_buf_gen;
uint32_t g_xe_fetch_resolve_epoch;
uint32_t g_xe_fetch_linear_mask;
uint32_t g_xe_fetch_linear_known;
struct {
    xe_fetch data;
    uint32_t gen, constant_slots, derived_slots, linear_mask;
} g_xe_fetch_shadow;

const s_shader_pack_xenos *xe_meta_ptr(s_shader_entry *entry)
{
    if (entry == 0)
        return 0;
    if (entry->xenos_cached == 1)
        return &entry->xenos_meta;
    if (entry->xenos_cached == 2)
        return 0;
    if (entry->xenos != SHADER_PACK_NOT_FOUND && shader_pack_get_xenos(g_pack, entry->xenos, &entry->xenos_meta)) {
        entry->xenos_cached = 1;
        return &entry->xenos_meta;
    }
    entry->xenos_cached = 2;
    return 0;
}

uint32_t xe_gpu_dword(const void *cpu)
{
    // GPU_CONVERT_CPU_TO_GPU_ADDRESS (d3d9gpu.h, GPU_ADDRESS_MASK 0x1fffffff), as the game builds its stream constants
    const uint32_t p = (uint32_t)(uintptr_t)cpu;
    return ((p & 0x1fffffffu) + (((p >> 20) + 0x200u) & 0x1000u)) >> 2;
}

// ---- the rows of set 0 a translated shader reads ----
//
// The translated shaders read set 0's blocks through access chains with constant indices (port/tools/xenos_spirv
// translate.odin, io.odin, helpers.odin: a float register c#, the dwords of a fetch constant, a vertex element's
// record, a system constant), except where the index is computed: a relatively addressed float register (c[a0 + n],
// c[aL + n], clamped to the 256 registers) and the stream record of a vertex element. Each module is walked once for
// the 16-byte rows of each block it may read: a constant index names its row, a computed one the whole array it
// indexes, and any other use of a block variable (a load of the whole block, a pointer passed on, an index or type the
// walk does not know) the whole block. xe_block compares and copies only the rows a draw's shaders read; the others
// may hold anything. A float register read by constant index is never one of the shader's literal constants (the
// translator inlines those), so the device's shadow serves such a shader as it is; a shader that also reads
// relatively has every register in its rows, and gets the literals written over the shadow (xe_bind).
struct s_xe_module_rows {
    uint32_t module;                  // pack module index
    s_xe_rows rows[k_xe_block_count]; // per binding of set 0 (e_xe_block)
};
t_list<s_xe_module_rows> g_xe_module_rows;
s_map g_xe_module_rows_index; // module index + 1 -> g_xe_module_rows index
void prewarm_lock();
void prewarm_unlock();

// Rows [offset, offset + size) of `block`; bytes past the block's end (or an unknown size, ~0u) mean all of it.
void xe_rows_add(s_xe_rows *rows, uint32_t block, uint32_t offset, uint32_t size)
{
    const uint32_t block_size = k_xe_block_size[block];
    if (size == ~0u || offset >= block_size || size > block_size - offset) {
        offset = 0;
        size = block_size;
    }
    if (size == 0)
        return;
    for (uint32_t r = offset / 16u; r <= (offset + size - 1u) / 16u; r++)
        rows->bits[r / 32u] |= 1u << (r % 32u);
}

struct s_spirv_id {
    uint16_t opcode;       // of the instruction that made it (a type, OpConstant, OpVariable), 0: none of those
    uint32_t a, b;         // int / float: width; vector: component type, count; array: element type, length id;
                           // struct: first member in the member lists, count; pointer: storage class, pointee;
                           // constant: type, value (its first word); variable: pointer type
    uint32_t binding, set; // OpDecorate Binding / DescriptorSet (+1; 0: none)
    uint32_t stride;       // OpDecorate ArrayStride
};

// The byte size of a type in a block (std140 offsets and strides as decorated), ~0u when the walk cannot tell.
uint32_t spirv_type_size(const s_spirv_id *ids, uint32_t bound, const uint32_t *member_type,
                         const uint32_t *member_offset, uint32_t type, uint32_t depth = 0)
{
    if (type >= bound || depth > 8)
        return ~0u;
    const s_spirv_id &t = ids[type];
    switch (t.opcode) {
    case 21: // OpTypeInt
    case 22: // OpTypeFloat
        return t.a / 8u;
    case 23: { // OpTypeVector
        const uint32_t component = spirv_type_size(ids, bound, member_type, member_offset, t.a, depth + 1);
        return component == ~0u ? ~0u : component * t.b;
    }
    case 28: { // OpTypeArray
        if (t.b >= bound || ids[t.b].opcode != 43)
            return ~0u;
        const uint32_t stride = t.stride != 0 ? t.stride
                                              : spirv_type_size(ids, bound, member_type, member_offset, t.a, depth + 1);
        return stride == ~0u || ids[t.b].b > 0xffffu ? ~0u : stride * ids[t.b].b;
    }
    case 30: { // OpTypeStruct
        uint32_t size = 0;
        for (uint32_t m = 0; m < t.b; m++) {
            const uint32_t member = spirv_type_size(ids, bound, member_type, member_offset, member_type[t.a + m], depth + 1);
            if (member == ~0u || member_offset[t.a + m] == ~0u)
                return ~0u;
            size = member_offset[t.a + m] + member > size ? member_offset[t.a + m] + member : size;
        }
        return size;
    }
    default: return ~0u;
    }
}

// Whether word i of a function-body instruction may be an id: false for the literal operands of the instructions
// translated shaders are full of (an extended instruction's number, shuffle and composite indices, memory operands,
// merge controls, switch literals, debug lines), so that a literal equal to a block variable's id does not count as
// a use of it. Any other instruction's words all count (a literal mistaken for an id only costs rows).
bool spirv_operand_may_be_id(uint32_t opcode, uint32_t i)
{
    switch (opcode) {
    case 8:   // OpLine: file, line, column
    case 317: // OpNoLine
        return false;
    case 12: return i != 4u;           // OpExtInst: result type, result, set, instruction number, operands
    case 59: return i == 1u || i == 4u; // OpVariable: result type, result, storage class, initializer
    case 61: return i <= 3u;           // OpLoad: result type, result, pointer, memory operands
    case 62: return i <= 2u;           // OpStore: pointer, object, memory operands
    case 79: return i <= 4u;           // OpVectorShuffle: result type, result, two vectors, component literals
    case 81: return i <= 3u;           // OpCompositeExtract: result type, result, composite, index literals
    case 82: return i <= 4u;           // OpCompositeInsert: result type, result, object, composite, index literals
    case 246: return i <= 2u;          // OpLoopMerge: merge block, continue target, loop control
    case 247: return i <= 1u;          // OpSelectionMerge: merge block, selection control
    case 250: return i <= 3u;          // OpBranchConditional: condition, two labels, branch weights
    case 251: return i <= 2u || i % 2u == 0u; // OpSwitch: selector, default, (literal, label) pairs (32-bit selector)
    default: return true;
    }
}


// The rows of each block a module reads (all of them when the module cannot be walked).
void xe_scan_module_rows(const uint32_t *code, size_t words, s_xe_rows rows[k_xe_block_count])
{
    memset(rows, 0, sizeof(s_xe_rows) * k_xe_block_count);
    const uint32_t bound = code != 0 && words >= 5 && code[0] == 0x07230203u ? code[3] : 0u;
    s_spirv_id *ids = bound != 0 && bound < (1u << 22) ? (s_spirv_id *)calloc(bound, sizeof(s_spirv_id)) : 0;
    // OpTypeStruct members (type, Offset), and the OpMemberDecorate Offsets they take theirs from
    struct s_member_offset {
        uint32_t type, member, offset;
    };
    t_list<uint32_t> member_type = {}, member_offset = {};
    t_list<s_member_offset> decorated = {};
    bool ok = ids != 0;
    for (size_t w = 5; ok && w < words;) {
        const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
        if (length == 0 || w + length > words) {
            ok = false;
            break;
        }
        const uint32_t *op = code + w;
        if (opcode == 54u) // OpFunction: the declarations are done
            break;
        if (opcode == 71u && length >= 4u && op[1] < bound) { // OpDecorate: Binding, DescriptorSet, ArrayStride
            if (op[2] == 33u)
                ids[op[1]].binding = op[3] + 1u;
            else if (op[2] == 34u)
                ids[op[1]].set = op[3] + 1u;
            else if (op[2] == 6u)
                ids[op[1]].stride = op[3];
        } else if (opcode == 72u && length >= 5u && op[3] == 35u) { // OpMemberDecorate Offset
            s_member_offset *d = decorated.push();
            ok = d != 0;
            if (ok) {
                d->type = op[1];
                d->member = op[2];
                d->offset = op[4];
            }
        } else if ((opcode == 21u || opcode == 22u) && length >= 3u && op[1] < bound) {
            ids[op[1]].opcode = (uint16_t)opcode;
            ids[op[1]].a = op[2];
        } else if ((opcode == 23u || opcode == 28u || opcode == 32u) && length >= 4u && op[1] < bound) {
            ids[op[1]].opcode = (uint16_t)opcode;
            ids[op[1]].a = op[2];
            ids[op[1]].b = op[3];
        } else if (opcode == 30u && length >= 2u && op[1] < bound) { // OpTypeStruct
            ids[op[1]].opcode = 30;
            ids[op[1]].a = member_type.count;
            ids[op[1]].b = length - 2u;
            for (uint32_t m = 0; ok && m < length - 2u; m++) {
                uint32_t *t = member_type.push(), *o = t != 0 ? member_offset.push() : 0;
                ok = o != 0;
                if (ok) {
                    *t = op[2 + m];
                    *o = ~0u;
                    for (uint32_t i = 0; i < decorated.count; i++)
                        if (decorated.items[i].type == op[1] && decorated.items[i].member == m)
                            *o = decorated.items[i].offset;
                }
            }
        } else if (opcode == 43u && length >= 4u && op[2] < bound) { // OpConstant
            ids[op[2]].opcode = 43;
            ids[op[2]].a = op[1];
            ids[op[2]].b = op[3];
        } else if (opcode == 59u && length >= 4u && op[2] < bound) { // OpVariable
            ids[op[2]].opcode = 59;
            ids[op[2]].a = op[1];
            ids[op[2]].b = op[3];
        }
        w += length;
    }
    // the block a variable is: Uniform storage, set 0, a binding of e_xe_block
    auto block_of = [&](uint32_t id) -> uint32_t {
        if (id >= bound || ids[id].opcode != 59 || ids[id].b != 2u /* Uniform */ || ids[id].set != 1u ||
            ids[id].binding == 0 || ids[id].binding > k_xe_block_count)
            return ~0u;
        return ids[id].binding - 1u;
    };
    bool in_function = false;
    for (size_t w = 5; ok && w < words;) {
        const uint32_t opcode = code[w] & 0xffffu, length = code[w] >> 16;
        if (length == 0 || w + length > words) {
            ok = false;
            break;
        }
        const uint32_t *op = code + w;
        w += length;
        in_function |= opcode == 54u;
        if (!in_function)
            continue; // decorations, names and the entry point's interface list name the variables without reading
        const uint32_t block = (opcode == 65u || opcode == 66u) && length >= 4u ? block_of(op[3]) : ~0u;
        if (block == ~0u) {
            // any other instruction naming a block variable (or an operand the walk cannot tell from an id that
            // happens to equal one): the whole block
            for (uint32_t i = 1; i < length; i++) {
                const uint32_t b = spirv_operand_may_be_id(opcode, i) ? block_of(op[i]) : ~0u;
                if (b != ~0u)
                    xe_rows_add(&rows[b], b, 0, ~0u);
            }
            continue;
        }
        // OpAccessChain / OpInBoundsAccessChain into the block: walk its indices from the block's struct
        const uint32_t pointer = ids[op[3]].a;
        uint32_t type = pointer < bound && ids[pointer].opcode == 32 ? ids[pointer].b : ~0u, offset = 0, size = ~0u;
        bool walked = type != ~0u;
        for (uint32_t i = 4; walked && i < length; i++) {
            if (type >= bound) {
                walked = false;
                break;
            }
            const s_spirv_id &t = ids[type];
            const uint32_t index = op[i];
            const bool constant = index < bound && ids[index].opcode == 43;
            const uint32_t value = constant ? ids[index].b : 0u;
            if (t.opcode == 30) { // a struct member: always a constant index
                walked = constant && value < t.b && member_offset.items[t.a + value] != ~0u;
                if (walked) {
                    offset += member_offset.items[t.a + value];
                    type = member_type.items[t.a + value];
                }
            } else if (t.opcode == 28 || t.opcode == 23) { // an array element or a vector component
                const uint32_t count = t.opcode == 23 ? t.b : t.b < bound && ids[t.b].opcode == 43 ? ids[t.b].b : 0u;
                const uint32_t stride = t.opcode == 28 && t.stride != 0
                                            ? t.stride
                                            : spirv_type_size(ids, bound, member_type.items, member_offset.items, t.a);
                walked = count != 0 && stride != ~0u && stride <= 4096u && count <= 4096u;
                if (walked && !constant) {
                    size = count * stride; // a computed index: the whole array
                    break;
                }
                walked = walked && value < count;
                if (walked) {
                    offset += value * stride;
                    type = t.a;
                }
            } else {
                walked = false;
            }
        }
        if (walked && size == ~0u)
            size = spirv_type_size(ids, bound, member_type.items, member_offset.items, type);
        xe_rows_add(&rows[block], block, walked ? offset : 0u, walked ? size : ~0u);
    }
    if (!ok)
        for (uint32_t b = 0; b < k_xe_block_count; b++)
            xe_rows_add(&rows[b], b, 0, ~0u);
    member_type.release();
    member_offset.release();
    decorated.release();
    free(ids);
}

// The rows a pack module reads (walked on its first use). SPIR-V only: a DXIL module (version 4) is not walked, so its
// draws upload every row of the blocks (NULL, as for a module that cannot be walked).
const s_xe_rows *xe_module_rows(uint32_t module)
{
    if (module == SHADER_PACK_NOT_FOUND || shader_pack_module_format(g_pack) != SHADER_PACK_MODULE_FORMAT_SPIRV)
        return 0;
    uint32_t index;
    if (map_find(&g_xe_module_rows_index, module + 1u, &index))
        return g_xe_module_rows.items[index].rows;
    s_xe_module_rows *entry = g_xe_module_rows.push();
    if (entry == 0)
        return 0;
    entry->module = module;
    size_t size = 0;
    prewarm_lock(); // the prewarm workers load modules too (gpu_pack_xenos_module)
    const uint32_t *code = shader_pack_module_code(g_pack, module, &size);
    prewarm_unlock();
    xe_scan_module_rows(code, code != 0 ? size / 4u : 0u, entry->rows);
    map_set(&g_xe_module_rows_index, module + 1u, g_xe_module_rows.count - 1u);
    return entry->rows;
}

// The direct cache avoids the map lookup on a hit. Interned layouts survive collisions and have stable addresses.
const uint32_t k_xe_rows_memo_size = 256u;
const s_xe_rows_memo *g_xe_rows_memo[k_xe_rows_memo_size];
t_list<s_xe_rows_memo *> g_xe_rows_layouts;
s_map g_xe_rows_index;

struct s_xe_offset_tuple {
    uint32_t frame;
    const s_xe_rows_memo *layout;
    uint32_t gens[k_xe_block_count];
    uint32_t offsets[k_xe_block_count];
    uint32_t resolve_epoch, linear_gamma_mask;
    uint32_t reusable;
};
s_xe_offset_tuple g_xe_offsets;

void xe_invalidate_draw_caches()
{
    g_xe_last_vf_template = 0;
    memset(g_xe_vf_templates, 0, sizeof(g_xe_vf_templates));
    memset(g_xe_pipe_fast, 0, sizeof(g_xe_pipe_fast));
    xe_invalidate_sampler_caches();
    g_xe_offsets.frame = 0;
    if (g_xe_abi6)
        for (auto &arena : g_xe_v6_arenas)
            xe_v6_reset_banks(arena);
    g_xe_sys_cache.valid = 0;
    g_xe_bool_loop_gen = 0;
    g_xe_fetch_buf_gen = 0;
    g_xe_fetch_linear_known = 0;
    g_xe_fetch_shadow.gen = 0;
    g_vp_cache.frame = 0;
    if (g_xe_blocks != 0)
        memset(g_xe_blocks, 0, k_xe_block_count * sizeof(*g_xe_blocks));
}

void xe_v6_rows_of_module(const s_shader_pack_xenos_module_meta &meta, s_xe_rows rows[k_xe_block_count])
{
    rows[k_xe_block_system].bits[0] |= meta.system_rows;
    for (uint32_t i = 0; i < 8; ++i)
        if (meta.bool_mask[i])
            rows[k_xe_block_bool_loop].bits[0] |= 1u << (i / 4u);
    for (uint32_t i = 0; i < 8; ++i)
        if (meta.loop_mask & (15u << (i * 4u)))
            rows[k_xe_block_bool_loop].bits[0] |= 1u << (2u + i);
    for (uint32_t i = 0; i < 3; ++i)
        rows[k_xe_block_fetch].bits[i] |= meta.fetch_rows[i];
    for (uint32_t i = 0; i < 4; ++i)
        rows[k_xe_block_vertex_fetch].bits[i] |= meta.vertex_fetch_rows[i];
}

uint32_t xe_fetch_slots(const s_xe_rows &rows)
{
    uint32_t slots = (rows.bits[1] >> 16) | (rows.bits[2] << 16);
    for (uint32_t n = 0; n < 32; ++n) {
        // Two six-dword fetch constants occupy three rows. Each spans a shared row.
        const uint32_t lo = 6u * n / 4u, hi = (6u * n + 5u) / 4u;
        if ((rows.bits[lo >> 5] & (1u << (lo & 31u))) || (rows.bits[hi >> 5] & (1u << (hi & 31u))))
            slots |= 1u << n;
    }
    return slots;
}

void xe_v6_float_rows(const s_shader_pack_xenos *shader, s_xe_rows &rows)
{
    if (!shader)
        return;
    const auto &meta = *shader->module_meta;
    memcpy(rows.bits, meta.direct_float_mask, sizeof(rows.bits));
    for (uint32_t r = meta.relative_lo; r < meta.relative_hi_exclusive; ++r)
        rows.bits[r >> 5] |= 1u << (r & 31u);
}

void xe_rows_of_module(uint32_t module, s_xe_rows rows[k_xe_block_count])
{
    if (g_xe_abi6) {
        if (const auto *meta = shader_pack_xenos_get_module_meta(g_pack, module))
            xe_v6_rows_of_module(*meta, rows);
        return;
    }
    const s_xe_rows *module_rows = xe_module_rows(module);
    for (uint32_t b = 0; b < k_xe_block_count; b++) {
        if (module_rows == 0) {
            xe_rows_add(&rows[b], b, 0, ~0u);
            continue;
        }
        for (uint32_t i = 0; i < k_xe_block_rows / 32u; i++)
            rows[b].bits[i] |= module_rows[b].bits[i];
    }
}

uint32_t xe_rows_next_run(const s_xe_rows &rows, uint32_t end, uint32_t *row);

// The rows a draw's pipeline reads: its translated shaders' and its helper stages' (the tessellation helpers, the
// rectangle-list geometry shader; vs: the vertex, tessellation evaluation or compute shader).
const s_xe_rows_memo *xe_pipeline_rows(uint32_t kind, const s_shader_pack_xenos *vs, const s_shader_pack_xenos *ps,
                                      uint32_t extra, s_shader_entry *vs_entry, s_shader_entry *ps_entry)
{
    // Shaders that share a module can still carry different literals, so key by shader index.
    const uint32_t ps_index = ps != 0 ? ps->index : SHADER_PACK_NOT_FOUND;
    uint32_t hash = (vs->index * 2654435761u) ^ (ps_index * 2246822519u) ^
                    (kind * 3266489917u) ^ (extra * 668265263u);
    hash = hash ? hash : 1u;
    const s_xe_rows_memo *&hot = g_xe_rows_memo[(hash ^ (hash >> 16)) & (k_xe_rows_memo_size - 1u)];
    auto matches = [&](const s_xe_rows_memo *layout) {
        return layout && layout->kind == kind && layout->vs == vs->index && layout->ps == ps_index &&
               layout->extra == extra;
    };
    if (matches(hot))
        return hot;
    uint32_t *head = map_slot(&g_xe_rows_index, hash, true);
    if (!head)
        return 0;
    for (uint32_t i = *head; i != 0;) {
        const auto *layout = g_xe_rows_layouts.items[i - 1u];
        if (matches(layout))
            return hot = layout;
        i = layout->next;
    }
    s_xe_rows_memo memo = {};
    xe_rows_of_module(vs->module_index, memo.rows);
    if (ps != 0)
        xe_rows_of_module(ps->module_index, memo.rows);
    if (xe_kind_has_geometry(kind))
        xe_rows_of_module(extra, memo.rows);
    if (kind == k_xe_pipeline_tessellated)
        for (uint32_t helper = 0; helper < 2; helper++)
            xe_rows_of_module(shader_pack_xenos_helper(g_pack, helper), memo.rows);
    if (g_xe_abi6) {
        xe_v6_float_rows(vs, memo.rows[k_xe_block_float_vs]);
        xe_v6_float_rows(ps, memo.rows[k_xe_block_float_ps]);
    }
    memo.fetch_slots = xe_fetch_slots(memo.rows[k_xe_block_fetch]);
    uint32_t run_count = 0;
    for (uint32_t b = 0; b < k_xe_block_count; b++) {
        memo.rows[b].chunk_mask = xe_rows_compute_chunk_mask(memo.rows[b].bits);
        memo.first_run[b] = (uint8_t)run_count;
        if (b != k_xe_block_float_vs && b != k_xe_block_float_ps) {
            for (uint32_t r = 0, n; (n = xe_rows_next_run(memo.rows[b], k_xe_block_size[b] / 16u, &r)); r += n)
                memo.runs[run_count++] = { (uint8_t)r, (uint8_t)n };
        }
    }
    memo.first_run[k_xe_block_count] = (uint8_t)run_count;
    const s_shader_pack_xenos *metas[2] = { vs, ps };
    s_shader_entry *entries[2] = { vs_entry, ps_entry };
    for (uint32_t s = 0; s < 2; s++) {
        const s_xe_rows &rows = memo.rows[k_xe_block_float_vs + s];
        memo.literals[s] = 1;
        memo.shadow_chunks[s] = rows.chunk_mask;
        if (rows.chunk_mask == 0)
            continue;
        uint32_t index;
        const uint32_t *mask;
        if (g_xe_abi6) {
            index = metas[s]->index + 2u;
            mask = metas[s]->overlay->literal_row_mask;
            for (uint32_t i = 0; i < 8; ++i)
                if (rows.bits[i])
                    memo.v6_float[s].table_lines |= 1u << (i / 4u);
        } else {
            index = xe_float_literal_index(entries[s], metas[s]);
            if (index == 0)
                return 0;
            const s_xe_dedup_literals *literals = xe_literals_at(index);
            if (literals == 0 || (rows.chunk_mask & literals->mask.chunk_mask) == 0)
                continue;
            mask = literals->mask.bits;
        }
        uint32_t overlap[k_xe_block_rows / 32u];
        uint32_t shadow[k_xe_block_rows / 32u];
        for (uint32_t i = 0; i < k_xe_block_rows / 32u; i++) {
            overlap[i] = rows.bits[i] & mask[i];
            shadow[i] = rows.bits[i] & ~mask[i];
        }
        const uint64_t overlays = xe_rows_compute_chunk_mask(overlap);
        if (overlays != 0) {
            memo.literals[s] = index;
            memo.shadow_chunks[s] = xe_rows_compute_chunk_mask(shadow);
            if (g_xe_abi6) {
                auto &layout = memo.v6_float[s];
                layout.overlay_mask = overlays;
                for (uint32_t i = 0; i < metas[s]->def_count;) {
                    const uint32_t c = metas[s]->defs[i].reg >> 2;
                    layout.first_def[c] = (uint8_t)i++;
                    while (i < metas[s]->def_count && (metas[s]->defs[i].reg >> 2) == c)
                        ++i;
                }
            }
        }
    }
    memo.kind = kind;
    memo.vs = vs->index;
    memo.ps = ps_index;
    memo.extra = extra;
    memo.next = *head;
    auto *layout = (s_xe_rows_memo *)malloc(sizeof(memo));
    if (!layout)
        return 0;
    s_xe_rows_memo **entry = g_xe_rows_layouts.push();
    if (!entry) {
        free(layout);
        return 0;
    }
    *layout = memo;
    *entry = layout;
    *head = g_xe_rows_layouts.count;
    return hot = layout;
}


// The universal layout (the backend's: gpu_xenos_layout), the ring's constant sets, and the features the translated
// shaders need.
bool xenos_startup()
{
    const s_gpu_caps &caps = g_gpu->caps;
    if (!caps.clip_distance) {
        log_text("the device has no clip distances: the Xenos shader pack cannot be used\n");
        return false;
    }
    if (!caps.geometry)
        log_text("no geometry shaders: rectangle lists are skipped with the Xenos shader pack\n");
    if (!caps.tessellation)
        log_text("no tessellation shaders: tessellated water is skipped with the Xenos shader pack\n");
    else if (!caps.full_draw_index)
        log_text("no full 32-bit index values (the water factors are 32-bit index values): tessellated water is "
                 "skipped with the Xenos shader pack\n");
    // the backend checks its descriptor limits against the layout's (192 samplers, 128 sampled images, 6 dynamic
    // uniform buffers) and logs why it cannot make it
    s_gpu_xenos_layout layout = {};
    if (!gpu_xenos_layout(g_gpu, g_pack, &layout))
        return false;
    g_xe_set_layout[XE_SET_CONSTANTS] = layout.constants;
    g_xe_set_layout[XE_SET_TEXTURES] = layout.textures;
    g_xe_layout = layout.layout;
    g_xe_device_addresses = caps.buffer_device_address != 0;
    for (uint32_t i = 0; i < caps.frames_in_flight; i++) {
        g_xe_constant_set[i] = gpu_bind_group_allocate(g_gpu, g_xe_set_layout[XE_SET_CONSTANTS], k_gpu_bind_persistent);
        if (g_xe_constant_set[i] == GPU_NULL_HANDLE)
            return false;
        for (uint32_t b = 0; b < k_xe_block_count; b++) {
            if (!g_xe_abi6 || (b != k_xe_block_float_vs && b != k_xe_block_float_ps))
                gpu_bind_write_buffer(g_gpu, g_xe_constant_set[i], b, 0, k_gpu_binding_uniform_buffer_dynamic,
                                      g_rings[i].buffer, 0, k_xe_block_size[b]);
        }
        if (g_xe_device_addresses)
            gpu_buffer_device_address(g_gpu, g_rings[i].buffer, &g_xe_ring_address[i][0], &g_xe_ring_address[i][1]);
    }
    g_xe_blocks = (s_xe_block_cache *)calloc(k_xe_block_count, sizeof(s_xe_block_cache));
    if (g_xe_blocks == 0)
        return false;
    if (g_xe_abi6) {
        const s_gpu_image *dummies[] = { g_dummy_2d, g_dummy_3d, g_dummy_cube, g_dummy_array };
        for (uint32_t i = 0; i < 4; ++i) {
            if (!dummies[i])
                return false;
            const gpu_view view = gpu_image_view(g_gpu, dummies[i]);
            if (view == GPU_NULL_HANDLE)
                return false;
            vulkan_bindless_set_dummy(g_vk, i, vulkan_gpu_detail::handle_cast<uint64_t>(view));
        }
        vulkan_bindless_set_dummy(g_vk, 4, vulkan_gpu_detail::handle_cast<uint64_t>(g_default_sampler));
    }
    prewarm_start(); // pipeline libraries (when the device has them) and their workers
    return true;
}

void xenos_shutdown()
{
    prewarm_stop(); // the workers first: they use the pack's modules and the layout
    if (g_xe_abi6) {
        vulkan_bindless_destroy(g_vk);
        xe_v6_release_arenas(g_vk);
    }
#ifdef HALO3_64BIT
    xe_readback_shutdown();
#endif
    for (uint32_t i = 0; i < g_xe_pipelines.count; i++)
        gpu_pipeline_destroy(g_gpu, g_xe_pipelines.items[i].pipeline);
    for (uint32_t i = 0; i < g_xe_compute.count; i++)
        gpu_pipeline_destroy(g_gpu, g_xe_compute.items[i].pipeline);
    for (uint32_t i = 0; i < g_xe_buffers.count; i++)
        gpu_buffer_destroy(g_gpu, g_xe_buffers.items[i].buffer);
    for (uint32_t i = 0; i < g_xe_exports.count; i++)
        gpu_buffer_destroy(g_gpu, g_xe_exports.items[i].buffer);
    for (s_xe_export_view &view : g_xe_export_views)
        if (view.image != 0)
            gpu_image_destroy(g_gpu, view.image);
    memset(g_xe_export_views, 0, sizeof(g_xe_export_views));
    g_xe_texture_exports = 0;
    for (uint32_t i = 0; i < GPU_FRAMES_MAX; i++)
        if (g_xe_constant_set[i] != GPU_NULL_HANDLE)
            gpu_bind_group_free(g_gpu, g_xe_constant_set[i]);
    memset(g_xe_constant_set, 0, sizeof(g_xe_constant_set));
    g_xe_pipelines.release();
    g_xe_compute.release();
    g_xe_buffers.release();
    g_xe_exports.release();
    if (g_gpu != 0)
        gpu_bind_groups_reset_cached(g_gpu);
    g_xe_texture_sets.release();
    g_xe_texture_set_epoch = 0;
    map_free(&g_xe_pipeline_index);
    g_xe_last_entry = 0;
    map_free(&g_xe_buffer_index);
    map_free(&g_xe_texture_set_index);
    g_xe_module_rows.release();
    map_free(&g_xe_module_rows_index);
    for (uint32_t i = 0; i < g_xe_dedup_literals.count; i++)
        free(g_xe_dedup_literals.items[i].defs);
    g_xe_dedup_literals.release();
    map_free(&g_xe_dedup_literals_index);
    for (uint32_t i = 0; i < g_shaders.count; i++) {
        g_shaders.items[i].xenos_cached = 0;
        g_shaders.items[i].dedup_index = 0;
        g_shaders.items[i].xenos_compute = 0;
    }
    memset(&g_xe_sys_cache, 0, sizeof(g_xe_sys_cache));
    g_pass_color_aliases_depth = false;
    g_depth_resolve_edram_count = 0;
    g_depth_resolve_edram_overflow = false;
    memset(g_xe_rows_memo, 0, sizeof(g_xe_rows_memo));
    for (uint32_t i = 0; i < g_xe_rows_layouts.count; ++i)
        free(g_xe_rows_layouts.items[i]);
    g_xe_rows_layouts.release();
    map_free(&g_xe_rows_index);
    xe_invalidate_draw_caches();
    free(g_xe_blocks);
    g_xe_blocks = 0;
    g_xe_layout = GPU_NULL_HANDLE;
    g_xe_set_layout[0] = g_xe_set_layout[1] = GPU_NULL_HANDLE;
    g_xenos = false;
}

// The next run of rows at or after *row and before `end`: its first row in *row, its length returned (0: none).
uint32_t xe_rows_next_run(const s_xe_rows &rows, uint32_t end, uint32_t *row)
{
    uint32_t r = *row;
    while (r < end) {
        const uint32_t set = rows.bits[r / 32u] >> (r % 32u);
        if (set != 0) {
            r += (uint32_t)__builtin_ctz(set);
            break;
        }
        r = (r / 32u + 1u) * 32u;
    }
    if (r >= end)
        return 0;
    uint32_t e = r;
    while (e < end) {
        const uint32_t clear = ~rows.bits[e / 32u] >> (e % 32u); // zero past the word's end: the run goes on
        if (clear != 0) {
            e += (uint32_t)__builtin_ctz(clear);
            break;
        }
        e = (e / 32u + 1u) * 32u;
    }
    *row = r;
    return (e < end ? e : end) - r;
}

// Copying and comparing whole rows: a run of a few rows (a register or two, the common case) moved 16 bytes at a time
// in place, a longer one through memcpy / memcmp (a call per run of a few rows cost more than the bytes).
typedef uint64_t t_xe_row64 __attribute__((vector_size(16)));
inline void xe_rows_copy(uint8_t *dst, const uint8_t *src, uint32_t rows)
{
    if (rows == 1u) {
        __builtin_memcpy_inline(dst, src, 16);
        return;
    }
    copy_upload_bytes(dst, src, 16u * rows);
}

inline void xe_rows_copy_both(uint8_t *dst, uint8_t *cache_dst, const uint8_t *src, uint32_t rows)
{
    if (rows == 1u) {
        t_xe_row64 row;
        memcpy(&row, src, 16);
        memcpy(dst, &row, 16);
        memcpy(cache_dst, &row, 16);
        return;
    }
    if (rows > 16u) {
        copy_upload_bytes(dst, src, 16u * rows);
        memcpy(cache_dst, src, 16u * rows);
        return;
    }
    for (uint32_t i = 0; i < rows; i++) {
        t_xe_row64 row;
        memcpy(&row, src + 16u * i, 16);
        memcpy(dst + 16u * i, &row, 16);
        memcpy(cache_dst + 16u * i, &row, 16);
    }
}

inline bool xe_rows_equal(const uint8_t *a, const uint8_t *b, uint32_t rows)
{
    if (rows == 1u) {
        uint64_t a0, a1, b0, b1;
        memcpy(&a0, a, 8);
        memcpy(&a1, a + 8, 8);
        memcpy(&b0, b, 8);
        memcpy(&b1, b + 8, 8);
        return ((a0 ^ b0) | (a1 ^ b1)) == 0;
    }
    if (rows > 4u)
        return memcmp(a, b, 16u * rows) == 0;
    t_xe_row64 differ = {};
    for (uint32_t i = 0; i < rows; i++) {
        t_xe_row64 x, y;
        memcpy(&x, a + 16u * i, 16);
        memcpy(&y, b + 16u * i, 16);
        differ |= x ^ y;
    }
    return (differ[0] | differ[1]) == 0;
}

// Precomputed deduplicated literal table for a Xenos shader (the last literal of a register wins).
const s_xe_dedup_literals *xe_dedup_literals(const s_shader_pack_xenos *meta)
{
    if (meta == 0 || meta->def_count == 0)
        return 0;
    uint32_t index = 0;
    if (map_find(&g_xe_dedup_literals_index, meta->index + 1u, &index))
        return &g_xe_dedup_literals.items[index];
    s_xe_dedup_literals *entry = g_xe_dedup_literals.push();
    if (entry == 0)
        return 0;
    entry->xenos_index = meta->index;
    // Deduplicate: last literal targeting each register (< 256) wins.
    int32_t slot_of_reg[256];
    for (uint32_t r = 0; r < 256; r++)
        slot_of_reg[r] = -1;
    s_xe_dedup_def temp[256];
    uint32_t count = 0;
    for (uint32_t d = 0; d < meta->def_count; d++) {
        const uint32_t reg = meta->defs[d].reg;
        if (reg >= 256u)
            continue;
        entry->mask.bits[reg / 32u] |= 1u << (reg % 32u);
        int32_t slot = slot_of_reg[reg];
        if (slot < 0) {
            slot = (int32_t)count++;
            slot_of_reg[reg] = slot;
            temp[slot].reg = reg;
        }
        memcpy(temp[slot].value, meta->defs[d].value, 16);
    }
    entry->mask.chunk_mask = xe_rows_compute_chunk_mask(entry->mask.bits);
    if (count != 0) {
        entry->defs = (s_xe_dedup_def *)malloc(count * sizeof(s_xe_dedup_def));
        if (entry->defs == 0) {
            g_xe_dedup_literals.count--;
            return 0;
        }
        memcpy(entry->defs, temp, count * sizeof(s_xe_dedup_def));
        entry->count = count;
    }
    map_set(&g_xe_dedup_literals_index, meta->index + 1u, g_xe_dedup_literals.count - 1u);
    return entry;
}

// A constant block in this frame's ring; its offset for the dynamic binding. Only `rows` of it are read (the draw's
// shaders', xe_pipeline_rows). `literals`: precomputed deduplicated literal constants of a shader that addresses its
// float registers relatively, whose registers hold them instead of `data`'s.
// When `gen != 0`, the caller tracks mutations via a generation counter: `gen == cache.last_gen` hits in O(1), and
// `gen != cache.last_gen` checks per-4-register `chunk_gen` when provided (skipping ring alloc + copy when none of the
// shader's active chunks changed) and otherwise writes directly to the ring staging buffer without cache.data copies.
bool xe_block(uint32_t block, const void *data, const s_xe_rows_memo &live, uint32_t *out_offset,
              const s_xe_dedup_literals *literals = 0, uint32_t gen = 0, const uint32_t *chunk_gen = 0,
              uint64_t shadow_chunks = 0)
{
    const s_xe_rows &rows = live.rows[block];
    // These descriptors still need an in-range offset even when the pipeline reads no rows.
    if (rows.chunk_mask == 0) {
        *out_offset = 0;
        return true;
    }
    s_xe_block_cache &cache = g_xe_blocks[block];
    const uint32_t size = k_xe_block_size[block], end = size / 16u, words = (end + 31u) / 32u;
    const uint8_t *source = (const uint8_t *)data;
    const s_xe_row_run *first = live.runs + live.first_run[block];
    const s_xe_row_run *last = live.runs + live.first_run[block + 1u];
    if (gen != 0) {
        const uint32_t literals_id = literals != 0 ? literals->xenos_index : SHADER_PACK_NOT_FOUND;
        const bool compatible = cache.frame == g_frame_serial && cache.literals_id == literals_id;
        const bool same_gen = compatible && cache.last_gen == gen;
        if (same_gen && cache.covered_rows == &rows) {
            *out_offset = cache.offset;
            return true;
        }
        if (compatible) {
            uint32_t missing = 0;
            for (uint32_t i = 0; i < words; i++)
                missing |= rows.bits[i] & ~cache.valid.bits[i];
            bool same_chunks = chunk_gen != 0 && cache.chunk_gen_valid != 0;
            if (missing == 0 && !same_gen && same_chunks) {
                const unsigned stage = block - k_xe_block_float_vs;
                if (stage < 2 && g_record_packet && chunk_gen == g_decoded.banks[stage].chunks &&
                    cache.last_gen == g_record_packet->changes.float_gen[stage]) {
                    same_chunks = !(shadow_chunks & g_record_packet->changes.chunks[stage]);
                } else {
                    for (uint64_t mask = shadow_chunks; mask != 0 && same_chunks; mask &= mask - 1u) {
                        const uint32_t chunk = (uint32_t)__builtin_ctzll(mask);
                        same_chunks = chunk_gen[chunk] == cache.chunk_gen[chunk];
                    }
                }
            }
            if (missing == 0 && (same_gen || same_chunks)) {
                cache.covered_rows = &rows;
                *out_offset = cache.offset;
                return true;
            }
        }
        uint8_t *dst = (uint8_t *)ring_alloc(size, g_uniform_alignment, out_offset);
        if (dst == 0)
            return false;
        // Preserve prior rows when the source is still at the same generation. A chunk hit leaves the generation
        // intact so another shader reading a changed row will still refresh it.
        const bool carry = same_gen;
        if (!carry && first != last) {
            memcpy(cache.valid.bits, rows.bits, words * sizeof(uint32_t));
            for (auto run = first; run != last; ++run)
                xe_rows_copy(dst + 16u * run->first, source + 16u * run->first, run->count);
        } else {
            [[clang::uninitialized]] s_xe_rows plain;
            for (uint32_t i = 0; i < words; i++) {
                const uint32_t bits = rows.bits[i] | (carry ? cache.valid.bits[i] : 0u);
                cache.valid.bits[i] = bits;
                plain.bits[i] = literals != 0 ? bits & ~literals->mask.bits[i] : bits;
            }
            for (uint32_t r = 0, n; (n = xe_rows_next_run(plain, end, &r)) != 0; r += n)
                xe_rows_copy(dst + 16u * r, source + 16u * r, n);
        }
        if (literals != 0) {
            for (uint32_t d = 0; d < literals->count; d++) {
                const s_xe_dedup_def &def = literals->defs[d];
                if ((cache.valid.bits[def.reg / 32u] & (1u << (def.reg % 32u))) != 0)
                    memcpy(dst + 16u * def.reg, def.value, 16);
            }
        }
        cache.valid.chunk_mask = rows.chunk_mask | (carry ? cache.valid.chunk_mask : 0u);
        if (chunk_gen != 0) {
            for (uint64_t mask = cache.valid.chunk_mask; mask != 0; mask &= mask - 1u) {
                const uint32_t chunk = (uint32_t)__builtin_ctzll(mask);
                cache.chunk_gen[chunk] = chunk_gen[chunk];
            }
        }
        cache.frame = g_frame_serial;
        cache.offset = *out_offset;
        cache.last_gen = gen;
        cache.covered_rows = &rows;
        cache.literals_id = literals_id;
        cache.data_valid = 0;
        cache.chunk_gen_valid = chunk_gen != 0;
        return true;
    }
    if (literals == 0) {
        bool carry = false;
        if (cache.frame == g_frame_serial && cache.data_valid && first != last && cache.covered_rows == &rows) {
            auto run = first;
            for (; run != last; ++run)
                if (!xe_rows_equal(cache.data + 16u * run->first, source + 16u * run->first, run->count))
                    break;
            if (run == last) {
                *out_offset = cache.offset;
                return true;
            }
        } else if (cache.frame == g_frame_serial) {
            uint32_t missing = 0, any_common = 0;
            [[clang::uninitialized]] s_xe_rows common;
            for (uint32_t i = 0; i < words; i++) {
                const uint32_t r = rows.bits[i], v = cache.valid.bits[i];
                missing |= r & ~v;
                common.bits[i] = r & v;
                any_common |= common.bits[i];
            }
            bool same = cache.data_valid != 0;
            if (same && any_common != 0) {
                for (uint32_t r = 0, n; same && (n = xe_rows_next_run(common, end, &r)) != 0; r += n)
                    same = xe_rows_equal(cache.data + 16u * r, source + 16u * r, n);
            }
            if (missing == 0 && same) {
                cache.covered_rows = &rows;
                *out_offset = cache.offset;
                return true;
            }
            carry = same; // carry the rows the last copy held (with `data`'s values: those the other shaders read)
        }
        uint8_t *dst = (uint8_t *)ring_alloc(size, g_uniform_alignment, out_offset);
        if (dst == 0)
            return false;
        if (carry) {
            [[clang::uninitialized]] s_xe_rows write;
            for (uint32_t i = 0; i < words; i++)
                write.bits[i] = rows.bits[i] | cache.valid.bits[i];
            for (uint32_t r = 0, n; (n = xe_rows_next_run(write, end, &r)) != 0; r += n)
                xe_rows_copy_both(dst + 16u * r, cache.data + 16u * r, source + 16u * r, n);
            for (uint32_t i = 0; i < words; i++)
                cache.valid.bits[i] = write.bits[i];
            cache.valid.chunk_mask = rows.chunk_mask | cache.valid.chunk_mask;
        } else {
            if (first != last) {
                for (auto run = first; run != last; ++run) {
                    const unsigned off = 16u * run->first;
                    xe_rows_copy_both(dst + off, cache.data + off, source + off, run->count);
                }
            } else {
                for (uint32_t r = 0, n; (n = xe_rows_next_run(rows, end, &r)) != 0; r += n)
                    xe_rows_copy_both(dst + 16u * r, cache.data + 16u * r, source + 16u * r, n);
            }
            for (uint32_t i = 0; i < words; i++)
                cache.valid.bits[i] = rows.bits[i];
            cache.valid.chunk_mask = rows.chunk_mask;
        }
        cache.frame = g_frame_serial;
        cache.offset = *out_offset;
        cache.last_gen = 0;
        cache.covered_rows = &rows;
        cache.literals_id = SHADER_PACK_NOT_FOUND;
        cache.data_valid = 1;
        cache.chunk_gen_valid = 0;
        return true;
    }
    // Precomputed deduplicated literal registers and the rows read from `data`.
    [[clang::uninitialized]] s_xe_rows literal_rows, plain;
    for (uint32_t i = 0; i < words; i++) {
        literal_rows.bits[i] = literals->mask.bits[i] & rows.bits[i];
        plain.bits[i] = rows.bits[i] & ~literal_rows.bits[i];
    }
    [[clang::uninitialized]] s_xe_rows write;
    for (uint32_t i = 0; i < words; i++)
        write.bits[i] = rows.bits[i];
    if (cache.frame == g_frame_serial && cache.data_valid != 0) {
        bool subset = true, same = true;
        [[clang::uninitialized]] s_xe_rows common;
        for (uint32_t i = 0; i < words; i++) {
            subset &= (rows.bits[i] & ~cache.valid.bits[i]) == 0;
            common.bits[i] = plain.bits[i] & cache.valid.bits[i];
        }
        for (uint32_t r = 0, n; same && (n = xe_rows_next_run(common, end, &r)) != 0; r += n)
            same = xe_rows_equal(cache.data + 16u * r, source + 16u * r, n);
        for (uint32_t d = 0; same && d < literals->count; d++) {
            const uint32_t reg = literals->defs[d].reg;
            if (reg < end && (literal_rows.bits[reg / 32u] & (1u << (reg % 32u))) != 0 &&
                (cache.valid.bits[reg / 32u] & (1u << (reg % 32u))) != 0)
                same = xe_rows_equal(cache.data + 16u * reg, (const uint8_t *)literals->defs[d].value, 1);
        }
        if (subset && same) {
            *out_offset = cache.offset;
            return true;
        }
        if (same) // carry the rows the last copy held (with `data`'s values: those the other shaders read)
            for (uint32_t i = 0; i < words; i++) {
                const uint32_t carried = cache.valid.bits[i] & ~rows.bits[i];
                write.bits[i] |= carried;
                plain.bits[i] |= carried;
            }
    }
    uint8_t *dst = (uint8_t *)ring_alloc(size, g_uniform_alignment, out_offset);
    if (dst == 0)
        return false;
    for (uint32_t r = 0, n; (n = xe_rows_next_run(plain, end, &r)) != 0; r += n)
        xe_rows_copy_both(dst + 16u * r, cache.data + 16u * r, source + 16u * r, n);
    for (uint32_t d = 0; d < literals->count; d++) {
        const uint32_t reg = literals->defs[d].reg;
        if (reg < end && (literal_rows.bits[reg / 32u] & (1u << (reg % 32u))) != 0) {
            memcpy(dst + 16u * reg, literals->defs[d].value, 16);
            memcpy(cache.data + 16u * reg, literals->defs[d].value, 16);
        }
    }
    for (uint32_t i = 0; i < words; i++)
        cache.valid.bits[i] = write.bits[i];
    cache.valid.chunk_mask = xe_rows_compute_chunk_mask(write.bits, words);
    cache.frame = g_frame_serial;
    cache.offset = *out_offset;
    cache.last_gen = 0;
    cache.literals_id = literals->xenos_index;
    cache.data_valid = 1;
    cache.chunk_gen_valid = 0;
    return true;
}

uint32_t xe_float_literal_index(s_shader_entry *entry, const s_shader_pack_xenos *meta)
{
    if (entry != 0 && entry->dedup_index != 0)
        return entry->dedup_index;
    uint32_t index = 1;
    if (meta != 0 && meta->def_count != 0) {
        const s_xe_dedup_literals *dedup = xe_dedup_literals(meta);
        if (dedup == 0)
            return 0;
        if (dedup->count != 0)
            index = (uint32_t)(dedup - g_xe_dedup_literals.items) + 2u;
    }
    if (entry != 0)
        entry->dedup_index = index;
    return index;
}

// Float register `reg` as the shader sees it (a literal constant of its, else the shadow's), for the CPU's reads.
void xe_float_register(const float *shadow, const s_shader_pack_xenos &meta, uint32_t reg, uint32_t out[4])
{
    memset(out, 0, 16);
    if (shadow != 0 && reg < 256)
        memcpy(out, shadow + 4u * reg, 16);
    for (uint32_t d = 0; d < meta.def_count; d++)
        if (meta.defs[d].reg == reg)
            memcpy(out, meta.defs[d].value, 16);
}

void xe_bool_loop_constants(xe_bool_loop *out)
{
    // D3DConstants: VertexShaderB[4], PixelShaderB[4] (GPU bools 0-127, 128-255), VertexShaderI[16], PixelShaderI[16]
    // (GPU loops 0-15, 16-31): the GPU register files in order
    if (g_shadows.vertex_bool != 0)
        memcpy(out->bools, g_shadows.vertex_bool, 16);
    else
        memset(out->bools, 0, 16);
    if (g_shadows.pixel_bool != 0)
        memcpy(out->bools + 4, g_shadows.pixel_bool, 16);
    else
        memset(out->bools + 4, 0, 16);
    if (g_shadows.vertex_int != 0)
        memcpy(out->loops, g_shadows.vertex_int, 64);
    else
        memset(out->loops, 0, 64);
    if (g_shadows.pixel_int != 0)
        memcpy(out->loops + 16, g_shadows.pixel_int, 64);
    else
        memset(out->loops + 16, 0, 64);
}

const void *xe_bool_loop_data(const s_port_vk_draw *draw, uint32_t gen)
{
    static_assert(offsetof(xe_bool_loop, loops) == 8 * sizeof(uint32_t), "decoded flow register layout");
    if (draw == &g_decoded.draw && g_shadows.vertex_bool && g_shadows.pixel_bool &&
        g_shadows.vertex_int && g_shadows.pixel_int)
        return g_decoded.flow;
    static xe_bool_loop gathered;
    if (!gen || g_xe_bool_loop_gen != gen) {
        xe_bool_loop_constants(&gathered);
        g_xe_bool_loop_gen = gen;
    }
    return &gathered;
}

// The fetch constants of the bound samplers (zero elsewhere) and what the shaders read derived from each
// (xe_fetch_derive: the degamma mask and the exp_adjust scale), for the fetch constants whose rows the pipeline reads
// (`rows`, xe_pipeline_rows); the others keep what they held (xe_block copies only rows read, and any it carries hold
// what its copy says).
void xe_fetch_constants(const s_port_vk_draw *draw, const s_xe_rows &rows,
                        uint32_t linear_gamma_mask, uint32_t slots)
{
    static_assert(PORT_VK_SAMPLER_COUNT <= 32, "one Xenos fetch constant per sampler slot");
    static_assert(offsetof(xe_fetch, derived) == 48u * 16u, "xe_fetch: 48 rows of constants, then one derived row each");
    auto &shadow = g_xe_fetch_shadow;
    const uint32_t gen = draw ? draw->samplers_any_gen : 0;
    if (!gen || shadow.gen != gen) {
        const auto *changes = record_changes_for(draw);
        const uint32_t dirty = gen && shadow.gen && changes && shadow.gen == changes->sampler_gen
                             ? changes->sampler_slots : UINT32_MAX;
        shadow.gen = gen;
        shadow.constant_slots &= ~dirty;
        shadow.derived_slots &= ~dirty;
    }
    const uint32_t derived = (rows.bits[1] >> 16) | (rows.bits[2] << 16);
    uint32_t derive = derived & (~shadow.derived_slots | (shadow.linear_mask ^ linear_gamma_mask));
    slots &= ~shadow.constant_slots;
    if (!(slots | derive))
        return;
    shadow.constant_slots |= slots;
    shadow.derived_slots |= derive;
    shadow.linear_mask = (shadow.linear_mask & ~derive) | (linear_gamma_mask & derive);
    while (slots) {
        const uint32_t n = (uint32_t)__builtin_ctz(slots);
        slots &= slots - 1u;
        uint32_t *c = shadow.data.constants[n];
        if (draw != 0 && n < PORT_VK_SAMPLER_COUNT && draw->samplers[n].bound) {
            const s_port_vk_sampler &slot = draw->samplers[n];
            memcpy(c, slot.texture.source.fetch_constant, 24);
            // the sampler's fields the translated shaders read themselves (no host sampler state covers them: the
            // stacked fetches' layer addressing and filtering, xenos_spirv io.odin sample_stacked) as the hardware
            // constant holds them, not the texture header's defaults: ClampZ (dword 0 bits 16-18; the D3DTADDRESS
            // values address_w holds are the GPUCLAMP ones) and VolMagFilter / VolMinFilter (dword 4 bits 0, 1)
            c[0] = (c[0] & ~(7u << 16)) | (slot.state.address_w & 7u) << 16;
            c[4] = (c[4] & ~3u) | (slot.state.mag_filter_z & 1u) | (slot.state.min_filter_z & 1u) << 1;
        } else {
            memset(c, 0, 24);
        }
    }
    while (derive) {
        const uint32_t n = (uint32_t)__builtin_ctz(derive);
        derive &= derive - 1;
        xe_fetch_derive(shadow.data.constants[n], shadow.data.derived[n]);
        if (linear_gamma_mask & (1u << n))
            shadow.data.derived[n][0] = 0;
    }
}

// ---- memexport targets ----

s_xe_export *xe_export_containing(const void *p, uint32_t *out_offset)
{
    const uint8_t *q = (const uint8_t *)p;
    for (uint32_t i = g_xe_exports.count; p != 0 && i-- > 0;) { // the latest copy of a range first
        s_xe_export &e = g_xe_exports.items[i];
        const uint8_t *base = (const uint8_t *)e.base;
        if (q >= base && q < base + e.size) {
            *out_offset = (uint32_t)(q - base);
            return &e;
        }
    }
    return 0;
}

// The export whose device copy holds the bytes at p (as xe_export_containing), or 0 when the memory itself does: none
// exported there, or the CPU locked the memory since (cpu_current: a fetch reads what the CPU wrote).
const s_xe_export *xe_export_holding(const void *p, uint32_t *out_offset)
{
    const s_xe_export *e = xe_export_containing(p, out_offset);
    return e != 0 && !e->cpu_current ? e : 0;
}

// The GPUENDIAN swap of a texture format (k_xenos_endian_*: 8in16, 8in32, 16in32), or a memexport stream constant's
// Endian128 (the same 0-3, then 4 8in64 and 5 8in128), over n bytes in place; its own inverse.
void xe_swap_endian(uint8_t *p, uint32_t n, uint32_t endian)
{
    // byte i of each 4-, 8- or 16-byte group trades places with byte i ^ m
    const uint32_t m = endian == k_xenos_endian_8in16 ? 1u : endian == k_xenos_endian_8in32 ? 3u
                       : endian == k_xenos_endian_16in32 ? 2u : endian == 4u ? 7u : endian == 5u ? 15u : 0u;
    if (m == 0)
        return;
    const uint32_t group = m + 1u > 4u ? m + 1u : 4u;
    for (uint32_t i = 0; i + group - 1u < n; i += group) {
        uint8_t q[16];
        memcpy(q, p + i, group);
        for (uint32_t k = 0; k < group; k++)
            p[i + k] = q[k ^ m];
    }
}

// A device buffer for `size` bytes of exported memory at `base`, filled with the guest bytes (what the GPU would read
// before anything exported into them), through the GPUENDIAN swap `endian` (a texture export's: s_xe_export); 0 when
// it cannot be made.
s_gpu_buffer *xe_export_buffer(const void *base, uint32_t size, uint32_t endian, uint32_t address[2])
{
    g_frame_trace.exports++;
    s_gpu_buffer *buffer = gpu_buffer_create(g_gpu, size != 0 ? size : 16u,
                                             k_gpu_buffer_storage | k_gpu_buffer_index |
                                                 k_gpu_buffer_transfer_src | k_gpu_buffer_transfer_dst,
                                             k_gpu_memory_gpu);
    uint8_t *swapped = buffer != 0 && endian != 0 && size != 0 ? (uint8_t *)malloc(size) : 0;
    if (swapped != 0) {
        memcpy(swapped, base, size);
        xe_swap_endian(swapped, size, endian);
    }
    if (buffer == 0 || (endian != 0 && size != 0 && swapped == 0) ||
        gpu_buffer_upload(g_gpu, buffer, 0, swapped != 0 ? swapped : base, size) != k_gpu_ok) {
        report(key_of(base) ^ 0x78657871u, "export storage creation failed");
        free(swapped);
        if (buffer != 0)
            gpu_buffer_destroy(g_gpu, buffer);
        return 0;
    }
    free(swapped);
    address[0] = address[1] = 0;
    if (g_xe_device_addresses)
        gpu_buffer_device_address(g_gpu, buffer, &address[0], &address[1]);
    return buffer;
}

// The device copy of exported memory, made at its first export from the guest bytes (what the GPU would read). The
// copy stays the memory's contents from then on (the Xbox GPU writes the memory itself), until the memory is the game's
// again (a buffer placed over it, the pager rewrote it: xe_apply_placements) or the CPU locked it and read it all back
// (cpu_current: the memory is the contents again, made into a new copy here). An export of the same memory with a
// larger size grows the copy: what the GPU wrote into the old range is carried over, the rest comes from the guest
// bytes. `texture`, `endian`: the export's (s_export_slot), for a new copy.
s_xe_export *xe_export_for(const void *base, uint32_t size, uint32_t texture, uint32_t endian)
{
    uint32_t offset = 0;
    s_xe_export *e = xe_export_containing(base, &offset);
    if (e != 0 && offset == 0 && e->size >= size && !e->cpu_current)
        return e;
    if (e != 0 && offset == 0) {
        uint32_t address[2];
        const uint32_t new_size = e->size >= size ? e->size : size;
        s_gpu_buffer *made = begin_frame() ? xe_export_buffer(base, new_size, e->endian, address) : 0;
        if (made == 0)
            return 0;
        if (!e->cpu_current) {
            // the new buffer's guest bytes are uploaded ahead of the frame (its first upload); the copy runs in the
            // frame, after the exports that wrote the old one
            gpu_cmd_copy_buffer(g_gpu, e->buffer, 0, made, 0, e->size);
            if (!gpu_rendering(g_gpu, 0, 0))
                clear_pass_attachments();
        }
        gpu_buffer_retire(g_gpu, e->buffer); // draws recorded this frame may still read it
        e->buffer = made;
        e->size = new_size;
        record_export_held({base, new_size}, true);
        e->address[0] = address[0];
        e->address[1] = address[1];
        e->cpu_current = 0;
        e->written = ++g_xe_export_serial;
        return e;
    }
    if (e != 0) {
        report(key_of(base) ^ 0x78657870u, "an exported range overlapping another export (a separate copy is made)");
    }
    e = g_xe_exports.push();
    if (e == 0)
        return 0;
    memset(e, 0, sizeof(*e));
    e->base = base;
    e->size = size;
    e->gpu_dword = xe_gpu_dword(base);
    e->texture = texture;
    e->endian = endian;
    e->written = ++g_xe_export_serial;
    e->buffer = xe_export_buffer(base, size, endian, e->address);
    if (e->buffer == 0) {
        g_xe_exports.count--;
        return 0;
    }
    record_export_held({base, size}, true);
    g_xe_texture_exports += texture != 0 ? 1u : 0u;
    return e;
}

// The device copies of exports overlapping [base, base + size) no longer hold that memory (a buffer placed over it, the
// pager rewrote it: xe_apply_placements).
void xe_exports_forget(const void *base, uint32_t size)
{
    const uint8_t *begin = (const uint8_t *)base, *end = begin + size;
    uint32_t kept = 0;
    for (uint32_t i = 0; i < g_xe_exports.count; i++) {
        s_xe_export &e = g_xe_exports.items[i];
        const uint8_t *export_begin = (const uint8_t *)e.base, *export_end = export_begin + e.size;
        if (export_begin < end && begin < export_end) {
            record_export_held({e.base, e.size}, false, false);
            gpu_buffer_retire(g_gpu, e.buffer); // draws recorded this frame may still read it
            g_xe_texture_exports -= e.texture != 0 ? 1u : 0u;
            continue;
        }
        g_xe_exports.items[kept++] = e; // the order (latest copy last) is kept
    }
    if (kept != g_xe_exports.count) {
        g_xe_exports.count = kept;
        ++g_xe_export_serial;
    }
}

// The ranges queued since the render thread last looked (port_vk_memory_placed, port_vk_memory_streamed): the exports
// they replace are dropped.
void xe_apply_placements()
{
    if (__atomic_load_n(&g_placed_count, __ATOMIC_RELAXED) == 0 && __atomic_load_n(&g_placed_overflow, __ATOMIC_RELAXED) == 0)
        return;
    struct s_applying {
        s_applying() { if (g_record_worker) record_drop_export_hints(); }
        ~s_applying() { if (g_record_worker) record_finish_export_invalidation(); }
    } applying;
    s_placed_range pending[k_placed_capacity];
    placed_lock();
    const uint32_t count = g_placed_count;
    const bool overflow = g_placed_overflow != 0;
    memcpy(pending, g_placed, count * sizeof(pending[0]));
    g_placed_count = 0;
    g_placed_overflow = 0;
    placed_unlock();
    if (overflow) {
        xe_exports_forget(0, 0xffffffffu);
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        const s_placed_range &r = pending[i];
        // a header placed inside an export's memory is a view of what the GPU wrote there (the water's per-draw header
        // over its part of the tessellation factors, render_water_draw_indexed_tessellated_primitive): the copy stays
        bool view = false;
        for (uint32_t k = 0; k < g_xe_exports.count && !r.streamed && !view; k++) {
            const s_xe_export &e = g_xe_exports.items[k];
            const uintptr_t begin = (uintptr_t)r.base, export_begin = (uintptr_t)e.base;
            view = begin >= export_begin && begin - export_begin <= e.size && r.size <= e.size - (begin - export_begin);
        }
        if (!view)
            xe_exports_forget(r.base, r.size);
    }
}

// A texture over memory a texture export wrote: the screenshot export's background_sampler, which fetches the pixel
// about to be exported to add the tile's to it (screenshot_memexport.hlsl: the tiles' guard pixels overlap their
// neighbours', and what each tile adds there accumulates). On the Xbox the fetch reads the memory the earlier tiles
// exported into; here that is the export's copy, so the view is the copy's bytes over the texture's level 0, copied into
// an image when it is bound after a draw exported into it. The copy holds the bytes in the order the Xenos writes them:
// the image is an R8G8B8A8 one of those bytes, whose view swizzle puts each component where the format's GPUENDIAN
// swap and conversion (xenos_texture_convert) would. false: the memory is no texture export's (the caller converts the
// texture as usual); true: *out is the view, GPU_NULL_HANDLE (a dummy texture) when the image cannot be made.
bool xe_exported_texture_view(const s_port_vk_texture *texture, uint32_t image_dim, gpu_view *out,
                              s_gpu_image **out_image)
{
    *out = GPU_NULL_HANDLE;
    if (g_xe_texture_exports == 0 || texture->source.base == 0)
        return false;
    // the image keeps the raw codes of GPUSIGN_GAMMA channels, which the shader degammas (as texture_view's)
    s_xenos_texture_desc desc = texture->source.desc;
    for (uint32_t c = 0; c < 4; c++)
        if (xenos_d3dformat_sign(desc.d3d_format, c) == 3u)
            desc.d3d_format &= ~(3u << (9u + 2u * c));
    s_xenos_texture_layout layout;
    if (!xenos_texture_get_layout(&desc, &layout) || layout.levels[0].from_mip_address != 0)
        return false;
    const s_xenos_level_layout &level = layout.levels[0];
    const uint8_t *memory = (const uint8_t *)texture->source.base + level.offset;
    const uint32_t bytes = level.row_pitch_bytes * level.height_blocks;
    s_xe_export *e = 0; // the latest texture export over the level
    for (uint32_t i = g_xe_exports.count; i-- > 0 && e == 0;) {
        s_xe_export &x = g_xe_exports.items[i];
        const uint8_t *b = (const uint8_t *)x.base;
        if (x.texture != 0 && b < memory + bytes && memory < b + x.size)
            e = &x;
    }
    if (e == 0)
        return false;
    // a linear 2D level of 32-bit texels the converter keeps as 8-bit RGBA or BGRA (only copied and swapped), inside
    // the export: the screenshot buffer's LIN_A8R8G8B8
    const uint8_t *export_base = (const uint8_t *)e->base;
    const uint64_t needed = desc.height != 0 && desc.width != 0
                                ? (uint64_t)level.row_pitch_bytes * (desc.height - 1u) + (uint64_t)desc.width * 4u
                                : 0u;
    s_pc_texture_info info;
    if (desc.dimension != k_xenos_dimension_2d || image_dim != k_shader_pack_image_2d || desc.levels != 1 ||
        xenos_d3dformat_tiled(desc.d3d_format) || layout.block_width != 1 || layout.block_height != 1 ||
        layout.bytes_per_block != 4 || level.row_pitch_bytes % 4u != 0 || needed == 0 || memory < export_base ||
        (memory - export_base) % 4 != 0 || // a buffer-to-image copy starts at a whole texel
        (uint64_t)(memory - export_base) + needed > e->size || !xenos_texture_get_pc_info(&desc, &info) ||
        (info.pc_format != k_pc_format_r8g8b8a8_unorm && info.pc_format != k_pc_format_b8g8r8a8_unorm)) {
        char detail[160];
        snprintf(detail, sizeof(detail), "header %p format %08x %ux%u levels %u", texture->key, desc.d3d_format,
                 desc.width, desc.height, desc.levels);
        report(key_of(texture->key) ^ 0x78747677u,
               "a texture over exported memory other than one linear 2D 8_8_8_8 level (read from memory, not the "
               "export)", detail);
        return false;
    }
    s_xe_export_view *view = 0;
    for (s_xe_export_view &v : g_xe_export_views)
        if (v.image != 0 && v.memory == memory && v.d3d_format == desc.d3d_format && v.width == desc.width &&
            v.height == desc.height)
            view = &v;
    if (view == 0) {
        view = &g_xe_export_views[g_xe_export_view_next++ % k_xe_export_views];
        if (view->image != 0)
            gpu_image_retire(g_gpu, view->image); // draws recorded this frame may still sample it
        memset(view, 0, sizeof(*view));
        // converted byte p (the swap's output) is copy byte p ^ m; the format's component c is converted byte
        // byte_of[c] (B8G8R8A8: R is byte 2)
        const uint32_t endian = xenos_d3dformat_endian(desc.d3d_format);
        const uint32_t m = endian == k_xenos_endian_8in16 ? 1u : endian == k_xenos_endian_8in32 ? 3u
                           : endian == k_xenos_endian_16in32 ? 2u : 0u;
        static const uint8_t k_rgba_byte[4] = { 0, 1, 2, 3 }, k_bgra_byte[4] = { 2, 1, 0, 3 };
        const uint8_t *byte_of = info.pc_format == k_pc_format_b8g8r8a8_unorm ? k_bgra_byte : k_rgba_byte;
        uint32_t swizzle[4];
        for (uint32_t c = 0; c < 4; c++) {
            const uint8_t s = info.swizzle[c];
            swizzle[c] = s <= k_xenos_swizzle_w ? (uint32_t)k_gpu_swizzle_r + (byte_of[s] ^ m) : swizzle_of(s);
        }
        s_gpu_image_desc image_desc = {};
        image_desc.view_type = k_gpu_view_2d;
        image_desc.format = k_gpu_format_r8g8b8a8_unorm;
        image_desc.width = desc.width;
        image_desc.height = desc.height;
        image_desc.depth = 1;
        image_desc.mip_levels = 1;
        image_desc.array_layers = 1;
        image_desc.usage = k_gpu_image_sampled | k_gpu_image_transfer_dst;
        image_desc.swizzle = GPU_SWIZZLE(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);
        view->image = gpu_image_create(g_gpu, &image_desc);
        if (view->image == 0) {
            report(key_of(texture->key) ^ 0x78747669u, "the image of a texture over exported memory could not be made "
                                                       "(dummy texture used)");
            return true;
        }
        view->memory = memory;
        view->d3d_format = desc.d3d_format;
        view->width = desc.width;
        view->height = desc.height;
    }
    if (view->written != e->written) {
        gpu_cmd_copy_buffer_to_image(g_gpu, e->buffer, (uint32_t)(memory - export_base),
                                     level.row_pitch_bytes / 4u, view->image, desc.width, desc.height);
        if (!gpu_rendering(g_gpu, 0, 0))
            clear_pass_attachments();
        view->written = e->written;
    }
    if (out_image)
        *out_image = view->image;
    *out = gpu_image_view(g_gpu, view->image);
    return true;
}

// A lock consumes the bytes immediately. Vulkan can put all of its copies after the exporting draws on the same
// queue, then make the bytes visible to the CPU with one frame fence. The staging allocation is reused across locks.
struct s_xe_readback_range {
    s_xe_export *exported;
    uint32_t from, to, endian;
    bool take_ownership;
};

#ifdef HALO3_64BIT
// Small buffer exports that the CPU has locked before are likely to be locked again (the ripple event buffers
// alternate between GPU exports and CPU writes). Copy their final version in the presenting submission so a
// later lock can wait for that fence without interrupting the next render pass.
const uint32_t k_xe_prefetch_bytes = 16u << 10;
struct s_xe_readback_snapshot {
    const void *base;
    s_gpu_buffer *staging;
    uint32_t locked_frame, copied_frame, slot, written;
};
s_xe_readback_snapshot g_xe_snapshots[4];

s_xe_readback_snapshot *xe_readback_snapshot(const void *base)
{
    for (s_xe_readback_snapshot &snapshot : g_xe_snapshots)
        if (snapshot.base == base)
            return &snapshot;
    return 0;
}

void xe_readback_track(const s_xe_export &e)
{
    if (e.texture != 0 || e.size == 0 || e.size > k_xe_prefetch_bytes || (e.size & 3u) != 0)
        return;
    s_xe_readback_snapshot *snapshot = xe_readback_snapshot(e.base);
    if (snapshot == 0) {
        snapshot = &g_xe_snapshots[0];
        for (s_xe_readback_snapshot &candidate : g_xe_snapshots)
            if (candidate.base == 0 ||
                g_frame_serial - candidate.locked_frame > g_frame_serial - snapshot->locked_frame)
                snapshot = &candidate;
        snapshot->base = e.base;
        snapshot->copied_frame = 0;
    }
    snapshot->locked_frame = g_frame_serial;
}

void xe_readback_prefetch()
{
    if (g_gpu->caps.kind != k_gpu_backend_vulkan || gpu_occlusion_active(g_gpu))
        return;
    for (s_xe_readback_snapshot &snapshot : g_xe_snapshots) {
        if (snapshot.base == 0 || g_frame_serial - snapshot.locked_frame > 120u)
            continue;
        uint32_t offset;
        const s_xe_export *e = xe_export_containing(snapshot.base, &offset);
        if (e == 0 || offset != 0 || e->texture != 0 || e->cpu_current || e->size == 0 ||
            e->size > k_xe_prefetch_bytes || (e->size & 3u) != 0 ||
            (snapshot.copied_frame != 0 && e->written == snapshot.written))
            continue;
        if (snapshot.staging == 0)
            snapshot.staging = gpu_buffer_create(g_gpu, k_xe_prefetch_bytes, k_gpu_buffer_transfer_dst,
                                                 k_gpu_memory_readback);
        if (snapshot.staging == 0)
            continue;
        gpu_cmd_copy_buffer(g_gpu, e->buffer, 0, snapshot.staging, 0, e->size);
        snapshot.written = e->written;
        snapshot.copied_frame = g_frame_serial;
        snapshot.slot = gpu_frame_index(g_gpu);
    }
}

void xe_readback_prefetch_failed()
{
    for (s_xe_readback_snapshot &snapshot : g_xe_snapshots)
        if (snapshot.copied_frame == g_frame_serial)
            snapshot.copied_frame = 0;
}

void xe_readback_shutdown()
{
    if (g_xe_readback != 0)
        gpu_buffer_destroy(g_gpu, g_xe_readback);
    g_xe_readback = 0;
    g_xe_readback_capacity = 0;
    for (s_xe_readback_snapshot &snapshot : g_xe_snapshots)
        if (snapshot.staging != 0)
            gpu_buffer_destroy(g_gpu, snapshot.staging);
    memset(g_xe_snapshots, 0, sizeof(g_xe_snapshots));
}
#endif

struct s_xe_readbacks {
    bool finished = false;
    bool read_back = false;
    bool texture_lock;
#ifdef HALO3_64BIT
    t_list<s_xe_readback_range> ranges = {};
#endif

    explicit s_xe_readbacks(bool texture) : texture_lock(texture) {}

    void apply(const s_xe_readback_range &range, const uint8_t *staged)
    {
        s_xe_export &e = *range.exported;
        uint8_t *memory = (uint8_t *)e.base + range.from;
        if (staged != 0)
            memcpy(memory, staged, range.to - range.from);
        g_frame_trace.readbacks++;
        xe_swap_endian(memory, range.to - range.from, range.endian);
        read_back = true;
        if (range.take_ownership) {
            record_export_held({e.base, e.size}, false);
            e.cpu_current = 1;
            ++g_xe_export_serial;
        }
    }

    void download(const s_xe_readback_range &range)
    {
        if (!finished) {
            if (g_frame_open) {
                end_pass();
                gpu_cmd_memory_barrier(g_gpu);
            }
            finish_frame();
            finished = true;
        }
        const s_xe_export &e = *range.exported;
        if (gpu_buffer_download(g_gpu, e.buffer, range.from, (uint8_t *)e.base + range.from,
                                range.to - range.from) == k_gpu_ok)
            apply(range, 0);
        else
            report(key_of(e.base) ^ (texture_lock ? 0x78726264u : 0x78726262u),
                   texture_lock ? "a CPU read of exported memory failed (the memory keeps what it held)"
                                : "a CPU lock of exported memory failed to read it back (the memory keeps what it held)");
    }

#ifdef HALO3_64BIT
    // vkCmdCopyBuffer's transfer barriers include host reads. Keep the legacy path for sizes that cannot be
    // represented in one staging buffer, active queries, and failures before the batch can be submitted.
    bool batch()
    {
        uint64_t size = 0;
        for (uint32_t i = 0; i < ranges.count; i++) {
            const s_xe_readback_range &range = ranges.items[i];
            const uint32_t bytes = range.to - range.from;
            if (bytes == 0 || (bytes & 3u) != 0)
                return false;
            size += ((uint64_t)bytes + 3u) & ~3ull;
        }
        if (size > 0x7fffffffu || gpu_occlusion_active(g_gpu))
            return false;
        if (g_xe_readback_capacity < size) {
            uint32_t capacity = 4096u;
            while (capacity < size)
                capacity *= 2u;
            s_gpu_buffer *staging = gpu_buffer_create(g_gpu, capacity, k_gpu_buffer_transfer_dst, k_gpu_memory_readback);
            if (staging == 0)
                return false;
            if (g_xe_readback != 0)
                gpu_buffer_destroy(g_gpu, g_xe_readback);
            g_xe_readback = staging;
            g_xe_readback_capacity = capacity;
        }
        const uint8_t *mapped = (const uint8_t *)gpu_buffer_mapped(g_gpu, g_xe_readback);
        if (mapped == 0 || !begin_frame())
            return false;
        uint32_t offset = 0;
        for (uint32_t i = 0; i < ranges.count; i++) {
            const s_xe_readback_range &range = ranges.items[i];
            const uint32_t bytes = range.to - range.from;
            gpu_cmd_copy_buffer(g_gpu, range.exported->buffer, range.from, g_xe_readback, offset, bytes);
            offset += (bytes + 3u) & ~3u;
        }

        const gpu_result result = finish_frame_wait();
        finished = true;
        if (result != k_gpu_ok) {
            report(0x78657262u, "submitting or waiting for the export readback batch failed");
            // A failed wait may leave the buffer in flight. Retire it before another lock can use it.
            gpu_buffer_retire(g_gpu, g_xe_readback);
            g_xe_readback = 0;
            g_xe_readback_capacity = 0;
            return true;
        }
        gpu_buffer_invalidate(g_gpu, g_xe_readback);
        offset = 0;
        for (uint32_t i = 0; i < ranges.count; i++) {
            const s_xe_readback_range &range = ranges.items[i];
            apply(range, mapped + offset);
            offset += (range.to - range.from + 3u) & ~3u;
        }
        return true;
    }

    // One matching snapshot is the common buffer-lock case. Mixed or overlapping ranges use the ordered batch.
    bool prefetched()
    {
        if (texture_lock || ranges.count != 1)
            return false;
        const s_xe_readback_range &range = ranges.items[0];
        const s_xe_readback_snapshot *snapshot = xe_readback_snapshot(range.exported->base);
        if (snapshot == 0 || snapshot->copied_frame == 0 || snapshot->written != range.exported->written)
            return false;
        // Once the slot has been reused, begin_frame has already waited for this copy.
        if (g_frame_serial - snapshot->copied_frame < g_gpu->caps.frames_in_flight &&
            gpu_frame_wait(g_gpu, snapshot->slot) != k_gpu_ok)
            return false;
        const uint8_t *mapped = (const uint8_t *)gpu_buffer_mapped(g_gpu, snapshot->staging);
        if (mapped == 0)
            return false;
        gpu_buffer_invalidate(g_gpu, snapshot->staging);
        apply(range, mapped + range.from);
        return true;
    }

    void flush()
    {
        if (ranges.count == 0)
            return;
        const long long started = g_frame_trace.on ? frame_stats_now() : 0;
        const bool ready = prefetched();
        const bool batched = !ready && batch();
        if (!ready && !batched)
            for (uint32_t i = 0; i < ranges.count; i++)
                download(ranges.items[i]);
        if (!texture_lock)
            for (uint32_t i = 0; i < ranges.count; i++)
                xe_readback_track(*ranges.items[i].exported);
        if (started != 0) {
            char text[192];
            snprintf(text, sizeof(text), "ftrace export: frame %u %s lock %s ranges %u first_bytes %u %.2f ms\n",
                     g_presented, texture_lock ? "texture" : "buffer", ready ? "prefetched" : batched ? "batch" : "download",
                     ranges.count, ranges.items[0].to - ranges.items[0].from,
                     frame_stats_ms(started, frame_stats_now()));
            port_write_error(text);
        }
        ranges.count = 0;
    }
#endif

    void add(s_xe_export &e, uint32_t from, uint32_t to, uint32_t endian, bool take_ownership)
    {
        const s_xe_readback_range range = { &e, from, to, endian, take_ownership };
#ifdef HALO3_64BIT
        if (g_gpu->caps.kind == k_gpu_backend_vulkan) {
            if (s_xe_readback_range *slot = ranges.push()) {
                *slot = range;
                return;
            }
            flush();
        }
#endif
        download(range);
    }

    bool complete()
    {
#ifdef HALO3_64BIT
        flush();
        ranges.release();
#endif
        return read_back;
    }
};

// port_vk_texture_lock, version 3 pack: the CPU reads level / layer of a texture, and memory exports wrote (some of) its
// bytes, which here are in the exports' copies. The lock waits for those bytes and writes them into the level's
// memory, the texture format's GPUENDIAN swap undone: the host order the CPU reads in (as a
// resolve's write-back leaves it), which is what the Xbox CPU reads in its own order (the screenshot's
// D3DXSaveSurfaceToFileInMemory, through gui_custom_bitmap_storage_capture_screenshot's own header over the buffer).
// A copy read back whole and so is held by the CPU (cpu_current, xe_export_for). Only on a lock of exported memory:
// draws never pay for it.
void xe_export_lock(const s_port_d3d_texture_source &source, uint32_t level_index, uint32_t layer)
{
    xe_apply_placements(); // placements since the last draw (port_vk_memory_placed)
    if (g_xe_exports.count == 0)
        return;
    s_xenos_texture_layout layout;
    if (level_index >= 16 || level_index >= source.desc.levels || !xenos_texture_get_layout(&source.desc, &layout))
        return;
    const s_xenos_level_layout &level = layout.levels[level_index];
    uint8_t *memory = (uint8_t *)(level.from_mip_address ? source.mip : source.base);
    if (memory == 0)
        return;
    memory += level.offset + layer * level.slice_stride;
    uint32_t slices = 1;
    if (source.desc.dimension == k_xenos_dimension_3d) {
        slices = source.desc.depth >> level_index;
        slices = slices != 0 ? slices : 1u;
    }
    // addresses as 64-bit numbers: no wrap at the top of a 32-bit address space
    const uint64_t level_begin = (uintptr_t)memory;
    const uint64_t level_end = level_begin + (uint64_t)level.row_pitch_bytes * level.height_blocks * slices;
    const uint32_t endian = xenos_d3dformat_endian(source.desc.d3d_format);
    s_xe_readbacks readbacks(true);
    for (uint32_t i = 0; i < g_xe_exports.count; i++) { // the latest copy last: it wins where copies overlap
        s_xe_export &e = g_xe_exports.items[i];
        uint8_t *const export_base = (uint8_t *)e.base;
        const uint64_t export_begin = (uintptr_t)export_base, export_end = export_begin + e.size;
        const uint64_t lo = level_begin > export_begin ? level_begin : export_begin;
        const uint64_t hi = level_end < export_end ? level_end : export_end;
        if (lo >= hi || e.cpu_current)
            continue; // no overlap; or the CPU already holds the copy (and may have written the memory since)
        // whole 4-byte groups (the swap's unit): bytes of the copy past the level's are the memory's too. A texture
        // export is read back whole: it is memory the CPU reads and writes, which it then holds (cpu_current) even
        // when the lock covers less of it (its size is whole pages, the level's bytes need not be)
        uint32_t from = (uint32_t)(lo - export_begin) & ~3u;
        uint32_t to = ((uint32_t)(hi - export_begin) + 3u) & ~3u;
        to = to < e.size ? to : e.size;
        // the swap of the CPU's view: the locked format's, or a texture export's own over all of its memory
        uint32_t swap = endian;
        if (e.texture != 0) {
            from = 0;
            to = e.size;
            swap = e.endian;
        }
        readbacks.add(e, from, to, swap, from == 0 && to == e.size && swap == e.endian);
    }
    readbacks.complete();
}

// port_vk_buffer_lock, version 3 pack: the CPU is about to read [base, base + size) of a vertex or index buffer and,
// unless read_only, to write it. On the Xbox a Lock waits until the GPU is done with the buffer, and the CPU then sees
// the memory as the GPU left it; here memory exports wrote it into their device copies. Each export over the range
// is read back whole into the memory before the lock returns, its swap
// undone (`endian`: the host order the CPU reads in). A writing lock leaves the memory to the CPU (cpu_current): the
// copy no longer holds it, so fetches read the memory (xe_export_holding) and the next export of it starts from the
// memory (xe_export_for). The water writes its interaction events into the event buffer its previous frame's ripple
// export wrote (water_renderer_handle_interaction_events), which the export after the lock reads; without this the copy
// kept the first events ever written. Only on a lock of exported memory: draws never pay for it. True when an export
// was read back: the memory's bytes are now the export's, which a read-only lock must still count as a change.
bool xe_buffer_lock(const void *base, uint32_t size, bool read_only)
{
    xe_apply_placements(); // placements since the last draw (port_vk_memory_placed)
    const uintptr_t begin = (uintptr_t)base, end = begin + size;
    s_xe_readbacks readbacks(false);
    for (uint32_t i = 0; i < g_xe_exports.count; i++) { // the latest copy last: it wins where copies overlap
        s_xe_export &e = g_xe_exports.items[i];
        const uintptr_t export_begin = (uintptr_t)e.base;
        if (end <= export_begin || export_begin + e.size <= begin || e.cpu_current)
            continue; // no overlap; or the CPU already holds the memory (and may have written it since)
        readbacks.add(e, 0, e.size, e.endian, !read_only);
    }
    return readbacks.complete();
}

// Where byte `offset` of a buffer is for the translated shaders (xe_vertex_stream address_lo / _hi, xe_system.memexport
// z / w): with device addresses the buffer's (`address`: gpu_buffer_device_address) plus the offset; without, the offset
// alone, into the view of the buffer the draw binds for that stream or export (xe_bind: gpu_cmd_xenos_bind_memory, SRV
// t{stream} / UAV u{export} space2 on Direct3D 12).
void xe_location(const uint32_t address[2], uint32_t offset, uint32_t *out_lo, uint32_t *out_hi)
{
    if (!g_xe_device_addresses) {
        *out_lo = offset;
        *out_hi = 0;
        return;
    }
    const uint64_t a = ((uint64_t)address[1] << 32 | address[0]) + offset;
    *out_lo = (uint32_t)a;
    *out_hi = (uint32_t)(a >> 32);
}

// Stream constant s of a memexport shader (eA: its float register, from the shadow `floats` or its literal constants);
// false when it names no register or the register holds none (the fixed bits the Xenos checks: the shader writes
// nothing through anything else).
bool xe_stream_constant(const s_shader_pack_xenos &meta, const float *floats, uint32_t s, uint32_t ea[4])
{
    const uint32_t reg = meta.streams[s];
    if (reg >= 256)
        return false;
    xe_float_register(floats, meta, reg, ea);
    return ea[0] >> 30 == 1u && ea[1] >> 23 == 0x96u && ea[2] >> 20 == 0x4b0u && ea[3] >> 23 == 0x96u;
}

// The Endian128 (dword 2 bits 0-2) of the first of a memexport shader's stream constants into [base, base + size): the
// byte order its draw writes that memory in (s_xe_export::endian); `fallback` when none points there.
uint32_t xe_stream_endian(const s_shader_pack_xenos &meta, const float *floats, const void *base, uint32_t size,
                          uint32_t fallback)
{
    const uint32_t first = xe_gpu_dword(base);
    for (uint32_t s = 0; s < meta.stream_count; s++) {
        uint32_t ea[4];
        if (xe_stream_constant(meta, floats, s, ea) && (ea[0] & 0x3fffffffu) - first < size / 4u)
            return ea[2] & 7u;
    }
    return fallback;
}

// xe_system.memexport: the exported ranges the shader's stream constants (its float registers: the shadow `floats` and
// its literal constants) point into, one entry per range: the open BeginExport's memory, or an earlier
// export's. The shader finds a stream's entry by its base address.
void xe_memexport_streams(const s_shader_pack_xenos &meta, const float *floats, xe_system *sys,
                          s_gpu_buffer *views[XE_D3D12_MEMEXPORT_BUFFERS])
{
    uint32_t entries = 0;
    for (uint32_t s = 0; s < meta.stream_count; s++) {
        uint32_t ea[4];
        if (!xe_stream_constant(meta, floats, s, ea))
            continue;
        const uint32_t base = ea[0] & 0x3fffffffu;
        s_xe_export *target = 0;
        for (uint32_t i = g_xe_exports.count; i-- > 0 && target == 0;) {
            s_xe_export &e = g_xe_exports.items[i];
            if (base - e.gpu_dword < e.size / 4u)
                target = &e;
        }
        if (target == 0) {
            report(meta.index ^ 0x6d657873u, "a memexport stream constant naming memory no export covers (not written)",
                   meta.name);
            continue;
        }
        // the draw writes the copy: views of it are copied again, and the CPU no longer holds its contents
        target->written = ++g_xe_export_serial;
        if (target->cpu_current) record_export_held({target->base, target->size}, true);
        target->cpu_current = 0;
        if (target->texture == 0)
            target->endian = ea[2] & 7u; // the byte order of what it writes (a CPU lock reads it back through it)
        bool listed = false;
        for (uint32_t i = 0; i < entries; i++)
            listed |= sys->memexport[i][0] == target->gpu_dword;
        if (listed)
            continue;
        if (entries == 4) {
            report(meta.index ^ 0x6d657834u, "a memexport shader writing more than 4 exported ranges (the rest not written)",
                   meta.name);
            break;
        }
        if (views)
            views[entries] = target->buffer;
        uint32_t *m = sys->memexport[entries++];
        m[0] = target->gpu_dword;
        m[1] = target->size / 4u;
        xe_location(target->address, 0, &m[2], &m[3]);
    }
}

// ---- vertex fetch ----

// Where a stream's guest bytes are for this draw (vertex 0 after the SetStreamSource offset): exported memory (`e`, the
// export holding stream.data at export_offset: xe_export_containing), a device copy of an unchanged vertex buffer, or
// the vertices the draw reads copied into the ring. *out_buffer: the buffer they are in (the stream's view without
// device addresses: xe_location).
bool xe_stream(const s_port_vk_stream &stream, unsigned slot, s_vertex_range *range, const s_xe_export *e, uint32_t export_offset,
               xe_vertex_stream *out, s_gpu_buffer **out_buffer, bool *out_static = 0)
{
    *out_buffer = 0;
    if (out_static != 0)
        *out_static = false;
    if (stream.data == 0 || stream.size <= stream.offset) {
        memset(out, 0, sizeof(*out));
        return false;
    }
    const uint32_t bytes = stream.size - stream.offset;
    if (e != 0) {
        xe_location(e->address, export_offset + stream.offset, &out->address_lo, &out->address_hi);
        *out_buffer = e->buffer;
        out->size_bytes = e->size > export_offset + stream.offset ? e->size - export_offset - stream.offset : 0u;
        if (out_static != 0)
            *out_static = true;
        return true;
    }
    if (stream.key != 0) {
        const uint32_t generation = touch_generation(stream.key);
        uint32_t index;
        s_xe_buffer *entry = 0;
        if (map_find(&g_xe_buffer_index, key_of(stream.key), &index))
            entry = &g_xe_buffers.items[index];
        if (entry != 0 && entry->data == stream.data && entry->size == stream.size && !entry->dynamic &&
            entry->generation == generation && entry->buffer != 0) {
            xe_location(entry->address, stream.offset, &out->address_lo, &out->address_hi);
            *out_buffer = entry->buffer;
            out->size_bytes = bytes;
            if (out_static != 0)
                *out_static = true;
            return true;
        }
        if (entry != 0 && entry->buffer != 0 && entry->data == stream.data && entry->size == stream.size) {
            entry->dynamic = 1; // rewritten after its upload: the ring from now on
        } else {
            if (entry == 0) {
                entry = g_xe_buffers.push();
                if (entry == 0)
                    return false;
                map_set(&g_xe_buffer_index, key_of(stream.key), g_xe_buffers.count - 1u);
            } else if (entry->buffer != 0) {
                gpu_buffer_retire(g_gpu, entry->buffer);
                entry->buffer = 0;
            }
            entry->key = stream.key;
            entry->data = stream.data;
            entry->size = stream.size;
            entry->generation = generation;
            entry->dynamic = 0;
            entry->buffer = gpu_buffer_create(g_gpu, stream.size, k_gpu_buffer_storage,
                                              k_gpu_memory_gpu);
            if (entry->buffer == 0 ||
                gpu_buffer_upload(g_gpu, entry->buffer, 0, stream.data, stream.size) != k_gpu_ok) {
                if (entry->buffer != 0)
                    gpu_buffer_destroy(g_gpu, entry->buffer); // never used: made again by the next draw
                entry->buffer = 0;
                return false;
            }
            g_converted_bytes[k_converted_static] += stream.size;
            if (g_xe_device_addresses)
                gpu_buffer_device_address(g_gpu, entry->buffer, &entry->address[0], &entry->address[1]);
            xe_location(entry->address, stream.offset, &out->address_lo, &out->address_hi);
            *out_buffer = entry->buffer;
            out->size_bytes = bytes;
            if (out_static != 0)
                *out_static = true;
            return true;
        }
    }
    // the ring: the vertices [first, end) the draw reads, at the address vertex 0 would have
    const uint32_t stride = stream.stride != 0 ? stream.stride : 4u;
    const uint32_t vertices = bytes / stride;
    uint32_t first = 0, end = vertices;
    if (range != 0) {
        vertex_range_resolve(range);
        if (!range->whole && stream.stride != 0) { // stride 0: every vertex reads the same bytes, from the start
            first = range->first < vertices ? range->first : vertices;
            end = range->end < vertices ? range->end : vertices;
            end = end > first ? end : first;
        }
    }
    // whole vertices only, plus the bytes of a last partial one (an element may end before the stride)
    const uint32_t lead = first * stride;
    const uint32_t copy = end == vertices ? bytes - lead : (end - first) * stride;
    uint32_t offset;
    void *dst = ring_alloc_after(copy != 0 ? copy : 4u, 16, lead, &offset);
    if (dst == 0)
        return false;
    copy_upload_bytes((uint8_t *)dst, vertex_bytes(stream, slot, stream.offset + lead), copy);
    g_converted_bytes[stream.key == 0 ? k_converted_user_pointer : k_converted_dynamic] += copy;
    xe_location(g_xe_ring_address[gpu_frame_index(g_gpu)], offset - lead, &out->address_lo, &out->address_hi);
    *out_buffer = ring_buffer();
    out->size_bytes = lead + copy;
    return true;
}

// xe_vertex_fetch for a vertex shader: its container's vertex elements matched by (usage, usage index) to the
// declaration, as the Xbox runtime patches the vfetch instructions; one stream entry per (D3D stream, GPUENDIAN).
// `rows`: the rows the pipeline reads (xe_pipeline_rows), zeroed first (what is not filled reads zeros); the others
// are left as they are (xe_block copies only the rows read). views: the buffer of each stream entry (NULL: none), for
// xe_bind without device addresses.
uint32_t xe_next_vertex_fetch_gen()
{
    if (++g_xe_vf_gen_counter == 0) {
        g_xe_vf_gen_counter = 1;
        for (s_xe_vf_template &entry : g_xe_vf_templates) {
            entry.stream_valid = 0;
            entry.ring.frame = 0;
        }
        g_xe_blocks[k_xe_block_vertex_fetch].frame = 0;
        g_xe_offsets.frame = 0;
    }
    return g_xe_vf_gen_counter;
}

bool xe_vertex_fetch_fill(s_draw_geometry draw, uint64_t dirty_mask, const s_shader_pack_xenos &meta, s_vertex_range *range,
                          const s_xe_rows &rows, xe_vertex_fetch *out, s_gpu_buffer *views[XE_D3D12_VERTEX_STREAMS],
                          const xe_vertex_fetch **out_ptr = 0, uint32_t *out_vf_gen = 0, uint32_t *export_slots = 0)
{
    if (export_slots)
        *export_slots = 0;
    if (meta.element_count == 0) {
        if (!g_xe_device_addresses)
            memset(views, 0, sizeof(s_gpu_buffer *) * XE_D3D12_VERTEX_STREAMS);
        for (uint32_t r = 0, n; (n = xe_rows_next_run(rows, sizeof(*out) / 16u, &r)) != 0; r += n)
            memset((uint8_t *)out + 16u * r, 0, 16u * n);
        if (out_ptr != 0)
            *out_ptr = out;
        if (out_vf_gen != 0)
            *out_vf_gen = 0;
        return true;
    }
    const uint32_t touch_epoch = __atomic_load_n(&g_port_vk_resource_touch_epoch, __ATOMIC_ACQUIRE);
    const void *declaration_key = draw.declaration();
    const uint32_t stream_gen = draw.override ? 0u : draw->stream_gen;
    auto matches = [&](s_xe_vf_template &cached) {
        if (!cached.valid || !cached.stream_valid || !stream_gen || cached.vs_index != meta.index ||
            cached.declaration != declaration_key || cached.export_serial != g_xe_export_serial ||
            cached.touch_epoch != touch_epoch)
            return false;
        if (cached.stream_gen != stream_gen) {
            const auto *changes = record_changes_for(draw.packet);
            if (!changes || cached.stream_gen != changes->stream_gen || (cached.used_streams & changes->stream_slots))
                return false;
            cached.stream_gen = stream_gen;
        }
        return true;
    };
    auto reuse = [&](s_xe_vf_template &cached) {
        g_xe_last_vf_template = &cached;
        if (!g_xe_device_addresses)
            memcpy(views, cached.views, sizeof(cached.views));
        if (out_ptr != 0)
            *out_ptr = &cached.data;
        else
            memcpy(out, &cached.data, sizeof(*out));
        if (out_vf_gen != 0)
            *out_vf_gen = cached.vf_gen;
        if (export_slots)
            *export_slots = cached.export_slots;
        return true;
    };
    if ((dirty_mask & (PORT_VK_DIRTY_STREAMS | PORT_VK_DIRTY_VDECL | PORT_VK_DIRTY_VS)) == 0 &&
        g_xe_last_vf_template != 0 && matches(*g_xe_last_vf_template))
        return reuse(*g_xe_last_vf_template);
    const uint32_t tpl_idx = ((meta.index * 2654435761u) ^ (key_of(declaration_key) * 2246822519u)) &
                             (k_xe_vf_template_size - 1u);
    s_xe_vf_template &tpl = g_xe_vf_templates[tpl_idx];
    if (matches(tpl))
        return reuse(tpl);
    // 2) Template match fast path: same (vs, declaration) and matching stream endian/export mask -> fill only active streams
    [[clang::uninitialized]] const s_xe_export *stream_export[PORT_VK_MAX_STREAMS];
    [[clang::uninitialized]] uint32_t stream_export_offset[PORT_VK_MAX_STREAMS];
    bool all_static = true;
    // Different endian views interpret the same guest bytes. Upload once per D3D
    // stream in this draw, even for mutable UP and mapped particle streams.
    auto fill_slot = [&](uint32_t slot) {
        const unsigned d3d_slot = tpl.slot_stream[slot], first = tpl.first_stream_slot[d3d_slot];
        const auto &stream = draw.stream(d3d_slot);
        auto &s = tpl.data.stream[slot];
        if (slot == first) {
            bool stream_static = false;
            if (!(draw.stream_mask() & (1u << d3d_slot)) ||
                !xe_stream(stream, d3d_slot, range, stream_export[d3d_slot], stream_export_offset[d3d_slot],
                           &s, &tpl.views[slot], &stream_static)) {
                report_stream(draw, shader_entry(draw->vertex_shader), d3d_slot);
                return false;
            }
            all_static &= stream_static;
        } else {
            s = tpl.data.stream[first];
            tpl.views[slot] = tpl.views[first];
        }
        s.stride_endian = (stream.stride & 0xffffu) | (uint32_t(tpl.slot_endian[slot]) << 16);
        if (stream_export[d3d_slot]) tpl.export_slots |= 1u << slot;
        return true;
    };
    if (tpl.valid && tpl.vs_index == meta.index && tpl.declaration == draw.declaration()) {
        uint32_t cur_be_mask = 0;
        uint32_t sm = tpl.used_streams;
        while (sm) {
            const uint32_t s = (uint32_t)__builtin_ctz(sm);
            sm &= sm - 1u;
            stream_export_offset[s] = 0;
            stream_export[s] =
                draw.stream(s).data != 0 ? xe_export_holding(draw.stream(s).data, &stream_export_offset[s]) : 0;
            if (draw.stream(s).big_endian || stream_export[s] != 0)
                cur_be_mask |= 1u << s;
        }
        if (cur_be_mask == tpl.be_mask) {
            tpl.stream_valid = 0;
            tpl.vf_gen = 0;
            tpl.export_slots = 0;
            if (!g_xe_device_addresses)
                memset(tpl.views, 0, sizeof(tpl.views));
            for (uint32_t slot = 0; slot < tpl.slot_count; ++slot)
                if (!fill_slot(slot)) return false;
            if (!g_xe_device_addresses)
                memcpy(views, tpl.views, sizeof(tpl.views));
            // This fill owns a fresh version even when the next draw must refill
            // mutable streams. xe_block can upload its live rows without a CPU mirror.
            tpl.vf_gen = xe_next_vertex_fetch_gen();
            if (all_static && stream_gen != 0) {
                tpl.stream_valid = 1;
                tpl.stream_gen = stream_gen;
                tpl.export_serial = g_xe_export_serial;
                tpl.touch_epoch = touch_epoch;
                g_xe_last_vf_template = &tpl;
            }
            if (out_ptr != 0)
                *out_ptr = &tpl.data;
            else
                memcpy(out, &tpl.data, sizeof(*out));
            if (out_vf_gen != 0)
                *out_vf_gen = tpl.vf_gen;
            if (export_slots)
                *export_slots = tpl.export_slots;
            return true;
        }
    }
    // 3) Cold miss: build template for (vs, declaration, be_mask)
    uint32_t hash = 0;
    const s_xenos_vertex_declaration *declaration = declaration_for(draw.declaration(), &hash);
    if (declaration == 0) {
        if (!g_xe_device_addresses)
            memset(views, 0, sizeof(s_gpu_buffer *) * XE_D3D12_VERTEX_STREAMS);
        for (uint32_t r = 0, n; (n = xe_rows_next_run(rows, sizeof(*out) / 16u, &r)) != 0; r += n)
            memset((uint8_t *)out + 16u * r, 0, 16u * n);
        report(meta.index ^ 0x6e6f6463u, "a draw without a vertex declaration (its vertex fetches read zeros)", meta.name);
        if (out_ptr != 0)
            *out_ptr = out;
        if (out_vf_gen != 0)
            *out_vf_gen = 0;
        return true;
    }
    tpl.valid = 0;
    tpl.stream_valid = 0;
    tpl.export_slots = 0;
    tpl.vf_gen = 0;
    memset(&tpl.data, 0, sizeof(tpl.data));
    memset(tpl.views, 0, sizeof(tpl.views));
    uint32_t slot_key[16];
    uint32_t slots = 0;
    uint32_t used_streams = 0;
    uint32_t be_mask = 0;
    // the export each D3D stream's memory is in, looked up once a stream (bit s of exports_known: stream s looked up)
    uint32_t exports_known = 0;
    // element[31] is the translator's, for vfetches outside the container's list (zeros)
    if (meta.element_count > 31)
        report(meta.index ^ 0x656c3331u, "a vertex shader with more than 31 vertex elements (the rest read zeros)", meta.name);
    for (uint32_t k = 0; k < meta.element_count && k < 31; k++) {
        const s_shader_pack_file_xenos_element &want = meta.elements[k];
        const s_xenos_d3d_vertex_element *el = find_element(declaration, want.usage, want.usage_index);
        if (el == 0 || el->stream >= PORT_VK_MAX_STREAMS) {
            report((meta.index * 131u + want.usage * 16u + want.usage_index) ^ 0x656c6d74u,
                   "a vertex shader element the declaration has no element for (reads zeros)", meta.name);
            continue;
        }
        const s_port_vk_stream &stream = draw.stream(el->stream);
        if (!(used_streams & (1u << el->stream)))
            tpl.first_stream_slot[el->stream] = uint8_t(slots);
        used_streams |= 1u << el->stream;
        if ((exports_known & (1u << el->stream)) == 0) {
            exports_known |= 1u << el->stream;
            stream_export_offset[el->stream] = 0;
            stream_export[el->stream] =
                stream.data != 0 ? xe_export_holding(stream.data, &stream_export_offset[el->stream]) : 0;
        }
        // the bytes' order: Xenos order for data in that order and for memory a memexport wrote (in the order of the
        // game's stream constant, which matches the declaration it is read with), else host order
        const bool exported = stream_export[el->stream] != 0;
        if (stream.big_endian || exported)
            be_mask |= 1u << el->stream;
        const uint32_t endian = stream.big_endian || exported ? (el->type >> 6) & 3u : 0u;
        const uint32_t key = el->stream | endian << 8;
        uint32_t slot = 0;
        while (slot < slots && slot_key[slot] != key)
            slot++;
        if (slot == slots) {
            if (slots == 16)
                continue;
            slot_key[slots] = key;
            tpl.slot_stream[slots] = (uint8_t)el->stream;
            tpl.slot_endian[slots] = (uint8_t)endian;
            slots++;
            if (!fill_slot(slot)) return false;
        }
        xe_vertex_element &e = tpl.data.element[k];
        e.stream = slot;
        e.offset_bytes = el->offset;
        e.format = el->type & 0x3fu;
        e.flags = xe_vertex_element_flags(el->type);
        xe_vertex_unpack_fill(e.format, e.flags, &tpl.data.unpack[k], tpl.data.denominator[k]);
    }
    tpl.valid = 1;
    tpl.vs_index = meta.index;
    tpl.declaration = draw.declaration();
    tpl.be_mask = be_mask;
    tpl.used_streams = used_streams;
    tpl.slot_count = slots;
    if (!g_xe_device_addresses)
        memcpy(views, tpl.views, sizeof(tpl.views));
    tpl.vf_gen = xe_next_vertex_fetch_gen();
    if (all_static && stream_gen != 0) {
        tpl.stream_valid = 1;
        tpl.stream_gen = stream_gen;
        tpl.export_serial = g_xe_export_serial;
        tpl.touch_epoch = touch_epoch;
        g_xe_last_vf_template = &tpl;
    }
    if (out_ptr != 0)
        *out_ptr = &tpl.data;
    else
        memcpy(out, &tpl.data, sizeof(*out));
    if (out_vf_gen != 0)
        *out_vf_gen = tpl.vf_gen;
    if (export_slots)
        *export_slots = tpl.export_slots;
    return true;
}

// ---- textures and samplers ----

// The view type the texture of a draw's sampler slot has: a resolve wrote it (slot_resolved) -> the resolved image,
// always 2D (texture_view); else its own dimension.
uint32_t xe_view_type(const s_port_vk_draw *draw, uint32_t fetch)
{
    return slot_resolved(draw, fetch) != 0 ? (uint32_t)k_gpu_view_2d
                                           : view_type_of(draw->samplers[fetch].texture.source.desc.dimension);
}

// The view of a fetch constant's texture for a binding of view type `view_type`, or GPU_NULL_HANDLE when nothing is
// bound, the texture is of another view type (xe_view_type) or could not be converted (texture_view reports it).
gpu_view xe_texture_view(const s_port_vk_draw *draw, uint32_t fetch, uint32_t view_type)
{
    if (fetch >= PORT_VK_SAMPLER_COUNT || !draw->samplers[fetch].bound)
        return GPU_NULL_HANDLE;
    const uint32_t actual = xe_view_type(draw, fetch);
    if (actual != view_type && !(view_type == k_gpu_view_2d_array && actual == k_gpu_view_2d))
        return GPU_NULL_HANDLE;
    const uint32_t image_dim = view_type == k_gpu_view_3d     ? k_shader_pack_image_3d
                               : view_type == k_gpu_view_cube ? k_shader_pack_image_cube
                               : view_type == k_gpu_view_2d_array
                                   ? (actual == k_gpu_view_2d ? k_shader_pack_image_2d_array
                                                                      : k_shader_pack_image_none)
                                   : k_shader_pack_image_2d;
    // looked up again here: a conversion by an earlier fetch of the draw may have grown g_textures
    return texture_view_resolved(&draw->samplers[fetch].texture, image_dim, slot_resolved(draw, fetch));
}

gpu_view xe_dummy(uint32_t view_type)
{
    switch (view_type) {
    case k_gpu_view_3d: return gpu_image_view(g_gpu, g_dummy_3d);
    case k_gpu_view_cube: return gpu_image_view(g_gpu, g_dummy_cube);
    case k_gpu_view_2d_array: return gpu_image_view(g_gpu, g_dummy_array);
    default: return gpu_image_view(g_gpu, g_dummy_2d);
    }
}

inline s_port_vk_sampler_state xe_effective_sampler_state(const s_port_vk_sampler &slot)
{
    if (!slot.bound) {
        s_port_vk_sampler_state state = {};
        state.mag_filter = state.min_filter = state.mip_filter = 1;
        state.max_mip_level = 15;
        return state;
    }
    return sampler_state_of(slot);
}

inline bool xe_same_sampler_state(const s_port_vk_sampler_state &a, const s_port_vk_sampler_state &b)
{
    // Filter and addressing changes can reject the cached sampler before the mip
    // bounds and LOD bias. Compare the latter bitwise, including signed zero.
    constexpr size_t mip = offsetof(s_port_vk_sampler_state, lod_bias);
    return !memcmp(&a, &b, mip) &&
           !memcmp((const uint8_t *)&a + mip, (const uint8_t *)&b + mip, sizeof(a) - mip);
}

gpu_sampler xe_sampler_from_state(s_port_vk_sampler_state state, uint32_t variant, uint32_t *entry_index)
{
    switch (variant) {
    case XE_SAMPLER_POINT: state.mag_filter = state.min_filter = 0; break;
    case XE_SAMPLER_POINT_MIP_POINT:
        state.mag_filter = state.min_filter = 0;
        if (state.mip_filter != 2)
            state.mip_filter = 0;
        break;
    case XE_SAMPLER_LINEAR: state.mag_filter = state.min_filter = 1; break;
    case XE_SAMPLER_LINEAR_MIP_LINEAR:
        state.mag_filter = state.min_filter = 1;
        if (state.mip_filter != 2)
            state.mip_filter = 1;
        break;
    case XE_SAMPLER_NO_ANISO:
        state.mag_filter = state.mag_filter == 4 ? 1u : state.mag_filter;
        state.min_filter = state.min_filter == 4 ? 1u : state.min_filter;
        break;
    default: break;
    }
    return sampler_for(state, false, entry_index);
}

// Each token belongs to one native view and descriptor binding. The slot cache holds the
// token pointer only while its view epoch is current.
s_vulkan_bindless_token *xe_view_token(s_gpu_image *owner, gpu_view view, uint32_t binding)
{
    if (!owner || view == GPU_NULL_HANDLE)
        return 0;
    auto *image = (s_vulkan_image *)owner;
    const VkImageView native = vulkan_gpu_detail::handle_cast<VkImageView>(view);
    if (native == image->view)
        return &image->bindless_slots[binding];
    if (native == image->sampled_view)
        return &image->bindless_sampled[binding];
    if (binding == 3 && native == image->array_view)
        return &image->bindless_array;
    if (binding == 0 && native == image->swizzled_view)
        return &image->bindless_swizzled;
    return 0;
}

// Set 1 for the textures the draw's shaders fetch (a binding no shader uses is left unwritten: never accessed); a set
// made earlier (this frame or before: g_xe_texture_set_epoch) with the same descriptors is bound again.
bool xe_texture_set_once(const s_port_vk_draw *draw, const s_shader_pack_xenos *const *metas, uint32_t meta_count,
                         s_xe_draw_textures *out, uint32_t *out_linear_gamma_mask)
{
    const uint32_t resolve_epoch = __atomic_load_n(&g_resolve_epoch, __ATOMIC_ACQUIRE);
    const uint32_t touch_epoch = __atomic_load_n(&g_port_vk_resource_touch_epoch, __ATOMIC_ACQUIRE);
    const auto view_epoch = []() { return g_vk ? g_vk->view_epoch : gpu_view_epoch(g_gpu); };
    const uint32_t epoch_gathered = view_epoch(); // before the views are looked up (made)
    const uint32_t draw_scale = bound_res_scale();
    const uint32_t m0 = (meta_count > 0 && metas[0] != 0) ? metas[0]->index : SHADER_PACK_NOT_FOUND;
    const uint32_t m1 = (meta_count > 1 && metas[1] != 0) ? metas[1]->index : SHADER_PACK_NOT_FOUND;
    const bool versioned = !g_texture_recheck && meta_count <= 2 && draw->samplers_any_gen != 0;
    auto reuse = [&](s_xe_last_texture_set &cached) {
        if (g_xe_abi6) {
            out->tables = &g_xe_v6_texture_tables[&cached - g_xe_last_tex_set];
            if (cached.bindless_recording != g_vk->bindless_serial) {
                // Pin the cached slots for this recording by index. A revision change invalidates
                // the cache before any previously evicted index can be used again.
                for (uint32_t m = 0; m < meta_count; ++m) {
                    if (const auto *meta = metas[m]) {
                        for (uint32_t i = 0; i < meta->module_meta->image_count; ++i) {
                            const uint32_t dim = meta->image_keys[i] >> 5;
                            g_vk->bindless.bindings[dim].slots[out->tables->images[m][i]].last_use = g_vk->bindless_serial;
                        }
                        for (uint32_t i = 0; i < meta->module_meta->sampler_count; ++i)
                            g_vk->bindless.bindings[4].slots[out->tables->samplers[m][i]].last_use = g_vk->bindless_serial;
                    }
                }
                cached.bindless_recording = g_vk->bindless_serial;
            }
        } else
            out->set = cached.set;
        if (out_linear_gamma_mask != 0)
            *out_linear_gamma_mask = cached.linear_gamma_mask;
        return true;
    };
    const uint32_t tex_cache_idx = ((m0 * 2654435761u) ^ (m1 * 2246822519u) ^ meta_count) & (k_xe_tex_set_cache_size - 1u);
    s_xe_last_texture_set &tc = g_xe_last_tex_set[tex_cache_idx];
    const bool same_shaders = tc.valid && tc.meta_count == meta_count && tc.meta_index[0] == m0 && tc.meta_index[1] == m1;
    uint32_t changed_slots = UINT32_MAX;
    bool patch_tables = false;
    if (versioned && same_shaders && tc.resolve_epoch == resolve_epoch && tc.view_epoch == epoch_gathered &&
        tc.touch_epoch == touch_epoch && tc.export_serial == g_xe_export_serial && tc.draw_scale == draw_scale &&
        (!g_xe_abi6 || tc.bindless_revision == g_vk->bindless_revision)) {
        if (draw->samplers_any_gen == tc.samplers_any_gen)
            return reuse(tc);
        patch_tables = g_xe_abi6 && tc.bindless_recording == g_vk->bindless_serial;
        uint32_t changes;
        const auto *packet_changes = record_changes_for(draw);
        if (packet_changes && tc.samplers_any_gen == packet_changes->sampler_gen) {
            changes = tc.fetch_mask & packet_changes->sampler_slots;
        } else {
            changes = 0;
            for (uint32_t mask = tc.fetch_mask; mask; mask &= mask - 1u) {
                const uint32_t s = (uint32_t)__builtin_ctz(mask);
                if (!draw->sampler_gen[s] || draw->sampler_gen[s] != tc.slot_gen[s]) {
                    changes |= 1u << s;
                    if (!patch_tables)
                        break;
                }
            }
        }
        if (!changes) {
            tc.samplers_any_gen = draw->samplers_any_gen;
            return reuse(tc);
        }
        if (patch_tables)
            changed_slots = changes;
    }
    // written before they are read (count): not zeroed on every draw (-ftrivial-auto-var-init)
    [[clang::uninitialized]] uint32_t binding[k_xe_max_texture_writes];
    [[clang::uninitialized]] uint64_t handle[k_xe_max_texture_writes];
    uint32_t count = 0;
    uint32_t fetch_mask = patch_tables ? tc.fetch_mask : 0u;
    uint32_t linear_gamma_mask = patch_tables ? tc.linear_gamma_mask & ~changed_slots : 0u;
    bool cacheable = versioned;
    bool overflow = false;
    uint64_t seen[5] = {};
    auto add = [&](uint32_t b, uint64_t h) {
        if (b < 320u) {
            const uint64_t bit = 1ull << (b & 63u);
            if ((seen[b >> 6] & bit) != 0)
                return;
            seen[b >> 6] |= bit;
        } else {
            for (uint32_t i = 0; i < count; i++)
                if (binding[i] == b)
                    return;
        }
        if (count == k_xe_max_texture_writes) {
            overflow = true;
            return;
        }
        binding[count] = b;
        handle[count++] = h;
    };
    auto get_slot_cache = [&](uint32_t fetch) -> s_xe_slot_cache & {
        s_xe_slot_cache &sc = g_xe_slot_cache[fetch];
        const uint32_t sgen = draw->sampler_gen[fetch];
        // An earlier fetch in this set can retire a view while converting or repacking an aliased texture.
        const uint32_t slot_view_epoch = view_epoch();
        const uint32_t slot_resolve_epoch = __atomic_load_n(&g_resolve_epoch, __ATOMIC_ACQUIRE);
        const uint32_t slot_touch_epoch = __atomic_load_n(&g_port_vk_resource_touch_epoch, __ATOMIC_ACQUIRE);
        if (sgen == 0)
            cacheable = false;
        if (g_texture_recheck || sgen == 0 || sc.gen != sgen || sc.resolve_epoch != slot_resolve_epoch ||
            sc.view_epoch != slot_view_epoch || sc.touch_epoch != slot_touch_epoch ||
            sc.export_serial != g_xe_export_serial || sc.draw_scale != draw_scale) {
            const s_port_vk_sampler &slot = draw->samplers[fetch];
            s_texture_entry *resolved = slot.bound ? slot_resolved(draw, fetch) : 0;
            sc.actual_type = resolved != 0 ? (uint32_t)k_gpu_view_2d
                                           : view_type_of(slot.texture.source.desc.dimension);
            sc.resolved_index = resolved != 0 ? (uint32_t)(resolved - g_textures.items) + 1u : 0u;
            sc.view_valid = 0;
            sc.array_view_valid = 0;
            sc.skip_gamma = 0;
            sc.gen = sgen;
            sc.resolve_epoch = slot_resolve_epoch;
            sc.view_epoch = slot_view_epoch;
            sc.touch_epoch = slot_touch_epoch;
            sc.export_serial = g_xe_export_serial;
            sc.draw_scale = draw_scale;
        }
        return sc;
    };
    auto slot_view = [&](uint32_t fetch, s_xe_slot_cache &sc, uint32_t view_type, uint32_t binding = 0,
                         s_vulkan_bindless_token **token = 0) -> gpu_view {
        const s_port_vk_sampler &slot = draw->samplers[fetch];
        if (!slot.bound)
            return GPU_NULL_HANDLE;
        if (view_type == sc.actual_type) {
            if (!sc.view_valid) {
                s_texture_entry *resolved =
                    sc.resolved_index != 0 && sc.resolved_index <= g_textures.count
                        ? &g_textures.items[sc.resolved_index - 1u]
                        : 0;
                const uint32_t image_dim = view_type == k_gpu_view_3d         ? k_shader_pack_image_3d
                                           : view_type == k_gpu_view_cube     ? k_shader_pack_image_cube
                                           : view_type == k_gpu_view_2d_array ? k_shader_pack_image_none
                                                                              : k_shader_pack_image_2d;
                bool skip_gamma = false;
                bool view_cacheable = true;
                s_gpu_image *owner = 0;
                sc.view = texture_view_resolved(&slot.texture, image_dim, resolved, &skip_gamma, &view_cacheable,
                                                token ? &owner : 0);
                if (token)
                    sc.view_token = xe_view_token(owner, sc.view, binding);
                sc.view_valid = view_cacheable && sc.view != GPU_NULL_HANDLE;
                sc.skip_gamma = skip_gamma && sc.view != GPU_NULL_HANDLE;
            }
            if (sc.skip_gamma != 0)
                linear_gamma_mask |= 1u << fetch;
            if (!sc.view_valid)
                cacheable = false;
            if (token)
                *token = sc.view_token;
            return sc.view;
        }
        if (view_type == k_gpu_view_2d_array && sc.actual_type == k_gpu_view_2d) {
            if (!sc.array_view_valid) {
                s_texture_entry *resolved =
                    sc.resolved_index != 0 && sc.resolved_index <= g_textures.count
                        ? &g_textures.items[sc.resolved_index - 1u]
                        : 0;
                s_gpu_image *owner = 0;
                sc.array_view = texture_view_resolved(&slot.texture, k_shader_pack_image_2d_array, resolved, 0, 0,
                                                      token ? &owner : 0);
                if (token)
                    sc.array_view_token = xe_view_token(owner, sc.array_view, binding);
                if (sc.array_view != GPU_NULL_HANDLE)
                    sc.array_view_valid = 1;
            }
            if (!sc.array_view_valid)
                cacheable = false;
            if (token)
                *token = sc.array_view_token;
            return sc.array_view;
        }
        return GPU_NULL_HANDLE;
    };
    auto slot_sampler = [&](uint32_t fetch, uint32_t variant, s_xe_slot_cache *sc,
                            uint32_t *entry_index = 0) -> gpu_sampler {
        static const s_port_vk_sampler k_unbound = {};
        if (!sc || variant >= 6)
            return xe_sampler_from_state(xe_effective_sampler_state(fetch < PORT_VK_SAMPLER_COUNT
                                                                      ? draw->samplers[fetch] : k_unbound),
                                         variant, entry_index);
        // Samplers depend on the slot state and texture mip bounds. Resolving an
        // image or checking its view epochs again here adds no sampler information.
        const uint32_t gen = draw->sampler_gen[fetch];
        if (!gen || sc->sampler_gen != gen || !sc->last_state_valid) {
            const auto state = xe_effective_sampler_state(draw->samplers[fetch]);
            if (!sc->last_state_valid || !xe_same_sampler_state(sc->last_state, state)) {
                sc->last_state = state;
                sc->sampler_valid_mask = 0;
                sc->last_state_valid = 1;
            }
            sc->sampler_gen = gen;
            if (!gen)
                cacheable = false;
        }
        const uint8_t bit = (uint8_t)(1u << variant);
        if ((sc->sampler_valid_mask & bit) == 0) {
            sc->sampler[variant] = xe_sampler_from_state(sc->last_state, variant, &sc->sampler_entry[variant]);
            if (sc->sampler[variant] != g_default_sampler)
                sc->sampler_valid_mask |= bit;
        }
        if (sc->sampler[variant] == g_default_sampler)
            cacheable = false;
        if (entry_index)
            *entry_index = sc->sampler_entry[variant];
        return sc->sampler[variant];
    };
    auto remember_last_set = [&](gpu_bind_group set, uint32_t cur_epoch) {
        if (cacheable && cur_epoch == epoch_gathered &&
            __atomic_load_n(&g_resolve_epoch, __ATOMIC_ACQUIRE) == resolve_epoch &&
            __atomic_load_n(&g_port_vk_resource_touch_epoch, __ATOMIC_ACQUIRE) == touch_epoch) {
            tc.valid = 1;
            tc.meta_count = meta_count;
            tc.meta_index[0] = m0;
            tc.meta_index[1] = m1;
            tc.fetch_mask = fetch_mask;
            tc.samplers_any_gen = draw->samplers_any_gen;
            uint32_t fm = fetch_mask & changed_slots;
            while (fm) {
                const uint32_t s = (uint32_t)__builtin_ctz(fm);
                fm &= fm - 1u;
                tc.slot_gen[s] = draw->sampler_gen[s];
            }
            tc.resolve_epoch = resolve_epoch;
            tc.view_epoch = cur_epoch;
            tc.touch_epoch = touch_epoch;
            tc.export_serial = g_xe_export_serial;
            tc.draw_scale = draw_scale;
            tc.linear_gamma_mask = linear_gamma_mask;
            tc.set = set;
            if (g_xe_abi6) {
                tc.bindless_recording = g_vk->bindless_serial;
                tc.bindless_revision = g_vk->bindless_revision;
            }
        }
    };
    if (g_xe_abi6) {
        s_xe_v6_texture_tables &tables = g_xe_v6_texture_tables[tex_cache_idx];
        if (!same_shaders) // table lengths can differ even when their common prefix has the same indices
            tables.dirty_tables = 15u;
        tc.valid = 0;
        static const uint32_t types[] = { k_gpu_view_2d, k_gpu_view_3d, k_gpu_view_cube, k_gpu_view_2d_array };
        for (uint32_t stage = 0; stage < meta_count; ++stage) {
            const s_shader_pack_xenos *meta = metas[stage];
            if (!meta)
                continue;
            for (uint32_t i = 0; i < meta->module_meta->image_count; ++i) {
                const uint32_t key = meta->image_keys[i], dim = key >> 5, fetch = key & 31u;
                if ((changed_slots & (1u << fetch)) == 0)
                    continue;
                gpu_view view = GPU_NULL_HANDLE;
                s_vulkan_bindless_token *token = 0;
                if (fetch < PORT_VK_SAMPLER_COUNT) {
                    fetch_mask |= 1u << fetch;
                    view = slot_view(fetch, get_slot_cache(fetch), types[dim], dim, &token);
                }
                const uint64_t identity = vulkan_gpu_detail::handle_cast<uint64_t>(view);
                const uint32_t slot = token ? vulkan_bindless_slot_fast(g_vk, dim, identity, *token)
                                           : identity ? vulkan_bindless_slot(g_vk, dim, identity) : 0u;
                if (slot == UINT32_MAX)
                    return false;
                if (tables.images[stage][i] != slot) {
                    tables.images[stage][i] = slot;
                    tables.dirty_tables |= 1u << stage;
                }
            }
            for (uint32_t i = 0; i < meta->module_meta->sampler_count; ++i) {
                const uint32_t key = meta->sampler_keys[i], variant = key >> 5, fetch = key & 31u;
                if ((changed_slots & (1u << fetch)) == 0)
                    continue;
                s_xe_slot_cache *sc = 0;
                if (fetch < PORT_VK_SAMPLER_COUNT) {
                    fetch_mask |= 1u << fetch;
                    sc = &g_xe_slot_cache[fetch];
                }
                uint32_t entry = 0;
                const gpu_sampler sampler = slot_sampler(fetch, variant, sc, &entry);
                const uint64_t identity = vulkan_gpu_detail::handle_cast<uint64_t>(sampler);
                const uint32_t slot = entry ? vulkan_bindless_slot_fast(g_vk, 4, identity, g_samplers.items[entry - 1u].bindless)
                                           : vulkan_bindless_slot(g_vk, 4, identity);
                if (slot == UINT32_MAX)
                    return false;
                if (tables.samplers[stage][i] != slot) {
                    tables.samplers[stage][i] = slot;
                    tables.dirty_tables |= 4u << stage;
                }
            }
        }
        if (patch_tables && (view_epoch() != epoch_gathered ||
            __atomic_load_n(&g_resolve_epoch, __ATOMIC_ACQUIRE) != resolve_epoch ||
            __atomic_load_n(&g_port_vk_resource_touch_epoch, __ATOMIC_ACQUIRE) != touch_epoch)) {
            // Gathering a changed fetch can retire a view shared by a skipped fetch. The invalidated
            // table forces a full gather on the retry.
            return xe_texture_set_once(draw, metas, meta_count, out, out_linear_gamma_mask);
        }
        out->tables = &tables;
        *out_linear_gamma_mask = linear_gamma_mask;
        remember_last_set(GPU_NULL_HANDLE, view_epoch());
        return true;
    }

    static const s_port_vk_sampler k_unbound = {};
    for (uint32_t m = 0; m < meta_count; m++) {
        const s_shader_pack_xenos *meta = metas[m];
        for (uint32_t t = 0; meta != 0 && t < meta->texture_count; t++) {
            const s_shader_pack_file_xenos_texture &use = meta->textures[t];
            const bool valid_slot = use.fetch < PORT_VK_SAMPLER_COUNT;
            const s_port_vk_sampler &slot = valid_slot ? draw->samplers[use.fetch] : k_unbound;
            s_xe_slot_cache *sc = 0;
            if (valid_slot) {
                fetch_mask |= 1u << use.fetch;
                sc = &get_slot_cache(use.fetch);
            }
            if (use.dim == 1 || use.dim == 3) {
                // tfetch3D: a 3D texture or stacked 2D layers, by the fetch constant (both bound, one a dummy)
                gpu_view v3 = sc != 0 ? slot_view(use.fetch, *sc, k_gpu_view_3d) : GPU_NULL_HANDLE;
                gpu_view va = sc != 0 ? slot_view(use.fetch, *sc, k_gpu_view_2d_array) : GPU_NULL_HANDLE;
                add(XE_BINDING_TEX_3D + use.fetch, (uint64_t)(v3 != GPU_NULL_HANDLE ? v3 : xe_dummy(k_gpu_view_3d)));
                add(XE_BINDING_TEX_STACKED + use.fetch,
                    (uint64_t)(va != GPU_NULL_HANDLE ? va : xe_dummy(k_gpu_view_2d_array)));
            } else {
                const uint32_t type = use.dim == 2 ? (uint32_t)k_gpu_view_cube : (uint32_t)k_gpu_view_2d;
                gpu_view v = sc != 0 ? slot_view(use.fetch, *sc, type) : GPU_NULL_HANDLE;
                if (slot.bound && sc != 0 && sc->actual_type != type) {
                    const uint32_t actual = sc->actual_type;
                    [[clang::uninitialized]] char detail[200];
                    snprintf(detail, sizeof(detail), "%s fetch %u (%s) of a %s texture (format %08x %ux%ux%u)",
                             meta->name, (uint32_t)use.fetch, use.dim == 2 ? "cube" : "2D",
                             actual == k_gpu_view_3d           ? "3D"
                             : actual == k_gpu_view_cube       ? "cube"
                             : actual == k_gpu_view_2d_array ? "stacked"
                                                                       : "2D",
                             slot.texture.source.desc.d3d_format, slot.texture.source.desc.width,
                             slot.texture.source.desc.height, slot.texture.source.desc.depth);
                    report(meta->index * 0x101u ^ use.fetch ^ 0x78747666u,
                           "a texture fetch of another dimension than the bound texture's (dummy texture used)", detail);
                }
                add((use.dim == 2 ? XE_BINDING_TEX_CUBE : XE_BINDING_TEX_2D) + use.fetch,
                    (uint64_t)(v != GPU_NULL_HANDLE ? v : xe_dummy(type)));
            }
            const gpu_sampler samp = slot_sampler(use.fetch, use.sampler, sc);
            if (samp == g_default_sampler)
                cacheable = false;
            add(XE_BINDING_SAMPLER + XE_SAMPLER_VARIANT_STRIDE * use.sampler + use.fetch, (uint64_t)samp);
        }
    }
    if (overflow) {
        report(metas[0]->index ^ 0x74786f76u, "a draw fetching more textures and samplers than a set records (skipped)",
               metas[0]->name);
        return false;
    }
    if (out_linear_gamma_mask != 0)
        *out_linear_gamma_mask = linear_gamma_mask;
    out->set = GPU_NULL_HANDLE;
    if (count == 0) {
        remember_last_set(GPU_NULL_HANDLE, view_epoch());
        return true;
    }
    const uint32_t epoch = view_epoch();
    if (epoch != epoch_gathered) {
        // a view or sampler went away while the handles were gathered (it may be one of them, valid until the frame's
        // fence): a set of this frame's only, never cached
        uint32_t fm = fetch_mask;
        while (fm) {
            const uint32_t s = (uint32_t)__builtin_ctz(fm);
            fm &= fm - 1u;
            g_xe_slot_cache[s].gen = 0;
        }
        for (uint32_t i = 0; i < k_xe_tex_set_cache_size; i++)
            g_xe_last_tex_set[i].valid = 0;
        gpu_bind_group set = gpu_bind_group_allocate(g_gpu, g_xe_set_layout[1], k_gpu_bind_frame_textures);
        if (set == GPU_NULL_HANDLE)
            return false;
        gpu_bind_write_textures(g_gpu, set, count, binding, handle, XE_BINDING_SAMPLER);
        g_frame_trace.texture_sets++;
        out->set = set;
        return true;
    }
    if (g_xe_texture_set_epoch != epoch || g_xe_texture_sets.count >= k_xe_texture_set_cap) {
        // a cached set may hold a view or sampler that is gone (or the cache is full): drop them all (their pools are
        // destroyed once the frames that bound them are done)
        gpu_bind_groups_reset_cached(g_gpu);
        g_xe_texture_set_epoch = epoch;
        g_xe_texture_sets.count = 0;
        map_free(&g_xe_texture_set_index);
        for (uint32_t i = 0; i < k_xe_tex_set_cache_size; i++)
            g_xe_last_tex_set[i].valid = 0;
    }
    uint32_t hash = xe_hash_words(handle, count * 8u, xe_hash_words(binding, count * 4u));
    hash = hash != 0 ? hash : 1u;
    uint32_t head;
    if (map_find(&g_xe_texture_set_index, hash, &head)) {
        for (uint32_t i = head; i != 0 && i <= g_xe_texture_sets.count; i = g_xe_texture_sets.items[i - 1u].next) {
            const s_xe_texture_set &s = g_xe_texture_sets.items[i - 1u];
            if (s.count == count && memcmp(s.binding, binding, count * 4u) == 0 &&
                memcmp(s.handle, handle, count * 8u) == 0) {
                out->set = s.set;
                remember_last_set(s.set, epoch);
                return true;
            }
        }
    }
    gpu_bind_group set = gpu_bind_group_allocate(g_gpu, g_xe_set_layout[1], k_gpu_bind_cached_textures);
    if (set == GPU_NULL_HANDLE)
        return false;
    gpu_bind_write_textures(g_gpu, set, count, binding, handle, XE_BINDING_SAMPLER);
    g_frame_trace.texture_sets++;
    if (s_xe_texture_set *s = g_xe_texture_sets.push()) {
        s->hash = hash;
        s->count = count;
        memcpy(s->binding, binding, count * 4u);
        memcpy(s->handle, handle, count * 8u);
        s->set = set;
        if (uint32_t *slot = map_slot(&g_xe_texture_set_index, hash, true)) {
            s->next = *slot;
            *slot = g_xe_texture_sets.count;
        }
    }
    out->set = set;
    remember_last_set(set, epoch);
    return true;
}

// If all slots of a binding were used by this command buffer, submit it and retry preparation.
bool xe_texture_set(const s_port_vk_draw *draw, const s_shader_pack_xenos *const *metas,
                    uint32_t count, s_xe_draw_textures *out, uint32_t *linear_gamma_mask)
{
    if (g_xe_abi6) {
        bool empty = true;
        for (uint32_t s = count; s-- > 0;) {
            if (metas[s] && (metas[s]->module_meta->image_count || metas[s]->module_meta->sampler_count)) {
                empty = false;
                break;
            }
        }
        if (empty) {
            static s_xe_v6_texture_tables empty_tables;
            out->tables = &empty_tables;
            *linear_gamma_mask = 0;
            return true;
        }
    }
    if (xe_texture_set_once(draw, metas, count, out, linear_gamma_mask))
        return true;
    if (!g_xe_abi6)
        return false;

    s_query_entry *query = g_active_query && gpu_occlusion_active(g_gpu) ? query_entry(g_active_query, false) : 0;
    if (query)
        gpu_occlusion_end(g_gpu);
    if (finish_frame_wait() != k_gpu_ok)
        return false;
    if (query) {
        const uint32_t i = query->count - 1u;
        uint64_t samples = 0;
        if (!gpu_occlusion_result(g_gpu, query->serial[i], query->record[i], &samples))
            return false;
        query->completed_samples += samples;
        query->count = i;
    }
    if (!begin_frame())
        return false;
    if (query) {
        const uint32_t i = query->count;
        if (!gpu_occlusion_begin(g_gpu, &query->serial[i], &query->record[i]))
            return false;
        ++query->count;
    }
    if (xe_texture_set_once(draw, metas, count, out, linear_gamma_mask))
        return true;
    report(0x626e646cu, "unable to allocate ABI 6 bindless descriptor slots");
    return false;
}

// ---- system constants, binding ----

void xe_system_fill(const s_port_vk_draw *draw, xe_system *sys)
{
    memset(sys, 0, sizeof(*sys));
    sys->ndc_scale[0] = sys->ndc_scale[1] = sys->ndc_scale[2] = sys->ndc_scale[3] = 1.0f;
    // guest pixels per host pixel (1/HALO3_RES_SCALE): VPOS = floor(host pixel) * pixel_scale stays in guest pixels, as
    // xenia applies its resolution scale inversion after truncating; VPOS is tile-relative as on the Xbox (the shaders
    // add their own tile offset, the render target is the tile's), and the half-pixel offset is the viewport's
    // (set_viewport_scissor). Point sizes are divided by it (io.odin).
    sys->pixel_scale[0] = sys->pixel_scale[1] = 1.0f / (float)bound_res_scale();
    // the Xenos alpha test on oC0.a (fixed function: RB_COLORCONTROL), D3DCMP_* = the Xenos CompareFunction
    const s_port_vk_render_state &rs = draw->state;
    sys->alpha_test[0] = rs.alpha_test_enable ? rs.alpha_func & 7u : 7u;
    const float reference = (float)(rs.alpha_ref & 0xffu) * (1.0f / 255.0f); // D3DRS_ALPHAREF is a BYTE
    memcpy(&sys->alpha_test[1], &reference, 4);
    for (uint32_t i = 0; i < 4; i++) {
        int32_t bias = g_bound_color[i] != 0 ? g_bound_color[i]->color_exp_bias : 0;
        bias = bias < -32 ? -32 : bias > 31 ? 31 : bias;
        const uint32_t bits = (uint32_t)(127 + bias) << 23; // 2^bias (RB_COLORi_INFO.ColorExpBias)
        memcpy(&sys->color_exp_bias[i], &bits, 4);
    }
    // the vertex shader point size clamp (PA_SU_POINT_MINMAX, guest pixel diameters; io.odin divides by pixel_scale)
    sys->point[0] = draw->point_size_min;
    sys->point[1] = draw->point_size_max;
    // guest pixels to NDC, 2 / the guest viewport size, for a point geometry shader that expands points itself (the
    // Direct3D 12 pack's: caps.point_sprites 0); the SPIR-V shaders leave it to the rasterizer (PointSize) and never
    // read it. A draw without a viewport covers the rendering area (set_viewport_scissor).
    float viewport_width = draw->viewport[2], viewport_height = draw->viewport[3];
    if (viewport_width <= 0.0f || viewport_height <= 0.0f) {
        uint32_t width = 0, height = 0;
        gpu_rendering(g_gpu, &width, &height);
        viewport_width = (float)width / (float)bound_res_scale();
        viewport_height = (float)height / (float)bound_res_scale();
    }
    sys->point[2] = viewport_width > 0.0f ? 2.0f / viewport_width : 0.0f;
    sys->point[3] = viewport_height > 0.0f ? 2.0f / viewport_height : 0.0f;
    sys->tessellation[0] = 1.0f;
    sys->tessellation[1] = 64.0f;
    for (uint32_t i = 0; i < XE_CLIP_PLANES; i++)
        if ((draw->clip_plane_enable & (1u << i)) != 0)
            memcpy(sys->clip_planes[i], draw->clip_planes[i], 16);
}

// Fast cached path for xe_system when clip_plane_enable == 0 and no per-draw overrides (point sprites / base vertex):
// returns pointer to g_xe_sys_cache.sys and its generation counter so xe_block(k_xe_block_system) hits in O(1).
const xe_system *xe_system_fill_cached(const s_port_vk_draw *draw, uint32_t *out_gen)
{
    const uint32_t res_scale = bound_res_scale();
    const s_port_vk_render_state &rs = draw->state;
    const uint32_t alpha_packed = (rs.alpha_test_enable ? (rs.alpha_func & 7u) : 7u) | ((rs.alpha_ref & 0xffu) << 8);
    int32_t biases[4];
    for (uint32_t i = 0; i < 4; i++) {
        int32_t b = g_bound_color[i] != 0 ? g_bound_color[i]->color_exp_bias : 0;
        biases[i] = b < -32 ? -32 : b > 31 ? 31 : b;
    }
    float viewport_width = draw->viewport[2], viewport_height = draw->viewport[3];
    if (viewport_width <= 0.0f || viewport_height <= 0.0f) {
        uint32_t width = 0, height = 0;
        gpu_rendering(g_gpu, &width, &height);
        viewport_width = (float)width / (float)res_scale;
        viewport_height = (float)height / (float)res_scale;
    }
    s_xe_sys_cache &c = g_xe_sys_cache;
    if (!c.valid || c.res_scale != res_scale || c.alpha_packed != alpha_packed ||
        c.color_exp_bias[0] != biases[0] || c.color_exp_bias[1] != biases[1] ||
        c.color_exp_bias[2] != biases[2] || c.color_exp_bias[3] != biases[3] ||
        c.point_min != draw->point_size_min || c.point_max != draw->point_size_max ||
        c.viewport_w != viewport_width || c.viewport_h != viewport_height) {
        xe_system_fill(draw, &c.sys);
        c.valid = 1;
        c.res_scale = res_scale;
        c.alpha_packed = alpha_packed;
        memcpy(c.color_exp_bias, biases, sizeof(biases));
        c.point_min = draw->point_size_min;
        c.point_max = draw->point_size_max;
        c.viewport_w = viewport_width;
        c.viewport_h = viewport_height;
        if (++c.gen == 0)
            c.gen = 1;
    }
    *out_gen = c.gen;
    return &c.sys;
}

uint32_t xe_prepare_fetch_block(const s_port_vk_draw *draw, const s_xe_rows &rows,
                                uint32_t samp_gen, uint32_t resolve_epoch, uint32_t linear_gamma_mask,
                                uint32_t slots)
{
    const uint32_t derived_slots = (rows.bits[1] >> 16) | (rows.bits[2] << 16);
    if (g_xe_fetch_buf_gen != 0 &&
        (g_xe_fetch_resolve_epoch != resolve_epoch ||
         ((g_xe_fetch_linear_mask ^ linear_gamma_mask) & g_xe_fetch_linear_known & derived_slots) != 0)) {
        g_xe_blocks[k_xe_block_fetch].last_gen = 0;
        g_xe_blocks[k_xe_block_fetch].data_valid = 0;
        g_xe_fetch_buf_gen = 0;
        g_xe_fetch_linear_known = 0;
    }
    bool fetch_hit = false;
    if (samp_gen != 0 && g_xe_fetch_buf_gen == samp_gen && (g_xe_fetch_linear_known & derived_slots) == derived_slots) {
        const s_xe_block_cache &fc = g_xe_blocks[k_xe_block_fetch];
        if (fc.frame == g_frame_serial && fc.last_gen == samp_gen) {
            const uint32_t words = (k_xe_block_size[k_xe_block_fetch] / 16u + 31u) / 32u;
            uint32_t missing = 0;
            for (uint32_t i = 0; i < words; i++)
                missing |= rows.bits[i] & ~fc.valid.bits[i];
            fetch_hit = missing == 0;
        }
    }
    uint32_t fetch_gen = 0;
    if (fetch_hit) {
        fetch_gen = samp_gen;
    } else {
        xe_fetch_constants(draw, rows, linear_gamma_mask, slots);
        // Only tag `fetch` with samp_gen when no stale rows from another gen can be carried from `fetch`.
        if (samp_gen != 0 && (g_xe_fetch_buf_gen == samp_gen || g_xe_blocks[k_xe_block_fetch].last_gen != samp_gen)) {
            if (g_xe_fetch_buf_gen != samp_gen) {
                g_xe_fetch_linear_known = derived_slots;
                g_xe_fetch_linear_mask = linear_gamma_mask & derived_slots;
            } else {
                g_xe_fetch_linear_known |= derived_slots;
                g_xe_fetch_linear_mask =
                    (g_xe_fetch_linear_mask & ~derived_slots) | (linear_gamma_mask & derived_slots);
            }
            g_xe_fetch_buf_gen = samp_gen;
            g_xe_fetch_resolve_epoch = resolve_epoch;
            fetch_gen = samp_gen;
        } else {
            g_xe_fetch_buf_gen = 0;
            g_xe_fetch_linear_known = 0;
        }
    }
    return fetch_gen;
}

void xe_v6_push(s_xe_v6_arena &arena, const s_vulkan_pipeline *pipeline, const s_port_vk_draw *draw,
                const s_shader_pack_xenos *vs, const s_shader_pack_xenos *ps, const s_xe_rows_memo &live,
                s_xe_v6_texture_tables &tables)
{
    auto &push = tables.push;
    push.vs_chunk_table_off = xe_v6_chunk_table(arena, 0, *draw, live, vs);
    push.ps_chunk_table_off = xe_v6_chunk_table(arena, 1, *draw, live, ps);
    const uint32_t dirty = tables.arena_epoch != arena.epoch ? 15u : tables.dirty_tables;
    if (dirty) {
        push.arena_bda = arena.address;
        const s_shader_pack_xenos *metas[] = { vs, ps };
        for (uint32_t s = 0; s < 2; ++s) {
            const auto *meta = metas[s] ? metas[s]->module_meta : 0;
            if (dirty & (1u << s)) {
                auto &offset = s ? push.ps_tex_table_off : push.vs_tex_table_off;
                offset = xe_v6_write_table(arena, tables.images[s], meta ? meta->image_count : 0u);
            }
            if (dirty & (4u << s)) {
                auto &offset = s ? push.ps_samp_table_off : push.vs_samp_table_off;
                offset = xe_v6_write_table(arena, tables.samplers[s], meta ? meta->sampler_count : 0u);
            }
        }
        tables.arena_epoch = arena.epoch;
        tables.dirty_tables = 0;
    }
    constexpr uint32_t chunks = offsetof(xe_push_v6, vs_chunk_table_off);
    constexpr uint32_t textures = offsetof(xe_push_v6, vs_tex_table_off);
    const auto *bytes = (const uint8_t *)&push;
    const auto *last = g_vk->pushed_bytes;
    if (vulkan_recording::command_state_known(g_vk) && g_vk->pushed_layout == pipeline->layout &&
        g_vk->pushed_stages == k_vulkan_xenos_push_stages && !memcmp(last, bytes, chunks)) {
        const bool same_chunks = !memcmp(last + chunks, bytes + chunks, textures - chunks);
        const bool same_textures = !memcmp(last + textures, bytes + textures, sizeof(push) - textures);
        if (same_chunks && same_textures)
            return;
        // The arena address is already installed. A constants-only draw needs just
        // the two chunk offsets. Keep the backend's full push shadow authoritative.
        const uint32_t first = same_chunks ? textures : chunks;
        const uint32_t end = same_textures ? textures : sizeof(push);
        const auto cmd = vulkan_recording::recording_command_buffer(g_vk, "vulkan_cmd_push_constants");
        if (cmd) {
            g_vk->vk.vkCmdPushConstants(cmd, pipeline->layout, k_vulkan_xenos_push_stages, first, end - first, bytes + first);
            memcpy(g_vk->pushed_bytes, bytes, sizeof(push));
        }
    } else {
        vulkan_cmd_push_constants_inline(g_vk, pipeline->layout, k_vulkan_xenos_push_stages, 0, sizeof(push), &push);
    }
}

template <uint32_t block>
inline bool xe_v6_block_hit(const s_xe_rows &rows, uint32_t gen, uint32_t &offset)
{
    if (!rows.chunk_mask) {
        offset = 0;
        return true;
    }
    auto &cache = g_xe_blocks[block];
    if (!gen || cache.frame != g_frame_serial || cache.last_gen != gen)
        return false;
    if (cache.covered_rows != &rows) {
        constexpr uint32_t words = (k_xe_block_size[block] / 16u + 31u) / 32u;
        for (uint32_t i = 0; i < words; ++i)
            if (rows.bits[i] & ~cache.valid.bits[i])
                return false;
        cache.covered_rows = &rows;
    }
    offset = cache.offset;
    return true;
}

bool xe_v6_bind_blocks(const s_xe_rows_memo &live, const s_port_vk_draw *draw, const xe_system *sys,
                       const xe_vertex_fetch *vertex_fetch, uint32_t sys_gen, uint32_t vf_gen,
                       uint32_t linear_gamma_mask, uint32_t offsets[4])
{
    static const xe_vertex_fetch zero_vf = {};
    static uint32_t fetch_gamma, fetch_slots;
    const auto &system_rows = live.rows[k_xe_block_system];
    if (!xe_v6_block_hit<k_xe_block_system>(system_rows, sys_gen, offsets[0]) &&
        !xe_block(k_xe_block_system, sys, live, &offsets[0], 0, sys_gen))
        return false;

    const auto &bool_rows = live.rows[k_xe_block_bool_loop];
    const uint32_t bool_gen = draw->bool_loop_gen;
    auto &flow = g_xe_blocks[k_xe_block_bool_loop];
    const auto *changes = record_changes_for(draw);
    if (bool_rows.chunk_mask && bool_gen && flow.last_gen && flow.last_gen != bool_gen &&
        changes && flow.last_gen == changes->flow_gen && !(flow.valid.bits[0] & changes->flow_rows))
        flow.last_gen = bool_gen;
    if (!xe_v6_block_hit<k_xe_block_bool_loop>(bool_rows, bool_gen, offsets[1])) {
        if (!xe_block(k_xe_block_bool_loop, xe_bool_loop_data(draw, bool_gen), live, &offsets[1], 0, bool_gen))
            return false;
    }

    const auto &fetch_rows = live.rows[k_xe_block_fetch];
    offsets[2] = 0;
    if (fetch_rows.chunk_mask) {
        auto &cache = g_xe_blocks[k_xe_block_fetch];
        const uint32_t gen = draw->samplers_any_gen;
        const uint32_t derived = (fetch_rows.bits[1] >> 16) | (fetch_rows.bits[2] << 16);
        const uint32_t cached_derived = (cache.valid.bits[1] >> 16) | (cache.valid.bits[2] << 16);
        if (gen && cache.last_gen && cache.last_gen != gen && changes &&
            cache.last_gen == changes->sampler_gen && !(fetch_slots & changes->sampler_slots))
            cache.last_gen = gen;
        // The gathered gamma mask is the only view-dependent input to xe_fetch_constants.
        if (cache.last_gen == gen && ((fetch_gamma ^ linear_gamma_mask) & derived & cached_derived)) {
            cache.last_gen = 0; // a fresh upload may only carry rows from the same fetch state
            cache.data_valid = 0;
        }
        if (!xe_v6_block_hit<k_xe_block_fetch>(fetch_rows, gen, offsets[2])) {
            const bool carry = gen && cache.frame == g_frame_serial && cache.last_gen == gen;
            xe_fetch_constants(draw, fetch_rows, linear_gamma_mask, live.fetch_slots);
            if (!xe_block(k_xe_block_fetch, &g_xe_fetch_shadow.data, live, &offsets[2], 0, gen))
                return false;
            fetch_slots = (carry ? fetch_slots : 0u) | live.fetch_slots;
            fetch_gamma = (carry ? fetch_gamma & ~derived : 0u) | (linear_gamma_mask & derived);
        }
    }

    const auto &vf_rows = live.rows[k_xe_block_vertex_fetch];
    if (!vf_rows.chunk_mask) {
        offsets[3] = 0;
        return true;
    }
    auto *tpl = g_xe_last_vf_template;
    if (vf_gen && tpl && vertex_fetch == &tpl->data && tpl->vf_gen == vf_gen) {
        auto &ring = tpl->ring;
        if (ring.frame == g_frame_serial && ring.gen == vf_gen) {
            uint32_t missing = 0;
            for (uint32_t i = 0; i < 4; ++i)
                missing |= vf_rows.bits[i] & ~ring.rows[i];
            if (!missing) {
                offsets[3] = ring.offset;
                return true;
            }
        }
        if (!xe_block(k_xe_block_vertex_fetch, vertex_fetch, live, &offsets[3], 0, vf_gen))
            return false;
        ring.frame = g_frame_serial;
        ring.gen = vf_gen;
        ring.offset = offsets[3];
        __builtin_memcpy(ring.rows, g_xe_blocks[k_xe_block_vertex_fetch].valid.bits, sizeof(ring.rows));
        return true;
    }
    return xe_v6_block_hit<k_xe_block_vertex_fetch>(vf_rows, vf_gen, offsets[3]) ||
           xe_block(k_xe_block_vertex_fetch, vertex_fetch ? vertex_fetch : &zero_vf, live, &offsets[3], 0, vf_gen);
}

// Set 0's blocks into the ring (the memexport table of a shader that exports first) and both sets bound (pipeline's
// bind point). vs: the vertex, tessellation evaluation or compute shader; rows: what the pipeline reads of each block
// (xe_pipeline_rows). stream_views: xe_vertex_fetch_fill's (with vertex_fetch), bound with the memexport destinations
// when the shaders reach them through views rather than device addresses.
bool xe_bind(const s_gpu_pipeline *pipeline, const s_port_vk_draw *draw, const xe_system *sys,
             const s_shader_pack_xenos *vs, const s_shader_pack_xenos *ps, const xe_vertex_fetch *vertex_fetch,
             s_gpu_buffer *const *stream_views, const s_xe_draw_textures &textures, const s_xe_rows_memo &live,
             uint32_t vf_gen, uint32_t sys_gen, uint32_t linear_gamma_mask, uint32_t (*export_targets)[4] = 0)
{
    static const float k_zero_floats[1024] = {};
    static const xe_vertex_fetch zero_fetch = {};
    [[clang::uninitialized]] s_gpu_buffer *export_views[XE_D3D12_MEMEXPORT_BUFFERS];
    [[clang::uninitialized]] xe_system memexport_sys;
    const bool memory_views = !g_vk && !g_xe_device_addresses;
    bool has_memexport = false;
    if (vs != 0 && (vs->flags & k_shader_pack_xenos_memexport) != 0) {
        if (memory_views)
            memset(export_views, 0, sizeof(export_views));
        memexport_sys = *sys;
        xe_memexport_streams(*vs, g_shadows.vertex_float, &memexport_sys, memory_views ? export_views : 0);
        sys = &memexport_sys;
        sys_gen = 0;
        has_memexport = true;
    } else if (ps != 0 && (ps->flags & k_shader_pack_xenos_memexport) != 0) {
        if (memory_views)
            memset(export_views, 0, sizeof(export_views));
        memexport_sys = *sys;
        xe_memexport_streams(*ps, g_shadows.pixel_float, &memexport_sys, memory_views ? export_views : 0);
        sys = &memexport_sys;
        sys_gen = 0;
        has_memexport = true;
    }
    if (g_xe_abi6) {
        uint32_t offsets[4];
        if (!xe_v6_bind_blocks(live, draw, sys, vertex_fetch, sys_gen, vertex_fetch ? vf_gen : 0u,
                               linear_gamma_mask, offsets))
            return false;
        s_xe_v6_arena *arena = xe_v6_prepare_arena();
        if (!arena) {
            report(0x61626936u, "unable to allocate a new ABI 6 constant arena");
            return false;
        }
        const auto *native = vulkan_gpu_native_pipeline(pipeline);
        xe_v6_push(*arena, native, draw, vs, ps, live, *textures.tables);
        const VkDescriptorSet set0 = vulkan_gpu_native_bind_group(g_xe_constant_set[g_vk->frame_index]);
        vulkan_cmd_bind_group_pair_native(g_vk, native, set0, 4, offsets, g_vk->bindless.set);
        if (export_targets && has_memexport)
            memcpy(export_targets, sys->memexport, sizeof(sys->memexport));
        return true;
    }
    const uint32_t gens[k_xe_block_count] = {
        sys_gen, draw ? draw->vs_float_gen : 0u, draw ? draw->ps_float_gen : 0u,
        draw ? draw->bool_loop_gen : 0u, draw ? draw->samplers_any_gen : 0u, vertex_fetch ? vf_gen : 0u,
    };
    const uint32_t resolve_epoch = __atomic_load_n(&g_resolve_epoch, __ATOMIC_ACQUIRE);
    const s_xe_rows *rows = live.rows;
    s_xe_offset_tuple &cached = g_xe_offsets;
    const uint32_t all_blocks = (1u << k_xe_block_count) - 1u;
    // Each reused offset still names its immutable upload in the current ring frame.
    const bool same_frame = cached.frame != 0 && cached.frame == g_frame_serial;
    const bool same_layout = cached.layout == &live;
    const bool same_fetch = cached.resolve_epoch == resolve_epoch && cached.linear_gamma_mask == linear_gamma_mask;
    if (!(same_frame && same_layout && same_fetch && cached.reusable == all_blocks &&
          __builtin_memcmp(gens, cached.gens, sizeof(gens)) == 0)) {
        uint32_t reusable = 0;
        uint32_t offsets[k_xe_block_count];
        for (uint32_t b = 0; b < k_xe_block_count; b++) {
            const uint32_t bit = 1u << b;
            if (rows[b].chunk_mask == 0) {
                offsets[b] = 0;
                reusable |= bit;
                continue;
            }
            const bool same_literals = !same_frame || (b == k_xe_block_float_vs ? live.literals[0] == cached.layout->literals[0]
                                       : b == k_xe_block_float_ps ? live.literals[1] == cached.layout->literals[1] : true);
            if (same_frame && (cached.reusable & bit) != 0 && gens[b] != 0 && cached.gens[b] == gens[b] &&
                (b != k_xe_block_fetch || same_fetch) &&
                (same_layout || (same_literals && __builtin_memcmp(rows[b].bits, cached.layout->rows[b].bits,
                                                                   sizeof(rows[b].bits)) == 0))) {
                offsets[b] = cached.offsets[b];
                reusable |= bit;
                continue;
            }
            const void *data = 0;
            const s_xe_dedup_literals *literals = 0;
            const uint32_t *chunks = 0;
            uint64_t shadow_chunks = 0;
            uint32_t gen = gens[b];
            switch (b) {
            case k_xe_block_system:
                data = sys;
                break;
            case k_xe_block_float_vs:
                data = g_shadows.vertex_float ? g_shadows.vertex_float : k_zero_floats;
                literals = xe_literals_at(live.literals[0]);
                chunks = draw ? draw->vs_chunk_gen : 0;
                shadow_chunks = live.shadow_chunks[0];
                break;
            case k_xe_block_float_ps:
                data = g_shadows.pixel_float ? g_shadows.pixel_float : k_zero_floats;
                literals = xe_literals_at(live.literals[1]);
                chunks = draw ? draw->ps_chunk_gen : 0;
                shadow_chunks = live.shadow_chunks[1];
                break;
            case k_xe_block_bool_loop:
                data = xe_bool_loop_data(draw, gen);
                break;
            case k_xe_block_fetch:
                gen = xe_prepare_fetch_block(draw, rows[b], gen, resolve_epoch, linear_gamma_mask,
                                             live.fetch_slots);
                data = &g_xe_fetch_shadow.data;
                break;
            case k_xe_block_vertex_fetch:
                data = vertex_fetch ? vertex_fetch : &zero_fetch;
                break;
            }
            if (!xe_block(b, data, live, &offsets[b], literals, gen, chunks, shadow_chunks))
                return false;
            if (gen != 0)
                reusable |= bit;
        }
        if (!same_layout || cached.frame == 0)
            cached.layout = &live;
        cached.frame = g_frame_serial;
        cached.resolve_epoch = resolve_epoch;
        cached.linear_gamma_mask = linear_gamma_mask;
        cached.reusable = reusable;
        memcpy(cached.gens, gens, sizeof(gens));
        memcpy(cached.offsets, offsets, sizeof(offsets));
    }
    const uint32_t *offsets = cached.offsets;
    const gpu_bind_group cset = g_xe_constant_set[g_vk ? g_vk->frame_index : gpu_frame_index(g_gpu)];
    if (g_vk) {
        const auto *native = vulkan_gpu_native_pipeline(pipeline);
        const VkDescriptorSet set0 = vulkan_gpu_native_bind_group(cset);
        if (textures.set != GPU_NULL_HANDLE) {
            vulkan_cmd_bind_group_pair_native(g_vk, native, set0, k_xe_block_count, offsets,
                                               vulkan_gpu_native_bind_group(textures.set));
        } else {
            vulkan_cmd_bind_group_native(g_vk, native, XE_SET_CONSTANTS, set0, k_xe_block_count, offsets);
        }
    } else if (textures.set != GPU_NULL_HANDLE) {
        gpu_cmd_bind_group_pair(g_gpu, pipeline, cset, k_xe_block_count, offsets, textures.set);
    } else {
        gpu_cmd_bind_group_dynamic(g_gpu, pipeline, XE_SET_CONSTANTS, cset, k_xe_block_count, offsets);
    }
    if (memory_views) {
        static s_gpu_buffer *const k_no_streams[XE_D3D12_VERTEX_STREAMS] = {};
        static s_gpu_buffer *const k_no_exports[XE_D3D12_MEMEXPORT_BUFFERS] = {};
        gpu_cmd_xenos_bind_memory(g_gpu, pipeline, vertex_fetch != 0 && stream_views != 0 ? stream_views : k_no_streams,
                                  has_memexport ? export_views : k_no_exports);
    }
    if (export_targets && has_memexport)
        memcpy(export_targets, sys->memexport, sizeof(sys->memexport));
    return true;
}

// ---- pipeline libraries and their prewarm ----
//
// With pipeline libraries (caps.pipeline_library) a Xenos pipeline is linked from four parts (gpu_backend.h,
// gpu_pipeline_library_*): its vertex input (topology), pre-rasterization (the vertex shader: the expensive part),
// fragment shader (the pixel shader: the expensive part) and fragment output (attachments, blending). Each part is
// compiled once, whatever state the pipelines that use it have; a new pipeline is only a link (fast, well under a
// millisecond), optimized later on a worker and swapped in (HALO3_PIPELINE_OPTIMIZE=0: never).
//
// The expensive parts are compiled ahead of time: when the game creates a shader (port_vk_shader_register, as a map
// loads), its part (and a memexport shader's whole compute pipeline, which needs no libraries) is queued for a pool of
// worker threads (HALO3_PIPELINE_THREADS, 1 to 16; default half the processors, up to 12; HALO3_PIPELINE_PREWARM=0: no
// pool, parts are compiled when a draw first needs them). A draw needing a part no worker has started compiles it at
// once on the draw thread; one being compiled is waited for. HALO3_PIPELINE_ASYNC=1 skips such a draw instead (the
// part goes to the front of the queue): no stall ever, but the first frames after a map loads miss what is not
// compiled yet. Everything goes through the backend's pipeline cache, so pipeline_cache.bin keeps it all for the next
// run. On Vulkan, workers replay the installed graphics keys followed by locally observed keys ahead of bulk
// shader registration. A draw that arrives early can move its exact key to the front of the replay queue.
enum e_xe_part {
    k_xe_part_vertex_input,
    k_xe_part_pre_rasterization,
    k_xe_part_fragment_shader,
    k_xe_part_fragment_output,
    k_xe_part_compute,          // not a library: a memexport shader's whole compute pipeline (xe_compute_pipeline)
};
enum e_xe_library_status {
    k_xe_library_new,      // made by xe_library, not queued
    k_xe_library_queued,   // waiting for a worker
    k_xe_library_building, // being compiled (by a worker or the draw thread)
    k_xe_library_ready,
    k_xe_library_failed,
};

struct s_xe_library {
    uint32_t part;                 // e_xe_part
    uint32_t key[4];               // vertex input: topology, primitive restart, patch control points;
                                   // pre-rasterization: e_xe_pipeline_kind, vertex module, extra (GS module), polygon
                                   // mode; fragment shader: pixel module (SHADER_PACK_NOT_FOUND: none)
    s_gpu_graphics_state output; // fragment output: the attachment part of the state (else zero)
    uint32_t hash;
    uint32_t next;                 // hash collision chain, index + 1 (0: end)
    uint32_t status;               // e_xe_library_status (atomic; changed under g_prewarm.monitor)
    uint32_t worker;               // compiled on a worker
    uint32_t urgent;               // in g_prewarm.urgent (under the monitor)
    gpu_pipeline_handle handle;
};

// An optimized link of a pipeline the draw thread fast-linked (gpu_pipeline_replace once done).
struct s_xe_optimize {
    gpu_pipeline_handle libraries[4];
    s_gpu_pipeline *pipeline;
    gpu_pipeline_handle result;
};

enum e_xe_replay_status {
    k_xe_replay_queued,
    k_xe_replay_building,
    k_xe_replay_done,
};

struct s_xe_replay {
    s_xe_pipeline_key key;
    s_xe_library *parts[4];
    uint32_t vertex_module, pixel_module;
    s_gpu_pipeline *pipeline;
    uint32_t status; // e_xe_replay_status, under the monitor
    uint32_t next;   // hash collision chain, index + 1
};

struct s_xe_key_file_header {
    uint32_t magic, version, key_size, pack, count, hash;
};

const char k_xe_legacy_key_file[] = "pipeline_keys.bin";
char g_xe_key_file[64];
const uint32_t k_xe_key_magic = 0x4b503348u; // "H3PK"
const uint32_t k_xe_key_version = 2;
const uint32_t k_xe_key_monolithic = 0x80000000u; // kind bit: full state usable with or without GPL
const uint32_t k_xe_key_fixed16_rg = 0xfffffffeu; // guest EDRAM color formats that select host formats per GPU
const uint32_t k_xe_key_fixed16_rgba = 0xffffffffu;
const uint32_t k_xe_key_capacity = 8192;
// The versioned wire layout is little-endian 32-bit words, including IEEE binary32 floats. Formats use e_gpu_format.
static_assert(sizeof(s_xe_key_file_header) == 24 && sizeof(s_xe_pipeline_key) == 324 &&
              offsetof(s_xe_pipeline_key, state) == 16 && offsetof(s_gpu_graphics_state, sample_count) == 104 &&
              offsetof(s_gpu_graphics_state, color_formats) == 116 &&
              offsetof(s_gpu_graphics_state, depth_format) == 260 && sizeof(float) == 4 &&
              __FLT_MANT_DIG__ == 24 && __FLT_MAX_EXP__ == 128, "pipeline key wire layout");
t_list<s_xe_pipeline_key> g_xe_keys; // userdata keys and new observations, in first-use order
t_list<s_xe_replay> g_xe_replay;     // fixed after startup, until the workers are joined
s_map g_xe_replay_index;
uint32_t g_xe_key_pack;

void xe_key_cache_load();
void xe_key_cache_write(s_xe_key_file_header *file);
s_gpu_pipeline *xe_replay_build(const s_xe_replay &job);

struct s_prewarm {
    uint32_t replay_head, replay_built, replay_used;
    uint32_t seed_count, seed_pending;
    bool first_seeded_scene;
    s_xe_replay *replay_urgent;
    s_xe_key_file_header *key_save;
    uint32_t key_saved;
    bool key_saving;
    s_port_gpu_monitor *monitor;
    void *threads[16];
    uint32_t thread_count;
    uint32_t quit;
    // pending work (under monitor): libraries for the workers, links to optimize, optimized links done
    t_list<s_xe_library *> queue;
    t_list<s_xe_library *> urgent; // HALO3_PIPELINE_ASYNC: libraries a skipped draw needs, before the queue
    uint32_t queue_head;
    t_list<s_xe_optimize> optimize, optimized;
    uint32_t optimize_head;
    // statistics (under monitor)
    uint32_t built_by_workers, built_by_draw, waited, failed, optimized_count, deferred;
    uint32_t save_requested, saving; // pipeline_cache_save: a write for a worker, one in progress
    double worker_ms;
};
s_prewarm g_prewarm;
bool g_xe_libraries_on;    // the backend has pipeline libraries and the device uses them
bool g_xe_fast_link;       // graphicsPipelineLibraryFastLinking: link fast, optimize on a worker
bool g_xe_optimize = true; // HALO3_PIPELINE_OPTIMIZE
bool g_xe_async;          // HALO3_PIPELINE_ASYNC=1: a draw whose shader parts are not compiled yet is skipped
t_list<s_xe_library *> g_xe_libraries; // the draw thread's (owned; a worker only touches the one it compiles)
s_map g_xe_library_index;  // hash -> head of collision chain (g_xe_libraries index + 1)

void prewarm_lock()
{
    if (g_prewarm.monitor != 0)
        port_gpu_monitor_lock(g_prewarm.monitor);
}

void prewarm_unlock()
{
    if (g_prewarm.monitor != 0)
        port_gpu_monitor_unlock(g_prewarm.monitor);
}

// A pack module's gpu_shader (gpu_pack_xenos_module's cache is not thread-safe: workers share it).
gpu_shader xe_module(uint32_t module_index)
{
    prewarm_lock();
    gpu_shader module = gpu_pack_xenos_module(g_gpu, g_pack, module_index);
    prewarm_unlock();
    return module;
}

long long xe_ticks()
{
    long long count = 0;
    QueryPerformanceCounter(&count);
    return count;
}

double xe_ticks_ms(long long from, long long to)
{
    long long frequency = 0;
    QueryPerformanceFrequency(&frequency);
    return frequency > 0 ? (double)(to - from) * 1000.0 / (double)frequency : 0.0;
}

// Compiles a library (any thread; the caller set it building).
gpu_pipeline_handle xe_library_build(const s_xe_library &library)
{
    switch (library.part) {
    case k_xe_part_vertex_input: {
        s_gpu_graphics_state state = {};
        state.topology = library.key[0];
        state.primitive_restart = library.key[1];
        return gpu_pipeline_library_vertex_input(g_gpu, &state, library.key[2]);
    }
    case k_xe_part_pre_rasterization: {
        const uint32_t kind = library.key[0], vs = library.key[1], extra = library.key[2];
        s_gpu_shader_stage stages[3];
        uint32_t n = 0;
        if (kind == k_xe_pipeline_tessellated) {
            const uint32_t tess_vs = shader_pack_xenos_helper(g_pack, 0), tess_tcs = shader_pack_xenos_helper(g_pack, 1);
            if (tess_vs == SHADER_PACK_NOT_FOUND || tess_tcs == SHADER_PACK_NOT_FOUND)
                return GPU_NULL_HANDLE;
            stages[n++] = s_gpu_shader_stage{ (uint32_t)k_gpu_stage_vertex, xe_module(tess_vs), 0 };
            stages[n++] = s_gpu_shader_stage{ (uint32_t)k_gpu_stage_tess_control, xe_module(tess_tcs), 0 };
            stages[n++] = s_gpu_shader_stage{ (uint32_t)k_gpu_stage_tess_eval, xe_module(vs), 0 };
        } else {
            stages[n++] = s_gpu_shader_stage{ (uint32_t)k_gpu_stage_vertex, xe_module(vs), 0 };
            if (xe_kind_has_geometry(kind))
                stages[n++] = s_gpu_shader_stage{ (uint32_t)k_gpu_stage_geometry, xe_module(extra), 0 };
        }
        s_gpu_graphics_state state = {};
        state.polygon_mode = library.key[3];
        return gpu_pipeline_library_pre_rasterization(g_gpu, stages, n, g_xe_layout, &state,
                                                      kind == k_xe_pipeline_tessellated ? 3u : 0u);
    }
    case k_xe_part_fragment_shader: {
        gpu_shader module = GPU_NULL_HANDLE;
        if (library.key[0] != SHADER_PACK_NOT_FOUND && (module = xe_module(library.key[0])) == GPU_NULL_HANDLE)
            return GPU_NULL_HANDLE;
        return gpu_pipeline_library_fragment_shader(g_gpu, module, g_xe_layout, library.key[1]);
    }
    case k_xe_part_fragment_output:
        return gpu_pipeline_library_fragment_output(g_gpu, &library.output);
    case k_xe_part_compute: {
        gpu_shader module = xe_module(library.key[0]);
        return module != GPU_NULL_HANDLE ? gpu_compute_pipeline_create_handle(g_gpu, module, 0, g_xe_layout)
                                        : GPU_NULL_HANDLE;
    }
    }
    return GPU_NULL_HANDLE;
}

// The finished library's status (under the monitor, which wakes its waiters).
void xe_library_finish(s_xe_library *library, gpu_pipeline_handle handle, bool worker, double ms)
{
    prewarm_lock();
    library->handle = handle;
    library->worker = worker ? 1u : 0u;
    __atomic_store_n(&library->status, handle != GPU_NULL_HANDLE ? (uint32_t)k_xe_library_ready : (uint32_t)k_xe_library_failed,
                     __ATOMIC_RELEASE);
    if (handle == GPU_NULL_HANDLE)
        g_prewarm.failed++;
    else if (worker)
        g_prewarm.built_by_workers++;
    else
        g_prewarm.built_by_draw++;
    if (worker)
        g_prewarm.worker_ms += ms;
    if (g_prewarm.monitor != 0)
        port_gpu_monitor_broadcast(g_prewarm.monitor);
    prewarm_unlock();
}

void prewarm_worker(void *)
{
    port_gpu_thread_set_low_priority();
    s_prewarm &p = g_prewarm;
    port_gpu_monitor_lock(p.monitor);
    for (;;) {
        while (!p.quit && p.urgent.count == 0 && p.queue_head == p.queue.count && !p.save_requested &&
               p.replay_urgent == 0 && p.replay_head == g_xe_replay.count && p.key_save == 0 &&
               p.optimize_head == p.optimize.count)
            port_gpu_monitor_wait(p.monitor);
        if (p.quit)
            break;
        if (p.key_save != 0 && p.replay_urgent == 0) {
            s_xe_key_file_header *file = p.key_save;
            p.key_save = 0;
            p.key_saving = true;
            port_gpu_monitor_unlock(p.monitor);
            xe_key_cache_write(file);
            port_gpu_monitor_lock(p.monitor);
            continue;
        }
        if (p.replay_urgent != 0 || (p.urgent.count == 0 && p.replay_head < g_xe_replay.count)) {
            s_xe_replay &job = p.replay_urgent != 0 ? *p.replay_urgent : g_xe_replay.items[p.replay_head++];
            p.replay_urgent = 0;
            if (job.status != k_xe_replay_queued)
                continue;
            job.status = k_xe_replay_building;
            port_gpu_monitor_unlock(p.monitor);
            s_gpu_pipeline *pipeline = xe_replay_build(job);
            port_gpu_monitor_lock(p.monitor);
            job.pipeline = pipeline;
            job.status = k_xe_replay_done;
            p.replay_built += pipeline != 0 ? 1u : 0u;
            if ((uint32_t)(&job - g_xe_replay.items) < p.seed_count)
                p.seed_pending--;
            port_gpu_monitor_broadcast(p.monitor);
            continue;
        }
        if (p.urgent.count != 0 || p.queue_head < p.queue.count) {
            // what a draw is waiting for first (HALO3_PIPELINE_ASYNC: the newest), then in the order shaders came
            s_xe_library *library = p.urgent.count != 0 ? p.urgent.items[--p.urgent.count] : p.queue.items[p.queue_head++];
            library->urgent = 0;
            if (p.queue_head == p.queue.count)
                p.queue_head = p.queue.count = 0;
            if (__atomic_load_n(&library->status, __ATOMIC_ACQUIRE) != k_xe_library_queued)
                continue; // the draw thread took it
            __atomic_store_n(&library->status, (uint32_t)k_xe_library_building, __ATOMIC_RELEASE);
            port_gpu_monitor_unlock(p.monitor);
            const long long started = xe_ticks();
            gpu_pipeline_handle handle = xe_library_build(*library);
            xe_library_finish(library, handle, true, xe_ticks_ms(started, xe_ticks()));
            port_gpu_monitor_lock(p.monitor);
            continue;
        }
        if (p.save_requested) { // pipeline_cache_save, once no part is waiting
            p.save_requested = 0;
            p.saving = 1;
            port_gpu_monitor_unlock(p.monitor);
            pipeline_cache_write(&g_pipeline_cache_written_size, &g_pipeline_cache_written_hash);
            port_gpu_monitor_lock(p.monitor);
            p.saving = 0;
            port_gpu_monitor_broadcast(p.monitor);
            continue;
        }
        s_xe_optimize job = p.optimize.items[p.optimize_head++];
        if (p.optimize_head == p.optimize.count)
            p.optimize_head = p.optimize.count = 0;
        port_gpu_monitor_unlock(p.monitor);
        job.result = gpu_pipeline_link_handle(g_gpu, job.libraries, g_xe_layout, 1);
        port_gpu_monitor_lock(p.monitor);
        if (job.result != GPU_NULL_HANDLE) {
            if (s_xe_optimize *done = p.optimized.push())
                *done = job;
            else
                gpu_pipeline_library_destroy(g_gpu, job.result);
        }
    }
    port_gpu_monitor_unlock(p.monitor);
}

// Start the pool before bulk Xenos shader registration.
void prewarm_start()
{
    g_xe_libraries_on = g_gpu->caps.pipeline_library != 0;
    g_xe_fast_link = g_gpu->caps.pipeline_library_fast_link != 0;
    const char *optimize = getenv("HALO3_PIPELINE_OPTIMIZE");
    g_xe_optimize = optimize == 0 || strcmp(optimize, "0") != 0;
    const char *async = getenv("HALO3_PIPELINE_ASYNC");
    g_xe_async = async != 0 ? strcmp(async, "0") != 0 : (g_dump_directory == 0);
    const char *prewarm = getenv("HALO3_PIPELINE_PREWARM");
    if (prewarm != 0 && strcmp(prewarm, "0") == 0)
        return;
    const uint32_t cpus = port_gpu_cpu_count();
    uint32_t threads = cpus <= 4u ? 1u : cpus / 2u;
    if (const char *setting = getenv("HALO3_PIPELINE_THREADS")) {
        threads = 0;
        for (const char *c = setting; *c >= '0' && *c <= '9' && threads < 100u; c++)
            threads = threads * 10u + (uint32_t)(*c - '0');
    } else {
        threads = threads > 12u ? 12u : threads;
    }
    threads = threads > 16u ? 16u : threads; // HALO3_PIPELINE_THREADS: up to 16
    if (threads == 0)
        return;
    // the pack's module table attached on this thread first (gpu_pack_xenos_module's first call)
    gpu_pack_xenos_module(g_gpu, g_pack, 0);
    g_prewarm.monitor = port_gpu_monitor_create();
    if (g_prewarm.monitor == 0)
        return;
    if (g_gpu->caps.kind == k_gpu_backend_vulkan) {
        g_xe_link_layout_monitor = port_gpu_monitor_create();
        if (g_xe_link_layout_monitor != 0)
            xe_key_cache_load();
    }
    for (uint32_t i = 0; i < threads; i++) {
        g_prewarm.threads[g_prewarm.thread_count] = port_gpu_thread_start(prewarm_worker, 0);
        if (g_prewarm.threads[g_prewarm.thread_count] != 0)
            g_prewarm.thread_count++;
    }
    if (g_prewarm.thread_count == 0) {
        g_prewarm.replay_head = g_xe_replay.count;
        port_gpu_monitor_destroy(g_prewarm.monitor);
        g_prewarm.monitor = 0;
    }
}

// Stops the pool (xenos_shutdown) and destroys every library; the pipelines linked from them stay valid.
void prewarm_stop()
{
    s_prewarm &p = g_prewarm;
    if (p.monitor != 0) {
        port_gpu_monitor_lock(p.monitor);
        p.quit = 1;
        port_gpu_monitor_broadcast(p.monitor);
        port_gpu_monitor_unlock(p.monitor);
        for (uint32_t i = 0; i < p.thread_count; i++)
            port_gpu_thread_join(p.threads[i]);
        port_gpu_monitor_destroy(p.monitor);
    }
    for (uint32_t i = 0; i < p.optimized.count; i++)
        gpu_pipeline_library_destroy(g_gpu, p.optimized.items[i].result);
    for (uint32_t i = 0; i < g_xe_replay.count; i++)
        if (g_xe_replay.items[i].pipeline != 0)
            gpu_pipeline_destroy(g_gpu, g_xe_replay.items[i].pipeline);
    g_xe_replay.release();
    g_xe_keys.release();
    map_free(&g_xe_replay_index);
    free(p.key_save);
    if (g_xe_link_layout_monitor != 0)
        port_gpu_monitor_destroy(g_xe_link_layout_monitor);
    g_xe_link_layout_monitor = 0;
    p.queue.release();
    p.urgent.release();
    p.optimize.release();
    p.optimized.release();
    for (uint32_t i = 0; i < g_xe_libraries.count; i++) {
        gpu_pipeline_library_destroy(g_gpu, g_xe_libraries.items[i]->handle);
        free(g_xe_libraries.items[i]);
    }
    g_xe_libraries.release();
    map_free(&g_xe_library_index);
    memset(&p, 0, sizeof(p));
    g_xe_libraries_on = false;
}

// pipeline_cache_save's write for a worker (true), or (wait, or no pool: false) any write in progress finished so the
// caller writes itself.
bool prewarm_save_pipeline_cache(bool wait)
{
    s_prewarm &p = g_prewarm;
    if (p.monitor == 0)
        return false;
    port_gpu_monitor_lock(p.monitor);
    if (wait) {
        p.save_requested = 0;
        while (p.saving)
            port_gpu_monitor_wait(p.monitor);
    } else {
        p.save_requested = 1;
        port_gpu_monitor_broadcast(p.monitor);
    }
    port_gpu_monitor_unlock(p.monitor);
    return !wait;
}

// Optimized links the workers finished take the place of their fast links (each frame, before drawing).
void prewarm_frame()
{
    s_prewarm &p = g_prewarm;
    if (p.monitor == 0)
        return;
    port_gpu_monitor_lock(p.monitor);
    for (uint32_t i = 0; i < p.optimized.count; i++)
        gpu_pipeline_replace(g_gpu, p.optimized.items[i].pipeline, p.optimized.items[i].result);
    p.optimized_count += p.optimized.count;
    p.optimized.count = 0;
    port_gpu_monitor_unlock(p.monitor);
}

// The library of a part with this key (made, not compiled, if new).
s_xe_library *xe_library(uint32_t part, uint32_t k0, uint32_t k1, uint32_t k2, uint32_t k3,
                         const s_gpu_graphics_state *output = 0)
{
    s_xe_library probe;
    memset(&probe, 0, sizeof(probe));
    probe.part = part;
    probe.key[0] = k0, probe.key[1] = k1, probe.key[2] = k2, probe.key[3] = k3;
    if (output != 0) {
        // the fragment output part of the state only
        const s_gpu_graphics_state &s = *output;
        probe.output.sample_count = s.sample_count;
        probe.output.alpha_to_coverage = s.alpha_to_coverage;
        probe.output.color_attachment_count = s.color_attachment_count;
        memcpy(probe.output.color_formats, s.color_formats, sizeof(s.color_formats));
        memcpy(probe.output.blend, s.blend, sizeof(s.blend));
        probe.output.depth_format = s.depth_format;
        probe.output.stencil_format = s.stencil_format;
        memcpy(probe.output.blend_constants, s.blend_constants, sizeof(s.blend_constants));
    }
    uint32_t hash = gpu_hash_bytes(2166136261u, &probe.part, sizeof(probe.part) + sizeof(probe.key));
    hash = gpu_hash_bytes(hash, &probe.output, sizeof(probe.output));
    hash = hash != 0 ? hash : 1u;
    uint32_t head;
    if (map_find(&g_xe_library_index, hash, &head)) {
        for (uint32_t i = head; i != 0; i = g_xe_libraries.items[i - 1u]->next) {
            s_xe_library *l = g_xe_libraries.items[i - 1u];
            if (l->part == part && memcmp(l->key, probe.key, sizeof(probe.key)) == 0 &&
                memcmp(&l->output, &probe.output, sizeof(probe.output)) == 0)
                return l;
        }
    }
    s_xe_library *library = (s_xe_library *)malloc(sizeof(s_xe_library));
    s_xe_library **slot = library != 0 ? g_xe_libraries.push() : 0;
    if (slot == 0) {
        free(library);
        return 0;
    }
    *library = probe;
    library->hash = hash;
    library->next = 0;
    library->status = k_xe_library_new;
    *slot = library;
    if (uint32_t *head_slot = map_slot(&g_xe_library_index, hash, true)) {
        library->next = *head_slot;
        *head_slot = g_xe_libraries.count;
    }
    return library;
}

// Queues a new library for the workers (no pool: nothing; the draw that needs it compiles it); first: ahead of the
// queue (with what HALO3_PIPELINE_ASYNC draws wait for).
void xe_library_prewarm(s_xe_library *library, bool first = false)
{
    if (library == 0 || g_prewarm.monitor == 0 ||
        __atomic_load_n(&library->status, __ATOMIC_ACQUIRE) != k_xe_library_new)
        return;
    prewarm_lock();
    // An observed link can claim a new part before shader registration queues it.
    if (__atomic_load_n(&library->status, __ATOMIC_ACQUIRE) == k_xe_library_new) {
        if (s_xe_library **slot = first ? g_prewarm.urgent.push() : g_prewarm.queue.push()) {
            *slot = library;
            library->urgent = first ? 1u : 0u;
            __atomic_store_n(&library->status, (uint32_t)k_xe_library_queued, __ATOMIC_RELEASE);
            port_gpu_monitor_broadcast(g_prewarm.monitor);
        }
    }
    prewarm_unlock();
}

// A library's handle for a draw: compiled now on this thread unless a worker has it (then waited for).
gpu_pipeline_handle xe_library_get(s_xe_library *library, bool worker = false)
{
    if (library == 0)
        return GPU_NULL_HANDLE;
    if (__atomic_load_n(&library->status, __ATOMIC_ACQUIRE) == k_xe_library_ready)
        return library->handle;
    prewarm_lock();
    uint32_t status = __atomic_load_n(&library->status, __ATOMIC_ACQUIRE);
    if (status == k_xe_library_new || status == k_xe_library_queued) {
        __atomic_store_n(&library->status, (uint32_t)k_xe_library_building, __ATOMIC_RELEASE);
        prewarm_unlock();
        const long long started = xe_ticks();
        const gpu_pipeline_handle handle = xe_library_build(*library);
        xe_library_finish(library, handle, worker, worker ? xe_ticks_ms(started, xe_ticks()) : 0.0);
        if (!worker && g_frame_trace.on) {
            // HALO3_FRAME_TRACE: a part the draw thread compiled itself (no worker had it): which, and how long
            char text[160];
            snprintf(text, sizeof(text), "ftrace part: frame %u part %u key %u %u %u %u: %.2f ms on the draw thread\n",
                     g_presented, library->part, library->key[0], library->key[1], library->key[2], library->key[3],
                     xe_ticks_ms(started, xe_ticks()));
            port_write_error(text);
        }
        prewarm_lock();
    } else if (status == k_xe_library_building) {
        if (!worker)
            g_prewarm.waited++;
        while (__atomic_load_n(&library->status, __ATOMIC_ACQUIRE) == k_xe_library_building)
            port_gpu_monitor_wait(g_prewarm.monitor); // building means a worker: the monitor exists
    }
    gpu_pipeline_handle handle = library->handle;
    prewarm_unlock();
    return handle;
}

// HALO3_PIPELINE_ASYNC: whether a shader part is compiled (or failed); if not, it goes to the front of the workers'
// queue (a deferred draw).
bool xe_library_async_ready(s_xe_library *library)
{
    if (library == 0)
        return true;
    uint32_t status = __atomic_load_n(&library->status, __ATOMIC_ACQUIRE);
    if (status == k_xe_library_ready || status == k_xe_library_failed)
        return true;
    port_gpu_monitor_lock(g_prewarm.monitor);
    status = __atomic_load_n(&library->status, __ATOMIC_ACQUIRE);
    if ((status == k_xe_library_new || status == k_xe_library_queued) && !library->urgent) {
        if (s_xe_library **slot = g_prewarm.urgent.push()) {
            *slot = library;
            library->urgent = 1;
            __atomic_store_n(&library->status, (uint32_t)k_xe_library_queued, __ATOMIC_RELEASE);
            port_gpu_monitor_broadcast(g_prewarm.monitor);
        }
    }
    port_gpu_monitor_unlock(g_prewarm.monitor);
    return status == k_xe_library_ready || status == k_xe_library_failed;
}

// A shader the game created (port_vk_shader_register): what a draw or dispatch with it will compile, queued for the
// workers: a memexport shader's compute pipeline; with pipeline libraries, a pixel shader's fragment shader part
// and a vertex shader's pre-rasterization part (as drawn with triangles, or for a shader behind the tessellator, the
// water's, with the tessellation helpers; rectangle lists add a geometry shader, on first use).
void xe_prewarm_shader(uint32_t xenos)
{
    s_shader_pack_xenos meta;
    if (g_prewarm.monitor == 0 || !shader_pack_get_xenos(g_pack, xenos, &meta))
        return;
    if (meta.model == 5) // compute: few, slow to compile, and never deferred (they export memory): first
        xe_library_prewarm(xe_library(k_xe_part_compute, meta.module_index, 0, 0, 0), true);
    else if (meta.model == 4 && g_xe_libraries_on) // fragment
        xe_library_prewarm(xe_library(k_xe_part_fragment_shader, meta.module_index, 0, 0, 0));
    else if (meta.model == 0 && g_xe_libraries_on) // vertex
        xe_library_prewarm(xe_library(k_xe_part_pre_rasterization, k_xe_pipeline_vertex, meta.module_index, 0,
                                      (uint32_t)k_gpu_polygon_fill));
    else if (meta.model == k_shader_pack_xenos_tess_eval && g_xe_libraries_on) // drawn only tessellated (the water)
        xe_library_prewarm(xe_library(k_xe_part_pre_rasterization, k_xe_pipeline_tessellated, meta.module_index, 0,
                                      (uint32_t)k_gpu_polygon_fill));
}

void xe_pipeline_parts(uint32_t kind, uint32_t vs, uint32_t ps, uint32_t extra, const s_gpu_graphics_state &state,
                       s_xe_library *parts[4])
{
    const uint32_t patch = kind == k_xe_pipeline_tessellated ? 3u : 0u;
    parts[0] = xe_library(k_xe_part_vertex_input, patch != 0 ? (uint32_t)k_gpu_topology_patch_list : state.topology,
                          state.primitive_restart, patch, 0);
    parts[1] = xe_library(k_xe_part_pre_rasterization, kind, vs, xe_kind_has_geometry(kind) ? extra : 0u,
                          state.polygon_mode);
    parts[2] = xe_library(k_xe_part_fragment_shader, ps,
                          ps != SHADER_PACK_NOT_FOUND && state.fragcoord_offset[0] != 0 ? 1u : 0u, 0, 0);
    parts[3] = xe_library(k_xe_part_fragment_output, 0, 0, 0, 0, &state);
}

// Called with the monitor held. Replay links only need optimization once a draw uses them.
void xe_optimize_link(s_gpu_pipeline *pipeline, s_xe_library *const parts[4])
{
    if (!g_xe_fast_link || !g_xe_optimize)
        return;
    if (s_xe_optimize *job = g_prewarm.optimize.push()) {
        for (uint32_t i = 0; i < 4; i++)
            job->libraries[i] = parts[i]->handle;
        job->pipeline = pipeline;
        port_gpu_monitor_broadcast(g_prewarm.monitor);
    }
}

s_gpu_pipeline *xe_link_parts(s_xe_library *const parts[4], const s_gpu_graphics_state &state, bool worker)
{
    gpu_pipeline_handle libraries[4];
    for (uint32_t i = 0; i < 4; i++) {
        libraries[i] = xe_library_get(parts[i], worker);
        if (libraries[i] == GPU_NULL_HANDLE)
            return 0;
    }
    s_gpu_pipeline *pipeline;
    {
        s_xe_layout_lock lock(true);
        pipeline = gpu_pipeline_link(g_gpu, libraries, g_xe_layout, &state, g_xe_fast_link ? 0u : 1u);
    }
    if (pipeline != 0 && !worker && g_prewarm.monitor != 0) {
        prewarm_lock();
        xe_optimize_link(pipeline, parts);
        prewarm_unlock();
    }
    return pipeline;
}

// A Xenos pipeline linked from its parts (fast; optimized on a worker when the driver links fast).
s_gpu_pipeline *xe_pipeline_link(uint32_t kind, const s_shader_pack_xenos *vs, const s_shader_pack_xenos *ps,
                                 uint32_t extra, const s_gpu_graphics_state &state, bool *out_pending)
{
    s_xe_library *parts[4];
    xe_pipeline_parts(kind, vs->module_index, ps != 0 ? ps->module_index : SHADER_PACK_NOT_FOUND, extra, state, parts);
    // HALO3_PIPELINE_ASYNC: shader parts not compiled yet go to the front of the workers' queue and the draw is
    // skipped (nothing is kept: the next draw with this state asks again). Never a draw that exports memory: what it
    // writes would be lost, not only late.
    *out_pending = false;
    bool exports = ((vs->flags | (ps != 0 ? ps->flags : 0u)) & k_shader_pack_xenos_memexport) != 0;
#ifdef HALO3_64BIT
    exports |= g_export.base != 0;
#endif
    if (g_xe_async && g_prewarm.monitor != 0 && !exports) {
        const bool vertex_ready = xe_library_async_ready(parts[1]);
        const bool fragment_ready = xe_library_async_ready(parts[2]); // both asked for at once
        if (!vertex_ready || !fragment_ready) {
            g_prewarm.deferred++; // the draw thread's (read by pipeline_prewarm_status on it)
            *out_pending = true;
            return 0;
        }
    }
    return xe_link_parts(parts, state, false);
}

// Shared by replay workers and the monolithic draw fallback.
s_gpu_pipeline *xe_pipeline_monolithic(uint32_t kind, uint32_t vs, uint32_t ps, uint32_t extra,
                                      const s_gpu_graphics_state &state)
{
    s_gpu_shader_stage stages[5];
    uint32_t n = 0;
    if (kind == k_xe_pipeline_tessellated) {
        const uint32_t tess_vs = shader_pack_xenos_helper(g_pack, 0), tess_tcs = shader_pack_xenos_helper(g_pack, 1);
        if (tess_vs == SHADER_PACK_NOT_FOUND || tess_tcs == SHADER_PACK_NOT_FOUND)
            return 0;
        stages[n++] = { k_gpu_stage_vertex, xe_module(tess_vs), 0 };
        stages[n++] = { k_gpu_stage_tess_control, xe_module(tess_tcs), 0 };
        stages[n++] = { k_gpu_stage_tess_eval, xe_module(vs), 0 };
    } else {
        stages[n++] = { k_gpu_stage_vertex, xe_module(vs), 0 };
        if (xe_kind_has_geometry(kind))
            stages[n++] = { k_gpu_stage_geometry, xe_module(extra), 0 };
    }
    if (ps != SHADER_PACK_NOT_FOUND)
        stages[n++] = { k_gpu_stage_fragment, xe_module(ps), 0 };
    for (uint32_t i = 0; i < n; i++)
        if (stages[i].module == GPU_NULL_HANDLE)
            return 0;
    s_xe_layout_lock lock(true);
    return gpu_graphics_pipeline_create_stages(g_gpu, stages, n, g_xe_layout, &state, 3);
}

// All supported hosts are little-endian x86. Keep the disk layout explicit and reject other byte orders at build time.
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "pipeline key byte order");

// Key indices refer to the pack's shader table, not transient shader handles or the driver's cache UUID.
uint32_t xe_key_pack_signature()
{
    const uint32_t count = shader_pack_xenos_count(g_pack);
    const uint32_t header[] = { shader_pack_version(g_pack), shader_pack_xenos_abi_version(g_pack),
                                shader_pack_module_count(g_pack), count, shader_pack_xenos_helper(g_pack, 0),
                                shader_pack_xenos_helper(g_pack, 1) };
    uint32_t hash = gpu_hash_bytes(2166136261u, header, sizeof(header));
    if (g_xe_abi6) {
        const uint32_t crc = shader_pack_table_crc(g_pack);
        hash = gpu_hash_bytes(hash, &crc, sizeof(crc));
    }
    for (uint32_t i = 0; i < count; i++) {
        s_shader_pack_xenos shader;
        if (shader_pack_get_xenos(g_pack, i, &shader)) {
            const uint32_t mapping[] = { shader.module_index, shader.stage, shader.model, shader.flags,
                                         shader.vs_outputs, shader.ps_inputs, shader.microcode_size };
            hash = gpu_hash_bytes(hash, mapping, sizeof(mapping));
            hash = gpu_hash_bytes(hash, shader.name, strlen(shader.name) + 1u);
        }
    }
    return hash;
}

s_xe_replay *xe_replay_find(const s_xe_pipeline_key &key, uint32_t hash)
{
    uint32_t head;
    if (!map_find(&g_xe_replay_index, hash, &head))
        return 0;
    for (uint32_t i = head; i != 0; i = g_xe_replay.items[i - 1u].next) {
        s_xe_replay &job = g_xe_replay.items[i - 1u];
        if (memcmp(&job.key, &key, sizeof(key)) == 0)
            return &job;
    }
    return 0;
}

bool xe_replay_shaders(const s_xe_pipeline_key &key, s_shader_pack_xenos *vs, s_shader_pack_xenos *ps)
{
    const s_gpu_caps &caps = g_gpu->caps;
    const s_gpu_graphics_state &state = key.state;
    if (key.kind > k_xe_pipeline_points || state.topology > k_gpu_topology_patch_list ||
        state.polygon_mode > k_gpu_polygon_point || state.color_attachment_count > GPU_MAX_COLOR_ATTACHMENTS ||
        state.cull_mode > (k_gpu_cull_front | k_gpu_cull_back) ||
        state.front_face > k_gpu_front_face_counter_clockwise || state.depth_compare > k_gpu_compare_always ||
        state.primitive_restart > 1 || state.depth_test > 1 || state.depth_write > 1 ||
        state.depth_bias_enable > 1 || state.stencil_test > 1 || state.alpha_to_coverage > 1 ||
        (state.sample_count != 1 && state.sample_count != 2 && state.sample_count != 4))
        return false;
    const s_gpu_stencil_face *faces[] = { &state.stencil_front, &state.stencil_back };
    for (const s_gpu_stencil_face *face : faces)
        if (face->fail_op > k_gpu_stencil_decrement_wrap || face->pass_op > k_gpu_stencil_decrement_wrap ||
            face->depth_fail_op > k_gpu_stencil_decrement_wrap || face->compare_op > k_gpu_compare_always)
            return false;
    for (uint32_t i = 0; i < state.color_attachment_count; i++) {
        const s_gpu_blend_attachment &blend = state.blend[i];
        if (state.color_formats[i] >= k_gpu_format_count || blend.enable > 1 || blend.write_mask > 0xfu ||
            blend.color_op > k_gpu_blend_op_max ||
            blend.alpha_op > k_gpu_blend_op_max || blend.src_color > k_gpu_blend_src_alpha_saturate ||
            blend.dst_color > k_gpu_blend_src_alpha_saturate || blend.src_alpha > k_gpu_blend_src_alpha_saturate ||
            blend.dst_alpha > k_gpu_blend_src_alpha_saturate)
            return false;
    }
    if (!shader_pack_get_xenos(g_pack, key.vertex, vs))
        return false;
    if (key.kind == k_xe_pipeline_tessellated) {
        if (!caps.tessellation || !caps.full_draw_index || vs->model != k_shader_pack_xenos_tess_eval)
            return false;
    } else if (vs->model != k_shader_pack_xenos_vertex) {
        return false;
    }
    if (xe_kind_has_geometry(key.kind)) {
        const uint32_t module = key.kind == k_xe_pipeline_rectangles ? shader_pack_xenos_rect_gs(g_pack, vs->vs_outputs)
                                                                     : shader_pack_xenos_point_gs(g_pack);
        if (!caps.geometry || module == SHADER_PACK_NOT_FOUND || module != key.extra)
            return false;
    }
    return key.pixel == SHADER_PACK_NOT_FOUND ||
           (shader_pack_get_xenos(g_pack, key.pixel, ps) && ps->model == k_shader_pack_xenos_fragment);
}

// Translate portable attachment selectors to the formats the current renderer uses.
bool xe_key_resolve_formats(s_gpu_graphics_state &state)
{
    uint32_t *depth_formats[] = { &state.depth_format, &state.stencil_format };
    for (uint32_t *format : depth_formats) {
        if (*format == k_gpu_format_d24_unorm_s8_uint || *format == k_gpu_format_d32_sfloat_s8_uint)
            *format = g_gpu->caps.depth_stencil_format;
        else if (*format != k_gpu_format_undefined)
            return false;
    }
    for (uint32_t &format : state.color_formats) {
        if (format == k_xe_key_fixed16_rg || format == k_xe_key_fixed16_rgba)
            format = edram_fixed16_host_format(format == k_xe_key_fixed16_rgba);
    }
    return true;
}

bool xe_key_cache_add(const s_xe_key_file_header &file, uint32_t size, bool seed)
{
    const auto *keys = (const s_xe_pipeline_key *)(&file + 1);
    if (size < sizeof(file) || file.magic != k_xe_key_magic ||
        (file.version != 1 && file.version != k_xe_key_version) || file.key_size != sizeof(*keys) ||
        file.pack != g_xe_key_pack || file.count > k_xe_key_capacity ||
        size != sizeof(file) + file.count * sizeof(*keys) ||
        file.hash != pipeline_cache_hash((const uint8_t *)keys, size - sizeof(file)))
        return false;
    for (uint32_t i = 0; i < file.count; i++) {
        s_xe_pipeline_key key = keys[i];
        const bool monolithic = (key.kind & k_xe_key_monolithic) != 0;
        key.kind &= ~k_xe_key_monolithic;
        if (key.kind > k_xe_pipeline_points || (file.version == 1 && monolithic))
            continue;
        if (!seed) {
            s_xe_pipeline_key *remembered = g_xe_keys.push();
            if (remembered == 0)
                break;
            *remembered = keys[i];
        }
        if ((!g_xe_libraries_on && !monolithic) || !xe_key_resolve_formats(key.state))
            continue;
        if (g_xe_libraries_on && monolithic)
            gpu_graphics_state_static_key(g_gpu, &key.state, &key.state);
        s_shader_pack_xenos vs, ps;
        if (!xe_replay_shaders(key, &vs, &ps))
            continue;
        const uint32_t hash = xe_hash_words(&key, sizeof(key));
        const uint32_t map_key = hash != 0 ? hash : 1u;
        if (xe_replay_find(key, map_key) != 0)
            continue;
        uint32_t *head = map_slot(&g_xe_replay_index, map_key, true);
        s_xe_replay *job = head != 0 ? g_xe_replay.push() : 0;
        if (job == 0)
            break;
        *job = {};
        job->key = key;
        job->next = *head;
        *head = g_xe_replay.count;
        job->vertex_module = vs.module_index;
        job->pixel_module = key.pixel != SHADER_PACK_NOT_FOUND ? ps.module_index : SHADER_PACK_NOT_FOUND;
        if (g_xe_libraries_on)
            xe_pipeline_parts(key.kind, job->vertex_module, job->pixel_module, key.extra, key.state, job->parts);
    }
    return true;
}

void xe_key_cache_load()
{
    g_xe_key_pack = xe_key_pack_signature();
    if (g_xe_abi6)
        snprintf(g_xe_key_file, sizeof(g_xe_key_file), "pipeline_keys.abi6.%08x.bin", shader_pack_table_crc(g_pack));
    else
        memcpy(g_xe_key_file, k_xe_legacy_key_file, sizeof(k_xe_legacy_key_file));
    const size_t key_name_bytes = strlen(g_xe_key_file) + 1u;
    const uint32_t capacity = sizeof(s_xe_key_file_header) + k_xe_key_capacity * sizeof(s_xe_pipeline_key);
    auto *file = (s_xe_key_file_header *)malloc(capacity);
    if (file == 0)
        return;
    const char *pack = getenv("HALO3_SHADER_PACK");
    char beside_pack[PORT_STORAGE_PATH], beside_exe[PORT_STORAGE_PATH], exe[PORT_STORAGE_PATH];
    beside_pack[0] = beside_exe[0] = '\0';
    if (pack != 0 && *pack != '\0') {
        const char *name = pack;
        for (const char *c = pack; *c != '\0'; c++)
            if (*c == '/' || *c == '\\')
                name = c + 1;
        const size_t prefix = (size_t)(name - pack);
        if (prefix + key_name_bytes <= sizeof(beside_pack)) {
            memcpy(beside_pack, pack, prefix);
            memcpy(beside_pack + prefix, g_xe_key_file, key_name_bytes);
        }
    }
    if (port_host_executable_directory(exe, sizeof(exe)) &&
        strlen(exe) + 1 + key_name_bytes <= sizeof(beside_exe))
        snprintf(beside_exe, sizeof(beside_exe), "%s/%s", exe, g_xe_key_file);
    const char *paths[] = { beside_pack, beside_exe, g_xe_abi6 ? "" : "play/pipeline_keys.bin",
                            g_xe_abi6 ? "" : "shaders/pipeline_keys.bin", g_xe_abi6 ? "" : "port/data/pipeline_keys.bin" };
    for (const char *path : paths) {
        uint32_t size = 0;
        if (*path != '\0' && port_host_read_file(path, file, capacity, &size) == 0 &&
            xe_key_cache_add(*file, size, true) && g_xe_replay.count != 0) {
            char message[96];
            snprintf(message, sizeof(message), "queued %u shipped pipeline keys (%s)\n", g_xe_replay.count,
                     g_xe_libraries_on ? "GPL" : "monolithic");
            device_log_text(message);
            break;
        }
    }
    g_prewarm.seed_count = g_prewarm.seed_pending = g_xe_replay.count;
    if (g_prewarm.seed_count != 0) {
        // The first scene also dispatches memexports. These few compute pipelines never defer and can be
        // warming before the map registers its shaders, alongside the graphics replay.
        for (uint32_t i = 0; i < shader_pack_xenos_count(g_pack); i++) {
            s_shader_pack_xenos shader;
            if (shader_pack_get_xenos(g_pack, i, &shader) && shader.model == k_shader_pack_xenos_compute)
                xe_prewarm_shader(i);
        }
    }
    uint32_t size = 0;
    if (port_storage_data_read(g_xe_key_file, file, capacity, &size) == 0)
        xe_key_cache_add(*file, size, false);
    g_prewarm.key_saved = g_xe_keys.count;
    free(file);
}

void xe_key_cache_write(s_xe_key_file_header *file)
{
    const uint32_t bytes = file->count * sizeof(s_xe_pipeline_key);
    file->hash = pipeline_cache_hash((const uint8_t *)(file + 1), bytes);
    const bool saved = port_storage_data_write(g_xe_key_file, file, sizeof(*file) + bytes) == 0;
    if (!saved)
        device_log_text("cannot write the observed pipeline keys\n");
    prewarm_lock();
    if (saved)
        g_prewarm.key_saved = file->count;
    g_prewarm.key_saving = false;
    port_gpu_monitor_broadcast(g_prewarm.monitor);
    prewarm_unlock();
    free(file);
}

void xe_key_cache_save(bool wait)
{
    s_prewarm &p = g_prewarm;
    if (g_xe_link_layout_monitor == 0 || p.monitor == 0)
        return;
    prewarm_lock();
    if (wait) {
        while (p.key_saving)
            port_gpu_monitor_wait(p.monitor);
        free(p.key_save);
        p.key_save = 0;
    }
    if (p.key_saving || p.key_save != 0 || p.key_saved == g_xe_keys.count) {
        prewarm_unlock();
        return;
    }
    const uint32_t bytes = g_xe_keys.count * sizeof(s_xe_pipeline_key);
    s_xe_key_file_header *file = (s_xe_key_file_header *)malloc(sizeof(*file) + bytes);
    if (file == 0) {
        prewarm_unlock();
        return;
    }
    *file = { k_xe_key_magic, k_xe_key_version, sizeof(s_xe_pipeline_key), g_xe_key_pack, g_xe_keys.count, 0 };
    memcpy(file + 1, g_xe_keys.items, bytes);
    if (!wait) {
        p.key_save = file;
        port_gpu_monitor_broadcast(p.monitor);
    }
    prewarm_unlock();
    if (wait)
        xe_key_cache_write(file);
}

s_gpu_pipeline *xe_replay_build(const s_xe_replay &job)
{
    return g_xe_libraries_on ? xe_link_parts(job.parts, job.key.state, true)
                             : xe_pipeline_monolithic(job.key.kind, job.vertex_module, job.pixel_module,
                                                      job.key.extra, job.key.state);
}

void xe_replay_pump()
{
    if (!g_record_worker && g_window && !g_window_closed && !port_window_pump()) {
        g_window_closed = 1;
        port_window_close();
        port_exit_flush();
        port_exit(0);
    }
    Sleep(1);
}

// Keep the last intro/loading frame visible while the startup queue gets a short head start at the first 3D scene.
// Exact keys still get demand priority after this budget, so a slow driver cannot hold up startup for other maps.
void xe_replay_prepare_scene(s_xe_replay *job)
{
    s_prewarm &p = g_prewarm;
    if (p.first_seeded_scene || g_xe_async || job->key.state.depth_format == k_gpu_format_undefined ||
        (uint32_t)(job - g_xe_replay.items) >= p.seed_count)
        return;
    p.first_seeded_scene = true;
    const long long started = xe_ticks();
    prewarm_lock();
    const uint32_t pending = p.seed_pending;
    while (p.seed_pending != 0 && xe_ticks_ms(started, xe_ticks()) < 2000.0) {
        prewarm_unlock();
        xe_replay_pump();
        prewarm_lock();
    }
    const uint32_t remaining = p.seed_pending;
    prewarm_unlock();
    if (pending != 0) {
        char text[144];
        snprintf(text, sizeof(text), "pipeline seed at first 3D scene: %.1f ms, %u of %u pending keys finished\n",
                 xe_ticks_ms(started, xe_ticks()), pending - remaining, pending);
        device_log_text(text);
    }
}

s_gpu_pipeline *xe_replay_take(s_xe_replay *job, bool allow_defer, bool *pending, long long stats_started)
{
    prewarm_lock();
    if (job->status == k_xe_replay_queued) {
        g_prewarm.replay_urgent = job;
        port_gpu_monitor_broadcast(g_prewarm.monitor);
    }
    if (allow_defer && job->status != k_xe_replay_done) {
        g_prewarm.deferred++;
        *pending = true;
        prewarm_unlock();
        return 0;
    }
    const bool waiting = job->status != k_xe_replay_done;
    if (waiting)
        g_prewarm.waited++;
    const long long started = waiting && g_frame_trace.on ? frame_stats_now() : 0;
    while (job->status != k_xe_replay_done) {
        if (g_record_worker || !g_window || g_window_closed) {
            port_gpu_monitor_wait(g_prewarm.monitor);
            continue;
        }
        // On Linux the drawing thread also owns the event pump.
        prewarm_unlock();
        xe_replay_pump();
        prewarm_lock();
    }
    s_gpu_pipeline *pipeline = job->pipeline;
    if (pipeline != 0) {
        job->pipeline = 0; // the runtime pipeline table now owns it
        g_prewarm.replay_used++;
        if (g_xe_libraries_on)
            xe_optimize_link(pipeline, job->parts);
    }
    prewarm_unlock();
    if (waiting && pipeline != 0)
        pipeline_stats_end(stats_started);
    if (started != 0) {
        char text[112];
        snprintf(text, sizeof(text), "ftrace replay: frame %u waited %.2f ms for an observed link\n",
                 g_presented, frame_stats_ms(started, frame_stats_now()));
        port_write_error(text);
    }
    return pipeline;
}

void xe_key_observe(s_xe_pipeline_key key)
{
    if (g_xe_keys.count >= k_xe_key_capacity)
        return;
    if (!g_xe_libraries_on)
        key.kind |= k_xe_key_monolithic;
    if (key.state.depth_format != k_gpu_format_undefined)
        key.state.depth_format = k_gpu_format_d24_unorm_s8_uint;
    if (key.state.stencil_format != k_gpu_format_undefined)
        key.state.stencil_format = k_gpu_format_d24_unorm_s8_uint;
    for (uint32_t i = 0; i < key.state.color_attachment_count; i++) {
        if (key.state.color_formats[i] == k_gpu_format_undefined || g_bound_color[i] == 0)
            continue;
        const uint32_t layout = texel_layout(g_bound_color[i]->d3d_format);
        if (layout == k_xenos_format_16_16_edram)
            key.state.color_formats[i] = k_xe_key_fixed16_rg;
        else if (layout == k_xenos_format_16_16_16_16_edram)
            key.state.color_formats[i] = k_xe_key_fixed16_rgba;
    }
    if (s_xe_pipeline_key *observed = g_xe_keys.push())
        *observed = key;
}

const char *pipeline_prewarm_status()
{
    static char text[384];
    if (!g_xe_libraries_on && g_prewarm.monitor == 0)
        return "";
    prewarm_lock();
    const s_prewarm &p = g_prewarm;
    snprintf(text, sizeof(text),
             "; pipeline parts: %u compiled on %u workers (%.0f ms of work), %u on the draw thread, %u waited for, %u "
             "queued, %u failed; %u links optimized (%u pending); %u draws deferred (HALO3_PIPELINE_ASYNC)",
             p.built_by_workers, p.thread_count, p.worker_ms, p.built_by_draw, p.waited,
             p.queue.count - p.queue_head + p.urgent.count, p.failed, p.optimized_count, p.optimize.count - p.optimize_head,
             p.deferred);
    if (g_xe_link_layout_monitor != 0) {
        const size_t length = strlen(text);
        snprintf(text + length, sizeof(text) - length, "; observed links %u queued, %u built, %u used; %u keys saved",
                 g_xe_replay.count - p.replay_head, p.replay_built, p.replay_used, p.key_saved);
    }
    prewarm_unlock();
    return text;
}

// ---- pipelines ----

// xe_pipeline's answer (entry `index` of g_xe_pipelines), kept for the next draw that asks with the same key.
s_gpu_pipeline *xe_pipeline_remember(uint32_t index)
{
    g_xe_last_entry = index + 1u;
    return g_xe_pipelines.items[index].pipeline;
}

s_gpu_pipeline *xe_pipeline(uint32_t kind, const s_shader_pack_xenos *vs, const s_shader_pack_xenos *ps,
                            uint32_t extra, const s_gpu_graphics_state &state)
{
    // every word is written below (no padding: all 32-bit fields), not zeroed first on every draw
    [[clang::uninitialized]] s_xe_pipeline_key key;
    key.vertex = vs->index;
    key.pixel = ps != 0 ? ps->index : SHADER_PACK_NOT_FOUND;
    key.kind = kind;
    key.extra = extra;
    // A pipeline linked from libraries has every depth / stencil / rasterization value but the polygon mode dynamic
    // (s_gpu_graphics_state's run from cull_mode up to sample_count) and is bound with the draw's own
    // (gpu_cmd_bind_pipeline_state): one link serves them all. Keyed with them, each new cull mode, depth bias or
    // stencil reference linked another pipeline, and consecutive draws that differed only there bound a new one.
    if (g_xe_libraries_on)
        gpu_graphics_state_static_key(g_gpu, &state, &key.state);
    else
        key.state = state;
    if (g_xe_last_entry) {
        const auto &last = g_xe_pipelines.items[g_xe_last_entry - 1u];
        if (last.key.vertex == key.vertex && last.key.pixel == key.pixel && last.key.kind == kind &&
            last.key.extra == extra && !memcmp(&last.key.state, &key.state, sizeof(key.state)))
            return last.pipeline;
    }
    uint32_t hash = xe_hash_words(&key, sizeof(key));
    hash = hash != 0 ? hash : 1u;
    uint32_t head;
    if (map_find(&g_xe_pipeline_index, hash, &head)) {
        for (uint32_t i = head; i != 0; i = g_xe_pipelines.items[i - 1u].next) {
            const s_xe_pipeline_entry &e = g_xe_pipelines.items[i - 1u];
            if (memcmp(&e.key, &key, sizeof(key)) == 0)
                return xe_pipeline_remember(i - 1u);
        }
    }
    s_xe_replay *replay = g_prewarm.monitor != 0 ? xe_replay_find(key, hash) : 0;
    if (replay != 0)
        xe_replay_prepare_scene(replay);
    const long long started = pipeline_stats_begin();
    bool pending = false;
    s_gpu_pipeline *pipeline = 0;
    if (replay != 0) {
        bool exports = ((vs->flags | (ps != 0 ? ps->flags : 0u)) & k_shader_pack_xenos_memexport) != 0;
#ifdef HALO3_64BIT
        exports |= g_export.base != 0;
#endif
        pipeline = xe_replay_take(replay, g_xe_libraries_on && g_xe_async && !exports, &pending, started);
        if (pending)
            return 0;
    }
    if (pipeline == 0) {
        pipeline = g_xe_libraries_on ? xe_pipeline_link(kind, vs, ps, extra, state, &pending)
                                    : xe_pipeline_monolithic(kind, vs->module_index,
                                          ps != 0 ? ps->module_index : SHADER_PACK_NOT_FOUND, extra, state);
        if (pending)
            return 0; // HALO3_PIPELINE_ASYNC: skipped until its shader parts are compiled
        pipeline_stats_end(started);
    }
    uint32_t made = ~0u; // its entry (none: out of memory)
    if (s_xe_pipeline_entry *entry = g_xe_pipelines.push()) {
        entry->key = key;
        entry->hash = hash;
        entry->pipeline = pipeline;
        made = g_xe_pipelines.count - 1u;
        if (uint32_t *slot = map_slot(&g_xe_pipeline_index, hash, true)) {
            entry->next = *slot;
            *slot = g_xe_pipelines.count;
        }
        if (pipeline != 0 && replay == 0 && g_xe_link_layout_monitor != 0 && g_prewarm.monitor != 0)
            xe_key_observe(key);
    }
    if (pipeline == 0) {
        char detail[200];
        snprintf(detail, sizeof(detail), "vs %s, ps %s", vs->name, ps != 0 ? ps->name : "none");
        report(hash ^ 0x78706970u, "Xenos pipeline creation failed (draws with it are skipped)", detail);
    }
    if (made != ~0u)
        return xe_pipeline_remember(made);
    g_xe_last_entry = 0;
    return pipeline;
}

// The full shader-facing bounds only limit *where* a kernel may access memory. Track its actual addressing
// separately. Combine intersecting field spans of interleaved records, keeping disjoint arrays and strips apart.
struct s_xe_memexport_sync {
    s_memexport_sync_range ranges[64]; // at most 8 strip rows x 4 writes, or 11 writes + 2 reads per vfetch
    uint32_t count;

    void add(uint64_t base, uint64_t end, bool write)
    {
        if (base >= end)
            return;
        for (uint32_t i = 0; i < count;) {
            const auto &r = ranges[i];
            if (r.write != write || end < r.base || r.end < base) {
                ++i;
                continue;
            }
            if (r.base < base) base = r.base;
            if (r.end > end) end = r.end;
            ranges[i] = ranges[--count];
        }
        ranges[count++] = { base, end, write };
    }
};

struct s_xe_record_span { uint32_t first, last; }; // inclusive vertex indices

bool xe_export_integer(uint32_t bits, uint32_t &out)
{
    const float f = __builtin_bit_cast(float, bits);
    if (!(f >= 0 && f <= 0x7fffff) || (float)(uint32_t)f != f)
        return false;
    out = (uint32_t)f;
    return true;
}

// Physical microcode identities. The effect shaders export eM0 with
// mad eA, trunc(vertex * c[N+1].x + c[N+1].y), (0,1,0,0), c[N].
// Spawns take the output index from a fetched queue. Beams and light volumes indirect through 8 strip rows.
struct s_xe_export_layout {
    uint32_t reg, fields, strip_reg, elements_per_record, written_elements;
};
constexpr uint64_t k_particle_update = 0x9b47717aa6a9df19ull, k_particle_spawn = 0xc9c5cf035d1f7f5bull;
constexpr uint64_t k_contrail_update = 0x3f0ab54fbec0fde7ull, k_contrail_spawn = 0x453ed24449943f70ull;
constexpr uint64_t k_ripple_update = 0x452e7dccb42eff4full, k_ripple_add = 0x50c621c196fee6e5ull;
constexpr uint64_t k_water_factors = 0x18c3b2dc019d7b74ull;

s_xe_export_layout xe_export_layout(uint64_t hash)
{
    switch (hash) {
    case k_particle_update: return {160, 11, 0, 0, 1};
    case k_particle_spawn:  return {152, 11, 0, 0, 1};
    case k_contrail_update:
    case k_contrail_spawn:  return {136, 8, 0, 0, 1};
    case 0x97f75382522bf9f8ull: return {124, 4, 167, 0, 1}; // beam_update
    case 0xb680d07a4be83b3full: return {124, 3, 156, 0, 1}; // light_volume_update
    case k_ripple_update:   return {130, 1, 0, 8, 6}; // eA offsets 0,1,2,3,5
    case k_ripple_add:      return {130, 1, 0, 8, 8};
    case k_water_factors:   return {130, 1, 0, 3, 3};
    default:                return {};
    }
}

uint32_t xe_export_records(const s_shader_pack_xenos &cs, uint64_t hash, const s_xe_export_layout &layout,
                           s_xe_record_span draw, s_xe_record_span (&records)[8])
{
    if (hash == k_particle_spawn || hash == k_contrail_spawn)
        return 0; // use the stream constant's count
    if (layout.strip_reg) {
        if (draw.last >= 8 * 16)
            return 0;
        uint32_t n = 0;
        for (uint32_t row = draw.first / 16; row <= draw.last / 16; ++row) {
            uint32_t c[4], physical;
            xe_float_register(g_shadows.vertex_float, cs, layout.strip_reg + row, c);
            if (!xe_export_integer(c[0], physical) || physical > 0x7fffff / 16)
                return 0;
            records[n++] = {physical * 16 + (row == draw.first / 16 ? draw.first % 16 : 0),
                            physical * 16 + (row == draw.last / 16 ? draw.last % 16 : 15)};
        }
        return n;
    }
    if (hash == k_ripple_add) {
        uint32_t c[4], offset, limit;
        xe_float_register(g_shadows.vertex_float, cs, 138, c);
        if (!xe_export_integer(c[0], offset) || offset + draw.last > 0x7fffff)
            return 0;
        xe_float_register(g_shadows.vertex_float, cs, 139, c);
        if (!xe_export_integer(c[0], limit) || !limit)
            return 0;
        draw.first += offset;
        draw.last += offset;
        if (draw.last >= limit) {
            if (draw.first < limit) {
                records[0] = {draw.first, limit - 1};
                records[1] = {0, draw.last - limit};
                return 2;
            }
            draw.first -= limit;
            draw.last -= limit;
        }
    }
    records[0] = draw;
    return 1;
}

void xe_memexport_sync_ranges(const s_shader_pack_xenos &cs, uint64_t hash, const xe_system &sys,
                              const xe_vertex_fetch &fetch, uint32_t read_slots, s_xe_memexport_sync &sync)
{
    const uint32_t start = sys.vertex[1], count = sys.vertex[2];
    if (!count)
        return;
    const auto layout = xe_export_layout(hash);
    s_xe_record_span draw = {start, start + count - 1};
    // r0.x is a float. Only use integer slice arithmetic where the vertex indices and vfetch rounding are exact.
    bool exact_indices = layout.fields && (uint64_t)start + count <= 0x800000;
    if (exact_indices && hash == k_water_factors) {
        uint32_t c[4], offset;
        xe_float_register(g_shadows.vertex_float, cs, 131, c);
        exact_indices = xe_export_integer(c[0], offset) && draw.last + offset <= 0x7fffff;
        if (exact_indices) {
            draw.first += offset;
            draw.last += offset;
        }
    }
    s_xe_record_span records[8];
    const uint32_t n = exact_indices ? xe_export_records(cs, hash, layout, draw, records) : 0;
    if (!layout.fields) {
        // The pack records constant operands, but an unknown kernel can modify eA.x/z/w before a store.
        for (const auto &m : sys.memexport) {
            const uint64_t base = (uint64_t)m[3] << 32 | m[2];
            sync.add(base, base + (uint64_t)m[1] * 4, true);
        }
    }
    for (uint32_t f = 0; f < layout.fields; ++f) {
        const uint32_t reg = layout.reg + 2 * f;
        uint32_t ea[4];
        xe_float_register(g_shadows.vertex_float, cs, reg, ea);
        if (ea[0] >> 30 != 1 || ea[2] >> 20 != 0x4b0 || ea[3] >> 23 != 0x96)
            continue;
        const uint32_t format = (ea[2] >> 8) & 63;
        uint32_t bytes;
        switch (format) { // io.odin memexport_store_fn
        case 38: bytes = 16; break;
        case 37: case 32: case 26: bytes = 8; break;
        case 36: case 31: case 25: case 7: case 6: bytes = 4; break;
        default: continue;
        }
        for (const auto &m : sys.memexport) {
            const uint32_t skip = (ea[0] & 0x3fffffff) - m[0];
            if (skip >= m[1])
                continue;
            const uint64_t base = ((uint64_t)m[3] << 32 | m[2]) + (uint64_t)skip * 4;
            uint32_t limit = (m[1] - skip) / (bytes / 4);
            if (limit > (ea[3] & 0x7fffff)) limit = ea[3] & 0x7fffff;
            uint32_t stride = layout.elements_per_record, offset = 0;
            bool bounded = n && ea[1] >> 23 == 0x96;
            if (bounded && !stride) {
                uint32_t c[4];
                xe_float_register(g_shadows.vertex_float, cs, reg + 1, c);
                bounded = xe_export_integer(c[0], stride) && xe_export_integer(c[1], offset);
            }
            offset += ea[1] & 0x7fffff;
            for (uint32_t i = 0; bounded && i < n; ++i) {
                const uint64_t first = (uint64_t)records[i].first * stride + offset;
                uint64_t end = (uint64_t)records[i].last * stride + offset + layout.written_elements;
                bounded = end <= 0x800000; // eA.y's mantissa encodes the element index
                if (end > limit) end = limit;
                if (bounded && first < end)
                    sync.add(base + first * bytes, base + end * bytes, true);
            }
            if (!bounded)
                sync.add(base, base + (uint64_t)limit * bytes, true);
            break; // memexport_store_fn selects the first table entry containing eA.x
        }
    }

    // Only export-backed fetches can alias a pending write. Export-backed textures are copied at a transfer
    // boundary. emit_vfetch loads FOUR dwords per element and redirects an out-of-bounds dword load to word 0.
    while (read_slots) {
        const uint32_t slot = (uint32_t)__builtin_ctz(read_slots);
        read_slots &= read_slots - 1;
        const auto &s = fetch.stream[slot];
        const uint64_t base = (uint64_t)s.address_hi << 32 | s.address_lo;
        const uint32_t stride = s.stride_endian & 0xffff;
        bool direct = exact_indices && cs.element_count <= 31;
        uint32_t lo = UINT32_MAX, hi = 0;
        for (uint32_t k = 0; direct && k < cs.element_count; ++k) {
            const auto &el = fetch.element[k];
            if (el.stream != slot) continue;
            direct = hash != k_water_factors || k < 3; // position vfetches use indices fetched from the mesh
            if (el.offset_bytes < lo) lo = el.offset_bytes;
            if (el.offset_bytes > hi) hi = el.offset_bytes;
        }
        const uint64_t first = (uint64_t)draw.first * stride + lo;
        uint64_t end = (uint64_t)draw.last * stride + hi + 16;
        if (!direct || lo == UINT32_MAX || end > UINT32_MAX) {
            sync.add(base, base + (s.size_bytes < 4 ? 4 : s.size_bytes), false);
        } else {
            if (end > s.size_bytes) {
                sync.add(base, base + 4, false);
                end = s.size_bytes;
            }
            sync.add(base + (first & ~uint64_t(3)), base + (end & ~uint64_t(3)), false);
        }
    }
}

// A memexport vertex shader's compute pipeline (one per module: the same module makes the same pipeline). With the
// pipeline prewarm, a worker compiled it when the shader was created (or is compiling it: waited for). Never deferred
// (HALO3_PIPELINE_ASYNC): a skipped dispatch would lose the memory it exports.
s_gpu_pipeline *xe_compute_pipeline(const s_shader_pack_xenos &cs, s_shader_entry *shader)
{
    if (shader->xenos_compute != 0)
        return shader->xenos_compute;
    for (uint32_t i = 0; i < g_xe_compute.count; i++)
        if (g_xe_compute.items[i].shader_index == cs.module_index)
            return shader->xenos_compute = g_xe_compute.items[i].pipeline;
    s_xe_library *library = xe_library(k_xe_part_compute, cs.module_index, 0, 0, 0);
    const long long started = pipeline_stats_begin();
    // the library's handle becomes the pipeline's (it owns it from now on)
    gpu_pipeline_handle handle = xe_library_get(library);
    s_gpu_pipeline *pipeline = gpu_pipeline_adopt(g_gpu, handle, g_xe_layout, k_gpu_bind_compute);
    if (library != 0)
        library->handle = GPU_NULL_HANDLE;
    if (pipeline == 0 && handle != GPU_NULL_HANDLE)
        gpu_pipeline_library_destroy(g_gpu, handle);
    pipeline_stats_end(started);
    if (s_compute_entry *entry = g_xe_compute.push()) {
        entry->shader_index = cs.module_index;
        entry->pipeline = pipeline;
    } else if (pipeline != 0) {
        gpu_pipeline_destroy(g_gpu, pipeline);
        pipeline = 0;
    }
    if (pipeline == 0)
        report(cs.index ^ 0x78637069u, "Xenos compute pipeline creation failed (its dispatches are skipped)", cs.name);
    return shader->xenos_compute = pipeline;
}

// ---- draws ----

void xe_report_unknown(s_shader_entry *bad, const char *what)
{
    if (bad == 0 || bad->reported)
        return;
    bad->reported = 1;
    char text[160];
    snprintf(text, sizeof(text), "a %s shader whose microcode (hash %08x%08x) the Xenos pack does not know (its draws are "
             "skipped)\n", what, bad->hash_hi, bad->hash_lo);
    device_log_text(text);
}

// A memexport vertex shader (translated to a compute shader): guest vertex start + i is invocation i.
void xenos_memexport_draw(s_draw_geometry draw, uint64_t dirty_mask, const s_shader_pack_xenos &cs, uint32_t start, uint32_t count,
                          uint32_t indexed, s_shader_entry *cs_entry)
{
    if (indexed) {
        report(cs.index ^ 0x6d786978u, "an indexed memexport draw (skipped)", cs.name);
        return;
    }
    if (!begin_frame())
        return;
    const s_shader_pack_xenos *metas[1] = { &cs };
    s_xe_draw_textures textures = {};
    uint32_t linear_gamma_mask = 0;
    if (g_xe_abi6 && !xe_texture_set(draw.packet, metas, 1, &textures, &linear_gamma_mask))
        return;
    s_gpu_pipeline *pipeline = xe_compute_pipeline(cs, cs_entry);
    if (pipeline == 0)
        return;
    [[clang::uninitialized]] xe_system sys; // written whole by xe_system_fill
    xe_system_fill(draw.packet, &sys);
    sys.vertex[1] = start;
    sys.vertex[2] = count;
    s_vertex_range range = {};
    range.known = true;
    range.first = start;
    range.end = start + count;
    range.whole = true; // an update reads its records by index, the spawn its queue: whole streams
    const s_xe_rows_memo *live = xe_pipeline_rows(k_xe_pipeline_vertex, &cs, 0, 0, cs_entry, 0);
    if (live == 0)
        return;
    static xe_vertex_fetch vertex_fetch;
    const xe_vertex_fetch *vf_ptr = &vertex_fetch;
    uint32_t vf_gen = 0;
    uint32_t export_slots = 0;
    s_gpu_buffer *stream_views[XE_D3D12_VERTEX_STREAMS];
    if (!xe_vertex_fetch_fill(draw, dirty_mask, cs, &range, live->rows[k_xe_block_vertex_fetch], &vertex_fetch, g_vk ? 0 : stream_views,
                              &vf_ptr, &vf_gen, g_vk ? &export_slots : 0))
        return;
    if (!g_xe_abi6 && !xe_texture_set(draw.packet, metas, 1, &textures, &linear_gamma_mask))
        return;
    end_pass();
    if (g_vk) {
        vulkan_cmd_bind_pipeline_native(g_vk, vulkan_gpu_native_pipeline(pipeline));
    } else {
        gpu_cmd_memory_barrier(g_gpu);
        gpu_cmd_bind_pipeline(g_gpu, pipeline);
    }
    if (!xe_bind(pipeline, draw.packet, &sys, &cs, 0, vf_ptr, g_vk ? 0 : stream_views, textures, *live, vf_gen, 0,
                 linear_gamma_mask, g_vk ? sys.memexport : 0))
        return;
    if (g_vk) {
        const uint32_t read_slots = export_slots & live->rows[k_xe_block_vertex_fetch].bits[0];
        [[clang::uninitialized]] s_xe_memexport_sync sync;
        sync.count = 0;
        xe_memexport_sync_ranges(cs, (uint64_t)cs_entry->hash_hi << 32 | cs_entry->hash_lo, sys, *vf_ptr, read_slots, sync);
        vulkan_cmd_dispatch_memexport(g_vk, (count + 63u) / 64u, sync.ranges, sync.count);
    } else {
        gpu_cmd_dispatch(g_gpu, (count + 63u) / 64u, 1, 1);
        gpu_cmd_memory_barrier(g_gpu);
    }
}

// The fixed-function state of a draw for the translated shaders: as port_vk_draw's, the exponent biases and tile
// origin moved to xe_system (pipelines do not depend on them), color writes off for targets the pixel shader does not
// export (they keep their contents, as on the Xenos); `slots`: the color slots attached (attached_color_slots).
void xe_state(const s_port_vk_draw *draw, uint32_t topology, bool restart, const s_shader_pack_xenos *ps,
              uint32_t slots, s_gpu_graphics_state *state)
{
    build_state(draw->state, topology, restart, slots, state);
    memset(state->color_exp_bias, 0, sizeof(state->color_exp_bias));
    // Xenos translated shaders take tile offset from xe_system; reuse fragcoord_offset[0] for
    // alpha_test_enable (SpecId 0) so sizeof(s_xe_pipeline_key) stays 324 bytes.
    const bool alpha_test = ps != 0 && draw->state.alpha_test_enable != 0 && (draw->state.alpha_func & 7u) != 7u;
    state->fragcoord_offset[0] = alpha_test ? 1 : 0;
    state->fragcoord_offset[1] = 0;
    for (uint32_t i = 0; i < GPU_MAX_COLOR_ATTACHMENTS; i++)
        if (ps == 0 || (ps->color_outputs & (1u << i)) == 0)
            state->blend[i].write_mask = 0;
}

s_xe_static_state xe_static_state(const s_port_vk_render_state &rs, const s_shader_pack_xenos *ps, uint32_t slots)
{
    s_xe_static_state key = {};
    key.depth_format = g_pass_depth_format;
    const uint32_t fill = rs.fill_mode == 0x25 ? 1u : rs.fill_mode == 1 ? 2u : 0u;
    const uint32_t alpha_test = ps && rs.alpha_test_enable && (rs.alpha_func & 7u) != 7u;
    key.modes = fill | (alpha_test << 2);
    for (uint32_t i = 0; i < 4; ++i) {
        const s_target_entry *target = g_bound_color[i];
        const bool bound = target && target->image;
        if (bound && !(slots & (1u << i))) {
            key.modes |= 1u << (20u + i);
            continue;
        }
        key.color_formats[i] = bound ? target->host_format : (uint32_t)k_gpu_format_undefined;
        const uint32_t blend = rs.blend_control[i] & 0x1fff1fffu;
        key.blend[i] = blend;
        if (blend != 0x00010001u) {
            const auto constant = [](uint32_t f) { return (f & 0x1cu) == 12u; };
            if (constant(blend) || constant(blend >> 8) || constant(blend >> 16) || constant(blend >> 24))
                key.blend_factor = rs.blend_factor;
        }
        if (ps && (ps->color_outputs & (1u << i)))
            key.modes |= (rs.color_write_enable[i] & 15u) << (4u + 4u * i);
    }
    return key;
}

bool xe_graphics_pipeline(s_xe_pipeline_fast_cache &cache, const s_xe_rows_memo &live, const s_port_vk_draw *draw,
                           const s_shader_pack_xenos *vs, const s_shader_pack_xenos *ps,
                           uint32_t topology, bool restart, uint32_t slots)
{
    const uint32_t scale = g_pass_depth && draw->state.slope_scale_depth_bias ? bound_res_scale() : 1u;
    const bool same_draw = draw->state_gen != 0 && cache.dynamic_gen == draw->state_gen;
    const bool same_pass = cache.pass_epoch == g_pass_format_epoch;
    const bool same_identity = cache.pipeline && cache.live == &live && cache.topology == topology &&
                               cache.restart == (uint32_t)restart && cache.slots == slots;
    if (same_identity && same_draw && same_pass && cache.scale == scale)
        return true;

    const s_xe_static_state key = xe_static_state(draw->state, ps, slots);
    if (same_identity && cache.static_state == key) {
        if (!same_draw || cache.scale != scale) {
            if (g_vk && g_xe_libraries_on) {
                build_raster_depth_state(draw->state, scale, cache.state);
                vulkan_gpu_translate_dynamic_state(&cache.state, &cache.dynamic_state);
            } else {
                const size_t first = offsetof(s_gpu_graphics_state, cull_mode);
                const size_t bytes = offsetof(s_gpu_graphics_state, sample_count) - first;
                [[clang::uninitialized]] s_gpu_graphics_state state;
                state.stencil_format = cache.state.stencil_format;
                build_raster_depth_state(draw->state, scale, state);
                if (__builtin_memcmp((const uint8_t *)&cache.state + first, (const uint8_t *)&state + first, bytes) != 0) {
                    __builtin_memcpy((uint8_t *)&cache.state + first, (const uint8_t *)&state + first, bytes);
                    cache.pipeline = xe_pipeline(live.kind, vs, ps, live.extra, cache.state);
                }
            }
        }
    } else {
        xe_state(draw, topology, restart, ps, slots, &cache.state);
        cache.pipeline = xe_pipeline(live.kind, vs, ps, live.extra, cache.state);
        if (cache.pipeline && g_vk && vulkan_gpu_native_pipeline(cache.pipeline)->dynamic)
            vulkan_gpu_translate_dynamic_state(&cache.state, &cache.dynamic_state);
        cache.static_state = key;
        cache.live = &live;
        cache.topology = topology;
        cache.restart = restart;
        cache.slots = slots;
    }
    cache.dynamic_gen = draw->state_gen;
    cache.scale = scale;
    cache.pass_epoch = g_pass_format_epoch;
    return cache.pipeline != 0;
}

void xenos_draw(s_draw_geometry draw, uint64_t dirty_mask, uint32_t primitive_type, uint32_t start, uint32_t count, int base_vertex,
                uint32_t indexed, s_shader_entry *vs_entry)
{
    xe_apply_placements(); // placements since the last draw (port_vk_memory_placed)
    if (draw->vertex_shader == 0) {
        report(0x6e6f7673u, "a draw without a vertex shader (skipped)");
        return;
    }
    if (vs_entry == 0) {
        report(0x6e6f7368u, "a draw with an unregistered vertex shader (skipped)");
        return;
    }
    const s_shader_pack_xenos *vs = xe_meta_ptr(vs_entry);
    if (vs == 0) {
        xe_report_unknown(vs_entry, "vertex");
        return;
    }
    // the memory an open BeginExport names, as the GPU sees it, before anything reads or writes it (an update shader
    // fetches the records it rewrites from the same memory): a buffer's in the byte order the draw exports it in
    if (g_export.base != 0) {
        uint32_t endian = g_export.endian;
        if (g_export.texture == 0 && (vs->flags & k_shader_pack_xenos_memexport) != 0 && g_shadows.vertex_float != 0)
            endian = xe_stream_endian(*vs, g_shadows.vertex_float, g_export.base, g_export.size, endian);
        xe_export_for(g_export.base, g_export.size, g_export.texture, endian);
    }
    if (vs->model == k_shader_pack_xenos_compute) {
        xenos_memexport_draw(draw, dirty_mask, *vs, start, count, indexed, vs_entry);
        return;
    }
    if (vs->model != k_shader_pack_xenos_vertex) {
        report(vs->index ^ 0x74657364u, "a draw of a tessellation vertex shader without tessellation (skipped)",
               vs->name);
        return;
    }
    const bool depth_only = draw->pixel_shader == 0;
    s_shader_entry *ps_entry = depth_only ? 0 : shader_entry(draw->pixel_shader);
    const s_shader_pack_xenos *ps = depth_only ? 0 : xe_meta_ptr(ps_entry);
    if (!depth_only && (ps_entry == 0 || ps == 0)) {
        if (ps_entry == 0)
            report(0x6e6f7370u, "a draw with an unregistered pixel shader (skipped)");
        else
            xe_report_unknown(ps_entry, "pixel");
        return;
    }
    if (depth_only && draw->state.z_enable != 0 && (draw->state.z_func & 7u) == 0u && draw->state.stencil_enable == 0)
        return; // touches nothing but the Xbox's hierarchical Z (rasterizer_rebuild_hierarchical_z_buffer)
    if (!depth_only && (ps->ps_inputs & ~vs->vs_outputs) != 0)
        report(vs->index * 0x9e3779b1u ^ ps->index ^ 0x6c6e6b73u,
               "a pixel shader reading interpolators its vertex shader does not write (undefined)", ps->name);
    uint32_t topology, kind = k_xe_pipeline_vertex, extra = 0;
    bool quads = false, fan = false;
    switch (primitive_type) {
    case PORT_VK_PT_POINTLIST:
        topology = k_gpu_topology_point_list;
        // the point size comes from the shader (the Xenos falls back on PA_SU_POINT_SIZE, which the translator does not
        // model): a vertex shader that writes none cannot draw points
        if ((vs->flags & k_shader_pack_xenos_point_size) == 0) {
            report(vs->index ^ 0x70746c73u, "a point list whose vertex shader writes no point size (skipped)",
                   vs->name);
            return;
        }
        // a backend whose rasterizer draws no sizable points (Direct3D 12) expands them in the pack's geometry shader
        if (!g_gpu->caps.point_sprites) {
            extra = shader_pack_xenos_point_gs(g_pack);
            if (!g_gpu->caps.geometry || extra == SHADER_PACK_NOT_FOUND) {
                report(0x70746773u, "a point list without its geometry shader (skipped)", vs->name);
                return;
            }
            kind = k_xe_pipeline_points;
        }
        break;
    case PORT_VK_PT_LINELIST: topology = k_gpu_topology_line_list; break;
    case PORT_VK_PT_LINESTRIP: topology = k_gpu_topology_line_strip; break;
    case PORT_VK_PT_TRIANGLELIST: topology = k_gpu_topology_triangle_list; break;
    case PORT_VK_PT_TRIANGLEFAN:
        // a backend without fans (Direct3D 12) draws the fan's triangles as a list (fan_indices)
        fan = !g_gpu->caps.triangle_fans;
        topology = fan ? k_gpu_topology_triangle_list : k_gpu_topology_triangle_fan;
        break;
    case PORT_VK_PT_TRIANGLESTRIP: topology = k_gpu_topology_triangle_strip; break;
    case PORT_VK_PT_QUADLIST:
        topology = k_gpu_topology_triangle_list;
        quads = true;
        break;
    case PORT_VK_PT_RECTLIST:
        topology = k_gpu_topology_triangle_list;
        extra = shader_pack_xenos_rect_gs(g_pack, vs->vs_outputs);
        if (!g_gpu->caps.geometry || extra == SHADER_PACK_NOT_FOUND) {
            report(vs->vs_outputs ^ 0x72656374u, "a rectangle list without its geometry shader (skipped)", vs->name);
            return;
        }
        kind = k_xe_pipeline_rectangles;
        break;
    default:
        report(primitive_type * 0x9e37u + 3u, "a primitive type the renderer does not draw (skipped)");
        return;
    }
    const uint32_t written = written_color_slots(draw->state, depth_only ? 0u : ps->color_outputs);
    uint32_t slots = attached_color_slots(written);
    if (!begin_frame())
        return;
    // Prepare/convert bound textures (e.g. convert_pwl, downscale, CPU uploads) BEFORE ensure_pass() so compute/transfer
    // work never forces a freshly-opened 0-draw render pass to end and re-open.
    const s_shader_pack_xenos *metas[2] = { vs, ps };
    s_xe_draw_textures textures = {};
    uint32_t linear_gamma_mask = 0;
    if (!xe_texture_set(draw.packet, metas, 2, &textures, &linear_gamma_mask))
        return;
    const bool strip = topology == k_gpu_topology_triangle_strip || topology == k_gpu_topology_line_strip ||
                       topology == k_gpu_topology_triangle_fan;
    const bool restart = indexed && strip && draw->state.primitive_reset_enable;
    // the vertices the draw reads, for streams copied into the ring (dynamic and user-pointer data)
    s_vertex_range range = {};
    if (indexed) {
        range.indices = &draw.indices();
        range.index_first = start;
        range.index_count = count;
        range.base_vertex = base_vertex;
        if (draw.indices().data == 0 || (draw.indices().index_size != 2 && draw.indices().index_size != 4) ||
            ((uint64_t)start + count) * draw.indices().index_size > draw.indices().size)
            range.whole = true;
    } else {
        range.known = true;
        range.first = start;
        range.end = start + count;
    }
    const auto *layout = xe_pipeline_rows(kind, vs, ps, extra, vs_entry, ps_entry);
    if (!layout)
        return;
    const auto &live = *layout;
    static xe_vertex_fetch vertex_fetch;
    const xe_vertex_fetch *vf_ptr = &vertex_fetch;
    uint32_t vf_gen = 0;
    [[clang::uninitialized]] s_gpu_buffer *stream_views[XE_D3D12_VERTEX_STREAMS];
    if (!xe_vertex_fetch_fill(draw, dirty_mask, *vs, &range, live.rows[k_xe_block_vertex_fetch], &vertex_fetch, g_vk ? 0 : stream_views, &vf_ptr,
                              &vf_gen))
        return;
    s_data_binding index_binding = {};
    uint32_t index_count = 0;
    if (fan) {
        if (!fan_indices(draw.indices(), indexed, start, count, indexed && draw->state.primitive_reset_enable,
                         &index_binding, &index_count))
            return;
    } else if (indexed) {
        if (!upload_indices(draw.indices(), start, count, quads, &index_binding, &index_count))
            return;
    } else if (quads) {
        index_count = count / 4u * 6u;
        uint32_t *dst = (uint32_t *)ring_alloc(index_count * 4u + 4u, 16, &index_binding.offset);
        if (dst == 0)
            return;
        for (uint32_t q = 0; q < count / 4u; q++) {
            const uint32_t b = start + 4u * q;
            const uint32_t list[6] = { b, b + 1u, b + 2u, b, b + 2u, b + 3u };
            memcpy(dst + 6u * q, list, sizeof(list));
        }
        index_binding.buffer = ring_buffer();
    }
    if (!ensure_pass(touches_depth(draw->state), slots, written, &slots))
        return;
    auto &graphics = xe_pipe_fast_slot(live, topology, slots, restart);
    if (!xe_graphics_pipeline(graphics, live, draw.packet, vs, ps, topology, restart, slots))
        return;
    // after the pass has begun (edram_written): a depth restore names the targets' content as this draw leaves it
    if (!depth_only)
        note_depth_restore(draw.packet);
    [[clang::uninitialized]] xe_system sys_local;
    const xe_system *sys_ptr = 0;
    uint32_t sys_gen = 0;
    if (draw->clip_plane_enable == 0 && primitive_type != PORT_VK_PT_POINTLIST &&
        !g_gpu->caps.vertex_id_excludes_base_vertex) {
        sys_ptr = xe_system_fill_cached(draw.packet, &sys_gen);
    } else {
        xe_system_fill(draw.packet, &sys_local);
        // the Xenos rasterizes a point list as sprites: the pixel shader's param gen register carries their coordinates
        // (PsParamGen zw, read as SPRITETEXCOORD: the loading screen's sparkles) and a negative y (io.odin)
        if (primitive_type == PORT_VK_PT_POINTLIST)
            sys_local.alpha_test[2] |= XE_SYS_FLAG_POINT_PRIMITIVE;
        // gl_VertexIndex counts from the draw's vertex offset (indexed) or first vertex; Direct3D 12's SV_VertexID from 0
        // (caps.vertex_id_excludes_base_vertex): the shaders add xe_system.vertex.x, so it carries them there (quads
        // and fans drawn from the ring's index lists hold absolute vertex numbers already)
        if (g_gpu->caps.vertex_id_excludes_base_vertex)
            sys_local.vertex[0] += indexed ? (uint32_t)base_vertex : quads || fan ? 0u : start;
        sys_ptr = &sys_local;
    }
    checkpoint_draw(draw, vs_entry, ps_entry, primitive_type, start, count, indexed);
    xe_bind_graphics_pipeline(graphics);
    if (!xe_bind(graphics.pipeline, draw.packet, sys_ptr, vs, ps, vf_ptr, g_vk ? 0 : stream_views, textures, live, vf_gen, sys_gen,
                 linear_gamma_mask))
        return;
    set_viewport_scissor(draw.packet);
    if (indexed || quads || fan) {
        draw_indexed_from(index_binding, index_count, indexed ? base_vertex : 0, 1, 0);
    } else {
        record_draw(count, 1, start, 0);
    }
}

// DrawIndexedTessellatedVertices: Halo 3's adaptive water patches through the tessellator. The "index buffer" is the
// factor buffer (exported by the water_tessellation pass, or written by the CPU), three float edge factors a patch.
void xenos_draw_tessellated(s_draw_geometry draw, uint64_t dirty_mask, uint32_t primitive_type, int base_patch, uint32_t start_index,
                            uint32_t index_count, uint32_t mode, float minimum_level, float maximum_level)
{
    xe_apply_placements(); // placements since the last draw (port_vk_memory_placed)
    const bool depth_only = draw->pixel_shader == 0;
    s_shader_entry *vs_entry = shader_entry(draw->vertex_shader);
    s_shader_entry *ps_entry = depth_only ? 0 : shader_entry(draw->pixel_shader);
    const s_shader_pack_xenos *vs = xe_meta_ptr(vs_entry);
    const s_shader_pack_xenos *ps = depth_only ? 0 : xe_meta_ptr(ps_entry);
    if (vs == 0 || (!depth_only && ps == 0)) {
        xe_report_unknown(vs != 0 ? ps_entry : vs_entry, vs != 0 ? "pixel" : "vertex");
        return;
    }
    if (vs->model != k_shader_pack_xenos_tess_eval || primitive_type != 17u /* D3DTPT_TRIPATCH */ ||
        mode != 2u /* D3DTM_ADAPTIVE */ || !g_gpu->caps.tessellation || !g_gpu->caps.full_draw_index) {
        report(0x74657331u ^ (primitive_type << 8) ^ mode,
               "a tessellated draw other than adaptive triangle patches of a tessellation shader (skipped)");
        return;
    }
    // index_count counts patches (render_water_draw_indexed_tessellated_primitive: D3DTESSPRIMITIVEVERTEXCOUNT
    // [D3DTPT_TRIPATCH]); the XDK programs three factors a patch, from start_index patches in
    const uint32_t factors = index_count * 3u;
    const uint8_t *data = (const uint8_t *)draw.indices().data;
    if (data == 0 || ((uint64_t)start_index + index_count) * 12u > draw.indices().size) {
        report(0x74657333u, "tessellated patches without their factor buffer (skipped)");
        return;
    }
    const uint32_t written = written_color_slots(draw->state, depth_only ? 0u : ps->color_outputs);
    uint32_t slots = attached_color_slots(written);
    if (!begin_frame())
        return;
    const s_shader_pack_xenos *metas[2] = { vs, ps };
    s_xe_draw_textures textures = {};
    uint32_t linear_gamma_mask = 0;
    if (!xe_texture_set(draw.packet, metas, 2, &textures, &linear_gamma_mask))
        return;
    // the factors as 32-bit indices, in their guest byte order (the helper vertex shader swaps them)
    s_data_binding factor_binding = {};
    uint32_t export_offset = 0;
    bool factors_exported = false;
    if (const s_xe_export *e = xe_export_holding(data, &export_offset)) {
        factor_binding.buffer = e->buffer;
        factors_exported = true;
        factor_binding.offset = export_offset + start_index * 12u;
    } else {
        void *dst = ring_alloc(index_count * 12u, 16, &factor_binding.offset);
        if (dst == 0)
            return;
        memcpy(dst, index_bytes(draw.indices(), (size_t)start_index * 12u), index_count * 12u);
        factor_binding.buffer = ring_buffer();
    }
    s_vertex_range range = {};
    const auto *layout = xe_pipeline_rows(k_xe_pipeline_tessellated, vs, ps, 0, vs_entry, ps_entry);
    if (!layout)
        return;
    const auto &live = *layout;
    range.whole = true;
    static xe_vertex_fetch vertex_fetch;
    const xe_vertex_fetch *vf_ptr = &vertex_fetch;
    uint32_t vf_gen = 0;
    s_gpu_buffer *stream_views[XE_D3D12_VERTEX_STREAMS];
    if (!xe_vertex_fetch_fill(draw, dirty_mask, *vs, &range, live.rows[k_xe_block_vertex_fetch], &vertex_fetch, g_vk ? 0 : stream_views, &vf_ptr,
                              &vf_gen))
        return;
    if (!ensure_pass(touches_depth(draw->state), slots, written, &slots))
        return;
    auto &graphics = xe_pipe_fast_slot(live, k_gpu_topology_patch_list, slots, false);
    if (!xe_graphics_pipeline(graphics, live, draw.packet, vs, ps, k_gpu_topology_patch_list, false, slots))
        return;
    [[clang::uninitialized]] xe_system sys; // written whole by xe_system_fill
    xe_system_fill(draw.packet, &sys);
    sys.vertex[0] = (uint32_t)base_patch; // the patch index the shader sees: gl_PrimitiveID + VGT_INDX_OFFSET
    sys.tessellation[0] = minimum_level + 1.0f;
    sys.tessellation[1] = maximum_level + 1.0f;
    // the factor bytes' order: what the Xenos wrote into exported memory (the factor pass's stream constant: GPUENDIAN_8IN32,
    // as the INDEX32 buffer's fetch reads it), else the index buffer's own
    const uint32_t endian = factors_exported || draw.indices().big_endian ? 2u : 0u;
    memcpy(&sys.tessellation[2], &endian, 4);
    checkpoint_draw(draw, vs_entry, ps_entry, primitive_type, start_index, index_count, 1);
    xe_bind_graphics_pipeline(graphics);
    if (!xe_bind(graphics.pipeline, draw.packet, &sys, vs, ps, vf_ptr, g_vk ? 0 : stream_views, textures, live, vf_gen, 0,
                 linear_gamma_mask))
        return;
    set_viewport_scissor(draw.packet);
    draw_indexed_from(factor_binding, factors, 0, 1, 0, true);
}

int xenos_export_read(const void *base, uint32_t record, uint32_t count, void *out, uint32_t *out_stride)
{
    xe_apply_placements(); // placements since the last draw (port_vk_memory_placed)
    uint32_t offset = 0;
    const s_xe_export *e = xe_export_holding(base, &offset);
    // the guest bytes as the Xenos wrote them, in 16-byte records (the record size is the shader's business)
    const uint32_t stride = 16;
    if (e == 0 || offset != 0 || record > e->size / stride || count > e->size / stride - record)
        return 0;
    if (out_stride != 0)
        *out_stride = stride;
    if (count == 0)
        return 1;
    finish_frame();
    return gpu_buffer_download(g_gpu, e->buffer, record * stride, out, count * stride) == k_gpu_ok;
}
} // namespace

extern "C" {

s_port_vk_storage * PORT_MS_ABI port_vulkan_storage_create(uint32_t size, const void *data)
{
    if (record_remote()) {
        return record_result([&] { return port_vulkan_storage_create(size, data); });
    }
    if (g_started <= 0 || size == 0)
        return 0;
    s_port_vk_storage *storage = (s_port_vk_storage *)calloc(1, sizeof(s_port_vk_storage));
    if (storage == 0)
        return 0;
    storage->size = size;
    storage->buffer = gpu_buffer_create(g_gpu, size, k_gpu_buffer_storage |
                                        k_gpu_buffer_transfer_src | k_gpu_buffer_transfer_dst,
                                        k_gpu_memory_gpu);
    void *zeros = data == 0 ? calloc(1, size) : 0;
    if (storage->buffer == 0 || (data == 0 && zeros == 0) ||
        gpu_buffer_upload(g_gpu, storage->buffer, 0, data != 0 ? data : zeros, size) != k_gpu_ok) {
        free(zeros);
        if (storage->buffer != 0)
            gpu_buffer_destroy(g_gpu, storage->buffer);
        free(storage);
        return 0;
    }
    free(zeros);
    return storage;
}

void PORT_MS_ABI port_vulkan_storage_destroy(s_port_vk_storage *storage)
{
    if (record_remote()) {
        return record_dispatch([=] { port_vulkan_storage_destroy(storage); });
    }
    if (storage == 0)
        return;
    gpu_buffer_retire(g_gpu, storage->buffer); // recorded draws or a frame in flight may still use it
    free(storage);
}

int PORT_MS_ABI port_vulkan_storage_read(s_port_vk_storage *storage, uint32_t offset, uint32_t size, void *out)
{
    if (record_remote()) {
        return record_result([&] { return port_vulkan_storage_read(storage, offset, size, out); });
    }
    if (g_started <= 0 || storage == 0 || out == 0)
        return 0;
    finish_frame();
    return gpu_buffer_download(g_gpu, storage->buffer, offset, out, size) == k_gpu_ok;
}

int PORT_MS_ABI port_vulkan_dispatch_explicit_compute(const s_port_vk_compute_dispatch *dispatch)
{
    if (record_remote())
        return record_result([&] {
            const auto saved = g_shadows;
            g_shadows = g_live_shadows;
            const int ok = port_vulkan_dispatch_explicit_compute(dispatch);
            g_shadows = saved;
            return ok;
        });
    if (g_started <= 0 || dispatch == 0 || g_pack == 0)
        return 0;
    const uint32_t shader_index = shader_pack_find_explicit(g_pack, dispatch->explicit_index, k_shader_pack_stage_compute);
    s_shader_pack_shader_info info;
    if (shader_index == SHADER_PACK_NOT_FOUND || !shader_pack_get_shader(g_pack, shader_index, &info)) {
        report(dispatch->explicit_index ^ 0x63736e66u, "a dispatch of an explicit slot without a compute shader");
        return 0;
    }
    return record_compute(shader_index, dispatch->groups_x, dispatch->groups_y, dispatch->groups_z, 0, {}, 0, 0, 0,
                          dispatch) ? 1 : 0;
}

void PORT_MS_ABI port_vk_begin_export(uint32_t index, const void *base, uint32_t size, uint32_t xbox_stride)
{
    if (record_remote()) {
        return record_target_command({index == 0 ? base : nullptr, size}, nullptr, [=] {
            port_vk_begin_export(index, base, size, xbox_stride);
        });
    }
    if (index != 0) {
        report(0x65787031u + index, "a memory export through a slot other than 0 (not modelled)");
        return;
    }
    memset(&g_export, 0, sizeof(g_export));
    g_export.base = base;
    g_export.size = size;
    g_export.xbox_stride = xbox_stride;
}

static void begin_texture_export(uint32_t index, const void *texture_header, const s_port_d3d_texture_source &source)
{
    if (g_started <= 0 || texture_header == 0)
        return;
    if (index != 0) {
        report(0x65787031u + index, "a memory export through a slot other than 0 (not modelled)");
        return;
    }
    if (!g_xenos) {
        report(0x74786578u, "a texture's memory export without a version 3 shader pack (not modelled)");
        return;
    }
    // the base level's memory, as XGSetTextureHeader sizes it (whole 4 KB pages, the allocation the game makes)
    s_xenos_texture_layout layout;
    if (source.base == 0 ||
        !xenos_texture_get_layout(&source.desc, &layout) || layout.base_size == 0) {
        report(key_of(texture_header) ^ 0x74786562u, "a memory export of a texture without base memory (not modelled)");
        return;
    }
    memset(&g_export, 0, sizeof(g_export));
    g_export.base = source.base;
    g_export.size = layout.base_size;
    g_export.texture = 1;
    g_export.endian = xenos_d3dformat_endian(source.desc.d3d_format);
}

void PORT_MS_ABI port_vk_begin_texture_export(uint32_t index, const void *texture_header)
{
    if (g_started <= 0 || !texture_header) return;
    s_port_d3d_texture_source source = {};
    if (!port_d3d_texture_get_source(texture_header, &source)) source.base = nullptr;
    auto begin = [=] { begin_texture_export(index, texture_header, source); };
    if (!record_remote()) return begin();
    s_record_range spans[2] = {};
    if (index == 0 && source.base) record_texture_ranges(source, spans);
    record_target_command(spans[0], nullptr, begin);
}

void PORT_MS_ABI port_vk_end_export(uint32_t index)
{
    if (record_remote()) {
        if (!record_owner())
            return record_sync([&] { port_vk_end_export(index); });
        return record_post([=] { port_vk_end_export(index); });
    }
    if (index == 0) {
        memset(&g_export, 0, sizeof(g_export));
        if (g_record_worker) {
            s_record_resource_guard guard;
            g_record_open_export = {};
        }
    }
}

int PORT_MS_ABI port_vk_export_read(const void *base, uint32_t record, uint32_t count, void *out, uint32_t *out_stride)
{
    if (record_remote()) {
        return record_result([&] { return port_vk_export_read(base, record, count, out, out_stride); });
    }
    if (g_xenos)
        return xenos_export_read(base, record, count, out, out_stride);
    const s_export_buffer *entry = export_buffer_find(base);
    if (entry == 0 || entry->storage == 0 || record > entry->records || count > entry->records - record)
        return 0;
    if (out_stride != 0)
        *out_stride = entry->stride;
    return count == 0 || port_vulkan_storage_read(entry->storage, record * entry->stride, count * entry->stride, out);
}

void PORT_MS_ABI port_vk_invalidate_draw_caches(void)
{
    if (record_remote()) {
        __atomic_add_fetch(&g_record_revision, 1u, __ATOMIC_RELEASE);
        return record_dispatch([] { xe_invalidate_draw_caches(); });
    }
    xe_invalidate_draw_caches();
}

void PORT_MS_ABI port_vk_texture_header_changed(void)
{
    resource_touch_changed();
}

void PORT_MS_ABI port_vk_draw_tessellated(const s_port_vk_draw *packet, uint64_t dirty_mask, uint32_t primitive_type,
                                          int base_patch, uint32_t start_index, uint32_t index_count, uint32_t mode,
                                          float minimum_level, float maximum_level)
{
    if (record_remote() && packet) {
        record_draw(packet, dirty_mask, { primitive_type, start_index, index_count, base_patch, 1, mode,
                                          minimum_level, maximum_level, true }, nullptr);
        return;
    }
    if (g_started <= 0 || packet == 0 || index_count == 0)
        return;
    const s_draw_geometry draw = { packet, 0 };
    g_half_pixel_offset = draw->pixel_center_offset != 0.0f;
    ring_ensure_headroom(); // before the draw records anything
    if (g_xenos) {
        xenos_draw_tessellated(draw, dirty_mask, primitive_type, base_patch, start_index, index_count, mode, minimum_level,
                               maximum_level);
        return;
    }
    s_shader_entry *vs = shader_entry(draw->vertex_shader);
    s_shader_pack_shader_info vs_info = {};
    if (vs == 0 || vs->shader_index == SHADER_PACK_NOT_FOUND ||
        !shader_pack_get_shader(g_pack, vs->shader_index, &vs_info) || vs_info.vertex_type != k_shader_pack_vertex_water ||
        primitive_type != 17u /* D3DTPT_TRIPATCH */ || mode != 2u /* D3DTM_ADAPTIVE */) {
        report(0x74657331u ^ (primitive_type << 8) ^ mode,
               "a tessellated draw other than Halo 3's adaptive water triangle patches (skipped)");
        return;
    }
    // adaptive: the "index buffer" holds three factors a patch (the factor buffer the factor pass wrote). index_count
    // counts patches: the game takes it from D3DTESSPRIMITIVEVERTEXCOUNT[D3DTPT_TRIPATCH] = { 1, 0 } (retail .rdata
    // 0x82021410; render_water_draw_indexed_tessellated_primitive), and the XDK programs 3 * index_count factors
    // (xenia xenos.h: 4D5307E6 water, "3 times the invocation count"). start_index is taken in patches too (Halo passes 0
    // and offsets the buffer header instead).
    const uint32_t patches = index_count;
    const uint8_t *data = (const uint8_t *)draw.indices().data;
    const float *factors = 0;
    if (data != 0 && ((uint64_t)start_index + patches) * 12u <= draw.indices().size) {
        factors = water_factors_for(data + (size_t)start_index * 12u, patches * 12u);
        if (factors == 0) {
            report(0x74657332u, "water patches whose factor buffer no factor pass wrote this frame (minimum level)");
        }
    } else {
        report(0x74657333u, "water patches without their factor buffer (minimum level)");
    }
    // the patch index the vertex shader sees, plus its k_water_index_offset (c131.x): the water triangle
    const float *c = g_shadows.vertex_float;
    const int64_t first = (int64_t)base_patch + (c != 0 ? (int32_t)c[4 * 131] : 0);
    if (first < 0)
        return;
    water_draw_patches(draw, (uint32_t)first, patches, factors, factors != 0 ? minimum_level : 1.0f,
                       factors != 0 ? maximum_level : 1.0f);
}

} // extern "C"
