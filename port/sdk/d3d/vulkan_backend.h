#ifndef PORT_D3D_VULKAN_BACKEND_H
#define PORT_D3D_VULKAN_BACKEND_H

/*
 * The PC port's Vulkan 1.3 backend: device setup, resources, pipelines built from the shader pack
 * (port/shaders/shader_pack.h), a frame ring, an offscreen render target with readback, and optional window
 * presentation. Dynamic rendering throughout (no render passes or framebuffers).
 *
 * vulkan_gpu.cpp implements the GPU seam (gpu_backend.h) over this library. Host Vulkan callers can also record
 * directly through the tracked command helpers in vulkan_internal.h.
 *
 * Files:
 *   vulkan_backend.cpp  everything below except the shader-pack section
 *   vulkan_pack.cpp     the shader-pack section (shader module cache, reflected layouts, pack pipelines)
 *   vulkan_gpu.cpp      the s_gpu_backend over this library (gpu_backend.h)
 *   xenos_vertex.cpp    Halo 3's vertex declarations and pipeline vertex input (xenos_vertex.h; neutral formats)
 *   vulkan_mini.h       the Vulkan API subset; vulkan_host.h the loader (port/sdk/runtime/vulkan_host_*.c)
 *
 * Calling side. The backend is host code (no XDK headers; Vulkan structs in the host's C ABI). Everything in this
 * header uses only pointers, 32-bit integers, floats, Vulkan handles passed by value, and structs of 4-byte members,
 * so the game's MS-ABI code (i686-pc-windows-msvc-elf on Linux) can call it directly. The raw-access functions
 * (vulkan_backend_functions and friends) and the Vulkan structs of vulkan_mini.h are for host-ABI code only.
 *
 * Threading: one thread records and submits; the backend has no locks.
 *
 * Lifetimes: objects are destroyed immediately; the caller makes sure the GPU is done with them (a finished frame,
 * vulkan_backend_wait_idle). Caches (descriptor set layouts, pipeline layouts, shader modules) belong to the backend.
 *
 * Results are VkResult (VK_SUCCESS = 0); creators returning objects return NULL / VK_NULL_HANDLE on failure. Every
 * failure is also logged with its reason.
 */

#include <stddef.h>
#include <stdint.h>

#include "vulkan_mini.h"
#include "xenos_vertex.h"

