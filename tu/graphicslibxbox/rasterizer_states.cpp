// graphicslibxbox/rasterizer_states.obj: from the records 0x14d118 0x14d4b8 0x152178 0x167948 0x167be0 0x167cc8 0x168258 0x16d438 0x16d528 0x170de8 0x175948 0x177040 0x17ec80 0x17f118 0x192020 0x195e88 0x196118 0x20eca0 0x2a3ef8 0x448910 0x448e38 0x449068
#include "render/views/render_player_view.h"
#include "cutscene/cinematics.h"
#include "rasterizer/dx9/rasterizer_screenshot_types.h"
#include "rasterizer/rasterizer_shader_parameter_calls.h"
#include "rasterizer/rasterizer_state_globals.h"
// @address 0x81dab0 g_rasterizer_current_z_buffer_mode
// @address 0x81dab8 ?g_current_alpha_blend_mode@c_rasterizer@@0W4e_alpha_blend_mode@1@A
#include "rasterizer/rasterizer_depth_bias_globals.h"
// @compiler xdk6130
#include "objects/object_render_state_tls.h"
// @flags /TP /O2
#include "rasterizer/rasterizer_main_globals.h"
// @address 0x8c4694 ?g_device@c_rasterizer@@0PAUD3DDevice@@A
// @address 0x168d98 ?clearf@c_rasterizer@@SAXKPBTreal_vector4d@@MK@Z
// @address 0x459200 ?draw_fullscreen_quad@c_rasterizer@@SAXXZ
// @address 0x14c8a8 ?set_render_target@c_rasterizer@@SAXJW4e_surface@1@@Z
// @address 0x168618 ?set_depth_stencil_surface@c_rasterizer@@SAXW4e_surface@1@@Z
// @address 0x16dcb0 ?refresh_tiling_render_targets@c_rasterizer@@SAXXZ
// @address 0x167f60 ?begin_high_quality_blend@c_rasterizer@@SAXXZ
// @address 0x168258 ?set_z_buffer_mode@c_rasterizer@@SAXW4e_z_buffer_mode@1@@Z
#include "math/real_math_types.h"
#include "objects/object_iterator.h"
#include <xtl.h>
#include "rasterizer/rasterizer_states.h"
#include "memory/data_types.h"
#include "render/shader_reference_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "rasterizer/global_shader_parameter_types.h"
#include "render_methods/render_method_submit_globals.h"
#include "render_methods/render_method_submit_samplers.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_global_definitions.h"

#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "tag_files/tag_types.h"


#include "rasterizer/rasterizer_shader_definitions.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer_resource_definitions.h"

#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_stencil.h"
#include "visibility/visibility_projections_and_volumes.h"
// @address 0x73a1d8 ?x_last_sampler_textures@c_rasterizer@@0PAJA
long c_rasterizer::x_last_sampler_textures[26] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

extern "C" {
// @address 0x81daac g_rasterizer_stencil_mode
c_rasterizer::e_stencil_mode g_rasterizer_stencil_mode;
}


#include "visibility/visibility_render_objects.h"
#include "rasterizer/rasterizer_tiling_types.h"
#include "rasterizer/rasterizer_tiling_globals.h"
#include "render/render_visibility_cached_state_types.h"
#if !defined(_XBOX) || defined(__clang__)
#include "math/matrix_math_constants.h"
#include "rasterizer/rasterizer_depth_bias.h"
#endif

__forceinline bool c_vertex_declaration_table::set(e_vertex_type vertex_type, e_transfer_vector_vertex_types transfer_type, e_entry_point entry)
    {
        bool result = true;
        D3DVertexDeclaration *declaration = rasterizer_get_vertex_declaration(vertex_type, transfer_type, entry);
        if (c_rasterizer::get_device()->SetVertexDeclaration(declaration) < 0)
            result = false;
        return result;
    }


void c_rasterizer::set_alpha_blend_mode(e_alpha_blend_mode mode)
{
    if (g_current_alpha_blend_mode != mode)
    {
        g_current_alpha_blend_mode = mode;
        rasterizer_set_device_alpha_blend_mode(g_device, mode);
    }
}

void c_rasterizer::set_sampler_texture_direct(long sampler, D3DTexture *texture)
{
    g_device->SetTexture(sampler, texture);
}

// @address 0x14d118 ?set_vertex_shader@c_rasterizer@@SA_NPBVc_rasterizer_vertex_shader@@W4e_vertex_type@@W4e_transfer_vector_vertex_types@@W4e_entry_point@@@Z
bool c_rasterizer::set_vertex_shader(
    const c_rasterizer_vertex_shader *shader, e_vertex_type vertex_type, e_transfer_vector_vertex_types transfer_type, e_entry_point entry)
{
    g_render_method_submit_cache = k_default_render_method_submit_cache;
    if (shader) {
        D3DVertexShader *d3d_shader = shader->get_d3d_shader(vertex_type, entry, 0);
        return c_vertex_declaration_table::set(vertex_type, _transfer_vector_vertex_none, entry) && d3d_shader && g_device->SetVertexShader(d3d_shader) >= 0;
    } else {
        return g_device->SetVertexShader(0) == 0;
    }
}

// @address 0x14d4b8 ?set_pixel_shader@c_rasterizer@@SA_NPBVc_rasterizer_pixel_shader@@W4e_entry_point@@@Z
bool c_rasterizer::set_pixel_shader(const c_rasterizer_pixel_shader *shader, e_entry_point entry)
{
    g_render_method_submit_cache = k_default_render_method_submit_cache;
    if (shader) {
        D3DPixelShader *d3d_shader = shader->get_d3d_shader(entry, 0);
        return d3d_shader && g_device->SetPixelShader(d3d_shader) >= 0;
    } else {
        return g_device->SetPixelShader(0) == 0;
    }
}

