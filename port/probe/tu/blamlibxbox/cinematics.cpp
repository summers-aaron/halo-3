// blamlibxbox/cinematics.obj: from the records 0x8b4d8 0x119858 0x1349a8 0x18fef0 0x1909c0 0x191118 0x1b7030 0x1d6ec8 0x1d74b8 0x23f350 0x23f360 0x23f448 0x23f4d0 0x23f4f0 0x23f5e0 0x23f728 0x23f818 0x23f8c0 0x23f9f8 0x23faa8 0x23fe10 0x23ff48 0x23ffd8 0x23ffe8 0x240050 0x2400c8 0x2401d8 0x2402a0 0x2403e8 0x240830 0x2408e0 0x240a28 0x240b88 0x240c08 0x240cf8 0x240dc8 0x240ea0 0x240fe8 0x241058 0x2411e8 0x241478 0x2415e8 0x241688 0x2416c0 0x241760 0x241798 0x301498 0x3332f0 0x448150 0x451878 0x4549e8 0x454b58 0x487830 0x708148 0x708158 0x70bd00 0x70bd18
#define _PPC_
#include "sound/sound_pause_state_types.h"
#include "cseries/string_literal_pool.h"
#include "camera/cinematics_main_requests.h"
#include "rasterizer/rasterizer_device_state.h"
#include "objects/object_name_list.h"
#include "camera/cinematics_dependencies.h"
#include "game/game_allegiance_data.h"
#include "scenario/scenario_runtime_symbols.h"
#include "rasterizer/rasterizer.h"
#include "sound/game_sound_globals.h"
#include "interface/chud/chud_manager.h"
#include "objects/object_runtime_globals_types.h"
#include "render/render_texture_camera_globals.h"
#include "multithreading/thread_tls.h"
#include "memory/gameplay_tls_globals.h"
#include "camera/cinematic_scene_types.h"
#include "camera/cinematic_types.h"
#include "game/game_globals.h"
#include "text/font_cache_thread_state_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "text/font_id_types.h"
#include "math/matrix_math_types.h"
#include "game/game_globals_storage.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "objects/object_iterator.h"
#include "objects/object_placement.h"
#include "objects/object_datum.h"
#include "interface/chud/chud_globals_definitions.h"
#include <windef.h>
#include <winbase.h>
#include <d3d9.h>
#include "cseries/cseries.h"
#include <string.h>
#include <math.h>
#include "game/game_time_types.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "game/players.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "units/units.h"
#include "cutscene/cinematics.h"
#include "game/game_system_types.h"
#include "effects/player_effects.h"
#include "sound/sound_manager_types.h"
#include "math/real_math_types.h"
#include "game/player_identifier_types.h"
#include "cseries/language.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_cinematic_reference_types.h"
#include "cache/restricted_memory.h"
#include "game/game_time.h"
#include "interface/interface_constants.h"
#include "rasterizer/rasterizer_main.h"
#include "sound/sound_manager.h"
#include "text/draw_string.h"
#include "interface/chud/chud.h"
#include "interface/user_interface.h"
#include "objects/lights.h"
#include "objects/light_cinematic_types.h"
#include "sound/sound_classes.h"
#include "sound/sound_class_definitions.h"
#include "sound/sound_classes_types.h"
#include "sound/sound_globals_reference_types.h"
#include "text/font_cache_lock.h"
#include "main/loading.h"
#include "sound/game_sound_definitions.h"
#include "interface/controller_types.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "objects/object_marker_globals_types.h"
#include "interface/chud/chud_dispose_state_types.h"
#include "cutscene/cinematic_title_types.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "game/player_mapping_types.h"
#include "math/real_math_globals.h"
#include "rasterizer/rasterizer_dx9_dynamic_geometry.h"
#include "game/game_time_inline.h"
#include "cutscene/cinematic_new_globals_types.h"
#include "physics/havok_monitor_storage.h"
#include "camera/camera_script_globals_types.h"
#include "sound/sound_pause.h"
#include "objects/object_lifecycle.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_registration.h"
#include "camera/camera_scripting.h"
#include <stdlib.h>
#include "objects/object_broadphase_globals.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "math/xenon_matrix_types.h"
#include "interface/chud/chud_user_data_types.h"
#include "math/xm_scalar_sincos_constants.h"
#include "scenario/scenario_cutscene_title_types.h"
#include "text/font_cache_types.h"
#include "text/draw_string_types.h"
#include "interface/chud/chud_draw_string.h"
#include "interface/chud/chud_messaging_manager.h"
#include "memory/data_types.h"
#include "game/player_types.h"
#include "units/unit_types.h"
#include "items/weapon_definitions.h"
#include "units/unit_definitions.h"
#include "game/player_with_unit_iterator.h"
#include "scenario/scenario_starting_profile.h"
#include "text/string_list_strings.h"
#include "text/rasterizer_draw_string_types.h"
#include "objects/object_header_access.h"
#include "memory/data.h"
#include "units/unit_inventory_inline.h"

struct projectile_datum;
struct s_font_header;

struct presentation_parameters : D3DPRESENT_PARAMETERS {};
typedef char cinematic_presentation_size[(sizeof(void *) != 4 || ((sizeof(presentation_parameters) == 0x7c))) ? 1 : -1];

// @address 0x8c4839 g_cinematic_lightmap_shadow_enabled
// @address 0x11df8c0 g_cinematic_light_globals
// @address 0x7f4310 g_object_marker_globals
extern "C" {
s_object_marker_globals g_object_marker_globals[2];
}
// @address 0x11df69c cinematic_new_globals
// @address 0x11df6b8 cinematic_globals
__declspec(thread) s_cinematic_new_globals_definition *cinematic_new_globals;
__declspec(thread) s_cinematic_globals_definition *cinematic_globals;
extern "C" {
__declspec(thread) s_cinematic_light_globals *g_cinematic_light_globals;
}
// @address 0x8c4694 g_rasterizer_device
// @address 0x948f78 main_globals
// @address 0x10b0 k_font_cache_vtable
// @address 0x25888 k_observer_default_aspect_ratio
// @address 0x74b910 g_havok_monitor_globals
// @address 0x73042c g_interface_aspect_ratio_scaling
extern "C" {
real_vector2d g_interface_aspect_ratio_scaling = { 1.0f, 1.0f };
}
// @address 0x81da20 g_rasterizer_last_rectangles
extern "C" {
s_rasterizer_last_rectangles g_rasterizer_last_rectangles;
}
// @address 0x1401e8 g_controller_globals
// @address 0x73a248 ?render_globals@c_rasterizer@@2Us_rasterizer_render_globals@@A
// @address 0x8a1ed8 g_active_cinematic_zone_mask
extern "C" {
unsigned int g_active_cinematic_zone_mask;
}
// @flags /TP /O2 /Oc
// @address 0x18fef0 ?draw_quad@@YAXPATrectangle2d@@K@Z
// @address 0x1d6ec8 ?__tls_set_g_cinematic_globals_allocator@@YAXPAX@Z
// @address 0x23f9f8 ?cinematic_set_title_delayed@@YAXFM@Z
// @address 0x23fe10 ?cinematic_move_attached_objects@@YAXJJJJ@Z
// @address 0x240fe8 ?cinematic_destroy@@YAXXZ
// @address 0x2415e8 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x2416c0 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_new_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x241798 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_light_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x454b58 ?XMMatrixLookToLH@@YA?AU_XMMATRIX@@U__vector4@@00@Z
// @address 0x2401d8 cinematic_compute_letterbox_coverage
// @address 0x191118 ?cinematic_clear_letterbox_bars@@YAX_N@Z
typedef unsigned int uint;

struct s_cinematics_wchar_string { wchar_t characters[256]; };


// @address 0x7831f4 g_cinematic_light_globals_allocator

// 0x23f4f0




// 0x8b4d8
bool cinematic_in_progress()
{
    bool result = false;
    if (cinematic_globals) {
        result = cinematic_globals->in_progress;
    }
    return result;
}


void draw_quad(union rectangle2d *rect, unsigned long flags)
{
    if (flags & 0xff000000) {
        point2d points[4];
        points[0].x = rect->x0;
        points[0].y = rect->y0;
        points[1].x = rect->x1;
        points[1].y = rect->y0;
        points[2].x = rect->x1;
        points[2].y = rect->y1;
        points[3].x = rect->x0;
        points[3].y = rect->y1;
        rasterizer_quad_screenspace(points, flags, NULL, 0, false);
    }
}

void __tls_set_g_cinematic_globals_allocator(void *value)
{
    game_time_globals = (game_time_globals_definition *)value;
}

// @address 0x23f350 __tls_set_g_cinematic_light_globals_allocator
extern "C" void __tls_set_g_cinematic_light_globals_allocator(void *value)
{
    g_cinematic_light_globals = static_cast<s_cinematic_light_globals *>(value);
}

// @address 0x23f360 cinematic_initialize
extern "C" void cinematic_initialize(void)
{
    cinematic_globals = (s_cinematic_globals_definition *)g_cinematic_globals_allocator.reserve_memory("cinematic globals", 0,
        sizeof(s_cinematic_globals_definition), 0);
    memset(cinematic_globals, 0, sizeof(*cinematic_globals));
    cinematic_new_globals = (s_cinematic_new_globals_definition *)g_cinematic_new_globals_allocator.reserve_memory("cinematic new globals", 0,
        sizeof(s_cinematic_new_globals_definition), 0);
    memset(cinematic_new_globals, 0, sizeof(*cinematic_new_globals));
    g_cinematic_light_globals = static_cast<s_cinematic_light_globals *>(
        g_cinematic_light_globals_allocator.reserve_memory("cinematic light globals", 0, sizeof(s_cinematic_light_globals), 0));
    memset(g_cinematic_light_globals, 0, sizeof(s_cinematic_light_globals));
    cinematic_new_globals->current_scene = -1;
    cinematic_new_globals->scene_tag_index = -1;
}

// @address 0x5bec98 ?cinematic_dispose@@YAXXZ
void cinematic_dispose()
{
}

// @address 0x23f448 ?cinematic_initialize_for_new_map@@YAXXZ
void cinematic_initialize_for_new_map()
{
    s_cinematic_globals_definition *globals = cinematic_globals;
    memset(globals, 0, sizeof(*globals));
    s_cinematic_new_globals_definition *new_globals = cinematic_new_globals;
    memset(new_globals, 0, sizeof(*new_globals));
    memset(g_cinematic_light_globals, 0, sizeof(s_cinematic_light_globals));
    for (long i = 0; i < 6; ++i) {
        ((long *)globals->titles)[i] = -1;
    }
    new_globals->current_scene = -1;
    new_globals->scene_tag_index = -1;
}

