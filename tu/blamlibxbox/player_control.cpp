// blamlibxbox/player_control.obj: from the records 0x89438 0x98900 0x98a78 0x992b8 0x11a288 0x11a2b8 0x211cf8 0x211d70 0x211e28 0x211ea8 0x214700 0x2147b0 0x215130 0x215358 0x2153b8 0x215458 0x215568 0x216068 0x2166d8 0x216868 0x216970 0x216a40 0x216b08 0x216be8 0x216c80 0x216d90 0x216e40 0x216eb0 0x216f50 0x216f68 0x216f88 0x217028 0x217040 0x217640 0x219400 0x232d10 0x707f40 0x707f50
#include "memory/datum_index.h"
#include "memory/data_inline.h"
#include "game/player_control_deterministic_globals.h"
#include "game/player_control_globals.h"
#include "game/players_globals_types.h"
#include "game/players_global_data.h"
#include "game/campaign_metagame_globals.h"
#include "memory/tls_data_arrays.h"
#include "math/matrix_math_types.h"
#include "interface/c_player_profile.h"
#include "interface/controller_types.h"
#include "math/real_math.h"
#include "shell/shell.h"
#include "objects/object_reference_types.h"
#include "game/game_globals_storage.h"
#include "game/game_state_accessors.h"
#include "units/unit_inventory_inline.h"
#include "objects/object_types.h"
#include "units/biped_types.h"
#include "units/vehicle_types.h"
#include <math.h>
#include <string.h>
#include "game/game_globals.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "game/players.h"
#include "objects/objects.h"
#include "units/units.h"
#include "game/player_control.h"
#include "interface/user_interface_objectives.h"
#include "items/weapons.h"
#include "motor/motor_system.h"
#include "objects/object_early_movers.h"
#include "units/bipeds.h"
#include "ai/cl_engine.h"
#include "cutscene/cinematics.h"
#include "game/game_time.h"
#include "game/game_time_access.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_controller.h"
#include "main/main.h"
#include "math/matrix_math.h"
#include "units/vehicles.h"
#include "units/unit_control.h"
#include "units/unit_camera.h"
#include "units/unit_definitions.h"
#include "memory/data_types.h"
#include "game/player_masks_types.h"
#include "cache/cache_files.h"
#include "input/input_abstraction.h"
#include "input/input_abstraction_types.h"
#include "game/player_control_constants.h"
#include "interface/user_interface_window_manager.h"
#include "multithreading/threads.h"
#include "game/player_mapping_types.h"
#include "interface/user_interface_utilities.h"
#include "game/campaign_metagame.h"
#include "render/render_objects_static_lighting.h"
#include "simulation/simulation_world_types.h"
#include "game/player_control_types.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "effects/effect_counts.h"
#include "memory/gameplay_tls_globals.h"
#include "cseries/crt_xdk.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard_types.h"
#include "game/player_control_director_types.h"
#include "simulation/simulation_globals.h"
#include "main/main_time_globals.h"
#include "game/aim_assist_control.h"
#include "game/player_information_definition.h"
#include "game/player_types.h"
#include "objects/object_early_movers_tls.h"
#include "animations/animation_manager.h"
#include "items/weapon_definitions.h"
#include "cutscene/cinematic_globals_types.h"
#include "scenario/scenario_game_globals.h"
#include "game/game_time_types.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header_access.h"
#include "memory/data.h"
#include "interface/user_interface_sound.h"
#include "units/unit_animation.h"
enum { _aim_assist_secondary_bit = 0, _aim_assist_multiplayer_bit = 1 };
struct s_player_control_aim_assist_angles
{
    float yaw;
    float pitch;
    float scale;
};
// @address 0x11df748 g_player_control_globals_deterministic
__declspec(thread) s_player_control_deterministic_globals *g_player_control_globals_deterministic;
// @address 0x11df678 player_control_globals
__declspec(thread) s_player_control_globals *player_control_globals;
// @address 0x808560 g_simulation_globals
// @address 0x11401e8 g_controller_globals
// @flags /TP /O2
// @address 0x11a288 ?player_control_get_target_autoaim_level@@YAMJ@Z
// @address 0x11a2b8 ?player_control_get_zoom_level@@YAFJ@Z

enum e_user_interface_sound_effect;
const float k_radians_per_degree = 0.017453292f;
// @address 0x992b8 ?evaluate_piecewise_linear_function@@YAMFQAMM@Z
// @address 0x217640 ?__tls_set_g_player_control_globals_allocator@@YAXPAX@Z
// @address 0x232d10 ?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z

typedef real_vector2d vector2;
typedef real_point3d point3;
typedef s_unit_weapon_set s_weapons;
struct s_player_control_user_offset { long value; };

#if !defined(_XBOX) || defined(__clang__)
struct s_player_control_tap_latch
{
    bool recorded;
    long time;
    long flags;
    unsigned short buttons;
    float primary;
    float secondary;
    long latched_flags;
    unsigned short latched_buttons;
    float latched_primary;
    float latched_secondary;
    float latched_seconds;
};

static s_player_control_tap_latch g_player_control_tap_latches[NUMBEROF(player_control_globals->users)];
#endif

static __forceinline s_player_control_state *player_control_get_state(long player_index)
{
    return &player_control_globals->states[player_index];
}

// @address 0x89438 ?down_frames@c_abstract_button@@QBAEXZ

class c_restricted_memory;

namespace {
static __forceinline long restricted_region_add_player_control_member(const char *name, const char *type,
    unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[3].add_member(name, type, size, alignment_bits, havok_notify_thread_running, 0, 0);
    void *address = restricted_region_get_member_address(3, member_index);
    player_control_globals = (s_player_control_globals *)address;
    return member_index;
}
static __forceinline long restricted_region_add_player_control_deterministic_member(const char *name,
    const char *type, unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[3].add_member(name, type, size, alignment_bits, __tls_set_g_objectives_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member_index);
    g_player_control_globals_deterministic = (s_player_control_deterministic_globals *)address;
    return member_index;
}
}

// @address 0x216eb0 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_control_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z

// @address 0x216f88 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z

// match: 0x211ea8: inline lets player_control_get_controller_input inline down_frames; out of line r29/r25 shift to r7/r9/r26
#if defined(_XBOX) && !defined(__clang__)
inline unsigned char c_abstract_button::down_frames() const
#else
unsigned char c_abstract_button::down_frames() const
#endif
{
    uint8 frames = 0;
    if (!latched() && access_valid()) {
        frames = m_down_frames;
    }
    return frames;
}

// @address 0x211cf8 ?down_amount@c_abstract_button@@QAAMXZ
inline float c_abstract_button::down_amount()
{
    float amount = 0.0f;
    if (!latched() && access_valid()) {
        amount = m_down_amount;
    }
    return amount;
}

// The March release map folds these empty game-system callbacks into the shared blr at 0x5bec98.
// @address 0x5bec98 ?player_control_dispose@@YAXXZ
void player_control_dispose()
{
}

// @address 0x5bec98 ?player_control_dispose_from_old_map@@YAXXZ
void player_control_dispose_from_old_map()
{
}

// @address 0x98a78 ?player_control_initialize_for_new_map@@YAXXZ
void player_control_initialize_for_new_map()
{
    player_control_globals->input_locked_flags = 0;
    player_control_globals->input_flags = 0;
    player_control_globals->input_held_flags = 0;
    player_control_globals->input_released_flags = 0;
    g_player_control_globals_deterministic->action_test_flags = 0;
    for (long input_user_index = 0; input_user_index < 4; input_user_index++) {
        player_control_set_user_unit(input_user_index, -1);
        if (input_user_index != -1) {
            s_player_control_globals *globals = player_control_globals;
            s_player_control_state *state = &globals->states[input_user_index];
            if (!globals->flag || state->unit_index != -1) {
                state->unit_index = -1;
                player_control_set_action_from_unit(-1, &state->action);
            }
        }
    }
    player_control_globals->flag = true;
    player_control_globals->a = 1.0f;
    player_control_globals->b = 1.0f;
    player_control_globals->c = 0.0f;
    player_control_globals->enabled = false;
    player_control_globals->d = 0.0f;
    player_control_globals->e = 0.0f;
#if !defined(_XBOX) || defined(__clang__)
    memset(g_player_control_tap_latches, 0, sizeof(g_player_control_tap_latches));
#endif
}

float evaluate_piecewise_linear_function(short sample_count, float samples[], float input)
{
  bool input_is_negative = input < k_real_zero;
  long last_sample_index = sample_count - 1;
  float sample_position = MIN(MAX(fabsf(input) * (float)last_sample_index, k_real_zero), (float)sample_count - k_real_one);
  short sample_index = (short)sample_position;
  sample_index = MIN(MAX(sample_index, 0), last_sample_index);
  short next_sample_index = MIN(sample_index + 1, last_sample_index);
  float value = samples[sample_index] + (samples[next_sample_index] - samples[sample_index]) * (sample_position - (float)sample_index);
  if (input_is_negative)
    value = -value;
  return value;
}

float player_control_get_target_autoaim_level(long player_index)
{
    return MAX(player_control_get_state(player_index)->action.aim_assist.primary_autoaim_level,
               player_control_get_state(player_index)->action.aim_assist.secondary_autoaim_level);
}

short player_control_get_zoom_level(long player_index)
{
    short zoom_level = -1;
    if (player_index != -1) {
        s_player_control_state *state = player_control_get_state(player_index);
        zoom_level = state->action.zoom;
    }
    return zoom_level;
}

