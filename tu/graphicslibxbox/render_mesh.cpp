#include "geometry/geometry_definitions.h"
#include "rasterizer/rasterizer_state_globals.h"
// graphicslibxbox/render_mesh.obj: from the records 0x171020 0x174580 0x175810 0x175898 0x230668 0x459f78
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "tag_files/tag_groups.h"
#include "effects/effect_array_globals.h"
#include "rasterizer/rasterizer_primitives.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include <xtl.h>
#include "rasterizer/rasterizer.h"
#include <string.h>
#include "render/render_mesh.h"
#include "render/render_lightmap_shadows_globals.h"
#include "rasterizer/xenon/rasterizer_xenon_submit.h"
#include "render_methods/render_method_submit.h"
#include "render_methods/render_method_submit_globals.h"
#include "render_methods/render_method_data.h"
#include "effects/decals.h"
#include "rasterizer/xenon/rasterizer_xenon_main_globals.h"
// @address 0x8368d0 g_player_render_tuning
// @address 0x8c46eb ?m_valid@c_render_method_data@@2_NA
// @address 0x16fd28 rasterizer_set_alpha_test_enabled
// @address 0x16f0a8 rasterizer_set_packed_blend_state
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "models/model_skinning_matrix_types.h"
#include "render/render.h"
#include "render/render_geometry.h"
#include "render/render_objects.h"
#include "rasterizer/rasterizer_vertex_types.h"
#include "render_methods/render_method.h"
#include "math/real_math_types.h"
#include "render_methods/render_method_definitions.h"
#include "tag_files/tag_types.h"
#include "render/shader_extern_info_types.h"

// @address 0x81dc00 g_last_skinning_matrix_address
const s_model_skinning_matrix *g_last_skinning_matrix_address;


// @address 0x81dacc g_last_skinning_matrix_count


#include "geometry/render_geometry_mesh_types.h"
#include "geometry/render_geometry_parts.h"


#include "geometry/render_geometry_types.h"
#include "geometry/geometry_material_types.h"

#include "cutscene/cinematic_globals_types.h"
#include "game/players_globals.h"


#include "effects/decal_definition_types.h"

#include "effects/decal_system_types.h"
#include "memory/data.h"

#include "effects/decal_types.h"

void real_matrix4x3_to_s_model_skinning_matrix(s_model_skinning_matrix *skinning, const real_matrix4x3 *matrix)
{
    skinning->rows[0][0] = matrix->scale * matrix->rotation.forward.i;
    skinning->rows[0][1] = matrix->scale * matrix->rotation.left.i;
    skinning->rows[0][2] = matrix->scale * matrix->rotation.up.i;
    skinning->rows[0][3] = matrix->position.x;
    skinning->rows[1][0] = matrix->scale * matrix->rotation.forward.j;
    skinning->rows[1][1] = matrix->scale * matrix->rotation.left.j;
    skinning->rows[1][2] = matrix->scale * matrix->rotation.up.j;
    skinning->rows[1][3] = matrix->position.y;
    skinning->rows[2][0] = matrix->scale * matrix->rotation.forward.k;
    skinning->rows[2][1] = matrix->scale * matrix->rotation.left.k;
    skinning->rows[2][2] = matrix->scale * matrix->rotation.up.k;
    skinning->rows[2][3] = matrix->position.z;
}

// @address 0x175810 ?render_mesh_submit_rigid_matrix@@YAXPBUreal_matrix4x3@@@Z
void render_mesh_submit_rigid_matrix(struct real_matrix4x3 const * matrix)
{
    s_model_skinning_matrix constants;
    real_matrix4x3_to_s_model_skinning_matrix(&constants, matrix);
    if (c_render_globals::get_skinning_constants_owned()) {
        D3DVECTOR4 *destination;
        c_rasterizer::get_device()->GpuBeginVertexShaderConstantF4(16, &destination, 4);
        memcpy(destination, &constants, sizeof(constants));
    } else {
        c_rasterizer::get_device()->SetVertexShaderConstantF(16, constants.rows[0], 3);
    }
}



void render_mesh_submit_skinning(const s_model_skinning_matrix *matrices, long matrix_count)
{
    if (c_render_globals::get_skinning_constants_owned()) {
        D3DVECTOR4 *constants;
        c_rasterizer::get_device()->GpuBeginVertexShaderConstantF4(16, &constants, ((matrix_count + 1) * 3) & ~3);
        memcpy(constants, matrices, matrix_count * 3 * sizeof(real_vector4d));
    } else {
        ::c_rasterizer::set_vertex_shader_constant(16, matrix_count * 3, (real_vector4d const *)(const float *)matrices);
    }
    g_last_skinning_matrix_address = matrices;
    g_last_skinning_matrix_count = matrix_count;
}

