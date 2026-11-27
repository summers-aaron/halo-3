// networkinglibxbox/simulation_players.obj: from the records 0xa5428 0x3bde18 0x410250 0x4102e0 0x410348 0x410480 0x410560 0x4107e0 0x4108d0 0x410a78 0x410b68 0x410fa0
// @flags /TP /O2 /Oc
#include "memory/data_types.h"
// @address 0x410348 simulation_player_collection_build
// @address 0x410480 ?simulation_player_collection_get_in_game_mask@@YAKPBUs_player_collection@@@Z
// @address 0x4107e0 simulation_player_update_apply_left_game
// @address 0x4108d0 simulation_player_update_apply_swap
// @address 0x410fa0 ?simulation_player_update_apply_configuration@@YA_NPBUsimulation_player_update@@@Z
#include <string.h>
#include "game/game_time_types.h"
#include "game/game_time_access.h"
#include "game/players.h"
#include "simulation/simulation_players.h"
#include "memory/data.h"
#include "game/player_types.h"
#include "simulation/game_interface/simulation_game_interface.h"
#include "game/player_types.h"
#include "simulation/simulation_players_types.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"


#include "simulation/simulation_players_types.h"
#include "networking/replication/replication_motion.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_distributed_view.h"
#include "game/players_globals.h"
#include "simulation/simulation_view.h"
#include "networking/replication/replication_encoding.h"

struct s_game_state;
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "game/player_configuration.h"
namespace {
static __forceinline bool simulation_players_is_authority(const c_simulation_world *world)
{
    return !(world->m_world_type == 4 || world->m_world_type == 6 || world->m_world_type == 8);
}
static __forceinline c_simulation_view *simulation_players_get_authority_view(const c_simulation_world *world)
{
    c_simulation_view *view = 0;
    s_simulation_world_view_iterator iterator;
    iterator.type_mask = 10;
    iterator.index = 0;
    world->iterator_next(&iterator, &view);
    return view;
}
}

// @address 0xa5428 ?generate_action@c_simulation_player@@QAA_NPAUplayer_action@@PAK@Z
bool c_simulation_player::generate_action(player_action *action, unsigned long *flags)
{
    bool result = false;
    switch (m_state) {
    case 4:
    case 5: {
        c_simulation_view *view;
        if (simulation_players_is_authority(m_world))
            view = m_world->get_client_view_by_machine_identifier(&m_machine_identifier);
        else
            view = simulation_players_get_authority_view(m_world);
        if (view) {
            c_replication_control_view *control = &view->m_distributed_view->control;
            if (control) {
                s_player_motion motion;
                if (control->retrieve_motion(m_control_index, &motion)) {
                    player_action simulated_action;
                    simulation_game_reconstruct_simulated_action(&motion, &simulated_action);
                    m_last_action = simulated_action;
                    m_last_action_flags = 0;
                    m_last_time = game_time_get();
                }
            }
        }
        break;
    }
    default:
        break;
    }
    if (m_state != 2 && m_last_time != -1) {
        float timeout = g_network_configuration.simulation_actor_control_timeout;
        if (game_ticks_to_seconds((float)(game_time_get() - m_last_time)) < timeout &&
            player_is_action_valid(&m_last_action))
        {
            *action = m_last_action;
            *flags = m_last_action_flags;
            result = true;
        }
    }
    return result;
}

bool c_simulation_player::generate_prediction(s_player_prediction *prediction)
{
    bool result = false;
    long control_index = m_control_index;
    if (m_state == 4)
    {
        c_simulation_view *view = m_world->get_client_view_by_machine_identifier(&m_machine_identifier);
        if (view)
        {
            c_replication_control_view *control = &view->m_distributed_view->control;
            if (control && control->retrieve_prediction(control_index, prediction))
                result = player_prediction_valid(prediction);
        }
    }
    return result;
}

// @address 0x410250 ?build_motion_data@c_simulation_player@@QBA_NPBUplayer_action@@_NPAUs_player_motion@@@Z
bool c_simulation_player::build_motion_data(const player_action *action, bool force, s_player_motion *motion) const
{
    player_datum *datum = (player_datum *)(tls_player_data()->data + 0x370 * (unsigned short)m_player_index);
    simulation_game_build_motion_data_from_action(action, datum->unit_index, motion);
    if (force)
        return true;
    return datum->unit_index != -1;
}