// 0x23f4d0
void cinematic_dispose_from_old_map()
{
    cinematic_globals->in_progress = false;
    cinematic_globals->old_map_active = false;
}

// @address 0x23f4f0 cinematic_update_activation
extern "C" void cinematic_update_activation(const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set,
    c_scenario_resource_registry *pending_zone_registry)
{
    long cinematic_tag_index = cinematic_new_globals->current_scene;
    if (cinematic_tag_index != -1) {
        bool active = false;
        long cinematic_index = 0;
        scenario *cinematics = global_scenario;
        if (cinematics->cinematics.count > 0) {
            do {
                if (((s_scenario_cinematic_reference *)cinematics->cinematics.address)[cinematic_index].cinematic.index == cinematic_tag_index &&
                    (new_zone_set->cinematic_zone_mask & (1u << cinematic_index)) != 0) {
                    active = true;
                }
                ++cinematic_index;
            } while (cinematic_index < cinematics->cinematics.count);
        }
        if (!active) {
            s_havok_globals *havok = &g_havok_monitor_globals;
            havok->lock_count_4c += 1;
            cinematic_stop();
            havok->lock_count_4c -= 1;
        }
    }
    if ((old_zone_set->cinematic_zone_mask | new_zone_set->cinematic_zone_mask) != new_zone_set->cinematic_zone_mask && g_chud_cinematic_state->index != -1) {
        chud_stop_screen_effect();
    }
}

// 0x23f728
// @address 0x23f728 cinematic_stop
// 0x23f5e0
// @address 0x23f5e0 cinematic_start
static __forceinline void cinematics1_object_set_custom_animations_prevent_lipsync_head_movement(bool enable)
{
    object_globals->custom_animations_prevent_lipsync_head_movement = enable;
}
static __forceinline void cinematics1_object_set_custom_animations_hold_on_last_frame(bool enable)
{
    object_globals->custom_animations_hold_on_last_frame = enable;
}
static __forceinline void cinematics1_player_input_enable(bool enable) { players_globals->inhibited = !enable; }
static __forceinline void cinematics1_ai_globals_dialogue_enable(bool enable) { ai_globals->dialogue_enabled = enable; }
static __forceinline bool cinematics1_game_is_playback()
{
    bool result = false;
    if (game_globals->options.game_simulation == 4) {
        result = true;
    }
    return result;
}
static __forceinline void cinematics1_projectiles_delete_all()
{
    if (!cinematics1_game_is_playback()) {
        c_object_iterator<projectile_datum> iterator;
        iterator.begin(32, 0);
        while (iterator.next()) {
            object_delete(iterator.get_index());
        }
    }
}
static __forceinline void cinematics1_game_pvs_enable_scripted_camera_pvs() { game_globals->scripted_camera_pvs = true; }
static __forceinline void cinematics1_hs_disable_cinematic_lightmap_shadow() { g_cinematic_lightmap_shadow_enabled = false; }
static __forceinline void cinematics1_scripting_game_sound_enable_ambience_details(bool enable)
{
    g_game_sound_globals->environment.ambience_details_enabled = enable;
}
extern "C" void cinematic_start(void)
{
    cinematics1_object_set_custom_animations_prevent_lipsync_head_movement(true);
    cinematics1_object_set_custom_animations_hold_on_last_frame(true);
    cinematics1_player_input_enable(false);
    cinematics1_ai_globals_dialogue_enable(false);
    cinematic_globals->in_progress = true;
    cinematic_globals->skip = false;
    cinematics1_projectiles_delete_all();
    cinematics1_game_pvs_enable_scripted_camera_pvs();
    cinematics1_hs_disable_cinematic_lightmap_shadow();
    cinematics1_scripting_game_sound_enable_ambience_details(true);
    if (!game_is_cooperative()) {
        XNotifyDelayUI(0x7fffffff);
    }
    user_interface_set_player_count_mode();
    g_camera_script_globals->camera_track.cinematic_tag_index = -1;
    g_camera_script_globals->camera_track.cinematic_time = 0;
    g_camera_script_globals->mode = 5;
    g_camera_script_globals->animation_changed = true;
    g_camera_script_globals->camera_track.cinematic_transition = false;
    g_camera_script_globals->camera_track.render_main_camera_to_texture = false;
}
extern "C" void cinematic_stop(void)
{
    s_cinematic_globals_definition *cinematic = cinematic_globals;
    object_globals->custom_animations_prevent_lipsync_head_movement = false;
    object_globals->custom_animations_hold_on_last_frame = false;
    game_globals->scripted_camera_pvs = false;
    players_globals->inhibited = false;
    ai_globals->dialogue_enabled = true;
    if (cinematic->in_progress) {
        scripted_player_effect_screen_fade_out(0.0f, 0.0f, 0.0f, 0);
    }
    cinematic->in_progress = false;
    cinematic->skip = false;
    s_camera_script_globals *director = g_camera_script_globals;
    memset(&director->field_of_view_time, 0, 4 * sizeof(float));
    director->mode = 1;
    director->animation_changed = true;
    g_render_texture_camera_globals->enabled = false;
    g_render_texture_camera_globals->dynamic_lights = false;
    cinematic_destroy();
    g_cinematic_lightmap_shadow_enabled = 0;
    user_interface_set_player_count_mode();
    XNotifyDelayUI(0);
}

enum e_sound_pause_state;

// @address 0x23f818 cinematic_set_paused
extern "C" void cinematic_set_paused(unsigned char pause_requested)
{
    unsigned char cinematic_running;
    if (pause_requested != 0) {
        cinematic_running = 0;
        if (!((cinematic_globals == 0))) {
            cinematic_running = cinematic_globals->in_progress;
        }
        if (cinematic_running == 0)
            return;
        XNotifyDelayUI(0);
        if (g_sound_manager_globals->paused == 1)
            return;
        sound_pause(_sound_pause_paused, true);
        return;
    }
    if (!(((unsigned int)g_sound_manager_globals->paused == 0))) {
        sound_pause(_sound_pause_unpaused, false);
    }
    return;
}

void cinematic_set_title_delayed(short title_index, float delay_seconds)
{
    short slot;
    for (slot = 0; slot < 4; ++slot) {
        if (cinematic_globals->titles[slot].index == -1) break;
    }
    if (slot < 4) {
        cinematic_globals->titles[slot].index = title_index;
        cinematic_globals->titles[slot].elapsed_ticks = (short)-game_seconds_to_ticks_round(delay_seconds);
    }
}

// The per-thread object marker pass state objects.cpp keeps while marker lookups run.


static __forceinline s_object_marker_globals *cinematics_get_object_marker_globals()
{
    return &g_object_marker_globals[g_registered_thread_index - 1];
}
static __forceinline void cinematics_object_marker_begin()
{
    s_object_marker_globals *globals = cinematics_get_object_marker_globals();
    globals->mark = globals->mark + 1;
    globals->active = true;
}
static __forceinline bool cinematics_object_mark(long object_index)
{
    s_object_marker_globals *globals = cinematics_get_object_marker_globals();
    long absolute_index = (unsigned short)object_index;
    long *slot = &globals->marks[absolute_index];
    long previous_mark = *slot;
    long mark = globals->mark;
    *slot = mark;
    return previous_mark != mark;
}
static __forceinline void cinematics_object_marker_end()
{
    cinematics_get_object_marker_globals()->active = false;
}

// 0x23fe10
void cinematic_move_attached_objects(long parent_object_index, long old_marker_name, long new_marker_name, long child_marker_name)
{
    object_marker marker;
    if (parent_object_index != -1 && internal_object_get_markers_by_string_id(parent_object_index, old_marker_name, &marker, 1, false) == 1) {
        object_datum *parent = (object_datum *)object_get(parent_object_index);
        long child_index = parent->first_child_object_index;
        cinematics_object_marker_begin();
        if (child_index != -1) {
            short marker_node_index = marker.node_index;
            do {
                object_datum *child = (object_datum *)((object_header_datum *)object_header_data->data)[(unsigned short)child_index].datum;
                long next_child_index = child->next_object_index;
                if (cinematics_object_mark(child_index) && child->parent_node_index == marker_node_index) {
                    object_detach(child_index);
                    object_attach_to_marker_with_type(parent_object_index, new_marker_name, child_index, child_marker_name);
                }
                child_index = next_child_index;
            } while (child_index != -1);
        }
        cinematics_object_marker_end();
    }
}

// @address 0x23ff48 cinematic_predict_sounds_in_range
extern "C" void cinematic_predict_sounds_in_range(s_cinematic_scene *scene, int target_shot_index, int first_frame, int end_frame)
{
    if (scene != 0 && target_shot_index >= 0 && target_shot_index < scene->shots.count) {
        hs_cinematic_shot *shot = &((hs_cinematic_shot *)scene->shots.address)[target_shot_index];
        int sound_index = 0;
        if (0 < shot->dialogue_count) {
            do {
                hs_cinematic_dialogue *sound = &shot->dialogue[sound_index];
                int sound_frame = sound->frame;
                if (sound_frame >= first_frame && sound_frame < end_frame)
                    scripted_sound_predict(sound->sound.index);
                sound_index = sound_index + 1;
            } while (sound_index < shot->dialogue_count);
        }
    }
}

// @address 0x23ffd8 cinematic_predict_shot_sounds
extern "C" void cinematic_predict_shot_sounds(s_cinematic_scene *scene, long shot_index, long end_frame, void *unused)
{
    cinematic_predict_sounds_in_range(scene, shot_index, 0, end_frame);
}

// @address 0x23ffe8 cinematic_get_skip_fade
float cinematic_get_skip_fade(void)
{
    float fraction = (float)(game_time_globals->time - cinematic_globals->start_time) * 0.1f;
    if (fraction <= 0.0f) {
        return 0.0f;
    }
    if (fraction >= 1.0f) {
        return 1.0f;
    }
    return fraction;
}

// @address 0x240050 cinematic_can_skip
bool cinematic_can_skip(void)
{
    bool result = false;
    bool in_progress = false;
    if (cinematic_globals) {
        in_progress = cinematic_globals->in_progress;
    }
    if (in_progress) {
        if (cinematic_new_globals) {
            long scene_index = cinematic_new_globals->current_scene;
            if (scene_index != -1) {
                result = ((s_cinematic_scene *)tag_get('cisc', scene_index))->type != 2;
            }
        }
    }
    return result;
}

