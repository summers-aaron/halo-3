// objectslibxbox/emblems.obj: from the records 0x3672a0 0x452b00
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
// @address 0x3672a0 ?emblem_set_render_constants_for_emblem@@YA_NPIBUemblem_info@@@Z
#define XVIDEO_MODE XDK_XVIDEO_MODE
#include <xtl.h>
#undef XVIDEO_MODE
#include <ppcintrinsics.h>
#include "objects/emblems.h"
#include "objects/emblem_info.h"
#include "rasterizer/rasterizer_main.h"
#include "game/players.h"
#include "rasterizer/rasterizer_states.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_vertex_screen_types.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "rasterizer/rasterizer_dynamic_render_targets.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes


// Zeroed 8-byte object: the emblem setter writes all 8 bytes; the object and
// render-method function evaluators read the first 6 back through this object.
// @address 0x8c4440 g_emblem_render_info
extern "C" { emblem_info g_emblem_render_info; }
struct fRGBA {
    float red, green, blue, alpha;
    __forceinline fRGBA() {}
    __forceinline fRGBA(real_rgb_color color, float a) : red(color.red), green(color.green), blue(color.blue), alpha(a) {}
};


static __forceinline real_rgb_color emblems_player_color(long index) {
    real_rgb_color color;
    return *player_profile_get_rgb_color(&color, index);
}
static __forceinline void emblems_to_vector(const fRGBA *color, real_vector4d *vector) {
    vector->i = color->red; vector->j = color->green; vector->k = color->blue; vector->w = color->alpha;
}
static __forceinline bool emblems_test(const unsigned char *flags, long bit) {
    bool result = (*flags & (1 << bit)) != 0;
    return result;
}
// match: 0x3672a0: __restrict keeps r9/r10 with flags lbz between ld and std to g_emblem_render_info; without it regs recolor r10/r11 and std hoists above lbz
bool emblem_set_render_constants_for_emblem(const emblem_info *__restrict info) {
    g_emblem_render_info = *info;
    bool background = !emblems_test(&info->flags, 0);
    bool primary = emblems_test(&info->flags, 1);
    bool secondary = emblems_test(&info->flags, 2);
    fRGBA color;
    color = fRGBA(emblems_player_color(info->background_color), background ? 1.0f : 0.0f);
    real_vector4d background_constant;
    emblems_to_vector(&color, &background_constant);
    color = fRGBA(emblems_player_color(info->primary_color), primary ? 1.0f : 0.0f);
    real_vector4d primary_constant;
    emblems_to_vector(&color, &primary_constant);
    color = fRGBA(emblems_player_color(info->secondary_color), secondary ? 1.0f : 0.0f);
    real_vector4d secondary_constant;
    emblems_to_vector(&color, &secondary_constant);
    c_rasterizer::get_device()->SetPixelShaderConstantF(55, background_constant.n, 1);
    c_rasterizer::get_device()->SetPixelShaderConstantF(56, primary_constant.n, 1);
    c_rasterizer::get_device()->SetPixelShaderConstantF(57, secondary_constant.n, 1);
    return true;
}

// @address 0x836908 g_rasterizer_shared_shader_cache
// @address 0x452b00 emblem_draw_screen_quad
extern "C" void emblem_draw_screen_quad(void *loading_screen, const real_point2d *position, const real_point2d *anchor, D3DTexture *texture, float scale)
{
    c_rasterizer::get_device()->SetTexture(0, texture);
    if (g_rasterizer_sampler_filter_modes[0] != 2)
    {
        g_rasterizer_sampler_filter_modes[0] = 2;
        rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 0, 2, 2);
    }
    c_rasterizer::set_sampler_address_mode(
        0, c_rasterizer::_sampler_address_border, c_rasterizer::_sampler_address_border, c_rasterizer::_sampler_address_clamp);
    c_rasterizer::get_device()->SetTexture(1, NULL);
    c_rasterizer::get_device()->SetTexture(2, NULL);
    D3DDevice_SetPixelShader(c_rasterizer::get_device(), g_rasterizer_shared_shader_cache.pixel_shaders[5]);
    D3DDevice_SetVertexShader(c_rasterizer::get_device(), g_rasterizer_shared_shader_cache.vertex_shaders[5]);
    float constants[4];
    constants[0] = 1.0f;
    constants[1] = 1.0f;
    constants[2] = 1.0f;
    constants[3] = 1.0f;
    c_rasterizer::get_device()->SetPixelShaderConstantF(1, constants, 1);
    if (c_rasterizer::g_current_alpha_blend_mode != c_rasterizer::_alpha_blend_additive)
    {
        c_rasterizer::g_current_alpha_blend_mode = c_rasterizer::_alpha_blend_additive;
        D3DDevice *device = c_rasterizer::get_device();
        D3DDevice_SetRenderState_AlphaBlendEnable(device, TRUE);
        D3DDevice_SetRenderState_BlendOp(device, D3DBLENDOP_ADD);
        D3DDevice_SetRenderState_SrcBlend(device, D3DBLEND_ONE);
        D3DDevice_SetRenderState_DestBlend(device, D3DBLEND_ONE);
    }
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), D3DCULL_NONE);
    D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(),
        rasterizer_get_vertex_declaration(_vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default));
    D3DSURFACE_DESC desc;
    D3DTexture_GetLevelDesc(texture, 0, &desc);
    float width = (float)desc.Width;
    float height = (float)desc.Height;
    float screen_width;
    float screen_height;
    if (rasterizer_main_get_video_mode()->fIsWideScreen)
    {
        screen_width = 1280.0f;
        screen_height = 720.0f;
    }
    else
    {
        screen_width = 640.0f;
        screen_height = 480.0f;
    }
    float scale_x = width / screen_width * scale;
    float scale_y = height / screen_height * scale;
    const real_point2d *const *anchor_reference = &anchor;
    const real_point2d *const *position_reference = &position;
    float x0 = (*position_reference)->x - (*anchor_reference)->x * scale_x;
    float y0 = (*position_reference)->y - (*anchor_reference)->y * scale_y;
    float x1 = (float)((1.0 - (*anchor_reference)->x) * scale_x + (*position_reference)->x);
    float y1 = (float)((1.0 - (*anchor_reference)->y) * scale_y + (*position_reference)->y);
    BOOL disabled1 = FALSE;
    BOOL disabled0 = FALSE;
    rasterizer_vertex_screen vertices[4];
    vertices[0].x = x0;
    vertices[0].y = y0;
    vertices[0].u = 0.0f;
    vertices[0].v = 1.0f;
    vertices[1].x = x1;
    vertices[1].y = y0;
    vertices[1].u = 1.0f;
    vertices[1].v = 1.0f;
    vertices[2].x = x1;
    vertices[2].y = y1;
    vertices[2].u = 1.0f;
    vertices[2].v = 0.0f;
    vertices[3].x = x0;
    vertices[3].y = y1;
    vertices[3].u = 0.0f;
    vertices[3].v = 0.0f;
    D3DDevice_SetPixelShaderConstantB(c_rasterizer::get_device(), 4, &disabled1, 1);
    D3DDevice_SetPixelShaderConstantB(c_rasterizer::get_device(), 5, &disabled0, 1);
    rasterizer_draw_quads(vertices, 1);
}
