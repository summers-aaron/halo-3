// graphicslibxbox/render_structure.obj: from the records 0x88c60 0x162958 0x163450 0x163490 0x1660f8 0x1752e8 0x1753b0 0x175490 0x175a78 0x175c68 0x17a778 0x17a830 0x182688 0x18f2c8 0x449748 0x449758 0x449760 0x4497a0 0x449890 0x4498d0 0x449910 0x4499b0 0x16f8c0
// @address 0x74b898 g_geometry_globals
// @address 0x8c46eb ?m_valid@c_render_method_data@@2_NA
// @address 0x8c482e g_render_method_submit_cache_bypass
// @address 0x8c482f g_render_method_submit_cached_state_enabled
#include "geometry/geometry_material_types.h"
#include "structures/structure_instanced_geometry.h"
#include "physics/collision_bsp_cache_types.h"
#include "geometry/geometry_definitions.h"
// @flags /TP /O2 /Oc
#include "render/render_objects_globals.h"
#include "physics/breakable_surface_globals.h"
#include "render/render_objects_static_lighting.h"
#include "cseries/long_sort.h"
#include "render/render_water_globals.h"
#if !defined(_XBOX) || defined(__clang__)
#include "render/render_water.h"
#include "../../port/sdk/runtime/port_config.h"
#endif
#include "render_methods/render_method_submit_globals.h"
#include "render_methods/render_method_data.h"
#include "objects/object_reference_types.h"
#include "geometry/compression_info_types.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
// @address 0x88c60 ?get_resources@c_structure_bsp_resource_interface@@QBAPAUs_structure_bsp_resources@@XZ
// @address 0x163450 ?pop_marker@c_structure_renderer@@SAXXZ
// @address 0x163490 ?get_is_transparent@c_render_method@@SA_NJ@Z
// @address 0x175a78 structure_renderer_setup_instance_lighting
// @address 0x17a830 xm_convert_half_to_float_stream
// @address 0x449748 ?initialize_for_new_structure_bsp@c_structure_renderer@@SAXK@Z
// @address 0x449758 ?dispose_from_old_structure_bsp@c_structure_renderer@@SAXK@Z
// @address 0x449760 ?initialize_for_new_map@c_structure_renderer@@SAXXZ
// @address 0x449760 ?dispose_from_old_map@c_structure_renderer@@SAXXZ
// @address 0x4497a0 ?reset_bsp_cached_data@c_structure_renderer@@CAXK@Z
// @address 0x449890 ?instance_list_sort_proc@@YA_NFFPBX@Z
// @address 0x18f2c8 ?render_transparent_cluster_mesh_part@c_structure_renderer@@SAXPBXJ@Z
// @address 0x4499b0 structure_renderer_render_static_lighting
// @address 0x449910 structure_renderer_render_shadows_generate
#include <xtl.h>
#include "multithreading/thread_tls.h"
#include "rasterizer/rasterizer.h"
#include <xboxmath.h>
#include <string.h>
#include "math/real_math.h"
#include "models/model_skinning.h"
#include "physics/breakable_surfaces.h"
#include "render/render_mesh.h"
#include "render/render_structure.h"
#include "render/render.h"
#include "render/render_structure_lighting.h"
#include "render_methods/render_method_submit_geometry.h"
#include "math/xbox_math.h"
#include "render_methods/render_method_submit.h"
#include "scenario/scenario_lightmap_definitions.h"
#include "rasterizer/rasterizer_states.h"
#include "scenario/scenario_geometry.h"
#include "math/matrix_math.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_lightmap_types.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "geometry/render_geometry_types.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "effects/transparent_sort_layer_types.h"
#include "geometry/geometry_block_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "structures/structure_bsp_tag.h"
#include "render/lighting_interface.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cache/geometry_storage.h"
#include "structures/structure_runtime_resources.h"
#include "physics/breakable_surface_types.h"
#include "render/render_transparents.h"
#include "render/render_player_view_globals.h"

#include "rasterizer/rasterizer_geometry_types.h"

// a base class 0x320 bytes into s_geometry_storage_member: converting a pointer to it checks for null




#include "render/render_structure_globals_types.h"
#include "memory/datum_index.h"

#include "scenario/wind_definitions.h"
// @address 0x81dc34 ?g_wind_values@@3Us_wind_values@@A
s_wind_values g_wind_values;
render_structure_globals g_render_structure_globals = {0};
c_sorter<s_render_cluster_part, 512> g_cluster_sorted_order = {{0}, 0, g_render_structure_globals.cluster_parts};
#include "render/structure_renderer.h"

#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "render_methods/render_method.h"
#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"
#include "cseries/read_write_barrier.h"
#include "geometry/render_geometry_part_types.h"
#include "geometry/render_geometry_mesh_types.h"
#include "scenario/scenario_lightmap_lighting_types.h"
#include "rasterizer/rasterizer_stencil.h"
#include "geometry/geometry_material_types.h"
#include "structures/structure_instanced_geometry.h"
#include "geometry/transparent_sorting_node_types.h"
#include "cseries/cseries.h"
#include "cubemaps/cubemap_sample.h"
#include "bitmaps/bitmap_group.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"
#include "tag_files/tag_groups.h"

#if !defined(_XBOX) || defined(__clang__)
#include "geometry/render_geometry_parts.h"
#include "main/main_render_prediction_types.h"
#include "models/render_model_definitions.h"
#include "rasterizer/xenon/rasterizer_shader_bounds.h"
#include "rasterizer/xenon/rasterizer_xenon_submit.h"
#include "render/render_lightmap_shadows_globals.h"
#include "render/views/player_view_types.h"
#include "render/views/render_player_view.h"
#include "scenario/scenario_access.h"
#include "scenario/scenario_definitions.h"
#include "sky_atm/atmosphere_external.h"
#include "sky_atm/real_linear_rgb_color_types.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_globals.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "visibility/visibility_region_types.h"
#endif

struct s_render_cluster
{
    long structure_bsp_index;
    long cluster_index;
    long flags;
};

struct __declspec(align(16)) s_lighting_data
{
    real_vector3d dominant_direction;
    float direction_padding;
    real_rgb_color dominant_intensity;
    float intensity_padding;
    float red[9];
    float green[9];
    float blue[9];
};

struct s_render_method_extern_parameter {
    long type;
    long register_index;
    long register_count;
    unsigned long flags;
    const char *name;
};

struct s_render_method_extern_parameter_table {
    s_render_method_extern_parameter parameters[49];
    long reserved;
};

s_structure_bsp_resources *c_structure_bsp_resource_interface::get_resources() const
{
  unsigned int index = m_datum_index;
  s_geometry_storage_member *member = &g_geometry_globals.manager->m_resource_cache.m_geometry_storage;
  s_geometry_second_base *secondary = member;
  return static_cast<s_structure_bsp_resources *>(
      secondary->m_per_thread_access_cache[get_current_thread_index()]->m_resource_values[datum_index_to_absolute_index(index)]);
}

void c_structure_renderer::pop_marker()
{
  g_render_structure_globals.depth--;
  g_render_structure_globals.current_a = g_render_structure_globals.stack_a[g_render_structure_globals.depth];
  g_cluster_sorted_order.m_count = (short)g_render_structure_globals.current_a;
  g_render_structure_globals.current_b = g_render_structure_globals.stack_b[g_render_structure_globals.depth];
}

class c_rasterizer_compiled_pixel_shader;

// match: 0x162958: inline lets cl expand this into submit_visibility; plain, submit_visibility keeps the call and its size changes by 48 bytes
#if defined(_XBOX) && !defined(__clang__)
inline bool c_render_method::get_is_transparent(long index)
#else
bool c_render_method::get_is_transparent(long index)
#endif
{
    bool result = index != -1 && get(index)->get_postprocess_definition()->alpha_blend_mode != 0;
    return result;
}


namespace {
static __forceinline scenario *render_structure_global_scenario_try_and_get()
{
    return global_scenario;
}
}