// @address 0x240830 subtitle_set
void subtitle_set(long string_id, float delay_seconds)
{
    bool found = false;
    if (global_scenario) {
        long string_list_index = ((scenario *)global_scenario)->cinematic_subtitles.index;
        s_cinematics_wchar_string string;
        string.characters[0] = 0;
        string_list_get_normal_string_by_language(string_list_index, string_id, get_current_language(), &string);
        if (string.characters[0]) {
            found = true;
        }
    }
    if (found) {
        cinematic_globals->subtitle.string_id = string_id;
        cinematic_globals->subtitle.duration = delay_seconds + 1.5f;
        cinematic_globals->subtitle.time_remaining = delay_seconds + 1.5f;
    }
}


// @address 0x240a28 ?cinematic_object_create_from_cinematic_scene@@YAXPBD@Z
void cinematic_object_create_from_cinematic_scene(const char *name)
{
    if (cinematic_new_globals->scene_tag_index != -1)
    {
        s_cinematic_scene *scene = (s_cinematic_scene *)tag_get('cisc', cinematic_new_globals->scene_tag_index);
        long scene_object_index = -1;
        for (long i = 0; i < scene->objects.count; i++)
        {
            s_cinematic_scene_object *scene_object = (s_cinematic_scene_object *)tag_block_get_element_with_size(
                &scene->objects, i, sizeof(s_cinematic_scene_object));
            if (strcmp(scene_object->name, name) == 0)
                scene_object_index = i;
        }
        if (scene_object_index != -1)
        {
            s_cinematic_scene_object *scene_object = (s_cinematic_scene_object *)tag_block_get_element_with_size(
                &scene->objects, scene_object_index, sizeof(s_cinematic_scene_object));
            long object_definition_index = scene_object->object.index;
            s_tag_block *attachments = &scene_object->attachments;
            long object_index = cinematic_object_create(name, scene_object->variant_name, object_definition_index,
                (e_scene_object_type)scene_object->object.group_tag);
            if (object_index != -1)
            {
                for (long i = 0; i < attachments->count; i++)
                {
                    s_cinematic_scene_object_attachment *attachment =
                        (s_cinematic_scene_object_attachment *)tag_block_get_element_with_size(
                            attachments, i, sizeof(s_cinematic_scene_object_attachment));
                    long child_object_index = cinematic_object_create(attachment->object_name, 0,
                        attachment->attachment_type.index, _scene_object_type_unit);
                    if (child_object_index != -1)
                    {
                        object_attach_to_marker_with_type(cinematic_new_globals->entries[object_index].object_index, attachment->parent_marker_name,
                            cinematic_new_globals->entries[child_object_index].object_index, attachment->child_marker_name);
                    }
                }
            }
        }
    }
}

// @address 0x240b88 cinematic_object_get
long cinematic_object_get(const char *name)
{
    long result = -1;
    for (long index = 0; index < cinematic_new_globals->count; ++index) {
        if (result != -1) {
            break;
        }
        if (strcmp(name, cinematic_new_globals->entries[index].name) == 0) {
            result = cinematic_new_globals->entries[index].object_index;
        }
    }
    return result;
}

// @address 0x240c08 cinematic_object_get_from_scene
extern "C" long cinematic_object_get_from_scene(long handle)
{
    long result = -1;
    long scene_index = cinematic_new_globals->scene_tag_index;
    if (scene_index != -1) {
        s_cinematic_scene *list = (s_cinematic_scene *)tag_get('cisc', scene_index);
        if (handle >= 0 && handle < list->objects.count) {
            s_cinematic_scene_object *entry = &((s_cinematic_scene_object *)list->objects.address)[handle];
            if ((entry->flags & 1) != 0) {
                short name_index = scenario_object_name_index_from_string((scenario *)global_scenario, entry->name);
                if (name_index >= 0 && name_index < 0x800)
                    return ((long *)g_object_name_list)[name_index];
                return -1;
            }
            result = cinematic_object_get(entry->name);
        }
    }
    return result;
}

// @address 0x240cf8 cinematic_object_get_unit
long cinematic_object_get_unit(const char *name)
{
    long result = -1;
    for (long index = 0; index < cinematic_new_globals->count; ++index) {
        if (result != -1) {
            break;
        }
        if (strcmp(name, cinematic_new_globals->entries[index].name) == 0) {
            long object_index = cinematic_new_globals->entries[index].object_index;
            if (object_index != -1 && ((1 << object_get(object_index)->object_type) & 0x1003) != 0) {
                result = object_index;
            }
        }
    }
    return result;
}

// @address 0x240dc8 cinematic_object_get_scenery
long cinematic_object_get_scenery(const char *name)
{
    long result = -1;
    for (long index = 0; index < cinematic_new_globals->count; ++index) {
        if (result != -1) {
            break;
        }
        if (strcmp(name, cinematic_new_globals->entries[index].name) == 0) {
            long object_index = cinematic_new_globals->entries[index].object_index;
            if (object_index != -1) {
                long t = object_get(object_index)->object_type;
                if (((1 << t) & (1 << 6)) || ((1 << t) & (1 << 13))) {
                    result = object_index;
                }
            }
        }
    }
    return result;
}


// @address 0x240ea0 ?cinematic_object_destroy_internal@@YAXJ@Z
void cinematic_object_destroy_internal(long index)
{
    long object_index = cinematic_new_globals->entries[index].object_index;
    if (object_index != -1) {
        if (object_try_and_get_and_verify_type(object_index, (unsigned long)-1) != 0) {
            objects_purge_deleted_object(object_index);
        }
        cinematic_new_globals->entries[index].object_index = -1;
    }
    char *name = cinematic_new_globals->entries[index].name;
    strncpy(name, "", 32);
    name[31] = 0;
}

// 0x240fe8
void cinematic_destroy()
{
    lights_reset_cinematic_lighting();
    for (long index = 0; index < cinematic_new_globals->count; ++index) {
        ::cinematic_object_destroy_internal(index);
    }
    cinematic_new_globals->count = 0;
    cinematic_new_globals->current_scene = -1;
    cinematic_new_globals->scene_tag_index = -1;
    cinematic_new_globals->current_shot = 0;
}

// @address 0x2411e8 cinematic_reset_groups
extern "C" void cinematic_reset_groups(void)
{
    cinematic_new_globals->group_count = 0;
    for (long group_index = 0; group_index < 32; ++group_index) {
        cinematic_new_globals->groups[group_index].count = 0;
        for (long slot = 0; slot < 24; ++slot) {
            cinematic_new_globals->groups[group_index].object_indices[slot] = -1;
        }
    }
    if (cinematic_new_globals->scene_tag_index != -1) {
        s_cinematic_scene *scene_list = (s_cinematic_scene *)tag_get('cisc', cinematic_new_globals->scene_tag_index);
        if (scene_list && scene_list->objects.count > 0) {
            for (long index = 0; index < scene_list->objects.count; ++index) {
                long object_index = cinematic_object_get_from_scene(index);
                if (object_index != -1) {
                    object_get(object_index)->flags &= ~0x8000;
                }
            }
        }
    }
}

// @address 0x241478 cinematic_iterate_shots
extern "C" void cinematic_iterate_shots(long ticks, cinematics_shot_proc_t shot_proc, void *user_data)
{
    long cinematic_tag_index = cinematic_new_globals->current_scene;
    if (cinematic_tag_index == -1) {
        return;
    }
    long scene_tag_index = cinematic_new_globals->scene_tag_index;
    if (scene_tag_index == -1) {
        return;
    }
    long current_shot = cinematic_new_globals->current_shot;
    s_cinematic *cinematic = (s_cinematic *)global_tag_instances[(unsigned short)cinematic_tag_index].definition;
    if (current_shot < 0 || current_shot >= ((s_cinematic_scene *)tag_get('cisc', scene_tag_index))->shots.count) {
        return;
    }
    long first_shot = current_shot + 1;
    long remaining = ticks;
    long found_scene_index = -1;
    s_cinematic *scene_list = (s_cinematic *)tag_get('cine', cinematic_tag_index);
    for (long index = 0; index < scene_list->scenes.count; ++index) {
        if (((s_cinematic_scene_reference *)scene_list->scenes.address)[index].scene.index == scene_tag_index) {
            found_scene_index = index;
        }
    }
    for (long scene_index = found_scene_index; remaining > 0; ++scene_index, first_shot = 0) {
        if (scene_index < 0 || scene_index >= cinematic->scenes.count) {
            return;
        }
        s_cinematic_scene *scene = 
            (s_cinematic_scene *)tag_get('cisc', ((s_cinematic_scene_reference *)cinematic->scenes.address)[scene_index].scene.index);
        for (long shot_index = first_shot; shot_index >= 0 && shot_index < scene->shots.count; ++shot_index) {
            hs_cinematic_shot *shot = &((hs_cinematic_shot *)scene->shots.address)[shot_index];
            shot_proc(scene, shot_index, remaining, user_data);
            remaining -= shot->duration;
        }
    }
}

// 0x2415e8
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x2415e8: alignment_bits==0 assumption folds the forwarded argument to li r7,0
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_register_member(3, name, type, size, alignment_bits,
        __tls_set_g_game_time_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x241688 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x2416c0
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_new_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x2416c0: alignment_bits==0 assumption folds the forwarded argument to li r7,0
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_register_member(3, name, type, size, alignment_bits,
        __tls_set_g_object_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x241760 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_new_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_new_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x241798
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_light_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x241798: alignment_bits==0 assumption folds the forwarded argument to li r7,0
    __assume(alignment_bits == 0);
    long member = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, alignment_bits,
        __tls_set_g_cinematic_light_globals_allocator, 0, 0);
    g_cinematic_light_globals = static_cast<s_cinematic_light_globals *>(restricted_region_get_member_address(3, member));
    m_member_index = member;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}


static __forceinline bool cinematics_structure_active(long index)
{
    if (index == -1 || (unsigned long)index > 1000) return true;
    return index < 32 && ((1UL << index) & g_active_cinematic_zone_mask) != 0;
}

// @address 0x301498 cinematic_activate_scene_object
extern "C" void cinematic_activate_scene_object(long cinematic_index, long scene_index, long scene_object_index)
{
    s_cinematic_scene *scene = scripted_camera_get_scene(cinematic_index, scene_index);
    if (scene && cinematics_structure_active(cinematic_index) &&
        scene_object_index >= 0 && scene_object_index < scene->objects.count) {
        s_cinematic_scene_object *entry = &((s_cinematic_scene_object *)scene->objects.address)[scene_object_index];
        if (entry != 0) {
            cinematic_object_create_from_cinematic_scene(entry->name);
            long object_index = cinematic_object_get(entry->name);
            if (object_index != -1) {
                object_set_hidden(object_index, false);
                object_set_always_active(object_index, true);
                object_get(object_index)->flags |= 0x200000;
            }
        }
    }
}

