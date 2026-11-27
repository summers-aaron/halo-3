#ifndef PORT_D3D_VULKAN_INTERNAL_H
#define PORT_D3D_VULKAN_INTERNAL_H

/*
 * The Vulkan backend's private state and tracked command recording interface. Host C++ only (no exceptions,
 * RTTI or dynamic initializers); keep Vulkan types on the host side of the game's MS-ABI calls.
 */

#include "vulkan_backend.h"
#include "gpu_backend.h"
#include "gpu_crt.h"
#include "../../shaders/xenos_spirv_abi.h"

#define VULKAN_MAX_FRAMES 4
#define VULKAN_MAX_SETS 4  /* descriptor sets per pipeline layout (the pack uses 0 and 1) */

/* A chain of descriptor pools: allocation moves to the next pool (creating it) when one is exhausted. */
struct s_vulkan_descriptor_pools {
    VkDescriptorPool *pools;
    uint32_t count;
    uint32_t capacity;
    uint32_t current;
    uint32_t textures; /* pools sized for the universal Xenos texture set (vulkan_descriptor_set_allocate_textures) */
};

#define VULKAN_TEXTURE_SETS_PER_POOL 64u

/* Objects a frame may still use, destroyed once its fence has been waited for (vulkan_buffer_retire,
 * vulkan_pipeline_replace). */
struct s_vulkan_retired {
    s_vulkan_buffer *buffer;
    s_vulkan_image *image;
    VkPipeline pipeline; /* a pipeline handle vulkan_pipeline_replace swapped out */
    VkDescriptorPool descriptor_pool; /* a pool of vulkan_texture_set_allocate's sets (vulkan_texture_sets_reset) */
};
struct s_vulkan_retire_list {
    struct s_vulkan_retired *items;
    uint32_t count, capacity;
};

/* Buffers the GPU is done with, kept for reuse: vulkan_buffer_create of the same size, usage and memory takes one
 * back instead of allocating. A buffer made and dropped every frame (an upload's staging buffer, a memexport copy
 * of memory the game places again each frame) then costs no driver memory allocation, whose kernel calls are a
 * large part of a frame at high frame rates. A pooled buffer unused for VULKAN_BUFFER_POOL_FRAMES frames is
 * destroyed, and the pool keeps at most VULKAN_BUFFER_POOL_BYTES (the oldest go first). */
#define VULKAN_BUFFER_POOL_CAPACITY 256u
#define VULKAN_BUFFER_POOL_BYTES (128u << 20)
#define VULKAN_BUFFER_POOL_FRAMES 240u

/* Each frame slot's staging ring: the staging memory of the uploads the frame records ahead of its commands (a new
 * texture's levels, a buffer's first contents), reused once the slot's fence has signalled. A texture the game first
 * draws mid-frame then costs no staging allocation (a driver call and fresh pages to fault in for every texture); an
 * upload larger than the space left gets its own staging buffer, as before. */
#define VULKAN_STAGING_RING_BYTES (32u << 20)
struct s_vulkan_pooled_buffer {
    struct s_vulkan_buffer *buffer;
    uint32_t serial; /* backend->frame_serial when it was pooled */
};

/* Occlusion queries (vulkan_occlusion_*): one pool per frame slot; a game query is a record of the query slots its
 * draws ran in (a rendering split by a copy or a compute pass ends the slot and begins another). */
#define VULKAN_OCCLUSION_SLOTS 1024u
#define VULKAN_OCCLUSION_RECORDS 256u
#define VULKAN_OCCLUSION_RECORD_SLOTS 8u
struct s_vulkan_occlusion_record {
    uint32_t slots[VULKAN_OCCLUSION_RECORD_SLOTS];
    uint32_t slot_count;
};

#define VULKAN_GPU_PASS_TRACE_QUERIES 256u
#define VULKAN_GPU_PASS_TRACE_MAX_SPANS 120u
struct s_vulkan_gpu_pass_span {
    const char *kind;
    uint32_t w, h, fmt, draws;
    uint32_t q_begin, q_end;
};

