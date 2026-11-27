// networkinglibxbox/simulation_world.obj: from the records 0x97580 0x97990 0x980e8 0x98160 0xa24c8 0xa25c0 0xa46c0 0xa6a28 0x3b53b8 0x3b5468 0x3bc948 0x3bcc90 0x3bcde8 0x3bcf48 0x3bd028 0x3bd140 0x3bd1f8 0x3bd2c8 0x3bd380 0x3bd538 0x3bd5a0 0x3bd618 0x3bd7a8 0x3bd860 0x3bd908 0x3bda30 0x3bdb40 0x3bdc90 0x3bdf98 0x3be090 0x3be188 0x3be590 0x3be658 0x3be7a0 0x3be920 0x3bed30 0x3bee08 0x3beeb0 0x3bef68 0x3bf000 0x3bf0a0 0x3bf128 0x3bf1c8 0x3bf2d0 0x3bf3d8 0x3bf4a8 0x3bf538 0x3bf610 0x3bf710 0x3bf8f8 0x3bfa48 0x3bfb50 0x3bfbb0 0x3bfd00 0x3bfd70 0x3bfde0 0x3bfe40 0x3bff68 0x3c0018 0x3d6800
#include "simulation/simulation_watcher_types.h"
#include "math/real_math_constants.h"
#include "simulation/simulation_distributed_world_types.h"
#include "networking/simulation/simulation_entity_definition_types.h"
#include "cseries/crt_xdk.h"
#include "cseries/read_write_barrier.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "game/game_simulation_types.h"
#include "simulation/simulation_queue_global_event_types.h"
#include "networking/network_observer_owner_types.h"
#include "simulation/simulation_globals.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "game/game_globals_storage.h"
#include "units/unit_control.h"
#include "game/player_types.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "simulation/simulation.h"
#include "units/units.h"
#include "game/game_engine_simulation.h"
#include "game/game_results.h"
#include "hs/hs_runtime.h"
#include "main/main_time.h"
#include "networking/network_memory.h"
#include "networking/network_time.h"
#include "objects/object_placement.h"
#include "rasterizer/rasterizer_globals.h"
#include "simulation/game_interface/simulation_game_interface.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/simulation_queue_entities.h"
#include "simulation/simulation_queue_events.h"
#include "simulation/simulation_queue_global_events.h"
#include "saved_games/game_state_procs.h"
#include "saved_games/saved_film.h"
#include "simulation/simulation_view.h"
#include "simulation/simulation_world.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/messages/network_messages_simulation_synchronous.h"
#include "memory/bitstream.h"
#include "simulation/simulation_players_types.h"
#include "networking/replication/replication_prediction.h"
#include "simulation/player_action.h"
#include "networking/network_configuration_types.h"
#include "game/game_engine_scoring_globals.h"
#include "simulation/simulation_view_reason_types.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include "simulation/simulation_view_establishment_mode_types.h"
#include "simulation/simulation_watcher.h"
#include "math/matrix_math_constants.h"
#include <string.h>
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_membership.h"
#include "simulation/simulation_world_services.h"
#include "simulation/simulation_queue_types.h"
#include "simulation/simulation_queue.h"
#include "simulation/simulation_update_types.h"
#include "simulation/simulation_update_metadata.h"
#include "networking/replication/replication_entity_manager_view.h"
#include "networking/replication/replication_event_manager_view.h"
#include "networking/replication/replication_entity_manager.h"
#include "networking/replication/replication_event_manager.h"
#include "simulation/simulation_world_types.h"
#include "networking/network_memory_types.h"
#include "saved_games/saved_film_globals.h"
#include "game/players_globals.h"
#include "game/player_access.h"
#include "networking/session/network_session_parameter_synchronous_out_of_sync.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/session/network_session_parameter_observer.h"
#include "simulation/simulation_distributed_view.h"
#include "memory/gameplay_tls_globals.h"
#include "networking/network_configuration.h"
#include "game/game_engine_candy_monitor_types.h"
#include "networking/network_time_types.h"
#include "memory/game_state_globals.h"
#include "math/integer_math.h"
#include "math/real_math_fraction_constants.h"
#include "networking/replication/replication_motion.h"
#include "game/game_time_types.h"
#include "networking/simulation/simulation_type_collection.h"
#include "game/game_simulation_predicates.h"
#include "networking/messages/network_message_handler_types.h"
#include "networking/network_time_since_access.h"
// @flags /TP /O2 /Oc
// @address 0x808560 g_simulation_globals
// @address 0x980e8 ?iterator_next@c_simulation_world@@QBA_NPAUs_simulation_world_view_iterator@@PAPAVc_simulation_view@@@Z
// @address 0x3bd7a8 ?get_player_active_mask@c_simulation_world@@QBAKXZ

enum e_director_perspective;
struct simulation_update;
struct s_simulation_update_metadata;

class c_network_observer;
class c_simulation_watcher;
class c_simulation_world;


typedef simulation_update s_simulation_update;
struct s_simulation_world_machine_address { unsigned __int64 value : 48; };
struct s_server_update_queue_element {
    long ready;
    s_simulation_update update;
    s_simulation_update_metadata metadata;
    int playback_control_mode;
    int received_time;
    int update_number;
    s_server_update_queue_element *queue_next;
};

struct s_simulation_player_acknowledgement {
    unsigned long update;
    unsigned long flags;
    s_player_identifier players[16];
};

// match: 0x3bcde8, 0x3bc948: compact lifecycle layout differs from the virtual database interface; see field-disputes.tsv.
struct s_simulation_world_database_initialization {
    bool initialized;
    bool deleting;
    c_simulation_world *world;
    c_replication_entity_manager *entity_manager;
    c_simulation_type_collection *types;
};

__forceinline void c_replication_entity_manager::reset() {
    for (long view_index = 0; view_index < 15; view_index++) {
        if (m_views[view_index])
            m_views[view_index]->reset();
    }
    memset(m_entity_data, 0, sizeof(m_entity_data));
    m_next_entity = 0;
}

long c_simulation_world::update_queue_get_available_updates() const
{
    long available_updates;
    if (!update_queue_length)
    {
        available_updates = 0;
    }
    else if (update_queue_length_copy == update_queue_length)
    {
        available_updates = update_queue_length;
    }
    else
    {
        available_updates = 0;
        for (s_server_update_queue_element *element = update_queue_head;
             element && !element->ready; element = element->queue_next)
        {
            ++available_updates;
        }
    }
    return available_updates;
}

// @address 0x80877c network_shared_memory_globals
// @address 0x94e3b0 g_saved_film_timer_globals


namespace {
static __forceinline long simulation_world_find_machine(const s_players_globals *table,
    const s_machine_identifier *identifier)
{
    unsigned __int64 address = ((const s_simulation_world_machine_address *)identifier)->value;
    long mask = table->machine_valid_mask;
    long machine_index = 0;
    while (mask != 0) {
        if (mask & 1) {
            unsigned __int64 candidate = ((const s_simulation_world_machine_address *)&table->machines[machine_index])->value;
            if (address == candidate)
                return machine_index;
        }
        mask >>= 1;
        machine_index++;
    }
    return -1;
}
}

// @address 0xa24c8 simulation_world_find_player
extern "C" c_simulation_player *simulation_world_find_player(
    c_simulation_world *world, const s_machine_identifier *identifier, long user_index)
{
    c_simulation_player *result = 0;
    long machine_index = simulation_world_find_machine(tls_players_globals(), identifier);
    if (machine_index != -1) {
        for (long i = 0; i < 16; i++) {
            if (world->m_players[i].m_control_index != -1) {
                player_datum *datum = datum_get<player_datum>(player_data, world->m_players[i].m_player_index);
                if (datum->machine_index == machine_index && datum->user_index == user_index) {
                    result = &world->m_players[i];
                    break;
                }
            }
        }
    }
    return result;
}

static __forceinline void simulation_world_free_network_memory(void *allocation)
{
    c_rockall_heap *heap = network_shared_memory_globals.heap;
    heap->m_rockall_front_end->Delete(allocation, -1);
    heap->m_allocation_count--;
}

// @address 0x3bcc90 ?reset_world@c_simulation_world@@QAAXXZ
void c_simulation_world::reset_world()
{
    m_immediate_update = false;
    m_out_of_sync = false;
    m_gamestate_flushed = false;
    m_gamestate_flushed_outside_game_tick = false;
    m_gamestate_loaded_from_saved_film = false;

    if (is_distributed()) {
        m_distributed_world->m_replication_entity_manager.reset();
        m_distributed_world->m_replication_event_manager.reset();
        m_distributed_world->m_entity_database.reset();
        simulation_gamestate_entities_notify_simulation_world_reset();
        delete_all_actors();
    }

    if (runs_simulation()) {
        simulation.clear();
    }

    if (m_time_running) {
        m_time_running = false;
    }

    if (!runs_simulation()) {
        update_queue_reset();
    }
}

// @address 0x3bd1f8 simulation_world_update_views_with_message
extern "C" void simulation_world_update_views_with_message(c_simulation_world *world, long message_a,
    long message_b, long message_c)
{
    bool sends = world->m_world_type != 4 && world->m_world_type != 6 && world->m_world_type != 8;
    if (sends) {
        s_simulation_world_view_iterator iterator;
        c_simulation_view *view;
        world->iterator_begin(&iterator, 4);
        if (world->iterator_next(&iterator, &view)) {
            do {
                s_network_message_synchronous_update_ack_message message;
                memset(&message, 0, sizeof(message));
                message.mode = message_a;
                message.received_time = message_b;
                message.update_number = message_c;
                view->send_message(_network_message_type_synchronous_playback_control, sizeof(message), &message,
                    false);
            } while (world->iterator_next(&iterator, &view));
        }
    }
}

// @address 0x3bd5a0 ?remove_all_views@c_simulation_world@@QAAXXZ
void c_simulation_world::remove_all_views()
{
    s_simulation_world_view_iterator iterator;
    c_simulation_view *view;
    iterator_begin(&iterator, 0xffffffff);
    if (iterator_next(&iterator, &view)) {
        do {
            simulation_remove_view_from_world(view);
        } while (iterator_next(&iterator, &view));
    }
}

// @address 0x3bd028 update_queue_delete_ready_head
extern "C" void update_queue_delete_ready_head(c_simulation_world *world)
{
    bool applied = false;
    int received_time;
    if (world->update_queue_length > 0) {
        s_server_update_queue_element *element = world->update_queue_head;
        if (element->ready == 1) {
            applied = true;
            s_server_update_queue_element *tail = world->update_queue_tail;
            received_time = element->received_time;
            int update_number = element->update_number;
            world->update_queue_head = element->queue_next;
            if (tail == element)
                world->update_queue_tail = 0;
            c_rockall_heap *heap = network_shared_memory_globals.heap;
            int size;
            if (!heap->m_rockall_front_end->Details(element, &size))
                size = -1;
            simulation_world_free_network_memory(element);
            world->update_queue_length--;
            if (update_number != -1)
                world->update_queue_applied_number = update_number;
        }
    }
    if (applied) {
        g_saved_film_timer_globals.timer_other = received_time;
        g_simulation_globals.film_update_pending = true;
    }
}


struct s_owner;

