// port/sdk/d3d: Vulkan objects from the shader pack (port/shaders/shader_pack.h): the module cache, pipeline layouts
// from the reflected bindings, and pipelines, as thin wrappers over vulkan_backend.cpp's raw entry points.
// vulkan_pack_release is in vulkan_backend.cpp (vulkan_backend_destroy uses it, and it needs no pack).
#include "vulkan_internal.h"
#include "../../shaders/shader_pack.h"

// Binds the module cache to a pack (one at a time).
static bool attach_pack(s_vulkan_backend *backend, s_shader_pack *pack)
{
    if (backend->pack == pack)
        return true;
    if (backend->pack != 0) {
        vulkan_log(backend, "vulkan: a second shader pack is in use: call vulkan_pack_release for the first");
        return false;
    }
    uint32_t count = shader_pack_module_count(pack);
    backend->pack_modules = (VkShaderModule *)calloc(count + 1, sizeof(VkShaderModule));
    backend->pack_entry_points = (const char **)calloc(count + 1, sizeof(const char *));
    backend->pack_output_masks = (uint32_t *)calloc(count + 1, sizeof(uint32_t));
    if (backend->pack_modules == 0 || backend->pack_entry_points == 0 || backend->pack_output_masks == 0) {
        vulkan_pack_release(backend);
        return false;
    }
    backend->pack_module_count = count;
    backend->pack = pack;
    return true;
}

extern "C" VkShaderModule vulkan_pack_shader_module(s_vulkan_backend *backend, s_shader_pack *pack,
                                                    uint32_t module_index, const char **out_entry_point)
{
    if (out_entry_point != 0)
        *out_entry_point = 0;
    if (!attach_pack(backend, pack))
        return VK_NULL_HANDLE;
    if (module_index >= backend->pack_module_count) {
        vulkan_log(backend, "vulkan: shader pack module %u of %u", module_index, backend->pack_module_count);
        return VK_NULL_HANDLE;
    }
    if (backend->pack_modules[module_index] == VK_NULL_HANDLE) {
        size_t size = 0;
        const uint32_t *code = shader_pack_module_code(pack, module_index, &size);
        if (code == 0) {
            vulkan_log(backend, "vulkan: shader pack module %u cannot be read: %s", module_index,
                       shader_pack_last_error());
            return VK_NULL_HANDLE;
        }
        // Each pack module is one shader with one entry point (DXC names it after the HLSL function).
        const char *entry = 0;
        for (uint32_t stage = 0; stage < 3 && entry == 0; stage++)
            entry = vulkan_spirv_entry_point(code, size, stage);
        if (entry == 0) {
            vulkan_log(backend, "vulkan: shader pack module %u has no entry point", module_index);
            return VK_NULL_HANDLE;
        }
        // pixel shaders take the render targets' exponent biases and the tile offset (vulkan_spirv_patch_fragment)
        size_t patched_size = 0;
        uint32_t *patched = vulkan_spirv_patch_fragment(code, size, &patched_size);
        VkShaderModule module = vulkan_shader_module_create(backend, patched != 0 ? patched : code,
                                                            patched != 0 ? patched_size : size);
        free(patched);
        if (module == VK_NULL_HANDLE)
            return VK_NULL_HANDLE;
        backend->pack_modules[module_index] = module;
        backend->pack_entry_points[module_index] = entry;  // in the pack's cached code, valid until shader_pack_close
        backend->pack_output_masks[module_index] = vulkan_spirv_output_mask(code, size);
    }
    if (out_entry_point != 0)
        *out_entry_point = backend->pack_entry_points[module_index];
    return backend->pack_modules[module_index];
}

// The bindings of one descriptor set, as they are gathered from several shaders.
struct s_set_bindings {
    s_vulkan_layout_binding *bindings;
    uint32_t count;
    uint32_t capacity;
};

static uint32_t stage_flags(uint32_t stage)
{
    switch (stage) {
    case k_shader_pack_stage_vertex: return VK_SHADER_STAGE_VERTEX_BIT;
    case k_shader_pack_stage_pixel: return VK_SHADER_STAGE_FRAGMENT_BIT;
    default: return VK_SHADER_STAGE_COMPUTE_BIT;
    }
}

// Adds a shader's binding to its set: a binding number already there (from another stage) must agree on type and
// count, and then gains this stage.
static bool add_binding(s_vulkan_backend *backend, s_set_bindings *set, const s_shader_pack_binding &binding,
                        uint32_t stage, const char *shader_name)
{
    for (uint32_t i = 0; i < set->count; i++) {
        s_vulkan_layout_binding &existing = set->bindings[i];
        if (existing.binding != binding.binding)
            continue;
        if (existing.descriptor_type != binding.descriptor_type || existing.count != binding.count) {
            vulkan_log(backend, "vulkan: %s: set %u binding %u (%s) disagrees with another stage's", shader_name,
                       binding.set, binding.binding, binding.name);
            return false;
        }
        existing.stage_flags |= stage_flags(stage);
        return true;
    }
    if (set->count == set->capacity) {
        uint32_t capacity = set->capacity != 0 ? set->capacity * 2 : 32;
        s_vulkan_layout_binding *grown =
            (s_vulkan_layout_binding *)realloc(set->bindings, capacity * sizeof(s_vulkan_layout_binding));
        if (grown == 0)
            return false;
        set->bindings = grown;
        set->capacity = capacity;
    }
    s_vulkan_layout_binding &added = set->bindings[set->count++];
    added.binding = binding.binding;
    added.descriptor_type = binding.descriptor_type;
    added.count = binding.count;
    added.stage_flags = stage_flags(stage);
    return true;
}

