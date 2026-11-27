// networkinglibxbox/simulation_view.obj: from the records 0x3d6c68 0x3d7040 0x3d71e8 0x3d72c0 0x3d7330 0x3d7410 0x3d7510 0x3d75e0 0x3d7868 0x3d7938 0x3d79c8 0x3d7b48 0x3d7d60 0x3d7e70 0x3d7f60 0x3d81f0 0x3d8278 0x3d8488 0x3d86f8 0x3d8820 0x3d88f8 0x3d8aa8 0x4c0540 0x4c08d0
// @flags /TP /O2 /J /Oc
#include "simulation/simulation_watcher_types.h"
#include "game/game_results_storage_types.h"
#include "networking/simulation/simulation_entity_definition_types.h"
#include "simulation/simulation_distributed_world_types.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/replication/replication_entity_view_state_types.h"
#include "networking/network_observer_owner_types.h"
#include "memory/tls_globals.h"
#include "game/game_time_types.h"
#include "interface/gui_widget_types.h"
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include <string.h>
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "game/game.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "game/game_results.h"
#include "networking/network_time.h"
#include "cache/cache_files.h"
#include "saved_games/saved_film.h"
#include "memory/crc.h"
#include "simulation/simulation_view.h"
#include "networking/transport/transport_security.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/messages/network_message_handler_types.h"
#include "simulation/simulation_players_types.h"
#include "simulation/simulation_world.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "simulation/simulation_world_types.h"
#include "saved_games/text_stream_types.h"
#include "networking/network_channel_client_info_types.h"
#include "saved_games/game_state.h"
#include "simulation/simulation_view_reason_types.h"
#include "networking/tools/network_blf.h"
#include "game/game_results_team_data_types.h"
#include "simulation/simulation_view_establishment_mode_types.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/online/online_file_game_api_parser.h"
#include "game/game_results_replicator.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/network_time_types.h"
#include "simulation/simulation_globals.h"
#include "main/main_runtime_globals.h"
#include "simulation/simulation_view_api.h"
#include "memory/game_state_globals.h"
#include "networking/delivery/network_channel.h"
#include "networking/session/network_session_parameter_observer.h"
#include "networking/replication/replication_scheduler.h"
#include "networking/replication/replication_entity_manager_view.h"
#include "networking/replication/replication_entity_manager.h"
#include "networking/replication/replication_event_manager.h"
#include "simulation/simulation_distributed_view.h"
#include "main/main_flag_state.h"
#include "interface/gui_widget.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "networking/tools/blf_header_types.h"
#include "interface/user_interface_error_manager.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/campaign/gui_screen_download_widget.h"
#include "networking/tools/blf_authentication_types.h"
#include "networking/online/online_files_globals.h"
#include "networking/logic/storage/network_http_buffer_url_api.h"
#include "game/game_results_incremental_update.h"
#include "game/game_results_incremental_storage.h"
#include "networking/network_time_since_access.h"
struct s_simulation_view_bitmap_header
{
    s_blf_header header;
    long data_size;
    unsigned char data[0];
};

struct s_simulation_view_game_results_message
{
    long establishment_identifier;
    long update_number;
    s_game_results_incremental_update update;
};

struct s_simulation_view_download_url
{
public:
    s_network_http_buffer_url value;
    __forceinline s_simulation_view_download_url()
    {
        value.name[0] = 0;
        value.service_type = 0;
        value.cache_mode = 0;
        value.lifetime_seconds = 600;
        value.queue_type = 0;
    }
    __forceinline s_simulation_view_download_url(const char *name)
    {
        s_network_http_buffer_url &url = value;
        strncpy(url.name, name, 256);
        url.name[255] = 0;
        url.service_type = 0;
        url.cache_mode = 0;
        url.lifetime_seconds = 600;
        url.queue_type = 0;
    }
    __forceinline void set(const char *name)
    {
        s_simulation_view_download_url url(name);
        memcpy(&value, &url.value, sizeof(value));
    }
};

#pragma pack(push, 1)
struct s_download_compressed_chunk : s_blf_header
{
    unsigned char compression_type;
    unsigned long decoded_size;
};
#pragma pack(pop)

// @address 0x3d86f8 simulation_view_is_join_in_progress
// @address 0x3d72c0 simulation_view_handle_join_timeout
// @address 0x3d8418 ?network_time_since@@YAJPAVc_simulation_view@@@Z
// @address 0x3d81f0 ?kill_view@c_simulation_view@@AAAXW4e_simulation_view_reason@@@Z
// @address 0x3d8820 ?handle_view_establishment@c_game_results_replicator@@QAAX_N@Z
// @address 0x3d7330 ?update@c_simulation_view@@QAAXXZ
// @address 0x3d7410 ?set_view_establishment@c_simulation_view@@QAAXW4e_simulation_view_establishment_mode@@J@Z
// @address 0x3d75e0 ?handle_remote_establishment@c_simulation_view@@QAA_NW4e_simulation_view_establishment_mode@@JJPBX@Z
// @address 0x3d7868 simulation_view_is_session_valid
// @address 0x3d79c8 ?handle_player_acknowledge@c_simulation_view@@QAA_NKKPBUs_player_identifier@@@Z
// @address 0x3d7d60 ?handle_synchronous_actions@c_simulation_view@@QAA_NJJKPBUplayer_action@@K@Z
// @address 0x3d7e70 ?dispatch_synchronous_actions@c_simulation_view@@QAAXKPBUplayer_action@@K@Z
// @address 0x3bd7a8 ?get_player_active_mask@c_simulation_world@@QBAKXZ
// @address 0x3d5a30 ?get_player_is_in_game@c_simulation_watcher@@QBA_NJPBUs_player_identifier@@@Z
// @address 0x3d88f8 ?update@c_game_results_replicator@@QAAXXZ
// @address 0x8c4044 ?g_game_results_globals@@3U0@A
// @address 0x3d7040 ?detach_observer_channel@c_simulation_view@@QAAXXZ
// @address 0x3d71e8 ?send_message@c_simulation_view@@QBAXW4e_network_message_type@@JPBX_N@Z
// @address 0x3d7510 simulation_view_send_establishment
// @address 0x419d68 ?reset@c_replication_event_manager_view@@QAAXXZ
// @address 0x41a690 ?reset@c_replication_entity_manager_view@@QAAXXZ
// @address 0x3d9cd0 ?observer_channel_backlogged@c_network_observer@@QBA_NW4e_network_observer_owner@@JW4e_network_message_type@@@Z
// @address 0x3d9568 ?observer_channel_send_message@c_network_observer@@QAAXW4e_network_observer_owner@@J_NW4e_network_message_type@@JPBX@Z
// @address 0x80608 GetTickCount
// @address 0x39f290 ?is_out_of_sync@c_simulation_world@@QBA_NXZ
// @address 0xa33f80 ?g_current_game_results@@3Vc_game_results@@A
// @address 0x8087a8 ?g_network_configuration@@3Us_network_configuration@@A
// @address 0x3d7f60 ?update_view_activation_state@c_simulation_view@@AAAXXZ
// @address 0x3d8488 simulation_view_send_catchup_state
typedef c_replication_scheduler s_simulation_replication_scheduler;


