#ifndef PORT_D3D_GPU_BACKEND_H
#define PORT_D3D_GPU_BACKEND_H

/*
 * The GPU backend seam: the one interface between the PC renderer's Xenos GPU frontend (vulkan_device.cpp: the D3D
 * device's render targets, EDRAM aliasing, resolves, texture cache, constant blocks, vertex fetch, memexport, shader
 * pack lookup, presentation pacing; every Xenos semantic) and a host graphics API. The frontend includes this header
 * and nothing of an API: no Vulkan or Direct3D 12 types, enums or calls. A backend is a table of operations
 * (s_gpu_backend_ops) and a description of what it can do (s_gpu_caps); gpu_backend.cpp picks one at start-up
 * (HALO3_RENDERER, halo3.ini [display] renderer: gpu_renderer_requested).
 *
 * Backends:
 *   vulkan_gpu.cpp    Vulkan 1.3 (Linux, Windows): the operations over vulkan_backend.h, translating the enums below
 *   d3d12_backend.cpp Direct3D 12 (Windows): the operations over ID3D12Device (d3d12_mini.h; d3d12.dll and dxgi.dll
 *                     loaded at run time), the pack's root signature as the Xenos layout
 *
 * Model. The operations are those of a modern explicit API, in neutral terms: buffers, images and their views,
 * samplers, shader modules from the shader pack, pipelines (whole, or linked from library parts where the backend has
 * them), bind groups (Vulkan descriptor sets; D3D12 descriptor table allocations and root descriptors) of bind layouts
 * (descriptor set layouts; root signature ranges), a ring of frames recorded on one thread, dynamic rendering into
 * images, the port's compute helper passes (edram_7e3, pwl_convert, depth_pack, display_gamma, resolve_scale,
 * resolve_downscale: helper_binding.hlsli) behind one operation each, occlusion queries and a swapchain.
 *
 * The translated Xenos shaders (shader pack version 3: SPIR-V, version 4: DXIL; port/shaders/xenos_spirv_abi.h) share
 * one binding contract on every backend, "register = binding, space = set":
 *   bind group 0  XE_BLOCK count dynamic uniform buffers (the six constant blocks, ring offsets per draw)
 *   bind group 1  128 sampled images + 192 samplers (the fetch constants' textures and sampler variants)
 *   and either device addresses in the blocks (caps.buffer_device_address: Vulkan) or the vertex-stream and memexport
 *   views of gpu_cmd_xenos_bind_memory (D3D12: t0-t15 / u0-u3 space2), whose offsets the blocks hold instead.
 * gpu_xenos_layout makes that layout (Vulkan: the two set layouts and their pipeline layout; D3D12: the pack's root
 * signature, s_shader_pack_file_header_v4.root_signature_module).
 *
 * Capabilities a backend states (s_gpu_caps) and the frontend honours:
 *   buffer_device_address            vertex fetch and memexport by 64-bit device address, else by view + byte offset;
 *   vertex_id_excludes_base_vertex   SV_VertexID excludes BaseVertexLocation / StartVertexLocation (D3D12), so the
 *                                    frontend folds them into XeSystem.vertex.x (not for tessellated draws);
 *   point_sprites                    the rasterizer draws point sprites from PointSize (Vulkan); else point lists go
 *                                    through the pack's point geometry shader with XeSystem.point.zw;
 *   triangle_fans                    the API draws triangle fans (Vulkan); else the frontend draws a fan as the list
 *                                    of its triangles (Direct3D 12 has no fans);
 *   pipeline_library                 graphics pipelines may be linked from parts (VK_EXT_graphics_pipeline_library);
 *   shader_module_format             the pack module format the backend takes (SPIR-V or DXIL; a pack of the other
 *                                    format is refused at start-up).
 *
 * Handles. Objects with state of their own on the frontend's side are opaque pointers (s_gpu_buffer, s_gpu_image,
 * s_gpu_render_target, s_gpu_pipeline): each backend allocates its own and casts. Plain API objects are 64-bit
 * handles (GPU_DEFINE_HANDLE, as Vulkan's non-dispatchable handles: typed pointers on 64-bit hosts, uint64_t on the
 * port's 32-bit targets), GPU_NULL_HANDLE when none.
 *
 * Host C++ only (halo3_gpu): not for the game's MS-ABI code, which talks to the frontend (vulkan_device.h). Threading:
 * one thread records and submits; object creation that the pipeline prewarm runs on worker threads (pipeline and
 * library parts, compute pipeline handles, pack modules under the frontend's lock) is thread-safe. Lifetimes: objects
 * are destroyed at once (the caller makes sure the GPU is done with them) unless retired (destroyed once the frames
 * that may use them are done). Failures are logged by the backend with their reason; creators return NULL /
 * GPU_NULL_HANDLE, other operations a gpu_result.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct s_shader_pack;
struct s_xenos_vertex_declaration;

/* ---- handles ---- */

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__)
#define GPU_DEFINE_HANDLE(name) typedef struct name##_T *name;
#else
#define GPU_DEFINE_HANDLE(name) typedef uint64_t name;
#endif
#define GPU_NULL_HANDLE 0

GPU_DEFINE_HANDLE(gpu_view)            /* a view of an image shaders read (VkImageView; a D3D12 SRV) */
GPU_DEFINE_HANDLE(gpu_sampler)         /* VkSampler; a D3D12 sampler descriptor */
GPU_DEFINE_HANDLE(gpu_shader)          /* a shader module (a SPIR-V module; a DXIL container) */
GPU_DEFINE_HANDLE(gpu_bind_layout)     /* VkDescriptorSetLayout; root signature ranges */
GPU_DEFINE_HANDLE(gpu_pipeline_layout) /* VkPipelineLayout; a root signature */
GPU_DEFINE_HANDLE(gpu_bind_group)      /* VkDescriptorSet; descriptor table allocations */
GPU_DEFINE_HANDLE(gpu_pipeline_handle) /* a bare pipeline object or library part (VkPipeline; ID3D12PipelineState) */

typedef struct s_gpu_backend s_gpu_backend;
typedef struct s_gpu_buffer s_gpu_buffer;
typedef struct s_gpu_image s_gpu_image;
typedef struct s_gpu_render_target s_gpu_render_target;
typedef struct s_gpu_pipeline s_gpu_pipeline;

/* ---- results ---- */

typedef int32_t gpu_result;
enum e_gpu_result {
    k_gpu_ok = 0,
    k_gpu_not_ready = 1,        /* not finished (a frame slot being recorded) */
    k_gpu_incomplete = 2,       /* data was cut short to the size given (the pipeline cache) */
    k_gpu_suboptimal = 3,       /* the swapchain still works but should be made again */
    k_gpu_error = -1,           /* any other failure (logged) */
    k_gpu_out_of_memory = -2,
    k_gpu_device_lost = -3,
    k_gpu_out_of_date = -4,     /* the swapchain must be made again before the next acquire */
    k_gpu_surface_lost = -5,
    k_gpu_unsupported = -6,     /* the backend, device or platform cannot do it */
};
/* "VK_SUCCESS"-style text for logs (the backend's own name for the result where it has one). */
const char *gpu_result_string(gpu_result result);

/* ---- formats ---- */

/* Image, render target and vertex attribute formats. The normalized / integer families keep their members in the
 * order UNORM, SNORM, USCALED, SSCALED, UINT, SINT (xenos_vertex.cpp picks a member by its offset from the family's
 * UNORM), and the 32-bit ones UINT, SINT, SFLOAT. Packed formats name their components from the most significant bit
 * down (a2b10g10r10: R in bits 0-9, as the Xenos 2_10_10_10 lays it out). */
enum e_gpu_format {
    k_gpu_format_undefined = 0,

    k_gpu_format_r8g8b8a8_unorm, k_gpu_format_r8g8b8a8_snorm, k_gpu_format_r8g8b8a8_uscaled, k_gpu_format_r8g8b8a8_sscaled,
    k_gpu_format_r8g8b8a8_uint, k_gpu_format_r8g8b8a8_sint, k_gpu_format_r8g8b8a8_srgb,
    k_gpu_format_b8g8r8a8_unorm, k_gpu_format_b8g8r8a8_snorm, k_gpu_format_b8g8r8a8_uscaled, k_gpu_format_b8g8r8a8_sscaled,
    k_gpu_format_b8g8r8a8_uint, k_gpu_format_b8g8r8a8_sint, k_gpu_format_b8g8r8a8_srgb,
    k_gpu_format_a2b10g10r10_unorm, k_gpu_format_a2b10g10r10_snorm, k_gpu_format_a2b10g10r10_uscaled,
    k_gpu_format_a2b10g10r10_sscaled, k_gpu_format_a2b10g10r10_uint, k_gpu_format_a2b10g10r10_sint,
    k_gpu_format_r16g16_unorm, k_gpu_format_r16g16_snorm, k_gpu_format_r16g16_uscaled, k_gpu_format_r16g16_sscaled,
    k_gpu_format_r16g16_uint, k_gpu_format_r16g16_sint, k_gpu_format_r16g16_sfloat,
    k_gpu_format_r16g16b16a16_unorm, k_gpu_format_r16g16b16a16_snorm, k_gpu_format_r16g16b16a16_uscaled,
    k_gpu_format_r16g16b16a16_sscaled, k_gpu_format_r16g16b16a16_uint, k_gpu_format_r16g16b16a16_sint,
    k_gpu_format_r16g16b16a16_sfloat,
    k_gpu_format_r32_uint, k_gpu_format_r32_sint, k_gpu_format_r32_sfloat,
    k_gpu_format_r32g32_uint, k_gpu_format_r32g32_sint, k_gpu_format_r32g32_sfloat,
    k_gpu_format_r32g32b32_uint, k_gpu_format_r32g32b32_sint, k_gpu_format_r32g32b32_sfloat,
    k_gpu_format_r32g32b32a32_uint, k_gpu_format_r32g32b32a32_sint, k_gpu_format_r32g32b32a32_sfloat,

    k_gpu_format_r8_unorm, k_gpu_format_r8_snorm,
    k_gpu_format_r8g8_unorm, k_gpu_format_r8g8_snorm,
    k_gpu_format_r16_unorm, k_gpu_format_r16_snorm, k_gpu_format_r16_sfloat,
    k_gpu_format_b4g4r4a4_unorm_pack16, k_gpu_format_b5g6r5_unorm_pack16, k_gpu_format_a1r5g5b5_unorm_pack16,

