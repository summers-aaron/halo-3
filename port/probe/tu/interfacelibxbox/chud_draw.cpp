// interfacelibxbox/chud_draw.obj: from the records 0x167510 0x16a978 0x185ac8 0x186c08 0x186e58 0x187098 0x1872a8 0x1874c8 0x189420 0x18b750 0x18b848 0x18b9c8 0x18bc20 0x18be78 0x18c068 0x18c630 0x18cb60 0x18cf40 0x18d280 0x18d498 0x18de28 0x191e70 0x451c20 0x4544f0 0x472128 0x488460 0x491af8 0x49b6d0 0x49b830 0x49b908 0x49ba60 0x49c320 0x49c6e8 0x49c9d8 0x49cc80 0x49cec8 0x49d168 0x49d440 0x49d4b0 0x49d718 0x49d7a8 0x49dad8 0x49dd28 0x49e000 0x49e138 0x49ec38 0x49ed78 0x49f130 0x49f1b8 0x49f2b0 0x49f880 0x49fbe0 0x49fd48 0x49fec8 0x4a0190 0x4a02d8 0x4a0668 0x4a07d0 0x4a0958
// @address 0x8c4694 g_rasterizer_device
// @address 0x11df680 game_globals
// @address 0x11df70c g_director_globals
// @address 0x10b0 k_font_cache_vtable
// @address 0x14d5b0 D3DDevice_SetPixelShader
// @address 0x11df6dc x_persistent_user_data
// @address 0x81dab8 g_rasterizer_alpha_blend_mode
// @address 0x11df8cc g_observer_globals
// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @address 0x25888 k_observer_default_aspect_ratio
// @address 0x5a5518 D3DDevice_SetRenderState_PointSpriteEnable
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"
#include "game/game_allegiance_data.h"
#include "cseries/cseries_unicode.h"
#include "bitmaps/bitmap_group.h"
#include "memory/tls_data_arrays.h"
#include "text/font_id_types.h"
#include "rasterizer/render_target_types.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "memory/static_arrays.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "objects/object_datum.h"
#include "game/game.h"
#include "interface/user_interface.h"
#include "objects/objects.h"
#include "sound/game_sound.h"
#include "interface/chud/chud_definitions.h"
#include "interface/chud/chud_draw.h"
#include "interface/chud/chud_messaging.h"
#include "rasterizer/rasterizer_main.h"
#include "rasterizer/rasterizer_states.h"
#include "interface/user_interface_shared_globals.h"
#include "render/render_camera_types.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "bitmaps/bitmap_data.h"
#include "math/function_definitions.h"
#include "cseries/cseries.h"
#include "effects/player_effects.h"
#include "interface/chud/chud_draw_externs.h"
#include "interface/chud/chud_draw_types.h"
#include <math.h>
#define XVIDEO_MODE XDK_VIDEO_MODE
#include <xtl.h>
#undef XVIDEO_MODE
#include "interface/chud/chud_cortana_definition_types.h"
#include "interface/chud/utf32_types.h"
#include "interface/chud/chud_navpoints.h"
#include "rasterizer/rasterizer_dynamic_render_targets.h"
#include "cutscene/cinematics.h"
#include "game/game_time.h"
#include "interface/chud/chud.h"
#include "main/main_render.h"
#include "rasterizer/xenon/rasterizer_xenon_main.h"
#include "render/render_cameras.h"
#include "objects/emblems.h"
#include "text/draw_string.h"
#include <string.h>
#include "math/matrix_math.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "interface/chud/chud_globals_definitions.h"
#include "interface/chud_draw_globals.h"
#include "units/unit_types.h"
#include "items/weapon_definitions.h"
#include "interface/chud/chud_directional_damage.h"
#include "cutscene/cinematic_globals_types.h"
#include "interface/chud/chud_user_data_types.h"
#include "tag_files/tag_groups.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer.h"
#include "text/font_character_types.h"
#include "interface/chud/chud_draw_string.h"
#include "text/font_cache_types.h"
#include "game/game_engine_string_types.h"
#include "objects/emblem_info.h"
#include "rasterizer/rasterizer_dynamic_render_target_types.h"
#include "render/views/player_view_types.h"
#include "camera/observer_globals_types.h"
#include "interface/chud/chud_motion_sensor.h"
#include "cseries/crt_xdk.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "sound/game_sound_types.h"
#include "sound/game_looping_sound_data.h"
#include "game/player_control_types.h"
#include "main/main_render_loading_types.h"
#include "rasterizer/rasterizer_vertex_screen_types.h"
#include "math/real_math.h"
#include "objects/object_header_access.h"
#include "memory/datum_index.h"

class c_rasterizer_texture_ref;
struct s_widget_geometry;
struct s_chud_aux_animation_data;
class c_chud_navpoint_manager;
struct s_chud_globals_definition;
class c_font_cache;
struct s_font_header;
struct D3DPixelShader;
struct D3DVertexShader;
struct s_entry;
struct chud_navpoints_widget_geometry;

// String ids of the chud text widgets that read the persistent display state.
enum
{
    _string_id_chud_variant_name = 966,
    _string_id_chud_talking_player_name = 967,
    _string_id_chud_arming_meter_name = 968,
    _string_id_chud_time_left = 969,
    _string_id_chud_training_text = 970,
    _string_id_chud_campaign_objective_text = 1195,
};

struct s_chud_keyframe_scalar
{
    unsigned long time_offset_ms;
    float value;
    static __forceinline void initialize_result(float *result) { *result = 1.0f; }
    static __forceinline void lerp(const float *a, const float *b, float fraction, float *result) { *result = *a + (*b - *a) * fraction; }
};

struct s_chud_keyframe_scalar_zero
{
    unsigned long time_offset_ms;
    float value;
    static __forceinline void initialize_result(float *result) { *result = 0.0f; }
    static __forceinline void lerp(const float *a, const float *b, float fraction, float *result) { *result = *a + (*b - *a) * fraction; }
};

struct s_chud_keyframe_rotation
{
    unsigned long time_offset_ms;
    real_euler_angles3d value;
    static __forceinline void initialize_result(real_euler_angles3d *result) { result->yaw = 0.0f; result->pitch = 0.0f; result->roll = 0.0f; }
    static __forceinline void lerp(const real_euler_angles3d *a, const real_euler_angles3d *b, float fraction, real_euler_angles3d *result)
    {
        for (long i = 0; i < 3; i++)
            result->n[i] = a->n[i] + (b->n[i] - a->n[i]) * fraction;
    }
};

struct s_chud_keyframe_scale
{
    unsigned long time_offset_ms;
    real_vector2d value;
    static __forceinline void initialize_result(real_vector2d *result) { result->i = 1.0f; result->j = 1.0f; }
    static __forceinline void lerp(const real_vector2d *a, const real_vector2d *b, float fraction, real_vector2d *result)
    {
        for (long i = 0; i < 2; i++)
            result->n[i] = a->n[i] + (b->n[i] - a->n[i]) * fraction;
    }
};

struct s_chud_keyframe_texture
{
    unsigned long time_offset_ms;
    real_vector4d value;
    static __forceinline void initialize_result(real_vector4d *result) { result->i = 1.0f; result->j = 1.0f; result->k = 0.0f; result->l = 0.0f; }
    static __forceinline void lerp(const real_vector4d *a, const real_vector4d *b, float fraction, real_vector4d *result)
    {
        chud_animation_interpolate_reals(a->n, b->n, fraction, result->n, 4);
    }
};

struct s_chud_animation
{
    s_tag_block keyframes;              // count, address, definition
    c_function_definition function;
};

struct s_chud_draw_widget_data
{
    long user_index;
    s_chud_runtime_widget_datum *runtime;
    void *definition;
    s_chud_text_widget *parent_widget;
    s_chud_text_widget *widget;
};

struct s_widget_geometry_quad_fancy_vertex
{
    real_point3d position;
    unsigned long color;
    real_point2d texcoord;
};

struct s_widget_geometry_quad_fancy
{
    s_widget_geometry_quad_fancy_vertex vertices[4];
};

struct s_chud_loading_screen
{
    long initialized;
    D3DVertexDeclaration *declarations[3];
    D3DTexture *textures[4];
    D3DVertexBuffer *buffers[4];
    D3DTexture *emblems[2];
    D3DTexture *detail_texture;
    float profile[64];
    float samples[32][64];
    float padding;
    float coefficients[32 * 12];
};

struct s_chud_loading_vertex
{
    real_point3d position;
    real_point2d texcoord;
};

// @address 0x49e000 ?chud_text_widget_compute_geometry@@YA_NPAUs_chud_draw_widget_data@@PAUs_widget_geometry@@PAUs_chud_render_data@@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xea4f8 ?evaluate_scalar@c_function_definition@@QBAMMM@Z
// @address 0x167510 ?set_constants@c_curved_hud_test@@QAAXJ@Z
// @address 0x185ac8 chud_draw_has_active_timed_slots
// @address 0x186c08 ??$chud_apply_widget_animation_definition_template@Treal_euler_angles3d@@Us_chud_keyframe_rotation@@@@YAXPAUs_chud_animation@@MMMPATreal_euler_angles3d@@@Z
// @address 0x186e58 ??$chud_apply_widget_animation_definition_template@Treal_vector2d@@Us_chud_keyframe_scale@@@@YAXPAUs_chud_animation@@MMMPATreal_vector2d@@@Z
// @address 0x187098 ??$chud_apply_widget_animation_definition_template@MUs_chud_keyframe_scalar@@@@YAXPAUs_chud_animation@@MMMPAM@Z
// @address 0x1872a8 ??$chud_apply_widget_animation_definition_template@MUs_chud_keyframe_scalar_zero@@@@YAXPAUs_chud_animation@@MMMPAM@Z
// @address 0x1874c8 ??$chud_apply_widget_animation_definition_template@Treal_vector4d@@Us_chud_keyframe_texture@@@@YAXPAUs_chud_animation@@MMMPATreal_vector4d@@@Z
// @address 0x189420 ??0c_chud_draw_string@@QAA@XZ
// @address 0x189498 ??0c_draw_string@@QAA@XZ
// @address 0x18a118 ?predict_character@c_chud_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@@Z
// @address 0x18aea0 ?precache_character@c_chud_draw_string@@UAA_NPAVc_font_cache@@W4e_font_id@@Uutf32@@PAPBUs_font_character@@@Z
// @address 0x18b558 ?draw_character@c_chud_draw_string@@EAAXPAVc_font_cache@@Uutf32@@KKMMMMMM@Z
// @address 0x2902a0 ??1c_chud_draw_string@@UAA@XZ
// @address 0x290508 ?draw@c_draw_string@@UAA_NPAVc_font_cache@@PBD@Z
// @address 0x18a520 ?draw@c_draw_string@@UAA_NPAVc_font_cache@@PBUutf32@@@Z
// @address 0x18b400 ?draw@c_draw_string@@UAA_NPAVc_font_cache@@PB_W@Z
// @address 0x18b750 ?chud_widget_geometry_add_character@@YAXPAUs_widget_geometry@@PBTreal_rectangle2d@@1M@Z
// @address 0x18b848 chud_draw_try_and_get_screen_point
// @address 0x18b9c8 ?inverse_transform@c_curved_hud_test@@QAAXPATreal_point2d@@PAM1@Z
// @address 0x18c068 ?blip_build_geometry@c_chud_motion_sensor@@AAAXPAUs_widget_geometry_quad_fancy@@PAUs_motion_sensor_blip@1@KMMPATreal_point2d@@M@Z
// @address 0x18cb60 chud_point_tracker_update_screen_position
// @address 0x18cf40 ?get_managed_navpoint_arrow_color@c_chud_navpoint_manager@@CAKPAUs_managed_navpoint@1@@Z
// @address 0x18d280 ?get_managed_navpoint_text_color@c_chud_navpoint_manager@@CAKPAUs_managed_navpoint@1@@Z
// @address 0x191e70 ?chud_draw_set_shaders@@YAXJ@Z
// @address 0x472128 ?chud_user_should_draw@@YA_NJ@Z
// @address 0x49b6d0 chud_draw_is_coop_vehicle_visible
// @address 0x49c9d8 ?chud_apply_widget_animation_definition@@YAXMPAUs_chud_animation_definition@@PAUreal_matrix4x3@@PAUs_widget_geometry@@PAUs_chud_aux_animation_data@@@Z
// @address 0x49cec8 ?chud_apply_render_data@@YA_NPAUs_chud_render_data@@_N@Z
// @address 0x14d118 ?set_vertex_shader@c_rasterizer@@SA_NPBVc_rasterizer_vertex_shader@@W4e_vertex_type@@W4e_transfer_vector_vertex_types@@W4e_entry_point@@@Z
// @address 0x14d4b8 ?set_pixel_shader@c_rasterizer@@SA_NPBVc_rasterizer_pixel_shader@@W4e_entry_point@@@Z
// @address 0x49d440 ?chud_render_data_fetch_scalar_output@@YAMFPBUs_chud_widget_render_data@@PBUs_chud_runtime_widget_datum@@@Z
// @address 0x49d4b0 ?chud_bitmap_widget_compute_bitmap_info@@YA_NJJJPAUs_chud_bitmap_info@@@Z
// @address 0x49d718 ?chud_compute_geometry_common@@YAXPAUs_chud_widget_placement_data@@PAUs_widget_geometry@@@Z
// @address 0x49d7a8 ?chud_widget_build_quads_from_bounds@@YAXPATreal_rectangle2d@@00PAUs_widget_geometry@@MM_N@Z
// @address 0x49f130 ?chud_expand_pixel32@@YAXKPATreal_vector4d@@@Z
// @address 0x49f1b8 ?chud_build_custom_render_data@@YAXPAUs_chud_render_data@@J@Z
// @address 0x49f880 ?set_constants@c_chud_cortana_effect@@QAAXMMM@Z
// @address 0x49fbe0 ?update_noise@c_chud_cortana_effect@@QAAXM@Z
// @address 0x4a0190 chud_cortana_effect_new_random_basis
// @address 0x4a0668 ?chud_build_geometry_from_navpoint@@YAMJJJMMPAUs_widget_geometry@@@Z
// @address 0x4a0958 chud_draw_get_normalized_screen_point


STATIC_ASSERT(sizeof(s_chud_runtime_widget_datum) == 0x18, chud_runtime_widget_size);
STATIC_ASSERT(offsetof(s_chud_runtime_widget_datum, next_index) == 0xc, chud_runtime_widget_next_offset);

// @address 0x737504 g_tiled_render_x_scale
// @address 0x737508 g_tiled_render_y_scale
// @address 0x8c4830 g_tiled_render_x_offset
// @address 0x8c4834 g_tiled_render_y_offset

// @address 0x8977a0 g_chud_draw_globals
s_chud_draw_globals g_chud_draw_globals;

typedef c_chud_navpoint_manager::s_managed_navpoint s_managed_navpoint;

static __forceinline unsigned long chud_draw_get_skin_color(long skin_index, long color_index)
{
    unsigned long color = 0;
    skin_index = MIN(MAX(skin_index, 0), chud_globals->row_count - 1);
    color_index = MIN(MAX(color_index, 0), 27);
    if (chud_globals->row_count > 0)
        color = chud_globals->rows[skin_index].colors[color_index];
    return color;
}


static __forceinline void chud_draw_compute_geometry_common_internal(
    s_widget_geometry *geometry, float x_scale, float y_scale, float a, float b, float c, float d)
{
    set_real_point3d(&geometry->position,
        geometry->x0 + x_scale * (geometry->x1 - geometry->x0),
        geometry->y0 + y_scale * (geometry->y1 - geometry->y0),
        0.0f);
    geometry->scale.i = c;
    geometry->scale.j = d;
    geometry->offset.x = a;
    geometry->offset.y = b;
    geometry->offset.z = 0.0f;
    geometry->rotation.yaw = 0.0f;
    geometry->rotation.pitch = 0.0f;
    geometry->rotation.roll = 0.0f;
}
static __forceinline bool chud_draw_game_skull_is_active_secondary(long skull)
{
    unsigned long skulls = game_globals->options.game_mode == 1 ? game_globals->options.secondary_skulls : 0;
    bool result = false;
    if (skull >= 0 && skull < 5)
        result = ((1 << skull) & skulls) != 0;
    return result;
}
template <typename t_keyframe>
static __forceinline t_keyframe *chud_draw_animation_keyframe_get(s_chud_animation *animation, long index)
{
    return (t_keyframe *)tag_block_get_element_with_size(&animation->keyframes, index, sizeof(t_keyframe));
}

extern "C" bool chud_draw_has_active_timed_slots(s_directional_damage *entries, long index)
{
    bool result = false;
    if (g_director_globals->slots[index].perspective == 0)
    {
        long time = game_time_globals->time;
        for (long slot = 0; slot < 4; slot++)
        {
            if (entries[slot].end_time > time)
            {
                result = true;
                break;
            }
        }
    }
    return result;
}

// 0x187098, 0x1872a8
template <typename t_value, typename t_keyframe>
void chud_apply_widget_animation_definition_template(s_chud_animation *animation, float time_ms, float input, float scale, t_value *result)
{
    t_keyframe::initialize_result(result);
    if (animation)
    {
        float function_value = animation->function.evaluate_scalar(input, 1.0f);
        float time = function_value * scale;
        long index;
        for (index = animation->keyframes.count - 2; index >= 0; index--)
        {
            t_keyframe *keyframe = chud_draw_animation_keyframe_get<t_keyframe>(animation, index);
            if (!((float)keyframe->time_offset_ms > time_ms) || index == 0)
                break;
        }
        t_keyframe *begin_keyframe = chud_draw_animation_keyframe_get<t_keyframe>(animation, index);
        t_keyframe *end_keyframe = chud_draw_animation_keyframe_get<t_keyframe>(animation, index + 1);
        float fraction = (time - (float)begin_keyframe->time_offset_ms) / (float)(unsigned long)(end_keyframe->time_offset_ms - begin_keyframe->time_offset_ms);
        fraction = MIN(MAX(fraction, 0.0f), 1.0f);
        t_keyframe::lerp(&begin_keyframe->value, &end_keyframe->value, fraction, result);
    }
}
template void chud_apply_widget_animation_definition_template<float, s_chud_keyframe_scalar>(s_chud_animation *, float, float, float, float *);
template void chud_apply_widget_animation_definition_template<float, s_chud_keyframe_scalar_zero>(s_chud_animation *, float, float, float, float *);
template void chud_apply_widget_animation_definition_template<real_euler_angles3d, s_chud_keyframe_rotation>(
    s_chud_animation *, float, float, float, real_euler_angles3d *);
template void chud_apply_widget_animation_definition_template<real_vector2d, s_chud_keyframe_scale>(s_chud_animation *, float, float, float, real_vector2d *);
template void chud_apply_widget_animation_definition_template<real_vector4d, s_chud_keyframe_texture>(s_chud_animation *, float, float, float, real_vector4d *);


// 0x189420

// @address 0x189688 ?font_get_header@@YAPBUs_font_header@@J@Z

// @address 0x1898a0 ?set_bounds@c_draw_string@@QAAXPBTrectangle2d@@@Z
// @address 0x189a10 ?precache_wchar_string_for_font@c_draw_string@@QAA_NPAVc_font_cache@@PB_WW4e_font_id@@@Z
// @address 0x18a350 ?compute_wchar_string_bounds@c_draw_string@@QAA_NPAVc_font_cache@@PB_WPATrectangle2d@@2@Z

// @address 0x189960 ??0c_font_cache@@QAA@XZ
c_chud_draw_string::c_chud_draw_string() :
    m_geometry(0),
    m_scale(1.0f)
{
    set_display_resolution_scale_adjustment(1.0f);
    set_real_rectangle2d(&m_clip, 0.0f, 1000.0f, 0.0f, 1000.0f);
}

void chud_widget_geometry_add_character(s_widget_geometry *geometry, const real_rectangle2d *texture_bounds, const real_rectangle2d *bounds, float scale)
{
    if (geometry->character_count < 96)
    {
        const float uv_scale = 0.001953125f;
        s_widget_geometry_character *character = &geometry->characters[geometry->character_count];
        geometry->character_count++;

        float half_width = 0.5f * (bounds->x1 - bounds->x0);
        float half_height = 0.5f * (bounds->y1 - bounds->y0);
        float center_x = bounds->x0 + half_width;
        float center_y = bounds->y0 + half_height;

        set_real_point2d(&character->vertices[0].position, center_x - scale * half_width, center_y - scale * half_height);
        set_real_point2d(&character->vertices[1].position, center_x + scale * half_width, center_y - scale * half_height);
        set_real_point2d(&character->vertices[2].position, center_x + scale * half_width, center_y + scale * half_height);
        set_real_point2d(&character->vertices[3].position, center_x - scale * half_width, center_y + scale * half_height);

        set_real_point2d(&character->vertices[0].texcoord, uv_scale * texture_bounds->x0, uv_scale * texture_bounds->y0);
        set_real_point2d(&character->vertices[1].texcoord, uv_scale * texture_bounds->x1, uv_scale * texture_bounds->y0);
        set_real_point2d(&character->vertices[2].texcoord, uv_scale * texture_bounds->x1, uv_scale * texture_bounds->y1);
        set_real_point2d(&character->vertices[3].texcoord, uv_scale * texture_bounds->x0, uv_scale * texture_bounds->y1);
    }
}

// @address 0x491af8 chud_draw_get_string_width
extern "C" float chud_draw_get_string_width(long, const wchar_t *string)
{
    c_chud_draw_string draw;
    c_font_cache cache;
    rectangle2d bounds;
    float width = 0.0f;
    bounds.x1 = 1000;
    bounds.x0 = 0;
    bounds.y0 = 0;
    bounds.y1 = 1000;

    draw.m_font_id = _full_screen_hud_message_font;
    draw.m_font_header = font_get_header(_full_screen_hud_message_font);
    draw.m_style = _text_style_none;
    draw.m_justification = _text_justification_left;
    draw.set_bounds(&bounds);

    if (string != 0 && string[0] != 0 && draw.precache_wchar_string_for_font(&cache, string, _full_screen_hud_message_font))
    {
        rectangle2d cursor_bounds;
        draw.compute_wchar_string_bounds(&cache, string, &bounds, &cursor_bounds);
        width = (float)(bounds.x1 - bounds.x0);
    }

    return width;
}


static __forceinline bool chud_draw_real_is_finite(float value)
{
    union { float value; long bits; } representation;
    representation.value = value;
    return (representation.bits & 0x7f800000) != 0x7f800000;
}