class c_simulation_entity_database;
struct s_entity_definition;

struct s_view_message
{
    long type;
    long value;
    bool valid;
    long size;
    char data[60];
};


struct s_simulation_view_catchup_message {
    unsigned char type;
    long offset;
    long size;
    long reserved;
    unsigned char data[65528];
};
typedef char simulation_view_catchup_offset[(offsetof(s_simulation_view_catchup_message, offset) == 4) ? 1 : -1];
typedef char simulation_view_catchup_data_offset[(offsetof(s_simulation_view_catchup_message, data) == 16) ? 1 : -1];


class c_simulation_world;




static __forceinline bool simulation_view_entity_has_fatal_error(const c_replication_entity_manager_view *view)
{
    return view->fatal_error;
}
static __forceinline bool simulation_view_event_has_fatal_error(const c_replication_event_manager_view *view)
{
    return view->m_client_disabled;
}
static __forceinline bool simulation_view_results_has_fatal_error(const c_game_results_replicator *replicator)
{
    return replicator->fatal_error;
}
static __forceinline bool simulation_view_active(const c_simulation_view *view)
{
    return view->m_active;
}
static __forceinline long simulation_view_get_next_update_number(const c_simulation_world *world)
{
    return world->next_update_number;
}
static __forceinline long simulation_view_get_establishment_identifier(const c_simulation_view *view)
{
    return view->m_establishment_identifier;
}
static __forceinline bool simulation_view_game_results_recording()
{
    return g_game_results_globals.active;
}
static __forceinline c_replication_event_manager *simulation_view_get_event_manager(c_simulation_world *world)
{
    return &world->m_distributed_world->m_replication_event_manager;
}
static __forceinline c_replication_entity_manager *simulation_view_get_entity_manager(c_simulation_world *world)
{
    return &world->m_distributed_world->m_replication_entity_manager;
}
static __forceinline void simulation_view_deregister_telemetry_provider(s_simulation_replication_scheduler *scheduler)
{
    scheduler->m_telemetry_provider = 0;
}
static __forceinline void simulation_view_destroy_scheduler(s_simulation_replication_scheduler *scheduler)
{
    scheduler->m_initialized = false;
}
static __forceinline bool simulation_view_player_exists(const c_simulation_player *player)
{
    return player->m_control_index != -1;
}
static __forceinline void simulation_view_synchronous_catchup_terminate(c_simulation_view *view)
{
    if (view->m_catchup_attempt != -1)
    {
        view->m_catchup_attempt = -1;
        view->m_catchup_data = 0;
        view->m_catchup_start = -1;
        view->m_catchup_end = -1;
    }
}

static __forceinline bool simulation_view_player_is_in_game(const c_simulation_world *world, long player_index, const s_player_identifier *identifier)
{
    long absolute_index = (unsigned short)player_index;
    bool result = false;
    if (absolute_index >= 0 && (unsigned long)absolute_index < 16)
    {
        const c_simulation_player *player = &world->m_players[absolute_index];
        if (simulation_view_player_exists(player))
        {
            s_player_identifier player_identifier;
            player_identifier = player->m_identifier;
            if (memcmp(identifier, &player_identifier, sizeof(player_identifier)) == 0 &&
                world->m_watcher->get_player_is_in_game(absolute_index, identifier))
                result = true;
        }
    }
    return result;
}

static __forceinline bool simulation_view_game_results_finalized()
{
    return g_current_game_results.initialized && g_current_game_results.finalized;
}
static __forceinline void simulation_view_deregister_client(s_simulation_replication_scheduler *scheduler, long index)
{
    scheduler->m_total_terminator_bits -= scheduler->m_client_terminator_bits[index];
    scheduler->m_clients[index] = 0;
}
static __forceinline void simulation_view_deregister_entity_view(c_replication_entity_manager *manager, long index)
{
    manager->m_views[index]->reset();
    manager->m_views[index] = 0;
    manager->m_view_mask &= ~(1 << index);
}
static __forceinline void simulation_view_deregister_event_view(c_replication_event_manager *manager, long index)
{
    manager->m_views[index]->reset();
    manager->m_view_mask &= ~(1 << index);
    manager->m_views[index] = 0;
}
static __forceinline void simulation_view_destroy_event_view(c_replication_event_manager_view *view)
{
    view->reset();
    view->m_initialized = false;
}
static __forceinline void simulation_view_destroy_entity_view(c_replication_entity_manager_view *view)
{
    view->initialized = false;
}
static __forceinline void simulation_view_observer_channel_release(c_network_observer *observer, long index)
{
    c_network_observer::s_channel_observer *channel_observer = &observer->m_channel_observers[index];
    channel_observer->owner_flags &= ~(1 << 3);
}

void c_simulation_view::detach_observer_channel()
{
    if (m_establishment_mode != 0)
        set_view_establishment(_simulation_view_establishment_none, -1);
    if (!this->is_distributed() && m_catchup_attempt != -1)
        simulation_view_synchronous_catchup_terminate(this);
    m_channel->m_simulation_interface = 0;
    m_simulation_interface.dispose();
    if (this->is_distributed())
    {
        c_replication_event_manager *event_manager = simulation_view_get_event_manager(m_world);
        c_replication_entity_manager *entity_manager = simulation_view_get_entity_manager(m_world);
        simulation_view_deregister_client(&m_distributed_view->scheduler, 1);
        simulation_view_deregister_client(&m_distributed_view->scheduler, 0);
        simulation_view_deregister_telemetry_provider(&m_distributed_view->scheduler);
        simulation_view_deregister_entity_view(entity_manager, m_view_index);
        simulation_view_deregister_event_view(event_manager, m_view_index);
        ((c_replication_control_view *)&m_distributed_view->control)->destroy();
        simulation_view_destroy_event_view(&m_distributed_view->events);
        simulation_view_destroy_entity_view(&m_distributed_view->entities);
        simulation_view_destroy_scheduler(&m_distributed_view->scheduler);
    }
    simulation_view_observer_channel_release(m_observer, m_observer_channel_index);
    m_channel_connection_identifier = -1;
    m_channel = 0;
    m_observer_channel_index = -1;
    m_observer = 0;
}

