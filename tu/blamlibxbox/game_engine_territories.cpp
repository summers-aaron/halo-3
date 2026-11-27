// blamlibxbox/game_engine_territories.obj: from the records 0x2ef758 0x2ef778 0x2ef8d0 0x2efc30 0x2efd20 0x2efe28 0x2eff10 0x2f0008 0x2f0248 0x2f0318 0x2f05c0 0x2f0630 0x2f09d8 0x2f09f8 0x2f0ac0 0x2f0ae8 0x2f0b70 0x2f0cb8 0x2f0d48 0x2f0f50 0x2f1108 0x2f14a8 0x2f1800 0x2f1c40 0x2f1d00 0x2f1e30 0x2f1ee8 0x2f1f68 0x2f2048
#include "memory/tls_globals.h"
#include "tag_files/tag_groups.h"
#include "game/game_engine_kill_flags_types.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "items/multiplayer_weapon_tracker.h"
#include <string.h>
#include "game/game_engine_runtime_constants.h"
#include "text/language_pack.h"
#include <stdlib.h>
#include "math/real_math.h"
#include "memory/data.h"
#include "game/game_time_types.h"
#include "game/game_variant.h"
#include "tag_files/tag_types.h"
#include "objects/object_iterator.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_area_set.h"
#include "game/game_engine_notifications.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "objects/multiplayer_game_objects.h"
#include "game/game_engine_vip.h"
#include "objects/object_datum.h"
#include "game/game_engine_territories.h"
#include "text/unicode.h"
#include "networking/network_utilities.h"
#include "game/player_types.h"
#include "memory/data_types.h"
#include "game/game_engine_event_data.h"
#include "game/statborg_entry_types.h"
#include "cache/cache_files.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "objects/object_placement.h"
#include "ai/weapon_datum_types.h"
#include "items/weapon_definitions.h"
#include "game/game_engine_territories_variant.h"
#include "game/game_engine_default.h"
#include "game/game_engine_area_types.h"
#include "game/game_engine_territories_types.h"
#include "game/game_engine_base_variant.h"
#include <new>
#include "game/game_engine_list.h"
#include "game/game_engine_current_inline.h"
#include "game/game_engine_spawn_influencer.h"
#include "game/game_engine_area_set_types.h"
#include "game/game_engine_kill_handler.h"
#include "game/player_in_game_iterator.h"
#include "game/game_results_types.h"
#include "game/game_results.h"
#include "interface/chud/chud_user_data_types.h"
#include "game/player_mapping_types.h"
#include "game/game_engine_settings_definitions.h"
#include "scenario/scenario.h"
#include "game/game_engine_interface_state_types.h"
#include "simulation/simulation_entity_types.h"
// @compiler xdk6130
// @flags /TP /O2 /Oc

struct s_game_engine_globals;
typedef char territories_variant_defender_traits_offset[
    (sizeof(void *) != 4 || ((offsetof(c_game_engine_territories_variant, m_defender_traits) == 0x1b8))) ? 1 : -1];
typedef char territories_variant_attacker_traits_offset[
    (sizeof(void *) != 4 || ((offsetof(c_game_engine_territories_variant, m_attacker_traits) == 0x1d4))) ? 1 : -1];
// @address 0x8c43d4 g_territories_variant
static c_game_engine_territories_variant *g_territories_variant;
struct cache_file_tag_instance;
struct territories_color_constants { real_argb_color white, gray; };

class c_territories_area_object_iterator : public c_object_iterator_base
{
public:
    __forceinline void begin(unsigned long type_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, 0, match_flags);
    }
    __forceinline bool next() { return object_iterator_next_with_match_flags_internal(); }
};

struct territories_global_definition { char prefix[0x1a0]; long globals_index; };

struct territories_globals_tag { char prefix[0x10]; struct territories_globals_runtime *runtime; };

struct territories_globals_runtime { char prefix[0x118]; struct territories_multiplayer_globals *multiplayer; };

struct territories_multiplayer_globals { char prefix[0x180]; float territory_navpoint_offset; };

struct territories_game_object_references { s_tag_reference references[6]; };

struct territories_scenario_definition { char prefix[0x10]; territories_game_object_references *references; };

namespace {
static __forceinline bool base_allow_weapon_pickup(long player_index, long object_index)
{
    bool result = game_engine_in_round();
    if (result)
    {
        player_datum *player = datum_get<player_datum>(player_data, player_index);
        result = player->traits.m_weapon_traits.m_weapon_pickup != 2;
    }
    return result;
}
static __forceinline unsigned short weapon_get_multiplayer_type(long object_index)
{
    weapon_datum *weapon = (weapon_datum *)(*datum_get<s_object_header>(object_header_data, object_index)).datum;
    weapon_definition *definition = ((weapon_definition *)global_tag_instances[datum_index_to_absolute_index(weapon->definition_index)].definition);
    return definition->multiplayer_weapon_type;
}
static __forceinline void set_all_controller_teams(long (&teams)[8], const long &team)
{
    for (long index = 0; index < 8; ++index)
        teams[index] = team;
}
}
static __forceinline s_territories_globals *territories_globals()
{
    // match: 0x2f1108: the member accessor adds an inline this copy that reverses the indexed add.
    return (s_territories_globals *)game_engine_globals->variant_globals;
}
static __forceinline const c_game_variant *territories_game_variant_get()
{
    game_options *options = &game_globals->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}
static __forceinline bool territories_game_is_predicted()
{
    bool result = false;
    if (game_globals->options.simulation_type == 4)
        result = true;
    return result;
}

// @address 0x420c8 ??_7c_territories_engine@@6B@
// @address 0x79d438 ?internal_territories_engine@@3Vc_territories_engine@@A
c_territories_engine internal_territories_engine;

// @address 0x42e650 ?get_type@c_territories_engine@@UBAJXZ
long c_territories_engine::get_type() const
{
    return _game_engine_territories;
}

// @address 0x428ad0 ?get_simulation_entity_type@c_territories_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_territories_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_territories_engine_globals;
}

// @address 0x42d298 ?leader_event_enabled@c_territories_engine@@UBA_NXZ
bool c_territories_engine::leader_event_enabled() const
{
    return false;
}

long c_territories_engine::get_score_to_win_round() const
{
    return territories_globals()->count;
}
// @address 0x2ef778 ?initialize_for_new_map@c_territories_engine@@UBA_NXZ
bool c_territories_engine::initialize_for_new_map() const
{
    new (&game_engine_globals->territories()) s_territories_globals;
    territories_game_variant_get()->get_active_variant()->can_be_cast_to(_game_engine_territories, (const void **)&g_territories_variant);
    if (!territories_game_is_predicted())
    {
        game_engine_globals->territories().defending_team = -1;
        simulation_action_game_engine_globals_update(0x4000);
    }
    return true;
}