struct s_vulkan_frame {
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkCommandBuffer upload_command_buffer; /* first-use uploads recorded while the frame is open, submitted ahead of
                                             command_buffer in the same batch (vulkan_frame_end) */
    uint32_t upload_recording;           /* upload_command_buffer has been begun this frame */
    VkFence fence;                       /* signaled when the GPU has finished the slot's last submission */
    VkSemaphore acquire_semaphore;       /* signaled by vkAcquireNextImageKHR for this slot's swapchain image */
    struct s_vulkan_descriptor_pools transient_pools;
    struct s_vulkan_descriptor_pools texture_pools; /* vulkan_descriptor_set_allocate_textures (transient) */
    uint32_t acquired_image;             /* swapchain image acquired in this frame, or VULKAN_NO_IMAGE */
    struct s_vulkan_retire_list retired; /* destroyed when the slot is next begun */
    uint32_t serial;                     /* backend->frame_serial of the recording the slot holds */
    uint64_t bindless_serial;
    VkQueryPool occlusion_pool;          /* VULKAN_OCCLUSION_SLOTS occlusion queries, reset at the frame's begin */
    uint32_t occlusion_slots;            /* slots begun this frame */
    uint32_t occlusion_reset_count;      /* slots to reset on next begin (all on first use, then prev occlusion_slots) */
    uint32_t occlusion_records;          /* records made this frame */
    struct s_vulkan_occlusion_record records[VULKAN_OCCLUSION_RECORDS];
    /* the previous recording's results, read when the slot is next begun (its fence has signalled) */
    uint32_t results_serial;
    uint32_t result_count;
    uint64_t results[VULKAN_OCCLUSION_RECORDS]; /* samples per record */
    VkQueryPool timestamp_pool;          /* 2 timestamps (or 256 with HALO3_GPU_PASS_TRACE): frame GPU time */
    uint32_t timestamps_written;         /* the slot's last recording wrote them */
    uint32_t pass_trace_next_query;      /* next timestamp query index when HALO3_GPU_PASS_TRACE is on */
    uint32_t pass_trace_span_count;
    struct s_vulkan_gpu_pass_span *pass_trace_spans; /* allocated only when HALO3_GPU_PASS_TRACE is on */
    struct s_vulkan_buffer *staging_ring; /* VULKAN_STAGING_RING_BYTES of mapped upload memory for the uploads recorded
                                            in upload_command_buffer (made on first use, kept) */
    uint32_t staging_used;               /* its bytes taken this frame */
};

#define VULKAN_NO_IMAGE 0xffffffffu

/* The descriptor sets and dynamic offsets known at one bind point. A bind with another layout forgets the other
 * set numbers because an incompatible layout can disturb them. */
struct s_vulkan_bound_sets {
    VkPipelineLayout layout;
    uint32_t valid_mask;
    VkDescriptorSet sets[VULKAN_MAX_SETS];
    uint32_t offset_count[VULKAN_MAX_SETS];
    uint32_t offsets[VULKAN_MAX_SETS][VULKAN_MAX_DYNAMIC_OFFSETS];
};

struct s_vulkan_bound_vertex_buffer {
    VkBuffer buffer;
    VkDeviceSize offset, stride;
};

struct s_vulkan_persistent_set {
    VkDescriptorSet set;
    VkDescriptorPool pool;
};

struct s_vulkan_set_layout_entry {
    uint32_t hash;
    uint32_t binding_count;
    struct s_vulkan_layout_binding *bindings;  /* sorted by binding */
    VkDescriptorSetLayout layout;
};

struct s_vulkan_pipeline_layout_entry {
    uint32_t hash;
    uint32_t set_count;
    VkDescriptorSetLayout set_layouts[VULKAN_MAX_SETS];
    VkPushConstantRange push;
    VkPipelineLayout layout;
};

/* Dense entries with an open-addressing index (slot = entry index + 1, 0 = empty). */
struct s_vulkan_cache_index {
    uint32_t *slots;
    uint32_t slot_count;  /* a power of two, at least twice the entry count */
};

struct s_vulkan_buffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
    uint32_t size;
    uint32_t memory_usage;   /* e_vulkan_memory_usage */
    uint32_t coherent;
    void *mapped;
    uint32_t uploads;        /* vulkan_buffer_upload calls so far: the first one may be deferred to the frame's batch */
    uint32_t usage;          /* the VkBufferUsageFlags it was created with (buffer pool matching) */
};

#ifdef HALO3_64BIT
// Only optimal sampled images share these pages; buffers and attachments never alias them.
struct s_vulkan_texture_memory {
    VkDeviceMemory memory;
    uint32_t type, live;
    uint32_t used[16]; // 512 pages of 64 KiB (32 MiB)
    s_vulkan_texture_memory *next;
};
#endif
struct s_vulkan_bindless_token {
    uint32_t slot, epoch;
};

struct s_vulkan_image {
    VkImage image;
    VkDeviceMemory memory;
#ifdef HALO3_64BIT
    s_vulkan_texture_memory *texture_memory;
    uint32_t memory_page, memory_pages;
#endif
    VkImageView view;
    VkImageView sampled_view; /* depth/stencil formats with SAMPLED usage: the depth aspect alone (a sampled view
                                 may name one aspect); VK_NULL_HANDLE otherwise */
    VkImageView array_view;   /* 2D images sampled through a 2D_ARRAY binding (Xenos k2DOrStacked); created lazily */
    VkImageView stencil_view; /* depth/stencil formats with SAMPLED usage: the stencil aspect alone, read as unsigned
                                 integers (vulkan_cmd_pack_depth_stencil); created lazily */
    VkImageView swizzled_view; /* the image through another swizzle than desc.swizzle (vulkan_image_swizzled_view): a
                                  storage image's own view keeps the identity; created lazily */
    uint32_t swizzled_view_swizzle; /* VULKAN_SWIZZLE of swizzled_view */
    struct s_vulkan_image_desc desc;
    uint32_t aspect;         /* VkImageAspectFlags */
    uint32_t layout;         /* VkImageLayout of every subresource (the backend moves whole images) */
    uint32_t attachment_dirty; /* nonzero if written/cleared as an attachment since its last barrier */
    s_vulkan_bindless_token bindless_slots[4];
    s_vulkan_bindless_token bindless_sampled[4];
    s_vulkan_bindless_token bindless_array, bindless_swizzled;
};