void c_simulation_view::send_message(e_network_message_type message_type, long size, const void *message, bool check_backlog) const
{
    if (m_world->is_out_of_sync())
        return;
    if (m_observer_channel_index != -1)
    {
        if (check_backlog && m_observer->observer_channel_backlogged(_network_observer_owner_simulation, m_observer_channel_index, message_type))
        {
            c_network_observer::s_channel_observer *channel_observer = &m_observer->m_channel_observers[m_observer_channel_index];
            c_network_channel *channel = &channel_observer->channel;
            if (channel->m_state == 5)
            {
                if (!(channel_observer->message_type_flags & (1i64 << message_type)))
                    channel_observer->message_type_flags |= 1i64 << message_type;
            }
        }
        else
            m_observer->observer_channel_send_message(_network_observer_owner_simulation, m_observer_channel_index, false,
                message_type, size, message);
    }
}

extern "C" void simulation_view_send_establishment(c_simulation_view *view)
{
    s_view_message message;
    memset(&message, 0, sizeof(message));
    long type = view->m_establishment_mode;
    message.type = type;
    message.value = view->m_establishment_identifier;
    if (type == 2)
    {
        long size = 0;
        const void *data = 0;
        if (cache_file_try_and_get_map_digest((unsigned long *)&size, &data))
        {
            message.size = size;
            if (size > 0)
                memcpy(message.data, data, size);
        }
        else
        {
            message.size = 0;
        }
        message.valid = true;
    }
    view->send_message(_network_message_type_view_establishment, sizeof(message), &message, false);
}

extern "C" void simulation_view_handle_join_timeout(c_simulation_view *view)
{
    bool quiet = view->m_view_type == 3 || view->m_view_type == 4;
    if (!quiet && view->m_catchup_attempt != -1)
        network_time_since(view);
    view->kill_view(_simulation_view_reason_join_timeout);
}

void c_simulation_view::update()
{
    if (!is_dead() && this->is_distributed())
    {
        if (simulation_view_entity_has_fatal_error(&m_distributed_view->entities))
            kill_view(_simulation_view_reason_entity_replication_error);
        else if (simulation_view_event_has_fatal_error(&m_distributed_view->events))
            kill_view(_simulation_view_reason_event_replication_error);
        else if (simulation_view_results_has_fatal_error(&m_distributed_view->game_results))
            kill_view(_simulation_view_reason_game_results_replication_error);
    }
    if (!is_dead() && this->is_distributed())
        m_distributed_view->game_results.update();
}

void c_simulation_view::set_view_establishment(e_simulation_view_establishment_mode establishment_mode, long establishment_identifier)
{
    bool valid_transition;
    if (establishment_mode < 2)
        valid_transition = establishment_identifier == -1;
    else if (establishment_mode == 2)
        valid_transition = establishment_identifier >= 0;
    else if (m_establishment_mode == 5 && establishment_mode == 4)
        valid_transition = establishment_identifier >= 0;
    else
        valid_transition = establishment_identifier == m_establishment_identifier && establishment_mode == m_establishment_mode + 1;
    if (!valid_transition)
        kill_view(_simulation_view_reason_invalid_establishment);
    else if (m_establishment_mode != establishment_mode || m_establishment_identifier != establishment_identifier)
    {
        m_establishment_mode = establishment_mode;
        m_establishment_identifier = establishment_identifier;
        simulation_view_send_establishment(this);
        this->update_view_activation_state();
    }
}

bool c_simulation_view::handle_remote_establishment(e_simulation_view_establishment_mode establishment_mode, long establishment_identifier,
    long validation_identifier, const void *validation_data)
{
    bool result = false;
    if (m_world && m_channel)
    {
        bool send_establishment = false;
        long old_mode = m_establishment_mode;
        long old_identifier = m_establishment_identifier;
        long old_remote_mode = m_remote_establishment_mode;
        long old_remote_identifier = m_remote_establishment_identifier;
        m_remote_establishment_mode = establishment_mode;
        m_remote_establishment_identifier = establishment_identifier;
        if (!is_dead())
        {
            if (this->is_client_view())
            {
                if (m_establishment_mode < 2)
                {
                    if (m_remote_establishment_mode >= 2 || m_remote_establishment_identifier != -1)
                        send_establishment = true;
                }
                else if (m_establishment_mode == 2)
                {
                    if (m_remote_establishment_mode != 2 || m_remote_establishment_identifier != m_establishment_identifier)
                    {
                        if (old_remote_mode == 2 && old_remote_identifier == m_establishment_identifier)
                            kill_view(_simulation_view_reason_invalid_establishment);
                        else if (m_remote_establishment_mode < 1)
                        {
                            if (!this->is_distributed() && m_catchup_attempt != -1)
                                simulation_view_synchronous_catchup_terminate(this);
                            m_establishment_mode = _simulation_view_establishment_mode_ready_to_connect;
                            m_establishment_identifier = -1;
                            simulation_view_send_establishment(this);
                            this->update_view_activation_state();
                        }
                        else
                            send_establishment = true;
                    }
                }
                else
                {
                    if (m_remote_establishment_mode == 0)
                        kill_view(_simulation_view_reason_remote_establishment_lost);
                    else if (!(m_remote_establishment_identifier == m_establishment_identifier &&
                        m_remote_establishment_mode == 4 && old_remote_mode == 5) &&
                        !(m_remote_establishment_identifier == m_establishment_identifier && m_remote_establishment_mode > 2 &&
                        m_remote_establishment_mode <= m_establishment_mode && m_remote_establishment_mode == old_remote_mode + 1))
                        kill_view(_simulation_view_reason_invalid_establishment);
                }
            }
            else
            {
                if (establishment_mode == 2 && m_establishment_identifier == -1)
                    set_view_establishment(establishment_mode, establishment_identifier);
                else if (establishment_mode >= 2 && m_establishment_identifier == establishment_identifier)
                    set_view_establishment(establishment_mode, establishment_identifier);
                else if (m_establishment_mode >= 2)
                    kill_view(_simulation_view_reason_remote_establishment_lost);
                else if (old_remote_mode == 0 && establishment_mode > 0)
                    send_establishment = true;
            }
        }
        if (!is_dead())
        {
            if (validation_identifier != -1)
            {
                if (!simulation_view_is_session_valid(this, validation_identifier, validation_data))
                    kill_view(_simulation_view_reason_map_digest_mismatch);
            }
            else if (establishment_mode == 2)
                kill_view(_simulation_view_reason_invalid_establishment);
        }
        if (send_establishment)
            simulation_view_send_establishment(this);
        this->update_view_activation_state();
        result = true;
    }
    return result;
}