long c_territories_engine::get_time_left_in_ticks(long time_left, bool include_sudden_death, bool include_grace_period) const
{
    if (territories_globals()->ending_timer != -1)
    {
        if (time_left <= territories_globals()->ending_timer)
            return territories_globals()->ending_timer;
    }
    else if (territories_globals()->sudden_death_timer != -1)
    {
        if (g_territories_variant->get_sudden_death_time() > 0)
            return territories_globals()->sudden_death_timer;
        return 1;
    }
    return time_left;
}
e_game_team c_territories_engine::get_defending_team_index() const
{
    return (e_game_team)territories_globals()->defending_team;
}
bool c_territories_engine::allow_weapon_pickup(long player_index, long object_index) const
{
    bool result = base_allow_weapon_pickup(player_index, object_index);
    if (result && ((1 << (*datum_get<s_object_header>(object_header_data,
        object_index)).object_type) & 4) != 0 && weapon_get_multiplayer_type(object_index) == 1)
        result = false;
    return result;
}
// @address 0x2f0ac0 ?multiplayer_weapon_register@c_territories_engine@@UBAXJ@Z
void c_territories_engine::multiplayer_weapon_register(long weapon_index) const
{
    weapon_datum *weapon = (weapon_datum *)datum_get<s_object_header>(object_header_data, weapon_index)->datum;
    update_territory_flag_color(weapon->multiplayer_weapon_identifier);
}
// @address 0x2f0ae8 ?parse_multiplayer_string_token@c_territories_engine@@UBAJJPB_WJPBUs_game_engine_event_data@@PA_WJ@Z
long c_territories_engine::parse_multiplayer_string_token(
    long type, const wchar_t *token, long token_length, const s_game_engine_event_data *event_data, wchar_t *buffer, long buffer_size) const
{
    long result = 0;

    if (wcsncmp(L"#territory_number", token, token_length) == 0 && event_data != 0 && event_data->data_word != -1)
    {
        long territory_index = event_data->data_word;
        result = usnprintf(buffer, buffer_size, game_engine_number_format(), territory_index + 1);
        if (result < 0)
            result = buffer_size;
    }
    return result;
}
// @address 0x2f0b70 ?recover_state_before_promotion@c_territories_engine@@UBAXXZ
void c_territories_engine::recover_state_before_promotion() const
{
    int tr11;
    if (g_territories_variant->get_one_sided())
    {
        tr11 = -1;
        s_game_engine_globals *tr6;
        tr6 = game_engine_globals;
        short tr10;
        tr10 = tr6->round_index;
        if (tr10 > 0)
        {
            unsigned int tr7;
            tr7 = tr10;
            do
            {
                int tr3;
                tr3 = (int)game_engine_next_active_team((e_game_team)tr11);
                tr11 = tr3;
                if (tr11 == -1)
                    tr11 = (int)game_engine_next_active_team((e_game_team)tr3);
                --tr7;
            } while (tr7 != 0);
        }
        tr6->territories().defending_team = tr11;
        simulation_action_game_engine_globals_update(16384);
    }
    c_game_statborg *statborg = 0;
    if (current_game_engine())
        statborg = &game_engine_globals->statborg;
    int tr28;
    tr28 = 16;
    short *team_stats = &statborg->m_team_stats[0][8];
    int tr29;
    tr29 = 7;
    do
    {
        int tr30;
        tr30 = 1 << tr28;
        team_stats[1] = 0;
        simulation_action_game_statborg_update(tr30);
        team_stats[0] = 0;
        simulation_action_game_statborg_update(tr30);
        team_stats[-8] = 0;
        simulation_action_game_statborg_update(tr30);
        tr29 = tr29 - 1;
        team_stats += 12;
        tr28 = tr28 + 1;
    } while ((unsigned int)tr29 != 0);
    initialize_for_new_round();
}
void c_territories_engine::build_simulation_baseline(long state_size, void *state) const
{
    s_territories_engine_state_data *territories_state = (s_territories_engine_state_data *)state;
    memset(territories_state, 0, sizeof(*territories_state));
    territories_state->defending_team = -1;
    territories_state->grace_period_time = -1;
    territories_state->sudden_death_time = -1;
    long team = 8;
    set_all_controller_teams(territories_state->territory_controller_team, team);
    build_global_baseline(territories_state);
}


struct s_multiplayer_object_boundary_geometry_data;
// @address 0x2efc30 ?emit_game_start_event@c_territories_engine@@UBAXJ@Z
void c_territories_engine::emit_game_start_event(long player_index) const
{
    s_game_engine_globals *engine_globals;
    unsigned char predicted;
    s_game_engine_event_data activation_event;
    predicted = 0;
    if (game_globals->options.simulation_type == 4)
        predicted = 1;
    if (predicted == 0 && game_engine_get_round() == 0)
    {
        activation_event.event = 0;
        activation_event.data = 0;
        activation_event.flags = player_index;
        activation_event.identifier = -1;
        activation_event.type = 9;
        activation_event.cause_player_index = -1;
        activation_event.cause_team_index = -1;
        activation_event.effect_player_index = -1;
        activation_event.effect_team_index = -1;
        activation_event.data_word = -1;
        game_engine_send_event(&activation_event);
        if (g_territories_variant->get_one_sided())
        {
            activation_event.cause_team_index = -1;
            activation_event.type = 9;
            activation_event.identifier = -1;
            activation_event.cause_player_index = -1;
            activation_event.effect_player_index = -1;
            engine_globals = game_engine_globals;
            activation_event.effect_team_index = -1;
            activation_event.event = 4;
            activation_event.data = 0;
            activation_event.data_word = -1;
            activation_event.flags = player_index;
            activation_event.cause_team_index = engine_globals->territories().defending_team;
            game_engine_send_event(&activation_event);
        }
    }
}
// @address 0x2efd20 ?apply_game_engine_traits_for_player@c_territories_engine@@UBAXJPAVc_player_traits@@@Z
void c_territories_engine::apply_game_engine_traits_for_player(long player_index, class c_player_traits *traits) const
{
    s_game_engine_globals *engine_globals;
    s_territory_area_set *territory_set;
    int player_team_designator;
    s_territory_data *territory_entry;
    unsigned char inside_defended_area;
    unsigned char inside_attacker_area;
    inside_defended_area = 0;
    inside_attacker_area = 0;
    engine_globals = game_engine_globals;
    territory_set = &engine_globals->territories();
    territory_entry = engine_globals->territories().begin();
    while (territory_entry != territory_set->end())
    {
        if (player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)territory_entry))
        {
            player_team_designator = (int)game_engine_team_index_to_team_designator(
                (e_game_team)((player_datum *)player_data->data)[(unsigned short)player_index].configuration.host.team_index);
            if (!((player_team_designator != territory_entry->team_designator)))
                inside_defended_area = 1;
            else
                inside_attacker_area = 1;
        }
        ++territory_entry;
    }
    if (!((((unsigned char)inside_defended_area) == 0)))
    {
        traits->set(&g_territories_variant->m_defender_traits, false);
        return;
    }
    if (!((((unsigned char)inside_attacker_area) == 0)))
        traits->set(&g_territories_variant->m_attacker_traits, false);
    return;
}
// @address 0x2eff10 ?assemble_spawn_influencers_for_player@c_territories_engine@@UBAXJPAUs_netgame_goal_influencer@@PAJ@Z
void c_territories_engine::assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers, long *count) const
{
    int tr3;
    tr3 = game_engine_team_index_to_team_designator((e_game_team)datum_get<player_datum>(player_data,
        player_index)->configuration.host.team_index);
    s_territory_data *tr11;
    *count = 0;
    s_territory_area_set *tr9;
    tr11 = (s_territory_data *)game_engine_globals;
    tr9 = &((s_game_engine_globals *)tr11)->territories();
    tr11 = ((s_game_engine_globals *)tr11)->territories().begin();
    for (; tr11 != tr9->end(); ++tr11) {
        if (tr11->team_designator == tr3) {
            influencers[*count].type = 13;
            influencers[*count].object_index = tr11->get_object_index();
            *count = *count + 1;
        } else if (tr11->team_designator != 8) {
            influencers[*count].type = 14;
            influencers[*count].object_index = tr11->get_object_index();
            *count = *count + 1;
        }
        if (*count == 16)
            break;
    }
}