// @address 0x152178 ?set_scissor_rect@c_rasterizer@@SAXPBTrectangle2d@@@Z
void c_rasterizer::set_scissor_rect(const rectangle2d *rectangle)
{
    if (rectangle) {
        RECT d3d_rectangle;
        d3d_rectangle.left = rectangle->x0;
        d3d_rectangle.top = rectangle->y0;
        d3d_rectangle.right = rectangle->x1;
        d3d_rectangle.bottom = rectangle->y1;
        g_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
        g_device->SetScissorRect(&d3d_rectangle);
    } else {
        g_device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    }
}

// @address 0x167948 ?set_sampler_texture@c_rasterizer@@SAXJVc_rasterizer_texture_ref@@@Z
void c_rasterizer::set_sampler_texture(long sampler, c_rasterizer_texture_ref texture_ref)
{
    if (texture_ref.get_datum_ref() == -1) {
        g_device->SetTexture(sampler, 0);
        x_last_sampler_textures[sampler] = -1;
    } else if (x_last_sampler_textures[sampler] != texture_ref.get_datum_ref()) {
        x_last_sampler_textures[sampler] = texture_ref.get_datum_ref();
        if (texture_ref.valid()) {
            D3DBaseTexture *texture = texture_ref.get_d3d_texture();
            g_device->SetTexture(sampler, texture);
        } else {
            g_device->SetTexture(sampler, 0);
        }
    }
}

// @address 0x16d528 ?set_stencil_mode@c_rasterizer@@SAXW4e_stencil_mode@1@@Z
void c_rasterizer::set_stencil_mode(e_stencil_mode mode)
{
    if (mode != g_rasterizer_stencil_mode) {
        g_rasterizer_stencil_mode = mode;
        switch (mode) {
        case _stencil_mode_off:
            g_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            break;
        case _stencil_mode_old_stipple:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILREF, 255);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            g_device->SetRenderState(D3DRS_STENCILMASK, 0);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            break;
        case _stencil_mode_stipple_write:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            g_device->SetRenderState(D3DRS_STENCILREF, 32);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 0);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 32);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            break;
        case _stencil_mode_stipple_read:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
            g_device->SetRenderState(D3DRS_STENCILREF, 32);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 32);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            break;
        case _stencil_mode_shadow_apply:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            g_device->SetRenderState(D3DRS_STENCILREF, 64);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_REPLACE);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_ZERO);
            g_device->SetRenderState(D3DRS_STENCILMASK, 0);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 127);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 64);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        case _stencil_mode_shadow_generate:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
            g_device->SetRenderState(D3DRS_STENCILREF, 64);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_ZERO);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 255);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 127);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 64);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        case _stencil_mode_shadow_apply_write:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
            g_device->SetRenderState(D3DRS_STENCILREF, 64);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 255);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 64);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        case _stencil_mode_shadow_apply_write_always:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            g_device->SetRenderState(D3DRS_STENCILREF, 64);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 255);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 64);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        case _stencil_mode_shadow_apply_write_increment:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
            g_device->SetRenderState(D3DRS_STENCILREF, 64);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 255);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 127);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 64);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        case _stencil_mode_shadow_generate_hi:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            g_device->SetRenderState(D3DRS_STENCILREF, 64);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_REPLACE);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_REPLACE);
            g_device->SetRenderState(D3DRS_STENCILMASK, 0);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 127);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 64);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        case _stencil_mode_clear_hi:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            g_device->SetRenderState(D3DRS_STENCILREF, 0);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 0);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 255);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 255);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_EQUAL);
            break;
        case _stencil_mode_object_write:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            g_device->SetRenderState(D3DRS_STENCILREF, 128);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 0);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 255);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 128);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        case _stencil_mode_object_read:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
            g_device->SetRenderState(D3DRS_STENCILREF, 128);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 255);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 128);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        case _stencil_mode_shadow_read:
            g_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
            g_device->SetRenderState(D3DRS_STENCILREF, 64);
            g_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO);
            g_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            g_device->SetRenderState(D3DRS_STENCILMASK, 255);
            g_device->SetRenderState(D3DRS_STENCILWRITEMASK, 127);
            g_device->SetRenderState(D3DRS_HISTENCILENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, TRUE);
            g_device->SetRenderState(D3DRS_HISTENCILREF, 64);
            g_device->SetRenderState(D3DRS_HISTENCILFUNC, D3DHSCMP_NOTEQUAL);
            break;
        }
    }
}

// @address 0x8086b7 g_rasterizer_stencil_ref
extern "C" {
#if !defined(_XBOX) || defined(__clang__)
// The Xbox linker supplies this byte in the shared BSS range.
unsigned char g_rasterizer_stencil_ref;
#endif
// @address 0x8c484e g_render_static_lighting_active
bool g_render_static_lighting_active;
}


