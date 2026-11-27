// blamlibxbox/game_engine_simulation.obj: from the records 0x29f760 0x29f7c0 0x29f810 0x29f860 0x29f8c0 0x29f968 0x29fdf8 0x29fec0 0x29ffd8 0x2a0068 0x2a01f8 0x2a02e0 0x2a03b8 0x2a0630 0x2a0928
// @flags /TP /O2 /Oc
#include "camera/director.h"
#include "memory/tls_globals.h"
#include "simulation/simulation_entity_types.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "game/game_variant_inline.h"
#include "game/game_time_types.h"
#include <stdlib.h>
#include "game/game_variant.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include <string.h>
#include "memory/data.h"
#include "tag_files/tag_groups.h"
#include "game/game_engine.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_simulation.h"
#include "game/game_engine_spawning.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_results.h"
#include "interface/chud/chud.h"
#include "interface/chud/chud_messaging.h"
#include "game/game_engine_multiplayer_sounds.h"
#include "game/players.h"
#include "game/player_types.h"
#include "interface/chud/chud_medal_message_types.h"
#include "cache/cache_files.h"
#include "interface/chud/chud_message_context_types.h"
#include "cseries/language.h"
#include "cseries/language_types.h"
#include "game/game_engine_default.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_list.h"
#include "game/game_engine_event_data.h"
#include "game/multiplayer_event_response_definition.h"
#include "game/multiplayer_runtime_globals.h"
#include "game/game_engine_settings_definitions.h"
#include "game/game_globals.h"
#include "scenario/scenario.h"
#include "game/player_in_game_iterator.h"
#include "game/game_engine_base_variant.h"
#include "game/game_engine_simulation_externs.h"
#include "game/game_engine_teleporter_types.h"
#include "interface/chud/chud_user_data_types.h"
#include "camera/director_globals_types.h"
typedef char game_engine_simulation_sound_permutation_size_check[(sizeof(s_game_engine_simulation_sound_permutation) == 0xc8) ? 1 : -1];
typedef char multiplayer_event_response_size_check[(sizeof(s_multiplayer_event_response_definition) == 0x104) ? 1 : -1];
typedef char multiplayer_event_response_sound_offset_check[(offsetof(s_multiplayer_event_response_definition, sounds) == 0x34) ? 1 : -1];

enum e_game_engine_event_response_category
{
    _game_engine_event_response_general = 0,
    _game_engine_event_response_flavor = 1,
    _game_engine_event_response_slayer = 2,
    _game_engine_event_response_ctf = 3,
    _game_engine_event_response_oddball = 4,
    _game_engine_event_response_king = 6,
    _game_engine_event_response_vip = 7,
    _game_engine_event_response_juggernaut = 8,
    _game_engine_event_response_territories = 9,
    _game_engine_event_response_assault = 10,
    _game_engine_event_response_infection = 11,
};

struct s_game_engine_simulation_wchar_string
{
    wchar_t characters[256];
};

// @address 0x29ffd8 ?game_engine_events_compare@@YAHPBX0@Z
// @address 0x2a01f8 ?notify_audience_member_of_event@@YAXPBUs_game_engine_event_data@@PAJJ0@Z
// @address 0x2a02e0 ?audience_member_find_response@@YAPAUs_multiplayer_event_response_definition@@JPBUs_game_engine_event_data@@@Z
// @address 0x2a03b8 ?find_event_response@@YAPAUs_multiplayer_event_response_definition@@JPBUs_game_engine_event_data@@J@Z
// @address 0x29f860 ?game_engine_globals_get_player_gamestate_index@@YAJF@Z
// @address 0x29fdf8 game_engine_event_build_player_response_list
// @address 0x29fec0 game_engine_handle_event
// @address 0x2a0068 ?game_engine_events_update@@YAXXZ
// @address 0x74b540 g_current_language

enum e_director_perspective;
struct s_game_engine_state_data;
struct s_game_engine_globals;
static __forceinline c_game_engine *active_engine_for_gamestate(void)
{
  c_game_engine *result;
  if (tls_game_engine_globals()->game_engine_index >= 1 && tls_game_engine_globals()->game_engine_index <= 10)
    result = game_engines[tls_game_engine_globals()->game_engine_index];
  else
    result = 0;
  return result;
}