// @address 0x3bd140 simulation_world_set_out_of_sync
extern "C" void simulation_world_set_out_of_sync(c_simulation_world *world, bool determinism_failure)
{
    if (!world->m_out_of_sync) {
        s_network_session_parameter_synchronous_out_of_sync out_of_sync;
        out_of_sync.report_out_of_sync = true;
        out_of_sync.out_of_sync = determinism_failure;
        ((c_network_session_parameters *)network_life_cycle_globals_get()
            ->manager.m_active_squad_session->m_session_parameters)->m_parameters.synchronous_out_of_sync.request_change(&out_of_sync);
        world->m_out_of_sync = true;
        world->m_out_of_sync_determinism_failure = determinism_failure;
    }
}


// @address 0x3bd2c8 ?time_start@c_simulation_world@@QAAXJ_N@Z
void c_simulation_world::time_start(long update_number, bool force)
{
    next_update_number = update_number;
    if (!runs_simulation()) {
        if (!force) {
            s_server_update_queue_element *head = update_queue_head;
            if (head) {
                if (head->ready == 0 && head->update.update_number != update_number) {
                    simulation_world_set_out_of_sync(this, false);
                    force = true;
                }
            } else {
                force = true;
            }
        }
        if (force) {
            update_queue_reset();
            update_queue_reset_value = -1;
            last_update_number = -1;
        }
        update_queue_applied_number = update_number;
    }
    m_time_running = true;
}

// @address 0x3bcf48 ?process_input@c_simulation_world@@QAAXKPBUplayer_action@@K@Z
void c_simulation_world::process_input(unsigned long mask, const player_action *actions, unsigned long flags)
{
    if (runs_simulation()) {
        for (long user_index = 0; user_index < 4; user_index++) {
            if (mask & (1 << user_index)) {
                const s_machine_identifier *identifier = &m_machine_identifier;
                c_simulation_player *player = simulation_world_find_player(this, identifier, user_index);
                if (player)
                    player->handle_local_input(&actions[user_index], flags);
            }
        }
    } else if (m_world_type == 4) {
        c_simulation_view *view = get_authority_view();
        view->dispatch_synchronous_actions(mask, actions, flags);
    }
}
static __forceinline void simulation_world_init_player(c_simulation_world *world, long absolute, long datum,
    long player_type,
    const s_player_identifier *id, const s_machine_identifier *machine_id)
{
    c_simulation_player *simulation = &world->m_players[absolute];
    simulation->m_control_index = absolute;
    simulation->m_player_index = datum;
    simulation->m_state = player_type;
    simulation->m_identifier = *id;
    simulation->m_machine_identifier = *machine_id;
    simulation->m_pending_deletion = false;
    simulation->m_world = world;
    simulation->m_active = !world->is_authority();
    simulation->m_last_time = -1;
    player_action_clear(&simulation->m_last_action);
}

// @address 0x3bd618 ?create_player@c_simulation_world@@QAAXJW4e_game_simulation_type@@@Z
void c_simulation_world::create_player(long player_index, e_game_simulation_type simulation_type)
{
    long absolute_index = player_index & 0xffff;
    player_datum *player = player_get(player_index);
    bool local_player = player_is_on_local_machine(player_index);
    long player_type = -1;
    switch (simulation_type) {
    case _game_simulation_local: player_type = 0; break;
    case _game_simulation_synchronous_client:
    case _game_simulation_synchronous_server: player_type = local_player ? 0 : 3; break;
    case _game_simulation_distributed_client: player_type = local_player ? 1 : 5; break;
    case _game_simulation_distributed_server: player_type = local_player ? 0 : 4; break;
    // match: 0x3bd618: unreachable hint omits dead branch check
    default: __assume(0);
    }
    long user_index = -1;
    do {
        if (player_index != -1)
            user_index = ((long)::director_get_perspective(player_index, user_index));
    } while (user_index != -1);
    simulation_world_init_player(this, absolute_index, player_index, player_type, &player->player_identifier,
        &player->machine_identifier);
}

bool c_simulation_world::iterator_next(s_simulation_world_view_iterator *iterator, c_simulation_view **view) const
{
    // match: 0xd3f88: game_results_update inlines this; reading views through a const alias keeps it 1124B (12B larger without).
    const c_simulation_world *world = this;
    bool result = false;
    while (iterator->index >= 0 && iterator->index < 15) {
        int index = iterator->index;
        iterator->index = index + 1;
        c_simulation_view *current = world->views[index];
        if (current && ((1 << current->m_view_type) & iterator->type_mask)) {
            result = true;
            *view = current;
            break;
        }
    }
    return result;
}

unsigned long c_simulation_world::get_player_active_mask() const
{
    unsigned long mask = 0;
    for (long player_index = 0; player_index < 16; player_index++) {
        const c_simulation_player *player = &m_players[player_index];
        if (player->m_control_index != -1 && player->m_active)
            mask |= 1 << player_index;
    }
    return mask;
}

// @address 0x3bd860 simulation_world_get_max_unacknowledged_updates
extern "C" long simulation_world_get_max_unacknowledged_updates(c_simulation_world *world)
{
    long result = 0;
    s_simulation_world_view_iterator iterator;
    c_simulation_view *view;
    iterator.type_mask = 4;
    iterator.index = 0;
    while (world->iterator_next(&iterator, &view)) {
        if (view->m_active && view->m_synchronous_update_number != -1) {
            long behind = world->next_update_number - view->m_synchronous_update_number - 1;
            if (behind > result)
                result = behind;
        }
    }
    return result;
}


// @address 0x3bd908 simulation_world_compute_available_update_count
extern "C" long simulation_world_compute_available_update_count(c_simulation_world *world)
{
    long tr31 = world->m_maximum_updates;
    long unacknowledged_updates = simulation_world_get_max_unacknowledged_updates(world);
    long tr3;
    if (tr31 - unacknowledged_updates < 0) {
        tr3 = 0;
    } else {
        tr31 -= unacknowledged_updates;
        if (tr31 != 0) {
            s_simulation_world_view_iterator l58;
            l58.type_mask = 4;
            l58.index = 0;
            c_simulation_view *l50;
            if ((unsigned char)world->iterator_next(&l58, &l50) != 0) {
                __int64 tr10;
                long tr11;
                for (;;) {
                    c_simulation_view *tr7 = l50;
                    bool backlogged;
                    long tr5 = tr7->m_observer_channel_index;
                    if (tr5 != -1) {
                        backlogged = tr7->m_observer->observer_channel_backlogged(_network_observer_owner_simulation, tr5,
                            _network_message_type_synchronous_update);
                    } else {
                        backlogged = true;
                    }
                    if (!backlogged) {
                        if ((unsigned char)world->iterator_next(&l58, &l50) == 0) {
                            break;
                        } else {
                            continue;
                        }
                    } else {
                        tr11 = tr7->m_observer_channel_index;
                        tr3 = 0;
                        if (tr11 != -1) {
                            c_network_observer::s_channel_observer *channel = &tr7->m_observer->m_channel_observers[tr11];
                            if (channel->channel.get_state() == _network_channel_state_connected) {
                                tr10 = channel->message_type_flags;
                                if ((tr10 & (1ui64 << _network_message_type_synchronous_update)) == 0) {
                                    channel->message_type_flags = tr10 | (1i64 << _network_message_type_synchronous_update);
                                }
                            }
                        }
                    }
                L10:;
                    return tr3;
                }
        }
        }
        tr3 = tr31;
    }
    goto L10;
    return 0;
}


// @address 0x3beeb0 ?drop_simulation_from_active_to_joining@c_simulation_world@@QAAXXZ
void c_simulation_world::drop_simulation_from_active_to_joining()
{
    if (m_world_state == _simulation_world_state_active) {
        c_simulation_view *view = 0;
        s_simulation_world_view_iterator iterator;
        iterator_begin(&iterator, 20);
        while (iterator_next(&iterator, &view)) {
            if (!view->is_dead() && view->get_view_establishment_mode() == _simulation_view_establishment_mode_joined) {
                view->set_view_establishment(_simulation_view_establishment_mode_joining,
                    view->get_view_establishment_identifier());
            }
        }
        change_state_joining(m_watcher->m_machine_valid_mask);
    }
}

void c_simulation_world::delete_all_players()
{
    for (long player_index = 0; player_index < 16; player_index++) {
        if (m_players[player_index].exists())
            {
            c_simulation_player *player = &m_players[player_index];
            player->m_control_index = -1;
            player->m_player_index = -1;
            player->m_state = -1;
            player->m_world = 0;
        }
    }
}

// @address 0x3bf000 ?create_actor@c_simulation_world@@QAAJJJ@Z
long c_simulation_world::create_actor(long actor_index, long unit_index)
{
    long new_actor_index = actor_index;
    if (new_actor_index == -1) {
        for (long index = 0; index < 16; index++) {
            if (!m_actors[index].exists()) {
                new_actor_index = index;
                break;
            }
        }
    }
    if (new_actor_index != -1)
        m_actors[new_actor_index].initialize(this, new_actor_index, unit_index);
    return new_actor_index;
}

void c_simulation_world::delete_all_actors()
{
    for (long actor_index = 0; actor_index < 16; actor_index++) {
        if (m_actors[actor_index].exists())
            m_actors[actor_index].destroy();
    }
}

// match: 0x3bda30: March 825546b0 calls csmemset (82889dc8), which forwards to the external CRT call.
#pragma function(memset)
// @address 0x3bda30 simulation_world_update_synchronous_views
void simulation_world_update_synchronous_views(c_simulation_world *world, const simulation_update *update,
    const s_simulation_update_metadata *metadata)
{
    c_simulation_view *view;
    s_simulation_update_metadata update_metadata;
    if ((bool)((metadata->flags >> 2) & 1)) {
        update_metadata = *metadata;
    } else {
        csmemset(&update_metadata, 0, sizeof(update_metadata));
        update_metadata.flags |= FLAG(1);
    }
    s_simulation_world_view_iterator iterator;
    world->iterator_begin(&iterator, 4);
    while (world->iterator_next(&iterator, &view)) {
        if (view->m_establishment_mode == _simulation_view_establishment_mode_joined) {
            s_network_message_synchronous_update message;
            csmemset(&message, 0, sizeof(message));
            message.update = *update;
            message.metadata = update_metadata;
            view->send_message(_network_message_type_synchronous_update, sizeof(message), &message, false);
        }
    }
}
// match: 0x3bda30: restore the default intrinsic expansion for other calls.
#pragma intrinsic(memset)

// @address 0x3bdc90 simulation_world_handle_synchronous_update_ack
extern "C" int simulation_world_handle_synchronous_update_ack(c_simulation_world *world, unsigned long mode,
    long received_time, long update_number)
{
    int result = 0;
    if (mode >= 1) {
        if (mode == 1) {
            bool game_ready = false;
            game_globals_storage *game = tls_game_globals();
            if (game != 0 && game->in_progress != 0 && game->initializing == 0 && game->map_active != 0)
                game_ready = true;
            if (game_ready && game_is_cooperative() && !game_is_film_playback_active())
                saved_film_initialize_timer();
            result = 1;
        }
    } else {
        if (!world->synchronous_gamestate_write_in_progress() && world->m_world_state == _simulation_world_state_active && world->m_time_running) {
            bool queued = false;
            c_simulation_world *queue = world;
            s_server_update_queue_element *element =
                (s_server_update_queue_element *)(network_shared_memory_globals.heap)->allocate(sizeof(s_server_update_queue_element));
            if (element != 0) {
                if (queue->update_queue_tail == 0)
                    queue->update_queue_head = element;
                else
                    queue->update_queue_tail->queue_next = element;
                element->playback_control_mode = 0;
                element->queue_next = 0;
                element->received_time = received_time;
                element->update_number = update_number;
                queued = true;
                element->ready = 1;
                queue->update_queue_tail = element;
                queue->update_queue_reset_value = 1;
                queue->last_update_number = update_number;
                queue->update_queue_length++;
            }
            if (!queued)
                g_simulation_globals.fatal_error = true;
            else
                result = 1;
        } else {
            simulation_world_set_out_of_sync(world, false);
        }
    }
    return result;
}

