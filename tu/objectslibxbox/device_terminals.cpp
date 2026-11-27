// objectslibxbox/device_terminals.obj: from the records 0x3647f8 0x364818 0x364bc8 0x364d08 0x365c60 0x365e38 0x365ea8 0x366018 0x366250 0x366298 0x366430 0x495f20
#include "game/player_control_deterministic_globals.h"
#include "math/real_math_constants.h"
#include "render/camera_fx_globals_types.h"
#include "game/player_control_globals.h"
#include "memory/tls_globals.h"
#include "sound/game_sound.h"
#include "game/game_achievements_types.h"
#include "interface/c_player_profile.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "shell/shell.h"
#include "cseries/cseries_wrapped.h"
#include "game/game_globals_storage.h"
#include <math.h>
#include "math/real_math.h"
#include "memory/data.h"
#include "interface/gui_widget.h"
#include <new>
#include <string.h>
#include "game/game_time_types.h"
#include "interface/user_interface_messages.h"
#include "game/players.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "objects/objects.h"
#include "camera/director.h"
#include "items/equipment.h"
#include "render/camera_fx_settings.h"
#include "devices/device_terminals.h"
#include "devices/device_datum.h"
#include "devices/device_definitions.h"
#include "game/player_types.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "interface/gui_screens/terminals/gui_screen_terminals.h"
#include "objects/object_datum.h"
#include "sound/game_sound_definitions.h"
#include "cache/cache_files.h"
#include "tag_files/tag_groups.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_window_manager.h"
#include "render/camera_fx_globals.h"
#include "game/game_achievements.h"
#include "hs/hs_function_globals.h"
#include "interface/chud/chud_scripting_globals_types.h"
#include "items/equipment_globals.h"
#include "game/players_globals_types.h"
#include "game/player_control_types.h"
#include "camera/camera_scripting_globals.h"
#include "objects/object_header.h"
#include "game/game_time_inline.h"
#include "game/game_globals_inline.h"
#include "math/random_math_range.h"
// @address 0x35f08 k_scripted_sound_default_decibels
// @address 0x11df75c g_chud_scripting_globals
// @address 0x11df67c players_globals
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x3647f8 ?apply_initial_state@c_load_terminal_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
// @address 0x364818 ?terminal_new@@YA_NJPAUobject_placement_data@@PA_N@Z
// @address 0x365ea8 ?terminal_seek@@YAXJMMM@Z
// @address 0x366018 ?terminal_seek_to_zero@@YAXJM@Z
// @address 0x366250 ?terminal_get_reading_mode@@YA_NJ@Z
// @address 0x366430 ?terminal_compute_function_value@@YA_NJJJPAMPA_N1@Z
// @address 0x495f20 ?get_scroll_rate@c_gui_screen_terminal@@QBAMXZ

enum
{
    _string_id_terminal_error = 0x3ea,
    _string_id_activation = 0x3eb,
    _string_id_layer0_x = 0x3ec,
    _string_id_layer0_y = 0x3ed,
    _string_id_layer1_x = 0x3ee,
    _string_id_layer1_y = 0x3ef,
    _string_id_hex_intensity = 0x3f0,
    _string_id_overlay_intensity = 0x3f1,
    _string_id_reading_mode = 0x3f2,
    _string_id_frame_color = 0x3f3,
    _string_id_scroll_speed = 0x3f4,
    _string_id_terminal_screen = 0x104cd
};

class c_load_terminal_screen_message : public c_load_screen_message
{
public:
    __forceinline c_load_terminal_screen_message(e_controller_index controller_index, e_window_index window_index, long layered_position, long terminal_index)
        : c_load_screen_message(_string_id_terminal_screen, controller_index, window_index, layered_position), m_terminal_index(terminal_index)
    {
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen_widget) const;
private:
    long m_terminal_index;
};

typedef char terminal_flags_offset[(offsetof(s_terminal_datum, flags) == 0x228) ? 1 : -1];
typedef char terminal_state_ticks_offset[(offsetof(s_terminal_datum, state_ticks) == 0x22c) ? 1 : -1];
typedef char terminal_screen_state_ticks_offset[(offsetof(s_terminal_datum, screen_state_ticks) == 0x230) ? 1 : -1];
typedef char terminal_reading_ticks_offset[(offsetof(s_terminal_datum, reading_ticks) == 0x25a) ? 1 : -1];
typedef char terminal_seek_start_position_offset[(offsetof(s_terminal_datum, seek_start_position) == 0x284) ? 1 : -1];
typedef char terminal_final_screen_state_offset[(offsetof(s_terminal_datum, final_screen_state) == 0x29c) ? 1 : -1];