static __forceinline const c_game_variant *territories_current_game_variant() {
    game_options *options = &game_globals->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}
static __forceinline long territories_game_engine_get_round_time_limit_seconds()
{
    return territories_current_game_variant()->get_active_variant()->get_miscellaneous_options()->get_round_time_limit_minutes() * 60;
}
static __forceinline long territories_game_seconds_integer_to_ticks(long seconds) { return seconds * TLS_GAME_TIME_GLOBALS->ticks_per_second; }
static __forceinline long territories_game_engine_round_time_get() {
    return (TLS_GAME_TIME_GLOBALS->time-game_engine_globals->round_start_time)<0 ? 0 : (TLS_GAME_TIME_GLOBALS->time-game_engine_globals->round_start_time);
}
// @address 0x2f14a8 ?update_sudden_death@@YAXXZ
void update_sudden_death()
{
    bool contested=false;
    for(s_territory_data *territory=territories_globals()->begin(); territory!=territories_globals()->end(); ++territory) {
        if(territory->flags==2 || territory->flags==3) contested=true;
    }
    if(territories_globals()->ending_timer != -1) {
        if(contested) {
            territories_globals()->ending_timer=-1;
            simulation_action_game_engine_globals_update(0x10000);
        } else {
            --territories_globals()->ending_timer;
            simulation_action_game_engine_globals_update(0x10000);
            if(territories_globals()->ending_timer==-1) {
                territories_globals()->sudden_death_timer=-1;
                simulation_action_game_engine_globals_update(0x8000);
            }
        }
    } else if(territories_globals()->sudden_death_timer != -1) {
        if(g_territories_variant->get_sudden_death_time()>0) {
            --territories_globals()->sudden_death_timer;
            simulation_action_game_engine_globals_update(0x8000);
        }
        if(!contested) {
            territories_globals()->ending_timer=territories_game_seconds_integer_to_ticks(6)-1;
            simulation_action_game_engine_globals_update(0x10000);
        }
    } else if(g_territories_variant->get_sudden_death_enabled() && contested) {
        bool start_sudden_death=false;
        long time_left = territories_game_seconds_integer_to_ticks(territories_game_engine_get_round_time_limit_seconds()) -
            territories_game_engine_round_time_get();
        if(g_territories_variant->get_miscellaneous_options()->get_round_time_limit_minutes() != 0 && time_left==0) {
            for(s_territory_data *territory=territories_globals()->begin(); territory!=territories_globals()->end(); ++territory) {
                if(territory->flags==0) {
                    territory->flags=1;
                    simulation_action_game_engine_globals_update(1 << ((territory - territories_globals()->areas)+6));
                }
            }
            start_sudden_death=true;
        } else {
            long team=8;
            for(s_territory_data *territory=territories_globals()->begin(); territory!=territories_globals()->end(); ++territory) {
                if(team==8) team=territory->team_designator;
                if(team==8 || team!=territory->team_designator || (g_territories_variant->get_one_sided() && territory->team_designator==0)) {
                    team=8;
                    break;
                }
            }
            start_sudden_death=team!=8;
        }
        if(start_sudden_death) {
            territories_globals()->sudden_death_timer=MAX(0,territories_game_seconds_integer_to_ticks(g_territories_variant->get_sudden_death_time()));
            simulation_action_game_engine_globals_update(0x8000);
        }
    }
}


// @address 0x2f0248 ?update@c_territories_engine@@UBAXXZ
void c_territories_engine::update() const
{
    ((c_area_set<s_territory_data, 8> *)territories_globals())->update_areas();
    if (game_engine_in_round())
    {
        for (s_territory_data *area = territories_globals()->begin(); area != territories_globals()->end(); ++area)
            update_territory(area - territories_globals()->areas);
        if (!territories_game_is_predicted())
            update_sudden_death();
    }
}

// @address 0x2f1f68 ?initialize@?$c_area_set@Us_territory_data@@$07@@QAAXJJW4e_multiplayer_team_designator@@@Z
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::initialize(long maximum_count_value, long area_type_flags_value,
    e_multiplayer_team_designator team_designator)
{
    m_team_designator = 8;
    m_area_type_flags = 1088;
    m_area_flags = 8192;
    update_areas();
    if (territories_game_is_predicted())
        return;
    t_area *selected_area = 0;
    for (t_area *area = begin(); area != end(); ++area)
    {
        if (((c_area *)area)->is_selected())
        {
            selected_area = area;
            break;
        }
    }
    if (selected_area != 0)
        ((c_area *)selected_area)->set_selected(false);
}
template void c_area_set<s_territory_data, 8>::initialize(long, long, e_multiplayer_team_designator);

