// networkinglibxbox/simulation_watcher.obj: from the records 0x972a8 0x982a8 0xa4af0 0x3d4a78 0x3d4c08 0x3d5030 0x3d53b0 0x3d5498 0x3d5618 0x3d5778 0x3d5920 0x3d59a8 0x3d5a30 0x3d5aa8 0x3d5ba8 0x3d5ce0 0x3d61a8 0x3d62f0 0x3d63d8 0x3d6548 0x3d6678
// @flags /TP /O2
// @address 0x982a8 ?in_online_networked_session@c_simulation_watcher@@QBA_NXZ
// @address 0x3d4a78 ?can_generate_updates@c_simulation_world@@QBA_NXZ
// @address 0x3d5618 ?need_to_generate_updates@c_simulation_watcher@@QBA_NXZ
// @address 0x3d5920 ?get_machine_index_by_identifier@c_simulation_watcher@@QBAJPBUs_machine_identifier@@@Z
// @address 0x3d5a30 ?get_player_is_in_game@c_simulation_watcher@@QBA_NJPBUs_player_identifier@@@Z
// @address 0x3d5aa8 ?get_machine_connectivity@c_simulation_watcher@@QBA_NPBUs_machine_identifier@@@Z
// @address 0x3d5778 ?get_pending_acknowledgement@c_simulation_watcher@@QAA_N_NPAKPAUs_machine_identifier@@11PAUs_player_identifier@@@Z
// @address 0x3d5ba8 ?player_update_find_swap_location@c_simulation_watcher@@CAXPBUs_player_collection@@JPBUs_player_identifier@@PAJPA_N@Z
// @address 0x3d6678 ?setup_view@c_simulation_watcher@@AAA_NJPBUs_network_session_peer@@J@Z
// @address 0x980e8 ?iterator_next@c_simulation_world@@QBA_NPAUs_simulation_world_view_iterator@@PAPAVc_simulation_view@@@Z
// @address 0x3bfb50 ?disconnect@c_simulation_world@@QAAXXZ
// @address 0x3d81f0 ?kill_view@c_simulation_view@@AAAXW4e_simulation_view_reason@@@Z
// @address 0x3c9c08 ?get_peer_from_observer_channel@c_network_session_membership@@QBAJJ@Z
// @address 0x3d9cd0 ?observer_channel_backlogged@c_network_observer@@QBA_NW4e_network_observer_owner@@JW4e_network_message_type@@@Z
// @address 0x94818 ?network_life_cycle_in_group_session@@YA_NPAPAVc_network_session@@@Z
// @address 0x4102e0 ?simulation_player_collection_clear@@YAXPAUs_player_collection@@@Z
// @address 0x410480 ?simulation_player_collection_get_in_game_mask@@YAKPBUs_player_collection@@@Z
// @address 0x3ca610 ?get_player_from_identifier@c_network_session_membership@@QBAJPBUs_player_identifier@@@Z
// @address 0x39ff30 ?simulation_remove_view_from_world@@YAXPAVc_simulation_view@@@Z
// @address 0x3bf538 ?get_client_view_by_machine_identifier@c_simulation_world@@QAAPAVc_simulation_view@@PBUs_machine_identifier@@@Z

class c_simulation_world;
class c_network_observer;
class c_network_session;
// @address 0x410560 ?simulation_player_collection_apply_update@@YAXPAUs_player_collection@@PBUsimulation_player_update@@@Z

                                               // size 0x118

struct s_network_globals;
#include "game/player_types.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "networking/network_observer_owner_types.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include <string.h>
#include "cseries/cseries.h"
#include "game/players.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "simulation/simulation_players.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/transport/transport_security.h"
#include "networking/messages/network_message_type_collection.h"
#include "simulation/watcher_machine_collection_types.h"
#include "simulation/simulation_view_reason_types.h"
#include "networking/network_globals.h"
#include "networking/network_globals_types.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session.h"
#include "game/players_globals.h"
#include "networking/session/network_session_membership_types.h"
#include "networking/logic/network_session_interface_globals.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "simulation/simulation_watcher_types.h"
#include "networking/delivery/network_channel.h"
#include "networking/session/network_session_parameter_observer.h"
#include "simulation/simulation_view.h"
#include "simulation/simulation_world_types.h"
#include "networking/session/network_session_parameters.h"
#include "simulation/simulation_services.h"

enum { _gamestate_reset_machines_bit = 1 };


namespace {

static __forceinline void watcher_transport_secure_address_extract_identifier(const s_transport_secure_address *address,
    s_machine_identifier *identifier)
{
    *identifier = *(const s_machine_identifier *)address->identifier;
}
static __forceinline const game_options *watcher_game_options_get()
{
    return &tls_game_globals()->options;
}
static __forceinline bool watcher_game_is_campaign() { return watcher_game_options_get()->game_mode == 1; }
static __forceinline bool watcher_game_is_multiplayer() { return watcher_game_options_get()->game_mode == 2; }
static __forceinline bool watcher_network_initialized() { return network_globals.initialized; }
static __forceinline bool watcher_get_observer(c_network_observer **observer)
{
    bool result = false;
    const s_life_cycle_globals *globals = network_life_cycle_globals_get();
    if (globals->initialized)
    {
        *observer = globals->manager.m_observer;
        result = true;
    }
    return result;
}
static __forceinline bool watcher_network_life_cycle_game_is_in_progress()
{
    bool result = false;
    const s_life_cycle_globals *globals = network_life_cycle_globals_get();
    if (globals->initialized)
    {
        long state = globals->manager.get_current_state();
        if (state == 3 || state == 15)
            result = true;
    }
    return result;
}
}




    __forceinline long c_network_observer::observer_channel_find_by_machine_identifier(e_network_observer_owner owner,
        const s_transport_unique_identifier *identifier) const
    {
        for (long channel_index = 0; channel_index < 32; channel_index++)
        {
            const s_channel_observer *observer = &m_channel_observers[channel_index];
            if (observer->state != 0 && (observer->owner_flags & (1 << owner)) != 0)
            {
                const s_transport_secure_address *secure_address = &observer->secure_identifier;
                s_transport_unique_identifier remote_identifier = *(const s_transport_unique_identifier *)secure_address->identifier;
                if (memcmp(identifier, &remote_identifier, sizeof(s_transport_unique_identifier)) == 0)
                    return channel_index;
            }
        }
        return NONE;
    }