// @address 0x3bdf98 ?distributed_authority_dispatch_actor_control@c_simulation_world@@AAAXKPBUunit_control_data@@@Z
void c_simulation_world::distributed_authority_dispatch_actor_control(unsigned long actor_mask,
    const unit_control_data *controls)
{
    const unit_control_data *control = controls;
    long actor_index = 0;
    c_simulation_actor *actor = m_actors;
    for (; actor_index < 16; actor_index++, control++, actor++) {
        if (actor->exists() && (actor_mask & (1 << actor_index))) {
            s_player_motion motion;
            simulation_game_build_motion_data_from_control(control, actor->m_unit_index, &motion);
            long control_index = actor->m_simulation_actor_index + 16;
            s_simulation_world_view_iterator iterator;
            c_simulation_view *view;
            iterator_begin(&iterator, 16);
            while (iterator_next(&iterator, &view)) {
                c_replication_control_view *control_view = &view->m_distributed_view->control;
                if (control_view != 0) {
                    control_view->m_motions[control_index] = motion;
                    unsigned long pending_mask = control_view->m_motion_available_send;
                    control_view->m_motion_available_send = pending_mask | (1 << control_index);
                }
            }
        }
    }
}

// @address 0x3bf128 ?recreate_players@c_simulation_world@@QAAXW4e_game_simulation_type@@@Z
void c_simulation_world::recreate_players(e_game_simulation_type simulation_type)
{
    delete_all_players();
    c_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (!(player->flags & 2))
            create_player(iterator.get_index(), simulation_type);
    }
}

static __forceinline bool simulation_world_is_synchronous(const c_simulation_world *world)
{
    long type = world->m_world_type;
    return type == 3 || type == 4 || type == 5 || type == 6;
}

bool c_simulation_world::synchronous_gamestate_write_in_progress() const
{
    bool result = false;
    if (exists() && !is_authority() && simulation_world_is_synchronous(this) && get_authority_view())
        result = get_authority_view()->synchronous_catchup_in_progress();
    return result;
}

c_simulation_view *c_simulation_world::get_client_view_by_machine_index(long machine_index)
{
    c_simulation_view *result = 0;
    s_simulation_world_view_iterator iterator;
    iterator_begin(&iterator, 20);
    c_simulation_view *view;
    while (iterator_next(&iterator, &view)) {
        if (view->get_machine_index() == machine_index) {
            result = view;
            break;
        }
    }
    return result;
}

c_simulation_view *c_simulation_world::get_client_view_by_machine_identifier(const s_machine_identifier *machine_identifier)
{
    c_simulation_view *result = 0;
    s_simulation_world_view_iterator iterator;
    iterator_begin(&iterator, 20);
    c_simulation_view *view;
    s_machine_identifier view_identifier;
    while (iterator_next(&iterator, &view)) {
        view->get_machine_identifier(&view_identifier);
        if (memcmp(&view_identifier, machine_identifier, sizeof(s_machine_identifier)) == 0) {
            result = view;
            break;
        }
    }
    return result;
}


// @address 0x3bfe40 ?simulation_queue_allocate@c_simulation_world@@QAAXW4e_simulation_queue_element_type@@JPAPAUs_simulation_queue_element@@@Z
void c_simulation_world::simulation_queue_allocate(e_simulation_queue_element_type type, long size,
    s_simulation_queue_element **element)
{
    if (runs_simulation()) {
        *element = 0;
        unsigned long type_bit = 1 << type;
        if (type_bit & 0x780) {
            bookkeeping.allocate(size, element);
        } else {
            bool allocation_restricted = false;
            if (!(type_bit & 112)) {
                if ((float)simulation.allocated_bytes / 131072.0f > g_network_configuration.simulation_queue_allocated_bytes_limit ||
                    (float)simulation.allocated_elements / 4096.0f > g_network_configuration.simulation_queue_allocated_elements_limit)
                    allocation_restricted = true;
            }
            if (!allocation_restricted)
                simulation.allocate(size, element);
        }
        if (*element)
            (*element)->type = type;
    }
}

static __forceinline void simulation_world_queue_deallocate(c_simulation_queue *queue,
    s_simulation_queue_element *element)
{
    if (queue->initialized) {
#ifdef HALO3_64BIT
        queue->allocated_bytes -= element->data_size + sizeof(s_simulation_queue_element);
#else
        queue->allocated_bytes -= element->data_size + 16;
#endif
        queue->allocated_elements--;
        ::network_heap_free_block((void *)(element));
    }
}

// @address 0x3bff68 ?simulation_queue_free@c_simulation_world@@QAAXPAUs_simulation_queue_element@@@Z
void c_simulation_world::simulation_queue_free(s_simulation_queue_element *element)
{
    if (runs_simulation()) {
        if ((unsigned long)(1 << element->type) & 0x780)
            simulation_world_queue_deallocate(&bookkeeping, element);
        else
            simulation_world_queue_deallocate(&simulation, element);
    }
}

void c_simulation_world::simulation_queue_enqueue(s_simulation_queue_element *element)
{
    if (runs_simulation()) {
        if ((unsigned long)(1 << element->type) & 0x780)
            bookkeeping.enqueue(element);
        else
            simulation.enqueue(element);
    }
}

// @address 0x3d6800 ?build_player_actions@c_simulation_world@@QAAXPAUsimulation_update@@@Z
void c_simulation_world::build_player_actions(simulation_update *update)
{
    c_simulation_queue *source = (c_simulation_queue *)update;
    c_simulation_queue *destination = (c_simulation_queue *)this;
    long count = source->queued_count();
    for (long index = 0; index < count; index++) {
        s_simulation_queue_element *element = 0;
        source->deque(&element);
        source->allocated_elements--;
        source->allocated_bytes -= element->data_size + sizeof(s_simulation_queue_element);
        destination->allocated_elements++;
        destination->allocated_bytes = destination->allocated_bytes + element->data_size + sizeof(s_simulation_queue_element);
        destination->enqueue(element);
        if (element->type == 6) {
            long data_size = element->data_size;
            unsigned char *data = (unsigned char *)element->data;
            bool stop = false;
            c_bitstream packet;
            packet.m_data_size_alignment = 1;
            packet.m_data_size_bytes = data_size;
            packet.m_data = data;
            packet.m_data_max = data + data_size;
            packet.reset(0);
            packet.reset(3);
            long value = packet.read_integer(0, 4);
            long minimum = 3;
            long maximum = 5;
            if (!packet.error_occurred() && value >= minimum && value <= maximum)
                stop = true;
            packet.finish_reading();
            if (stop)
                break;
        }
    }
    destination->initialized = true;
}

namespace {
static __forceinline c_rockall_heap *simulation_world_network_heap()
{
    return network_shared_memory_globals.heap;
}
static __forceinline void simulation_world_destroy_update(simulation_update *update)
{
    update->bookkeeping.dispose();
    update->simulation.dispose();
}
static __forceinline void simulation_world_release_update_memory(void *allocation)
{
    long allocation_size = simulation_world_network_heap()->get_allocation_size(allocation);
    simulation_world_free_network_memory(allocation);
}
}
// @address 0x3bf2d0 ?update_queue_reset@c_simulation_world@@AAAXXZ
void c_simulation_world::update_queue_reset()
{
    s_server_update_queue_element *element = update_queue_head;
    while (element) {
        s_server_update_queue_element *next = element->queue_next;
        if (element->ready == 0)
            simulation_world_destroy_update(&element->update);
        simulation_world_release_update_memory(element);
        element = next;
    }
    update_queue_head = 0;
    update_queue_tail = 0;
    update_queue_length = 0;
    update_queue_length_copy = 0;
    update_queue_reset_value = -1;
    last_update_number = -1;
    update_queue_applied_number = 0;
}

// @address 0x3bf3d8 ?update_queue_handle_server_update@c_simulation_world@@AAA_NPBUsimulation_update@@PBUs_simulation_update_metadata@@@Z
bool c_simulation_world::update_queue_handle_server_update(const s_simulation_update *update,
    const s_simulation_update_metadata *metadata)
{
    bool result = false;
    s_server_update_queue_element *element;
    element = (s_server_update_queue_element *)(network_shared_memory_globals.heap)->allocate(sizeof(s_server_update_queue_element));
    if (element != 0) {
        if (update_queue_tail == 0)
            update_queue_head = element;
        else
            update_queue_tail->queue_next = element;
        element->ready = result;
        element->metadata = *metadata;
        element->queue_next = 0;
        update_queue_tail = element;
        update_queue_length++;
        update_queue_length_copy++;
        simulation_update *payload = &element->update;
        if (payload != 0) {
        memcpy(payload, update, sizeof(*update));
        update_queue_reset_value = result;
        last_update_number = update->update_number;
        result = true;
        }
    }
    return result;
}