// 0x18b848
extern "C" bool chud_draw_try_and_get_screen_point(const render_camera *camera, const render_projection *projection,
    const rectangle2d *bounds, const float point[3], real_point2d *screen_point)
{
    bool result = false;
    if (!bounds)
        bounds = &camera->display_pixel_bounds;
    if (chud_draw_real_is_finite(point[2]) && !(point[2] >= 0.0f))
    {
        float width = (float)(bounds->x1 - bounds->x0);
        float height = (float)(bounds->y1 - bounds->y0);
        float inverse_depth = -1.0f / point[2];
        screen_point->x = (projection->projection_matrix[0][0] * point[0] + projection->projection_matrix[2][0] * point[2]) * inverse_depth;
        screen_point->y = -(projection->projection_matrix[1][1] * point[1] + projection->projection_matrix[2][1] * point[2]) * inverse_depth;
        if (!(screen_point->x < -1.0f) && !(screen_point->x > 1.0f) && !(screen_point->y < -1.0f) && !(screen_point->y > 1.0f))
            result = true;
        float x = screen_point->x;
        float y = screen_point->y;
        x = (x + 1.0f) * 0.5f;
        y = (y + 1.0f) * 0.5f;
        screen_point->x = x * width + (float)bounds->x0;
        screen_point->y = y * height + (float)bounds->y0;
    }
    else
        set_real_point2d(screen_point, 0.0f, 0.0f);
    return result;
}

// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0xbb2a8 ?angle_between_vectors3d@@YAMPBTreal_vector3d@@0@Z
static __forceinline float chud_draw_noise_period()
{
    return 1.0f;
}
template <typename t_type>
static __forceinline const t_type &chud_draw_max(const t_type &a, const t_type &b)
{
    return a < b ? b : a;
}

const float chud_draw_anchor_basis_scale = 1.0f;

// @address 0x49b908 ?chud_compute_anchor_basis_from_tracker@@YA_NPAVc_chud_point_tracker@@PAUreal_matrix4x3@@_N@Z
bool chud_compute_anchor_basis_from_tracker(c_chud_point_tracker *tracker, real_matrix4x3 *anchor_basis, bool unused_flag)
{
    bool result = true;
    real_vector3d forward = (*global_identity4x3).forward;
    real_vector3d up = (*global_identity4x3).up;
    real_vector3d left = (*global_identity4x3).left;
    real_point2d point;
    tracker->get_position_virtual(&point);
    real_point3d position;
    set_real_point3d(&position, point.x, point.y, 0.0f);
    if (tracker->get_offscreen_amount() > 0.0f) {
        set_real_vector3d(&left, point.x - g_chud_draw_globals.width * 0.5f, point.y - g_chud_draw_globals.height * 0.5f, 0.0f);
        normalize3d(&left);
        const real_vector3d global_up = { 0.0f, 0.0f, 1.0f };
        cross_product3d(&left, &global_up, &forward);
    }
    if (result) {
        anchor_basis->scale = chud_draw_anchor_basis_scale;
        anchor_basis->forward = forward;
        anchor_basis->left = left;
        anchor_basis->up = up;
        anchor_basis->position = position;
    }
    return result;
}
// 0x18b9c8
void c_curved_hud_test::inverse_transform(real_point2d *point, float *x, float *y)
{
    real_point2d normalized = *point;
    normalized.x = (normalized.x - (m_origin.x + m_half_size.i)) / m_half_size.i;
    normalized.y = (normalized.y - (m_origin.y + m_half_size.j)) / m_half_size.j;
    real_vector3d direction;
    set_real_vector3d(&direction, normalized.x / m_projection_scale.i, normalized.y / m_projection_scale.j, 1.0f);
    real_vector3d origin;
    set_real_vector3d(&origin, 0.0f, 0.0f, -m_distance);
    float a = dot_product3d(&direction, &direction);
    float b = 2.0f * direction.k * origin.k;
    float c = origin.k * origin.k - 1.0f;
    float discriminant = b * b - 4.0f * a * c;
    if (!(discriminant <= 0.0f))
    {
        float far_distance = (-b + sqrtf(discriminant)) / (2.0f * a);
        float near_distance = (-b - sqrtf(discriminant)) / (2.0f * a);
        float distance = MAX(far_distance, near_distance);
        real_vector3d intersection;
        point_from_line3d(&origin, &direction, distance, &intersection);
        normalize3d(&intersection);
        real_vector3d horizontal_center, horizontal_edge, horizontal_point, horizontal_cross;
        set_real_vector3d(&horizontal_center, m_shader_constants[0].i, 0.0f, m_shader_constants[0].k);
        set_real_vector3d(&horizontal_edge, m_shader_constants[1].i, 0.0f, m_shader_constants[1].k);
        set_real_vector3d(&horizontal_point, intersection.i, 0.0f, intersection.k);
        cross_product3d(&horizontal_point, &horizontal_center, &horizontal_cross);
        float horizontal_sign = horizontal_cross.j > 0.0f ? -1.0f : 1.0f;
        float horizontal_angle = angle_between_vectors3d(&horizontal_center, &horizontal_edge);
        *x = horizontal_sign * angle_between_vectors3d(&horizontal_center, &horizontal_point) / chud_draw_max(horizontal_angle, 0.0001f);
        real_vector3d vertical_center, vertical_edge, vertical_point, vertical_cross;
        set_real_vector3d(&vertical_center, 0.0f, m_shader_constants[0].j, m_shader_constants[0].k);
        set_real_vector3d(&vertical_edge, 0.0f, m_shader_constants[3].j, m_shader_constants[3].k);
        set_real_vector3d(&vertical_point, 0.0f, intersection.j, intersection.k);
        cross_product3d(&vertical_point, &vertical_center, &vertical_cross);
        float vertical_sign = vertical_cross.i > 0.0f ? -1.0f : 1.0f;
        float vertical_angle = angle_between_vectors3d(&vertical_center, &vertical_edge);
        *y = vertical_sign * angle_between_vectors3d(&vertical_center, &vertical_point) / chud_draw_max(vertical_angle, 0.0001f);
    }
    else
    {
        *x = 0.0f;
        *y = 0.0f;
    }
}

// 0x18cf40
unsigned long c_chud_navpoint_manager::get_managed_navpoint_arrow_color(s_managed_navpoint *navpoint)
{
    unsigned long color = 0;

    switch (navpoint->color_type)
    {
    case 0:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 15);
        break;
    case 1:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 16);
        break;
    case 2:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 17);
        break;
    case 3:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 18);
        break;
    default:
        break;
    }

    switch (navpoint->state)
    {
    case 1:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 24);
        break;
    case 2:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 25);
        break;
    case 3:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 26);
        break;
    case 4:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 27);
        break;
    default:
        break;
    }

    return color;
}

// 0x18d280
unsigned long c_chud_navpoint_manager::get_managed_navpoint_text_color(s_managed_navpoint *navpoint)
{
    unsigned long color = 0;

    switch (navpoint->color_type)
    {
    case 0:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 15);
        break;
    case 1:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 16);
        break;
    case 2:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 17);
        break;
    case 3:
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 18);
        break;
    default:
        break;
    }

    if (navpoint->state == 4)
        color = chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 27);

    return color;
}

extern "C" {
// @address 0x836908 g_rasterizer_shared_shader_cache


}

// 0x191e70
void chud_draw_set_shaders(long shader_index)
{
    D3DDevice_SetPixelShader(g_rasterizer_device, (D3DPixelShader *)g_rasterizer_shared_shader_cache.pixel_shaders[shader_index]);
    D3DDevice_SetVertexShader(g_rasterizer_device, g_rasterizer_shared_shader_cache.vertex_shaders[shader_index]);
}

// 0x472128
bool chud_user_should_draw(long user_index)
{
    bool result = true;

    if (user_index == NONE)
        result = false;
    else if (!g_director_globals->directors[user_index].get_director()->should_draw_hud())
        result = false;
    else if (inl_cinematic_in_progress())
        result = false;
    else if (chud_draw_game_skull_is_active_secondary(1))
        result = false;

    return result;
}


static __forceinline long chud_draw_unit_weapon_object(const unit_datum *unit, short weapon)
{
    long result = -1;
    if (weapon != -1)
        result = unit->unit.weapon_object_indices[weapon];
    return result;
}

extern "C" long chud_draw_is_coop_vehicle_visible(long user_index)
{
    c_director *entry = g_director_globals->directors[user_index].get_director();
    long result = 1;
    if (entry->get_camera()->get_type() == 0)
    {
        long object_index = entry->get_camera()->get_target();
        if (object_index != -1 && object_try_and_get_and_verify_type(object_index, 0x1003) != 0)
        {
            long last_index;
            long parent_index = object_index;
            do
            {
                last_index = parent_index;
                parent_index = object_get(parent_index)->parent_object_index;
            } while (parent_index != -1);
            if (last_index == object_index)
                result = 0;
            unit_datum *unit = (unit_datum *)object_get(object_index);
            short seat = unit->unit.current_weapon_index;
            if (seat != -1 && unit->unit.weapon_object_indices[seat] != -1)
            {
                long seat_object_index = chud_draw_unit_weapon_object(unit, unit->unit.current_weapon_index);
                object_datum *seat_object = object_get(seat_object_index);
                weapon_definition *definition =
                    (weapon_definition *)tag_get('weap', seat_object->definition_index);
                if ((bool)((definition->flags >> 13) & 1))
                    return 1;
            }
        }
    }
    else if (entry->get_camera()->get_type() != 3)
    {
        return 0;
    }
    return result;
}


// 0x49cec8
bool chud_apply_render_data(s_chud_render_data *render_data, bool fancy)
{
    bool result = false;
    if (render_data->pixel_shader_tag_index != NONE && render_data->vertex_shader_tag_index != NONE)
    {
        c_rasterizer::set_vertex_shader(static_cast<const c_rasterizer_vertex_shader *>(tag_get('vtsh', render_data->vertex_shader_tag_index)),
            fancy ? _vertex_type_chud_fancy : _vertex_type_chud_simple, _transfer_vector_vertex_none, _entry_point_default);
        c_rasterizer::set_pixel_shader(
        static_cast<const c_rasterizer_pixel_shader *>(tag_get('pixl', render_data->pixel_shader_tag_index)),
        _entry_point_default);
        g_rasterizer_device->SetVertexShaderConstantF(32, (const float *)&render_data->texture_transform, 1);
        g_rasterizer_device->SetPixelShaderConstantF(24, (const float *)&render_data->chud_color_outputs[0], 1);
        g_rasterizer_device->SetPixelShaderConstantF(25, (const float *)&render_data->chud_color_outputs[1], 1);
        g_rasterizer_device->SetPixelShaderConstantF(26, (const float *)&render_data->chud_color_outputs[2], 1);
        g_rasterizer_device->SetPixelShaderConstantF(27, (const float *)&render_data->chud_color_outputs[3], 1);
        g_rasterizer_device->SetPixelShaderConstantF(28, (const float *)&render_data->chud_color_outputs[4], 1);
        g_rasterizer_device->SetPixelShaderConstantF(29, (const float *)&render_data->chud_color_outputs[5], 1);
        g_rasterizer_device->SetPixelShaderConstantF(30, (const float *)&render_data->chud_scalar_output_abcd, 1);
        g_rasterizer_device->SetPixelShaderConstantF(31, (const float *)&render_data->chud_scalar_output_ef, 1);
        result = render_data->chud_scalar_output_ef.l > 0.0f;
    }
    return result;
}

// 0x49d440
float chud_render_data_fetch_scalar_output(short output, const s_chud_widget_render_data *render_data, const s_chud_runtime_widget_datum *widget)
{
    float result = 0.0f;

    switch (output)
    {
    case 0:
    case 1:
        result = widget->external_inputs[output];
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        result = render_data->scalar_outputs[output - 2];
        break;
    case 6:
    case 7:
        result = 0.0f;
        break;
    default:
        break;
    }

    return result;
}

// 0x49d4b0
bool chud_bitmap_widget_compute_bitmap_info(long group_index, long sequence_index, long sprite_index, s_chud_bitmap_info *bitmap_info)
{
    bool result = false;

    bitmap_info->group_index = NONE;
    bitmap_info->bitmap_index = NONE;
    bitmap_info->width = 1;
    bitmap_info->height = 1;
    set_real_rectangle2d(&bitmap_info->bounds, 0.0f, 1.0f, 0.0f, 1.0f);
    bitmap_info->real_width = 1.0f;
    bitmap_info->real_height = 1.0f;

    if (group_index != NONE)
    {
        bitmap_group *group = (bitmap_group *)tag_get('bitm', group_index);
        long bitmap_index = 0;
        long sprite_spacing = 0;
        real_rectangle2d bounds;
        set_real_rectangle2d(&bounds, 0.0f, 1.0f, 0.0f, 1.0f);

        if (sequence_index < group->sequences.count)
        {
            bitmap_sequence *sequence = (bitmap_sequence *)tag_block_get_element_with_size(
                &group->sequences, sequence_index, sizeof(bitmap_sequence));
            bitmap_index = MIN(MAX(sequence->first_bitmap_index, 0), group->bitmaps.count - 1);
            if (sequence->sprites.count > 0)
            {
                bitmap_sprite *sprite = (bitmap_sprite *)tag_block_get_element_with_size(&sequence->sprites, sprite_index, sizeof(bitmap_sprite));
                bounds = sprite->bounds;
                bitmap_index = sprite->bitmap_index;
                sprite_spacing = group->sprite_spacing;
            }
        }

        bitmap_data *bitmap = (bitmap_data *)tag_block_get_element_with_size(&group->bitmaps, bitmap_index, sizeof(bitmap_data));
        bitmap_info->group_index = group_index;
        bitmap_info->bitmap_index = bitmap_index;
        bitmap_info->width = bitmap->width;
        bitmap_info->height = bitmap->height;

        if (sprite_spacing)
        {
            float x_spacing = (float)sprite_spacing / (float)bitmap_info->width;
            float y_spacing = (float)sprite_spacing / (float)bitmap_info->height;
            bounds.x0 += x_spacing;
            bounds.x1 -= x_spacing;
            bounds.y0 += y_spacing;
            bounds.y1 -= y_spacing;
        }

        bitmap_info->bounds = bounds;
        bitmap_info->real_width = (float)bitmap->width * (bounds.x1 - bounds.x0);
        bitmap_info->real_height = (float)bitmap->height * (bounds.y1 - bounds.y0);
        result = true;
    }

    return result;
}

static __forceinline s_chud_animation *chud_draw_animation_get(const s_tag_block *block)
{
    return (s_chud_animation *)(block->count ? block->address : 0);
}
void chud_apply_widget_animation_definition(float time, struct s_chud_animation_definition *animation_definition,
    // match: 0x49c9d8: without __restrict the animation_data loads at +0x40 and +0x4c move after the store that precedes each
    struct real_matrix4x3 *basis, struct s_widget_geometry *geometry, struct s_chud_aux_animation_data *__restrict animation_data)
{
    float period = (float)animation_definition->get_period_msec();
    float fraction = time / period;
    fraction = ((fraction > 0.0f ? fraction : 0.0f) < 1.0f ? (fraction > 0.0f ? fraction : 0.0f) : 1.0f);
    {
        real_euler_angles3d rotation;
        chud_apply_widget_animation_definition_template<real_euler_angles3d, s_chud_keyframe_rotation>(
            chud_draw_animation_get(&animation_definition->rotation), time, fraction, period, &rotation);
        geometry->rotation = rotation;
    }
    {
        real_vector2d scale;
        chud_apply_widget_animation_definition_template<real_vector2d, s_chud_keyframe_scale>(
            chud_draw_animation_get(&animation_definition->scale), time, fraction, period, &scale);
        geometry->scale.i *= scale.i;
        geometry->scale.j *= scale.j;
    }
    {
        union { real_point3d point; real_euler_angles3d angles; } position;
        chud_apply_widget_animation_definition_template<real_euler_angles3d, s_chud_keyframe_rotation>(
            chud_draw_animation_get(&animation_definition->position), time, fraction, period, &position.angles);
        float xscale = g_chud_draw_globals.width / 1280.0f;
        float yscale = g_chud_draw_globals.height / 720.0f;
        if (animation_definition->flags & 8)
        {
            xscale = 1.0f;
            yscale = 1.0f;
        }
        position.point.x *= xscale;
        position.point.y *= yscale;
        position.point.z *= xscale > yscale ? xscale : yscale;
        if (geometry->scale.i > 0.0001f)
            position.point.x /= geometry->scale.i;
        if (geometry->scale.j > 0.0001f)
            position.point.y /= geometry->scale.j;
        geometry->position.x += position.point.x;
        geometry->position.y += position.point.y;
        geometry->position.z += position.point.z;
    }
    {
        float scalar;
        chud_apply_widget_animation_definition_template<float, s_chud_keyframe_scalar>(
            chud_draw_animation_get(&animation_definition->scalar), time, fraction, period, &scalar);
        animation_data->scalar = scalar;
    }
    {
        float scalar;
        chud_apply_widget_animation_definition_template<float, s_chud_keyframe_scalar_zero>(
            chud_draw_animation_get(&animation_definition->scalar_zero), time, fraction, period, &scalar);
        animation_data->zero = scalar;
    }
    {
        real_vector4d texture;
        chud_apply_widget_animation_definition_template<real_vector4d, s_chud_keyframe_texture>(
            chud_draw_animation_get(&animation_definition->texture), time, fraction, period, &texture);
        animation_data->texture = texture;
    }
}

void chud_compute_geometry_common(s_chud_widget_placement_data *placement, s_widget_geometry *geometry)
{
    chud_draw_compute_geometry_common_internal(geometry,
        0.5f * (placement->anchor_x + 1.0f),
        0.5f * (placement->anchor_y + 1.0f),
        placement->offset.x, placement->offset.y, placement->scale.i, placement->scale.j);
}

// 0x49d7a8
void chud_widget_build_quads_from_bounds(real_rectangle2d *bounds, real_rectangle2d *position_bounds, real_rectangle2d *texture_bounds,
    s_widget_geometry *geometry, float x_scale, float y_scale, bool extend_edges)
{
    geometry->character_count = 0;
    geometry->bounds = *bounds;
    geometry->texture_bounds = *texture_bounds;
    long width = (long)(position_bounds->x1 - position_bounds->x0);
    long height = (long)(position_bounds->y1 - position_bounds->y0);
    long columns = (long)MIN(MAX((float)width * x_scale * 0.05f, 1.0f), 12.0f);
    long rows = (long)MIN(MAX((float)height * y_scale * 0.05f, 1.0f), 8.0f);
    if (columns * rows <= 96)
    {
        float inverse_columns = 1.0f / (float)columns;
        float inverse_rows = 1.0f / (float)rows;
        long quad_index = 0;
        geometry->character_count = columns * rows;
        float x = position_bounds->x0;
        float y = position_bounds->y0;
        float width = position_bounds->x1 - position_bounds->x0;
        float height = position_bounds->y1 - position_bounds->y0;
        float u = texture_bounds->x0;
        float v = texture_bounds->y0;
        float texture_width = texture_bounds->x1 - texture_bounds->x0;
        float texture_height = texture_bounds->y1 - texture_bounds->y0;
        for (long row = 0; row < rows; row++)
        {
            float y0 = (float)row * inverse_rows;
            float y1 = (float)(row + 1) * inverse_rows;
            if (extend_edges && row == rows - 1)
                y1 += 10.0f;
            for (long column = 0; column < columns; column++)
            {
                float x0 = (float)column * inverse_columns;
                float x1 = (float)(column + 1) * inverse_columns;
                if (extend_edges && column == columns - 1)
                    x1 += 10.0f;
                set_real_point2d(&geometry->characters[quad_index].vertices[0].position, x + x0 * width, y + y0 * height);
                set_real_point2d(&geometry->characters[quad_index].vertices[1].position, x + x1 * width, y + y0 * height);
                set_real_point2d(&geometry->characters[quad_index].vertices[2].position, x + x1 * width, y + y1 * height);
                set_real_point2d(&geometry->characters[quad_index].vertices[3].position, x + x0 * width, y + y1 * height);
                set_real_point2d(&geometry->characters[quad_index].vertices[0].texcoord, u + x0 * texture_width, v + y0 * texture_height);
                set_real_point2d(&geometry->characters[quad_index].vertices[1].texcoord, u + x1 * texture_width, v + y0 * texture_height);
                set_real_point2d(&geometry->characters[quad_index].vertices[2].texcoord, u + x1 * texture_width, v + y1 * texture_height);
                set_real_point2d(&geometry->characters[quad_index].vertices[3].texcoord, u + x0 * texture_width, v + y1 * texture_height);
                quad_index++;
            }
        }
    }
}


// @address 0x49e138 ?chud_text_widget_compute_string@@YA_NPAUs_chud_draw_widget_data@@PAUs_chud_render_data@@PAV?$c_static_wchar_string@$0BAA@@@@Z
// @address 0x134408 ?chud_get_string@@YAXJJPAV?$c_static_wchar_string@$0BAA@@@@Z
// @address 0x187710 ?print@?$c_static_wchar_string@$0BAA@@@QAAPB_WPB_WZZ
// @address 0x198168 ?append_print@?$c_static_wchar_string@$0BAA@@@QAAPB_WPB_WZZ
// @address 0x1bfe40 ?set@?$c_static_wchar_string@$0BAA@@@QAAXPB_W@Z