struct s_vulkan_render_target {
    s_vulkan_image *color;
    s_vulkan_image *depth;   /* NULL without depth */
    s_vulkan_buffer *readback;
    uint32_t width;
    uint32_t height;
    uint32_t readback_row_pitch;
};

/* Draw state emitted by pipelines linked from graphics pipeline libraries. */
struct s_vulkan_dynamic_state {
    VkCullModeFlags cull_mode;
    VkFrontFace front_face;
    VkBool32 depth_test;
    VkBool32 depth_write;
    VkCompareOp depth_compare;
    VkBool32 depth_bias_enable;
    float depth_bias_constant;
    float depth_bias_slope;
    VkBool32 stencil_test;
    s_vulkan_stencil_face stencil_front;
    s_vulkan_stencil_face stencil_back;
};

struct s_vulkan_pipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;
    uint32_t bind_point;     /* VkPipelineBindPoint */
    uint32_t null_binding;   /* binding of the null vertex buffer, or 0xffffffff */
    uint32_t vertex_streams; /* bit s: the vertex shader reads stream s (graphics pipelines; the null buffer excluded) */
    uint32_t set_count;
    VkDescriptorSetLayout set_layouts[VULKAN_MAX_SETS];
    uint32_t dynamic;        /* linked from pipeline libraries: dynamic_state is set when it is bound */
    bool dynamic_viewport_scissor;
    bool dynamic_vertex_stride;
    s_vulkan_dynamic_state dynamic_state;
};

/* The backend's compute passes over images (vulkan_backend.cpp image_pass), made on first use. */
enum e_vulkan_image_pass {
    k_vulkan_image_pass_7e3,           /* vulkan_cmd_reinterpret_7e3 */
    k_vulkan_image_pass_display_gamma, /* vulkan_cmd_apply_display_gamma */
    k_vulkan_image_pass_resolve_scale, /* vulkan_cmd_scale_image_rect: R16G16B16A16_SFLOAT */
    k_vulkan_image_pass_resolve_scale_any, /* vulkan_cmd_scale_image_rect: the other float formats (no declared format) */
    k_vulkan_image_pass_pwl_decode,    /* vulkan_cmd_convert_pwl: gamma codes -> linear */
    k_vulkan_image_pass_pwl_encode,    /* vulkan_cmd_convert_pwl: linear -> gamma codes */
    k_vulkan_image_pass_downscale,     /* vulkan_cmd_downscale_image: N x N box average (HALO3_RES_SCALE) */
    k_vulkan_image_pass_depth_pack_20e4,   /* vulkan_cmd_pack_depth_stencil: 24_8_FLOAT depth */
    k_vulkan_image_pass_depth_pack_unorm24, /* vulkan_cmd_pack_depth_stencil: 24_8 depth */
#ifdef HALO3_64BIT
    k_vulkan_image_pass_depth_gather,
#endif
    k_vulkan_image_pass_count
};

struct s_vulkan_image_pass {
    VkShaderModule module;
    struct s_vulkan_pipeline *pipeline;
    uint32_t failed;
};

constexpr VkShaderStageFlags k_vulkan_xenos_push_stages =
    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT |
    VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

struct s_vulkan_bindless_slot {
    uint64_t identity;
    uint64_t last_use;
    uint16_t next;
};

struct s_vulkan_bindless_binding {
    s_vulkan_bindless_slot *slots;
    uint16_t *buckets;
    uint32_t next_unused;
};

struct s_vulkan_bindless {
    VkDescriptorSetLayout layout;
    VkDescriptorPool pool;
    VkDescriptorSet set;
    s_vulkan_bindless_binding bindings[5];
};

struct s_vulkan_backend {
    struct s_vulkan_functions vk;