template<>
void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_control_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x216eb0: without this the add_member call drops the li r7,0 that materializes the known-zero alignment_bits
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_player_control_member(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

template<>
void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_control_globals_deterministic_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x216f88: without this the deterministic add_member call drops the li r7,0 materializing alignment_bits
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_player_control_deterministic_member(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

void __tls_set_g_player_control_globals_allocator(void *allocator)
{
    g_campaing_metagame_globals = (s_campaing_metagame_globals *)allocator;
}

// @address 0x11df730 g_effect_counts
void __tls_set_g_player_control_globals_deterministic_allocator(void *allocator)
{
    g_effect_counts = (s_effect_counts *)allocator;
}

extern "C" {
// @address 0x3e304 k_player_control_globals_allocation_name
// @address 0x3e31c k_player_control_deterministic_allocation_name
// @rdata 0x8203e304..0x8203e344
const char k_player_control_globals_allocation_name[24] = "player control globals";
const char k_player_control_deterministic_allocation_name[40] = "player control globals deterministic";
}
// @rdata-end
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z

// @address 0x211d70 ?player_control_initialize@@YAXXZ
void player_control_initialize()
{
    player_control_globals = (s_player_control_globals *)g_player_control_globals_allocator.reserve_memory(k_player_control_globals_allocation_name,
        0,
        1420,
        0);
    g_player_control_globals_deterministic =
        (s_player_control_deterministic_globals *)
        g_player_control_globals_deterministic_allocator.reserve_memory(k_player_control_deterministic_allocation_name,
            0,
            4,
            0);
    player_control_globals->flag = false;
    player_control_globals->a = 1.0f;
    player_control_globals->b = 1.0f;
    player_control_globals->c = 0.0f;
    player_control_globals->enabled = false;
    player_control_globals->d = 0.0f;
    player_control_globals->e = 0.0f;
}

// @address 0x211e28 player_control_can_use_debug_controls
extern "C" bool player_control_can_use_debug_controls(void)
{
    return game_in_progress() && game_globals->options.game_mode != 3 && game_globals->options.game_network_type != 3;
}

struct s_input_abstraction_globals;

// @address 0x215130 player_control_update_debug_toggles
extern "C" void player_control_update_debug_toggles(void)
{
    int mapped_user_index;
    s_game_input_state *user_button_state;
    unsigned char first_modifier_slot;
    unsigned char first_modifier_reload;
    c_abstract_button *first_modifier_button;
    signed char button_locked_by;
    unsigned char second_modifier_slot;
    unsigned char second_modifier_reload;
    unsigned char first_toggle_slot;
    unsigned char first_toggle_reload;
    s_player_control_globals *first_toggle_globals;
    unsigned char second_toggle_slot;
    unsigned char second_toggle_reload;
    s_player_control_globals *second_toggle_globals;
    unsigned char debug_controls_ready;
    unsigned char button_access_allowed;
    unsigned char modifier_down_frames;
    int debug_controller_index;
    debug_controls_ready = !((((unsigned char)player_control_can_use_debug_controls()) == 0)) &&
        (((unsigned int)player_control_globals->thread_active != 0));
    if ((((unsigned char)debug_controls_ready) != 0))
    {
        debug_controller_index = 0;
        do
        {
            mapped_user_index = user_interface_controller_get_rumble_enabled(debug_controller_index);
            if (((unsigned int)mapped_user_index > 3))
                continue;
            user_button_state = &input_abstraction_globals.input_states[mapped_user_index];
            if ((user_button_state == 0))
                continue;
            first_modifier_slot = user_button_state->abstract_buttons[_input_action_crouch];
            modifier_down_frames = 0;
            user_button_state->buttons[first_modifier_slot].m_button_index = 13;
            first_modifier_reload = user_button_state->abstract_buttons[_input_action_crouch];
            first_modifier_button = &user_button_state->buttons[first_modifier_reload];
            if (((unsigned int)(first_modifier_button->m_flags & 1) == 0))
            {
                button_locked_by = first_modifier_button->m_alternate_button_index;
                button_access_allowed = !(!((button_locked_by == -1)) && ((button_locked_by != first_modifier_button->m_button_index)));
                if ((((unsigned char)button_access_allowed) != 0))
                    modifier_down_frames = first_modifier_button->m_down_frames;
            }
            if ((((unsigned char)modifier_down_frames) == 0))
                continue;
            second_modifier_slot = user_button_state->abstract_buttons[_input_action_zoom];
            user_button_state->buttons[second_modifier_slot].m_button_index = 14;
            second_modifier_reload = user_button_state->abstract_buttons[_input_action_zoom];
            if ((((unsigned char)((c_abstract_button *)&user_button_state->buttons[second_modifier_reload])
                         ->down_frames()) == 0))
                continue;
            first_toggle_slot = user_button_state->abstract_buttons[_input_action_unknown22];
            user_button_state->buttons[first_toggle_slot].m_button_index = 22;
            first_toggle_reload = user_button_state->abstract_buttons[_input_action_unknown22];
            if ((((unsigned char)((c_abstract_button *)&user_button_state->buttons[first_toggle_reload])
                         ->down_frames()) == 1))
            {
                first_toggle_globals = player_control_globals;
                first_toggle_globals->thread_enabled = (first_toggle_globals->thread_enabled == 0);
                ::user_interface_play_sound(_ui_sound_dialog, -1);
            }
            second_toggle_slot = user_button_state->abstract_buttons[_input_action_unknown23];
            user_button_state->buttons[second_toggle_slot].m_button_index = 23;
            second_toggle_reload = user_button_state->abstract_buttons[_input_action_unknown23];
            if ((((unsigned char)((c_abstract_button *)&user_button_state->buttons[second_toggle_reload])
                         ->down_frames()) != 1))
                continue;
            second_toggle_globals = player_control_globals;
            second_toggle_globals->thread_unknown58a = (second_toggle_globals->thread_unknown58a == 0);
            ::user_interface_play_sound(_ui_sound_dialog, -1);
        } while (++debug_controller_index < 4);
    }
}


// @address 0x215358 player_control_is_old_controls_active
extern "C" bool player_control_is_old_controls_active(void)
{
    return player_control_can_use_debug_controls() && player_control_globals->thread_active &&
        player_control_globals->thread_enabled;
}

// @address 0x216f50 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_control_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_control_globals_allocator>::allocate(unsigned int size,
    const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x217028 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_control_globals_deterministic_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x217040 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_control_globals_deterministic_allocator>::deallocate(void *address)
{
    g_player_control_globals_deterministic = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x219400 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_campaing_metagame_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_campaing_metagame_allocator>::deallocate(void *address)
{
    int value = m_member_index;
    g_campaing_metagame_globals = 0;
    restricted_region_free_member(3, value);
}

// @address 0xa1970 ?player_action_clear@@YAXPAUplayer_action@@@Z

// @address 0x2153b8 player_control_set_user_unit
extern "C" void player_control_set_user_unit(long input_user_index, long unit_index)
{
    if (input_user_index != -1) {
        s_player_control_globals *globals = player_control_globals;
        s_player_control_user_state *user = &globals->users[input_user_index];
        if (!globals->flag || user->unit_index != unit_index) {
            memset(user, 0, sizeof(*user));
            user->unit_index = unit_index;
            player_control_clear_action(&user->action);
            user->maximum_pitch = 1.4922565f;
            user->target_unit_index = -1;
            user->minimum_pitch = -1.4922565f;
            player_control_set_action_from_unit(user->unit_index, &user->action);
        }
    }
}

// @address 0x215458 ?player_control_set_external_action@@YAXJPBUplayer_action@@@Z
void player_control_set_external_action(long player_index, const player_action *action)
{
    control *state = &player_control_get_state(player_index)->action;
    state->look = action->facing;
    state->action_flags = action->action_flags;
    state->flags = action->flags;
    state->weapons = action->weapons;
    state->grenade = action->grenade;
    state->zoom = action->zoom;
    state->move = action->move;
    state->primary = action->primary;
    state->secondary = action->secondary;
    state->action_context = action->action_context;
    state->byte = action->byte;
    state->extra = action->extra;
    state->extra_short = action->extra_short;
    memcpy(&state->aim_assist, &action->aim_assist, sizeof(state->aim_assist));
}

#define PLAYER_CONTROL_TEST_BIT(flags, bit) (((flags) >> (bit)) & 1)
static __forceinline void player_control_flags_set(unsigned long *flags, long bit, bool enable)
{
    if (enable)
        *flags |= (1 << bit);
    else
        *flags &= ~(1 << bit);
}

// @address 0x216068 player_control_update_input_flags
extern "C" void player_control_update_input_flags(s_game_input_state *input_state, s_player_control_input *input)
{
    if (input->button_flags & (1 << 2))
        player_control_globals->input_flags |= 1;
    if (input->action_flags & (1 << 1))
        player_control_globals->input_flags |= 2;
    if (input->action_flags & (1 << 28))
        player_control_globals->input_flags |= 32;
    if (input->action_flags & (1 << 2))
        player_control_globals->input_flags |= 0x100000;
    if (input->action_flags & (1 << 5))
        player_control_globals->input_flags |= 64;
    if (input->control_flags & (1 << 4))
        player_control_globals->input_flags |= 4;
    if (input->control_flags & (1 << 5))
        player_control_globals->input_flags |= 8;
    if (input->control_flags & (1 << 2))
        player_control_globals->input_flags |= 0x200;
    if (input->control_flags & (1 << 0))
        player_control_globals->input_flags |= 128;
    if (input->control_flags & (1 << 1))
        player_control_globals->input_flags |= 0x100;
    if (input->button_flags & (1 << 12))
        player_control_globals->input_flags |= 0x2000000;
    if (input->button_flags & (1 << 13))
        player_control_globals->input_flags |= 0x4000000;
    if (input->primary_trigger > 0.0f)
        player_control_globals->input_flags |= 16;
    if (input_state->get_button(11).get_down_frames())
        player_control_globals->input_flags |= 0x40000;
    if (input_state->get_button(12).get_down_frames())
        player_control_globals->input_flags |= 0x80000;
    if (input->pitch_delta > 0.0f)
        player_control_globals->input_flags |= 0x400;
    else if (input->pitch_delta < 0.0f)
        player_control_globals->input_flags |= 0x800;
    if (input->yaw_delta > 0.0f)
        player_control_globals->input_flags |= 0x1000;
    else if (input->yaw_delta < 0.0f)
        player_control_globals->input_flags |= 0x2000;
    if (input->throttle.x > 0.0f)
        player_control_globals->input_flags |= 0x4000;
    else if (input->throttle.x < 0.0f)
        player_control_globals->input_flags |= 0x8000;
    if (input->throttle.y > 0.0f)
        player_control_globals->input_flags |= 0x10000;
    else if (input->throttle.y < 0.0f)
        player_control_globals->input_flags |= 0x20000;
    if (input->vehicle_pitch > 0.0f)
        player_control_globals->input_flags |= 0x800000;
    else if (input->vehicle_pitch < 0.0f)
        player_control_globals->input_flags |= 0x1000000;
    s_player_control_globals *globals = player_control_globals;
    if (flag_test((unsigned long)globals->input_locked_flags, 0L)) {
        input->button_flags = (input->button_flags & 0xfff1) | 8;
    } else if (flag_test((unsigned long)globals->input_held_flags, 0L)) {
        player_control_flags_set(&globals->input_held_flags, 0, flag_test((unsigned long)input->button_flags, 2L));
        input->button_flags = (input->button_flags & 0xfff1) | 8;
    }
    if (flag_test((unsigned long)globals->input_locked_flags, 2L)) {
        input->control_flags &= ~16;
    } else if (flag_test((unsigned long)globals->input_held_flags, 2L)) {
        player_control_flags_set(&globals->input_held_flags, 2, flag_test((unsigned long)input->control_flags, 4L));
        input->control_flags &= ~16;
    }
    if (flag_test((unsigned long)globals->input_locked_flags, 3L)) {
        input->control_flags &= ~32;
    } else if (flag_test((unsigned long)globals->input_held_flags, 3L)) {
        player_control_flags_set(&globals->input_held_flags, 3, flag_test((unsigned long)input->control_flags, 5L));
        input->control_flags &= ~32;
    }
    if (flag_test((unsigned long)globals->input_flags, 21L))
        input->pitch_delta = (float)fabs(input->pitch_delta);
    else if (flag_test((unsigned long)globals->input_flags, 22L))
        input->pitch_delta = -(float)fabs(input->pitch_delta);
    if (flag_test((unsigned long)globals->input_held_flags, 12L))
        input->yaw_delta = input->yaw_delta > 0.0f ? 0.0f : input->yaw_delta;
    if (flag_test((unsigned long)globals->input_held_flags, 13L))
        input->yaw_delta = input->yaw_delta > 0.0f ? input->yaw_delta : 0.0f;
    if (flag_test((unsigned long)globals->input_held_flags, 10L))
        input->pitch_delta = input->pitch_delta > 0.0f ? 0.0f : input->pitch_delta;
    if (flag_test((unsigned long)globals->input_held_flags, 11L))
        input->pitch_delta = input->pitch_delta > 0.0f ? input->pitch_delta : 0.0f;
}

static __forceinline long player_control_first_active_input_user()
{
    long result = -1;
    for (long user_index = 0; user_index < 4; user_index++) {
        if (player_mapping_globals->player_user_mapping[user_index] != -1) {
            result = user_index;
            break;
        }
    }
    return result;
}

// @address 0x2166d8 player_control_get_look_pitch
extern "C" float player_control_get_look_pitch(void)
{
    long input_user_index = player_control_first_active_input_user();
    float pitch = 0.0f;
    if (input_user_index != -1)
        pitch = player_control_globals->users[input_user_index].action.look.pitch;
    return pitch;
}

struct cache_file_tag_instance;
// @address 0xa9548 ?vehicle_get_type@@YA?AW4e_vehicle_type@@J@Z
namespace {
static __forceinline bool player_control_object_is_vehicle(long object_index)
{
    unit_datum *object = ((unit_datum *)object_get(object_index));
    return ((1 << object->object.object_identifier.m_type) & 2) != 0;
}
}
// @address 0x216868 ?user_currently_piloting_aircraft@@YA_NJ@Z
bool user_currently_piloting_aircraft(long input_user_index)
{
    bool piloting_aircraft = false;
    long unit_index = player_control_globals->users[input_user_index].unit_index;
    if (unit_index != -1) {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 3);
        if (unit->object.parent_object_index != -1 &&
            unit->unit.parent_seat_index != -1 &&
            player_control_object_is_vehicle(unit->object.parent_object_index)) {
            long vehicle_index = ((unit_datum *)object_get_and_verify_type(unit_index, 3))->object.parent_object_index;
            short seat_index = unit->unit.parent_seat_index;
            if (((1 << vehicle_get_type(vehicle_index)) & 148) != 0) {
                unit_datum *vehicle = (unit_datum *)object_get_and_verify_type(vehicle_index, 2);
                unit_definition *definition =
                    (unit_definition *)tag_get(0, vehicle->object.definition_index);
                if (definition->seats.begin()[seat_index].is_driver_seat())
                    piloting_aircraft = true;
            }
        }
    }
    return piloting_aircraft;
}

// @address 0x216a40 player_control_set_input_scale
extern "C" void player_control_set_input_scale(float value, float duration)
{
    s_player_control_globals *globals;
    value = MIN(MAX(value, 0.0f), 1.0f);
    globals = player_control_globals;
    globals->b = value;
    if (duration > 0.0f) {
        duration = MIN(MAX(duration, k_real_epsilon), 60.0f);
        globals->c = duration;
        globals->enabled = true;
    } else {
        float &current = globals->a;
        float &time = globals->c;
        current = value;
        time = 0.0f;
        globals->enabled = fabsf(value - 1.0f) >= k_real_epsilon;
    }
}

static __forceinline float player_control_clamp_angle(float angle)
{
    return MIN(MAX(angle, k_real_epsilon), 5000.0f);
}
// @address 0x216b08 player_control_set_look_point
extern "C" void player_control_set_look_point(long player_index, long point_reference, float angle)
{
    s_player_control_user_state *user = &player_control_globals->users[player_index];
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    if (point_reference != -1 && point_try_and_get_frame(point_reference, &position, &forward, &up)) {
        angle = player_control_clamp_angle(angle);
        user->angle = angle * k_radians_per_degree;
        user->active = 1;
        user->position = position;
    }
}

static __forceinline void player_control_aim_assist_clear(s_aim_assist_targeting_result *result)
{
    memset(result, 0, sizeof(*result));
    result->target_object_index = -1;
    result->model_index = -1;
    result->target_player_index = -1;
}

// @address 0x216be8 player_control_clear_action
extern "C" void player_control_clear_action(control *action)
{
    memset(action, 0, sizeof(*action));
    action->weapons.set_identifier = -1;
    action->weapons.weapon_indices[0] = -1;
    action->weapons.weapon_indices[1] = -1;
    action->grenade = -1;
    action->zoom = -1;
    player_control_aim_assist_clear(&action->aim_assist);
    memset(&action->action_context, 0, sizeof(action->action_context));
    memset(&action->action_context.interaction, 0, sizeof(action->action_context.interaction));
    action->action_context.interaction.object_index = -1;
    action->action_context.melee_target_unit = -1;
}

// @address 0x216c80 player_control_build_action
extern "C" void player_control_build_action(const control *input, player_action *out)
{
    player_action_clear(out);
    out->facing = input->look;
    out->move = input->move;
    out->action_flags = input->action_flags;
    out->flags = input->flags;
    out->weapons = input->weapons;
    out->grenade = input->grenade;
    out->zoom = input->zoom;
    out->primary = input->primary;
    out->secondary = input->secondary;
    out->action_context = input->action_context;
    out->byte = input->byte;
    out->extra = input->extra;
    out->extra_short = input->extra_short;
    memcpy(&out->aim_assist, &input->aim_assist, sizeof(out->aim_assist));
}

struct s_data_array;
union real_euler_angles2d;
union real_vector3d;
// @address 0xc1a90 ?euler_angles2d_from_vector3d@@YAPATreal_euler_angles2d@@PAT1@PBTreal_vector3d@@@Z

// @address 0x216d90 player_control_set_action_from_unit
extern "C" void player_control_set_action_from_unit(long unit_index, control *action)
{
    player_control_clear_action(action);
    if (unit_index == -1)
        return;
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    euler_angles2d_from_vector3d(&action->look, &unit->unit.desired_aiming_vector);
    if (action->look.yaw < 0.0f)
        action->look.yaw = action->look.yaw + 6.2831855f;
    action->weapons = unit->unit.desired_weapon_set;
    action->grenade = unit->unit.desired_grenade_index;
    action->zoom = unit->unit.desired_zoom_level;
}

// @address 0x216e40 player_control_update_state_actions
extern "C" void player_control_update_state_actions(long user_index)
{
    s_player_control_user_state *user = &player_control_globals->users[user_index];
    if (user->unit_index != -1) {
        for (long i = 0; i < 4; i++) {
            s_player_control_state *state = &player_control_globals->states[i];
            if (state->unit_index == user->unit_index)
                memcpy(&state->action, &user->action, sizeof(user->action));
        }
    }
}

// @address 0x216f68 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_control_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_control_globals_allocator>::deallocate(void *address)
{
    int value = m_member_index;
    player_control_globals = 0;
    restricted_region_free_member(3, value);
}

class c_window_manager;

// @address 0x216970 player_control_is_ui_blocking_input
extern "C" bool player_control_is_ui_blocking_input(e_controller_index controller_index, s_player_control_input *input)
{
    c_gui_screen_scoreboard *scoreboard = c_gui_screen_scoreboard::get_scoreboard_screen(controller_index);
    bool suppress = scoreboard && gui_screen_scoreboard_is_interactive(scoreboard);
    if (suppress && input)
    {
        input->primary_trigger = 0.0f;
        input->secondary_trigger = 0.0f;
        memset(&input->yaw_delta, 0, sizeof(input->yaw_delta) + sizeof(input->pitch_delta));
        input->vehicle_pitch = 0.0f;
        input->control_flags = 0;
        input->button_flags = 0;
        input->action_flags &= 0x20001;
        long *actions = input->suppressed_actions;
        for (long index = 0; index < 9; ++index)
            actions[index] = 0;
        actions[1] = -1;
        actions[2] = -1;
        actions[0] = -1;
    }
    return window_manager_is_controller_input_suppressed(&g_window_manager, controller_index);
}

// 0x214700: true when anything (director, scoreboard, ui, player control globals, cinematics) blocks this user's input

struct s_game_input_state;
// @address 0x214700 player_control_is_input_inhibited
extern "C" unsigned char player_control_is_input_inhibited(
    long user_index, e_controller_index controller_index, s_game_input_state *input_states, s_player_control_input *input)
{
    unsigned char inhibited = tls_director_globals()->directors[user_index].get_director()->inhibits_input();
    inhibited |= player_control_is_ui_blocking_input(controller_index, input);
    inhibited |= main_has_pending_state_changes();
    bool globals_inhibited = false;
    s_campaing_metagame_globals *globals = g_campaing_metagame_globals;
    if (globals)
    {
        long flags = globals->flags;
        if ((flags & (1 << 1)) != 0)
            globals_inhibited = (flags & (1 << 3)) != 0;
    }
    return players_globals->suppressed | globals_inhibited | inhibited;
}

static __forceinline long player_index_from_user_index_inline(long user_index)
{
    long player_index = -1;
    if (user_index != -1)
        player_index = player_mapping_globals->player_user_mapping[user_index];
    return player_index;
}
static __forceinline void player_control_simulation_process_input(unsigned long mask, const player_action *actions, unsigned long flags)
{
    if (!g_simulation_globals.aborted && g_simulation_globals.world->exists())
        g_simulation_globals.world->process_input(mask, actions, flags);
}
static __forceinline void player_control_main_time_mark_input_collection_time()
{
    __int64 milliseconds = GetTickCount();
    milliseconds |= g_main_time_globals->milliseconds & 0xffffffff00000000ui64;
    if (milliseconds < g_main_time_globals->milliseconds)
        milliseconds += 1i64 << 32;
    g_main_time_globals->input_collection_milliseconds = milliseconds;
}

// ---- player_control_get_controller_input ----
#define PLAYER_CONTROL_SET_FLAG(flags, bit, value) ((value) ? ((flags) |= (1 << (bit))) : ((flags) &= ~(1 << (bit))))

static __forceinline unit_seat *player_control_unit_seat_get(const unit_datum *unit)
{
    unit_datum *parent = ((unit_datum *)object_get(unit->object.parent_object_index));
    unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
    return &definition->seats.begin()[unit->unit.parent_seat_index];
}

static __forceinline long player_control_unit_get_current_primary_weapon(unit_datum *unit)
{
    long result = -1;
    if (unit->unit.current_weapon_indices[0] != -1)
        result = unit->unit.weapon_object_indices[unit->unit.current_weapon_indices[0]];
    return result;
}

static __forceinline long player_control_unit_get_primary_weapon(const long &unit_index)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    return unit_inventory_get_weapon_or_none(unit_index, unit->unit.current_weapon_indices[0]);
}

static __forceinline bool player_control_object_is_biped(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    long type_mask = 1;
    return ((1 << header->object_type) & type_mask) != 0;
}

static __forceinline bool player_control_unit_is_dual_wielding(unit_datum *unit)
{
    bool result = false;
    if (unit->unit.current_weapon_indices[0] != -1 && unit->unit.current_weapon_indices[1] != -1)
        result = true;
    return result;
}

static __forceinline bool player_control_unit_is_hard_pinging(unit_datum *unit)
{
    bool result = false;
    c_animation_manager *animation = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
    if (flag_test((unsigned long)animation->m_flags, 6L) && animation->m_current_goal.state == 0x94)
        result = true;
    return result;
}

static __forceinline bool player_control_object_get_early_mover_transform(long object_index, real_matrix4x3 *transform)
{
    long mover_index = object_early_mover_local_space_index(object_index);
    bool result = false;
    if (mover_index != -1 && g_object_early_movers_globals->movers[mover_index].valid1)
    {
        memcpy(transform, &g_object_early_movers_globals->movers[mover_index].transform, sizeof(*transform));
        result = true;
    }
    return result;
}

static __forceinline bool player_control_look_allowed()
{
    bool input_enabled = !PLAYER_CONTROL_TEST_BIT(player_control_globals->input_disabled_flags, 0);
    return input_enabled && !game_time_is_paused();
}

static __forceinline long object_get_ultimate_parent_unchecked_inline(long object_index)
{
    long ultimate_parent_index;
    do
    {
        unit_datum *object = ((unit_datum *)object_get(object_index));
        ultimate_parent_index = object_index;
        object_index = object->object.parent_object_index;
    }
    while (object_index != -1);
    return ultimate_parent_index;
}

static __forceinline bool player_control_unit_is_airborne(long unit_index)
{
    if (unit_index != -1)
    {
        if (player_control_object_is_biped(unit_index))
        {
            if (motor_system_in_airborne_state(unit_index))
                return true;
        }
    }
    return false;
}

static inline s_player_control_user_offset player_control_get_input_user_offset(long input_user_index)
{
    s_player_control_user_offset offset;
    offset.value = input_user_index * sizeof(s_player_control_user_state);
    return offset;
}

static __forceinline s_player_control_user_state *player_control_get_input_user_state(long input_user_index)
{
    ASSERT(input_user_index >= 0 && input_user_index < 4);
    s_player_control_user_offset user_offset = player_control_get_input_user_offset(input_user_index);
    return (s_player_control_user_state *)((char *)((s_player_control_globals *)player_control_globals)->users + user_offset.value);
}

// @address 0x211ea8 ?player_control_get_controller_input@@YAXJW4e_controller_index@@MMPAPAUs_game_input_state@@PAUs_player_control_input@@@Z
void player_control_get_controller_input(long input_user_index, e_controller_index controller_index, float world_seconds_elapsed,
    float game_seconds_elapsed, s_game_input_state **input_states, s_player_control_input *input)
{
    s_game_globals *const &game_globals = scenario_get_game_globals();
    s_player_control_constants *constants = game_globals->player_control.begin();
    s_player_control_user_state *state = player_control_get_input_user_state(input_user_index);
    long player_index = -1;
    if (input_user_index != -1)
        player_index = player_mapping_globals->player_user_mapping[input_user_index];
    s_game_input_state *input_state = NULL;
    bool input_state_valid = false;

    memset(input, 0, sizeof(*input));
    player_control_aim_assist_clear(&input->aim_assist);

    if (controller_index >= 0 && controller_index < 4)
    {
        input_state = &input_abstraction_globals.input_states[controller_index];
        *input_states = input_state;
        input_state_valid = true;
    }
    if (!input_state_valid)
        return;

    bool crouch_lock_enabled = g_controller_globals.controllers[controller_index].get_player_profile_interface()->get_controller_auto_center_look();

    bool look_override = false;
    if (player_index != -1)
        look_override = datum_get<player_datum>(player_data, player_index)->look_override;
    if (look_override)
    {
        state->action.byte = true;
        state->action.extra.x = input_state->look.i;
        state->action.extra.y = input_state->look.j;
    }
    else
    {
        state->action.byte = false;
    }

    if (input_state->get_button(_input_action_unknown24).down_frames())
        input->control_flags |= 0x40;
    else
        input->control_flags &= ~0x40;
    if (input_state->get_button(_input_action_unknown25).down_frames())
        input->control_flags |= 0x80;
    else
        input->control_flags &= ~0x80;

    float yaw_speed = input_abstraction_globals.look_settings[controller_index].preferences.horizontal_look_sensitivity * 0.017453292f;
    float pitch_speed = input_abstraction_globals.look_settings[controller_index].preferences.vertical_look_sensitivity * 0.017453292f;
    if (state->unit_index != -1)
    {
        unit_datum *unit = ((unit_datum *)object_get(state->unit_index));
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
        {
            unit_datum *parent = ((unit_datum *)object_get(unit->object.parent_object_index));
            unit_seat *seat =
                &((unit_definition *)tag_get(0, parent->object.definition_index))->seats.begin()[unit->unit.parent_seat_index];
            float t = 0.0f;
            float range = seat->speed_reference_bounds[1] - seat->speed_reference_bounds[0];
            if (range > 0.0f)
            {
                real_vector3d *velocity = &parent->object.translational_velocity;
                float speed = sqrtf(velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k);
                t = (speed - seat->speed_reference_bounds[0]) / range;
                t = PIN(t, 0.0f, 1.0f);
            }
            if (seat->speed_exponent != 0.0f)
                t = (float)pow(t, seat->speed_exponent);
            if (seat->yaw_rate_bounds[0] > 0.0f || seat->yaw_rate_bounds[1] > 0.0f)
                yaw_speed = ((seat->yaw_rate_bounds[1] - seat->yaw_rate_bounds[0]) * t + seat->yaw_rate_bounds[0]) * 0.017453292f;
            if (seat->pitch_rate_bounds[0] > 0.0f || seat->pitch_rate_bounds[1] > 0.0f)
                pitch_speed = ((seat->pitch_rate_bounds[1] - seat->pitch_rate_bounds[0]) * t + seat->pitch_rate_bounds[0]) * 0.017453292f;
        }

        float throttling = pitch_speed;
        for (long weapon_slot = 0; weapon_slot < 2; weapon_slot++)
        {
            unit_datum *unit = ((unit_datum *)object_get(state->unit_index));
            signed char weapon = unit->unit.current_weapon_indices[weapon_slot];
            if (weapon != -1 && unit->unit.weapon_object_indices[weapon] != -1)
            {
                float weapon_throttling = weapon_get_pitch_throttling(unit->unit.weapon_object_indices[weapon]);
                if (throttling > weapon_throttling)
                    throttling = weapon_throttling;
            }
        }
        if (pitch_speed > throttling)
            pitch_speed = throttling;
        if (yaw_speed > throttling * 2.0f)
            yaw_speed = throttling * 2.0f;
    }

    input->throttle.x = input_state->throttle.i;
    input->throttle.y = input_state->throttle.j;
    if (state->unit_index != -1)
    {
        long weapon_index = player_control_unit_get_current_primary_weapon(((unit_datum *)object_get(state->unit_index)));
        if (weapon_index != -1)
        {
            input->throttle.x = (1.0f - weapon_compute_movement_penalty(weapon_index, true, state->action.zoom != -1)) * input->throttle.x;
            input->throttle.y = (1.0f - weapon_compute_movement_penalty(weapon_index, false, state->action.zoom != -1)) * input->throttle.y;
        }
    }

    bool look_allowed = player_control_look_allowed();
    if (look_allowed)
    {
        float yaw_rate;
        float pitch_rate;
        float look_pitch = user_currently_piloting_aircraft(input_user_index) ? input_state->aircraft_pitch : input_state->look.j;
        if (input_state->raw_look)
        {
            float yaw;
            float pitch;
            if (world_seconds_elapsed > 0.0f)
            {
                float inverse = 1.0f / world_seconds_elapsed;
                yaw = input_state->look.i * inverse;
                pitch = inverse * look_pitch;
            }
            else
            {
                pitch = 0.0f;
                yaw = 0.0f;
            }
            yaw_rate = yaw * yaw_speed * 0.47746482f;
            pitch_rate = pitch * pitch_speed * 0.95492965f;
        }
        else
        {
            float scale;
            float abs_pitch = fabsf(look_pitch);
            float abs_yaw = fabsf(input_state->look.i);
            if (abs_pitch > 0.1f && abs_yaw > 0.1f)
            {
                vector2 corner;
                corner.i = 0.0f;
                corner.j = 0.0f;
                if (abs_pitch > abs_yaw)
                {
                    corner.j = abs_yaw / abs_pitch;
                    corner.i = 1.0f;
                }
                else
                {
                    corner.i = abs_pitch / abs_yaw;
                    corner.j = 1.0f;
                }
                scale = magnitude2d(&corner);
            }
            else
            {
                scale = 1.0f;
            }
            float yaw = PIN(input_state->look.i * scale, -1.0f, 1.0f);
            float pitch = PIN(scale * look_pitch, -1.0f, 1.0f);
            ASSERT(constants->look_function.count > 1);
            yaw_rate = evaluate_piecewise_linear_function((short)constants->look_function.count,
                (float *)constants->look_function.address, yaw) * yaw_speed;
            pitch_rate = evaluate_piecewise_linear_function((short)constants->look_function.count,
                (float *)constants->look_function.address, pitch) * pitch_speed;
            ASSERT(constants->look_yaw_acceleration_time > 0.0f);
            if (fabsf(yaw) >= constants->look_acceleration_threshold)
            {
                float t = state->yaw_acceleration_time / constants->look_yaw_acceleration_time;
                yaw_rate *= 1.0f + PIN(t, 0.0f, 1.0f) * (constants->look_yaw_acceleration_scale - 1.0f);
                state->yaw_acceleration_time += world_seconds_elapsed;
            }
            else
            {
                state->yaw_acceleration_time = 0.0f;
            }
            ASSERT(constants->look_pitch_acceleration_time > 0.0f);
            if (fabsf(pitch) >= constants->look_acceleration_threshold)
            {
                float t = state->pitch_acceleration_time / constants->look_pitch_acceleration_time;
                pitch_rate *= 1.0f + PIN(t, 0.0f, 1.0f) * (constants->look_pitch_acceleration_scale - 1.0f);
                state->pitch_acceleration_time += world_seconds_elapsed;
            }
            else
            {
                state->pitch_acceleration_time = 0.0f;
            }
        }

        if (state->unit_index != -1 && state->action.zoom != -1)
        {
            float inverse = 1.0f / unit_get_zoom_magnification(state->unit_index, state->action.zoom);
            yaw_rate = inverse * yaw_rate;
            pitch_rate = inverse * pitch_rate;
        }
        if (state->unit_index != -1)
        {
            unit_datum *unit = ((unit_datum *)object_get(state->unit_index));
            float factor = 1.0f - unit->unit.look_difficulty_scale * global_game_globals->player_information.begin()->stun_turning_penalty;
            yaw_rate = factor * yaw_rate;
            pitch_rate = factor * pitch_rate;
        }

        {
            c_aim_assist_flags aim_assist_flags;
            aim_assist_flags.clear();
            if ((::game_globals->options.game_mode == 2))
                aim_assist_flags.set(_aim_assist_multiplayer_bit, true);
            real_vector3d aiming_vector;
            s_player_control_aim_assist_angles aim_angles;
            player_build_aiming_vector_from_facing(player_index, &state->action.look, &aiming_vector);
            aim_assist_compute_control(input_user_index, aim_assist_flags, &aiming_vector, state->action.zoom,
                (real_euler_angles2d *)&aim_angles, &input->aim_assist);
            if ((::game_globals->options.game_mode == 2) && input->aim_assist.target_object_index == -1)
            {
                player_control_aim_assist_clear(&input->aim_assist);
                s_aim_assist_targeting_result secondary;
                s_player_control_aim_assist_angles secondary_angles;
                aim_assist_flags.set(_aim_assist_secondary_bit, true);
                aim_assist_compute_control(input_user_index, aim_assist_flags, &aiming_vector, state->action.zoom,
                    (real_euler_angles2d *)&secondary_angles, &secondary);
                if (secondary.target_object_index != -1)
                {
                    input->aim_assist.target_object_index = secondary.target_object_index;
                    input->aim_assist.model_index = secondary.model_index;
                    input->aim_assist.flags = secondary.flags;
                    input->aim_assist.lead_vector = secondary.lead_vector;
                }
            }

            bool aim_assist_active = aim_angles.scale > 0.0f;
            state->aim_assist_valid = aim_assist_active;
            if (aim_assist_active && world_seconds_elapsed > 0.0f &&
                (fabsf(yaw_rate) > 0.0001f || fabsf(pitch_rate) > 0.0001f || fabsf(input->throttle.x) > 0.0001f || fabsf(input->throttle.y) > 0.0001f))
            {
                float time_scale = game_seconds_elapsed / world_seconds_elapsed;
                float magnetism = 1.0f - PIN(constants->magnetism_scale, 0.0f, 1.0f) * aim_angles.scale;
                float limit = PIN(constants->magnetism_limit, 0.0f, 1.0f) * aim_angles.scale;
                float yaw_delta = aim_angles.yaw * time_scale;
                float pitch_delta = aim_angles.pitch * time_scale;
                yaw_delta = PIN(yaw_delta, -3.1415927f, 3.1415927f);
                pitch_delta = PIN(pitch_delta, -1.5707964f, 1.5707964f);
                yaw_rate = yaw_delta * limit + magnetism * yaw_rate;
                pitch_rate = pitch_delta * limit + magnetism * pitch_rate;
            }
        }

        input->yaw_delta = yaw_rate * world_seconds_elapsed;
        input->pitch_delta = pitch_rate * world_seconds_elapsed;
        input->vehicle_pitch = input_state->vehicle_pitch;

        long unit_index = state->unit_index;
        if (unit_index != -1)
        {
            s_object_header *header = object_header_get(unit_index);
            if ((1 << header->object_type) & 1)
            {
                unit_datum *unit = (unit_datum *)header->object;
                long physics_object = object_accepted_local_physics_object_get(object_get_ultimate_parent_unchecked_inline(unit_index));
                real_matrix4x3 transform;
                if (physics_object != -1 && player_control_object_get_early_mover_transform(
                        object_get_ultimate_parent_unchecked_inline(unit_index), &transform))
                {
                    if (physics_object == state->early_mover_object)
                    {
                        real_matrix4x3 inverse;
                        real_matrix4x3 delta;
                        real_euler_angles2d new_angles;
                        real_vector3d *forward = &unit->object.forward;
                        matrix4x3_inverse(&state->early_mover_transform, &inverse);
                        matrix4x3_compute_product(&transform, &inverse, &delta);
                        {
                            real_vector3d facing;
                            matrix4x3_transform_vector(&delta, forward, &facing);
                            euler_angles2d_from_vector3d(&new_angles, &facing);
                        }
                        {
                            real_euler_angles2d old_angles;
                            euler_angles2d_from_vector3d(&old_angles, forward);
                            input->yaw_delta = (new_angles.yaw - old_angles.yaw) + input->yaw_delta;
                            input->pitch_delta = (new_angles.pitch - old_angles.pitch) + input->pitch_delta;
                        }
                    }
                    memcpy(&state->early_mover_transform, &transform, sizeof(transform));
                    state->early_mover_object = physics_object;
                }
                else
                {
                    state->early_mover_object = -1;
                }
            }
        }
    }
    else
    {
        input->yaw_delta = 0.0f;
        input->pitch_delta = 0.0f;
        input->vehicle_pitch = 0.0f;
    }

    unsigned char crouch_input_suppressed = player_control_is_input_inhibited(input_user_index, controller_index, 0, NULL);
    bool airborne = player_control_unit_is_airborne(state->unit_index);
    bool weapon_prevents_crouching = false;
    if (state->unit_index != -1)
    {
        long weapon_index = player_control_unit_get_current_primary_weapon(((unit_datum *)object_get(state->unit_index)));
        if (weapon_index != -1)
            weapon_prevents_crouching =
                (((weapon_definition *)tag_get(0, ((unit_datum *)object_get(weapon_index))->object.definition_index))
                        ->flags >>
                    18) &
                1;
    }
    if (!crouch_input_suppressed)
    {
        if (weapon_prevents_crouching)
        {
            state->crouch_locked = false;
        }
        else if (!crouch_lock_enabled || airborne)
        {
            if (input_state->get_button(_input_action_crouch).down_frames() == 1 &&
                (airborne || input->throttle.y * input->throttle.y + input->throttle.x * input->throttle.x < 0.25f))
                state->crouch_locked = true;
            if (!input_state->get_button(_input_action_crouch).down_frames())
                state->crouch_locked = false;
        }
        else
        {
            if (input_state->get_button(_input_action_crouch).down_frames() == 1 &&
                (state->crouch_locked || input->throttle.y * input->throttle.y + input->throttle.x * input->throttle.x < 0.25f))
                state->crouch_locked = !state->crouch_locked;
            if (!input_state->get_button(_input_action_crouch).down_frames() && state->crouch_frames >= 15)
                state->crouch_locked = false;
            if (input_state->get_button(_input_action_crouch).down_frames())
#if !defined(_XBOX) || defined(__clang__)
            {
                if (input_state->get_button(_input_action_crouch).down_frames() == 1 || input_abstraction_input_frame_advanced())
                    state->crouch_frames++;
            }
#else
                state->crouch_frames++;
#endif
            else
                state->crouch_frames = 0;
        }
    }
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 0, state->crouch_locked);

    bool zoom = input_state->get_button(_input_action_zoom).get_down_frames() == 1;
    if (zoom && state->action.zoom == -1)
    {
        long weapon_index = state->unit_index != -1 ? player_control_unit_get_current_primary_weapon(((unit_datum *)object_get(state->unit_index))) : -1;
        if (state->unit_index != -1)
        {
            unit_datum *unit = ((unit_datum *)object_get(state->unit_index));
            if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1 &&
                flag_test((unsigned long)((unit_definition *)tag_get(0,
                    ((unit_datum *)object_get(unit->object.parent_object_index))->object.definition_index))
                    ->seats.begin()[unit->unit.parent_seat_index].flags, 3L))
            {
                weapon_index = player_control_unit_get_primary_weapon(unit->object.parent_object_index);
            }
        }
        if (input_state->buttons[0].down_amount() > 127.0f || input_state->buttons[1].down_amount() > 127.0f)
            zoom = false;
        if (weapon_index != -1 && weapon_rotate_zoom_level(weapon_index, -1) == -1)
        {
            float deadzone;
            if (global_game_globals->player_control.begin()->crouch_look_deadzone == 0.0f)
                deadzone = 0.8f;
            else
                deadzone = global_game_globals->player_control.begin()->crouch_look_deadzone;
            float yaw = input_state->look.i;
            float pitch = input_state->look.j;
            if (yaw * yaw + pitch * pitch > deadzone * deadzone)
                zoom = false;
        }
    }
    PLAYER_CONTROL_SET_FLAG(input->control_flags, 2, zoom);
    PLAYER_CONTROL_SET_FLAG(input->control_flags, 3, input_state->get_button(_input_action_zoom).get_down_frames());

    bool swap_triggers = false;
    if (state->unit_index != -1 && player_control_unit_is_dual_wielding(((unit_datum *)object_get(state->unit_index))))
    {
        c_player_profile_interface *profile = g_controller_globals.controllers[controller_index].get_player_profile_interface();
        if (profile && profile->get_button_preset() == 1)
            swap_triggers = true;
    }
    input->primary_trigger = input_state->get_button(swap_triggers ? _input_action_left_trigger : _input_action_right_trigger).get_down_amount();
    input->secondary_trigger = input_state->get_button(swap_triggers ? _input_action_right_trigger : _input_action_left_trigger).get_down_amount();

    if (state->unit_index != -1 && player_control_unit_is_dual_wielding(((unit_datum *)object_get(state->unit_index))) &&
        !(player_index != -1 && game_engine_is_player_in_edit_mode(player_index, NULL, NULL)))
    {
        input_state->get_button(_input_action_left_trigger).unlock_to_access();
    }
    else
    {
        input_state->get_button(_input_action_left_trigger).lock();
    }

    if (!game_is_cooperative())
    {
        c_abstract_button &skip = input_state->get_button(_input_action_skip_cinematic);
        if (skip.access_valid())
        {
            bool skip_allowed = false;
            if (cinematic_globals)
                skip_allowed = cinematic_globals->in_progress;
            if (skip_allowed && skip.down_frames())
            {
                if (main_loading_get_action())
                {
                    input->button_flags |= 0x1000;
                    skip.set_unlocked();
                }
            }
            else
            {
                skip.set_locked();
            }
        }
        if (players_globals->suppressed)
        {
            c_abstract_button &button = input_state->get_button(_input_action_unknown19);
            if (button.down_frames())
            {
                button.latch();
                input->button_flags |= 0x2000;
            }
        }
    }

    bool in_seat = false;
    if (state->unit_index != -1)
    {
        unit_datum *unit = ((unit_datum *)object_get(state->unit_index));
        if (unit->unit.parent_seat_index != -1 && unit->object.parent_object_index != -1)
            in_seat = true;
    }
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 18,
        input_state->get_button(swap_triggers ? _input_action_left_trigger : _input_action_right_trigger).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 23,
        input_state->get_button(swap_triggers ? _input_action_right_trigger : _input_action_left_trigger).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 19,
        input_state->get_button(swap_triggers ? _input_action_right_trigger : _input_action_left_trigger).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 11, input_state->get_button(_input_action_left_trigger).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 14, in_seat ? input_state->get_button(_input_action_left_trigger).down_frames() : false);
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 31, in_seat ? input_state->get_button(_input_action_melee).down_frames() : false);
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 28, input_state->get_button(_input_action_pick_up_left).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 7, input_state->get_button(_input_action_unknown15).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 8, input_state->get_button(_input_action_unknown16).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 2, input_state->get_button(_input_action_unknown17).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 22, input_state->get_button(_input_action_unknown17).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 27, input_state->get_button(_input_action_unknown17).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 1, input_state->get_button(_input_action_jump).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 4, input_state->get_button(_input_action_reload_left).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 5, input_state->get_button(_input_action_use).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->action_flags, 6, input_state->get_button(_input_action_right_trigger).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->control_flags, 4, input_state->get_button(_input_action_unknown18).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->control_flags, 5, input_state->get_button(_input_action_unknown19).get_down_frames());

    if (input_state->get_button(_input_action_action).get_down_frames() &&
        input_state->get_button(_input_action_reload_right).down_frames() &&
        input_state->get_button(_input_action_crouch).down_frames() &&
        input_state->get_button(_input_action_jump).down_frames())
    {
#if !defined(_XBOX) || defined(__clang__)
        // Only fire on the pass that reaches 90 input frames.
        const long first_frames = input_abstraction_button_down_input_frames(controller_index, input_state->get_button(_input_action_unknown20));
        const long second_frames = input_abstraction_button_down_input_frames(controller_index, input_state->get_button(_input_action_unknown21));
        if (first_frames == 90 && input_abstraction_input_frame_advanced())
            input->control_flags |= 0x100;
        if (second_frames == 90 && input_abstraction_input_frame_advanced())
            input->control_flags |= 0x200;
#else
        long first_frames = input_state->get_button(_input_action_unknown20).down_frames();
        long second_frames = input_state->get_button(_input_action_unknown21).down_frames();
        if (first_frames == 90)
            input->control_flags |= 0x100;
        if (second_frames == 90)
            input->control_flags |= 0x200;
#endif
    }

    bool switch_grenade = input_state->get_button(_input_action_switch_grenade).get_down_frames() > 0;
    PLAYER_CONTROL_SET_FLAG(input->control_flags, 1, !switch_grenade && state->switch_grenade_held);
    state->switch_grenade_held = switch_grenade;

    if (crouch_lock_enabled && state->crouch_locked)
    {
        if (PLAYER_CONTROL_TEST_BIT(input->action_flags, 1))
            state->crouch_locked = false;
        long unit_index = state->unit_index;
        if (unit_index != -1)
        {
            if (unit_animation_is_running_damage_overlay(unit_index) || player_control_unit_is_hard_pinging(((unit_datum *)object_get(unit_index))))
                state->crouch_locked = false;
            if (PLAYER_CONTROL_TEST_BIT(input->action_flags, 6))
            {
                long weapon_index = player_control_unit_get_current_primary_weapon(((unit_datum *)object_get(unit_index)));
                if (weapon_index != -1 && weapon_is_melee_only(weapon_index))
                    state->crouch_locked = false;
            }
        }
    }

    long minimum_hold_msec = (long)(constants->minimum_hold_time * 1000.0f);
    PLAYER_CONTROL_SET_FLAG(input->button_flags, 0, input_state->get_button(_input_action_action).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->button_flags, 1, input_state->get_button(_input_action_action).get_down_msec() >= minimum_hold_msec);
    PLAYER_CONTROL_SET_FLAG(input->button_flags, 4, input_state->get_button(_input_action_reload_right).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->button_flags, 5, input_state->get_button(_input_action_reload_right).get_down_msec() >= minimum_hold_msec);
    PLAYER_CONTROL_SET_FLAG(input->button_flags, 8, input_state->get_button(_input_action_switch_weapon).get_down_frames());
    PLAYER_CONTROL_SET_FLAG(input->button_flags, 9, input_state->get_button(_input_action_switch_weapon).get_down_msec() >= minimum_hold_msec);
    if (input_state->get_button(_input_action_switch_weapon).get_down_frames())
    {
        state->switch_weapon_held = true;
    }
    else
    {
        if (state->switch_weapon_held && !state->switch_weapon_consumed)
            input->control_flags |= 1;
        state->switch_weapon_held = false;
        state->switch_weapon_consumed = false;
    }

    player_control_update_input_flags((s_game_input_state *)input_state, input);
}