// @address 0x49dad8 ?chud_bitmap_widget_compute_geometry@@YA_NPAUs_chud_draw_widget_data@@PAUs_widget_geometry@@@Z
bool chud_bitmap_widget_compute_geometry(s_chud_draw_widget_data *draw, s_widget_geometry *geometry)
{
    s_chud_bitmap_widget *widget = (s_chud_bitmap_widget *)draw->widget;
    bool result = false;
    s_chud_widget_placement_data *placement = widget->base.placement.count
        ? (s_chud_widget_placement_data *)tag_block_get_element_with_size(&widget->base.placement, 0, sizeof(s_chud_widget_placement_data)) : 0;
    long sequence_offset = 0;
    if (widget->flags & (1 << 5))
        sequence_offset = g_chud_draw_globals.skin_index;
    s_chud_bitmap_info bitmap_info;
    if (chud_bitmap_widget_compute_bitmap_info(widget->bitmap.index, widget->sequence_index + sequence_offset, 0, &bitmap_info) && placement)
    {
        float margin = 0.0f;
        float x_scale = 1.0f;
        float y_scale = 1.0f;
        {
            s_entry *bitmap;
            ::bitmap_group_get_xenon_hardware_format(bitmap_info.group_index, bitmap_info.bitmap_index,
                (c_rasterizer_texture_ref *)&geometry->texture_handle, (bitmap_data const **)&bitmap, (__int64 *)(0));
        }
        geometry->render_type = (widget->flags & (1 << 4)) ? 0 : 3;
        if (widget->flags & (1 << 2))
        {
            float width = g_chud_draw_globals.width / (placement->scale.i < 0.01f ? 0.01f : placement->scale.i) * 0.75f;
            float height = g_chud_draw_globals.height / (placement->scale.j < 0.01f ? 0.01f : placement->scale.j) * 0.75f;
            x_scale = width / bitmap_info.real_width;
            y_scale = height / bitmap_info.real_height;
        }
        else
        {
            margin = 2.0f;
            float x_margin = 2.0f / (float)bitmap_info.width;
            float y_margin = 2.0f / (float)bitmap_info.height;
            bitmap_info.bounds.x0 -= x_margin;
            bitmap_info.bounds.x1 += x_margin;
            bitmap_info.bounds.y0 -= y_margin;
            bitmap_info.bounds.y1 += y_margin;
        }
        real_rectangle2d bounds;
        set_real_rectangle2d(&bounds, 0.0f, bitmap_info.real_width * x_scale, 0.0f, bitmap_info.real_height * y_scale);
        real_rectangle2d position_bounds;
        set_real_rectangle2d(
            &position_bounds, -margin, bitmap_info.real_width *x_scale + margin, -margin, bitmap_info.real_height *y_scale + margin);
        real_rectangle2d texture_bounds;
        set_real_rectangle2d(
            &texture_bounds, bitmap_info.bounds.x0, bitmap_info.bounds.x1 *x_scale, bitmap_info.bounds.y0, bitmap_info.bounds.y1 *y_scale);
        chud_widget_build_quads_from_bounds(
            &bounds, &position_bounds, &texture_bounds, geometry, placement->scale.i, placement->scale.j, (widget->flags & (1 << 2)) != 0);
        chud_compute_geometry_common(placement, geometry);
        result = true;
    }
    return result;
}
static __forceinline long chud_draw_round(float value)
{
    return (long)((value > 0.0f ? 0.5f : -0.5f) + value);
}

static __forceinline float chud_draw_text_scalar(long string_id, s_chud_render_data *render)
{
        float result = 0.0f;
        switch (string_id)
        {
        case _chud_render_scalar_output_a: result = render->chud_scalar_output_abcd.i; break;
        case _chud_render_scalar_output_b: result = render->chud_scalar_output_abcd.j; break;
        case _chud_render_scalar_output_c: result = render->chud_scalar_output_abcd.k; break;
        case _chud_render_scalar_output_d: result = render->chud_scalar_output_abcd.l; break;
        case _chud_render_scalar_output_e: result = render->chud_scalar_output_ef.i; break;
        case _chud_render_scalar_output_f: result = render->chud_scalar_output_ef.j; break;
        }
    return result;
}

bool chud_text_widget_compute_string(s_chud_draw_widget_data *draw, s_chud_render_data *render, c_static_wchar_string<256> *string)
{
    bool result = false;
    s_chud_text_widget *widget = draw->widget;
    if (widget->flags & 1)
    {
        c_static_wchar_string<256> number;
        wchar_t prefix[8];
        wchar_t suffix[8];
        memset(prefix, 0, sizeof(prefix));
        memset(suffix, 0, sizeof(suffix));
        long prefix_length = 0;
        long suffix_length = 0;
        if (widget->flags & (1 << 4)) suffix[suffix_length++] = L'm';
        if (widget->flags & (1 << 9)) suffix[suffix_length++] = L'x';
        if (widget->flags & (1 << 10))
        {
            prefix[prefix_length++] = L'[';
            suffix[suffix_length++] = L']';
        }
        if (widget->flags & (1 << 3)) prefix[prefix_length++] = L'+';
        if (widget->flags & (1 << 14)) prefix[prefix_length++] = L'-';
        float value = chud_draw_text_scalar(widget->string_id, render);
        long precision = 0;
        if (widget->flags & (1 << 5)) precision = 2;
        else if (widget->flags & (1 << 6)) precision = 3;
        else if (widget->flags & (1 << 7)) precision = 5;
        if (value == 3.402823466e38f)
        {
            string->print(L"\xe065");
        }
        else if (widget->flags & (1 << 13))
        {
            c_static_wchar_string_1024 label;
            label.string[0] = 0;
            const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
            if (globals && globals->global_strings.index != -1)
                text_group_get_string(globals->global_strings.index, 1184, &label);
            string->print(L"%s%d", label.string, (long)(float)floor(value));
        }
        else if (widget->flags & (1 << 12))
        {
            long hours = (long)floorf(value * (1.0f / 3600.0f));
            long minutes = (long)(float)floor((value - (float)hours * 3600.0f) * (1.0f / 60.0f));
            long seconds = (long)(value - (float)hours * 3600.0f - (float)minutes * 60.0f);
            if (hours)
                string->print(L"%02d:%02d:%02d", hours, minutes, seconds);
            else
                string->print(L"%02d:%02d", minutes, seconds);
        }
        else if (widget->flags & (1 << 11))
        {
            long seconds = (long)(float)floor(value);
            long fraction = (long)(float)floor((value - (float)seconds) * 60.0f);
            const wchar_t *padding = fraction <= 9 ? L"0" : L"";
            string->print(L"%d:%s%d", seconds, padding, fraction);
        }
        else if (widget->flags & (1 << 8))
        {
            long rounded = chud_draw_round(value);
bool negative;
if (rounded < 0) negative = true;
else negative = false;
            long absolute = abs(rounded);
            number.print(L"%d", absolute);
            string->print(negative ? L"-" : L"");
            string->append_print(L"%d", absolute);
        }
        else
        {
            switch (precision)
            {
            case 0:
                {
                    long rounded = chud_draw_round(value);
                    if (widget->flags & 2)
                    {
                        long clamped = (rounded > -99 ? rounded : -99) < 99 ? (rounded > -99 ? rounded : -99) : 99;
                        if (clamped < 10) number.print(L"0%d", clamped);
                        else number.print(L"%d", clamped);
                    }
                    else if (widget->flags & 4)
                    {
                        long clamped = (rounded > -999 ? rounded : -999) < 999 ? (rounded > -999 ? rounded : -999) : 999;
                        if (clamped < 10) number.print(L"00%d", clamped);
                        else if (clamped < 100) number.print(L"0%d", clamped);
                        else number.print(L"%d", clamped);
                    }
                    else number.print(L"%d", rounded);
                }
                break;
            case 2: number.print(L"%.2f", value); break;
            case 3: number.print(L"%.3f", value); break;
            case 5: number.print(L"%.5f", value); break;
            }
            string->print(L"%s%s%s", prefix, number.get_string(), suffix);
        }
        result = true;
    }
    else
    {
        s_chud_persistent_display_state *persistent = &c_chud_manager::x_persistent_user_data[draw->user_index].m_display_state;
        switch (widget->string_id)
        {
        case _string_id_chud_variant_name: string->set(persistent->text_4b8); break;
        case _string_id_chud_talking_player_name: string->set(persistent->text_478); break;
        case _string_id_chud_arming_meter_name: string->set(persistent->text_438); break;
        case _string_id_chud_time_left: string->set(persistent->text_410); break;
        case _string_id_chud_training_text: string->set(persistent->player_training_text); break;
        case _string_id_chud_campaign_objective_text: string->set(persistent->cutscene_title); break;
        default: chud_get_string(draw->user_index, widget->string_id, string); break;
        }
        result = true;
    }
    return result;
}

void chud_expand_pixel32(unsigned long pixel, real_vector4d *color)
{
    const float scale = 0.003921569f;

    color->i = (float)((pixel >> 16) & 0xff) * scale;
    color->j = (float)((pixel >> 8) & 0xff) * scale;
    color->k = (float)(pixel & 0xff) * scale;
    color->l = (float)(pixel >> 24) * scale;
}



// @address 0x488460 ?begin_frame@c_chud_cortana_effect@@SAXXZ
void c_chud_cortana_effect::begin_frame()
{
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)17);
    if (g_rasterizer_alpha_blend_mode != 1)
    {
        D3DDevice *device = g_rasterizer_device;
        g_rasterizer_alpha_blend_mode = 1;
        D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
        D3DDevice_SetRenderState_BlendOp(device, 0);
        D3DDevice_SetRenderState_SrcBlend(device, 1);
        D3DDevice_SetRenderState_DestBlend(device, 1);
    }
    s_dynamic_render_target *target = c_dynamic_render_targets::get_target_from_target_type(_render_target_type_texture_camera);
    c_rasterizer::clear(15, 0, 1.0f, 0);
    if (target)
    {
        float width = g_chud_draw_globals.width;
        float height = g_chud_draw_globals.height;
        real_point3d position = *global_origin3d;
        real_vector3d up = *global_up3d;
        real_vector3d left = *global_left3d;
        float radius = (width > height ? height : width) * 1.17f;
        set_real_point3d(&position, width * 0.5f, height * 0.5f, 0.0f);
        real_rectangle2d texture_rect;
        set_real_rectangle2d(&texture_rect, -0.39999998f, 1.4f, -0.39999998f, 1.4f);
        real_rectangle2d rect;
        set_real_rectangle2d(&rect, -radius, radius, -radius, radius);
        chud_draw_cortana_effect_background(&rect, &position, &up, &left, &texture_rect, dynamic_render_targets_set_accumulation_texture, 13);
    }
}

static __forceinline real_vector4d *chud_draw_set_real_vector4d(real_vector4d *vector, float i, float j, float k, float l)
{
    vector->i = i;
    vector->j = j;
    vector->k = k;
    vector->l = l;
    return vector;
}

void c_curved_hud_test::set_constants(long)
{
    g_rasterizer_device->SetVertexShaderConstantF(24, (const float *)&m_shader_constants[0], 1);
    g_rasterizer_device->SetVertexShaderConstantF(25, (const float *)&m_shader_constants[1], 1);
    g_rasterizer_device->SetVertexShaderConstantF(26, (const float *)&m_shader_constants[2], 1);
    g_rasterizer_device->SetVertexShaderConstantF(27, (const float *)&m_shader_constants[3], 1);
    real_vector4d origin_scale;
    chud_draw_set_real_vector4d(&origin_scale, m_origin.x, m_half_size.i, m_origin.y, m_half_size.j);
    real_vector4d distance_projection;
    chud_draw_set_real_vector4d(&distance_projection, m_projection_scale.i, m_projection_scale.j, m_distance, m_radians_per_pixel);
    g_rasterizer_device->SetVertexShaderConstantF(28, (const float *)&origin_scale, 1);
    g_rasterizer_device->SetVertexShaderConstantF(29, (const float *)&distance_projection, 1);
    s_chud_draw_globals *globals = &g_chud_draw_globals;
    real_vector4d viewport;
    chud_draw_set_real_vector4d(&viewport,
        (float)(int)(globals->right - globals->left),
        (float)(int)(globals->bottom - globals->top),
        globals->width, globals->height);
    g_rasterizer_device->SetVertexShaderConstantF(23, (const float *)&viewport, 1);
    real_vector4d screenspace;
    chud_draw_set_real_vector4d(&screenspace, g_tiled_render_x_scale, g_tiled_render_y_scale, g_tiled_render_x_offset, g_tiled_render_y_offset);
    g_rasterizer_device->SetVertexShaderConstantF(31, (const float *)&screenspace, 1);
}

void chud_build_custom_render_data(s_chud_render_data *render_data, long shader_index)
{
    memset(render_data, 0, sizeof(*render_data));
    s_tag_reference *vertex_shader = NULL;
    if (shader_index >= 0)
    {
        if (shader_index < chud_globals->shader_count)
            vertex_shader = &(chud_globals->shaders)[shader_index].vertex_shader;
    }
    s_tag_reference *pixel_shader = NULL;
    if (shader_index >= 0)
    {
        if (shader_index < chud_globals->shader_count)
            pixel_shader = &(chud_globals->shaders)[shader_index].pixel_shader;
    }
    render_data->vertex_shader_tag_index = vertex_shader ? vertex_shader->index : -1;
    render_data->pixel_shader_tag_index = pixel_shader ? pixel_shader->index : -1;
    render_data->chud_scalar_output_ef.l = g_chud_draw_globals.global_hud_alpha;
    chud_draw_set_real_vector4d(&render_data->texture_transform, 1.0f, 1.0f, 0.0f, 0.0f);
}

// 0x49f880
void c_chud_cortana_effect::set_constants(float blend, float amount, float fade)
{
    real_vector4d vertex_constant2;
    chud_draw_set_real_vector4d(&vertex_constant2,
        m_scale * (m_vertex_bounds[0].lower + blend * (m_vertex_bounds[0].upper - m_vertex_bounds[0].lower)),
        m_scale * (m_vertex_bounds[1].lower + blend * (m_vertex_bounds[1].upper - m_vertex_bounds[1].lower)),
        m_scale * (m_vertex_bounds[3].lower + blend * (m_vertex_bounds[3].upper - m_vertex_bounds[3].lower)),
        m_scale * (m_vertex_bounds[4].lower + blend * (m_vertex_bounds[4].upper - m_vertex_bounds[4].lower)));

    real_vector4d vertex_constant0;
    chud_draw_set_real_vector4d(&vertex_constant0, m_vertex_constant0.i, m_vertex_constant0.j, m_vertex_constant0.k, m_vertex_constant0.l);
    real_vector4d vertex_constant1;
    chud_draw_set_real_vector4d(&vertex_constant1, m_vertex_constant1.position.x, m_vertex_constant1.position.y,
        m_vertex_constant1.velocity.i, m_vertex_constant1.velocity.j);
    real_vector4d vertex_constant3;
    chud_draw_set_real_vector4d(&vertex_constant3, m_vertex_constant3.i, m_vertex_constant3.j, m_vertex_constant3.k, m_vertex_constant3.l);
    real_vector4d vertex_constant4;
    chud_draw_set_real_vector4d(&vertex_constant4, m_vertex_constant4.position.x, m_vertex_constant4.position.y,
        m_vertex_constant4.velocity.i, m_vertex_constant4.velocity.j);

    g_rasterizer_device->SetVertexShaderConstantF(35, (const float *)&vertex_constant0, 1);
    g_rasterizer_device->SetVertexShaderConstantF(36, (const float *)&vertex_constant1, 1);
    g_rasterizer_device->SetVertexShaderConstantF(37, (const float *)&vertex_constant2, 1);
    g_rasterizer_device->SetVertexShaderConstantF(38, (const float *)&vertex_constant3, 1);
    g_rasterizer_device->SetVertexShaderConstantF(39, (const float *)&vertex_constant4, 1);

    real_vector4d pixel_constant0;
    chud_draw_set_real_vector4d(&pixel_constant0, amount, 1.0f - fade, m_pixel_constant_z, m_pixel_constant_w);
    real_vector4d pixel_constant1;
    chud_draw_set_real_vector4d(&pixel_constant1,
        m_scale * (m_pixel_bounds[0].lower + blend * (m_pixel_bounds[0].upper - m_pixel_bounds[0].lower)),
        m_scale * (m_pixel_bounds[1].lower + blend * (m_pixel_bounds[1].upper - m_pixel_bounds[1].lower)),
        m_scale * (m_pixel_bounds[2].lower + blend * (m_pixel_bounds[2].upper - m_pixel_bounds[2].lower)),
        m_scale * (m_pixel_bounds[3].lower + blend * (m_pixel_bounds[3].upper - m_pixel_bounds[3].lower)));
    real_vector4d pixel_constant2;
    chud_draw_set_real_vector4d(&pixel_constant2,
        m_scale * (m_pixel_bounds[4].lower + blend * (m_pixel_bounds[4].upper - m_pixel_bounds[4].lower)),
        m_scale * (m_pixel_bounds[5].lower + blend * (m_pixel_bounds[5].upper - m_pixel_bounds[5].lower)),
        m_pixel_constant_tail, 0.0f);

    g_rasterizer_device->SetPixelShaderConstantF(38, (const float *)&pixel_constant0, 1);
    g_rasterizer_device->SetPixelShaderConstantF(39, (const float *)&pixel_constant1, 1);
    g_rasterizer_device->SetPixelShaderConstantF(40, (const float *)&pixel_constant2, 1);
}


void c_chud_cortana_effect::update_noise(float blend)
{
    if (!game_time_is_paused())
    {
        float noise = m_noise_bounds.lower + blend * (m_noise_bounds.upper - m_noise_bounds.lower);
        float vertex_noise = m_vertex_bounds[2].lower + blend * (m_vertex_bounds[2].upper - m_vertex_bounds[2].lower);
        m_pixel_constant_z = m_pixel_constant_z + noise / (float)game_tick_rate();
        m_pixel_constant_w = m_pixel_constant_w + vertex_noise / (float)game_tick_rate();
    }
    if (m_pixel_constant_z > chud_draw_noise_period())
    {
        memcpy(&m_vertex_constant0, &m_vertex_constant1, sizeof(m_vertex_constant0));
        chud_cortana_effect_new_random_basis(&m_vertex_constant1);
        float value = m_pixel_constant_z;
        m_pixel_constant_z = (float)fmod((double)value, 1.0);
    }
    if (m_pixel_constant_w > chud_draw_noise_period())
    {
        memcpy(&m_vertex_constant3, &m_vertex_constant4, sizeof(m_vertex_constant3));
        chud_cortana_effect_new_random_basis(&m_vertex_constant4);
        float value = m_pixel_constant_w;
        m_pixel_constant_w = (float)fmod((double)value, 1.0);
    }
}

// @address 0x49fd48 chud_draw_cortana_effect_background
void chud_draw_cortana_effect_background(real_rectangle2d *rect, const real_point3d *position, const real_vector3d *up,
    const real_vector3d *left, real_rectangle2d *texture_rect, void (*callback)(), long shader_index)
{
    s_widget_geometry geometry;
    geometry.texture_handle = -1;
    real_matrix4x3 basis;
    basis.position = *position;
    basis.scale = 1.0f;
    basis.forward = (*global_identity4x3).forward;
    basis.left = (*global_identity4x3).left;
    basis.up = (*global_identity4x3).up;
    s_chud_render_data render_data;
    chud_build_custom_render_data(&render_data, shader_index);
    render_data.chud_color_outputs[0].i = 0.0f;
    render_data.chud_color_outputs[0].j = 0.3f;
    render_data.chud_color_outputs[0].k = 0.6f;
    render_data.chud_color_outputs[0].l = 0.9f;
    chud_widget_build_quads_from_bounds(rect, rect, texture_rect, &geometry, 1.0f, 1.0f, false);
    geometry.position.x = 0.0f;
    geometry.position.y = 0.0f;
    geometry.position.z = 0.0f;
    geometry.scale.i = 1.0f;
    geometry.texture_handle = -1;
    geometry.scale.j = 1.0f;
    geometry.offset.x = 0.0f;
    geometry.render_type = 3;
    geometry.offset.y = 0.0f;
    geometry.offset.z = 0.0f;
    geometry.rotation.yaw = 0.0f;
    geometry.rotation.pitch = 0.0f;
    geometry.rotation.roll = 0.0f;
    if (chud_apply_render_data(&render_data, false)) {
        callback();
        chud_draw_widget_geometry(&geometry, &basis, false, false);
    }
}

// @rdata 0x82022518
static const float chud_draw_strip_red = 0.3f;
// @rdata 0x82022514
static const float chud_draw_strip_green = 0.6f;
// @rdata 0x820265bc
static const float chud_draw_strip_blue = 0.9f;
static __forceinline real_vector3d *chud_draw_strip_negate(const real_vector3d *vector, real_vector3d *result)
{
    result->i = -vector->i;
    result->j = -vector->j;
    result->k = -vector->k;
    return result;
}
static __forceinline real_vector3d *chud_draw_strip_cross(const real_vector3d *a, const real_vector3d *b, float bi, real_vector3d *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * bi - a->i * b->k;
    result->k = a->i * b->j - a->j * bi;
    return result;
}
// @address 0x49fec8 chud_draw_cinematic_strip
extern "C" void chud_draw_cinematic_strip(long strip_index, long, float p2, float p3, s_widget_geometry *geometry)
{
    real_matrix4x3 basis;
    float dist;
    {
    real_vector3d forward;
    real_vector3d left;
    real_vector3d up;
    real_point3d position;
    forward = (*global_identity4x3).forward;
    up = (*global_identity4x3).up;
    left = (*global_identity4x3).left;
    float angle = (float)strip_index * 0.12566371f;
    float sx = (float)sin((double)angle);
    float half_width = g_chud_draw_globals.width * 0.5f;
    sx *= half_width;
    float hcos = (float)cos((double)angle);
    half_width = g_chud_draw_globals.width * 0.5f;
    float x = sx + half_width;
    float half_height = g_chud_draw_globals.height * 0.5f;
    hcos *= half_height;
    float y = hcos + half_height;
    set_real_point3d(&position, x, y, 0.0f);
    set_real_vector3d(&left, x - half_width, y - half_height, 0.0f);
    dist = sqrtf(hcos * hcos + sx * sx);
    normalize3d(&left);
    chud_draw_strip_negate(&left, &left);
    basis.scale = 1.0f;
    basis.left = left;
    basis.up = up;
    chud_draw_strip_cross(&left, &up, up.i, &forward);
    basis.forward = forward;
    basis.position = position;
    }
    s_chud_render_data render_data;
    chud_build_custom_render_data(&render_data, 11);
    {
    float v = dist * p3 * 0.45f;
    render_data.chud_color_outputs[0].i = 0.0f;
    geometry->position.x = 0.0f;
    geometry->position.y = 0.0f;
    geometry->position.z = 0.0f;
    geometry->offset.x = 0.0f;
    geometry->offset.z = 0.0f;
    geometry->rotation.yaw = 0.0f;
    geometry->rotation.pitch = 0.0f;
    geometry->texture_handle = -1;
    render_data.chud_color_outputs[0].j = chud_draw_strip_red;
    render_data.chud_color_outputs[0].k = chud_draw_strip_green;
    render_data.chud_color_outputs[0].l = chud_draw_strip_blue;
    geometry->render_type = 1;
    geometry->rotation.roll = 0.0f;
    geometry->scale.i = v;
    geometry->scale.j = v;
    geometry->offset.y = (float)((1.0 - p2) * -dist);
    const bitmap_data *bitmap_entry;
    ::bitmap_group_get_xenon_hardware_format(chud_globals->cinematic_strip_resource.index, 0,
        (::c_rasterizer_texture_ref *)&geometry->texture_handle, (::bitmap_data const **)&bitmap_entry, (__int64 *)0);
    geometry->render_type = 1;
    if (chud_apply_render_data(&render_data, false))
        chud_draw_widget_geometry(geometry, &basis, false, false);
    }
}

