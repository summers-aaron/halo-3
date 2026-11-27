// graphicslibxbox/render_transparents.obj: from the records 0x9bcb8 0x17f810 0x463400 0x463558
// @flags /TP /O2 /Oc
// @address 0x9bcb8 ?plane3d_distance_to_point@@YAMPBUreal_plane3d@@PBTreal_point3d@@@Z
// @address 0x463400 render_transparents_test_sort_order
// @address 0x865334 ?m_markers@c_transparency_renderer@@0PAUs_transparency_marker@@A
// @address 0x865340 ?transparents@c_transparency_renderer@@0V?$c_static_array@Us_transparent_types@@$0EAA@@@A
// @address 0x7beac0 ?transparent_sorted_order@c_transparency_renderer@@0V?$c_sorter@Us_transparent_types@@$0EAA@@@A
// @address 0x8c484c g_transparent_sort_mode
// @address 0x72cb03 g_render_hiz_stencil_enabled
// @address 0x10f5d70 g_rasterizer_xenon_memory
#include <xtl.h>
#include <ppcintrinsics.h>
#define REAL_MATH_PLANE_DISTANCE_LINKAGE extern
#include "math/real_math.h"
#undef REAL_MATH_PLANE_DISTANCE_LINKAGE
#include "render/render_transparents.h"
#include "render/render_objects.h"
#include "render/render_structure.h"
#include "render/lighting_interface.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/xenon/rasterizer_xenon_memory.h"
#include "render/render_structure_globals_types.h"

s_transparency_marker c_transparency_renderer::m_markers[6];

// The sorter holds a link-time pointer to the transparent records, so it stays an
// aggregate with no constructor call.
c_static_array<s_transparent_types, 1024> c_transparency_renderer::transparents;
c_sorter<s_transparent_types, 1024> c_transparency_renderer::transparent_sorted_order = { { 0 }, 0, transparents.m_storage };

struct s_transparent_side_vote { float value; };

extern "C" bool render_transparents_test_sort_order(unsigned short index_a, unsigned short index_b, s_transparent_types *records)
{
    s_transparent_types *a = &records[index_a];
    s_transparent_types *b = &records[index_b];
    if (a->sort_layer != b->sort_layer)
        return a->sort_layer > b->sort_layer;
    if (g_transparent_sort_mode == 2 && a->has_plane + b->has_plane == 1) {
        struct { real_point3d point; float pad; real_plane3d plane; } l;
        bool behind;
        bool in_front;
        if (a->has_plane) {
            l.point = b->position;
            l.plane = a->plane;
            in_front = false;
            behind = true;
        } else {
            l.point = a->position;
            l.plane = b->plane;
            in_front = true;
            behind = false;
        }
        float distance = l.point.x * l.plane.n.i + l.plane.n.j * l.point.y;
        distance += l.plane.n.k * l.point.z;
        if (distance - l.plane.d < 0.0f)
            return behind;
        return in_front;
    }
    return a->sort_key > b->sort_key;
}

// @address 0x17f810 ?render@c_transparency_renderer@@SAX_N@Z
void c_transparency_renderer::render(bool set_z_buffer)
{
    if (g_render_hiz_stencil_enabled)
        render_objects_set_hiz_stencil_state(&g_rasterizer_xenon_memory);
    if (set_z_buffer)
        ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)1);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 7);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), D3DCULL_CW);
    if (!g_render_structure_globals.lightmap_geometry_valid)
        c_lighting_interface::setup_default_lighting();
    long first_transparent = m_markers[m_current_marker_index].first_transparent_index;
    long total_transparent_count = m_total_transparent_count;
    for (long transparent_index = first_transparent; transparent_index < total_transparent_count; ++transparent_index) {
        s_transparent_types *transparent = &transparents[transparent_sorted_order.get_order_entry(transparent_index)];
        transparent->render_callback(transparent->render_data, transparent->user_data);
    }
}

static __forceinline s_transparent_side_vote render_transparents_sign(float distance)
{
    s_transparent_side_vote result;
    result.value = (float)__fsel(distance, 1.0f, -1.0f);
    return result;
}

// @address 0x463558 render_transparents_compute_side_votes
extern "C" long render_transparents_compute_side_votes(unsigned short index_a, unsigned short index_b)
{
    const s_transparent_types *a = &c_transparency_renderer::transparents[index_a];
    const s_transparent_types *b = &c_transparency_renderer::transparents[index_b];
    float sum = 0.0f;
    for (long i = 0; i < 9; ++i)
        sum += render_transparents_sign(plane3d_distance_to_point(&b->plane, &a->sample_points[i])).value;
    return (long)sum;
}
