// blamlibxbox/game_engine_notifications.obj: from the records 0x1116e8 0x275610 0x2757a8 0x275870 0x275a98 0x2768d0 0x276ba8 0x276cc0 0x276f88 0x276fe8 0x277148 0x277308 0x2773e0 0x2774c0 0x2775c8 0x2776f0 0x277890 0x277930 0x277a28 0x277b90 0x35cc78 0x275968
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_engine_notifications.h"
#include "memory/tls_globals.h"
#include "game/game_engine_kill_flags_types.h"
#include "game/player_with_unit_iterator.h"
#include "game/game_time_inline.h"
#include "camera/director.h"
#include "game/game_engine_waypoint_constants.h"
#include "units/unit_seat.h"
#include "units/unit_seat_flags.h"
#include "memory/data_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "items/multiplayer_weapon_tracker.h"
#include "units/motor_enums.h"
#include "cseries/cseries.h"
#include <stdlib.h>
#include <string.h>
#include "game/game.h"
#include "game/game_engine.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "game/game_engine_player_traits.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_results.h"
#include "game/game_engine_simulation.h"
#include "memory/data.h"
#include "motor/action_system.h"
#include "game/game_engine_event_data.h"
#include "game/player_types.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_default.h"
#include "items/item_types.h"
#include "objects/object_datum.h"
#include "items/weapon_definitions.h"
#include "game/game_time_types.h"
#include "game/game_engine_list.h"
#include "game/game_engine_get_inline.h"
#include "tag_files/tag_groups.h"
#include "units/unit_types.h"
#include "units/unit_definitions.h"
#include "main/main_game.h"
#include "camera/director_notification_types.h"
#include "camera/saved_film_director_storage.h"
#include "game/game_engine_teleporter_types.h"
#include "game/game_engine_notifications_dependencies.h"
#include "game/player_in_game_iterator.h"
#include "game/game_results_types.h"
#include "ai/weapon_datum_types.h"
#include "units/biped_types.h"
#include "physics/character_physics_mode_melee.h"
#include "game/game_engine_simulation_externs.h"
#include "memory/datum_index.h"
#include "objects/damage_reporting_types.h"

// @address 0x3356c k_game_engine_invalid_dead_point

// @address 0x275870 ?game_engine_player_nearby_item@@YAXJJ@Z
// @address 0x2774c0 ?game_engine_player_changed_indices@@YAXJJ@Z
// @address 0x2775c8 game_engine_player_prepare_to_change_team
// @address 0x2776f0 ?game_engine_player_changed_teams@@YAXJW4e_game_team@@0@Z

enum e_director_perspective;


struct s_game_engine_globals;
struct s_data_array;

// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0xa0a10 ?current_game_engine@@YAPBVc_game_engine@@XZ


// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z
// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ


static __forceinline bool notifications_game_is_predicted(void)
{
    bool result = false;
    if (tls_game_globals()->options.game_simulation == 4)
        result = true;
    return result;
}


static __forceinline int notifications_engine_index_out_of_range(int x)
{
    if ((x < 1)) return 1;
    if ((x > 10)) return 1;
    return 0;
}

static __forceinline bool notifications_area_set_contains(c_area_set<c_teleporter_area, 32> *table, int object_index)
{
    bool found = false;
    c_teleporter_area *area = table->m_areas.begin();
    c_teleporter_area *end = table->m_areas.begin() + table->m_area_count;
    for (; area != end; ++area)
    {
        if (area->m_object_index == object_index)
        {
            found = true;
            break;
        }
    }
    return found;
}


// @address 0x2757a8 game_engine_record_vehicle_exit
extern "C" void game_engine_record_vehicle_exit(long object_index, long weapon_index)
{
    unit_datum *unit = (unit_datum *)datum_get<s_object_header>(tls_object_header_data(), object_index)->datum;
    if (tls_game_globals()->options.game_mode == 2 && !notifications_game_is_predicted()) {
        long player_index = unit->unit.player_index;
        if (player_index != -1 && action_executing(object_index, _action_vehicle_ejection)) {
            player_datum *player = (player_datum *)datum_try_and_get(tls_player_data(), player_index);
            if (player) {
                player->last_vehicle_exit_index = weapon_index;
                player->last_vehicle_exit_time = game_time_get();
            }
        }
    }
}

void game_engine_player_nearby_item(long player_index, long object_index)
{
    if (game_engine_get()) {
        item_datum *item = (item_datum *)datum_get<s_object_header>(tls_object_header_data(), object_index)->datum;
        if ((1 << item->object.object_type) & 4) {
            weapon_definition *definition = (weapon_definition *)global_tag_instances[datum_index_to_absolute_index(item->object.definition_index)].definition;
            if (definition->multiplayer_weapon_type != 0) {
                c_game_engine *engine = (c_game_engine *)current_game_engine();
                engine->player_nearby_multiplayer_weapon(player_index, object_index);
                return;
            }
            item->relevance_time = game_time_get();
        }
    }
}

static __forceinline player_datum *notifications_player_get(long player_index)
{
    return datum_get<player_datum>(tls_player_data(), player_index);
}
static __forceinline unsigned short notifications_next_shot_identifier(void)
{
    unsigned short identifier = ((s_game_engine_globals *)game_engine_globals)->next_shot_identifier;
    ++((s_game_engine_globals *)game_engine_globals)->next_shot_identifier;
    return identifier;
}
// @address 0x276ba8 ?game_engine_notify_shot_fired@@YAFJE@Z
short game_engine_notify_shot_fired(long player_index, unsigned char flags)
{
    short result = -1;
    if (game_engine_get() && player_index != -1)
    {
        long maximum_age = game_tick_rate() * 60;
        player_datum *player = notifications_player_get(player_index);
        long selected_shot = 0;
        long oldest_age = 0;
        long time = game_time_get();
        for (long shot_index = 0; shot_index < 8; ++shot_index)
        {
            long age = time - player->shots[shot_index].time;
            if (player->shots[shot_index].identifier == 0xffff)
            {
                selected_shot = shot_index;
                break;
            }
            else if (age > maximum_age)
            {
                selected_shot = shot_index;
                break;
            }
            else if (age > oldest_age)
            {
                selected_shot = shot_index;
                oldest_age = age;
            }
        }
        result = notifications_next_shot_identifier();
        player->shots[selected_shot].identifier = result;
        player->shots[selected_shot].flags = flags;
        player->shots[selected_shot].time = time;
    }
    return result;
}