    /* Hot command-recording state: kept contiguous right after `vk` so draw/pass recording stays in a few cache
       lines (away from cold device properties, the frame ring, and the 128 KiB checkpoint ring). */
    VkCommandBuffer command_buffer;   /* active frame's command_buffer while recording, 0 outside a frame */
    VkDevice device;
    uint32_t recording;
    uint32_t checkpoints;             /* HALO3_VULKAN_CHECKPOINTS: VK_NV_device_diagnostic_checkpoints is on */
    uint32_t rendering;
    uint32_t rendering_dirty;
    uint32_t rendering_width;         /* the current rendering's area and attachments (vulkan_cmd_clear_attachments) */
    uint32_t rendering_height;
    uint32_t rendering_epoch;
    uint32_t rendering_color_count;
    uint32_t rendering_has_depth;
    uint32_t rendering_has_stencil;
    uint32_t frame_index;             /* slot of the frame being (or last) recorded */
    uint32_t pass_trace_period;       /* HALO3_GPU_PASS_TRACE: 0 off, >0 dump every N frames (default 120 when =1) */
    uint32_t pass_trace_active_span;  /* index + 1 of currently open rendering pass span, 0 none */
    uint32_t dynamic_valid;           /* which parts of dynamic_state have been set on the command buffer */
    uint32_t bound_vertex_mask;       /* which vertex bindings have a known buffer, offset, and dynamic stride */
    uint32_t viewport_valid;
    uint32_t scissor_valid;
    mutable bool untracked_commands;  /* the raw frame command buffer was handed to another recorder */
    uint32_t synchronization2;
    uint32_t occlusion_active;        /* record index + 1 of the open occlusion query (this frame), 0 none */
    uint32_t occlusion_running;       /* its slot + 1 while a vkCmdBeginQuery is open in the rendering */
    uint32_t view_epoch;              /* vulkan_backend_view_epoch */
    /* Bindings and dynamic state known on the frame command buffer. */
    VkPipeline bound_pipelines[2];    /* by bind point: graphics, compute */
    VkPipelineLayout bound_pipeline_layouts[2];
    VkPipelineLayout pushed_layout;
    VkShaderStageFlags pushed_stages;
    uint8_t pushed_bytes[sizeof(xe_push_v6)];
    VkBuffer bound_index_buffer;      /* VK_NULL_HANDLE: none */
    VkDeviceSize bound_index_offset;
    VkIndexType bound_index_type;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineStageFlags2 shader_stages; /* enabled shader stages for barrier masks */
    struct s_vulkan_bind_counters bind_counters;
    struct s_vulkan_bound_sets bound_sets[2]; /* by bind point: graphics, compute */
    s_vulkan_dynamic_state dynamic_state;
    struct s_vulkan_bound_vertex_buffer bound_vertices[XENOS_VERTEX_MAX_STREAMS];
    s_vulkan_image *rendering_colors[VULKAN_MAX_COLOR_ATTACHMENTS];
    s_vulkan_image *rendering_depth;

    /* Cold device, setup, and cache state. */
    void *library;
    vulkan_log_function log;
    void *log_context;

    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
    VkPhysicalDevice physical_device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkQueue queue;
    uint32_t queue_family;
    uint32_t window_system;
    uint32_t presentation;
    uint32_t validation;
    uint32_t sampler_anisotropy;
    uint32_t sampler_mirror_clamp_to_edge; /* samplerMirrorClampToEdge is on: D3DTADDRESS_MIRRORONCE samples exactly */
    uint32_t storage_write_without_format; /* shaderStorageImageWriteWithoutFormat is on (vulkan_cmd_convert_pwl) */
    uint32_t occlusion_precise;          /* occlusionQueryPrecise is on: exact sample counts */
    uint32_t frame_serial;               /* recordings begun (vulkan_frame_begin) */
    uint32_t loss_reported;
    uint32_t buffer_addresses; /* with checkpoints: every buffer's device address is logged (vulkan: buffer ...) */
    /* the features translated Xenos shaders need (s_vulkan_backend_create_info::xenos_features) */
    uint32_t device_address;   /* bufferDeviceAddress is on: buffers get SHADER_DEVICE_ADDRESS (vulkan_buffer_device_address) */
    uint32_t tessellation;     /* tessellationShader is on */
    uint32_t geometry;         /* geometryShader is on */
    uint32_t clip_distance;    /* shaderClipDistance is on */
    uint32_t xenos_requested;  /* the creator asked for them */
    bool xenos_abi6_supported;
    s_vulkan_bindless bindless;
    uint32_t bindless_epoch;
    uint64_t bindless_revision;
    uint64_t bindless_serial, bindless_submitted, bindless_completed;
    uint32_t full_draw_index;  /* fullDrawIndexUint32 is on (32-bit index values above 2^24 - 1) */
    uint32_t pipeline_library; /* VK_EXT_graphics_pipeline_library is on (vulkan_pipeline_library_*) */
    uint32_t pipeline_library_fast_link; /* graphicsPipelineLibraryFastLinking */
    struct s_vulkan_descriptor_pools texture_set_pools; /* vulkan_texture_set_allocate */
    struct s_vulkan_descriptor_pools free_texture_pools; /* reset and reused instead of vkDestroyDescriptorPool */
    uint32_t rebar_upload;                        /* nonzero when a >=1 GiB DEVICE_LOCAL|HOST_VISIBLE|COHERENT heap exists */
    struct s_vulkan_image_pass image_passes[k_vulkan_image_pass_count]; /* the backend's own compute passes */
#ifdef HALO3_64BIT
    struct s_vulkan_buffer *depth_gather_buffer;
    s_vulkan_texture_memory *texture_memory;
#endif
    vulkan_checkpoint_describe_function checkpoint_describe;
    void *checkpoint_context;
    uint32_t command_serial;
    uint32_t depth_format;
    uint32_t allocation_count;
    uint32_t memory_allocations; /* vkAllocateMemory calls so far (HALO3_FRAME_STATS) */
    float timestamp_period;      /* nanoseconds per timestamp tick; 0: no GPU frame times */
    float gpu_frame_ms;          /* GPU time of the last finished frame command buffer (timestamps), -1: none */
    VkPipelineCache pipeline_cache;
    uint32_t pipelines_created; /* graphics and compute pipelines made through pipeline_cache (its contents grew) */

    /* Frame ring. */
    uint32_t frame_count;
    struct s_vulkan_frame frames[VULKAN_MAX_FRAMES];