    k_gpu_format_bc1_rgba_unorm, k_gpu_format_bc1_rgba_srgb,
    k_gpu_format_bc2_unorm, k_gpu_format_bc2_srgb,
    k_gpu_format_bc3_unorm, k_gpu_format_bc3_srgb,
    k_gpu_format_bc4_unorm, k_gpu_format_bc5_unorm, k_gpu_format_bc5_snorm,

    k_gpu_format_d24_unorm_s8_uint,
    k_gpu_format_d32_sfloat_s8_uint,

    k_gpu_format_count
};

/* Block size of a format: 4x4 for the BC formats, 1x1 otherwise. 0 for formats without a texel size here (undefined,
 * depth / stencil). */
int gpu_format_block_info(uint32_t format, uint32_t *out_block_width, uint32_t *out_block_height,
                          uint32_t *out_bytes_per_block);
/* The format's name ("r8g8b8a8_unorm"), for logs and traces. */
const char *gpu_format_name(uint32_t format);
/* Writes 8-bit RGB pixels as a binary PPM (P6); format: k_gpu_format_r8g8b8a8_* or b8g8r8a8_* (alpha dropped).
 * Returns 1 on success. */
int gpu_write_ppm(const char *path, const void *pixels, uint32_t width, uint32_t height, uint32_t row_pitch,
                  uint32_t format);
/* FNV-1a over bytes, from `hash` (2166136261u to start). */
uint32_t gpu_hash_bytes(uint32_t hash, const void *data, size_t size);

/* ---- enums of the fixed-function state ---- */

/* The comparison functions in the Xenos CompareFunction order, which the Xbox D3DCMP_* values are (D3DCMP_NEVER 0 ..
 * D3DCMP_ALWAYS 7: the device takes D3DRS_ZFUNC & 7 as is). */
enum e_gpu_compare_op {
    k_gpu_compare_never = 0,
    k_gpu_compare_less = 1,
    k_gpu_compare_equal = 2,
    k_gpu_compare_less_equal = 3,
    k_gpu_compare_greater = 4,
    k_gpu_compare_not_equal = 5,
    k_gpu_compare_greater_equal = 6,
    k_gpu_compare_always = 7,
};

/* The stencil operations in the Xbox D3DSTENCILOP / Xenos StencilOp order. */
enum e_gpu_stencil_op {
    k_gpu_stencil_keep = 0,
    k_gpu_stencil_zero = 1,
    k_gpu_stencil_replace = 2,
    k_gpu_stencil_increment_clamp = 3,
    k_gpu_stencil_decrement_clamp = 4,
    k_gpu_stencil_invert = 5,
    k_gpu_stencil_increment_wrap = 6,
    k_gpu_stencil_decrement_wrap = 7,
};

enum e_gpu_blend_factor {
    k_gpu_blend_zero = 0,
    k_gpu_blend_one,
    k_gpu_blend_src_color,
    k_gpu_blend_one_minus_src_color,
    k_gpu_blend_dst_color,
    k_gpu_blend_one_minus_dst_color,
    k_gpu_blend_src_alpha,
    k_gpu_blend_one_minus_src_alpha,
    k_gpu_blend_dst_alpha,
    k_gpu_blend_one_minus_dst_alpha,
    k_gpu_blend_constant_color,
    k_gpu_blend_one_minus_constant_color,
    k_gpu_blend_constant_alpha,
    k_gpu_blend_one_minus_constant_alpha,
    k_gpu_blend_src_alpha_saturate,
};

enum e_gpu_blend_op {
    k_gpu_blend_op_add = 0,
    k_gpu_blend_op_subtract,
    k_gpu_blend_op_reverse_subtract,
    k_gpu_blend_op_min,
    k_gpu_blend_op_max,
};

/* Color write mask bits: D3DCOLORWRITEENABLE_RED .. ALPHA. */
#define GPU_COLOR_WRITE_R 1u
#define GPU_COLOR_WRITE_G 2u
#define GPU_COLOR_WRITE_B 4u
#define GPU_COLOR_WRITE_A 8u
#define GPU_COLOR_WRITE_ALL 15u

enum e_gpu_topology {
    k_gpu_topology_point_list = 0,
    k_gpu_topology_line_list,
    k_gpu_topology_line_strip,
    k_gpu_topology_triangle_list,
    k_gpu_topology_triangle_strip,
    k_gpu_topology_triangle_fan,
    k_gpu_topology_patch_list,  /* the tessellator's input (patch_control_points per patch) */
};

enum e_gpu_polygon_mode {
    k_gpu_polygon_fill = 0,
    k_gpu_polygon_line,
    k_gpu_polygon_point,
};

enum e_gpu_cull_mode {
    k_gpu_cull_none = 0,
    k_gpu_cull_front = 1,
    k_gpu_cull_back = 2,
};

/* Which winding faces the viewer, in D3D's framebuffer orientation (top-left origin, y down): every backend presents
 * the same orientation (Vulkan: a negative-height viewport), so the D3D winding holds as it is. */
enum e_gpu_front_face {
    k_gpu_front_face_clockwise = 0,         /* D3D's default */
    k_gpu_front_face_counter_clockwise = 1,
};

enum e_gpu_index_type {
    k_gpu_index_uint16 = 0,
    k_gpu_index_uint32 = 1,
};

/* Shader stages (bits). */
enum e_gpu_stage {
    k_gpu_stage_vertex = 1u,
    k_gpu_stage_tess_control = 2u,    /* Vulkan tessellation control; D3D12 hull shader */
    k_gpu_stage_tess_eval = 4u,       /* Vulkan tessellation evaluation; D3D12 domain shader */
    k_gpu_stage_geometry = 8u,
    k_gpu_stage_fragment = 16u,
    k_gpu_stage_compute = 32u,
    k_gpu_stage_all_graphics = 31u,
    k_gpu_stage_all = 63u,
};

enum e_gpu_bind_point {
    k_gpu_bind_graphics = 0,
    k_gpu_bind_compute = 1,
};

/* ---- the backend ---- */

/* Receives one line of log text (ending in '\n'). */
typedef void (*gpu_log_function)(void *context, const char *text);
/* Writes a description of a checkpoint marker (gpu_cmd_checkpoint) to the log. */
typedef void (*gpu_checkpoint_describe_function)(void *context, const void *marker);

enum e_gpu_backend_kind {
    k_gpu_backend_none = 0,
    k_gpu_backend_vulkan = 1,
    k_gpu_backend_d3d12 = 2,
};

/* Window systems a surface may be created for (instance extensions or factories are chosen at creation). The values
 * are port_window_system()'s (port/sdk/runtime/port_window.h). */
enum e_gpu_window_system {
    k_gpu_window_system_none = 0,     /* headless: offscreen rendering only */
    k_gpu_window_system_win32 = 1,
    k_gpu_window_system_xlib = 2,
    k_gpu_window_system_xcb = 3,
    k_gpu_window_system_headless = 4, /* a swapchain with no window (testing) */
};

struct s_gpu_backend_create_info {
    const char *application_name;     /* NULL: "halo3" */
    uint32_t window_system;           /* e_gpu_window_system */
    uint32_t validation;              /* nonzero: the API's validation layer when installed */
    uint32_t frames_in_flight;        /* frames recorded ahead of the GPU: 1..4, 0 for 2 */
    int32_t device_index;             /* the adapter to use, -1 to choose (discrete > integrated > virtual > CPU) */
    const void *pipeline_cache_data;  /* the backend's own pipeline cache blob (gpu_get_pipeline_cache), or NULL */
    uint32_t pipeline_cache_size;
    gpu_log_function log;             /* NULL: port_write_error (port/sdk/runtime) */
    void *log_context;
    uint32_t xenos_features;          /* nonzero: also turn on what translated Xenos shaders use (shader pack version
                                         3 / 4) when the device has it (caps report what it got) */
};

/* What a backend and its device can do. Filled at creation, constant afterwards. */
#define GPU_FRAMES_MAX 4u
struct s_gpu_caps {
    uint32_t kind;                    /* e_gpu_backend_kind */
    const char *name;                 /* the backend's log name: "vulkan", "d3d12" */
    const char *api_name;             /* "Vulkan", "Direct3D 12" */
    uint32_t api_major, api_minor;    /* the device's API version */
    char device_name[256];
    uint32_t shader_module_format;    /* SHADER_PACK_MODULE_FORMAT_SPIRV / _DXIL (port/shaders/shader_pack.h) */
    const char *pipeline_cache_file;  /* the data file of its pipeline cache blob ("pipeline_cache.bin") */
    uint32_t frames_in_flight;        /* frame slots: gpu_frame_index is 0 .. frames_in_flight - 1 (<= GPU_FRAMES_MAX) */
    uint32_t uniform_offset_alignment;/* bind offsets of uniform buffers are multiples of this (<= 4096) */
    uint32_t depth_stencil_format;    /* e_gpu_format of depth / stencil targets (d32_sfloat_s8_uint, else d24) */
    uint32_t validation;              /* the API's validation layer is on */
    uint32_t checkpoints;             /* device-loss checkpoints are on (gpu_cmd_checkpoint) */
    /* the translated Xenos shaders' needs (see the top) */
    uint32_t buffer_device_address;   /* 1: vertex fetch / memexport by device address; 0: by view + byte offset */
    uint32_t vertex_id_excludes_base_vertex;
    uint32_t point_sprites;           /* 1: points rasterized as sprites from PointSize; 0: the pack's point GS */
    uint32_t triangle_fans;           /* 1: k_gpu_topology_triangle_fan drawn as such; 0: fans come as triangle lists */
    uint32_t clip_distance;
    uint32_t geometry;                /* geometry shaders (rectangle lists; point lists without point_sprites) */
    uint32_t tessellation;
    uint32_t full_draw_index;         /* 32-bit index values above 2^24 (the water factors as indices) */
    uint32_t sampler_mirror_clamp_to_edge; /* D3DTADDRESS_MIRRORONCE */
    uint32_t pipeline_library;        /* graphics pipelines linked from library parts (gpu_pipeline_library_*) */
    uint32_t pipeline_library_fast_link; /* an unoptimized link is cheap: link fast, optimize on a worker */
    /* the universal Xenos layout's bind counts must fit (gpu_xenos_layout checks them too) */
    uint32_t max_stage_samplers, max_stage_sampled_images, max_stage_uniform_buffers, max_dynamic_uniform_buffers;
    uint32_t max_bind_groups;
};

