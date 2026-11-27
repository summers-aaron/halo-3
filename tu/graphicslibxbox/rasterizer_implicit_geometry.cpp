// graphicslibxbox/rasterizer_implicit_geometry.obj: from the records 0x44e660 0x467070 0x467080 0x467118 0x4677e0 0x4678e0 0x467980 0x467998
// @flags /TP /O2
// @address 0x11df7d4 g_rasterizer_implicit_geometry_data
extern "C" {
__declspec(thread) void *g_rasterizer_implicit_geometry_data;
}
// @address 0x8c4694 g_rasterizer_device
#include "rasterizer/rasterizer_main_globals.h"
#include "render/render_objects_globals.h"
// @address 0x467080 implicit_geometry_new_primitive
// @address 0x467980 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_rasterizer_implicit_geometry_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
#include <xtl.h>
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include <math.h>
#include "cseries/cseries.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_globals.h"
#include "game/game_time_access.h"
#include "cache/restricted_memory_regions.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
#include "render_methods/render_method_submit.h"
#include "render/views/render_player_view.h"
#include "rasterizer/rasterizer_states.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "sky_atm/atmosphere.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "memory/tls_data_arrays.h"
#include "cache/restricted_memory_layout.h"
// @address 0x4677e0 ?initialize_circle_strip@@YAXXZ

// Registers a member of field layout 1 (see c_restricted_memory::add_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.

#include "rasterizer/rasterizer_implicit_geometry_types.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "render_methods/render_method.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "rasterizer/rasterizer.h"
struct s_weighted_atmosphere_parameters;
#include "sky_atm/atmosphere_fog_interface.h"
#include "render/render_player_view_globals.h"
extern "C" {
// @address 0x8c482d g_decorator_albedo_active
bool g_decorator_albedo_active;
}

enum
{
    _implicit_geometry_cylinder = 0,
    _implicit_geometry_sphere,
    _implicit_geometry_box,
    k_implicit_geometry_sphere_bands = 32,
    k_implicit_geometry_box_sides = 4
};