void chud_cortana_effect_new_random_basis(s_chud_cortana_effect_basis *basis)
{
    basis->position.x = _real_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
    basis->position.y = _real_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
    do
    {
        basis->velocity.i = _real_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
        basis->velocity.j = _real_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
    }
    while (normalize2d(&basis->velocity) == 0.0f);
    basis->velocity.i *= 0.2f;
    basis->velocity.j *= 0.2f;
}
float chud_build_geometry_from_navpoint(
    long user_index, long navpoint_index, long sprite_index, float rotation, float scale, struct s_widget_geometry *geometry)
{
    chud_table_row *definition = &(chud_globals->rows)[chud_get_user_skin_index(user_index)];
    s_chud_bitmap_info bitmap_info;
    chud_bitmap_widget_compute_bitmap_info(definition->navpoint_bitmap.index, navpoint_index, sprite_index, &bitmap_info);
    real_rectangle2d bounds;
    set_real_rectangle2d(&bounds, -bitmap_info.real_width * 0.5f, bitmap_info.real_width * 0.5f, 0.0f, bitmap_info.real_height);
    real_rectangle2d texture_bounds = bitmap_info.bounds;
    chud_widget_build_quads_from_bounds(&bounds, &bounds, &texture_bounds, geometry, 1.0f, 1.0f, false);
    chud_draw_compute_geometry_common_internal(geometry, 0.5f, 1.0f, 0.0f, rotation, scale, scale);
    geometry->texture_handle = -1;
    if (definition->navpoint_arrow_bitmap.index != -1)
    {
        const ::bitmap_data *bitmap_data;
        ::bitmap_group_get_xenon_hardware_format(definition->navpoint_bitmap.index, 0, (c_rasterizer_texture_ref *)&geometry->texture_handle,
            &bitmap_data, 0);
        geometry->render_type = 3;
    }
    return bitmap_info.real_height * scale;
}

extern "C" void chud_draw_get_normalized_screen_point(long user_index, real_point2d *screen_point)
{
    real_point2d viewport_point;
    real_point3d point;
    point.x = 0.0f;
    s_player_globals *observer_globals = g_observer_globals;
    c_player_view *view = c_player_view::get_current();
    point.z = -1.0f;
    // match: 0x4a0958: without __restrict the window_pixel_bounds halfword loads at 0x34/0x36 swap order and the two subtracts swap operands
    render_camera * __restrict camera = &view->rasterizer_camera;
    rectangle2d *bounds = &camera->window_pixel_bounds;
    point.y = -observer_globals->blocks[user_index].camera_result.direction_error;
    chud_draw_try_and_get_screen_point(camera, &view->rasterizer_projection, bounds, point.n, &viewport_point);
    float fv1;
    float fv2;
    if (camera->frustum_adjustment)
    {
        float y_scale = camera->frustum_scale.y;
        float x_scale = camera->frustum_scale.x;
        float half = 0.5f;
        float width = (float)(camera->window_pixel_bounds.x1 - camera->window_pixel_bounds.x0);
        float height = (float)(camera->window_pixel_bounds.y1 - bounds->y0);
        fv1 = (viewport_point.x - ((half - camera->frustum_center.x / x_scale * half) * width)) * x_scale + (width * half);
        fv2 = (viewport_point.y - (((camera->frustum_center.y / y_scale + 1.0f) * half) * height)) * y_scale + (height * half);
    }
    else
    {
        fv2 = viewport_point.y;
        fv1 = viewport_point.x;
    }
    short x0 = camera->window_pixel_bounds.x0;
    short y0 = camera->window_pixel_bounds.y0;
    short x1 = camera->window_pixel_bounds.x1;
    short y1 = camera->window_pixel_bounds.y1;
    screen_point->x = (fv1 - (float)x0) / (float)(x1 - x0);
    screen_point->y = (fv2 - (float)y0) / (float)(y1 - y0);
}



static __forceinline long chud_draw_font_to_font_id(short font)
{
    long result = -1;
    switch (font)
    {
    case _chud_widget_text_font_full_screen_hud_message: result = _full_screen_hud_message_font; break;
    case _chud_widget_text_font_split_screen_hud_message: result = _split_screen_hud_message_font; break;
    case _chud_widget_text_font_terminal: result = _terminal_font; break;
    case _chud_widget_text_font_body_text: result = _body_text_font; break;
    case _chud_widget_text_font_title: result = _title_font; break;
    case _chud_widget_text_font_super_large: result = _super_large_font; break;
    case _chud_widget_text_font_large_body_text: result = _large_body_font; break;
    case _chud_widget_text_font_english_body_text: result = _english_body_text_font; break;
    case _chud_widget_text_font_hud_number: result = _hud_number_font; break;
    case _chud_widget_text_font_subtitle: result = _subtitle_font; break;
    case _chud_widget_text_font_main_menu: result = _main_menu_font; break;
    // match: 0x49e000: __assume(0) drops the cmplwi r11,10 / bgtlr range check (and its li r3,-1) ahead of the jump table
    default: __assume(0);
    }
    return result;
}

bool chud_text_widget_compute_geometry(s_chud_draw_widget_data *draw, s_widget_geometry *geometry, s_chud_render_data *render)
{
    s_chud_text_widget *widget = draw->widget;
    s_chud_widget_placement_data *placement =
        widget->base.placement.count
            ? (s_chud_widget_placement_data *)tag_block_get_element_with_size(
                &widget->base.placement, 0, sizeof(s_chud_widget_placement_data))
            : 0;
    bool result = false;
    if (placement)
    {
        c_static_wchar_string<256> string;
        if (chud_text_widget_compute_string(draw, render, &string))
        {
            long font = chud_draw_font_to_font_id(widget->font);
            if (chud_compute_text_geometry((chud_navpoints_widget_geometry *)geometry, string.get_string(), font, false, 1.0f, 0))
            {
                chud_compute_geometry_common(placement, geometry);
                result = true;
            }
        }
    }
    return result;
}

static __forceinline void chud_draw_blip_shape_get_bounds(s_chud_globals_definition *table, unsigned char shape, real_rectangle2d *bounds)
{
    if (chud_globals != 0 && table->blip_shape.index != -1)
    {
        bitmap_group *definition = (bitmap_group *)tag_get('blip', table->blip_shape.index);
        bitmap_sequence *blip_shape = (bitmap_sequence *)tag_block_get_element_with_size(&definition->sequences, shape, sizeof(bitmap_sequence));
        bitmap_sprite *shape_bounds = (bitmap_sprite *)tag_block_get_element_with_size(&blip_shape->sprites, 0, sizeof(bitmap_sprite));
        *bounds = shape_bounds->bounds;
    }
    else
    {
        set_real_rectangle2d(bounds, 0.0f, 1.0f, 0.0f, 1.0f);
    }
}
static __forceinline unsigned long chud_draw_blip_skin_color(s_chud_globals_definition *table, long skin_index, long color_index)
{
    unsigned long color = 0;
    skin_index = MIN(MAX(skin_index, 0), table->row_count - 1);
    color_index = MIN(MAX(color_index, 0), 27);
    if (table->row_count > 0)
        color = table->rows[skin_index].colors[color_index];
    return color;
}
// 0x18be78
// @address 0x18be78 ?history_build_geometry@c_chud_motion_sensor@@AAAJJMPAUs_widget_geometry_quad_fancy@@J@Z
long c_chud_motion_sensor::history_build_geometry(long history_index, float fraction, s_widget_geometry_quad_fancy *quad, long)
{
    s_motion_sensor_contact *history = &m_contacts[history_index];
    long drawn = 0;
    const s_chud_curvature_info *view_scale = g_chud_draw_globals.curvature_info;
    const float blip_scale = view_scale->blip_scale;
    const float blip_forward_scale = view_scale->blip_forward_scale;
    real_point2d position = g_chud_draw_globals.origin;

    float rfT0 = chud_globals->blip_radius[5];
    unsigned long alpha_normal = (unsigned long)((float)pow((double)fraction, (double)rfT0) * 255.0f);
    unsigned long alpha_fade = (unsigned long)((float)pow((double)fraction, 0.5) * 255.0f);

    float rfT1 = chud_globals->blip_radius[4];
    float fade_inv = (float)pow((double)fraction, (double)rfT1);
    float fade_mid = 1.0f - fade_inv;
    float forward_x_normal = (float)((double)(chud_globals->blip_radius[3] * fade_mid) + (double)fade_inv);
    float forward_x_fade = (float)((double)(fade_mid * 3.0f) + (double)fade_inv);

    float forward_scale = 1.0f;
    float range_scale = m_sensor_range * 3.0479999f;
    if (fabs(range_scale - 10.0f) < 1.0f)
        forward_scale = 2.0f;
    if (fabs(range_scale - 75.0f) < 1.0f)
        forward_scale = 0.5f;
    if (fabs(range_scale - 150.0f) < 1.0f)
        forward_scale = 0.33f;

    for (long i = 0; i < history->active; i++)
    {
        if (drawn < 100)
        {
            s_motion_sensor_blip *blip = &history->blips[i];
            ++drawn;
            s_widget_geometry_quad_fancy *current_quad = quad++;
            blip_build_geometry(current_quad, blip,
                blip->intensity == 7 ? alpha_fade : alpha_normal,
                blip->intensity == 7 ? forward_x_fade : forward_x_normal,
                forward_scale * blip_forward_scale,
                &position, blip_scale);
        }
    }
    return drawn;
}

// 0x18c068
void c_chud_motion_sensor::blip_build_geometry(s_widget_geometry_quad_fancy *quad, s_motion_sensor_blip *blip, unsigned long alpha,
    float forward_x, float forward_y, real_point2d *position, float scale)
{
    float radius = chud_globals->blip_radius[blip->size] * forward_x * forward_y;
    s_chud_globals_definition *table = chud_globals;

    real_rectangle2d bounds;
    chud_draw_blip_shape_get_bounds(table, blip->shape, &bounds);

    if (blip->intensity == 6)
    {
        real_vector3d direction;
        real_vector3d perpendicular;
        const real_vector3d up = { 0.0f, 0.0f, 1.0f };
        set_real_vector3d(&direction, blip->x, blip->y, 0.0f);
        cross_product3d(&direction, &up, &perpendicular);
        set_real_point3d(&quad->vertices[1].position, position->x + blip->x * scale, position->y + blip->y * scale, 0.0f);
        point_from_line3d(&quad->vertices[1].position, &perpendicular, -radius, &quad->vertices[1].position);
        point_from_line3d(&quad->vertices[1].position, &direction, radius * 2.0f, &quad->vertices[2].position);
        point_from_line3d(&quad->vertices[2].position, &perpendicular, radius * 2.0f, &quad->vertices[3].position);
        point_from_line3d(&quad->vertices[3].position, &direction, radius * -2.0f, &quad->vertices[0].position);
    }
    else
    {
        // match: 0x18c068: without __restrict the center->x/y loads from 0(r5)/4(r5) move and the vertex floats reallocate (fr12 becomes fr13)
        real_point2d *__restrict center = position;
        set_real_point3d(&quad->vertices[0].position, center->x + blip->x * scale - radius, center->y + blip->y * scale - radius, 0.0f);
        set_real_point3d(&quad->vertices[1].position, center->x + blip->x * scale + radius, center->y + blip->y * scale - radius, 0.0f);
        set_real_point3d(&quad->vertices[2].position, center->x + blip->x * scale + radius, center->y + blip->y * scale + radius, 0.0f);
        set_real_point3d(&quad->vertices[3].position, center->x + blip->x * scale - radius, center->y + blip->y * scale + radius, 0.0f);
    }

    set_real_point2d(&quad->vertices[0].texcoord, bounds.x0, bounds.y0);
    set_real_point2d(&quad->vertices[1].texcoord, bounds.x1, bounds.y0);
    set_real_point2d(&quad->vertices[2].texcoord, bounds.x1, bounds.y1);
    set_real_point2d(&quad->vertices[3].texcoord, bounds.x0, bounds.y1);

    unsigned long color;
    switch (blip->intensity)
    {
    case 0: color = chud_draw_blip_skin_color(table, g_chud_draw_globals.skin_index, 7); break;
    case 1: color = chud_draw_blip_skin_color(table, g_chud_draw_globals.skin_index, 8); break;
    case 4: color = chud_draw_blip_skin_color(table, g_chud_draw_globals.skin_index, 10); break;
    case 2: color = chud_draw_blip_skin_color(table, g_chud_draw_globals.skin_index, 11); break;
    case 3: color = chud_draw_blip_skin_color(table, g_chud_draw_globals.skin_index, 9); break;
    case 7: color = chud_draw_blip_skin_color(table, g_chud_draw_globals.skin_index, 12); break;
    case 5: color = chud_draw_blip_skin_color(table, g_chud_draw_globals.skin_index, 13); break;
    case 6: color = chud_draw_blip_skin_color(table, g_chud_draw_globals.skin_index, 14); break;
    default: break;
    }
    color |= alpha << 24;
    quad->vertices[0].color = color;
    quad->vertices[1].color = color;
    quad->vertices[2].color = color;
    quad->vertices[3].color = color;
}

static __forceinline chud_table_row *chud_draw_get_medal_skin(long user_index)
{
    return &(chud_globals->rows)[chud_get_user_skin_index(user_index)];
}
// @address 0x4a07d0 chud_build_medal_geometry
extern "C" void chud_build_medal_geometry(long user_index, long medal_index, s_widget_geometry *geometry)
{
    chud_table_row *definition = chud_draw_get_medal_skin(user_index);
    s_chud_globals_definition *globals = chud_globals;
    game_globals_storage *game = game_globals;
    const ::bitmap_data *bitmap_data;
    s_chud_bitmap_info info;
    if (game->options.game_mode == 1)
        chud_bitmap_widget_compute_bitmap_info(globals->medal_bitmap.index, medal_index, 0, &info);
    else
        chud_bitmap_widget_compute_bitmap_info(definition->medal_bitmap.index, medal_index, 0, &info);
    real_rectangle2d bounds;
    set_real_rectangle2d(&bounds, 0.0f, info.real_width, 0.0f, info.real_height);
    real_rectangle2d texture_bounds = info.bounds;
    chud_widget_build_quads_from_bounds(&bounds, &bounds, &texture_bounds, geometry, 1.0f, 1.0f, false);
    geometry->position.z = 0.0f;
    geometry->scale.i = 1.0f;
    geometry->scale.j = 0.5f;
    geometry->offset.x = 0.0f;
    geometry->offset.y = 0.0f;
    geometry->offset.z = 0.0f;
    geometry->rotation.yaw = 0.0f;
    geometry->rotation.pitch = 0.0f;
    geometry->rotation.roll = 0.0f;
    geometry->texture_handle = -1;
    geometry->render_type = 3;
    geometry->position.x = (geometry->x1 - geometry->x0) * 0.5f + geometry->x0;
    geometry->position.y = (geometry->y1 - geometry->y0) * 0.5f + geometry->y0;
    if (game->options.game_mode == 1)
        ::bitmap_group_get_xenon_hardware_format(globals->medal_bitmap.index, 0,
            (c_rasterizer_texture_ref *)&geometry->texture_handle, &bitmap_data, 0);
    else
        ::bitmap_group_get_xenon_hardware_format(definition->medal_bitmap.index, 0,
            (c_rasterizer_texture_ref *)&geometry->texture_handle, &bitmap_data, 0);
}

// @address 0x49c6e8 ?chud_apply_widget_animation@@YAXPAUs_chud_draw_widget_data@@PAUreal_matrix4x3@@PAUs_widget_geometry@@PAUs_chud_render_data@@@Z
void chud_apply_widget_animation(s_chud_draw_widget_data *draw, real_matrix4x3 *basis, s_widget_geometry *geometry, s_chud_render_data *render)
{
    if (draw->widget->base.animations.count && draw->widget->base.animations.address)
    {
        s_chud_runtime_widget_datum *runtime = draw->runtime;
        long animation_index;
        switch (runtime->state)
        {
        case 1: animation_index = 0; break;
        case 2: animation_index = 1; break;
        case 3: animation_index = 2; break;
        case 4: animation_index = 3; break;
        case 5: animation_index = 4; break;
        case 6: animation_index = 5; break;
        // match: 0x49c6e8: unreachable hint omits dead branch check
        default: __assume(0);
        }
        s_chud_widget_animation_entry *reference = &static_cast<s_chud_widget_animation_entry *>(draw->widget->base.animations.address)[animation_index];
        if (reference->animation.index != NONE)
        {
            s_chud_aux_animation_data aux;
            aux.scalar = 1.0f;
            aux.zero = 0.0f;
            s_chud_animation_definition *definition = static_cast<s_chud_animation_definition *>(
                global_tag_instances[datum_index_to_absolute_index(reference->animation.index)].definition);
            float period = (float)definition->get_period_msec();
            float time = 0.0f;
            switch ((unsigned long)reference->input)
            {
            case 0: time = (float)runtime->state_time; break;
            case 1: time = runtime->external_inputs[0] * 1000.0f; break;
            case 2: time = runtime->external_inputs[1] * 1000.0f; break;
            }
            if (!(period > 0.0f))
                time = 0.0f;
            else if (definition->flags & 2)
                time = (float)fmod(time, period);
            else if (definition->flags & 4)
            {
                float cycle = period * 2.0f;
                time = (float)fmod(time, cycle);
                if (!(time < period))
                    time = cycle - time;
            }
            if (reference->flags & 1)
            {
                if (!(time <= period))
                    time = period;
                time = period - time;
            }
            if (!(time >= 0.0f))
                time = 0.0f;
            ::chud_apply_widget_animation_definition(time, definition, basis,
                (s_widget_geometry *)geometry, &aux);
            if (render)
            {
                if (aux.zero != 0.0f && draw->widget->base.render_data.count && draw->widget->base.render_data.address)
                {
                    s_chud_widget_render_data *widget_render = static_cast<s_chud_widget_render_data *>(draw->widget->base.render_data.address);
                    for (long index = 0; index < 6; ++index)
                    {
                        if (widget_render->scalar_inputs[index] == 6)
                        {
                            switch (index)
                            {
                            case 0:
                            case 1:
                            case 2:
                            case 3: render->chud_scalar_output_abcd.n[index] = aux.zero; break;
                            case 4:
                            case 5: render->chud_scalar_output_ef.n[index - 4] = aux.zero; break;
                            }
                        }
                    }
                }
                render->texture_transform = aux.texture;
                if (aux.scalar != 1.0f)
                    render->chud_scalar_output_ef.l *= aux.scalar;
            }
        }
    }
}

// ---- 0x49ec38 chud_geometry_build_transform ----


// @address 0x49ec38 ?chud_geometry_build_transform@@YAXPAUs_widget_geometry@@PAUreal_matrix4x3@@@Z
void chud_geometry_build_transform(s_widget_geometry *geometry, real_matrix4x3 *matrix)
{
    real_matrix3x3 rotation;
    matrix3x3_rotation_from_angles(&rotation, geometry->rotation.yaw, geometry->rotation.pitch, geometry->rotation.roll);
    matrix->forward = (*global_identity4x3).forward;
    matrix->left = (*global_identity4x3).left;
    matrix->up = (*global_identity4x3).up;
    matrix->position = geometry->offset;
    matrix->scale = 1.0f;
    scale_vector3d(&matrix->forward, geometry->scale.i, &matrix->forward);
    scale_vector3d(&matrix->left, geometry->scale.j, &matrix->left);
    matrix3x3_transform_vector(&rotation, &matrix->forward, &matrix->forward);
    matrix3x3_transform_vector(&rotation, &matrix->left, &matrix->left);
    matrix3x3_transform_vector(&rotation, &matrix->up, &matrix->up);
}

// @address 0x49ed78 ?chud_draw_widget_geometry@@YAXPAUs_widget_geometry@@PAUreal_matrix4x3@@_N2@Z
void chud_draw_widget_geometry(s_widget_geometry *widget_geometry, real_matrix4x3 *anchor_basis, bool p2, bool p3)
{
    real_matrix4x3 matrix;
    chud_geometry_build_transform(widget_geometry, &matrix);
    matrix4x3_compute_product(anchor_basis, &matrix, &matrix);
    D3DVertexDeclaration *declaration = rasterizer_get_vertex_declaration(_vertex_type_chud_simple, _transfer_vector_vertex_none, _entry_point_default);
    D3DDevice_SetVertexDeclaration(g_rasterizer_device, declaration);
    D3DDevice *device = g_rasterizer_device;
    real_vector4d texture_constant;
    bool has_texture = ((c_rasterizer_texture_ref *)&widget_geometry->texture_handle)->valid();
    if (has_texture) {
        c_rasterizer::set_sampler_texture(0, c_rasterizer_texture_ref(widget_geometry->texture_handle));
        long address_mode;
        if (!p2 && !p3)
            address_mode = widget_geometry->render_type;
        else
            address_mode = 1;
        c_rasterizer::set_sampler_address_mode(0, (c_rasterizer::e_sampler_address_mode)(address_mode),
            (c_rasterizer::e_sampler_address_mode)(address_mode), (c_rasterizer::e_sampler_address_mode)(address_mode));
        if (g_rasterizer_sampler_filter_modes[0] != 1) {
            g_rasterizer_sampler_filter_modes[0] = 1;
            rasterizer_device_set_sampler_filter_modes(g_rasterizer_device, 0, 1, 1);
        }
    }
    real_vector4d position_constant;
    position_constant.i = widget_geometry->position.x;
    position_constant.j = widget_geometry->position.y;
    position_constant.k = -widget_geometry->position.z;
    position_constant.l = 0.0f;
    texture_constant.i = widget_geometry->texture_bounds.x0;
    texture_constant.j = widget_geometry->texture_bounds.x1;
    texture_constant.k = widget_geometry->texture_bounds.y0;
    texture_constant.l = widget_geometry->texture_bounds.y1;
    real_vector4d transform[3];
    transform[0].i = matrix.forward.i;
    transform[0].j = matrix.left.i;
    transform[0].k = matrix.up.i;
    transform[0].l = matrix.position.x;
    transform[1].i = matrix.forward.j;
    transform[1].j = matrix.left.j;
    transform[1].k = matrix.up.j;
    transform[1].l = matrix.position.y;
    transform[2].i = matrix.forward.k;
    transform[2].j = matrix.left.k;
    transform[2].k = matrix.up.k;
    transform[2].l = matrix.position.z;
    g_rasterizer_device->SetVertexShaderConstantF(19, (const float *)&position_constant, 1);
    g_rasterizer_device->SetVertexShaderConstantF(20, (const float *)transform, 3);
    g_rasterizer_device->SetPixelShaderConstantF(32, (const float *)&texture_constant, 1);
    normalize3d(&transform[0].xyz);
    normalize3d(&transform[1].xyz);
    normalize3d(&transform[2].xyz);
    g_rasterizer_device->SetPixelShaderConstantF(50, (const float *)transform, 3);
    long outer_count = (p3 != 0) + 1;
    long inner_count = (p2 != 0) + 1;
    for (long i = 0; i < outer_count; i++) {
        for (long j = 0; j < inner_count; j++) {
            float y_sign = i != 0 ? -1.0f : 1.0f;
            float x_sign = j != 0 ? -1.0f : 1.0f;
            real_vector4d constants;
            constants.i = x_sign;
            constants.j = y_sign;
            constants.k = 0.0f;
            constants.l = 0.0f;
            g_rasterizer_device->SetVertexShaderConstantF(30, (const float *)&constants, 1);
            g_rasterizer_device->SetPixelShaderConstantF(53, (const float *)&constants, 1);
            D3DDevice_SetIndices(device, 0);
            if (widget_geometry->character_count > 0)
                D3DDevice_DrawVerticesUP(device, D3DPT_QUADLIST, widget_geometry->character_count << 2, widget_geometry->characters, 16);
        }
    }
}