// @address 0x4102e0 ?simulation_player_collection_clear@@YAXPAUs_player_collection@@@Z
void simulation_player_collection_clear(s_player_collection *collection)
{
    memset(collection, 0, sizeof(*collection));
    for (long player_index = 0; player_index < 16; player_index++) {
        s_player_entry *player = &collection->players[player_index];
        player->left_game = false;
        player->left_game_time = -1;
        player->machine_user_index = -1;
        player->machine_index = -1;
    }
}

void simulation_player_collection_build(s_player_collection *collection)
{
    c_data_iterator<player_datum> it;
    it.begin(tls_player_data());
    while (it.next()) {
        int absolute_index = (unsigned short)it.get_index();
        player_datum *player = it.get_datum();
        s_player_entry *entry = &collection->players[absolute_index];
        collection->valid_mask |= 1 << absolute_index;
        entry->identifier = player->player_identifier;
        if (player->flags & 2) {
            entry->left_game = true;
            entry->left_game_time = player->left_game_time;
            memset(&entry->machine_identifier, 0, sizeof(entry->machine_identifier));
            entry->machine_user_index = -1;
            entry->machine_index = -1;
        } else {
            entry->left_game = false;
            entry->left_game_time = -1;
            entry->machine_identifier = player->machine_identifier;
            entry->machine_user_index = player->controller_index;
            entry->machine_index = player->user_index;
            entry->configuration = player->desired_configuration;
        }
    }
}

unsigned long simulation_player_collection_get_in_game_mask(const s_player_collection *collection)
{
    unsigned long in_game_mask = 0;
    unsigned long valid_mask = collection->valid_mask;
    for (long player_index = 0; player_index < 16; player_index++) {
        if (valid_mask & (1 << player_index)) {
            if (!collection->players[player_index].left_game)
                in_game_mask |= 1 << player_index;
            else
                in_game_mask &= ~(1 << player_index);
        }
    }
    return in_game_mask;
}

// @address 0x410560 ?simulation_player_collection_apply_update@@YAXPAUs_player_collection@@PBUsimulation_player_update@@@Z
void simulation_player_collection_apply_update(s_player_collection *collection, const simulation_player_update *update)
{
    s_player_entry *player = &collection->players[update->player_index];
    switch (update->update_type) {
    case 0:
        player->left_game = true;
        player->left_game_time = game_time_get();
        memset(&player->machine_identifier, 0, sizeof(player->machine_identifier));
        player->machine_index = -1;
        player->machine_user_index = -1;
        break;
    case 1: {
        s_player_entry *swap_player = &collection->players[update->swap_player_index];
        bool player1_valid = (collection->valid_mask & (1 << update->player_index)) != 0;
        bool player2_valid = (collection->valid_mask & (1 << update->swap_player_index)) != 0;
        bool player1_left_game = player->left_game;
        bool player2_left_game = swap_player->left_game;
        long player1_left_game_time = player->left_game_time;
        long player2_left_game_time = swap_player->left_game_time;
        if (player2_valid)
            collection->valid_mask |= 1 << update->player_index;
        else
            collection->valid_mask &= ~(1 << update->player_index);
        player->identifier = update->swap_player_identifier;
        player->left_game = player2_left_game;
        player->left_game_time = player2_left_game_time;
        if (player1_valid)
            collection->valid_mask |= 1 << update->swap_player_index;
        else
            collection->valid_mask &= ~(1 << update->swap_player_index);
        swap_player->identifier = update->player_identifier;
        swap_player->left_game = player1_left_game;
        swap_player->left_game_time = player1_left_game_time;
        break;
    }
    case 2:
        collection->valid_mask &= ~(1 << update->player_index);
        memset(&player->identifier, 0, sizeof(player->identifier));
        player->left_game = false;
        player->left_game_time = -1;
        break;
    case 3:
        if (collection->valid_mask & (1 << update->player_index)) {
            player->left_game = false;
            player->left_game_time = -1;
        } else {
            collection->valid_mask |= 1 << update->player_index;
            player->identifier = update->player_identifier;
            if (update->player_left_game) {
                player->left_game = true;
                player->left_game_time = game_time_get();
            }
        }
        player->machine_identifier = update->machine_identifier;
        player->machine_index = update->machine_user_index;
        player->machine_user_index = update->machine_controller_index;
        player->configuration = update->player_data;
        break;
    case 4:
        player->configuration = update->player_data;
        break;
    default:
        // match: 0x410560: unreachable hint omits dead branch check
        __assume(0);
    }
}