// @address 0x4aee0 k_mesh_part_type_entry_points
// @rdata 0x8204aee0..0x8204aef0
const long k_mesh_part_type_entry_points[] = {
    _entry_point_static_sh, _entry_point_static_prt_ambient, _entry_point_static_prt_linear, _entry_point_static_prt_quadratic};
// @rdata-end

namespace {
__forceinline bool render_mesh_game_paused()
{
    bool paused = false;
    if (cinematic_globals)
        paused = cinematic_globals->in_progress;
    return paused;
}
}


// @address 0x174580 render_mesh_part
extern "C" void render_mesh_part(const s_render_geometry *geometry,
        const s_mesh *mesh,
        const s_part *part,
        int mode,
        const unsigned long *selection,
        const geometry_material *materials)
{
    const c_rasterizer_index_buffer *buffer = render_geometry_get_index_buffer(geometry, (unsigned short)mesh->index_buffer_indices[0]);
    c_rasterizer_vertex_buffer *resource = (c_rasterizer_vertex_buffer *)render_geometry_get_vertex_buffer(geometry,
        (unsigned short)mesh->vertex_buffer_indices[0]);
    c_rasterizer::set_vertex_stream(resource);
    unsigned int index = (unsigned short)mesh->vertex_buffer_indices[2];
    if (index != 0xffff || (index = (unsigned short)mesh->vertex_buffer_indices[1]) != 0xffff) {
        c_rasterizer_vertex_buffer *other = (c_rasterizer_vertex_buffer *)render_geometry_get_vertex_buffer(geometry, index);
        if (other)
            c_rasterizer::set_vertex_stream(other);
    }
    if ((unsigned short)mesh->vertex_buffer_indices[3] != 0xffff) {
        c_rasterizer_vertex_buffer *extra = (c_rasterizer_vertex_buffer *)render_geometry_get_vertex_buffer(geometry,
        (unsigned short)mesh->vertex_buffer_indices[3]);
        c_rasterizer::set_vertex_stream(extra);
    }
    int kind = mesh->prt_type;
    if (mode == _entry_point_static_prt_quadratic) {
        if (mesh->flags & 1) {
            mode = _entry_point_static_per_vertex_color;
        } else {
            bool fast = false;
            if (!render_mesh_game_paused() && !((s_players_globals *)tls_players_globals())->suppressed)
                fast = ((g_player_render_tuning.flags >> 1) & 1) != 0;
            if (!fast)
                mode = k_mesh_part_type_entry_points[kind];
            else
                mode = _entry_point_static_sh;
        }
    }
    long tag_index = materials[part->material_index].render_method.index;
    const c_render_method *tag = TAG_GET('rm  ', c_render_method, tag_index);
    bool enabled = false;
    if (::render_method_submit(tag_index, (c_render_method const *)(const c_render_method *)tag, (e_vertex_type)mesh->vertex_type,
            (e_transfer_vector_vertex_types)kind, (e_entry_point)mode, 0))
    {
        if (tag->get_postprocess_definition()->flags & 2) {
            rasterizer_set_alpha_test_enabled(1, (intptr_t)part >> 4);
            enabled = true;
        }
        {
            int start;
            int count;
            if (!selection) {
                count = part->index_count;
                start = part->index_start;
            } else {
                start = -1;
                int last = -1;
                int i = 0;
                if (part->subpart_count > 0)
                do {
                    if ((1 << (i & 31)) & selection[i >> 5]) {
                        const s_subpart *range = &static_cast<const s_subpart *>(mesh->subparts.address)[part->first_subpart + i];
                        if (start == -1) {
                            start = range->index_start;
                            last = range->index_count + start - 1;
                        } else {
                            unsigned short first = range->index_start;
                            if (first != last + 1) {
                                c_rasterizer::draw_indexed_primitive(buffer, 0, resource->get_vertex_count(), start, last - start + 1);
                                start = range->index_start;
                                last = range->index_count + start - 1;
                            } else {
                                last = range->index_count + first - 1;
                            }
                        }
                    }
                    i++;
                } while (i < part->subpart_count);
                if (start == -1)
                {
                    if (enabled)
                        rasterizer_set_alpha_test_enabled(0, 0);
                    return;
                }
                count = last - start + 1;
            }
            c_rasterizer::draw_indexed_primitive(buffer, 0, resource->get_vertex_count(), start, count);
        }
    }
    if (enabled)
        rasterizer_set_alpha_test_enabled(0, 0);
}


static __forceinline c_decal_system *render_mesh_decal_get_parent_system(const c_decal *decal)
{
    s_data_array *array = (s_data_array *)g_decal_system_data;
    return datum_get<c_decal_system>(array, decal->m_system_index);
}