// @address 0x3bf710 ?relinquish_authority@c_simulation_world@@QAA_NXZ
bool c_simulation_world::relinquish_authority()
{
    int world_type;
    s_machine_identifier *asynchronous_machine_identifier;
    c_simulation_watcher *asynchronous_watcher;
    c_simulation_distributed_world *asynchronous_distributed_world;
    c_simulation_type_collection *asynchronous_type_collection;
    s_machine_identifier *synchronous_machine_identifier;
    c_simulation_watcher *synchronous_watcher;
    c_simulation_distributed_world *synchronous_distributed_world;
    c_simulation_type_collection *synchronous_type_collection;
    unsigned char attached_to_map;
    bool result;
    s_machine_identifier *asynchronous_saved_identifier_destination;
    int machine_index;
    s_machine_identifier *asynchronous_identifier_source;
    s_machine_identifier *asynchronous_saved_identifier_source;
    s_machine_identifier *asynchronous_identifier_destination;
    e_game_simulation_type client_simulation_type;
    s_machine_identifier *synchronous_saved_identifier_destination;
    s_machine_identifier *synchronous_identifier_source;
    s_machine_identifier *synchronous_saved_identifier_source;
    s_machine_identifier *synchronous_identifier_destination;
    s_machine_identifier machine_identifier_storage;
    s_simulation_queue_gamestate_clear_data gamestate_clear_data;
    result = false;
    world_type = m_world_type;
    if (world_type == _simulation_world_type_synchronous_authority
        || world_type == _simulation_world_type_distributed_authority)
    {
        if (world_type != _simulation_world_type_synchronous_authority) {
            asynchronous_machine_identifier = &m_machine_identifier;
            asynchronous_watcher = m_watcher;
            asynchronous_distributed_world = m_distributed_world;
            asynchronous_saved_identifier_destination = &machine_identifier_storage;
            machine_index = m_local_machine_index;
            asynchronous_identifier_source = asynchronous_machine_identifier;
            memcpy(asynchronous_saved_identifier_destination, asynchronous_identifier_source, sizeof(s_machine_identifier));
            attached_to_map = m_attached_to_map;
            this->destroy_world();
            // match: 0x3bf710: keep the byte pun; typing both arms changes code in other units (8B at 0x3d8820)
            if (g_game_results_globals.active) {
                *(unsigned char *)&(*(signed char *)&g_game_results_globals) = 0;
            }
            asynchronous_type_collection = g_simulation_globals.type_collection;
            g_simulation_globals.skip_next_world_reset = false;
            initialize_world(_game_simulation_distributed_client, 0, false, asynchronous_type_collection,
                asynchronous_watcher,
                asynchronous_distributed_world);
            simulation_queue_game_simulation_insert(0);
            simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_main_save_map);
            simulation_gamestate_entities_build_clear_flags(&gamestate_clear_data);
            simulation_queue_gamestates_delete_insert(&gamestate_clear_data);
            asynchronous_saved_identifier_source = &machine_identifier_storage;
            asynchronous_identifier_destination = asynchronous_machine_identifier;
            memcpy(asynchronous_identifier_destination, asynchronous_saved_identifier_source, sizeof(s_machine_identifier));
            client_simulation_type = _game_simulation_distributed_client;
        }
        else
        {
            synchronous_machine_identifier = &m_machine_identifier;
            synchronous_watcher = m_watcher;
            synchronous_distributed_world = m_distributed_world;
            synchronous_saved_identifier_destination = &machine_identifier_storage;
            machine_index = m_local_machine_index;
            synchronous_identifier_source = synchronous_machine_identifier;
            memcpy(synchronous_saved_identifier_destination, synchronous_identifier_source, sizeof(s_machine_identifier));
            attached_to_map = m_attached_to_map;
            this->destroy_world();
            if (g_game_results_globals.active)
                g_game_results_globals.active = false;
            game_simulation_set(_game_simulation_synchronous_client);
            synchronous_type_collection = g_simulation_globals.type_collection;
            g_simulation_globals.skip_next_world_reset = false;
            initialize_world(_game_simulation_synchronous_client, 0, true, synchronous_type_collection, synchronous_watcher,
                synchronous_distributed_world);
            synchronous_saved_identifier_source = &machine_identifier_storage;
            synchronous_identifier_destination = synchronous_machine_identifier;
            memcpy(synchronous_identifier_destination, synchronous_saved_identifier_source, sizeof(s_machine_identifier));
            client_simulation_type = _game_simulation_synchronous_client;
        }
        m_local_machine_index = machine_index;
        m_machine_identifier_valid = 1;
        this->recreate_players(client_simulation_type);
        if (attached_to_map) {
            m_attached_to_map = 1;
        }
        result = true;
    }
    return result;
}

// @address 0x3bf8f8 ?claim_authority@c_simulation_world@@QAA_NXZ
bool c_simulation_world::claim_authority()
{
    bool result = false;
    long world_type = m_world_type;
    if (world_type != 4) {
        if (world_type == 8) {
            disconnect();
            m_world_type = 7;
            simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_set_simulation_to_distributed_server);
            m_distributed_world->m_replication_entity_manager.make_all_local();
            simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_claim_authority);
            recreate_players(_game_simulation_distributed_server);
            return true;
        }
    } else if (!synchronous_gamestate_write_in_progress()) {
        c_simulation_watcher *watcher = (c_simulation_watcher *)m_watcher;
        c_simulation_distributed_world *distributed_world = m_distributed_world;
        long machine_index = m_local_machine_index;
        s_machine_identifier machine_identifier = m_machine_identifier;
        bool attached = m_attached_to_map;
        destroy_world();
        game_simulation_set(_game_simulation_synchronous_server);
        initialize_world(_game_simulation_synchronous_server, 0, false, g_simulation_globals.type_collection,
            watcher, distributed_world);
        m_machine_identifier = machine_identifier;
        result = true;
        m_local_machine_index = machine_index;
        m_machine_identifier_valid = result;
        recreate_players(_game_simulation_synchronous_server);
        if (attached)
            m_attached_to_map = result;
        change_state_joining(0);
        change_state_active();
    }
    return result;
}


// @address 0x3bfa48 ?claim_authority_gameworld@c_simulation_world@@SAXXZ
void c_simulation_world::claim_authority_gameworld()
{
    bool distributed = false;
    unsigned char multiplayer_engine;
    if ((unsigned int)(tls_game_globals()->options.simulation_type - 4) <= 1)
        distributed = true;
    if (distributed)
    {
        multiplayer_engine = (tls_game_globals()->options.game_mode == 2 && (unsigned char)game_engine_running() != 0);
        hs_runtime_dispose_from_old_map();
        hs_runtime_initialize_for_new_map();
        ::object_placement_create_global_objects(tls_game_globals()->options.game_mode, 1);
        ::object_placement_create_active_zone_set_objects(0);
        if (multiplayer_engine != 0)
        {
            tls_game_engine_globals()->get_candy_monitor_manager()->initialize_for_new_map();
            map_variant_initialize_for_new_map(&tls_game_engine_globals()->map_info);
        }
        if ((unsigned char)game_engine_running() != 0)
            game_engine_promote_to_simulation_authority();
        if ((unsigned char)game_engine_running() != 0 && (unsigned char)game_is_cooperative() == 0)
        {
            g_game_results_globals.active = 1;
            g_game_results_globals.finished = 0;
            game_results_notify_player_indices_changed();
            game_results_notify_active_teams_changed();
        }
    }
}

// @address 0x3bfb50 ?disconnect@c_simulation_world@@QAAXXZ
void c_simulation_world::disconnect()
{
    this->remove_all_views();
    long state = m_world_state;
    bool connected = false;
    if ((unsigned long)(state - 4) <= 2)
        connected = true;
    if (!connected && state != 3)
        return;
    change_state_disconnected();
}

struct s_game_state_compressed_buffer;

// @address 0x3bfbb0 ?change_state_internal@c_simulation_world@@QAAXW4e_simulation_world_state@@@Z
void c_simulation_world::change_state_internal(e_simulation_world_state state)
{
    if (state != _simulation_world_state_active) {
        time_set_immediate_update(false);
        if (m_time_running)
            m_time_running = false;
    }
    long old_state = m_world_state;
    if (old_state != _simulation_world_state_joining) {
        if (old_state == _simulation_world_state_active)
            m_last_active_time = network_time_get();
    }
    else {
        bool adjust_time = m_world_type != 4 && m_world_type != 6 && m_world_type != 8;
        if (adjust_time) {
            long saved_time = m_join_start_time;
            m_total_join_time = m_total_join_time - saved_time
                + (g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount());
        }
        if (state != _simulation_world_state_active)
            m_join_attempt_count++;
    }
    if (state != _simulation_world_state_joining) {
        if (game_state_globals.simulation_start_pending)
            game_state_globals.simulation_start_pending = false;
        if (game_state_globals.simulation_started)
            game_state_dispose_compressed_buffer(&game_state_globals.compressed);
    }
    m_world_state = state;
}

// @address 0x3bfd00 ?change_state_joining@c_simulation_world@@QAAXK@Z
void c_simulation_world::change_state_joining(unsigned long flags)
{
    change_state_internal(_simulation_world_state_joining);
    unsigned long now;
    if (g_network_time_globals.enabled)
        now = g_network_time_globals.time;
    else
        now = GetTickCount();
    long state_flags = m_join_flags;
    m_join_start_time = now;
    m_join_flags = flags | state_flags;
}

// @address 0x3bfd70 ?change_state_active@c_simulation_world@@QAAXXZ
void c_simulation_world::change_state_active()
{
    m_join_attempt_count = 0;
    if (m_time_running)
        time_set_immediate_update(false);
    else
        time_start(next_update_number, true);
    change_state_internal(_simulation_world_state_active);
    m_join_start_time = 0;
}

// @address 0x3bf610 simulation_world_test_all_client_views_active
extern "C" bool simulation_world_test_all_client_views_active(c_simulation_world *world)
{
    bool result = true;
    if (world->is_authority() && world->m_world_state == _simulation_world_state_active && !world->is_local()) {
        s_simulation_world_view_iterator iterator;
        c_simulation_view *slot;
        iterator.type_mask = 0x14;
        iterator.index = 0;
        while (world->iterator_next(&iterator, &slot)) {
            if (!slot->m_active) {
                result = false;
                break;
            }
        }
    }
    return result;
}


// @address 0x80608 GetTickCount
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0xa1970 ?player_action_clear@@YAXPAUplayer_action@@@Z
// @address 0xa58d0 ?clear@c_simulation_queue@@QAAXXZ
// @address 0xa8730 ?unit_control_clear@@YAXPAUunit_control_data@@@Z
// @address 0x3af058 ?simulation_gamestate_entities_notify_simulation_world_reset@@YAXXZ
// @address 0x3d5778 ?get_pending_acknowledgement@c_simulation_watcher@@QAA_N_NPAKPAUs_machine_identifier@@11PAUs_player_identifier@@@Z
// @address 0x3d6b50 ?allocate@c_simulation_queue@@QAAXJPAPAUs_simulation_queue_element@@@Z
// @address 0x3d71e8 ?send_message@c_simulation_view@@QBAXW4e_network_message_type@@JPBX_N@Z
// @address 0x3d7410 ?set_view_establishment@c_simulation_view@@QAAXW4e_simulation_view_establishment_mode@@J@Z
// @address 0x3d8418 ?network_time_since@@YAJPAVc_simulation_view@@@Z
// @address 0x409530 ?reset@c_simulation_entity_database@@QAAXXZ
// @address 0x419d68 ?reset@c_replication_event_manager_view@@QAAXXZ
// @address 0x41a690 ?reset@c_replication_entity_manager_view@@QAAXXZ
// @address 0x57ead8 ?allocate@c_rockall_heap@@QAAPAXJ@Z
// @address 0x3a2f98 ?set@?$c_generic_network_session_parameter@Us_network_session_parameter_synchronous_out_of_sync@@@@QAA_NPBUs_network_session_parameter_synchronous_out_of_sync@@@Z

__forceinline void c_simulation_event_handler::destroy()
{
    m_event_manager->m_client = 0;
    m_world = 0;
    m_event_manager = 0;
    m_type_collection = 0;
    m_initialized = false;
}

// @address 0x3bcde8 ?destroy_world@c_simulation_world@@QAAXXZ
void c_simulation_world::destroy_world()
{
    disconnect();
    while (view_count > 0)
        simulation_remove_view_from_world(views[view_count - 1]);
    delete_all_players();
    delete_all_actors();
    if (is_distributed()) {
        m_distributed_world->event_handler.destroy();
        c_simulation_entity_database *database = &m_distributed_world->m_entity_database;
        database->reset();
        s_simulation_world_database_initialization *database_layout = (s_simulation_world_database_initialization *)database;
        database_layout->entity_manager->m_client = 0;
        database_layout->initialized = false;
        database_layout->world = 0;
        database_layout->entity_manager = 0;
        database_layout->types = 0;
        simulation_gamestate_entities_notify_simulation_world_reset();
    }
    if (runs_simulation()) {
        bookkeeping.dispose();
        simulation.dispose();
    } else {
        update_queue_reset();
    }
    m_watcher = 0;
    m_distributed_world = 0;
    m_world_type = 0;
}