namespace {
static __forceinline player_datum *simulation_player_update_datum_try_get(long absolute_index)
{
    player_datum *result = 0;
    s_data_array *array = tls_player_data();
    if (absolute_index != -1 && absolute_index >= 0 && absolute_index < array->first_unallocated) {
        player_datum *datum = (player_datum *)(array->data + array->size * absolute_index);
        if (datum->identifier != 0)
            result = datum;
    }
    return result;
}
}

bool simulation_player_update_apply_left_game(const simulation_player_update *update)
{
    long index = update->player_index;
    s_data_array *array = tls_player_data();
    player_datum *datum = 0;
    if (index != -1 && index >= 0 && index < array->first_unallocated) {
        player_datum *candidate = (player_datum *)(array->data + array->size * index);
        if (candidate->identifier != 0) datum = candidate;
    }
    bool result = false;
    if (datum != 0) {
        if (memcmp(datum->player_identifier.data, &update->player_identifier, sizeof(datum->player_identifier.data)) == 0 && !(datum->flags & 2)) {
            long identifier = -1;
            if (index != -1)
                identifier = (*(unsigned short *)(array->data + array->size * index) << 16) | index;
            ::player_leave_game_internal(identifier);
            result = true;
        }
    }
    return result;
}

bool simulation_player_update_apply_swap(const simulation_player_update *update)
{
    player_datum *player = simulation_player_update_datum_try_get(update->player_index);
    player_datum *swap_player = simulation_player_update_datum_try_get(update->swap_player_index);
    bool success = false;
    if (player || swap_player) {
        if ((!player || (player->flags & 2)) && (!swap_player || (swap_player->flags & 2))) {
            s_player_identifier identifier;
            s_player_identifier swap_identifier;
            if (player)
                identifier = player->player_identifier;
            else
                memset(&identifier, 0, sizeof(identifier));
            if (swap_player)
                swap_identifier = swap_player->player_identifier;
            else
                memset(&swap_identifier, 0, sizeof(swap_identifier));
            if (memcmp(&identifier, &update->player_identifier, sizeof(identifier)) == 0 &&
                memcmp(&swap_identifier, &update->swap_player_identifier, sizeof(swap_identifier)) == 0)
            {
                player_swap(update->player_index, update->swap_player_index);
                success = true;
            }
        }
    }
    return success;
}


// @address 0x410a78 ?simulation_player_update_apply_remove@@YA_NPBUsimulation_player_update@@@Z
bool simulation_player_update_apply_remove(const simulation_player_update *update)
{
    long index = update->player_index;
    s_data_array *array = tls_player_data();
    player_datum *datum = 0;
    if (index != -1 && index >= 0 && index < array->first_unallocated) {
        player_datum *candidate = (player_datum *)(array->data + array->size * index);
        if (candidate->identifier != 0) datum = candidate;
    }
    bool result = false;
    if (datum != 0) {
        if (memcmp(datum->player_identifier.data, &update->player_identifier, sizeof(datum->player_identifier.data)) == 0 && (datum->flags & 2)) {
            long player_index = -1;
            if (index != -1)
                player_index = (*(unsigned short *)(array->data + array->size * index) << 16) | index;
            player_delete(player_index);
            result = true;
        }
    }
    return result;
}