    /* One-off submissions (uploads), waited for at once. */
    VkCommandPool immediate_pool;
    VkCommandBuffer immediate_command_buffer;
    VkFence immediate_fence;
    uint32_t immediate_submits; /* one-off submissions so far: each one stalls the CPU on its fence (HALO3_FRAME_STATS) */
    uint32_t deferred_uploads;  /* uploads recorded into a frame's upload command buffer instead (no CPU stall) */

    struct s_vulkan_pooled_buffer buffer_pool[VULKAN_BUFFER_POOL_CAPACITY]; /* oldest first */
    uint32_t buffer_pool_count;
    uint32_t buffer_pool_bytes;
    uint32_t buffers_reused;    /* vulkan_buffer_create calls served from the pool (HALO3_FRAME_STATS) */

    struct s_vulkan_descriptor_pools persistent_pools;
    struct s_vulkan_persistent_set *persistent_sets;  /* which pool each persistent set came from */
    uint32_t persistent_set_count;
    uint32_t persistent_set_capacity;

    struct s_vulkan_set_layout_entry *set_layouts;
    uint32_t set_layout_count;
    uint32_t set_layout_capacity;
    struct s_vulkan_cache_index set_layout_index;
    struct s_vulkan_pipeline_layout_entry *pipeline_layouts;
    uint32_t pipeline_layout_count;
    uint32_t pipeline_layout_capacity;
    struct s_vulkan_cache_index pipeline_layout_index;

    /* Shader pack module cache (vulkan_pack.cpp). */
    struct s_shader_pack *pack;
    VkShaderModule *pack_modules;
    const char **pack_entry_points;
    uint32_t *pack_output_masks;   /* per module: the color locations a fragment shader writes (vulkan_spirv_output_mask) */
    uint32_t pack_module_count;

    s_vulkan_buffer *null_vertex_buffer;  /* k_xenos_vertex_null_buffer_data, created on first use */

    /* Presentation. */
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    uint32_t swapchain_format;
    uint32_t swapchain_width;
    uint32_t swapchain_height;
    uint32_t swapchain_image_count;
    VkImage *swapchain_images;
    uint32_t *swapchain_layouts;
    VkSemaphore *present_semaphores;      /* per image: the frame that drew it signals, present waits */
    uint32_t present_image;               /* image of the last submitted frame to present, or VULKAN_NO_IMAGE */

    /* Diagnostics (HALO3_VULKAN_CHECKPOINTS only; kept at end of struct to stay out of hot cache lines). */
    const char *command_names[8192];  /* the backend's own checkpoints (one per command): marker 0x80000000 | serial */
    const void *command_markers[8192]; /* the caller's marker of a vulkan_cmd_checkpoint command */
};

VkPipelineLayout vulkan_pipeline_layout_with_push(s_vulkan_backend *backend, const VkDescriptorSetLayout *sets,
                                                  uint32_t count, const VkPushConstantRange &push);
bool vulkan_bindless_create(s_vulkan_backend *backend);
void vulkan_bindless_destroy(s_vulkan_backend *backend);
void vulkan_bindless_set_dummy(s_vulkan_backend *backend, uint32_t binding, uint64_t identity);
/* Returns UINT32_MAX when this command buffer must be submitted before a slot can be reused. */
uint32_t vulkan_bindless_slot(s_vulkan_backend *backend, uint32_t binding, uint64_t identity);

inline uint32_t vulkan_bindless_slot_fast(s_vulkan_backend *backend, uint32_t binding,
                                           uint64_t identity, s_vulkan_bindless_token &token)
{
    if (token.epoch == backend->bindless_epoch && token.slot != 0) {
        backend->bindless.bindings[binding].slots[token.slot].last_use = backend->bindless_serial;
        return token.slot;
    }
    // An unrelated eviction advances the epoch. Revalidate this index directly before probing the allocator.
    if (token.slot != 0) {
        auto &entry = backend->bindless.bindings[binding].slots[token.slot];
        if (entry.identity == identity) {
            entry.last_use = backend->bindless_serial;
            if (backend->bindless_epoch != 0)
                token.epoch = backend->bindless_epoch;
            return token.slot;
        }
    }
    const uint32_t slot = vulkan_bindless_slot(backend, binding, identity);
    if (slot != UINT32_MAX && backend->bindless_epoch != 0)
        token = { slot, backend->bindless_epoch };
    return slot;
}

/* Logs one line (printf subset: %s %c %d %i %u %x %X %p %%, with '0' flag and width); a '\n' is appended. */
void vulkan_log(s_vulkan_backend *backend, const char *format, ...);
/* FNV-1a over bytes, chained from hash (start with 2166136261). */
uint32_t vulkan_hash_bytes(uint32_t hash, const void *data, size_t size);
/* e_gpu_format (gpu_backend.h) to VkFormat and back (vulkan_gpu.cpp, the seam's Vulkan backend): VK_FORMAT_UNDEFINED /
 * k_gpu_format_undefined for a format the other side has no counterpart of. */
uint32_t vulkan_format_from_gpu(uint32_t format);
uint32_t gpu_format_from_vulkan(uint32_t format);