bool c_simulation_watcher::in_online_networked_session() const
{
  c_network_session *session = m_session;
  bool result = false;
  if (session && session->m_session_class == 2 && session->m_local_state != 0 &&
      session->m_session_membership.m_current_membership.update_number != -1) {
    c_network_session_membership *membership = &session->m_session_membership;
    if (membership && membership->m_current_membership.peer_count > 1)
      result = true;
  }
  return result;
}

// match: 0x3d4a78: the shared get_world_type() form shrinks this by 20B (SW3 check-4.log).
static __forceinline bool simulation_watcher_world_is_synchronous(const c_simulation_world *world)
{
    return world->m_world_type == 3 || world->m_world_type == 4 || world->m_world_type == 5 || world->m_world_type == 6;
}

// 0x3d4a78
bool c_simulation_world::can_generate_updates() const
{
    bool can_generate = true;
    if (simulation_watcher_world_is_synchronous(this) && is_authority())
    {
        if (m_world_state != 4)
            can_generate = false;
        if (can_generate)
        {
            s_simulation_world_view_iterator iterator;
            c_simulation_view *view;
            iterator_begin(&iterator, 4);
            while (iterator_next(&iterator, &view))
            {
                if (view->observer_channel_backlogged(_network_message_type_synchronous_update))
                {
                    view->observer_channel_set_waiting_on_backlog(_network_message_type_synchronous_update);
                    can_generate = false;
                    break;
                }
            }
        }
    }
    return can_generate;
}

// @address 0x3d4c08 ?setup_connection@c_simulation_watcher@@QAAXXZ
void c_simulation_watcher::setup_connection()
{
    if (watcher_game_is_campaign() || watcher_game_is_multiplayer())
    {
        c_network_session *session = 0;
        if (watcher_network_initialized() &&
            (network_life_cycle_in_group_session(&session) || network_life_cycle_try_and_get_squad_session(&session)) &&
            session->get_session_membership_unsafe() != 0)
            m_session = session;
    }
    if (m_session != 0)
    {
        const c_network_session_membership *membership = m_session->get_session_membership_unsafe();
        c_network_observer *observer = 0;
        watcher_get_observer(&observer);
        m_observer = observer;
        m_observer->register_owner(_network_observer_owner_simulation, (c_network_channel_owner *)this);
        m_observer->update_owner(_network_observer_owner_simulation, m_session->managed_session_index());
        unsigned long machine_valid_mask = membership->get_peer_valid_mask();
        s_machine_identifier machine_identifiers[16];
        memset(machine_identifiers, 0, sizeof(machine_identifiers));
        for (long peer_index = 0; peer_index < 16; peer_index++)
        {
            if (membership->is_peer_valid(peer_index))
                watcher_transport_secure_address_extract_identifier(membership->get_peer_address(peer_index),
                    &machine_identifiers[peer_index]);
        }
        m_local_session_membership_update_number = m_session->get_local_session_membership_update_number();
        m_session_membership_update_number = membership->update_number();
        m_machine_valid_mask = machine_valid_mask;
        m_local_machine_index = membership->local_peer_index();
        memcpy(m_machine_identifiers, machine_identifiers, sizeof(m_machine_identifiers));
        m_world->set_machine_identifier(&m_machine_identifiers[m_local_machine_index]);
        m_world->set_machine_index(m_local_machine_index);
        m_machines_from_gamestate = true;
        m_connection_setup = true;
    }
    else
    {
        const game_options *options = watcher_game_options_get();
        m_machine_valid_mask = options->machine_valid_mask;
        memcpy(m_machine_identifiers, options->machines, sizeof(m_machine_identifiers));
        m_machines_from_gamestate = false;
        m_local_machine_index = -1;
        if (options->local_machine_exists)
        {
            for (long machine_index = 0; machine_index < 16; machine_index++)
            {
                if ((m_machine_valid_mask & (1 << machine_index)) &&
                    memcmp(&m_machine_identifiers[machine_index], &options->local_machine, sizeof(s_machine_identifier)) == 0)
                    m_local_machine_index = machine_index;
            }
        }
        if (m_local_machine_index != -1)
        {
            m_world->set_machine_identifier(&m_machine_identifiers[m_local_machine_index]);
            m_world->set_machine_index(m_local_machine_index);
        }
    }
}

namespace {
static __forceinline void watcher_players_get_machines(unsigned long *machine_valid_mask, s_machine_identifier *machine_identifiers)
{
    s_players_globals *globals = tls_players_globals();
    *machine_valid_mask = globals->machine_valid_mask;
    memcpy(machine_identifiers, globals->machines, sizeof(globals->machines));
}
static __forceinline bool watcher_players_get_local_machine(s_machine_identifier *machine_identifier)
{
    s_players_globals *globals = tls_players_globals();
    if (globals->network_valid)
        *machine_identifier = globals->machine;
    return globals->network_valid;
}
}