void c_load_terminal_screen_message::apply_initial_state(c_gui_screen_widget *screen_widget) const
{
    c_gui_screen_terminal *terminal_screen = static_cast<c_gui_screen_terminal *>(screen_widget);
    terminal_screen->initialize(m_terminal_index);
}

// @address 0x24820 k_terminal_scroll_rates
// @rdata 0x82024820..0x8202483c
const float k_terminal_scroll_rates[7] = { 1.64f, 3.68f, 1.82f, 5.53f, 4.43f, 15.76f, 1.82f };
// @rdata-end

// @address 0x119a468 g_terminal_layer1_x
// @address 0x119a46c g_terminal_layer1_y
float g_terminal_layer1_x;
float g_terminal_layer1_y;

static __forceinline void device_terminals_seek_direction(float speed, real_vector2d *velocity)
{
    float angle = _real_random_range(tls_random_seed(), 0.0f, 6.2831855f);
    set_real_vector2d(velocity, (float)cos(angle), (float)sin(angle));
    scale_vector2d(velocity, speed, velocity);
}

bool terminal_new(long terminal_index, object_placement_data *placement, bool *)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    terminal->flags = 0;
    terminal->state = _terminal_state_inactive;
    terminal->state_ticks = 0;
    terminal->hidden_object_index = -1;
    terminal->screen_state = 0;
    float *orientation = &terminal->orientation.i;
    orientation[0] = 0.0f;
    orientation[1] = 0.0f;
    terminal->screen_state_ticks = 0;
    orientation[2] = 0.0f;
    terminal->velocity = *global_zero_vector2d;
    real_point2d origin = *global_origin2d;
    terminal->reading_transition = 0.0f;
    terminal->overlay_intensity = 0.0f;
    terminal->reading_mode = false;
    terminal->hex_intensity_limit = 1.0f;
    terminal->reading_ticks = 0;
    terminal->reading_duration = 0;
    terminal->seeking = false;
    terminal->position = origin;
    terminal->seek_finished = false;
    terminal->seeking_zero = false;
    terminal->seek_ticks = 0;
    terminal->seek_duration = 0;
    terminal->seek_end_duration = 0;
    terminal->seek_velocity = *global_zero_vector2d;
    terminal->seek_start_velocity = *global_zero_vector2d;
    terminal->seek_acceleration = *global_zero_vector2d;
    real_vector2d jerk = *global_zero_vector2d;
    terminal->final_screen_state = false;
    terminal->seek_jerk = jerk;
    return true;
}

// @address 0x364bc8 terminal_close
extern "C" void terminal_close(long terminal_index)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    s_terminal_definition *definition = TAG_GET('term', s_terminal_definition, terminal->definition_index);
    c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, _string_id_terminal_screen);
    if (screen)
        screen->transition_out_with_transition_type(0, _screen_transition_normal);
    players_globals->suppressed = false;
    player_control_globals->input_flags &= ~0x4000000;
    float value_a = camera_fx_globals->current_a;
    camera_fx_animate_exposure(value_a, value_a, camera_fx_globals->current_b, 0.0f, 0.5f);
    long sound_index = definition->scripted_sound.index;
    if (sound_index != -1)
        scripted_sound_new_internal(sound_index, terminal_index, 1.0f, k_scripted_sound_default_decibels, k_scripted_sound_default_decibels, -1, 0);
    director_script_camera(false);
    g_chud_scripting_globals->scripting.enabled = true;
    if (terminal->hidden_object_index != -1)
        object_set_hidden(terminal->hidden_object_index, false);
    terminal = (s_terminal_datum *)object_get(terminal_index);
    terminal->screen_state = 0;
    terminal->screen_state_ticks = 0;
}

// @address 0x365e38 terminal_is_unread
extern "C" unsigned char terminal_is_unread(int terminal_index)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    s_terminal_definition *definition = TAG_GET('term', s_terminal_definition, terminal->definition_index);
    unsigned char result = true;
    if (((1 << (definition->terminal_number - 1)) & players_globals->terminals_read) != 0)
        result = false;
    return result;
}