// @address 0x98900 ?player_control_update@@YAXMM@Z
void player_control_update(float world_seconds_elapsed, float game_seconds_elapsed)
{
    unsigned long user_mask = 0;
    player_action actions[4];
    for (long user_index = 0; user_index < 4; user_index++) {
        long player_index = player_index_from_user_index_inline(user_index);
        if (player_index != -1) {
            player_control_update_player(player_index, user_index,
                player_mapping_globals->player_input_controller_mapping[datum_index_to_absolute_index(player_index)],
                world_seconds_elapsed, game_seconds_elapsed);
            player_action *action = &actions[user_index];
            s_player_control_user_state *user = &player_control_globals->users[user_index];
            player_control_build_action(&user->action, action);
            if (user->unit_index != -1) {
                action->unit_valid = 1;
                action->unit_flags = ((unit_datum *)object_get(user->unit_index))->unit.unit_input_flags;
            }
            player_control_update_state_actions(user_index);
            user_mask |= 1 << user_index;
        }
    }
    player_control_simulation_process_input(user_mask, actions, player_control_globals->input_flags);
    player_control_main_time_mark_input_collection_time();
    player_control_update_debug_toggles();
}

// @address 0x7614dc k_player_control_full_turn
// @address 0x7614e0 k_player_control_half_turn
float k_player_control_full_turn = 6.28318530717958647692f;
float k_player_control_half_turn = 3.14159265358979323846f;
static __forceinline float player_control_signed_angular_difference(float a, float b)
{
    float difference = b - a;
    if (difference >= k_player_control_half_turn)
        difference -= k_player_control_full_turn;
    if (difference <= -k_player_control_half_turn)
        difference += k_player_control_full_turn;
    return difference;
}
static __forceinline unit_seat *player_control_desired_seat_get(long index, short seat)
{
    object_datum *object = object_get(index);
    unit_definition *definition =
        (unit_definition *)tag_get(0, object->definition_index);
    return &definition->seats.begin()[seat];
}

