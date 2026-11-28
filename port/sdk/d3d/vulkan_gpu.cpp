// port/sdk/d3d: the Vulkan backend of the GPU seam (gpu_backend.h). Each operation is the vulkan_backend.h function of
// the same name with the seam's neutral types translated: e_gpu_* enums to Vulkan's, opaque objects and 64-bit handles
// cast to the backend's own. Host code (halo3_gpu); see vulkan_backend.h for what each Vulkan function does.
#include "vulkan_internal.h"
#include "../../shaders/shader_pack.h"
#include "../../shaders/xenos_spirv_abi.h"

namespace {

using vulkan_gpu_detail::handle_cast;

struct s_vulkan_gpu {
    s_gpu_backend base;               // first: the seam's s_gpu_backend * is this object
    s_vulkan_backend *vk;
    const s_shader_pack *xenos_pack;
    s_gpu_xenos_layout xenos;
    uint32_t memory_views_reported;
};

s_vulkan_backend *vk(s_gpu_backend *gpu) { return ((s_vulkan_gpu *)gpu)->vk; }
const s_vulkan_backend *vk(const s_gpu_backend *gpu) { return ((const s_vulkan_gpu *)gpu)->vk; }

// The seam's opaque objects are the backend's own.
s_vulkan_image *vk_image(s_gpu_image *image) { return (s_vulkan_image *)image; }
const s_vulkan_image *vk_image(const s_gpu_image *image) { return (const s_vulkan_image *)image; }
s_vulkan_render_target *vk_target(s_gpu_render_target *target) { return (s_vulkan_render_target *)target; }
const s_vulkan_render_target *vk_target(const s_gpu_render_target *target) { return (const s_vulkan_render_target *)target; }

// ---- enums ----

constexpr VkFormat k_vk_formats[] = {
    VK_FORMAT_UNDEFINED,
    VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_SNORM, VK_FORMAT_R8G8B8A8_USCALED, VK_FORMAT_R8G8B8A8_SSCALED,
    VK_FORMAT_R8G8B8A8_UINT, VK_FORMAT_R8G8B8A8_SINT, VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM, VK_FORMAT_B8G8R8A8_USCALED, VK_FORMAT_B8G8R8A8_SSCALED,
    VK_FORMAT_B8G8R8A8_UINT, VK_FORMAT_B8G8R8A8_SINT, VK_FORMAT_B8G8R8A8_SRGB,
    VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_FORMAT_A2B10G10R10_SNORM_PACK32, VK_FORMAT_A2B10G10R10_USCALED_PACK32,
    VK_FORMAT_A2B10G10R10_SSCALED_PACK32, VK_FORMAT_A2B10G10R10_UINT_PACK32, VK_FORMAT_A2B10G10R10_SINT_PACK32,
    VK_FORMAT_R16G16_UNORM, VK_FORMAT_R16G16_SNORM, VK_FORMAT_R16G16_USCALED, VK_FORMAT_R16G16_SSCALED,
    VK_FORMAT_R16G16_UINT, VK_FORMAT_R16G16_SINT, VK_FORMAT_R16G16_SFLOAT,
    VK_FORMAT_R16G16B16A16_UNORM, VK_FORMAT_R16G16B16A16_SNORM, VK_FORMAT_R16G16B16A16_USCALED,
    VK_FORMAT_R16G16B16A16_SSCALED, VK_FORMAT_R16G16B16A16_UINT, VK_FORMAT_R16G16B16A16_SINT,
    VK_FORMAT_R16G16B16A16_SFLOAT,
    VK_FORMAT_R32_UINT, VK_FORMAT_R32_SINT, VK_FORMAT_R32_SFLOAT,
    VK_FORMAT_R32G32_UINT, VK_FORMAT_R32G32_SINT, VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R32G32B32_UINT, VK_FORMAT_R32G32B32_SINT, VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32A32_UINT, VK_FORMAT_R32G32B32A32_SINT, VK_FORMAT_R32G32B32A32_SFLOAT,
    VK_FORMAT_R8_UNORM, VK_FORMAT_R8_SNORM,
    VK_FORMAT_R8G8_UNORM, VK_FORMAT_R8G8_SNORM,
    VK_FORMAT_R16_UNORM, VK_FORMAT_R16_SNORM, VK_FORMAT_R16_SFLOAT,
    VK_FORMAT_B4G4R4A4_UNORM_PACK16, VK_FORMAT_B5G6R5_UNORM_PACK16, VK_FORMAT_A1R5G5B5_UNORM_PACK16,
    VK_FORMAT_BC1_RGBA_UNORM_BLOCK, VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
    VK_FORMAT_BC2_UNORM_BLOCK, VK_FORMAT_BC2_SRGB_BLOCK,
    VK_FORMAT_BC3_UNORM_BLOCK, VK_FORMAT_BC3_SRGB_BLOCK,
    VK_FORMAT_BC4_UNORM_BLOCK, VK_FORMAT_BC5_UNORM_BLOCK, VK_FORMAT_BC5_SNORM_BLOCK,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT_S8_UINT,
};
static_assert(sizeof(k_vk_formats) / sizeof(k_vk_formats[0]) == k_gpu_format_count, "a VkFormat per e_gpu_format");

struct s_gpu_formats_by_vk {
    uint8_t formats[256];
};

constexpr s_gpu_formats_by_vk make_gpu_formats_by_vk()
{
    static_assert(k_gpu_format_undefined == 0 && k_gpu_format_count <= 256, "reverse format table entries fit in a byte");
    s_gpu_formats_by_vk table = {};
    for (uint32_t i = 1; i < k_gpu_format_count; i++) {
        const uint32_t format = (uint32_t)k_vk_formats[i];
        if (format != VK_FORMAT_UNDEFINED && format < sizeof(table.formats) &&
            table.formats[format] == k_gpu_format_undefined)
            table.formats[format] = (uint8_t)i;
    }
    return table;
}
constexpr s_gpu_formats_by_vk k_gpu_formats_by_vk = make_gpu_formats_by_vk();

const VkBlendFactor k_vk_blend_factor[15] = {
    VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_SRC_COLOR, VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    VK_BLEND_FACTOR_DST_COLOR, VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR, VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_FACTOR_DST_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    VK_BLEND_FACTOR_CONSTANT_COLOR, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR, VK_BLEND_FACTOR_CONSTANT_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA, VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
};
const VkBlendOp k_vk_blend_op[5] = {
    VK_BLEND_OP_ADD, VK_BLEND_OP_SUBTRACT, VK_BLEND_OP_REVERSE_SUBTRACT, VK_BLEND_OP_MIN, VK_BLEND_OP_MAX,
};
const VkPrimitiveTopology k_vk_topology[7] = {
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
    VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
};
const VkImageViewType k_vk_view_type[5] = {
    VK_IMAGE_VIEW_TYPE_2D, // none: never made
    VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_VIEW_TYPE_3D, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_VIEW_TYPE_2D_ARRAY,
};
const VkSamplerAddressMode k_vk_address[5] = {
    VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
};
const VkBorderColor k_vk_border[3] = {
    VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
};
const VkComponentSwizzle k_vk_swizzle[7] = {
    VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ONE, VK_COMPONENT_SWIZZLE_R,
    VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
};
const VkDescriptorType k_vk_descriptor[6] = {
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_SAMPLER,
};

template <typename T, size_t N> T pick(const T (&table)[N], uint32_t value, T fallback)
{
    return value < N ? table[value] : fallback;
}

static_assert((uint32_t)k_gpu_compare_never == (uint32_t)VK_COMPARE_OP_NEVER &&
              (uint32_t)k_gpu_compare_always == (uint32_t)VK_COMPARE_OP_ALWAYS &&
              (uint32_t)k_gpu_stencil_keep == (uint32_t)VK_STENCIL_OP_KEEP &&
              (uint32_t)k_gpu_stencil_decrement_wrap == (uint32_t)VK_STENCIL_OP_DECREMENT_AND_WRAP,
              "GPU compare and stencil ops use Vulkan's enum order");

VkCompareOp vk_compare(uint32_t op)
{
    return op <= VK_COMPARE_OP_ALWAYS ? (VkCompareOp)op : VK_COMPARE_OP_ALWAYS;
}
uint32_t vk_stencil(uint32_t op) { return op <= VK_STENCIL_OP_DECREMENT_AND_WRAP ? op : VK_STENCIL_OP_KEEP; }
uint32_t vk_cull(uint32_t mode)
{
    return mode == k_gpu_cull_front ? VK_CULL_MODE_FRONT_BIT : mode == k_gpu_cull_back ? VK_CULL_MODE_BACK_BIT
                                                                                        : VK_CULL_MODE_NONE;
}
VkFrontFace vk_front_face(uint32_t face)
{
    return face == k_gpu_front_face_counter_clockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
}
uint32_t vk_polygon(uint32_t mode)
{
    return mode == k_gpu_polygon_line ? VK_POLYGON_MODE_LINE : mode == k_gpu_polygon_point ? VK_POLYGON_MODE_POINT
                                                                                             : VK_POLYGON_MODE_FILL;
}
uint32_t vk_sample_count(uint32_t count)
{
    return count == 0 ? 0u : count == 2 ? VK_SAMPLE_COUNT_2_BIT : count == 4 ? VK_SAMPLE_COUNT_4_BIT : VK_SAMPLE_COUNT_1_BIT;
}
uint32_t vk_filter(uint32_t filter) { return filter == k_gpu_filter_linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST; }
uint32_t vk_mipmap(uint32_t filter)
{
    return filter == k_gpu_filter_linear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
}
uint32_t vk_stages(uint32_t stages)
{
    uint32_t out = 0;
    out |= (stages & k_gpu_stage_vertex) != 0 ? (uint32_t)VK_SHADER_STAGE_VERTEX_BIT : 0u;
    out |= (stages & k_gpu_stage_tess_control) != 0 ? (uint32_t)VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT : 0u;
    out |= (stages & k_gpu_stage_tess_eval) != 0 ? (uint32_t)VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT : 0u;
    out |= (stages & k_gpu_stage_geometry) != 0 ? (uint32_t)VK_SHADER_STAGE_GEOMETRY_BIT : 0u;
    out |= (stages & k_gpu_stage_fragment) != 0 ? (uint32_t)VK_SHADER_STAGE_FRAGMENT_BIT : 0u;
    out |= (stages & k_gpu_stage_compute) != 0 ? (uint32_t)VK_SHADER_STAGE_COMPUTE_BIT : 0u;
    return out;
}
uint32_t vk_image_usage(uint32_t usage)
{
    uint32_t out = 0;
    out |= (usage & k_gpu_image_transfer_src) != 0 ? (uint32_t)VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0u;
    out |= (usage & k_gpu_image_transfer_dst) != 0 ? (uint32_t)VK_IMAGE_USAGE_TRANSFER_DST_BIT : 0u;
    out |= (usage & k_gpu_image_sampled) != 0 ? (uint32_t)VK_IMAGE_USAGE_SAMPLED_BIT : 0u;
    out |= (usage & k_gpu_image_storage) != 0 ? (uint32_t)VK_IMAGE_USAGE_STORAGE_BIT : 0u;
    out |= (usage & k_gpu_image_color_attachment) != 0 ? (uint32_t)VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0u;
    out |= (usage & k_gpu_image_depth_stencil_attachment) != 0 ? (uint32_t)VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                                                                 : 0u;
    return out;
}
uint32_t gpu_image_usage_of(uint32_t usage)
{
    uint32_t out = 0;
    out |= (usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0 ? (uint32_t)k_gpu_image_transfer_src : 0u;
    out |= (usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0 ? (uint32_t)k_gpu_image_transfer_dst : 0u;
    out |= (usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0 ? (uint32_t)k_gpu_image_sampled : 0u;
    out |= (usage & VK_IMAGE_USAGE_STORAGE_BIT) != 0 ? (uint32_t)k_gpu_image_storage : 0u;
    out |= (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0 ? (uint32_t)k_gpu_image_color_attachment : 0u;
    out |= (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0 ? (uint32_t)k_gpu_image_depth_stencil_attachment : 0u;
    return out;
}
uint32_t vk_buffer_usage(uint32_t usage)
{
    uint32_t out = 0;
    out |= (usage & k_gpu_buffer_transfer_src) != 0 ? (uint32_t)VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0u;
    out |= (usage & k_gpu_buffer_transfer_dst) != 0 ? (uint32_t)VK_BUFFER_USAGE_TRANSFER_DST_BIT : 0u;
    out |= (usage & k_gpu_buffer_uniform) != 0 ? (uint32_t)VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : 0u;
    out |= (usage & k_gpu_buffer_storage) != 0 ? (uint32_t)VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : 0u;
    out |= (usage & k_gpu_buffer_index) != 0 ? (uint32_t)VK_BUFFER_USAGE_INDEX_BUFFER_BIT : 0u;
    out |= (usage & k_gpu_buffer_vertex) != 0 ? (uint32_t)VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : 0u;
    return out;
}
uint32_t vk_memory_usage(uint32_t memory)
{
    return memory == k_gpu_memory_upload     ? (uint32_t)k_vulkan_memory_upload
           : memory == k_gpu_memory_readback ? (uint32_t)k_vulkan_memory_readback
                                             : (uint32_t)k_vulkan_memory_gpu;
}
uint32_t vk_swizzle(uint32_t swizzle)
{
    uint32_t out = 0;
    for (uint32_t c = 0; c < 4; c++)
        out |= (uint32_t)pick(k_vk_swizzle, (swizzle >> (8u * c)) & 0xffu, VK_COMPONENT_SWIZZLE_IDENTITY) << (8u * c);
    return out;
}
uint32_t gpu_swizzle_of(uint32_t swizzle)
{
    // VkComponentSwizzle IDENTITY .. A and e_gpu_swizzle identity .. a are the same sequence
    uint32_t out = 0;
    for (uint32_t c = 0; c < 4; c++) {
        const uint32_t v = (swizzle >> (8u * c)) & 0xffu;
        uint32_t g = k_gpu_swizzle_identity;
        for (uint32_t i = 0; i < 7; i++)
            if ((uint32_t)k_vk_swizzle[i] == v)
                g = i;
        out |= g << (8u * c);
    }
    return out;
}
uint32_t gpu_view_type_of(uint32_t view_type)
{
    switch (view_type) {
    case VK_IMAGE_VIEW_TYPE_3D: return k_gpu_view_3d;
    case VK_IMAGE_VIEW_TYPE_CUBE: return k_gpu_view_cube;
    case VK_IMAGE_VIEW_TYPE_2D_ARRAY: return k_gpu_view_2d_array;
    case VK_IMAGE_VIEW_TYPE_2D: return k_gpu_view_2d;
    default: return k_gpu_view_none;
    }
}

gpu_result gpu_result_of(VkResult result)
{
    switch (result) {
    case VK_SUCCESS: return k_gpu_ok;
    case VK_NOT_READY: return k_gpu_not_ready;
    case VK_INCOMPLETE: return k_gpu_incomplete;
    case VK_SUBOPTIMAL_KHR: return k_gpu_suboptimal;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return k_gpu_out_of_memory;
    case VK_ERROR_DEVICE_LOST: return k_gpu_device_lost;
    case VK_ERROR_OUT_OF_DATE_KHR: return k_gpu_out_of_date;
    case VK_ERROR_SURFACE_LOST_KHR: return k_gpu_surface_lost;
    default: return k_gpu_error;
    }
}

// ---- structures ----

void vk_stencil_face(const s_gpu_stencil_face &in, s_vulkan_stencil_face *out)
{
    out->fail_op = vk_stencil(in.fail_op);
    out->pass_op = vk_stencil(in.pass_op);
    out->depth_fail_op = vk_stencil(in.depth_fail_op);
    out->compare_op = vk_compare(in.compare_op);
    out->compare_mask = in.compare_mask;
    out->write_mask = in.write_mask;
    out->reference = in.reference;
}

// The seam's per-draw adapter translates only the fields Vulkan will consume. Cached state is fully initialized.
template <typename State>
void vk_dynamic_state(const s_gpu_graphics_state &in, State *out, bool active_only = false)
{
    out->cull_mode = vk_cull(in.cull_mode);
    out->front_face = vk_front_face(in.front_face);
    out->depth_test = in.depth_test;
    if (!active_only || in.depth_test != 0) {
        out->depth_write = in.depth_write;
        out->depth_compare = vk_compare(in.depth_compare);
    }
    out->depth_bias_enable = in.depth_bias_enable;
    if (!active_only || in.depth_bias_enable != 0) {
        out->depth_bias_constant = in.depth_bias_constant;
        out->depth_bias_slope = in.depth_bias_slope;
    }
    out->stencil_test = in.stencil_test;
    if (!active_only || in.stencil_test != 0) {
        vk_stencil_face(in.stencil_front, &out->stencil_front);
        vk_stencil_face(in.stencil_back, &out->stencil_back);
    }
}

void vk_state(const s_gpu_graphics_state &in, s_vulkan_graphics_state *out)
{
    memset(out, 0, sizeof(*out));
    out->topology = (uint32_t)pick(k_vk_topology, in.topology, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    out->primitive_restart = in.primitive_restart;
    out->polygon_mode = vk_polygon(in.polygon_mode);
    vk_dynamic_state(in, out);
    out->sample_count = vk_sample_count(in.sample_count);
    out->alpha_to_coverage = in.alpha_to_coverage;
    out->color_attachment_count = in.color_attachment_count;
    for (uint32_t i = 0; i < GPU_MAX_COLOR_ATTACHMENTS; i++) {
        out->color_formats[i] = vulkan_format_from_gpu(in.color_formats[i]);
        const s_gpu_blend_attachment &b = in.blend[i];
        s_vulkan_blend_attachment &o = out->blend[i];
        o.enable = b.enable;
        o.src_color = (uint32_t)pick(k_vk_blend_factor, b.src_color, VK_BLEND_FACTOR_ONE);
        o.dst_color = (uint32_t)pick(k_vk_blend_factor, b.dst_color, VK_BLEND_FACTOR_ZERO);
        o.color_op = (uint32_t)pick(k_vk_blend_op, b.color_op, VK_BLEND_OP_ADD);
        o.src_alpha = (uint32_t)pick(k_vk_blend_factor, b.src_alpha, VK_BLEND_FACTOR_ONE);
        o.dst_alpha = (uint32_t)pick(k_vk_blend_factor, b.dst_alpha, VK_BLEND_FACTOR_ZERO);
        o.alpha_op = (uint32_t)pick(k_vk_blend_op, b.alpha_op, VK_BLEND_OP_ADD);
        o.write_mask = (b.write_mask & GPU_COLOR_WRITE_R ? (uint32_t)VK_COLOR_COMPONENT_R_BIT : 0u) |
                       (b.write_mask & GPU_COLOR_WRITE_G ? (uint32_t)VK_COLOR_COMPONENT_G_BIT : 0u) |
                       (b.write_mask & GPU_COLOR_WRITE_B ? (uint32_t)VK_COLOR_COMPONENT_B_BIT : 0u) |
                       (b.write_mask & GPU_COLOR_WRITE_A ? (uint32_t)VK_COLOR_COMPONENT_A_BIT : 0u);
        out->color_exp_bias[i] = in.color_exp_bias[i];
    }
    out->depth_format = vulkan_format_from_gpu(in.depth_format);
    out->stencil_format = vulkan_format_from_gpu(in.stencil_format);
    out->fragcoord_offset[0] = in.fragcoord_offset[0];
    out->fragcoord_offset[1] = in.fragcoord_offset[1];
    for (uint32_t i = 0; i < 4; i++)
        out->blend_constants[i] = in.blend_constants[i];
}

void vk_image_desc(const s_gpu_image_desc &in, s_vulkan_image_desc *out)
{
    out->view_type = (uint32_t)pick(k_vk_view_type, in.view_type, VK_IMAGE_VIEW_TYPE_2D);
    out->format = vulkan_format_from_gpu(in.format);
    out->width = in.width;
    out->height = in.height;
    out->depth = in.depth;
    out->mip_levels = in.mip_levels;
    out->array_layers = in.array_layers;
    out->usage = vk_image_usage(in.usage);
    out->swizzle = vk_swizzle(in.swizzle);
}

void vk_shader_stages(const s_gpu_shader_stage *stages, uint32_t count, s_vulkan_shader_stage *out)
{
    for (uint32_t i = 0; i < count; i++) {
        out[i].stage = vk_stages(stages[i].stage);
        out[i].module = handle_cast<VkShaderModule>(stages[i].module);
        out[i].entry = stages[i].entry;
    }
}

const uint32_t k_max_stages = 5;

// ---- the operations ----

void op_destroy(s_gpu_backend *gpu)
{
    s_vulkan_gpu *self = (s_vulkan_gpu *)gpu;
    vulkan_backend_destroy(self->vk);
    free(self);
}

gpu_result op_wait_idle(s_gpu_backend *gpu) { return gpu_result_of(vulkan_backend_wait_idle(vk(gpu))); }

gpu_result op_get_pipeline_cache(s_gpu_backend *gpu, void *data, size_t *size)
{
    return gpu_result_of(vulkan_backend_get_pipeline_cache(vk(gpu), data, size));
}

uint32_t op_pipelines_created(const s_gpu_backend *gpu) { return vulkan_backend_pipelines_created(vk(gpu)); }
uint32_t op_frame_index(const s_gpu_backend *gpu) { return vk(gpu)->frame_index; }
uint32_t op_view_epoch(const s_gpu_backend *gpu) { return vulkan_backend_view_epoch(vk(gpu)); }

void op_get_stats(const s_gpu_backend *gpu, s_gpu_stats *out)
{
    const s_vulkan_backend *v = vk(gpu);
    s_vulkan_bind_counters b;
    vulkan_backend_bind_counters(v, &b);
    out->immediate_submits = v->immediate_submits;
    out->deferred_uploads = v->deferred_uploads;
    out->memory_allocations = v->memory_allocations;
    out->buffers_reused = v->buffers_reused;
    out->gpu_frame_ms = v->gpu_frame_ms;
    out->pipelines = b.pipelines;
    out->pipelines_skipped = b.pipelines_skipped;
    out->dynamic_states = b.dynamic_states;
    out->sets = b.sets;
    out->sets_skipped = b.sets_skipped;
    out->index_buffers = b.index_buffers;
    out->index_buffers_skipped = b.index_buffers_skipped;
    out->barriers = b.barriers;
    out->transitions_skipped = b.transitions_skipped;
}

int op_rendering(const s_gpu_backend *gpu, uint32_t *out_width, uint32_t *out_height)
{
    const s_vulkan_backend *v = vk(gpu);
    if (out_width != 0)
        *out_width = v->rendering_width;
    if (out_height != 0)
        *out_height = v->rendering_height;
    return v->rendering != 0;
}

int op_occlusion_active(const s_gpu_backend *gpu) { return vk(gpu)->occlusion_active != 0; }

void op_set_checkpoint_describe(s_gpu_backend *gpu, gpu_checkpoint_describe_function describe, void *context)
{
    vulkan_backend_set_checkpoint_describe(vk(gpu), describe, context);
}

uint32_t op_format_storage(s_gpu_backend *gpu, uint32_t format)
{
    return vulkan_format_storage(vk(gpu), vulkan_format_from_gpu(format));
}

uint32_t op_format_blendable_filterable(s_gpu_backend *gpu, uint32_t format)
{
    return vulkan_format_blendable_filterable(vk(gpu), vulkan_format_from_gpu(format));
}

// buffers

s_gpu_buffer *op_buffer_create(s_gpu_backend *gpu, uint32_t size, uint32_t usage, uint32_t memory_usage)
{
    return (s_gpu_buffer *)vulkan_buffer_create(vk(gpu), size, vk_buffer_usage(usage), vk_memory_usage(memory_usage));
}
void op_buffer_destroy(s_gpu_backend *gpu, s_gpu_buffer *buffer)
{
    vulkan_buffer_destroy(vk(gpu), vulkan_gpu_native_buffer(buffer));
}
void op_buffer_retire(s_gpu_backend *gpu, s_gpu_buffer *buffer)
{
    vulkan_buffer_retire(vk(gpu), vulkan_gpu_native_buffer(buffer));
}
void *op_buffer_mapped(const s_gpu_backend *, const s_gpu_buffer *buffer)
{
    return vulkan_buffer_mapped(vulkan_gpu_native_buffer(buffer));
}
void op_buffer_invalidate(s_gpu_backend *gpu, s_gpu_buffer *buffer)
{
    vulkan_buffer_invalidate(vk(gpu), vulkan_gpu_native_buffer(buffer));
}
gpu_result op_buffer_download(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, void *data, uint32_t size)
{
    return gpu_result_of(vulkan_buffer_download(vk(gpu), vulkan_gpu_native_buffer(buffer), offset, data, size));
}
gpu_result op_buffer_upload(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, const void *data, uint32_t size)
{
    return gpu_result_of(vulkan_buffer_upload(vk(gpu), vulkan_gpu_native_buffer(buffer), offset, data, size));
}
void op_buffer_device_address(const s_gpu_backend *gpu, const s_gpu_buffer *buffer, uint32_t *out_lo, uint32_t *out_hi)
{
    vulkan_buffer_device_address(vk(gpu), vulkan_gpu_native_buffer(buffer), out_lo, out_hi);
}

// images, samplers

s_gpu_image *op_image_create(s_gpu_backend *gpu, const s_gpu_image_desc *desc)
{
    s_vulkan_image_desc d;
    vk_image_desc(*desc, &d);
#ifdef HALO3_64BIT
    // The EDRAM depth gather reads depth/stencil surfaces through sampled views.
    if (d.format == VK_FORMAT_D32_SFLOAT_S8_UINT && (d.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0 &&
        (d.usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0 && (d.usage & VK_IMAGE_USAGE_SAMPLED_BIT) == 0 &&
        (vulkan_backend_format_features(vk(gpu), d.format, 0) & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0)
        d.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
#endif
    return (s_gpu_image *)vulkan_image_create(vk(gpu), &d);
}
void op_image_destroy(s_gpu_backend *gpu, s_gpu_image *image) { vulkan_image_destroy(vk(gpu), vk_image(image)); }
void op_image_retire(s_gpu_backend *gpu, s_gpu_image *image) { vulkan_image_retire(vk(gpu), vk_image(image)); }
void op_image_get_desc(const s_gpu_backend *, const s_gpu_image *image, s_gpu_image_desc *out)
{
    s_vulkan_image_desc d;
    vulkan_image_get_desc(vk_image(image), &d);
    out->view_type = gpu_view_type_of(d.view_type);
    out->format = gpu_format_from_vulkan(d.format);
    out->width = d.width;
    out->height = d.height;
    out->depth = d.depth;
    out->mip_levels = d.mip_levels;
    out->array_layers = d.array_layers;
    out->usage = gpu_image_usage_of(d.usage);
    out->swizzle = gpu_swizzle_of(d.swizzle);
}
gpu_view op_image_view(const s_gpu_backend *, const s_gpu_image *image)
{
    return handle_cast<gpu_view>(vulkan_image_view(vk_image(image)));
}
gpu_view op_image_sampled_view(const s_gpu_backend *, const s_gpu_image *image)
{
    return handle_cast<gpu_view>(vulkan_image_sampled_view(vk_image(image)));
}
gpu_view op_image_array_view(s_gpu_backend *gpu, s_gpu_image *image)
{
    return handle_cast<gpu_view>(vulkan_image_array_view(vk(gpu), vk_image(image)));
}
gpu_view op_image_swizzled_view(s_gpu_backend *gpu, s_gpu_image *image, uint32_t swizzle)
{
    return handle_cast<gpu_view>(vulkan_image_swizzled_view(vk(gpu), vk_image(image), vk_swizzle(swizzle)));
}
gpu_result op_image_upload(s_gpu_backend *gpu, s_gpu_image *image, const s_gpu_image_region *regions, uint32_t region_count,
                           const void *data, uint32_t size)
{
    static_assert(sizeof(s_gpu_image_region) == sizeof(s_vulkan_image_region), "the same nine fields");
    s_vulkan_image_region local[64];
    s_vulkan_image_region *r = region_count <= 64 ? local : (s_vulkan_image_region *)malloc(region_count * sizeof(*r));
    if (r == 0)
        return k_gpu_out_of_memory;
    for (uint32_t i = 0; i < region_count; i++) {
        const s_gpu_image_region &g = regions[i];
        r[i] = s_vulkan_image_region{ g.mip_level, g.base_layer, g.layer_count, g.width, g.height,
                                      g.depth, g.offset, g.row_pitch, g.slice_pitch };
    }
    const VkResult result = vulkan_image_upload(vk(gpu), vk_image(image), r, region_count, data, size);
    if (r != local)
        free(r);
    return gpu_result_of(result);
}
gpu_sampler op_sampler_create(s_gpu_backend *gpu, const s_gpu_sampler_desc *desc)
{
    s_vulkan_sampler_desc d = {};
    d.mag_filter = vk_filter(desc->mag_filter);
    d.min_filter = vk_filter(desc->min_filter);
    d.mipmap_mode = vk_mipmap(desc->mipmap_mode);
    d.address_u = (uint32_t)pick(k_vk_address, desc->address_u, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    d.address_v = (uint32_t)pick(k_vk_address, desc->address_v, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    d.address_w = (uint32_t)pick(k_vk_address, desc->address_w, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    d.mip_lod_bias = desc->mip_lod_bias;
    d.max_anisotropy = desc->max_anisotropy;
    d.compare_enable = desc->compare_enable;
    d.compare_op = vk_compare(desc->compare_op);
    d.min_lod = desc->min_lod;
    d.max_lod = desc->max_lod;
    d.border_color = (uint32_t)pick(k_vk_border, desc->border_color, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK);
    return handle_cast<gpu_sampler>(vulkan_sampler_create(vk(gpu), &d));
}
void op_sampler_destroy(s_gpu_backend *gpu, gpu_sampler sampler)
{
    vulkan_sampler_destroy(vk(gpu), handle_cast<VkSampler>(sampler));
}

// shader pack modules and pipelines

gpu_shader op_pack_shader_module(s_gpu_backend *gpu, s_shader_pack *pack, uint32_t module_index, const char **out_entry)
{
    return handle_cast<gpu_shader>(vulkan_pack_shader_module(vk(gpu), pack, module_index, out_entry));
}
gpu_shader op_pack_xenos_module(s_gpu_backend *gpu, s_shader_pack *pack, uint32_t module_index)
{
    return handle_cast<gpu_shader>(vulkan_pack_xenos_module(vk(gpu), pack, module_index));
}
void op_pack_release(s_gpu_backend *gpu) { vulkan_pack_release(vk(gpu)); }
s_gpu_pipeline *op_pack_graphics_pipeline(s_gpu_backend *gpu, s_shader_pack *pack, uint32_t vertex_shader,
                                          uint32_t pixel_shader, const s_xenos_vertex_declaration *declaration,
                                          const s_gpu_graphics_state *state)
{
    s_vulkan_graphics_state s;
    vk_state(*state, &s);
    return (s_gpu_pipeline *)vulkan_pack_graphics_pipeline(vk(gpu), pack, vertex_shader, pixel_shader, declaration, &s);
}
s_gpu_pipeline *op_pack_compute_pipeline(s_gpu_backend *gpu, s_shader_pack *pack, uint32_t compute_shader)
{
    return (s_gpu_pipeline *)vulkan_pack_compute_pipeline(vk(gpu), pack, compute_shader);
}

// The universal Xenos layout (xenos_spirv_abi.h): set 0 the six blocks as dynamic uniform buffers, set 1 the 128
// images and 192 samplers, every stage. The translated SPIR-V reads vertices and writes memexport by device address.
int op_xenos_layout(s_gpu_backend *gpu, s_shader_pack *pack, s_gpu_xenos_layout *out)
{
    s_vulkan_gpu *self = (s_vulkan_gpu *)gpu;
    if (self->xenos_pack != 0) {
        if (self->xenos_pack != pack)
            return 0;
        *out = self->xenos;
        return 1;
    }
    s_vulkan_backend *v = self->vk;
    if (!v->device_address) {
        vulkan_log(v, "vulkan: the device has no bufferDeviceAddress: the Xenos shader pack cannot be used");
        return 0;
    }
    const bool abi6 = shader_pack_xenos_abi_version(pack) == XE_ABI_VERSION_BINDLESS;
    const uint32_t blocks = XE_BINDING_VERTEX_FETCH + 1u;
    const VkPhysicalDeviceLimits &limits = v->properties.limits;
    const uint32_t samplers = 6u * XE_SAMPLER_VARIANT_STRIDE, images = XE_BINDING_SAMPLER;
    if (abi6 && !vulkan_backend_supports_xenos_abi6(v))
        return 0;
    if (!abi6 && (limits.maxPerStageDescriptorSamplers < samplers || limits.maxDescriptorSetSamplers < samplers ||
        limits.maxPerStageDescriptorSampledImages < images || limits.maxDescriptorSetSampledImages < images ||
        limits.maxPerStageDescriptorUniformBuffers < blocks || limits.maxDescriptorSetUniformBuffersDynamic < blocks ||
        limits.maxBoundDescriptorSets < 2)) {
        vulkan_log(v, "vulkan: the device's descriptor limits are below the Xenos pipeline layout's (192 samplers, 128 "
                      "sampled images, 6 dynamic uniform buffers): the Xenos shader pack cannot be used");
        return 0;
    }
    const uint32_t all = VK_SHADER_STAGE_ALL;
    s_vulkan_layout_binding constants[XE_BINDING_VERTEX_FETCH + 1];
    uint32_t constant_count = 0;
    for (uint32_t b = 0; b < blocks; b++) {
        if (!abi6 || (b != XE_BINDING_FLOAT_VS && b != XE_BINDING_FLOAT_PS))
            constants[constant_count++] = { b, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, all };
    }
    VkDescriptorSetLayout set_layouts[2];
    set_layouts[XE_SET_CONSTANTS] = vulkan_descriptor_set_layout_get(v, constants, constant_count);
    if (abi6) {
        if (!vulkan_bindless_create(v))
            return 0;
        set_layouts[XE_SET_TEXTURES] = v->bindless.layout;
    } else {
        s_vulkan_layout_binding textures[images + samplers];
        for (uint32_t b = 0; b < images + samplers; b++) {
            const VkDescriptorType type = b < images ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE : VK_DESCRIPTOR_TYPE_SAMPLER;
            textures[b] = { b, (uint32_t)type, 1, all };
        }
        set_layouts[XE_SET_TEXTURES] = vulkan_descriptor_set_layout_get(v, textures, images + samplers);
    }
    if (set_layouts[0] == VK_NULL_HANDLE || set_layouts[1] == VK_NULL_HANDLE)
        return 0;
    const VkPipelineLayout layout = abi6 ? vulkan_pipeline_layout_with_push(v, set_layouts, 2,
                                                 { k_vulkan_xenos_push_stages, 0, sizeof(xe_push_v6) })
                                         : vulkan_pipeline_layout_get(v, set_layouts, 2);
    if (layout == VK_NULL_HANDLE)
        return 0;
    self->xenos.layout = handle_cast<gpu_pipeline_layout>(layout);
    self->xenos.constants = handle_cast<gpu_bind_layout>(set_layouts[XE_SET_CONSTANTS]);
    self->xenos.textures = handle_cast<gpu_bind_layout>(set_layouts[XE_SET_TEXTURES]);
    self->xenos_pack = pack;
    *out = self->xenos;
    return 1;
}

s_gpu_pipeline *op_graphics_pipeline_create_stages(s_gpu_backend *gpu, const s_gpu_shader_stage *stages,
                                                   uint32_t stage_count, gpu_pipeline_layout layout,
                                                   const s_gpu_graphics_state *state, uint32_t patch_control_points)
{
    s_vulkan_shader_stage s[k_max_stages];
    if (stage_count > k_max_stages)
        return 0;
    vk_shader_stages(stages, stage_count, s);
    s_vulkan_graphics_state st;
    vk_state(*state, &st);
    return (s_gpu_pipeline *)vulkan_graphics_pipeline_create_stages(vk(gpu), s, stage_count,
                                                                    handle_cast<VkPipelineLayout>(layout), &st,
                                                                    patch_control_points);
}
gpu_pipeline_handle op_compute_pipeline_create_handle(s_gpu_backend *gpu, gpu_shader module, const char *entry,
                                                      gpu_pipeline_layout layout)
{
    return handle_cast<gpu_pipeline_handle>(vulkan_compute_pipeline_create_handle(
        vk(gpu), handle_cast<VkShaderModule>(module), entry, handle_cast<VkPipelineLayout>(layout)));
}
s_gpu_pipeline *op_pipeline_adopt(s_gpu_backend *gpu, gpu_pipeline_handle handle, gpu_pipeline_layout layout,
                                  uint32_t bind_point)
{
    return (s_gpu_pipeline *)vulkan_pipeline_adopt(
        vk(gpu), handle_cast<VkPipeline>(handle), handle_cast<VkPipelineLayout>(layout),
        bind_point == k_gpu_bind_compute ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS);
}
void op_pipeline_destroy(s_gpu_backend *gpu, s_gpu_pipeline *pipeline)
{
    vulkan_pipeline_destroy(vk(gpu), vulkan_gpu_native_pipeline(pipeline));
}
gpu_bind_layout op_pipeline_bind_layout(const s_gpu_backend *, const s_gpu_pipeline *pipeline, uint32_t set)
{
    return handle_cast<gpu_bind_layout>(vulkan_pipeline_set_layout(vulkan_gpu_native_pipeline(pipeline), set));
}
uint32_t op_pipeline_vertex_streams(const s_gpu_backend *, const s_gpu_pipeline *pipeline)
{
    return vulkan_gpu_native_pipeline(pipeline)->vertex_streams;
}
gpu_pipeline_handle op_pipeline_library_vertex_input(s_gpu_backend *gpu, const s_gpu_graphics_state *state,
                                                     uint32_t patch_control_points)
{
    s_vulkan_graphics_state s;
    vk_state(*state, &s);
    return handle_cast<gpu_pipeline_handle>(vulkan_pipeline_library_vertex_input(vk(gpu), &s, patch_control_points));
}
gpu_pipeline_handle op_pipeline_library_pre_rasterization(s_gpu_backend *gpu, const s_gpu_shader_stage *stages,
                                                          uint32_t stage_count, gpu_pipeline_layout layout,
                                                          const s_gpu_graphics_state *state,
                                                          uint32_t patch_control_points)
{
    s_vulkan_shader_stage s[k_max_stages];
    if (stage_count > k_max_stages)
        return GPU_NULL_HANDLE;
    vk_shader_stages(stages, stage_count, s);
    s_vulkan_graphics_state st;
    vk_state(*state, &st);
    return handle_cast<gpu_pipeline_handle>(vulkan_pipeline_library_pre_rasterization(
        vk(gpu), s, stage_count, handle_cast<VkPipelineLayout>(layout), &st, patch_control_points));
}
gpu_pipeline_handle op_pipeline_library_fragment_shader(s_gpu_backend *gpu, gpu_shader fragment,
                                                      gpu_pipeline_layout layout, uint32_t alpha_test_enable)
{
    return handle_cast<gpu_pipeline_handle>(vulkan_pipeline_library_fragment_shader(
        vk(gpu), handle_cast<VkShaderModule>(fragment), handle_cast<VkPipelineLayout>(layout), alpha_test_enable));
}
gpu_pipeline_handle op_pipeline_library_fragment_output(s_gpu_backend *gpu, const s_gpu_graphics_state *state)
{
    s_vulkan_graphics_state s;
    vk_state(*state, &s);
    return handle_cast<gpu_pipeline_handle>(vulkan_pipeline_library_fragment_output(vk(gpu), &s));
}
void op_pipeline_library_destroy(s_gpu_backend *gpu, gpu_pipeline_handle library)
{
    vulkan_pipeline_library_destroy(vk(gpu), handle_cast<VkPipeline>(library));
}
s_gpu_pipeline *op_pipeline_link(s_gpu_backend *gpu, const gpu_pipeline_handle libraries[4], gpu_pipeline_layout layout,
                                 const s_gpu_graphics_state *state, uint32_t optimize)
{
    VkPipeline parts[4];
    for (uint32_t i = 0; i < 4; i++)
        parts[i] = handle_cast<VkPipeline>(libraries[i]);
    s_vulkan_graphics_state s;
    vk_state(*state, &s);
    return (s_gpu_pipeline *)vulkan_pipeline_link(vk(gpu), parts, handle_cast<VkPipelineLayout>(layout), &s, optimize);
}
gpu_pipeline_handle op_pipeline_link_handle(s_gpu_backend *gpu, const gpu_pipeline_handle libraries[4],
                                            gpu_pipeline_layout layout, uint32_t optimize)
{
    VkPipeline parts[4];
    for (uint32_t i = 0; i < 4; i++)
        parts[i] = handle_cast<VkPipeline>(libraries[i]);
    return handle_cast<gpu_pipeline_handle>(
        vulkan_pipeline_link_handle(vk(gpu), parts, handle_cast<VkPipelineLayout>(layout), optimize));
}
void op_pipeline_replace(s_gpu_backend *gpu, s_gpu_pipeline *pipeline, gpu_pipeline_handle handle)
{
    vulkan_pipeline_replace(vk(gpu), vulkan_gpu_native_pipeline(pipeline), handle_cast<VkPipeline>(handle));
}
// A library pipeline's key: its state without the dynamic run (the same fields vulkan_graphics_state_static_key clears).
void op_graphics_state_static_key(const s_gpu_backend *, const s_gpu_graphics_state *state, s_gpu_graphics_state *out)
{
    *out = *state;
    const size_t first = offsetof(s_gpu_graphics_state, cull_mode), end = offsetof(s_gpu_graphics_state, sample_count);
    memset((uint8_t *)out + first, 0, end - first);
}

// bind groups

gpu_bind_group op_bind_group_allocate(s_gpu_backend *gpu, gpu_bind_layout layout, uint32_t lifetime)
{
    s_vulkan_backend *v = vk(gpu);
    const VkDescriptorSetLayout set_layout = handle_cast<VkDescriptorSetLayout>(layout);
    VkDescriptorSet set = VK_NULL_HANDLE;
    switch (lifetime) {
    case k_gpu_bind_frame: set = vulkan_descriptor_set_allocate(v, set_layout, 1); break;
    case k_gpu_bind_frame_textures: set = vulkan_descriptor_set_allocate_textures(v, set_layout); break;
    case k_gpu_bind_cached_textures: set = vulkan_texture_set_allocate(v, set_layout); break;
    default: set = vulkan_descriptor_set_allocate(v, set_layout, 0); break;
    }
    return handle_cast<gpu_bind_group>(set);
}
void op_bind_group_free(s_gpu_backend *gpu, gpu_bind_group group)
{
    vulkan_descriptor_set_free(vk(gpu), vulkan_gpu_native_bind_group(group));
}
void op_bind_groups_reset_cached(s_gpu_backend *gpu) { vulkan_texture_sets_reset(vk(gpu)); }
void op_bind_write_buffer(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                          uint32_t type, s_gpu_buffer *buffer, uint32_t offset, uint32_t range)
{
    vulkan_descriptor_write_buffer(vk(gpu), vulkan_gpu_native_bind_group(group), binding, array_element,
                                   (uint32_t)pick(k_vk_descriptor, type, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
                                   vulkan_gpu_native_buffer(buffer), offset, range);
}
void op_bind_write_image(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                         uint32_t type, gpu_view view)
{
    vulkan_descriptor_write_image(vk(gpu), vulkan_gpu_native_bind_group(group), binding, array_element,
                                  (uint32_t)pick(k_vk_descriptor, type, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE),
                                  handle_cast<VkImageView>(view));
}
void op_bind_write_sampler(s_gpu_backend *gpu, gpu_bind_group group, uint32_t binding, uint32_t array_element,
                           gpu_sampler sampler)
{
    vulkan_descriptor_write_sampler(vk(gpu), vulkan_gpu_native_bind_group(group), binding, array_element,
                                    handle_cast<VkSampler>(sampler));
}
void op_bind_write_textures(s_gpu_backend *gpu, gpu_bind_group group, uint32_t count, const uint32_t *binding,
                            const uint64_t *handle, uint32_t first_sampler)
{
    // gpu_view / gpu_sampler values are the VkImageView / VkSampler handles
    vulkan_descriptor_write_textures(vk(gpu), vulkan_gpu_native_bind_group(group), count, binding, handle, first_sampler);
}

// frames

gpu_result op_frame_begin(s_gpu_backend *gpu) { return gpu_result_of(vulkan_frame_begin(vk(gpu))); }
gpu_result op_frame_end(s_gpu_backend *gpu, uint32_t wait) { return gpu_result_of(vulkan_frame_end(vk(gpu), wait)); }
gpu_result op_frame_wait(s_gpu_backend *gpu, uint32_t slot) { return gpu_result_of(vulkan_frame_wait(vk(gpu), slot)); }

// offscreen render targets

s_gpu_render_target *op_render_target_create(s_gpu_backend *gpu, uint32_t width, uint32_t height, uint32_t color_format,
                                             uint32_t flags)
{
    const uint32_t vk_flags = ((flags & k_gpu_render_target_depth) != 0 ? (uint32_t)k_vulkan_render_target_depth : 0u) |
                              ((flags & k_gpu_render_target_storage) != 0 ? (uint32_t)k_vulkan_render_target_storage : 0u);
    return (s_gpu_render_target *)vulkan_render_target_create(vk(gpu), width, height,
                                                              vulkan_format_from_gpu(color_format), vk_flags);
}
void op_render_target_destroy(s_gpu_backend *gpu, s_gpu_render_target *target)
{
    vulkan_render_target_destroy(vk(gpu), vk_target(target));
}
void op_render_target_get_info(const s_gpu_backend *, const s_gpu_render_target *target, s_gpu_render_target_info *out)
{
    s_vulkan_render_target_info info;
    vulkan_render_target_get_info(vk_target(target), &info);
    out->width = info.width;
    out->height = info.height;
    out->color_format = gpu_format_from_vulkan(info.color_format);
    out->depth_format = gpu_format_from_vulkan(info.depth_format);
    out->readback_row_pitch = info.readback_row_pitch;
}
s_gpu_image *op_render_target_color_image(s_gpu_backend *, s_gpu_render_target *target)
{
    return (s_gpu_image *)vulkan_render_target_color_image(vk_target(target));
}
const void *op_render_target_read(s_gpu_backend *gpu, s_gpu_render_target *target)
{
    return vulkan_render_target_read(vk(gpu), vk_target(target));
}

// commands

void vk_clear(const s_gpu_clear &in, s_vulkan_clear *out)
{
    out->flags = ((in.flags & k_gpu_clear_color) != 0 ? (uint32_t)k_vulkan_clear_color : 0u) |
                 ((in.flags & k_gpu_clear_depth) != 0 ? (uint32_t)k_vulkan_clear_depth : 0u) |
                 ((in.flags & k_gpu_clear_stencil) != 0 ? (uint32_t)k_vulkan_clear_stencil : 0u);
    out->color_mask = in.color_mask;
    for (uint32_t i = 0; i < 4; i++)
        out->color[i] = in.color[i];
    out->depth = in.depth;
    out->stencil = in.stencil;
}

void op_cmd_begin_rendering(s_gpu_backend *gpu, s_gpu_image *const *colors, uint32_t color_count, s_gpu_image *depth,
                            const s_gpu_clear *clear)
{
    [[clang::uninitialized]] s_vulkan_clear c;
    if (clear != 0)
        vk_clear(*clear, &c);
    vulkan_cmd_begin_rendering_images(vk(gpu), (s_vulkan_image *const *)colors, color_count, vk_image(depth),
                                      clear != 0 ? &c : 0);
}
void op_cmd_end_rendering(s_gpu_backend *gpu) { vulkan_cmd_end_rendering(vk(gpu)); }
void op_cmd_clear_attachments(s_gpu_backend *gpu, const s_gpu_clear *clear, const int32_t *rects, uint32_t rect_count)
{
    [[clang::uninitialized]] s_vulkan_clear c;
    vk_clear(*clear, &c);
    vulkan_cmd_clear_attachments(vk(gpu), &c, rects, rect_count);
}
void op_cmd_copy_image(s_gpu_backend *gpu, s_gpu_image *source, int32_t source_x, int32_t source_y, uint32_t width,
                       uint32_t height, s_gpu_image *destination, uint32_t destination_level, uint32_t destination_layer,
                       int32_t destination_x, int32_t destination_y, uint32_t destination_width,
                       uint32_t destination_height)
{
    vulkan_cmd_copy_image(vk(gpu), vk_image(source), source_x, source_y, width, height, vk_image(destination),
                          destination_level, destination_layer, destination_x, destination_y, destination_width,
                          destination_height);
}
void op_cmd_copy_depth(s_gpu_backend *gpu, s_gpu_image *source, int32_t source_x, int32_t source_y, uint32_t width,
                       uint32_t height, s_gpu_image *destination, int32_t destination_x, int32_t destination_y)
{
    vulkan_cmd_copy_depth(vk(gpu), vk_image(source), source_x, source_y, width, height, vk_image(destination),
                          destination_x, destination_y);
}
void op_cmd_copy_rects(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, const s_gpu_copy_rect *rects,
                       uint32_t count)
{
    [[clang::uninitialized]] s_vulkan_copy_rect local[64];
    s_vulkan_copy_rect *r = count <= 64 ? local : (s_vulkan_copy_rect *)malloc(count * sizeof(*r));
    if (r == 0)
        return;
    for (uint32_t i = 0; i < count; i++)
        r[i] = s_vulkan_copy_rect{ rects[i].source_x, rects[i].source_y, rects[i].destination_x, rects[i].destination_y,
                                   rects[i].width, rects[i].height };
    vulkan_cmd_copy_rects(vk(gpu), vk_image(source), vk_image(destination), r, count);
    if (r != local)
        free(r);
}
void op_cmd_set_viewport(s_gpu_backend *gpu, float x, float y, float width, float height, float min_depth, float max_depth)
{
    vulkan_cmd_set_viewport_inline(vk(gpu), x, y, width, height, min_depth, max_depth);
}
void op_cmd_set_scissor(s_gpu_backend *gpu, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    vulkan_cmd_set_scissor_inline(vk(gpu), x, y, width, height);
}
void op_cmd_bind_pipeline(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, const s_gpu_graphics_state *state)
{
    const s_vulkan_pipeline *p = vulkan_gpu_native_pipeline(pipeline);
    if (state != 0 && p->dynamic) {
        [[clang::uninitialized]] s_vulkan_dynamic_state translated;
        vk_dynamic_state(*state, &translated, true);
        vulkan_cmd_bind_pipeline_native(vk(gpu), p, &translated);
    } else {
        vulkan_cmd_bind_pipeline_native(vk(gpu), p);
    }
}
void op_cmd_bind_group(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, uint32_t set, gpu_bind_group group,
                       uint32_t offset_count, const uint32_t *offsets)
{
    vulkan_cmd_bind_group_native(vk(gpu), vulkan_gpu_native_pipeline(pipeline), set, vulkan_gpu_native_bind_group(group),
                                 offset_count, offsets);
}
void op_cmd_bind_group_pair(s_gpu_backend *gpu, const s_gpu_pipeline *pipeline, gpu_bind_group group0,
                            uint32_t offset_count0, const uint32_t *offsets0, gpu_bind_group group1)
{
    vulkan_cmd_bind_group_pair_native(vk(gpu), vulkan_gpu_native_pipeline(pipeline), vulkan_gpu_native_bind_group(group0),
                                      offset_count0, offsets0, vulkan_gpu_native_bind_group(group1));
}
void op_cmd_bind_vertex_buffer(s_gpu_backend *gpu, uint32_t binding, const s_gpu_buffer *buffer, uint32_t offset,
                               uint32_t stride)
{
    vulkan_cmd_bind_vertex_buffer_native(vk(gpu), binding, vulkan_gpu_native_buffer(buffer), offset, stride);
}
void op_cmd_bind_index_buffer(s_gpu_backend *gpu, const s_gpu_buffer *buffer, uint32_t offset, uint32_t index_type)
{
    static_assert((uint32_t)k_gpu_index_uint16 == (uint32_t)VK_INDEX_TYPE_UINT16 &&
                  (uint32_t)k_gpu_index_uint32 == (uint32_t)VK_INDEX_TYPE_UINT32,
                  "GPU index types use Vulkan's enum values");
    const VkIndexType type = index_type <= k_gpu_index_uint32 ? (VkIndexType)index_type : VK_INDEX_TYPE_UINT16;
    vulkan_cmd_bind_index_buffer_native(vk(gpu), vulkan_gpu_native_buffer(buffer), offset, type);
}
// The translated SPIR-V reads and writes guest memory by device address (caps.buffer_device_address): views are never
// bound on Vulkan, and a call is the frontend's mistake.
void op_cmd_xenos_bind_memory(s_gpu_backend *gpu, const s_gpu_pipeline *, s_gpu_buffer *const *, s_gpu_buffer *const *)
{
    s_vulkan_gpu *self = (s_vulkan_gpu *)gpu;
    if (self->memory_views_reported++ == 0)
        vulkan_log(self->vk, "vulkan: memory views bound for Xenos shaders that read by device address (ignored)");
}
void op_cmd_draw(s_gpu_backend *gpu, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                 uint32_t first_instance)
{
    vulkan_cmd_draw_inline(vk(gpu), vertex_count, instance_count, first_vertex, first_instance);
}
void op_cmd_draw_indexed(s_gpu_backend *gpu, uint32_t index_count, uint32_t instance_count, uint32_t first_index,
                         int32_t vertex_offset, uint32_t first_instance)
{
    vulkan_cmd_draw_indexed_inline(vk(gpu), index_count, instance_count, first_index, vertex_offset, first_instance);
}
void op_cmd_dispatch(s_gpu_backend *gpu, uint32_t groups_x, uint32_t groups_y, uint32_t groups_z)
{
    vulkan_cmd_dispatch_inline(vk(gpu), groups_x, groups_y, groups_z);
}
void op_cmd_memory_barrier(s_gpu_backend *gpu) { vulkan_cmd_memory_barrier(vk(gpu)); }
void op_cmd_copy_buffer(s_gpu_backend *gpu, s_gpu_buffer *source, uint32_t source_offset, s_gpu_buffer *destination,
                        uint32_t destination_offset, uint32_t size)
{
    vulkan_cmd_copy_buffer(vk(gpu), vulkan_gpu_native_buffer(source), source_offset, vulkan_gpu_native_buffer(destination),
                           destination_offset, size);
}
void op_cmd_copy_buffer_to_image(s_gpu_backend *gpu, s_gpu_buffer *buffer, uint32_t offset, uint32_t row_length,
                                 s_gpu_image *image, uint32_t width, uint32_t height)
{
    vulkan_cmd_copy_buffer_to_image(vk(gpu), vulkan_gpu_native_buffer(buffer), offset, row_length, vk_image(image), width,
                                    height);
}
void op_cmd_copy_image_to_buffer(s_gpu_backend *gpu, s_gpu_image *image, uint32_t level, uint32_t layer, int32_t x,
                                 int32_t y, uint32_t width, uint32_t height, s_gpu_buffer *buffer, uint32_t offset)
{
    vulkan_cmd_copy_image_to_buffer(vk(gpu), vk_image(image), level, layer, x, y, width, height,
                                    vulkan_gpu_native_buffer(buffer), offset);
}
void op_cmd_readback(s_gpu_backend *gpu, s_gpu_render_target *target) { vulkan_cmd_readback(vk(gpu), vk_target(target)); }
void op_cmd_checkpoint(s_gpu_backend *gpu, const void *marker) { vulkan_cmd_checkpoint(vk(gpu), marker); }

// helper passes

void op_cmd_reinterpret_7e3(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination)
{
    vulkan_cmd_reinterpret_7e3(vk(gpu), vk_image(source), vk_image(destination));
}
uint32_t op_cmd_convert_pwl(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, uint32_t encode)
{
    return vulkan_cmd_convert_pwl(vk(gpu), vk_image(source), vk_image(destination), encode);
}
uint32_t op_cmd_downscale_image(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination)
{
    return vulkan_cmd_downscale_image(vk(gpu), vk_image(source), vk_image(destination));
}
uint32_t op_cmd_pack_depth_stencil(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, uint32_t float_depth)
{
    return vulkan_cmd_pack_depth_stencil(vk(gpu), vk_image(source), vk_image(destination), float_depth);
}
void op_cmd_apply_display_gamma(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination, s_gpu_buffer *ramp,
                                uint32_t ramp_offset)
{
    static_assert(GPU_DISPLAY_GAMMA_RAMP_SIZE == k_vulkan_display_gamma_ramp_size, "display_gamma.hlsl's ramp");
    vulkan_cmd_apply_display_gamma(vk(gpu), vk_image(source), vk_image(destination), vulkan_gpu_native_buffer(ramp),
                                    ramp_offset);
}
uint32_t op_cmd_scale_image_rect(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                 s_gpu_buffer *parameters, uint32_t parameters_offset, uint32_t width, uint32_t height)
{
    static_assert(GPU_SCALE_IMAGE_PARAMETERS_SIZE == k_vulkan_scale_image_parameters_size, "resolve_scale.hlsl's");
    return vulkan_cmd_scale_image_rect(vk(gpu), vk_image(source), vk_image(destination), vulkan_gpu_native_buffer(parameters),
                                       parameters_offset, width, height);
}

// occlusion queries

int op_occlusion_begin(s_gpu_backend *gpu, uint32_t *out_serial, uint32_t *out_record)
{
    return vulkan_occlusion_begin(vk(gpu), out_serial, out_record);
}
void op_occlusion_end(s_gpu_backend *gpu) { vulkan_occlusion_end(vk(gpu)); }
int op_occlusion_result(s_gpu_backend *gpu, uint32_t serial, uint32_t record, uint64_t *out_samples)
{
    return vulkan_occlusion_result(vk(gpu), serial, record, out_samples);
}

// presentation

gpu_result op_surface_create(s_gpu_backend *gpu, uint32_t window_system, void *display, uintptr_t window)
{
    switch (window_system) {
    case k_gpu_window_system_xlib:
        return gpu_result_of(vulkan_surface_create_xlib(vk(gpu), display, (unsigned long)window));
    case k_gpu_window_system_win32:
        return gpu_result_of(vulkan_surface_create_win32(vk(gpu), display, (void *)window));
    case k_gpu_window_system_xcb:
        return gpu_result_of(vulkan_surface_create_xcb(vk(gpu), display, (uint32_t)window));
    case k_gpu_window_system_wayland:
        return gpu_result_of(vulkan_surface_create_wayland(vk(gpu), display, (void *)window));
    case k_gpu_window_system_headless:
        return gpu_result_of(vulkan_surface_create_headless(vk(gpu)));
    default:
        return k_gpu_unsupported;
    }
}
void op_surface_destroy(s_gpu_backend *gpu) { vulkan_surface_destroy(vk(gpu)); }
gpu_result op_swapchain_create(s_gpu_backend *gpu, uint32_t width, uint32_t height, uint32_t vsync)
{
    return gpu_result_of(vulkan_swapchain_create(vk(gpu), width, height, vsync));
}
void op_swapchain_destroy(s_gpu_backend *gpu) { vulkan_swapchain_destroy(vk(gpu)); }
gpu_result op_swapchain_acquire(s_gpu_backend *gpu, uint32_t *out_image_index)
{
    return gpu_result_of(vulkan_swapchain_acquire(vk(gpu), out_image_index));
}
void op_cmd_blit_to_swapchain(s_gpu_backend *gpu, s_gpu_render_target *target)
{
    vulkan_cmd_blit_to_swapchain(vk(gpu), vk_target(target));
}
gpu_result op_swapchain_present(s_gpu_backend *gpu) { return gpu_result_of(vulkan_swapchain_present(vk(gpu))); }

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

} // namespace

extern "C" s_vulkan_backend *vulkan_gpu_native_backend(s_gpu_backend *gpu)
{
    return gpu != 0 && gpu->ops == &k_ops ? vk(gpu) : 0;
}

void vulkan_gpu_translate_dynamic_state(const s_gpu_graphics_state *state, s_vulkan_dynamic_state *out)
{
    vk_dynamic_state(*state, out);
}

#ifdef HALO3_64BIT
extern "C" uint32_t vulkan_gpu_gather_depth(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                             s_gpu_buffer *parameters, uint32_t offset, uint32_t width, uint32_t height)
{
    s_vulkan_backend *backend = vulkan_gpu_native_backend(gpu);
    if (backend == 0)
        return 0;
    return vulkan_cmd_gather_depth(backend, vk_image(source), vk_image(destination), vulkan_gpu_native_buffer(parameters),
                                   offset, width, height);
}
#endif

uint32_t vulkan_format_from_gpu(uint32_t format)
{
    return format < k_gpu_format_count ? (uint32_t)k_vk_formats[format] : (uint32_t)VK_FORMAT_UNDEFINED;
}

uint32_t gpu_format_from_vulkan(uint32_t format)
{
    if (format < sizeof(k_gpu_formats_by_vk.formats))
        return k_gpu_formats_by_vk.formats[format];
    for (uint32_t i = 1; i < k_gpu_format_count; i++)
        if ((uint32_t)k_vk_formats[i] == format)
            return i;
    return k_gpu_format_undefined;
}

extern "C" gpu_result vulkan_gpu_backend_create(const s_gpu_backend_create_info *info, s_gpu_backend **out_backend)
{
    *out_backend = 0;
    s_vulkan_backend_create_info create = {};
    create.application_name = info->application_name;
    create.window_system = info->window_system == k_gpu_window_system_win32     ? (uint32_t)k_vulkan_window_system_win32
                           : info->window_system == k_gpu_window_system_xlib     ? (uint32_t)k_vulkan_window_system_xlib
                           : info->window_system == k_gpu_window_system_xcb      ? (uint32_t)k_vulkan_window_system_xcb
                           : info->window_system == k_gpu_window_system_wayland  ? (uint32_t)k_vulkan_window_system_wayland
                           : info->window_system == k_gpu_window_system_headless ? (uint32_t)k_vulkan_window_system_headless
                                                                                  : (uint32_t)k_vulkan_window_system_none;
    create.validation = info->validation;
    create.frames_in_flight = info->frames_in_flight;
    create.device_index = info->device_index;
    create.pipeline_cache_data = info->pipeline_cache_data;
    create.pipeline_cache_size = info->pipeline_cache_size;
    create.log = info->log;
    create.log_context = info->log_context;
    create.xenos_features = info->xenos_features;
    s_vulkan_gpu *self = (s_vulkan_gpu *)calloc(1, sizeof(s_vulkan_gpu));
    if (self == 0)
        return k_gpu_out_of_memory;
    const VkResult result = vulkan_backend_create(&create, &self->vk);
    if (result != VK_SUCCESS) {
        free(self);
        return gpu_result_of(result);
    }
    s_vulkan_device_info device;
    vulkan_backend_get_device_info(self->vk, &device);
    const VkPhysicalDeviceLimits &limits = self->vk->properties.limits;
    s_gpu_caps &caps = self->base.caps;
    caps.kind = k_gpu_backend_vulkan;
    caps.name = "vulkan";
    caps.api_name = "Vulkan";
    caps.api_major = device.api_version >> 22;
    caps.api_minor = (device.api_version >> 12) & 0x3ffu;
    memcpy(caps.device_name, device.name, sizeof(caps.device_name));
    caps.device_name[sizeof(caps.device_name) - 1] = '\0';
    caps.shader_module_format = SHADER_PACK_MODULE_FORMAT_SPIRV;
    caps.pipeline_cache_file = gpu_backend_pipeline_cache_file(k_gpu_backend_vulkan);
    caps.frames_in_flight = self->vk->frame_count;
    caps.uniform_offset_alignment = limits.minUniformBufferOffsetAlignment <= 0xffffffffu
                                        ? (uint32_t)limits.minUniformBufferOffsetAlignment : 0xffffffffu;
    caps.depth_stencil_format = gpu_format_from_vulkan(device.depth_format);
    caps.validation = device.validation;
    caps.checkpoints = self->vk->checkpoints;
    caps.buffer_device_address = device.buffer_device_address;
    caps.vertex_id_excludes_base_vertex = 0; // gl_VertexIndex includes firstVertex / vertexOffset
    caps.point_sprites = 1;                  // PointSize
    caps.triangle_fans = 1;                  // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN
    caps.clip_distance = device.clip_distance;
    caps.geometry = device.geometry;
    caps.tessellation = device.tessellation;
    caps.full_draw_index = device.full_draw_index;
    caps.sampler_mirror_clamp_to_edge = self->vk->sampler_mirror_clamp_to_edge;
    caps.pipeline_library = device.pipeline_library;
    caps.pipeline_library_fast_link = device.pipeline_library_fast_link;
    caps.max_stage_samplers = limits.maxPerStageDescriptorSamplers < limits.maxDescriptorSetSamplers
                                  ? limits.maxPerStageDescriptorSamplers : limits.maxDescriptorSetSamplers;
    caps.max_stage_sampled_images = limits.maxPerStageDescriptorSampledImages < limits.maxDescriptorSetSampledImages
                                        ? limits.maxPerStageDescriptorSampledImages
                                        : limits.maxDescriptorSetSampledImages;
    caps.max_stage_uniform_buffers = limits.maxPerStageDescriptorUniformBuffers;
    caps.max_dynamic_uniform_buffers = limits.maxDescriptorSetUniformBuffersDynamic;
    caps.max_bind_groups = limits.maxBoundDescriptorSets;
    self->base.ops = &k_ops;
    self->base.frame_index_ptr = &self->vk->frame_index;
    self->base.rendering_ptr = &self->vk->rendering;
    self->base.rendering_width_ptr = &self->vk->rendering_width;
    self->base.rendering_height_ptr = &self->vk->rendering_height;
    *out_backend = &self->base;
    return k_gpu_ok;
}