e_simulation_entity_type game_engine_globals_get_simulation_entity_type(void)
{
  e_simulation_entity_type result = _simulation_entity_type_none;
  if (active_engine_for_gamestate())
    result = active_engine_for_gamestate()->get_simulation_entity_type();
  return result;
}

long game_engine_globals_get_gamestate_index(void)
{
  long result = -1;
  if (active_engine_for_gamestate())
    result = tls_game_engine_globals()->gamestate_index;
  return result;
}

long game_engine_globals_get_statborg_gamestate_index(void)
{
  long result = -1;
  if (active_engine_for_gamestate())
    result = tls_game_engine_globals()->statborg_gamestate_index;
  return result;
}

long game_engine_globals_get_player_gamestate_index(short player_index)
{
  long result = -1;
  if (active_engine_for_gamestate())
    result = tls_game_engine_globals()->player_gamestate_indices[player_index];
  return result;
}

bool game_engine_globals_apply_simulation_update(unsigned long update_mask, long state_size, const void *state_data)
{
  c_game_engine *engine = active_engine_for_gamestate();
  bool result = engine->apply_simulation_update(update_mask, state_size, state_data);
  if (result)
    tls_game_engine_globals()->flags |= 4;
  return result;
}

static __forceinline float game_engine_simulation_seconds_to_ticks_real(float seconds)
{
    return seconds * (float)tls_game_time_globals()->ticks_per_second;
}

static __forceinline s_game_globals *game_engine_simulation_get_game_globals()
{
    return ((s_game_globals *)global_game_globals);
}
static __forceinline s_multiplayer_event_response_definition *game_engine_simulation_response_get(
    s_tag_block *block, long index)
{
    return &((s_multiplayer_event_response_definition *)block->address)[index];
}

#define GAME_ENGINE_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define GAME_ENGINE_MAX(a, b) (((a) > (b)) ? (a) : (b))

static __forceinline long game_engine_simulation_ticks_per_second(void)
{
    return tls_game_time_globals()->ticks_per_second;
}
static __forceinline long game_engine_simulation_seconds_integer_to_ticks(long seconds)
{
    return seconds * tls_game_time_globals()->ticks_per_second;
}
static __forceinline long game_engine_simulation_get_round_time_limit_seconds(void)
{
    return current_game_variant()->get_active_variant()->get_miscellaneous_options()->get_round_time_limit_minutes()
        * 60;
}
static __forceinline c_game_statborg *game_engine_simulation_get_statborg(void)
{
    c_game_engine *engine = active_engine_for_gamestate();
    c_game_statborg *result = 0;
    if (engine)
        result = &tls_game_engine_globals()->statborg;
    return result;
}
static __forceinline bool game_engine_simulation_has_teams(void)
{
    return active_engine_for_gamestate() && (tls_game_engine_globals()->flags & 8) != 0;
}

static __forceinline bool game_engine_simulation_queued_event_test(unsigned char flags, long bit)
{
    bool result = (flags & (1 << bit)) != 0;
    return result;
}
static __forceinline void game_engine_simulation_queued_event_clear(unsigned char *flags)
{
    *flags = 0;
}
static __forceinline void game_engine_simulation_queued_event_set(unsigned char *flags, long bit, bool value)
{
    if (value)
        *flags = (unsigned char)(*flags | (1 << bit));
    else
        *flags = (unsigned char)(*flags & ~(1 << bit));
}
static __forceinline long game_engine_simulation_time_get(void)
{
    return tls_game_time_globals()->time;
}
static __forceinline bool game_engine_simulation_is_predicted(void)
{
    bool result = false;
    if (tls_game_globals()->options.game_simulation == 4)
        result = true;
    return result;
}