// @address 0x170de8 rasterizer_set_shadow_stencil_ref
void rasterizer_set_shadow_stencil_ref(int stencil_index, int bit6_flag, int bit5_flag, int bit7_flag)
{
    if (g_render_static_lighting_active == 0) {
        int index_field = (stencil_index >> 3) & 31;
        int flag_bits = (bit7_flag != 0 ? 128 : 0) + (bit6_flag != 0 ? 64 : 0) + (bit5_flag != 0 ? 32 : 0);
        int clamped_index;
        if ((unsigned int)index_field > 31) {
            clamped_index = 31;
        } else {
            clamped_index = index_field;
        }
        unsigned char reference = (unsigned char)(clamped_index + flag_bits);
        if (g_rasterizer_stencil_mode != c_rasterizer::_stencil_mode_shadow_generate_hi_ref) {
            g_rasterizer_stencil_mode = c_rasterizer::_stencil_mode_shadow_generate_hi_ref;
            g_rasterizer_stencil_ref = reference;
            D3DDevice_SetRenderState_StencilEnable(c_rasterizer::get_device(), 1);
            D3DDevice_SetRenderState_StencilFunc(c_rasterizer::get_device(), 6);
            D3DDevice_SetRenderState_StencilMask(c_rasterizer::get_device(), 31);
            D3DDevice_SetRenderState_StencilWriteMask(c_rasterizer::get_device(), 224);
            D3DDevice_SetRenderState_StencilPass(c_rasterizer::get_device(), 2);
            D3DDevice_SetRenderState_StencilFail(c_rasterizer::get_device(), 0);
            D3DDevice_SetRenderState_StencilZFail(c_rasterizer::get_device(), 0);
            D3DDevice_SetRenderState_HiStencilEnable(c_rasterizer::get_device(), 0);
            D3DDevice_SetRenderState_HiStencilWriteEnable(c_rasterizer::get_device(), 0);
            D3DDevice_SetRenderState_StencilRef(c_rasterizer::get_device(), reference);
        } else if (reference != g_rasterizer_stencil_ref) {
            g_rasterizer_stencil_ref = reference;
            D3DDevice_SetRenderState_StencilRef(c_rasterizer::get_device(), reference);
        }
    }
    return;
}

extern "C" {
// @address 0x8c46d4 k_rasterizer_depth_bias_table
float k_rasterizer_depth_bias_table[2];
// @address 0x73a1b0 k_decal_slope_scale_bias
float k_decal_slope_scale_bias = 1.414f;
}

static const float k_decal_depth_bias = 0.003f;
static const float k_mode_6_slope_scale_bias = -0.5f;
static const float k_mode_6_depth_bias = -0.000005f;

static float rasterizer_states_decal_2_depth_bias = 0.0008f;
#if defined(_XBOX) && !defined(__clang__)
// @address 0x73a1c0 rasterizer_states_mode_5_slope_scale_bias
static float rasterizer_states_mode_5_slope_scale_bias = -0.1f;
// @address 0x73a1c4 rasterizer_states_mode_7_depth_bias
static float rasterizer_states_mode_7_depth_bias = -0.0001f;
#else // other units read these extern "C" ones (rasterizer/rasterizer_depth_bias.h, math/matrix_math_constants.h)
const float rasterizer_states_mode_5_slope_scale_bias = -0.1f;
const float rasterizer_states_mode_7_depth_bias = -0.0001f;
#endif

void c_rasterizer::set_z_buffer_mode(e_z_buffer_mode z_buffer_mode)
{
    unsigned char use_floating_point_z_buffer = g_rasterizer_use_floating_point_z_buffer;
    float scale = 1.0f;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x168258: shared base register with k_rasterizer_depth_bias_table; by name adds a lis, frame 0x90->0xA0
    union { float as_float; unsigned long as_dword; } slope_scale_bias = { k_rasterizer_depth_bias_table[-1] };
    union { float as_float; unsigned long as_dword; } depth_bias = { k_rasterizer_depth_bias_table[-1] };
#else
    union { float as_float; unsigned long as_dword; } slope_scale_bias = { g_rasterizer_depth_bias };
    union { float as_float; unsigned long as_dword; } depth_bias = { g_rasterizer_depth_bias };
#endif
    unsigned long z_function;
    if (use_floating_point_z_buffer) {
        z_function = 6;
        scale = visibility_projections_and_volumes_minus_one;
    } else {
        z_function = 3;
    }

    if (z_buffer_mode != g_rasterizer_current_z_buffer_mode || use_floating_point_z_buffer != g_rasterizer_current_z_buffer_floating_point) {
        g_rasterizer_current_z_buffer_mode = z_buffer_mode;
        g_rasterizer_current_z_buffer_floating_point = use_floating_point_z_buffer;

        switch (z_buffer_mode) {
        case _z_buffer_mode_normal:
            D3DDevice_SetRenderState_ZEnable(g_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_device, 1);
            D3DDevice_SetRenderState_ZFunc(g_device, z_function);
            break;
        case _z_buffer_mode_read_only:
            D3DDevice_SetRenderState_ZEnable(g_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_device, 0);
            D3DDevice_SetRenderState_ZFunc(g_device, z_function);
            break;
        case _z_buffer_mode_7:
            D3DDevice_SetRenderState_ZEnable(g_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_device, 0);
            D3DDevice_SetRenderState_ZFunc(g_device, z_function);
            slope_scale_bias.as_float = k_rasterizer_depth_bias_table[1];
            depth_bias.as_float = rasterizer_states_mode_7_depth_bias;
            break;
        case _z_buffer_mode_off:
            D3DDevice_SetRenderState_ZEnable(g_device, 0);
            break;
        case _z_buffer_mode_decal:
            D3DDevice_SetRenderState_ZEnable(g_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_device, 1);
            D3DDevice_SetRenderState_ZFunc(g_device, z_function);
            slope_scale_bias.as_float = k_decal_slope_scale_bias;
            depth_bias.as_float = k_decal_depth_bias;
            break;
        case _z_buffer_mode_decal_2:
            D3DDevice_SetRenderState_ZEnable(g_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_device, 1);
            D3DDevice_SetRenderState_ZFunc(g_device, z_function);
            slope_scale_bias.as_float = k_decal_slope_scale_bias;
            depth_bias.as_float = rasterizer_states_decal_2_depth_bias;
            break;
        case _z_buffer_mode_5:
            D3DDevice_SetRenderState_ZEnable(g_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_device, 0);
            D3DDevice_SetRenderState_ZFunc(g_device, z_function);
            slope_scale_bias.as_float = rasterizer_states_mode_5_slope_scale_bias;
            depth_bias.as_float = k_rasterizer_depth_bias_table[0];
            break;
        case _z_buffer_mode_6:
            D3DDevice_SetRenderState_ZEnable(g_device, 1);
            D3DDevice_SetRenderState_ZWriteEnable(g_device, 0);
            D3DDevice_SetRenderState_ZFunc(g_device, z_function);
            slope_scale_bias.as_float = k_mode_6_slope_scale_bias;
            depth_bias.as_float = k_mode_6_depth_bias;
        default:
            slope_scale_bias.as_float *= scale;
            depth_bias.as_float *= scale;
            D3DDevice_SetRenderState_SlopeScaleDepthBias(g_device, slope_scale_bias.as_dword);
            goto apply_depth_bias;
        }

        slope_scale_bias.as_float *= scale;
        depth_bias.as_float *= scale;
        D3DDevice_SetRenderState_SlopeScaleDepthBias(g_device, slope_scale_bias.as_dword);
apply_depth_bias:
        D3DDevice_SetRenderState_DepthBias(g_device, depth_bias.as_dword);
    }
}