// @address 0x3d5030 ?handle_new_gamestate@c_simulation_watcher@@QAAXK@Z
void c_simulation_watcher::handle_new_gamestate(unsigned long flags)
{
    long local_machine_index = NONE;
    unsigned long gamestate_machine_valid_mask;
    s_machine_identifier gamestate_machine_identifiers[16];
    s_machine_identifier local_machine_identifier;

    if (m_world->runs_simulation())
    {
        if (!TEST_BIT(flags, _gamestate_reset_machines_bit))
        {
            local_machine_index = NONE;
            watcher_players_get_machines(&gamestate_machine_valid_mask, gamestate_machine_identifiers);
            m_world->get_machine_identifier(&local_machine_identifier);
            for (long machine_index = 0; machine_index < 16; machine_index++)
            {
                if (TEST_BIT(gamestate_machine_valid_mask, machine_index) &&
                    memcmp(&gamestate_machine_identifiers[machine_index], &local_machine_identifier, sizeof(s_machine_identifier)) == 0)
                {
                    local_machine_index = machine_index;
                }
            }

            if (local_machine_index == NONE)
                players_set_local_machine(NULL);
            else
                players_set_local_machine(&gamestate_machine_identifiers[local_machine_index]);
            m_machines_from_gamestate = true;
        }
        else
        {
            s_machine_identifier local_machine_identifier;
            bool local_machine_valid = watcher_players_get_local_machine(&local_machine_identifier);

            m_machine_valid_mask = FLAG(0);
            m_local_machine_index = 0;
            memset(m_machine_identifiers, 0, sizeof(m_machine_identifiers));
            if (local_machine_valid)
                m_machine_identifiers[m_local_machine_index] = local_machine_identifier;

            m_world->set_machine_identifier(&m_machine_identifiers[m_local_machine_index]);
            m_world->set_machine_index(m_local_machine_index);
            players_set_machines(m_machine_valid_mask, m_machine_identifiers);
            players_set_local_machine(&m_machine_identifiers[m_local_machine_index]);
            m_machines_from_gamestate = false;
        }
        handle_player_creation();
    }
    else if (!m_world->is_playback())
    {
        local_machine_index = NONE;
        watcher_players_get_machines(&gamestate_machine_valid_mask, gamestate_machine_identifiers);
        m_world->get_machine_identifier(&local_machine_identifier);
        for (long machine_index = 0; machine_index < 16; machine_index++)
        {
            if (TEST_BIT(gamestate_machine_valid_mask, machine_index) &&
                memcmp(&gamestate_machine_identifiers[machine_index], &local_machine_identifier, sizeof(s_machine_identifier)) == 0)
            {
                local_machine_index = machine_index;
            }
        }

        if (local_machine_index == NONE)
            players_set_local_machine(NULL);
        else
            players_set_local_machine(&gamestate_machine_identifiers[local_machine_index]);
        m_machines_from_gamestate = true;
    }
}

// @address 0x3d53b0 ?handle_player_creation@c_simulation_watcher@@QAAXXZ
void c_simulation_watcher::handle_player_creation()
{
    simulation_player_collection_clear(&m_players);
    simulation_player_collection_build((s_player_collection *)&m_players);
    watcher_players_get_machines(&m_player_machines.valid_mask, m_player_machines.identifiers);
    m_last_player_update = -1;
    m_connection_setup = true;
    if (m_world->is_authority())
        m_world->deactivate_all_players();
}

static __forceinline bool simulation_watcher_session_established(const c_network_session *session)
{
    bool established = false;
    switch (session->m_local_state)
    {
    case _network_session_state_none:
    case _network_session_state_peer_creating:
    case _network_session_state_peer_joining:
    case _network_session_state_peer_join_abort:
    case _network_session_state_election:
        established = false;
        break;
    case _network_session_state_peer_established:
    case _network_session_state_peer_leaving:
    case _network_session_state_host_established:
    case _network_session_state_host_disband:
    case _network_session_state_host_handoff:
    case _network_session_state_host_reestablish:
        established = true;
        break;
    default:
        // match: 0x3d5498: without it the session-state switch becomes a jump table instead of one cmplwi 4/blt range test
        __assume(0);
    }
    return established;
}

static __forceinline bool simulation_watcher_world_player_exists(c_simulation_world *world, long player_index)
{
    long index = (unsigned short)player_index;
    bool result = false;
    if (index >= 0 && index < 16u)
    {
        const c_simulation_player *player = &world->m_players[index];
        if (player->exists())
            result = true;
    }
    return result;
}

static __forceinline void simulation_watcher_world_mark_player_pending_deletion(c_simulation_world *world, long player_index)
{
    c_simulation_player *player = &world->m_players[player_index];
    if (player->active())
        player->set_active(false);
    player->set_pending_deletion(true);
}

// @address 0x3d5498 ?handle_player_leave_game@c_simulation_watcher@@QAAXJ@Z
void c_simulation_watcher::handle_player_leave_game(long player_index)
{
    if (m_world->is_authority() && simulation_watcher_world_player_exists(m_world, player_index))
    {
        if (m_world->get_world_type() == 3 && watcher_game_is_campaign())
        {
            if (m_session && simulation_watcher_session_established(m_session))
            {
                const c_network_session_membership *membership = m_session->get_session_membership();
                long observer_channel_index = -1;
                long session_player_index = membership->get_player_from_identifier(&m_players.players[player_index].identifier);
                if (session_player_index != NONE)
                {
                    long peer_index = membership->m_current_membership.players[session_player_index].peer_index;
                    if (peer_index != NONE)
                        observer_channel_index = membership->get_observer_channel_index(peer_index);
                }
                ::network_squad_session_get_matchmaking_tip(3, observer_channel_index);
            }
        }
        simulation_watcher_world_mark_player_pending_deletion(m_world, player_index);
    }
}