static __forceinline void *territories_area_datum_try_and_get(s_data_array *data, long handle)
{
    if (handle == -1)
        return 0;
    long absolute_index = handle & 0xffff;
    if (absolute_index >= data->first_unallocated)
        return 0;
    s_object_header *header = (s_object_header *)(data->data + data->size * absolute_index);
    long identifier = header->identifier;
    if (identifier == 0 || identifier != (handle >> 16))
        return 0;
    return header;
}
static __forceinline object_datum *territories_area_object_try_and_get_and_verify_type(long object_index, unsigned long type_mask)
{
    s_object_header *header = (s_object_header *)territories_area_datum_try_and_get((s_data_array *)object_header_data, object_index);
    if (!header || !((1 << header->object_type) & type_mask))
        return 0;
    return (object_datum *)header->datum;
}
static __forceinline s_multiplayer_object_properties *territories_area_object_get_owner_data(long object_index)
{
    object_datum *object = territories_area_object_try_and_get_and_verify_type(object_index, -1);
    if (object && object->multiplayer_data_offset != -1)
        return (s_multiplayer_object_properties *)((char *)object + object->multiplayer_data_offset);
    return 0;
}
static __forceinline s_object_header *territories_area_object_get_header(long object_index)
{
    return &(*datum_get<s_object_header>(object_header_data, object_index));
}
static __forceinline long territories_area_get_object_index(const s_territory_data &area) { return area.m_object_index; }
static __forceinline void territories_area_set_object_index(s_territory_data &area, long index) { area.m_object_index = index; }
static __forceinline void territories_area_calculate_boundary_geometry(s_territory_data &area)
{
    unsigned __int64 wide = (unsigned int)area.m_object_index;
    if ((int)wide != -1)
        calculate_influence_boundary_geometry((int)wide, &area, 0);
}

// @address 0x2f2048 ?update_areas@?$c_area_set@Us_territory_data@@$07@@IAAXXZ
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::update_areas()
{
    long handles[20];
    t_area temporary;
    c_territories_area_object_iterator iterator;
    long count = 0;

    iterator.begin(m_area_type_flags, m_area_flags);
    while (iterator.next() && count < 20)
    {
        long object_index = iterator.get_index();
        s_multiplayer_object_properties *owner_data = territories_area_object_get_owner_data(object_index);
        if (!(territories_area_object_get_header(object_index)->flags & 0x10))
        {
            if (!owner_data || owner_data->team_designator == m_team_designator)
                handles[count++] = object_index;
        }
    }
    qsort(handles, count, sizeof(long), c_area_set_compare_by_order_index);
    if (count > k_area_count)
        count = k_area_count;
    for (long i = 0; i < count; i++)
    {
        long object_index = handles[i];
        if (territories_area_get_object_index(m_areas[i]) != object_index)
        {
            memcpy(&temporary, &m_areas[i], sizeof(t_area));
            for (long j = i; j < m_area_count; j++)
            {
                if (territories_area_get_object_index(m_areas[j]) == object_index)
                {
                    memcpy(&m_areas[i], &m_areas[j], sizeof(t_area));
                    memcpy(&m_areas[j], &temporary, sizeof(t_area));
                    break;
                }
            }
        }
        territories_area_set_object_index(m_areas[i], object_index);
        territories_area_calculate_boundary_geometry(m_areas[i]);
    }
    m_area_count = count;
}
template void c_area_set<s_territory_data, 8>::update_areas();

float blink(float minimum, float maximum, float frequency)
{
    minimum = 0.25f;
    maximum = 0.95f;
    frequency = 4.0f;

    float t = (float)TLS_GAME_TIME_GLOBALS->time * (2.0f * frequency) / (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
    long cycle = (long)t;
    float fraction;
    if (cycle % 2)
        fraction = 1.0f - (t - (float)cycle);
    else
        fraction = t - (float)cycle;

    return minimum + (maximum - minimum) * fraction;
}


static __forceinline e_game_team territories_game_engine_team_designator_to_team_index(e_multiplayer_team_designator designator)
{
    return (e_game_team)game_engine_globals->team_mapping[designator];
}

static __forceinline void territories_game_engine_get_team_color(e_game_team team_index, real_rgb_color *color)
{
    *color = network_get_team_color(team_index);
}

// @address 0x2f1e30 ?update_territory_flag_color@@YAXJ@Z
void update_territory_flag_color(long territory_index)
{
    if (territory_index >= 0 && territory_index < territories_globals()->get_area_count())
    {
        const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)territory_index);
        if (tracker != NULL)
        {
            e_game_team team_index = territories_game_engine_team_designator_to_team_index(
                (e_multiplayer_team_designator)(*territories_globals())[territory_index].team_designator);
            real_rgb_color color;
            territories_game_engine_get_team_color(team_index, &color);
            object_set_base_change_color_by_index(tracker->weapon_index, 0, &color);
        }
    }
}

// @address 0x2f1ee8 ?adjust_statborg_team_stat@@YAXW4e_game_team@@W4e_statborg_entry@@FJ@Z
void adjust_statborg_team_stat(e_game_team team_index, e_statborg_entry statistic, short adjustment, long game_results_statistic)
{
    bool adjust = true;
    if (g_territories_variant->get_one_sided() && team_index == territories_globals()->defending_team)
        adjust = false;
    if (adjust)
        game_engine_get_statborg()->adjust_team_stat(team_index, statistic, adjustment, game_results_statistic);
}
static __forceinline player_datum *territories_player_datum_get(long player_index)
{
    return datum_get<player_datum>(player_data, player_index);
}