// @address 0x410b68 ?simulation_player_update_apply_added@@YA_NPBUsimulation_player_update@@@Z
bool simulation_player_update_apply_added(const simulation_player_update *update)
{
    player_datum *player = simulation_player_update_datum_try_get(update->player_index);
    bool success = false;
    bool valid = false;
    long machine_index = -1;
    if (update->player_left_game ||
        (update->machine_user_index >= 0 && update->machine_user_index < 4 && update->machine_controller_index >= 0 && update->machine_controller_index < 4))
        valid = true;
    if (valid && !update->player_left_game) {
        s_players_globals *machine_globals = tls_players_globals();
        unsigned long machine_valid_mask = machine_globals->machine_valid_mask;
        s_machine_identifier machine_identifiers[16];
        memcpy(machine_identifiers, machine_globals->machines, sizeof(machine_identifiers));
        for (long index = 0; index < 16; index++) {
            if ((machine_valid_mask & (1 << index)) != 0 &&
                memcmp(&update->machine_identifier, &machine_identifiers[index], sizeof(s_machine_identifier)) == 0)
            {
                machine_index = index;
                break;
            }
        }
        if (machine_index == -1)
            valid = false;
    }
    if (valid) {
        c_data_iterator<player_datum> iterator;
        iterator.begin(tls_player_data());
        while (iterator.next()) {
            if ((short)iterator.get_index() != update->player_index &&
                memcmp(&iterator.get_datum()->player_identifier, &update->player_identifier, sizeof(s_player_identifier)) == 0)
            {
                valid = false;
                break;
            }
        }
    }
    if (valid && !update->player_left_game) {
        c_data_iterator<player_datum> iterator;
        iterator.begin(tls_player_data());
        while (iterator.next()) {
            player_datum *datum = iterator.get_datum();
            if ((short)datum->machine_index == machine_index && (short)iterator.get_index() != update->player_index) {
                if (datum->user_index == update->machine_user_index || datum->controller_index == update->machine_controller_index) {
                    valid = false;
                    break;
                }
            }
        }
    }
    if (valid) {
        game_globals_storage *game_globals = tls_game_globals();
        game_player_options options;
        memset(&options, 0, sizeof(options));
        options.valid = true;
        options.left_game = update->player_left_game;
        if (!update->player_left_game) {
            options.machine_identifier = update->machine_identifier;
            options.user_index = (short)update->machine_user_index;
            options.controller_index = update->machine_controller_index;
        } else {
            options.user_index = -1;
            options.controller_index = -1;
        }
        options.player_identifier = update->player_identifier;
        memcpy(&options.configuration, &update->player_data, sizeof(options.configuration));
        if (game_globals->options.game_variant.flags & 1) {
            if (options.configuration.host.team_index > 0) {
                if (options.configuration.host.team_index >= 7)
                    options.configuration.host.team_index = 7;
            } else {
                options.configuration.host.team_index = 0;
            }
        } else {
            options.configuration.host.team_index = update->player_index;
        }
        if (!player) {
            if (player_new(update->player_index, &options, update->player_joined_in_progress) != -1)
                success = true;
        } else if (memcmp(&player->player_identifier, &options.player_identifier, sizeof(s_player_identifier)) == 0 && (player->flags & 2)) {
            s_data_array *players = tls_player_data();
            long player_index = -1;
            if (update->player_index != -1)
                player_index = (((player_datum *)(players->data + players->size * update->player_index))->identifier << 16) | update->player_index;
            player_rejoined_game(player_index, &options, update->player_joined_in_progress);
            success = true;
        }
    }
    return success;
}
extern "C" {
}

bool simulation_player_update_apply_configuration(const simulation_player_update *request)
{
    long index = request->player_index;
    s_data_array *array = tls_player_data();
    player_datum *datum = 0;
    if (index != -1 && index >= 0 && index < array->first_unallocated) {
        player_datum *candidate = (player_datum *)(array->data + array->size * index);
        if (candidate->identifier != 0) datum = candidate;
    }
    bool result = false;
    if (datum != 0) {
        if (memcmp(datum->player_identifier.data, &request->player_identifier, sizeof(datum->player_identifier.data)) == 0) {
            long identifier = -1;
            if (index != -1)
                identifier = (*(unsigned short *)(array->data + array->size * index) << 16) | index;
            ::player_set_configuration(identifier, &request->player_data);
            result = true;
        }
    }
    return result;
}

// @address 0x3bde18 ?distributed_authority_dispatch_player_actions@c_simulation_world@@AAAXKPBUplayer_action@@@Z
void c_simulation_world::distributed_authority_dispatch_player_actions(unsigned long flags, const player_action *actions)
{
    for (long player_index = 0; player_index < 16; player_index++) {
        c_simulation_player *player = &m_players[player_index];
        if (player->m_control_index == -1 || (flags & (1 << player_index)) == 0)
            continue;
        s_player_motion motion;
        if (!((c_simulation_player *)player)->build_motion_data(&actions[player_index], false, &motion))
            continue;
        long control_index = m_players[player_index].m_control_index;
        s_machine_identifier machine_identifier;
        memcpy(&machine_identifier, &player->m_machine_identifier, sizeof(machine_identifier));
        s_simulation_world_view_iterator iterator;
        iterator.type_mask = 16;
        iterator.index = 0;
        c_simulation_view *view;
        while (iterator_next(&iterator, &view)) {
            s_machine_identifier view_machine_identifier;
            memcpy(&view_machine_identifier, &view->m_machine_identifier, sizeof(view_machine_identifier));
            if (memcmp(&view_machine_identifier, &machine_identifier, sizeof(machine_identifier)) != 0) {
                c_replication_control_view *control = &((s_simulation_distributed_view *)view->m_distributed_view)->control;
                if (control != 0) {
                    control->send_motion(control_index, &motion);
                }
            }
        }
    }
}