void terminal_seek(long terminal_index, float time, float speed, float end_time)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    device_terminals_seek_direction(speed, &terminal->seek_velocity);
    terminal->seek_start_velocity = terminal->velocity;
    terminal->seek_ticks = 0;
    terminal->seek_duration = (short)game_seconds_to_ticks_round(time);
    terminal->seek_end_duration = (short)game_seconds_to_ticks_round(end_time);
    terminal->seeking = true;
    terminal->seek_finished = false;
    terminal->seeking_zero = false;
}

void terminal_seek_to_zero(long terminal_index, float time)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    terminal->seek_start_velocity = terminal->velocity;
    terminal->seek_velocity = *global_zero_vector2d;
    terminal->seek_ticks = 0;
    terminal->seek_duration = (short)game_seconds_to_ticks_round(time);
    terminal->seek_end_duration = 0;
    terminal->seek_finished = false;
    terminal->seeking_zero = true;
    time = (float)terminal->seek_duration * tls_game_time_globals()->tick_length;
    bool negative_x = terminal->velocity.i < 0.0f;
    bool negative_y = terminal->velocity.j < 0.0f;
    float offset_x = negative_x ? -0.625f : 0.375f;
    float offset_y = negative_y ? -0.625f : 0.375f;
    short turns_x = (short)(terminal->velocity.i * time * 0.75f);
    short turns_y = (short)(terminal->velocity.j * time * 0.75f);
    if (negative_x) turns_x = -turns_x;
    if (negative_y) turns_y = -turns_y;
    real_point2d destination = { (float)turns_x + offset_x, (float)turns_y + offset_y };
    terminal->seek_start_position = terminal->position;
    float time_squared = time * time;
    float time_cubed = time_squared * time;
    real_vector2d displacement;
    vector_from_points2d(&terminal->seek_start_position, &destination, &displacement);
    float jerk_x = terminal->seek_start_velocity.i / time_squared - displacement.i / time_cubed * 2.0f;
    float acceleration_x = terminal->seek_start_velocity.i / time * -0.5f - jerk_x * time * 1.5f;
    terminal->seek_acceleration.i = acceleration_x;
    terminal->seek_jerk.i = jerk_x;
    float jerk_y = terminal->seek_start_velocity.j / time_squared - displacement.j / time_cubed * 2.0f;
    float acceleration_y = terminal->seek_start_velocity.j / time * -0.5f - jerk_y * time * 1.5f;
    terminal->seek_acceleration.j = acceleration_y;
    terminal->seek_jerk.j = jerk_y;
}

bool terminal_get_reading_mode(long terminal_index)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    if (terminal->reading_duration == terminal->reading_ticks)
        return terminal->reading_mode;
    else
        return !terminal->reading_mode;
}

static __forceinline bool device_terminals_controller_flag_test(const c_controller_interface *controller, long bit)
{
    return (controller->flags & (1 << bit)) != 0;
}


// @address 0x366298 terminal_mark_completed
extern "C" void terminal_mark_completed(long terminal_index)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    s_terminal_definition *definition = TAG_GET('term', s_terminal_definition, terminal->definition_index);
    short difficulty = game_is_campaign() ? tls_game_globals()->options.campaign_difficulty : 1;
    bool legendary = difficulty >= 3;
    for (e_controller_index controller_index = _controller0; controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
        c_controller_interface *controller = controller_get(controller_index);
        bool valid = controller->is_signed_in_to_machine() || device_terminals_controller_flag_test(controller, 2);
        if (valid)
        {
            c_player_profile_interface *profile = controller->get_player_profile_interface();
            long completed;
            if (legendary)
                completed = profile->get_unknown_setting_9a();
            else
                completed = profile->get_unknown_setting_98();
            long terminal_bit = 1 << (definition->terminal_number - 1);
            if ((terminal_bit & completed) == 0)
            {
                long new_completed = terminal_bit | completed;
                if ((new_completed & 0x7f) == 0x7f)
                    game_grant_achievement_to_local_player(controller_index, _achievement_marathon_man, false);
                if (legendary)
                    profile->set_unknown_setting_9a((short)new_completed, true);
                else
                    profile->set_unknown_setting_98((short)new_completed, true);
            }
        }
    }
    if (definition->terminal_number > 0)
        players_globals->terminals_read |= 1 << (definition->terminal_number - 1);
}