extern "C" bool simulation_view_is_session_valid(c_simulation_view *view, unsigned long identifier, const void *data)
{
    unsigned long session_size = 0;
    const void *session_data = 0;
    bool matched = false;
    if (cache_file_try_and_get_map_digest(&session_size, &session_data) != 0)
    {
        game_globals_storage *globals = tls_game_globals();
        unsigned int full;
        if (globals != 0 && (globals->initializing || globals->map_active))
            full = 1;
        else
            full = 0;
        if ((unsigned char)full == 0 || globals->options.game_mode != 1)
            full = 1;
        else
            full = 0;
        int compared;
        if (identifier == 60)
            compared = cache_file_compare_digests(
                (const unsigned char *)data, session_size, (const unsigned char *)session_data, (unsigned char)full);
        else
            compared = 0;
        matched = true;
        if ((compared & 0xffU) == 0)
            matched = false;
    }
    return matched;
}

// @address 0x3d7938 ?client_join_is_finished@c_simulation_view@@QBA_NXZ
bool c_simulation_view::client_join_is_finished(void) const
{
    bool result = false;
    if (this->is_distributed())
    {
        if (!simulation_view_is_join_in_progress(const_cast<c_simulation_view *>(this)))
            result = true;
    }
    else
    {
        if (!this->synchronous_catchup_in_progress())
            result = true;
    }
    return result;
}

bool c_simulation_view::handle_player_acknowledge(unsigned long player_exists_mask, unsigned long player_in_game_mask,
    const s_player_identifier *identifiers)
{
    bool result = false;
    if (this->established())
    {
        unsigned long acknowledged_players = 0;
        for (long player_index = 0; player_index < 16; ++player_index)
        {
            if ((player_exists_mask & (1 << player_index)) && (player_in_game_mask & (1 << player_index)) &&
                simulation_view_player_is_in_game(m_world, player_index, &identifiers[player_index]))
                acknowledged_players |= 1 << player_index;
        }
        if (this->client_in_game())
        {
            if (m_world->get_player_active_mask() & ~acknowledged_players)
                kill_view(_simulation_view_reason_player_acknowledgement_failed);
        }
        m_acknowledged_player_mask = acknowledged_players;
        result = true;
    }
    return result;
}

bool c_simulation_view::handle_synchronous_actions(
    long action_number, long update_number, unsigned long action_mask, const player_action *actions, unsigned long checksum)
{
    bool result = false;
    c_simulation_world *world = m_world;
    long world_type = world->m_world_type;
    bool unusable = world_type == 2 || world_type == 5 || world_type == 6;
    if (!unusable && action_number >= m_synchronous_action_number && update_number >= m_synchronous_update_number &&
        update_number < world->next_update_number)
    {
        if (world->m_world_state == 4)
        {
            m_synchronous_action_number = action_number;
            m_synchronous_update_number = update_number;
            for (long player_index = 0; player_index < 4; ++player_index)
            {
                if (action_mask & (1 << player_index))
                {
                    c_simulation_player *player = simulation_world_find_player(world, &m_machine_identifier, player_index);
                    if (player && player->m_state == 3)
                    {
                        player->m_last_action = actions[player_index];
                        player->m_last_action_flags = checksum;
                        player->m_last_time = tls_game_time_globals()->time;
                    }
                }
            }
        }
        result = true;
    }
    return result;
}

void c_simulation_view::dispatch_synchronous_actions(unsigned long action_mask, const player_action *actions, unsigned long checksum)
{
    if (simulation_view_active(this))
    {
        s_network_message_synchronous_actions message;
        memset(&message, 0, sizeof(message));
        message.action_number = m_action_number;
        m_action_number++;
        message.update_number = simulation_view_get_next_update_number(m_world) - 1;
        message.action_mask = action_mask;
        for (long player_index = 0; player_index < 4; ++player_index)
        {
            if (action_mask & (1 << player_index))
                memcpy(&message.actions[player_index], &actions[player_index], sizeof(player_action));
        }
        message.checksum = checksum;
        send_message(_network_message_type_synchronous_actions, sizeof(message), &message, true);
    }
}