static __forceinline void interpolate_scalar_inline(float *value, float target, float rate)
{
    float difference = target - *value;
    float lower_bound = -rate;
    *value = MIN(MAX(difference, lower_bound), rate) + *value;
}

// @address 0x215568 ?player_control_modify_desired_angles@@YAXJMMMM@Z
void player_control_modify_desired_angles(long user_index, float yaw_delta, float pitch_delta, float frame_time, float unused)
{
    s_player_control_user_state *state = (s_player_control_user_state *)&player_control_globals->users[user_index];
    s_player_control_constants *tuning = global_game_globals->player_control.begin();
    float lower = -1.492256522f, upper = 1.492256522f;
    s_unit_camera_info camera;
    real_point3d position;
    real_vector3d axis;
    unit_get_camera_info(state->unit_index, &camera);
    const float two_pi = 6.283185482f;
    if (state->active) {
        position = camera.position;
        unit_get_camera_position(camera.unit_index, &position);
        real_vector3d direction;
        vector_from_points3d(&position, &state->position, &direction);
        if (fabs(normalize3d(&direction)) >= 0.0001f) {
            real_euler_angles2d target;
            euler_angles2d_from_vector3d(&target, &direction);
            float yaw = player_control_signed_angular_difference(state->action.look.yaw, target.yaw);
            float pitch = player_control_signed_angular_difference(state->action.look.pitch, target.pitch);
            float yaw_abs = fabsf(yaw), pitch_abs = fabsf(pitch);
            float speed = state->angle * 0.75f, base = state->angle * 0.25f;
            float yaw_speed = (yaw_abs * 0.318309873f) * speed + base;
            float pitch_speed = (pitch_abs * 0.318309873f) * speed + base;
            if (yaw_abs > 1.57079637f) pitch_speed = 0.0f;
            else if (yaw_abs > 0.785398185f) {
                float scale = (yaw_abs-0.785398185f)*1.2732395f;
                scale = PIN(scale, 0.0f, 1.0f);
                pitch_speed = scale * pitch_speed;
            }
            float tick = MAX(game_time_globals->tick_length, k_real_epsilon);
            float inverse_tick = 1.0f / tick;
            float yaw_step = inverse_tick * yaw_abs;
            float pitch_step = inverse_tick * pitch_abs;
            if (yaw_step > yaw_speed) yaw_step = yaw_speed;
            if (pitch_step > pitch_speed) pitch_step = pitch_speed;
            float yaw_change = yaw_step * tick;
            float pitch_change = pitch_step * tick;
            pitch_delta = pitch > 0.0f ? pitch_change : -pitch_change;
            yaw_delta = yaw > 0.0f ? yaw_change : -yaw_change;
        } else {
            yaw_delta = 0.0f;
            pitch_delta = 0.0f;
        }
    }
    state->action.look.yaw += yaw_delta;
    if (camera.seat_index != -1) {
        unit_seat *seat = player_control_desired_seat_get(camera.unit_index, camera.seat_index);
        if (seat->left_aiming_angle != 0.0f || seat->right_aiming_angle != 0.0f) {
        real_matrix4x3 *nodes;
        object_datum *object = object_get(camera.unit_index);
        if (object->parent_object_index == -1 || object->parent_node_index == -1 || !(unsigned char)flag_test((unsigned long)seat->flags, 26L)) {
            long current = camera.unit_index, root = -1;
            while (current != -1) { root = current; current = object_get(current)->parent_object_index; }
            object = object_get(root);
            nodes = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
        } else {
            nodes = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
        }
        real_euler_angles2d orientation;
        euler_angles2d_from_vector3d(&orientation, &nodes->forward);
        float min_yaw = seat->left_aiming_angle + orientation.yaw, max_yaw = seat->right_aiming_angle + orientation.yaw;
        float range = player_control_signed_angular_difference(min_yaw, max_yaw);
        float from_max = player_control_signed_angular_difference(state->action.look.yaw, max_yaw);
        float from_min = player_control_signed_angular_difference(min_yaw, state->action.look.yaw);
        if (range < 0.0f) range += two_pi;
        if ((from_max < 0.0f || from_max >= range) && (from_min < 0.0f || from_min >= range)) {
            if (fabsf(from_min) < fabsf(from_max)) state->action.look.yaw = min_yaw;
            else state->action.look.yaw = max_yaw;
        }
    }
    }
    while (state->action.look.yaw < 0.0f) state->action.look.yaw += two_pi;
    while (state->action.look.yaw > two_pi) state->action.look.yaw -= two_pi;
    if (camera.camera) {
        float neutral = camera.camera->pitch_auto_level;
        if (camera.camera->pitch_range.upper != 0.0f || camera.camera->pitch_range.lower != 0.0f) {
            lower = camera.camera->pitch_range.lower;
            upper = camera.camera->pitch_range.upper;
            lower = PIN(lower, -1.492256522f, 1.492256522f);
            upper = PIN(upper, -1.492256522f, 1.492256522f);
        }
        if (neutral != 0.0f || state->autolevel) {
            float error = fabsf(state->action.look.pitch-neutral) * 0.63661975f;
            real_point3d origin; real_vector3d velocity, mover_velocity;
            object_get_origin(camera.unit_index, &origin);
            object_get_velocities(camera.unit_index, &velocity, 0);
            if (object_get_early_mover_local_space_velocity(camera.unit_index, &origin, &mover_velocity, &axis, false, false)) {
                velocity.i -= mover_velocity.i; velocity.j -= mover_velocity.j; velocity.k -= mover_velocity.k;
            }
            float magnitude = sqrtf((velocity.j*velocity.j + velocity.k*velocity.k) + velocity.i*velocity.i);
            magnitude = MIN(magnitude, 3.0f);
            magnitude *= frame_time;
            if (neutral != 0.0f) interpolate_scalar(&state->action.look.pitch, neutral, magnitude * error * 0.08f);
            else interpolate_scalar(&state->action.look.pitch, neutral, tuning->recenter_scale * magnitude * error);
        }
    }
    if (camera.weapon_index != -1) {
        object_datum *weapon = object_get(camera.weapon_index);
        if (weapon->definition_index != -1) {
            weapon_definition *definition =
                (weapon_definition *)tag_get(0, weapon->definition_index);
            if (definition->third_person_pitch_bounds.lower != 0.0f || definition->third_person_pitch_bounds.upper != 0.0f) {
                lower = MAX(lower, definition->third_person_pitch_bounds.lower*k_radians_per_degree);
                float maximum = definition->third_person_pitch_bounds.upper*k_radians_per_degree;
                if (upper > maximum) upper = maximum;
                lower = PIN(lower, -1.492256522f, 1.492256522f);
                upper = PIN(upper, -1.492256522f, 1.492256522f);
            }
        }
    }
    s_player_control_globals *globals = player_control_globals;
    if ((unsigned char)flag_test((unsigned long)globals->input_flags, 21L) || (unsigned char)flag_test((unsigned long)globals->input_flags, 22L)) {
        lower = globals->d*k_radians_per_degree;
        upper = globals->e*k_radians_per_degree;
        lower = PIN(lower, -1.492256522f, 1.492256522f);
        upper = PIN(upper, -1.492256522f, 1.492256522f);
    }
    float inverse_ticks = 0.001953125f;
    if (camera.seat_index != -1)
    {
        long ticks =
            (long)(player_control_desired_seat_get(camera.unit_index, camera.seat_index)->interpolation_ticks * game_time_globals->ticks_per_second +
                0.5f);
        if (ticks > 0)
            inverse_ticks = 1.0f / (float)ticks;
    }
    interpolate_scalar_inline(&state->minimum_pitch, lower, inverse_ticks * two_pi);
    interpolate_scalar_inline(&state->maximum_pitch, upper, inverse_ticks * two_pi);
    state->action.look.pitch = pitch_delta + state->action.look.pitch;
    state->action.look.pitch = PIN(state->action.look.pitch, state->minimum_pitch, state->maximum_pitch);
    if (pitch_delta == 0.0f && camera.camera) {
        float lower_soft = camera.camera->pitch_lower_margin + lower, upper_soft = upper - camera.camera->pitch_upper_margin;
        if (state->action.look.pitch < lower_soft) {
            float distance = fabsf(state->action.look.pitch-lower_soft)*0.63661975f;
            float rate = game_time_globals->tick_length*camera.camera->pitch_recenter_rate;
            interpolate_scalar(&state->action.look.pitch, lower_soft, distance*rate);
        }
        if (state->action.look.pitch > upper_soft) {
            float distance = fabsf(state->action.look.pitch-upper_soft)*0.63661975f;
            float rate = game_time_globals->tick_length*camera.camera->pitch_recenter_rate;
            interpolate_scalar(&state->action.look.pitch, upper_soft, distance*rate);
        }
    }
    if (camera.camera && !(camera.camera->flags & 1) && state->unit_index != camera.unit_index &&
        (camera.camera->pitch_range.upper != 0.0f || camera.camera->pitch_range.lower != 0.0f))
    {
        real_vector3d view;
        real_matrix4x3 basis;
        {
        real_vector3d forward, up;
        object_get_orientation(camera.unit_index, &forward, &up);
        vector3d_from_euler_angles2d(&view, &state->action.look);
        matrix4x3_rotation_from_vectors(&basis, &forward, &up);
        basis.position = *global_origin3d;
        }
        matrix4x3_inverse_transform_vector(&basis, &view, &view);
        real_euler_angles2d local;
        euler_angles2d_from_vector3d(&local, &view);
        if (local.pitch > camera.camera->pitch_range.upper || local.pitch < camera.camera->pitch_range.lower) {
            local.pitch = PIN(local.pitch, camera.camera->pitch_range.lower, camera.camera->pitch_range.upper);
            vector3d_from_euler_angles2d(&view, &local);
            matrix4x3_transform_vector(&basis, &view, &view);
            euler_angles2d_from_vector3d(&state->action.look, &view);
            if (state->action.look.yaw < 0.0f) state->action.look.yaw += two_pi;
            state->action.look.pitch = PIN(state->action.look.pitch, -1.492256522f, 1.492256522f);
        }
    }
    if (state->pitch_kick_ticks > 0) {
        state->action.look.pitch = state->pitch_kick + state->action.look.pitch;
        --state->pitch_kick_ticks;
        state->action.look.pitch = PIN(state->action.look.pitch, -1.492256522f, 1.492256522f);
    }
}

