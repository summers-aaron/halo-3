// graphicslibxbox/rasterizer_xenon_main.obj: from the records 0x1469e0 0x147490 0x14c1f0 0x167f60 0x168d98 0x16cec0 0x16d1c0 0x16dc58 0x16dcb0 0x16ec10 0x192880 0x195780 0x447cd8 0x447da8 0x447f50 0x447fe0 0x4482a0 0x4482a8 0x448640 0x4486b8 0x448728 0x449000 0x44eb70 0x456cc8 0x456e20 0x456f00 0x457040 0x4571d8 0x4572c8
#include "render/render_mesh.h"
#include "rasterizer/render_surfaces_interface.h"
#include "rasterizer/rasterizer_render_target_types.h"
#include "rasterizer/xenon/rasterizer_xenon_main_imports.h"
#include "rasterizer/xenon/rasterizer_xenon_main_globals.h"
#include "rasterizer/rasterizer_shader_gpr_globals.h"
#include "rasterizer/rasterizer_device_globals.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "math/real_math_types.h"
#include "math/real_constant_pool_access.h"
#include <string.h>
#include <new>
#include <xtl.h>
#include "rasterizer/xenon_d3d_library.h"
#include <math.h>
#include "cseries/cseries.h"
#include "cseries/runtime_state.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "rasterizer/xenon/rasterizer_xenon_main.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "cutscene/cinematics.h"
#include "networking/online/online_files.h"
#include "main/game_preferences.h"
#include "tag_files/file_reference.h"
#include "rasterizer/xenon/rasterizer_xenon_memory_request_types.h"
#include "saved_games/buffered_file_writer_types.h"
#include "saved_games/saved_game_files_io.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "memory/physical_memory_map.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_LUT_control.h"
#include "multithreading/synchronized_value.h"
#include "multithreading/thread_tls.h"
#include "rasterizer/xenon/rasterizer_xenon_memory_types.h"
#include "rasterizer/xenon/rasterizer_xenon_device_info_types.h"
#include "bitmaps/dds_header.h"

class c_rasterizer_xenon_memory;

struct s_resolve_short_rectangle;

struct presentation_parameters;

struct s_rasterizer_screenshot_file_writer {
    s_buffered_file_writer writer;
    char buffer[4096];
};

struct s_rasterizer_xenon_main_rgb16 {
    unsigned short red, green, blue;
};