// @address 0x44e660 render_method_submit_set_shader_state
extern "C" bool render_method_submit_set_shader_state(const c_render_method *render_method, int vertex_type, int transfer_type, int entry_point, bool force)
{
    bool result = false;
    int boolean_constant;
    if (render_method)
    {
        const s_render_method_postprocess_definition *postprocess = render_method->get_postprocess_definition();
        if (postprocess->template_reference.index != -1)
        {
            const rasterizer_implicit_geometry_template *shader_template =
                (const rasterizer_implicit_geometry_template *)tag_get('rmt2', postprocess->template_reference.index);
            if (!force && entry_point != 1 && entry_point != 10)
            {
                if (render_method->flags & 2)
                    render_player_view_set_atmosphere_index(render_method->atmosphere_index);
                else if (render_method->flags & 1)
                    c_atmosphere_fog_interface::disable_atmosphere();
                else if (atmosphere_constants_index != -2)
                {
                    atmosphere_constants_index = -2;
                    c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
                }
            }
            if (shader_template->entry_point_mask & (1 << entry_point))
            {
                const s_render_method_texture *textures = (const s_render_method_texture *)postprocess->textures.address;
                const real_vector4d *values = (const real_vector4d *)postprocess->real_constants.address;
                unsigned long boolean_values = postprocess->boolean_values;
                const long *integer_values = (const long *)postprocess->integer_values.address;
                const rasterizer_implicit_geometry_binding *bindings = (const rasterizer_implicit_geometry_binding *)shader_template->bindings.address;
                const rasterizer_implicit_geometry_entry *entry = &((const rasterizer_implicit_geometry_entry *)shader_template->entries
                        .address)[((const unsigned short *)shader_template->entry_points.address)[entry_point] & 0x3ff];
                if (shader_template->pixel_shader.index != -1 && shader_template->vertex_shader.index != -1)
                {
                    if (force)
                    {
                        const c_rasterizer_compiled_pixel_shader *shader = render_method->get_compiled_pixel_shader((e_entry_point)entry_point);
                        if (shader)
                        {
                            D3DDevice_SetPixelShader(g_rasterizer_device, shader->get_d3d_shader());
                            result = true;
                        }
                        else
                            result = false;
                    }
                    else
                        result = set_pixel_and_vertex_shaders(render_method, (e_vertex_type)vertex_type,
                            (e_transfer_vector_vertex_types)transfer_type, (e_entry_point)entry_point);
                    if (result)
                    {
                        if (entry_point == 9 || entry_point == 17)
                        {
                            if (c_rasterizer::g_current_alpha_blend_mode != c_rasterizer::_alpha_blend_additive)
                            {
                                c_rasterizer::g_current_alpha_blend_mode = c_rasterizer::_alpha_blend_additive;
                                D3DDevice *device = g_rasterizer_device;
                                D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
                                D3DDevice_SetRenderState_BlendOp(device, 0);
                                D3DDevice_SetRenderState_SrcBlend(device, 1);
                                D3DDevice_SetRenderState_DestBlend(device, 1);
                            }
                        }
                        else if (entry_point != 12 && !g_decorator_albedo_active)
                        {
                            long mode = postprocess->alpha_blend_mode;
                            if (mode != c_rasterizer::g_current_alpha_blend_mode)
                            {
                                c_rasterizer::g_current_alpha_blend_mode = static_cast<c_rasterizer::e_alpha_blend_mode>(mode);
                                rasterizer_set_device_alpha_blend_mode(g_rasterizer_device, mode);
                            }
                        }
                        render_method_submit_update_externs(render_method, entry_point, 1, 0);
                        for (long index = 0; index < (long)(entry->textures >> 10); ++index)
                        {
                            const rasterizer_implicit_geometry_binding *binding = &bindings[(entry->textures & 0x3ff) + index];
                            long sampler = binding->destination;
                            const s_render_method_texture *texture = &textures[binding->source];
                            if (binding->flags & 1)
                                sampler += 20;
                            long address_mode = texture->address_mode & 15;
                            c_rasterizer::set_sampler_address_mode(sampler, (c_rasterizer::e_sampler_address_mode)address_mode,
                                (c_rasterizer::e_sampler_address_mode)(texture->address_mode >> 4),
                                (c_rasterizer::e_sampler_address_mode)address_mode);
                            c_rasterizer::set_sampler_filter_mode(sampler, static_cast<c_rasterizer::e_sampler_filter_mode>(texture->filter_mode));
                            if (!render_method_submit_set_dynamic_texture(0, sampler, texture->type))
                            {
                                long bitmap_index = texture->bitmap.index;
                                if (bitmap_index != -1)
                                {
                                    const bitmap_group *bitmap =
                                        (const bitmap_group *)tag_get('bitm', bitmap_index);
                                    long frame = texture->index > 0 ? texture->index : 0;
                                    long maximum = bitmap->bitmaps.count - 1;
                                    render_method_submit_set_texture(
                                        sampler, bitmap_index, frame < maximum ? (texture->index > 0 ? texture->index : 0) : maximum);
                                }
                                else if (texture->type)
                                {
                                    long globals_index = global_game_globals->rasterizer_globals_ref.index;
                                    const c_rasterizer_globals *globals = globals_index != -1
                                        ? (const c_rasterizer_globals *)tag_get('rasg', globals_index)
                                        : 0;
                                    c_rasterizer::set_sampler_texture(sampler, globals->get_default_texture_rasterizer_texture_ref(0));
                                }
                            }
                        }
                        for (long index = 0; index < (long)(entry->pixel_constants >> 10); ++index)
                        {
                            const rasterizer_implicit_geometry_binding *binding = &bindings[(entry->pixel_constants & 0x3ff) + index];
                            c_rasterizer::set_pixel_shader_constant(binding->destination, 1, &values[binding->source]);
                        }
                        for (long index = 0; index < (long)(entry->integer_constants >> 10); ++index)
                        {
                            const rasterizer_implicit_geometry_binding *binding = &bindings[(entry->integer_constants & 0x3ff) + index];
                            int value[4];
                            value[0] = integer_values[binding->source];
                            value[1] = 0;
                            value[2] = 1;
                            value[3] = 0;
                            D3DDevice_SetPixelShaderConstantI(g_rasterizer_device, binding->destination, value, 1);
                        }
                        for (long index = 0; index < (long)(entry->vertex_constants >> 10); ++index)
                        {
                            const rasterizer_implicit_geometry_binding *binding = &bindings[(entry->vertex_constants & 0x3ff) + index];
                            c_rasterizer::set_vertex_shader_constant(binding->destination, 1, &values[binding->source]);
                        }
                        for (long index = 0; index < (long)(entry->boolean_constants >> 10); ++index)
                        {
                            const rasterizer_implicit_geometry_binding *binding = &bindings[(entry->boolean_constants & 0x3ff) + index];
                            boolean_constant = (boolean_values & (1 << binding->source)) != 0;
                            D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, binding->destination, &boolean_constant, 1);
                        }
                    }
                }
            }
        }
    }
    return result;
}