/* Counters since the backend was made (HALO3_FRAME_STATS, HALO3_FRAME_TRACE). */
struct s_gpu_stats {
    uint32_t immediate_submits;       /* one-off submissions waited for at once (first-use uploads, readbacks) */
    uint32_t deferred_uploads;        /* uploads batched ahead of the frame's commands */
    uint32_t memory_allocations;
    uint32_t buffers_reused;          /* buffer creations served from the pool of retired ones */
    float gpu_frame_ms;               /* GPU time of the last finished frame (timestamps), < 0 when unknown */
    /* binds the frame command buffers recorded and those skipped as repeats */
    uint32_t pipelines, pipelines_skipped;
    uint32_t dynamic_states;          /* binds of a library pipeline whose dynamic state changed */
    uint32_t sets, sets_skipped;      /* bind groups */
    uint32_t index_buffers, index_buffers_skipped;
    uint32_t barriers;                /* image barrier commands */
    uint32_t transitions_skipped;     /* a read-only layout to itself: no barrier needed */
};

/* ---- memory, buffers ---- */

enum e_gpu_memory_usage {
    k_gpu_memory_gpu = 0,      /* device-local; filled by gpu_buffer_upload (staging) or the GPU */
    k_gpu_memory_upload = 1,   /* host-visible, coherent, persistently mapped; CPU writes, GPU reads */
    k_gpu_memory_readback = 2, /* host-visible (cached preferred), persistently mapped; GPU writes */
};

/* Buffer use (bits). */
enum e_gpu_buffer_usage {
    k_gpu_buffer_transfer_src = 1u,
    k_gpu_buffer_transfer_dst = 2u,   /* added for k_gpu_memory_gpu */
    k_gpu_buffer_uniform = 4u,
    k_gpu_buffer_storage = 8u,        /* storage buffers; raw views (D3D12); device addresses (Vulkan) */
    k_gpu_buffer_index = 16u,
    k_gpu_buffer_vertex = 32u,
};

/* ---- images, samplers ---- */

enum e_gpu_view_type {
    k_gpu_view_none = 0,
    k_gpu_view_2d = 1,
    k_gpu_view_3d = 2,
    k_gpu_view_cube = 3,      /* array_layers 6 */
    k_gpu_view_2d_array = 4,
};

/* Image use (bits). 0 means sampled | transfer_dst. */
enum e_gpu_image_usage {
    k_gpu_image_transfer_src = 1u,
    k_gpu_image_transfer_dst = 2u,
    k_gpu_image_sampled = 4u,
    k_gpu_image_storage = 8u,
    k_gpu_image_color_attachment = 16u,
    k_gpu_image_depth_stencil_attachment = 32u,
};

/* A view's component source (GPU_SWIZZLE builds the four). */
enum e_gpu_swizzle {
    k_gpu_swizzle_identity = 0,
    k_gpu_swizzle_zero = 1,
    k_gpu_swizzle_one = 2,
    k_gpu_swizzle_r = 3,
    k_gpu_swizzle_g = 4,
    k_gpu_swizzle_b = 5,
    k_gpu_swizzle_a = 6,
};
#define GPU_SWIZZLE(r, g, b, a) ((uint32_t)(r) | (uint32_t)(g) << 8 | (uint32_t)(b) << 16 | (uint32_t)(a) << 24)

struct s_gpu_image_desc {
    uint32_t view_type;    /* e_gpu_view_type of the image's view */
    uint32_t format;       /* e_gpu_format */
    uint32_t width;
    uint32_t height;
    uint32_t depth;        /* 3D depth, 1 otherwise */
    uint32_t mip_levels;
    uint32_t array_layers; /* 6 for a cube */
    uint32_t usage;        /* e_gpu_image_usage bits; 0: sampled | transfer_dst */
    uint32_t swizzle;      /* the view's component mapping (GPU_SWIZZLE; 0 identity): a Xenos texture's D3DFORMAT
                              swizzle (AY8 XXXX, A8 000R, X8R8G8B8 RGB1) applied on the GPU */
};

/* One block of the data given to gpu_image_upload. Pitches of 0 mean tightly packed. */
struct s_gpu_image_region {
    uint32_t mip_level;
    uint32_t base_layer;
    uint32_t layer_count;
    uint32_t width;        /* texels of this level */
    uint32_t height;
    uint32_t depth;
    uint32_t offset;       /* byte offset in the data */
    uint32_t row_pitch;    /* bytes between rows of blocks */
    uint32_t slice_pitch;  /* bytes between depth slices and between layers */
};

enum e_gpu_filter {
    k_gpu_filter_nearest = 0,
    k_gpu_filter_linear = 1,
};

enum e_gpu_address_mode {
    k_gpu_address_repeat = 0,
    k_gpu_address_mirrored_repeat,
    k_gpu_address_clamp_to_edge,
    k_gpu_address_clamp_to_border,
    k_gpu_address_mirror_clamp_to_edge, /* caps.sampler_mirror_clamp_to_edge */
};

enum e_gpu_border_color {
    k_gpu_border_transparent_black = 0,
    k_gpu_border_opaque_black,
    k_gpu_border_opaque_white,
};

struct s_gpu_sampler_desc {
    uint32_t mag_filter;       /* e_gpu_filter */
    uint32_t min_filter;
    uint32_t mipmap_mode;      /* e_gpu_filter between levels */
    uint32_t address_u;        /* e_gpu_address_mode */
    uint32_t address_v;
    uint32_t address_w;
    float mip_lod_bias;
    float max_anisotropy;      /* <= 1: off; clamped to the device limit */
    uint32_t compare_enable;
    uint32_t compare_op;       /* e_gpu_compare_op */
    float min_lod;
    float max_lod;             /* 1000: all levels */
    uint32_t border_color;     /* e_gpu_border_color */
};

/* ---- bind groups ---- */

enum e_gpu_binding_type {
    k_gpu_binding_uniform_buffer = 0,
    k_gpu_binding_uniform_buffer_dynamic,  /* its offset given when the group is bound (D3D12: a root CBV) */
    k_gpu_binding_storage_buffer,
    k_gpu_binding_sampled_image,
    k_gpu_binding_storage_image,
    k_gpu_binding_sampler,
};

enum e_gpu_bind_lifetime {
    k_gpu_bind_persistent = 0,       /* until gpu_bind_group_free */
    k_gpu_bind_frame = 1,            /* recycled when the frame slot recording now is next begun */
    k_gpu_bind_frame_textures = 2,   /* k_gpu_bind_frame, from pools sized for the universal Xenos texture group */
    k_gpu_bind_cached_textures = 3,  /* Xenos texture groups kept across frames, until gpu_bind_groups_reset_cached
                                        (after which they must not be bound again: their pools go once the frames
                                        that bound them are done). The caller resets whenever gpu_view_epoch changed */
};

/* The universal Xenos layout (xenos_spirv_abi.h; see the top). */
struct s_gpu_xenos_layout {
    gpu_pipeline_layout layout;
    gpu_bind_layout constants;   /* bind group XE_SET_CONSTANTS: the blocks, dynamic uniform buffers 0..5 */
    gpu_bind_layout textures;    /* bind group XE_SET_TEXTURES: images 0..127, samplers 128..319 */
};

/* ---- shaders, pipelines ---- */

#define GPU_MAX_COLOR_ATTACHMENTS 4

struct s_gpu_stencil_face {
    uint32_t fail_op;          /* e_gpu_stencil_op */
    uint32_t pass_op;
    uint32_t depth_fail_op;
    uint32_t compare_op;       /* e_gpu_compare_op */
    uint32_t compare_mask;
    uint32_t write_mask;
    uint32_t reference;
};

struct s_gpu_blend_attachment {
    uint32_t enable;
    uint32_t src_color;        /* e_gpu_blend_factor */
    uint32_t dst_color;
    uint32_t color_op;         /* e_gpu_blend_op */
    uint32_t src_alpha;
    uint32_t dst_alpha;
    uint32_t alpha_op;
    uint32_t write_mask;       /* GPU_COLOR_WRITE_* */
};

/* Fixed-function state of a graphics pipeline. Viewport, scissor and vertex binding strides are dynamic. The run from
 * cull_mode to stencil_back is what a pipeline linked from library parts takes as dynamic state
 * (gpu_graphics_state_static_key clears it for such a pipeline's key). */
struct s_gpu_graphics_state {
    uint32_t topology;         /* e_gpu_topology */
    uint32_t primitive_restart;
    uint32_t polygon_mode;     /* e_gpu_polygon_mode */
    uint32_t cull_mode;        /* e_gpu_cull_mode */
    uint32_t front_face;       /* e_gpu_front_face */
    uint32_t depth_test;
    uint32_t depth_write;
    uint32_t depth_compare;    /* e_gpu_compare_op */
    uint32_t depth_bias_enable;
    float depth_bias_constant; /* in units of the depth format's resolution (2^-24 for 24-bit depth) */
    float depth_bias_slope;
    uint32_t stencil_test;
    struct s_gpu_stencil_face stencil_front;
    struct s_gpu_stencil_face stencil_back;
    uint32_t sample_count;     /* 1, 2, 4; 0: 1 */
    uint32_t alpha_to_coverage;
    uint32_t color_attachment_count;
    uint32_t color_formats[GPU_MAX_COLOR_ATTACHMENTS];        /* e_gpu_format */
    struct s_gpu_blend_attachment blend[GPU_MAX_COLOR_ATTACHMENTS];
    uint32_t depth_format;     /* e_gpu_format of the depth attachment, undefined for none */
    uint32_t stencil_format;   /* e_gpu_format of the stencil attachment, undefined for none */
    int32_t color_exp_bias[GPU_MAX_COLOR_ATTACHMENTS]; /* version 2 pack fragment shaders: color output i scaled by
                                                          2^bias (a Xenos render target's ColorExpBias) */
    int32_t fragcoord_offset[2];   /* version 2 pack fragment shaders: added to SV_Position x / y (the screen origin
                                      of a predicated tile) */
    float blend_constants[4];      /* R, G, B, A of the constant blend factors (zero when no attachment uses them) */
};

/* Opaque, triangle list, no culling (front faces clockwise, as D3D's), no depth or stencil test (compare less_equal
 * once enabled), one color attachment of color_format (or none for undefined) and depth_stencil_format as the depth
 * and (if it has stencil) the stencil attachment. */
void gpu_graphics_state_default(struct s_gpu_graphics_state *state, uint32_t color_format, uint32_t depth_stencil_format);

/* One shader stage of a pipeline made from stages. */
struct s_gpu_shader_stage {
    uint32_t stage;            /* e_gpu_stage: vertex, tess_control / _eval, geometry, fragment */
    gpu_shader module;
    const char *entry;         /* NULL: the module's own ("main") */
};

/* ---- offscreen render targets ---- */