// @address 0x2f1800 ?capture_territory@@YAXJW4e_multiplayer_team_designator@@@Z
void capture_territory(long territory_index, e_multiplayer_team_designator team_designator)
{
    c_player_in_game_iterator iterator;
    s_game_engine_event_data event1;
    s_game_engine_event_data event2;
    s_game_engine_event_data event3;
    s_territory_area_set *area_set = territories_globals();
    s_territory_data *territory = &area_set->areas[territory_index];
    bool create_flag = true;
    if (territory->team_designator != 8)
    {
        adjust_statborg_team_stat((e_game_team)game_engine_globals->team_mapping[territory->team_designator], _statborg_entry_score, -1, 6);
        adjust_statborg_team_stat((e_game_team)game_engine_globals->team_mapping[territory->team_designator], _statborg_entry_territories_owned, -1, 44);
        event1.cause_team_index = -1;
        event1.effect_team_index = -1;
        event1.data_word = -1;
        event1.type = 9;
        event1.event = 2;
        event1.identifier = -1;
        event1.flags = -1;
        event1.cause_player_index = -1;
        event1.effect_player_index = -1;
        event1.data = 0;
        event1.effect_team_index = game_engine_globals->team_mapping[territory->team_designator];
        event1.cause_team_index = game_engine_globals->team_mapping[team_designator];
        event1.data_word = (short)territory_index;
        game_engine_send_event(&event1);
        create_flag = false;
    }
    adjust_statborg_team_stat((e_game_team)game_engine_globals->team_mapping[team_designator], _statborg_entry_score, 1, 6);
    adjust_statborg_team_stat((e_game_team)game_engine_globals->team_mapping[team_designator], _statborg_entry_territories_owned, 1, 44);
    adjust_statborg_team_stat((e_game_team)game_engine_globals->team_mapping[team_designator], _statborg_entry_territories_captures, 1, 45);
    event2.cause_team_index = -1;
    event2.type = 9;
    event2.event = 1;
    event2.identifier = -1;
    event2.flags = -1;
    event2.cause_player_index = -1;
    event2.effect_player_index = -1;
    event2.effect_team_index = -1;
    event2.data = 0;
    event2.data_word = -1;
    event2.cause_team_index = game_engine_globals->team_mapping[team_designator];
    if (territory->team_designator != 8)
        event2.effect_team_index = game_engine_globals->team_mapping[territory->team_designator];
    event2.data_word = (short)territory_index;
    game_engine_send_event(&event2);
    territory->team_designator = team_designator;
    territory->flags = 0;
    unsigned short *progress = territory->capture_progress;
    long progress_count = 8;
    do {
        *progress++ = 0;
    } while (--progress_count != 0);
    unsigned char *capturing = territory->capturing_teams;
    long capturing_count = 8;
    do {
        *capturing++ = 0;
    } while (--capturing_count != 0);
    if (create_flag)
        create_territory_flag(territory_index);
    else
        update_territory_flag_color(territory_index);
    if (g_territories_variant->get_lock_after_first_capture())
    {
        game_engine_globals->territories().areas[territory_index].flags = 1;
        simulation_action_game_engine_globals_update(1 << (territory_index + 6));
        if (team_designator != 8)
        {
            event3.cause_team_index = -1;
            event3.type = 9;
            event3.event = 3;
            event3.identifier = -1;
            event3.flags = -1;
            event3.cause_player_index = -1;
            event3.effect_player_index = -1;
            event3.effect_team_index = -1;
            event3.data = 0;
            event3.data_word = -1;
            event3.cause_team_index = game_engine_globals->team_mapping[team_designator];
            event3.data_word = (short)territory_index;
            game_engine_send_event(&event3);
        }
    }
    simulation_action_game_engine_globals_update(territory_index + 6);
    iterator.begin();
    if (iterator.next())
    {
        do {
            if (game_engine_team_index_to_team_designator((e_game_team)((player_datum *)iterator.get_datum())->configuration.host.team_index) == team_designator
                && player_in_multiplayer_object_boundary(iterator.get_index(), (const s_multiplayer_object_boundary_geometry_data *)territory))
            {
                game_results_statistic_increment((unsigned short)iterator.get_index(), _game_team_none,
                    _game_results_statistic_territories_captures, 1);
                break;
            }
        } while (iterator.next());
    }
    if (g_territories_variant->get_respawn_on_capture() != c_game_engine_territories_variant::_territories_respawn_on_capture_disabled)
    {
        iterator.begin();
        if (iterator.next())
        {
            do {
                player_datum *player = (player_datum *)iterator.get_datum();
                if ((player->flags & 2) == 0 && (player->flags & 1) != 0 && player->unit_index == -1)
                {
                    e_multiplayer_team_designator player_team = game_engine_team_index_to_team_designator((e_game_team)player->configuration.host.team_index);
                    bool same_team = player_team == team_designator;
                    short respawn_mode = g_territories_variant->get_respawn_on_capture();
                    if (respawn_mode == c_game_engine_territories_variant::_territories_respawn_on_capture_all
                        || (respawn_mode == c_game_engine_territories_variant::_territories_respawn_on_capture_enemy && !same_team)
                        || (respawn_mode == c_game_engine_territories_variant::_territories_respawn_on_capture_ally && same_team))
                    {
                        if (game_engine_player_is_out_of_lives(iterator.get_index()))
                        {
                            player->lives = 1;
                            simulation_action_game_engine_player_update((short)iterator.get_index(), 0x200);
                        }
                        player->respawn_timer = 0;
                        player->respawn_seconds = 0;
                        simulation_action_game_engine_player_update((short)iterator.get_index(), 0x800);
                    }
                }
            } while (iterator.next());
        }
    }
}

// @address 0x2efe28 ?player_killed_player@c_territories_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void c_territories_engine::player_killed_player(
    long killer, long victim, long dead_unit, bool suicide, c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const
{
    game_engine_handle_player_killed((void *)this, killer, victim, dead_unit, suicide, kill_flags, damage);
    if (killer != -1 && !suicide)
    {
        long team = game_engine_team_index_to_team_designator((e_game_team)territories_player_datum_get(killer)->configuration.host.team_index);
        s_territory_data *territory = territories_globals()->begin();
        s_territory_area_set *areas = territories_globals();
        for (; territory != areas->end(); ++territory)
        {
            if (territory->team_designator == team &&
                player_in_multiplayer_object_boundary(dead_unit, (const s_multiplayer_object_boundary_geometry_data *)territory))
            {
                game_engine_get_statborg()->adjust_player_stat(killer, _statborg_entry_territories_ousts, 1, 46, true);
                break;
            }
        }
    }
}

struct s_game_globals;
// @rdata 0x8202a1a4 4
static const float k_territories_navpoint_frequency = 4.0f;
// @rdata 0x8202215c 4
static const float k_territories_navpoint_maximum = 0.95f;
// @rdata 0x8202251c 4
static const float k_territories_navpoint_zero = 0.0f;
// @rdata 0x820221fc 4
static const float k_territories_navpoint_one = 1.0f;
static __forceinline const float &territories_navpoint_get_one() { return k_territories_navpoint_one; }
static __forceinline void territories_navpoint_point_from_line(const real_point3d *point, const real_vector3d *vector, float t,
    real_point3d *result)
{
    result->x = point->x + vector->i * t;
    result->y = point->y + vector->j * t;
    result->z = point->z + vector->k * t;
}
static __forceinline void territories_navpoint_get_center(const c_area *area, real_point3d *center)
{
    *center = area->m_boundary_geometry.matrix.position;
    const territories_global_definition *globals = (const territories_global_definition *)global_game_globals;
    const territories_globals_tag *definition =
        (const territories_globals_tag *)(global_tag_instances[datum_index_to_absolute_index(globals->globals_index)].definition);
    float offset = definition->runtime->multiplayer->territory_navpoint_offset;
    float &z = center->z; z += offset;
    center->x = offset * k_territories_navpoint_zero + center->x;
    center->y = offset * k_territories_navpoint_zero + center->y;
}
static __forceinline void territories_chud_submit_navpoint(long user_index, s_chud_navpoint *navpoint)
{
    if ((unsigned long)user_index <= 3)
        chud_persistent_user_data()[user_index].m_navpoint_manager.submit_navpoint(navpoint);
}
// @address 0x2f0008 ?submit_nav_points@c_territories_engine@@UBAXJJ@Z
void c_territories_engine::submit_nav_points(long user_index, long target_player_index) const
{
    c_game_engine::submit_nav_points(user_index, target_player_index);
    long player_index = -1;
    if (user_index != -1)
        player_index = tls_player_mapping_globals()->player_indices[user_index];
    e_multiplayer_team_designator team = game_engine_team_index_to_team_designator(
            (e_game_team)territories_player_datum_get(player_index)->configuration.host.team_index);
    s_chud_navpoint navpoint;
    navpoint.type = 2;
    for (s_territory_data *territory = territories_globals()->begin(); territory != territories_globals()->end(); ++territory)
    {
        territories_navpoint_get_center(territory, &navpoint.position);
        navpoint.object_index = territory - territories_globals()->areas;
        navpoint.icon = navpoint.object_index + 11;
        navpoint.flags = 0xc0;
        navpoint.name[0] = 0;
        if (territory->team_designator == team)
            navpoint.state = 0;
        else if (territory->team_designator == 8)
            navpoint.state = 1;
        else
            navpoint.state = 2;
        if (territory->flags == 1)
        {
            navpoint.icon = 10;
            navpoint.scale = 0.25f;
        }
        else if (territory->flags == 3 || territory->flags == 2)
        {
            navpoint.flags = 0xc1;
            float frequency = k_territories_navpoint_frequency;
            float maximum = k_territories_navpoint_maximum;
            navpoint.scale = blink(0.25f, maximum, frequency);
        }
        else
            navpoint.scale = territories_navpoint_get_one();
        territories_chud_submit_navpoint(user_index, &navpoint);
    }
}