void c_simulation_view::update_view_activation_state(void)
{
    bool simulation_established = false;
    bool simulation_active = false;
    if (this->m_world && this->m_channel &&
        this->m_establishment_identifier == this->m_remote_establishment_identifier)
    {
        simulation_established = this->m_establishment_mode >= 2 && this->m_remote_establishment_mode >= 2;
        simulation_active = this->m_establishment_mode >= 5 && this->m_remote_establishment_mode >= 5;
    }
    if (simulation_established != this->established())
    {
        this->m_simulation_interface.set_established(simulation_established);
        if (!simulation_established)
        {
            if (this->is_client_view())
            {
                this->m_acknowledged_player_mask = 0;
                if (this->m_view_type == 2)
                {
                    this->m_synchronous_action_number = -1;
                    this->m_synchronous_update_number = -1;
                    if (this->m_catchup_attempt != -1)
                    {
                        this->m_catchup_attempt = -1;
                        this->m_catchup_data = 0;
                        this->m_catchup_start = -1;
                        this->m_catchup_end = -1;
                    }
                }
                else if (this->m_view_type == 4)
                {
                    if (this->m_distributed_view->entities.transmitting)
                        this->m_distributed_view->entities.transmitting = false;
                }
            }
            else if (this->m_view_type == 1)
            {
                this->m_action_number = 0;
                if (this->m_catchup_attempt != -1)
                {
                    this->m_catchup_attempt = -1;
                    this->m_catchup_data = 0;
                    this->m_catchup_start = -1;
                    this->m_catchup_end = -1;
                }
            }
        }
        if (this->is_distributed())
            this->m_distributed_view->game_results.handle_view_establishment(simulation_established);
        c_simulation_world *world = this->m_world;
        if (simulation_established)
        {
            c_simulation_view *authority = 0;
            s_simulation_world_view_iterator iterator;
            iterator.type_mask = 10;
            iterator.index = 0;
            world->iterator_next(&iterator, &authority);
            if (this == authority)
            {
                simulation_reset_world_for_authority();
                if (!simulation_globals_get()->reset_in_progress)
                    world->send_player_acknowledgements(true);
            }
        }
    }
    if (this->m_active != simulation_active)
    {
        this->m_active = simulation_active;
        c_simulation_world *world = this->m_world;
        if (simulation_active)
        {
            c_simulation_view *authority = 0;
            s_simulation_world_view_iterator iterator;
            iterator.type_mask = 10;
            iterator.index = 0;
            world->iterator_next(&iterator, &authority);
            if (this == authority && world->m_world_state == 3)
                world->update_client_join_progress();
        }
        {
            c_simulation_view *authority = 0;
            s_simulation_world_view_iterator iterator;
            iterator.type_mask = 10;
            iterator.index = 0;
            world->iterator_next(&iterator, &authority);
            if (this == authority && world->m_world_state == 4 && !simulation_active && this->m_establishment_mode == 4)
                world->change_state_joining(0);
        }
    }
}

void c_simulation_view::kill_view(e_simulation_view_reason simulation_view_reason)
{
    if (!is_dead())
    {
        set_view_establishment(_simulation_view_establishment_none, -1);
        m_death_reason = simulation_view_reason;
        if (m_world->m_world_type == 3 && tls_game_globals()->options.game_mode == 1)
        {
            ::network_squad_session_get_matchmaking_tip(3, m_observer_channel_index);
        }
    }
}

static __forceinline unsigned long simulation_view_crc_new()
{
    return 0xFFFFFFFF;
}

static __forceinline bool simulation_view_get_compressed_game_state(c_simulation_view *view)
{
    if (game_state_globals.catchup_state == 3)
        return true;
    if (game_is_cooperative())
        return saved_film_try_and_get_game_state();
    void *buffer = 0;
    unsigned long buffer_size;
    c_simulation_world *world = view->m_world;
    long world_type = world->m_world_type;
    bool is_dedicated = world_type != 4 && world_type != 6 && world_type != 8;
    if (is_dedicated)
        world->m_gamestate_flushed = true;
    return game_state_compress_current_state(&game_state_globals.compressed, 1, 0,
        &buffer, &buffer_size);
}


// @address 0x3d7b48 ?handle_synchronous_gamestate@c_simulation_view@@QAA_NPBUs_network_message_synchronous_gamestate@@PBX@Z
bool c_simulation_view::handle_synchronous_gamestate(const s_network_message_synchronous_gamestate *message, const void *chunk)
{
    bool success = false;
    if (game_state_globals.simulation_started)
    {
        switch (message->chunk_type)
        {
        case 0:
            if (m_world->m_world_state == 3 && m_catchup_attempt == -1)
            {
                m_catchup_attempt = 0;
                m_catchup_end = network_time_get();
                m_catchup_finish_time = -1;
                m_world->time_start(message->chunk_offset, true);
                m_catchup_data = 2;
                c_simulation_world *world = m_world;
                long world_type = world->m_world_type;
                bool immediate = world_type == 2 || world_type == 5 || world_type == 6;
                if (!immediate)
                    world->time_set_immediate_update(true);
                success = true;
            }
            break;
        case 1:
            if (m_catchup_attempt != -1 && m_catchup_attempt == message->chunk_offset)
            {
                long size = message->chunk_size;
                long offset = message->chunk_offset;
                if ((unsigned long)offset <= 0x3fffff)
                {
                    if (offset + size >= 0x400000)
                        size = 0x400000 - offset;
                    memcpy((char *)game_state_globals.compressed_buffer + offset, chunk, size);
                }
                game_state_globals.catchup_state = 1;
                success = true;
                m_catchup_attempt = message->chunk_size + message->chunk_offset;
            }
            break;
        case 2:
            {
                if (m_catchup_attempt != -1)
                {
                    game_state_globals.compressed_size = m_catchup_attempt;
                    if (crc_checksum_buffer(0xFFFFFFFF, game_state_globals.compressed_buffer,
                        game_state_globals.compressed_size) == (unsigned long)message->chunk_offset)
                    {
                        if (!main_globals.gamestate_ready)
                            main_globals.gamestate_ready = true;
                        m_world->m_gamestate_received = true;
                        m_catchup_data = 5;
                        success = true;
                        break;
                    }
                }
                kill_view(_simulation_view_reason_synchronous_catchup_failed);
            }
            break;
        }
    }
    return success;
}

// @address 0x3d8278 ?synchronous_catchup_initiate@c_simulation_view@@QAA_NXZ
bool c_simulation_view::synchronous_catchup_initiate(void)
{
    bool success = true;
    bool result = false;
    m_synchronous_catchup_attempts++;
    if (game_state_globals.simulation_started)
        success = simulation_view_get_compressed_game_state(this);
    if (success) {
        c_simulation_world *world = m_world;
        long time;
        m_catchup_attempt = 0;
        m_catchup_data = 1;
        m_catchup_start = world->next_update_number;
        if (g_network_time_globals.enabled)
            time = g_network_time_globals.time;
        else
            time = GetTickCount();
        m_catchup_end = time;
        m_catchup_finish_time = -1;
        memset(&m_catchup_word0, 0, sizeof(m_catchup_word0) + sizeof(m_catchup_data1) +
            sizeof(m_catchup_data2) + sizeof(m_catchup_data3));
        m_catchup_message_type = 2;
        m_catchup_data1 = crc_checksum_buffer(simulation_view_crc_new(), game_state_globals.compressed_buffer,
            game_state_globals.compressed_size);
        const void *game_state_buffer = game_state_globals.game_state_buffer;
        m_catchup_data3 = simulation_view_crc_new();
        m_catchup_data3 = crc_checksum_buffer(m_catchup_data3, game_state_buffer, 0x7e0000);
        result = true;
    }
    return result;
}