namespace {
static __forceinline float circle_sine(float angle) { return (float)sin(angle); }
static __forceinline float circle_cosine(float angle) { return (float)cos(angle); }
}

// @address 0x467070 ?__tls_set_g_rasterizer_implicit_geometry_data_allocator@@YAXPAX@Z
void __tls_set_g_rasterizer_implicit_geometry_data_allocator(void *allocator)
{
    g_rasterizer_implicit_geometry_data = allocator;
}

void *implicit_geometry_new_primitive(int object_index, const void *color, const void *transform)
{
    void *result = 0;
    s_implicit_geometry_globals *globals = (s_implicit_geometry_globals *)g_rasterizer_implicit_geometry_data;
    int count = globals->count;
    if (count < 64) {
        s_implicit_geometry_primitive *primitive = &globals->primitives[count];
        globals->count = count + 1;
        primitive->render_method_index = object_index;
        primitive->color = *(const real_argb_color *)color;
        primitive->transform = *(const real_matrix4x3 *)transform;
        result = primitive;
    }
    return result;
}

// @address 0x467118 implicit_geometry_render_primitive
extern "C" void implicit_geometry_render_primitive(const void *context, long user)
{
    const s_implicit_geometry_primitive *primitive = (const s_implicit_geometry_primitive *)context;
    long game_time = game_time_get();
    float game_seconds = (float)game_time;
    float seconds = game_ticks_to_seconds(game_seconds);
    float scroll = seconds / 3.0f;
    float phase = scroll - floor(scroll);

    if (c_rasterizer::set_explicit_shaders(64, _vertex_type_implicit, _transfer_vector_vertex_none, _entry_point_default))
    {
        D3DDevice *device = g_rasterizer_device;
        device->SetIndices(NULL);
        if (primitive->render_method_index != NONE)
        {
            render_method_submit(primitive->render_method_index,
                (const c_render_method *)tag_get('rm  ', primitive->render_method_index),
                _vertex_type_implicit, _transfer_vector_vertex_none, _entry_point_static_sh, true);
        }

        BOOL not_box = primitive->type != _implicit_geometry_box;
        real_vector4d color;
        color.x = primitive->color.red;
        color.y = primitive->color.green;
        color.z = primitive->color.blue;
        color.w = primitive->color.alpha;
        g_rasterizer_device->SetVertexShaderConstantF(20, (const float *)&color, 1);
        D3DDevice_SetVertexShaderConstantB(g_rasterizer_device, 7, &not_box, 1);

        real_vector4d transform[3];
        transform[0].x = primitive->transform.forward.i;
        transform[0].y = primitive->transform.left.i;
        transform[0].z = primitive->transform.up.i;
        transform[0].w = primitive->transform.position.x;
        transform[1].x = primitive->transform.forward.j;
        transform[1].y = primitive->transform.left.j;
        transform[1].z = primitive->transform.up.j;
        transform[1].w = primitive->transform.position.y;
        transform[2].x = primitive->transform.forward.k;
        transform[2].y = primitive->transform.left.k;
        transform[2].z = primitive->transform.up.k;
        transform[2].w = primitive->transform.position.z;
        g_rasterizer_device->SetVertexShaderConstantF(24, (const float *)transform, 3);

        // declared at this scope so it keeps its own stack slot apart from the sphere loop temps
        real_vector4d box_texture_constant;
        if (primitive->type == _implicit_geometry_cylinder)
        {
            float top_offset = primitive->parameters[1];
            float bottom_offset = primitive->parameters[2];
            float height = bottom_offset + top_offset;
            float inverse_height = 1.0f / height;
            float radius = primitive->parameters[0];
            float circumference = radius * inverse_height * 6.2831855f;
            float texture_repeat = MAX(1.0f, floor(circumference + 0.5f));
            float center = primitive->parameters[1] - height * 0.5f;
            float height_scale = height > 0.0001f ? inverse_height : 0.0f;

            real_vector4d texture_constant;
            texture_constant.x = texture_repeat;
            texture_constant.y = 1.0f;
            texture_constant.z = phase;
            texture_constant.w = 0.0f;

            real_vector4d shape_constant;
            float bottom = primitive->parameters[2] * height_scale;
            float top = primitive->parameters[1] * height_scale;
            shape_constant.x = -(bottom * height) - center;
            shape_constant.y = primitive->parameters[0];
            shape_constant.z = top * height - center - shape_constant.x;
            shape_constant.w = 0.0f;

            g_rasterizer_device->SetVertexShaderConstantF(21, (const float *)&texture_constant, 1);
            g_rasterizer_device->SetVertexShaderConstantF(23, (const float *)&shape_constant, 1);

            DWORD cull_mode = D3DDevice_GetRenderState_CullMode(g_rasterizer_device);
            D3DDevice_SetRenderState_CullMode(g_rasterizer_device, D3DCULL_NONE);
            device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 126,
                ((s_implicit_geometry_globals *)g_rasterizer_implicit_geometry_data)->circle_strip, sizeof(s_circle_strip_vertex));
            D3DDevice_SetRenderState_CullMode(g_rasterizer_device, cull_mode);
        }
        else if (primitive->type == _implicit_geometry_sphere)
        {
            for (long band = 0; band < k_implicit_geometry_sphere_bands; band++)
            {
                float lower_v = (float)band / 32.0f;
                float latitude0 = lower_v * 3.1415927f - 1.5707964f;
                float upper_v = (float)(band + 1) / 32.0f;
                float latitude1 = upper_v * 3.1415927f - 1.5707964f;
                latitude0 = PIN(latitude0, -1.5707964f, 1.5707964f);
                latitude1 = PIN(latitude1, -1.5707964f, 1.5707964f);
                float radius = primitive->parameters[0];
                float ring_radius0 = circle_cosine(latitude0) * radius;
                float ring_height0 = circle_sine(latitude0) * radius;
                float ring_radius1 = circle_cosine(latitude1) * radius;
                float ring_height1 = circle_sine(latitude1) * radius;

                real_vector4d texture_constant;
                texture_constant.x = 4.0f;
                texture_constant.y = (upper_v - lower_v) * 2.0f;
                texture_constant.z = phase;
                texture_constant.w = lower_v * 2.0f;

                real_vector4d shape_constant;
                shape_constant.x = ring_height0;
                shape_constant.y = ring_radius0;
                shape_constant.z = ring_height1 - ring_height0;
                shape_constant.w = ring_radius1 - ring_radius0;

                g_rasterizer_device->SetVertexShaderConstantF(21, (const float *)&texture_constant, 1);
                g_rasterizer_device->SetVertexShaderConstantF(23, (const float *)&shape_constant, 1);

                DWORD cull_mode = D3DDevice_GetRenderState_CullMode(g_rasterizer_device);
                D3DDevice_SetRenderState_CullMode(g_rasterizer_device, D3DCULL_NONE);
                device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 126,
                ((s_implicit_geometry_globals *)g_rasterizer_implicit_geometry_data)->circle_strip, sizeof(s_circle_strip_vertex));
                D3DDevice_SetRenderState_CullMode(g_rasterizer_device, cull_mode);
            }
        }
        else
        {
            const s_implicit_geometry_primitive *box = primitive;
            // read the vertical bounds through a reference so the vertex stores re-read them each pass
            const s_implicit_geometry_primitive *const *box_reference = &box;
            s_circle_strip_vertex vertices[(k_implicit_geometry_box_sides + 1) * 2];
            float length = primitive->parameters[1];
            float height = primitive->parameters[2] + primitive->parameters[3];
            float width = primitive->parameters[0];
            s_circle_strip_vertex *vertex = vertices;
            float perimeter = (width + length) * 2.0f;
            float u = 0.0f;
            float center = primitive->parameters[2] - height * 0.5f;
            float inverse_perimeter = 1.0f / perimeter;
            for (long side = 0; side < k_implicit_geometry_box_sides + 1; side++)
            {
                long corner = side % k_implicit_geometry_box_sides;
                for (long edge = 0; edge < 2; edge++)
                {
                    vertex->x = corner == 0 || corner == 1 ? width * -0.5f : width * 0.5f;
                    vertex->y = corner == 0 || corner == 3 ? length * -0.5f : length * 0.5f;
                    vertex->z = edge ? -(*box_reference)->parameters[3] - center : (*box_reference)->parameters[2] - center;
                    vertex->u = u;
                    vertex->v = edge ? 0.0f : 1.0f;
                    vertex++;
                }
                u += side & 1 ? primitive->parameters[0] * inverse_perimeter : primitive->parameters[1] * inverse_perimeter;
            }

            float texture_repeat = MAX(1.0f, floor(perimeter / height + 0.5f));
            box_texture_constant.x = texture_repeat;
            box_texture_constant.y = 1.0f;
            box_texture_constant.z = phase;
            box_texture_constant.w = 0.0f;
            c_rasterizer::set_vertex_shader_constant(21, 1, &box_texture_constant);

            DWORD cull_mode = D3DDevice_GetRenderState_CullMode(g_rasterizer_device);
            D3DDevice_SetRenderState_CullMode(g_rasterizer_device, D3DCULL_NONE);
            device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 8, vertices, sizeof(s_circle_strip_vertex));
            D3DDevice_SetRenderState_CullMode(g_rasterizer_device, cull_mode);
        }
    }
}