static __forceinline c_area_set<s_territory_data, 8> *territories_get_area_set()
{
    return (c_area_set<s_territory_data, 8> *)territories_globals();
}

static __forceinline c_game_engine *territories_current_game_engine()
{
    long index = game_engine_globals->game_engine_index;
    if (index < 1)
        return 0;
    if (index > 10)
        return 0;
    return game_engines[index];
}

static __forceinline long territories_game_engine_get_round()
{
    if (territories_current_game_engine())
        return game_engine_globals->round_index;
    else
        return 0;
}

static __forceinline e_game_team territories_game_engine_next_team(e_game_team team, bool active)
{
    e_game_team result = _game_team_none;
    for (team = (e_game_team)(team + 1); team < 8; team = (e_game_team)(team + 1))
    {
        if (((game_engine_globals->active_teams & (1 << team)) != 0) == active)
        {
            result = team;
            break;
        }
    }
    return result;
}


static __forceinline void territories_delete_territory_flag(long territory_index)
{
    if (!territories_game_is_predicted() && territory_index >= 0 && territory_index < 8)
    {
        const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)territory_index);
        if (tracker)
        {
            long weapon_index = tracker->weapon_index;
            if (weapon_index != -1)
            {
                game_engine_multiplayer_weapon_deregister(weapon_index);
                object_delete(weapon_index);
            }
        }
    }
}

// @address 0x2ef8d0 ?initialize_for_new_round@c_territories_engine@@UBA_NXZ
bool c_territories_engine::initialize_for_new_round() const
{
    territories_get_area_set()->initialize(1088, 8192, _multiplayer_team_designator_none);
    if (!territories_game_is_predicted())
    {
        territories_globals()->sudden_death_timer = -1;
        territories_globals()->ending_timer = -1;
        simulation_action_game_engine_globals_update(0x18000);

        for (long territory_index = 0; territory_index < 8; territory_index++)
            territories_delete_territory_flag(territory_index);

        for (s_territory_data *territory = territories_get_area_set()->begin(); territory != territories_get_area_set()->end(); ++territory)
        {
            territory->team_designator = 8;
            for (long index = 0; index < 8; ++index)
                territory->capture_progress[index] = 0;
            for (long index = 0; index < 8; ++index)
                territory->capturing_teams[index] = 0;
            territory->flags = 0;
            simulation_action_game_engine_globals_update(1 << (territories_get_area_set()->get_absolute_index(territory) + 6));
        }

        if (g_territories_variant->get_one_sided())
        {
            e_game_team defending_team = territories_game_engine_next_team((e_game_team)territories_globals()->defending_team, true);
            territories_globals()->defending_team = defending_team;
            if (defending_team == _game_team_none)
                territories_globals()->defending_team = territories_game_engine_next_team(_game_team_none, true);
            game_engine_build_valid_team_mapping();
            simulation_action_game_engine_globals_update(0x4000);
            if (territories_game_engine_get_round() != 0)
            {
                s_game_engine_event_data event_data;
                game_engine_initialize_event(9, 4, &event_data);
                event_data.cause_team_index = territories_globals()->defending_team;
                game_engine_send_event(&event_data);
            }
        }

        if (g_territories_variant->get_one_sided())
        {
            for (s_territory_data *territory = territories_get_area_set()->begin(); territory != territories_get_area_set()->end(); ++territory)
            {
                territory->team_designator = 0;
                create_territory_flag(territories_get_area_set()->get_absolute_index(territory));
            }
        }
    }
    return true;
}

struct s_damage_owner;
static __forceinline territories_scenario_definition *territories_scenario_definition_get(void)
{
    territories_scenario_definition *definition = 0;
    const territories_global_definition *globals = (const territories_global_definition *)global_game_globals;
    if (globals && globals->globals_index != -1)
        definition = (territories_scenario_definition *)(global_tag_instances[datum_index_to_absolute_index(globals->globals_index)].definition);
    return definition;
}
static __forceinline long territories_game_engine_get_game_object_definition_index(long type)
{
    long result = -1;
    if (game_engine_running())
    {
        territories_scenario_definition *definition = territories_scenario_definition_get();
        if (definition)
        {
            territories_game_object_references *references = definition->references;
            switch (type)
            {
            // match: 0x2f1d00: default is unreachable; without the hint the switch gains cmplwi r30,5 + bgt and a retargeted default slot
            default: __assume(0);
            case 2: result = references->references[3].index; break;
            case 3: result = references->references[2].index; break;
            case 4: result = references->references[4].index; break;
            case 5: result = references->references[5].index; break;
            case 0: result = references->references[0].index; break;
            case 1: result = references->references[1].index; break;
            }
        }
    }
    return result;
}
// @address 0x2f1d00 ?create_territory_flag@@YAJJ@Z
long create_territory_flag(long territory_index)
{
    s_game_engine_globals *engine_globals = game_engine_globals;
    e_game_team team_index = (e_game_team)engine_globals->team_mapping[engine_globals->territories().areas[territory_index].team_designator];
    object_placement_data placement;
    object_placement_data_new(&placement, territories_game_engine_get_game_object_definition_index(3), -1, 0);
    placement.parent_object_index = engine_globals->territories().areas[territory_index].get_object_index();
    placement.parent_marker = 0x40004;
    placement.child_marker = 0x40004;
    placement.active_change_colors = 1;
    territories_game_engine_get_team_color(team_index, &placement.change_colors[0]);
    long object_index = object_new(&placement);
    if (object_index != -1)
    {
        game_engine_multiplayer_weapon_register(object_index, (short)territory_index);
        simulation_action_object_create(object_index);
    }
    return object_index;
}