// @address 0x29f968 ?game_engine_promote_to_simulation_authority@@YAXXZ
void game_engine_promote_to_simulation_authority(void)
{
    long event_index = tls_game_engine_globals()->predicted_event + 1;
    tls_game_engine_globals()->predicted_event = event_index - (event_index / 64) * 64;

    if (!active_engine_for_gamestate() || tls_game_engine_globals()->gamestate_index == -1)
        simulation_action_game_engine_globals_create();

    for (long player_index = 0; player_index < 16; player_index++) {
        if (!active_engine_for_gamestate() || tls_game_engine_globals()->player_gamestate_indices[(short)player_index] == -1)
            simulation_action_game_engine_player_create((short)player_index);
    }

    if (!active_engine_for_gamestate() || tls_game_engine_globals()->statborg_gamestate_index == -1)
        simulation_action_game_statborg_create();

    if (game_engine_simulation_get_statborg())
        game_engine_simulation_get_statborg()->stats_promote_to_authority();

    if (game_engine_simulation_has_teams()) {
        game_engine_recompute_active_teams();
        game_engine_build_allowable_team_designators();
        game_engine_build_teams();
    }

    if (!TEST_BIT(tls_game_engine_globals()->flags, 2)) {
        if (!tls_game_globals()->game_finished) {
            long maximum_rounds = game_results_get_maximum_rounds_completed();
            tls_game_engine_globals()->round_index = GAME_ENGINE_MIN(GAME_ENGINE_MAX(maximum_rounds, tls_game_engine_globals()->round_index), 31);
            simulation_action_game_engine_globals_update(16);
        }
        active_engine_for_gamestate()->recover_state_before_promotion();
        tls_game_engine_globals()->flags |= 4;
    }

    c_area_set<c_teleporter_area, 32> *slot_table = (c_area_set<c_teleporter_area, 32> *)tls_game_engine_globals()->teleporter_slot_table;
    memset(slot_table, 0, sizeof(*slot_table));
    slot_table->initialize(0x440, 112, _multiplayer_team_designator_none);
    active_engine_for_gamestate()->promote_to_simulation_authority();

    long engine_state = tls_game_engine_globals()->game_engine_state;
    if (engine_state != 0) {
        bool predicted = game_engine_simulation_is_predicted();
        if (!predicted)
            tls_game_engine_globals()->desired_game_engine_state = engine_state;
    }

    if (game_engine_in_round()) {
        long time_limit = game_engine_simulation_get_round_time_limit_seconds();
        if (time_limit != 0) {
            long time_remaining = time_limit - tls_game_engine_globals()->timer_seconds;
            if (time_remaining < 1)
                time_remaining = 1;
            tls_game_engine_globals()->round_start_time =
                game_engine_simulation_time_get() - game_engine_simulation_seconds_integer_to_ticks(time_remaining);
        }
    }

    if (game_engine_in_round()) {
        c_player_in_game_iterator players;
        players.begin();
        while (players.next()) {
            long player_index = players.m_iterator.m_iterator.index;
            game_engine_player_set_fade_state(player_index, 0);
            if (players.m_iterator.m_datum->unit_index == -1) {
                long ticks = game_engine_simulation_ticks_per_second();
                players.m_iterator.m_datum->respawn_timer = GAME_ENGINE_MAX(ticks, players.m_iterator.m_datum->respawn_seconds * ticks);
            }
            if (game_engine_limited_lives_active() && players.m_iterator.m_datum->count >= 0x8000) {
                players.m_iterator.m_datum->count = game_engine_get_current_variant()->lives_per_round;
                ::simulation_action_game_engine_player_update((short)player_index, 0x200);
            }
        }
    }
}

extern "C" void game_engine_event_build_player_response_list(const s_game_engine_event_data *event)
{
    long audience = event->audience_player_index;
    if (audience == -1) {
        c_player_in_game_iterator players;
        players.begin();
        players.m_iterator.m_iterator.data = tls_player_data();
        players.m_iterator.m_iterator.absolute_index = -1;
        players.m_iterator.m_iterator.index = -1;
        while (players.next()) {
            long player_index = players.m_iterator.m_iterator.index;
            s_multiplayer_event_response_definition *response = audience_member_find_response(player_index, event);
            if (response)
                player_submit_event_response(player_index, event, response);
        }
    } else {
        s_multiplayer_event_response_definition *response = audience_member_find_response(audience, event);
        if (response)
            player_submit_event_response(audience, event, response);
    }
}