void initialize_circle_strip()
{
    s_circle_strip_vertex *vertex = ((s_implicit_geometry_globals *)g_rasterizer_implicit_geometry_data)->circle_strip;
    for (long index = 0; index < 64; ++index) {
        float fraction = (float)index / 63.0f;
        float angle = 6.2831855f * fraction;
        float sine = circle_sine(angle);
        float cosine = circle_cosine(angle);
        for (long side = 0; side < 2; ++side) {
            vertex->x = sine;
            vertex->y = cosine;
            vertex->z = side != 0 ? 1.0f : 0.0f;
            vertex->u = fraction;
            vertex->v = side != 0 ? 1.0f : 0.0f;
            ++vertex;
        }
    }
}

// @address 0x4678e0 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_rasterizer_implicit_geometry_data_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_rasterizer_implicit_geometry_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0,
      __tls_set_g_rasterizer_implicit_geometry_data_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_rasterizer_implicit_geometry_data = address;
  return restricted_region_get_base_address(3) + g_restricted_regions[3].get_member_offset(member_index);
}

// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z

// @address 0x467998 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_rasterizer_implicit_geometry_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_rasterizer_implicit_geometry_data_allocator>::deallocate(void *memory)
{
    g_rasterizer_implicit_geometry_data = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x467980
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_rasterizer_implicit_geometry_data_allocator>::allocate(
    unsigned int size, const char *name)
{
    return (void *)reserve_memory(name, "", size, 0);
}

// @address 0x7bf760 ?g_rasterizer_implicit_geometry_data_allocator@@3V?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_rasterizer_implicit_geometry_data_allocator@@YAXPAX@Z@@A
t_implicit_geometry_allocation_manager g_rasterizer_implicit_geometry_data_allocator;