// The global shader parameter table: one entry per e_render_method_extern (49). Entries are
// verbatim image bytes; names are the shipped .rdata strings. Entries 24-26 carry
// a set pad byte after pixel_shader in the shipped image; the range below preserves it.
// @rdata 0x82025088..0x8202545c
extern "C" {
// @address 0x25088 k_global_shader_parameters
const s_global_shader_parameter k_global_shader_parameters[49] = {
    { 0, -1, 6, false, "_render_method_extern_none" },
    { 3, 10, 0, false, "_render_method_extern_texture_global_target_texaccum" },
    { 3, 11, 0, false, "_render_method_extern_texture_global_target_normal" },
    { 3, 11, 0, false, "_render_method_extern_texture_global_target_z" },
    { 1, -1, 6, false, "_render_method_extern_texture_global_target_shadow_buffer1" },
    { 1, -1, 6, false, "_render_method_extern_texture_global_target_shadow_buffer2" },
    { 1, -1, 6, false, "_render_method_extern_texture_global_target_shadow_buffer3" },
    { 1, -1, 6, false, "_render_method_extern_texture_global_target_shadow_buffer4" },
    { 1, -1, 6, false, "_render_method_extern_texture_global_target_texture_camera" },
    { 1, -1, 6, false, "_render_method_extern_texture_global_target_reflection" },
    { 1, -1, 6, false, "_render_method_extern_texture_global_target_refraction" },
    { 2, 12, 0, false, "_render_method_extern_texture_lightprobe_texture" },
    { 2, 13, 0, false, "_render_method_extern_texture_dominant_light_intensity_map" },
    { 0, -1, 6, false, "_render_method_extern_texture_unused1" },
    { 0, -1, 6, false, "_render_method_extern_texture_unused2" },
    { 0, -1, 6, false, "_render_method_extern_object_change_color_primary" },
    { 0, -1, 6, false, "_render_method_extern_object_change_color_secondary" },
    { 0, -1, 6, false, "_render_method_extern_object_change_color_tertiary" },
    { 0, -1, 6, false, "_render_method_extern_object_change_color_quaternary" },
    { 0, -1, 6, false, "_render_method_extern_object_emblem_color_background" },
    { 0, -1, 6, false, "_render_method_extern_object_emblem_color_primary" },
    { 0, -1, 6, false, "_render_method_extern_object_emblem_color_secondary" },
    { 0, -1, 6, false, "_render_method_extern_texture_dynamic_environment_map_0" },
    { 0, -1, 6, false, "_render_method_extern_texture_dynamic_environment_map_1" },
    { 1, -1, 6, false, "_render_method_extern_texture_cook_torrance_cc0236" },
    { 1, -1, 6, false, "_render_method_extern_texture_cook_torrance_dd0236" },
    { 1, -1, 6, false, "_render_method_extern_texture_cook_torrance_c78d78" },
    { 0, -1, 6, false, "_render_method_extern_light_dir_0" },
    { 0, -1, 6, false, "_render_method_extern_light_color_0" },
    { 0, -1, 6, false, "_render_method_extern_light_dir_1" },
    { 0, -1, 6, false, "_render_method_extern_light_color_1" },
    { 0, -1, 6, false, "_render_method_extern_light_dir_2" },
    { 0, -1, 6, false, "_render_method_extern_light_color_2" },
    { 0, -1, 6, false, "_render_method_extern_light_dir_3" },
    { 0, -1, 6, false, "_render_method_extern_light_color_3" },
    { 0, -1, 6, false, "_render_method_extern_texture_unused_3" },
    { 0, -1, 6, false, "_render_method_extern_texture_unused_4" },
    { 0, -1, 6, false, "_render_method_extern_texture_unused_5" },
    { 0, -1, 6, false, "_render_method_extern_texture_dynamic_light_gel_0" },
    { 0, -1, 6, false, "_render_method_extern_flat_envmap_matrix_x" },
    { 0, -1, 6, false, "_render_method_extern_flat_envmap_matrix_y" },
    { 0, -1, 6, false, "_render_method_extern_flat_envmap_matrix_z" },
    { 0, -1, 6, false, "_render_method_extern_debug_tint" },
    { 1, -1, 6, false, "_render_method_extern_screen_constants" },
    { 0, -1, 6, false, "_render_method_extern_active_camo_distortion_texture" },
    { 3, 10, 0, false, "_render_method_extern_scene_ldr_texture" },
    { 3, 11, 0, false, "_render_method_extern_scene_hdr_texture" },
    { 1, -1, 6, false, "_render_method_extern_water_memory_export_address" },
    { 1, -1, 6, false, "_render_method_extern_tree_animation_timer" },
};
}
// @rdata-end