struct s_gpu_render_target_info {
    uint32_t width;
    uint32_t height;
    uint32_t color_format;     /* e_gpu_format */
    uint32_t depth_format;     /* e_gpu_format, undefined without depth */
    uint32_t readback_row_pitch;
};

enum e_gpu_render_target_flags {
    k_gpu_render_target_depth = 1,   /* with the device's depth / stencil image */
    k_gpu_render_target_storage = 2, /* the color image is also a storage image (the format must allow it) */
};

/* ---- commands ---- */

enum e_gpu_clear_flags {
    k_gpu_clear_color = 1,
    k_gpu_clear_depth = 2,
    k_gpu_clear_stencil = 4,
};

struct s_gpu_clear {
    uint32_t flags;            /* e_gpu_clear_flags; attachments not cleared are loaded */
    uint32_t color_mask;       /* the color attachments k_gpu_clear_color clears (bit i: attachment i; D3DCLEAR_TARGET0..3) */
    float color[4];
    float depth;
    uint32_t stencil;
};

/* One rectangle of gpu_cmd_copy_rects, in texels of both images. */
struct s_gpu_copy_rect {
    int32_t source_x, source_y, destination_x, destination_y;
    uint32_t width, height;
};

/* gpu_cmd_apply_display_gamma's ramp (display_gamma.hlsl): uint4 header (x: 0 none, 1 a 256-entry D3DGAMMARAMP, 2 a
 * 128-segment D3DPWLGAMMA), then uint4 table[256] (red / green / blue in .xyz: ramp words, or PWL segments as
 * Base | Delta << 16). */
#define GPU_DISPLAY_GAMMA_RAMP_SIZE (16u * 257u)
/* gpu_cmd_scale_image_rect's parameters (resolve_scale.hlsl): int4 { x, y, width, height }, float4 { factor, clamp, low,
 * high }. */
#define GPU_SCALE_IMAGE_PARAMETERS_SIZE 32u

/* ---- the operations ---- */

struct s_gpu_backend_ops {
    /* backend */
    void (*destroy)(s_gpu_backend *gpu);
    gpu_result (*wait_idle)(s_gpu_backend *gpu);
    gpu_result (*get_pipeline_cache)(s_gpu_backend *gpu, void *data, size_t *size);
    uint32_t (*pipelines_created)(const s_gpu_backend *gpu);
    uint32_t (*frame_index)(const s_gpu_backend *gpu);
    uint32_t (*view_epoch)(const s_gpu_backend *gpu);
    void (*get_stats)(const s_gpu_backend *gpu, struct s_gpu_stats *out);
    int (*rendering)(const s_gpu_backend *gpu, uint32_t *out_width, uint32_t *out_height);
    int (*occlusion_active)(const s_gpu_backend *gpu);
    void (*set_checkpoint_describe)(s_gpu_backend *gpu, gpu_checkpoint_describe_function describe, void *context);
    uint32_t (*format_storage)(s_gpu_backend *gpu, uint32_t format);
    uint32_t (*format_blendable_filterable)(s_gpu_backend *gpu, uint32_t format);

    /* buffers */
    s_gpu_buffer *(*buffer_create)(s_gpu_backend *gpu, uint32_t size, uint32_t usage, uint32_t memory_usage);
    void (*buffer_destroy)(s_gpu_backend *gpu, s_gpu_buffer *buffer);
    void (*buffer_retire)(s_gpu_backend *gpu, s_gpu_buffer *buffer);
    void *(*buffer_mapped)(const s_gpu_backend *gpu, const s_gpu_buffer *buffer);
    void (*buffer_invalidate)(s_gpu_backend *gpu, s_gpu_buffer *buffer);
    gpu_result (*buffer_download)(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, void *data, uint32_t size);
    gpu_result (*buffer_upload)(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, const void *data,
                                uint32_t size);
    void (*buffer_device_address)(const s_gpu_backend *gpu, const s_gpu_buffer *buffer, uint32_t *out_lo,
                                  uint32_t *out_hi);

    /* images, samplers */
    s_gpu_image *(*image_create)(s_gpu_backend *gpu, const struct s_gpu_image_desc *desc);
    void (*image_destroy)(s_gpu_backend *gpu, s_gpu_image *image);
    void (*image_retire)(s_gpu_backend *gpu, s_gpu_image *image);
    void (*image_get_desc)(const s_gpu_backend *gpu, const s_gpu_image *image, struct s_gpu_image_desc *out);
    gpu_view (*image_view)(const s_gpu_backend *gpu, const s_gpu_image *image);
    gpu_view (*image_sampled_view)(const s_gpu_backend *gpu, const s_gpu_image *image);
    gpu_view (*image_array_view)(s_gpu_backend *gpu, s_gpu_image *image);
    gpu_view (*image_swizzled_view)(s_gpu_backend *gpu, s_gpu_image *image, uint32_t swizzle);
    gpu_result (*image_upload)(s_gpu_backend *gpu, s_gpu_image *image, const struct s_gpu_image_region *regions,
                               uint32_t region_count, const void *data, uint32_t size);
    gpu_sampler (*sampler_create)(s_gpu_backend *gpu, const struct s_gpu_sampler_desc *desc);
    void (*sampler_destroy)(s_gpu_backend *gpu, gpu_sampler sampler);

    /* shader pack modules and pipelines */
    gpu_shader (*pack_shader_module)(s_gpu_backend *gpu, struct s_shader_pack *pack, uint32_t module_index,
                                     const char **out_entry_point);
    gpu_shader (*pack_xenos_module)(s_gpu_backend *gpu, struct s_shader_pack *pack, uint32_t module_index);
    void (*pack_release)(s_gpu_backend *gpu);
    s_gpu_pipeline *(*pack_graphics_pipeline)(s_gpu_backend *gpu, struct s_shader_pack *pack, uint32_t vertex_shader,
                                              uint32_t pixel_shader, const struct s_xenos_vertex_declaration *declaration,
                                              const struct s_gpu_graphics_state *state);
    s_gpu_pipeline *(*pack_compute_pipeline)(s_gpu_backend *gpu, struct s_shader_pack *pack, uint32_t compute_shader);
    int (*xenos_layout)(s_gpu_backend *gpu, struct s_shader_pack *pack, struct s_gpu_xenos_layout *out);
    s_gpu_pipeline *(*graphics_pipeline_create_stages)(s_gpu_backend *gpu, const struct s_gpu_shader_stage *stages,
                                                       uint32_t stage_count, gpu_pipeline_layout layout,
                                                       const struct s_gpu_graphics_state *state,
                                                       uint32_t patch_control_points);
    gpu_pipeline_handle (*compute_pipeline_create_handle)(s_gpu_backend *gpu, gpu_shader module, const char *entry,
                                                          gpu_pipeline_layout layout);
    s_gpu_pipeline *(*pipeline_adopt)(s_gpu_backend *gpu, gpu_pipeline_handle handle, gpu_pipeline_layout layout,
                                      uint32_t bind_point);
    void (*pipeline_destroy)(s_gpu_backend *gpu, s_gpu_pipeline *pipeline);
    gpu_bind_layout (*pipeline_bind_layout)(const s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, uint32_t set);
    uint32_t (*pipeline_vertex_streams)(const s_gpu_backend *gpu, const s_gpu_pipeline *pipeline);
    gpu_pipeline_handle (*pipeline_library_vertex_input)(s_gpu_backend *gpu, const struct s_gpu_graphics_state *state,
                                                         uint32_t patch_control_points);
    gpu_pipeline_handle (*pipeline_library_pre_rasterization)(s_gpu_backend *gpu, const struct s_gpu_shader_stage *stages,
                                                              uint32_t stage_count, gpu_pipeline_layout layout,
                                                              const struct s_gpu_graphics_state *state,
                                                              uint32_t patch_control_points);
    gpu_pipeline_handle (*pipeline_library_fragment_shader)(s_gpu_backend *gpu, gpu_shader fragment,
                                                            gpu_pipeline_layout layout, uint32_t alpha_test_enable);
    gpu_pipeline_handle (*pipeline_library_fragment_output)(s_gpu_backend *gpu, const struct s_gpu_graphics_state *state);
    void (*pipeline_library_destroy)(s_gpu_backend *gpu, gpu_pipeline_handle library);
    s_gpu_pipeline *(*pipeline_link)(s_gpu_backend *gpu, const gpu_pipeline_handle libraries[4], gpu_pipeline_layout layout,
                                     const struct s_gpu_graphics_state *state, uint32_t optimize);
    gpu_pipeline_handle (*pipeline_link_handle)(s_gpu_backend *gpu, const gpu_pipeline_handle libraries[4],
                                                gpu_pipeline_layout layout, uint32_t optimize);
    void (*pipeline_replace)(s_gpu_backend *gpu, s_gpu_pipeline *pipeline, gpu_pipeline_handle handle);
    void (*graphics_state_static_key)(const s_gpu_backend *gpu, const struct s_gpu_graphics_state *state,
                                      struct s_gpu_graphics_state *out);

    /* bind groups */
    gpu_bind_group (*bind_group_allocate)(s_gpu_backend *gpu, gpu_bind_layout layout, uint32_t lifetime);
    void (*bind_group_free)(s_gpu_backend *gpu, gpu_bind_group group);
    void (*bind_groups_reset_cached)(s_gpu_backend *gpu);
    void (*bind_write_buffer)(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                              uint32_t type, s_gpu_buffer *buffer, uint32_t offset, uint32_t range);
    void (*bind_write_image)(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                             uint32_t type, gpu_view view);
    void (*bind_write_sampler)(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                               gpu_sampler sampler);
    void (*bind_write_textures)(s_gpu_backend *gpu, gpu_bind_group group, uint32_t count, const uint32_t *binding,
                                const uint64_t *handle, uint32_t first_sampler);

    /* frames */
    gpu_result (*frame_begin)(s_gpu_backend *gpu);
    gpu_result (*frame_end)(s_gpu_backend *gpu, uint32_t wait);
    gpu_result (*frame_wait)(s_gpu_backend *gpu, uint32_t slot);

    /* offscreen render targets */
    s_gpu_render_target *(*render_target_create)(s_gpu_backend *gpu, uint32_t width, uint32_t height,
                                                 uint32_t color_format, uint32_t flags);
    void (*render_target_destroy)(s_gpu_backend *gpu, s_gpu_render_target *target);
    void (*render_target_get_info)(const s_gpu_backend *gpu, const s_gpu_render_target *target,
                                   struct s_gpu_render_target_info *out);
    s_gpu_image *(*render_target_color_image)(s_gpu_backend *gpu, s_gpu_render_target *target);
    const void *(*render_target_read)(s_gpu_backend *gpu, s_gpu_render_target *target);