static __forceinline const c_decal_definition *render_mesh_decal_get_definition(const c_decal *decal)
{
    const s_decals_system_definition *system = (const s_decals_system_definition *)
        TAG_GET('decs', s_decals_system_definition, render_mesh_decal_get_parent_system(decal)->m_definition_index);
    return &system->decals[decal->m_definition_index];
}

// @address 0x230668 ?render@c_decal@@QBAXXZ
void c_decal::render() const
{
    const c_decal_definition *definition = render_mesh_decal_get_definition(this);
    const c_decal_system *system = render_mesh_decal_get_parent_system(this);
    long pass = definition->pass;
    BOOL enabled = TRUE;
    c_rasterizer::get_device()->SetVertexShaderConstantB(2, &enabled, 1);
    x_currently_submitting_decal = this;
    unsigned long flags = definition->flags;
    if (!::render_method_submit(system->m_definition_index, &definition->shader,
            (e_vertex_type)(((~(flags >> 1)) << 2) & 4), _transfer_vector_vertex_none, _entry_point_default, 0))
        return;
    x_currently_submitting_decal = 0;
    if (pass == c_decal_definition::_pass_default) {
        if (definition->flags & 1) {
            D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
            // match: 0x230668: the constant-mode blend setup is inlined here; a call to the shared method stays a call.
            if (c_rasterizer::g_current_separate_alpha_blend_mode != c_rasterizer::_separate_alpha_blend_constant_alpha) {
                c_rasterizer::g_current_separate_alpha_blend_mode = c_rasterizer::_separate_alpha_blend_constant_alpha;
                c_rasterizer::get_device()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, 1);
                c_rasterizer::get_device()->SetRenderState(D3DRS_BLENDOPALPHA, 0);
                c_rasterizer::get_device()->SetRenderState(D3DRS_SRCBLENDALPHA, 14);
                c_rasterizer::get_device()->SetRenderState(D3DRS_DESTBLENDALPHA, 7);
            }
            c_rasterizer::get_device()->SetRenderState(D3DRS_BLENDFACTOR, (long)(definition->multiplier * 255.0f) << 24);
        } else {
            D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 7);
        }
        if ((unsigned char)((definition->flags >> 1) & 1))
            rasterizer_set_packed_blend_state(c_rasterizer::get_device(), 1, 0x10706);
    }

    float fade = decals_system_get_distance_fade(system) * get_fade();
    ::c_rasterizer::set_pixel_shader_constant(32, 1, (real_vector4d const *)&fade);
    ::c_rasterizer::set_vertex_shader_constant(228, 1, (real_vector4d const *)&m_texture_origin);
    if ((unsigned char)((m_flags >> 1) & 1)) {
        D3DDevice_DrawVerticesUP(c_rasterizer::get_device(), D3DPT_TRIANGLESTRIP, 4, m_vertices, 32);
    } else {
        c_rasterizer::set_vertex_stream(&((const c_mesh_allocator<c_rasterizer_vertex_buffer,
        rasterizer_vertex_world> *)get_vertex_allocator())->m_buffer);
        c_rasterizer::draw_indexed_primitive(
            &((const c_mesh_allocator<c_rasterizer_index_buffer, unsigned short> *)get_index_allocator())->m_buffer,
            get_vertex_start(),
            m_first_cache_count,
            get_index_start(),
            m_second_cache_count);
    }
}


// @address 0x459f78 ?render_dynamic_mesh@@YAXPAXJJJJPAUs_shader_extern_info@@_N@Z
void render_dynamic_mesh(void *vertices,
        long primitive_count,
        long render_method_index,
        long cull_mode,
        long entry_point,
        struct s_shader_extern_info *shader_extern_info,
        bool)
{
    bool submitted = true;
    if (shader_extern_info != 0)
    {
        render_method_submit_set_extern_info(
            31 - _CountLeadingZeros(-((long)g_active_structure_bsp_mask) & ((long)g_active_structure_bsp_mask)), shader_extern_info);
        if (entry_point >= 5)
            render_objects_set_lighting_constants(&shader_extern_info->lighting, 0, entry_point);
    }
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), cull_mode);
    if (render_method_index != -1)
        submitted = ::render_method_submit(render_method_index,
        (c_render_method const *)TAG_GET('rm  ',
        const c_render_method,
        render_method_index),
        _vertex_type_world,
            _transfer_vector_vertex_none, (e_entry_point)entry_point, 0);
    if (submitted != 0)
        c_rasterizer::get_device()->DrawPrimitiveUP(g_primitive_types[c_rasterizer_index_buffer::_primitive_type_triangle_list],
        primitive_count,
        vertices,
        32);
    if (shader_extern_info != 0)
    {
        c_render_method_data::m_valid = false;
        render_method_submit_valid();
    }
}