// @address 0x175948 rasterizer_set_global_shader_parameter
extern "C" void rasterizer_set_global_shader_parameter(long index)
{
    long parameter_index = index;
    if (k_global_shader_parameters[parameter_index].register_count >= 1)
    {
        real_vector4d value;
#if !defined(_XBOX) || defined(__clang__)
        render_method_submit_get_extern_constant(index, &value);
#else
        // The Xbox sampler call takes the index from the register left by the constant lookup.
        index = render_method_submit_get_extern_constant(index, &value);
#endif
        if (k_global_shader_parameters[parameter_index].pixel_shader)
        {
            c_rasterizer::get_device()->SetPixelShaderConstantF(k_global_shader_parameters[parameter_index].register_index, value.n, 1);
            // match: 0x175948: unreachable hint omits dead branch check
            __assume(0);
        }
        else
            c_rasterizer::get_device()->SetVertexShaderConstantF(k_global_shader_parameters[parameter_index].register_index, value.n, 1);
    }
    render_method_submit_set_extern_sampler(0, 0, index, k_global_shader_parameters[parameter_index].register_index);
}

// @address 0x177040 ?set_separate_alpha_blend_mode@c_rasterizer@@SAXW4e_separate_alpha_blend_mode@1@@Z
void c_rasterizer::set_separate_alpha_blend_mode(e_separate_alpha_blend_mode mode)
{
    if (mode != g_current_separate_alpha_blend_mode) {
        g_current_separate_alpha_blend_mode = mode;
        switch (mode) {
        // match: 0x177040: unreachable hint omits dead branch check
        default: __assume(0); break;
        case c_rasterizer::_separate_alpha_blend_off:
            g_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, 0);
            break;
        case c_rasterizer::_separate_alpha_blend_one_zero:
            g_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, 1);
            g_device->SetRenderState(D3DRS_BLENDOPALPHA, 0);
            g_device->SetRenderState(D3DRS_SRCBLENDALPHA, 1);
            g_device->SetRenderState(D3DRS_DESTBLENDALPHA, 0);
            break;
        case c_rasterizer::_separate_alpha_blend_one_one:
            g_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, 1);
            g_device->SetRenderState(D3DRS_BLENDOPALPHA, 0);
            g_device->SetRenderState(D3DRS_SRCBLENDALPHA, 1);
            g_device->SetRenderState(D3DRS_DESTBLENDALPHA, 1);
            break;
        case c_rasterizer::_separate_alpha_blend_dest_alpha_zero:
            g_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, 1);
            g_device->SetRenderState(D3DRS_BLENDOPALPHA, 0);
            g_device->SetRenderState(D3DRS_SRCBLENDALPHA, 10);
            g_device->SetRenderState(D3DRS_DESTBLENDALPHA, 0);
            break;
        case c_rasterizer::_separate_alpha_blend_constant_alpha:
            g_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, 1);
            g_device->SetRenderState(D3DRS_BLENDOPALPHA, 0);
            g_device->SetRenderState(D3DRS_SRCBLENDALPHA, 14);
            g_device->SetRenderState(D3DRS_DESTBLENDALPHA, 7);
            break;
        }
    }
}

static __forceinline c_rasterizer::e_surface rasterizer_states_get_surface_accum_LDR(bool hdr_alpha_blend)
{
    c_rasterizer::e_surface result = c_rasterizer::_surface_accum_LDR;
    c_rasterizer::g_HDR_alpha_blend = hdr_alpha_blend;
    return result;
}

static __forceinline c_rasterizer::e_surface rasterizer_states_get_surface_accum_HDR(bool hdr_alpha_blend)
{
    c_rasterizer::e_surface result = c_rasterizer::_surface_accum_HDR;
    c_rasterizer::g_HDR_alpha_blend = hdr_alpha_blend;
    return result;
}

// @address 0x17ec80 ?setup_targets_dynamic_light@c_rasterizer@@SAXXZ
void c_rasterizer::setup_targets_dynamic_light()
{
    set_depth_stencil_surface(_surface_depth_stencil);
    set_render_target(0, rasterizer_states_get_surface_accum_LDR(true));
    set_render_target(1, rasterizer_states_get_surface_accum_HDR(true));
    refresh_tiling_render_targets();
    begin_high_quality_blend();
}

// @address 0x17f118 ?clear_sampler_textures@c_rasterizer@@SAXK@Z
void c_rasterizer::clear_sampler_textures(unsigned long unused)
{
    for (long sampler = 0; sampler < 26; ++sampler) {
        c_rasterizer::get_device()->SetTexture(sampler, 0);
        x_last_sampler_textures[sampler] = -1;
    }
}


extern "C" {
// @address 0x7c6110 g_rasterizer_sampler_address_modes
long g_rasterizer_sampler_address_modes[26] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
// @address 0x73a1c8 k_d3d_texture_address_modes
unsigned long k_d3d_texture_address_modes[4] = { 0, 2, 1, 6 };
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
long g_rasterizer_sampler_filter_modes[26] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
}