// @address 0x276f88 game_engine_notify_player_activated
extern "C" void game_engine_notify_player_activated(long object_index)
{
    c_game_engine *engine = game_engine_get();
    if (engine)
        engine->player_activated(object_index);
}


// @address 0xf6048 ?game_engine_get_statborg@@YAPAVc_game_statborg@@XZ
// @address 0x3a9a60 ?simulation_action_game_statborg_update@@YAXK@Z

__forceinline void c_game_statborg::player_indices_swapped(long player_index1, long player_index2)
{
    if (!notifications_game_is_predicted())
    {
        long absolute_player_index1 = player_index1;
        long absolute_player_index2 = player_index2;
        s_player_statborg_statistics temporary_stats = player_stats[absolute_player_index1];
        player_stats[absolute_player_index1] = player_stats[absolute_player_index2];
        player_stats[absolute_player_index2] = temporary_stats;
        simulation_action_game_statborg_update(1 << absolute_player_index1);
        simulation_action_game_statborg_update(1 << absolute_player_index2);
    }
}

// @address 0x244720 ?game_results_notify_player_indices_changed@@YAXXZ
static __forceinline void notifications_events_notify_player_deleted(long player_index)
{
    for (long event_index = 0; event_index < 64; ++event_index)
    {
        s_game_engine_queued_event *queued_event = &((s_game_engine_globals *)game_engine_globals)->queued_events[event_index];
        if (queued_event->flags.test(0) && !queued_event->flags.test(1) &&
            (queued_event->event.flags == player_index ||
             queued_event->event.cause_player_index == player_index ||
             queued_event->event.effect_player_index == player_index))
        {
            queued_event->flags.set(0, false);
        }
    }
}
// @address 0x277308 ?game_engine_player_deleted@@YAXJ@Z
void game_engine_player_deleted(long player_index)
{
    if (game_engine_get())
    {
        long absolute_player_index = (unsigned short)player_index;
        game_engine_get_statborg()->player_deleted(player_index);
        ::simulation_action_game_engine_player_update((short)absolute_player_index, 0x1ffff);
        game_results_notify_player_indices_changed();
        notifications_events_notify_player_deleted(player_index);
    }
}

// 0x2773e0
void game_engine_player_indices_swapped(long player_index1, long player_index2)
{
    if (game_engine_get())
        game_engine_get_statborg()->player_indices_swapped(player_index1, player_index2);
}


static __forceinline void notifications_events_player_changed_indices(long old_player_index, long new_player_index)
{
    for (long event_index = 0; event_index < 64; ++event_index)
    {
        s_game_engine_queued_event *queued_event = &((s_game_engine_globals *)game_engine_globals)->queued_events[event_index];
        if ((queued_event->flags.get_unsafe() & 1) != 0 && (queued_event->flags.get_unsafe() & 2) == 0)
        {
            if (queued_event->event.flags == old_player_index)
                queued_event->event.flags = new_player_index;
            if (queued_event->event.cause_player_index == old_player_index)
                queued_event->event.cause_player_index = new_player_index;
            if (queued_event->event.effect_player_index == old_player_index)
                queued_event->event.effect_player_index = new_player_index;
        }
    }
}
void game_engine_player_changed_indices(long old_player_index, long new_player_index)
{
    if (game_engine_get())
    {
        long old_absolute_player_index = (unsigned short)old_player_index;
        long new_absolute_player_index = (unsigned short)new_player_index;
        const c_game_engine *engine = game_engine_get();
        engine->player_changed_indices(old_player_index, new_player_index);
        ::simulation_action_game_engine_player_update((short)old_absolute_player_index, 0x1ffff);
        ::simulation_action_game_engine_player_update((short)new_absolute_player_index, 0x1ffff);
        game_results_notify_player_indices_changed();
        notifications_events_player_changed_indices(old_player_index, new_player_index);
    }
}

// @address 0x277ee8 ?game_engine_has_teams@@YA_NXZ

static __forceinline player_datum *notification_prepare_player_get(long index)
{
    return datum_get<player_datum>(tls_player_data(), index);
}

extern "C" void game_engine_player_prepare_to_change_team(long player_index, long new_team)
{
    if (game_engine_get() && game_engine_has_teams() && !notifications_game_is_predicted())
    {
        player_datum *player = notification_prepare_player_get(player_index);
        long respawn_ticks = player->respawn_timer > 2 * game_tick_rate()
            ? player->respawn_timer : 2 * game_tick_rate();
        if (new_team != player->configuration.host.team && player->unit_index != -1)
        {
            object_cause_default_damage(player->unit_index, 0, 1, 1, 0, 0);
            if (player->unit_index != -1)
                player->respawn_timer = respawn_ticks;
        }
    }
}


struct s_player_datum;


// @address 0xadef0 ?unit_get_head_position@@YAXJPATreal_point3d@@@Z

static __forceinline const c_game_variant *notifications_current_game_variant(void)
{
    const game_options *options = &tls_game_globals()->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}