extern "C" void game_engine_handle_event(const s_game_engine_event_data *event, bool queue)
{
    if (!queue) {
        game_engine_event_build_player_response_list(event);
        return;
    }
    long time = game_engine_simulation_time_get();
    s_game_engine_globals *globals = (s_game_engine_globals *)tls_game_engine_globals();
    s_game_engine_queued_event *queued = &globals->queued_events[event->identifier];
    bool duplicate = false;
    if (game_engine_simulation_queued_event_test(queued->flags.m_storage, 0))
        duplicate = true;
    if (duplicate)
        return;
    bool persistent = false;
    if (game_engine_simulation_is_predicted())
        globals->predicted_event = event->identifier;
    if (event->event == 12 || event->event == 25)
        persistent = true;
    game_engine_simulation_queued_event_clear(&queued->flags.m_storage);
    game_engine_simulation_queued_event_set(&queued->flags.m_storage, 0, true);
    game_engine_simulation_queued_event_set(&queued->flags.m_storage, 2, persistent);
    queued->time = time;
    queued->event = *event;
}

int game_engine_events_compare(const void *left, const void *right)
{
    const s_game_engine_queued_event *left_event = (const s_game_engine_queued_event *)left;
    const s_game_engine_queued_event *right_event = (const s_game_engine_queued_event *)right;
    unsigned long threshold = (unsigned long)game_engine_simulation_seconds_to_ticks_real(0.15f);
    long time_difference = left_event->time - right_event->time;
    long priority_difference = left_event->event.identifier - right_event->event.identifier;
    if ((unsigned long)(time_difference >= 0 ? time_difference : -time_difference) < threshold) {
        if ((priority_difference >= 0 ? priority_difference : -priority_difference) > 32)
            return time_difference;
    }
    return priority_difference;
}

void game_engine_events_update(void)
{
    unsigned long delay = (unsigned long)game_engine_simulation_seconds_to_ticks_real(0.15f);
    unsigned long alternate_delay = (unsigned long)game_engine_simulation_seconds_to_ticks_real(2.0f);
    long now = tls_game_time_globals()->time;
    s_game_engine_queued_event pending[64];
    long count = 0;
    for (long i = 0; i < 64; ++i) {
        s_game_engine_queued_event *event = &((s_game_engine_globals *)tls_game_engine_globals())->queued_events[i];
        if (game_engine_simulation_queued_event_test(event->flags.m_storage, 0)
            && !game_engine_simulation_queued_event_test(event->flags.m_storage, 1)) {
            unsigned long wait = game_engine_simulation_queued_event_test(event->flags.m_storage, 2) ? alternate_delay : delay;
            if ((unsigned long)(now - event->time) >= wait) {
                memcpy(&pending[count], event, sizeof(s_game_engine_queued_event));
                ++count;
            }
        }
    }
    qsort(pending, count, sizeof(s_game_engine_queued_event), game_engine_events_compare);
    for (long i = 0; i < count; ++i) {
        s_game_engine_queued_event *event = &((s_game_engine_globals *)tls_game_engine_globals())->queued_events[pending[i].event.identifier];
        game_engine_event_build_player_response_list(&event->event);
        game_engine_simulation_queued_event_set(&event->flags.m_storage, 1, true);
        game_engine_simulation_queued_event_set(&event->flags.m_storage, 0, false);
    }
}

void notify_audience_member_of_event(
    struct s_game_engine_event_data const *event, long *list, long max_count, struct s_game_engine_event_data const *_sym_p3)
{
    long count = 0;
    memset(list, -1, 16 * sizeof(long));
    long audience = event->audience_player_index;
    *((long *)_sym_p3) = 0;
    if (audience == -1) {
        c_player_in_game_iterator players;
        players.begin();
        players.m_iterator.m_iterator.data = tls_player_data();
        players.m_iterator.m_iterator.absolute_index = -1;
        players.m_iterator.m_iterator.index = -1;
        while (players.next() && count < 16) {
            long index = players.m_iterator.m_iterator.index;
            if (audience_member_find_response(index, event)) list[count++] = index;
        }
    } else {
        if (audience_member_find_response(audience, event)) list[count++] = audience;
    }
    *((long *)_sym_p3) = count;
}