static __forceinline bool watcher_update_queue_empty(const c_simulation_queue *queue)
{
    return queue->queued_count() == 0;
}
static __forceinline bool watcher_update_queues_empty(const c_simulation_world *world)
{
    return watcher_update_queue_empty(&world->bookkeeping) && watcher_update_queue_empty(&world->simulation);
}
bool c_simulation_watcher::need_to_generate_updates(void) const
{
    bool need_updates = false;
    if (m_world && !m_world->is_dead() &&
        m_world->runs_simulation() &&
        m_world->can_generate_updates())
    {
        if (m_machines_from_gamestate)
            need_updates = true;
        if (m_session && m_session->established())
        {
            const c_network_session_parameter_session_mode *parameters = &m_session->get_session_parameters()->m_parameters.session_mode;
            if (parameters->get() == 3)
            {
                const c_network_session_membership *membership = m_session->get_session_membership();
                const s_network_session_peer *host = membership->get_host_peer();
                if (m_last_player_update !=
                    m_session->get_local_session_membership_update_number())
                    need_updates = true;
            }
        }
        if (!watcher_update_queues_empty(m_world))
            need_updates = true;
    }
    return need_updates;
}

bool c_simulation_watcher::get_pending_acknowledgement(
    bool flag, unsigned long *type, s_machine_identifier *machine, unsigned long *p4, unsigned long *p5, s_player_identifier *player)
{
    bool result = false;
    if (flag || m_connection_setup)
    {
        *type = m_machine_valid_mask;
        memcpy(machine, m_machine_identifiers, sizeof(m_machine_identifiers));
        *p4 = m_players.valid_mask;
        *p5 = simulation_player_collection_get_in_game_mask(&m_players);
        memset(player, 0, sizeof(*player) * NUMBEROF(m_players.players));
        for (long player_index = 0; player_index < 16; player_index++)
        {
            if (m_players.valid_mask & (1 << player_index))
                player[player_index] = m_players.players[player_index].identifier;
        }
        m_connection_setup = false;
        result = true;
    }
    return result;
}

long c_simulation_watcher::get_machine_index_by_identifier(const s_machine_identifier *identifier) const
{
  long result = -1;
  for (long i = 0; i < 16; i++) {
    if ((1 << i) & m_machine_valid_mask) {
      if (memcmp(m_machine_identifiers[i].data, identifier->data, sizeof(*identifier)) == 0)
        result = i;
    }
  }
  return result;
}

// @address 0x3d59a8 simulation_watcher_handle_machine_boot
extern "C" void simulation_watcher_handle_machine_boot(c_simulation_watcher *watcher, unsigned long mask,
    e_network_session_boot_reason reason)
{
    long machine_index = 0;
    s_machine_identifier *identifier = watcher->m_machine_identifiers;
    do {
        unsigned long bit = 1 << machine_index;
        if ((watcher->m_machine_valid_mask & bit) != 0 && (bit & mask) != 0)
        {
            long peer_index = watcher->m_session->m_session_membership.get_peer_from_unique_identifier(
                (const s_transport_unique_identifier *)identifier);
            if (peer_index != -1)
                watcher->m_session->host_boot_machine(peer_index, reason);
        }
        machine_index++;
        identifier++;
    } while (machine_index < 16);
}

bool c_simulation_watcher::get_player_is_in_game(long index, const s_player_identifier *identifier) const
{
  bool result = false;
  if (m_players.valid_mask & (1 << index)) {
    const s_player_entry *slot = &m_players.players[index];
    if (memcmp(identifier->data, slot->identifier.data, sizeof(*identifier)) == 0 && slot->left_game == 0)
      result = true;
  }
  return result;
}


bool c_simulation_watcher::get_machine_connectivity(struct s_machine_identifier const * machine_identifier) const
{
    bool connected = false;
    if (m_observer && this->get_machine_index_by_identifier(machine_identifier) != NONE)
    {
        long channel_index = m_observer->observer_channel_find_by_machine_identifier(_network_observer_owner_simulation,
            (const s_transport_unique_identifier *)machine_identifier);
        if (channel_index != NONE && m_observer->observer_channel_connected(_network_observer_owner_simulation, channel_index))
            connected = true;
    }
    return connected;
}

// match: 0x3d5ba8: without __restrict the collection flags word at 0(r3) is reloaded for each slot bit test instead of staying in r27
void c_simulation_watcher::player_update_find_swap_location(struct s_player_collection const *__restrict collection, long add_player_index,
    struct s_player_identifier const *add_player_identifier, long *swap_player_index, bool *remove_existing_player)
{
    long matching_player_absolute_index = -1;
    *swap_player_index = -1;
    *remove_existing_player = (collection->valid_mask & (1 << add_player_index)) != 0;
    for (long player_index = 0; player_index < 16; ++player_index) {
        if (collection->valid_mask & (1 << player_index)) {
            const s_player_entry *player = &collection->players[player_index];
            if (player->left_game && memcmp(&player->identifier, add_player_identifier, sizeof(player->identifier)) == 0)
                matching_player_absolute_index = player_index;
        }
    }
    if (matching_player_absolute_index == add_player_index) {
        *remove_existing_player = false;
    } else if (matching_player_absolute_index != -1) {
        *swap_player_index = matching_player_absolute_index;
        *remove_existing_player = false;
    } else if ((collection->valid_mask & (1 << add_player_index)) && collection->players[add_player_index].left_game) {
        long swap_player_left_game_time = 0x7fffffff;
        for (long player_index = 0; player_index < 16; ++player_index) {
            if (!(collection->valid_mask & (1 << player_index))) {
                *swap_player_index = player_index;
                *remove_existing_player = false;
                break;
            } else {
                const s_player_entry *player = &collection->players[player_index];
                if (player->left_game && player->left_game_time < swap_player_left_game_time) {
                    *swap_player_index = player_index;
                    swap_player_left_game_time = player->left_game_time;
                }
            }
        }
    }
}