static __forceinline long notifications_inherit_respawn_ticks(long player_index, long ticks, long earliest_ticks)
{
    player_datum *player = notification_prepare_player_get(player_index);
    long result = ticks;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        player_datum *other = (player_datum *)iterator.m_iterator.m_datum;
        if (other->respawn_timer > 0 &&
            !current_game_engine()->team_is_enemy((e_game_team)other->configuration.host.team,
            (e_game_team)player->configuration.host.team))
        {
            if (other->respawn_timer >= earliest_ticks && other->respawn_timer < result)
                result = other->respawn_timer;
        }
    }
    return result;
}

// @address 0x2768d0 ?game_engine_setup_player_for_respawn@@YAXJ@Z
void game_engine_setup_player_for_respawn(long player_index)
{
    player_datum *player = notification_prepare_player_get(player_index);
    s_game_engine_player_waypoint *waypoint = &((s_game_engine_globals *)game_engine_globals)->player_waypoints[(short)player_index];
    long respawn_ticks = game_seconds_to_ticks_round((float)notifications_current_game_variant()->respawn_time_seconds);
    long minimum_ticks = game_seconds_to_ticks_round(1.0f);
    long earliest_ticks = player->respawn_penalty_ticks > minimum_ticks ? player->respawn_penalty_ticks : minimum_ticks;
    long ticks = player->respawn_penalty_ticks + respawn_ticks > minimum_ticks ? player->respawn_penalty_ticks + respawn_ticks : minimum_ticks;
    if (game_engine_has_teams() && (notifications_current_game_variant()->respawn_option_flags & 1) != 0)
        ticks = notifications_inherit_respawn_ticks(player_index, ticks, earliest_ticks);
    if (notifications_current_game_variant()->respawn_growth_seconds == 0)
        player->respawn_penalty_ticks = 0;
    if (player->unit_index != -1)
    {
        waypoint->active = true;
        unit_get_head_position(player->unit_index, &waypoint->position);
    }
    else
    {
        waypoint->active = false;
        waypoint->position = k_game_engine_invalid_dead_point;
    }
    ::simulation_action_game_engine_player_update((short)player_index, 1);
    if (!notifications_game_is_predicted())
    {
        long time = game_time_get();
        long *last_time = &((s_game_engine_globals *)game_engine_globals)->last_respawn_time;
        long *count = &((s_game_engine_globals *)game_engine_globals)->respawn_count;
        if (time == *last_time)
            ticks += 2 * *count;
        else
            *count = 0;
        *last_time = time;
        (*count)++;
        game_engine_player_set_respawn_timer(player_index, ticks);
    }
    waypoint->ticks = (unsigned short)ticks;
    waypoint->initial_ticks = (unsigned short)ticks;
}

// @address 0x278530 ?game_engine_recompute_active_teams@@YAXXZ
// @address 0x208d20 ?game_engine_adjust_team_score_for_composition@@YAXW4e_game_team@@@Z
// @address 0x24fc40 ?game_engine_send_event@@YAXPAUs_game_engine_event_data@@@Z


void game_engine_player_changed_teams(long player_index, e_game_team team, e_game_team team2)
{
    if (game_engine_get() && game_engine_has_teams())
    {
        c_game_statborg *statborg = game_engine_get_statborg();
        if (!notifications_game_is_predicted())
        {
            unsigned short index = (unsigned short)player_index;
            statborg->player_stats[index].values[0] = 0;
            simulation_action_game_statborg_update(1 << index);
        }
        current_game_engine()->player_changed_teams(player_index);
        if (!notifications_game_is_predicted())
        {
            game_engine_recompute_active_teams();
            game_engine_adjust_team_score_for_composition(team);
            game_engine_adjust_team_score_for_composition(team2);
        }
        if (!notifications_game_is_predicted() && game_time_get() > 1)
        {
            s_game_engine_event_data event;
            event.type = 0;
            event.event = 24;
            event.identifier = -1;
            event.flags = -1;
            event.cause_player_index = -1;
            event.cause_team_index = -1;
            event.effect_player_index = -1;
            event.effect_team_index = -1;
            event.data = 0;
            event.data_word = -1;
            event.cause_player_index = player_index;
            event.cause_team_index = notification_prepare_player_get(player_index)->configuration.host.team;
            event.effect_team_index = team2;
            game_engine_send_event((s_game_engine_event_data *)&event);
        }
    }
}


// @address 0x277148 ?game_engine_player_rejoined@@YAXJ@Z
void game_engine_player_rejoined(long player_index)
{
    if (game_engine_get())
    {
        long absolute_player_index = datum_index_to_absolute_index(player_index);
        if (!notifications_game_is_predicted() && game_engine_has_teams())
        {
            player_datum *player = notification_prepare_player_get(player_index);
            game_engine_recompute_active_teams();
            game_engine_adjust_team_score_for_composition((e_game_team)player->configuration.host.team);
        }
        game_engine_get()->player_rejoined(player_index);
        game_engine_assemble_player_traits(player_index);
        if (!notifications_game_is_predicted())
            ::simulation_action_game_engine_player_update((short)absolute_player_index, 62);
        if (!notifications_game_is_predicted())
        {
            s_game_engine_event_data event;
            event.type = 0;
            event.event = 25;
            event.identifier = -1;
            event.flags = -1;
            event.cause_player_index = -1;
            event.cause_team_index = -1;
            event.effect_player_index = -1;
            event.effect_team_index = -1;
            event.data = 0;
            event.data_word = -1;
            event.cause_player_index = player_index;
            event.cause_team_index = notification_prepare_player_get(player_index)->configuration.host.team;
            game_engine_send_event((s_game_engine_event_data *)&event);
        }
        game_results_notify_player_indices_changed();
        game_results_statistic_set(absolute_player_index, _game_team_none, _game_results_statistic_games_played, 1);
    }
}