s_multiplayer_event_response_definition *audience_member_find_response(long player_index, const s_game_engine_event_data *event_data)
{
    s_multiplayer_event_response_definition *result;
    player_datum *player = datum_get<player_datum>(tls_player_data(), player_index);
    if (player_index == event_data->cause_player_index) {
        result = find_event_response(player_index, event_data, 0);
        if (result)
            return result;
    }
    if (player_index == event_data->effect_player_index) {
        result = find_event_response(player_index, event_data, 2);
        if (result)
            return result;
    }
    long team_index = player->configuration.host.team_index;
    if (team_index == event_data->cause_team_index) {
        result = find_event_response(player_index, event_data, 1);
        if (result)
            return result;
    }
    if (team_index == event_data->effect_team_index) {
        result = find_event_response(player_index, event_data, 3);
        if (result)
            return result;
    }
    return find_event_response(player_index, event_data, 4);
}

s_multiplayer_event_response_definition *find_event_response(long player_index, const s_game_engine_event_data *event_data, long audience)
{
    s_multiplayer_event_response_definition *result = 0;
    s_tag_block *responses = 0;
    s_multiplayer_runtime_globals_definition *runtime =
        (s_multiplayer_runtime_globals_definition *)TAG_GET(
            'mulg', s_multiplayer_globals_definition,
            game_engine_simulation_get_game_globals()->multiplayer_globals_index)->runtime.address;
    switch (event_data->type) {
    case _game_engine_event_response_general: responses = &runtime->general_events; break;
    case _game_engine_event_response_slayer: responses = &runtime->slayer_events; break;
    case _game_engine_event_response_ctf: responses = &runtime->ctf_events; break;
    case _game_engine_event_response_oddball: responses = &runtime->oddball_events; break;
    case _game_engine_event_response_king: responses = &runtime->king_events; break;
    case _game_engine_event_response_flavor: responses = &runtime->flavor_events; break;
    case _game_engine_event_response_juggernaut: responses = &runtime->juggernaut_events; break;
    case _game_engine_event_response_territories: responses = &runtime->territories_events; break;
    case _game_engine_event_response_assault: responses = &runtime->assault_events; break;
    case _game_engine_event_response_infection: responses = &runtime->infection_events; break;
    case _game_engine_event_response_vip: responses = &runtime->vip_events; break;
    }
    for (long response_index = 0; response_index < responses->count; ++response_index) {
        s_multiplayer_event_response_definition *response = game_engine_simulation_response_get(responses, response_index);
        if (response->event == event_data->event && response->audience == audience) {
            bool valid = true;
            switch (response->required_field) {
            case 1: valid = valid && event_data->cause_player_index != -1; break;
            case 2: valid = valid && event_data->cause_team_index != -1; break;
            case 3: valid = valid && event_data->effect_player_index != -1; break;
            case 4: valid = valid && event_data->effect_team_index != -1; break;
            }
            switch (response->excluded_audience) {
            case 1: valid = valid && event_data->cause_player_index != player_index; break;
            case 2:
                valid = valid && event_data->cause_team_index
                    != datum_get<player_datum>(tls_player_data(), player_index)->configuration.host.team_index;
                break;
            case 3: valid = valid && event_data->effect_player_index != player_index; break;
            case 4:
                valid = valid && event_data->effect_team_index
                    != datum_get<player_datum>(tls_player_data(), player_index)->configuration.host.team_index;
                break;
            }
            if (valid) {
                result = response;
                break;
            }
        }
    }
    return result;
}

static __forceinline int game_engine_simulation_player_index_out_of_range(long player_index)
{
    if (player_index < 0)
        return 1;
    if (player_index >= 4)
        return 1;
    return 0;
}