    /* commands */
    void (*cmd_begin_rendering)(s_gpu_backend *gpu, s_gpu_image *const *colors, uint32_t color_count, s_gpu_image *depth,
                                const struct s_gpu_clear *clear);
    void (*cmd_end_rendering)(s_gpu_backend *gpu);
    void (*cmd_clear_attachments)(s_gpu_backend *gpu, const struct s_gpu_clear *clear, const int32_t *rects,
                                  uint32_t rect_count);
    void (*cmd_copy_image)(s_gpu_backend *gpu, s_gpu_image *source, int32_t source_x, int32_t source_y, uint32_t width,
                           uint32_t height, s_gpu_image *destination, uint32_t destination_level,
                           uint32_t destination_layer, int32_t destination_x, int32_t destination_y,
                           uint32_t destination_width, uint32_t destination_height);
    void (*cmd_copy_depth)(s_gpu_backend *gpu, s_gpu_image *source, int32_t source_x, int32_t source_y, uint32_t width,
                           uint32_t height, s_gpu_image *destination, int32_t destination_x, int32_t destination_y);
    void (*cmd_copy_rects)(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                           const struct s_gpu_copy_rect *rects, uint32_t count);
    void (*cmd_set_viewport)(s_gpu_backend *gpu, float x, float y, float width, float height, float min_depth,
                             float max_depth);
    void (*cmd_set_scissor)(s_gpu_backend *gpu, int32_t x, int32_t y, uint32_t width, uint32_t height);
    void (*cmd_bind_pipeline)(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline,
                              const struct s_gpu_graphics_state *state);
    void (*cmd_bind_group)(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, uint32_t set, gpu_bind_group group,
                           uint32_t offset_count, const uint32_t *offsets);
    void (*cmd_bind_group_pair)(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, gpu_bind_group group0,
                                uint32_t offset_count0, const uint32_t *offsets0, gpu_bind_group group1);
    void (*cmd_bind_vertex_buffer)(s_gpu_backend *gpu, uint32_t binding, const s_gpu_buffer *buffer, uint32_t offset,
                                   uint32_t stride);
    void (*cmd_bind_index_buffer)(s_gpu_backend *gpu, const s_gpu_buffer *buffer, uint32_t offset, uint32_t index_type);
    void (*cmd_xenos_bind_memory)(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, s_gpu_buffer *const *streams,
                                  s_gpu_buffer *const *exports);
    void (*cmd_draw)(s_gpu_backend *gpu, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                     uint32_t first_instance);
    void (*cmd_draw_indexed)(s_gpu_backend *gpu, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                             int32_t vertex_offset, uint32_t first_instance);
    void (*cmd_dispatch)(s_gpu_backend *gpu, uint32_t groups_x, uint32_t groups_y, uint32_t groups_z);
    void (*cmd_memory_barrier)(s_gpu_backend *gpu);
    void (*cmd_copy_buffer)(s_gpu_backend *gpu, s_gpu_buffer *source, uint32_t source_offset, s_gpu_buffer *destination,
                            uint32_t destination_offset, uint32_t size);
    void (*cmd_copy_buffer_to_image)(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, uint32_t row_length,
                                     s_gpu_image *image, uint32_t width, uint32_t height);
    void (*cmd_copy_image_to_buffer)(s_gpu_backend *gpu, s_gpu_image *image, uint32_t level, uint32_t layer, int32_t x,
                                     int32_t y, uint32_t width, uint32_t height, s_gpu_buffer *buffer, uint32_t offset);
    void (*cmd_readback)(s_gpu_backend *gpu, s_gpu_render_target *target);
    void (*cmd_checkpoint)(s_gpu_backend *gpu, const void *marker);

    /* the port's compute helper passes (helper_binding.hlsli) */
    void (*cmd_reinterpret_7e3)(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination);
    uint32_t (*cmd_convert_pwl)(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, uint32_t encode);
    uint32_t (*cmd_downscale_image)(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination);
    uint32_t (*cmd_pack_depth_stencil)(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                       uint32_t float_depth);
    void (*cmd_apply_display_gamma)(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                    s_gpu_buffer *ramp, uint32_t ramp_offset);
    uint32_t (*cmd_scale_image_rect)(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                     s_gpu_buffer *parameters, uint32_t parameters_offset, uint32_t width,
                                     uint32_t height);

    /* occlusion queries */
    int (*occlusion_begin)(s_gpu_backend *gpu, uint32_t *out_serial, uint32_t *out_record);
    void (*occlusion_end)(s_gpu_backend *gpu);
    int (*occlusion_result)(s_gpu_backend *gpu, uint32_t serial, uint32_t record, uint64_t *out_samples);

    /* presentation */
    gpu_result (*surface_create)(s_gpu_backend *gpu, uint32_t window_system, void *display, uintptr_t window);
    void (*surface_destroy)(s_gpu_backend *gpu);
    gpu_result (*swapchain_create)(s_gpu_backend *gpu, uint32_t width, uint32_t height, uint32_t vsync);
    void (*swapchain_destroy)(s_gpu_backend *gpu);
    gpu_result (*swapchain_acquire)(s_gpu_backend *gpu, uint32_t *out_image_index);
    void (*cmd_blit_to_swapchain)(s_gpu_backend *gpu, s_gpu_render_target *target);
    gpu_result (*swapchain_present)(s_gpu_backend *gpu);
};

/* A backend: its operations and capabilities, followed by its own state (each backend's object begins with this). */
struct s_gpu_backend {
    const struct s_gpu_backend_ops *ops;
    struct s_gpu_caps caps;
    /* Direct pointers to backend hot state for zero-overhead inline accessors (NULL falls back to ops). */
    const uint32_t *frame_index_ptr;
    const uint32_t *rendering_ptr;
    const uint32_t *rendering_width_ptr;
    const uint32_t *rendering_height_ptr;
};

/* ---- selection (gpu_backend.cpp) ---- */

enum e_gpu_renderer {
    k_gpu_renderer_auto = 0,   /* the platform's default: Vulkan (Linux; Windows with Direct3D 12 when it cannot start) */
    k_gpu_renderer_vulkan = 1,
    k_gpu_renderer_d3d12 = 2,
};

/* The renderer the launch asks for: HALO3_RENDERER (auto | vulkan | d3d12; port_config fills it from halo3.ini's
 * [display] renderer when the launch did not set it), auto when unset. A value it cannot take is reported and taken as
 * auto. */
uint32_t gpu_renderer_requested(void);
/* Whether a renderer that cannot start may fall back to the other: HALO3_RENDERER_FALLBACK=0 says no (the renderer
 * then stays off with the reason logged, as when no backend can start); anything else, or unset, yes. */
int gpu_renderer_fallback_allowed(void);
/* The backends to try for a request, best first, in out_kinds (e_gpu_backend_kind; at most 2). A request for a backend
 * this build or platform does not have logs why and is replaced by the default unless fallback is off (then nothing).
 * Returns the count. */
uint32_t gpu_backend_candidates(uint32_t request, uint32_t *out_kinds);
/* The pipeline cache data file of a backend kind ("pipeline_cache.bin" for Vulkan: its blob is the driver's). */
const char *gpu_backend_pipeline_cache_file(uint32_t kind);
/* The shader pack a backend kind draws with when the launch names one for it alone (HALO3_SHADER_PACK_DXIL for
 * Direct3D 12, which port_config sets to halo3_shaders_dxil.pack when it finds one), else NULL: shader_pack_open's own
 * choice (HALO3_SHADER_PACK, then the default files). */
const char *gpu_backend_shader_pack(uint32_t kind);
/* The log name of a backend kind ("vulkan", "d3d12"). */
const char *gpu_backend_name(uint32_t kind);
/* Makes a backend of `kind`. k_gpu_unsupported when this build or platform has none of that kind. */
gpu_result gpu_backend_create(uint32_t kind, const struct s_gpu_backend_create_info *info, s_gpu_backend **out_backend);

/* The backends' own creators (gpu_backend.cpp's registry). */
gpu_result vulkan_gpu_backend_create(const struct s_gpu_backend_create_info *info, s_gpu_backend **out_backend);
#ifdef HALO3_64BIT
/* Fast path for a 4x EDRAM depth view. Returns 0 when the caller should use rectangle copies. */
uint32_t gpu_cmd_gather_depth(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                             s_gpu_buffer *parameters, uint32_t offset, uint32_t width, uint32_t height);
#endif
gpu_result d3d12_gpu_backend_create(const struct s_gpu_backend_create_info *info, s_gpu_backend **out_backend);

#ifdef __cplusplus
}
#endif

/* ---- calls (the frontend's spelling of the operations) ---- */

#ifdef __cplusplus
#define GPU_INLINE inline
#else
#define GPU_INLINE static inline
#endif

GPU_INLINE void gpu_backend_destroy(s_gpu_backend *gpu) { if (gpu != 0) gpu->ops->destroy(gpu); }
GPU_INLINE const struct s_gpu_caps *gpu_caps(const s_gpu_backend *gpu) { return &gpu->caps; }
GPU_INLINE gpu_result gpu_wait_idle(s_gpu_backend *gpu) { return gpu->ops->wait_idle(gpu); }
GPU_INLINE gpu_result gpu_get_pipeline_cache(s_gpu_backend *gpu, void *data, size_t *size)
{ return gpu->ops->get_pipeline_cache(gpu, data, size); }
GPU_INLINE uint32_t gpu_pipelines_created(const s_gpu_backend *gpu) { return gpu->ops->pipelines_created(gpu); }
GPU_INLINE uint32_t gpu_frame_index(const s_gpu_backend *gpu)
{ return gpu->frame_index_ptr != 0 ? *gpu->frame_index_ptr : gpu->ops->frame_index(gpu); }
GPU_INLINE uint32_t gpu_view_epoch(const s_gpu_backend *gpu) { return gpu->ops->view_epoch(gpu); }
GPU_INLINE void gpu_get_stats(const s_gpu_backend *gpu, struct s_gpu_stats *out) { gpu->ops->get_stats(gpu, out); }
/* 1 inside a rendering (gpu_cmd_begin_rendering .. end), with its size. */
GPU_INLINE int gpu_rendering(const s_gpu_backend *gpu, uint32_t *out_width, uint32_t *out_height)
{
    if (gpu->rendering_ptr != 0) {
        if (out_width != 0)
            *out_width = *gpu->rendering_width_ptr;
        if (out_height != 0)
            *out_height = *gpu->rendering_height_ptr;
        return *gpu->rendering_ptr != 0 ? 1 : 0;
    }
    return gpu->ops->rendering(gpu, out_width, out_height);
}
GPU_INLINE int gpu_occlusion_active(const s_gpu_backend *gpu) { return gpu->ops->occlusion_active(gpu); }
GPU_INLINE void gpu_set_checkpoint_describe(s_gpu_backend *gpu, gpu_checkpoint_describe_function describe, void *context)
{ gpu->ops->set_checkpoint_describe(gpu, describe, context); }
/* Whether images of the format can be storage images. */
GPU_INLINE uint32_t gpu_format_storage(s_gpu_backend *gpu, uint32_t format) { return gpu->ops->format_storage(gpu, format); }
/* Whether images of the format can be blended color attachments and linearly filtered. */
GPU_INLINE uint32_t gpu_format_blendable_filterable(s_gpu_backend *gpu, uint32_t format)
{ return gpu->ops->format_blendable_filterable(gpu, format); }