// @address 0x167be0 ?set_sampler_address_mode@c_rasterizer@@SAXJW4e_sampler_address_mode@1@00@Z
void c_rasterizer::set_sampler_address_mode(long sampler, e_sampler_address_mode u, e_sampler_address_mode v, e_sampler_address_mode w)
{
  long packed = (((u << 4) | v) << 4) | w;
  if (g_rasterizer_sampler_address_modes[sampler] != packed && sampler >= 0 && (unsigned long)sampler < 26) {
    g_rasterizer_sampler_address_modes[sampler] = packed;
    c_rasterizer::get_device()->SetSamplerState(sampler, D3DSAMP_ADDRESSU, k_d3d_texture_address_modes[u]);
    c_rasterizer::get_device()->SetSamplerState(sampler, D3DSAMP_ADDRESSV, k_d3d_texture_address_modes[v]);
    c_rasterizer::get_device()->SetSamplerState(sampler, D3DSAMP_ADDRESSW, k_d3d_texture_address_modes[w]);
  }
}

// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
extern "C" void rasterizer_device_set_sampler_filter_modes(D3DDevice *device, long sampler, long anisotropy_mode, long filter_mode)
{
    // match: 0x195e88: callers supply modes 0..7. Without the hint, cmplwi and bgt repeat the range check before the jump table.
    __assume((unsigned long)filter_mode <= 7);
    switch (filter_mode)
    {
    case c_rasterizer::_sampler_filter_mode_trilinear:
        device->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(sampler, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        break;
    case c_rasterizer::_sampler_filter_mode_point:
        device->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        device->SetSamplerState(sampler, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        device->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        device->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
        device->SetSamplerState(sampler, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(sampler, D3DSAMP_MAXANISOTROPY, anisotropy_mode - 2);
        break;
    case c_rasterizer::_sampler_filter_mode_bilinear:
        device->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(sampler, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        break;
    case 7:
        device->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(sampler, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        device->SetSamplerState(sampler, D3DSAMP_SEPARATEZFILTERENABLE, TRUE);
        device->SetSamplerState(sampler, D3DSAMP_MAGFILTERZ, D3DTEXF_POINT);
        device->SetSamplerState(sampler, D3DSAMP_MINFILTERZ, D3DTEXF_POINT);
        break;
    }
}

// @address 0x167cc8 ?set_sampler_filter_mode@c_rasterizer@@SAXJW4e_sampler_filter_mode@1@@Z
void c_rasterizer::set_sampler_filter_mode(long sampler, e_sampler_filter_mode filter_mode)
{
  if (g_rasterizer_sampler_filter_modes[sampler] != filter_mode) {
    if (sampler >= 0 && (unsigned long)sampler < 26)
      g_rasterizer_sampler_filter_modes[sampler] = filter_mode;
    if ((unsigned long)filter_mode <= 7)
      rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), sampler, filter_mode, filter_mode);
  }
}

// @address 0x16d438 ?set_explicit_shaders@c_rasterizer@@SA_NJW4e_vertex_type@@W4e_transfer_vector_vertex_types@@W4e_entry_point@@@Z
bool c_rasterizer::set_explicit_shaders(long explicit_shader_index, e_vertex_type vertex_type,
    e_transfer_vector_vertex_types transfer_vector_vertex_type, e_entry_point entry_point)
{
    bool result = false;
    if (global_game_globals) {
        long group_index = global_game_globals->rasterizer_globals_ref.index;
        c_rasterizer_globals *group;
        if (group_index != -1)
            group = TAG_GET('rasg', c_rasterizer_globals, group_index);
        else
            group = 0;
        const c_rasterizer_globals::s_explicit_shader *entry = group->get_explicit_shaders(explicit_shader_index);
        if (entry->vertex.index != -1 && entry->pixel.index != -1) {
            const c_rasterizer_vertex_shader *vertex_shader = TAG_GET('vtsh', c_rasterizer_vertex_shader, entry->vertex.index);
            const c_rasterizer_pixel_shader *pixel_shader = TAG_GET('pixl', c_rasterizer_pixel_shader, entry->pixel.index);
            bool first = c_rasterizer::set_vertex_shader(
                vertex_shader, vertex_type, _transfer_vector_vertex_none, entry_point);
            bool second = c_rasterizer::set_pixel_shader(pixel_shader, entry_point);
            result = second & first;
        }
    }
    g_render_method_submit_cache = k_default_render_method_submit_cache;
    return result;
}

static __forceinline void rasterizer_states_set_z_buffer_off()
{
    float scale = 1.0f;
    union { float as_float; unsigned long as_dword; } slope_scale_bias = { g_rasterizer_depth_bias };
    union { float as_float; unsigned long as_dword; } depth_bias = { g_rasterizer_depth_bias };
    if (g_rasterizer_use_floating_point_z_buffer)
        scale = -1.0f;
    if (g_rasterizer_current_z_buffer_mode != 2 || g_rasterizer_use_floating_point_z_buffer != g_rasterizer_current_z_buffer_floating_point) {
        g_rasterizer_current_z_buffer_mode = 2;
        g_rasterizer_current_z_buffer_floating_point = g_rasterizer_use_floating_point_z_buffer;
        D3DDevice_SetRenderState_ZEnable(c_rasterizer::get_device(), 0);
        slope_scale_bias.as_float *= scale;
        depth_bias.as_float *= scale;
        D3DDevice_SetRenderState_SlopeScaleDepthBias(c_rasterizer::get_device(), slope_scale_bias.as_dword);
        D3DDevice_SetRenderState_DepthBias(c_rasterizer::get_device(), depth_bias.as_dword);
    }
}


// @address 0x192020 rasterizer_draw_quads
void rasterizer_draw_quads(const void *vertices, long quad_count)
{
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), D3DCULL_NONE);
    rasterizer_states_set_z_buffer_off();
    D3DDevice_SetIndices(c_rasterizer::get_device(), 0);
    D3DDevice_DrawVerticesUP(c_rasterizer::get_device(), D3DPT_QUADLIST, quad_count * 4, vertices, 20);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), D3DCULL_CW);
}