// @address 0x8c46b4 ?g_tiling_configuration@c_rasterizer@@2W4e_tiling_configuration@1@A
// @flags /TP /O2 /Oc
// @address 0x254f8 k_rasterizer_tiling_settings
// @rdata 0x820254f8..0x82025888
extern "C" {
const s_rasterizer_tiling_settings k_rasterizer_tiling_settings[][6] = {{
    { 1152, 640, 1152, 640, 1, 0, {
        { 0, 0, 1152, 640 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    } },
    { 1152, 640, 1152, 320, 2, 1, {
        { 0, 0, 1152, 320 },
        { 0, 320, 1152, 640 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    } },
    { 1152, 640, 1152, 160, 4, 2, {
        { 0, 0, 1152, 160 },
        { 0, 160, 1152, 320 },
        { 0, 320, 1152, 480 },
        { 0, 480, 1152, 640 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    } },
    { 1152, 320, 1152, 320, 1, 1, {
        { 0, 0, 1152, 320 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    } },
    { 576, 320, 576, 320, 1, 2, {
        { 0, 0, 576, 320 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    } },
    { 864, 320, 864, 320, 1, 1, {
        { 0, 0, 864, 320 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    } },
}};
}
// @rdata-end
// @address 0x8c446e g_rasterizer_requested_tiling_enabled
extern "C" {
unsigned char g_rasterizer_requested_tiling_enabled;
}
// @address 0x5a6518 D3DDevice_GetRenderTarget
// @address 0x5a6560 D3DDevice_GetDepthStencilSurface
// @address 0x5a6780 D3DDevice_SetSurfaces
// @address 0x5a7618 D3DResource_Release
// @address 0x5a65a0 D3DDevice_SetGammaRamp
// @address 0x5a6668 D3DDevice_SetPWLGamma


// @address 0x73a278 g_trilinear_threshold
extern "C" {
unsigned long g_trilinear_threshold = 3;
}
// @address 0x73a27c g_rasterizer_present_immediate_threshold
extern "C" {
unsigned long g_rasterizer_present_immediate_threshold = 10;
}
// @address 0x81dacc g_last_skinning_matrix_count
extern "C" {
long g_last_skinning_matrix_count;
}
// @address 0x586e8 k_rasterizer_gamma_decode_exponent
extern "C" {
const double k_rasterizer_gamma_decode_exponent = (double)0.45454544f;
}



// @address 0x8c46c8 g_rasterizer_max_vs_gprs
// @address 0x8c46cc g_rasterizer_max_ps_gprs
#if !defined(_XBOX) || defined(__clang__)
// The native Xbox link places these slots in the geometry unit's shared BSS.
extern "C" {
unsigned long g_rasterizer_max_vs_gprs;
unsigned long g_rasterizer_max_ps_gprs;
}
#endif
// @address 0x8c46b8 g_antialiasing_mode
// @address 0x8c46ae g_rasterizer_tiling_enabled
extern "C" {
unsigned char g_rasterizer_tiling_enabled;
}


// @address 0x8c46b0 ?g_screen_resolution@c_rasterizer@@2W4e_screen_resolution@1@A


inline const s_rasterizer_tiling_settings *c_rasterizer::get_tiling_settings(long screen_resolution, long tiling_configuration)
{
#if !defined(_XBOX) || defined(__clang__)
    // PC (port/GAME-CHANGES.md, "Custom resolutions"): the one table at the render size, rasterizer_render_targets.cpp's
    return rasterizer_xenon_main_get_tiling_settings(screen_resolution, tiling_configuration);
#else
    return &k_rasterizer_tiling_settings[screen_resolution][tiling_configuration];
#endif
}

#if !defined(_XBOX) || defined(__clang__)
static void rasterizer_xenon_main_check_live_resolution(D3DDevice *device);
#endif

bool c_rasterizer::begin_frame()
{
#if !defined(_XBOX) || defined(__clang__)
    rasterizer_xenon_main_check_live_resolution(g_device);
#endif
    // Keep the trilinear threshold addressable by the external globals table.
    (void)&g_trilinear_threshold;
    g_device->SetRenderState(D3DRS_PRESENTIMMEDIATETHRESHOLD, g_rasterizer_present_immediate_threshold);
    if (g_trilinear_threshold <= 3) {
        for (long sampler = 0; sampler < 26; sampler++)
            g_device->SetSamplerState(sampler, D3DSAMP_TRILINEARTHRESHOLD, g_trilinear_threshold);
    }
    g_last_skinning_matrix_address = 0;
    g_last_skinning_matrix_count = 0;
    return true;
}

void c_rasterizer::begin_high_quality_blend()
{
    g_device->SetRenderState(D3DRS_HIGHPRECISIONBLENDENABLE, TRUE);
    g_device->SetRenderState(D3DRS_HIGHPRECISIONBLENDENABLE1, TRUE);
    g_device->SetRenderState(D3DRS_HIGHPRECISIONBLENDENABLE2, TRUE);
    g_device->SetRenderState(D3DRS_HIGHPRECISIONBLENDENABLE3, TRUE);
}

// @address 0x168d98 ?clearf@c_rasterizer@@SAXKPBTreal_vector4d@@MK@Z
void c_rasterizer::clearf(unsigned long flags, const real_vector4d *color, float z, unsigned long stencil)
{
    if (m_use_floating_point_z_buffer)
        z = real_constant_pool_one() - z;
    D3DVECTOR4 clear_color;
    clear_color.x = color->i;
    clear_color.y = color->j;
    clear_color.z = color->k;
    clear_color.w = color->l;
    g_device->ClearF(flags, NULL, &clear_color, z, stencil);
}

// @address 0x16d1c0 ?get_is_tiling_enabled@c_rasterizer@@SA_NXZ
bool c_rasterizer::get_is_tiling_enabled()
{
    if (g_antialiasing_mode != 0)
        return false;
    return get_tiling_settings(c_rasterizer::g_screen_resolution,
        c_rasterizer::g_tiling_configuration)->tile_count > !g_rasterizer_tiling_enabled;
}

void c_rasterizer::end_high_quality_blend()
{
    g_device->SetRenderState(D3DRS_HIGHPRECISIONBLENDENABLE, FALSE);
    g_device->SetRenderState(D3DRS_HIGHPRECISIONBLENDENABLE1, FALSE);
    g_device->SetRenderState(D3DRS_HIGHPRECISIONBLENDENABLE2, FALSE);
    g_device->SetRenderState(D3DRS_HIGHPRECISIONBLENDENABLE3, FALSE);
}


// @address 0x16dcb0 ?refresh_tiling_render_targets@c_rasterizer@@SAXXZ
void c_rasterizer::refresh_tiling_render_targets()
{
    if (!get_is_tiling_enabled())
        return;
    D3DSurface *render_targets[5];
    render_targets[0] = D3DDevice_GetDepthStencilSurface(g_device);
    render_targets[1] = D3DDevice_GetRenderTarget(g_device, 0);
    render_targets[2] = D3DDevice_GetRenderTarget(g_device, 1);
    render_targets[3] = D3DDevice_GetRenderTarget(g_device, 2);
    render_targets[4] = D3DDevice_GetRenderTarget(g_device, 3);
    D3DDevice_SetSurfaces(g_device, (const D3DSURFACES *)render_targets, 1);
    if (render_targets[0])
        D3DResource_Release((D3DResource *)render_targets[0]);
    if (render_targets[1])
        D3DResource_Release((D3DResource *)render_targets[1]);
    if (render_targets[2])
        D3DResource_Release((D3DResource *)render_targets[2]);
    if (render_targets[3])
        D3DResource_Release((D3DResource *)render_targets[3]);
    if (render_targets[4])
        D3DResource_Release((D3DResource *)render_targets[4]);
}

// @address 0x16ec10 ?clear@c_rasterizer@@SAXKKME@Z
void c_rasterizer::clear(unsigned long flags, unsigned long color, float z, unsigned char stencil)
{
    if (flags & D3DCLEAR_STENCIL) {
        g_device->SetRenderState(D3DRS_HISTENCILENABLE, FALSE);
        g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, TRUE);
        g_device->SetRenderState(D3DRS_HISTENCILREF, 0);
        g_device->SetRenderState(D3DRS_HISTENCILFUNC, 1);
        g_device->FlushHiZStencil(D3DFHZS_ASYNCHRONOUS);
    }
    if (m_use_floating_point_z_buffer)
        z = real_constant_pool_one() - z;
    g_device->Clear(0, NULL, flags, color, z, 0);
    g_device->SetRenderState(D3DRS_HISTENCILWRITEENABLE, FALSE);
}




// @address 0x7bb578 g_rasterizer_lut_desired
extern "C" {
c_rasterizer_LUT_control g_rasterizer_lut_desired =
    { _gamma_ramp_7e3, 2.2f, 1.95f, 2, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
}
// @address 0x7bb5a0 g_rasterizer_lut_current
extern "C" {
c_rasterizer_LUT_control g_rasterizer_lut_current =
    { (e_gamma_ramp_type)NONE, 0.0f, 0.0f, 2, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
}




static __forceinline c_render_surface *rasterizer_xenon_main_get_render_surface(c_rasterizer::e_surface surface)
{
    return g_rasterizer_surface_groups[surface].get_specialized_surface(surface);
}

static __forceinline D3DTexture *rasterizer_xenon_main_get_d3d_texture(c_render_surface *surface)
{
    return (D3DTexture *)surface->resource;
}

static __forceinline void rasterizer_xenon_main_reset_gprs()
{
    if (g_rasterizer_gpr_allocation != 0)
    {
        (*rasterizer_shader_gpr_get_globals())[0] = 0;
        (*rasterizer_shader_gpr_get_globals())[1] = 0;
        c_rasterizer::get_device()->SetShaderGPRAllocation(0, 0, 0);
        g_rasterizer_gpr_allocation = 0;
        unsigned long flags;
        c_rasterizer::get_device()->GetShaderGPRAllocation(&flags, &(*rasterizer_shader_gpr_get_globals())[0], &(*rasterizer_shader_gpr_get_globals())[1]);
    }
}

// PC (port/GAME-CHANGES.md, "In-game Xbox Guide"): the Xbox Guide, drawn over the finished frame the way the
// console's system UI is composited over the title's (port/probe/extra/guide_ingame_ui.cpp,
// port/sdk/xam/guide_ingame.h). Nothing when it is closed.
extern "C" void guide_ingame_ui_render(void);

// @address 0x192880 ?end_frame@c_rasterizer@@SA_NXZ
bool c_rasterizer::end_frame()
{
    if (c_render_surfaces_interface::initialized())
    {
        e_surface surface = _surface_display;
        c_render_surface *render_surface = rasterizer_xenon_main_get_render_surface(surface);
        D3DTexture *front_buffer = rasterizer_xenon_main_get_d3d_texture(render_surface);
        guide_ingame_ui_render();
        g_device->SynchronizeToPresentationInterval();
        resolve_surface(surface, 0, NULL, 0, 0);
        rasterizer_xenon_main_reset_gprs();
        g_device->Swap(front_buffer, NULL);
    }
    else
    {
        rasterizer_xenon_main_reset_gprs();
        g_device->Present(NULL, NULL, NULL, NULL);
    }
    g_rasterizer_frame_has_ended = true;
    set_DAC_LUT_curve();
    return true;
}

// @address 0x195780 ?set_DAC_LUT_curve@c_rasterizer@@SAXXZ
void c_rasterizer::set_DAC_LUT_curve()
{
    D3DGAMMARAMP ramp;
    D3DPWLGAMMA pwl;
    g_rasterizer_lut_desired.m_setting = global_preferences_get_brightness_setting();
    bool same = g_rasterizer_lut_desired.m_type == g_rasterizer_lut_current.m_type &&
        g_rasterizer_lut_desired.m_brightness == g_rasterizer_lut_current.m_brightness &&
        g_rasterizer_lut_desired.m_gamma == g_rasterizer_lut_current.m_gamma &&
        g_rasterizer_lut_desired.m_setting == g_rasterizer_lut_current.m_setting &&
        g_rasterizer_lut_desired.m_black.red == g_rasterizer_lut_current.m_black.red &&
        g_rasterizer_lut_desired.m_black.green == g_rasterizer_lut_current.m_black.green &&
        g_rasterizer_lut_desired.m_black.blue == g_rasterizer_lut_current.m_black.blue &&
        g_rasterizer_lut_desired.m_white.red == g_rasterizer_lut_current.m_white.red &&
        g_rasterizer_lut_desired.m_white.green == g_rasterizer_lut_current.m_white.green &&
        g_rasterizer_lut_desired.m_white.blue == g_rasterizer_lut_current.m_white.blue;
    if (!same)
    {
        g_rasterizer_lut_current = g_rasterizer_lut_desired;
        c_render_surface *surface = g_rasterizer_surface_groups[1].get_specialized_surface((c_rasterizer::e_surface)1);
        unsigned int format = surface->description.format & 63;
        if (format == 6)
        {
            g_rasterizer_lut_desired.compute_LUT(ramp);
            D3DDevice_SetGammaRamp(g_device, 0, &ramp);
        }
        else
        {
            g_rasterizer_lut_desired.compute_LUT(pwl);
            D3DDevice_SetPWLGamma(g_device, 0, &pwl);
        }
    }
}

// @address 0x8368cc g_command_buffer_device
extern "C" {
D3DDevice *g_command_buffer_device;
}



c_rasterizer_xenon_memory g_rasterizer_xenon_memory;


// @address 0x81dabc g_rasterizer_xenon_device_info
// @address 0x8c485c g_d3d_resource_allocator_device
// @address 0x8c4690 g_rasterizer_direct3d
extern "C" {
Direct3D *g_rasterizer_direct3d;
}

#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_rasterizer_xenon_device_info g_rasterizer_xenon_device_info;
}
#endif


static __forceinline unsigned int rasterizer_xenon_main_lower(unsigned int value)
{
    return value > 0 ? value : 0;
}

// @address 0x447cd8 rasterizer_xenon_encode_rgb565
extern "C" unsigned short rasterizer_xenon_encode_rgb565(const unsigned short *channels)
{
    unsigned char red = (unsigned char)((int)rasterizer_xenon_main_lower(channels[0]) < 255
        ? rasterizer_xenon_main_lower(channels[0]) : 255);
    int green = (unsigned char)((int)rasterizer_xenon_main_lower(channels[1]) < 255
        ? rasterizer_xenon_main_lower(channels[1]) : 255);
    unsigned char blue = (int)rasterizer_xenon_main_lower(channels[2]) < 255
        ? rasterizer_xenon_main_lower(channels[2]) : 255;
    return ((red << 16 >> 8 | ((unsigned char)green << 8) >> 8) & 0xffe000ff | (((unsigned char)green << 8) >> 16 | red) << 5 & 2096896)
            << 3 &
        16777184 & 65535 |
        blue >> 3 & 31;
}


static __forceinline unsigned long rasterizer_xenon_main_byteswap(unsigned long v)
{
    return (v << 24) | ((v & 0xff00) << 8) | ((v >> 8) & 0xff00) | (v >> 24);
}

static __forceinline float rasterizer_xenon_main_nonnegative_channel(unsigned short channel)
{
    return rasterizer_xenon_decode_10bit_float(channel) * 255.0f > 0.0f ? rasterizer_xenon_decode_10bit_float(channel) * 255.0f : 0.0f;
}

static __forceinline unsigned char rasterizer_xenon_main_decode_channel(unsigned short channel)
{
    return (unsigned char)(rasterizer_xenon_main_nonnegative_channel(channel) < 255.0f ? rasterizer_xenon_main_nonnegative_channel(channel) : 255.0f);
}

// @address 0x447da8 rasterizer_xenon_main_decode_rgb10
extern "C" unsigned short *rasterizer_xenon_main_decode_rgb10(unsigned short *channels, unsigned long packed)
{
    // match: 0x447da8: retain the accepted pointer web; removing it changes two register allocations.
    long c2_z = 0;
    unsigned short *ch1_0 = channels + 1;
    unsigned short *ch1 = c2_z ? ch1_0 : ch1_0;
    unsigned long color = rasterizer_xenon_main_byteswap(packed);
    unsigned short red = (unsigned short)((color >> 20) & 1023);
    unsigned short green = (unsigned short)((color >> 10) & 1023);
    unsigned short blue = (unsigned short)(color & 1023);
    channels[0] = 0;
    *ch1 = 0;
    channels[2] = 0;
    channels[0] = rasterizer_xenon_main_decode_channel(red);
    channels[1] = rasterizer_xenon_main_decode_channel(green);
    channels[2] = rasterizer_xenon_main_decode_channel(blue);
    return channels;
}


// @address 0x447f50 rasterizer_xenon_decode_10bit_float
extern "C" float rasterizer_xenon_decode_10bit_float(unsigned short index)
{
    long mantissa = index & 127;
    long exponent = (index >> 7) & 7;
    long biased_exponent = exponent > 1 ? exponent : 1;
    long implicit_bit = exponent < 1 ? exponent : 1;
    double linear = ldexp((double)((float)mantissa * 0.0078125f + (float)implicit_bit), biased_exponent - 8);
    return (float)pow((double)(float)linear, k_rasterizer_gamma_decode_exponent);
}

#if !defined(_XBOX) || defined(__clang__)
// PC (port/GAME-CHANGES.md, "Custom resolutions"): the size the game renders at. The 360 renders its own 1152x640 for
// every display (the video scaler fits it to the TV); a custom resolution ([display] resolution = WxH, halo3.ini or
// HALO3_RESOLUTION) renders at the display's size instead, in square pixels (XGetVideoMode reports that display), so the
// back buffer (cinematic_get_letterbox_coverage), every screen-sized surface (rasterizer_render_targets.cpp) and the
// views follow c_rasterizer::render_globals.
extern "C" int port_config_resolution(unsigned long *width, unsigned long *height);
extern "C" unsigned int __cdecl port_config_shadow_resolution(void);
extern "C" unsigned int port_vk_config_revision(void);
extern "C" void rasterizer_render_targets_update_tiling_settings(void);
extern "C" void rasterizer_main_release_surfaces_for_resize(void);
extern "C" void rasterizer_main_recreate_surfaces_for_resize(void);
extern "C" void port_write_error(const char *text);

// Returns whether the size is a custom resolution's.
static bool rasterizer_xenon_main_get_screen_size(long *width, long *height)
{
    unsigned long custom_width, custom_height;
    if (port_config_resolution(&custom_width, &custom_height))
    {
        *width = (long)custom_width;
        *height = (long)custom_height;
        return true;
    }
    *width = 1152;
    *height = 640;
    return false;
}

static void rasterizer_xenon_main_initialize_screen_size(void)
{
    const bool custom =
        rasterizer_xenon_main_get_screen_size(&c_rasterizer::render_globals.width, &c_rasterizer::render_globals.height);
    unsigned int shadow_res = port_config_shadow_resolution();
    if (shadow_res != 512u && shadow_res != 1024u && shadow_res != 2048u)
        shadow_res = 512u;
    c_rasterizer::render_globals.shadow_size = (long)shadow_res;
    rasterizer_render_targets_update_tiling_settings();
    char text[96];
    csnzprintf(text, sizeof(text), "port: rasterizer: rendering at %ldx%ld%s\n", c_rasterizer::render_globals.width,
        c_rasterizer::render_globals.height, custom ? " (custom resolution)" : " (native)");
    port_write_error(text);
}

static void rasterizer_xenon_main_check_live_resolution(D3DDevice *device)
{
    if (device == NULL)
        return;
    static unsigned int s_last_res_revision = 0;
    const unsigned int revision = port_vk_config_revision();
    if (revision == s_last_res_revision)
        return;
    s_last_res_revision = revision;
    long new_w = 0, new_h = 0;
    const bool custom = rasterizer_xenon_main_get_screen_size(&new_w, &new_h);
    if (new_w == c_rasterizer::render_globals.width && new_h == c_rasterizer::render_globals.height)
        return;
    rasterizer_main_release_surfaces_for_resize();
    c_rasterizer::render_globals.width = new_w;
    c_rasterizer::render_globals.height = new_h;
    rasterizer_render_targets_update_tiling_settings();
    D3DPRESENT_PARAMETERS parameters;
    cinematic_get_letterbox_coverage((presentation_parameters *)&parameters);
    parameters.BackBufferWidth = new_w;
    parameters.BackBufferHeight = new_h;
    parameters.VideoScalerParameters.ScalerSourceRect.x2 = new_w;
    parameters.VideoScalerParameters.ScalerSourceRect.y2 = new_h;
    device->Reset(&parameters);
    rasterizer_main_recreate_surfaces_for_resize();
    g_rasterizer_xenon_memory.reset();
    g_rasterizer_lut_current.m_brightness = 0.0f;
    char text[96];
    csnzprintf(text, sizeof(text), "port: rasterizer: rendering at %ldx%ld%s\n", c_rasterizer::render_globals.width,
        c_rasterizer::render_globals.height, custom ? " (custom resolution)" : " (native)");
    port_write_error(text);
}
#endif

// @address 0x447fe0 ?initialize_device@c_rasterizer@@CA_N_N0@Z
bool c_rasterizer::initialize_device(bool, bool)
{
    bool success = false;
#if !defined(_XBOX) || defined(__clang__)
    rasterizer_xenon_main_initialize_screen_size();
#endif
    g_rasterizer_direct3d = Direct3DCreate9(D3D_SDK_VERSION);
    rasterizer_globals.presentation_interval = 0;
    D3DPRESENT_PARAMETERS parameters;
    cinematic_get_letterbox_coverage((presentation_parameters *)&parameters);
    if (SUCCEEDED(Direct3D_CreateDevice(0, D3DDEVTYPE_HAL, NULL, 0, &parameters, &g_device)))
    {
        g_thread_owning_device.set(get_current_thread_index());
        g_device->GetDeviceCaps(&g_rasterizer_xenon_device_info.caps);
        g_device->GetDisplayMode(0, &g_rasterizer_xenon_device_info.display_mode);
        success = true;
        g_d3d_resource_allocator_device = g_device;
        begin_high_quality_blend();
        if (g_rasterizer_gpr_allocation != 1)
        {
            (*rasterizer_shader_gpr_get_globals())[0] = 48;
            (*rasterizer_shader_gpr_get_globals())[1] = 80;
            g_device->SetShaderGPRAllocation(0, 48, 80);
            g_rasterizer_gpr_allocation = 1;
        }
    }
    Direct3D_CreateDevice(0, D3DDEVTYPE_COMMAND_BUFFER, NULL, D3DCREATE_NO_SHADER_PATCHING, NULL, &g_command_buffer_device);
    g_rasterizer_xenon_memory.initialize();
    if (success)
        g_rasterizer_frame_has_ended = false;
    c_rasterizer::g_tiling_configuration = (c_rasterizer::e_tiling_configuration)-1;
    update_tiling_configuration();
    return success;
}

// @address 0x14c1f0 ?set_gprs_allocation@c_rasterizer@@SA?AW4e_gpr_allocation@1@W421@@Z
c_rasterizer::e_gpr_allocation c_rasterizer::set_gprs_allocation(e_gpr_allocation allocation)
{
    e_gpr_allocation previous_allocation = (e_gpr_allocation)g_rasterizer_gpr_allocation;
    if (previous_allocation == allocation)
        return previous_allocation;
    // match: 0x14c1f0: value range assumption guides branch lowering
    __assume((unsigned long)allocation <= 5);
    switch (allocation) {
    case 0:
        (*rasterizer_shader_gpr_get_globals())[0] = 0;
        (*rasterizer_shader_gpr_get_globals())[1] = 0;
        break;
    case 1:
        (*rasterizer_shader_gpr_get_globals())[0] = 48;
        (*rasterizer_shader_gpr_get_globals())[1] = 80;
        break;
    case 2:
        (*rasterizer_shader_gpr_get_globals())[0] = g_rasterizer_max_vs_gprs;
        (*rasterizer_shader_gpr_get_globals())[1] = 128 - (*rasterizer_shader_gpr_get_globals())[0];
        break;
    case 3:
        (*rasterizer_shader_gpr_get_globals())[0] = 128 - g_rasterizer_max_ps_gprs;
        (*rasterizer_shader_gpr_get_globals())[1] = g_rasterizer_max_ps_gprs;
        break;
    case 4:
        (*rasterizer_shader_gpr_get_globals())[0] = 112;
        (*rasterizer_shader_gpr_get_globals())[1] = 16;
        break;
    case 5:
        (*rasterizer_shader_gpr_get_globals())[0] = 75;
        (*rasterizer_shader_gpr_get_globals())[1] = 53;
        break;
    default:
        break;
    }
    c_rasterizer::g_device->SetShaderGPRAllocation(0, (*rasterizer_shader_gpr_get_globals())[0], (*rasterizer_shader_gpr_get_globals())[1]);
    g_rasterizer_gpr_allocation = allocation;
    if (allocation == 0) {
        unsigned long flags;
        c_rasterizer::g_device->GetShaderGPRAllocation(&flags, &(*rasterizer_shader_gpr_get_globals())[0], &(*rasterizer_shader_gpr_get_globals())[1]);
    }
    return previous_allocation;
}



// @address 0x4482a0 ?rasterizer_vblank_callback@@YAXPBX@Z
void rasterizer_vblank_callback(const void *callback_data)
{
    rasterizer_globals_vblank_callback(((const D3DSWAPDATA *)callback_data)->Swap);
}



static __forceinline unsigned long rasterizer_xenon_main_byteswap_long(unsigned long v)
{
    return (v << 24) | ((v & 0xff00) << 8) | ((v >> 8) & 0xff00) | (v >> 24);
}

// @address 0x4482a8 rasterizer_save_screenshot
extern "C" bool rasterizer_save_screenshot(const char *path, bool supersample)
{
    bool success = false;
    if (path != NULL && c_render_surfaces_interface::initialized())
    {
        s_file_reference file;
        union { unsigned long error; unsigned short swapped; } scratch;
        scratch.error = 0;
        if (file_reference_create_from_path(&file, path, false) && ::file_create(&file) && file_open(&file, 2, &scratch.error))
        {
            c_render_surface *render_surface = rasterizer_xenon_main_get_render_surface((c_rasterizer::e_surface)1);
            D3DTexture *texture = rasterizer_xenon_main_get_d3d_texture(render_surface);
            long scale = supersample ? 2 : 1;
            long scale_squared = scale * scale;
            s_rasterizer_screenshot_file_writer screenshot;
            screenshot.writer.buffer_position = 0;
            screenshot.writer.buffer_size = sizeof(screenshot.buffer);
            screenshot.writer.file = &file;
            screenshot.writer.buffer = screenshot.buffer;
            if (texture != NULL)
            {
                D3DSURFACE_DESC desc;
                D3DLOCKED_RECT locked;
                s_dds_header header;
                D3DCubeTexture_GetLevelDesc((D3DCubeTexture *)texture, 0, &desc);
                D3DTexture_LockRect(texture, 0, &locked, NULL, 16);
                memset(&header.flags, 0, sizeof(header) - sizeof(header.size));
                unsigned short row[6144];
                row[0] = 0;
                for (long channel = 1; channel < 3; channel++)
                    row[channel] = 0;
                memset(&row[3], 0, sizeof(row) - 3 * sizeof(row[0]));
                header.size = sizeof(header);
                header.width = desc.Width / scale;
                header.height = desc.Height / scale;
                header.flags = 0x1007;
                header.pixel_format_size = 32;
                header.pixel_format_flags = 64;
                header.pixel_format_bit_count = 16;
                header.red_mask = 0xf800;
                header.green_mask = 0x7e0;
                header.blue_mask = 0x1f;
                header.alpha_mask = 0;
                header.caps = 0x1000;
                long y;
                scratch.error = 0x44445320;
                unsigned long *header_words = (unsigned long *)&header;
                long words_left = 31;
                do
                {
                    *header_words = rasterizer_xenon_main_byteswap_long(*header_words);
                    header_words++;
                    words_left--;
                } while (words_left > 0);
                buffered_file_write(&screenshot.writer, &scratch.error, sizeof(scratch.error));
                buffered_file_write(&screenshot.writer, &header, sizeof(header));
                if (desc.Width <= 2048)
                {
                y = 0;
                if (desc.Height != 0)
                {
                    do
                    {
                        for (long x = 0; x < desc.Width; x++)
                        {
                            unsigned long offset = XGAddress2DTiledOffset(x, y, (unsigned long)locked.Pitch >> 2, 4);
                            long column = x / scale;
                            unsigned short decoded[3];
                            s_rasterizer_xenon_main_rgb16 sample = *(s_rasterizer_xenon_main_rgb16 *)rasterizer_xenon_main_decode_rgb10(
                                decoded, ((unsigned long *)locked.pBits)[offset]);
                            row[column * 3] += sample.red / scale_squared;
                            row[column * 3 + 1] += sample.green / scale_squared;
                            row[column * 3 + 2] += sample.blue / scale_squared;
                        }
                        if (y % scale == scale - 1)
                        {
                            for (unsigned long x = 0; x < desc.Width / scale; x++)
                            {
                                unsigned short packed = rasterizer_xenon_encode_rgb565(&row[x * 3]);
                                scratch.swapped = (unsigned short)((packed << 8) | (packed >> 8));
                                buffered_file_write(&screenshot.writer, &scratch.swapped, sizeof(scratch.swapped));
                            }
                            memset(row, 0, sizeof(row));
                        }
                        y++;
                    } while (y < desc.Height);
                }
                }
                texture->UnlockRect(0);
                success = true;
            }
            buffered_file_writer_flush(&screenshot.writer);
            file_close(&file);
        }
    }
    return success;
}

// @address 0x448640 ?rasterizer_device_release_thread@c_rasterizer@@SAXXZ
void c_rasterizer::rasterizer_device_release_thread()
{
    g_thread_owning_device.set(-1);
    clear_sampler_textures(~0UL);
    g_device->UnsetAll();
    g_device->InsertFence();
    g_device->ReleaseThreadOwnership();
    g_command_buffer_device->ReleaseThreadOwnership();
}

void c_rasterizer::rasterizer_device_acquire_thread()
{
    g_thread_owning_device.set(get_current_thread_index());
    g_device->AcquireThreadOwnership();
    g_command_buffer_device->AcquireThreadOwnership();
    g_device->InsertFence();
}


extern "C" {
// @address 0x832a68 g_rasterizer_resolved_surface_mask
// @address 0x8c448c g_rasterizer_resolve_samples
long g_rasterizer_resolve_samples;
}


static __forceinline void rasterizer_xenon_main_mark_resolved(const c_render_surface *surface)
{
    for (long word = 0; word < 7; ++word)
        g_rasterizer_resolved_surface_mask[word] &= ~surface->clear_mask[word];
    g_rasterizer_resolved_surface_mask[surface->index >> 5] |= 1 << (surface->index & 31);
}

static __forceinline const s_rasterizer_tiling_settings *rasterizer_xenon_main_tile_lookup(
    const s_rasterizer_tiling_settings (*const &table)[6], long resolution, long configuration)
{
    return &table[resolution][configuration];
}

static __forceinline const long &rasterizer_xenon_main_tile_count(const s_rasterizer_tiling_settings (*const &table)[6],
        long resolution,
        long configuration)
{
    return table[resolution][configuration].tile_count;
}

static __forceinline const D3DRECT *rasterizer_xenon_main_tile_rects(const s_rasterizer_tiling_settings (*const &table)[6],
        long resolution,
        long configuration)
{
    return table[resolution][configuration].tile_rects;
}

// @address 0x448728 ?resolve_surface_tiled@c_rasterizer@@SAXW4e_surface@1@J_N1@Z
void c_rasterizer::resolve_surface_tiled(e_surface surface_index, long source_render_target, bool, bool force_samples)
{
    c_render_surface *surface =
        (c_render_surface *)rasterizer_xenon_main_get_render_surface(surface_index);
    D3DBaseTexture *texture = (D3DBaseTexture *)&surface->texture;
    long samples = g_rasterizer_resolve_samples;
    if (samples < 1)
        samples = 1;
    long resolve_target = source_render_target;
    if (force_samples || source_render_target == 4)
        resolve_target |= samples << 4;
    unsigned long flags = (surface->description.resolve_exponent_bias << 26) | resolve_target;
    long tile_count = rasterizer_xenon_main_get_tiling_settings(c_rasterizer::g_screen_resolution,
        c_rasterizer::g_tiling_configuration)->tile_count;
    const D3DRECT *rects = rasterizer_xenon_main_get_tiling_settings(c_rasterizer::g_screen_resolution,
        c_rasterizer::g_tiling_configuration)->tile_rects;
    for (long tile = 0; tile < tile_count; ++tile)
    {
        c_rasterizer::get_device()->SetPredication((1 << (tile * 2)) | (2 << (tile * 2)));
        c_rasterizer::get_device()->Resolve(flags, &rects[tile], texture, (const D3DPOINT *)&rects[tile], 0, 0, NULL, 1.0f, 0, NULL);
    }
    c_rasterizer::get_device()->SetPredication(0);
    rasterizer_xenon_main_mark_resolved(surface);
}

// @address 0x449000 rasterizer_set_light_render_targets
extern "C" void rasterizer_set_light_render_targets(D3DSurface *render_target0, D3DSurface *render_target1, D3DSurface *depth_stencil)
{
    rasterizer_set_depth_stencil_surface_direct(depth_stencil);
    c_rasterizer::g_device->SetRenderTarget(0, render_target0);
    c_rasterizer::g_device->SetRenderTarget(1, render_target1);
    c_rasterizer::end_high_quality_blend();
    c_rasterizer::refresh_tiling_render_targets();
}

// @address 0x44eb70 rasterizer_xenon_memory_reset
extern "C" void rasterizer_xenon_memory_reset(unsigned long structure_bsp_mask)
{
    g_rasterizer_xenon_memory.reset();
}

// @address 0x456e20 ?initialize@c_rasterizer_xenon_memory@@QAA_NXZ
bool c_rasterizer_xenon_memory::initialize()
{
    g_rasterizer_xenon_memory.m_runtime_buffer = ::runtime_state_malloc("", "", 0xc0000);
    void *physical_buffer = ::_physical_memory_malloc((memory_stage)1, (char const *)(NULL), 0x1c0000, 0, (char const *)(NULL), -1);
    g_rasterizer_xenon_memory.m_physical_buffer = physical_buffer;
    if (physical_buffer)
    {
        XPhysicalProtect(physical_buffer, 0x1c0000, PAGE_READWRITE | PAGE_WRITECOMBINE);
        XQueryMemoryProtect(physical_buffer);
    }
    g_rasterizer_xenon_memory.m_command_buffer = D3DDevice_CreateCommandBuffer(0x4000, 0);
    return g_rasterizer_xenon_memory.m_command_buffer && g_rasterizer_xenon_memory.m_physical_buffer;
}

#ifdef HALO3_64BIT
// Backing request storage matching s_rasterizer_entry's 24-byte 64-bit layout and offsets
// (s_rasterizer_xenon_memory_request in rasterizer_xenon_memory_request_types.h is 20 bytes
// on LP64/LLP64 because `long pad;` is 4 bytes instead of an 8-byte D3DCommandBuffer *).
typedef char rasterizer_entry_buffer_offset_64[(offsetof(s_rasterizer_entry, buffer) == 8) ? 1 : -1];
typedef char rasterizer_entry_flags_offset_64[(offsetof(s_rasterizer_entry, flags) == 16) ? 1 : -1];
typedef char rasterizer_entry_key_index_offset_64[(offsetof(s_rasterizer_entry, key_index) == 20) ? 1 : -1];
typedef char rasterizer_entry_size_64[(sizeof(s_rasterizer_entry) == 24) ? 1 : -1];
static s_rasterizer_entry g_rasterizer_requests_64[3072];
#endif

// @address 0x457040 ?reset@c_rasterizer_xenon_memory@@QAAXXZ
void c_rasterizer_xenon_memory::reset()
{
    if (c_rasterizer::g_device)
        c_rasterizer::g_device->BlockUntilIdle();
    g_rasterizer_xenon_memory.m_request_count = 0;
    g_rasterizer_xenon_memory.m_stage = 0;
    g_rasterizer_xenon_memory.m_entry_count = 0;
    g_rasterizer_xenon_memory.runtime_offset = 0;
    g_rasterizer_xenon_memory.physical_offset = 0;
    g_rasterizer_xenon_memory.collision_overflow_count = 0;
    g_rasterizer_xenon_memory.collision_count = 0;
    g_rasterizer_xenon_memory.command_buffer_build_count = 0;
    g_rasterizer_xenon_memory.command_buffer_invalidation_count = 0;
    g_rasterizer_xenon_memory.command_buffer_clone_failure_count = 0;
    g_rasterizer_xenon_memory.texture_entry_count = 0;
    memset(g_rasterizer_xenon_memory.hash_table, 0, sizeof(g_rasterizer_xenon_memory.hash_table));
    memset(g_rasterizer_xenon_memory.m_requests, 0, sizeof(g_rasterizer_xenon_memory.m_requests));
#ifdef HALO3_64BIT
    memset(g_rasterizer_requests_64, 0, sizeof(g_rasterizer_requests_64));
#endif
    g_rasterizer_xenon_memory.m_command_buffer_flags = 16;
    g_rasterizer_xenon_memory.m_tile_rects = NULL;
    g_rasterizer_xenon_memory.m_tile_count = 0;
    if (c_rasterizer::get_is_tiling_enabled())
    {
        g_rasterizer_xenon_memory.m_command_buffer_flags = 18;
        g_rasterizer_xenon_memory.m_tile_count = c_rasterizer::get_tiling_settings(c_rasterizer::g_screen_resolution,
        c_rasterizer::g_tiling_configuration)->tile_count;
        g_rasterizer_xenon_memory.m_tile_rects = c_rasterizer::get_tiling_settings(c_rasterizer::g_screen_resolution,
        c_rasterizer::g_tiling_configuration)->tile_rects;
    }
}

// @address 0x4571d8 ?new_request@c_rasterizer_xenon_memory@@QAA_NPAPAUs_rasterizer_xenon_memory_request@@_KJ@Z
bool c_rasterizer_xenon_memory::new_request(s_rasterizer_xenon_memory_request **out_request, unsigned __int64 data, long type)
{
    bool result = false;
    if (g_rasterizer_xenon_memory.m_request_count < 3072)
    {
#ifdef HALO3_64BIT
        s_rasterizer_entry *entry = &g_rasterizer_requests_64[g_rasterizer_xenon_memory.m_request_count];
        *out_request = (s_rasterizer_xenon_memory_request *)entry;
        entry->key_low = (unsigned int)data;
        entry->key_high = (unsigned int)(data >> 32);
        entry->buffer = 0;
        entry->flags = (((unsigned int)type & 0x3fu) << 26) |
                       (((unsigned int)g_rasterizer_xenon_memory.m_stage & 0x1fffu) << 8);
        entry->key_index = (unsigned short)g_rasterizer_xenon_memory.m_entry_count;
        entry->key_count = 0;
#else
        *out_request = &g_rasterizer_xenon_memory.m_requests[g_rasterizer_xenon_memory.m_request_count];
        (*out_request)->data_low = (unsigned long)data;
        (*out_request)->data_high = (unsigned long)(data >> 32);
        (*out_request)->type = type;
        (*out_request)->shared_type = 0;
        (*out_request)->unknown_flag0 = 0;
        (*out_request)->unknown_flag1 = 0;
        (*out_request)->unknown_bits = 0;
        (*out_request)->stage = g_rasterizer_xenon_memory.m_stage;
        (*out_request)->entry_count = 0;
        (*out_request)->first_entry = (unsigned short)g_rasterizer_xenon_memory.m_entry_count;
#endif
        g_rasterizer_xenon_memory.m_request_count++;
        result = true;
    }
    return result;
}

// @address 0x4572c8 ?new_entry@c_rasterizer_xenon_memory@@QAAPAUs_rasterizer_xenon_memory_entry@@PAUs_rasterizer_xenon_memory_request@@@Z
s_rasterizer_xenon_memory_entry *c_rasterizer_xenon_memory::new_entry(s_rasterizer_xenon_memory_request *request)
{
    s_rasterizer_xenon_memory_entry *entry = NULL;
#ifdef HALO3_64BIT
    s_rasterizer_entry *cached = (s_rasterizer_entry *)request;
    if (((cached->flags >> 23) & 7u) == 0)
    {
        long entry_index = -1;
        if (g_rasterizer_xenon_memory.m_entry_count < 12288)
            entry_index = g_rasterizer_xenon_memory.m_entry_count++;
        if (entry_index != -1)
        {
            cached->key_count++;
            entry = &g_rasterizer_xenon_memory.m_entries[entry_index];
        }
    }
    else
    {
        long entry_index = cached->key_index + g_rasterizer_xenon_memory.m_shared_entry_count++;
        entry = &g_rasterizer_xenon_memory.m_entries[entry_index];
    }
#else
    if (!request->shared_type)
    {
        long entry_index = -1;
        if (g_rasterizer_xenon_memory.m_entry_count < 12288)
            entry_index = g_rasterizer_xenon_memory.m_entry_count++;
        if (entry_index != -1)
        {
            request->entry_count++;
            entry = &g_rasterizer_xenon_memory.m_entries[entry_index];
        }
    }
    else
    {
        long entry_index = request->first_entry + g_rasterizer_xenon_memory.m_shared_entry_count++;
        entry = &g_rasterizer_xenon_memory.m_entries[entry_index];
    }
#endif
    return entry;
}

static const unsigned char rasterizer_xenon_main_mask_bits[8] = { 52, 52, 53, 27, 27, 27, 27, 23 };

// @address 0x456f00 rasterizer_xenon_memory_initialize_masks
extern "C" void rasterizer_xenon_memory_initialize_masks(c_rasterizer_xenon_memory *)
{
    g_rasterizer_xenon_memory.vertex_constant_mask.words[0] = 0;
    memset(&g_rasterizer_xenon_memory.vertex_constant_mask.words[1], 0, sizeof(s_rasterizer_xenon_mask) - sizeof(unsigned __int64));
    g_rasterizer_xenon_memory.pixel_constant_masks[1].words[0] = 0;
    memset(&g_rasterizer_xenon_memory.pixel_constant_masks[1].words[1], 0, sizeof(s_rasterizer_xenon_mask) - sizeof(unsigned __int64));
    g_rasterizer_xenon_memory.pixel_constant_masks[0].words[0] = 0;
    memset(&g_rasterizer_xenon_memory.pixel_constant_masks[0].words[1], 0, sizeof(s_rasterizer_xenon_mask) - sizeof(unsigned __int64));
    s_rasterizer_xenon_mask *mask_a = &g_rasterizer_xenon_memory.pixel_constant_masks[0];
    s_rasterizer_xenon_mask *mask_b = &g_rasterizer_xenon_memory.pixel_constant_masks[1];
    s_rasterizer_xenon_mask *mask_c = &g_rasterizer_xenon_memory.vertex_constant_mask;
    for (long index = 0; index < 8; index++)
        mask_b->words[0] |= 1ui64 << rasterizer_xenon_main_mask_bits[index];
    for (long index = 0; index < 25; index++)
        mask_b->words[0] |= 1ui64 << index;
    for (long index = 0; index < 4; index++)
        mask_a->words[0] |= 1 << index;
    for (long index = 0; index < 58; index++)
        mask_c->words[0] |= 1ui64 << index;
}

// @address 0x456cc8 ??0c_rasterizer_xenon_memory@@QAA@XZ
c_rasterizer_xenon_memory::c_rasterizer_xenon_memory()
{
    // match: 0x456cc8: this constructor initializes the global singleton.
    __assume(this == &g_rasterizer_xenon_memory);
    m_runtime_buffer = NULL;
    m_physical_buffer = NULL;
    m_command_buffer = NULL;
    m_tiling_configuration = -1;
    alternate_entry_point = 0;
    state_a.m_Mask[0] = -1;
    state_a.m_Mask[1] = -1;
    state_a.m_Mask[2] = -1;
    state_a.m_Mask[3] = -1;
    state_a.m_Mask[4] = -1;
    transparent_state.m_Mask[0] = -1;
    transparent_state.m_Mask[1] = -1;
    transparent_state.m_Mask[4] = -1;
    opaque_state.m_Mask[0] = -1;
    opaque_state.m_Mask[1] = -1;
    opaque_state.m_Mask[4] = -1;
    transparent_state.m_Mask[3] = -2;
    opaque_state.m_Mask[3] = -2;
    transparent_state.m_Mask[2] = -0x1e0408;
    opaque_state.m_Mask[2] = -0x1e0001;
    rasterizer_xenon_memory_initialize_masks(this);
    reset();
}


// @address 0x8c46ad g_rasterizer_in_ui_shell
extern "C" {
unsigned char g_rasterizer_in_ui_shell;
}

// @address 0x147490 ?update_tiling_configuration@c_rasterizer@@CAXXZ
void c_rasterizer::update_tiling_configuration()
{
    bool configuration_changed = g_rasterizer_current_buffer_2_frames != g_rasterizer_requested_buffer_2_frames;
    if (g_rasterizer_tiling_enabled != g_rasterizer_requested_tiling_enabled)
    {
        g_rasterizer_tiling_enabled = g_rasterizer_requested_tiling_enabled;
        g_device->SetRenderState(D3DRS_BUFFER2FRAMES, get_is_tiling_enabled());
        configuration_changed = true;
    }
    long configuration = c_rasterizer::g_tiling_configuration_debug;
    if (g_rasterizer_in_ui_shell)
        configuration = 2;
    if (c_rasterizer::g_tiling_configuration != configuration)
    {
        c_rasterizer::g_tiling_configuration = (c_rasterizer::e_tiling_configuration)PIN(configuration, 0, 2);
        g_device->SetRenderState(D3DRS_BUFFER2FRAMES, get_is_tiling_enabled());
        configuration_changed = true;
    }
    if (configuration_changed && !get_is_tiling_enabled())
    {
        g_device->SetRenderState(D3DRS_BUFFER2FRAMES, g_rasterizer_requested_buffer_2_frames);
        g_rasterizer_current_buffer_2_frames = g_rasterizer_requested_buffer_2_frames;
    }
    if (c_rasterizer::g_screen_resolution != c_rasterizer::g_screen_resolution_debug)
    {
        c_rasterizer::g_screen_resolution = (e_screen_resolution)PIN(c_rasterizer::g_screen_resolution_debug,
        0,
        k_screen_resolution_count - 1);
        c_rasterizer::g_screen_resolution_debug = c_rasterizer::g_screen_resolution;
        D3DPRESENT_PARAMETERS parameters;
        cinematic_get_letterbox_coverage((presentation_parameters *)&parameters);
        if (c_rasterizer::g_screen_resolution == 0)
        {
#if !defined(_XBOX) || defined(__clang__)
            // PC: screen resolution 0 is the size initialize_device chose (1152x640, or a custom resolution's)
            long width, height;
            rasterizer_xenon_main_get_screen_size(&width, &height);
            parameters.BackBufferWidth = width;
            parameters.BackBufferHeight = height;
            parameters.VideoScalerParameters.ScalerSourceRect.x2 = width;
            parameters.VideoScalerParameters.ScalerSourceRect.y2 = height;
            c_rasterizer::render_globals.width = width;
            c_rasterizer::render_globals.height = height;
#else
            parameters.BackBufferWidth = 1152;
            parameters.BackBufferHeight = 640;
            parameters.VideoScalerParameters.ScalerSourceRect.x2 = 1152;
            parameters.VideoScalerParameters.ScalerSourceRect.y2 = 640;
            c_rasterizer::render_globals.width = 1152;
            c_rasterizer::render_globals.height = 640;
#endif
        }
        g_device->Reset(&parameters);
        g_rasterizer_lut_current.m_brightness = 0.0f;
    }
}


#ifdef HALO3_64BIT
extern "C" int port_d3d_tag_shell_bracket();
extern "C" void port_d3d_shell_surface(unsigned int role, D3DSurface *surface, D3DBaseTexture *resolve);

static void rasterizer_tag_shell_surfaces()
{
    if (!port_d3d_tag_shell_bracket())
        return;
    // Capture all four accumulator views. The title can change HDR and alpha-blend modes inside the bracket.
    const int roles[] = { 2, 3, 8, 13, 14, 17, 18, 19 };
    for (int role : roles) {
        c_render_surface_group &group = g_rasterizer_surface_groups[role];
        if (!group.surfaces || !group.count)
            continue;
        c_render_surface *view = group.get_specialized_surface((c_rasterizer::e_surface)role);
        const int views = (group.description.flags & 8) ? 4 : 1;
        view -= (view - group.surfaces) % views;
        if (view < group.surfaces || view - group.surfaces + views > group.count)
            continue;
        for (int i = 0; i < views; ++i)
            port_d3d_shell_surface(role, view[i].surface, (D3DBaseTexture *)&view[i].texture);
    }
}
#endif

// @address 0x16cec0 ?begin_tiling@c_rasterizer@@SAXXZ
// match: 0x16cec0: use the CRT memset call for the shader constant clear.
#pragma function(memset)
void c_rasterizer::begin_tiling()
{
    if (c_rasterizer::get_is_tiling_enabled())
    {
        const D3DVECTOR4 clear_color = { 0.0f, 0.0f, 0.0f, 0.0f };
        long tile_count = rasterizer_xenon_main_get_tiling_settings(c_rasterizer::g_screen_resolution,
        c_rasterizer::g_tiling_configuration)->tile_count;
        const D3DRECT *rects = rasterizer_xenon_main_get_tiling_settings(c_rasterizer::g_screen_resolution,
        c_rasterizer::g_tiling_configuration)->tile_rects;
        c_rasterizer::g_device->BeginTiling(1, tile_count, rects, &clear_color, 1.0f, 0);
#ifdef HALO3_64BIT
        if (g_rasterizer_in_ui_shell && g_tiling_configuration == 2 && tile_count == 4)
            rasterizer_tag_shell_surfaces();
#endif
        real_vector4d scale;
        scale.x = 1.0f / (rects[0].x2 - rects[0].x1);
        scale.y = 1.0f / (rects[0].y2 - rects[0].y1);
        scale.z = 0.5f / (rects[0].x2 - rects[0].x1);
        scale.w = 0.5f / (rects[0].y2 - rects[0].y1);
        c_rasterizer::g_device->GpuOwnPixelShaderConstantF(108, 4);
        for (long tile = 0; tile < tile_count; ++tile)
        {
            c_rasterizer::g_device->SetPredication(3 << (tile * 2));
            D3DVECTOR4 *constant_data;
            c_rasterizer::g_device->GpuBeginPixelShaderConstantF4(108, &constant_data, 4);
            real_vector4d *constants = new (constant_data) real_vector4d[4];
            memset(constants, 0, sizeof(*constants) * 4);
            constants[0].x = (float)rects[tile].x1;
            constants[0].y = (float)rects[tile].y1;
            constants[1] = scale;
            c_rasterizer::g_device->GpuEndPixelShaderConstantF4();
        }
        c_rasterizer::g_device->SetPredication(0);
    }
    else
    {
        real_vector4d offset = { 0.0f };
        c_rasterizer::g_device->SetPixelShaderConstantF(108, offset.n, 1);
        float width = (float)rasterizer_xenon_main_get_render_surface((c_rasterizer::e_surface)19)->description.width;
        float height = (float)rasterizer_xenon_main_get_render_surface((c_rasterizer::e_surface)19)->description.height;
        real_vector4d scale;
        scale.x = 1.0f / width;
        scale.y = 1.0f / height;
        scale.z = scale.x * 0.5f;
        scale.w = scale.y * 0.5f;
        c_rasterizer::g_device->SetPixelShaderConstantF(109, scale.n, 1);
    }
}

// match: 0x16cec0: enable inline memset outside begin_tiling.
#pragma intrinsic(memset)

void c_rasterizer::end_tiling()
{
    D3DTagCollection_SetPixelShaderConstantFTag(&c_rasterizer::get_device()->m_Pending, 108, 4);
    c_rasterizer::get_device()->EndTiling(0, 0, 0, 0, 1.0f, 0, 0);
    const real_vector4d zero = { 0.0f };
    c_rasterizer::get_device()->SetPixelShaderConstantF(108, zero.n, 1);
}

// @address 0x73a25c g_rasterizer_gpr_allocation
extern "C" {
long g_rasterizer_gpr_allocation = 6;
}
extern "C" {
// @address 0x72ce7b g_rasterizer_requested_buffer_2_frames
bool g_rasterizer_requested_buffer_2_frames = true;
// @address 0x72ce7e g_rasterizer_current_buffer_2_frames
bool g_rasterizer_current_buffer_2_frames = true;
}

// @address 0x8c46be g_rasterizer_frame_has_ended
extern "C" {
bool g_rasterizer_frame_has_ended;
}

// @address 0x5aab00 D3DDevice_CreateCommandBuffer
// @address 0x5a5200 D3DDevice_SetRenderState_Buffer2Frames