// @address 0x2f0d48 ?build_simulation_update@c_territories_engine@@UBAXPAKJPAX@Z
void c_territories_engine::build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const
{
    s_territories_engine_state_data *state = (s_territories_engine_state_data *)state_data;
    unsigned long changed_mask = 0;
    unsigned long global_update_mask = *update_mask & 63;
    if (global_update_mask)
        build_global_update(global_update_mask, &changed_mask, (s_game_engine_state_data *)state);
    for (long index = 0; index < territories_globals()->get_area_count(); ++index) {
        if (*update_mask & (1 << (index + 6))) {
            s_territory_data &area = (*territories_globals())[index];
            bool changed = false;
            if (state->territory_controller_team[index] != area.team_designator) {
                changed = true;
                state->territory_controller_team[index] = area.team_designator;
            }
            for (long team = 0; team < 8; ++team) {
                if (state->territory_capture_time[index][team] != area.capturing_teams[team]) {
                    changed = true;
                    state->territory_capture_time[index][team] = area.capturing_teams[team];
                    break;
                }
            }
            if (state->territory_state[index] != area.flags) {
                changed = true;
                state->territory_state[index] = (char)area.flags;
            }
            if (changed)
                changed_mask |= 1 << (index + 6);
            else
                changed_mask &= ~(1 << (index + 6));
        }
    }
    if (*update_mask & (1 << 14)) {
        long defending_team = territories_globals()->defending_team;
        if (state->defending_team != defending_team) {
            state->defending_team = defending_team;
            changed_mask |= 0x4000;
        }
    }
    if (*update_mask & (1 << 15)) {
        long sudden_death_time = territories_globals()->sudden_death_timer;
        if (state->sudden_death_time != sudden_death_time) {
            state->sudden_death_time = sudden_death_time;
            changed_mask |= 0x8000;
        }
    }
    if (*update_mask & (1 << 16)) {
        long grace_period_time = territories_globals()->ending_timer;
        if (state->grace_period_time != grace_period_time) {
            state->grace_period_time = grace_period_time;
            changed_mask |= 0x10000;
        }
    }
    *update_mask = changed_mask;
}

// @address 0x2f0f50 ?apply_simulation_update@c_territories_engine@@UBA_NKJPBX@Z
bool c_territories_engine::apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_territories_engine_state_data *state = (const s_territories_engine_state_data *)state_data;
    bool success = true;
    if ((update_mask & 63))
        success = success && apply_global_update((update_mask & 63), state);
    for (long index = 0; index < 8; ++index)
    {
        if (update_mask & (1 << (index + 6)))
        {
            s_territory_data &area = (*territories_globals())[index];
            if (area.team_designator != state->territory_controller_team[index])
            {
                area.team_designator = state->territory_controller_team[index];
                update_territory_flag_color(index);
            }
            for (long team = 0; team < 8; ++team)
            {
                area.capturing_teams[team] = state->territory_capture_time[index][team];
                area.capture_progress[team] = (unsigned short)(TLS_GAME_TIME_GLOBALS->ticks_per_second * state->territory_capture_time[index][team] * 6);
            }
            if (state->territory_state[index] >= 0 && state->territory_state[index] < 4)
                area.flags = state->territory_state[index];
            else
                success = false;
        }
    }
    if (update_mask & (1 << 14))
        territories_globals()->defending_team = state->defending_team;
    if (update_mask & (1 << 15))
        territories_globals()->sudden_death_timer = state->sudden_death_time;
    if (update_mask & (1 << 16))
        territories_globals()->ending_timer = state->grace_period_time;
    return success;
}


static __forceinline unsigned long territories_hud_color_to_pixel32(float alpha, const real_rgb_color *color)
{
    return ((((((unsigned long)(long)(alpha * 255.0f) << 8)
        | (unsigned long)(long)(color->red * 255.0f)) << 8)
        | (unsigned long)(long)(color->green * 255.0f)) << 8)
        | (unsigned long)(long)(color->blue * 255.0f);
}