// @address 0x18bc20 ?draw@c_chud_motion_sensor@@QAAXXZ
void c_chud_motion_sensor::draw()
{
    s_chud_render_data render;
    chud_build_custom_render_data(&render, 8);
    D3DDevice_SetVertexDeclaration(
        g_rasterizer_device, rasterizer_get_vertex_declaration(_vertex_type_chud_fancy, _transfer_vector_vertex_none, _entry_point_default));
    if (chud_apply_render_data(&render, true))
    {
        D3DDevice *device = g_rasterizer_device;
        D3DDevice_SetIndices(device, 0);
        {
            c_rasterizer_texture_ref texture(NONE);
            s_entry *entry;
            ::bitmap_group_get_xenon_hardware_format(
                chud_globals->blip_shape.index, 0, &texture, (bitmap_data const **)&entry, (__int64 *)(0));
            c_rasterizer::set_sampler_texture(0, texture);
        }
        c_rasterizer::set_sampler_address_mode(
            0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        if (g_rasterizer_sampler_filter_modes[0] != 2)
        {
            g_rasterizer_sampler_filter_modes[0] = 2;
            rasterizer_device_set_sampler_filter_modes(g_rasterizer_device, 0, 2, 2);
        }
        s_widget_geometry_quad_fancy history_quads[100];
        for (long i = 0; i < 10; ++i)
        {
            float fraction = (float)(i + 1) * 0.1f;
            long index = (m_valid_index + i + 1) % 10;
            long count = history_build_geometry(index, fraction, history_quads, 100);
            if (count > 0)
                D3DDevice_DrawVerticesUP(device, D3DPT_QUADLIST, count << 2, history_quads, 24);
        }
        if (m_custom_blip_count > 0)
        {
            s_widget_geometry_quad_fancy quads[20];
            real_point2d position = g_chud_draw_globals.origin;
            float scale = g_chud_draw_globals.curvature_info->blip_scale;
            float forward_scale = g_chud_draw_globals.curvature_info->blip_forward_scale;
            for (long i = 0; i < m_custom_blip_count; ++i)
                blip_build_geometry(&quads[i], &m_custom_blips[i], (long)(m_navpoint_data[i] * 255.0f), 1.5f, forward_scale, &position, scale);
            if (m_custom_blip_count > 0)
                D3DDevice_DrawVerticesUP(device, D3DPT_QUADLIST, m_custom_blip_count << 2, quads, 24);
        }
    }
}

// @address 0x7c8864 g_chud_default_widget_init_needed
bool g_chud_default_widget_init_needed = true;
// @address 0x119ae58 g_chud_default_widget_render_data
s_chud_widget_render_data g_chud_default_widget_render_data;


static __forceinline bool chud_draw_bitmap_get_hardware_format(long group_index, long bitmap_index,
    c_rasterizer_texture_ref *texture)
{
    const bitmap_data *bitmap;
    return bitmap_group_get_xenon_hardware_format(group_index, bitmap_index, texture, &bitmap, 0) != 0;
}

// @address 0x49c320 chud_bitmap_widget_draw
extern "C" void chud_bitmap_widget_draw(long user_index, s_chud_draw_widget_data *draw, bool text)
{
    s_chud_widget_placement_data *placement = draw->widget->base.placement.count
        ? (s_chud_widget_placement_data *)draw->widget->base.placement.address : 0;
    if (!placement || (draw->runtime->flags & 1))
        return;

    s_widget_geometry geometry;
    geometry.texture_handle = NONE;
    s_chud_render_data render;
    real_matrix4x3 basis;
    if (!chud_build_widget_render_data(draw, &render, text))
        return;
    chud_compute_anchor_basis(user_index, draw, placement->anchor, &basis);
    if (!chud_bitmap_widget_compute_geometry(draw, &geometry))
        return;

    s_chud_text_widget *widget = draw->widget;
    chud_apply_widget_animation(draw, &basis, &geometry, &render);
    if (widget->flags & (1 << 10))
        render.chud_scalar_output_ef.l *= render.chud_color_outputs[0].l;
    if (!chud_apply_render_data(&render, false))
        return;

    bool horizontal = (widget->flags & 1) != 0;
    bool vertical = (widget->flags & 2) != 0;
    bool cortana = ((widget->flags >> 3) & 1) != 0;
    bool use_emblem = (widget->flags & 0x3c0) != 0;
    bool draw_geometry = true;
    if (cortana)
    {
        s_dynamic_render_target *target = c_dynamic_render_targets::chud_get_target(_render_target_type_texture_camera);
        if (!target)
            draw_geometry = false;
        else
        {
            c_rasterizer::set_sampler_texture(1, target->accumulation_texture);
            c_rasterizer::set_sampler_address_mode(1, c_rasterizer::_sampler_address_clamp,
                c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            chud_draw_set_sampler_filter_mode(1, 1);
        }
    }
    if (use_emblem)
    {
        s_chud_globals_definition *globals = chud_globals;
        if (globals->emblem_bitmap.index != NONE)
        {
            emblem_info *emblem = 0;
            draw_geometry = false;
            s_chud_persistent_display_state *user = &c_chud_manager::x_persistent_user_data[user_index].m_display_state;
            if (widget->flags & (1 << 6)) emblem = &user->emblems[0];
            if (widget->flags & (1 << 7)) emblem = &user->emblems[1];
            if (widget->flags & (1 << 8)) emblem = &user->emblems[2];
            if (widget->flags & (1 << 9)) emblem = &user->emblems[3];
            if (emblem)
            {
                c_rasterizer_texture_ref background(NONE);
                c_rasterizer_texture_ref foreground(NONE);
                bitmap_group *group = (bitmap_group *)global_tag_instances
                    [datum_index_to_absolute_index(globals->emblem_bitmap.index)].definition;
                long background_index = emblem->background_emblem % group->bitmaps.count;
                long foreground_index = emblem->foreground_emblem % group->bitmaps.count;
                if (chud_draw_bitmap_get_hardware_format(globals->emblem_bitmap.index, background_index, &background)
                    && chud_draw_bitmap_get_hardware_format(globals->emblem_bitmap.index, foreground_index, &foreground)
                    && emblem_set_render_constants_for_emblem(emblem))
                {
                    c_rasterizer::set_sampler_texture(0, background);
                    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp,
                        c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
                    chud_draw_set_sampler_filter_mode(0, 2);
                    c_rasterizer::set_sampler_texture(1, foreground);
                    c_rasterizer::set_sampler_address_mode(1, c_rasterizer::_sampler_address_clamp,
                        c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
                    chud_draw_set_sampler_filter_mode(1, 2);
                    geometry.texture_handle = NONE;
                    draw_geometry = true;
                }
            }
        }
    }
    if (draw_geometry)
        chud_draw_widget_geometry(&geometry, &basis, horizontal, vertical);
    if (cortana)
        c_rasterizer::set_sampler_texture(1, c_rasterizer_texture_ref(NONE));
}

// @address 0x49cc80 chud_build_widget_render_data
extern "C" bool chud_build_widget_render_data(s_chud_draw_widget_data *draw, s_chud_render_data *render_data, bool text)
{
    if (g_chud_default_widget_init_needed)
    {
        g_chud_default_widget_render_data.shader_type = 0;
        g_chud_default_widget_render_data.external_inputs[0] = 1;
        g_chud_default_widget_render_data.external_inputs[1] = 1;
        g_chud_default_widget_render_data.color_inputs[0] = 14;
        g_chud_default_widget_render_data.color_inputs[1] = 15;
        g_chud_default_widget_render_data.color_inputs[2] = 16;
        g_chud_default_widget_render_data.color_inputs[3] = 17;
        g_chud_default_widget_render_data.scalar_inputs[0] = 0;
        g_chud_default_widget_render_data.scalar_inputs[1] = 0;
        g_chud_default_widget_render_data.scalar_inputs[2] = 0;
        g_chud_default_widget_render_data.scalar_inputs[3] = 0;
        g_chud_default_widget_render_data.scalar_inputs[4] = 0;
        g_chud_default_widget_render_data.scalar_inputs[5] = 0;
        g_chud_default_widget_init_needed = false;
    }

    s_chud_text_widget *widget = draw->widget;
    s_chud_widget_render_data *widget_render = widget->base.render_data.count
        ? (s_chud_widget_render_data *)tag_block_get_element_with_size(&widget->base.render_data, 0, sizeof(s_chud_widget_render_data)) : NULL;
    if (!widget_render)
        widget_render = &g_chud_default_widget_render_data;

    bool result;
    if ((widget_render->shader_type != 20) ^ text)
    {

    for (long i = 0; i < 6; i++)
        chud_render_data_fetch_color_output(draw->user_index, widget_render->color_inputs[i], widget_render, &render_data->chud_color_outputs[i]);

    chud_draw_set_real_vector4d(&render_data->chud_scalar_output_abcd,
        chud_render_data_fetch_scalar_output(widget_render->scalar_inputs[0], widget_render, draw->runtime),
        chud_render_data_fetch_scalar_output(widget_render->scalar_inputs[1], widget_render, draw->runtime),
        chud_render_data_fetch_scalar_output(widget_render->scalar_inputs[2], widget_render, draw->runtime),
        chud_render_data_fetch_scalar_output(widget_render->scalar_inputs[3], widget_render, draw->runtime));
    chud_draw_set_real_vector4d(&render_data->chud_scalar_output_ef,
        chud_render_data_fetch_scalar_output(widget_render->scalar_inputs[4], widget_render, draw->runtime),
        chud_render_data_fetch_scalar_output(widget_render->scalar_inputs[5], widget_render, draw->runtime),
        0.0f,
        g_chud_draw_globals.global_hud_alpha);

    long shader_index = widget_render->shader_type;
    s_tag_reference *vertex_shader = NULL;
    if (shader_index >= 0)
    {
        if (shader_index < chud_globals->shader_count)
            vertex_shader = &(chud_globals->shaders)[shader_index].vertex_shader;
    }
    s_tag_reference *pixel_shader = NULL;
    if (shader_index >= 0)
    {
        if (shader_index < chud_globals->shader_count)
            pixel_shader = &(chud_globals->shaders)[shader_index].pixel_shader;
    }
    render_data->vertex_shader_tag_index = vertex_shader ? vertex_shader->index : -1;
    render_data->pixel_shader_tag_index = pixel_shader ? pixel_shader->index : -1;
    chud_draw_set_real_vector4d(&render_data->texture_transform, 1.0f, 1.0f, 0.0f, 0.0f);

        result = true;
        if (text)
            g_chud_text_render_target_dirty = true;
    }
    else
    {
        result = false;
    }
    return result;
}
// @address 0x49dd28 chud_text_widget_draw
extern "C" void chud_text_widget_draw(int p0, s_chud_draw_widget_data *draw, bool p2)
{
    s_widget_geometry geometry;
    s_chud_render_data render;
    real_matrix4x3 anchor;

    s_chud_text_widget *widget = draw->widget;
    if (widget->base.placement.count != 0)
    {
        void *element = widget->base.placement.address;
        if (element != 0 && !(draw->runtime->flags & 1))
        {
            geometry.texture_handle = -1;
            if (chud_build_widget_render_data(draw, &render, p2) != 0)
            {
                chud_compute_anchor_basis(p0, draw, *(unsigned short *)element, &anchor);
                if (chud_text_widget_compute_geometry(draw, &geometry, &render))
                {
                    chud_apply_widget_animation(draw, &anchor, &geometry, &render);
                    if (chud_apply_render_data(&render, false))
                    {
                        chud_draw_widget_geometry(&geometry, &anchor, false, false);
                    }
                }
            }
        }
    }
}

// match: 0x49f2b0: keep the prior direct TLS array read without an added operator-> inline layer, and OR the flag word
// directly: flags.set(1, true) reorders the direction/camera_forward float loads and swaps the fsubs/fmadds operands.
#define CHUD_DRAW_LOOPING_SOUND_STOP(sound_index) \
    if ((sound_index) != -1) \
        ((game_looping_sound_datum *)game_looping_sound_data.m_data->data)[(sound_index) & 0xffff].flags.m_storage |= 2
#define CHUD_DRAW_LOOPING_SOUND_SET_SCALE(sound_index, sound_scale) \
    if ((sound_index) != -1) \
        ((game_looping_sound_datum *)game_looping_sound_data.m_data->data)[(sound_index) & 0xffff].scale = (sound_scale)

// @address 0x49f2b0 ?compute_effect_config@c_chud_cortana_effect@@QAAXJ@Z
void c_chud_cortana_effect::compute_effect_config(long user_index)
{
    real_point3d camera_position = observer_get_camera(user_index)->position;
    real_vector3d camera_forward = observer_get_camera(user_index)->forward;
    real_vector3d direction;
    vector_from_points3d(&camera_position, &m_position, &direction);
    normalize3d(&direction);
    real_vector3d displacement;
    vector_from_points3d(&camera_position, &m_position, &displacement);
    float distance = sqrtf(magnitude_squared3d(&displacement));
    float facing = MAX(0.0f, dot_product3d(&direction, &camera_forward));
    s_chud_cortana_definition *definition = (s_chud_cortana_definition *)tag_block_get_element_with_size(
        &chud_globals->cortana_definitions, 0, sizeof(s_chud_cortana_definition));
    long index = 0;
    if (definition->suck_definitions.count - 1 > 0)
    {
        do
        {
            s_chud_cortana_suck_definition *suck = (s_chud_cortana_suck_definition *)tag_block_get_element_with_size(
                &definition->suck_definitions, index, sizeof(s_chud_cortana_suck_definition));
            if (suck->maximum_distance < distance)
                break;
            index++;
        } while (index < definition->suck_definitions.count - 1);
    }
    long high_index = index;
    long low_index = MAX(0, index - 1);
    s_chud_cortana_suck_definition *high = (s_chud_cortana_suck_definition *)tag_block_get_element_with_size(
        &definition->suck_definitions, high_index, sizeof(s_chud_cortana_suck_definition));
    s_chud_cortana_suck_definition *low = (s_chud_cortana_suck_definition *)tag_block_get_element_with_size(
        &definition->suck_definitions, low_index, sizeof(s_chud_cortana_suck_definition));
    float blend = 0.5f;
    if (index != 0)
    {
        blend = (distance - high->maximum_distance) / (low->maximum_distance - high->maximum_distance);
        blend = MIN(MAX(blend, 0.0f), 1.0f);
    }
    long count = 24;
    float *high_a = high->near_config;
    float *high_b = high->far_config;
    float *low_a = low->near_config;
    float *low_b = low->far_config;
    float *bounds = &m_noise_bounds.lower;
    long i = 0;
    do
    {
        float high_value = high_b[i] + facing * (high_a[i] - high_b[i]);
        float low_value = low_b[i] + facing * (low_a[i] - low_b[i]);
        bounds[i] = high_value + blend * (low_value - high_value);
        i++;
    } while (i < count);
    if (m_state0 != 0 && m_state0 < (1 << high_index))
    {
        if ((1 << low_index) == m_state0)
        {
            CHUD_DRAW_LOOPING_SOUND_STOP(m_secondary_sounds[0]);
            CHUD_DRAW_LOOPING_SOUND_STOP(m_secondary_sounds[1]);
            m_secondary_sounds[0] = m_sounds[0];
            m_secondary_sounds[1] = m_sounds[1];
            m_state1 = 1 << low_index;
            m_state0 = 0;
        }
        else
        {
            CHUD_DRAW_LOOPING_SOUND_STOP(m_secondary_sounds[0]);
            CHUD_DRAW_LOOPING_SOUND_STOP(m_secondary_sounds[1]);
            CHUD_DRAW_LOOPING_SOUND_STOP(m_sounds[0]);
            CHUD_DRAW_LOOPING_SOUND_STOP(m_sounds[1]);
            m_state0 = 0;
            m_state1 = 0;
        }
    }
    else if (m_state1 != 0 && m_state1 > (1 << low_index))
    {
        if ((1 << high_index) == m_state1)
        {
            CHUD_DRAW_LOOPING_SOUND_STOP(m_sounds[0]);
            CHUD_DRAW_LOOPING_SOUND_STOP(m_sounds[1]);
            m_sounds[0] = m_secondary_sounds[0];
            m_sounds[1] = m_secondary_sounds[1];
            m_state0 = 1 << high_index;
            m_state1 = 0;
        }
        else
        {
            CHUD_DRAW_LOOPING_SOUND_STOP(m_secondary_sounds[0]);
            CHUD_DRAW_LOOPING_SOUND_STOP(m_secondary_sounds[1]);
            CHUD_DRAW_LOOPING_SOUND_STOP(m_sounds[0]);
            CHUD_DRAW_LOOPING_SOUND_STOP(m_sounds[1]);
            m_state0 = 0;
            m_state1 = 0;
        }
    }
    if (m_state0 == 0)
    {
        m_sounds[0] = unattached_looping_sound_start(high->near_sound.index, -1, 1.0f);
        m_sounds[1] = unattached_looping_sound_start(high->far_sound.index, -1, 1.0f);
        m_state0 = 1 << high_index;
    }
    if (m_state1 == 0)
    {
        m_secondary_sounds[0] = unattached_looping_sound_start(low->near_sound.index, -1, 1.0f);
        m_secondary_sounds[1] = unattached_looping_sound_start(low->far_sound.index, -1, 1.0f);
        m_state1 = 1 << low_index;
    }
    float scale0 = (1.0f - blend) * facing;
    float scale1 = (1.0f - blend) * (1.0f - facing);
    float scale2 = blend * facing;
    float scale3 = blend * (1.0f - facing);
    CHUD_DRAW_LOOPING_SOUND_SET_SCALE(m_sounds[0], scale0);
    CHUD_DRAW_LOOPING_SOUND_SET_SCALE(m_sounds[1], scale1);
    CHUD_DRAW_LOOPING_SOUND_SET_SCALE(m_secondary_sounds[0], scale2);
    CHUD_DRAW_LOOPING_SOUND_SET_SCALE(m_secondary_sounds[1], scale3);
    m_effect_blend = 0.0f;
    if (m_sounds[0] != -1)
    {
        float amplitude = unattached_looping_sound_get_amplitude(m_sounds[0]);
        m_effect_blend += amplitude * scale0;
    }
    if (m_sounds[1] != -1)
    {
        float amplitude = unattached_looping_sound_get_amplitude(m_sounds[1]);
        m_effect_blend += amplitude * scale1;
    }
    if (m_secondary_sounds[0] != -1)
    {
        float amplitude = unattached_looping_sound_get_amplitude(m_secondary_sounds[0]);
        m_effect_blend += amplitude * scale2;
    }
    if (m_secondary_sounds[1] != -1)
    {
        float amplitude = unattached_looping_sound_get_amplitude(m_secondary_sounds[1]);
        m_effect_blend += amplitude * scale3;
    }
}

static __forceinline void chud_draw_expand_argb_color(const real_argb_color *color, real_vector4d *result)
{
    chud_draw_set_real_vector4d(result, color->red, color->green, color->blue, color->alpha);
}
static __forceinline unsigned long chud_draw_damage_skin_color(const s_chud_globals_definition *table, long skin_index)
{
    unsigned long color = 0;
    skin_index = MIN(MAX(skin_index, 0), table->row_count - 1);
    if (table->row_count > 0)
        color = table->rows[skin_index].colors[23];
    return color;
}
// @address 0x49d168 chud_render_data_fetch_color_output
extern "C" void chud_render_data_fetch_color_output(long user_index, short output_index, s_chud_widget_render_data *widget, real_vector4d *output)
{
    s_chud_persistent_display_state *user = &c_chud_manager::x_persistent_user_data[user_index].m_display_state;
    short index = output_index;
    s_chud_globals_definition *table = chud_globals;
    s_chud_draw_globals *draw_globals = &g_chud_draw_globals;
    if (index >= 14 && index < 42)
    {
        unsigned long color = chud_globals_get_skin_color((const s_chud_globals_definition *)table, draw_globals->skin_index, index - 14);
        chud_expand_pixel32(color, output);
    }
    else
    {
        switch (index)
        {
        case 0: chud_expand_pixel32(widget->color_outputs[0], output); break;
        case 1: chud_expand_pixel32(widget->color_outputs[1], output); break;
        case 2: chud_expand_pixel32(widget->color_outputs[2], output); break;
        case 3: chud_expand_pixel32(widget->color_outputs[3], output); break;
        case 4: chud_expand_pixel32(0, output); break;
        case 5: chud_expand_pixel32(0, output); break;
        case 6: chud_expand_pixel32(user->icon_4fc, output); break;
        case 7: chud_expand_pixel32(user->icon_500, output); break;
        case 8: chud_expand_pixel32(user->icon_504, output); break;
        case _chud_render_color_output_real_a: chud_draw_expand_argb_color(&user->colors[0], output); break;
        case _chud_render_color_output_real_b: chud_draw_expand_argb_color(&user->colors[1], output); break;
        case _chud_render_color_output_real_c: chud_draw_expand_argb_color(&user->colors[2], output); break;
        case _chud_render_color_output_real_d: chud_draw_expand_argb_color(&user->colors[3], output); break;
        }
    }
    if (user->equipment_opacity > 0.0f)
    {
        if (MAX(output->i, MAX(output->j, output->k)) > 0.0f &&
            (widget->external_inputs[0] == 4 || widget->external_inputs[0] == 20 || widget->external_inputs[0] == 22 || widget->external_inputs[0] == 24))
        {
            real_vector4d color;
            chud_expand_pixel32(chud_draw_damage_skin_color((const s_chud_globals_definition *)chud_globals, draw_globals->skin_index), &color);
            output->i = (1.0f - user->equipment_opacity) * output->i + user->equipment_opacity * color.i;
            output->j = (1.0f - user->equipment_opacity) * output->j + user->equipment_opacity * color.j;
            output->k = (1.0f - user->equipment_opacity) * output->k + user->equipment_opacity * color.k;
        }
    }
}


// @address 0x49b830 chud_compute_parent_widget_transform
extern "C" unsigned char chud_compute_parent_widget_transform(long user_index, s_chud_draw_widget_data *draw, real_matrix4x3 *transform)
{
    bool result = false;
    s_chud_text_widget *parent = draw->parent_widget;
    if (parent->base.placement.count)
    {
        s_chud_widget_placement_data *placement = (s_chud_widget_placement_data *)parent->base.placement.address;
        if (placement)
        {
            real_matrix4x3 basis;
            s_widget_geometry geometry;
            if (chud_compute_anchor_basis(user_index, draw, placement->anchor, &basis))
            {
                geometry.x0 = 0.0f;
                geometry.x1 = 0.0f;
                geometry.y0 = 0.0f;
                geometry.y1 = 0.0f;
                geometry.texture_handle = NONE;
                chud_compute_geometry_common(placement, &geometry);
                s_chud_text_widget *widget = draw->widget;
                draw->widget = draw->parent_widget;
                chud_apply_widget_animation(draw, &basis, &geometry, 0);
                draw->widget = widget;
                chud_geometry_build_transform(&geometry, transform);
                matrix4x3_compute_product(&basis, transform, transform);
                result = true;
            }
        }
    }
    return result;
}


// @address 0x49ba60 chud_compute_anchor_basis
extern "C" unsigned char chud_compute_anchor_basis(long user_index, s_chud_draw_widget_data *draw, long anchor, real_matrix4x3 *basis)
{
    bool result = true;
    bool use_widget_transform = false;
    c_player_view *view = c_player_view::get_current();
    render_camera *camera = &view->rasterizer_camera;
    render_projection *projection = &view->rasterizer_projection;
    real_vector3d forward = (*global_identity4x3).forward;
    real_vector3d up = (*global_identity4x3).up;
    real_vector3d left = (*global_identity4x3).left;
    chud_table_row *row = &(chud_globals->rows)[chud_get_user_skin_index(user_index)];
    real_point3d anchor_point;
    real_matrix4x3 widget_transform;
    switch (anchor)
    {
    case _chud_draw_anchor_top_left:
        anchor_point.x = 0.0f;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_grenade_human:
    case _chud_draw_anchor_grenade_plasma:
    case _chud_draw_anchor_grenade_brute:
    case _chud_draw_anchor_grenade_fire:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        long index = anchor - _chud_draw_anchor_grenade_human;
        if (user->m_display_state.perspective_changed != 0 && anchor == _chud_draw_anchor_grenade_fire)
            index -= 1;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        anchor_point.x = row->grenade_anchor_offset * (float)index;
        break;
    }
    case _chud_draw_anchor_top_right:
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_bottom_right:
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_bottom_left:
        anchor_point.x = 0.0f;
        anchor_point.y = g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_tracked_object:
    {
        s_chud_persistent_display_state *update = &c_chud_manager::x_persistent_user_data[draw->user_index].m_display_state;
        if (update->point_tracker_enabled != 0 && chud_compute_anchor_basis_from_tracker(&update->point_tracker, basis, true))
        {
            anchor_point = basis->position;
            break;
        }
    }
    case _chud_draw_anchor_crosshair:
    case _chud_draw_anchor_reticle:
    {
        s_player_globals *observer = g_observer_globals;
        rectangle2d *bounds = &camera->window_pixel_bounds;
        real_point2d viewport_point;
        real_point3d project_point;
        project_point.x = 0.0f;
        project_point.y = -observer->blocks[draw->user_index].camera_result.direction_error;
        project_point.z = -1.0f;
        chud_draw_try_and_get_screen_point(camera, projection, bounds, project_point.n, &viewport_point);
        float x;
        float y;
        if (camera->frustum_adjustment)
        {
            float width = (float)(camera->window_pixel_bounds.x1 - camera->window_pixel_bounds.x0);
            float height = (float)(camera->window_pixel_bounds.y1 - bounds->y0);
            x = (viewport_point.x - (0.5f - camera->frustum_center.x / camera->frustum_scale.x * 0.5f) * width) * camera->frustum_scale.x + width * 0.5f;
            y = (viewport_point.y - ((camera->frustum_center.y / camera->frustum_scale.y + 1.0f) * 0.5f) * height) * camera->frustum_scale.y + height * 0.5f;
        }
        else
        {
            y = viewport_point.y;
            x = viewport_point.x;
        }
        short x0 = camera->window_pixel_bounds.x0;
        short y0 = bounds->y0;
        real_point2d curve_point;
        curve_point.y = y - (float)y0;
        curve_point.x = x - (float)x0;
        c_curved_hud_test curved_test;
        float curved_x;
        float curved_y;
        curved_test.inverse_transform(&curve_point, &curved_x, &curved_y);
        anchor_point.x = curved_x * g_chud_draw_globals.width;
        anchor_point.y = (1.0f - curved_y) * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    }
    case _chud_draw_anchor_top_center:
        anchor_point.x = g_chud_draw_globals.width * 0.5f;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_curvature_origin:
        anchor_point.x = g_chud_draw_globals.curvature_pixel_scale.i;
        anchor_point.y = g_chud_draw_globals.curvature_pixel_scale.j;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_bottom_center:
        anchor_point.x = g_chud_draw_globals.width * 0.5f;
        anchor_point.y = g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_parent:
    {
        anchor_point.x = 0.0f;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        if (chud_compute_parent_widget_transform(user_index, draw, &widget_transform))
            use_widget_transform = 1;
        break;
    }
    case _chud_draw_anchor_center:
        anchor_point.x = g_chud_draw_globals.width * 0.5f;
        anchor_point.y = g_chud_draw_globals.height * 0.5f;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_backpack_weapon:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        anchor_point.x = user->m_display_state.highlight_value;
        anchor_point.y = user->m_display_state.weapon_state_value;
        anchor_point.z = 0.0f;
        break;
    }
    case _chud_draw_anchor_equipment:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        anchor_point.x = 0.0f;
        anchor_point.y = user->m_display_state.weapon_state_value;
        anchor_point.z = 0.0f;
        break;
    }
    case _chud_draw_anchor_scoreboard_friendly:
    case _chud_draw_anchor_scoreboard_friendly_score_offset:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = g_chud_draw_globals.height - user->m_display_state.visible_opacity * row->scoreboard_spacing;
        anchor_point.z = 0.0f;
        if (anchor == _chud_draw_anchor_scoreboard_friendly_score_offset)
            anchor_point.x = g_chud_draw_globals.width - (float)user->m_display_state.count_text_width;
        break;
    }
    case _chud_draw_anchor_scoreboard_enemy:
    case _chud_draw_anchor_scoreboard_enemy_score_offset:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = g_chud_draw_globals.height - (1.0f - user->m_display_state.visible_opacity) * row->scoreboard_spacing;
        anchor_point.z = 0.0f;
        if (anchor == _chud_draw_anchor_scoreboard_enemy_score_offset)
            anchor_point.x = g_chud_draw_globals.width - (float)user->m_display_state.count_text_width;
        break;
    }
    case _chud_draw_anchor_messaging:
        anchor_point.x = 0.0f;
        anchor_point.y = g_chud_draw_globals.curvature_info->notification_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_state_message_right:
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = g_chud_draw_globals.curvature_info->state_left_right_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_state_message_left:
        anchor_point.x = 0.0f;
        anchor_point.y = g_chud_draw_globals.curvature_info->state_left_right_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_message_bottom_state:
        anchor_point.x = (float)(g_chud_draw_globals.width * 0.5);
        anchor_point.y = g_chud_draw_globals.curvature_info->message_anchor_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_message_bottom_primary:
        anchor_point.x = (float)(g_chud_draw_globals.width * 0.5);
        anchor_point.y = g_chud_draw_globals.curvature_info->state_message_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_medal:
    case _chud_draw_anchor_medal_slot_one:
    case _chud_draw_anchor_medal_slot_two:
    case _chud_draw_anchor_medal_slot_three:
    case _chud_draw_anchor_medal_slot_four:
    {
        float base_offset = chud_globals->scoreboard_top_y;
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[g_chud_draw_globals.user_index];
        long medal_index = user->m_display_state.color_group;
        float spacing = chud_globals->scoreboard_spacing;
        if (medal_index < 0)
            medal_index = 0;
        float scaled = (float)medal_index * spacing;
        float offset = 0.0f;
        switch (anchor)
        {
        case _chud_draw_anchor_medal: offset = scaled + base_offset; break;
        case _chud_draw_anchor_medal_slot_one: offset = scaled; break;
        case _chud_draw_anchor_medal_slot_two: offset = scaled - (double)spacing; break;
        case _chud_draw_anchor_medal_slot_three: offset = scaled - spacing * 2.0; break;
        case _chud_draw_anchor_medal_slot_four: offset = scaled - spacing * 3.0; break;
        }
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.z = 0.0f;
        anchor_point.y = g_chud_draw_globals.height - offset;
        break;
    }
    default:
        result = 0;
        break;
    }
    if (result)
    {
        basis->scale = 1.0f;
        basis->forward = forward;
        basis->left = left;
        basis->up = up;
        basis->position = anchor_point;
        if (use_widget_transform)
            matrix4x3_compute_product(&widget_transform, basis, basis);
    }
    return result;
}