// @address 0x7adae4 g_simulation_catchup_threshold
long g_simulation_catchup_threshold = 2560;
long g_simulation_catchup_bandwidth_limit = g_simulation_catchup_threshold;
static __forceinline s_game_state_compressed_buffer *simulation_view_game_state_get_compressor()
{
    return &game_state_globals.compressed;
}
static __forceinline long simulation_view_channel_get_message_space_available(const c_network_channel *channel)
{
    long available = 0;
    if (channel->m_state == 5)
        available = channel->m_message_queue.get_space_available();
    return available;
}
static __forceinline const char *simulation_view_game_state_get_buffer(
    const s_game_state_compressed_buffer *compressor, long offset, long requested_size, long *size)
{
    const char *buffer = NULL;
    *size = requested_size;
    if (offset >= 0 && offset < compressor->extra) {
        if (offset + requested_size >= compressor->extra)
            *size = compressor->extra - offset;
        buffer = (const char *)compressor->compressed_buffer + offset;
    } else {
        *size = 0;
    }
    return buffer;
}
// @address 0x3d8488 simulation_view_send_catchup_state
extern "C" void simulation_view_send_catchup_state(c_simulation_view *view)
{
    s_game_state_compressed_buffer *compressor = simulation_view_game_state_get_compressor();
    if (view->established()) {
        if (!compressor->active) {
            view->kill_view(_simulation_view_reason_synchronous_catchup_failed);
        } else {
            bool complete = false;
            long minimum_space = g_simulation_catchup_threshold * 3 / 2;
            for (;;) {
                if (simulation_view_channel_get_message_space_available(view->m_channel) < minimum_space || complete) break;
                view->m_catchup_end = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
                switch (view->m_catchup_data) {
                case 1: {
                    struct { unsigned char type; char pad[3]; long index; long reserved[2]; } message;
                    memset(&message, 0, sizeof(message));
                    message.type = 0;
                    message.index = view->m_catchup_start;
                    view->send_message(_network_message_type_synchronous_gamestate, sizeof(message), &message, false);
                    view->m_catchup_data = 3;
                    break;
                }
                case 3: {
                    long *attempt = &view->m_catchup_attempt;
                    long offset = *attempt;
                    long count = g_simulation_catchup_bandwidth_limit;
                    const char *source = simulation_view_game_state_get_buffer(compressor, offset, count, &count);
                    if (count > 0) {
                        s_simulation_view_catchup_message message;
                        memset(&message, 0, offsetof(s_simulation_view_catchup_message, data));
                        message.type = 1;
                        message.offset = offset;
                        message.size = count;
                        memcpy(message.data, source, count);
                        view->send_message(_network_message_type_synchronous_gamestate, message.size + 16, &message, false);
                        view->m_catchup_attempt = count + view->m_catchup_attempt;
                    }
                    if (count != g_simulation_catchup_bandwidth_limit) view->m_catchup_data = 4;
                    break;
                }
                case 4:
                    view->send_message(_network_message_type_synchronous_gamestate, 16, &view->m_catchup_word0, false);
                    complete = true;
                    view->m_catchup_data = 5;
                    break;
                case 5:
                    complete = true;
                    break;
                default:
                    view->kill_view(_simulation_view_reason_synchronous_catchup_failed);
                    break;
                }
            }
        }
    }
}

static __forceinline long simulation_view_next_active_entity(const c_replication_entity_manager_view *view, long previous)
{
    long index;
    if (previous == -1)
        index = 0;
    else
        index = (previous & 0x3ff) + 1;
    long result = -1;
    for (; index < 1024; ++index)
    {
        short state = view->entries[index].state;
        if (state == 1 || state == 2)
        {
            result = view->entries[index].id;
            break;
        }
    }
    return result;
}

extern "C" unsigned char simulation_view_is_join_in_progress(c_simulation_view *view)
{
    bool result = false;
    if (view->m_distributed_view->entities.transmitting)
    {
        long entity_index = -1;
        c_replication_entity_manager *manager = &view->m_world->m_distributed_world->m_replication_entity_manager;
        c_simulation_entity_database *database = &view->m_world->m_distributed_world->m_entity_database;
        while ((entity_index = simulation_view_next_active_entity(&view->m_distributed_view->entities, entity_index)) != -1)
        {
            long absolute_index = entity_index & 0x3ff;
            if (!(manager->m_entity_data[absolute_index].flags & 2))
            {
                s_simulation_entity *entity = &database->channels[absolute_index];
                c_simulation_entity_definition *definition = database->listeners[entity->entity_type + 1];
                if (definition->entity_replication_required_for_view_activation(entity))
                    { result = 1; break; }
            }
        }
    }
    return result;
}

void c_game_results_replicator::handle_view_establishment(bool established)
{
    if (established) {
        if ((1 << context->m_view_type) & 20U) {
            memset(&m_incremental, 0, sizeof(m_incremental));
            m_sending_updates = 1;
            m_last_update_time = -1;
            m_next_update_number = 0;
        } else {
            memset(&m_incremental, 0, sizeof(m_incremental));
            m_next_update_number = 0;
            g_game_results_globals.updating = true;
            m_receiving_updates = 1;
        }
    } else {
        if (m_sending_updates)
            m_sending_updates = 0;
        if (m_receiving_updates) {
            g_game_results_globals.updating = false;
            m_receiving_updates = 0;
        }
    }
}

void c_game_results_replicator::update()
{
    if (context->is_distributed() && context->established() && m_sending_updates && simulation_view_game_results_recording() &&
        (m_last_update_time == -1 || !m_incremental.finalized && simulation_view_game_results_finalized() ||
            network_time_since(m_last_update_time) > g_network_configuration.game_results_update_interval))
    {
        if (!context->observer_channel_backlogged(_network_message_type_distributed_game_results))
            send_game_results_update();
        else
            context->observer_channel_set_waiting_on_backlog(_network_message_type_distributed_game_results);
    }
}