/* Buffers. usage: e_gpu_buffer_usage bits; memory_usage: e_gpu_memory_usage. retire: destroyed once no frame can use it
 * (any buffer a recorded command may reference). mapped: the persistent mapping of an upload or readback buffer (NULL
 * for k_gpu_memory_gpu). download / upload: through the mapping when host-visible, else through a staging buffer and a
 * copy submitted and waited for at once (a frame still being recorded is not seen). device_address: the 64-bit
 * address as two halves (0, 0 without caps.buffer_device_address). */
GPU_INLINE s_gpu_buffer *gpu_buffer_create(s_gpu_backend *gpu, uint32_t size, uint32_t usage, uint32_t memory_usage)
{ return gpu->ops->buffer_create(gpu, size, usage, memory_usage); }
GPU_INLINE void gpu_buffer_destroy(s_gpu_backend *gpu, s_gpu_buffer *buffer) { gpu->ops->buffer_destroy(gpu, buffer); }
GPU_INLINE void gpu_buffer_retire(s_gpu_backend *gpu, s_gpu_buffer *buffer) { gpu->ops->buffer_retire(gpu, buffer); }
GPU_INLINE void *gpu_buffer_mapped(const s_gpu_backend *gpu, const s_gpu_buffer *buffer)
{ return gpu->ops->buffer_mapped(gpu, buffer); }
GPU_INLINE void gpu_buffer_invalidate(s_gpu_backend *gpu, s_gpu_buffer *buffer) { gpu->ops->buffer_invalidate(gpu, buffer); }
GPU_INLINE gpu_result gpu_buffer_download(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, void *data,
                                          uint32_t size)
{ return gpu->ops->buffer_download(gpu, buffer, offset, data, size); }
GPU_INLINE gpu_result gpu_buffer_upload(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, const void *data,
                                        uint32_t size)
{ return gpu->ops->buffer_upload(gpu, buffer, offset, data, size); }
GPU_INLINE void gpu_buffer_device_address(const s_gpu_backend *gpu, const s_gpu_buffer *buffer, uint32_t *out_lo,
                                          uint32_t *out_hi)
{ gpu->ops->buffer_device_address(gpu, buffer, out_lo, out_hi); }

/* Images. image_view: the view of the image's own type and swizzle; sampled_view: that, except a depth / stencil
 * image's depth alone; array_view: a 2D array view of a 2D image (tfetch3D of stacked 2D textures); swizzled_view: one
 * other swizzle for the image's life (a storage image sampled through a texture header's swizzle; a second is refused).
 * upload: through a staging copy submitted and waited for at once; the image is left ready for sampling. */
GPU_INLINE s_gpu_image *gpu_image_create(s_gpu_backend *gpu, const struct s_gpu_image_desc *desc)
{ return gpu->ops->image_create(gpu, desc); }
GPU_INLINE void gpu_image_destroy(s_gpu_backend *gpu, s_gpu_image *image) { gpu->ops->image_destroy(gpu, image); }
GPU_INLINE void gpu_image_retire(s_gpu_backend *gpu, s_gpu_image *image) { gpu->ops->image_retire(gpu, image); }
GPU_INLINE void gpu_image_get_desc(const s_gpu_backend *gpu, const s_gpu_image *image, struct s_gpu_image_desc *out)
{ gpu->ops->image_get_desc(gpu, image, out); }
GPU_INLINE gpu_view gpu_image_view(const s_gpu_backend *gpu, const s_gpu_image *image)
{ return gpu->ops->image_view(gpu, image); }
GPU_INLINE gpu_view gpu_image_sampled_view(const s_gpu_backend *gpu, const s_gpu_image *image)
{ return gpu->ops->image_sampled_view(gpu, image); }
GPU_INLINE gpu_view gpu_image_array_view(s_gpu_backend *gpu, s_gpu_image *image) { return gpu->ops->image_array_view(gpu, image); }
GPU_INLINE gpu_view gpu_image_swizzled_view(s_gpu_backend *gpu, s_gpu_image *image, uint32_t swizzle)
{ return gpu->ops->image_swizzled_view(gpu, image, swizzle); }
GPU_INLINE gpu_result gpu_image_upload(s_gpu_backend *gpu, s_gpu_image *image, const struct s_gpu_image_region *regions,
                                       uint32_t region_count, const void *data, uint32_t size)
{ return gpu->ops->image_upload(gpu, image, regions, region_count, data, size); }
GPU_INLINE gpu_sampler gpu_sampler_create(s_gpu_backend *gpu, const struct s_gpu_sampler_desc *desc)
{ return gpu->ops->sampler_create(gpu, desc); }
GPU_INLINE void gpu_sampler_destroy(s_gpu_backend *gpu, gpu_sampler sampler) { gpu->ops->sampler_destroy(gpu, sampler); }

/* Shader pack modules: created on first use and cached until gpu_pack_release (one pack at a time). pack_shader_module:
 * a version 2 module (its entry point name in *out_entry_point); pack_xenos_module: a version 3 / 4 module (the
 * backend's module format, caps.shader_module_format). Not thread-safe: the prewarm workers call them under the
 * frontend's lock. */
GPU_INLINE gpu_shader gpu_pack_shader_module(s_gpu_backend *gpu, struct s_shader_pack *pack, uint32_t module_index,
                                             const char **out_entry_point)
{ return gpu->ops->pack_shader_module(gpu, pack, module_index, out_entry_point); }
GPU_INLINE gpu_shader gpu_pack_xenos_module(s_gpu_backend *gpu, struct s_shader_pack *pack, uint32_t module_index)
{ return gpu->ops->pack_xenos_module(gpu, pack, module_index); }
GPU_INLINE void gpu_pack_release(s_gpu_backend *gpu) { gpu->ops->pack_release(gpu); }
/* Version 2 pack pipelines: a vertex and a pixel shader with the pack's reflected layout (each shader's bindings in
 * its own bind group: vertex 0, pixel 1) and the declaration's vertex input (NULL: the vertex shader's vertex type
 * chooses one); a compute shader (bind group 0). */
GPU_INLINE s_gpu_pipeline *gpu_pack_graphics_pipeline(s_gpu_backend *gpu, struct s_shader_pack *pack,
                                                      uint32_t vertex_shader, uint32_t pixel_shader,
                                                      const struct s_xenos_vertex_declaration *declaration,
                                                      const struct s_gpu_graphics_state *state)
{ return gpu->ops->pack_graphics_pipeline(gpu, pack, vertex_shader, pixel_shader, declaration, state); }
GPU_INLINE s_gpu_pipeline *gpu_pack_compute_pipeline(s_gpu_backend *gpu, struct s_shader_pack *pack,
                                                     uint32_t compute_shader)
{ return gpu->ops->pack_compute_pipeline(gpu, pack, compute_shader); }
/* The universal Xenos layout (see the top), made once and owned by the backend. 0 (the reason logged) when the
 * device cannot hold it. */
GPU_INLINE int gpu_xenos_layout(s_gpu_backend *gpu, struct s_shader_pack *pack, struct s_gpu_xenos_layout *out)
{ return gpu->ops->xenos_layout(gpu, pack, out); }
/* A graphics pipeline of translated Xenos shaders from any stages (the vertex stage first; no fragment stage: depth
 * only), with no vertex input (the shaders fetch their own vertices). A tessellation control stage makes the topology
 * a patch list of patch_control_points. state->color_exp_bias and fragcoord_offset are not used. */
GPU_INLINE s_gpu_pipeline *gpu_graphics_pipeline_create_stages(s_gpu_backend *gpu, const struct s_gpu_shader_stage *stages,
                                                               uint32_t stage_count, gpu_pipeline_layout layout,
                                                               const struct s_gpu_graphics_state *state,
                                                               uint32_t patch_control_points)
{ return gpu->ops->graphics_pipeline_create_stages(gpu, stages, stage_count, layout, state, patch_control_points); }
/* A compute pipeline as a bare handle (any thread: the prewarm), and a bare handle made into a pipeline the caller
 * owns (bind_point: e_gpu_bind_point). */
GPU_INLINE gpu_pipeline_handle gpu_compute_pipeline_create_handle(s_gpu_backend *gpu, gpu_shader module, const char *entry,
                                                                  gpu_pipeline_layout layout)
{ return gpu->ops->compute_pipeline_create_handle(gpu, module, entry, layout); }
GPU_INLINE s_gpu_pipeline *gpu_pipeline_adopt(s_gpu_backend *gpu, gpu_pipeline_handle handle, gpu_pipeline_layout layout,
                                              uint32_t bind_point)
{ return gpu->ops->pipeline_adopt(gpu, handle, layout, bind_point); }
GPU_INLINE void gpu_pipeline_destroy(s_gpu_backend *gpu, s_gpu_pipeline *pipeline) { gpu->ops->pipeline_destroy(gpu, pipeline); }
/* The bind layout of bind group `set` of a pipeline's layout (GPU_NULL_HANDLE past its groups). */
GPU_INLINE gpu_bind_layout gpu_pipeline_bind_layout(const s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, uint32_t set)
{ return gpu->ops->pipeline_bind_layout(gpu, pipeline, set); }
/* The vertex streams a graphics pipeline of the pack (gpu_pack_graphics_pipeline) reads: bit s for stream s (its vertex
 * input's bindings, the null binding excluded); 0 for compute and Xenos pipelines (they fetch their own vertices). */