// @address 0xed598 ?matrix4x3_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z

static __forceinline real_euler_angles2d *chud_draw_view_reference_get(long view_index)
{
    if (view_index != -1)
        return &player_control_globals->states[view_index].action.look;
    return 0;
}

static __forceinline void chud_draw_screen_point_to_virtual(real_point2d *point, short x0, short y0)
{
    point->x -= (float)x0;
    point->y -= (float)y0;
    point->x = (point->x - g_tiled_render_x_offset) / g_tiled_render_x_scale;
    point->y = (point->y - g_tiled_render_y_offset) / g_tiled_render_y_scale;
}

// 0x18cb60
extern "C" void chud_point_tracker_update_screen_position(c_chud_point_tracker *tracker, float maximum_distance)
{
    if (!tracker->m_valid)
        return;
    c_player_view *view = c_player_view::get_current();
    render_camera *camera = &view->rasterizer_camera;
    render_projection *projection = &view->rasterizer_projection;
    real_euler_angles2d *reference = chud_draw_view_reference_get(view->view_index);
    float camera_x = camera->position.x;
    float camera_y = camera->position.y;
    float camera_z = camera->position.z;
    float dx = tracker->m_world_position.x - camera_x;
    float dy = tracker->m_world_position.y - camera_y;
    float dz = tracker->m_world_position.z - camera_z;
    real_vector3d direction = { dx, dy, dz };
    real_vector3d adjusted;
    const real_vector3d *forward = &camera->forward;
    chud_compute_limited_direction(&direction, forward, (const real_euler_angles2d *)reference, 1.3089969f, &adjusted, &tracker->m_turn_direction);
    matrix4x3_transform_vector(&projection->world_to_view, &adjusted, &direction);
    if (direction.k > camera->near_clip)
        direction.k = camera->near_clip;
    bool behind = forward->j * dy + forward->k * dz + forward->i * dx < 0.0f;
    tracker->m_behind = behind;
    rectangle2d bounds = camera->window_pixel_bounds;
    real_point2d screen;
    screen.x = screen.y = 0.0f;
    if (maximum_distance > 0.0f)
    {
        short x0 = bounds.x0;
        short y0 = bounds.y0;
        long half_width = (bounds.x1 - x0) / 2;
        long half_height = (bounds.y1 - y0) / 2;
        chud_draw_try_and_get_screen_point(camera, projection, &bounds, direction.n, &screen);
        float center_y = (float)half_height;
        float center_x = (float)half_width;
        float origin_x = (float)x0;
        float origin_y = (float)y0;
        // match: 0x18cb60: without this the fmuls before the origin subtract swaps its operands (fmuls fr0,fr12,fr0 becomes fmuls fr0,fr0,fr12)
        __assume(origin_x == origin_x);
        screen.x -= origin_x;
        screen.y -= origin_y;
        screen.x = (screen.x - g_tiled_render_x_offset) / g_tiled_render_x_scale;
        screen.y = (screen.y - g_tiled_render_y_offset) / g_tiled_render_y_scale;
        float offset_x = screen.x - center_x;
        float offset_y = screen.y - center_y;
        float ny = offset_y / center_y;
        float nx = offset_x / center_x;
        float sum_sq = nx * nx + ny * ny;
        float distance = sqrtf(sum_sq);
        if (distance > maximum_distance || tracker->m_behind)
        {
            tracker->m_offscreen_amount = distance - maximum_distance;
            tracker->m_offscreen_amount = tracker->m_offscreen_amount > 0.0001f ? tracker->m_offscreen_amount : 0.0001f;
            screen.x = offset_x;
            screen.y = offset_y;
            if (distance > 0.0f)
            {
                float scale = maximum_distance / distance;
                screen.x *= scale;
                screen.y *= scale;
                float clamped_x = (float)half_width + screen.x;
                float clamped_y = (float)half_height + screen.y;
                // match: 0x18cb60: without this the fmuls before the origin subtract swaps its operands (fmuls fr0,fr12,fr0 becomes fmuls fr0,fr0,fr12)
                __assume(clamped_x == clamped_x);
                // match: 0x18cb60: without this the same fmuls swaps its operands; without both clamp assumes the clamp result moves from fr31 to fr28
                __assume(clamped_y == clamped_y);
                screen.x = clamped_x;
                screen.y = clamped_y;
            }
            else
            {
                tracker->m_offscreen_amount = 0.0f;
            }
        }
        else
        {
            tracker->m_offscreen_amount = 0.0f;
        }
    }
    else
    {
        if (behind || !chud_draw_try_and_get_screen_point(camera, projection, &bounds, direction.n, &screen))
            tracker->m_offscreen_amount = 1.0f;
        else
            tracker->m_offscreen_amount = 0.0f;
        chud_draw_screen_point_to_virtual(&screen, bounds.x0, bounds.y0);
    }
    tracker->m_screen_position = screen;
    s_chud_draw_globals *draw_globals = &g_chud_draw_globals;
    if (draw_globals->width > 0.0f && draw_globals->height > 0.0f)
    {
        c_curved_hud_test curved_test;
        curved_test.inverse_transform(&tracker->m_screen_position, &tracker->m_position.x, &tracker->m_position.y);
        tracker->m_position.x = tracker->m_position.x * draw_globals->width;
        tracker->m_position.y = (1.0f - tracker->m_position.y) * draw_globals->height;
    }
    else
    {
        tracker->m_position = screen;
    }
}


static __forceinline float chud_draw_curve_average(const real_point2d *curve, float near_x, float far_x)
{
    return (rasterizer_main_evaluate_curve(curve, near_x) + rasterizer_main_evaluate_curve(curve, far_x)) * 0.5f;
}


// @address 0x2588c k_widescreen_aspect_ratio
static __forceinline float chud_draw_aspect_ratio()
{
#if !defined(_XBOX) || defined(__clang__)
    // PC (port/GAME-CHANGES.md, "Custom resolutions"): the display's (as chud_draw_loading_setup_view's projection)
    return rasterizer_main_get_video_aspect_ratio();
#else
    if (rasterizer_main_get_video_mode()->fIsWideScreen != 0)
        return k_widescreen_aspect_ratio;
    return k_observer_default_aspect_ratio;
#endif
}

static const real_vector3d k_chud_draw_projection_up = { 0.0f, 1.0f, 0.0f };

// @address 0x4544f0 chud_loading_compute_projection_corners
extern "C" void chud_loading_compute_projection_corners(const s_main_render_camera_curves *curves, real_point3d *bottom_left,
    real_point3d *bottom_right, real_point3d *top_right, real_point3d *top_left)
{
    float angle = chud_draw_curve_average(curves->position_angle, -0.1f, 0.1f);
    float distance = chud_draw_curve_average(curves->position_radius, -0.1f, 0.1f);
    float near_height = rasterizer_main_evaluate_curve(curves->position_height, -0.1f);
    float far_height = rasterizer_main_evaluate_curve(curves->position_height, 0.1f);
    real_point3d eye;
    set_real_point3d(&eye, (float)cos(angle) * distance, (far_height + near_height) * 0.5f, (float)sin(angle) * distance);

    angle = chud_draw_curve_average(curves->direction_angle, -0.1f, 0.1f);
    distance = chud_draw_curve_average(curves->direction_radius, -0.1f, 0.1f);
    near_height = rasterizer_main_evaluate_curve(curves->direction_height, -0.1f);
    far_height = rasterizer_main_evaluate_curve(curves->direction_height, 0.1f);
    real_point3d target;
    set_real_point3d(&target, (float)cos(angle) * distance, (far_height + near_height) * 0.5f, (float)sin(angle) * distance);

    real_vector3d forward;
    vector_from_points3d(&eye, &target, &forward);
    normalize3d(&forward);
    real_vector3d right;
    real_vector3d up_axis = k_chud_draw_projection_up;
    cross_product3d(&forward, &up_axis, &right);
    normalize3d(&right);
    real_vector3d up;
    cross_product3d(&right, &forward, &up);

    float half_height = (float)tan(0.52359879f);
    float aspect_ratio = chud_draw_aspect_ratio();
    float half_width = -(aspect_ratio * half_height);

    real_point3d center;
    point_from_line3d(&eye, &forward, 1.0f, &center);
    real_point3d bottom;
    real_point3d top;
    point_from_line3d(&center, &up, -half_height, &bottom);
    point_from_line3d(&center, &up, half_height, &top);
    point_from_line3d(&bottom, &right, -half_width, bottom_left);
    point_from_line3d(&bottom, &right, half_width, bottom_right);
    point_from_line3d(&top, &right, half_width, top_right);
    point_from_line3d(&top, &right, -half_width, top_left);
}

static __forceinline real_vector4d *chud_draw_loading_vector(real_vector4d *v, float i, float j, float k, float l)
{
    v->i = i; v->j = j; v->k = k; v->l = l;
    return v;
}
static __forceinline void s_chud_loading_vertex_bool(long start, long count, const BOOL *values)
{
    c_rasterizer::get_device()->SetVertexShaderConstantB(start, values, count);
}
static __forceinline void s_chud_loading_vertex_constant(const long &start, long count, const real_vector4d *values)
{
    D3DDevice &device = *c_rasterizer::get_device();
    device.SetVertexShaderConstantF(start, values->n, count);
}
static __forceinline void chud_draw_loading_pixel_constant(long start, long count, const real_vector4d *values)
{
    c_rasterizer::get_device()->SetPixelShaderConstantF(start, (const float *)values, count);
}
static __forceinline void chud_draw_loading_coefficient_constant(D3DDevice *const &first_device, const long &start, long count, const real_vector4d *values)
{
    D3DDevice &device = *c_rasterizer::get_device();
    if (D3DISINTCONST(count) && count == 1)
    {
        first_device->m_Constants.VertexShaderF[(UINT)start].x = values->n[0];
        device.m_Constants.VertexShaderF[(UINT)start].y = values->n[1];
        device.m_Constants.VertexShaderF[(UINT)start].z = values->n[2];
        device.m_Constants.VertexShaderF[(UINT)start].w = values->n[3];
        D3DTagCollection_Set(&first_device->m_Pending, D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS), D3DTag_ShaderConstantMask(start, count));
    }
    else
    {
        D3DDevice_SetVertexShaderConstantFN(first_device, start, values->n, count, D3DTag_ShaderConstantMask(start, count));
    }
}
static __forceinline void chud_draw_loading_pixel_bool(long start, long count, const BOOL *values)
{
    c_rasterizer::get_device()->SetPixelShaderConstantB(start, values, count);
}
static inline float chud_draw_loading_profile(const s_chud_loading_screen *screen, long index)
{
    return screen->profile[index];
}
static inline const float *chud_draw_loading_samples(const s_chud_loading_screen *screen, long index)
{
    return screen->samples[index];
}
static __forceinline void chud_draw_loading_inverse_sine(double angle, float *inverse)
{
    *inverse = 1.0f / (float)sin(angle);
}
static __forceinline void chud_draw_loading_blend(long mode)
{
    if (g_rasterizer_alpha_blend_mode != mode)
    {
        g_rasterizer_alpha_blend_mode = mode;
        rasterizer_set_device_alpha_blend_mode(c_rasterizer::get_device(), mode);
    }
}