// @address 0x277890 game_engine_mark_user_unit_changed
extern "C" void game_engine_mark_user_unit_changed(long player_index, long object_index)
{
    if (!game_is_cooperative() &&
        ((1u << ((s_controllers *)g_director_globals)->slots[player_index].perspective) & 3) &&
        object_index != -1) {
        ((s_game_engine_globals *)game_engine_globals)->user_fade[player_index] = 1.0f;
        ((s_game_engine_globals *)game_engine_globals)->pending_flags |= 1 << player_index;
    }
}

// @address 0x20a238 ?game_engine_multiplayer_weapon_get_internal@@YAPAUs_multiplayer_weapon_tracker@@J@Z
static __forceinline weapon_datum *inventory_object_get(long object_index)
{
    return (weapon_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
}

// @address 0x1116e8 ?game_engine_handle_deleted_object@@YAXJ@Z
void game_engine_handle_deleted_object(long object_index)
{
    weapon_datum *object = inventory_object_get(object_index);
    int engine_index;
    c_game_engine *engine;
    c_area_set<c_teleporter_area, 32> *table;

    if (((1 << ((item_datum *)object)->object.object_type) & 4) != 0)
    {
        if ((unsigned char)((object->weapon_flags.get_unsafe() >> 6) & 1) != 0)
        {
            game_engine_multiplayer_weapon_deregister(object_index);
        }
    }

    engine_index = ((s_game_engine_globals *)game_engine_globals)->game_engine_index;
    if (!notifications_engine_index_out_of_range(engine_index))
    {
        engine = game_engines[engine_index];
        if (engine != 0)
        {
            engine->handle_deleted_object(object_index);
        }
    }

    if (main_game_globals.game_change_in_progress == 0)
    {
        table = (c_area_set<c_teleporter_area, 32> *)&((s_game_engine_globals *)game_engine_globals)->teleporter_slot_table;
        if (notifications_area_set_contains(table, object_index) != 0)
        {
            table->update();
        }
    }
}

static __forceinline s_multiplayer_weapon_tracker *notifications_multiplayer_weapon_tracker_get(long index)
{
    return &((s_game_engine_globals *)game_engine_globals)->multiplayer_weapons[index];
}
// 0x277930
void game_engine_multiplayer_weapon_register(long weapon_index, short multiplayer_weapon_identifier)
{
    if (game_engine_get())
    {
        weapon_datum *weapon = inventory_object_get(weapon_index);
        if (((s_game_engine_globals *)game_engine_globals)->multiplayer_weapon_count < 8u)
        {
            long tracker_index = ((s_game_engine_globals *)game_engine_globals)->multiplayer_weapon_count++;
            s_multiplayer_weapon_tracker *tracker = notifications_multiplayer_weapon_tracker_get(tracker_index);
            weapon->weapon_flags.set(6, true);
            weapon->multiplayer_weapon_identifier = multiplayer_weapon_identifier;
            ::simulation_action_object_update(weapon_index, 0x80000);
            tracker->weapon_index = weapon_index;
            tracker->id = multiplayer_weapon_identifier;
            tracker->carrier_unit_index = -1;
            tracker->carrier_player_index = -1;
            const c_game_engine *engine = current_game_engine();
            engine->multiplayer_weapon_register(weapon_index);
            if (weapon->item.unit_object_index != -1)
                game_engine_multiplayer_weapon_picked_up(weapon_index, weapon->item.unit_object_index);
        }
    }
}


// @address 0x277a28 ?game_engine_multiplayer_weapon_deregister@@YAXJ@Z
void game_engine_multiplayer_weapon_deregister(long weapon_index)
{
    if (game_engine_get())
    {
        weapon_datum *weapon = inventory_object_get(weapon_index);
        s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get_internal(weapon_index);
        if (tracker->carrier_unit_index != -1)
            game_engine_multiplayer_weapon_drop_internal(weapon_index, weapon->item.unit_object_index);
        const c_game_engine *engine = game_engine_get();
        engine->multiplayer_weapon_deregister(weapon_index);
        weapon->weapon_flags.set(6, false);
        weapon->multiplayer_weapon_identifier = -1;
        ::simulation_action_object_update(weapon_index, 0x80000);
        long tracker_index = tracker - &((s_game_engine_globals *)game_engine_globals)->multiplayer_weapons[0];
        if (tracker_index < (long)((s_game_engine_globals *)game_engine_globals)->multiplayer_weapon_count - 1)
        {
            s_multiplayer_weapon_tracker *destination = notifications_multiplayer_weapon_tracker_get(tracker_index);
            *destination = *notifications_multiplayer_weapon_tracker_get(((s_game_engine_globals *)game_engine_globals)->multiplayer_weapon_count - 1);
        }
        --((s_game_engine_globals *)game_engine_globals)->multiplayer_weapon_count;
    }
}

// @address 0x277b90 ?game_engine_multiplayer_weapon_picked_up@@YAXJJ@Z
void game_engine_multiplayer_weapon_picked_up(long weapon_index, long unit_index)
{
    if (game_engine_get()) {
        weapon_datum *weapon = inventory_object_get(weapon_index);
        unit_datum *unit = (unit_datum *)inventory_object_get(unit_index);
        s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get_internal(weapon_index);
        tracker->carrier_unit_index = unit_index;
        tracker->carrier_player_index = unit->unit.player_index;
        weapon->weapon_flags.set(7, true);
        game_engine_get()->multiplayer_weapon_picked_up(weapon_index, unit_index);
    }
}

// @address 0x35cc78 ?item_update_player_inventory@@YAXJ@Z
void item_update_player_inventory(long weapon_index)
{
    weapon_datum *weapon = inventory_object_get(weapon_index);
    bool owned_by_player = false;
    if (weapon->item.owner_relation != 0)
        owned_by_player = ((unit_datum *)inventory_object_get(weapon->item.unit_object_index))->unit.player_index != -1;
    if (owned_by_player)
        weapon->item.inventory_flags |= 2;
    else
        weapon->item.inventory_flags &= ~2;
    if ((1 << ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(weapon_index)].object_type) & 4) {
        if ((unsigned char)((weapon->weapon_flags.get_unsafe() >> 6) & 1) && game_engine_get()) {
            s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get_internal(weapon_index);
            if (tracker->carrier_unit_index != -1)
                tracker->carrier_player_index = ((unit_datum *)inventory_object_get(tracker->carrier_unit_index))->unit.player_index;
        }
    }
}

