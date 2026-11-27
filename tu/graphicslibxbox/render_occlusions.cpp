// graphicslibxbox/render_occlusions.obj: from the records 0x17eaf8 0x46a2b0 0x46a390
// @flags /TP /O2 /Oc
#include "render/render_object_storage.h"
#include "render/render_view_stack_globals.h"
#include "render/render_lightmap_shadows_calls.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "cseries/long_sort.h"
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include <xtl.h>
#include <math.h>
#include <string.h>
#include "cseries/cseries.h"
#include "scenario/scenario.h"
#include "render/render_structure.h"
#include "visibility/visibility_projection_types.h"
#include "render/shader_reference_types.h"
#include "visibility/visibility_globals.h"


#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "visibility/visibility_collection_types.h"
#include "visibility/visibility_collection.h"
#include "render/render_structure_globals_types.h"
#include "render/render_object_globals_types.h"

#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"

#include "render/structure_renderer.h"


#include "render/views/occlusion_view.h"

#include "render/render_transparents.h"
#include "effects/effects_globals.h"
#include "visibility/visibility_region_types.h"
#include "render/render_occlusions.h"
#include "cubemaps/cubemap_sample.h"


struct s_effect_camera_info;

extern "C" {
// @address 0x81dc04 g_view_stack
// @address 0x8c4878 g_effect_camera_world_info
}



// @address 0x17eaf8 render_occlusions_setup_depth_target
void render_occlusions_setup_depth_target(long unused, long x1, long y1)
{
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)2);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);
    g_render_method_submit_cache = k_default_render_method_submit_cache;
    D3DDevice_SetPixelShader(c_rasterizer::get_device(), 0);
    c_rasterizer::end_high_quality_blend();
    rectangle2d bounds;
    bounds.y0 = 0;
    bounds.x0 = 0;
    bounds.x1 = x1;
    bounds.y1 = y1;
    c_rasterizer::set_viewport(bounds, 0.0f, 1.0f);
    c_rasterizer::clear(16, -1, 1.0f, 0);
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
}


extern "C" {
}


// @address 0x46a2b0 ?compute_visibility@c_occlusion_view@@UAAXPBTreal_point3d@@@Z
void c_occlusion_view::compute_visibility(const real_point3d *point)
{
    s_visibility_input buffer;
    c_visibility_collection collection(&buffer);
    float separation = sqrtf(
        m_object_bounding_box.half_extents.j * m_object_bounding_box.half_extents.j
        + m_object_bounding_box.half_extents.k * m_object_bounding_box.half_extents.k);
    long structure_bsp_index;
    long leaf_index = scenario_leaf_index_from_point(point, &structure_bsp_index);
    s_cluster_reference cluster_reference;
    if (leaf_index != NONE)
        cluster_reference = scenario_cluster_reference_from_leaf_index(structure_bsp_index, leaf_index);
    else
        cluster_reference_invalidate(&cluster_reference);
    collection.compute(_collection_type_occlusion, 32, 0, 0, point, separation,
            ((s_visibility_region_build *)g_visibility_globals.collection_input)->marker_index, cluster_reference,
            ((s_visibility_region_build *)g_visibility_globals.collection_input)->option0,
            ((s_visibility_region_build *)g_visibility_globals.collection_input)->option1);
}

static __forceinline void render_occlusions_push_structure_marker()
{
    render_structure_globals *globals = &g_render_structure_globals;
    ++globals->depth;
    globals->cluster_markers[globals->depth] = globals->current_a;
    globals->instance_markers[globals->depth] = globals->current_b;
}

static __forceinline void render_occlusions_pop_structure_marker()
{
    --g_render_structure_globals.depth;
    g_render_structure_globals.current_a =
        g_render_structure_globals.cluster_markers[g_render_structure_globals.depth + 1];
    g_cluster_sorted_order.m_count = (unsigned short)g_render_structure_globals.current_a;
    g_render_structure_globals.current_b =
        g_render_structure_globals.instance_markers[g_render_structure_globals.depth + 1];
}

static __forceinline void render_occlusions_push_object_marker()
{
    s_render_object_globals *globals = g_render_object_globals;
    ++globals->marker;
    globals->stack_a[globals->marker] = globals->current_a;
    globals->stack_b[globals->marker] = globals->current_b;
}

static __forceinline void render_occlusions_pop_object_marker()
{
    s_render_object_globals *globals = g_render_object_globals;
    --globals->marker;
    globals->current_a = globals->stack_a[globals->marker + 1];
    globals->current_b = globals->stack_b[globals->marker + 1];
}

static __forceinline void render_occlusions()
{
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 0);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    long first = g_render_structure_globals.cluster_markers[g_render_structure_globals.depth];
    unsigned long count = (unsigned short)g_cluster_sorted_order.m_count - first;
    if (count >= 2)
        templated_sort<short>::qsort(&g_cluster_sorted_order.m_order[first], count, cluster_sort_proc, (const void *)g_cluster_sorted_order.m_context);
    c_structure_renderer::render_cluster_parts(_entry_point_shadow_generate, 2);
}

// @address 0x46a390 ?render@c_occlusion_view@@UAAXPBTreal_point3d@@@Z
void c_occlusion_view::render(const real_point3d *point)
{
    c_visible_items::push_marker();
    memcpy(&m_object_bounding_box, &g_effect_camera_world_info, sizeof(m_object_bounding_box));
    compute_visibility(point);
    setup_camera(c_rasterizer::render_globals.shadow_size, c_rasterizer::render_globals.shadow_size);
    render_setup();
    render_occlusions_push_structure_marker();
    render_occlusions_push_object_marker();
    render_submit_visibility();
    c_view::begin(this);
    render_occlusions_setup_depth_target(1, c_rasterizer::render_globals.shadow_size, c_rasterizer::render_globals.shadow_size);
    render_occlusions();
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)2, 4, 0, 0, 0);
    c_view::end();
    c_rasterizer::clear_sampler_textures(0xffff);
    render_occlusions_pop_object_marker();
    render_occlusions_pop_structure_marker();
    c_visible_items::pop_marker();
}