static __forceinline void simulation_world_entity_manager_initialize(c_replication_entity_manager *manager)
{
    manager->m_client = 0;
    manager->m_view_mask = 0;
    for (long view_index = 0; view_index < 15; view_index++)
        manager->m_views[view_index] = 0;
    manager->reset();
}
static __forceinline void simulation_world_event_manager_initialize(c_replication_event_manager *manager,
    c_replication_entity_manager *entity_manager)
{
    c_replication_event_manager *layout = manager;
    layout->m_entity_manager = entity_manager;
    layout->m_client = 0;
    layout->m_view_mask = 0;
    memset(layout->m_views, 0, sizeof(layout->m_views));
    layout->m_outgoing_event_count = 0;
    layout->m_outgoing_events = 0;
}
static __forceinline void simulation_world_entity_database_initialize(c_simulation_entity_database *database,
    c_simulation_world *world,
    c_replication_entity_manager *entity_manager, c_simulation_type_collection *types)
{
    s_simulation_world_database_initialization *layout = (s_simulation_world_database_initialization *)database;
    layout->deleting = false;
    layout->world = world;
    layout->entity_manager = entity_manager;
    layout->types = types;
    entity_manager->m_client = database;
    database->reset();
    layout->initialized = true;
}
__forceinline void c_simulation_event_handler::initialize(c_simulation_world *world,
    c_replication_event_manager *event_manager, c_simulation_type_collection *type_collection,
    c_simulation_entity_database *entity_database)
{
    m_world = world;
    m_type_collection = type_collection;
    m_event_manager = event_manager;
    m_entity_database = entity_database;
    event_manager->m_client = this;
    m_initialized = true;
}
static __forceinline void simulation_world_queue_initialize(c_simulation_queue *queue)
{
    queue->allocated_elements = 0;
    queue->allocated_bytes = 0;
    queue->count = 0;
    queue->bytes = 0;
    queue->first = 0;
    queue->last = 0;
    queue->initialized = true;
}

// match: 0x98160: the inline distribution boundary preserves the call-site instruction count.
__forceinline void simulation_world_distribute_update(c_simulation_world *world, const simulation_update *update,
    const s_simulation_update_metadata *metadata)
{
    if (simulation_world_is_synchronous(world))
        simulation_world_update_synchronous_views(world, update, metadata);
    if (world->is_distributed()) {
        const simulation_update *inputs = update;
        world->distributed_authority_dispatch_player_actions(inputs->player_valid_mask, inputs->actions);
        world->distributed_authority_dispatch_actor_control(inputs->actor_valid_mask, inputs->actor_controls);
    }
}

// @address 0x97580 ?update@c_simulation_world@@QAAXXZ
void c_simulation_world::update()
{
    bool is_client = m_world_type != 4 && m_world_type != 6 && m_world_type != 8;
    if (is_client) {
        is_client = false;
        if ((unsigned long)(m_world_state - 4) <= 2)
            is_client = true;
        if (!is_client && m_world_state != _simulation_world_state_joining && m_world_state != _simulation_world_state_dead
            && !is_out_of_sync())
            simulation_world_update_disconnected(this);
        if (m_world_state == _simulation_world_state_joining)
            update_authority_join_progress();
        if (m_world_state == _simulation_world_state_active)
            simulation_world_update_active(this);
        if (m_world_state == _simulation_world_state_handoff)
            simulation_world_update_all_view_establishments(this);
        unsigned long left_game_mask = simulation_player_left_game(this);
        s_player_identifier identifier;
        for (long player_index = 0; player_index < 16; player_index++) {
            c_simulation_player *player = &m_players[player_index];
            if (player->m_control_index != -1 && !player->m_active) {
                identifier = player->m_identifier;
                if (((c_simulation_watcher *)m_watcher)->get_player_is_in_game(player_index, &identifier)
                    && ((1 << player_index) & left_game_mask) != 0 && !player->m_pending_deletion)
                    player->m_active = true;
            }
        }
    } else {
        is_client = false;
        if ((unsigned long)(m_world_state - 4) <= 2)
            is_client = true;
        if (!is_client && m_world_state != _simulation_world_state_joining && m_world_state != _simulation_world_state_dead
            && !is_out_of_sync())
            update_authority_handoff();
        if (m_world_state == _simulation_world_state_joining)
            update_client_join_progress();
        if (m_world_state != _simulation_world_state_active && m_world_state != _simulation_world_state_dead) {
            long elapsed = network_time_since(m_last_active_time);
            bool timed_out = false;
            if (m_join_attempt_count >= g_network_configuration.simulation_maximum_join_attempts
                || elapsed >= g_network_configuration.simulation_join_failure_timeout)
                timed_out = true;
            if (timed_out && m_world_state != _simulation_world_state_dead) {
                disconnect();
                change_state_internal(_simulation_world_state_dead);
            }
        }
        long world_state = m_world_state;
        bool check_views = false;
        if ((unsigned long)(world_state - 4) <= 2)
            check_views = true;
        if (check_views || world_state == _simulation_world_state_joining) {
            c_simulation_view *view = 0;
            s_simulation_world_view_iterator iterator;
            iterator_begin(&iterator, 10);
            bool disconnect_view = false;
            iterator_next(&iterator, &view);
            if (!view) {
                disconnect_view = true;
            } else {
                bool established = view->established();
                if (!established && world_state != _simulation_world_state_joining)
                    disconnect_view = true;
            }
            if (disconnect_view)
                disconnect();
        }
        if (!g_simulation_globals.reset_in_progress) {
            send_player_acknowledgements(false);
            bool synchronous = simulation_world_is_synchronous(this);
            if (synchronous && m_world_type != 4) {
                c_simulation_view *view = 0;
                s_simulation_world_view_iterator iterator;
                iterator_begin(&iterator, 10);
                iterator_next(&iterator, &view);
                if (view) {
                    bool established = view->established();
                    if (established) {
                        long update_number = next_update_number - 1;
                        if (update_number != m_last_distributed_update_number) {
                            long message_update_number = update_number;
                            view->send_message(_network_message_type_synchronous_acknowledge, 4,
                                &message_update_number, true);
                            m_last_distributed_update_number = update_number;
                        }
                    }
                }
            }
        }
    }
    c_simulation_view *view;
    s_simulation_world_view_iterator iterator;
    iterator.index = 0;
    iterator.type_mask = -1;
    if (iterator_next(&iterator, &view)) {
        do {
            view->update();
        } while (iterator_next(&iterator, &view));
    }
}

// @address 0x98160 ?distribute_update@c_simulation_world@@SAXPBUsimulation_update@@PBUs_simulation_update_metadata@@@Z
void c_simulation_world::distribute_update(const simulation_update *update,
    const s_simulation_update_metadata *metadata)
{
    if (g_simulation_globals.world->is_authority())
        simulation_world_distribute_update(g_simulation_globals.world, update, metadata);
    c_simulation_world *current_world = g_simulation_globals.world;
    current_world->next_update_number = update->update_number + 1;

}

// @address 0x3bc948 ?initialize_world@c_simulation_world@@QAAXW4e_game_simulation_type@@J_NPAVc_simulation_type_collection@@PAVc_simulation_watcher@@PAVc_simulation_distributed_world@@@Z
void c_simulation_world::initialize_world(e_game_simulation_type simulation_type, long playback_type, bool reset_time,
    c_simulation_type_collection *types, c_simulation_watcher *watcher,
        c_simulation_distributed_world *distributed_world)
{
    m_watcher = watcher;
    if (playback_type != 0) {
        if (playback_type != 1) {
            if (playback_type != 2)
                m_world_type = _simulation_world_type_synchronous_playback_client;
            else
                m_world_type = _simulation_world_type_synchronous_playback_authority;
        } else {
            m_world_type = _simulation_world_type_local_playback;
        }
    } else {
        switch (simulation_type) {
        case _game_simulation_local: m_world_type = _simulation_world_type_local; break;
        case _game_simulation_synchronous_server: m_world_type = _simulation_world_type_synchronous_authority; break;
        case _game_simulation_synchronous_client: m_world_type = _simulation_world_type_synchronous_client; break;
        case _game_simulation_distributed_server: m_world_type = _simulation_world_type_distributed_authority; break;
        case _game_simulation_distributed_client: m_world_type = _simulation_world_type_distributed_client; break;
        // match: 0x3bc948: unreachable hint omits dead branch check
        default: __assume(0);
        }
    }
    m_world_state = _simulation_world_state_none;
    m_time_running = false;
    m_immediate_update = false;
    m_attached_to_map = false;
    m_out_of_sync = false;
    if (reset_time) {
        next_update_number = 0;
        m_last_distributed_update_number = -1;
    }
    m_gamestate_flushed = false;
    m_gamestate_flushed_outside_game_tick = false;
    m_gamestate_received = false;
    m_gamestate_loaded_from_saved_film = false;
    m_join_attempt_count = 0;
    m_last_active_time = network_time_get();
    m_next_establishment_identifier = 0;
    m_total_join_time = 0;
    m_handoff_state = 0;
    m_maximum_updates = 128;
    m_handoff_time = -1;
    if (is_distributed()) {
        m_distributed_world = distributed_world;
        simulation_world_entity_manager_initialize(&distributed_world->m_replication_entity_manager);
        simulation_world_event_manager_initialize(&m_distributed_world->m_replication_event_manager,
            &m_distributed_world->m_replication_entity_manager);
        simulation_world_entity_database_initialize(&m_distributed_world->m_entity_database, this,
            &m_distributed_world->m_replication_entity_manager, types);
        m_distributed_world->event_handler.initialize(this, &m_distributed_world->m_replication_event_manager,
            types, &m_distributed_world->m_entity_database);
    }
    if (simulation_world_is_synchronous(this))
        // match: 0x3bc948: the unsigned -1 keeps its own register (r22, 0xB0 frame);
        // a long store shares it with the later -1 stores (r23, 0xA0 frame).
        *(unsigned long *)&m_synchronous_catchup_failure_time = -1;
    if (runs_simulation()) {
        simulation_world_queue_initialize(&bookkeeping);
        simulation_world_queue_initialize(&simulation);
    }
    if (!runs_simulation()) {
        update_queue_length = 0;
        update_queue_length_copy = 0;
        update_queue_head = 0;
        update_queue_tail = 0;
        update_queue_reset();
    }
    view_count = 0;
    memset(views, 0, sizeof(views));
    m_machine_identifier_valid = false;
    m_local_machine_index = -1;
    change_state_disconnected();
}

// @address 0x3be090 simulation_world_update_disconnected
extern "C" void simulation_world_update_disconnected(c_simulation_world *world)
{
    if (simulation_world_is_synchronous(world) && !game_state_globals.simulation_started) {
        game_state_globals.simulation_start_pending = true;
        return;
    }
    if (world->is_local()) {
        world->change_state_joining(0);
        world->change_state_active();
        return;
    }
    world->change_state_joining(((c_simulation_watcher *)world->m_watcher)->m_machine_valid_mask);
}