// out-of-line XMConvertHalfToFloatStream copy, written as the xbox math header has it
FLOAT* xm_convert_half_to_float_stream
(
    FLOAT*      pOutputStream, 
    UINT        OutputStride, 
    CONST HALF* pInputStream, 
    UINT        InputStride, 
    UINT        HalfCount
)
{
#if defined(_NO_INTRINSICS_)

    UINT  i;
    BYTE* pHalf = (BYTE*)pInputStream;
    BYTE* pFloat = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < HalfCount; i++)
    {
        *(FLOAT*)pFloat = XMConvertHalfToFloat(*(HALF*)pHalf);
        pHalf += InputStride;
        pFloat += OutputStride; 
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    UINT  i;
    BYTE* pHalf = (BYTE*)pInputStream;
    BYTE* pFloat = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < HalfCount; i++)
    {
        *(FLOAT*)pFloat = XMConvertHalfToFloat(*(HALF*)pHalf);
        pHalf += InputStride;
        pFloat += OutputStride; 
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    CONST UINT UnrollCount = 8;
    XMVECTOR V0[8], V1[8], V2[8], V3[8];
    UINT_PTR InputVector = (UINT_PTR)pInputStream;
    UINT_PTR OutputVector = (UINT_PTR)pOutputStream;

//    C_ASSERT(UnrollCount == 8);
    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);
    XMASSERT(((UINT_PTR)pInputStream & 1) == 0);
    XMASSERT(((UINT_PTR)pOutputStream & 3) == 0);

    if (InputStride == sizeof(HALF))
    {
        if (OutputStride == sizeof(FLOAT))
        {
            UINT GroupIndex;
            CONST UINT VGroupSize = 2 * sizeof(XMVECTOR);
            UINT_PTR OutputStreamEnd = OutputVector + HalfCount * sizeof(FLOAT);
            UINT_PTR OutputStreamCGroupBase = XMMin(OutputVector + (XM_CACHE_LINE_SIZE - 1), OutputStreamEnd) & ~(XM_CACHE_LINE_SIZE - 1);
            UINT_PTR OutputStreamCGroupEnd = OutputStreamCGroupBase + ((OutputStreamEnd - OutputStreamCGroupBase) & ~(4 * XM_CACHE_LINE_SIZE - 1));
            UINT_PTR OutputStreamVGroupBase = XMMin(OutputVector + (VGroupSize - 1), OutputStreamEnd) & ~(VGroupSize - 1);
            UINT_PTR OutputStreamVGroupEnd = OutputStreamEnd & ~(VGroupSize - 1);

            __dcbt(0,                      (VOID*)OutputStreamCGroupBase);
            __dcbt(XM_CACHE_LINE_SIZE,     (VOID*)OutputStreamCGroupBase);
            __dcbt(XM_CACHE_LINE_SIZE * 2, (VOID*)OutputStreamCGroupBase);
            __dcbt(XM_CACHE_LINE_SIZE * 3, (VOID*)OutputStreamCGroupBase);

            while (OutputVector < OutputStreamVGroupBase)
            {
                V0[0] = __lvlx((VOID*)InputVector, 0);
                V0[0] = __vsplth(V0[0], 0);
                V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

                __stvewx(V0[0], (VOID*)OutputVector, 0);

                InputVector += sizeof(HALF); 
                OutputVector += sizeof(FLOAT);
            }

            while (OutputVector < OutputStreamCGroupBase)
            {
                V0[0] = __lvlx((VOID*)InputVector, 0);
                V1[0] = __lvrx((VOID*)InputVector, 16);
                V0[0] = __vor(V0[0], V1[0]);
                InputVector += 8 * sizeof(HALF); 

                V1[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);
                V0[0] = __vsldoi(V0[0], V0[0], 2 << 2);
                V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

                __stvx(V0[0], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V1[0], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
            }

            while (OutputVector < OutputStreamCGroupEnd)
            {
                __dcbt(XM_CACHE_LINE_SIZE * 4, (VOID*)OutputVector);
                __dcbt(XM_CACHE_LINE_SIZE * 5, (VOID*)OutputVector);
                __dcbt(XM_CACHE_LINE_SIZE * 6, (VOID*)OutputVector);
                __dcbt(XM_CACHE_LINE_SIZE * 7, (VOID*)OutputVector);

                for (GroupIndex = 0; GroupIndex < 4; GroupIndex++)
                {
                    V0[0] = __lvlx((VOID*)InputVector, 0);
                    V1[0] = __lvrx((VOID*)InputVector, 16);
                    InputVector += 8 * sizeof(HALF); 
                    V0[1] = __lvlx((VOID*)InputVector, 0);
                    V1[1] = __lvrx((VOID*)InputVector, 16);
                    InputVector += 8 * sizeof(HALF); 
                    V0[2] = __lvlx((VOID*)InputVector, 0);
                    V1[2] = __lvrx((VOID*)InputVector, 16);
                    InputVector += 8 * sizeof(HALF); 
                    V0[3] = __lvlx((VOID*)InputVector, 0);
                    V1[3] = __lvrx((VOID*)InputVector, 16);
                    InputVector += 8 * sizeof(HALF); 
                    V0[4] = __lvlx((VOID*)InputVector, 0);
                    V1[4] = __lvrx((VOID*)InputVector, 16);
                    InputVector += 8 * sizeof(HALF); 
                    V0[5] = __lvlx((VOID*)InputVector, 0);
                    V1[5] = __lvrx((VOID*)InputVector, 16);
                    InputVector += 8 * sizeof(HALF); 
                    V0[6] = __lvlx((VOID*)InputVector, 0);
                    V1[6] = __lvrx((VOID*)InputVector, 16);
                    InputVector += 8 * sizeof(HALF); 
                    V0[7] = __lvlx((VOID*)InputVector, 0);
                    V1[7] = __lvrx((VOID*)InputVector, 16);
                    InputVector += 8 * sizeof(HALF); 

                    V0[0] = __vor(V0[0], V1[0]);
                    V0[1] = __vor(V0[1], V1[1]);
                    V0[2] = __vor(V0[2], V1[2]);
                    V0[3] = __vor(V0[3], V1[3]);
                    V0[4] = __vor(V0[4], V1[4]);
                    V0[5] = __vor(V0[5], V1[5]);
                    V0[6] = __vor(V0[6], V1[6]);
                    V0[7] = __vor(V0[7], V1[7]);

                    V1[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);
                    V1[1] = __vupkd3d(V0[1], VPACK_FLOAT16_4);
                    V1[2] = __vupkd3d(V0[2], VPACK_FLOAT16_4);
                    V1[3] = __vupkd3d(V0[3], VPACK_FLOAT16_4);
                    V1[4] = __vupkd3d(V0[4], VPACK_FLOAT16_4);
                    V1[5] = __vupkd3d(V0[5], VPACK_FLOAT16_4);
                    V1[6] = __vupkd3d(V0[6], VPACK_FLOAT16_4);
                    V1[7] = __vupkd3d(V0[7], VPACK_FLOAT16_4);

                    V0[0] = __vsldoi(V0[0], V0[0], 2 << 2);
                    V0[1] = __vsldoi(V0[1], V0[1], 2 << 2);
                    V0[2] = __vsldoi(V0[2], V0[2], 2 << 2);
                    V0[3] = __vsldoi(V0[3], V0[3], 2 << 2);
                    V0[4] = __vsldoi(V0[4], V0[4], 2 << 2);
                    V0[5] = __vsldoi(V0[5], V0[5], 2 << 2);
                    V0[6] = __vsldoi(V0[6], V0[6], 2 << 2);
                    V0[7] = __vsldoi(V0[7], V0[7], 2 << 2);

                    V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);
                    V0[1] = __vupkd3d(V0[1], VPACK_FLOAT16_4);
                    V0[2] = __vupkd3d(V0[2], VPACK_FLOAT16_4);
                    V0[3] = __vupkd3d(V0[3], VPACK_FLOAT16_4);
                    V0[4] = __vupkd3d(V0[4], VPACK_FLOAT16_4);
                    V0[5] = __vupkd3d(V0[5], VPACK_FLOAT16_4);
                    V0[6] = __vupkd3d(V0[6], VPACK_FLOAT16_4);
                    V0[7] = __vupkd3d(V0[7], VPACK_FLOAT16_4);

                    __stvx(V0[0], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                    __stvx(V1[0], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);

                    __stvx(V0[1], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                    __stvx(V1[1], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);

                    __stvx(V0[2], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                    __stvx(V1[2], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);

                    __stvx(V0[3], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                    __stvx(V1[3], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);

                    __stvx(V0[4], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                    __stvx(V1[4], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);

                    __stvx(V0[5], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                    __stvx(V1[5], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);

                    __stvx(V0[6], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                    __stvx(V1[6], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);

                    __stvx(V0[7], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                    __stvx(V1[7], (VOID*)OutputVector, 0);
                    OutputVector += 4 * sizeof(FLOAT);
                }
            }

            while (OutputVector < OutputStreamVGroupEnd)
            {
                V0[0] = __lvlx((VOID*)InputVector, 0);
                V1[0] = __lvrx((VOID*)InputVector, 16);
                V0[0] = __vor(V0[0], V1[0]);
                InputVector += 8 * sizeof(HALF); 

                V1[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);
                V0[0] = __vsldoi(V0[0], V0[0], 2 << 2);
                V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

                __stvx(V0[0], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V1[0], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
            }

            while (OutputVector < OutputStreamEnd)
            {
                V0[0] = __lvlx((VOID*)InputVector, 0);
                V0[0] = __vsplth(V0[0], 0);
                V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

                __stvewx(V0[0], (VOID*)OutputVector, 0);

                InputVector += sizeof(HALF); 
                OutputVector += sizeof(FLOAT);
            }
        }
        else
        {
            UINT GroupIndex;
            CONST UINT VGroupSize = sizeof(XMVECTOR);
            XMVECTOR V4[8], V5[8], V6[8], V7[8];
            UINT_PTR InputStreamEnd = InputVector + HalfCount * sizeof(HALF);
            UINT_PTR InputStreamCGroupBase = XMMin(InputVector + (XM_CACHE_LINE_SIZE - 1), InputStreamEnd) & ~(XM_CACHE_LINE_SIZE - 1);
            UINT_PTR InputStreamCGroupEnd = InputStreamCGroupBase + ((InputStreamEnd - InputStreamCGroupBase) & ~(4 * XM_CACHE_LINE_SIZE - 1));
            UINT_PTR InputStreamVGroupBase = XMMin(InputVector + (VGroupSize - 1), InputStreamEnd) & ~(VGroupSize - 1);
            UINT_PTR InputStreamVGroupEnd = InputStreamEnd & ~(VGroupSize - 1);

            __dcbt(0,                      (VOID*)InputStreamCGroupBase);
            __dcbt(XM_CACHE_LINE_SIZE,     (VOID*)InputStreamCGroupBase);
            __dcbt(XM_CACHE_LINE_SIZE * 2, (VOID*)InputStreamCGroupBase);
            __dcbt(XM_CACHE_LINE_SIZE * 3, (VOID*)InputStreamCGroupBase);

            while (InputVector < InputStreamVGroupBase)
            {
                V0[0] = __lvlx((VOID*)InputVector, 0);
                V0[0] = __vsplth(V0[0], 0);
                V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

                __stvewx(V0[0], (VOID*)OutputVector, 0);

                InputVector += sizeof(HALF); 
                OutputVector += OutputStride;
            }

            while (InputVector < InputStreamCGroupBase)
            {
                V3[0] = __lvx((VOID*)InputVector, 0);
                InputVector += 8 * sizeof(HALF); 

                V7[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);
                V3[0] = __vsldoi(V3[0], V3[0], 2 << 2);
                V3[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);

                V4[0] = __vspltw(V7[0], 0);
                V5[0] = __vspltw(V7[0], 1);
                V6[0] = __vspltw(V7[0], 2);
                V7[0] = __vspltw(V7[0], 3);

                V0[0] = __vspltw(V3[0], 0);
                V1[0] = __vspltw(V3[0], 1);
                V2[0] = __vspltw(V3[0], 2);
                V3[0] = __vspltw(V3[0], 3);

                __stvewx(V0[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V1[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V2[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V3[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;

                __stvewx(V4[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V5[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V6[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V7[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
            }

            while (InputVector < InputStreamCGroupEnd)
            {
                __dcbt(XM_CACHE_LINE_SIZE * 4, (VOID*)InputVector);
                __dcbt(XM_CACHE_LINE_SIZE * 5, (VOID*)InputVector);
                __dcbt(XM_CACHE_LINE_SIZE * 6, (VOID*)InputVector);
                __dcbt(XM_CACHE_LINE_SIZE * 7, (VOID*)InputVector);

                for (GroupIndex = 0; GroupIndex < 4; GroupIndex++)
                {
                    V3[0] = __lvx((VOID*)InputVector, 0);
                    InputVector += sizeof(XMVECTOR); 
                    V3[1] = __lvx((VOID*)InputVector, 0);
                    InputVector += sizeof(XMVECTOR); 
                    V3[2] = __lvx((VOID*)InputVector, 0);
                    InputVector += sizeof(XMVECTOR); 
                    V3[3] = __lvx((VOID*)InputVector, 0);
                    InputVector += sizeof(XMVECTOR); 
                    V3[4] = __lvx((VOID*)InputVector, 0);
                    InputVector += sizeof(XMVECTOR); 
                    V3[5] = __lvx((VOID*)InputVector, 0);
                    InputVector += sizeof(XMVECTOR); 
                    V3[6] = __lvx((VOID*)InputVector, 0);
                    InputVector += sizeof(XMVECTOR); 
                    V3[7] = __lvx((VOID*)InputVector, 0);
                    InputVector += sizeof(XMVECTOR); 

                    V7[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);
                    V7[1] = __vupkd3d(V3[1], VPACK_FLOAT16_4);
                    V7[2] = __vupkd3d(V3[2], VPACK_FLOAT16_4);
                    V7[3] = __vupkd3d(V3[3], VPACK_FLOAT16_4);
                    V7[4] = __vupkd3d(V3[4], VPACK_FLOAT16_4);
                    V7[5] = __vupkd3d(V3[5], VPACK_FLOAT16_4);
                    V7[6] = __vupkd3d(V3[6], VPACK_FLOAT16_4);
                    V7[7] = __vupkd3d(V3[7], VPACK_FLOAT16_4);

                    V3[0] = __vsldoi(V3[0], V3[0], 2 << 2);
                    V3[1] = __vsldoi(V3[1], V3[1], 2 << 2);
                    V3[2] = __vsldoi(V3[2], V3[2], 2 << 2);
                    V3[3] = __vsldoi(V3[3], V3[3], 2 << 2);
                    V3[4] = __vsldoi(V3[4], V3[4], 2 << 2);
                    V3[5] = __vsldoi(V3[5], V3[5], 2 << 2);
                    V3[6] = __vsldoi(V3[6], V3[6], 2 << 2);
                    V3[7] = __vsldoi(V3[7], V3[7], 2 << 2);

                    V3[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);
                    V3[1] = __vupkd3d(V3[1], VPACK_FLOAT16_4);
                    V3[2] = __vupkd3d(V3[2], VPACK_FLOAT16_4);
                    V3[3] = __vupkd3d(V3[3], VPACK_FLOAT16_4);
                    V3[4] = __vupkd3d(V3[4], VPACK_FLOAT16_4);
                    V3[5] = __vupkd3d(V3[5], VPACK_FLOAT16_4);
                    V3[6] = __vupkd3d(V3[6], VPACK_FLOAT16_4);
                    V3[7] = __vupkd3d(V3[7], VPACK_FLOAT16_4);

                    V4[0] = __vspltw(V7[0], 0);
                    V5[0] = __vspltw(V7[0], 1);
                    V6[0] = __vspltw(V7[0], 2);
                    V7[0] = __vspltw(V7[0], 3);

                    V0[0] = __vspltw(V3[0], 0);
                    V1[0] = __vspltw(V3[0], 1);
                    V2[0] = __vspltw(V3[0], 2);
                    V3[0] = __vspltw(V3[0], 3);

                    V4[1] = __vspltw(V7[1], 0);
                    V5[1] = __vspltw(V7[1], 1);
                    V6[1] = __vspltw(V7[1], 2);
                    V7[1] = __vspltw(V7[1], 3);

                    V0[1] = __vspltw(V3[1], 0);
                    V1[1] = __vspltw(V3[1], 1);
                    V2[1] = __vspltw(V3[1], 2);
                    V3[1] = __vspltw(V3[1], 3);

                    V4[2] = __vspltw(V7[2], 0);
                    V5[2] = __vspltw(V7[2], 1);
                    V6[2] = __vspltw(V7[2], 2);
                    V7[2] = __vspltw(V7[2], 3);

                    V0[2] = __vspltw(V3[2], 0);
                    V1[2] = __vspltw(V3[2], 1);
                    V2[2] = __vspltw(V3[2], 2);
                    V3[2] = __vspltw(V3[2], 3);

                    V4[3] = __vspltw(V7[3], 0);
                    V5[3] = __vspltw(V7[3], 1);
                    V6[3] = __vspltw(V7[3], 2);
                    V7[3] = __vspltw(V7[3], 3);
                                        
                    V0[3] = __vspltw(V3[3], 0);
                    V1[3] = __vspltw(V3[3], 1);
                    V2[3] = __vspltw(V3[3], 2);
                    V3[3] = __vspltw(V3[3], 3);

                    V4[4] = __vspltw(V7[4], 0);
                    V5[4] = __vspltw(V7[4], 1);
                    V6[4] = __vspltw(V7[4], 2);
                    V7[4] = __vspltw(V7[4], 3);
                                        
                    V0[4] = __vspltw(V3[4], 0);
                    V1[4] = __vspltw(V3[4], 1);
                    V2[4] = __vspltw(V3[4], 2);
                    V3[4] = __vspltw(V3[4], 3);

                    V4[5] = __vspltw(V7[5], 0);
                    V5[5] = __vspltw(V7[5], 1);
                    V6[5] = __vspltw(V7[5], 2);
                    V7[5] = __vspltw(V7[5], 3);
                                        
                    V0[5] = __vspltw(V3[5], 0);
                    V1[5] = __vspltw(V3[5], 1);
                    V2[5] = __vspltw(V3[5], 2);
                    V3[5] = __vspltw(V3[5], 3);

                    V4[6] = __vspltw(V7[6], 0);
                    V5[6] = __vspltw(V7[6], 1);
                    V6[6] = __vspltw(V7[6], 2);
                    V7[6] = __vspltw(V7[6], 3);
                                        
                    V0[6] = __vspltw(V3[6], 0);
                    V1[6] = __vspltw(V3[6], 1);
                    V2[6] = __vspltw(V3[6], 2);
                    V3[6] = __vspltw(V3[6], 3);

                    V4[7] = __vspltw(V7[7], 0);
                    V5[7] = __vspltw(V7[7], 1);
                    V6[7] = __vspltw(V7[7], 2);
                    V7[7] = __vspltw(V7[7], 3);

                    V0[7] = __vspltw(V3[7], 0);
                    V1[7] = __vspltw(V3[7], 1);
                    V2[7] = __vspltw(V3[7], 2);
                    V3[7] = __vspltw(V3[7], 3);

                    __stvewx(V0[0], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V1[0], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V2[0], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V3[0], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V4[0], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V5[0], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V6[0], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V7[0], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V0[1], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V1[1], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V2[1], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V3[1], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V4[1], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V5[1], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V6[1], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V7[1], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V0[2], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V1[2], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V2[2], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V3[2], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V4[2], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V5[2], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V6[2], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V7[2], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V0[3], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V1[3], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V2[3], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V3[3], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V4[3], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V5[3], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V6[3], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V7[3], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V0[4], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V1[4], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V2[4], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V3[4], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V4[4], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V5[4], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V6[4], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V7[4], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V0[5], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V1[5], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V2[5], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V3[5], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V4[5], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V5[5], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V6[5], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V7[5], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V0[6], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V1[6], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V2[6], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V3[6], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V4[6], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V5[6], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V6[6], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V7[6], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V0[7], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V1[7], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V2[7], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V3[7], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;

                    __stvewx(V4[7], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V5[7], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V6[7], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                    __stvewx(V7[7], (VOID*)OutputVector, 0);
                    OutputVector += OutputStride;
                }
            }

            while (InputVector < InputStreamVGroupEnd)
            {
                V3[0] = __lvx((VOID*)InputVector, 0);
                InputVector += 8 * sizeof(HALF); 

                V7[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);
                V3[0] = __vsldoi(V3[0], V3[0], 2 << 2);
                V3[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);

                V4[0] = __vspltw(V7[0], 0);
                V5[0] = __vspltw(V7[0], 1);
                V6[0] = __vspltw(V7[0], 2);
                V7[0] = __vspltw(V7[0], 3);

                V0[0] = __vspltw(V3[0], 0);
                V1[0] = __vspltw(V3[0], 1);
                V2[0] = __vspltw(V3[0], 2);
                V3[0] = __vspltw(V3[0], 3);

                __stvewx(V0[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V1[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V2[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V3[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;

                __stvewx(V4[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V5[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V6[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
                __stvewx(V7[0], (VOID*)OutputVector, 0);
                OutputVector += OutputStride;
            }

            while (InputVector < InputStreamEnd)
            {
                V0[0] = __lvlx((VOID*)InputVector, 0);
                V0[0] = __vsplth(V0[0], 0);
                V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

                __stvewx(V0[0], (VOID*)OutputVector, 0);

                InputVector += sizeof(HALF); 
                OutputVector += OutputStride;
            }
        }
    }
    else if (OutputStride == sizeof(FLOAT))
    {
        UINT GroupIndex;
        CONST UINT VGroupSize = 2 * sizeof(XMVECTOR);
        XMVECTOR V4[8], V5[8], V6[8], V7[8];
        UINT_PTR OutputStreamEnd = OutputVector + HalfCount * sizeof(FLOAT);
        UINT_PTR OutputStreamCGroupBase = XMMin(OutputVector + (XM_CACHE_LINE_SIZE - 1), OutputStreamEnd) & ~(XM_CACHE_LINE_SIZE - 1);
        UINT_PTR OutputStreamCGroupEnd = OutputStreamCGroupBase + ((OutputStreamEnd - OutputStreamCGroupBase) & ~(4 * XM_CACHE_LINE_SIZE - 1));
        UINT_PTR OutputStreamVGroupBase = XMMin(OutputVector + (VGroupSize - 1), OutputStreamEnd) & ~(VGroupSize - 1);
        UINT_PTR OutputStreamVGroupEnd = OutputStreamEnd & ~(VGroupSize - 1);

        __dcbt(0,                      (VOID*)OutputStreamCGroupBase);
        __dcbt(XM_CACHE_LINE_SIZE,     (VOID*)OutputStreamCGroupBase);
        __dcbt(XM_CACHE_LINE_SIZE * 2, (VOID*)OutputStreamCGroupBase);
        __dcbt(XM_CACHE_LINE_SIZE * 3, (VOID*)OutputStreamCGroupBase);

        while (OutputVector < OutputStreamVGroupBase)
        {
            V0[0] = __lvlx((VOID*)InputVector, 0);
            V0[0] = __vsplth(V0[0], 0);
            V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

            __stvewx(V0[0], (VOID*)OutputVector, 0);

            InputVector += InputStride; 
            OutputVector += sizeof(FLOAT);
        }

        while (OutputVector < OutputStreamCGroupBase)
        {
            V0[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V1[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V2[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V3[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V4[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V5[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V6[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V7[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;

            V0[0] = __vmrghh(V0[0], V1[0]);
            V2[0] = __vmrghh(V2[0], V3[0]);
            V4[0] = __vmrghh(V4[0], V5[0]);
            V6[0] = __vmrghh(V6[0], V7[0]);

            V0[0] = __vrlimi(V0[0], V2[0], 0x4, 3);
            V4[0] = __vrlimi(V4[0], V6[0], 0x4, 3);

            V3[0] = __vsldoi(V0[0], V0[0], 2 << 2);
            V7[0] = __vsldoi(V4[0], V4[0], 2 << 2);

            V3[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);
            V7[0] = __vupkd3d(V7[0], VPACK_FLOAT16_4);

            __stvx(V3[0], (VOID*)OutputVector, 0);
            OutputVector += 4 * sizeof(FLOAT);
            __stvx(V7[0], (VOID*)OutputVector, 0);
            OutputVector += 4 * sizeof(FLOAT);
        }

        while (OutputVector < OutputStreamCGroupEnd)
        {
            __dcbt(XM_CACHE_LINE_SIZE * 4, (VOID*)OutputVector);
            __dcbt(XM_CACHE_LINE_SIZE * 5, (VOID*)OutputVector);
            __dcbt(XM_CACHE_LINE_SIZE * 6, (VOID*)OutputVector);
            __dcbt(XM_CACHE_LINE_SIZE * 7, (VOID*)OutputVector);

            for (GroupIndex = 0; GroupIndex < 4; GroupIndex++)
            {
                V0[0] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V1[0] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V2[0] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V3[0] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V4[0] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V5[0] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V6[0] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V7[0] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;

                V0[1] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V1[1] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V2[1] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V3[1] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V4[1] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V5[1] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V6[1] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V7[1] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;

                V0[2] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V1[2] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V2[2] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V3[2] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V4[2] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V5[2] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V6[2] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V7[2] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;

                V0[3] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V1[3] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V2[3] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V3[3] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V4[3] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V5[3] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V6[3] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V7[3] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;

                V0[4] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V1[4] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V2[4] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V3[4] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V4[4] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V5[4] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V6[4] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V7[4] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;

                V0[5] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V1[5] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V2[5] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V3[5] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V4[5] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V5[5] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V6[5] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V7[5] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;

                V0[6] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V1[6] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V2[6] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V3[6] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V4[6] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V5[6] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V6[6] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V7[6] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;

                V0[7] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V1[7] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V2[7] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V3[7] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V4[7] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V5[7] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V6[7] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;
                V7[7] = __lvlx((VOID*)InputVector, 0);
                InputVector += InputStride;

                V0[0] = __vmrghh(V0[0], V1[0]);
                V2[0] = __vmrghh(V2[0], V3[0]);
                V4[0] = __vmrghh(V4[0], V5[0]);
                V6[0] = __vmrghh(V6[0], V7[0]);
                V0[1] = __vmrghh(V0[1], V1[1]);
                V2[1] = __vmrghh(V2[1], V3[1]);
                V4[1] = __vmrghh(V4[1], V5[1]);
                V6[1] = __vmrghh(V6[1], V7[1]);
                V0[2] = __vmrghh(V0[2], V1[2]);
                V2[2] = __vmrghh(V2[2], V3[2]);
                V4[2] = __vmrghh(V4[2], V5[2]);
                V6[2] = __vmrghh(V6[2], V7[2]);
                V0[3] = __vmrghh(V0[3], V1[3]);
                V2[3] = __vmrghh(V2[3], V3[3]);
                V4[3] = __vmrghh(V4[3], V5[3]);
                V6[3] = __vmrghh(V6[3], V7[3]);
                V0[4] = __vmrghh(V0[4], V1[4]);
                V2[4] = __vmrghh(V2[4], V3[4]);
                V4[4] = __vmrghh(V4[4], V5[4]);
                V6[4] = __vmrghh(V6[4], V7[4]);
                V0[5] = __vmrghh(V0[5], V1[5]);
                V2[5] = __vmrghh(V2[5], V3[5]);
                V4[5] = __vmrghh(V4[5], V5[5]);
                V6[5] = __vmrghh(V6[5], V7[5]);
                V0[6] = __vmrghh(V0[6], V1[6]);
                V2[6] = __vmrghh(V2[6], V3[6]);
                V4[6] = __vmrghh(V4[6], V5[6]);
                V6[6] = __vmrghh(V6[6], V7[6]);
                V0[7] = __vmrghh(V0[7], V1[7]);
                V2[7] = __vmrghh(V2[7], V3[7]);
                V4[7] = __vmrghh(V4[7], V5[7]);
                V6[7] = __vmrghh(V6[7], V7[7]);

                V0[0] = __vrlimi(V0[0], V2[0], 0x4, 3);
                V4[0] = __vrlimi(V4[0], V6[0], 0x4, 3);
                V0[1] = __vrlimi(V0[1], V2[1], 0x4, 3);
                V4[1] = __vrlimi(V4[1], V6[1], 0x4, 3);
                V0[2] = __vrlimi(V0[2], V2[2], 0x4, 3);
                V4[2] = __vrlimi(V4[2], V6[2], 0x4, 3);
                V0[3] = __vrlimi(V0[3], V2[3], 0x4, 3);
                V4[3] = __vrlimi(V4[3], V6[3], 0x4, 3);
                V0[4] = __vrlimi(V0[4], V2[4], 0x4, 3);
                V4[4] = __vrlimi(V4[4], V6[4], 0x4, 3);
                V0[5] = __vrlimi(V0[5], V2[5], 0x4, 3);
                V4[5] = __vrlimi(V4[5], V6[5], 0x4, 3);
                V0[6] = __vrlimi(V0[6], V2[6], 0x4, 3);
                V4[6] = __vrlimi(V4[6], V6[6], 0x4, 3);
                V0[7] = __vrlimi(V0[7], V2[7], 0x4, 3);
                V4[7] = __vrlimi(V4[7], V6[7], 0x4, 3);

                V3[0] = __vsldoi(V0[0], V0[0], 2 << 2);
                V7[0] = __vsldoi(V4[0], V4[0], 2 << 2);
                V3[1] = __vsldoi(V0[1], V0[1], 2 << 2);
                V7[1] = __vsldoi(V4[1], V4[1], 2 << 2);
                V3[2] = __vsldoi(V0[2], V0[2], 2 << 2);
                V7[2] = __vsldoi(V4[2], V4[2], 2 << 2);
                V3[3] = __vsldoi(V0[3], V0[3], 2 << 2);
                V7[3] = __vsldoi(V4[3], V4[3], 2 << 2);
                V3[4] = __vsldoi(V0[4], V0[4], 2 << 2);
                V7[4] = __vsldoi(V4[4], V4[4], 2 << 2);
                V3[5] = __vsldoi(V0[5], V0[5], 2 << 2);
                V7[5] = __vsldoi(V4[5], V4[5], 2 << 2);
                V3[6] = __vsldoi(V0[6], V0[6], 2 << 2);
                V7[6] = __vsldoi(V4[6], V4[6], 2 << 2);
                V3[7] = __vsldoi(V0[7], V0[7], 2 << 2);
                V7[7] = __vsldoi(V4[7], V4[7], 2 << 2);

                V3[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);
                V7[0] = __vupkd3d(V7[0], VPACK_FLOAT16_4);
                V3[1] = __vupkd3d(V3[1], VPACK_FLOAT16_4);
                V7[1] = __vupkd3d(V7[1], VPACK_FLOAT16_4);
                V3[2] = __vupkd3d(V3[2], VPACK_FLOAT16_4);
                V7[2] = __vupkd3d(V7[2], VPACK_FLOAT16_4);
                V3[3] = __vupkd3d(V3[3], VPACK_FLOAT16_4);
                V7[3] = __vupkd3d(V7[3], VPACK_FLOAT16_4);
                V3[4] = __vupkd3d(V3[4], VPACK_FLOAT16_4);
                V7[4] = __vupkd3d(V7[4], VPACK_FLOAT16_4);
                V3[5] = __vupkd3d(V3[5], VPACK_FLOAT16_4);
                V7[5] = __vupkd3d(V7[5], VPACK_FLOAT16_4);
                V3[6] = __vupkd3d(V3[6], VPACK_FLOAT16_4);
                V7[6] = __vupkd3d(V7[6], VPACK_FLOAT16_4);
                V3[7] = __vupkd3d(V3[7], VPACK_FLOAT16_4);
                V7[7] = __vupkd3d(V7[7], VPACK_FLOAT16_4);

                __stvx(V3[0], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V7[0], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V3[1], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V7[1], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V3[2], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V7[2], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V3[3], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V7[3], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V3[4], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V7[4], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V3[5], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V7[5], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V3[6], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V7[6], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V3[7], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
                __stvx(V7[7], (VOID*)OutputVector, 0);
                OutputVector += 4 * sizeof(FLOAT);
            }
        }

        while (OutputVector < OutputStreamVGroupEnd)
        {
            V0[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V1[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V2[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V3[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V4[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V5[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V6[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;
            V7[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride;

            V0[0] = __vmrghh(V0[0], V1[0]);
            V2[0] = __vmrghh(V2[0], V3[0]);
            V4[0] = __vmrghh(V4[0], V5[0]);
            V6[0] = __vmrghh(V6[0], V7[0]);

            V0[0] = __vrlimi(V0[0], V2[0], 0x4, 3);
            V4[0] = __vrlimi(V4[0], V6[0], 0x4, 3);

            V3[0] = __vsldoi(V0[0], V0[0], 2 << 2);
            V7[0] = __vsldoi(V4[0], V4[0], 2 << 2);

            V3[0] = __vupkd3d(V3[0], VPACK_FLOAT16_4);
            V7[0] = __vupkd3d(V7[0], VPACK_FLOAT16_4);

            __stvx(V3[0], (VOID*)OutputVector, 0);
            OutputVector += 4 * sizeof(FLOAT);
            __stvx(V7[0], (VOID*)OutputVector, 0);
            OutputVector += 4 * sizeof(FLOAT);
        }

        while (OutputVector < OutputStreamEnd)
        {
            V0[0] = __lvlx((VOID*)InputVector, 0);
            V0[0] = __vsplth(V0[0], 0);
            V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

            __stvewx(V0[0], (VOID*)OutputVector, 0);

            InputVector += InputStride; 
            OutputVector += sizeof(FLOAT);
        }
    }
    else
    {
        UINT_PTR InputStreamEnd = InputVector + HalfCount * InputStride;
        UINT_PTR InputStreamVGroupEnd = InputVector + (HalfCount & ~(UnrollCount - 1)) * InputStride;

        while (InputVector < InputStreamVGroupEnd)
        {
            V0[0] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride; 
            V0[1] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride; 
            V0[2] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride; 
            V0[3] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride; 
            V0[4] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride; 
            V0[5] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride; 
            V0[6] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride; 
            V0[7] = __lvlx((VOID*)InputVector, 0);
            InputVector += InputStride; 

            V0[0] = __vsplth(V0[0], 0);
            V0[1] = __vsplth(V0[1], 0);
            V0[2] = __vsplth(V0[2], 0);
            V0[3] = __vsplth(V0[3], 0);
            V0[4] = __vsplth(V0[4], 0);
            V0[5] = __vsplth(V0[5], 0);
            V0[6] = __vsplth(V0[6], 0);
            V0[7] = __vsplth(V0[7], 0);

            V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);
            V0[1] = __vupkd3d(V0[1], VPACK_FLOAT16_4);
            V0[2] = __vupkd3d(V0[2], VPACK_FLOAT16_4);
            V0[3] = __vupkd3d(V0[3], VPACK_FLOAT16_4);
            V0[4] = __vupkd3d(V0[4], VPACK_FLOAT16_4);
            V0[5] = __vupkd3d(V0[5], VPACK_FLOAT16_4);
            V0[6] = __vupkd3d(V0[6], VPACK_FLOAT16_4);
            V0[7] = __vupkd3d(V0[7], VPACK_FLOAT16_4);

            __stvewx(V0[0], (VOID*)OutputVector, 0);
            OutputVector += OutputStride;
            __stvewx(V0[1], (VOID*)OutputVector, 0);
            OutputVector += OutputStride;
            __stvewx(V0[2], (VOID*)OutputVector, 0);
            OutputVector += OutputStride;
            __stvewx(V0[3], (VOID*)OutputVector, 0);
            OutputVector += OutputStride;
            __stvewx(V0[4], (VOID*)OutputVector, 0);
            OutputVector += OutputStride;
            __stvewx(V0[5], (VOID*)OutputVector, 0);
            OutputVector += OutputStride;
            __stvewx(V0[6], (VOID*)OutputVector, 0);
            OutputVector += OutputStride;
            __stvewx(V0[7], (VOID*)OutputVector, 0);
            OutputVector += OutputStride;
        }

        while (InputVector < InputStreamEnd)
        {
            V0[0] = __lvlx((VOID*)InputVector, 0);
            V0[0] = __vsplth(V0[0], 0);
            V0[0] = __vupkd3d(V0[0], VPACK_FLOAT16_4);

            __stvewx(V0[0], (VOID*)OutputVector, 0);

            InputVector += InputStride; 
            OutputVector += OutputStride;
        }
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

void c_structure_renderer::render_transparent_cluster_mesh_part(const void *context, long mesh_part)
{
    render_cluster_mesh_part(mesh_part, _entry_point_static_per_pixel, 4);
}

void c_structure_renderer::initialize_for_new_map()
{
    reset();
}

void c_structure_renderer::dispose_from_old_map()
{
    g_render_structure_globals.current_a = 0;
    g_render_structure_globals.current_b = 0;
    g_render_structure_globals.depth = -1;
    g_cluster_sorted_order.m_count = 0;
    g_render_structure_globals.stack_b[5] = -1;
    g_render_structure_globals.cached_definition_index = -1;
    g_render_structure_globals.cached_instance_marker = -1;
}

// 0x449748
// @address 0x5bec98 ?initialize@c_structure_renderer@@SAXXZ
void c_structure_renderer::initialize()
{
}

// @address 0x5bec98 ?dispose@c_structure_renderer@@SAXXZ
void c_structure_renderer::dispose()
{
}

void c_structure_renderer::initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
    reset_bsp_cached_data(g_active_structure_bsp_mask);
}

void c_structure_renderer::dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
    reset_bsp_cached_data(0);
}

void c_structure_renderer::reset_bsp_cached_data(unsigned long structure_bsp_mask)
{
    g_render_structure_globals.lightmap_geometry_valid = true;
    memset(g_render_structure_globals.lightmap_bsp_data, 0, sizeof(g_render_structure_globals.lightmap_bsp_data));
    memset(g_render_structure_globals.render_geometry, 0, sizeof(g_render_structure_globals.render_geometry));
    scenario *current_scenario = render_structure_global_scenario_try_and_get();
    if (current_scenario)
    {
        for (long structure_bsp_index = 0; structure_bsp_index < 16; ++structure_bsp_index)
        {
            if (structure_bsp_mask & (1 << structure_bsp_index))
            {
                if (scenario_structure_bsp_has_valid_lightmap_geometry(global_scenario_index, structure_bsp_index))
                {
                    s_scenario_lightmap_bsp_data *lightmap_bsp_data = scenario_lightmap_find_bsp(
                        ((scenario *)current_scenario)->lightmap.index, structure_bsp_index);
                    g_render_structure_globals.lightmap_bsp_data[structure_bsp_index] = lightmap_bsp_data;
                    g_render_structure_globals.lightmap_geometry_valid &= lightmap_bsp_data != 0;
                }
                else
                {
                    g_render_structure_globals.lightmap_geometry_valid = false;
                }
                g_render_structure_globals.render_geometry[structure_bsp_index] = scenario_structure_bsp_get_render_geometry(
                    global_scenario_index, structure_bsp_index);
            }
        }
    }
    else
    {
        g_render_structure_globals.lightmap_geometry_valid = false;
    }
}

namespace {
static __forceinline s_visible_items &render_structure_get_global_items()
{
    return c_visible_items::m_items;
}
}

// 0x449890: compare visible instance lists in descending mesh order.
bool instance_list_sort_proc(short first_index, short second_index, const void *context)
{
    unsigned short first_list_index = first_index;
    unsigned short second_list_index = second_index;
    const s_visible_instance_list &first_list = render_structure_get_global_items().instance_lists[first_list_index];
    const s_visible_instance_list &second_list = render_structure_get_global_items().instance_lists[second_list_index];
    return first_list.mesh_index > second_list.mesh_index;
}

// @address 0x4498d0 ?cluster_sort_proc@@YA_NFFPBX@Z
bool cluster_sort_proc(short first_index, short second_index, const void *context)
{
    const s_render_cluster *clusters = static_cast<const s_render_cluster *>(context);
    const s_render_cluster *first_cluster = &clusters[static_cast<unsigned short>(first_index)];
    const s_render_cluster *second_cluster = &clusters[static_cast<unsigned short>(second_index)];
    return reinterpret_cast<intptr_t>(first_cluster) > reinterpret_cast<intptr_t>(second_cluster);
}

// 0x449910 (no dev name recorded; FUN_ name): draws the sorted cluster parts of one entry point
struct D3DDevice;
void structure_renderer_render_shadows_generate(e_entry_point entry_point)
{
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    long first = g_render_structure_globals.stack_a[g_render_structure_globals.depth - 1];
    long count = (unsigned short)g_cluster_sorted_order.m_count - first;
    if ((unsigned long)count >= 2)
    {
        templated_sort<short>::qsort(&g_cluster_sorted_order.m_order[first], count, cluster_sort_proc, g_cluster_sorted_order.m_context);
    }
    ::c_structure_renderer::render_cluster_parts(entry_point, 1);
}

// @address 0x81dc64 g_water_part_count
long g_water_part_count;

// @address 0x1660f8 ?update_water_structure_part_list@c_structure_renderer@@SAXPAJ0@Z
void c_structure_renderer::update_water_structure_part_list(long *, long *)
{
    long count = (unsigned short)g_cluster_sorted_order.m_count - (g_render_structure_globals.stack_a[g_render_structure_globals.depth - 1]);
    g_water_part_count = 0;
    if ((unsigned long)count >= 2)
    {
        templated_sort<short>::qsort(&g_cluster_sorted_order.m_order[(g_render_structure_globals.stack_a[g_render_structure_globals.depth - 1])],
            count, cluster_sort_proc, g_cluster_sorted_order.m_context);
    }
    for (long index = g_render_structure_globals.stack_a[g_render_structure_globals.depth - 1];
        index < (unsigned short)g_cluster_sorted_order.m_count; index++)
    {
        const s_render_cluster_part *part = &g_render_structure_globals.cluster_parts[(unsigned short)g_cluster_sorted_order.m_order[index]];
        const s_render_geometry *geometry = (const s_render_geometry *)g_render_structure_globals.render_geometry[part->cluster_reference.bsp_index];
        const s_mesh *section = (const s_mesh *)tag_block_get_element_with_size(
            &geometry->meshes, part->mesh_index, sizeof(s_mesh));
        const s_render_part *element = (const s_render_part *)tag_block_get_element_with_size(
            &section->parts, part->part_index, sizeof(s_render_part));
        if ((element->flags[0] & 8) != 0)
        {
#if defined(_XBOX) && !defined(__clang__)
            g_water_cluster_part_indices[g_water_part_count] = index;
#elif defined(HALO3_64BIT)
            g_water_render_structure_part_indices[g_water_part_count] = (unsigned short)g_cluster_sorted_order.m_order[index];
#else
            g_water_render_structure_part_indices[g_water_part_count] = index;
#endif
            _ReadWriteBarrier();
            g_water_part_count = g_water_part_count + 1;
        }
    }
}

// 0x175a78 (no dev name recorded; FUN_ name): lighting for one structure geometry instance



// 0x4499b0 (no dev name recorded; FUN_ name): draws the sorted cluster parts with the given z-buffer mode
void structure_renderer_render_static_lighting(c_rasterizer::e_z_buffer_mode z_buffer_mode)
{
    c_rasterizer::set_z_buffer_mode(z_buffer_mode);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    long first = g_render_structure_globals.stack_a[g_render_structure_globals.depth - 1];
    long count = (unsigned short)g_cluster_sorted_order.m_count - first;
    if ((unsigned long)count >= 2)
    {
        templated_sort<short>::qsort(&g_cluster_sorted_order.m_order[first], count, cluster_sort_proc, g_cluster_sorted_order.m_context);
    }
    ::c_structure_renderer::render_cluster_parts(_entry_point_shadow_generate, 2);
}

static __forceinline s_scenario_lightmap_bsp_data *render_structure_get_lightmap_bsp_data(long structure_bsp_index)
{
  return g_render_structure_globals.lightmap_bsp_data[structure_bsp_index];
}

// @address 0x175a78 structure_renderer_setup_instance_lighting
extern "C" long structure_renderer_setup_instance_lighting(long mode, long bsp_index, const s_scenario_lightmap_bsp_data *structure_lighting,
                  const s_render_geometry *object_lighting, const s_mesh *render_object,
                  const s_render_part *material_reference, const geometry_material *materials,
                  const s_render_instance_part *instance, const structure_instanced_geometry_instance *instance_render_lighting)
{
    long result = mode;
    bool use_default_lighting = false;
    if (mode == 3)
    {
        if (render_structure_get_lightmap_bsp_data(bsp_index) != NULL && structure_lighting != NULL)
        {
            const s_structure_lighting_cluster *cluster_ref =
                &((const s_structure_lighting_cluster *)structure_lighting->instanced_geometry_lighting.address)[instance->instance_index];
            short primary = cluster_ref->primary;
            if (primary != -1)
            {
                long sample_index = ((const s_lighting_cluster *)structure_lighting->static_per_vertex_lighting.address)[primary].sample_index;
                if (sample_index != -1)
                {
                    const c_rasterizer_vertex_buffer *samples = render_geometry_get_vertex_buffer(&structure_lighting->render_geometry, sample_index);
                    if (samples == NULL || samples->get_d3d_buffer() == NULL)
                    {
                        result = 5;
                        use_default_lighting = true;
                    }
                    else
                    {
                        c_rasterizer::set_vertex_stream(samples);
                        result = 4;
                    }
                }
            }
            else
            {
                short secondary = cluster_ref->secondary;
                if (secondary != -1 && secondary >= 0 && secondary < structure_lighting->instanced_geometry_light_probes.count)
                {
                    s_lighting_data lighting;
                    xm_convert_half_to_float_stream((FLOAT *)&lighting, 4,
        (const HALF *)((const unsigned char *)structure_lighting->instanced_geometry_light_probes.address + secondary * 72), 2, 36);
                    c_lighting_interface::calculate_and_set_ravi_constants(lighting.red, lighting.green, lighting.blue, 1.0f);
                    c_lighting_interface::set_object_dominant_light_direction_and_intensity(
                        &lighting.dominant_direction,
                        &lighting.dominant_intensity);
                    result = 5;
                    if (instance_render_lighting->lightmap_policy == 2 && (unsigned short)render_object->vertex_buffer_indices[3] != 0xffff)
                        result = 8;
                }
                else
                {
                    short object_cluster = instance_render_lighting->lightmap_texcoords_index;
                    if (object_cluster >= 0 && object_cluster < object_lighting->per_instance_lightmap_texcoords.count)
                    {
                        long sample_index =
                            ((const s_object_lighting_cluster *)object_lighting->per_instance_lightmap_texcoords.address)[object_cluster].sample_index;
                        if (sample_index != -1)
                        {
                            c_rasterizer::set_vertex_stream(render_geometry_get_vertex_buffer(object_lighting, sample_index));
                        }
                        else
                        {
                            result = 5;
                            use_default_lighting = true;
                        }
                    }
                    else
                    {
                        result = 5;
                        use_default_lighting = true;
                    }
                }
            }
        }
        else
        {
            use_default_lighting = true;
            result = 5;
        }
        if (materials[material_reference->render_method_index].render_method.group_tag == 0x726d666c && result == 3)
            use_default_lighting = true;
        if (use_default_lighting)
            c_lighting_interface::setup_default_lighting();
    }
    return result;
}


// @address 0x17a778 structure_renderer_render_lighting
extern "C" void structure_renderer_render_lighting()
{
    long first = g_render_structure_globals.stack_a[g_render_structure_globals.depth - 1];
    long count = (unsigned short)g_cluster_sorted_order.m_count - first;
    if ((unsigned long)count >= 2)
    {
        templated_sort<short>::qsort(&g_cluster_sorted_order.m_order[first], count, cluster_sort_proc, g_cluster_sorted_order.m_context);
    }
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    g_render_method_submit_cached_state_enabled = g_render_method_submit_cache_bypass == 0;
    ::c_structure_renderer::render_cluster_parts(_entry_point_static_per_pixel, 3);
    render_method_submit_reset_cached_state();
}


// @address 0x1752e8 ?render_albedo@c_structure_renderer@@SAXXZ
void c_structure_renderer::render_albedo()
{
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
    D3DDevice_SetRenderState_ColorWriteEnable(c_rasterizer::get_device(), 15);
    D3DDevice_SetRenderState_ColorWriteEnable1(c_rasterizer::get_device(), 15);
    long first = g_render_structure_globals.stack_a[g_render_structure_globals.depth - 1];
    long count = (unsigned short)g_cluster_sorted_order.m_count - first;
    if ((unsigned long)count >= 2)
    {
        templated_sort<short>::qsort(&g_cluster_sorted_order.m_order[first], count, cluster_sort_proc, g_cluster_sorted_order.m_context);
    }
    g_render_method_submit_cached_state_enabled = g_render_method_submit_cache_bypass == 0;
    ::c_structure_renderer::render_cluster_parts(_entry_point_albedo, 3);
    render_method_submit_reset_cached_state();
}

extern "C" {
// @address 0x8c484e g_render_static_lighting_active
}

namespace {
struct s_render_structure_instance_part_context
{
    long bsp_index = NONE;
    long instance_index;
    long definition_index;
    const s_render_geometry *geometry;
    const s_scenario_lightmap_bsp_data *lightmap;
    const geometry_material *materials;
    const structure_instanced_geometry_instance *instances;
    const structure_instanced_geometry_definition *definitions;
    const structure_instanced_geometry_instance *instance;
    const structure_instanced_geometry_definition *definition;
    const s_mesh *mesh;
};

void render_structure_draw_instance_part(s_render_structure_instance_part_context &context, long part_index, long mode, long count);
}

// @address 0x182688 structure_renderer_render_transparent_instance_part
extern "C" void structure_renderer_render_transparent_instance_part(long unused, long part_index)
{
  long mode = 3;
  if (render_structure_get_lightmap_bsp_data(g_render_structure_globals.parts[part_index].structure_bsp_index) == 0) {
    mode = 5;
    c_lighting_interface::setup_default_lighting();
  }
  structure_renderer_render_instance_part(part_index, mode, 4);
  g_render_structure_globals.stack_b[5] = -1;
  g_render_structure_globals.cached_definition_index = -1;
  g_render_structure_globals.cached_instance_marker = -1;
  if (g_render_static_lighting_active == 0)
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
}


// @address 0x1753b0 ?render_cluster_parts@c_structure_renderer@@SAXW4e_entry_point@@J@Z
void c_structure_renderer::render_cluster_parts(enum e_entry_point p0, long p1)
{
    long instance_part_index = g_render_structure_globals.stack_a[g_render_structure_globals.depth + 5];
    if (instance_part_index < g_render_structure_globals.current_b)
    {
        s_render_structure_instance_part_context context;
        do
        {
            render_structure_draw_instance_part(context, instance_part_index, p0, p1);
            instance_part_index = instance_part_index + 1;
        } while (instance_part_index < g_render_structure_globals.current_b);
    }
    g_render_structure_globals.stack_b[5] = -1;
    g_render_structure_globals.cached_definition_index = -1;
    g_render_structure_globals.cached_instance_marker = -1;
    rasterizer_set_shadow_stencil_ref(255, 0, 0, 0);
    long cluster_part_index = g_render_structure_globals.cluster_markers[g_render_structure_globals.depth];
    if (cluster_part_index < (unsigned short)g_cluster_sorted_order.m_count)
    {
        do
        {
            render_cluster_mesh_part(cluster_part_index, p0, p1);
            cluster_part_index = cluster_part_index + 1;
        } while (cluster_part_index < (unsigned short)g_cluster_sorted_order.m_count);
    }
    if ((unsigned char)g_render_static_lighting_active == 0)
    {
        c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
    }
}


namespace {
static __forceinline structure_bsp *get_structure_bsp_for_rendering(long structure_bsp_index)
{
    const scenario *current_scenario = (const scenario *)global_scenario;
    const scenario_structure_bsp_reference *reference = (const scenario_structure_bsp_reference *)tag_block_get_element_with_size(
        &current_scenario->structure_bsps, structure_bsp_index, sizeof(scenario_structure_bsp_reference));
    return (structure_bsp *)global_tag_instances[datum_index_to_absolute_index(reference->structure_bsp.index)].definition;
}
static __forceinline real_point3d *render_structure_matrix4x3_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    float x = point->x;
    float y = point->y;
    float z = point->z;
    x *= matrix->scale;
    y *= matrix->scale;
    z *= matrix->scale;
    result->x = x * matrix->forward.i + y * matrix->left.i + z * matrix->up.i + matrix->position.x;
    result->y = x * matrix->forward.j + y * matrix->left.j + z * matrix->up.j + matrix->position.y;
    result->z = x * matrix->forward.k + y * matrix->left.k + z * matrix->up.k + matrix->position.z;
    return result;
}

static __forceinline real_plane3d *render_structure_matrix4x3_transform_plane(const real_matrix4x3 *matrix, const real_plane3d *plane, real_plane3d *result)
{
    float i = plane->normal.i;
    float j = plane->normal.j;
    float k = plane->normal.k;
    result->normal.i = i * matrix->forward.i + j * matrix->left.i + k * matrix->up.i;
    result->normal.j = i * matrix->forward.j + j * matrix->left.j + k * matrix->up.j;
    result->normal.k = i * matrix->forward.k + j * matrix->left.k + k * matrix->up.k;
    result->d = plane->d * matrix->scale + dot_product3d(&result->normal, &matrix->position);
    return result;
}
static __forceinline bool render_part_has_render_method(const s_render_part *part, const geometry_material *materials)
{
    return part->part_type != 5 && part->render_method_index != -1 && materials[part->render_method_index].render_method.index != -1;
}
}

struct s_shader_reference;
struct s_part;
extern "C" {
// @address 0x119a414 g_breakable_surface_frame_index
// @address 0x8c29c7 g_breakable_surface_stencil_ref
long g_breakable_surface_frame_index;
unsigned char g_breakable_surface_stencil_ref;
}

namespace {
void render_structure_draw_instance_part(s_render_structure_instance_part_context &context, long part_index, long mode, long count)
{
    s_render_instance_part *instance_part = &g_render_structure_globals.parts[part_index];
    if (instance_part->lod != 0 && (instance_part->pass & count) != 0)
    {
        long bsp_index = instance_part->structure_bsp_index;
        long instance_index = instance_part->instance_index;
        if (bsp_index != context.bsp_index)
        {
            const structure_bsp *bsp = get_structure_bsp_for_rendering(bsp_index);
            context.bsp_index = bsp_index;
            context.geometry = g_render_structure_globals.render_geometry[bsp_index];
            context.lightmap = g_render_structure_globals.lightmap_bsp_data[bsp_index];
            context.materials = (const geometry_material *)bsp->materials.address;
            context.instances = (const structure_instanced_geometry_instance *)bsp->instanced_geometry_instances.address;
            context.definitions = g_collision_bsp_cache.users[get_current_thread_index()].entries[bsp_index].data;
            context.instance_index = NONE;
            context.definition = nullptr;
        }
        if (instance_index != context.instance_index)
        {
            context.instance_index = instance_index;
            context.instance = &context.instances[instance_index];
            long definition_index = context.instance->definition_index;
            if (!context.definition || definition_index != context.definition_index)
            {
                context.definition_index = definition_index;
                context.definition = &context.definitions[definition_index];
                context.mesh = &((const s_mesh *)context.geometry->meshes.address)[(unsigned short)context.definition->mesh_index];
            }
        }
        const s_render_geometry *geometry = context.geometry;
        const geometry_material *materials = context.materials;
        const structure_instanced_geometry_instance *instance = context.instance;
        const structure_instanced_geometry_definition *definition = context.definition;
        long definition_index = context.definition_index;
        if (bsp_index != g_render_structure_globals.stack_b[5])
        {
            g_render_structure_globals.stack_b[5] = bsp_index;
            g_render_structure_globals.cached_definition_index = -1;
            g_render_structure_globals.cached_instance_index = -1;
        }
        if (definition_index != g_render_structure_globals.cached_definition_index)
        {
            const s_compression_info *compression =
                &((const s_compression_info *)geometry->compression_info.address)[(unsigned short)definition->compression_index];
            render_mesh_submit_compression(compression);
            g_render_structure_globals.cached_definition_index = definition_index;
        }
        if (instance_index != (long)g_render_structure_globals.cached_instance_index)
        {
            render_mesh_submit_rigid_matrix(&instance->transform);
            g_render_structure_globals.cached_instance_index = instance_index;
        }
        s_cluster_reference option;
        option.bsp_index = (signed char)bsp_index;
        option.cluster_index = 0;
        union { unsigned short value; s_cluster_reference option; } invalid;
        invalid.value = 0xffff;
        long cubemap_index = instance->cubemap_index;
        render_method_submit_set_structure_data(true, bsp_index, invalid.option, instance_part->instance_index,
            option, cubemap_index, option, cubemap_index, 1.0f, 0, instance_part->instance_index, 0);
        const s_mesh *mesh = context.mesh;
        const s_render_part *part = &((const s_render_part *)mesh->parts.address)[instance_part->part_index];
        mode = structure_renderer_setup_instance_lighting(mode, bsp_index, context.lightmap,
            geometry, mesh,
            part, materials,
            instance_part, instance);
        unsigned long *selection = 0;
        rasterizer_set_shadow_stencil_ref(instance_part->lod, 0, 0, 0);
        unsigned char set_index = materials[part->render_method_index].breakable_surface_index;
        if (set_index != 255 && (instance->flags & 2) == 0)
        {
            s_breakable_surface_set *sets = breakable_surface_get_first_set(instance_part->structure_bsp_index, instance_part->instance_index);
            s_breakable_surface_set *set = sets ? &sets[set_index] : 0;
            selection = set ? set->surface_flags.get_writeable_bits_direct() : 0;
            if ((long)c_render_globals::get_frame_index() != g_breakable_surface_frame_index)
            {
                g_breakable_surface_frame_index = c_render_globals::get_frame_index();
                c_rasterizer::clear(32, 0, 0.0f, 0);
            }
            if (++g_breakable_surface_stencil_ref == 0)
                g_breakable_surface_stencil_ref = 1;
            rasterizer_set_stencil_mode_with_ref(c_rasterizer::_stencil_mode_decal_write, g_breakable_surface_stencil_ref);
        }
        __int64 mesh_part_key = ((__int64)(short)instance_part->part_index << 16) + (short)definition->mesh_index;
        unsigned __int64 key = (mesh_part_key << 32) + (__int64)geometry;
        render_method_submit_draw_mesh_part(key, geometry, mesh, (const s_part *)part, mode, selection, materials);
        if (materials[part->render_method_index].breakable_surface_index != 255 && (instance->flags & 2) == 0)
        {
            rasterizer_set_stencil_mode_with_ref(c_rasterizer::_stencil_mode_decal_read, g_breakable_surface_stencil_ref);
            breakable_surface_render_cracks(instance_part->structure_bsp_index, instance_part->instance_index,
                materials[part->render_method_index].breakable_surface_index);
        }
    }
    c_render_method_data::m_valid = false;
    render_method_submit_valid();
}
}

// @address 0x175490 structure_renderer_render_instance_part
extern "C" void structure_renderer_render_instance_part(long part_index, long mode, long count)
{
    s_render_structure_instance_part_context context;
    render_structure_draw_instance_part(context, part_index, mode, count);
}

// @address 0x162958 ?submit_visibility@c_structure_renderer@@SAX_N@Z
void c_structure_renderer::submit_visibility(bool render_transparents)
{
    long iterator;
    // match: 0x162958: value range assumption guides branch lowering
    __assume(render_transparents <= 1);
    for (s_visible_clusters *cluster =
             render_structure_get_global_items().clusters.list_iterator_new_reset(&iterator, c_visible_items::get_cluster_starting_index());
        cluster; cluster = render_structure_get_global_items().clusters.list_iterator_next_reset(&iterator))
    {
        long structure_bsp_index = cluster->cluster_reference.bsp_index;
        // match: 0x162958: value range assumption guides branch lowering
        __assume(structure_bsp_index >= 0);
        if (get_structure_bsp_for_rendering(structure_bsp_index)->materials.count > 0)
        {
            const s_render_geometry *render_geometry = g_render_structure_globals.render_geometry[structure_bsp_index];
            const geometry_material *materials =
                (const geometry_material *)get_structure_bsp_for_rendering(structure_bsp_index)->materials.address;
            if (cluster->flags & (1 << 3))
            {
                const s_mesh *mesh = (const s_mesh *)tag_block_get_element_with_size(
                    &render_geometry->meshes, cluster->mesh_index, sizeof(s_mesh));
                ASSERT(mesh->parts.count > 0 && mesh->subparts.count > 0);
                for (long part_index = 0; part_index < mesh->parts.count; part_index++)
                {
                    const s_render_part *part = (const s_render_part *)tag_block_get_element_with_size(&mesh->parts, part_index, sizeof(s_render_part));
                    if (cluster->visible_parts && (cluster->visible_parts[part_index >> 5] & (1 << (part_index & 31))) == 0)
                        continue;
                    ASSERT(part->render_method_index >= 0 &&
                        part->render_method_index < get_structure_bsp_for_rendering(structure_bsp_index)->materials.count);
                    if (render_part_has_render_method(part, materials) && (unsigned long)g_render_structure_globals.current_a <= 511)
                    {
                        s_render_cluster_part *cluster_part = &g_render_structure_globals.cluster_parts[g_render_structure_globals.current_a];
                        bool is_transparent = c_render_method::get_is_transparent(materials[part->render_method_index].render_method.index);
                        long pass = is_transparent ? 4 : (cluster->flags & 3);
                        ASSERT(iterator >= 0 && iterator < render_structure_get_global_items().clusters.get_count());
                        cluster_part->mesh_index = cluster->mesh_index;
                        cluster_part->part_index = (unsigned short)part_index;
                        cluster_part->pass = pass;
                        cluster_part->cluster_reference = cluster->cluster_reference;
                        if (render_transparents && is_transparent)
                        {
                            real_plane3d plane;
                            real_point3d point;
                            float radius = 0.0f;
                            short sorting_index = part->transparent_sorting_index;
                            plane.normal = *global_forward3d;
                            plane.d = 0.0f;
                            point = *global_origin3d;
                            if (sorting_index != -1)
                            {
                                if ((unsigned short)sorting_index & 0x8000)
                                    sorting_index = (short)(sorting_index - 0x8000);
                                ASSERT(part->transparent_sorting_index & 0x8000);
                                const s_transparent_sorting_node *node = (const s_transparent_sorting_node *)tag_block_get_element_with_size(
                                    &render_geometry->part_sorting_position, sorting_index, sizeof(s_transparent_sorting_node));
                                point = node->position;
                                radius = node->radius;
                                plane = node->plane;
                            }
                            e_transparent_sort_layer sort_layer = (e_transparent_sort_layer)c_render_method::get(
                                materials[part->render_method_index].render_method.index)
                                                                      ->get_sort_layer();
                            render_objects_add_transparent_element(&point, &plane, 0.0f, sort_layer,
                                c_structure_renderer::render_transparent_cluster_mesh_part, 0, g_render_structure_globals.current_a,
                                radius);
                        }
                        g_cluster_sorted_order.set_order_entry(g_render_structure_globals.current_a, (unsigned short)g_render_structure_globals.current_a);
                        g_render_structure_globals.current_a++;
                    }
                }
            }
        }
    }
    long instance_list_count = render_structure_get_global_items().instance_lists.get_count();
    long instance_list_start = c_visible_items::get_instance_list_starting_index();
    long sorted_list_count = instance_list_count - instance_list_start;
    c_sorter<s_render_instance_part, 1024> sorter;
    sorter.m_context = 0;
    sorter.fill_order((unsigned short)sorted_list_count, instance_list_start);
    sorter.sort(instance_list_sort_proc, 0);
    for (long list_iterator = 0; list_iterator < sorter.get_count(); list_iterator++)
    {
        long instance_list_index = sorter.get_order_entry(list_iterator);
        const s_visible_instance_list *list = &render_structure_get_global_items().instance_lists[instance_list_index];
        long structure_bsp_index = list->bsp_index;
        if (get_structure_bsp_for_rendering(structure_bsp_index)->materials.count > 0)
        {
            const s_render_geometry *render_geometry = g_render_structure_globals.render_geometry[structure_bsp_index];
            const geometry_material *materials =
                (const geometry_material *)get_structure_bsp_for_rendering(structure_bsp_index)->materials.address;
            long first_instance = list->first_instance;
            long last_instance = first_instance + list->instance_count;
            const s_mesh *mesh = (const s_mesh *)tag_block_get_element_with_size(
                &render_geometry->meshes, list->mesh_index, sizeof(s_mesh));
            for (long part_index = 0; part_index < mesh->parts.count; part_index++)
            {
                const s_render_part *part = (const s_render_part *)tag_block_get_element_with_size(&mesh->parts, part_index, sizeof(s_render_part));
                long render_method_index = materials[part->render_method_index].render_method.index;
                ASSERT(part->render_method_index >= 0 &&
                    part->render_method_index < get_structure_bsp_for_rendering(structure_bsp_index)->materials.count);
                if (render_part_has_render_method(part, materials))
                {
                    for (long instance_iterator = first_instance; instance_iterator < last_instance; instance_iterator++)
                    {
                        s_visible_instances *instance = &render_structure_get_global_items().instances[instance_iterator];
                        if (instance->visible_parts && (instance->visible_parts[part_index >> 5] & (1 << (part_index & 31))) == 0)
                            continue;
                        if (((instance->flags & (1 << 3)) || (instance->flags & (1 << 2))) && (unsigned long)g_render_structure_globals.current_b <= 1535)
                        {
                            s_render_instance_part *instance_part = &g_render_structure_globals.parts[g_render_structure_globals.current_b];
                            bool is_transparent = c_render_method::get_is_transparent(render_method_index);
                            long pass = is_transparent ? 4 : (instance->flags & 3);
                            instance_part->part_index = (unsigned short)part_index;
                            instance_part->pass = pass;
                            instance_part->structure_bsp_index = instance->bsp_index;
                            instance_part->mesh_index = list->mesh_index;
                            instance_part->lod = instance->lod;
                            instance_part->instance_index = instance->instance_index;
                            if (render_transparents && is_transparent)
                            {
                                const real_matrix4x3 *matrix = (const real_matrix4x3 *)tag_block_get_element_with_size(
                                    &get_structure_bsp_for_rendering(structure_bsp_index)->instanced_geometry_instances,
                                    instance_part->instance_index, sizeof(structure_instanced_geometry_instance));
                                real_plane3d plane;
                                real_point3d point;
                                float radius = 0.0f;
                                plane.normal = *global_forward3d;
                                plane.d = 0.0f;
                                if (part->transparent_sorting_index != -1)
                                {
                                    const s_transparent_sorting_node *node =
                                        (const s_transparent_sorting_node *)tag_block_get_element_with_size(
                                            &render_geometry->part_sorting_position, part->transparent_sorting_index,
                                            sizeof(s_transparent_sorting_node));
                                    point = node->position;
                                    radius = node->radius;
                                    plane = node->plane;
                                    render_structure_matrix4x3_transform_point(matrix, &point, &point);
                                    render_structure_matrix4x3_transform_plane(matrix, &plane, &plane);
                                }
                                else
                                {
                                    point = matrix->position;
                                }
                                e_transparent_sort_layer sort_layer = (e_transparent_sort_layer)c_render_method::get(
                                    materials[part->render_method_index].render_method.index)
                                                                          ->get_sort_layer();
                                render_objects_add_transparent_element(&point, &plane, 0.0f, sort_layer,
                                    (void (*)(const void *, long))structure_renderer_render_transparent_instance_part, 0,
                                    g_render_structure_globals.current_b, radius);
                            }
                            g_render_structure_globals.current_b++;
                        }
                    }
                }
            }
        }
    }
}


// @rdata 0x82025088..0x82025460
static const s_render_method_extern_parameter_table render_structure_shader_parameters = {
    {
        {0, -1, 6, 0, "_render_method_extern_none"},
        {3, 10, 0, 0, "_render_method_extern_texture_global_target_texaccum"},
        {3, 11, 0, 0, "_render_method_extern_texture_global_target_normal"},
        {3, 11, 0, 0, "_render_method_extern_texture_global_target_z"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_shadow_buffer1"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_shadow_buffer2"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_shadow_buffer3"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_shadow_buffer4"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_texture_camera"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_reflection"},
        {1, -1, 6, 0, "_render_method_extern_texture_global_target_refraction"},
        {2, 12, 0, 0, "_render_method_extern_texture_lightprobe_texture"},
        {2, 13, 0, 0, "_render_method_extern_texture_dominant_light_intensity_map"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused1"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused2"},
        {0, -1, 6, 0, "_render_method_extern_object_change_color_primary"},
        {0, -1, 6, 0, "_render_method_extern_object_change_color_secondary"},
        {0, -1, 6, 0, "_render_method_extern_object_change_color_tertiary"},
        {0, -1, 6, 0, "_render_method_extern_object_change_color_quaternary"},
        {0, -1, 6, 0, "_render_method_extern_object_emblem_color_background"},
        {0, -1, 6, 0, "_render_method_extern_object_emblem_color_primary"},
        {0, -1, 6, 0, "_render_method_extern_object_emblem_color_secondary"},
        {0, -1, 6, 0, "_render_method_extern_texture_dynamic_environment_map_0"},
        {0, -1, 6, 0, "_render_method_extern_texture_dynamic_environment_map_1"},
        {1, -1, 6, 65536, "_render_method_extern_texture_cook_torrance_cc0236"},
        {1, -1, 6, 65536, "_render_method_extern_texture_cook_torrance_dd0236"},
        {1, -1, 6, 65536, "_render_method_extern_texture_cook_torrance_c78d78"},
        {0, -1, 6, 0, "_render_method_extern_light_dir_0"},
        {0, -1, 6, 0, "_render_method_extern_light_color_0"},
        {0, -1, 6, 0, "_render_method_extern_light_dir_1"},
        {0, -1, 6, 0, "_render_method_extern_light_color_1"},
        {0, -1, 6, 0, "_render_method_extern_light_dir_2"},
        {0, -1, 6, 0, "_render_method_extern_light_color_2"},
        {0, -1, 6, 0, "_render_method_extern_light_dir_3"},
        {0, -1, 6, 0, "_render_method_extern_light_color_3"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused_3"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused_4"},
        {0, -1, 6, 0, "_render_method_extern_texture_unused_5"},
        {0, -1, 6, 0, "_render_method_extern_texture_dynamic_light_gel_0"},
        {0, -1, 6, 0, "_render_method_extern_flat_envmap_matrix_x"},
        {0, -1, 6, 0, "_render_method_extern_flat_envmap_matrix_y"},
        {0, -1, 6, 0, "_render_method_extern_flat_envmap_matrix_z"},
        {0, -1, 6, 0, "_render_method_extern_debug_tint"},
        {1, -1, 6, 0, "_render_method_extern_screen_constants"},
        {0, -1, 6, 0, "_render_method_extern_active_camo_distortion_texture"},
        {3, 10, 0, 0, "_render_method_extern_scene_ldr_texture"},
        {3, 11, 0, 0, "_render_method_extern_scene_hdr_texture"},
        {1, -1, 6, 0, "_render_method_extern_water_memory_export_address"},
        {1, -1, 6, 0, "_render_method_extern_tree_animation_timer"}
    },
    0x3c8efa35
};
// @rdata-end

extern "C" {
// @address 0x7bbaa4 ?m_lightmap_cluster_reference@c_render_method_data@@2Us_cluster_reference@@A
// @address 0x7bbaa8 ?m_cubemap_dynamic_0_cluster_reference@c_render_method_data@@2Us_cluster_reference@@A
// @address 0x7bbaac ?m_cubemap_dynamic_1_cluster_reference@c_render_method_data@@2Us_cluster_reference@@A
// @address 0x8c4708 ?m_cubemap_blend_factor@c_render_method_data@@2MA
// @address 0x8c470c ?m_context_interface@c_render_method_data@@2P6AMJJJ@ZA
// @address 0x737704 ?m_lightmap_instance_index@c_render_method_data@@2JA
// @address 0x738d1c ?m_lightmap_bsp_index@c_render_method_data@@2JA
// @address 0x73a150 ?m_cubemap_dynamic_0_index@c_render_method_data@@2JA
// @address 0x73a154 ?m_cubemap_dynamic_1_index@c_render_method_data@@2JA
// @address 0x73a158 ?m_context@c_render_method_data@@2JA
// @address 0x73a15c g_render_method_submit_cached_definition_index
}

static __forceinline void render_structure_submit_data(bool valid, long structure_bsp_index,
    s_cluster_reference lightmap_cluster_reference, long lightmap_instance_index, s_cluster_reference cubemap_dynamic_0_cluster_reference,
    long cubemap_dynamic_0_index, s_cluster_reference cubemap_dynamic_1_cluster_reference, long cubemap_dynamic_1_index,
    float cubemap_blend_factor, long context)
{
    c_render_method_data::m_valid = valid;
    BLAM_ASSUME(lightmap_cluster_reference.bsp_index == -1 || lightmap_cluster_reference.cluster_index == 255
        || structure_bsp_index == lightmap_cluster_reference.bsp_index);
    c_render_method_data::m_lightmap_bsp_index = structure_bsp_index;
    c_render_method_data::m_lightmap_cluster_reference = lightmap_cluster_reference;
    c_render_method_data::m_lightmap_instance_index = lightmap_instance_index;
    c_render_method_data::m_cubemap_dynamic_0_cluster_reference = cubemap_dynamic_0_cluster_reference;
    c_render_method_data::m_cubemap_dynamic_0_index = cubemap_dynamic_0_index;
    c_render_method_data::m_cubemap_dynamic_1_cluster_reference = cubemap_dynamic_1_cluster_reference;
    c_render_method_data::m_cubemap_dynamic_1_index = cubemap_dynamic_1_index;
    c_render_method_data::m_cubemap_blend_factor = cubemap_blend_factor;
    c_render_method_data::m_context_interface = 0;
    c_render_method_data::m_context = context;
    if (structure_bsp_index != -1 && ((unsigned char)lightmap_cluster_reference.bsp_index != 255 || lightmap_instance_index != -1)
        && structure_bsp_index != g_render_method_submit_cached_definition_index)
    {
        for (long i = 0; i < 49; i++)
        {
            if (render_structure_shader_parameters.parameters[i].type == 2)
                rasterizer_set_global_shader_parameter(i);
        }
        g_render_method_submit_cached_definition_index = structure_bsp_index;
    }
}

// @address 0x175c68
void c_structure_renderer::render_cluster_mesh_part(long mesh_part, e_entry_point entry_point, long count)
{
    s_render_cluster_part *cluster_part = &g_render_structure_globals.cluster_parts[g_cluster_sorted_order.get_order_entry(mesh_part)];
    if ((cluster_part->pass & count) != 0)
    {
        long bsp_index = cluster_part->cluster_reference.bsp_index;
        structure_bsp *bsp = get_structure_bsp_for_rendering(bsp_index);
        const s_render_geometry *geometry;
        geometry_material *materials = (geometry_material *)bsp->materials.address;
        long cubemap_index = c_dynamic_cubemap_sample::get_cluster_cubemap_index(cluster_part->cluster_reference);
        geometry = g_render_structure_globals.render_geometry[bsp_index];
        const s_mesh *mesh = (const s_mesh *)tag_block_get_element_with_size(&geometry->meshes, cluster_part->mesh_index, sizeof(s_mesh));
        const s_render_part *part = (const s_render_part *)tag_block_get_element_with_size(&mesh->parts, cluster_part->part_index, sizeof(s_render_part));
        if (!TEST_BIT(part->flags[0], 3))
        {
            unsigned long *selection = 0;
            const geometry_material *material = &materials[part->render_method_index];
            if (material->breakable_surface_index != 255)
            {
                s_breakable_surface_set *sets = breakable_surface_get_first_set(bsp_index, -1);
                s_breakable_surface_set *set = sets ? &sets[material->breakable_surface_index] : 0;
                selection = set ? set->surface_flags.get_writeable_bits_direct() : 0;
            }
            if (entry_point == 3 && (render_structure_get_lightmap_bsp_data(bsp_index) == 0 || (unsigned long)material->render_method.group_tag == 'rmfl'))
            {
                entry_point = _entry_point_static_sh;
                c_lighting_interface::setup_default_lighting();
            }
            long identifier = (bsp_index << 4) + (cluster_part->cluster_reference.cluster_index << 8) + cluster_part->part_index * 255;
            render_structure_submit_data(true, bsp_index, cluster_part->cluster_reference, -1,
                cluster_part->cluster_reference, cubemap_index, cluster_part->cluster_reference, cubemap_index, 1.0f, identifier);
            __int64 mesh_part_key = ((__int64)(short)cluster_part->part_index << 16) + (short)cluster_part->mesh_index;
            unsigned __int64 key = (mesh_part_key << 32) + (__int64)geometry;
            render_method_submit_draw_mesh_part(
                key, geometry, mesh, (const s_part *)part, entry_point, selection, materials);
            if (materials[part->render_method_index].breakable_surface_index != 255)
                breakable_surface_render_cracks(bsp_index, -1, materials[part->render_method_index].breakable_surface_index);
        }
    }
    c_render_method_data::m_valid = false;
    render_method_submit_valid();
}


// @address 0x16f8c0 ?decorator_set_wind@@YAXPBUs_wind_definition@@@Z
void decorator_set_wind(const s_wind_definition *wind)
{
    c_rasterizer_texture_ref texture;
    if (wind != 0 && wind->gust_noise_bitmap.index != -1)
    {
        const bitmap_data *bitmap;
        bitmap_group_get_xenon_hardware_format(wind->gust_noise_bitmap.index, 0, &texture, &bitmap, 0);
    }
    else
    {
        const s_game_globals *game_globals = global_game_globals;
        const c_rasterizer_globals *rasterizer_globals;
        if (game_globals->rasterizer_globals_ref.index != -1)
            rasterizer_globals = TAG_GET('rasg', const c_rasterizer_globals, game_globals->rasterizer_globals_ref.index);
        else
            rasterizer_globals = 0;
        texture = rasterizer_globals->get_default_texture_rasterizer_texture_ref(10);
    }
    c_rasterizer::set_sampler_texture(20, texture);
    c_rasterizer::set_sampler_address_mode(20, c_rasterizer::_sampler_address_wrap);
    if (g_rasterizer_sampler_filter_modes[20] != c_rasterizer::_sampler_filter_mode_bilinear)
    {
        g_rasterizer_sampler_filter_modes[20] = c_rasterizer::_sampler_filter_mode_bilinear;
        rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 20, c_rasterizer::_sampler_filter_mode_bilinear,
            c_rasterizer::_sampler_filter_mode_bilinear);
    }
    c_rasterizer::get_device()->SetVertexShaderConstantF(243, &g_wind_values.texture_x, 1);
    c_rasterizer::get_device()->SetVertexShaderConstantF(244, &g_wind_values.velocity_x, 1);
}

#if !defined(_XBOX) || defined(__clang__)
// Explicit shader indices by decorator render type.
static const long k_decorator_explicit_shaders[] = { 0x2f, 0x46, 0x47, 0x48, 0x49, 0x52 };

void c_structure_renderer::render_decorators()
{
    const float distance_scale = g_player_render_tuning.decorator_distance_scale;
    if (distance_scale == 0.0f)
        return;
    const float draw_distance_scale = port_config_draw_distance();

    const real_point3d camera_position = c_player_view::get_current()->render_camera_state.position;
    const s_cluster_reference camera_cluster = scenario_cluster_reference_from_point(&camera_position);
    const scenario *scenario_definition = global_scenario_get();
    long wind_index = global_game_globals->default_wind.index;
    if (camera_cluster.bsp_index != NONE)
    {
        const scenario_structure_bsp_reference *reference = TAG_BLOCK_GET_ELEMENT(
            &scenario_definition->structure_bsps, camera_cluster.bsp_index, const scenario_structure_bsp_reference);
        wind_index = reference->wind.index;
    }
    const s_wind_definition *wind = wind_index != NONE ? TAG_GET('wind', const s_wind_definition, wind_index) : 0;
    if (wind)
        wind->update(&g_wind_values);

    D3DDevice *device = c_rasterizer::get_device();
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
    D3DDevice_SetRenderState_ColorWriteEnable(device, 0xF);
    decorator_set_wind(wind);
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_stipple_write);

    const c_visibility_collection collection(g_visibility_globals.collection_input);
    const s_visibility_region *region = collection.get_region();
    long last_bsp_index = NONE;
    long last_cluster_index = NONE;
    long last_sky_index = NONE;
    real_vector4d sun_color = {{1.0f, 1.0f, 1.0f, 0.0f}};
    real_vector4d sun_direction = {{0.0f, 0.0f, 1.0f, 0.0f}};
    for (long visible_index = 0; visible_index < region->cluster_count; ++visible_index)
    {
        const visibility_cluster &visible = region->clusters[visible_index];
        const long bsp_index = visible.bsp_index;
        const long cluster_index = visible.cluster_index;
        if (last_bsp_index == bsp_index && last_cluster_index == cluster_index)
            continue;
        last_bsp_index = bsp_index;
        last_cluster_index = cluster_index;

        const structure_bsp *bsp = global_structure_bsp_get(bsp_index);
        const structure_cluster *cluster = TAG_BLOCK_GET_ELEMENT(&bsp->clusters, cluster_index, const structure_cluster);
        if (cluster->decorator_groups.count > 0)
        {
            const long sky_index = scenario_zone_set_structure_bsp_cluster_visible_sky_index_get(
                scenario_definition, global_zone_set_index, bsp_index, cluster_index);
            if (sky_index != NONE)
            {
                if (last_sky_index != sky_index)
                {
                    real_linear_rgb_color color;
                    real_vector3d direction;
                    atmosphere_query_sky_sun_parameters(sky_index, &color, &direction);
                    sun_color = real_vector4d{{color.red, color.green, color.blue, 0.0f}};
                    sun_direction = real_vector4d{{direction.i, direction.j, direction.k, 0.0f}};
                    last_sky_index = sky_index;
                }
                c_rasterizer::set_vertex_shader_constant(247, 1, &sun_direction);
                c_rasterizer::set_vertex_shader_constant(248, 1, &sun_color);
            }
        }

        const s_decorator_group *groups = static_cast<const s_decorator_group *>(cluster->decorator_groups.address);
        const s_tag_reference *sets = static_cast<const s_tag_reference *>(bsp->decorator_sets.address);
        for (long group_index = 0; group_index < cluster->decorator_groups.count; ++group_index)
        {
            rasterizer_set_alpha_test_enabled(true, group_index);
            const s_decorator_group &group = groups[group_index];
            if (group.instance_count == 0 || sets[group.decorator_set_index].index == NONE)
                continue;
            const s_decorator_set *set = TAG_GET('dctr', const s_decorator_set, sets[group.decorator_set_index].index);
            if (set->render_model.index == NONE)
                continue;

            const float pixel_scale = (distance_scale / render_player_view_compute_pixel_scale()) * draw_distance_scale;
            const float fade_start = set->fade_start * pixel_scale;
            const float fade_end = set->fade_end * pixel_scale;
            const float radius = group.instance_shader_parameters.l;
            if (distance3d(&group.bounding_center, &camera_position) > (1.0f - set->early_cull_fraction) * (radius + fade_end))
                continue;
            bool fully_contained;
            if (!visibility_region_test_sphere(region, visible_index, &group.bounding_center, radius, &fully_contained))
                continue;

            c_rasterizer::set_explicit_shaders(k_decorator_explicit_shaders[set->render_type], _vertex_type_decorator,
                _transfer_vector_vertex_none, _entry_point_default);
            const float fade_scale = 0.7f / (fade_start - fade_end);
            const real_vector4d fade = {{fade_scale, 1.0f - fade_scale * fade_start, 0.0f, 0.0f}};
            c_rasterizer::set_vertex_shader_constant(245, 1, &fade);
            c_rasterizer::set_vertex_shader_constant(246, 1, &set->vertex_constants_246);
            c_rasterizer::set_pixel_shader_constant(13, 1, &set->pixel_constants_13);
            c_rasterizer::set_vertex_shader_constant(249, 1, &set->vertex_constants_249);
            D3DDevice_SetRenderState_CullMode(device, TEST_BIT(set->flags, 0) ? D3DCULL_NONE : D3DCULL_CW);

            const render_model_definition *model = TAG_GET('mode', const render_model_definition, set->render_model.index);
            const s_render_geometry &geometry = model->render_geometry;
            c_rasterizer::set_vertex_stream(render_geometry_get_vertex_buffer(&geometry, 0));
            d3d9_set_vertex_shader_bounds(static_cast<const s_compression_info *>(geometry.compression_info.address));
            render_method_submit_set_texture(0, set->texture.index, 0);
            c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_wrap);
            c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_bilinear);

            const c_rasterizer_vertex_buffer *instance_buffer = render_geometry_get_vertex_buffer(
                &bsp->decorator_instance_buffer, group.instance_buffer_index);
            rasterizer_set_vertex_stream_with_offset(instance_buffer, group.instance_buffer_offset);
            // The first component of the culling center is also the last component of shader constant 241.
            const real_vector4d instance_constants[] = {
                group.instance_shader_parameters,
                {{group.instance_shader_parameters_tail.i, group.instance_shader_parameters_tail.j,
                    group.instance_shader_parameters_tail.k, group.bounding_center.x}},
            };
            c_rasterizer::set_vertex_shader_constant(240, 2, instance_constants);

            const s_mesh *mesh = TAG_BLOCK_GET_ELEMENT(&geometry.meshes, model->instance_starting_mesh_index, const s_mesh);
            const s_subpart *subpart = TAG_BLOCK_GET_ELEMENT(&mesh->subparts, 0, const s_subpart);
            const real_vector4d index_count = {{(float)subpart->index_count, 0.0f, 0.0f, 0.0f}};
            c_rasterizer::set_vertex_shader_constant(242, 1, &index_count);
            device->SetIndices(0);
            D3DDevice_DrawVertices(device, D3DPT_TRIANGLESTRIP, 0, (unsigned long)subpart->index_count * group.instance_count);
        }
    }

    c_render_method_data::m_valid = false;
    render_method_submit_valid();
    rasterizer_set_alpha_test_enabled(false, 0);
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
}
#endif