static __forceinline bool watcher_view_disconnected(const c_simulation_view *view)
{
    return view->m_view_type == 3 || view->m_view_type == 4;
}

static __forceinline c_simulation_view *watcher_find_view_by_observer_channel(c_simulation_world *world, long observer_channel_index)
{
    c_simulation_view *result = 0;
    s_simulation_world_view_iterator iterator;
    world->iterator_begin(&iterator, -1);
    c_simulation_view *view;
    while (world->iterator_next(&iterator, &view))
    {
        if (view->m_observer_channel_index == observer_channel_index)
        {
            result = view;
            break;
        }
    }
    return result;
}

// @address 0x42d380 ?desire_channel_heartbeat@c_simulation_watcher@@UBA_NJ@Z
bool c_simulation_watcher::desire_channel_heartbeat(long observer_channel_index) const
{
    return true;
}

// @address 0x3d62f0 ?channel_is_load_bearing@c_simulation_watcher@@UBAJJ@Z
long c_simulation_watcher::channel_is_load_bearing(long observer_channel_index) const
{
    long result = 1;
    c_simulation_world *world = m_world;
    if (world)
    {
        c_simulation_view *view = watcher_find_view_by_observer_channel(world, observer_channel_index);
        if (view && !watcher_view_disconnected(view))
        {
            result = 2;
            if (view->m_catchup_attempt != NONE)
                result = 3;
        }
    }
    return result;
}

// @address 0x42d298 ?attempt_channel_reconnection@c_simulation_watcher@@UBA_NJ_N@Z
bool c_simulation_watcher::attempt_channel_reconnection(long observer_channel_index, bool far_side_requested) const
{
    return false;
}

// @address 0x5bec98 ?notify_channel_connection@c_simulation_watcher@@UAAXJK_N@Z
void c_simulation_watcher::notify_channel_connection(long observer_channel_index, unsigned long connection_identifier, bool connected)
{
}

// @address 0x5bec98 ?notify_channel_died@c_simulation_watcher@@UAAXJ@Z
void c_simulation_watcher::notify_channel_died(long observer_channel_index)
{
}


// @address 0x3d63d8 ?maintain_simulation_authority@c_simulation_watcher@@AAA_NPAVc_network_session@@JV?$c_static_flags@$0BA@@@PBUs_network_session_peer@@@Z
bool c_simulation_watcher::maintain_simulation_authority(c_network_session *, long local_peer_index, c_static_flags<16>,
    const s_network_session_peer *)
{
    bool result = true;
    unsigned long live_machine_mask = 0;
    s_simulation_world_view_iterator iterator;
    iterator.type_mask = 0xffffffff;
    iterator.index = 0;
    c_simulation_view *view;
    while (m_world->iterator_next(&iterator, &view))
    {
        if (view->is_dead())
            simulation_remove_view_from_world(view);
        else
            live_machine_mask |= 1 << view->get_machine_index();
    }

    if (m_world->get_state() != 1)
    {
        const c_network_session_membership *membership = m_session->get_session_membership();
        local_peer_index = membership->local_peer_index();
        for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
            peer_index = (membership)->get_next_peer(peer_index))
        {
            if (peer_index == local_peer_index)
                continue;
            if (membership->m_current_membership.peers[peer_index].connection_state != 8)
                continue;
            if (((1 << peer_index) & live_machine_mask) != 0)
                continue;
            long observer_channel_index = m_session->get_peer_observer_channel(peer_index);
            setup_view(peer_index, &membership->m_current_membership.peers[peer_index], observer_channel_index);
        }
    }
    else
    {
        result = false;
    }
    return result;
}

// @address 0x3d6548 ?maintain_simulation_client@c_simulation_watcher@@AAA_NJ@Z
bool c_simulation_watcher::maintain_simulation_client(long machine_index)
{
    bool result = true;
    c_simulation_world *world = m_world;
    c_simulation_view *authority = world->get_authority_view();
    if (authority)
    {
        long death_reason;
        if (authority->is_dead(&death_reason))
            world->disconnect();
        else if (authority->get_machine_index() != machine_index)
            authority->kill_view(_simulation_view_reason_no_longer_authority);
    }
    else
    {
        world->disconnect();
    }
    if (m_world->get_state() != 1)
    {
        if (!m_world->get_authority_view())
        {
            const c_network_session_membership *membership = m_session->get_session_membership();
            long observer_channel_index = m_session->get_peer_observer_channel(membership->host_peer_index());
            setup_view(machine_index, membership->get_host_peer(), observer_channel_index);
        }
    }
    else
    {
        result = false;
    }
    return result;
}