// @address 0x3be188 ?update_authority_join_progress@c_simulation_world@@AAAXXZ
void c_simulation_world::update_authority_join_progress()
{
    const s_network_configuration *join_config = &g_network_configuration;
    unsigned long joining = 0;
    unsigned long waiting = 0;
    unsigned long joined = 0;
    unsigned long join_flags = ((c_simulation_watcher *)m_watcher)->m_machine_valid_mask;
    long elapsed = network_time_since(m_join_start_time);
    for (long machine_index = 0; machine_index < 16; machine_index++) {
        unsigned long bit = 1 << machine_index;
        if ((bit & join_flags) != 0 && machine_index != m_local_machine_index) {
            c_simulation_view *view = get_client_view_by_machine_index(machine_index);
            joining |= bit;
            if (view && !view->is_dead()) {
                if (view->established()) {
                    if ((m_join_flags & bit) != 0) {
                        if (update_joining_view(view) == _join_progress_incomplete)
                            waiting |= bit;
                        else
                            joined |= bit;
                    }
                } else {
                    simulation_world_update_view_establishment(this, view);
                }
            }
        }
    }
    bool update_timed_out = elapsed >= join_config->simulation_join_complete_timeout;
    bool join_incomplete = false;
    unsigned long remaining = (m_join_flags & ~joined) & joining;
    if (elapsed >= join_config->simulation_join_partial_timeout) {
        unsigned long remaining_count = count_bits<unsigned long>(remaining);
        unsigned long total_count = count_bits<unsigned long>(join_flags);
        if ((float)remaining_count / (float)total_count - join_config->simulation_join_completion_threshold < k_real_math_epsilon_storage)
            join_incomplete = true;
    }
    bool synchronous = simulation_world_is_synchronous(this);
    if (synchronous) {
        bool catchup_expired = false;
        join_incomplete = false;
        if (elapsed < (join_config->simulation_join_timeout << 2))
            update_timed_out = false;
        else
            catchup_expired = true;
        for (long i = 0; i < 16; i++) {
            unsigned long bit = 1 << i;
            if ((bit & joining) != 0 && (bit & waiting) != 0) {
                c_simulation_view *view = get_client_view_by_machine_index(i);
                long timeout = join_config->simulation_join_timeout;
                long catchup_end = view->m_catchup_end;
                if (catchup_end != -1
                    && (network_time_since(catchup_end) > timeout || catchup_expired))
                    simulation_view_handle_join_timeout(view);
            }
        }
    }
    if (remaining == 0 || update_timed_out || join_incomplete) {
        if (simulation_world_is_synchronous(this))
            simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_players_update_for_checkpoint);
        m_join_attempt_count = 0;
        if (m_time_running)
            time_set_immediate_update(false);
        else
            time_start(next_update_number, true);
        change_state_internal(_simulation_world_state_active);
        m_join_start_time = 0;
    }
}

// @address 0x3be7a0 ?update_client_join_progress@c_simulation_world@@AAAXXZ
void c_simulation_world::update_client_join_progress()
{
    c_simulation_view *view = get_authority_view();
    bool abort = false;
    if (!view) {
        abort = true;
    } else if (view->m_active) {
        change_state_active();
    } else {
        long elapsed;
        if (is_distributed()) {
            elapsed = network_time_since(this->m_join_start_time);
        } else if (view->m_catchup_attempt != -1) {
            if (view->m_catchup_data == 5) {
                long state = game_state_globals.catchup_state;
                if (state != 1 && state != 2) {
                if (state == 0) {
                    network_time_since(view);
                    view->send_message(_network_message_type_synchronous_client_ready, 0, 0, false);
                } else {
                    abort = true;
                }
                }
            }
            elapsed = 0;
        } else if (view->m_catchup_finish_time != -1) {
            elapsed = network_time_since(view->m_catchup_finish_time);
        } else {
            elapsed = network_time_since(this->m_join_start_time);
        }
        if (elapsed >= g_network_configuration.simulation_join_timeout)
            abort = true;
    }
    if (abort)
        disconnect();
}

// @address 0x3bee08 ?send_player_acknowledgements@c_simulation_world@@QAAX_N@Z
void c_simulation_world::send_player_acknowledgements(bool force)
{
    c_simulation_view *view = get_authority_view();
    if (view && view->established()) {
        unsigned long acknowledgement_type;
        s_machine_identifier machines[16];
        s_simulation_player_acknowledgement message;
        if (((c_simulation_watcher *)m_watcher)->get_pending_acknowledgement(force,
            &acknowledgement_type, machines, &message.update, &message.flags, message.players))
            view->send_message(_network_message_type_player_acknowledge, sizeof(message), &message, false);
    }
}

// @address 0x3bfde0 ?change_state_disconnected@c_simulation_world@@QAAXXZ
void c_simulation_world::change_state_disconnected()
{
    if (m_world_state != _simulation_world_state_disconnected) {
        change_state_internal(_simulation_world_state_disconnected);
        m_join_start_time = network_time_get();
    }
}

// @address 0x2251c k_real_zero_storage

// @address 0x3b53b8 simulation_world_compute_frame_rate_tier
extern "C" int simulation_world_compute_frame_rate_tier(void)
{
    int result = 0;
    float scale = rasterizer_get_average_frame_delta();
    float ratio = scale > k_real_hundredth_storage ? (float)rasterizer_get_refresh_rate() / scale : k_real_zero_storage;
    int rate = (int)ratio;
    if (rate >= 28)
        result = 1;
    else if (rate >= 24)
        result = 2;
    else if (rate != -1)
        result = 3;
    return result;
}

// @address 0x7f1957 g_simulation_authority_enabled

static __forceinline bool simulation_world_authority_in_squad_session(c_network_session **out_session)
{
    bool result = false;
    if (network_life_cycle_globals_get()->initialized) {
        c_network_session *session = network_life_cycle_globals_get()->manager.m_active_squad_session;
        if (!(session->current_local_state() == 0)) {
            if (out_session)
                *out_session = session;
            result = true;
        }
    }
    return result;
}

static __forceinline const long *simulation_world_authority_slot_value(const c_generic_network_session_parameter<long> *slot)
{
    const long *result = 0;
    if (slot->m_flags & 1)
        result = &slot->m_data;
    return result;
}

// Build the four CHUD network-status values.
// @address 0x3b5468 ?simulation_world_build_network_status@@YAXPAJ@Z
void simulation_world_build_network_status(long *values)
{
    values[0] = 0;
    // match: 0x3b5468: the compiler barrier keeps the flag load after the values[0] store and the flag test after the values[1] and values[2] stores.
    _ReadWriteBarrier();
    bool enabled = g_simulation_authority_enabled;
    values[1] = 0;
    values[2] = 0;
    values[3] = 0;
    if (!enabled)
        return;
    if (network_life_cycle_globals_get()->initialized) {
        c_network_observer *observer = network_life_cycle_globals_get()->manager.m_observer;
        if (observer != 0) {
            for (long index = 0; index < 32; index++) {
                const c_network_observer::s_channel_observer *channel = &observer->m_channel_observers[index];
                if (channel->state != 0 && channel->stream_packet_rate_halved) {
                    long latency = channel->latency_average;
                    if (latency != -1) {
                        if (latency <= 50)
                            values[0] = 5;
                        else if (latency >= 500)
                            values[0] = 50;
                        else
                            values[0] = latency / 10;
                        if (latency < 150)
                            values[1] = 1;
                        else if (latency < 200)
                            values[1] = 2;
                        else
                            values[1] = 3;
                    }
                    break;
                }
            }
        }
    }
    {
        c_network_session *session = 0;
        if (simulation_world_authority_in_squad_session(&session) && session->established()) {
            if (!((unsigned char)session->is_host())) {
                const c_network_session_parameters *parameters = (const c_network_session_parameters *)session->m_session_parameters;
                if (parameters->m_parameters.host_frame_quality.m_flags & 1)
                    values[2] = *simulation_world_authority_slot_value(&parameters->m_parameters.host_frame_quality);
            }
        }
    }
    values[3] = simulation_world_compute_frame_rate_tier();
    return;
}

// @address 0x3bed30 simulation_world_update_view_establishment
extern "C" void simulation_world_update_view_establishment(c_simulation_world *world, c_simulation_view *view)
{
    if (view->m_establishment_mode != _simulation_view_establishment_mode_established) {
        bool allocate;
        if (view->m_establishment_mode == _simulation_view_establishment_mode_ready_to_connect
            && view->m_remote_establishment_mode == _simulation_view_establishment_mode_ready_to_connect)
            allocate = true;
        else
            allocate = false;
        e_simulation_view_establishment_mode mode;
        long id_value;
        if (allocate) {
            long identifier = world->m_next_establishment_identifier;
            world->m_next_establishment_identifier = identifier + 1;
            if (identifier < 0) {
                view->kill_view(_simulation_view_reason_invalid_establishment);
                return;
            }
            if (view->m_establishment_mode == _simulation_view_establishment_mode_established
                && view->m_establishment_identifier == identifier)
                return;
            mode = _simulation_view_establishment_mode_established;
            id_value = identifier;
        } else {
            if (view->m_establishment_mode == _simulation_view_establishment_mode_ready_to_connect)
                return;
            id_value = -1;
            mode = _simulation_view_establishment_mode_ready_to_connect;
        }
        view->m_establishment_mode = mode;
        view->m_establishment_identifier = id_value;
        simulation_view_send_establishment(view);
        view->update_view_activation_state();
    }
}

// @address 0x3be590 simulation_world_update_all_view_establishments
extern "C" void simulation_world_update_all_view_establishments(c_simulation_world *world)
{
    s_simulation_world_view_iterator iterator;
    c_simulation_view *view;
    world->iterator_begin(&iterator, 0xffffffff);
    if (world->iterator_next(&iterator, &view)) {
        do {
            if (!view->is_dead() && view->get_view_establishment_mode() > _simulation_view_establishment_mode_established) {
                long identifier = view->get_view_establishment_identifier();
                if (identifier < 0) {
                    view->kill_view(_simulation_view_reason_invalid_establishment);
                } else {
                    _ReadWriteBarrier();
                    if (view->get_view_establishment_mode() == _simulation_view_establishment_mode_established
                        && view->get_view_establishment_identifier() == identifier)
                        continue;
                    view->m_establishment_mode = _simulation_view_establishment_mode_established;
                    view->m_establishment_identifier = identifier;
                    simulation_view_send_establishment(view);
                    view->update_view_activation_state();
                }
            }
        } while (world->iterator_next(&iterator, &view));
    }
}

// @address 0x3be658 ?update_authority_handoff@c_simulation_world@@AAAXXZ
void c_simulation_world::update_authority_handoff()
{
    c_simulation_view *view = get_authority_view();
    if (simulation_world_is_synchronous(this) && !game_state_globals.simulation_started) {
        game_state_globals.simulation_start_pending = true;
    } else if (view != 0 && view->get_channel() != 0 && !view->is_dead()) {
        if (m_watcher->get_session() != 0 && m_watcher->get_session()->current_local_state() == 4) {
            if (view->get_view_establishment_mode() != _simulation_view_establishment_mode_ready_to_connect
                || view->get_view_establishment_identifier() != -1) {
                view->m_establishment_mode = _simulation_view_establishment_mode_ready_to_connect;
                view->m_establishment_identifier = -1;
                simulation_view_send_establishment(view);
                view->update_view_activation_state();
            }
            change_state_joining(0);
        }
    }
}

static __forceinline void simulation_world_input_begin_writing(c_bitstream *stream)
{
    stream->m_data_size_alignment = 1;
    stream->m_state = 1;
    stream->m_current.current_stream_bit_position = 0;
    stream->m_current.current_memory_bit_position = 0;
    stream->m_position_stack_depth = 0;
    stream->m_error_occurred = false;
    stream->m_current.next_data = stream->m_data;
    stream->m_encoded_value_bits = 0;
    stream->m_current.window = 0;
    stream->m_current.window_bits_used = 0;
    stream->m_number_of_bits_rewound = 0;
    stream->m_number_of_position_resets = 0;
}