namespace vulkan_gpu_detail {

/* Non-dispatchable handles have the same representation in Vulkan and in the GPU seam. */
template <typename To, typename From> inline To handle_cast(From from)
{
    static_assert(sizeof(To) == sizeof(From), "64-bit handles");
    [[clang::uninitialized]] To to;
    memcpy(&to, &from, sizeof(to));
    return to;
}

} // namespace vulkan_gpu_detail

/* Use these only for objects from a GPU identified by vulkan_gpu_native_backend. */
inline s_vulkan_pipeline *vulkan_gpu_native_pipeline(s_gpu_pipeline *pipeline)
{
    return reinterpret_cast<s_vulkan_pipeline *>(pipeline);
}

inline const s_vulkan_pipeline *vulkan_gpu_native_pipeline(const s_gpu_pipeline *pipeline)
{
    return reinterpret_cast<const s_vulkan_pipeline *>(pipeline);
}

inline s_vulkan_buffer *vulkan_gpu_native_buffer(s_gpu_buffer *buffer)
{
    return reinterpret_cast<s_vulkan_buffer *>(buffer);
}

inline const s_vulkan_buffer *vulkan_gpu_native_buffer(const s_gpu_buffer *buffer)
{
    return reinterpret_cast<const s_vulkan_buffer *>(buffer);
}

inline VkDescriptorSet vulkan_gpu_native_bind_group(gpu_bind_group group)
{
    return vulkan_gpu_detail::handle_cast<VkDescriptorSet>(group);
}

/* Translate when the draw state changes. The resulting value can be reused across pipeline binds. */
void vulkan_gpu_translate_dynamic_state(const s_gpu_graphics_state *state, s_vulkan_dynamic_state *out);

namespace vulkan_recording {

[[gnu::cold]] VkCommandBuffer command_buffer_diagnostics(s_vulkan_backend *backend, const char *caller);

inline VkCommandBuffer recording_command_buffer(s_vulkan_backend *backend, const char *caller)
{
    const VkCommandBuffer command_buffer = backend->command_buffer;
    if (__builtin_expect(command_buffer == 0 || backend->checkpoints != 0, 0))
        return command_buffer_diagnostics(backend, caller);
    return command_buffer;
}

inline bool command_state_known(const s_vulkan_backend *backend)
{
    return backend->command_buffer != 0 && !backend->untracked_commands;
}

inline void invalidate_command_state(s_vulkan_backend *backend, bool preserve_sets = false)
{
    backend->dynamic_valid = 0;
    backend->bound_vertex_mask = 0;
    backend->viewport_valid = backend->scissor_valid = 0;
    backend->bound_pipelines[0] = backend->bound_pipelines[1] = VK_NULL_HANDLE;
    backend->bound_index_buffer = VK_NULL_HANDLE;
    if (!preserve_sets) {
        backend->bound_sets[0].valid_mask = backend->bound_sets[1].valid_mask = 0;
        backend->pushed_layout = VK_NULL_HANDLE;
    }
}

inline bool same_float(float a, float b)
{
    return __builtin_bit_cast(uint32_t, a) == __builtin_bit_cast(uint32_t, b);
}

inline uint32_t bind_point_index(uint32_t bind_point)
{
    return bind_point == VK_PIPELINE_BIND_POINT_COMPUTE ? 1u : 0u;
}

inline bool pipeline_is_bound(const s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline, uint32_t point)
{
    return command_state_known(backend) && backend->bound_pipelines[point] == pipeline->pipeline &&
           backend->bound_pipeline_layouts[point] == pipeline->layout &&
           (point == 1 || pipeline->dynamic_viewport_scissor);
}

void bind_pipeline_with_state(s_vulkan_backend *backend, VkCommandBuffer command_buffer,
                              const s_vulkan_pipeline *pipeline, uint32_t point, const s_vulkan_dynamic_state *state);

inline bool descriptor_set_is_bound(const s_vulkan_bound_sets &bound, uint32_t index, VkDescriptorSet set,
                                    uint32_t offset_count, const uint32_t *offsets)
{
    return index < VULKAN_MAX_SETS && (bound.valid_mask & (1u << index)) != 0 &&
           bound.sets[index] == set && bound.offset_count[index] == offset_count &&
           (offset_count == 0 || memcmp(bound.offsets[index], offsets, offset_count * sizeof(*offsets)) == 0);
}

void record_descriptor_sets(s_vulkan_backend *backend, VkCommandBuffer command_buffer, uint32_t bind_point,
                            VkPipelineLayout layout, uint32_t first, uint32_t count, const VkDescriptorSet *sets,
                            uint32_t offset_count, const uint32_t *offsets);

inline void bind_group(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline, uint32_t set_index,
                        VkDescriptorSet set, uint32_t offset_count, const uint32_t *offsets, const char *caller)
{
    const s_vulkan_bound_sets &bound = backend->bound_sets[bind_point_index(pipeline->bind_point)];
    if (command_state_known(backend) && bound.layout == pipeline->layout &&
        descriptor_set_is_bound(bound, set_index, set, offset_count, offsets)) {
        backend->bind_counters.sets_skipped++;
        return;
    }
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, caller);
    if (command_buffer != 0)
        record_descriptor_sets(backend, command_buffer, pipeline->bind_point, pipeline->layout, set_index, 1, &set,
                               offset_count, offsets);
}