// @address 0x451c20 chud_loading_draw_screen
extern "C" void chud_loading_draw_screen(s_chud_loading_screen *loading_screen, float field_of_view, float near_plane, float far_plane,
    float progress, const real_vector3d *position, const real_vector3d *direction)
{
    real_vector4d vertex_samples[65];
    real_vector4d pixel_samples[65];
    chud_draw_loading_setup_view(loading_screen, position, direction);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), D3DCULL_NONE);
    if (g_rasterizer_alpha_blend_mode != 1)
    {
        g_rasterizer_alpha_blend_mode = 1;
        D3DDevice *device = c_rasterizer::get_device();
        D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
        D3DDevice_SetRenderState_BlendOp(device, 0);
        D3DDevice_SetRenderState_SrcBlend(device, 1);
        D3DDevice_SetRenderState_DestBlend(device, 1);
    }
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
    real_vector4d pixel_parameters;
    real_vector4d screen_parameters;
    real_vector4d geometry_parameters;
    chud_draw_loading_vector(&pixel_parameters, 1.0f / 64.0f, 1.0f / 32.0f, 0.25f, field_of_view);
    float segment_fraction = 1.0f / 32.0f;
    chud_draw_loading_vector(&screen_parameters, 64.0f, 32.0f, 4.0f, 0.0f);
    chud_draw_loading_pixel_constant(10, 1, &pixel_parameters);
    chud_draw_loading_pixel_constant(11, 1, &screen_parameters);
    c_rasterizer::set_gprs_allocation((c_rasterizer::e_gpr_allocation)1);
    D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(), loading_screen->declarations[0]);
    chud_draw_set_shaders(3);
    D3DDevice_SetStreamSource(c_rasterizer::get_device(), 0, loading_screen->buffers[2], 0, 8, 1);
    int strips = 0;
    s_chud_loading_vertex_bool(4, 1, &strips);
    chud_draw_loading_pixel_bool(4, 1, &strips);
    long segment = 0;
    const float *coefficient_data = loading_screen->coefficients;
    const float *angle_data = loading_screen->profile;
    for (; segment < 32; ++segment, ++angle_data, coefficient_data += 12)
    {
        real_vector4d transform;
        real_vector4d dimensions;
        real_vector4d fade;
        const float *profile = angle_data;
        const float *const *profile_reference = &profile;
        const float angle = (float)segment * segment_fraction;
        chud_draw_loading_vector(&transform, angle * 6.2831855f, 0.19634955f, (*profile_reference)[0], (*profile_reference)[32]);
        chud_draw_loading_vector(&dimensions, 4.0f, 0.05000019f, 0.25f, 0.0f);
        s_chud_loading_vertex_constant(8, 1, &transform);
        s_chud_loading_vertex_constant(9, 1, &dimensions);
        chud_draw_loading_vector(&fade, 0.0f, 0.0f, progress, 0.0f);
        chud_draw_loading_pixel_constant(9, 1, &fade);
        const float *coefficient_value = coefficient_data;
        for (long coefficient = 0; coefficient < 6; ++coefficient_value, ++coefficient)
        {
            D3DDevice *coefficient_device = c_rasterizer::get_device();
            real_vector4d values;
            const float *coefficient_input = coefficient_value;
            const float *const *coefficient_reference = &coefficient_input;
            const float &slope = +(*coefficient_reference)[6];
            chud_draw_loading_vector(&values, (*coefficient_reference)[0], slope, 0.0f, 0.0f);
            chud_draw_loading_coefficient_constant(coefficient_device, 11 + coefficient, 1, &values);
        }
        D3DDevice_DrawVertices(c_rasterizer::get_device(), (D3DPRIMITIVETYPE)2, 0, 768);
    }
    D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(), loading_screen->declarations[0]);
    chud_draw_set_shaders(3);
    D3DDevice_SetStreamSource(c_rasterizer::get_device(), 0, loading_screen->buffers[3], 0, 8, 1);
    int rings = 1;
    s_chud_loading_vertex_bool(4, 1, &rings);
    chud_draw_loading_pixel_bool(4, 1, &rings);
    float heights[5] = {18.0f, 19.0f, 20.0f, 21.0f, 22.0f};
    float widths[5] = {3.1415927f, 2.5132742f, 1.8849556f, 1.2566371f, 0.62831855f};
    float distances[5] = {20.0f, 24.0f, 28.0f, 32.0f, 34.0f};
    float thicknesses[5] = {0.05f, 0.04f, 0.03f, 0.02f, 0.01f};
    for (long ring = 0; ring < 5; ++ring)
    {
        float width = widths[ring];
        double half_width = width * 0.5f;
        float height = heights[ring];
        float thickness = thicknesses[ring];
        float inverse_sine = 0.0f;
        float distance = distances[ring];
        float negative_half_width = width * -0.5f;
        thickness *= 0.4f;
        for (long segment = 0; segment < 48; ++segment)
        {
            real_vector4d transform;
            real_vector4d dimensions;
            chud_draw_loading_inverse_sine(half_width, &inverse_sine);
            const float angle = (float)segment * 0.020833334f;
            chud_draw_loading_vector(&transform, angle * 6.2831855f, 0.1308997f, negative_half_width, width);
            chud_draw_loading_vector(&dimensions, height, distance * inverse_sine, thickness, 0.0f);
            s_chud_loading_vertex_constant(8, 1, &transform);
            s_chud_loading_vertex_constant(9, 1, &dimensions);
            D3DDevice_DrawVertices(c_rasterizer::get_device(), (D3DPRIMITIVETYPE)2, 0, 1242);
        }
    }
    D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(), loading_screen->declarations[1]);
    chud_draw_set_shaders(4);
    D3DDevice_SetStreamSource(c_rasterizer::get_device(), 0, loading_screen->buffers[1], 0, 20, 1);
    D3DDevice_SetRenderState_PointSpriteEnable(c_rasterizer::get_device(), 1);
    D3DDevice_SetTexture(c_rasterizer::get_device(), 0, loading_screen->textures[3], 0x80000000);
    c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp,
        c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    for (long segment = 0; segment < 32; ++segment)
    {
        const long &sector = +segment;
        real_vector4d transform;
        real_vector4d fade;
        const float angle = (float)segment * segment_fraction;
        chud_draw_loading_vector(&transform, angle * 6.2831855f, 0.19634955f,
            chud_draw_loading_profile(loading_screen, sector), chud_draw_loading_profile(loading_screen, sector + 32));
        chud_draw_loading_vector(&geometry_parameters, 4.0f, 0.05000019f, 0.25f, 0.0f);
        s_chud_loading_vertex_constant(8, 1, &transform);
        s_chud_loading_vertex_constant(9, 1, &geometry_parameters);
        chud_draw_loading_vector(&fade, 0.0f, 0.0f, progress, g_main_render_loading_globals.last_elapsed);
        s_chud_loading_vertex_constant(11, 1, &fade);
        const float *current = chud_draw_loading_samples(loading_screen, sector);
        const float *next;
        if (segment == 31)
            next = chud_draw_loading_samples(loading_screen, 0);
        else
            next = (loading_screen->samples + sector + 1)[0];
        for (long point = 0; point < 65; ++point)
            chud_draw_loading_vector(&vertex_samples[point], point == 64 ? *next : current[point], 0.0f, 0.0f, 0.0f);
        c_rasterizer::set_vertex_shader_constant(15, 65, vertex_samples);
        int alternate = 0;
        s_chud_loading_vertex_bool(4, 1, &alternate);
        chud_draw_loading_pixel_bool(4, 1, &alternate);
        D3DDevice_DrawVertices(c_rasterizer::get_device(), (D3DPRIMITIVETYPE)1, 0, 8192);
        alternate = 1;
        s_chud_loading_vertex_bool(4, 1, &alternate);
        chud_draw_loading_pixel_bool(4, 1, &alternate);
        D3DDevice_DrawVertices(c_rasterizer::get_device(), (D3DPRIMITIVETYPE)1, 0, 8192);
    }
    D3DDevice_SetRenderState_PointSpriteEnable(c_rasterizer::get_device(), 0);
    c_rasterizer::set_gprs_allocation((c_rasterizer::e_gpr_allocation)2);
    D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(), loading_screen->declarations[0]);
    chud_draw_set_shaders(2);
    D3DDevice_SetStreamSource(c_rasterizer::get_device(), 0, loading_screen->buffers[0], 0, 8, 1);
    D3DDevice_SetTexture(c_rasterizer::get_device(), 0, loading_screen->textures[0], 0x80000000);
    c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_point);
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap);
    D3DDevice_SetTexture(c_rasterizer::get_device(), 1, loading_screen->textures[1], 0x40000000);
    c_rasterizer::set_sampler_filter_mode(1, c_rasterizer::_sampler_filter_mode_point);
    c_rasterizer::set_sampler_address_mode(1, c_rasterizer::_sampler_address_border,
        c_rasterizer::_sampler_address_border, c_rasterizer::_sampler_address_border);
    D3DDevice_SetTexture(c_rasterizer::get_device(), 2, loading_screen->textures[2], 0x20000000);
    c_rasterizer::set_sampler_filter_mode(2, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer::set_sampler_address_mode(2, c_rasterizer::_sampler_address_clamp,
        c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    D3DDevice_SetTexture(c_rasterizer::get_device(), 3, loading_screen->detail_texture, 0x10000000);
    c_rasterizer::set_sampler_filter_mode(3, c_rasterizer::_sampler_filter_mode_point);
    c_rasterizer::set_sampler_address_mode(3, c_rasterizer::_sampler_address_border,
        c_rasterizer::_sampler_address_border, c_rasterizer::_sampler_address_border);
    float scroll = 4.94686f;
    if (!main_render_test_loading_buttons())
        scroll = 0.0f;
    float scroll_step = scroll * segment_fraction;
    for (long segment = 0; segment < 32; ++segment)
    {
        const long &sector = +segment;
        float fraction = (float)segment * segment_fraction;
        const float *current = chud_draw_loading_samples(loading_screen, sector);
        const float *next;
        if (segment == 31)
            next = chud_draw_loading_samples(loading_screen, 0);
        else
            next = (loading_screen->samples + sector + 1)[0];
        for (long point = 0; point < 65; ++point)
            chud_draw_loading_vector(&pixel_samples[point], point == 64 ? *next : current[point], 0.0f, 0.0f, 0.0f);
        c_rasterizer::set_pixel_shader_constant(15, 65, pixel_samples);
        real_vector4d transform;
        real_vector4d dimensions;
        real_vector4d texture;
        real_vector4d fade;
        chud_draw_loading_vector(&transform, fraction * 6.2831855f, 0.19634955f,
            chud_draw_loading_profile(loading_screen, sector), chud_draw_loading_profile(loading_screen, sector + 32));
        chud_draw_loading_vector(&dimensions, 4.0f, 0.05000019f, 0.25f, 0.0f);
        chud_draw_loading_vector(&texture, fraction * scroll - 0.4830918f, scroll_step, 0.0f, 0.0f);
        s_chud_loading_vertex_constant(8, 1, &transform);
        s_chud_loading_vertex_constant(9, 1, &dimensions);
        s_chud_loading_vertex_constant(11, 1, &texture);
        chud_draw_loading_vector(&fade, 0.0f, 0.0f, progress, g_main_render_loading_globals.last_elapsed);
        chud_draw_loading_pixel_constant(9, 1, &fade);
        D3DDevice_DrawVertices(c_rasterizer::get_device(), (D3DPRIMITIVETYPE)13, 0, 9988);
    }
    float emblem_scale = rasterizer_main_get_video_mode()->fIsWideScreen ? 2.0f : 1.5f;
    real_point2d emblem_anchor, emblem_position;
    set_real_point2d(&emblem_anchor, 1.0f, 0.0f);
    set_real_point2d(&emblem_position, 0.85f, -0.55f);
    real_vector4d emblem_color;
    chud_draw_loading_vector(&emblem_color, near_plane, near_plane, near_plane, 1.0f);
    c_rasterizer::set_pixel_shader_constant(11, 1, &emblem_color);
    emblem_draw_screen_quad(loading_screen, &emblem_position, &emblem_anchor, loading_screen->emblems[1], emblem_scale * 2.0f);
    s_chud_loading_screen *emblem_screen = loading_screen;
    s_chud_loading_screen *const *emblem_screen_reference = &emblem_screen;
    real_point2d second_position, second_anchor;
    set_real_point2d(&second_position, 0.85f, -0.85f);
    set_real_point2d(&second_anchor, 1.0f, 0.0f);
    emblem_draw_screen_quad(loading_screen, &second_position, &second_anchor, (*emblem_screen_reference)->emblems[0], emblem_scale);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)17, 0, 0, 0, 0);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)1);
    c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)17);
    c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp,
        c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    D3DDevice_SetTexture(c_rasterizer::get_device(), 1, 0, 0x40000000);
    D3DDevice_SetTexture(c_rasterizer::get_device(), 2, 0, 0x20000000);
    chud_draw_set_shaders(5);
    real_vector4d composite_color;
    chud_draw_loading_vector(&composite_color, 0.5875f, 0.65f, 0.8333333f, 1.0f);
    c_rasterizer::set_pixel_shader_constant(11, 1, &composite_color);
    chud_draw_loading_blend(0);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), D3DCULL_NONE);
    D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(),
        rasterizer_get_vertex_declaration(_vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default));
    rasterizer_vertex_screen quad[4];
    quad[0].x = -1.0f; quad[0].y = -1.0f;
    quad[1].x = 1.0f; quad[1].y = -1.0f;
    quad[2].x = 1.0f; quad[2].y = 1.0f;
    quad[3].x = -1.0f; quad[3].y = 1.0f;
    quad[0].u = 0.0f; quad[0].v = 1.0f;
    quad[1].u = 1.0f; quad[1].v = 1.0f;
    quad[2].u = 1.0f; quad[2].v = 0.0f;
    quad[3].u = 0.0f; quad[3].v = 0.0f;
    int composite = 1;
    int projected = 0;
    chud_draw_loading_pixel_bool(4, 1, &composite);
    chud_draw_loading_pixel_bool(5, 1, &projected);
    rasterizer_draw_quads(quad, 1);
    if (!(far_plane <= 0.0f))
    {
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)67);
        c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)67);
        c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_point);
        c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp,
            c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
        D3DDevice_SetTexture(c_rasterizer::get_device(), 1, 0, 0x40000000);
        D3DDevice_SetTexture(c_rasterizer::get_device(), 2, 0, 0x20000000);
        D3DDevice_SetVertexDeclaration(c_rasterizer::get_device(), loading_screen->declarations[2]);
        chud_draw_set_shaders(6);
        real_vector4d overlay_color;
        chud_draw_loading_vector(&overlay_color, far_plane, 0.45f, 0.8333333f, 1.0f);
        c_rasterizer::set_pixel_shader_constant(11, 1, &overlay_color);
        chud_draw_loading_blend(1);
        D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), D3DCULL_NONE);
        s_chud_loading_vertex overlay[4];
        chud_loading_compute_projection_corners(
            &g_main_render_loading_globals.camera_curves, &overlay[0].position, &overlay[1].position, &overlay[2].position, &overlay[3].position);
        set_real_point2d(&overlay[0].texcoord, 0.0f, 1.0f);
        set_real_point2d(&overlay[1].texcoord, 1.0f, 1.0f);
        set_real_point2d(&overlay[2].texcoord, 1.0f, 0.0f);
        set_real_point2d(&overlay[3].texcoord, 0.0f, 0.0f);
        int composite = 1;
        int projected = 1;
        chud_draw_loading_pixel_bool(4, 1, &composite);
        chud_draw_loading_pixel_bool(5, 1, &projected);
        D3DDevice_DrawVerticesUP(c_rasterizer::get_device(), (D3DPRIMITIVETYPE)13, 4, overlay, sizeof(overlay[0]));
    }
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)1);
}

static __forceinline s_chud_text_widget *chud_draw_get_widget_by_index(
    s_chud_widget_collection *group, long index, unsigned char &is_text)
{
    s_chud_text_widget *result;
    if (index < group->bitmap_widget_count)
    {
        result = (s_chud_text_widget *)&group->bitmap_widgets[index];
        is_text = 0;
    }
    else
    {
        result = &group->text_widgets[index - group->bitmap_widget_count];
        is_text = 1;
    }
    return result;
}

// @address 0x16a978 chud_draw_widget
extern "C" void chud_draw_widget(long user_index, s_chud_runtime_widget_datum *widget, long definition_tag_index, bool text)
{
    s_chud_draw_widget_data draw;
    draw.user_index = user_index;
    draw.runtime = widget;
    draw.definition = tag_get('chdt', definition_tag_index);
    s_chud_widget_collection *group = (s_chud_widget_collection *)tag_block_get_element_with_size(
        (const s_tag_block *)draw.definition, widget->collection_index, sizeof(s_chud_widget_collection));
    draw.parent_widget = (s_chud_text_widget *)group;
    unsigned char text_widget;
    draw.widget = chud_draw_get_widget_by_index(group, widget->definition_index, text_widget);
    long can_draw = 1;
    if (game_is_cooperative() && group->scripting_class == 2)
        can_draw = chud_draw_is_coop_vehicle_visible(user_index);
    if ((unsigned char)can_draw)
    {
        if (!text_widget)
        {
            chud_bitmap_widget_draw(user_index, &draw, text);
            return;
        }
        chud_text_widget_draw(user_index, &draw, text);
    }
}