// @address 0x2a64c8 game_results_try_and_grant_achievement

static __forceinline void notifications_event_initialize(long type, long event, s_game_engine_event_data *data)
{
    data->type = type;
    data->event = event;
    data->identifier = -1;
    data->flags = -1;
    data->cause_player_index = -1;
    data->cause_team_index = -1;
    data->effect_player_index = -1;
    data->effect_team_index = -1;
    data->data = 0;
    data->data_word = -1;
}

static __forceinline void notifications_event_set_cause(long player_index, s_game_engine_event_data *data)
{
    data->cause_player_index = player_index;
    data->cause_team_index = datum_get<player_datum>(tls_player_data(), player_index)->configuration.host.team_index;
}


// @address 0x276fe8 ?game_engine_player_left@@YAXJ@Z
void game_engine_player_left(long player_index)
{
    if (game_engine_get())
    {
        if (!notifications_game_is_predicted() && game_engine_has_teams())
        {
            player_datum *player = notification_prepare_player_get(player_index);
            game_engine_recompute_active_teams();
            game_engine_adjust_team_score_for_composition((e_game_team)player->configuration.host.team);
        }
        game_engine_get()->player_left(player_index);
        s_game_engine_event_data event;
        notifications_event_initialize(0, 11, &event);
        event.cause_player_index = player_index;
        event.cause_team_index = notification_prepare_player_get(player_index)->configuration.host.team;
        game_engine_handle_event(&event, false);
        game_results_notify_player_indices_changed();
        ::simulation_action_game_engine_player_update((short)player_index, 0x2000);
    }
}

// @address 0x275610 ?game_engine_unit_entered_vehicle@@YAXJJ@Z
void game_engine_unit_entered_vehicle(long unit_index, long vehicle_index)
{
    game_globals_storage *globals = tls_game_globals();
    unit_datum *unit = (unit_datum *)datum_get<s_object_header>(tls_object_header_data(), unit_index)->datum;
    if (globals->options.game_mode == 2 && !notifications_game_is_predicted()) {
        if (unit->unit.player_index != -1 && unit->unit.parent_seat_index != -1) {
            item_datum *vehicle = (item_datum *)datum_get<s_object_header>(tls_object_header_data(), vehicle_index)->datum;
            unit_definition *definition = (unit_definition *)global_tag_instances[datum_index_to_absolute_index(vehicle->object.definition_index)].definition;
            if ((bool)(_rotl(definition->seats[unit->unit.parent_seat_index].flags, 21) & 1)) {
                s_game_engine_event_data event;
                notifications_event_initialize(0, 45, &event);
                notifications_event_set_cause(unit->unit.player_index, &event);
                game_engine_send_event(&event);
                long player_index = unit->unit.player_index;
                player_datum *player = (player_datum *)datum_try_and_get(tls_player_data(), player_index);
                if (player != 0 && player->last_vehicle_exit_index == vehicle_index) {
                    float elapsed = (float)(double)(int)(game_time_get() - player->last_vehicle_exit_time) * game_tick_length();
                    if (!(elapsed > 1e+01f))
                        game_results_try_and_grant_achievement(player_index, 43);
                }
            }
        }
    }
}

static __forceinline float notifications_game_ticks_to_seconds(long ticks)
{
    return (float)ticks * game_tick_length();
}

__forceinline void c_game_statborg::player_added(long player_index)
{
    if (!notifications_game_is_predicted())
    {
        long absolute_player_index = datum_index_to_absolute_index(player_index);
        memset(&player_stats[absolute_player_index], 0, sizeof(s_player_statborg_statistics));
        simulation_action_game_statborg_update(1 << absolute_player_index);
    }
}

// @address 0x276cc0 ?game_engine_player_added@@YAXJ@Z
void game_engine_player_added(long player_index)
{
    if (game_engine_get())
    {
        long absolute_player_index = datum_index_to_absolute_index(player_index);
        if (!notifications_game_is_predicted() && game_engine_has_teams())
        {
            player_datum *player = notification_prepare_player_get(player_index);
            game_engine_recompute_active_teams();
            game_engine_adjust_team_score_for_composition((e_game_team)player->configuration.host.team);
        }
        game_engine_assemble_player_traits(player_index);
        game_engine_get()->player_added(player_index);
        game_engine_get_statborg()->player_added(player_index);
        if (!notifications_game_is_predicted() && notifications_game_ticks_to_seconds(game_time_get()) > 1.0f)
        {
            s_game_engine_event_data event;
            notifications_event_initialize(0, 12, &event);
            notifications_event_set_cause(player_index, &event);
            game_engine_send_event(&event);
        }
        if (!notifications_game_is_predicted())
        {
            player_datum *player = notification_prepare_player_get(player_index);
            player->lives = notifications_current_game_variant()->lives_per_round;
            ::simulation_action_game_engine_player_update((short)absolute_player_index, 0x200);
        }
        long user_index = -1;
        do
        {
            if (player_index != -1)
                user_index = ((long)::director_get_perspective(player_index, user_index));
            if (user_index != -1)
            {
                ((s_game_engine_globals *)game_engine_globals)->user_fade[user_index] = 1.0f;
                ((s_game_engine_globals *)game_engine_globals)->pending_flags |= 1 << user_index;
            }
        } while (user_index != -1);
        game_results_notify_player_indices_changed();
        game_results_statistic_set(absolute_player_index, _game_team_none, _game_results_statistic_games_played, 1);
    }
}