// Rotate a 16-bit flag into the low bit for extraction.
#define TEST_BIT16(flags, bit) (_rotl((flags), 32 - (bit)) & 1)

bool c_controller_interface::in_use() const
{
    return is_signed_in_to_machine() || TEST_BIT16(flags, 2);
}

static __forceinline int cinematics_subtitle_range_0(int x)
{
    if ((x < 0)) return 1;
    if ((x >= 12)) return 1;
    return 0;
}
// @address 0x2400c8 ?subtitle_should_render@@YA_NXZ
bool subtitle_should_render(void)
{
    int raw_scenario_language;
    c_controller_interface *controller_entry;
    int subtitle_setting;
    int cached_language;
    bool should_render;
    unsigned char controller_in_use;
    int next_controller_slot;
    int scenario_language;
    int controller_slot;
    should_render = false;
    cached_language = get_current_language();
    scenario_language = 0;
    if (!((global_scenario == 0))) {
        raw_scenario_language = global_game_globals->language;
        if (!cinematics_subtitle_range_0(raw_scenario_language))
            scenario_language = raw_scenario_language;
    }
    controller_slot = 0;
    do {
        if (should_render)
            break;
        controller_entry = &g_controller_globals.controllers[controller_slot];
        controller_in_use = controller_entry->is_signed_in_to_machine() || TEST_BIT16((unsigned short)controller_entry->flags, 2);
        if (!((((unsigned char)controller_in_use) == 0))) {
            subtitle_setting = global_preferences_get_subtitle_setting(&controller_entry->player_profile);
            if (!(((unsigned int)subtitle_setting < 1))) {
                if ((unsigned int)subtitle_setting == 1)
                    should_render = true;
            } else {
                should_render = ((scenario_language - cached_language) != 0);
            }
        }
        next_controller_slot = -1;
        if (!(((unsigned int)controller_slot > 2))) {
            next_controller_slot = (controller_slot + 1);
        }
        controller_slot = next_controller_slot;
    } while (next_controller_slot != -1);
    return should_render;
}


// @address 0x1d74b8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_globals_allocator>::deallocate(void *allocation)
{
    game_time_globals = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x3332f0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_new_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_new_globals_allocator>::deallocate(void *allocation)
{
    int value = m_member_index;
    g_object_broadphase = 0;
    restricted_region_free_member(3, value);
}

extern "C" {
// @address 0x7831dc g_cinematic_globals_allocator
// @address 0x708148 ??__Eg_cinematic_globals_allocator@@YAXXZ
// @address 0x70bd00 ??__Fg_cinematic_globals_allocator@@YAXXZ
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_globals_allocator> g_cinematic_globals_allocator;
// @address 0x7831e8 g_cinematic_new_globals_allocator
// @address 0x708158 ??__Eg_cinematic_new_globals_allocator@@YAXXZ
// @address 0x70bd18 ??__Fg_cinematic_new_globals_allocator@@YAXXZ
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_new_globals_allocator> g_cinematic_new_globals_allocator;
}

static __forceinline long rasterizer_presentation_interval_to_d3d(long interval)
{
    long result = D3DPRESENT_INTERVAL_DEFAULT;
    switch (interval)
    {
    case 1:
        result = D3DPRESENT_INTERVAL_ONE;
        break;
    case 2:
        result = D3DPRESENT_INTERVAL_TWO;
        break;
    case 3:
        result = D3DPRESENT_INTERVAL_THREE;
        break;
    case 4:
        result = D3DPRESENT_INTERVAL_FOUR;
        break;
    case 5:
        result = D3DPRESENT_INTERVAL_IMMEDIATE;
        break;
    default:
        result = D3DPRESENT_INTERVAL_DEFAULT;
    }
    return result;
}
static __forceinline long rasterizer_get_d3d_presentation_interval()
{
    return rasterizer_presentation_interval_to_d3d(rasterizer_globals.presentation_interval);
}

// 0x2401d8
extern "C" float cinematic_compute_letterbox_coverage(void)
{
    short width;
    short height;
    if (rasterizer_main_get_video_mode()->fIsWideScreen != 0) {
        width = (short)c_rasterizer::render_globals.width;
        height = (short)c_rasterizer::render_globals.height;
    } else {
        width = 640;
        height = 480;
    }
    struct { float ratio; float fraction; } aspect;
    aspect.ratio = (float)width / (float)height;
    aspect.fraction = 1.0f - aspect.ratio * 0.42553192f;
    if (!(aspect.fraction > 0.0f)) {
        aspect.fraction = 0.0f;
    }
    float scaled = aspect.fraction * 0.5f;
    return scaled * cinematic_globals->letterbox_fraction;
}


// @address 0x448150 cinematic_get_letterbox_coverage
extern "C" void cinematic_get_letterbox_coverage(presentation_parameters *parameters) {
 memset(parameters,0,sizeof(*parameters));
 parameters->BackBufferWidth=c_rasterizer::render_globals.width;
 parameters->BackBufferHeight=c_rasterizer::render_globals.height;
 D3DFORMAT back_buffer_format=(D3DFORMAT)0x28280136;
 D3DFORMAT front_buffer_format=(D3DFORMAT)0x28280136;
 parameters->BackBufferFormat=back_buffer_format;
 parameters->BackBufferCount=1;
 parameters->MultiSampleType=(D3DMULTISAMPLE_TYPE)0;
 parameters->SwapEffect=(D3DSWAPEFFECT)1;
 parameters->EnableAutoDepthStencil=0;
 parameters->PresentationInterval=rasterizer_get_d3d_presentation_interval();
 parameters->DisableAutoBackBuffer=1;
 parameters->DisableAutoFrontBuffer=1;
 parameters->FrontBufferFormat=front_buffer_format;
 parameters->FrontBufferColorSpace=(D3DCOLORSPACE)0;
 parameters->RingBufferParameters.PrimarySize=0x8000;
 parameters->RingBufferParameters.SecondarySize=0x380000;
 rectangle2d bounds;
 c_rasterizer::get_display_pixel_bounds(&bounds);
 if(!rasterizer_main_get_video_mode()->fIsWideScreen) parameters->Flags |=1;
 parameters->VideoScalerParameters.ScaledOutputWidth=bounds.x1;
 parameters->VideoScalerParameters.ScaledOutputHeight=bounds.y1;
 parameters->VideoScalerParameters.ScalerSourceRect.y1=0;
 parameters->VideoScalerParameters.ScalerSourceRect.x1=0;
 parameters->VideoScalerParameters.ScalerSourceRect.x2=c_rasterizer::render_globals.width;
 parameters->VideoScalerParameters.ScalerSourceRect.y2=c_rasterizer::render_globals.height;
}

// 0x191118
void cinematic_clear_letterbox_bars(bool clear_depth)
{
    rectangle2d top_bar;
    rectangle2d bottom_bar;
    {
        rectangle2d bounds;
        cinematic_compute_letterbox_bars(&bounds, &top_bar, &bottom_bar);
    }
    calculate_aspect_ratio_scaling();
    real_point2d scale;
    memcpy(&scale, &g_interface_aspect_ratio_scaling, sizeof(scale));
    interface_scale_bounds(&top_bar, &scale);
    interface_scale_bounds(&bottom_bar, &scale);
    unsigned long clear_channels = 1;
    if (clear_depth) {
        clear_channels = 3;
    }
    if (top_bar.x1 - top_bar.x0 > 0 && top_bar.y1 - top_bar.y0 > 0) {
        c_rasterizer::set_viewport(top_bar, 0.0f, 1.0f);
        c_rasterizer::set_scissor_rect(&top_bar);
        c_rasterizer::clear(clear_channels, 0, 1.0f, 0);
    }
    if (bottom_bar.x1 - bottom_bar.x0 > 0 && bottom_bar.y1 - bottom_bar.y0 > 0) {
        c_rasterizer::set_viewport(bottom_bar, 0.0f, 1.0f);
        c_rasterizer::set_scissor_rect(&bottom_bar);
        c_rasterizer::clear(clear_channels, 0, 1.0f, 0);
    }
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
}

// The SDK's out-of-line XMMatrixLookToLH (xboxmath's xmmatrix.inl, VMX128 implementation).

static __forceinline XMVECTOR cinematics_vector_set(float x, float y, float z, float w)
{
    XMVECTOR XYZW, ZW, Y, W;
    XYZW = __lvlx(&x, 0);
    Y = __lvlx(&y, 0);
    ZW = __lvlx(&z, 0);
    W = __lvlx(&w, 0);
    XYZW = __vrlimi(XYZW, Y, 4, 3);
    ZW = __vrlimi(ZW, W, 4, 3);
    XYZW = __vrlimi(XYZW, ZW, 3, 2);
    return XYZW;
}

static __forceinline XMMATRIX cinematics_matrix_identity()
{
    XMVECTOR ZO = __vspltisw(0);
    XMMATRIX M;
    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);
    M.r[0] = __vpermwi(ZO, 0xEA);
    M.r[1] = __vpermwi(ZO, 0xBA);
    M.r[2] = __vpermwi(ZO, 0xAE);
    M.r[3] = __vpermwi(ZO, 0xAB);
    return M;
}

static __forceinline XMMATRIX cinematics_matrix_multiply(XMMATRIX M1, XMMATRIX M2)
{
    XMVECTOR M00, M01, M02, M03, M10, M11, M12, M13;
    XMVECTOR M20, M21, M22, M23, M30, M31, M32, M33;
    XMVECTOR M00M02, M01M03, M10M12, M11M13;
    XMVECTOR M20M22, M21M23, M30M32, M31M33;
    XMMATRIX M2T, P, Result;
    P.r[0] = __vmrghw(M2.r[0], M2.r[2]);
    P.r[1] = __vmrghw(M2.r[1], M2.r[3]);
    P.r[2] = __vmrglw(M2.r[0], M2.r[2]);
    P.r[3] = __vmrglw(M2.r[1], M2.r[3]);
    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);
    M00 = __vmsum4fp(M1.r[0], M2T.r[0]);
    M02 = __vmsum4fp(M1.r[0], M2T.r[2]);
    M01 = __vmsum4fp(M1.r[0], M2T.r[1]);
    M03 = __vmsum4fp(M1.r[0], M2T.r[3]);
    M10 = __vmsum4fp(M1.r[1], M2T.r[0]);
    M12 = __vmsum4fp(M1.r[1], M2T.r[2]);
    M11 = __vmsum4fp(M1.r[1], M2T.r[1]);
    M13 = __vmsum4fp(M1.r[1], M2T.r[3]);
    M20 = __vmsum4fp(M1.r[2], M2T.r[0]);
    M22 = __vmsum4fp(M1.r[2], M2T.r[2]);
    M21 = __vmsum4fp(M1.r[2], M2T.r[1]);
    M23 = __vmsum4fp(M1.r[2], M2T.r[3]);
    M30 = __vmsum4fp(M1.r[3], M2T.r[0]);
    M32 = __vmsum4fp(M1.r[3], M2T.r[2]);
    M31 = __vmsum4fp(M1.r[3], M2T.r[1]);
    M33 = __vmsum4fp(M1.r[3], M2T.r[3]);
    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);
    Result.r[0] = __vmrghw(M00M02, M01M03);
    Result.r[1] = __vmrghw(M10M12, M11M13);
    Result.r[2] = __vmrghw(M20M22, M21M23);
    Result.r[3] = __vmrghw(M30M32, M31M33);
    return Result;
}