// @address 0x4a02d8 chud_draw_directional_damage_overlay
extern "C" bool chud_draw_directional_damage_overlay(s_directional_damage *entries, long user_index)
{
    bool result = false;
    if (chud_draw_has_active_timed_slots(entries, user_index) && g_render_enable_screen_flashes)
    {
        s_widget_geometry geometry;
        geometry.texture_handle = -1;
        g_rasterizer_device->SetRenderState(D3DRS_COLORWRITEENABLE, 15);
        s_chud_render_data render_data;
        chud_build_custom_render_data(&render_data, 6);
        real_matrix4x3 basis;
        chud_compute_anchor_basis(user_index, 0, 0, &basis);
        real_rectangle2d bounds;
        set_real_rectangle2d(&bounds, -200.0f, g_chud_draw_globals.width + 200.0f,
            -200.0f, g_chud_draw_globals.height + 200.0f);
        chud_widget_build_quads_from_bounds(&bounds, &bounds, &bounds, &geometry, 1.0f, 1.0f, false);
        chud_draw_compute_geometry_common_internal(&geometry, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
        set_real_point3d(&geometry.position, 0.0f, 0.0f, 0.0f);
        geometry.texture_handle = -1;
        for (long index = 0; index < 4; ++index)
        {
            s_directional_damage *damage = &entries[index];
            real_vector4d color;
            real_vector4d position;
            if (damage->end_time > game_time_globals->time)
            {
                c_curved_hud_test curved_test;
                chud_point_tracker_update_screen_position(&damage->tracker, 1.25f);
                real_point2d point = damage->tracker.m_screen_position;
                float x;
                float y;
                curved_test.inverse_transform(&point, &x, &y);
                color.i = damage->color.red;
                color.j = damage->color.green;
                color.k = damage->color.blue;
                color.l = damage->first * damage->transition * damage->color.alpha;
                position.i = x * g_chud_draw_globals.width;
                position.j = (float)((1.0 - y) * g_chud_draw_globals.height);
                position.k = damage->intensity * g_chud_draw_globals.width;
                position.l = damage->second * damage->transition * damage->color.alpha;
            }
            else
            {
                color.i = 0.0f;
                color.j = 0.0f;
                color.k = 0.0f;
                color.l = 0.0f;
                position.i = 0.0f;
                position.j = 0.0f;
                position.k = 0.0f;
                position.l = 0.0f;
            }
            g_rasterizer_device->SetPixelShaderConstantF(34 + index * 2, (const float *)&color, 1);
            g_rasterizer_device->SetPixelShaderConstantF(35 + index * 2, (const float *)&position, 1);
        }
        real_vector4d screen;
        screen.i = g_chud_draw_globals.width * 0.5f;
        screen.j = g_chud_draw_globals.height * 0.5f;
        screen.k = g_chud_draw_globals.width * 0.3f;
        screen.l = g_chud_draw_globals.height * 0.3f;
        g_rasterizer_device->SetPixelShaderConstantF(42, (const float *)&screen, 1);
        if (chud_apply_render_data(&render_data, false))
        {
            chud_draw_widget_geometry(&geometry, &basis, false, false);
            result = true;
        }
    }
    return result;
}


extern "C" {


// @address 0x1328 k_two_string_format
}

static __forceinline unsigned long chud_draw_navpoint_text_length(const wchar_t *text, long maximum)
{
    unsigned long length = 0;
    while (length < maximum && text[length])
        ++length;
    return length;
}

__forceinline void c_chud_point_tracker::prepare_for_draw(float maximum_distance)
{
    chud_point_tracker_update_screen_position(this, maximum_distance);
}

// @address 0x18c630 chud_draw_navpoints
extern "C" void chud_draw_navpoints(c_chud_navpoint_manager *manager, long user_index)
{
    chud_table_row *definition = &(chud_globals->rows)[chud_get_user_skin_index(user_index)];
    if (definition->navpoint_bitmap.index != NONE)
    {
        for (long index = 0; index < 20; ++index)
        {
            const bool &valid = manager->navpoints[index].valid;
            c_chud_navpoint_manager::s_managed_navpoint *navpoint = &manager->navpoints[index];
            if (valid)
            {
                bool show_arrow = (navpoint->flags & 1) != 0;
                if (navpoint->state != -1 && player_mapping_globals->local_player_count == 1)
                    show_arrow = true;
                c_curved_hud_test curved_test;
                navpoint->tracker.update(&navpoint->position, true);
                if (show_arrow)
                    navpoint->tracker.prepare_for_draw(0.8f);
                else
                    navpoint->tracker.prepare_for_draw(0.0f);
                float offscreen_amount = navpoint->tracker.get_offscreen_amount();
                bool selected = index == manager->selected_index;
                bool show_distance = (float)index == manager->distance_index;
                s_widget_geometry geometry;
                geometry.texture_handle = NONE;
                s_chud_render_data render_data;
                chud_build_custom_render_data(&render_data, 10);
                real_matrix4x3 basis;
                chud_compute_anchor_basis_from_tracker(&navpoint->tracker, &basis, true);
                float vertical_offset = 0.0f;
                float scale = navpoint->scale;
                if (navpoint->arrow_index != NONE && (offscreen_amount == 0.0f || show_arrow))
                {
                    vertical_offset -= chud_build_geometry_from_navpoint(user_index, 1, navpoint->arrow_index, vertical_offset, scale, &geometry);
                    chud_expand_pixel32(c_chud_navpoint_manager::get_managed_navpoint_arrow_color(navpoint), &render_data.chud_color_outputs[0]);
                    render_data.chud_scalar_output_ef.l = navpoint->alpha * navpoint->progress;
                    if (chud_apply_render_data(&render_data, false))
                        chud_draw_widget_geometry(&geometry, &basis, false, false);
                }
                if (offscreen_amount == 0.0f)
                {
                    if (show_distance)
                    {
                        wchar_t text[128];
                        usnzprintf(text, 128, L"%s", manager->distance_text);
                        real_rectangle2d clip;
                        set_real_rectangle2d(&clip, 0.0f, (manager->distance_width + 1.0f) * manager->distance_progress, 0.0f, 1000.0f);
                        if (chud_compute_text_geometry((chud_navpoints_widget_geometry *)&geometry, text, 6, false, 1.0f, &clip))
                        {
                            geometry.x1 = manager->distance_width * manager->distance_progress;
                            float offset = 14.0f * scale;
                            float rotation = 0.0f;
                            float text_scale = 0.75f;
                            chud_draw_compute_geometry_common_internal(&geometry, 0.0f, 1.0f, offset, rotation, scale * text_scale, scale * text_scale);
                            chud_expand_pixel32(c_chud_navpoint_manager::get_managed_navpoint_text_color(navpoint), &render_data.chud_color_outputs[0]);
                            render_data.chud_scalar_output_ef.l = navpoint->alpha * navpoint->progress;
                            if (chud_apply_render_data(&render_data, false))
                                chud_draw_widget_geometry(&geometry, &basis, false, false);
                        }
                    }
                    if (chud_draw_navpoint_text_length(navpoint->text, 4) > 0 || selected)
                    {
                        wchar_t text[128];
                        real_rectangle2d clip;
                        if (selected)
                        {
                            usnzprintf(text, 128, k_two_string_format, manager->text, navpoint->text);
                            set_real_rectangle2d(&clip, (1.0f - manager->text_progress) * manager->text_width, 1000.0f, 0.0f, 1000.0f);
                        }
                        if (chud_compute_text_geometry((chud_navpoints_widget_geometry *)&geometry, selected ? text : navpoint->text, 6,
                                false, 1.0f, selected ? &clip : 0))
                        {
                            if (selected)
                                geometry.x0 = (1.0f - manager->text_progress) * manager->text_width;
                            chud_draw_compute_geometry_common_internal(&geometry, 0.5f, 1.0f, 0.0f, vertical_offset, scale, scale);
                            chud_expand_pixel32(c_chud_navpoint_manager::get_managed_navpoint_text_color(navpoint), &render_data.chud_color_outputs[0]);
                            vertical_offset -= (geometry.y1 - geometry.y0) * scale;
                            render_data.chud_scalar_output_ef.l = navpoint->alpha * navpoint->progress;
                            if (chud_apply_render_data(&render_data, false))
                                chud_draw_widget_geometry(&geometry, &basis, false, false);
                        }
                    }
                    if (navpoint->icon_index != NONE)
                    {
                        vertical_offset -= chud_build_geometry_from_navpoint(user_index, 0, navpoint->icon_index, vertical_offset, scale, &geometry);
                        chud_expand_pixel32(c_chud_navpoint_manager::get_managed_navpoint_text_color(navpoint), &render_data.chud_color_outputs[0]);
                        render_data.chud_scalar_output_ef.l = navpoint->alpha * navpoint->progress;
                        if (chud_apply_render_data(&render_data, false))
                            chud_draw_widget_geometry(&geometry, &basis, false, false);
                    }
                }
            }
        }
    }
}


static __forceinline unsigned char chud_draw_directional_anchor(long user_index, s_chud_draw_widget_data *draw, long anchor, real_matrix4x3 *basis)
{
    bool result = true;
    bool use_widget_transform = false;
    c_player_view *view = c_player_view::get_current();
    render_camera *camera = &view->rasterizer_camera;
    render_projection *projection = &view->rasterizer_projection;
    real_vector3d forward = global_identity4x3->forward;
    real_vector3d up = global_identity4x3->up;
    real_vector3d left = global_identity4x3->left;
    chud_table_row *row = &(chud_globals->rows)[chud_get_user_skin_index(user_index)];
    real_point3d anchor_point;
    real_matrix4x3 widget_transform;
    switch (anchor)
    {
    case _chud_draw_anchor_top_left:
        anchor_point.x = 0.0f;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_grenade_human:
    case _chud_draw_anchor_grenade_plasma:
    case _chud_draw_anchor_grenade_brute:
    case _chud_draw_anchor_grenade_fire:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        long index = anchor - _chud_draw_anchor_grenade_human;
        if (user->m_display_state.perspective_changed != 0 && anchor == _chud_draw_anchor_grenade_fire)
            index -= 1;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        anchor_point.x = row->grenade_anchor_offset * (float)index;
        break;
    }
    case _chud_draw_anchor_top_right:
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_bottom_right:
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_bottom_left:
        anchor_point.x = 0.0f;
        anchor_point.y = g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_tracked_object:
    {
        s_chud_persistent_display_state *update = &c_chud_manager::x_persistent_user_data[draw->user_index].m_display_state;
        if (update->point_tracker_enabled != 0 && chud_compute_anchor_basis_from_tracker(&update->point_tracker, basis, true))
        {
            anchor_point = basis->position;
            break;
        }
    }
    case _chud_draw_anchor_crosshair:
    case _chud_draw_anchor_reticle:
    {
        s_player_globals *observer = g_observer_globals;
        rectangle2d *bounds = &camera->window_pixel_bounds;
        real_point2d viewport_point;
        real_point3d project_point;
        project_point.x = 0.0f;
        project_point.y = -observer->blocks[draw->user_index].camera_result.direction_error;
        project_point.z = -1.0f;
        chud_draw_try_and_get_screen_point(camera, projection, bounds, project_point.n, &viewport_point);
        float x;
        float y;
        if (camera->frustum_adjustment)
        {
            float width = (float)(camera->window_pixel_bounds.x1 - camera->window_pixel_bounds.x0);
            float height = (float)(camera->window_pixel_bounds.y1 - bounds->y0);
            x = (viewport_point.x - (0.5f - camera->frustum_center.x / camera->frustum_scale.x * 0.5f) * width) * camera->frustum_scale.x + width * 0.5f;
            y = (viewport_point.y - ((camera->frustum_center.y / camera->frustum_scale.y + 1.0f) * 0.5f) * height) * camera->frustum_scale.y + height * 0.5f;
        }
        else
        {
            y = viewport_point.y;
            x = viewport_point.x;
        }
        short x0 = camera->window_pixel_bounds.x0;
        // match: 0x18de28: a plain long swaps r16/r17 (the -1 stored to 700(r1) moves to r16, the lwz 0(r13) to r17)
        const long &y0 = (unsigned short)bounds->y0;
        real_point2d curve_point;
        curve_point.y = y - (float)(short)y0;
        curve_point.x = x - (float)x0;
        c_curved_hud_test curved_test;
        float curved_x;
        float curved_y;
        curved_test.inverse_transform(&curve_point, &curved_x, &curved_y);
        anchor_point.x = curved_x * g_chud_draw_globals.width;
        anchor_point.y = (1.0f - curved_y) * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    }
    case _chud_draw_anchor_top_center:
        anchor_point.x = g_chud_draw_globals.width * 0.5f;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_curvature_origin:
        anchor_point.x = g_chud_draw_globals.curvature_pixel_scale.i;
        anchor_point.y = g_chud_draw_globals.curvature_pixel_scale.j;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_bottom_center:
        anchor_point.x = g_chud_draw_globals.width * 0.5f;
        anchor_point.y = g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_parent:
    {
        anchor_point.x = 0.0f;
        anchor_point.y = 0.0f;
        anchor_point.z = 0.0f;
        if (chud_compute_parent_widget_transform(user_index, draw, &widget_transform))
            use_widget_transform = 1;
        break;
    }
    case _chud_draw_anchor_center:
        anchor_point.x = g_chud_draw_globals.width * 0.5f;
        anchor_point.y = g_chud_draw_globals.height * 0.5f;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_backpack_weapon:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        anchor_point.x = user->m_display_state.highlight_value;
        anchor_point.y = user->m_display_state.weapon_state_value;
        anchor_point.z = 0.0f;
        break;
    }
    case _chud_draw_anchor_equipment:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        anchor_point.x = 0.0f;
        anchor_point.y = user->m_display_state.weapon_state_value;
        anchor_point.z = 0.0f;
        break;
    }
    case _chud_draw_anchor_scoreboard_friendly:
    case _chud_draw_anchor_scoreboard_friendly_score_offset:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = g_chud_draw_globals.height - user->m_display_state.visible_opacity * row->scoreboard_spacing;
        anchor_point.z = 0.0f;
        if (anchor == _chud_draw_anchor_scoreboard_friendly_score_offset)
            anchor_point.x = g_chud_draw_globals.width - (float)user->m_display_state.count_text_width;
        break;
    }
    case _chud_draw_anchor_scoreboard_enemy:
    case _chud_draw_anchor_scoreboard_enemy_score_offset:
    {
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[draw->user_index];
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = g_chud_draw_globals.height - (1.0f - user->m_display_state.visible_opacity) * row->scoreboard_spacing;
        anchor_point.z = 0.0f;
        if (anchor == _chud_draw_anchor_scoreboard_enemy_score_offset)
            anchor_point.x = g_chud_draw_globals.width - (float)user->m_display_state.count_text_width;
        break;
    }
    case _chud_draw_anchor_messaging:
        anchor_point.x = 0.0f;
        anchor_point.y = g_chud_draw_globals.curvature_info->notification_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_state_message_right:
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.y = g_chud_draw_globals.curvature_info->state_left_right_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_state_message_left:
        anchor_point.x = 0.0f;
        anchor_point.y = g_chud_draw_globals.curvature_info->state_left_right_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_message_bottom_state:
        anchor_point.x = (float)(g_chud_draw_globals.width * 0.5);
        anchor_point.y = g_chud_draw_globals.curvature_info->message_anchor_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_message_bottom_primary:
        anchor_point.x = (float)(g_chud_draw_globals.width * 0.5);
        anchor_point.y = g_chud_draw_globals.curvature_info->state_message_vertical_offset * g_chud_draw_globals.height;
        anchor_point.z = 0.0f;
        break;
    case _chud_draw_anchor_medal:
    case _chud_draw_anchor_medal_slot_one:
    case _chud_draw_anchor_medal_slot_two:
    case _chud_draw_anchor_medal_slot_three:
    case _chud_draw_anchor_medal_slot_four:
    {
        float base_offset = chud_globals->scoreboard_top_y;
        c_chud_persistent_user_data *user = &c_chud_manager::x_persistent_user_data[g_chud_draw_globals.user_index];
        long medal_index = user->m_display_state.color_group;
        float spacing = chud_globals->scoreboard_spacing;
        if (medal_index < 0)
            medal_index = 0;
        float scaled = (float)medal_index * spacing;
        float offset = 0.0f;
        switch (anchor)
        {
        case _chud_draw_anchor_medal: offset = scaled + base_offset; break;
        case _chud_draw_anchor_medal_slot_one: offset = scaled; break;
        case _chud_draw_anchor_medal_slot_two: offset = scaled - (double)spacing; break;
        case _chud_draw_anchor_medal_slot_three: offset = scaled - spacing * 2.0; break;
        case _chud_draw_anchor_medal_slot_four: offset = scaled - spacing * 3.0; break;
        }
        anchor_point.x = g_chud_draw_globals.width;
        anchor_point.z = 0.0f;
        anchor_point.y = g_chud_draw_globals.height - offset;
        break;
    }
    default:
        result = 0;
        break;
    }
    if (result)
    {
        basis->scale = 1.0f;
        basis->forward = forward;
        basis->left = left;
        basis->up = up;
        basis->position = anchor_point;
        if (use_widget_transform)
            matrix4x3_compute_product(&widget_transform, basis, basis);
    }
    return result;
}
typedef s_chud_medal_message s_chud_medal;

typedef s_chud_text_entry s_chud_state_text;

static __forceinline void chud_draw_expand_pixel32(unsigned long pixel, real_vector4d *color)
{
    const float scale = 0.003921569f;

    color->i = (float)((pixel >> 16) & 0xff) * scale;
    color->j = (float)((pixel >> 8) & 0xff) * scale;
    color->k = (float)(pixel & 0xff) * scale;
    color->l = (float)(pixel >> 24) * scale;
}

static __forceinline bool chud_draw_get_medal_sequence_index(long user_index, long medal, long *sequence_index)
{
    chud_table_row *definition = chud_draw_get_medal_skin(user_index);
    bool result = false;
    const unsigned long &medal_bitmap = definition->medal_bitmap.index;
    if ((long)medal_bitmap != NONE)
    {
        for (long index = 0; index < definition->medals.count; index++)
        {
            if (static_cast<long *>(definition->medals.address)[index] == medal)
            {
                *sequence_index = index;
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x18d498 ?draw@c_chud_messaging_manager@@QAAXJ@Z
void c_chud_messaging_manager::draw(long user_index)
{
    chud_table_row *definition = chud_draw_get_medal_skin(user_index);

    s_widget_geometry message_geometry;
    message_geometry.texture_handle = NONE;
    for (long message_index = 0; message_index < 5; message_index++)
    {
        s_chud_message *message = &m_messages[message_index];
        if (message->active && message->visible &&
            chud_compute_text_geometry((chud_navpoints_widget_geometry *)&message_geometry, message->text, 6, true, message->scale + 1.0f, 0))
        {
            float offset_scale = g_chud_draw_globals.curvature_info->message_offset_scale;
            float height_scale = g_chud_draw_globals.curvature_info->message_height_scale;
            real_matrix4x3 basis;
            chud_draw_directional_anchor(user_index, 0, 22, &basis);
            s_chud_render_data render_data;
            chud_build_custom_render_data(&render_data, 2);
            chud_expand_pixel32(chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 2), &render_data.chud_color_outputs[0]);
            chud_expand_pixel32(
                chud_globals_get_skin_color(chud_globals, g_chud_draw_globals.skin_index, message->context + 19),
                &render_data.chud_color_outputs[1]);
            render_data.chud_scalar_output_abcd.j = message->scale;
            render_data.chud_scalar_output_ef.l *= message->alpha;
            chud_draw_compute_geometry_common_internal(&message_geometry, 0.0f, 0.5f, 0.0f, (message->offset + 0.5f) * offset_scale,
                message->height * height_scale, message->height * height_scale);
            if (chud_apply_render_data(&render_data, false))
                chud_draw_widget_geometry(&message_geometry, &basis, false, false);
        }
    }

    s_widget_geometry state_geometry;
    state_geometry.texture_handle = NONE;
    for (long state_index = 0; state_index < 4; state_index++)
    {
        s_chud_state_text *state = &m_state_messages[state_index];
        if (state->active && state->precached &&
            chud_compute_text_geometry((chud_navpoints_widget_geometry *)&state_geometry, state->text, 6, true, state->scale, 0))
        {
            float scale = g_chud_draw_globals.curvature_info->state_text_scale;
            real_matrix4x3 basis;
            chud_compute_anchor_basis(user_index, 0, state_index + 25, &basis);
            s_chud_render_data render_data;
            chud_build_custom_render_data(&render_data, 2);
            chud_draw_expand_pixel32(chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 2), &render_data.chud_color_outputs[0]);
            chud_draw_expand_pixel32(chud_draw_get_skin_color(g_chud_draw_globals.skin_index, 22), &render_data.chud_color_outputs[1]);
            render_data.chud_scalar_output_abcd.j = state->offset;
            render_data.chud_scalar_output_ef.l *= state->alpha;
            float anchor_x = 0.0f;
            switch (state_index)
            {
            case 1: anchor_x = 0.0f; break;
            case 0: anchor_x = 1.0f; break;
            case 2: anchor_x = 0.5f; break;
            case 3: anchor_x = 0.5f; break;
            }
            chud_draw_compute_geometry_common_internal(&state_geometry, anchor_x, 0.0f, 0.0f, 0.0f, scale, scale);
            if (chud_apply_render_data(&render_data, false))
                chud_draw_widget_geometry(&state_geometry, &basis, false, false);
        }
    }

    s_widget_geometry medal_geometry;
    medal_geometry.texture_handle = NONE;
    for (long medal_index = 0; medal_index < 4; medal_index++)
    {
        s_chud_medal *medal = &m_medals[medal_index];
        if (medal->active && medal->shown)
        {
            long sequence_index;
            long animation_index;
            bool found;
            if (game_globals->options.game_mode == 1)
            {
                long value = medal->medal_value;
                if (chud_globals->medal_bitmap.index != NONE)
                {
                    sequence_index = MIN(MAX(value, 0),
                        static_cast<bitmap_group *>(tag_get('bitm', chud_globals->medal_bitmap.index))->sequences.count);
                }
                animation_index = chud_globals->medal_animation.index;
                found = true;
            }
            else
            {
                found = chud_draw_get_medal_sequence_index(user_index, medal->medal_value, &sequence_index);
                animation_index = definition->medal_animation.index;
            }
            if (found)
            {
                float width = game_globals->options.game_mode == 1 ? chud_globals->medal_width : g_chud_draw_globals.curvature_info->medal_width;
                float height = game_globals->options.game_mode == 1 ? chud_globals->medal_height : g_chud_draw_globals.curvature_info->medal_height;
                chud_build_medal_geometry(user_index, sequence_index, &medal_geometry);
                s_chud_render_data render_data;
                chud_build_custom_render_data(&render_data, 11);
                render_data.chud_scalar_output_ef.l *= medal->alpha;
                chud_draw_compute_geometry_common_internal(&medal_geometry, 0.5f, 0.5f, (medal->eased + 0.5f) * height, height * -0.5f, width, width);
                real_matrix4x3 basis;
                if (game_globals->options.game_mode == 1)
                {
                    chud_compute_anchor_basis(user_index, 0, _chud_draw_anchor_medal, &basis);
                    medal_geometry.offset.x += chud_globals->medal_offset_x;
                    medal_geometry.offset.y += chud_globals->medal_offset_y;
                }
                else
                {
                    chud_draw_directional_anchor(user_index, 0, 22, &basis);
                }
                if (animation_index != NONE)
                {
                    s_chud_aux_animation_data aux;
                    aux.scalar = 1.0f;
                    aux.zero = 0.0f;
                    float time = (float)((game_time_globals->time - medal->display_time) * 1000 / game_tick_rate());
                    ::chud_apply_widget_animation_definition(time, (::s_chud_animation_definition *)tag_get('chad', animation_index),
                        &basis, &medal_geometry, &aux);
                    render_data.chud_scalar_output_ef.l *= aux.scalar;
                }
                if (chud_apply_render_data(&render_data, false))
                    chud_draw_widget_geometry(&medal_geometry, &basis, false, false);
            }
        }
    }
}


static __forceinline real_rectangle2d *chud_draw_scale_rectangle(const real_rectangle2d *rectangle, float scale, real_rectangle2d *result)
{
    for (long index = 0; index < 4; ++index)
        result->n[index] = rectangle->n[index] * scale;
    return result;
}

static inline void chud_draw_update_damage_parameters(const s_directional_damage *damage,
    const float &alpha, s_widget_geometry *geometry, s_chud_render_data *render_data)
{
    const float *inputs[2] = { &damage->alpha, &damage->rotation };
    float *outputs[2] = { &geometry->rotation.yaw, &render_data->chud_scalar_output_ef.l };
    float scales[2] = { 1.0f, alpha };
    long channel = 0;
    *outputs[channel] = scales[channel] * *inputs[channel];
    channel = 1;
    *outputs[channel] = scales[channel] * *inputs[channel];
}

// @address 0x18de28 chud_draw_damage_indicators_and_curvature
extern "C" void chud_draw_damage_indicators_and_curvature(s_directional_damage *entries, long user_index)
{
    if (chud_draw_has_active_timed_slots(entries, user_index) && g_render_enable_screen_flashes)
    {
        chud_table_row *definition = &(chud_globals->rows)[chud_get_user_skin_index(user_index)];
        D3DDevice *device = c_rasterizer::get_device();
        if (definition->navpoint_arrow_bitmap.index != NONE)
        {
            s_widget_geometry geometry;
            geometry.texture_handle = NONE;
            D3DDevice_SetRenderState_ColorWriteEnable(device, 15);
            s_chud_render_data render_data;
            chud_build_custom_render_data(&render_data, 0);
            s_chud_draw_widget_data draw;
            draw.user_index = user_index;
            real_matrix4x3 basis;
            chud_draw_directional_anchor(user_index, &draw, _chud_draw_anchor_reticle, &basis);
            real_rectangle2d bounds;
            set_real_rectangle2d(&bounds, -40.0f, 40.0f, 0.0f, 40.0f);
            real_rectangle2d texture_bounds;
            set_real_rectangle2d(&texture_bounds, 0.0f, 1.0f, 0.0f, 1.0f);
            chud_widget_build_quads_from_bounds(&bounds, &bounds, &texture_bounds, &geometry, 1.0f, 1.0f, false);
            chud_draw_compute_geometry_common_internal(&geometry, 0.5f, 4.0f, 0.0f, 0.0f, 1.0f, 1.0f);
            geometry.texture_handle = NONE;
            if (definition->navpoint_arrow_bitmap.index != NONE)
            {
                const bitmap_data *bitmap;
                bitmap_group_get_xenon_hardware_format(definition->navpoint_arrow_bitmap.index, 0,
                    (c_rasterizer_texture_ref *)&geometry.texture_handle, &bitmap, 0);
                geometry.render_type = 3;
            }
            render_data.chud_color_outputs[1].i = 1.0f;
            render_data.chud_color_outputs[1].j = 0.0f;
            render_data.chud_color_outputs[1].k = 0.0f;
            render_data.chud_color_outputs[1].l = 1.0f;
            float alpha = render_data.chud_scalar_output_ef.l;
            for (long index = 0; index < 4; ++index)
            {
                if (entries[index].end_time > game_time_globals->time && !(entries[index].rotation <= 0.0f))
                {
                    chud_draw_update_damage_parameters(&entries[index], alpha, &geometry, &render_data);
                    if (chud_apply_render_data(&render_data, false))
                        chud_draw_widget_geometry(&geometry, &basis, false, false);
                }
            }
        }
        s_widget_geometry geometry;
        geometry.texture_handle = NONE;
        long bitmap_index = NONE;
        s_chud_render_data render_data;
        chud_build_custom_render_data(&render_data, 23);
        real_matrix4x3 basis;
        chud_compute_anchor_basis(user_index, 0, 0, &basis);
        s_chud_globals_definition *globals = chud_globals;
        float scale = globals->curvature_scale;
        if (scale < 1.0f)
            scale = 1.0f;
        long render_type = 0;
        real_rectangle2d bounds;
        set_real_rectangle2d(&bounds, -200.0f, g_chud_draw_globals.width + 200.0f, -200.0f, g_chud_draw_globals.height + 200.0f);
        real_rectangle2d texture_bounds;
        chud_draw_scale_rectangle(&bounds, (float)(1.0 / scale), &texture_bounds);
        chud_widget_build_quads_from_bounds(&bounds, &bounds, &texture_bounds, &geometry, 1.0f, 1.0f, false);
        chud_draw_compute_geometry_common_internal(&geometry, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
        set_real_point3d(&geometry.position, 0.0f, 0.0f, 0.0f);
        geometry.texture_handle = NONE;
        long skin_bitmap = ((chud_table_row *)globals->rows)[chud_get_user_skin_index(user_index)].curvature_bitmap.index;
        const bitmap_data *bitmap;
        if (skin_bitmap != NONE)
        {
            bitmap_index = skin_bitmap;
            bitmap_group_get_xenon_hardware_format(bitmap_index, 0, (c_rasterizer_texture_ref *)&geometry.texture_handle, &bitmap, 0);
            geometry.render_type = render_type;
        }
        if (bitmap_index == NONE)
        {
            bitmap_index = globals->curvature_bitmap.index;
            if (bitmap_index != NONE)
            {
                bitmap_group_get_xenon_hardware_format(bitmap_index, 0, (c_rasterizer_texture_ref *)&geometry.texture_handle, &bitmap, 0);
                geometry.render_type = render_type;
            }
        }
        if (g_rasterizer_alpha_blend_mode != 5)
        {
            D3DDevice *device = c_rasterizer::get_device();
            g_rasterizer_alpha_blend_mode = 5;
            D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
            D3DDevice_SetRenderState_BlendOp(device, 0);
            D3DDevice_SetRenderState_SrcBlend(device, 1);
            D3DDevice_SetRenderState_DestBlend(device, 7);
        }
        c_rasterizer::end_high_quality_blend();
        if (chud_apply_render_data(&render_data, false))
        {
            c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)25);
            c_rasterizer::set_sampler_address_mode(
                1, c_rasterizer::_sampler_address_border, c_rasterizer::_sampler_address_border, c_rasterizer::_sampler_address_border);
            chud_draw_widget_geometry(&geometry, &basis, false, false);
        }
    }
}