extern "C" VkPipelineLayout vulkan_pack_pipeline_layout(s_vulkan_backend *backend, s_shader_pack *pack,
                                                        const uint32_t *shader_indices, uint32_t shader_count)
{
    s_set_bindings sets[VULKAN_MAX_SETS] = {};
    uint32_t set_count = 0;
    bool ok = true;
    for (uint32_t s = 0; s < shader_count && ok; s++) {
        s_shader_pack_shader_info info;
        if (!shader_pack_get_shader(pack, shader_indices[s], &info)) {
            vulkan_log(backend, "vulkan: shader pack has no shader %u", shader_indices[s]);
            ok = false;
            break;
        }
        for (uint32_t b = 0; b < info.binding_count && ok; b++) {
            s_shader_pack_binding binding;
            if (!shader_pack_get_binding(pack, shader_indices[s], b, &binding)) {
                ok = false;
            } else if (binding.set >= VULKAN_MAX_SETS) {
                vulkan_log(backend, "vulkan: %s: binding %s in set %u (at most %u sets)", info.name, binding.name,
                           binding.set, (uint32_t)VULKAN_MAX_SETS);
                ok = false;
            } else {
                ok = add_binding(backend, &sets[binding.set], binding, info.stage, info.name);
                if (binding.set + 1 > set_count)
                    set_count = binding.set + 1;
            }
        }
    }
    VkDescriptorSetLayout set_layouts[VULKAN_MAX_SETS] = {};
    for (uint32_t i = 0; i < set_count && ok; i++) {
        set_layouts[i] = vulkan_descriptor_set_layout_get(backend, sets[i].bindings, sets[i].count);
        ok = set_layouts[i] != VK_NULL_HANDLE;
    }
    for (uint32_t i = 0; i < VULKAN_MAX_SETS; i++)
        free(sets[i].bindings);
    return ok ? vulkan_pipeline_layout_get(backend, set_layouts, set_count) : VK_NULL_HANDLE;
}

// The transfer type (transfer_vertex_decls index) a PRT entry point's vertex shader reads: ambient takes one
// coefficient (UBYTE4N), linear four (BYTE4N), quadratic nine (3 x DEC3N), as foliage.fx's PRT entry points declare.
static uint32_t entry_point_transfer_type(uint32_t entry_point)
{
    switch (entry_point) {
    case k_shader_pack_entry_static_prt_ambient: return 1;
    case k_shader_pack_entry_static_prt_linear: return 2;
    case k_shader_pack_entry_static_prt_quadratic: return 3;
    default: return 0;
    }
}

