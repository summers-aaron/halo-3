// graphicslibxbox/rasterizer_dynamic_render_targets.obj: from the records 0x467d98 0x467df0 0x467f40 0x467f88 0x4883b0
// @flags /TP /O2
// @address 0x832a88 g_rasterizer_surface_groups
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
#include "memory/data_types.h"
#include "rasterizer/render_target_types.h"
#include "memory/static_arrays.h"
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x45eee0 ?allocate@c_rasterizer_texture_ref@@SA?AV1@JJJJJ_NW4e_bitmap_type@@0PAX@Z
#include <xtl.h>
#include <math.h>
#include "rasterizer/rasterizer_dynamic_render_targets.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"
#include "rasterizer/rasterizer_dynamic_render_target_types.h"
#include "memory/data.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer_d3d_allocations.h"

enum e_bitmap_type { _bitmap_type_2d = 0 };
#include "rasterizer/rasterizer_render_target_types.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "rasterizer/rasterizer.h"

c_static_array<long, 6> c_dynamic_render_targets::m_used_targets;
s_data_array *c_dynamic_render_targets::m_targets;
extern "C" {
// @address 0x7bf7a0 g_rasterizer_accumulation_texture
c_rasterizer_texture_ref g_rasterizer_accumulation_texture;
// @address 0x7bf7a4 g_rasterizer_albedo_texture
c_rasterizer_texture_ref g_rasterizer_albedo_texture;
// @address 0x7bf7a8 g_rasterizer_normal_texture
c_rasterizer_texture_ref g_rasterizer_normal_texture;
// @address 0x88e344 g_rasterizer_accumulation_surface
D3DSurface *g_rasterizer_accumulation_surface;
// @address 0x88e348 g_rasterizer_normal_surface
D3DSurface *g_rasterizer_normal_surface;
// @address 0x88e34c g_rasterizer_albedo_surface
D3DSurface *g_rasterizer_albedo_surface;
// @address 0x88e350 g_rasterizer_depth_surface
D3DSurface *g_rasterizer_depth_surface;
// @address 0x8c4858 g_dynamic_render_target_alloc_mask
long g_dynamic_render_target_alloc_mask;
}
// @address 0x58830 k_dynamic_render_target_tile_width
// @address 0x58838 k_dynamic_render_target_tile_height
extern "C" {
const double k_dynamic_render_target_tile_width = 48.0;
const double k_dynamic_render_target_tile_height = (double)9.6f;
}
// @address 0x467d98 ?initialize@c_dynamic_render_targets@@SAXXZ
void c_dynamic_render_targets::initialize() {
    data_make_valid(m_targets);
    m_used_targets.set_all(-1);
    allocate_global_temporary_buffers();
}
// @address 0x467df0 ?allocate_target@c_dynamic_render_targets@@SAJW4e_render_target_type@@JJJJPAD@Z
long c_dynamic_render_targets::allocate_target(e_render_target_type target_type, long width, long height, long old_index, long reference, char *name)
{
    long target_index = -1;
    bool available = target_type == _render_target_type_explicit || (g_dynamic_render_target_alloc_mask & (1 << target_type)) == 0;
    if (available) {
        g_dynamic_render_target_alloc_mask |= 1 << target_type;
        s_data_array *targets = m_targets;
        target_index = datum_new(targets);
        m_used_targets[target_type] = target_index;
        if (target_index != -1) {
            s_dynamic_render_target *target = &((s_dynamic_render_target *)targets->data)[(unsigned short)target_index];
            target->type = target_type;
            target->reference = reference;
            long texture_width, texture_height;
            g_rasterizer_albedo_texture.get_width_height(0, &texture_width, &texture_height);
            g_rasterizer_normal_texture.get_width_height(0, &texture_width, &texture_height);
            g_rasterizer_accumulation_texture.get_width_height(0, &texture_width, &texture_height);
            target->accumulation_texture = g_rasterizer_accumulation_texture;
            target->albedo_texture = g_rasterizer_albedo_texture;
            target->normal_texture = g_rasterizer_normal_texture;
            target->depth_surface = g_rasterizer_depth_surface;
            target->albedo_surface = g_rasterizer_albedo_surface;
            target->normal_surface = g_rasterizer_normal_surface;
            target->accumulation_surface = g_rasterizer_accumulation_surface;
        }
    }
    return target_index;
}
s_dynamic_render_target *c_dynamic_render_targets::get_target_from_target_type(e_render_target_type target_type) {
    long target_index = m_used_targets[target_type];
    if (target_index != -1) {
        return get_target(target_index);
    } else {
        return 0;
    }
}
static __forceinline c_rasterizer_texture_ref rasterizer_dynamic_render_targets_allocate_albedo() {
    D3DTexture *texture = 0;
    D3DTexture *result = 0;
    if (c_d3d_resource_allocator::create_texture(384, 384, 1, 0, (D3DFORMAT)0x18280186, 1, &texture, 0, 0, 0) >= 0) result = texture;
    return c_rasterizer_texture_ref(result);
}
// @address 0x467f88 ?allocate_global_temporary_buffers@c_dynamic_render_targets@@SAXXZ
void c_dynamic_render_targets::allocate_global_temporary_buffers() {
    long tiles = (long)((float)ceil(k_dynamic_render_target_tile_width) * (float)ceil(k_dynamic_render_target_tile_height));
    g_rasterizer_albedo_texture = rasterizer_dynamic_render_targets_allocate_albedo();
    g_rasterizer_accumulation_texture = g_rasterizer_albedo_texture;
    g_rasterizer_normal_texture = c_rasterizer_texture_ref::allocate(384, 384, 1, 0x1a200187, 0, false, _bitmap_type_2d, true,
        (void *)(uintptr_t)(g_rasterizer_surface_groups[14].surfaces->resource->words[8] & ~4095));
    D3DSURFACE_PARAMETERS parameters;
    parameters.Base = 0; parameters.ColorExpBias = 0; parameters.HierarchicalZBase = 0;
    long hr = c_rasterizer::get_device()->CreateDepthStencilSurface(
        384, 384, (D3DFORMAT)0x1a220197, (D3DMULTISAMPLE_TYPE)2, 0, FALSE, &g_rasterizer_depth_surface, &parameters);
    parameters.Base = tiles;
    parameters.HierarchicalZBase = -1;
    hr &= c_rasterizer::get_device()->CreateRenderTarget(
        384, 384, (D3DFORMAT)0x18287f86, (D3DMULTISAMPLE_TYPE)2, 0, FALSE, &g_rasterizer_albedo_surface, &parameters);
    hr &= c_rasterizer::get_device()->CreateRenderTarget(
        384, 384, (D3DFORMAT)0x18287f86, (D3DMULTISAMPLE_TYPE)2, 0, FALSE, &g_rasterizer_accumulation_surface, &parameters);
    parameters.Base = tiles * 2;
    hr &= c_rasterizer::get_device()->CreateRenderTarget(
        384, 384, (D3DFORMAT)0x1a200187, (D3DMULTISAMPLE_TYPE)2, 0, FALSE, &g_rasterizer_normal_surface, &parameters);
}
// @address 0x4883b0 dynamic_render_targets_set_accumulation_texture
extern "C" void dynamic_render_targets_set_accumulation_texture()
{
    long target_index = c_dynamic_render_targets::m_used_targets[1];
    if (target_index != -1) {
        s_dynamic_render_target *target = c_dynamic_render_targets::get_target(target_index);
        if (target) {
            c_rasterizer_texture_ref texture_ref = target->accumulation_texture;
            c_rasterizer::set_sampler_texture(0, texture_ref);
            c_rasterizer::set_sampler_address_mode(
                0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            if (g_rasterizer_sampler_filter_modes[0] != 1) {
                g_rasterizer_sampler_filter_modes[0] = 1;
                rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 0, 1, 1);
            }
        }
    }
}