// @address 0x2f0630 ?get_hud_interface_state@c_territories_engine@@UBAXJPAUgame_engine_interface_state@@@Z
void c_territories_engine::get_hud_interface_state(long player_index, game_engine_interface_state *state) const
{
    s_game_engine_globals *globals = game_engine_globals;
    if (globals->territories().ending_timer != -1)
    {
        state->sudden_death_active = true;
    }
    else if (globals->territories().sudden_death_timer != -1)
    {
        state->sudden_death = true;
        state->sudden_death_active = g_territories_variant->get_sudden_death_time() > 0;
    }
    if (player_index != -1 && game_engine_in_round())
    {
        e_multiplayer_team_designator team = game_engine_team_index_to_team_designator(
            (e_game_team)territories_player_datum_get(player_index)->configuration.host.team_index);
        s_territory_data *selected = 0;
        for (s_territory_data *area = globals->territories().begin(); area != globals->territories().end(); ++area)
        {
            if (player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)area))
            {
                if (selected == 0 || area->capture_progress[team] > selected->capture_progress[team])
                    selected = area;
            }
        }
        if (selected != 0)
        {
            const territories_global_definition *game_globals = (const territories_global_definition *)global_game_globals;
            const s_multiplayer_globals_definition *definition =
                (const s_multiplayer_globals_definition *)(global_tag_instances[datum_index_to_absolute_index(game_globals->globals_index)].definition);
            const s_multiplayer_universal_globals_definition *universal = (const s_multiplayer_universal_globals_definition *)definition->universal.address;
            s_game_engine_event_data event_data;
            game_engine_initialize_event(9, -1, &event_data);
            event_data.cause_player_index = player_index;
            event_data.cause_team_index = territories_player_datum_get(player_index)->configuration.host.team_index;
            event_data.data_word = (short)(selected - globals->territories().areas);
            wchar_t format[256];
            if (selected->flags == 3)
            {
                string_list_get_string(universal->strings_index, 0x40009, format, 256);
                build_multiplayer_string(player_index, format, &event_data, 64, state->flag_return_text);
            }
            else if (selected->flags == 2)
            {
                string_list_get_string(universal->strings_index, 0x4000a, format, 256);
                build_multiplayer_string(player_index, format, &event_data, 64, state->flag_return_text);
            }
            if ((selected->flags == 3 || selected->flags == 2)
                && team != selected->team_designator && g_territories_variant->m_capture_time > 0)
            {
                real_rgb_color color;
                color = network_get_team_color(territories_game_engine_team_designator_to_team_index(team));
                state->flag_return_visible = true;
                state->flag_return_progress = (float)selected->capture_progress[team]
                    / (float)((TLS_GAME_TIME_GLOBALS->ticks_per_second * g_territories_variant->m_capture_time) * 6);
                float alpha;
                if (selected->flags == 2)
                    alpha = blink(0.25f, 0.95f, 4.0f);
                else
                    alpha = 1.0f;
                state->flag_return_color = territories_hud_color_to_pixel32(alpha, &color);
            }
        }
    }
}
// @address 0x2f0318 ?should_end_round@c_territories_engine@@UBA_NPAJ@Z
bool c_territories_engine::should_end_round(long *winner) const
{
    bool result = false;
    long team = 8;
    for (s_territory_data *territory = territories_globals()->begin(); territory != territories_globals()->end(); ++territory)
    {
        if (team == 8)
            team = territory->team_designator;
        if (territory->team_designator != team || (g_territories_variant->get_one_sided()
            && territory->team_designator == 0) || territory->team_designator == 8)
        {
            team = 8;
            break;
        }
        if (territory->flags == 2 || territory->flags == 3)
        {
            team = 8;
            break;
        }
    }
    if (team != 8)
    {
        if (territories_globals()->ending_timer == 0)
            result = true;
        else if (territories_globals()->ending_timer == -1)
        {
            territories_globals()->ending_timer = territories_game_seconds_integer_to_ticks(6) - 1;
            simulation_action_game_engine_globals_update(0x10000);
        }
    }
    bool all_locked = territories_globals()->get_area_count() > 0;
    for (s_territory_data *territory = territories_globals()->begin(); territory != territories_globals()->end(); ++territory)
    {
        if (territory->flags != 1)
        {
            all_locked = false;
            break;
        }
    }
    if (all_locked)
        result = true;
    if (result)
    {
        c_game_statborg *statborg = 0;
        s_game_engine_globals *state = game_engine_globals;
        if (state->game_engine_index >= 1 && state->game_engine_index <= 10 && game_engines[state->game_engine_index] != 0)
            statborg = &state->statborg;
        long best_team = 8;
        for (s_territory_data *territory = territories_globals()->begin(); territory != territories_globals()->end(); ++territory)
        {
            long team_designator = territory->team_designator;
            if (team_designator != 8)
            {
                if (best_team == 8 ||
                    statborg->get_team_stat(game_engine_globals->team_mapping[team_designator], 0) >
                        statborg->get_team_stat(game_engine_globals->team_mapping[best_team], 0))
                    best_team = team_designator;
            }
        }
        if (winner)
            *winner = game_engine_globals->team_mapping[best_team];
    }
    return result;
}
static __forceinline void territories_capture_progress_update(s_territory_data *territory, long team) {
territory->capturing_teams[team] = (unsigned char)(long)((float)(territory->capture_progress[team] / 6) * TLS_GAME_TIME_GLOBALS->tick_length);
}
static __forceinline void *territories_update_datum_get(s_data_array *data, long index)
{
    long byte_offset = (unsigned short)index;
    char *const &storage = data->data;
    // match: 0x2f1108: pins datum register colors and load order; without it the loads reorder and a later fmuls flips inputs
    __assume(storage != 0);
    byte_offset *= (long)sizeof(player_datum);
    // match: 0x2f1108: pins datum multiply/load order and register colors; without it they reorder and a later fmuls flips inputs
    __assume(byte_offset >= 0);
    void *result = storage + byte_offset;
    return result;
}
static __forceinline void territories_update_adjust_statistic(long player_index, bool adjust_team,
    e_game_results_statistic statistic, long amount)
{
    player_datum *player = (player_datum *)territories_update_datum_get(player_data, player_index);
    long absolute_index = (unsigned short)player_index;
    e_game_team team = adjust_team ? (e_game_team)player->configuration.host.team_index : _game_team_none;
    game_results_statistic_increment(absolute_index, team, statistic, amount);
}
// @address 0x2f1108 ?update_territory@@YAXJ@Z
void update_territory(long territory_index)
{
    s_territory_data *territory = &territories_globals()->areas[territory_index];
    long previous_flags = territory->flags;
    if (previous_flags != 1)
    {
        long team_player_counts[8] = { 0 };
        long team_count = 0;
        c_player_in_game_iterator iterator;
        // match: 0x2f1108: retain direct iterator reads and stores; accessor form moved an add operand tie (checks 2, 4, 6).
        iterator.m_iterator.m_iterator.absolute_index = -1;
        iterator.m_iterator.m_iterator.index = -1;
        iterator.m_iterator.m_iterator.data = player_data;
        while (iterator.next())
        {
            long player_index = iterator.m_iterator.m_iterator.index;
            if (player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)territory))
            {
                if (TLS_GAME_TIME_GLOBALS->time % TLS_GAME_TIME_GLOBALS->ticks_per_second == 0)
                    territories_update_adjust_statistic(player_index, true, _game_results_statistic_territories_time_in_territory, 1);
                e_multiplayer_team_designator team = game_engine_team_index_to_team_designator(
                    (e_game_team)((player_datum *)iterator.m_iterator.m_datum)->configuration.host.team_index);
                if (team_player_counts[team] == 0)
                    team_count++;
                team_player_counts[team]++;
            }
        }
        if (!territories_game_is_predicted())
        {
            if (team_count > 1)
                territory->flags = 2;
            else if (team_count == 0)
                territory->flags = 0;
        }
        for (long team = 0; team < 8; team++)
        {
            long player_count = team_player_counts[team];
            if (player_count > 0)
            {
                if (team_count == 1)
                {
                    if (team == territory->team_designator)
                    {
                        if (game_is_authoritative())
                            territory->flags = 0;
                    }
                    else
                    {
                        if (game_is_authoritative())
                            territory->flags = 3;
                        long progress = 6;
                        if (player_count == 2)
                            progress = 8;
                        else if (player_count >= 3)
                            progress = 9;
                        unsigned short *capture_progress = &territory->capture_progress[team];
                        *capture_progress += progress;
                        if (game_is_authoritative())
                        {
                            territories_capture_progress_update(territory, team);
                            if (territory->capture_progress[team] >= TLS_GAME_TIME_GLOBALS->ticks_per_second * g_territories_variant->m_capture_time * 6)
                                capture_territory(territory_index, (e_multiplayer_team_designator)team);
                            simulation_action_game_engine_globals_update(1 << (territory_index + 6));
                        }
                    }
                }
            }
            else if (territory->capture_progress[team] != 0)
            {
                territory->capture_progress[team] = (unsigned short)MAX(0, territory->capture_progress[team] - 6);
                if (game_is_authoritative())
                {
                    territories_capture_progress_update(territory, team);
                    simulation_action_game_engine_globals_update(1 << (territory_index + 6));
                }
            }
        }
        if (!territories_game_is_predicted() && previous_flags != territory->flags)
            simulation_action_game_engine_globals_update(1 << (territory_index + 6));
    }
    update_territory_flag_color(territory_index);
}