typedef float (c_abstract_button::*player_control_button_amount_member)();
player_control_button_amount_member player_control_down_amount_odr_use() { return &c_abstract_button::down_amount; }

static __forceinline short player_control_unit_get_grenade_count(long unit_index, short grenade_index)
{
    short count;
    if (grenade_index != -1)
        count = ((unit_datum *)object_get(unit_index))->unit.grenade_counts[grenade_index];
    else
        count = 0;
    return count;
}

static __forceinline bool player_control_unit_is_dual_wielding(long unit_index)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    bool dual_wielding = false;
    if (unit->unit.current_weapon_indices[0] != -1 && unit->unit.current_weapon_indices[1] != -1)
        dual_wielding = true;
    return dual_wielding;
}

static __forceinline bool player_control_controller_flight_inverted(long controller_index)
{
    bool flight_inverted = false;
    c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
    bool in_use = controller->is_signed_in_to_machine() || controller->is_unsigned_in_user();
    if (in_use)
        flight_inverted = controller->get_player_profile_interface()->get_controller_flight_inverted();
    return flight_inverted;
}

static __forceinline s_player_control_globals *player_control_globals_get()
{
    return player_control_globals;
}

static __forceinline void player_control_input_apply_scale(s_player_control_input *input)
{
    float scale = PIN(player_control_globals_get()->a, 0.0f, 1.0f);
    input->throttle.x *= scale;
    input->throttle.y *= scale;
    input->yaw_delta *= scale;
    input->pitch_delta *= scale;
    player_control_globals_get()->a = scale;
    if (scale < k_real_epsilon)
    {
        input->primary_trigger = 0.0f;
        input->secondary_trigger = 0.0f;
        input->action_flags = 0;
        input->control_flags = 0;
        input->button_flags &= 0x3000;
    }
}