static __forceinline XMMATRIX cinematics_matrix_transpose(XMMATRIX M)
{
    XMMATRIX P, MT;
    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);
    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);
    return MT;
}
// @address 0x2588c k_widescreen_aspect_ratio
// @address 0x5884c k_cinematics_perspective_m22
// @address 0x58848 k_cinematics_perspective_m32
#if !defined(_XBOX) || defined(__clang__)
const float k_widescreen_aspect_ratio = 1.77777779f;
const float k_cinematics_perspective_m22 = 1.00010002f;
const float k_cinematics_perspective_m32 = -0.010001f;
#endif

static __forceinline XMMATRIX cinematics_matrix_perspective(float angle, float aspect)
{
    float SinFov, CosFov, Height, Width;
    XMVECTOR ZO = __vspltisw(0), V;
    XMMATRIX M;
    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);
    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[3] = __vspltisw(0);
    M.r[2] = __vpermwi(ZO, 0xAB);
    XMScalarSinCos(&SinFov, &CosFov, 0.5f * angle);
    Height = CosFov / SinFov;
    Width = Height / aspect;
    V.v[0] = Width;
    V.v[1] = Height;
    V.v[2] = k_cinematics_perspective_m22;
    V.v[3] = k_cinematics_perspective_m32;
    M.r[0] = __vrlimi(M.r[0], V, 8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 4, 0);
    M.r[2] = __vrlimi(M.r[2], V, 2, 0);
    M.r[3] = __vrlimi(M.r[3], V, 2, 1);
    return M;
}


static __forceinline XMMATRIX cinematics_matrix_look_at(XMVECTOR EyePosition, XMVECTOR FocusPosition, XMVECTOR UpDirection)
{
    XMVECTOR EyeDirection;
    XMMATRIX M;
    EyeDirection = __vsubfp(FocusPosition, EyePosition);
    M = XMMatrixLookToLH(EyePosition, EyeDirection, UpDirection);
    return M;
}