// @address 0x4c08d0 simulation_view_set_download_bitmap
extern "C" unsigned char simulation_view_set_download_bitmap(c_gui_widget *widget, const s_simulation_view_bitmap_header *header, unsigned long size)
{
    unsigned char result = false;
    if (header && size >= 16)
    {
        c_gui_custom_bitmap_widget *bitmap = (c_gui_custom_bitmap_widget *)widget->get_child_widget(_gui_bitmap_widget, 0x1029d);
        const char *data = (const char *)header->data;
        if (bitmap && header->data_size <= (long)((const char *)header - data + size))
            result = gui_custom_bitmap_widget_load_bitmap_from_buffer(bitmap, true, data, header->data_size, 1);
    }
    return result;
}

// @address 0x4c0540 gui_screen_download_widget_update
extern "C" void gui_screen_download_widget_update(c_gui_screen_download_widget *screen)
{
    switch (screen->m_download_state)
    {
    case 2:
    {
        unsigned char success;
        const char *data;
        long size;
        long status;
        {
            const char *result = 0;
            long result_size = 0;
            status = screen->m_content_downloader.get_data(&result, &result_size);
            data = result;
            size = result_size;
        }
        if (status == 2)
        {
            if (data && size > 0)
            {
                success = false;
                unsigned long chunk_size = 0;
                const s_simulation_view_bitmap_header *bitmap = (const s_simulation_view_bitmap_header *)network_blf_find_chunk_in_file(
                    data, size, 'scnd', 1, _blf_file_authentication_none, &chunk_size);
                if (bitmap)
                    success = simulation_view_set_download_bitmap(screen, bitmap, chunk_size);
                else
                {
                    const void *payload = network_blf_find_chunk_in_file(data, size, '_cmp', 1, _blf_file_authentication_none, &chunk_size);
                    if (payload && (long)chunk_size <= 0x4d800)
                    {
                        const s_blf_header *header = static_cast<const s_blf_header *>(payload) - 1;
                        const s_download_compressed_chunk *chunk = static_cast<const s_download_compressed_chunk *>(header);
                        if (!chunk->compression_type && chunk->decoded_size <= 0x4d800)
                        {
                            unsigned long compressed_size = chunk->chunk_size - sizeof(*chunk);
                            char scratch[0x10000];
                            const unsigned long *decoded_size = &chunk->decoded_size;
                            char *destination = screen->m_decompressed;
                            if (((c_game_state_compressor *)decoded_size)
                                    ->game_state_decompress_buffer_and_load(compressed_size, (void *)(destination),
                                        (unsigned long *)(&screen->m_decompressed_size), 0x4d800, sizeof(scratch), (void *)(scratch)))
                                success = simulation_view_set_download_bitmap(screen,
                                    (const s_simulation_view_bitmap_header *)destination, *decoded_size);
                        }
                    }
                }
                screen->m_download_state = 3;
                if (!success)
                    screen->transition_out_with_transition_type(0, _screen_transition_normal);
                return;
            }
        }
        else if (status == 1)
            return;

    
        break;
    
    }
    case 1:
    {
        const char *data = 0;
        long size = 0;
        long status = screen->m_manifest_downloader.get_data(&data, &size);
        if (status == 2)
        {
            if (data && size > 0)
            {
                c_static_string<256> name;
                s_simulation_view_download_url url;
                char value[256];
                c_online_file_game_api_parser parser((char *)data, size);
                if (parser.read_string("InitialUrl: ", (char *)(value), sizeof(value)))
                    name.set(value);
                url.set(name.get_string());
                url.value.queue_type = 1;
                url.value.service_type = 2;
                http_buffer_downloader_set_url(&screen->m_content_downloader, &url.value);
                screen->m_download_state = 2;
                return;
            }
        }
        else if (status == 1)
            return;

    
        break;
    
    }
    case 0:
    {
        c_static_string<256> name;
        s_simulation_view_download_url url;
        name.print("/gameapi/FilesStageForDownload.ashx?userId=%016I64x&shareId=%016I64x&slot=%d&serverId=%s"
            "&startPosition=%d&fromAutoQueue=%d&preview=%d",
            screen->m_owner_xuid, screen->m_target_xuid, screen->m_slot, screen->m_filename, 0, 0, 1);
        url.set(name.get_string());
        url.value.queue_type = 1;
        url.value.service_type = 2;
        http_buffer_downloader_set_url(&screen->m_manifest_downloader, &url.value);
        screen->m_download_state = 1;
    
        return;
    }
    default:
        return;
    }
        e_controller_index controller = screen->get_any_responding_controller();
        g_gui_error_manager.post_error_with_custom_message(0x2003d, 0, 0, controller, false, true);
        screen->m_download_state = 3;
        screen->transition_out_with_transition_type(0, _screen_transition_normal);
}


// @address 0xa33f80 g_game_results_incremental_storage
static __forceinline void simulation_view_game_results_get_incremental(s_game_results_incremental *incremental)
{
    s_game_results_incremental_storage *results = &g_game_results_incremental_storage;
    incremental->summary_valid = results->summary_valid;
    incremental->summary_value = results->end_reason;
    incremental->finish_valid = results->finish_valid;
    incremental->finish_value = results->finish_value;
    incremental->finalized = results->finalized;
    memcpy(&incremental->players, &results->players, sizeof(incremental->players));
    memcpy(&incremental->statistics, &results->statistics, sizeof(incremental->statistics));
    memcpy(&incremental->teams, &results->teams, sizeof(incremental->teams));
    memcpy(&incremental->machines, &results->entries, sizeof(incremental->machines));
}
// @address 0x3d8aa8 ?send_game_results_update@c_game_results_replicator@@AAAXXZ
void c_game_results_replicator::send_game_results_update()
{
    s_simulation_view_game_results_message message;
    memset(&message, 0, sizeof(message));
    if (game_results_calculate_incremental_update(&m_incremental, &message.update))
    {
        if (!m_incremental.finalized)
        {
            message.establishment_identifier = context->m_establishment_identifier;
            message.update_number = m_next_update_number;
            context->send_message(_network_message_type_distributed_game_results, sizeof(message), &message, false);
            simulation_view_game_results_get_incremental(&m_incremental);
            m_next_update_number++;
        }
    }
    m_last_update_time = network_time_get();
}