#if !defined(_XBOX) || defined(__clang__)
static void player_control_clear_tap_latch(s_player_control_tap_latch *latch)
{
    latch->latched_flags = 0;
    latch->latched_buttons = 0;
    latch->latched_primary = 0.0f;
    latch->latched_secondary = 0.0f;
    latch->latched_seconds = 0.0f;
}

static void player_control_latch_trigger(float current, float previous, float &latched)
{
    if ((previous <= 0.0f || latched > 0.0f) && current > latched)
        latched = current;
}

static void player_control_latch_taps(long input_user_index, control *action, float world_seconds_elapsed)
{
    s_player_control_tap_latch *latch = &g_player_control_tap_latches[input_user_index];
    const long time = game_time_globals->time;
    const bool paused = game_time_is_paused();
    if (time != latch->time || paused)
    {
        // A tick consumed the previous action, or the pause menu owns input.
        player_control_clear_tap_latch(latch);
    }
    else if (latch->latched_flags || latch->latched_buttons || latch->latched_primary > 0.0f || latch->latched_secondary > 0.0f)
    {
        latch->latched_seconds += world_seconds_elapsed;
        if (latch->latched_seconds > 0.1f)
            player_control_clear_tap_latch(latch);
    }

    // Capture new presses from the raw action, then carry them to the next tick.
    if (latch->recorded && !paused)
    {
        latch->latched_flags |= action->flags & ~latch->flags;
        latch->latched_buttons |= action->action_flags & ~latch->buttons;
        player_control_latch_trigger(action->primary, latch->primary, latch->latched_primary);
        player_control_latch_trigger(action->secondary, latch->secondary, latch->latched_secondary);
    }
    latch->recorded = true;
    latch->time = time;
    latch->flags = action->flags;
    latch->buttons = action->action_flags;
    latch->primary = action->primary;
    latch->secondary = action->secondary;

    action->flags |= latch->latched_flags;
    action->action_flags |= latch->latched_buttons;
    if (latch->latched_primary > action->primary)
        action->primary = latch->latched_primary;
    if (latch->latched_secondary > action->secondary)
        action->secondary = latch->latched_secondary;
}
#endif