// @address 0x3d6678 ?setup_view@c_simulation_watcher@@AAA_NJPBUs_network_session_peer@@J@Z
bool c_simulation_watcher::setup_view(long machine_index, const s_network_session_peer *session_peer, long observer_channel_index)
{
    bool result = false;
    if (m_world->attached_to_map() &&
        observer_channel_index != NONE &&
        (m_session->m_session_membership.get_peer_from_observer_channel(observer_channel_index) != -1) &&
        m_observer->observer_channel_connected(_network_observer_owner_squad_one, observer_channel_index))
    {
        const c_network_channel *channel = m_observer->observer_channel_get_network_channel(_network_observer_owner_squad_one,
            observer_channel_index);
        if (channel->connected())
        {
            long view_type;
            switch (m_world->get_world_type())
            {
            case _network_session_state_peer_join_abort:
            case _network_session_state_peer_leaving:
                view_type = 2;
                break;
            case _network_session_state_host_disband:
                view_type = 4;
                break;
            case _network_session_state_peer_established:
            case _network_session_state_host_established:
                view_type = 1;
                break;
            case _network_session_state_host_handoff:
                view_type = 3;
                break;
            default:
                // match: 0x3d6678: unreachable hint omits dead branch check
                __assume(0);
            }

            s_machine_identifier machine_identifier;
            watcher_transport_secure_address_extract_identifier(&session_peer->secure_address, &machine_identifier);
            simulation_add_view_to_world(view_type, reinterpret_cast<const unsigned char *>(&machine_identifier), machine_index, 0);

            c_simulation_view *view;
            if (m_world->is_authority())
                view = m_world->get_client_view_by_machine_identifier(&machine_identifier);
            else
                view = m_world->get_authority_view();

            if (view)
            {
                view->attach_observer_channel(m_observer, observer_channel_index, &session_peer->secure_address);
                result = true;
            }
        }
    }
    return result;
}



// @address 0x3d61a8 ?apply_machines_to_players@c_simulation_watcher@@AAAXXZ
void c_simulation_watcher::apply_machines_to_players()
{
    for (long player_index = 0; player_index < 16; player_index++)
    {
        s_player_entry *player = &m_players.players[player_index];
        if ((m_players.valid_mask & (1 << player_index)) && !player->left_game)
        {
            bool player_machine_exists = false;
            for (long machine_index = 0; machine_index < 16; machine_index++)
            {
                if ((m_machine_valid_mask & (1 << machine_index)) &&
                    memcmp(&m_machine_identifiers[machine_index], &player->machine_identifier, sizeof(player->machine_identifier)) == 0)
                {
                    player_machine_exists = true;
                    break;
                }
            }
            if (!player_machine_exists)
            {
                simulation_player_update update;
                update.update_type = 0;
                update.player_index = player_index;
                update.player_identifier = player->identifier;
                simulation_player_collection_apply_update(&m_players, &update);
                handle_player_leave_game(player_index);
            }
        }
    }
    m_player_machines.valid_mask = m_machine_valid_mask;
    memcpy(m_player_machines.identifiers, m_machine_identifiers, sizeof(m_machine_identifiers));
    m_last_player_update = -1;
}

static __forceinline bool watcher_update_peer_valid(const c_static_flags<16> &valid, long index)
{
    return (valid.get_bits_direct()[index >> 5] & (1 << (index & 31))) != 0;
}
static __forceinline void watcher_update_extract_identifier(const s_transport_secure_address *address,s_machine_identifier *identifier) {
    *identifier = *(const s_machine_identifier *)address->identifier;
}
static __forceinline long watcher_find_machine(const s_machine_identifier *identifiers,
    unsigned long valid, const s_machine_identifier *identifier)
{
    for (long i = 0; i < 16; ++i)
        if ((valid & (1ul << i)) && memcmp(&identifiers[i], identifier, sizeof(*identifier)) == 0)
            return i;
    return -1;
}

// @address 0x3d5ce0 ?update_machines@c_simulation_watcher@@AAA_NPBUs_transport_secure_address@@JJJV?$c_static_flags@$0BA@@@PBUs_network_session_peer@@@Z
bool c_simulation_watcher::update_machines(const s_transport_secure_address *address,
    long local_update, long update, long player_update, c_static_flags<16> valid,
    const s_network_session_peer *peers)
{
    const s_network_session_peer (&peer_array)[16] = *(const s_network_session_peer (*)[16])peers;
    bool success = true;
    if (m_local_session_membership_update_number != local_update) {
        s_machine_identifier identifiers[16];
        memset(identifiers, 0, sizeof(identifiers));
        for (long i = 0; i < 16; ++i)
            if (watcher_update_peer_valid(valid,i)) {
                s_machine_identifier &identifier = identifiers[i];
                const s_transport_secure_address &peer_address = peer_array[i].secure_address;
                watcher_update_extract_identifier(&peer_address, &identifier);
            }
        long current_machine_index = m_world->m_local_machine_index;
        s_machine_identifier local_identifier;
        m_world->get_machine_identifier(&local_identifier);
        long local_index = watcher_find_machine(identifiers, valid.m_storage[0], &local_identifier);
        if (local_index == -1)
            success = false;
        else if (local_index != current_machine_index)
            m_world->set_machine_index(local_index);
        if (success) {
            s_simulation_world_view_iterator iterator;
            m_world->iterator_begin(&iterator, ~0ul);
            c_simulation_view *view;
            while (m_world->iterator_next(&iterator, &view)) {
                s_machine_identifier view_identifier = view->m_machine_identifier;
                long current_view_index = view->m_machine_index;
                long index = watcher_find_machine(identifiers, valid.m_storage[0], &view_identifier);
                if (index == -1)
                    simulation_remove_view_from_world(view);
                else if (index != current_view_index)
                    view->m_machine_index = index;
            }
        }
        if (success && m_world->is_authority()) {
                for (long i = 0; i < 16; ++i) {
                    if (m_world->player_is_active(i)) {
                        s_machine_identifier player_identifier;
                        m_world->get_player_machine(i,&player_identifier);
                        if (watcher_find_machine(identifiers, valid.m_storage[0], &player_identifier) == -1) {
                            m_world->mark_player_pending_deletion(i);
                        }
                    }
                }
        }
        if (success) {
            m_local_session_membership_update_number = local_update;
            m_session_membership_update_number = update;
            m_machine_valid_mask = valid.m_storage[0];
            m_local_machine_index = m_world->m_local_machine_index;
            memcpy(m_machine_identifiers, identifiers, sizeof(identifiers));
            m_machines_from_gamestate = true;
            m_connection_setup = true;
        }
    }
    return success;
}