struct e_game_results_damage_type_description;

static __forceinline player_datum *notifications_kill_player_get(long index)
{
    return datum_get<player_datum>(tls_player_data(), index);
}
static __forceinline unit_datum *notifications_kill_object_get(long index)
{
    return (unit_datum *)((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(index)].datum;
}
static __forceinline void notifications_kill_event_effect(long player_index, s_game_engine_event_data *event)
{
    event->effect_player_index = player_index;
    event->effect_team_index = (e_game_team)notifications_kill_player_get(player_index)->configuration.host.team;
}


static __forceinline long notifications_kill_inventory_weapon(const unit_datum *unit, short weapon_slot)
{
    return weapon_slot == -1 ? -1 : unit->unit.weapon_object_indices[weapon_slot];
}
static __forceinline bool notifications_kill_unit_is_action(s_object_header *header)
{
    unit_datum *unit = (unit_datum *)header->datum;
    return unit &&
        ((biped_datum *)unit)->biped.physics.get_mode() == c_character_physics_component::_mode_melee &&
        ((biped_datum *)unit)->biped.physics.get_mode_melee()->is_fast();
}
static __forceinline long notifications_kill_shared_lives(e_game_team team)
{
    return (signed char)((s_game_engine_globals *)game_engine_globals)->shared_team_lives[team];
}
static __forceinline void notifications_kill_consume_shared_life(e_game_team team)
{
    --((s_game_engine_globals *)game_engine_globals)->shared_team_lives[team];
}
static __forceinline long notifications_kill_elapsed()
{
    if (game_time_get() - ((s_game_engine_globals *)game_engine_globals)->round_start_time < 0)
        return 0;
    return game_time_get() - ((s_game_engine_globals *)game_engine_globals)->round_start_time;
}

// @rdata 0x82022dc0 4
static const float k_kill_notification_rounding = 0.5f;
// @rdata 0x8202215c 4
static const float k_kill_notification_airborne_time = 0.95f;
#if !defined(_MSC_VER) || defined(__clang__)
// Invalid dead-point sentinel owned by game_engine_notifications.obj; the PC link needs this
// definition, while the Xbox build takes the bytes from the image splice (address tag above).
// The words below are the shipped .rdata bytes for the link only.
extern "C" {
const real_point3d k_game_engine_invalid_dead_point = { 0.0f, 0.0f, 500.0f };
}
#endif
static __forceinline long notifications_kill_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)game_tick_rate() + k_kill_notification_rounding);
}

static __forceinline bool notifications_kill_use_shared_life(e_game_team team)
{
    bool result = false;
    if (::game_engine_is_team_active(team) && ((s_game_engine_globals *)game_engine_globals)->shared_team_lives[team] > 0)
    {
        result = true;
        ((s_game_engine_globals *)game_engine_globals)->shared_team_lives[team]--;
        simulation_action_game_engine_globals_update(2);
    }
    return result;
}

static __forceinline void notifications_kill_apply_penalties(long killer, long victim, player_datum *victim_player)
{
    if (killer != -1)
    {
        bool enemy = game_engine_team_is_enemy((e_game_team)notifications_kill_player_get(killer)->configuration.host.team,
            (e_game_team)victim_player->configuration.host.team);
        if (killer == victim)
        {
            const c_game_variant *variant = (const c_game_variant *)notifications_current_game_variant();
            victim_player->respawn_penalty_ticks += notifications_kill_seconds_to_ticks_round((float)variant->suicide_penalty_seconds);
        }
        else if (!enemy && !victim_player->respawn_timer)
        {
            player_datum *killer_player = notifications_kill_player_get(killer);
            const c_game_variant *variant = (const c_game_variant *)notifications_current_game_variant();
            killer_player->respawn_penalty_ticks += notifications_kill_seconds_to_ticks_round((float)variant->betrayal_penalty_seconds);
        }
    }
}