// @address 0x196118 rasterizer_set_device_alpha_blend_mode
extern "C" void rasterizer_set_device_alpha_blend_mode(D3DDevice *device, long alpha_blend_mode)
{
    switch (alpha_blend_mode)
    {
    case c_rasterizer::_alpha_blend_opaque:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        break;
    case c_rasterizer::_alpha_blend_additive:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        break;
    case c_rasterizer::_alpha_blend_multiply:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
        break;
    case c_rasterizer::_alpha_blend_alpha_blend:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        break;
    case c_rasterizer::_alpha_blend_inv_alpha_blend:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
        break;
    case c_rasterizer::_alpha_blend_double_multiply:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
        break;
    case c_rasterizer::_alpha_blend_pre_multiplied_alpha:
    case c_rasterizer::_alpha_blend_multiply_add:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        break;
    case c_rasterizer::_alpha_blend_maximum:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_MAX);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        break;
    case c_rasterizer::_alpha_blend_add_src_times_dstalpha:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTALPHA);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        break;
    case c_rasterizer::_alpha_blend_add_src_times_srcalpha:
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        break;
    case c_rasterizer::_alpha_blend_motion_blur_static:
    {
        D3DBLENDSTATE blend_state = { D3DBLEND_ONE, D3DBLENDOP_ADD, D3DBLEND_ZERO,
            D3DBLEND_CONSTANTALPHA, D3DBLENDOP_ADD, D3DBLEND_ZERO };
        c_rasterizer::get_device()->SetRenderState(D3DRS_BLENDFACTOR, 0);
        c_rasterizer::get_device()->SetBlendState(0, blend_state);
        break;
    }
    case c_rasterizer::_alpha_blend_motion_blur_inhibit:
    {
        D3DBLENDSTATE blend_state = { D3DBLEND_ONE, D3DBLENDOP_ADD, D3DBLEND_ZERO,
            D3DBLEND_CONSTANTALPHA, D3DBLENDOP_ADD, D3DBLEND_ZERO };
        c_rasterizer::get_device()->SetRenderState(D3DRS_BLENDFACTOR, -1);
        c_rasterizer::get_device()->SetBlendState(0, blend_state);
        break;
    }
    }
}

// @address 0x20eca0 ?set_pixel_shader_constant@c_rasterizer@@SAXJJPBTreal_vector4d@@@Z
void c_rasterizer::set_pixel_shader_constant(long _sym_p0, long _sym_p1, union real_vector4d const * _sym_p2)
{
  c_rasterizer::get_device()->SetPixelShaderConstantF(((UINT)_sym_p0), ((const float *)_sym_p2), ((DWORD)_sym_p1));
}

// @address 0x448910 rasterizer_rebuild_hierarchical_z_buffer
void rasterizer_rebuild_hierarchical_z_buffer(void)
{
  c_rasterizer::set_explicit_shaders(0x33, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default);
  g_render_method_submit_cache = k_default_render_method_submit_cache;
  D3DDevice_SetPixelShader(c_rasterizer::get_device(),0);
  D3DDevice_SetRenderState_ZEnable(c_rasterizer::get_device(),1);
  D3DDevice_SetRenderState_ZWriteEnable(c_rasterizer::get_device(),1);
  D3DDevice_SetRenderState_ZFunc(c_rasterizer::get_device(),0);
  D3DDevice_SetRenderState_HiZEnable(c_rasterizer::get_device(),0);
  D3DDevice_SetRenderState_HiZWriteEnable(c_rasterizer::get_device(),1);
  c_rasterizer::draw_fullscreen_quad();
  D3DDevice_SetRenderState_HiZEnable(c_rasterizer::get_device(),2);
  D3DDevice_SetRenderState_HiZWriteEnable(c_rasterizer::get_device(),2);
}