// @address 0x972a8 ?maintain_connection@c_simulation_watcher@@QAA_NXZ
bool c_simulation_watcher::maintain_connection()
{
    bool success = true;
    if (m_world)
    {
        if (!m_world->is_local())
        {
            if (watcher_network_life_cycle_game_is_in_progress())
            {
                if (m_session && !m_session->disconnected())
                {
                    bool established;
                    if (m_session->is_host())
                        established = m_session->host_established();
                    else
                        established = m_session->established();
                    if (established)
                    {
                        bool is_host = m_session->is_host();
                        if (!is_host && m_world->is_authority())
                            success = m_world->relinquish_authority();
                        else if (is_host && !m_world->is_authority())
                            success = m_world->claim_authority();
                        if (success && m_session->leaving_session())
                        {
                            if (is_host)
                                m_world->change_state_handoff();
                            else
                                m_world->change_state_leaving();
                        }
                        if (success)
                        {
                            const s_network_session_peer *host_peer = NULL;
                            const c_network_session_membership *membership = m_session->get_session_membership();
                            host_peer = membership->get_host_peer();
                            if (update_machines(&host_peer->secure_address, m_session->get_local_session_membership_update_number(),
                                    membership->update_number(), membership->local_peer_index(), membership->get_peer_valid_flags(),
                                    membership->get_peers()))
                            {
                                if (m_world->is_authority())
                                    success = maintain_simulation_authority(
                                        m_session, membership->local_peer_index(), membership->get_peer_valid_flags(), membership->get_peers());
                                else
                                    success = maintain_simulation_client(membership->host_peer_index());
                            }
                            else
                            {
                                success = false;
                            }
                        }
                        else
                        {
                            success = false;
                        }
                    }
                    else
                    {
                        m_world->disconnect();
                    }
                }
                else
                {
                    success = false;
                }
            }
            else
            {
                success = false;
            }
        }
    }
    else
    {
        success = false;
    }
    return success;
}


static __forceinline void simulation_player_update_generate_left_game(
    simulation_player_update *player_update, s_player_collection *players, long player_index, const s_player_identifier *player_identifier)
{
    player_update->update_type = 0;
    player_update->player_index = player_index;
    player_update->player_identifier = *player_identifier;
    simulation_player_collection_apply_update(players, player_update);
}

static __forceinline void simulation_player_update_generate_swap(
    simulation_player_update *player_update, s_player_collection *players, long player_index, long swap_player_index)
{
    player_update->update_type = 1;
    player_update->player_index = player_index;
    if (TEST_BIT(players->valid_mask, player_index))
    {
        const s_player_entry *collection_player = &players->players[player_index];
        player_update->player_identifier = collection_player->identifier;
    }
    else
    {
        memset(&player_update->player_identifier, 0, sizeof(player_update->player_identifier));
    }
    player_update->swap_player_index = swap_player_index;
    if (TEST_BIT(players->valid_mask, swap_player_index))
    {
        const s_player_entry *swap_player = &players->players[swap_player_index];
        player_update->swap_player_identifier = swap_player->identifier;
    }
    else
    {
        memset(&player_update->swap_player_identifier, 0, sizeof(player_update->swap_player_identifier));
    }
    simulation_player_collection_apply_update(players, player_update);
}

static __forceinline void simulation_player_update_generate_remove(simulation_player_update *player_update,
    s_player_collection *players, long player_index)
{
    const s_player_entry *collection_player = &players->players[player_index];
    player_update->update_type = 2;
    player_update->player_index = player_index;
    player_update->player_identifier = collection_player->identifier;
    simulation_player_collection_apply_update(players, player_update);
}

static __forceinline void simulation_player_update_generate_add(simulation_player_update *player_update, s_player_collection *players,
    long player_index, const s_player_identifier *player_identifier, const s_machine_identifier *machine_identifier,
    long machine_user_index, long machine_controller_index, const s_player_configuration *player_data, bool joined_in_progress)
{
    player_update->update_type = 3;
    player_update->player_index = player_index;
    player_update->player_identifier = *player_identifier;
    player_update->player_left_game = false;
    player_update->player_joined_in_progress = joined_in_progress;
    player_update->machine_identifier = *machine_identifier;
    player_update->machine_user_index = machine_user_index;
    player_update->machine_controller_index = machine_controller_index;
    player_update->player_data = *player_data;
    simulation_player_collection_apply_update(players, player_update);
}

static __forceinline void simulation_player_update_generate_configuration(simulation_player_update *player_update,
    s_player_collection *players, long player_index, const s_player_identifier *player_identifier,
    const s_player_configuration *player_data)
{
    const s_player_entry *collection_player = &players->players[player_index];
    player_update->update_type = 4;
    player_update->player_index = player_index;
    player_update->player_identifier = *player_identifier;
    memcpy(&player_update->player_data, player_data, sizeof(player_update->player_data));
    simulation_player_collection_apply_update(players, player_update);
}

__forceinline simulation_player_update *c_simulation_watcher::player_update_allocate(
    long *player_update_count, long maximum_player_update_count, simulation_player_update *player_updates) const
{
    (*player_update_count)++;
    return &player_updates[*player_update_count - 1];
}

