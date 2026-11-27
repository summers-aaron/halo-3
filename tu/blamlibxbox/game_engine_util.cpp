// blamlibxbox/game_engine_util.obj: from the records 0xa0a10 0x24ef98 0x24f120 0x24f198 0x24fb40 0x24fc40 0x24fce0 0x24fdd8 0x24fe90 0x250040 0x250160 0x250188 0x250230 0x277c80 0x278088
// @flags /TP /O2
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "objects/object_header_access.h"
#include "math/random_math_range.h"
#include "cseries/maximum.h"
#include "memory/tls_globals.h"
#include "game/game_globals.h"
#include "memory/static_arrays.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "tag_files/tag_types.h"
#include "cseries/cseries.h"
#include "game/game_engine.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_engine_current_entry.h"
#include "interface/user_interface_text.h"
#include "game/game_engine_simulation.h"
#include "objects/object_datum.h"
#include "text/unicode.h"
#include <wchar.h>
#include "game/weapon_set_types.h"
#include "game/statborg_entry_types.h"
#include "cache/cache_files.h"
#include "game/game_results.h"
#include "game/game_engine_event_data.h"
#include "scenario/scenario_tag.h"
#include "game/game_engine_default.h"
#include "game/game_engine_list.h"
#include "scenario/scenario.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "scenario/scenario_starting_profile.h"
#include "items/weapon_definitions.h"
#include "units/unit_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_results_statistic_types.h"
#include "game/game_results_statistic_globals.h"
#include "game/game_results_storage_types.h"
#include "game/game_engine_display.h"
// @address 0x8a410 wcslen

// @address 0xa0a10 ?current_game_engine@@YAPBVc_game_engine@@XZ
// @address 0x24f120 game_engine_util_compare_token
// @address 0x24fb40 ?game_engine_get_player_name@@YAXJPAV?$c_static_wchar_string@$0BAA@@@@Z
// @address 0x24fe90 ?game_engine_weapon_item_definition_index_from_absolute_weapons_selection_block_index@@YAJFW4e_weapon_set@@@Z
// @address 0x250160 game_engine_get_current_variant
// @address 0x24fce0 ?game_engine_get_multiplayer_weapon_selection_absolute_index@@YAFJ@Z
// @address 0x250040 ?game_engine_get_game_object_definition_index@@YAJJ@Z
// @address 0x250188 ?game_engine_get_carried_multiplayer_weapon_index@@YAJJJ@Z
// @address 0x24fdd8 ?game_engine_get_multiplayer_weapon_selection_name@@YAJF@Z
// @address 0x278088 game_engine_get_active_team_mask
// @address 0x24ef98 ?build_multiplayer_string@@YAXJPB_WPBUs_game_engine_event_data@@JPA_W@Z
// @address 0x24fc40 ?game_engine_send_event@@YAXPAUs_game_engine_event_data@@@Z
// @address 0x277c80 ?game_engine_handle_time_messages@@YAXHHJJPAH@Z
// @address 0x250230 ?player_get_composite_score@@YAJJ@Z
// @address 0x58f570 wcschr



static __forceinline s_game_globals *scenario_try_and_get_game_globals() { return global_game_globals; }
static __forceinline s_multiplayer_globals_definition *multiplayer_globals_get()
{
    s_multiplayer_globals_definition *result = 0;
    s_game_globals *g = scenario_try_and_get_game_globals();
    if (g && g->multiplayer_globals_index != -1)
        result = (s_multiplayer_globals_definition *)tag_get(0x6d756c67, g->multiplayer_globals_index);
    return result;
}
static __forceinline s_multiplayer_globals_definition *scenario_definition_get(void)
{
    s_multiplayer_globals_definition *definition = 0;
    if (((s_game_globals *)global_game_globals) && ((s_game_globals *)global_game_globals)->multiplayer_globals_index != -1)
        definition = (s_multiplayer_globals_definition *)global_tag_instances[
            datum_index_to_absolute_index(global_game_globals->multiplayer_globals_index)].definition;
    return definition;
}