// @address 0x2a0630 player_submit_event_response
extern "C" void player_submit_event_response(long player_index, const s_game_engine_event_data *event_data, s_multiplayer_event_response_definition *response)
{
    s_multiplayer_runtime_globals_definition *runtime =
        (s_multiplayer_runtime_globals_definition *)TAG_GET(
            'mulg', s_multiplayer_globals_definition,
            game_engine_simulation_get_game_globals()->multiplayer_globals_index)->runtime.address;
    bool play_sounds = true;
    long perspective = -1;
    s_game_engine_simulation_wchar_string message;
    s_game_engine_simulation_wchar_string alternate_message;
    wchar_t text[256];
    wchar_t alternate_text[256];
    while (true) {
        if (player_index != -1)
            perspective = ((long)::director_get_perspective(player_index, perspective));
        if (perspective == -1)
            return;
        long result = players_get_active_user_count();
        unsigned char suppressed;
        if (result > 1) {
            suppressed = (response->type == 3 || response->type == 4);
        } else {
            suppressed = 0;
        }
        if (!suppressed) {
            long message_string_id = response->message_string_id;
            if (message_string_id != -1 && message_string_id != 0) {
                e_language language = g_current_language;
                message.characters[0] = 0;
                long string_list_index = runtime->in_game_strings.index;
                if (language == -1) {
                    language = get_e_language_from_xbox_language(XTLGetLanguage());
                    g_current_language = language;
                }
                string_list_get_normal_string_by_language(string_list_index, message_string_id, language, &message);
                ::build_multiplayer_string(player_index, (wchar_t const *)(message.characters),
                    event_data, 256, (wchar_t *)(text));
                if (!(response->flags & 1)) {
                    if (!game_engine_simulation_player_index_out_of_range(perspective))
                        chud_persistent_user_data()[perspective].m_messaging_manager.post(
                            text, (e_chud_message_context)response->message_context);
                }
            }
            long medal_value = response->medal_value;
            if (medal_value != -1 && medal_value != 0) {
                if (!game_engine_simulation_player_index_out_of_range(perspective)) {
                    long medal_index = chud_get_user_skin_index(perspective);
                    chud_persistent_user_data()[perspective].m_messaging_manager.post_medal(medal_index, medal_value);
                }
            }
            long alternate_string_id = response->alternate_string_id;
            if (alternate_string_id != -1 && alternate_string_id != 0) {
                e_language language = g_current_language;
                alternate_message.characters[0] = 0;
                long string_list_index = runtime->in_game_strings.index;
                if (language == -1) {
                    language = get_e_language_from_xbox_language(XTLGetLanguage());
                    g_current_language = language;
                }
                string_list_get_normal_string_by_language(string_list_index, alternate_string_id, language, &alternate_message);
                ::build_multiplayer_string(player_index, (wchar_t const *)(alternate_message.characters),
                    event_data, 256, (wchar_t *)(alternate_text));
                if (!game_engine_simulation_player_index_out_of_range(perspective)) {
                    long ticks = (long)((float)tls_game_time_globals()->ticks_per_second * (float)response->delay_seconds + 0.5f);
                    chud_messaging_set_text(
                        (s_chud_text_block *)&chud_persistent_user_data()[perspective].m_messaging_manager,
                        alternate_text, ticks, -1, 3);
                }
            }
        }
        if (play_sounds && ((1 << tls_director_globals()->slots[perspective].perspective) & 3u) != 0) {
            game_engine_simulation_play_event_sound(player_index, event_data, response);
            play_sounds = false;
        }
    }
}

static __forceinline long game_engine_simulation_seconds_to_ticks_round(float seconds)
{
    return (long)((float)tls_game_time_globals()->ticks_per_second * seconds + 0.5f);
}

// @address 0x2a0928 game_engine_simulation_play_event_sound
extern "C" void game_engine_simulation_play_event_sound(long player, const s_game_engine_event_data *event, s_multiplayer_event_response_definition *response)
{
    bool localized = (response->sound_flags & 1) != 0;
    long language = localized ? get_current_language() : 0;
    long sound = response->sounds[language].index;
    long delay = response->sound_delay == 0.0f ? 0 : game_engine_simulation_seconds_to_ticks_round(response->sound_delay);
    if (response->sound_permutation_count > 0) {
        float random = _real_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
        float cumulative = 0.0f;
        for (long i = 0; i < response->sound_permutation_count; i++) {
            s_game_engine_simulation_sound_permutation *permutation = &response->sound_permutations[i];
            cumulative += permutation->weight;
            if (cumulative < random) {
                sound = permutation->sounds[language].index;
                localized = (permutation->flags & 1) != 0;
                break;
            }
        }
    }
    bool blocked = players_get_active_user_count() > 1 && (response->type == 1 || response->type == 4);
    if (sound != -1 && !blocked)
        game_engine_queue_multiplayer_sound(sound, delay, event, localized);
}