inline void prepare_draw(s_vulkan_backend *backend)
{
    if (__builtin_expect(!backend->rendering_dirty, 0)) {
        backend->rendering_dirty = 1;
        for (uint32_t i = 0; i < backend->rendering_color_count && i < VULKAN_MAX_COLOR_ATTACHMENTS; i++)
            if (backend->rendering_colors[i] != 0)
                backend->rendering_colors[i]->attachment_dirty = 1;
        if (backend->rendering_depth != 0)
            backend->rendering_depth->attachment_dirty = 1;
    }
    if (__builtin_expect(backend->pass_trace_active_span != 0, 0))
        backend->frames[backend->frame_index].pass_trace_spans[backend->pass_trace_active_span - 1u].draws++;
}

inline void record_dispatch(s_vulkan_backend *backend, VkCommandBuffer command_buffer,
                            uint32_t groups_x, uint32_t groups_y, uint32_t groups_z)
{
    backend->vk.vkCmdDispatch(command_buffer, groups_x, groups_y, groups_z);
    if (backend->bindless.set == VK_NULL_HANDLE)
        invalidate_command_state(backend);
}

void dispatch_traced(s_vulkan_backend *backend, VkCommandBuffer command_buffer, uint32_t groups_x, uint32_t groups_y,
                     uint32_t groups_z);

} // namespace vulkan_recording

inline void vulkan_cmd_push_constants_inline(s_vulkan_backend *backend, VkPipelineLayout layout,
                                             VkShaderStageFlags stages, uint32_t offset, uint32_t size, const void *data)
{
    const bool full = offset == 0 && size == sizeof(backend->pushed_bytes);
    if (full && vulkan_recording::command_state_known(backend) && backend->pushed_layout == layout &&
        backend->pushed_stages == stages && memcmp(backend->pushed_bytes, data, size) == 0)
        return;
    const VkCommandBuffer cmd = vulkan_recording::recording_command_buffer(backend, "vulkan_cmd_push_constants");
    if (cmd == 0)
        return;
    backend->vk.vkCmdPushConstants(cmd, layout, stages, offset, size, data);
    backend->pushed_layout = full ? layout : VK_NULL_HANDLE;
    if (full) {
        backend->pushed_stages = stages;
        memcpy(backend->pushed_bytes, data, size);
    }
}

/* Same D3D coordinates as vulkan_cmd_set_viewport. */
inline void vulkan_cmd_set_viewport_inline(s_vulkan_backend *backend, float x, float y, float width, float height,
                                           float min_depth, float max_depth)
{
    using namespace vulkan_recording;
    // Flip clip-space Y for the D3D-convention shaders.
    const float vy = y + height, vh = -height;
    const VkViewport &last = backend->viewport;
    if (command_state_known(backend) && backend->viewport_valid &&
        same_float(last.x, x) && same_float(last.y, vy) &&
        same_float(last.width, width) && same_float(last.height, vh) &&
        same_float(last.minDepth, min_depth) && same_float(last.maxDepth, max_depth))
        return;
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_set_viewport");
    if (command_buffer == 0)
        return;
    backend->viewport = { x, vy, width, vh, min_depth, max_depth };
    backend->viewport_valid = 1;
    backend->vk.vkCmdSetViewport(command_buffer, 0, 1, &backend->viewport);
}

inline void vulkan_cmd_set_scissor_inline(s_vulkan_backend *backend, int32_t x, int32_t y, uint32_t width,
                                          uint32_t height)
{
    using namespace vulkan_recording;
    if (command_state_known(backend) && backend->scissor_valid &&
        backend->scissor.offset.x == x && backend->scissor.offset.y == y &&
        backend->scissor.extent.width == width && backend->scissor.extent.height == height)
        return;
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_set_scissor");
    if (command_buffer == 0)
        return;
    backend->scissor = { { x, y }, { width, height } };
    backend->scissor_valid = 1;
    backend->vk.vkCmdSetScissor(command_buffer, 0, 1, &backend->scissor);
}

/* NULL uses the linked pipeline's default dynamic state. Static pipelines ignore the state argument. */
inline void vulkan_cmd_bind_pipeline_native(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                            const s_vulkan_dynamic_state *state = 0)
{
    using namespace vulkan_recording;
    const uint32_t point = bind_point_index(pipeline->bind_point);
    if (!pipeline->dynamic && pipeline->null_binding == 0xffffffffu && pipeline_is_bound(backend, pipeline, point)) {
        backend->bind_counters.pipelines_skipped++;
        return;
    }
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_bind_pipeline");
    if (command_buffer != 0)
        bind_pipeline_with_state(backend, command_buffer, pipeline, point, state);
}

inline void vulkan_cmd_bind_group_native(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                        uint32_t set_index, VkDescriptorSet set, uint32_t offset_count,
                                        const uint32_t *offsets)
{
    vulkan_recording::bind_group(backend, pipeline, set_index, set, offset_count, offsets,
                                 "vulkan_cmd_bind_descriptor_set_dynamic");
}