const c_game_engine *current_game_engine(void)
{
  if (((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index >= 1 &&
      ((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index <= 10) {
    return game_engines[((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index];
  }
  return 0;
}



static __forceinline void game_engine_util_copy_variant(wchar_t *dest, const wchar_t *source, long count) {
for (long i=0; i<=count; ++i) { wchar_t c=*source; dest[i]=c; if (!c) break; ++source; }
}
void build_multiplayer_string(long player_index, wchar_t const * string, struct s_game_engine_event_data const * event_data, long buffer_size, wchar_t * buffer)
{
    const wchar_t *source_ptr = string;
    wchar_t *dest_ptr = buffer;
    bool done = false;
    long remaining = buffer_size - 1;
    while (!done) {
        const wchar_t *token = wcschr(source_ptr, L'#');
        if (!token) {
            long length = wcslen(source_ptr);
            game_engine_util_copy_variant(dest_ptr, source_ptr, length > remaining ? remaining : length);
            dest_ptr += length;
            remaining -= length;
            done = true;
        } else {
            long prefix_length = token - source_ptr;
            const wchar_t *end = token + 1;
            while ((*end >= L'a' && *end <= L'z') || *end == L'_')
                ++end;
            long token_length = !end ? wcslen(token) : end - token;
            long copy_length = prefix_length > remaining ? remaining : prefix_length;
            game_engine_util_copy_variant(dest_ptr, source_ptr, copy_length);
            dest_ptr += copy_length;
            remaining -= copy_length;
            long written = parse_multiplayer_string_token(player_index, token, token_length, event_data, dest_ptr, remaining);
            dest_ptr += written;
            remaining -= written;
            if (!end) {
                done = true;
            } else {
                source_ptr = end;
                if (!written)
                    ++source_ptr;
            }
        }
    }
    *dest_ptr = 0;
}

// Compares a token (not null-terminated) against a name: the token's characters first, then the lengths.
extern "C" long game_engine_util_compare_token(const wchar_t *name, const wchar_t *token, long token_length)
{
  long name_length = wcslen(name);
  long result = wcsncmp(name, token, token_length);
  if (result == 0 && token_length != name_length) {
    if (token_length - name_length != 0) {
      if (token_length - name_length < 0)
        return -1;
      return 1;
    }
    result = 0;
  }
  return result;
}


namespace {
// match: 0x24fb40: typed validation keeps the original player-load register allocation.
static __forceinline player_datum *game_engine_util_try_player(const s_data_array *array, long player_index)
{
    player_datum *result = 0;
    if (player_index != -1) {
        long absolute_index = (unsigned short)player_index;
        if (absolute_index < array->first_unallocated) {
            player_datum *player = (player_datum *)((char *)array->data + absolute_index * array->size);
            if (player->identifier != 0 && player->identifier == (player_index >> 16))
                result = player;
        }
    }
    return result;
}

static __forceinline void game_engine_util_copy_string(wchar_t *destination, const wchar_t *source, long count)
{
    const wchar_t *read = source;
    for (long index = 0; index <= count - 1; ++index) {
        wchar_t character = *read;
        destination[index] = character;
        if (character == 0)
            break;
        ++read;
    }
    destination[count - 1] = 0;
}
static __forceinline void user_interface_parse_interface_string(long user_index, c_static_wchar_string<1024> *string)
{
    wchar_t buffer[256];
    game_engine_util_copy_string(buffer, string->get_string(), 256);
    user_interface_parse_string(user_index, buffer, 256);
    string->set(buffer);
}
}
void game_engine_get_player_name(long player_index, c_static_wchar_string<256> *name)
{
    player_datum *player = game_engine_util_try_player(tls_player_data(), player_index);
    if (player) {
        long user_index = -1;
        c_static_wchar_string<1024> player_name;
        player_name.set(player->configuration.host.name);
        user_interface_parse_interface_string(user_index, &player_name);
        name->set(player_name.get_string());
    } else {
        name->m_string[0] = 0;
    }
}

static __forceinline bool game_engine_util_simulation_is_predicted(char simulation)
{
  bool result = false;
  switch (simulation) {
  case 4:
    result = true;
    break;
  }
  return result;
}
void game_engine_send_event(s_game_engine_event_data *event_data)
{
  if (!game_engine_util_simulation_is_predicted(tls_game_globals()->options.game_simulation)) {
    s_game_engine_globals *globals = (s_game_engine_globals *)tls_game_engine_globals();
    event_data->identifier = globals->predicted_event;
    globals->predicted_event = (globals->predicted_event + 1) % 64;
    simulation_action_multiplayer_event(event_data);
    game_engine_handle_event(event_data, true);
  }
}

short game_engine_get_multiplayer_weapon_selection_absolute_index(long name)
{
    short result = -1;
    switch(name) {
    case -1: case 0: result = -1; break;
    case _multiplayer_weapon_selection_name_none: result = -2; break;
    case _multiplayer_weapon_selection_name_random: result = -3; break;
    default:
        s_multiplayer_globals_definition *m = multiplayer_globals_get();
        if (m && m->universal.count > 0) {
            s_multiplayer_universal_globals_definition *s = (s_multiplayer_universal_globals_definition *)m->universal.address;
            for (short index = 0; index < s->weapon_selection_count; ++index) {
                s_multiplayer_weapon_selection *e = &s->weapon_selections[index];
                if (e->name == name && e->weapon.index != -1) { result = index; break; }
            }
        }
        break;
    }
    return result;
}

long game_engine_get_multiplayer_weapon_selection_name(short absolute_index)
{
    long result = -1;
    if (absolute_index == -3)
        result = _multiplayer_weapon_selection_name_random;
    else if (absolute_index == -1)
        result = 1;
    else if (absolute_index == -2)
        result = _multiplayer_weapon_selection_name_none;
    else {
        s_multiplayer_globals_definition *multiplayer_globals = multiplayer_globals_get();
        if (multiplayer_globals && multiplayer_globals->universal.count > 0) {
            s_multiplayer_universal_globals_definition *weapon_selections =
                (s_multiplayer_universal_globals_definition *)multiplayer_globals->universal.address;
            if (absolute_index >= 0 && absolute_index < weapon_selections->weapon_selection_count) {
                s_multiplayer_weapon_selection *selection = &weapon_selections->weapon_selections[absolute_index];
                result = selection->name;
            }
        }
    }
    return result;
}

long game_engine_weapon_item_definition_index_from_absolute_weapons_selection_block_index(short absolute_index, e_weapon_set weapon_set)
{
    long result = -1;
    s_multiplayer_globals_definition *multiplayer_globals = multiplayer_globals_get();
    if (multiplayer_globals && multiplayer_globals->universal.count > 0) {
        s_multiplayer_universal_globals_definition *weapon_selections =
                (s_multiplayer_universal_globals_definition *)multiplayer_globals->universal.address;
        if (absolute_index == -3) {
            for (long attempt = 0; attempt < 100; attempt++) {
                absolute_index = random_range(tls_random_seed(), 0, (short)weapon_selections->weapon_selection_count);
                s_multiplayer_weapon_selection *selection = &weapon_selections->weapon_selections[absolute_index];
                if (selection->weapon.index != -1 && selection->weight > 0.0f)
                    break;
            }
        } else if (absolute_index == -1) {
            struct scenario *scenario = ((struct scenario *)global_scenario);
            s_scenario_starting_profile *profile = scenario->starting_profiles.count > 0
                ? (s_scenario_starting_profile *)scenario->starting_profiles.address : 0;
            if (profile) {
                long weapon = profile->weapons[weapon_set].weapon.index;
                for (long index = 0; index < weapon_selections->weapon_selection_count; index++) {
                    if (weapon_selections->weapon_selections[index].weapon.index == weapon) {
                        absolute_index = (short)index;
                        break;
                    }
                }
            }
        }
        if (absolute_index >= 0 && absolute_index < weapon_selections->weapon_selection_count) {
            s_multiplayer_weapon_selection *selection = &weapon_selections->weapon_selections[absolute_index];
            if (selection->weapon.index != -1)
                result = selection->weapon.index;
        }
    }
    return result;
}

long game_engine_get_game_object_definition_index(long type)
{
    long result = -1;
    if (game_engine_running()) {
        s_multiplayer_globals_definition *definition = scenario_definition_get();
        if (definition) {
            s_multiplayer_runtime_globals_definition *references = (s_multiplayer_runtime_globals_definition *)definition->runtime.address;
            switch (type) {
            // match: 0x250040: default __assume(0) omits the cmplwi type,5 / bgt default guard before the jump table
            default: __assume(0);
            case 2: result = references->ball.index; break;
            case 3: result = references->flag.index; break;
            case 4: result = references->bomb.index; break;
            case 5: result = references->vip_influence_area.index; break;
            case 0: result = references->editor_biped.index; break;
            case 1: result = references->editor_helper_object.index; break;
            }
        }
    }
    return result;
}

// The game options' session when the game mode is the session one (2), else null.
extern "C" c_game_variant *game_engine_get_current_variant(void)
{
  game_options *options = &tls_game_globals()->options;
  return options->game_mode == 2 ? &options->game_variant : 0;
}

// The first weapon the player's unit carries whose definition has this type.
long game_engine_get_carried_multiplayer_weapon_index(long player_index, long weapon_type)
{
  long result = -1;
  long unit_index = (*datum_get<player_datum>(tls_player_data(), player_index)).unit_index;
  if (unit_index != -1) {
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    for (long i = 0; i < 4; i++) {
      long weapon_index = unit->unit.weapon_object_indices[i];
      if (weapon_index != -1 &&
          TAG_GET(0, weapon_definition, object_get(weapon_index)->definition_index)->multiplayer_weapon_type == weapon_type) {
        result = weapon_index;
        break;
      }
    }
  }
  return result;
}

static __forceinline c_game_engine *game_engine_util_running_engine(void)
{
  s_game_engine_globals *globals = (s_game_engine_globals *)tls_game_engine_globals();
  if (globals->game_engine_index >= 1 && globals->game_engine_index <= 10)
    return game_engines[globals->game_engine_index];
  return 0;
}

extern "C" unsigned short game_engine_get_active_team_mask(void)
{
  unsigned short result = 0;
  if (game_engine_util_running_engine() && game_engine_has_teams())
    result = ((s_game_engine_globals *)tls_game_engine_globals())->valid_team_mask;
  return result;
}

static __forceinline int game_engine_util_range_check(int x) { if (x < 1) return 1; if (x > 10) return 1; return 0; }
static __forceinline int game_engine_util_bit_out_of_range(int x) { if (x < 0) return 1; if (x >= 25) return 1; return 0; }
class c_game_results;
extern "C" {
// @address 0x744ce0 g_game_engine_time_message_lookup
s_lookup g_game_engine_time_message_lookup[55] =
{
    { 0, 0, 23 },
    { 1, 1, 24 },
    { 10, 10, 16 },
    { 9, 9, 15 },
    { 8, 8, 14 },
    { 7, 7, 13 },
    { 6, 6, 12 },
    { 5, 5, 11 },
    { 4, 4, 10 },
    { 3, 3, 9 },
    { 2, 2, 8 },
    { 16, 16, 22 },
    { 15, 15, 21 },
    { 14, 14, 20 },
    { 13, 13, 19 },
    { 12, 12, 18 },
    { 11, 11, 17 },
    { 18, 18, -1 },
    { 17, 17, -1 },
    { 20, 20, -1 },
    { 19, 19, -1 },
    { 22, 22, -1 },
    { 21, 21, -1 },
    { 24, 24, -1 },
    { 23, 23, -1 },
    { 26, 26, -1 },
    { 25, 25, -1 },
    { 28, 28, -1 },
    { 27, 27, -1 },
    { 31, 31, -1 },
    { 30, 30, -1 },
    { 29, 29, -1 },
    { 32, 32, -1 },
    { 33, 33, -1 },
    { 34, 34, -1 },
    { 35, 35, -1 },
    { 36, 36, -1 },
    { 37, 37, -1 },
    { 38, 38, -1 },
    { 39, 39, -1 },
    { 40, 40, -1 },
    { 41, 41, -1 },
    { 42, 42, -1 },
    { 43, 43, -1 },
    { 44, 44, -1 },
    { 45, 45, -1 },
    { 46, 46, -1 },
    { 47, 47, -1 },
    { 48, 48, -1 },
    { 49, 49, -1 },
    { 50, 50, -1 },
    { 51, 51, -1 },
    { 52, 52, -1 },
    { 53, 53, -1 },
    { 54, 54, -1 },
};
}
// @address 0x745480 ?g_game_results_medal_statistic_definitions@@3PAUs_integer_statistic_definition@@A

void game_engine_handle_time_messages(
    int time_message_index, int results_absolute_index, long cause_player_datum, long effect_player_datum, int *message_flags)
{
    bool ok = true;
    int engine = ((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index;
    if (!game_engine_util_range_check(engine)) {
        c_game_engine *e = game_engines[engine];
        if (e != 0) ok = e->is_multiplayer_object_type_allowed(time_message_index);
    }
    if (!ok) return;
    int found = -1;
    if (g_game_results_globals.active && !g_game_results_globals.finished) {
        s_game_results_statistic *cell = &g_current_game_results.statistics.player[results_absolute_index].medals[time_message_index];
        int lo = g_game_results_medal_statistic_definitions[time_message_index].minimum;
        int hi = g_game_results_medal_statistic_definitions[time_message_index].maximum;
        int value = cell->value + 1;
        value = MIN(MAX(value, lo), hi);
        cell->value = value;
    }
    {
        unsigned int n = 55;
        int i = 0;
        s_lookup *p = g_game_engine_time_message_lookup;
        do {
            if (p->key == time_message_index) found = i;
            n--;
            i++;
            p++;
        } while (n != 0);
    }
    if (found == -1) return;
    int word = g_game_engine_time_message_lookup[found].value;
    int bit = g_game_engine_time_message_lookup[found].bit;
    if (!game_engine_util_bit_out_of_range(bit)) *message_flags |= 1 << bit;
    if (word == -1) return;
    bool mode4 = false;
    if ((unsigned char)tls_game_globals()->options.game_simulation == 4) mode4 = true;
    if (mode4) return;
    s_game_engine_event_data d;
    d.type = 1;
    d.event = word;
    d.identifier = -1;
    d.flags = -1;
    d.cause_player_index = -1;
    d.cause_team_index = -1;
    d.effect_player_index = -1;
    d.effect_team_index = -1;
    d.data = mode4;
    d.data_word = -1;
    if (cause_player_datum != -1) {
        d.cause_player_index = cause_player_datum;
        d.cause_team_index = (*datum_get<player_datum>(tls_player_data(), cause_player_datum)).configuration.host.team_index;
    }
    if (effect_player_datum != -1) {
        d.effect_player_index = effect_player_datum;
        d.effect_team_index = (*datum_get<player_datum>(tls_player_data(), effect_player_datum)).configuration.host.team_index;
    }
    game_engine_send_event(&d);
}

static __forceinline c_game_variant *game_engine_util_current_session(void)
{
  game_options *options = &tls_game_globals()->options;
  return options->game_mode == 2 ? &options->game_variant : 0;
}

#define GAME_ENGINE_UTIL_COPY_VALUE(result, buffer, buffer_size, value, value_length) \
  { long length = (value_length); result = (buffer_size) > length ? length : (buffer_size); \
    const wchar_t *s = (value); for (long i = 0; i <= result; ++s, ++i) { wchar_t c = *s; (buffer)[i] = c; if (!c) break; } }


// @address 0x24f198 ?parse_multiplayer_string_token@@YAJJPB_WJPBUs_game_engine_event_data@@PA_WJ@Z
long parse_multiplayer_string_token(long player_index, const wchar_t *token, long token_length, const s_game_engine_event_data *event,
                             wchar_t *buffer, long buffer_size)
{
  long result = 0;
  if (!game_engine_util_compare_token(L"#cause_player", token, token_length) && event && event->cause_player_index != -1) {
    c_static_wchar_string<256> name;
    game_engine_get_player_name(event->cause_player_index, &name);
    GAME_ENGINE_UTIL_COPY_VALUE(result, buffer, buffer_size, name.get_string(), wcslen(name.get_string()));
  } else if (!game_engine_util_compare_token(L"#effect_player", token, token_length) && event && event->effect_player_index != -1) {
    c_static_wchar_string<256> name;
    game_engine_get_player_name(event->effect_player_index, &name);
    GAME_ENGINE_UTIL_COPY_VALUE(result, buffer, buffer_size, name.get_string(), wcslen(name.get_string()));
  } else if (!game_engine_util_compare_token(L"#cause_team", token, token_length) && event) {
    if ((event->type == 0 && (event->event == 46 || event->event == 24)) || event->cause_team_index != -1) {
      c_static_wchar_string<256> name;
      game_engine_get_team_name(event->cause_team_index, &name);
      GAME_ENGINE_UTIL_COPY_VALUE(result, buffer, buffer_size, name.get_string(), wcslen(name.get_string()));
    }
  } else if (!game_engine_util_compare_token(L"#effect_team", token, token_length) && event) {
    if ((event->type == 0 && (event->event == 46 || event->event == 24)) || event->effect_team_index != -1) {
      c_static_wchar_string<256> name;
      game_engine_get_team_name(event->effect_team_index, &name);
      GAME_ENGINE_UTIL_COPY_VALUE(result, buffer, buffer_size, name.get_string(), wcslen(name.get_string()));
    }
  } else if (!game_engine_util_compare_token(L"#winning_team", token, token_length)) {
    c_static_wchar_string<256> name;
    long team = game_engine_get_winning_team();
    if (team == 8)
      team = 0;
    game_engine_get_team_name(team, &name);
    GAME_ENGINE_UTIL_COPY_VALUE(result, buffer, buffer_size, name.get_string(), wcslen(name.get_string()));
  } else if (!game_engine_util_compare_token(L"#winning_player", token, token_length)) {
    c_static_wchar_string<256> name;
    long player = game_engine_get_winning_player();
    game_engine_get_player_name(player, &name);
    GAME_ENGINE_UTIL_COPY_VALUE(result, buffer, buffer_size, name.get_string(), wcslen(name.get_string()));
  } else if (!game_engine_util_compare_token(L"#current_round", token, token_length)) {
    c_static_wchar_string<256> text;
    text.print(L"%d", game_engine_get_round() + 1);
    result = usnprintf(buffer, buffer_size, L"%s", text.get_string());
    if (result < 0)
      result = buffer_size;
  } else if (!game_engine_util_compare_token(L"#total_rounds", token, token_length)) {
    if (game_engine_util_current_session()) {
      c_static_wchar_string<256> text;
      text.print(L"%d", game_engine_util_current_session()->round_limit);
      result = usnprintf(buffer, buffer_size, L"%s", text.get_string());
      if (result < 0)
        result = buffer_size;
    }
  } else if (!game_engine_util_compare_token(L"#cause_team_score", token, token_length) && event && event->cause_team_index != -1) {
    c_static_wchar_string<256> text;
    game_engine_get_score_string(game_engine_get_team_rounds_won((e_game_team)event->cause_team_index), &text);
    result = usnprintf(buffer, buffer_size, L"%s", text.get_string());
    if (result < 0)
      result = buffer_size;
  } else if (!game_engine_util_compare_token(L"#cause_team_opponent_score", token, token_length) && game_engine_has_teams() && event) {
    c_static_wchar_string<256> text;
    long best = 0x80000000;
    for (long team = 0; team < 8; team++) {
      if (game_engine_is_team_active((e_game_team)team) && team != event->cause_team_index) {
        long score = game_engine_get_team_rounds_won((e_game_team)team);
        best = maximum(best, score);
      }
    }
    game_engine_get_score_string(best, &text);
    result = usnprintf(buffer, buffer_size, L"%s", text.get_string());
    if (result < 0)
      result = buffer_size;
  } else if (!game_engine_util_compare_token(L"#local_player_score", token, token_length)) {
    if (player_index != -1) {
      c_static_wchar_string<256> text;
      game_engine_get_score_string(game_engine_get_player_kills(player_index), &text);
      result = usnprintf(buffer, buffer_size, L"%s", text.get_string());
      if (result < 0)
        result = buffer_size;
    }
  } else if (!game_engine_util_compare_token(L"#local_team_score", token, token_length) && game_engine_has_teams()) {
    if (player_index != -1) {
      player_datum *player = &(*datum_get<player_datum>(tls_player_data(), player_index));
      if (player->configuration.host.team_index != -1) {
        c_static_wchar_string<256> text;
        game_engine_get_score_string(game_engine_get_team_rounds_won((e_game_team)player->configuration.host.team_index), &text);
        result = usnprintf(buffer, buffer_size, L"%s", text.get_string());
        if (result < 0)
          result = buffer_size;
      }
    }
  } else if (!game_engine_util_compare_token(L"#score_to_win", token, token_length)) {
    c_static_wchar_string<256> text;
    game_engine_get_score_string(game_engine_get_score_to_win_round(), &text);
    result = usnprintf(buffer, buffer_size, L"%s", text.get_string());
    if (result < 0)
      result = buffer_size;
  } else if (!game_engine_util_compare_token(L"#local_spawn_time", token, token_length)) {
    if (player_index != -1) {
      player_datum *player = &(*datum_get<player_datum>(tls_player_data(), player_index));
      result = usnprintf(buffer, buffer_size, L"%d", player->respawn_seconds);
      if (result < 0)
        result = buffer_size;
    }
  } else if (!game_engine_util_compare_token(L"#local_lives_remaining", token, token_length)) {
    if (player_index != -1) {
      player_datum *player = &(*datum_get<player_datum>(tls_player_data(), player_index));
      result = usnprintf(buffer, buffer_size, L"%d", player->lives);
      if (result < 0)
        result = buffer_size;
    }
  } else if (!game_engine_util_compare_token(L"#switch_side_time", token, token_length)) {
    result = usnprintf(buffer, buffer_size, L"%d", ((s_game_engine_globals *)tls_game_engine_globals())->timer_seconds);
    if (result < 0)
      result = buffer_size;
  } else {
    const c_game_engine *engine = current_game_engine();
    long length;
    if (engine && (length = engine->parse_multiplayer_string_token(player_index, token, token_length, event, buffer, buffer_size)) != 0)
      result = length;
    else
      GAME_ENGINE_UTIL_COPY_VALUE(result, buffer, buffer_size, token, token_length);
  }
  return result;
}

static __forceinline c_game_statborg *game_engine_util_get_statborg()
{
    c_game_statborg *result = 0;
    if (current_game_engine_entry())
        result = &((s_game_engine_globals *)tls_game_engine_globals())->statborg;
    return result;
}
static __forceinline short game_engine_util_get_team_stat(const c_game_statborg *statborg, e_game_team team, e_statborg_entry statistic)
{
    return statborg->m_team_stats[team][statistic];
}
static __forceinline short game_engine_util_get_player_stat(const c_game_statborg *statborg, long player_index, e_statborg_entry statistic)
{
    return statborg->m_player_stats[datum_index_to_absolute_index(player_index)][statistic];
}
long player_get_composite_score(long player_index)
{
    c_game_statborg *statborg = game_engine_util_get_statborg();
    long score = 0;
    if (statborg)
    {
        player_datum *player = datum_get<player_datum>(tls_player_data(), player_index);
        if (game_engine_has_teams())
        {
            if (player->configuration.host.team_index != -1)
                score = game_engine_util_get_team_stat(statborg, (e_game_team)player->configuration.host.team_index, _statborg_entry_score);
        }
        else
            score = game_engine_util_get_player_stat(statborg, player_index, _statborg_entry_score);
    }
    return score;
}