// @address 0x275a98 game_engine_notify_player_killed
extern "C" void game_engine_notify_player_killed(long killer,
    long dead_unit,
    long victim,
    bool betrayal,
    unsigned char damage,
    bool airborne)
{
    if (!game_engine_get())
        return;
    short victim_absolute_index = (short)victim;
    player_datum *victim_player = notifications_kill_player_get(victim);
    long killer_streak = 0;
    long damage_type = damage >> 2;
    long damage_flags = damage & 3;
    long damage_class = 0;
    if ((unsigned long)damage_type <= 60)
        damage_class = ((const long *)g_game_results_damage_type_descriptions)[damage_type];
    long message = -1;
    c_flags<e_game_engine_kill_flags, long, 24> kill_flags;
    kill_flags.clear();
    victim_player->last_death_round_ticks = notifications_kill_elapsed();
    notifications_kill_apply_penalties(killer, victim, victim_player);
    if (tls_game_globals()->options.game_mode == 2 &&
        !notifications_game_is_predicted() &&
        game_engine_in_round() &&
        victim_player->lives != -1)
    {
        victim_player->lives = (short)MAX(victim_player->lives - 1, 0);
        if (victim_player->lives == 0 && game_engine_has_teams())
        {
            if (notifications_kill_use_shared_life((e_game_team)victim_player->configuration.host.team))
                ++victim_player->lives;
        }
        simulation_action_game_engine_player_update(victim_absolute_index, 0x200);
    }
    game_engine_setup_player_for_respawn(victim);
    if (damage_type != 58)
    {
        if (victim_player->flags & 2)
            message = 11;
        else if (killer == -1)
        {
            if (damage_type == 1) message = 35;
            else if (damage_type != 33) message = 13;
        }
        else if (killer == victim)
            message = damage_type == 1 ? 35 : 1;
        else if (betrayal)
            message = 2;
        else
        {
            if (damage_class == 3)
            {
                kill_flags.set(_game_engine_kill_flag_melee, true);
                if (damage_flags == 2)
                {
                    message = 44;
                    kill_flags.set(_game_engine_kill_flag_assassination, true);
                }
                else
                    message = 37;
            }
            else if (damage_type == 2 || damage_flags == 3)
            {
                message = 36;
                kill_flags.set(_game_engine_kill_flag_splatter, true);
            }
            else if (damage_class == 4 && damage_flags == 2)
            {
                message = 42;
                kill_flags.set(_game_engine_kill_flag_sticky, true);
            }
            else if (damage_flags == 1 && (damage_type == 14 || damage_type == 15))
                message = 43;
            else if (damage_type == 23 || damage_type == 30)
                message = 55;
            else if (damage_type == 21)
                message = 54;
            else
                message = 0;
            kill_flags.set(_game_engine_kill_flag_headshot, damage_flags == 1);
            if (!game_engine_are_players_tied() && game_engine_get_player_place(victim) == 0)
                kill_flags.set(_game_engine_kill_flag_leader, true);
            if (game_engine_has_teams())
            {
                e_game_team team = (e_game_team)victim_player->configuration.host.team;
                if (::game_engine_is_team_active(team) && game_engine_teams_get_number_of_players_alive(team) == 1)
                    kill_flags.set(_game_engine_kill_flag_team_elimination, true);
            }
            if (!game_engine_has_teams())
            {
                long count = 0;
                c_player_with_unit_iterator iterator;
                iterator.iterator.data = tls_player_data();
                iterator.iterator.absolute_index = -1;
                iterator.iterator.index = -1;
                while (iterator.next())
                    ++count;
                if (count == 2) kill_flags.set(_game_engine_kill_flag_solo_elimination, true);
            }
        }
    }
    long override_message = game_engine_get()->player_killed_player_get_kill_message(killer, victim, betrayal);
    if (override_message != -1)
        message = override_message;
    if (killer != -1 && killer != victim && !betrayal)
    {
        long absolute_killer = datum_index_to_absolute_index(killer);
        c_game_statborg *statborg = game_engine_get_statborg();
        const s_player_statborg_statistics *killer_stats = &statborg->player_stats[absolute_killer];
        const s_player_statborg_statistics *victim_stats = &statborg->player_stats[datum_index_to_absolute_index(victim)];
        long multiple = killer_stats->values[12];
        long streak = killer_stats->values[11];
        long team_streak = (short)*(long *)&killer_stats->values[16];
        long sniper = killer_stats->values[18];
        long shotgun = killer_stats->values[19];
        long sword = killer_stats->values[20];
        long vehicle = killer_stats->values[21];
        bool ended_streak = victim_stats->values[11] >= 5 || victim_stats->values[18] >= 5 ||
            victim_stats->values[19] >= 5 || victim_stats->values[20] >= 5 || victim_stats->values[21] >= 5;
        killer_streak = streak;
        if (multiple == 10) ::game_engine_handle_time_messages(10, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (multiple == 9) ::game_engine_handle_time_messages(9, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (multiple == 8) ::game_engine_handle_time_messages(8, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (multiple == 7) ::game_engine_handle_time_messages(7, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (multiple == 6) ::game_engine_handle_time_messages(6, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (multiple == 5) ::game_engine_handle_time_messages(5, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (multiple == 4) ::game_engine_handle_time_messages(4, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (multiple == 3) ::game_engine_handle_time_messages(3, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (multiple == 2) ::game_engine_handle_time_messages(2, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        if (streak == 5) ::game_engine_handle_time_messages(11, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (streak == 10) ::game_engine_handle_time_messages(12, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (streak == 15) ::game_engine_handle_time_messages(13, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (streak == 20) ::game_engine_handle_time_messages(14, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (streak == 25) ::game_engine_handle_time_messages(15, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (streak == 30) ::game_engine_handle_time_messages(16, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        if (sniper == 5) ::game_engine_handle_time_messages(17, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (sniper == 10) ::game_engine_handle_time_messages(18, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        if (shotgun == 5) ::game_engine_handle_time_messages(19, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (shotgun == 10) ::game_engine_handle_time_messages(20, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        if (sword == 5) ::game_engine_handle_time_messages(23, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (sword == 10) ::game_engine_handle_time_messages(24, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        if (vehicle == 5) ::game_engine_handle_time_messages(21, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        else if (vehicle == 10) ::game_engine_handle_time_messages(22, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        if (team_streak >= 4 && game_engine_has_teams())
        {
            e_game_team team = (e_game_team)victim_player->configuration.host.team;
            if (::game_engine_is_team_active(team) && game_engine_teams_get_number_of_players_alive(team) <= 1)
                ::game_engine_handle_time_messages(0, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        }
        if (damage_flags == 0 || damage_flags == 1)
        {
            switch (damage_type)
            {
            case _damage_reporting_type_sniper_rifle: case _damage_reporting_type_beam_rifle:
                if (damage_flags == 1) ::game_engine_handle_time_messages(41, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
                break;
            case _damage_reporting_type_flame_thrower:
            case _damage_reporting_type_firebomb_grenade:
                ::game_engine_handle_time_messages(44, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
                break;
            case _damage_reporting_type_spartan_laser:
                ::game_engine_handle_time_messages(39, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
                break;
            }
        }
        if (victim_player->unit_index != -1)
        {
            s_object_header *headers = ((s_object_header *)tls_object_header_data()->data);
            s_object_header *unit_header = &headers[datum_index_to_absolute_index(victim_player->unit_index)];
            if (unit_header->object_type == 0)
            {
                unit_datum *unit = (unit_datum *)unit_header->datum;
                long weapon_index = notifications_kill_inventory_weapon(unit, unit->unit.current_weapon_index);
                if (weapon_index != -1)
                {
                    s_object_header *weapon_header = &headers[datum_index_to_absolute_index(weapon_index)];
                    if (weapon_header->object_type == 2)
                    {
                        weapon_definition *definition =
                            (weapon_definition *)global_tag_instances[datum_index_to_absolute_index(weapon_header->datum->definition_index)]
                                .definition;
                        bool action = notifications_kill_unit_is_action(unit_header);
                        if (definition->animation_weapon_class == 0x306 && action && damage_type != 25)
                        { ::game_engine_handle_time_messages(33,
                            absolute_killer,
                            killer,
                            victim,
                            (int *)((int *)&kill_flags)); message = 50; }
                    }
                }
            }
        }
        if (damage_type == 2 || damage_flags == 3)
            ::game_engine_handle_time_messages(34, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        if (damage_class == 4 && damage_flags == 2)
            ::game_engine_handle_time_messages(40, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        player_datum *killer_player = notifications_kill_player_get(killer);
        if (killer_player->unit_index == -1)
        {
            if ((float)killer_player->airborne_ticks * game_tick_length() > k_kill_notification_airborne_time && airborne)
            { ::game_engine_handle_time_messages(37, absolute_killer, killer, victim, (int *)((int *)&kill_flags)); message = 51; }
        }
        else if (game_engine_has_teams())
        {
            unit_datum *unit = notifications_kill_object_get(killer_player->unit_index);
            long parent = unit->object.parent_object_index;
            if (parent != -1 && (unsigned short)unit->unit.parent_seat_index != 0xffff)
            {
                short seat = unit->unit.parent_seat_index;
                long unit_index = killer_player->unit_index;
                long root;
                do { root = unit_index; unit_index = notifications_kill_object_get(root)->object.parent_object_index; } while (unit_index != -1);
                if (!unit_seat_test_flag(parent, (short)seat, _unit_seat_driver_bit) || root != parent)
                {
                    short driver_seat = ::unit_get_driver_seat(root);
                    if (driver_seat != -1)
                    {
                        long driver_index = unit_seat_get_occupant(root, driver_seat);
                        s_object_header *headers = ((s_object_header *)tls_object_header_data()->data);
                        if (driver_index != -1 && ((1 << headers[datum_index_to_absolute_index(driver_index)].object_type) & 0x1003))
                        {
                            unit_datum *driver = (unit_datum *)headers[datum_index_to_absolute_index(driver_index)].datum;
                            long player_index = driver->unit.player_index;
                            if (player_index != -1 && (unsigned long)(datum_index_to_absolute_index(player_index)) <= 15 &&
                                !game_engine_team_is_enemy((e_game_team)killer_player->configuration.host.team,
                                    (e_game_team)notifications_kill_player_get(datum_index_to_absolute_index(player_index))->configuration.host.team))
                            {
                                ::game_engine_handle_time_messages(45,
                                    datum_index_to_absolute_index(player_index),
                                    driver->unit.player_index,
                                    killer,
                                    (int *)((int *)&kill_flags));
                                if (!notifications_game_is_predicted())
                                {
                                    s_game_engine_event_data event;
                                    notifications_event_initialize(0, 56, &event);
                                    notifications_event_set_cause(driver->unit.player_index, &event);
                                    event.effect_player_index = killer;
                                    event.effect_team_index = (e_game_team)notifications_kill_player_get(killer)->configuration.host.team;
                                    game_engine_send_event(&event);
                                }
                            }
                        }
                    }
                }
            }
        }
        if (ended_streak)
            ::game_engine_handle_time_messages(38, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        if (damage_class == 3)
        {
            if (kill_flags.test(_game_engine_kill_flag_assassination))
                ::game_engine_handle_time_messages(42, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
            else
                ::game_engine_handle_time_messages(43, absolute_killer, killer, victim, (int *)((int *)&kill_flags));
        }
    }
    if (!notifications_game_is_predicted() && message != -1)
    {
        s_game_engine_event_data event;
        notifications_event_initialize(0, message, &event);
        if (killer != -1)
            notifications_event_set_cause(killer, &event);
        notifications_kill_event_effect(victim, &event);
        game_engine_send_event(&event);
    }
    game_engine_get()->player_killed_player(
        killer, dead_unit, victim, betrayal, c_flags<e_game_engine_kill_flags, long, 24>(kill_flags.get_unsafe()), killer_streak);
}


static __forceinline void *notifications_datum_try_and_get(const s_data_array *data, long datum_index)
{
    void *result = 0;
    if (datum_index != -1) {
        long absolute_index = datum_index & 0xffff;
        if (absolute_index < data->first_unallocated) {
            short *datum = (short *)(data->data + data->size * absolute_index);
            long identifier = *datum;
            if (identifier != 0 && identifier == (datum_index >> 16))
                result = datum;
        }
    }
    return result;
}

// @address 0x275968 ?game_engine_player_damaged_player@@YAXJJJ_N@Z
void game_engine_player_damaged_player(long player_index, long damage_index, long damaged_player_index, bool dead)
{
    if (notifications_datum_try_and_get(tls_player_data(), player_index) == 0)
        player_index = -1;
    if (notifications_datum_try_and_get(tls_player_data(), damaged_player_index) == 0)
        damaged_player_index = -1;
    else
        game_engine_player_set_fade_state(damaged_player_index, 3);
    if (game_engine_get() != 0 && damaged_player_index != -1)
    {
        const c_game_engine *engine = current_game_engine();
        engine->player_damaged_player(player_index, damage_index, damaged_player_index, dead);
    }
}