extern "C" s_vulkan_pipeline *vulkan_pack_graphics_pipeline(s_vulkan_backend *backend, s_shader_pack *pack,
                                                            uint32_t vertex_shader, uint32_t pixel_shader,
                                                            const s_xenos_vertex_declaration *declaration,
                                                            const s_vulkan_graphics_state *state)
{
    // pixel_shader SHADER_PACK_NOT_FOUND: no fragment stage (D3D's SetPixelShader(NULL): depth / stencil only)
    const bool depth_only = pixel_shader == SHADER_PACK_NOT_FOUND;
    s_shader_pack_shader_info vs, ps;
    if (!shader_pack_get_shader(pack, vertex_shader, &vs) || vs.stage != k_shader_pack_stage_vertex ||
        (!depth_only && (!shader_pack_get_shader(pack, pixel_shader, &ps) || ps.stage != k_shader_pack_stage_pixel))) {
        vulkan_log(backend, "vulkan: pack pipeline: shaders %u, %u are not a vertex and a pixel shader", vertex_shader,
                   pixel_shader);
        return 0;
    }
    const char *vs_entry = 0, *ps_entry = 0;
    VkShaderModule vs_module = vulkan_pack_shader_module(backend, pack, vs.module_index, &vs_entry);
    VkShaderModule ps_module = depth_only ? VK_NULL_HANDLE : vulkan_pack_shader_module(backend, pack, ps.module_index, &ps_entry);
    uint32_t shaders[2] = { vertex_shader, pixel_shader };
    VkPipelineLayout layout = vulkan_pack_pipeline_layout(backend, pack, shaders, depth_only ? 1u : 2u);
    if (vs_module == VK_NULL_HANDLE || (!depth_only && ps_module == VK_NULL_HANDLE) || layout == VK_NULL_HANDLE)
        return 0;
    // Without a fragment shader the color attachments' values are undefined (Vulkan): nothing may write them. Likewise
    // an attachment the fragment shader has no output for: on the Xenos a color target the pixel shader does not
    // export keeps its contents (decorators.hlsl writes only the LDR target while the HDR target is bound too).
    s_vulkan_graphics_state masked_state;
    const uint32_t outputs = depth_only ? 0u : backend->pack_output_masks[ps.module_index];
    // (a fragment shader in which no color output was found is used as it is)
    bool masked = depth_only;
    for (uint32_t i = 0; outputs != 0 && i < VULKAN_MAX_COLOR_ATTACHMENTS && !masked; i++)
        masked = (outputs & (1u << i)) == 0 && state->blend[i].write_mask != 0;
    if (masked) {
        masked_state = *state;
        for (uint32_t i = 0; i < VULKAN_MAX_COLOR_ATTACHMENTS; i++)
            if ((outputs & (1u << i)) == 0)
                masked_state.blend[i].write_mask = 0;
        state = &masked_state;
    }

    s_xenos_vertex_declaration derived;
    if (declaration == 0) {
        if (!xenos_vertex_declaration_for_entry_point(vs.vertex_type, vs.entry_point,
                                                       entry_point_transfer_type(vs.entry_point), &derived)) {
            vulkan_log(backend, "vulkan: %s: no vertex declaration for vertex type %u, entry point %u", vs.name,
                       vs.vertex_type, vs.entry_point);
            return 0;
        }
        declaration = &derived;
    }
    s_shader_pack_input inputs[XENOS_VERTEX_MAX_ATTRIBUTES];
    if (vs.input_count > XENOS_VERTEX_MAX_ATTRIBUTES) {
        vulkan_log(backend, "vulkan: %s: %u vertex inputs (at most %u)", vs.name, vs.input_count,
                   (uint32_t)XENOS_VERTEX_MAX_ATTRIBUTES);
        return 0;
    }
    for (uint32_t i = 0; i < vs.input_count; i++) {
        if (!shader_pack_get_input(pack, vertex_shader, i, &inputs[i])) {
            vulkan_log(backend, "vulkan: %s: vertex input %u cannot be read", vs.name, i);
            return 0;
        }
    }
    s_xenos_vertex_input vertex_input;
    uint32_t failed = 0;
    uint32_t result = xenos_vertex_input_build(declaration, inputs, vs.input_count, &vertex_input, &failed);
    if (result != k_xenos_vertex_input_ok) {
        vulkan_log(backend, "vulkan: %s: vertex input %s: %s", vs.name, failed < vs.input_count ? inputs[failed].semantic : "",
                   result == k_xenos_vertex_input_type_mismatch     ? "its element's type cannot be read as the input's"
                   : result == k_xenos_vertex_input_no_free_binding ? "no free binding for the null vertex buffer"
                                                                     : "too many inputs");
        return 0;
    }
    return vulkan_graphics_pipeline_create(backend, vs_module, vs_entry, ps_module, ps_entry, layout, &vertex_input,
                                           state);
}

extern "C" s_vulkan_pipeline *vulkan_pack_compute_pipeline(s_vulkan_backend *backend, s_shader_pack *pack,
                                                           uint32_t compute_shader)
{
    s_shader_pack_shader_info cs;
    if (!shader_pack_get_shader(pack, compute_shader, &cs) || cs.stage != k_shader_pack_stage_compute) {
        vulkan_log(backend, "vulkan: pack pipeline: shader %u is not a compute shader", compute_shader);
        return 0;
    }
    const char *entry = 0;
    VkShaderModule module = vulkan_pack_shader_module(backend, pack, cs.module_index, &entry);
    VkPipelineLayout layout = vulkan_pack_pipeline_layout(backend, pack, &compute_shader, 1);
    if (module == VK_NULL_HANDLE || layout == VK_NULL_HANDLE)
        return 0;
    return vulkan_compute_pipeline_create(backend, module, entry, layout);
}

// Version 3 (translated Xenos microcode): the module as it is, entry point "main" (no fragment patching: the shaders
// take their exponent biases and tile origin from xe_system, xenos_spirv_abi.h).
extern "C" VkShaderModule vulkan_pack_xenos_module(s_vulkan_backend *backend, s_shader_pack *pack, uint32_t module_index)
{
    if (!attach_pack(backend, pack))
        return VK_NULL_HANDLE;
    if (module_index >= backend->pack_module_count) {
        vulkan_log(backend, "vulkan: shader pack module %u of %u", module_index, backend->pack_module_count);
        return VK_NULL_HANDLE;
    }
    if (backend->pack_modules[module_index] == VK_NULL_HANDLE) {
        size_t size = 0;
        const uint32_t *code = shader_pack_module_code(pack, module_index, &size);
        if (code == 0) {
            vulkan_log(backend, "vulkan: shader pack module %u cannot be read: %s", module_index,
                       shader_pack_last_error());
            return VK_NULL_HANDLE;
        }
        VkShaderModule module = vulkan_shader_module_create(backend, code, size);
        if (module == VK_NULL_HANDLE)
            return VK_NULL_HANDLE;
        backend->pack_modules[module_index] = module;
        backend->pack_entry_points[module_index] = "main";
    }
    return backend->pack_modules[module_index];
}