// @address 0xa25c0 ?handle_local_input@c_simulation_player@@QAAXPBUplayer_action@@K@Z
void c_simulation_player::handle_local_input(const player_action *action, unsigned long flags)
{
    c_simulation_player *player = this;
    if (player->m_state == 1 || player->m_state == 2) {
        c_replication_control_view *control = 0;
        c_simulation_view *view = player->m_world->get_authority_view();
        if (view)
            control = &view->m_distributed_view->control;
        s_player_motion motion;
        bool motion_valid = player->build_motion_data(action, true, &motion);
        s_player_prediction prediction;
        bool prediction_valid = simulation_game_build_player_prediction(player->m_player_index, &prediction);
        if (control) {
            long control_index = player->m_control_index;
            if (motion_valid) {
                control->m_motions[control_index] = motion;
                control->m_motion_available_send |= 1 << control_index;
            }
            if (player->m_state == 1 && prediction_valid) {
                control->m_predictions[control_index] = prediction;
                control->m_prediction_available_send |= 1 << control_index;
            }
        }
    }
    unsigned char buffer[116];
    c_bitstream stream;
    stream.m_data_size_alignment = 1;
    stream.m_data = buffer;
    stream.m_data_max = buffer + sizeof(buffer);
    stream.m_data_size_bytes = sizeof(buffer);
    stream.reset(0);
    simulation_world_input_begin_writing(&stream);
    player_action_encode(&stream, action);
    stream.finish_writing(0);
    stream.reset(3);
    player_action_decode(&stream, &player->m_last_action);
    player->m_last_time = game_time_globals->time;
    player->m_last_action_flags = flags;
}

static __forceinline void simulation_world_build_machine_update(bool *valid, simulation_machine_update *update)
{
    c_simulation_watcher *watcher_object = g_simulation_globals.watcher;
    c_simulation_watcher *watcher = watcher_object;
    *valid = false;
    if (watcher->m_machines_from_gamestate) {
        update->machine_valid_mask = watcher->m_machine_valid_mask;
        memcpy(update->machine_identifiers, watcher->m_machine_identifiers, sizeof(watcher->m_machine_identifiers));
        watcher->apply_machines_to_players();
        *valid = true;
        watcher->m_machines_from_gamestate = false;
    }
}
static __forceinline void simulation_world_build_move_queue(c_simulation_queue *destination, c_simulation_queue *source)
{
    if (source->queued_count() > 0)
        ((c_simulation_world *)destination)->build_player_actions((simulation_update *)source);
}
static __forceinline void simulation_world_build_destroy_queue(c_simulation_queue *queue)
{
    if (queue->initialized) {
        queue->clear();
        queue->initialized = false;
    }
}

static __forceinline unsigned char simulation_world_build_test_flag(short flags, long bit)
{
    return ((unsigned long)flags >> bit) & 1;
}

// @address 0xa46c0 ?build_update@c_simulation_world@@QAAX_NPAUsimulation_update@@PAUs_simulation_update_metadata@@@Z
void c_simulation_world::build_update(bool simulation_in_progress, struct simulation_update * update_storage,
    struct s_simulation_update_metadata * metadata)
{
    if (this->runs_simulation()) {
        simulation_update *update = update_storage;
        metadata->flags |= 1;
        update->update_number = this->next_update_number;
        update->verify_game_time = game_time_globals->time;
        update->verify_random = *tls_random_seed();
        simulation_world_build_machine_update(&update->machine_update_exists, &update->machine_update);
        simulation_build_player_updates();
        if (simulation_in_progress)
            update->flags |= 1;
        else
            update->flags &= ~1;
        if ((unsigned short)update->flags & 1) {
            simulation_build_player_and_actor_update(this, update_storage);
            if (is_authority() && this->m_gamestate_flushed) {
                update->flags |= 2;
                this->m_gamestate_flushed = false;
            }
            if (this->m_gamestate_flushed_outside_game_tick) {
                update->flags |= 4;
                this->m_gamestate_flushed_outside_game_tick = false;
            }
        }
        simulation_world_queue_initialize(&update_storage->bookkeeping);
        simulation_world_queue_initialize(&update_storage->simulation);
        simulation_world_build_move_queue(&update_storage->bookkeeping, &bookkeeping);
        if (simulation.queued_count() > 0)
            update->flags |= 8;
        if (simulation_world_build_test_flag(update->flags, 0) || simulation_world_build_test_flag(update->flags, 3))
            simulation_world_build_move_queue(&update_storage->simulation, &simulation);
        unsigned char buffer[0x20000];
        c_bitstream stream;
        stream.m_data_size_alignment = 1;
        stream.m_data = buffer;
        stream.m_data_max = buffer + sizeof(buffer);
        stream.m_data_size_bytes = sizeof(buffer);
        stream.reset(0);
        stream.begin_writing(1, 0, false);
        simulation_update_encode(&stream, update_storage);
        stream.finish_writing(0);
        simulation_world_build_destroy_queue(&update_storage->bookkeeping);
        simulation_world_build_destroy_queue(&update_storage->simulation);
        stream.reset(3);
        simulation_update_decode(&stream, update_storage);
    } else {
        s_server_update_queue_element *element = update_queue_head;
        memcpy(update_storage, &element->update, sizeof(*update_storage));
        *metadata = element->metadata;
        update_queue_head = element->queue_next;
        if (update_queue_tail == element)
            update_queue_tail = 0;
        ::network_heap_free_block((void *)(element));
        update_queue_length--;
        update_queue_length_copy--;
        update_queue_applied_number++;
    }
}

// @address 0x3bd380 simulation_world_get_available_updates
extern "C" long simulation_world_get_available_updates(c_simulation_world *world, bool *match_remote_time,
    long *queued_updates)
{
    long queue_length = 0;
    long available_updates = 0;
    bool match_time = false;
    long maximum_updates;
    if (world->m_time_running)
    {
        queue_length = 0x7fffffff;
        available_updates = 0x7fffffff;
        switch (world->m_world_type)
        {
        case _simulation_world_type_local:
            break;
        case _simulation_world_type_local_playback:
            queue_length = world->update_queue_get_available_updates();
            available_updates = queue_length;
            break;
        case _simulation_world_type_synchronous_authority:
            maximum_updates = simulation_world_compute_available_update_count(world);
            available_updates = maximum_updates;
            break;
        case _simulation_world_type_synchronous_playback_authority:
        {
            maximum_updates = simulation_world_compute_available_update_count(world);
            queue_length = world->update_queue_get_available_updates();
            available_updates = queue_length <= maximum_updates ? queue_length : maximum_updates;
            break;
        }
        case _simulation_world_type_synchronous_client:
        case _simulation_world_type_synchronous_playback_client:
            queue_length = world->update_queue_get_available_updates();
            available_updates = queue_length;
            match_time = true;
            break;
        case _simulation_world_type_distributed_authority:
        case _simulation_world_type_distributed_client:
            break;
        default:
            // match: 0x3bd380: unreachable hint omits dead branch check
            __assume(0);
        }
    }
    if (queued_updates)
        *queued_updates = queue_length;
    if (match_remote_time)
        *match_remote_time = match_time;
    return available_updates;
}
// @address 0x3bd538 ?time_set_immediate_update@c_simulation_world@@QAAX_N@Z
void c_simulation_world::time_set_immediate_update(bool immediate)
{
    m_immediate_update = immediate;
    if (immediate && simulation_world_get_available_updates(this, 0, 0) > 0)
    {
        do
        {
            game_tick();
        } while (simulation_world_get_available_updates(this, 0, 0) > 0);
    }
}

// @address 0x97990 simulation_world_update_active
extern "C" void simulation_world_update_active(c_simulation_world *world)
{
    c_simulation_world *layout = world;
    const s_network_configuration *config = &g_network_configuration;
    long maximum_updates = 128;
    if (!world->is_local())
    {
        bool aborted = false;
        unsigned long local_machine_mask = 1 << world->m_local_machine_index;
        s_simulation_world_view_iterator iterator;
        c_simulation_view *view;
        iterator.type_mask = 20;
        iterator.index = 0;
        while (world->iterator_next(&iterator, &view))
        {
            if (view->m_death_reason != 0)
                continue;
            if (view->m_active)
            {
                local_machine_mask |= 1 << view->m_machine_index;
            }
            else if (view->m_establishment_mode != _simulation_view_establishment_mode_joined)
            {
                if (view->established())
                {
                    world->update_joining_view(view);
                }
                else
                {
                    simulation_world_update_view_establishment(world, view);
                    aborted = true;
                }
            }
        }

        if (((c_simulation_watcher *)world->m_watcher)->in_online_networked_session())
        {
            if (!simulation_world_is_synchronous(world))
            {
                unsigned long join_flags = ((c_simulation_watcher *)world->m_watcher)->m_machine_valid_mask;
                unsigned long join_mask = layout->m_join_start_time & ~local_machine_mask;
                join_mask &= join_flags;
                bool join_incomplete = false;
                if (join_mask != 0 && join_flags != 0)
                {
                    unsigned long boot_mask = 0;
                    for (long machine = 0; machine < 16; machine++)
                    {
                        unsigned long bit = 1 << machine;
                        if ((join_mask & bit) == 0)
                            continue;
                        c_simulation_watcher *watcher = (c_simulation_watcher *)world->m_watcher;
                        unsigned long player_mask = 0;
                        if ((watcher->m_machine_valid_mask & bit) != 0)
                        {
                            long peer =
                                watcher->m_session->get_session_membership()->get_peer_from_unique_identifier(
                                    (const s_transport_unique_identifier *)&watcher->m_machine_identifiers[machine]);
                            if (peer != -1)
                            {
                                const c_network_session_membership *membership = watcher->m_session->get_session_membership();
                                long player = membership->get_first_player();
                                while (player != -1)
                                {
                                    if (membership->get_player(player)->peer_index == peer)
                                        player_mask |= membership->get_player(player)->configuration.client.player_flags;
                                    player = membership->get_next_player(player);
                                }
                            }
                        }
                        if ((player_mask & 0x40) != 0)
                            boot_mask |= bit;
                    }
                    if (boot_mask != 0)
                        simulation_watcher_handle_machine_boot((c_simulation_watcher *)world->m_watcher, boot_mask,
                            _network_session_boot_reason_in_game_join_aborted);

                    long join_count = 0;
                    long flag_count = 0;
                    for (long machine = 0; machine < 16; machine++)
                    {
                        unsigned long bit = 1 << machine;
                        flag_count += (bit & join_flags) != 0;
                        join_count += (bit & join_mask) != 0;
                    }
                    join_incomplete = (float)join_count / (float)flag_count - config->simulation_join_active_fraction > rasterizer_states_mode_7_depth_bias;
                }
                if (join_incomplete)
                {
                    long elapsed = layout->m_total_join_time;
                    if (layout->m_world_state == _simulation_world_state_joining)
                        elapsed = network_time_since(layout->m_join_start_time) + elapsed;
                    bool timed_out = elapsed >= config->simulation_join_total_timeout;
                    if (timed_out)
                        simulation_watcher_handle_machine_boot((c_simulation_watcher *)world->m_watcher, join_mask,
                            _network_session_boot_reason_in_game_join_timed_out);
                    else
                        world->drop_simulation_from_active_to_joining();
                }
                else if (layout->m_world_state == _simulation_world_state_active)
                {
                    layout->m_join_start_time = (layout->m_join_start_time | local_machine_mask) & join_flags;
                }
            }
        }

        if (simulation_world_is_synchronous(world))
        {
            if (!world->is_playback()
                && !aborted
                && layout->m_world_state == _simulation_world_state_active
                && game_campaign_is_multiplayer())
            {
                bool distributed_client = false;
                if (tls_game_globals()->options.simulation_type == _game_simulation_distributed_client)
                    distributed_client = true;
                if (!distributed_client)
                {
                    if (players_joined_in_progress_and_waiting_to_respawn())
                        simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_players_update_for_checkpoint);
                }
            }
        }

        if (simulation_world_is_synchronous(world))
        {
            long update_number = simulation_world_get_max_unacknowledged_updates(world);
            switch (layout->m_handoff_state)
            {
            case 0:
                maximum_updates = 32;
                if (update_number > 8)
                {
                    layout->m_handoff_state = 1;
                    layout->m_handoff_time = network_time_get();
                }
                break;
            case 1:
                maximum_updates = 32;
                if (update_number > 8)
                {
                    if (network_time_since(layout->m_handoff_time) > 1000)
                    {
                        layout->m_handoff_state = 2;
                        maximum_updates = update_number;
                        layout->m_handoff_time = network_time_get();
                    }
                }
                else
                {
                    layout->m_handoff_state = 0;
                    layout->m_handoff_time = -1;
                }
                break;
            case 2:
                maximum_updates = 0;
                if (update_number == 0)
                {
                    layout->m_handoff_state = 3;
                    layout->m_handoff_time = -1;
                    maximum_updates = 8;
                }
                break;
            case 3:
                maximum_updates = 8;
                if (update_number < 8)
                {
                    if (layout->m_handoff_time == -1)
                    {
                        layout->m_handoff_time = network_time_get();
                    }
                    else if (network_time_since(layout->m_handoff_time) >= 1000)
                    {
                        layout->m_handoff_state = 0;
                        layout->m_handoff_time = -1;
                    }
                }
                else
                {
                    layout->m_handoff_time = -1;
                }
                break;
            }
        }
    }
    layout->m_maximum_updates = maximum_updates;
}