// @address 0x451878 chud_draw_loading_setup_view
extern "C" void chud_draw_loading_setup_view(void *, const real_vector3d *position, const real_vector3d *focus)
{
    XMVECTOR eye = cinematics_vector_set(position->i, position->j, position->k, 0.0f);
    XMVECTOR at = cinematics_vector_set(focus->i, focus->j, focus->k, 0.0f);
    XMVECTOR up = cinematics_vector_set(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX world = cinematics_matrix_identity();
    XMMATRIX view = cinematics_matrix_look_at(eye, at, up);
#if !defined(_XBOX) || defined(__clang__)
    // PC (port/GAME-CHANGES.md, "Custom resolutions"): the loading screen's projection has the display's aspect ratio
    // (rasterizer_main_get_video_aspect_ratio: 16:9 or 4:3 on the 360, as inlined here; a custom resolution's own), so
    // the ring is not stretched on 21:9. chud_loading_compute_projection_corners matches it.
    float aspect = rasterizer_main_get_video_aspect_ratio();
#else
    float aspect = rasterizer_main_get_video_mode()->fIsWideScreen ? k_widescreen_aspect_ratio : k_observer_default_aspect_ratio;
#endif
    XMMATRIX projection = cinematics_matrix_perspective(1.0471976f, aspect);
    XMMATRIX world_view = cinematics_matrix_multiply(world, view);
    XMMATRIX world_view_projection = cinematics_matrix_multiply(world_view, projection);
    XMMATRIX constants = cinematics_matrix_transpose(world_view_projection);
    g_rasterizer_device->SetVertexShaderConstantF(4, &constants.m[0][0], 4);
    g_rasterizer_device->SetVertexShaderConstantF(10, eye.v, 1);
}

// 0x454b58
XMMATRIX XMMatrixLookToLH(XMVECTOR EyePosition, XMVECTOR EyeDirection, XMVECTOR UpDirection)
{

    XMVECTOR NegEyePosition;
    XMVECTOR R0, R1, R2;
    XMVECTOR ZO;
    XMMATRIX P;
    XMVECTOR D;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR H, HS;
    XMVECTOR CrossV1, CrossV2;
    XMVECTOR CrossR2A, CrossR2B;
    XMVECTOR SignMask;
    XMMATRIX M;

    ZO = __vspltisw(0);

    H = __vspltisw(1);
    D = __vmsum3fp(EyeDirection, EyeDirection);
    H = __vcfsx(H, 1);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    Scale = __vmulfp(RcpLength, RcpLength);
    HS = H;
    HS = __vnmsubfp(OneHalfLengthSq, Scale, HS);
    HS = __vmaddfp(RcpLength, HS, RcpLength);

    R2 = __vmulfp(EyeDirection, HS);

    CrossV1 = __vpermwi(UpDirection, 0x63);
    CrossR2A = __vpermwi(R2, 0x87);
    R0 = __vmulfp(CrossV1, CrossR2A);
    CrossV1 = __vpermwi(UpDirection, 0x87);
    CrossR2B = __vpermwi(R2, 0x63);
    R0 = __vnmsubfp(CrossV1, CrossR2B, R0);

    D = __vmsum3fp(R0, R0);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddfp(RcpLength, H, RcpLength);
    SignMask = __vspltisw(-1);
    R0 = __vmulfp(R0, H);

    SignMask = __vslw(SignMask, SignMask);
    NegEyePosition = __vxor(EyePosition, SignMask);

    CrossV2 = __vpermwi(R0, 0x87);
    R1 = __vmulfp(CrossR2B, CrossV2);
    CrossV2 = __vpermwi(R0, 0x63);
    R1 = __vnmsubfp(CrossR2A, CrossV2, R1);

    M.r[0] = __vmsum3fp(R0, NegEyePosition);
    M.r[1] = __vmsum3fp(R1, NegEyePosition);
    M.r[2] = __vmsum3fp(R2, NegEyePosition);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vrlimi(M.r[0], R0, 0xE, 0);
    M.r[1] = __vrlimi(M.r[1], R1, 0xE, 0);
    M.r[2] = __vrlimi(M.r[2], R2, 0xE, 0);
    M.r[3] = __vpermwi(ZO, 0xAB);

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    M.r[0] = __vmrghw(P.r[0], P.r[1]);
    M.r[1] = __vmrglw(P.r[0], P.r[1]);
    M.r[2] = __vmrghw(P.r[2], P.r[3]);
    M.r[3] = __vmrglw(P.r[2], P.r[3]);

    return M;

}

static __forceinline long cinematics_first_active_user()
{
    long result = -1;
    for (long user_index = 0; user_index < 4; ++user_index) {
        if (player_mapping_globals->player_indices[user_index] != -1) {
            result = user_index;
            break;
        }
    }
    return result;
}

// @address 0x487830 cinematic_reset_scene_and_show_chud
extern "C" void cinematic_reset_scene_and_show_chud(void)
{
    // match: 0x487830: pointer __restrict qualifier prevents alias reload
    s_cinematic_new_globals_definition * __restrict globals = cinematic_new_globals;
    globals->scene_tag_index = -1;
    globals->current_shot = 0;
    for (long user_index = cinematics_first_active_user(); user_index != -1; user_index = players_next_active_user(user_index)) {
        c_chud_manager::x_persistent_user_data[user_index].m_flags[2] = true;
    }
}

// @address 0x23f8c0 main_loading_get_action

extern "C" bool main_loading_get_action(void)
{
    s_cinematic_globals_definition *cinematic = cinematic_globals;
    bool result = cinematic->skip_in_progress;
    if (result && cinematic->old_map_active) {
        bool other_users = false;
        bool simulation_active = false;
        if ((unsigned long)((signed char)game_globals->options.game_simulation - 2) <= 3) {
            simulation_active = true;
        }
        if (simulation_active) {
            s_players_globals *input = players_globals;
            if (input->local_machine_index != -1 && (input->machine_valid_mask & ~(1u << input->local_machine_index)) != 0) {
                other_users = true;
            }
        }
        if (other_users) {
            result = !((global_scenario->flags >> 10) & 1);
        } else {
            char path[256];
            bool loading = false;
            get_next_scenario_path(path, 256);
            if (path[0] != 0) {
                long status = loading_get_map_load_state(-1, path);
                if (status <= 1) {
                    loading = true;
                } else {
                    loading = status == 4;
                }
            }
            result = !loading;
        }
    }
    return result;
}
// @address 0x4549e8 ?XMScalarSinCos@@YAXPAM0M@Z

// @address 0xe40 k_xmscalar_sincos_sin2_coeffs
// @address 0xe80 k_xmscalar_sincos_cos2_coeffs
// @address 0xed0 k_xmscalar_sincos_cos1_coeffs
// @address 0xf10 k_xmscalar_sincos_sin0_coeffs
// @address 0xf20 k_xmscalar_sincos_cos0_coeffs
// @address 0xf30 k_xmscalar_sincos_sin1_coeffs
#if !defined(_XBOX) || defined(__clang__)
const XMVECTOR k_xmscalar_sincos_sin2_coeffs = { 2.81145736e-15f, -8.22063508e-18f, 1.95729415e-20f, -3.8681703e-23f };
const XMVECTOR k_xmscalar_sincos_cos2_coeffs = { 4.77947726e-14f, -1.56192068e-16f, 4.11031759e-19f, -8.89679096e-22f };
const XMVECTOR k_xmscalar_sincos_cos1_coeffs = { 2.48015876e-05f, -2.755732e-07f, 2.08767581e-09f, -1.14707454e-11f };
const XMVECTOR k_xmscalar_sincos_sin0_coeffs = { 1.0f, -0.166666672f, 0.00833333377f, -0.000198412701f };
const XMVECTOR k_xmscalar_sincos_cos0_coeffs = { 1.0f, -0.5f, 0.0416666679f, -0.00138888892f };
const XMVECTOR k_xmscalar_sincos_sin1_coeffs = { 2.75573188e-06f, -2.50521079e-08f, 1.60590444e-10f, -7.64716361e-13f };
#endif
static const float k_sincos_one = 1.0f;
void XMScalarSinCos(float *pSin, float *pCos, float Value)
{
    union { float f; unsigned long u; } half_turn;
    half_turn.f = Value;
    half_turn.u = (half_turn.u & 0x80000000) | 0x40490fdb;

    float quotient = (float)(long)((half_turn.f + Value) * 0.159154943f); // XM_1DIV2PI (XDK xboxmath.h)
    float y = Value - 6.2831855f * quotient;
    __declspec(align(16)) float powers[4];
    powers[0] = k_sincos_one;
    powers[1] = y;
    powers[2] = y * y;
    powers[3] = y * y * y;
    XMVECTOR V0123 = *(XMVECTOR *)powers;
    XMVECTOR V0246 = __vmulfp(V0123, V0123);
    XMVECTOR V1357 = __vmulfp(V0246, __vspltw(V0123, 1));
    XMVECTOR V8 = __vmulfp(__vspltw(V0246, 3), __vspltw(V0123, 2));
    XMVECTOR V8101214 = __vmulfp(V0246, V8);
    XMVECTOR V9111315 = __vmulfp(V1357, V8);
    XMVECTOR V16182022 = __vmulfp(V8101214, V8);
    XMVECTOR V17192123 = __vmulfp(V9111315, V8);
    XMVECTOR c1 = __vmsum4fp(V8101214, k_xmscalar_sincos_cos1_coeffs);
    XMVECTOR c2 = __vmsum4fp(V16182022, k_xmscalar_sincos_cos2_coeffs);
    XMVECTOR s1 = __vmsum4fp(V9111315, k_xmscalar_sincos_sin1_coeffs);
    XMVECTOR s2 = __vmsum4fp(V17192123, k_xmscalar_sincos_sin2_coeffs);
    XMVECTOR c0 = __vmsum4fp(V0246, k_xmscalar_sincos_cos0_coeffs);
    XMVECTOR s0 = __vmsum4fp(V1357, k_xmscalar_sincos_sin0_coeffs);
    float cos0 = c0.x;
    float cos1 = c1.x;
    float cos2 = c2.x;
    float sin0 = s0.x;
    float sin1 = s1.x;
    float sin2 = s2.x;
    *pCos = (cos2 + cos1) + cos0;
    *pSin = (sin2 + sin1) + sin0;
}


// @address 0x241058 cinematic_setup_scene_groups
extern "C" void cinematic_setup_scene_groups(long scene_index)
{
    cinematic_reset_groups();
    s_cinematic_new_globals_definition *new_globals = cinematic_new_globals;
    if (new_globals->scene_tag_index == -1) {
        return;
    }
    s_cinematic_scene *scene_list = (s_cinematic_scene *)tag_get('cisc', new_globals->scene_tag_index);
    if (scene_list == 0) {
        return;
    }
    if (scene_index < 0 || scene_index >= scene_list->shots.count) {
        return;
    }
    hs_cinematic_shot *scene =
        &((hs_cinematic_shot *)((s_cinematic_scene *)tag_get('cisc', new_globals->scene_tag_index))->shots.address)[scene_index];
    long time = game_time_globals->time;
    for (long group_index = 0; group_index < scene->group_count; ++group_index) {
        s_cinematics_group_source *source = &scene->groups[group_index];
        s_cinematics_new_group *group = &new_globals->groups[group_index];
        group->start = source->start;
        group->end = source->end;
        group->frame0 = source->frame0 + time;
        group->frame1 = source->frame1 + time;
        group->count = source->count;
        for (long slot = 0; slot < source->count; ++slot) {
            long object_index = source->object_indices[slot];
            if (object_index != -1) {
                long mapped = cinematic_object_get_from_scene(object_index);
                if (mapped != -1) {
                    group->object_indices[slot] = mapped;
                    object_get(mapped)->flags |= 0x8000;
                }
            }
        }
    }
    new_globals->group_count = scene->group_count;
}


// @address 0x119858 cinematic_update
extern "C" void cinematic_update(void)
{
    bool subtitle_done = false;
    if (cinematic_globals->subtitle.string_id != 0) {
        cinematic_globals->subtitle.time_remaining -= game_time_globals->tick_length;
        if (!(cinematic_globals->subtitle.time_remaining > 0.0f)) {
            subtitle_done = true;
        }
    }
    if (subtitle_done) {
        cinematic_globals->subtitle.string_id = 0;
        cinematic_globals->subtitle.duration_bits = 0;
        cinematic_globals->subtitle.time_remaining_bits = 0;
    }
    float gain = 1.0f;
    long start_frame;
    if (cinematic_globals) {
        // match: 0x119858: pointer __restrict qualifier prevents alias reload
        struct scenario *__restrict scenario = global_scenario;
        if (cinematic_globals->letterbox_closing || cinematic_globals->letterbox_fraction > 0.0f) {
            float fraction;
            if (cinematic_globals->letterbox_closing) {
                fraction = cinematic_globals->letterbox_fraction + game_time_globals->tick_length;
                cinematic_globals->letterbox_fraction = fraction;
                if (fraction > 1.0f) {
                    fraction = 1.0f;
                }
            } else {
                fraction = cinematic_globals->letterbox_fraction - game_time_globals->tick_length;
                cinematic_globals->letterbox_fraction = fraction;
                if (!(fraction > 0.0f)) {
                    fraction = 0.0f;
                }
            }
            cinematic_globals->letterbox_fraction = fraction;
        }
        for (long index = 0; index < 4; ++index) {
            s_cinematic_title *title = &cinematic_globals->titles[index];
            if (title->index >= 0 && title->index < scenario->cutscene_titles.count) {
                title->elapsed_ticks = (short)(title->elapsed_ticks + 1);
                s_scenario_cutscene_title *definition = &((s_scenario_cutscene_title *)scenario->cutscene_titles.address)[title->index];
                if (!(game_time_globals->tick_length * (float)(__int64)title->elapsed_ticks < definition->duration + definition->start_time)) {
                    title->index = -1;
                    title->elapsed_ticks = -1;
                }
            }
        }
    }
    bool in_progress = false;
    if (cinematic_globals) {
        in_progress = cinematic_globals->in_progress;
    }
    if (in_progress && cinematic_new_globals->current_scene != -1) {
        long scene_tag_index = cinematic_new_globals->scene_tag_index;
        if (scene_tag_index != -1) {
            long shot_index = cinematic_new_globals->current_shot;
            s_cinematic_scene *definition = (s_cinematic_scene *)tag_get('cisc', scene_tag_index);
            hs_cinematic_shot *shot = &((hs_cinematic_shot *)definition->shots.address)[shot_index];
            long frame;
            if (g_camera_script_globals->mode == 5) {
                frame = g_camera_script_globals->camera_track.cinematic_shot_index;
            } else {
                frame = start_frame;
            }
            long remaining = frame - shot->frame_count + 15;
            cinematic_predict_sounds_in_range(definition, shot_index, frame, frame + 15);
            if (remaining > 0) {
                cinematic_iterate_shots(remaining, cinematic_predict_shot_sounds, 0);
            }
        }
    }
    if (cinematic_globals->skip) {
        if (game_time_globals->time > cinematic_globals->skip_end_time) {
            volatile unsigned char &secondary_flag = main_globals.secondary_flag;
            secondary_flag = 0;
            main_globals.reset_requested = 0;
            main_globals.map_change_requested = 1;
        }
        gain -= cinematic_get_skip_fade();
    }
    s_game_globals *game_globals = global_game_globals;
    for (short class_index = 0; class_index < 54; ++class_index) {
        if (sound_class_is_valid(class_index)) {
            sound_globals_reference *options = game_globals->sound_globals.count() != 0 ? game_globals->sound_globals.begin() : 0;
            sound_classes *classes =
                (sound_classes *)tag_get('sncl', options->sound_classes.index);
            if (!(classes->classes[class_index].flags & 1)) {
                sound_class_set_gain_by_index(class_index, real_gain_to_decibels(gain), 0.0f, 4);
                game_globals = global_game_globals;
            }
        }
    }
}

extern "C"
{


}


static __forceinline long cinematic_render_seconds_to_ticks(float seconds)
{
    float ticks = seconds * (float)game_time_globals->ticks_per_second;
    return (long)(ticks + 0.5f);
}

static __forceinline long cinematic_render_layout_value(long user_index)
{
    chud_table_entry *entry = chud_get_layout_entry(user_index, chud_get_layout_index(user_index));
    return entry->message_row_count;
}

// @address 0x1349a8 chud_messaging_manager_update_messages
extern "C" void chud_messaging_manager_update_messages(c_chud_messaging_manager *data, long user_index)
{
    long found_index = -1;
    for (long index = 0; index < 5; index++)
    {
        s_chud_message *cursor = &data->m_messages[index];
        if (cursor->active != 0 && cursor->visible == 0)
        {
            c_chud_draw_string draw;
            c_font_cache cache;
            if (draw.precache_wchar_string_for_font(&cache, cursor->text, _full_screen_hud_message_font))
                found_index = index;
        }
    }
    if (found_index != -1)
    {
        for (long index = 0; index < 5; index++)
        {
            s_chud_message *entry = &data->m_messages[index];
            if (entry->active != 0 && entry->visible != 0)
                entry->target_row += 1.0f;
        }
        s_chud_message *selected = &data->m_messages[found_index];
        selected->visible = 1;
        selected->start_time = game_time_globals->time;
    }
    float tick_rate = 1.0f / ((float)game_time_globals->ticks_per_second * 0.25f);
    float max_value = (float)(cinematic_render_layout_value(user_index) + 3) + 1.0f;
    float whole = (float)(cinematic_render_layout_value(user_index) + 2);
    for (long index = 0; index < 5; index++)
    {
        s_chud_message *entry = &data->m_messages[index];
        if (entry->active != 0 && entry->visible != 0)
        {
            float current = entry->current_row;
            float limit = entry->target_row;
            if (current < limit)
            {
                float value = current + tick_rate;
                if (value > limit)
                    value = limit;
                entry->current_row = value;
                if (value > max_value)
                    value = max_value;
                entry->current_row = value;
            }
            float position = entry->current_row;
            float whole_value = (float)floor((double)position);
            float fraction = entry->current_row - whole_value;
            entry->offset = (float)pow((double)fraction, 0.5) + whole_value;
            if (whole_value == whole)
                entry->alpha = 1.0f - fraction;
            else if (whole_value > whole)
                entry->alpha = 0.0f;
            else
                entry->alpha = 1.0f;
            long elapsed = game_time_globals->time - entry->start_time;
            float fade = (float)elapsed / (float)cinematic_render_seconds_to_ticks(0.15f);
            fade = MIN(fade, 1.0f);
            entry->height = fade * 0.8f + 0.2f;
            float eased = (float)pow((double)fade, 0.5) * 0.8f + 0.2f;
            float scaled = eased * entry->alpha;
            entry->alpha = scaled;
            float ramp = (float)(elapsed - cinematic_render_seconds_to_ticks(3.5f)) / ((float)game_time_globals->ticks_per_second * 1.5f);
            ramp = PIN(ramp, 0.0f, 1.0f);
            float dimmed = (1.0f - ramp) * scaled;
            entry->alpha = dimmed;
            float tail = (float)(game_time_globals->time - entry->start_time) / (float)cinematic_render_seconds_to_ticks(0.5f);
            tail = MIN(tail, 1.0f);
            entry->scale = 1.0f - tail;
            if (dimmed < 0.0001f)
                entry->active = 0;
        }
    }
}



static __forceinline unit_datum *cinematics_weapon_unit_get(long object_index)
{
    return (unit_datum *)object_get(object_index);
}
static __forceinline bool cinematics_weapon_unit_is_dead(const unit_datum *unit)
{
    return ((unit->object.damage_flags >> 2) & 1) != 0;
}

// @address 0x23faa8 ?cinematic_create_and_attach_players_weapon@@YAXJJJ@Z
void cinematic_create_and_attach_players_weapon(long object_index, long parent_marker, long child_marker)
{
    if (object_index == -1)
        return;
    c_static_stack<long, 4> weapons;
    long first_unit_index = -1;
    {
        c_player_with_unit_iterator players;
        players.begin();
        while (players.next()) {
            long unit_index = players.datum->unit_index;
            if (unit_index != -1 && !cinematics_weapon_unit_is_dead(cinematics_weapon_unit_get(unit_index))) {
                first_unit_index = unit_index;
                break;
            }
        }
    }
    if (first_unit_index != -1) {
        long unit_index = first_unit_index;
        long inventory[3];
        unit_datum *unit = cinematics_weapon_unit_get(unit_index);
        inventory[0] = unit_inventory_get_weapon_or_none(unit_index, unit->unit.current_weapon_indices[0]);
        inventory[1] = unit_inventory_get_weapon_or_none(unit_index, unit->unit.current_weapon_indices[1]);
        inventory[2] = unit_get_current_backpack_weapon(unit_index);
        const long *inventory_weapon = inventory;
        unsigned long remaining = 3;
        do {
            if (*inventory_weapon != -1)
                weapons.push_back(cinematics_weapon_unit_get(*inventory_weapon)->object.definition_index);
            ++inventory_weapon;
        } while (--remaining != 0);
    }
    if (weapons.count() == 0) {
        const scenario *scenario_definition = global_scenario;
        if (scenario_definition->starting_profiles.count > 0) {
            const s_scenario_starting_profile *profile = (const s_scenario_starting_profile *)scenario_definition->starting_profiles.address;
            if (profile->weapons[0].weapon.index != -1)
                weapons.push_back(profile->weapons[0].weapon.index);
            if (profile->weapons[1].weapon.index != -1)
                weapons.push_back(profile->weapons[1].weapon.index);
        }
        if (weapons.count() == 0) {
            const object_header_datum *header = object_header_get(object_index);
            if (header->object_type == 0) {
                const unit_definition *definition =
                    (const unit_definition *)
                        tag_get('unit', ((unit_datum *)header->datum)->object.definition_index);
                for (long i = 0; i < definition->initial_weapons.count; ++i) {
                    long weapon = ((const s_tag_reference *)definition->initial_weapons.address)[i].index;
                    if (weapon != -1)
                        weapons.push_back(weapon);
                }
            }
        }
    }
    long selected_weapon = -1;
    long best_priority = 0x7fffffff;
    long weapon = -1;
    for (long i = 0; i < weapons.count(); i++) {
        long current_weapon = weapons[i];
        const weapon_definition *definition = (const weapon_definition *)tag_get('weap', current_weapon);
        long priority;
        switch (definition->animation_weapon_class) {
        case _string_id_rifle: priority = 0; break;
        case _string_id_melee:
        case _string_id_sword: priority = 2; break;
        case _string_id_missile:
        case _string_id_support: priority = 3; break;
        case _string_id_pistol: priority = 1; break;
        default: priority = 4; break;
        }
        if (priority == 0) {
            weapon = current_weapon;
            break;
        }
        if (selected_weapon == -1 || priority < best_priority) {
            best_priority = priority;
            selected_weapon = current_weapon;
        }
    }
    if (weapon == -1)
        weapon = selected_weapon;
    if (weapon != -1) {
        object_placement_data placement;
        object_placement_data_new(&placement, weapon, object_index, 0);
        long new_weapon = object_new(&placement);
        if (new_weapon != -1) {
            object_attach_to_marker_with_type(object_index, parent_marker, new_weapon, child_marker);
            object_set_scale(new_weapon, 1.0f, 0.0f, true);
        }
    }
}

// @address 0x2402a0 cinematic_compute_letterbox_bars
extern "C" void cinematic_compute_letterbox_bars(rectangle2d *bounds, rectangle2d *top_bar, rectangle2d *bottom_bar)
{
    rectangle2d fullscreen_bounds;
    if (rasterizer_main_get_video_mode()->fIsWideScreen != 0)
    {
        fullscreen_bounds.y0 = 0;
        fullscreen_bounds.x0 = 0;
        fullscreen_bounds.x1 = (short)c_rasterizer::render_globals.width;
        fullscreen_bounds.y1 = (short)c_rasterizer::render_globals.height;
    }
    else
    {
        fullscreen_bounds.y0 = 0;
        fullscreen_bounds.x0 = 0;
        fullscreen_bounds.x1 = 640;
        fullscreen_bounds.y1 = 480;
    }
    float coverage = cinematic_compute_letterbox_coverage();
    float height_real = (float)(fullscreen_bounds.y1 - fullscreen_bounds.y0);
    bounds->x0 = 0;
    top_bar->x0 = 0;
    top_bar->y0 = 0;
    bottom_bar->x0 = 0;
    short right = (short)(long)((float)fullscreen_bounds.x1 + 0.5f);
    bounds->x1 = right;
    top_bar->x1 = right;
    bottom_bar->x1 = right;
    float bar_height = (float)fullscreen_bounds.y0 + height_real * coverage;
    float bar_height_2 = (float)fullscreen_bounds.x0 + height_real * coverage;
    if (fullscreen_bounds.y0 != 0)
    {
        bounds->x0 = (short)(long)(bar_height + bar_height_2);
    }
    float top_bound = (float)fullscreen_bounds.y0 + bar_height;
    float top_bar_bound = (float)fullscreen_bounds.x0 + bar_height_2;
    bounds->y0 = (short)(long)(((float)fullscreen_bounds.y0 + 1.0f) * top_bound + 0.5f);
    bounds->y1 = (short)(long)((float)fullscreen_bounds.y0 + (float)fullscreen_bounds.y1 - bar_height + 0.5f);
    top_bar->y1 = (short)(long)(((float)fullscreen_bounds.x0 + 1.0f) * top_bar_bound + 0.5f);
    bottom_bar->y0 = (short)(long)((float)fullscreen_bounds.y1 - bar_height + 0.5f);
    bottom_bar->y1 = (short)(long)((float)fullscreen_bounds.y1 + 0.5f);
}




static const float k_cinematics_subtitle_fade_time = 1.5f;

static __forceinline long cinematics_subtitle_bound(short value)
{
    // match: 0x2403e8: zero-bit rotate keeps rotlwi+extsh lowering for the bound conversion
    return (short)_rotl(value, 0);
}

static inline long cinematics_subtitle_top(unsigned short value)
{
    // match: 0x2403e8: zero-bit rotate keeps rotlwi+extsh lowering for the top conversion
    return (short)_rotl(value, 0);
}

static __forceinline float cinematics_subtitle_alpha(const s_cinematic_subtitle *subtitle)
{
    float alpha = 1.0f;
    if (subtitle->time_remaining <= k_cinematics_subtitle_fade_time)
    {
        alpha = powf(subtitle->time_remaining / 1.5f, 1.9f);
    }
    return alpha;
}

// @address 0x2403e8
void subtitle_render()
{
    if (subtitle_should_render())
    {
        const scenario *scenario = global_scenario;
        if (scenario && scenario->cinematic_subtitles.index != -1)
        {
            c_rasterizer_draw_string draw_storage;
            c_draw_string &draw = draw_storage;
            c_font_cache cache;
            draw.m_font_id = _subtitle_font;
            draw.m_font_header = font_get_header(_subtitle_font);
            draw.m_justification = _text_justification_center;
            draw.m_flags.set(c_draw_string::_text_flag_wrap_horizontally, true);
            const s_cinematic_subtitle *subtitle = &cinematic_globals->subtitle;
            rectangle2d title_safe_bounds;
            if (rasterizer_main_get_video_mode()->fIsWideScreen)
                c_rasterizer::get_fullscreen_render_title_safe_pixel_bounds(&title_safe_bounds);
            else if (rasterizer_main_get_video_mode()->fIsHiDef)
            {
                title_safe_bounds.x0 = 32;
                title_safe_bounds.y0 = 24;
                title_safe_bounds.x1 = 608;
                title_safe_bounds.y1 = 456;
            }
            else
            {
                title_safe_bounds.x0 = 48;
                title_safe_bounds.y0 = 36;
                title_safe_bounds.x1 = 592;
                title_safe_bounds.y1 = 444;
            }
            rectangle2d bounds, top_bar, subtitle_bounds;
            cinematic_compute_letterbox_bars(&bounds, &top_bar, &subtitle_bounds);
            subtitle_bounds.y0 += 4;
            if (subtitle_bounds.x0 <= cinematics_subtitle_bound(title_safe_bounds.x0))
                subtitle_bounds.x0 = title_safe_bounds.x0;
            if (subtitle_bounds.x1 > cinematics_subtitle_bound(title_safe_bounds.x1))
                subtitle_bounds.x1 = title_safe_bounds.x1;
            long minimum_y = title_safe_bounds.y0;
            if (cinematics_subtitle_top(subtitle_bounds.y0) <= minimum_y)
                subtitle_bounds.y0 = title_safe_bounds.y0;
            if (subtitle_bounds.y1 > cinematics_subtitle_bound(title_safe_bounds.y1))
                subtitle_bounds.y1 = title_safe_bounds.y1;
            if (subtitle->string_id)
            {
                c_static_wchar_string<256> text;
                c_static_wchar_string<1024> full_text;
                text_group_get_string(scenario->cinematic_subtitles.index, subtitle->string_id, &full_text);
                long line_count = 0;
                long length = full_text.length();
                if (length - 1 > 0)
                {
                    long remaining = length - 1;
                    long index = 0;
                    do
                    {
                        if (full_text.get_string()[index] == L'|' && full_text.get_string()[index + 1] == L'n')
                            ++line_count;
                        ++index;
                    } while ((unsigned long)--remaining != 0);
                }
                float line;
                if (subtitle->duration < 1.5001f)
                    line = (float)line_count;
                else
                    line =
                        (subtitle->duration - subtitle->time_remaining) * (float)(line_count + 1) /
                        (subtitle->duration - k_cinematics_subtitle_fade_time);
                long current_line = 0;
                long string_index = 0;
                long character_index = 0;
                wchar_t line_text[256];
                long desired_line = (long)line;
                while (current_line <= desired_line)
                {
                    line_text[character_index] = full_text.get_string()[string_index];
                    ++string_index;
                    ++character_index;
                    if (string_index >= length || character_index + 1 >= NUMBEROF(line_text))
                    {
                        line_text[character_index] = 0;
                        current_line = desired_line + 1;
                    }
                    else if (string_index + 1 < length && full_text.get_string()[string_index] == L'|' && full_text.get_string()[string_index + 1] == L'n')
                    {
                        line_text[character_index] = 0;
                        ++current_line;
                        character_index = 0;
                        string_index += 2;
                    }
                }
                text.set(line_text);
                bool precached = draw.precache_wchar_string_for_font(&cache, text.get_string(), draw.m_font_id);
                if (precached)
                {
                    draw.set_bounds(&subtitle_bounds);
                    rectangle2d text_bounds, cursor_bounds;
                    draw.compute_wchar_string_bounds(&cache, text.get_string(), &text_bounds, &cursor_bounds);
                }
                if (precached)
                {
                    real_argb_color color;
                    color.red = 0.7607843f;
                    color.green = 0.92941177f;
                    color.blue = 1.0f;
                    color.alpha = cinematics_subtitle_alpha(subtitle);
                    real_argb_color shadow_color;
                    shadow_color.red = 0.0f;
                    shadow_color.green = 0.0f;
                    shadow_color.blue = 0.0f;
                    shadow_color.alpha = cinematics_subtitle_alpha(subtitle);
                    draw.set_color_deprecated(&color, false);
                    draw.set_shadow_color(&shadow_color);
                    draw.set_bounds(&subtitle_bounds);
                    draw.c_draw_string::draw(&cache, text.get_string());
                }
            }
        }
    }
}


static __forceinline const scenario *cinematics_title_scenario_get() { return global_scenario; }


static __forceinline void cinematic_fullscreen_pixel_bounds(rectangle2d *bounds)
{
    if (rasterizer_main_get_video_mode()->fIsWideScreen)
    {
        bounds->y0 = 0;
        bounds->x0 = 0;
        bounds->x1 = (short)c_rasterizer::render_globals.width;
        bounds->y1 = (short)c_rasterizer::render_globals.height;
    }
    else
    {
        bounds->y0 = 0;
        bounds->x0 = 0;
        bounds->x1 = 640;
        bounds->y1 = 480;
    }
}

static const float k_cinematic_title_color_epsilon = 0.0001f;

static __forceinline void cinematics_pixel32_to_real_rgb_color(unsigned long pixel, real_rgb_color *color)
{
    const float scale = 1.0f / 255.0f;
    color->red = (float)((pixel >> 16) & 255) * scale;
    color->green = (float)((pixel >> 8) & 255) * scale;
    color->blue = (float)(pixel & 255) * scale;
}

static __forceinline rectangle2d *cinematics_inset_rectangle(rectangle2d *bounds, short x, short y)
{
    bounds->x0 += x;
    bounds->x1 -= x;
    bounds->y0 += y;
    bounds->y1 -= y;
    return bounds;
}

static __forceinline void cinematics_pin_title_bounds(rectangle2d *bounds, const rectangle2d *safe_bounds)
{
    bounds->x0 = PIN(bounds->x0, safe_bounds->x0, safe_bounds->x1);
    bounds->x1 = PIN(bounds->x1, safe_bounds->x0, safe_bounds->x1);
    bounds->y0 = PIN(bounds->y0, safe_bounds->y0, safe_bounds->y1);
    bounds->y1 = PIN(bounds->y1, safe_bounds->y0, safe_bounds->y1);
}

static __forceinline void cinematics_fix_degenerate_title_bounds(rectangle2d *bounds, const rectangle2d *safe_bounds)
{
    if (bounds->x1 == bounds->x0 || bounds->y1 == bounds->y0)
    {
        rectangle2d screen_bounds;
        cinematic_fullscreen_pixel_bounds(&screen_bounds);
        bounds->x0 = 40;
        bounds->x1 = screen_bounds.x1 - 40;
        bounds->y0 = screen_bounds.y1 - 75;
        bounds->y1 = screen_bounds.y1 - 25;
        cinematics_pin_title_bounds(bounds, safe_bounds);
    }
}

// @address 0x1909c0 cinematic_render
extern "C" void cinematic_render(bool unused, bool render_titles)
{
    if (cinematic_globals)
    {
        if (cinematic_globals->letterbox_fraction > 0.0f)
        {
            rectangle2d bounds, top_bar, bottom_bar;
            cinematic_compute_letterbox_bars(&bounds, &top_bar, &bottom_bar);
            draw_quad(&top_bar, 0xff000000);
            draw_quad(&bottom_bar, 0xff000000);
        }
        if (render_titles)
        {
            for (long title_index = 0; title_index < 4; title_index++)
            {
                const s_cinematic_title *title = &cinematic_globals->titles[title_index];
                if (title->index != -1)
                {
                    long string_list_index = cinematics_title_scenario_get()->cutscene_title_strings.index;
                    const s_scenario_cutscene_title *definition =
                        &((const s_scenario_cutscene_title *)cinematics_title_scenario_get()->cutscene_titles.address)[title->index];
                    if (string_list_index != -1)
                    {
                        c_rasterizer_draw_string draw_storage;
                        c_draw_string &draw = draw_storage;
                        c_static_wchar_string<256> text;
                        c_font_cache cache;
                        string_list_get_normal_string(string_list_index, definition->name, &text);
                        draw.set_font((e_font_id)definition->font);
                        if (draw.precache_wchar_string_for_font(&cache, text.get_string(), draw.m_font_id))
                        {
                            rectangle2d bounds = definition->bounds;
                            rectangle2d safe_bounds;
                            cinematic_fullscreen_pixel_bounds(&safe_bounds);
                            cinematics_inset_rectangle(&safe_bounds,
                                (short)(long)((float)safe_bounds.x1 * 0.075f),
                                (short)(long)((float)safe_bounds.y1 * 0.05f));
                            cinematics_pin_title_bounds(&bounds, &safe_bounds);
                            cinematics_fix_degenerate_title_bounds(&bounds, &safe_bounds);

                            float seconds = (float)title->elapsed_ticks * game_time_globals->tick_length;
                            float alpha = 1.0f;
                            if (seconds < definition->fade_in_time)
                                alpha = seconds / definition->fade_in_time;
                            else if (seconds > definition->start_time)
                                alpha = 1.0f - (seconds - definition->start_time) / definition->duration;
                            alpha = PIN(alpha, 0.0f, 1.0f);

                            real_argb_color color;
                            cinematics_pixel32_to_real_rgb_color(definition->text_color, &color.rgb);
                            if (fabsf(color.red - 1.0f) < k_cinematic_title_color_epsilon &&
                                fabsf(color.green - 1.0f) < k_cinematic_title_color_epsilon &&
                                fabsf(color.blue - 1.0f) < k_cinematic_title_color_epsilon)
                            {
                                if (color.red > 0.8f)
                                    color.red = 0.8f;
                                if (color.green > 0.8f)
                                    color.green = 0.8f;
                                if (color.blue > 0.8f)
                                    color.blue = 0.8f;
                            }
                            real_argb_color shadow_color;
                            cinematics_pixel32_to_real_rgb_color(definition->shadow_color, &shadow_color.rgb);
                            color.alpha = alpha;
                            shadow_color.alpha = alpha;

                            switch (definition->vertical_justification)
                            {
                            case 1:
                                draw.m_flags.set_unsafe(draw.m_flags.get_unsafe() & ~(FLAG(2) | FLAG(3)));
                                break;
                            case 2:
                                draw.m_flags.set(c_draw_string::_text_flag_center_vertically, true);
                                break;
                            case 0:
                            case 3:
                                draw.m_flags.set(c_draw_string::_text_flag_align_bottom, true);
                                break;
                            }
                            draw.m_justification = (e_text_justification)definition->justification;
                            draw.m_flags.set(c_draw_string::_text_flag_wrap_horizontally, true);
                            draw.set_color_deprecated(&color, false);
                            draw.set_shadow_color(&shadow_color);
                            draw.set_bounds(&bounds);
                            draw.c_draw_string::draw(&cache, text.get_string());
                        }
                    }
                }
            }
        }
    }
    if (cinematic_globals->skip)
    {
        float fade = cinematic_get_skip_fade();
        rectangle2d bounds;
        cinematic_fullscreen_pixel_bounds(&bounds);
        draw_quad(&bounds, (long)(fade * 255.0f) << 24);
    }
    subtitle_render();
}


// @address 0x2408e0 ?cinematic_object_create@@YAJPBDJJW4e_scene_object_type@@@Z
long cinematic_object_create(const char *name, long variant_name, long definition_index, e_scene_object_type object_type)
{
    long index = -1;
    if (name != 0 && cinematic_object_get(name) == -1)
    {
        s_cinematic_new_globals_definition *globals = cinematic_new_globals;
        index = 0;
        while (globals->entries[index].object_index != -1 && index < globals->count)
            ++index;
        if (index >= globals->count)
        {
            globals->count = index + 1;
            if (globals->count > 100)
                index = -1;
        }
        if (index != -1)
        {
            object_placement_data placement;
            object_placement_data_new(&placement, definition_index, -1, 0);
            placement.position = *global_origin3d;
            placement.model_variant_index = variant_name;
            s_cinematics_name_entry *entry = &globals->entries[index];
            // match: 0x2408e0: value range assumption guides branch lowering
            __assume(entry != 0);
            entry->object_index = object_new(&placement);
            csstrnzcpy(cinematic_new_globals->entries[index].name, name, 32);
            cinematic_new_globals->entries[index].group_tag = object_type;
            if (cinematic_new_globals->entries[index].object_index == -1)
                index = -1;
        }
    }
    else
    {
        index = -1;
    }
    return index;
}