#ifdef __cplusplus
extern "C" {
#endif

struct s_shader_pack;
struct s_gpu_backend;

typedef struct s_vulkan_backend s_vulkan_backend;
typedef struct s_vulkan_buffer s_vulkan_buffer;
typedef struct s_vulkan_image s_vulkan_image;
typedef struct s_vulkan_render_target s_vulkan_render_target;
typedef struct s_vulkan_pipeline s_vulkan_pipeline;

/* Host only. The Vulkan backend pointer is valid until the GPU seam object is destroyed. */
s_vulkan_backend *vulkan_gpu_native_backend(struct s_gpu_backend *gpu);
/* Whether the device enabled the ABI 6 features and meets the descriptor and push-constant limits. */
uint32_t vulkan_backend_supports_xenos_abi6(const s_vulkan_backend *backend);

/* ---- backend ---- */

/* Receives one line of log text (ending in '\n'). */
typedef void (*vulkan_log_function)(void *context, const char *text);
/* Writes a description of a checkpoint marker (vulkan_cmd_checkpoint) to the log. */
typedef void (*vulkan_checkpoint_describe_function)(void *context, const void *marker);

/* Window systems a surface may later be created for (instance extensions are chosen at creation). */
enum e_vulkan_window_system {
    k_vulkan_window_system_none = 0,     /* headless: offscreen rendering only */
    k_vulkan_window_system_win32 = 1,    /* VK_KHR_win32_surface */
    k_vulkan_window_system_xlib = 2,     /* VK_KHR_xlib_surface */
    k_vulkan_window_system_xcb = 3,      /* VK_KHR_xcb_surface */
    k_vulkan_window_system_headless = 4, /* VK_EXT_headless_surface: a swapchain with no window (testing) */
    k_vulkan_window_system_wayland = 5,  /* VK_KHR_wayland_surface */
};

struct s_vulkan_backend_create_info {
    const char *application_name;     /* NULL: "halo3" */
    uint32_t window_system;           /* e_vulkan_window_system */
    uint32_t validation;              /* nonzero: VK_LAYER_KHRONOS_validation if installed (also when
                                         $HALO3_VULKAN_VALIDATION is 1) */
    uint32_t frames_in_flight;        /* frame slots: 1..4, 0 defaults to 2; HALO3_FRAMES_IN_FLIGHT overrides with 2..4 */
    int32_t device_index;             /* vkEnumeratePhysicalDevices index to use, -1 to choose (discrete >
                                         integrated > virtual > CPU); $HALO3_VULKAN_DEVICE overrides */
    const void *pipeline_cache_data;  /* initial VkPipelineCache contents (vulkan_backend_get_pipeline_cache), or NULL */
    uint32_t pipeline_cache_size;
    vulkan_log_function log;          /* NULL: port_write_error (port/sdk/runtime) */
    void *log_context;
    uint32_t xenos_features;          /* nonzero: also turn on what translated Xenos shaders use (shader pack version 3,
                                         port/shaders/xenos_spirv_abi.h) when the device has it: bufferDeviceAddress,
                                         tessellationShader, geometryShader (s_vulkan_device_info reports them) */
};

struct s_vulkan_device_info {
    char name[256];
    uint32_t device_type;      /* VkPhysicalDeviceType */
    uint32_t api_version;      /* VK_API_VERSION_MAJOR / _MINOR / _PATCH */
    uint32_t driver_version;   /* vendor encoded */
    uint32_t vendor_id;
    uint32_t device_id;
    uint32_t queue_family;     /* the graphics + compute queue's family */
    uint32_t synchronization2; /* 1 when barriers and submits use VK_KHR_synchronization2 (core 1.3) */
    uint32_t presentation;     /* 1 when VK_KHR_swapchain is enabled */
    uint32_t validation;       /* 1 when the validation layer is active */
    uint32_t depth_format;     /* VkFormat of depth/stencil targets: D32_SFLOAT_S8_UINT, else D24_UNORM_S8_UINT */
    uint32_t frames_in_flight;
    uint32_t buffer_device_address; /* 1 when buffers have device addresses (vulkan_buffer_device_address) */
    uint32_t tessellation;     /* 1 when tessellationShader is on */
    uint32_t geometry;         /* 1 when geometryShader is on */
    uint32_t clip_distance;    /* 1 when shaderClipDistance is on */
    uint32_t full_draw_index;  /* 1 when fullDrawIndexUint32 is on */
    uint32_t pipeline_library; /* 1 when VK_EXT_graphics_pipeline_library is on (vulkan_pipeline_library_*): with Xenos
                                  features, when the device has it, unless $HALO3_PIPELINE_LIBRARY is 0 */
    uint32_t pipeline_library_fast_link; /* graphicsPipelineLibraryFastLinking: linking without optimization is cheap */
};

VkResult vulkan_backend_create(const struct s_vulkan_backend_create_info *info, s_vulkan_backend **out_backend);
/* Waits for the GPU, then destroys everything the backend owns (objects the caller created must be destroyed
 * first). NULL is ignored. */
void vulkan_backend_destroy(s_vulkan_backend *backend);
void vulkan_backend_get_device_info(const s_vulkan_backend *backend, struct s_vulkan_device_info *out);
VkResult vulkan_backend_wait_idle(s_vulkan_backend *backend);
/* Copies the pipeline cache: with data NULL, *size receives the size; otherwise up to *size bytes are written. */
VkResult vulkan_backend_get_pipeline_cache(s_vulkan_backend *backend, void *data, size_t *size);
/* Pipelines created so far: a count that changes when the pipeline cache gains entries worth saving. */
uint32_t vulkan_backend_pipelines_created(const s_vulkan_backend *backend);
/* Supported VkFormatFeatureFlags of a format: optimal-tiling image features, buffer features. */
uint32_t vulkan_backend_format_features(s_vulkan_backend *backend, uint32_t format, uint32_t buffer_features);
const char *vulkan_result_string(VkResult result);

/* Raw access, for host-ABI code only. */
const struct s_vulkan_functions *vulkan_backend_functions(const s_vulkan_backend *backend);
VkInstance vulkan_backend_instance(const s_vulkan_backend *backend);
VkPhysicalDevice vulkan_backend_physical_device(const s_vulkan_backend *backend);
VkDevice vulkan_backend_device(const s_vulkan_backend *backend);
VkQueue vulkan_backend_queue(const s_vulkan_backend *backend);

/* ---- memory, buffers ---- */

enum e_vulkan_memory_usage {
    k_vulkan_memory_gpu = 0,      /* DEVICE_LOCAL; filled by vulkan_buffer_upload (staging) or the GPU */
    k_vulkan_memory_upload = 1,   /* HOST_VISIBLE | HOST_COHERENT, persistently mapped; CPU writes, GPU reads */
    k_vulkan_memory_readback = 2, /* HOST_VISIBLE (HOST_CACHED preferred), persistently mapped; GPU writes */
};

#define VULKAN_NO_MEMORY_TYPE 0xffffffffu
/* A memory type index allowed by type_bits with all the required and, if possible, all the preferred
 * VkMemoryPropertyFlags; VULKAN_NO_MEMORY_TYPE if none has the required ones. */
uint32_t vulkan_find_memory_type(const s_vulkan_backend *backend, uint32_t type_bits, uint32_t required,
                                 uint32_t preferred);

/* usage: VkBufferUsageFlags (TRANSFER_DST is added for k_vulkan_memory_gpu). */
s_vulkan_buffer *vulkan_buffer_create(s_vulkan_backend *backend, uint32_t size, uint32_t usage, uint32_t memory_usage);
void vulkan_buffer_destroy(s_vulkan_backend *backend, s_vulkan_buffer *buffer);
/* Destroys the buffer once no frame can use it any more: commands already recorded into the frame being recorded,
 * or submitted, may still read it, so it is destroyed when that frame slot's fence is next waited for (all earlier
 * submissions are then finished too). Use it for any buffer a draw may have referenced. */
void vulkan_buffer_retire(s_vulkan_backend *backend, s_vulkan_buffer *buffer);
VkBuffer vulkan_buffer_handle(const s_vulkan_buffer *buffer);
uint32_t vulkan_buffer_size(const s_vulkan_buffer *buffer);
/* The persistent mapping of an upload or readback buffer; NULL for k_vulkan_memory_gpu. */
void *vulkan_buffer_mapped(const s_vulkan_buffer *buffer);
/* Makes GPU writes to a readback buffer visible to the mapping (no-op on coherent memory). */
void vulkan_buffer_invalidate(s_vulkan_backend *backend, s_vulkan_buffer *buffer);
/* Reads bytes back from a buffer: through the mapping for host-visible buffers, otherwise through a readback buffer and a
 * copy submitted and waited for at once (after every earlier submission: a frame still being recorded is not seen). */
VkResult vulkan_buffer_download(s_vulkan_backend *backend, s_vulkan_buffer *buffer, uint32_t offset, void *data,
                                uint32_t size);
/* Writes bytes into a buffer: through the mapping for host-visible buffers, otherwise through a staging buffer and
 * a copy submitted and waited for at once. */
VkResult vulkan_buffer_upload(s_vulkan_backend *backend, s_vulkan_buffer *buffer, uint32_t offset, const void *data,
                              uint32_t size);
/* The buffer's VkDeviceAddress as two 32-bit halves (0, 0 without bufferDeviceAddress: s_vulkan_device_info). */
void vulkan_buffer_device_address(const s_vulkan_backend *backend, const s_vulkan_buffer *buffer, uint32_t *out_lo,
                                  uint32_t *out_hi);
/* A texel buffer view (uniform or storage texel buffer descriptors). */
VkBufferView vulkan_buffer_view_create(s_vulkan_backend *backend, s_vulkan_buffer *buffer, uint32_t format,
                                       uint32_t offset, uint32_t range);
void vulkan_buffer_view_destroy(s_vulkan_backend *backend, VkBufferView view);

/* ---- images, samplers ---- */

/* Block size of a format: 4x4 for the BC formats, 1x1 otherwise. Returns 0 for formats the backend does not know. */
int vulkan_format_block_info(uint32_t format, uint32_t *out_block_width, uint32_t *out_block_height,
                             uint32_t *out_bytes_per_block);

#define VULKAN_SWIZZLE(r, g, b, a) ((uint32_t)(r) | (uint32_t)(g) << 8 | (uint32_t)(b) << 16 | (uint32_t)(a) << 24)

struct s_vulkan_image_desc {
    uint32_t view_type;    /* VkImageViewType of the image's view: 2D, 2D_ARRAY, CUBE, 3D */
    uint32_t format;       /* VkFormat */
    uint32_t width;
    uint32_t height;
    uint32_t depth;        /* 3D depth, 1 otherwise */
    uint32_t mip_levels;
    uint32_t array_layers; /* 6 for a cube */
    uint32_t usage;        /* VkImageUsageFlags; 0: SAMPLED | TRANSFER_DST */
    uint32_t swizzle;      /* the view's VkComponentMapping: VkComponentSwizzle r | g << 8 | b << 16 | a << 24; 0 is identity
                              (VULKAN_SWIZZLE builds one). Lets a Xenos texture's D3DFORMAT swizzle (AY8 XXXX, A8 000R,
                              X8R8G8B8 RGB1) apply on the GPU */
};

/* One block of the data given to vulkan_image_upload. Pitches of 0 mean tightly packed. */
struct s_vulkan_image_region {
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

s_vulkan_image *vulkan_image_create(s_vulkan_backend *backend, const struct s_vulkan_image_desc *desc);
void vulkan_image_destroy(s_vulkan_backend *backend, s_vulkan_image *image);
/* vulkan_buffer_retire for an image (its views go with it). */
void vulkan_image_retire(s_vulkan_backend *backend, s_vulkan_image *image);
VkImage vulkan_image_handle(const s_vulkan_image *image);
VkImageView vulkan_image_view(const s_vulkan_image *image);
/* The view shaders sample: vulkan_image_view, except for a sampled depth/stencil image, whose depth aspect alone. */
VkImageView vulkan_image_sampled_view(const s_vulkan_image *image);
/* A 2D_ARRAY view for a 2D or 2D_ARRAY image (Xenos tfetch3D on k2DOrStacked 2D textures). */
VkImageView vulkan_image_array_view(s_vulkan_backend *backend, s_vulkan_image *image);
/* A color image through another swizzle (VULKAN_SWIZZLE) than its own view's: a storage image, whose own view must
 * keep the identity, sampled with a texture header's swizzle. One other swizzle for the image's life (the view is
 * kept with it): a second one is refused (VK_NULL_HANDLE). */
VkImageView vulkan_image_swizzled_view(s_vulkan_backend *backend, s_vulkan_image *image, uint32_t swizzle);
/* Copies data into the image through a staging buffer (submitted and waited for at once) and leaves the whole image
 * in SHADER_READ_ONLY_OPTIMAL. */
VkResult vulkan_image_upload(s_vulkan_backend *backend, s_vulkan_image *image, const struct s_vulkan_image_region *regions,
                             uint32_t region_count, const void *data, uint32_t size);
/* One 2D level and layer; row_pitch 0 for tightly packed rows. */
VkResult vulkan_image_upload_2d(s_vulkan_backend *backend, s_vulkan_image *image, uint32_t mip_level, uint32_t layer,
                                const void *data, uint32_t row_pitch);

struct s_vulkan_sampler_desc {
    uint32_t mag_filter;       /* VkFilter */
    uint32_t min_filter;
    uint32_t mipmap_mode;      /* VkSamplerMipmapMode */
    uint32_t address_u;        /* VkSamplerAddressMode */
    uint32_t address_v;
    uint32_t address_w;
    float mip_lod_bias;
    float max_anisotropy;      /* <= 1: off; clamped to the device limit */
    uint32_t compare_enable;
    uint32_t compare_op;       /* VkCompareOp */
    float min_lod;
    float max_lod;             /* 1000: all levels */
    uint32_t border_color;     /* VkBorderColor */
};

VkSampler vulkan_sampler_create(s_vulkan_backend *backend, const struct s_vulkan_sampler_desc *desc);
void vulkan_sampler_destroy(s_vulkan_backend *backend, VkSampler sampler);

/* ---- shaders, layouts, pipelines ---- */

VkShaderModule vulkan_shader_module_create(s_vulkan_backend *backend, const uint32_t *code, size_t size_bytes);
void vulkan_shader_module_destroy(s_vulkan_backend *backend, VkShaderModule module);
/* The name of the module's OpEntryPoint for a stage (e_shader_pack_stage), a pointer into the code; NULL if the
 * module has none. */
const char *vulkan_spirv_entry_point(const uint32_t *code, size_t size_bytes, uint32_t stage);
/* The color output locations a fragment shader writes (bit L: Location L), for the attachments it leaves alone. */
uint32_t vulkan_spirv_output_mask(const uint32_t *code, size_t size_bytes);
/* A pack fragment shader with the specialization constants graphics pipelines set: its color outputs scaled by
 * VULKAN_SPEC_COLOR_EXP_SCALE + location (default 1), its fragment coordinate offset by VULKAN_SPEC_FRAGCOORD_OFFSET
 * + 0 / 1 (default 0). New code (free() it, *out_size bytes), or NULL when there is nothing to patch (use it as it
 * is). */
uint32_t *vulkan_spirv_patch_fragment(const uint32_t *code, size_t size_bytes, size_t *out_size);

/* One descriptor binding of a set layout. */
struct s_vulkan_layout_binding {
    uint32_t binding;
    uint32_t descriptor_type;  /* VkDescriptorType */
    uint32_t count;
    uint32_t stage_flags;      /* VkShaderStageFlags */
};

/* A cached set layout for these bindings (in any order; equal binding lists share one layout). Owned by the backend. */
VkDescriptorSetLayout vulkan_descriptor_set_layout_get(s_vulkan_backend *backend,
                                                       const struct s_vulkan_layout_binding *bindings, uint32_t count);
/* A cached pipeline layout of these set layouts (set i = set_layouts[i]; VK_NULL_HANDLE entries become empty sets).
 * Owned by the backend. */
VkPipelineLayout vulkan_pipeline_layout_get(s_vulkan_backend *backend, const VkDescriptorSetLayout *set_layouts,
                                            uint32_t set_count);

struct s_vulkan_stencil_face {
    uint32_t fail_op;          /* VkStencilOp */
    uint32_t pass_op;
    uint32_t depth_fail_op;
    uint32_t compare_op;       /* VkCompareOp */
    uint32_t compare_mask;
    uint32_t write_mask;
    uint32_t reference;
};

struct s_vulkan_blend_attachment {
    uint32_t enable;
    uint32_t src_color;        /* VkBlendFactor */
    uint32_t dst_color;
    uint32_t color_op;         /* VkBlendOp */
    uint32_t src_alpha;
    uint32_t dst_alpha;
    uint32_t alpha_op;
    uint32_t write_mask;       /* VkColorComponentFlags */
};

#define VULKAN_MAX_COLOR_ATTACHMENTS 4

/* Fixed-function state of a graphics pipeline. Viewport, scissor and vertex binding strides are dynamic. */
struct s_vulkan_graphics_state {
    uint32_t topology;         /* VkPrimitiveTopology */
    uint32_t primitive_restart;
    uint32_t polygon_mode;     /* VkPolygonMode */
    uint32_t cull_mode;        /* VkCullModeFlags */
    uint32_t front_face;       /* VkFrontFace */
    uint32_t depth_test;
    uint32_t depth_write;
    uint32_t depth_compare;    /* VkCompareOp */
    uint32_t depth_bias_enable;
    float depth_bias_constant;
    float depth_bias_slope;
    uint32_t stencil_test;
    struct s_vulkan_stencil_face stencil_front;
    struct s_vulkan_stencil_face stencil_back;
    uint32_t sample_count;     /* VkSampleCountFlagBits; 0: 1 */
    uint32_t alpha_to_coverage;
    uint32_t color_attachment_count;
    uint32_t color_formats[VULKAN_MAX_COLOR_ATTACHMENTS];        /* VkFormat */
    struct s_vulkan_blend_attachment blend[VULKAN_MAX_COLOR_ATTACHMENTS];
    uint32_t depth_format;     /* VkFormat of the depth attachment, VK_FORMAT_UNDEFINED for none */
    uint32_t stencil_format;   /* VkFormat of the stencil attachment, VK_FORMAT_UNDEFINED for none */
    int32_t color_exp_bias[VULKAN_MAX_COLOR_ATTACHMENTS]; /* the fragment shader's color output i is scaled by 2^bias
                                                             (a Xenos render target's ColorExpBias; pack modules only,
                                                             VULKAN_SPEC_COLOR_EXP_SCALE) */
    int32_t fragcoord_offset[2];   /* added to the fragment shader's SV_Position x / y (pack modules only,
                                      VULKAN_SPEC_FRAGCOORD_OFFSET): the screen origin of a predicated tile */
    float blend_constants[4];      /* R, G, B, A of the CONSTANT_* blend factors (zero when no attachment uses them,
                                      so pipelines that do not read them share one key) */
};

/* Opaque, triangle list, no culling (front faces clockwise, as D3D's), no depth or stencil test (compare
 * LESS_OR_EQUAL once enabled), one color attachment of color_format (or none for VK_FORMAT_UNDEFINED) and
 * depth_stencil_format as the depth and (if it has stencil) the stencil attachment. */
void vulkan_graphics_state_default(struct s_vulkan_graphics_state *state, uint32_t color_format,
                                   uint32_t depth_stencil_format);

/* Pack fragment shaders scale each color output by a float specialization constant, VULKAN_SPEC_COLOR_EXP_SCALE + its
 * location (default 1; vulkan_pack_shader_module adds them), which graphics pipelines set to 2^color_exp_bias[i]. */
#define VULKAN_SPEC_COLOR_EXP_SCALE 0x7e30u
/* ... and offset their fragment coordinate by VULKAN_SPEC_FRAGCOORD_OFFSET + 0 / + 1 (default 0), which pipelines set to
 * fragcoord_offset. */
#define VULKAN_SPEC_FRAGCOORD_OFFSET 0x7e34u

/* A graphics pipeline from raw modules. fragment may be VK_NULL_HANDLE (depth-only); entry points NULL mean "main".
 * vertex_input NULL means no vertex input. The pack-driven functions below are thin wrappers over this. */
s_vulkan_pipeline *vulkan_graphics_pipeline_create(s_vulkan_backend *backend, VkShaderModule vertex,
                                                   const char *vertex_entry, VkShaderModule fragment,
                                                   const char *fragment_entry, VkPipelineLayout layout,
                                                   const struct s_xenos_vertex_input *vertex_input,
                                                   const struct s_vulkan_graphics_state *state);
/* The compute pipeline as a bare handle (any thread: the pipeline prewarm), and a handle made into a pipeline the
 * caller owns (bind_point: VkPipelineBindPoint). */
VkPipeline vulkan_compute_pipeline_create_handle(s_vulkan_backend *backend, VkShaderModule module, const char *entry,
                                                 VkPipelineLayout layout);
s_vulkan_pipeline *vulkan_pipeline_adopt(s_vulkan_backend *backend, VkPipeline handle, VkPipelineLayout layout,
                                         uint32_t bind_point);
s_vulkan_pipeline *vulkan_compute_pipeline_create(s_vulkan_backend *backend, VkShaderModule module, const char *entry,
                                                  VkPipelineLayout layout);
/* One shader stage of vulkan_graphics_pipeline_create_stages. */
struct s_vulkan_shader_stage {
    uint32_t stage;            /* VkShaderStageFlagBits: VERTEX, TESSELLATION_CONTROL / _EVALUATION, GEOMETRY, FRAGMENT */
    VkShaderModule module;
    const char *entry;         /* NULL: "main" */
};
/* A graphics pipeline from any stages (the vertex stage first; no fragment stage: depth only), with no vertex input
 * (the shaders fetch their own vertices) and no specialization constants: translated Xenos shaders. A tessellation
 * control stage makes the topology a patch list of patch_control_points. state->color_exp_bias and fragcoord_offset
 * are not used (the shaders take them from their constants). */
s_vulkan_pipeline *vulkan_graphics_pipeline_create_stages(s_vulkan_backend *backend,
                                                          const struct s_vulkan_shader_stage *stages, uint32_t stage_count,
                                                          VkPipelineLayout layout,
                                                          const struct s_vulkan_graphics_state *state,
                                                          uint32_t patch_control_points);
void vulkan_pipeline_destroy(s_vulkan_backend *backend, s_vulkan_pipeline *pipeline);

/* Pipeline libraries (VK_EXT_graphics_pipeline_library, s_vulkan_device_info::pipeline_library): a graphics pipeline
 * of translated Xenos shaders linked from four parts, each made once and shared by every pipeline that has it:
 *   vertex input        the topology and primitive restart (no vertex input: the shaders fetch their own);
 *   pre-rasterization   the vertex stage(s) and the polygon mode; cull mode, front face and depth bias are dynamic;
 *   fragment shader     the fragment stage (or none: depth only); depth and stencil test state is dynamic;
 *   fragment output     the attachments' formats, blending, sample count and alpha to coverage.
 * The parts are made with the pipeline cache and may be made on any thread (only Vulkan object creation, which is
 * thread-safe); a NULL handle is a failure (logged). A linked pipeline holds the dynamic state of its
 * s_vulkan_graphics_state and sets it when bound (vulkan_cmd_bind_pipeline; vulkan_cmd_bind_pipeline_state sets
 * another state's instead, so one link serves every dynamic state), so it is used as any other. Libraries
 * keep their link-time optimization information, so a pipeline can be linked again optimized
 * (vulkan_pipeline_link_handle with optimize) and swapped in (vulkan_pipeline_replace). */
VkPipeline vulkan_pipeline_library_vertex_input(s_vulkan_backend *backend, const struct s_vulkan_graphics_state *state,
                                                uint32_t patch_control_points);
VkPipeline vulkan_pipeline_library_pre_rasterization(s_vulkan_backend *backend, const struct s_vulkan_shader_stage *stages,
                                                     uint32_t stage_count, VkPipelineLayout layout,
                                                     const struct s_vulkan_graphics_state *state,
                                                     uint32_t patch_control_points);
VkPipeline vulkan_pipeline_library_fragment_shader(s_vulkan_backend *backend, VkShaderModule fragment,
                                                   VkPipelineLayout layout, uint32_t alpha_test_enable);
VkPipeline vulkan_pipeline_library_fragment_output(s_vulkan_backend *backend, const struct s_vulkan_graphics_state *state);
void vulkan_pipeline_library_destroy(s_vulkan_backend *backend, VkPipeline library);
/* libraries: vertex input, pre-rasterization, fragment shader, fragment output. optimize: link-time optimization
 * (slow, like a full pipeline), else a fast link. */
s_vulkan_pipeline *vulkan_pipeline_link(s_vulkan_backend *backend, const VkPipeline libraries[4], VkPipelineLayout layout,
                                        const struct s_vulkan_graphics_state *state, uint32_t optimize);
/* The same, as a bare handle (any thread), for vulkan_pipeline_replace. */
VkPipeline vulkan_pipeline_link_handle(s_vulkan_backend *backend, const VkPipeline libraries[4], VkPipelineLayout layout,
                                       uint32_t optimize);
/* Makes pipeline use handle from now on; its previous handle is destroyed once the frame being recorded is done. */
void vulkan_pipeline_replace(s_vulkan_backend *backend, s_vulkan_pipeline *pipeline, VkPipeline handle);
VkPipeline vulkan_pipeline_handle(const s_vulkan_pipeline *pipeline);
VkPipelineLayout vulkan_pipeline_layout(const s_vulkan_pipeline *pipeline);
/* The layout of descriptor set `set` (VK_NULL_HANDLE past the layout's sets). */
VkDescriptorSetLayout vulkan_pipeline_set_layout(const s_vulkan_pipeline *pipeline, uint32_t set);

/* ---- shader pack (vulkan_pack.cpp) ---- */

/* The VkShaderModule of a pack module, created on first use and cached until vulkan_pack_release; its entry point
 * name in *out_entry_point if not NULL. One pack at a time per backend. */
VkShaderModule vulkan_pack_shader_module(s_vulkan_backend *backend, struct s_shader_pack *pack, uint32_t module_index,
                                         const char **out_entry_point);
/* The pipeline layout of pack shaders (a vertex and a pixel shader, or one compute shader): each reflected binding
 * goes to its own set (vertex shaders set 0, pixel shaders set 1, compute set 0) with the stage of its shader; a
 * binding present in several shaders with the same type and count is shared by their stages. */
VkPipelineLayout vulkan_pack_pipeline_layout(s_vulkan_backend *backend, struct s_shader_pack *pack,
                                             const uint32_t *shader_indices, uint32_t shader_count);
/* A graphics pipeline from a pack vertex and pixel shader. declaration NULL: the vertex shader's vertex type and
 * entry point choose one (xenos_vertex_declaration_for_entry_point; PRT entry points take their own transfer
 * type). Vertex shader inputs are matched to it with xenos_vertex_input_build. */
s_vulkan_pipeline *vulkan_pack_graphics_pipeline(s_vulkan_backend *backend, struct s_shader_pack *pack,
                                                 uint32_t vertex_shader, uint32_t pixel_shader,
                                                 const struct s_xenos_vertex_declaration *declaration,
                                                 const struct s_vulkan_graphics_state *state);
s_vulkan_pipeline *vulkan_pack_compute_pipeline(s_vulkan_backend *backend, struct s_shader_pack *pack,
                                                uint32_t compute_shader);
/* Destroys the cached shader modules (before shader_pack_close; pipelines made from them stay valid). */
void vulkan_pack_release(s_vulkan_backend *backend);
/* Version 3 packs (translated Xenos microcode): the VkShaderModule of a module, used as it is (entry point "main"),
 * created on first use and cached as vulkan_pack_shader_module. */
VkShaderModule vulkan_pack_xenos_module(s_vulkan_backend *backend, struct s_shader_pack *pack, uint32_t module_index);

/* ---- descriptors ---- */

/* A descriptor set. Transient sets come from the current frame's pools and are recycled when that frame slot is next
 * begun (allocate them between vulkan_frame_begin and vulkan_frame_end); persistent sets live until
 * vulkan_descriptor_set_free. */
VkDescriptorSet vulkan_descriptor_set_allocate(s_vulkan_backend *backend, VkDescriptorSetLayout layout,
                                               uint32_t transient);
void vulkan_descriptor_set_free(s_vulkan_backend *backend, VkDescriptorSet set);
/* A transient set (as vulkan_descriptor_set_allocate's) from the frame's pools for large image + sampler layouts: the
 * universal Xenos texture set (xenos_spirv_abi.h set 1, 128 sampled images and 192 samplers a set). */
VkDescriptorSet vulkan_descriptor_set_allocate_textures(s_vulkan_backend *backend, VkDescriptorSetLayout layout);
/* A set of the same layouts that outlives the frame (a cache of the sets draws bind again frame after frame), from
 * pools of its own. Valid until vulkan_texture_sets_reset, which hands every such set's pool to the frame being (or last)
 * recorded, destroyed once that frame's fence has signalled (draws recorded so far may use its sets). A set holding a
 * view or sampler that is going away must not be bound again: the caller resets whenever vulkan_backend_view_epoch
 * has changed. */
VkDescriptorSet vulkan_texture_set_allocate(s_vulkan_backend *backend, VkDescriptorSetLayout layout);
void vulkan_texture_sets_reset(s_vulkan_backend *backend);
/* Advances whenever an image (its views) or a sampler is retired or destroyed. */
uint32_t vulkan_backend_view_epoch(const s_vulkan_backend *backend);
/* descriptor_type: UNIFORM_BUFFER or STORAGE_BUFFER (range 0: to the end of the buffer). */
void vulkan_descriptor_write_buffer(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding,
                                    uint32_t array_element, uint32_t descriptor_type, s_vulkan_buffer *buffer,
                                    uint32_t offset, uint32_t range);
/* descriptor_type: SAMPLED_IMAGE (read in SHADER_READ_ONLY_OPTIMAL) or STORAGE_IMAGE (GENERAL). */
void vulkan_descriptor_write_image(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding,
                                   uint32_t array_element, uint32_t descriptor_type, VkImageView view);
void vulkan_descriptor_write_sampler(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding,
                                     uint32_t array_element, VkSampler sampler);
/* `count` descriptors of `set` in one vkUpdateDescriptorSets (array element 0 each): binding[i] below first_sampler
 * is a SAMPLED_IMAGE, handle[i] its VkImageView (read in SHADER_READ_ONLY_OPTIMAL); from first_sampler on a
 * SAMPLER, handle[i] its VkSampler. A draw's texture set is written whole: one driver call instead of one a
 * descriptor. */
void vulkan_descriptor_write_textures(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t count,
                                      const uint32_t *binding, const uint64_t *handle, uint32_t first_sampler);
/* descriptor_type: UNIFORM_TEXEL_BUFFER or STORAGE_TEXEL_BUFFER. */
void vulkan_descriptor_write_texel_buffer(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding,
                                          uint32_t array_element, uint32_t descriptor_type, VkBufferView view);

/* ---- frames ---- */

/* Starts recording the next frame of the ring: waits for that slot's previous submission, recycles its command
 * buffer and transient descriptor sets. */
VkResult vulkan_frame_begin(s_vulkan_backend *backend);
/* Ends and submits the frame (waiting on the swapchain image acquired in it, if any); wait nonzero: also waits for
 * the GPU to finish it. */
VkResult vulkan_frame_end(s_vulkan_backend *backend, uint32_t wait);
/* The command buffer being recorded (between begin and end), for host-ABI code recording its own commands. */
VkCommandBuffer vulkan_frame_command_buffer(const s_vulkan_backend *backend);

/* ---- offscreen render target ---- */

struct s_vulkan_render_target_info {
    uint32_t width;
    uint32_t height;
    uint32_t color_format;     /* VkFormat */
    uint32_t depth_format;     /* VkFormat, VK_FORMAT_UNDEFINED without depth */
    uint32_t readback_row_pitch;
};

enum e_vulkan_render_target_flags {
    k_vulkan_render_target_depth = 1,   /* with the device's depth/stencil image */
    k_vulkan_render_target_storage = 2, /* the color image is also a storage image (the format must allow it) */
};

/* A color image (R8G8B8A8_UNORM or B8G8R8A8_UNORM, or any color-attachment format; also sampled and copyable),
 * optionally the device's depth/stencil image, and a host-visible readback buffer for the color image. flags:
 * e_vulkan_render_target_flags. */
s_vulkan_render_target *vulkan_render_target_create(s_vulkan_backend *backend, uint32_t width, uint32_t height,
                                                    uint32_t color_format, uint32_t flags);
void vulkan_render_target_destroy(s_vulkan_backend *backend, s_vulkan_render_target *target);
void vulkan_render_target_get_info(const s_vulkan_render_target *target, struct s_vulkan_render_target_info *out);
VkImageView vulkan_render_target_color_view(const s_vulkan_render_target *target);
/* The pixels copied by the last vulkan_cmd_readback, once its frame has finished (vulkan_frame_end with wait):
 * readback_row_pitch bytes per row. */
const void *vulkan_render_target_read(s_vulkan_backend *backend, s_vulkan_render_target *target);

/* Writes 8-bit RGB pixels as a binary PPM (P6). format: VK_FORMAT_R8G8B8A8_* or VK_FORMAT_B8G8R8A8_* (alpha is
 * dropped). Returns 1 on success. */
int vulkan_write_ppm(const char *path, const void *pixels, uint32_t width, uint32_t height, uint32_t row_pitch,
                     uint32_t format);

/* ---- commands (recorded into the current frame) ---- */

enum e_vulkan_clear_flags {
    k_vulkan_clear_color = 1,
    k_vulkan_clear_depth = 2,
    k_vulkan_clear_stencil = 4,
};

struct s_vulkan_clear {
    uint32_t flags;            /* e_vulkan_clear_flags; attachments not cleared are loaded */
    uint32_t color_mask;       /* the color attachments k_vulkan_clear_color clears (bit i: attachment i; D3D's
                                * D3DCLEAR_TARGET0..3); vulkan_cmd_begin_rendering's one target ignores it */
    float color[4];
    float depth;
    uint32_t stencil;
};

/* Begins dynamic rendering into the target (clear NULL: load everything), with the viewport and scissor set to the
 * whole target. */
void vulkan_cmd_begin_rendering(s_vulkan_backend *backend, s_vulkan_render_target *target,
                                const struct s_vulkan_clear *clear);
void vulkan_cmd_end_rendering(s_vulkan_backend *backend);
/* Dynamic rendering into any images: up to VULKAN_MAX_COLOR_ATTACHMENTS color images (NULL entries are left unbound)
 * and an optional depth/stencil image (images created with the attachment usages). The render area, viewport and
 * scissor are the first attachment's size. clear: as vulkan_cmd_begin_rendering, the color attachments its color_mask
 * names. */
void vulkan_cmd_begin_rendering_images(s_vulkan_backend *backend, s_vulkan_image *const *colors, uint32_t color_count,
                                       s_vulkan_image *depth, const struct s_vulkan_clear *clear);
/* Inside rendering: clears rectangles (x0, y0, x1, y1 each; NULL: the whole render area) of the attachments
 * clear->flags names (color: the bound ones clear->color_mask names). */
void vulkan_cmd_clear_attachments(s_vulkan_backend *backend, const struct s_vulkan_clear *clear, const int32_t *rects,
                                  uint32_t rect_count);
/* Outside rendering: copies a rectangle of a color image into one level and layer of another (scaling and format
 * conversion through vkCmdBlitImage when the sizes or formats differ), then leaves the destination
 * SHADER_READ_ONLY_OPTIMAL for sampling. */
void vulkan_cmd_copy_image(s_vulkan_backend *backend, s_vulkan_image *source, int32_t source_x, int32_t source_y,
                           uint32_t width, uint32_t height, s_vulkan_image *destination, uint32_t destination_level,
                           uint32_t destination_layer, int32_t destination_x, int32_t destination_y,
                           uint32_t destination_width, uint32_t destination_height);
/* Outside rendering: copies a rectangle of a depth/stencil image's depth into another image of the same format (a
 * resolve of the depth buffer into a texture), then leaves the destination SHADER_READ_ONLY_OPTIMAL for sampling
 * through vulkan_image_sampled_view. */
void vulkan_cmd_copy_depth(s_vulkan_backend *backend, s_vulkan_image *source, int32_t source_x, int32_t source_y,
                           uint32_t width, uint32_t height, s_vulkan_image *destination, int32_t destination_x,
                           int32_t destination_y);
/* One rectangle of vulkan_cmd_copy_rects, in texels of both images. */
struct s_vulkan_copy_rect {
    int32_t source_x, source_y, destination_x, destination_y;
    uint32_t width, height;
};
/* Outside rendering: copies rectangles between two images of one format (every aspect both have: depth and stencil,
 * or color), level 0 and layer 0, in one copy command. A destination with SAMPLED use is left SHADER_READ_ONLY_OPTIMAL,
 * another (a depth attachment, a scratch image) TRANSFER_DST_OPTIMAL until its next use moves it. */
void vulkan_cmd_copy_rects(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination,
                           const struct s_vulkan_copy_rect *rects, uint32_t count);
/* The color image of an offscreen render target (for vulkan_cmd_begin_rendering_images and vulkan_cmd_copy_image). */
s_vulkan_image *vulkan_render_target_color_image(s_vulkan_render_target *target);
/* An image's description (width, height, format, ...). */
void vulkan_image_get_desc(const s_vulkan_image *image, struct s_vulkan_image_desc *out);
/* A D3D viewport (top-left origin, in pixels; depth range min..max). The shaders are D3D's (clip space y up; the pack
 * is compiled without -fvk-invert-y), so the backend flips the Vulkan viewport (negative height, core since 1.1):
 * images come out the right way up, and D3D's winding holds (clockwise = front, see vulkan_graphics_state_default). */
void vulkan_cmd_set_viewport(s_vulkan_backend *backend, float x, float y, float width, float height, float min_depth,
                             float max_depth);
void vulkan_cmd_set_scissor(s_vulkan_backend *backend, int32_t x, int32_t y, uint32_t width, uint32_t height);
/* Also binds the null vertex buffer when the pipeline reads it (xenos_vertex.h). A pipeline linked from libraries
 * sets the dynamic state it was linked with. */
void vulkan_cmd_bind_pipeline(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline);
/* As vulkan_cmd_bind_pipeline, but a pipeline linked from libraries sets the dynamic state of `state` (the draw's)
 * instead of its own: one linked pipeline serves every draw whose state differs from it only in dynamic state
 * (vulkan_graphics_state_static_key). */
void vulkan_cmd_bind_pipeline_state(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                    const struct s_vulkan_graphics_state *state);
/* Copies `state` with its dynamic state of a pipeline linked from libraries (cull mode, front face, depth bias, depth
 * and stencil test values) set to fixed values, for the key of such a pipeline. */
void vulkan_graphics_state_static_key(const struct s_vulkan_graphics_state *state, struct s_vulkan_graphics_state *out);
/* Descriptor set and index buffer binds that repeat what the frame command buffer has bound (the same set, layout and
 * dynamic offsets; the same buffer, offset and type) record nothing. */
void vulkan_cmd_bind_descriptor_set(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline, uint32_t set_index,
                                    VkDescriptorSet set);
/* As vulkan_cmd_bind_descriptor_set, with the offsets of the set's dynamic buffers (in binding order, at most
 * VULKAN_MAX_DYNAMIC_OFFSETS). */
#define VULKAN_MAX_DYNAMIC_OFFSETS 8u
void vulkan_cmd_bind_descriptor_set_dynamic(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                            uint32_t set_index, VkDescriptorSet set, uint32_t offset_count,
                                            const uint32_t *offsets);
/* Coalesced bind of set 0 (with dynamic offsets) and set 1 (no dynamic offsets) in one vkCmdBindDescriptorSets when
 * both differ from the command buffer's cached bindings. */
void vulkan_cmd_bind_descriptor_sets_pair(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                          VkDescriptorSet set0, uint32_t offset_count0, const uint32_t *offsets0,
                                          VkDescriptorSet set1);
/* A vertex stream (D3D stream = binding) with its stride in bytes. */
void vulkan_cmd_bind_vertex_buffer(s_vulkan_backend *backend, uint32_t binding, const s_vulkan_buffer *buffer,
                                   uint32_t offset, uint32_t stride);
/* index_type: VK_INDEX_TYPE_UINT16 or UINT32. */
void vulkan_cmd_bind_index_buffer(s_vulkan_backend *backend, const s_vulkan_buffer *buffer, uint32_t offset,
                                  uint32_t index_type);
/* Commands the frame command buffers recorded and the binds that were skipped as repeats, since the backend was made
 * (HALO3_FRAME_TRACE). */
struct s_vulkan_bind_counters {
    uint32_t pipelines, pipelines_skipped;   /* vkCmdBindPipeline */
    uint32_t dynamic_states;                 /* binds of a library pipeline whose dynamic state changed (vkCmdSet*) */
    uint32_t sets, sets_skipped;             /* vkCmdBindDescriptorSets */
    uint32_t index_buffers, index_buffers_skipped;
    uint32_t barriers;                       /* image barrier commands (vkCmdPipelineBarrier2) */
    uint32_t transitions_skipped;            /* a read-only layout to itself: no barrier needed */
};
void vulkan_backend_bind_counters(const s_vulkan_backend *backend, struct s_vulkan_bind_counters *out);
void vulkan_cmd_draw(s_vulkan_backend *backend, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                     uint32_t first_instance);
void vulkan_cmd_draw_indexed(s_vulkan_backend *backend, uint32_t index_count, uint32_t instance_count,
                             uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);
void vulkan_cmd_dispatch(s_vulkan_backend *backend, uint32_t groups_x, uint32_t groups_y, uint32_t groups_z);
/* EDRAM aliasing. Whether optimal-tiling images of the format can be storage images. */
uint32_t vulkan_format_storage(s_vulkan_backend *backend, uint32_t format);
/* Whether optimal-tiling images of the format can be blended color attachments and linearly filtered (the render
 * target formats the device picks for the Xenos 16-bit fixed-point EDRAM targets). */
uint32_t vulkan_format_blendable_filterable(s_vulkan_backend *backend, uint32_t format);
/* Writes source, a float image standing for a Xenos 2_10_10_10_FLOAT (7e3) render target, into destination
 * (A2B10G10R10_UNORM, same size, made with STORAGE use) as a 2_10_10_10 surface over the same EDRAM reads it: the 7e3
 * bits of each color as 10-bit unorm values (edram_7e3.hlsl). */
void vulkan_cmd_reinterpret_7e3(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination);
/* The Xenos 8_8_8_8 <-> 8_8_8_8_GAMMA conversion (pwl_convert.hlsl) of source into destination (made with storage use;
 * the overlapping size): encode != 0 writes the PWL gamma codes of source's linear RGB, 0 the PWL degamma of its codes.
 * Alpha is copied. Returns 0 (nothing recorded) without shaderStorageImageWriteWithoutFormat or storage use. */
uint32_t vulkan_cmd_convert_pwl(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination,
                                uint32_t encode);
/* HALO3_RES_SCALE (resolve_downscale.hlsl): writes each texel of destination (made with storage use, the source's
 * format) as the average of the N x N source texels it covers, N = source size / destination size (whole numbers).
 * Returns 0 (nothing recorded) without shaderStorageImageWriteWithoutFormat or storage use. */
uint32_t vulkan_cmd_downscale_image(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination);
/* depth_pack.hlsl: writes each texel of destination (R8G8B8A8_UNORM, storage use, identity view, the source's size)
 * as the bytes of the Xenos 24_8 word of source's texel (a sampled depth/stencil image): (depth << 8) | stencil, the
 * low byte in R; depth as 20e4 float (float_depth != 0, a 24_8_FLOAT surface's) or unorm24. Leaves destination
 * SHADER_READ_ONLY_OPTIMAL. Returns 0 (nothing recorded) when the images do not fit or the pipeline cannot be made. */
uint32_t vulkan_cmd_pack_depth_stencil(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination,
                                       uint32_t float_depth);
#ifdef HALO3_64BIT
uint32_t vulkan_cmd_gather_depth(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination,
                                 s_vulkan_buffer *parameters, uint32_t offset, uint32_t width, uint32_t height);
#endif
/* Occlusion queries (D3DQUERYTYPE_OCCLUSION): begin opens a query in the recording frame and returns its handle (the
 * frame's serial and a record index), 0 when none can be had; the draws until end count their passing samples, across
 * renderings (each rendering it spans takes a query slot). result: 1 with the count once the frame has finished on the
 * GPU (or when the frame is too old to know: 0 samples), 0 while pending. */
int vulkan_occlusion_begin(s_vulkan_backend *backend, uint32_t *out_serial, uint32_t *out_record);
void vulkan_occlusion_end(s_vulkan_backend *backend);
int vulkan_occlusion_result(s_vulkan_backend *backend, uint32_t serial, uint32_t record, uint64_t *out_samples);
/* The display controller's gamma (display_gamma.hlsl): writes source's colors through the ramp at ramp_offset of a
 * uniform buffer (k_vulkan_display_gamma_ramp_size bytes: uint4 header, header.x 0 none, 1 a 256-entry D3DGAMMARAMP,
 * 2 a 128-segment D3DPWLGAMMA; then uint4 table[256], red / green / blue in .xyz: ramp words, or PWL segments as
 * Base | Delta << 16) into destination (R8G8B8A8_UNORM, same size, made with storage use). */
#define k_vulkan_display_gamma_ramp_size (16u * 257u)
void vulkan_cmd_apply_display_gamma(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination,
                                    s_vulkan_buffer *ramp, uint32_t ramp_offset);
/* A resolve's exponent bias (resolve_scale.hlsl): writes the rectangle (x, y, width, height) of source scaled by a
 * factor into the same rectangle of destination (made with storage use: R16G16B16A16_SFLOAT, or, with
 * shaderStorageImageWriteWithoutFormat, R16G16_SFLOAT, R32G32_SFLOAT or R32G32B32A32_SFLOAT). The parameters are at
 * parameters_offset of a uniform buffer: int4 { x, y, width, height }, float4 { factor, clamp, low, high } (clamp
 * non-zero: the scaled colors are clamped to [low, high]). Returns 0 when the destination format cannot be written. */
#define k_vulkan_scale_image_parameters_size 32u
uint32_t vulkan_cmd_scale_image_rect(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination,
                                     s_vulkan_buffer *parameters, uint32_t parameters_offset, uint32_t width,
                                     uint32_t height);
/* Diagnostics: with HALO3_VULKAN_CHECKPOINTS=1 on a device with VK_NV_device_diagnostic_checkpoints, a checkpoint
 * marks the commands recorded after it, and on a device loss the backend logs the last checkpoints the queue
 * reached, each described through `describe` (the marker is the caller's). Without it these do nothing. */
void vulkan_cmd_checkpoint(s_vulkan_backend *backend, const void *marker);
void vulkan_backend_set_checkpoint_describe(s_vulkan_backend *backend, vulkan_checkpoint_describe_function describe,
                                            void *context);
/* A full memory barrier between all earlier and all later commands (compute results feeding draws, etc.), and the
 * host's reads of mapped buffers once the frame has finished. */
void vulkan_cmd_memory_barrier(s_vulkan_backend *backend);
/* Copies `size` bytes between two buffers (the destination made with VK_BUFFER_USAGE_TRANSFER_DST_BIT), outside any
 * rendering, between full memory barriers: what earlier commands wrote is copied, later commands see the copy. */
void vulkan_cmd_copy_buffer(s_vulkan_backend *backend, s_vulkan_buffer *source, uint32_t source_offset,
                            s_vulkan_buffer *destination, uint32_t destination_offset, uint32_t size);
/* Copies texels from a buffer into level 0, layer 0 of a 2D image, the whole width x height from (0, 0): rows of
 * row_length texels from offset (VkBufferImageCopy's bufferRowLength). Outside any rendering, between full memory
 * barriers: what earlier commands wrote into the buffer is copied, later commands see the image (left in
 * SHADER_READ_ONLY_OPTIMAL) and may write the buffer again. */
void vulkan_cmd_copy_buffer_to_image(s_vulkan_backend *backend, s_vulkan_buffer *buffer, uint32_t offset,
                                     uint32_t row_length, s_vulkan_image *image, uint32_t width, uint32_t height);
/* Copies a rectangle of one level and layer of an image into a buffer (tightly packed rows, from offset), made visible to
 * the host once the frame has finished; the image keeps its layout. */
void vulkan_cmd_copy_image_to_buffer(s_vulkan_backend *backend, s_vulkan_image *image, uint32_t level, uint32_t layer,
                                     int32_t x, int32_t y, uint32_t width, uint32_t height, s_vulkan_buffer *buffer,
                                     uint32_t offset);
/* Waits until the frame last recorded in a slot (backend->frame_index at the time) has finished on the GPU;
 * VK_NOT_READY for the slot being recorded (it is not submitted). */
VkResult vulkan_frame_wait(s_vulkan_backend *backend, uint32_t slot);
/* Copies the target's color image into its readback buffer (outside rendering). */
void vulkan_cmd_readback(s_vulkan_backend *backend, s_vulkan_render_target *target);

/* ---- presentation (optional) ---- */

/* One surface per backend, for the window system given at creation. */
VkResult vulkan_surface_create_win32(s_vulkan_backend *backend, void *hinstance, void *hwnd);
VkResult vulkan_surface_create_xlib(s_vulkan_backend *backend, void *display, unsigned long window);
VkResult vulkan_surface_create_xcb(s_vulkan_backend *backend, void *connection, uint32_t window);
VkResult vulkan_surface_create_wayland(s_vulkan_backend *backend, void *display, void *surface);
VkResult vulkan_surface_create_headless(s_vulkan_backend *backend);
void vulkan_surface_destroy(s_vulkan_backend *backend);
/* Creates (or re-creates, after VK_ERROR_OUT_OF_DATE_KHR) the swapchain; width and height are used when the surface
 * does not dictate its size. vsync: FIFO, otherwise MAILBOX or IMMEDIATE when available. */
VkResult vulkan_swapchain_create(s_vulkan_backend *backend, uint32_t width, uint32_t height, uint32_t vsync);
void vulkan_swapchain_destroy(s_vulkan_backend *backend);
/* Within a frame: acquires the next image (its index in *out_image_index if not NULL). VK_ERROR_OUT_OF_DATE_KHR: re-create
 * the swapchain and retry. */
VkResult vulkan_swapchain_acquire(s_vulkan_backend *backend, uint32_t *out_image_index);
/* Within a frame, after acquiring and outside rendering: scales the target's color image onto the acquired image and
 * leaves it ready to present. */
void vulkan_cmd_blit_to_swapchain(s_vulkan_backend *backend, s_vulkan_render_target *target);
/* After vulkan_frame_end: presents the image acquired in that frame. */
VkResult vulkan_swapchain_present(s_vulkan_backend *backend);

#ifdef __cplusplus
}
#endif

#endif /* PORT_D3D_VULKAN_BACKEND_H */