// @address 0x3bdb40 simulation_world_handle_synchronous_update
extern "C" bool simulation_world_handle_synchronous_update(
    c_simulation_world *world, const simulation_update *update, const s_simulation_update_metadata *metadata)
{
    bool accepted = false;
    long expected_update_number;
    if (world->update_queue_reset_value == 1)
        expected_update_number = world->last_update_number;
    else if (world->update_queue_reset_value == 0)
        expected_update_number = world->last_update_number + 1;
    else
        expected_update_number = world->update_queue_applied_number;
    if (world->synchronous_gamestate_write_in_progress())
    {
        simulation_world_set_out_of_sync(world, false);
    }
    else if (expected_update_number != -1 && update->update_number != expected_update_number)
    {
        simulation_world_set_out_of_sync(world, false);
    }
    else if (world->m_world_state == _simulation_world_state_active || world->m_immediate_update)
    {
        if (world->update_queue_handle_server_update(update, metadata))
        {
            if (world->m_immediate_update)
            {
                if (simulation_starting_up())
                {
                    do
                    {
                        if (simulation_aborted())
                            break;
                        if (world->is_out_of_sync())
                            break;
                        if (simulation_world_get_available_updates(world, 0, 0) <= 0)
                            break;
                        game_tick();
                    } while (simulation_starting_up());
                }
            }
            accepted = true;
        }
        else
        {
            g_simulation_globals.fatal_error = true;
        }
    }
    else
    {
        simulation_world_set_out_of_sync(world, false);
    }
    return accepted;
}

static __forceinline c_simulation_type_collection *simulation_world_get_type_collection()
{
    return g_simulation_globals.type_collection;
}
static __forceinline void simulation_queue_event_apply(const s_simulation_queue_element *element)
{
    s_simulation_queue_decoded_event_data event_data;
    memset(&event_data, 0, sizeof(event_data));
    if (decode_event_from_buffer((unsigned char *)element->data, element->data_size, &event_data))
    {
        c_simulation_event_definition *definition =
            simulation_world_get_type_collection()->get_event_definition((e_simulation_event_type)event_data.event_type);
        definition->apply_game_event(event_data.reference_count,
            event_data.reference_count > 0 ? event_data.references : 0,
            event_data.payload_size, event_data.payload_size > 0 ? event_data.payload : 0);
    }
}
static __forceinline void simulation_queue_entity_update_apply(const s_simulation_queue_element *element)
{
    if (game_simulation::game_is_distributed())
    {
        s_simulation_queue_decoded_update_data update_data;
        memset(&update_data, 0, sizeof(update_data));
        if (decode_simulation_queue_update_from_buffer((unsigned char *)element->data, element->data_size,
            &update_data))
        {
            c_simulation_entity_definition *definition =
                simulation_world_get_type_collection()->get_entity_definition(update_data.entity_type);
            definition->update_game_entity(update_data.gamestate_index, update_data.update_mask,
                update_data.state_data_size, update_data.state_data);
        }
    }
}
static __forceinline void simulation_queue_game_simulation_apply(const s_simulation_queue_element *element)
{
    c_bitstream packet;
    bitstream_construct(&packet, (unsigned char *)element->data, element->data_size);
    packet.reset(3);
    long value = packet.read_integer("simulation-type", 1);
    if (!packet.error_occurred() && value == 0)
        game_simulation_set(_game_simulation_distributed_client);
    packet.finish_reading();
}

// @address 0xa6a28 ?apply_simulation_queue@c_simulation_world@@SAXPBVc_simulation_queue@@@Z
void c_simulation_world::apply_simulation_queue(const c_simulation_queue *simulation_queue)
{
    if (simulation_queue->queued_count() > 0)
    {
        s_simulation_queue_element *element = simulation_queue->get_first_element();
        while (element)
        {
            switch (element->type)
            {
            case _simulation_queue_element_type_event: simulation_queue_event_apply(element); break;
            case _simulation_queue_element_type_entity_creation: simulation_queue_entity_creation_apply(element); break;
            case _simulation_queue_element_type_entity_update: simulation_queue_entity_update_apply(element); break;
            case _simulation_queue_element_type_entity_deletion: simulation_queue_entity_deletion_apply(element); break;
            case _simulation_queue_element_type_entity_promotion: simulation_queue_entity_promotion_apply(element); break;
            case _simulation_queue_element_type_game_global_event: simulation_queue_game_global_event_apply(element); break;
            case _simulation_queue_element_type_game_simulation: simulation_queue_game_simulation_apply(element); break;
            case _simulation_queue_element_type_player_event: simulation_queue_player_event_apply(element); break;
            case _simulation_queue_element_type_player_update: simulation_queue_player_update_apply(element); break;
            case _simulation_queue_element_type_gamestates_delete: simulation_queue_gamestates_delete_apply(element); break;
            case _simulation_queue_element_type_editor_action: simulation_queue_sandbox_event_apply(element); break;
            }
            element = simulation_queue->get_next_element(element);
        }
    }
}


static __forceinline bool simulation_world_distributed_join_initiate(c_simulation_view *view)
{
    s_simulation_distributed_view *distributed_view = view->m_distributed_view;
    distributed_view->entities.replication_start_time = network_time_get();
    distributed_view->entities.transmitting = true;
    return true;
}

static __forceinline void set_joining_view_establishment(
    c_simulation_view *view, e_simulation_view_establishment_mode establishment_mode, long establishment_identifier)
{
    bool valid_transition;
    if (establishment_mode < _simulation_view_establishment_mode_established)
        valid_transition = establishment_identifier == -1;
    else if (establishment_mode == _simulation_view_establishment_mode_established)
        valid_transition = establishment_identifier >= 0;
    else if (view->m_establishment_mode == _simulation_view_establishment_mode_joined
        && establishment_mode == _simulation_view_establishment_mode_joining)
        valid_transition = establishment_identifier >= 0;
    else
        valid_transition = establishment_identifier == view->m_establishment_identifier
            && establishment_mode == view->m_establishment_mode + 1;
    if (!valid_transition)
    {
        view->kill_view(_simulation_view_reason_invalid_establishment);
    }
    else if (view->m_establishment_mode != establishment_mode
        || view->m_establishment_identifier != establishment_identifier)
    {
        view->m_establishment_mode = establishment_mode;
        view->m_establishment_identifier = establishment_identifier;
        simulation_view_send_establishment(view);
        view->update_view_activation_state();
    }
}

// @address 0x3be920 ?update_joining_view@c_simulation_world@@AAA?AW4e_join_progress@1@PAVc_simulation_view@@@Z
c_simulation_world::e_join_progress c_simulation_world::update_joining_view(c_simulation_view *view)
{
    const s_network_configuration *config = &g_network_configuration;
    e_join_progress progress = _join_progress_incomplete;
    if (view->get_view_establishment_mode() < _simulation_view_establishment_mode_ready_to_join)
    {
        bool ready_to_join;
        if (view->m_view_type != 2)
            ready_to_join = view_has_acknowledged_active_players(view);
        else
            ready_to_join = true;
        if (ready_to_join)
            set_joining_view_establishment(view, _simulation_view_establishment_mode_ready_to_join,
                view->get_view_establishment_identifier());
    }
    if (view->get_view_establishment_mode() == _simulation_view_establishment_mode_ready_to_join)
    {
        bool ready_to_advance = false;
        if (view->m_view_type != 2)
        {
            ready_to_advance = simulation_world_distributed_join_initiate(view);
        }
        else
        {
            bool initiate_catchup = false;
            if (m_world_state != _simulation_world_state_joining)
            {
                progress = _join_progress_failed;
            }
            else if (!m_gamestate_loaded_from_saved_film && !m_time_running && game_time_globals->time == 0)
            {
                ready_to_advance = true;
            }
            else
            {
                initiate_catchup = true;
            }
            if (initiate_catchup)
            {
                bool can_initiate = true;
                long failure_time = m_synchronous_catchup_failure_time;
                if (failure_time != -1 && network_time_since(failure_time) < config->simulation_catchup_retry_delay)
                    can_initiate = false;
                if (can_initiate)
                {
                    if (view->synchronous_catchup_initiate())
                    {
                        ready_to_advance = true;
                    }
                    else
                    {
                        long attempts = view->m_synchronous_catchup_attempts;
                        m_synchronous_catchup_failure_time = network_time_get();
                        if (attempts >= config->simulation_maximum_catchup_attempts)
                            progress = _join_progress_failed;
                    }
                }
            }
        }
        if (ready_to_advance)
            set_joining_view_establishment(view, _simulation_view_establishment_mode_joining,
                view->get_view_establishment_identifier());
    }
    if (progress != _join_progress_failed && view->get_view_establishment_mode() == _simulation_view_establishment_mode_joining)
    {
        if (!view->client_join_is_finished())
        {
            if (simulation_world_is_synchronous(this) && view->synchronous_catchup_in_progress())
                simulation_view_send_catchup_state(view);
            progress = _join_progress_incomplete;
        }
        else if (is_distributed() && !view_has_acknowledged_active_players(view))
        {
            progress = _join_progress_incomplete;
        }
        else
        {
            progress = _join_progress_complete;
        }
    }
    if (progress == _join_progress_complete && view->get_view_establishment_mode() == _simulation_view_establishment_mode_joining
        && m_world_state == _simulation_world_state_active)
    {
        if (view->m_view_type != 2)
        {
            simulation_view_is_join_in_progress(view);
        }
        else if (view->synchronous_catchup_in_progress())
        {
            network_time_since(view);
        }
        set_joining_view_establishment(view, _simulation_view_establishment_mode_joined,
            view->get_view_establishment_identifier());
    }
    if (progress == _join_progress_failed)
    {
        simulation_view_handle_join_timeout(view);
    }
    return progress;
}

// @address 0x334a8 k_real_hundredth_storage
const float k_real_hundredth_storage = 0.01f;