// @address 0x2147b0 player_control_update_player
extern "C" void player_control_update_player(
    long player_index, long input_user_index, long controller_index, float world_seconds_elapsed, float game_seconds_elapsed)
{
    s_game_input_state *input_states = NULL;
    s_player_control_constants *constants = global_game_globals->player_control.begin();
    s_player_control_user_state *user = &player_control_globals->users[input_user_index];
    s_player_control_input input;

    player_control_get_controller_input(
        input_user_index, (e_controller_index)controller_index, world_seconds_elapsed, game_seconds_elapsed, &input_states, &input);
    if (player_control_is_input_inhibited(input_user_index, (e_controller_index)controller_index, input_states, &input))
    {
        unsigned short button_flags = input.button_flags & 0x3000;
        bool primary_action = PLAYER_CONTROL_TEST_BIT(input.action_flags, 0);
        memset(&input, 0, sizeof(input));
        player_control_aim_assist_clear(&input.aim_assist);
        input.button_flags = button_flags;
        PLAYER_CONTROL_SET_FLAG(input.action_flags, 0, primary_action);
    }
    else if (player_control_globals_get()->enabled)
    {
        float tick_length = game_time_globals->tick_length;
        if (tick_length > k_real_epsilon)
        {
            float &current = player_control_globals_get()->a;
            float &time = player_control_globals_get()->c;
            if (time > tick_length)
            {
                current += (player_control_globals_get()->b - current) * (tick_length / time);
                time -= tick_length;
            }
            else
            {
                current = player_control_globals_get()->b;
                time = 0.0f;
            }
        }
        if (fabsf(player_control_globals_get()->a - 1.0f) < k_real_epsilon && fabsf(player_control_globals_get()->b - 1.0f) < k_real_epsilon)
            player_control_globals_get()->enabled = false;

        player_control_input_apply_scale(&input);
    }

    if (user->unit_index != -1)
    {
        unit_datum *unit = ((unit_datum *)object_get(user->unit_index));
        if (user->action.weapons.set_identifier != unit->unit.desired_weapon_set.set_identifier)
        {
            user->action.weapons = unit->unit.desired_weapon_set;
        }

        user->action.look_flags = 0;
        PLAYER_CONTROL_SET_FLAG(user->action.look_flags, 0, PLAYER_CONTROL_TEST_BIT(input.control_flags, 6));
        PLAYER_CONTROL_SET_FLAG(user->action.look_flags, 1, PLAYER_CONTROL_TEST_BIT(input.control_flags, 7));

        bool dual_wield = PLAYER_CONTROL_TEST_BIT(input.control_flags, 0);
        signed char weapon_index = user->action.weapons.weapon_indices[0];
        if (unit->unit.current_weapon_indices[1] != -1)
            dual_wield = false;
        if (weapon_index == -1)
            weapon_index = unit->unit.current_weapon_indices[0];
        if (dual_wield || weapon_index == -1)
            user->action.weapons.weapon_indices[0] = (signed char)unit_weapon_next_index(user->unit_index, -1, weapon_index, dual_wield);

        long unit_index = user->unit_index;
        short readied_weapon_index = unit_inventory_get_must_be_readied_weapon(unit_index);
        if (readied_weapon_index != -1 && unit->unit.current_weapon_indices[0] != readied_weapon_index)
        {
            user->action.weapons.weapon_indices[0] = (signed char)readied_weapon_index;
            user->action.weapons.weapon_indices[1] = -1;
        }
        if (user->action.weapons.weapon_indices[0] == user->action.weapons.weapon_indices[1] && user->action.weapons.weapon_indices[0] != -1)
            user->action.weapons.weapon_indices[1] = -1;

        if (user->action.grenade == -1 || player_control_unit_get_grenade_count(unit_index, user->action.grenade) == 0)
            user->action.grenade = unit->unit.desired_grenade_index;
        if ((input.control_flags & 2) != 0 || user->action.grenade == -1 || player_control_unit_get_grenade_count(unit_index, user->action.grenade) == 0)
        {
            if ((input.control_flags & 2) == 0 || !player_control_unit_is_dual_wielding(unit_index))
                user->action.grenade = unit_inventory_next_grenade(unit_index, user->action.grenade, 1);
        }

        bool cinematic_active = false;
        if (cinematic_globals)
            cinematic_active = cinematic_globals->in_progress;
        if (!cinematic_active &&
            tls_director_globals()->slots[input_user_index].perspective == 0 &&
            unit_index != -1 &&
            unit_can_zoom(unit_index))
        {
            if (PLAYER_CONTROL_TEST_BIT(input.control_flags, 2))
            {
                user->zoom_previous_level = user->action.zoom;
                user->action.zoom = unit_rotate_zoom_level(user->unit_index, user->action.zoom);
                if (user->action.zoom != -1)
                {
                    user->zoom_held = true;
                    user->zoom_held_ticks = 0;
                }
            }
            else if (PLAYER_CONTROL_TEST_BIT(input.control_flags, 3))
            {
#if !defined(_XBOX) || defined(__clang__)
                if (user->zoom_held && user->zoom_held_ticks < 127 && input_abstraction_input_frame_advanced())
#else
                if (user->zoom_held && user->zoom_held_ticks < 127)
#endif
                    user->zoom_held_ticks++;
            }
            else
            {
                if (user->zoom_held && user->zoom_held_ticks > 15)
                    user->action.zoom = user->zoom_previous_level;
                user->zoom_held = false;
            }
        }
        else
        {
            user->action.zoom = -1;
            user->zoom_held = false;
        }

        if (!tls_director_globals()->directors[input_user_index].get_director()->inhibits_facing())
            player_control_modify_desired_angles(input_user_index, input.yaw_delta, input.pitch_delta, world_seconds_elapsed, game_seconds_elapsed);

        if (unit->object.parent_object_index == -1)
        {
            bool flight_inverted = controller_index != -1 && player_control_controller_flight_inverted(controller_index);
            if (flight_inverted && fabsf(user->action.move.x) > 0.5f && input.pitch_delta < k_real_epsilon && !user->aim_assist_valid)
            {
                float autolevel_time = user->autolevel_time + world_seconds_elapsed;
                float maximum_time = game_time_globals->tick_length * (float)constants->minimum_autolevelling_ticks;
                autolevel_time = PIN(autolevel_time, 0.0f, maximum_time);
                user->autolevel_time = autolevel_time;
                user->autolevel = autolevel_time == maximum_time;
            }
            else
            {
                user->autolevel_time = 0.0f;
                user->autolevel = false;
            }
        }
        else
        {
            user->autolevel = false;
        }

        if (player_control_can_use_debug_controls())
        {
            if (PLAYER_CONTROL_TEST_BIT(input.control_flags, 8))
            {
                user->hud_toggled = !user->hud_toggled;
                user_interface_play_sound(_ui_sound_dialog, -1);
            }
            if (PLAYER_CONTROL_TEST_BIT(input.control_flags, 9))
            {
                player_control_globals_get()->thread_active = !player_control_globals_get()->thread_active;
                player_control_globals_get()->thread_enabled = false;
                player_control_globals_get()->thread_unknown58a = player_control_globals_get()->thread_active;
                user_interface_play_sound(_ui_sound_dialog, -1);
            }
            if (PLAYER_CONTROL_TEST_BIT(input.action_flags, 5) || PLAYER_CONTROL_TEST_BIT(input.action_flags, 6) ||
                PLAYER_CONTROL_TEST_BIT(input.action_flags, 9) || PLAYER_CONTROL_TEST_BIT(input.action_flags, 18) ||
                PLAYER_CONTROL_TEST_BIT(input.action_flags, 19) || PLAYER_CONTROL_TEST_BIT(input.action_flags, 23) ||
                PLAYER_CONTROL_TEST_BIT(input.action_flags, 24) || PLAYER_CONTROL_TEST_BIT(input.action_flags, 28))
            {
                user->hud_toggled = false;
            }
            if (user->hud_toggled)
                input.action_flags |= 0x4000;
        }
    }
    else
    {
        if (!tls_director_globals()->directors[input_user_index].get_director()->inhibits_facing())
            player_control_modify_desired_angles(input_user_index, input.yaw_delta, input.pitch_delta, world_seconds_elapsed, game_seconds_elapsed);
    }

    if (!game_engine_is_player_in_edit_mode(player_index, NULL, NULL))
        player_find_action_context(player_index, &user->action.action_context);
    user->action.aim_assist = input.aim_assist;
    user->action.flags = input.action_flags;
    user->action.action_flags = input.button_flags;
    user->action.move = input.throttle;
    user->action.primary = input.primary_trigger;
    user->action.secondary = input.secondary_trigger;
#if !defined(_XBOX) || defined(__clang__)
    player_control_latch_taps(input_user_index, &user->action, world_seconds_elapsed);
#endif

    if (user->unit_index != -1 && player_control_object_is_biped(user->unit_index))
    {
        biped_datum *biped = (biped_datum *)((unit_datum *)object_get(user->unit_index));
        if ((biped->biped.flags & 1) != 0)
        {
            real_vector3d aiming_vector = biped->unit.aiming_vector;
            biped_update_melee_turning(user->unit_index, &aiming_vector);
            euler_angles2d_from_vector3d(&user->action.look, &aiming_vector);
            user->action.look.yaw = fmodf(user->action.look.yaw + 6.2831855f, 6.2831855f);
            user->action.look.pitch = fmodf(user->action.look.pitch + 9.424778f, 6.2831855f) - 3.1415927f;
            biped->biped.flags &= ~1;
        }
    }
}

t_player_control_globals_allocator g_player_control_globals_allocator;
t_player_control_globals_deterministic_allocator g_player_control_globals_deterministic_allocator;