GPU_INLINE uint32_t gpu_pipeline_vertex_streams(const s_gpu_backend *gpu, const s_gpu_pipeline *pipeline)
{ return gpu->ops->pipeline_vertex_streams(gpu, pipeline); }
/* Pipeline library parts (caps.pipeline_library): a Xenos graphics pipeline linked from four parts, each made once and
 * shared: vertex input (topology, restart), pre-rasterization (the vertex stages, polygon mode), fragment shader (none:
 * depth only), fragment output (formats, blending, samples). Any thread. A linked pipeline holds the dynamic state of
 * its s_gpu_graphics_state and sets it when bound (gpu_cmd_bind_pipeline sets the draw's state instead), so one link
 * serves every state that differs only there (gpu_graphics_state_static_key). A fast link can be linked again
 * optimized (pipeline_link_handle) and swapped in (pipeline_replace: the old handle goes once the frame is done). */
GPU_INLINE gpu_pipeline_handle gpu_pipeline_library_vertex_input(s_gpu_backend *gpu, const struct s_gpu_graphics_state *state,
                                                                 uint32_t patch_control_points)
{ return gpu->ops->pipeline_library_vertex_input(gpu, state, patch_control_points); }
GPU_INLINE gpu_pipeline_handle gpu_pipeline_library_pre_rasterization(s_gpu_backend *gpu,
                                                                      const struct s_gpu_shader_stage *stages,
                                                                      uint32_t stage_count, gpu_pipeline_layout layout,
                                                                      const struct s_gpu_graphics_state *state,
                                                                      uint32_t patch_control_points)
{ return gpu->ops->pipeline_library_pre_rasterization(gpu, stages, stage_count, layout, state, patch_control_points); }
GPU_INLINE gpu_pipeline_handle gpu_pipeline_library_fragment_shader(s_gpu_backend *gpu, gpu_shader fragment,
                                                                    gpu_pipeline_layout layout,
                                                                    uint32_t alpha_test_enable)
{ return gpu->ops->pipeline_library_fragment_shader(gpu, fragment, layout, alpha_test_enable); }
GPU_INLINE gpu_pipeline_handle gpu_pipeline_library_fragment_output(s_gpu_backend *gpu,
                                                                    const struct s_gpu_graphics_state *state)
{ return gpu->ops->pipeline_library_fragment_output(gpu, state); }
GPU_INLINE void gpu_pipeline_library_destroy(s_gpu_backend *gpu, gpu_pipeline_handle library)
{ gpu->ops->pipeline_library_destroy(gpu, library); }
GPU_INLINE s_gpu_pipeline *gpu_pipeline_link(s_gpu_backend *gpu, const gpu_pipeline_handle libraries[4],
                                             gpu_pipeline_layout layout, const struct s_gpu_graphics_state *state,
                                             uint32_t optimize)
{ return gpu->ops->pipeline_link(gpu, libraries, layout, state, optimize); }
GPU_INLINE gpu_pipeline_handle gpu_pipeline_link_handle(s_gpu_backend *gpu, const gpu_pipeline_handle libraries[4],
                                                        gpu_pipeline_layout layout, uint32_t optimize)
{ return gpu->ops->pipeline_link_handle(gpu, libraries, layout, optimize); }
GPU_INLINE void gpu_pipeline_replace(s_gpu_backend *gpu, s_gpu_pipeline *pipeline, gpu_pipeline_handle handle)
{ gpu->ops->pipeline_replace(gpu, pipeline, handle); }
GPU_INLINE void gpu_graphics_state_static_key(const s_gpu_backend *gpu, const struct s_gpu_graphics_state *state,
                                              struct s_gpu_graphics_state *out)
{ gpu->ops->graphics_state_static_key(gpu, state, out); }

/* Bind groups (lifetime: e_gpu_bind_lifetime). write_buffer: type uniform_buffer(_dynamic) or storage_buffer, range 0
 * to the end of the buffer; write_image: sampled_image (read as sampled) or storage_image; write_textures: `count`
 * descriptors at once, binding[i] below first_sampler a sampled image (handle[i] its gpu_view), from first_sampler on a
 * sampler (handle[i] its gpu_sampler). */
GPU_INLINE gpu_bind_group gpu_bind_group_allocate(s_gpu_backend *gpu, gpu_bind_layout layout, uint32_t lifetime)
{ return gpu->ops->bind_group_allocate(gpu, layout, lifetime); }
GPU_INLINE void gpu_bind_group_free(s_gpu_backend *gpu, gpu_bind_group group) { gpu->ops->bind_group_free(gpu, group); }
GPU_INLINE void gpu_bind_groups_reset_cached(s_gpu_backend *gpu) { gpu->ops->bind_groups_reset_cached(gpu); }
GPU_INLINE void gpu_bind_write_buffer(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                                      uint32_t type, s_gpu_buffer *buffer, uint32_t offset, uint32_t range)
{ gpu->ops->bind_write_buffer(gpu, group, binding, array_element, type, buffer, offset, range); }
GPU_INLINE void gpu_bind_write_image(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                                     uint32_t type, gpu_view view)
{ gpu->ops->bind_write_image(gpu, group, binding, array_element, type, view); }
GPU_INLINE void gpu_bind_write_sampler(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                                       gpu_sampler sampler)
{ gpu->ops->bind_write_sampler(gpu, group, binding, array_element, sampler); }
GPU_INLINE void gpu_bind_write_textures(s_gpu_backend *gpu, gpu_bind_group group, uint32_t count, const uint32_t *binding,
                                        const uint64_t *handle, uint32_t first_sampler)
{ gpu->ops->bind_write_textures(gpu, group, count, binding, handle, first_sampler); }

/* Frames: begin waits for the slot's previous submission and recycles its frame resources; end submits (wait: and
 * waits for the GPU); frame_wait waits for the frame last recorded in a slot (k_gpu_not_ready: the slot being
 * recorded). */
GPU_INLINE gpu_result gpu_frame_begin(s_gpu_backend *gpu) { return gpu->ops->frame_begin(gpu); }
GPU_INLINE gpu_result gpu_frame_end(s_gpu_backend *gpu, uint32_t wait) { return gpu->ops->frame_end(gpu, wait); }
GPU_INLINE gpu_result gpu_frame_wait(s_gpu_backend *gpu, uint32_t slot) { return gpu->ops->frame_wait(gpu, slot); }

/* Offscreen render targets: a color image (also sampled and copyable), optionally the device's depth image
 * (flags: e_gpu_render_target_flags), and a host-visible readback buffer (gpu_cmd_readback; read once the frame has
 * finished). */
GPU_INLINE s_gpu_render_target *gpu_render_target_create(s_gpu_backend *gpu, uint32_t width, uint32_t height,
                                                         uint32_t color_format, uint32_t flags)
{ return gpu->ops->render_target_create(gpu, width, height, color_format, flags); }
GPU_INLINE void gpu_render_target_destroy(s_gpu_backend *gpu, s_gpu_render_target *target)
{ gpu->ops->render_target_destroy(gpu, target); }
GPU_INLINE void gpu_render_target_get_info(const s_gpu_backend *gpu, const s_gpu_render_target *target,
                                           struct s_gpu_render_target_info *out)
{ gpu->ops->render_target_get_info(gpu, target, out); }
GPU_INLINE s_gpu_image *gpu_render_target_color_image(s_gpu_backend *gpu, s_gpu_render_target *target)
{ return gpu->ops->render_target_color_image(gpu, target); }
GPU_INLINE const void *gpu_render_target_read(s_gpu_backend *gpu, s_gpu_render_target *target)
{ return gpu->ops->render_target_read(gpu, target); }

/* Commands, recorded into the current frame. begin_rendering: up to GPU_MAX_COLOR_ATTACHMENTS color images (NULL
 * entries unbound) and an optional depth / stencil image; the render area, viewport and scissor are the first
 * attachment's size; clear NULL loads everything. clear_attachments: inside rendering, rectangles (x0, y0, x1, y1;
 * NULL: all). copy_image: outside rendering, scaling / converting when sizes or formats differ; the destination is left
 * ready for sampling. copy_depth: a depth image's depth into another of the same format. copy_rects: rectangles between
 * two images of one format (every aspect both have). set_viewport: a D3D viewport (top-left origin; the backend gives
 * D3D's orientation). bind_pipeline: state NULL: the pipeline's own dynamic state; else that state's (library pipelines,
 * see above); binds that repeat what is bound record nothing. bind_group: offsets of its dynamic buffers (binding
 * order). memory_barrier: between all earlier and later commands, and the host's reads once the frame finished.
 * copy_buffer, copy_buffer_to_image: outside rendering, between full barriers. */
