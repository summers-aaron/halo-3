// graphicslibxbox/rasterizer_stipple.obj: from the records 0x16d2b8
// @flags /TP /O2 /Oc
#include "rasterizer/rasterizer_main_globals.h"
#include "math/real_math_types.h"
#include <xtl.h>
#include "rasterizer/rasterizer_states.h"
#include "tag_files/tag_groups.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_global_definitions.h"




#include "rasterizer/rasterizer_textures_cache.h"

#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"

#include "rasterizer/rasterizer_sampler_texture_calls.h"

// match: 0x16d2b8: grouped storage retains the source and both sampler slots
// across canonical calls; sampler slots are initialized immediately before use.
struct s_rasterizer_stipple_texture_slot
{
    long reference;
    __forceinline s_rasterizer_stipple_texture_slot() {}
};

struct s_rasterizer_stipple_textures
{
    c_rasterizer_texture_ref source;
    s_rasterizer_stipple_texture_slot sampler;
    s_rasterizer_stipple_texture_slot none;
};

void rasterizer_stipple_initialize(void)
{
    if (c_rasterizer::set_explicit_shaders(89, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default)) {
        if (c_rasterizer::g_current_alpha_blend_mode != 0) {
            c_rasterizer::g_current_alpha_blend_mode = c_rasterizer::_alpha_blend_opaque;
            c_rasterizer::g_device->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        }
        ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)2);
        c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
        c_rasterizer::set_scissor_rect(0);
        c_rasterizer::get_device()->SetRenderState(D3DRS_CULLMODE, 0);
        c_rasterizer::end_high_quality_blend();
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)8);
        c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
        c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
        c_rasterizer::refresh_tiling_render_targets();
        c_rasterizer::get_device()->SetRenderState(D3DRS_COLORWRITEENABLE, 1);
        long index = global_game_globals->rasterizer_globals_ref.index;
        c_rasterizer_globals *globals;
        if (index != -1)
            globals = (c_rasterizer_globals *)tag_get('rasg', index);
        else
            globals = 0;
        s_rasterizer_stipple_textures textures = { globals->get_default_texture_rasterizer_texture_ref(9) };
        textures.sampler.reference = textures.source.get_datum_ref();
        reinterpret_cast<rasterizer_sampler_texture_reference_call>(&c_rasterizer::set_sampler_texture)(0, (const c_rasterizer_texture_ref &)textures.sampler);
        c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)(1), (c_rasterizer::e_sampler_address_mode)(1),
            (c_rasterizer::e_sampler_address_mode)(1));
        c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_point);
        long height = c_rasterizer::get_surface_height((c_rasterizer::e_surface)8);
        long width = c_rasterizer::get_surface_width((c_rasterizer::e_surface)8);
        c_rasterizer::draw_fullscreen_quad(height, width);
        c_rasterizer::get_device()->SetRenderState(D3DRS_COLORWRITEENABLE, 15);
        textures.none.reference = -1;
        reinterpret_cast<rasterizer_sampler_texture_reference_call>(&c_rasterizer::set_sampler_texture)(0, (const c_rasterizer_texture_ref &)textures.none);
    }
}