bool terminal_compute_function_value(long terminal_index, long function_index, long, float *value, bool *active, bool *)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    float result = 0.0f;
    bool valid = true;
    switch (function_index)
    {
    case _string_id_terminal_error:
        result = terminal->orientation.j;
        break;
    case _string_id_activation:
        result = terminal->orientation.i;
        break;
    case _string_id_layer0_x:
        result = terminal->position.x;
        break;
    case _string_id_layer0_y:
        result = terminal->position.y;
        break;
    case _string_id_layer1_x:
        result = g_terminal_layer1_x;
        break;
    case _string_id_layer1_y:
        result = g_terminal_layer1_y;
        break;
    case _string_id_hex_intensity:
        result = terminal->orientation.i <= terminal->hex_intensity_limit ? terminal->orientation.i : terminal->hex_intensity_limit;
        break;
    case _string_id_overlay_intensity:
        result = terminal->overlay_intensity;
        break;
    case _string_id_reading_mode:
        result = terminal->reading_transition;
        break;
    case _string_id_frame_color:
        result = terminal->orientation.k;
        break;
    case _string_id_scroll_speed:
        result = sqrtf(terminal->velocity.i * terminal->velocity.i + terminal->velocity.j * terminal->velocity.j) * 0.16666667f;
        break;
    default:
        valid = false;
        break;
    }
    if (valid)
    {
        result = (result > 0.0f ? result : 0.0f) < 1.0f ? (result > 0.0f ? result : 0.0f) : 1.0f;
        *value = result;
        *active = result > 0.0f;
    }
    return valid;
}

float c_gui_screen_terminal::get_scroll_rate() const
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(m_terminal_index);
    s_terminal_definition *definition = TAG_GET('term', s_terminal_definition, terminal->definition_index);
    long index = definition->terminal_number - 1;
    if (index < 0 || index >= 7)
        index = 0;
    return k_terminal_scroll_rates[index];
}

// @rdata 0x8202251c
static const float k_terminal_zero = 0.0f;
// @rdata 0x8202974c
static const float k_terminal_facing_cosine = 0.70710677f;
// @rdata 0x8202a1a4 pooled
static const float k_terminal_player_distance_squared = 4.0f;

static __forceinline void device_terminals_direction(const real_point3d *position, const real_point3d *origin, real_vector3d *direction)
{
    direction->i = position->x - origin->x;
    direction->j = position->y - origin->y;
    direction->k = k_terminal_zero;
}

static __forceinline float device_terminals_distance_limit()
{
    return k_terminal_player_distance_squared;
}