GPU_INLINE void gpu_cmd_begin_rendering(s_gpu_backend *gpu, s_gpu_image *const *colors, uint32_t color_count,
                                        s_gpu_image *depth, const struct s_gpu_clear *clear)
{ gpu->ops->cmd_begin_rendering(gpu, colors, color_count, depth, clear); }
GPU_INLINE void gpu_cmd_end_rendering(s_gpu_backend *gpu) { gpu->ops->cmd_end_rendering(gpu); }
GPU_INLINE void gpu_cmd_clear_attachments(s_gpu_backend *gpu, const struct s_gpu_clear *clear, const int32_t *rects,
                                          uint32_t rect_count)
{ gpu->ops->cmd_clear_attachments(gpu, clear, rects, rect_count); }
GPU_INLINE void gpu_cmd_copy_image(s_gpu_backend *gpu, s_gpu_image *source, int32_t source_x, int32_t source_y,
                                   uint32_t width, uint32_t height, s_gpu_image *destination, uint32_t destination_level,
                                   uint32_t destination_layer, int32_t destination_x, int32_t destination_y,
                                   uint32_t destination_width, uint32_t destination_height)
{
    gpu->ops->cmd_copy_image(gpu, source, source_x, source_y, width, height, destination, destination_level,
                             destination_layer, destination_x, destination_y, destination_width, destination_height);
}
GPU_INLINE void gpu_cmd_copy_depth(s_gpu_backend *gpu, s_gpu_image *source, int32_t source_x, int32_t source_y,
                                   uint32_t width, uint32_t height, s_gpu_image *destination, int32_t destination_x,
                                   int32_t destination_y)
{ gpu->ops->cmd_copy_depth(gpu, source, source_x, source_y, width, height, destination, destination_x, destination_y); }
GPU_INLINE void gpu_cmd_copy_rects(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                   const struct s_gpu_copy_rect *rects, uint32_t count)
{ gpu->ops->cmd_copy_rects(gpu, source, destination, rects, count); }
GPU_INLINE void gpu_cmd_set_viewport(s_gpu_backend *gpu, float x, float y, float width, float height, float min_depth,
                                     float max_depth)
{ gpu->ops->cmd_set_viewport(gpu, x, y, width, height, min_depth, max_depth); }
GPU_INLINE void gpu_cmd_set_scissor(s_gpu_backend *gpu, int32_t x, int32_t y, uint32_t width, uint32_t height)
{ gpu->ops->cmd_set_scissor(gpu, x, y, width, height); }
GPU_INLINE void gpu_cmd_bind_pipeline(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline)
{ gpu->ops->cmd_bind_pipeline(gpu, pipeline, 0); }
GPU_INLINE void gpu_cmd_bind_pipeline_state(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline,
                                            const struct s_gpu_graphics_state *state)
{ gpu->ops->cmd_bind_pipeline(gpu, pipeline, state); }
GPU_INLINE void gpu_cmd_bind_group(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, uint32_t set, gpu_bind_group group)
{ gpu->ops->cmd_bind_group(gpu, pipeline, set, group, 0, 0); }
#define GPU_MAX_DYNAMIC_OFFSETS 8u
GPU_INLINE void gpu_cmd_bind_group_dynamic(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, uint32_t set,
                                           gpu_bind_group group, uint32_t offset_count, const uint32_t *offsets)
{ gpu->ops->cmd_bind_group(gpu, pipeline, set, group, offset_count, offsets); }
GPU_INLINE void gpu_cmd_bind_group_pair(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, gpu_bind_group group0,
                                        uint32_t offset_count0, const uint32_t *offsets0, gpu_bind_group group1)
{ gpu->ops->cmd_bind_group_pair(gpu, pipeline, group0, offset_count0, offsets0, group1); }
/* A vertex stream (version 2 packs' fixed-function vertex input: D3D stream = binding) with its stride. */
GPU_INLINE void gpu_cmd_bind_vertex_buffer(s_gpu_backend *gpu, uint32_t binding, const s_gpu_buffer *buffer,
                                           uint32_t offset, uint32_t stride)
{ gpu->ops->cmd_bind_vertex_buffer(gpu, binding, buffer, offset, stride); }
GPU_INLINE void gpu_cmd_bind_index_buffer(s_gpu_backend *gpu, const s_gpu_buffer *buffer, uint32_t offset,
                                          uint32_t index_type)
{ gpu->ops->cmd_bind_index_buffer(gpu, buffer, offset, index_type); }
/* Without caps.buffer_device_address (D3D12): the vertex-stream views t0-t15 space2 (streams[s]: the buffer behind
 * xe_vertex_fetch.stream[s], whose address_lo is the byte offset of vertex 0 in it) and the memexport views u0-u3
 * space2 (exports[e]: the buffer of xe_system.memexport[e], z its byte offset) of the next draws or dispatch with this
 * pipeline; NULL entries are null views. Whole buffers, raw (byte-addressed). Never called with device addresses. */
#define GPU_XENOS_STREAM_VIEWS 16u
#define GPU_XENOS_EXPORT_VIEWS 4u
GPU_INLINE void gpu_cmd_xenos_bind_memory(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline,
                                          s_gpu_buffer *const *streams, s_gpu_buffer *const *exports)
{ gpu->ops->cmd_xenos_bind_memory(gpu, pipeline, streams, exports); }
GPU_INLINE void gpu_cmd_draw(s_gpu_backend *gpu, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                             uint32_t first_instance)
{ gpu->ops->cmd_draw(gpu, vertex_count, instance_count, first_vertex, first_instance); }
GPU_INLINE void gpu_cmd_draw_indexed(s_gpu_backend *gpu, uint32_t index_count, uint32_t instance_count,
                                     uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{ gpu->ops->cmd_draw_indexed(gpu, index_count, instance_count, first_index, vertex_offset, first_instance); }
GPU_INLINE void gpu_cmd_dispatch(s_gpu_backend *gpu, uint32_t groups_x, uint32_t groups_y, uint32_t groups_z)
{ gpu->ops->cmd_dispatch(gpu, groups_x, groups_y, groups_z); }
GPU_INLINE void gpu_cmd_memory_barrier(s_gpu_backend *gpu) { gpu->ops->cmd_memory_barrier(gpu); }
GPU_INLINE void gpu_cmd_copy_buffer(s_gpu_backend *gpu, s_gpu_buffer *source, uint32_t source_offset,
                                    s_gpu_buffer *destination, uint32_t destination_offset, uint32_t size)
{ gpu->ops->cmd_copy_buffer(gpu, source, source_offset, destination, destination_offset, size); }
GPU_INLINE void gpu_cmd_copy_buffer_to_image(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset,
                                             uint32_t row_length, s_gpu_image *image, uint32_t width, uint32_t height)
{ gpu->ops->cmd_copy_buffer_to_image(gpu, buffer, offset, row_length, image, width, height); }
GPU_INLINE void gpu_cmd_copy_image_to_buffer(s_gpu_backend *gpu, s_gpu_image *image, uint32_t level, uint32_t layer,
                                             int32_t x, int32_t y, uint32_t width, uint32_t height, s_gpu_buffer *buffer,
                                             uint32_t offset)
{ gpu->ops->cmd_copy_image_to_buffer(gpu, image, level, layer, x, y, width, height, buffer, offset); }
GPU_INLINE void gpu_cmd_readback(s_gpu_backend *gpu, s_gpu_render_target *target) { gpu->ops->cmd_readback(gpu, target); }
/* Device-loss diagnostics: marks the commands recorded after it (with the backend's checkpoints on; else nothing). */
GPU_INLINE void gpu_cmd_checkpoint(s_gpu_backend *gpu, const void *marker) { gpu->ops->cmd_checkpoint(gpu, marker); }

/* The helper passes (outside rendering; destinations made with storage use). reinterpret_7e3: a float image standing
 * for a Xenos 2_10_10_10_FLOAT target into a2b10g10r10_unorm as 2_10_10_10 bits (edram_7e3.hlsl). convert_pwl: the
 * Xenos 8_8_8_8 <-> _GAMMA conversion (encode: PWL codes of linear RGB; else degamma), alpha copied. downscale_image:
 * each destination texel the average of the N x N source texels it covers (resolve_downscale.hlsl). pack_depth_stencil:
 * a sampled depth / stencil image into r8g8b8a8_unorm texels holding the Xenos 24_8 words (float_depth: 20e4, else
 * unorm24). apply_display_gamma: through the ramp (GPU_DISPLAY_GAMMA_RAMP_SIZE bytes at ramp_offset of a uniform
 * buffer). scale_image_rect: a resolve's exponent bias over a rectangle (GPU_SCALE_IMAGE_PARAMETERS_SIZE bytes of
 * parameters). Those returning uint32_t return 0 when nothing was recorded (format or feature missing). */
GPU_INLINE void gpu_cmd_reinterpret_7e3(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination)
{ gpu->ops->cmd_reinterpret_7e3(gpu, source, destination); }
GPU_INLINE uint32_t gpu_cmd_convert_pwl(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, uint32_t encode)
{ return gpu->ops->cmd_convert_pwl(gpu, source, destination, encode); }
GPU_INLINE uint32_t gpu_cmd_downscale_image(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination)
{ return gpu->ops->cmd_downscale_image(gpu, source, destination); }
GPU_INLINE uint32_t gpu_cmd_pack_depth_stencil(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                               uint32_t float_depth)
{ return gpu->ops->cmd_pack_depth_stencil(gpu, source, destination, float_depth); }
GPU_INLINE void gpu_cmd_apply_display_gamma(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                            s_gpu_buffer *ramp, uint32_t ramp_offset)
{ gpu->ops->cmd_apply_display_gamma(gpu, source, destination, ramp, ramp_offset); }
GPU_INLINE uint32_t gpu_cmd_scale_image_rect(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                             s_gpu_buffer *parameters, uint32_t parameters_offset, uint32_t width,
                                             uint32_t height)
{ return gpu->ops->cmd_scale_image_rect(gpu, source, destination, parameters, parameters_offset, width, height); }

/* Occlusion queries (D3DQUERYTYPE_OCCLUSION): begin opens one in the recording frame (its frame serial and record in
 * the outs; 0 when none can be had), the draws until end count their passing samples across renderings; result: 1
 * with the count once the frame has finished (or is too old to know: 0 samples), 0 while pending. */
GPU_INLINE int gpu_occlusion_begin(s_gpu_backend *gpu, uint32_t *out_serial, uint32_t *out_record)
{ return gpu->ops->occlusion_begin(gpu, out_serial, out_record); }
GPU_INLINE void gpu_occlusion_end(s_gpu_backend *gpu) { gpu->ops->occlusion_end(gpu); }
GPU_INLINE int gpu_occlusion_result(s_gpu_backend *gpu, uint32_t serial, uint32_t record, uint64_t *out_samples)
{ return gpu->ops->occlusion_result(gpu, serial, record, out_samples); }

/* Presentation: one surface per backend (window_system: e_gpu_window_system; display / window: the native handles,
 * port_window_native_display / _window); swapchain_create (again after k_gpu_out_of_date): vsync waits for the display,
 * else the lowest-latency mode there is; within a frame: acquire, then blit_to_swapchain (outside rendering); after
 * gpu_frame_end: present. */
GPU_INLINE gpu_result gpu_surface_create(s_gpu_backend *gpu, uint32_t window_system, void *display, uintptr_t window)
{ return gpu->ops->surface_create(gpu, window_system, display, window); }
GPU_INLINE void gpu_surface_destroy(s_gpu_backend *gpu) { gpu->ops->surface_destroy(gpu); }
GPU_INLINE gpu_result gpu_swapchain_create(s_gpu_backend *gpu, uint32_t width, uint32_t height, uint32_t vsync)
{ return gpu->ops->swapchain_create(gpu, width, height, vsync); }
GPU_INLINE void gpu_swapchain_destroy(s_gpu_backend *gpu) { gpu->ops->swapchain_destroy(gpu); }
GPU_INLINE gpu_result gpu_swapchain_acquire(s_gpu_backend *gpu, uint32_t *out_image_index)
{ return gpu->ops->swapchain_acquire(gpu, out_image_index); }
GPU_INLINE void gpu_cmd_blit_to_swapchain(s_gpu_backend *gpu, s_gpu_render_target *target)
{ gpu->ops->cmd_blit_to_swapchain(gpu, target); }
GPU_INLINE gpu_result gpu_swapchain_present(s_gpu_backend *gpu) { return gpu->ops->swapchain_present(gpu); }

#endif /* PORT_D3D_GPU_BACKEND_H */