__forceinline bool c_simulation_watcher::player_update_must_leave_game(long player_index, const s_player_entry *player,
    c_static_flags<16> peer_valid_flags, const s_machine_identifier *network_machine_identifiers,
    c_static_flags<16> network_player_valid_flags, const s_network_session_player *network_players) const
{
    bool must_leave = true;
    if (network_player_valid_flags.test(player_index))
    {
        const s_network_session_player *network_player = &network_players[player_index];
        if (memcmp(&player->identifier, &network_player->identifier, sizeof(player->identifier)) == 0 &&
            memcmp(&player->machine_identifier, &network_machine_identifiers[network_player->peer_index],
                sizeof(player->machine_identifier)) == 0 &&
            player->machine_index == network_player->peer_user_index &&
            player->machine_user_index == network_player->controller_index)
        {
            must_leave = false;
        }
    }
    return must_leave;
}

// @address 0xa4af0 ?generate_player_updates@c_simulation_watcher@@QAAXPAJJPAUsimulation_player_update@@@Z
void c_simulation_watcher::generate_player_updates(
    // match: 0xa4af0: pointer __restrict qualifier prevents alias reload
    long *__restrict player_update_count, long maximum_player_update_count, simulation_player_update *player_updates)
{
    long previous_update_count = *player_update_count;
    if (m_session && m_session->established() &&
        m_session->get_session_parameters()->m_parameters.session_mode.get() == 3)
    {
        const c_network_session_membership *membership = m_session->get_session_membership();
        if (m_last_player_update != m_session->get_local_session_membership_update_number())
        {
            s_machine_identifier peer_machine_identifiers[16];
            for (long peer_index = 0; peer_index < 16; peer_index++)
            {
                if (membership->is_peer_valid(peer_index))
                    watcher_update_extract_identifier(membership->get_peer_address(peer_index), &peer_machine_identifiers[peer_index]);
            }
            for (long player_index = 0; player_index < 16; player_index++)
            {
                const s_player_entry *collection_player = &m_players.players[player_index];
                if (TEST_BIT(m_players.valid_mask, player_index) && !collection_player->left_game)
                {
                    if (player_update_must_leave_game(player_index, collection_player, membership->get_peer_valid_flags(),
                            peer_machine_identifiers, membership->get_player_valid_flags(), membership->get_players()))
                    {
                        simulation_player_update *update = player_update_allocate(player_update_count, maximum_player_update_count,
                            player_updates);
                        simulation_player_update_generate_left_game(update, &m_players, player_index, &collection_player->identifier);
                        handle_player_leave_game(player_index);
                    }
                }
            }
            for (long player_index = 0; player_index < 16; player_index++)
            {
                if (membership->is_player_valid(player_index))
                {
                    const s_network_session_player *network_player = membership->get_player(player_index);
                    const s_player_entry *collection_player = &m_players.players[player_index];
                    if (!TEST_BIT(m_players.valid_mask, player_index) || collection_player->left_game)
                    {
                        bool add_player = true;
                        const s_machine_identifier *machine_identifier = &peer_machine_identifiers[network_player->peer_index];
                        if (network_player->controller_index == NONE)
                        {
                            add_player = false;
                        }
                        else
                        {
                            for (long test_player_index = 0; test_player_index < 16; test_player_index++)
                            {
                                if (TEST_BIT(m_players.valid_mask, test_player_index))
                                {
                                    const s_player_entry *test_player = &m_players.players[test_player_index];
                                    if (!test_player->left_game &&
                                        memcmp(&test_player->machine_identifier, machine_identifier,
                                            sizeof(test_player->machine_identifier)) == 0 &&
                                        (test_player->machine_index == network_player->peer_user_index ||
                                            test_player->machine_user_index == network_player->controller_index))
                                    {
                                        add_player = false;
                                        break;
                                    }
                                }
                            }
                        }
                        if (add_player)
                        {
                            long swap_player_index;
                            bool remove_existing_player;
                            player_update_find_swap_location(
                                &m_players, player_index, &network_player->identifier, &swap_player_index, &remove_existing_player);
                            if (swap_player_index != NONE && swap_player_index != player_index)
                            {
                                simulation_player_update *update = player_update_allocate(player_update_count,
                                    maximum_player_update_count, player_updates);
                                simulation_player_update_generate_swap(update, &m_players, player_index, swap_player_index);
                            }
                            if (remove_existing_player)
                            {
                                simulation_player_update *update = player_update_allocate(player_update_count,
                                    maximum_player_update_count, player_updates);
                                simulation_player_update_generate_remove(update, &m_players, player_index);
                            }
                            simulation_player_update *update = player_update_allocate(player_update_count, maximum_player_update_count,
                                player_updates);
                            bool joined_in_progress = !m_initial_player_update;
                            simulation_player_update_generate_add(update, &m_players, player_index, &network_player->identifier,
                                machine_identifier, network_player->peer_user_index, network_player->controller_index,
                                &network_player->configuration, joined_in_progress);
                        }
                    }
                    else if (memcmp(&network_player->configuration, &collection_player->configuration,
                        sizeof(network_player->configuration)) != 0)
                    {
                        simulation_player_update *update = player_update_allocate(player_update_count, maximum_player_update_count,
                            player_updates);
                        simulation_player_update_generate_configuration(
                            update, &m_players, player_index, &network_player->identifier, &network_player->configuration);
                    }
                }
            }
            m_last_player_update = m_session->get_local_session_membership_update_number();
            m_connection_setup = *player_update_count > previous_update_count;
            m_initial_player_update = false;
        }
    }
}