static __forceinline void simulation_view_initialize_scheduler(c_replication_scheduler *scheduler, long channel_index)
{
    scheduler->m_channel_index = channel_index;
    scheduler->m_total_terminator_bits = 0;
    for (long client_index = 0; client_index < 3; ++client_index)
    {
        scheduler->m_clients[client_index] = 0;
        scheduler->m_client_terminator_bits[client_index] = 0;
    }
    scheduler->m_telemetry_provider = 0;
    scheduler->m_initialized = true;
}
static __forceinline void simulation_view_initialize_telemetry_provider(c_simulation_view_telemetry_provider *provider, c_simulation_view *view)
{
    provider->m_view = view;
}
static __forceinline void simulation_view_register_telemetry_provider(c_replication_scheduler *scheduler,
    c_simulation_view_telemetry_provider *provider)
{
    scheduler->m_telemetry_provider = provider;
}
static __forceinline void simulation_view_initialize_event_view(
    c_replication_event_manager_view *view, long view_index, c_replication_event_manager *event_manager)
{
    view->m_view_index = view_index;
    view->m_manager = event_manager;
    view->m_packet_records = 0;
    view->m_packet_count = 0;
    view->m_initialized = true;
    view->m_client_disabled = false;
    view->m_events_pending = 0;
    view->m_events_delivered = 0;
    view->m_events_in_transit = 0;
}
static __forceinline void simulation_view_register_event_view(c_replication_event_manager *manager, long view_index,
    c_replication_event_manager_view *view)
{
    manager->m_view_mask |= 1 << view_index;
    manager->m_views[view_index] = view;
}
static __forceinline void simulation_view_initialize_entity_view(
    c_replication_entity_manager_view *view, long view_index, c_replication_entity_manager *entity_manager)
{
    view->local_index = view_index;
    view->definitions = entity_manager;
    view->packets = 0;
    view->packet_entity_count = 0;
    view->local_mask = 1 << view_index;
    for (long entity_index = 0; entity_index < 1024; ++entity_index)
    {
        s_replication_entity_view_entry *entry = &view->entries[entity_index];
        entry->id = -1;
        entry->state = _replication_entity_view_state_none;
        entry->update_mask = 0;
        entry->last_update_time = 0;
        entry->flags = c_flags<e_replication_entity_view_data_entity_flags, short, 1>(0);
    }
    memset(view->pending_bits, 0, sizeof(view->pending_bits));
    view->statistics.creations_pending = 0;
    view->statistics.creations_completed = 0;
    view->statistics.deletions_pending = 0;
    view->statistics.deletions_completed = 0;
    view->statistics.updates_pending = 0;
    view->statistics.updates_completed = 0;
    view->transmitting = false;
    view->fatal_error = false;
    view->initialized = true;
}
__forceinline void c_replication_entity_manager::register_view(long view_index, c_replication_entity_manager_view *view)
{
    m_views[view_index] = view;
    m_view_mask |= 1 << view_index;
    for (long absolute_index = 0; absolute_index < 1024; ++absolute_index)
    {
        s_replication_entity_data *entity = &m_entity_data[absolute_index];
        if ((entity->flags & 1) && (entity->flags & 4) && !(entity->flags & 2))
        {
            long entity_index = (entity->seed << 28) | absolute_index;
            m_views[view_index]->create_entity(entity_index);
            unsigned long update_mask = m_client->c_simulation_entity_database::generate_current_entity_update_mask(entity_index);
            if (update_mask)
                m_views[view_index]->set_entity_dirty(entity_index, update_mask);
        }
    }
}
static __forceinline void simulation_view_initialize_results_replicator(c_game_results_replicator *replicator, c_simulation_view *view)
{
    replicator->context = view;
    replicator->m_receiving_updates = 0;
    replicator->m_sending_updates = 0;
    replicator->fatal_error = false;
}
static __forceinline bool simulation_view_world_is_authority(const c_simulation_world *world)
{
    bool result;
    long world_type = world->m_world_type;
    if (world_type == 4 || world_type == 6 || world_type == 8)
        result = false;
    else
        result = true;
    return result;
}

// @address 0x3d6c68 ?attach_observer_channel@c_simulation_view@@QAAXPAVc_network_observer@@JPBUs_transport_secure_address@@@Z
void c_simulation_view::attach_observer_channel(c_network_observer *observer, long observer_channel_index, const s_transport_secure_address *address)
{
    observer->allocate_observer(_network_observer_owner_simulation, address);
    m_observer = observer;
    m_observer_channel_index = observer_channel_index;
    c_network_channel *channel = observer->observer_channel_get_network_channel(_network_observer_owner_simulation, observer_channel_index);
    m_channel = channel;
    unsigned long identifier = channel->m_identifier;
    m_death_reason = 0;
    m_channel_connection_identifier = identifier;
    if (this->is_distributed())
    {
        c_replication_event_manager *event_manager = simulation_view_get_event_manager(m_world);
        c_replication_entity_manager *entity_manager = simulation_view_get_entity_manager(m_world);
        simulation_view_initialize_scheduler(&m_distributed_view->scheduler, m_view_index);
        simulation_view_initialize_telemetry_provider(&m_distributed_view->telemetry_provider, this);
        simulation_view_register_telemetry_provider(&m_distributed_view->scheduler, &m_distributed_view->telemetry_provider);
        simulation_view_initialize_event_view(&m_distributed_view->events, m_view_index, event_manager);
        m_distributed_view->scheduler.register_client(0, &m_distributed_view->events);
        simulation_view_register_event_view(event_manager, m_view_index, &m_distributed_view->events);
        simulation_view_initialize_entity_view(&m_distributed_view->entities, m_view_index, entity_manager);
        m_distributed_view->scheduler.register_client(1, &m_distributed_view->entities);
        entity_manager->register_view(m_view_index, &m_distributed_view->entities);
        ((c_replication_control_view *)&m_distributed_view->control)->initialize(&m_distributed_view->telemetry_provider);
        m_distributed_view->scheduler.register_client(2, (c_replication_scheduler_client *)&m_distributed_view->control);
        simulation_view_initialize_results_replicator(&m_distributed_view->game_results, this);
    }
    long client_count = 0;
    unsigned long client_flags[4];
    c_network_channel_client *clients[4];
    if (this->is_distributed())
    {
        client_flags[client_count] = 26;
        clients[client_count] = &m_distributed_view->scheduler;
        client_count++;
    }
    m_simulation_interface.initialize(simulation_view_world_is_authority(m_world), this,
        simulation_notify_channel_closure, client_count, client_flags, clients);
    channel->m_simulation_interface = &m_simulation_interface;
    m_active = false;
    set_view_establishment(m_establishment_mode, m_establishment_identifier);
}