// @address 0x365c60 terminal_can_activate
extern "C" bool terminal_can_activate(long terminal_index, const real_point3d *position)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    bool result = false;
    if (terminal->state == _terminal_state_inactive && !(static_cast<unsigned short>(terminal->flags) & 1))
    {
        if (position)
        {
            real_point3d origin;
            real_vector3d forward, up;
            object_get_origin(terminal_index, &origin);
            object_get_orientation(terminal_index, &forward, &up);
            real_vector3d direction;
            device_terminals_direction(position, &origin, &direction);
            normalize3d(&direction);
            if ((forward.i * direction.i + direction.j * forward.j) + direction.k * forward.k > k_terminal_facing_cosine)
            {
                result = true;
                if (tls_game_globals()->options.game_mode == 1 && players_get_active_and_in_game_count(true) > 1)
                {
                    c_data_iterator<player_datum> iterator;
                    iterator.m_iterator.absolute_index = -1;
                    iterator.m_iterator.index = -1;
                    iterator.m_iterator.data = tls_player_data();
                    while (iterator.next())
                    {
                        player_datum *player = iterator.get_datum();
                        if (player->unit_index != -1)
                        {
                            real_point3d player_position;
                            object_get_origin(player->unit_index, &player_position);
                            float x = origin.x - player_position.x;
                            float y = origin.y - player_position.y;
                            float z = origin.z - player_position.z;
                            if (x*x + (z*z + y*y) > device_terminals_distance_limit())
                            {
                                result = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
        else
            result = true;
    }
    return result;
}

// @rdata 0x82024714 alias
static const long device_terminals_primary_labels[5] = { 0x104fc, 0x104fd, 0x104fe, 0x104ff, 0x10500 };
// @rdata 0x820247e4 alias
static const long device_terminals_secondary_labels[3] = { 0x104d1, 0x104d4, 0x104d3 };
// @rdata 0x820247f0 alias
static const long device_terminals_icon_labels[10] = { 0x104d1, 0x104d4, 0x104d3, 0x104d6, 0x104d7, 0x104d8, 0x104d9, 0x104da, 0x105be, 0x105bd };

static __forceinline long device_terminals_get_label(const long *const &labels, short index)
{
    return labels[index];
}

static __forceinline void device_terminals_set_reading_mode(long terminal_index, bool reading, short duration)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    if (terminal->reading_mode != reading)
    {
        terminal->reading_mode = reading;
        terminal->reading_duration = duration;
        terminal->reading_ticks = 0;
    }
    if (reading)
        terminal->seeking = false;
}

static __forceinline void device_terminals_set_screen_state(long terminal_index, short state)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    terminal->screen_state = state;
    terminal->screen_state_ticks = 0;
}

static __forceinline void device_terminals_stop_seeking(long terminal_index)
{
    ((s_terminal_datum *)object_get(terminal_index))->seeking = false;
}

static __forceinline void device_terminals_screen_set_final_state(c_gui_screen_terminal *terminal)
{
    short i;
    s_terminal_difficulty_definition *display;
    s_terminal_definition *definition;
    short difficulty;
    c_gui_widget *child;
    short icon_flags;
    short icon_mask;
    c_gui_custom_bitmap_widget *image_widget;
    short image_index;
    unsigned short image_id;

    if (terminal->m_state == 3)
        return;
    display = 0;
    definition = TAG_GET('term', s_terminal_definition, ((s_terminal_datum *)object_get(terminal->m_terminal_index))->definition_index);
    difficulty = game_is_campaign() ? tls_game_globals()->options.campaign_difficulty : 1;
    if (difficulty >= 0 && difficulty < 4)
        display = &definition->displays[difficulty];
    terminal->m_state = 3;
    terminal->m_selected_index = 0;
    terminal->m_state_ticks = 0;
    gui_screen_terminal_set_selected_string(terminal, 0);
    terminal->m_countdown = (short)game_seconds_to_ticks_round(1.0f);
    child = ((c_gui_widget *)terminal)->get_child_widget(_gui_text_widget, 0x104ce);
    child->set_visible(false);
    child = ((c_gui_widget *)terminal)->get_child_widget(_gui_bitmap_widget, 0x104db);
    icon_flags = 0;
    if (display)
        icon_flags = display->visible_images_b;
    if (child)
        child->set_visible(true);
    for (i = 0; i < 3; ++i)
    {
        child = ((c_gui_widget *)terminal)->get_child_widget(_gui_group_widget, device_terminals_get_label(device_terminals_secondary_labels, i));
        if (child)
            child->set_visible(((1 << i) & (short)icon_flags) != 0);
    }
    icon_mask = (short)(icon_flags & 0x3f8);
    for (i = 0; i < 10; ++i)
    {
        child = ((c_gui_widget *)terminal)->get_child_widget(_gui_bitmap_widget, device_terminals_get_label(device_terminals_icon_labels, i));
        if (child)
            child->set_visible(((1 << i) & icon_mask) != 0);
    }
    for (i = 0; i < 5; ++i)
    {
        child = ((c_gui_widget *)terminal)->get_child_widget(_gui_group_widget, device_terminals_get_label(device_terminals_primary_labels, i));
        if (child)
            child->set_visible(false);
    }
    if (!display)
        return;
    child = ((c_gui_widget *)terminal)->get_child_widget(_gui_bitmap_widget, 0x105bc);
    if (!child)
        return;
    image_widget = (c_gui_custom_bitmap_widget *)child;
    image_index = -1;
    image_id = display->image_b;
    if (image_id != 0xffff)
        image_index = (short)image_id;
    else
        child->set_visible(false);
    if (image_index >= 0 && image_index < 8)
    {
        strncpy(image_widget->m_map_image, k_terminal_map_image_names[image_index], 0x100);
        image_widget->m_loading = false;
        image_widget->m_task_id = 0;
        child->set_visible(true);
    }
}

// @rdata 0x8202a1a4 pooled
static const float device_terminals_activation_seconds = 4.0f;
// @rdata 0x82022c0c
static const float device_terminals_seek_seconds = 2.5f;
// @rdata 0x82022dbc
static const float device_terminals_seek_speed = 0.1f;
// @rdata 0x82026484
static const float device_terminals_reading_seconds = 1.5f;
// @rdata 0x82032900
static const float device_terminals_reading_delay_seconds = 20.0f;
// @rdata 0x82022850
static const float device_terminals_scroll_seconds = 10.0f;
// @rdata 0x82024264
static const float device_terminals_exit_seek_seconds = 5.0f;
// @rdata 0x820397e4
static const float device_terminals_exit_seek_speed = 6.0f;

// @rdata 0x82035f08 alias
static const float device_terminals_zero_decibels = 0.0f;

static __forceinline void device_terminals_play_reading_sound(long definition_index, long terminal_index, const real_decibel &gain)
{
    scripted_sound_new_internal(definition_index, terminal_index, k_real_one, gain, gain, -1, 0);
}

static __forceinline long device_terminals_update_seconds_to_ticks_round(float seconds, float half_tick)
{
    return (long)(seconds * (float)tls_game_time_globals()->ticks_per_second + half_tick);
}

static __forceinline long device_terminals_update_round_ticks(const float &ticks, float half_tick)
{
    return (long)(ticks + half_tick);
}

static __forceinline long device_terminals_update_half_seconds_to_ticks_round(float seconds, float half_tick)
{
    float ticks = (float)tls_game_time_globals()->ticks_per_second * seconds;
    return device_terminals_update_round_ticks(ticks * half_tick, half_tick);
}

static __forceinline bool device_terminals_update_flag_test(unsigned long flags, long bit)
{
    return _rotl(flags, 32 - bit) & 1;
}

static __forceinline float device_terminals_update_random_range(unsigned long *seed, float lower, float upper, float scale)
{
    random_seed_next(seed);
    return lower + (upper - lower) * (scale * (float)(*seed >> 16));
}

// @address 0x364d08 ?terminal_update@@YA_NJ@Z
bool terminal_update(long terminal_index)
{
    s_terminal_datum *terminal = (s_terminal_datum *)object_get(terminal_index);
    long old_state = terminal->state;
    long new_state = old_state;
    s_terminal_definition *definition = TAG_GET('term', s_terminal_definition, terminal->definition_index);
    float half_tick = k_real_half;
    switch (old_state)
    {
    case 0:
        if (static_cast<unsigned short>(terminal->flags) & 1)
        {
            terminal_activate_camera(terminal_index);
            new_state = 1;
        }
        break;
    case 1:
        if (terminal->state_ticks > device_terminals_update_seconds_to_ticks_round(device_terminals_activation_seconds, half_tick))
        {
            void *memory = user_interface_malloc_tracked(sizeof(c_load_terminal_screen_message), 0, 0);
            c_load_terminal_screen_message *message =
                memory ? new (memory) c_load_terminal_screen_message(k_any_controller, _console_window, 0x1000c, terminal_index) : 0;
            if (message)
                user_interface_messaging_post(message);
            player_control_globals->input_flags &= ~0x4000000;
            new_state = 2;
        }
        break;
    case 2:
        if (terminal->state_ticks > 5 && device_terminals_update_flag_test((long)g_player_control_globals_deterministic->action_test_flags, 26))
        {
            terminal_close(terminal_index);
            new_state = 0;
        }
        break;
    }
    if (new_state != old_state)
    {
        terminal->state = (e_terminal_state)new_state;
        terminal->state_ticks = 0;
    }
    else if (terminal->state_ticks < 0x7fff)
        ++terminal->state_ticks;

    switch (terminal->state)
    {
    case 0:
        if (terminal->orientation.i > 0.0f)
        {
            short duration = (short)device_terminals_update_seconds_to_ticks_round(k_real_two, half_tick);
            if (terminal->state_ticks > duration)
                terminal->orientation.i = 0.0f;
            else
                terminal->orientation.i = k_real_one - (float)terminal->state_ticks / (float)duration;
        }
        else
            terminal->orientation.i = 0.0f;
        terminal->orientation.k = 0.0f;
        break;
    case 1:
        terminal->orientation.i = (float)terminal->state_ticks /
            (float)device_terminals_update_seconds_to_ticks_round(device_terminals_activation_seconds, half_tick);
        terminal->orientation.k = (k_real_one - terminal->orientation.i) * k_real_third + 0.666f;
        break;
    case 2:
        terminal->orientation.i = k_real_one;
        break;
    }

    float orientation_scale = k_real_third;
    c_gui_screen_terminal *screen = (c_gui_screen_terminal *)g_window_manager.get_screen_by_name(_console_window, _string_id_terminal_screen);
    float random_scale = 1.0f / 65535.0f;
    bool update_screen = true;
    while (update_screen)
    {
        short old_screen_state = terminal->screen_state;
        short new_screen_state = old_screen_state;
        update_screen = false;
        switch (old_screen_state)
        {
        case 0:
            device_terminals_set_reading_mode(terminal_index, false, (short)device_terminals_update_seconds_to_ticks_round(k_real_one, half_tick));
            break;
        case 1:
            if (terminal->screen_state_ticks < device_terminals_update_seconds_to_ticks_round(device_terminals_activation_seconds, half_tick))
            {
                if (!terminal->seeking || terminal->seek_finished)
                {
                    float time = device_terminals_update_random_range(tls_random_seed(), device_terminals_seek_seconds, 3.5f, random_scale);
                    float speed = device_terminals_update_random_range(tls_random_seed(), device_terminals_seek_speed, 0.5f, random_scale);
                    terminal_seek(terminal_index, time, speed, 0.0f);
                }
            }
            else if (terminal->seek_finished)
            {
                if (!terminal->seeking_zero)
                {
                    float time = device_terminals_update_random_range(tls_random_seed(), device_terminals_seek_seconds, 3.5f, random_scale);
                    terminal_seek_to_zero(terminal_index, time);
                }
                else
                    new_screen_state = (1 << (definition->terminal_number - 1)) & players_globals->terminals_read ? 5 : 2;
            }
            break;
        case 2:
            device_terminals_set_reading_mode(
                terminal_index, true, (short)device_terminals_update_seconds_to_ticks_round(device_terminals_reading_seconds, half_tick));
            if (terminal_get_reading_mode(terminal_index) && screen)
                gui_screen_terminal_set_state(screen, 1);
            if (!device_terminals_update_flag_test(terminal->flags, 3) &&
                terminal->screen_state_ticks > device_terminals_update_seconds_to_ticks_round(device_terminals_reading_delay_seconds, half_tick))
            {
                new_screen_state = 3;
                if (definition->reading_sound.index != -1)
                {
                    device_terminals_play_reading_sound(
                        definition->reading_sound.index, terminal_index, *(const real_decibel *)&device_terminals_zero_decibels);
                }
            }
            terminal->orientation.j = 0.0f;
            terminal->orientation.k = terminal->reading_transition * orientation_scale + 0.666f;
            break;
        case 3:
        {
            float duration;
            if (screen)
            {
                gui_screen_terminal_set_state(screen, 2);
                duration = screen->get_scroll_rate() + device_terminals_activation_seconds;
            }
            else
                duration = device_terminals_scroll_seconds;
            terminal->orientation.j = k_real_one;
            terminal->orientation.k = orientation_scale;
            if (!terminal_is_unread(terminal_index))
            {
                if (terminal->screen_state_ticks > device_terminals_update_half_seconds_to_ticks_round(duration, half_tick))
                {
                    new_screen_state = 2;
                    terminal->flags |= 8;
                }
            }
            else if (terminal->screen_state_ticks > device_terminals_update_seconds_to_ticks_round(duration, half_tick))
                new_screen_state = 4;
            break;
        }
        case 4:
            if (screen)
                gui_screen_terminal_set_state(screen, 0);
            device_terminals_set_reading_mode(terminal_index, false, (short)device_terminals_update_seconds_to_ticks_round(k_real_one, half_tick));
            if (!terminal_get_reading_mode(terminal_index))
            {
                if (!terminal->seeking)
                    terminal_seek(terminal_index, device_terminals_exit_seek_seconds, device_terminals_exit_seek_speed, device_terminals_activation_seconds);
                else if (terminal->seek_finished)
                {
                    if (!terminal->seeking_zero)
                        terminal_seek_to_zero(terminal_index, device_terminals_exit_seek_seconds);
                    else
                    {
                        terminal_mark_completed(terminal_index);
                        new_screen_state = 5;
                    }
                }
            }
            terminal->orientation.j = terminal->reading_transition;
            terminal->orientation.k = (k_real_one - terminal->reading_transition + k_real_one) * orientation_scale;
            break;
        case 5:
            device_terminals_set_reading_mode(
                terminal_index, true, (short)device_terminals_update_seconds_to_ticks_round(device_terminals_reading_seconds, half_tick));
            if (terminal_get_reading_mode(terminal_index))
            {
                if (screen)
                    device_terminals_screen_set_final_state(screen);
                terminal->final_screen_state = true;
            }
            terminal->orientation.k = terminal->reading_transition * orientation_scale + 0.666f;
            break;
        }
        if (new_screen_state != old_screen_state)
        {
            device_terminals_set_screen_state(terminal_index, new_screen_state);
            update_screen = true;
        }
        else if (terminal->screen_state_ticks < 0x7fff)
            ++terminal->screen_state_ticks;
    }

    if (terminal->state == _terminal_state_activating || terminal->state == _terminal_state_active)
    {
        if (terminal->seek_ticks < terminal->seek_duration)
        {
            ++terminal->seek_ticks;
            if (terminal->seeking_zero)
            {
                float time = (float)terminal->seek_ticks * tls_game_time_globals()->tick_length;
                float time_squared = time * time;
                float time_cubed = time_squared * time;
                float acceleration_i = terminal->seek_acceleration.i;
                float jerk_i = terminal->seek_jerk.i;
                float start_x = terminal->seek_start_position.x;
                float start_velocity_i = terminal->seek_start_velocity.i;
                float acceleration_j = terminal->seek_acceleration.j;
                float jerk_j = terminal->seek_jerk.j;
                float start_y = terminal->seek_start_position.y;
                float start_velocity_j = terminal->seek_start_velocity.j;
                terminal->velocity.i = jerk_i * time_squared * 3.0f + acceleration_i * time * 2.0f + start_velocity_i;
                terminal->velocity.j = jerk_j * time_squared * 3.0f + acceleration_j * time * 2.0f + start_velocity_j;
                terminal->position.x = (acceleration_i * time_squared + (jerk_i * time_cubed + start_velocity_i * time)) + start_x;
                terminal->position.y = (acceleration_j * time_squared + (jerk_j * time_cubed + start_velocity_j * time)) + start_y;
            }
            else
            {
                float seek_fraction = (float)terminal->seek_ticks / (float)terminal->seek_duration;
                real_vector2d velocity_change;
                subtract_vectors2d(&terminal->seek_velocity, &terminal->seek_start_velocity, &velocity_change);
                point_from_line2d(&terminal->seek_start_velocity, &velocity_change, seek_fraction, &terminal->velocity);
            }
        }
        else
        {
            terminal->velocity = terminal->seek_velocity;
            if (terminal->seek_end_duration > 0)
                --terminal->seek_end_duration;
            if (terminal->seek_end_duration <= 0)
                terminal->seek_finished = true;
        }
        point_from_line2d(&terminal->position, &terminal->velocity, tls_game_time_globals()->tick_length, &terminal->position);
        float x = fmod(terminal->position.x, k_real_one);
        float y = fmod(terminal->position.y, k_real_one);
        if (x < 0.0f)
            x += 1.0f;
        if (y < 0.0f)
            y += 1.0f;
        terminal->position.x = PIN(x, 0.0f, 1.0f);
        terminal->position.y = PIN(y, 0.0f, 1.0f);
    }
    else
        terminal->velocity = *global_zero_vector2d;

    float reading_fraction;
    if ((short)terminal->reading_ticks < (short)terminal->reading_duration)
    {
        ++terminal->reading_ticks;
        reading_fraction = (float)(short)terminal->reading_ticks / (float)(short)terminal->reading_duration;
        if (!terminal->reading_mode)
            reading_fraction = 1.0f - reading_fraction;
    }
    else
        reading_fraction = terminal->reading_mode ? 1.0f : 0.0f;
    terminal->reading_transition = (1.0f - (float)cos(reading_fraction * 3.1415927f)) * 0.5f;
    terminal->overlay_intensity = reading_fraction;
    terminal->hex_intensity_limit = 1.0f - reading_fraction * 0.25f;
    terminal->orientation.i = PIN(terminal->orientation.i, 0.0f, 1.0f);
    return true;
}

// @address 0x73c0b0 k_real_third
extern "C" {
float k_real_third = 0.333f;
}