// @address 0x448e38 rasterizer_set_stencil_mode_with_ref
void rasterizer_set_stencil_mode_with_ref(c_rasterizer::e_stencil_mode mode, unsigned char reference)
{
    if (mode != g_rasterizer_stencil_mode) {
        g_rasterizer_stencil_mode = mode;
        g_rasterizer_stencil_ref = reference;
        if (mode != c_rasterizer::_stencil_mode_decal_write) {
            if (mode != c_rasterizer::_stencil_mode_decal_read) {
                if (mode == c_rasterizer::_stencil_mode_shadow_generate_hi_ref) {
                    D3DDevice_SetRenderState_StencilEnable(c_rasterizer::get_device(), 1);
                    D3DDevice_SetRenderState_StencilFunc(c_rasterizer::get_device(), 6);
                    D3DDevice_SetRenderState_StencilMask(c_rasterizer::get_device(), 31);
                    D3DDevice_SetRenderState_StencilWriteMask(c_rasterizer::get_device(), 224);
                    D3DDevice_SetRenderState_StencilPass(c_rasterizer::get_device(), 2);
                    D3DDevice_SetRenderState_StencilFail(c_rasterizer::get_device(), 0);
                    D3DDevice_SetRenderState_StencilZFail(c_rasterizer::get_device(), 0);
                    D3DDevice_SetRenderState_HiStencilEnable(c_rasterizer::get_device(), 0);
                    D3DDevice_SetRenderState_HiStencilWriteEnable(c_rasterizer::get_device(), 0);
                }
            } else {
                D3DDevice_SetRenderState_StencilEnable(c_rasterizer::get_device(), 1);
                D3DDevice_SetRenderState_StencilFunc(c_rasterizer::get_device(), 2);
                D3DDevice_SetRenderState_StencilRef(c_rasterizer::get_device(), reference);
                D3DDevice_SetRenderState_StencilZFail(c_rasterizer::get_device(), 0);
                D3DDevice_SetRenderState_StencilPass(c_rasterizer::get_device(), 0);
                D3DDevice_SetRenderState_StencilFail(c_rasterizer::get_device(), 0);
                D3DDevice_SetRenderState_StencilMask(c_rasterizer::get_device(), 255);
                D3DDevice_SetRenderState_StencilWriteMask(c_rasterizer::get_device(), 0);
                D3DDevice_SetRenderState_HiStencilEnable(c_rasterizer::get_device(), 0);
                D3DDevice_SetRenderState_HiStencilWriteEnable(c_rasterizer::get_device(), 0);
            }
        } else {
            D3DDevice_SetRenderState_StencilEnable(c_rasterizer::get_device(), 1);
            D3DDevice_SetRenderState_StencilFunc(c_rasterizer::get_device(), 7);
            D3DDevice_SetRenderState_StencilRef(c_rasterizer::get_device(), reference);
            D3DDevice_SetRenderState_StencilZFail(c_rasterizer::get_device(), 0);
            D3DDevice_SetRenderState_StencilPass(c_rasterizer::get_device(), 2);
            D3DDevice_SetRenderState_StencilFail(c_rasterizer::get_device(), 2);
            D3DDevice_SetRenderState_StencilMask(c_rasterizer::get_device(), 255);
            D3DDevice_SetRenderState_StencilWriteMask(c_rasterizer::get_device(), 255);
            D3DDevice_SetRenderState_HiStencilEnable(c_rasterizer::get_device(), 0);
            D3DDevice_SetRenderState_HiStencilWriteEnable(c_rasterizer::get_device(), 0);
        }
        D3DDevice_SetRenderState_StencilRef(c_rasterizer::get_device(), reference);
    } else if (reference != g_rasterizer_stencil_ref) {
        g_rasterizer_stencil_ref = reference;
        D3DDevice_SetRenderState_StencilRef(c_rasterizer::get_device(), reference);
    }
}


// @address 0x449068 rasterizer_setup_distortion_targets
void rasterizer_setup_distortion_targets(const rectangle2d *bounds, unsigned char special)
{
    float one = 1.0f;
    if (special) {
        c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_depth_stencil_multisample);
    } else {
        c_rasterizer::set_depth_stencil_surface(c_rasterizer::_surface_none);
        rasterizer_states_set_z_buffer_off();
    }
    c_rasterizer::set_render_target(0, c_rasterizer::_surface_distortion);
    c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
    c_rasterizer::set_render_target(2, c_rasterizer::_surface_none);
    c_rasterizer::set_render_target(3, c_rasterizer::_surface_none);
    real_vector4d viewport;
    viewport.i = 16.0f;
    viewport.j = 16.0f;
    viewport.k = 0.0f;
    viewport.l = 0.0f;
    c_rasterizer::clearf(1, &viewport, 0.0f, 0);
    c_rasterizer::set_viewport(*bounds, 0.0f, one);
    c_rasterizer::set_scissor_rect(bounds);
}

// @address 0x2a3ef8 rasterizer_resolve_accumulation_targets
extern "C" void rasterizer_resolve_accumulation_targets(const s_rasterizer_screenshot_context *context)
{
    c_object_iterator<object_datum> iterator;
    iterator.begin(0, 0);
    const bool &has_objects = iterator.next();
    if (has_objects) {
        long *index_table = g_object_render_data;
        do {
            const unsigned short &absolute_index = static_cast<unsigned short>(iterator.get_index());
            const long &cache_index = static_cast<long>(index_table[absolute_index]);
            if (cache_index != -1) {
                const unsigned short &entry = static_cast<unsigned short>(cache_index);
                // Preserve the unsigned all-bits-set cache invalidation store.
                ((s_visibility_cached_object_render_state *)g_cached_object_render_states->data)
                    [(unsigned short)entry].shader_info.frame_index = -1;
            }
        } while (iterator.next());
    }
    void *state = context->view;
    rasterizer_main_memory_globals *memory = rasterizer_memory_get_globals();
    memory->current_offset = 0;
    memory->GPU_frontend_remaining = 98304.0f;
    memory->maximum_offset = 0;
    memory->allocation_bytes = 0;
    player_view_render_frame(static_cast<c_player_view *>(state), 0);
    c_rasterizer::g_HDR_alpha_blend = 0;
    c_rasterizer::set_render_target(0, c_rasterizer::_surface_accum_HDR);
    cinematic_render(1, 0);
    c_rasterizer::g_HDR_alpha_blend = 0;
    c_rasterizer::set_render_target(0, c_rasterizer::_surface_accum_LDR);
    c_rasterizer::g_HDR_alpha_blend = 0;
    c_rasterizer::set_render_target(1, c_rasterizer::_surface_accum_HDR);
    if (rasterizer_tiling_get_state()->disabled != 0) {
        D3DDevice_BlockUntilIdle(c_rasterizer::get_device());
        c_rasterizer::g_HDR_alpha_blend = 0;
        c_rasterizer::resolve_surface(c_rasterizer::_surface_accum_LDR, 0, 0, 0, 0);
    } else {
        c_rasterizer::g_HDR_alpha_blend = 0;
        c_rasterizer::resolve_surface(c_rasterizer::_surface_accum_LDR, 0, 0, 0, 0);
        c_rasterizer::g_HDR_alpha_blend = 0;
        c_rasterizer::resolve_surface(c_rasterizer::_surface_accum_HDR, 1, 0, 0, 0);
    }
}

// @address 0x81dab4 g_rasterizer_separate_alpha_blend_mode

// @address 0x8c46bc g_rasterizer_use_floating_point_z_buffer