/* The dynamic offsets belong to set 0, in binding order. Set 1 has no dynamic descriptors. */
inline void vulkan_cmd_bind_group_pair_native(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                             VkDescriptorSet set0, uint32_t offset_count0, const uint32_t *offsets0,
                                             VkDescriptorSet set1)
{
    using namespace vulkan_recording;
    const uint32_t bind_point = pipeline->bind_point;
    const VkPipelineLayout layout = pipeline->layout;
    const s_vulkan_bound_sets &bound = backend->bound_sets[bind_point_index(bind_point)];
    const bool same_layout = command_state_known(backend) && bound.layout == layout;
    const bool same0 = same_layout && descriptor_set_is_bound(bound, 0, set0, offset_count0, offsets0);
    const bool same1 = same_layout && descriptor_set_is_bound(bound, 1, set1, 0, 0);
    backend->bind_counters.sets_skipped += (uint32_t)same0 + (uint32_t)same1;
    if (same0 && same1)
        return;

    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_bind_descriptor_sets_pair");
    if (command_buffer == 0)
        return;
    if (same0) {
        record_descriptor_sets(backend, command_buffer, bind_point, layout, 1, 1, &set1, 0, 0);
    } else if (same1) {
        record_descriptor_sets(backend, command_buffer, bind_point, layout, 0, 1, &set0, offset_count0, offsets0);
    } else {
        const VkDescriptorSet sets[2] = { set0, set1 };
        record_descriptor_sets(backend, command_buffer, bind_point, layout, 0, 2, sets, offset_count0, offsets0);
    }
}

inline void vulkan_cmd_bind_vertex_buffer_native(s_vulkan_backend *backend, uint32_t binding,
                                                const s_vulkan_buffer *buffer, VkDeviceSize offset, VkDeviceSize stride)
{
    using namespace vulkan_recording;
    const uint32_t mask = binding < XENOS_VERTEX_MAX_STREAMS ? 1u << binding : 0u;
    if (command_state_known(backend) && (backend->bound_vertex_mask & mask) != 0) {
        const s_vulkan_bound_vertex_buffer &bound = backend->bound_vertices[binding];
        if (bound.buffer == buffer->buffer && bound.offset == offset && bound.stride == stride)
            return;
    }
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_bind_vertex_buffer");
    if (command_buffer == 0)
        return;
    backend->vk.vkCmdBindVertexBuffers2(command_buffer, binding, 1, &buffer->buffer, &offset, 0, &stride);
    if (mask != 0) {
        backend->bound_vertices[binding] = { buffer->buffer, offset, stride };
        backend->bound_vertex_mask |= mask;
    }
}

inline void vulkan_cmd_bind_index_buffer_native(s_vulkan_backend *backend, const s_vulkan_buffer *buffer,
                                               VkDeviceSize offset, VkIndexType index_type)
{
    using namespace vulkan_recording;
    if (__builtin_expect(command_state_known(backend) && backend->bound_index_buffer != VK_NULL_HANDLE &&
                         backend->bound_index_buffer == buffer->buffer &&
                         backend->bound_index_offset == offset && backend->bound_index_type == index_type,
                         1)) {
        backend->bind_counters.index_buffers_skipped++;
        return;
    }
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_bind_index_buffer");
    if (command_buffer == 0)
        return;
    backend->vk.vkCmdBindIndexBuffer(command_buffer, buffer->buffer, offset, index_type);
    backend->bound_index_buffer = buffer->buffer;
    backend->bound_index_offset = offset;
    backend->bound_index_type = index_type;
    backend->bind_counters.index_buffers++;
}

inline void vulkan_cmd_draw_inline(s_vulkan_backend *backend, uint32_t vertex_count, uint32_t instance_count,
                                   uint32_t first_vertex, uint32_t first_instance)
{
    using namespace vulkan_recording;
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_draw");
    if (__builtin_expect(command_buffer != 0, 1)) {
        prepare_draw(backend);
        backend->vk.vkCmdDraw(command_buffer, vertex_count, instance_count, first_vertex, first_instance);
    }
}

inline void vulkan_cmd_draw_indexed_inline(s_vulkan_backend *backend, uint32_t index_count, uint32_t instance_count,
                                           uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{
    using namespace vulkan_recording;
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_draw_indexed");
    if (__builtin_expect(command_buffer != 0, 1)) {
        prepare_draw(backend);
        backend->vk.vkCmdDrawIndexed(command_buffer, index_count, instance_count, first_index, vertex_offset,
                                     first_instance);
    }
}

inline void vulkan_cmd_dispatch_inline(s_vulkan_backend *backend, uint32_t groups_x, uint32_t groups_y, uint32_t groups_z)
{
    using namespace vulkan_recording;
    const VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_dispatch");
    if (command_buffer == 0)
        return;
    if (__builtin_expect(backend->pass_trace_period != 0, 0))
        dispatch_traced(backend, command_buffer, groups_x, groups_y, groups_z);
    else
        record_dispatch(backend, command_buffer, groups_x, groups_y, groups_z);
}

#endif /* PORT_D3D_VULKAN_INTERNAL_H */
