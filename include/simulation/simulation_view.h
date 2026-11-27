#ifndef SIMULATION_SIMULATION_VIEW_H
#define SIMULATION_SIMULATION_VIEW_H

#include <stddef.h>
#include "cseries/cseries.h"
#include "memory/data_types.h"
#include "game/player_identifier_types.h"
#include "networking/delivery/network_channel.h"
#include "simulation/simulation_view_establishment_mode_types.h"
#include "simulation/simulation_view_reason_types.h"
#include "simulation/simulation_world.h"

class c_gui_screen_download_widget;
class c_network_observer;
class c_replication_control_view;
class c_simulation_view;
struct s_simulation_distributed_view;
struct s_transport_secure_address;
struct s_player_identifier;
struct player_action;
struct s_network_message_synchronous_gamestate;
enum e_network_message_type;

extern "C" void simulation_view_send_establishment(c_simulation_view *view);
extern "C" void simulation_view_handle_join_timeout(c_simulation_view *view);
extern "C" void simulation_view_send_catchup_state(c_simulation_view *view);
extern "C" unsigned char simulation_view_is_join_in_progress(c_simulation_view *self);
extern "C" void gui_screen_download_widget_update(c_gui_screen_download_widget *screen);
extern "C" bool simulation_view_is_session_valid(c_simulation_view *view, unsigned long identifier, const void *data);
extern "C" void set_joining_view_establishment(c_simulation_view *, e_simulation_view_establishment_mode, long);

class c_simulation_view
{
public:
    s_datum_header m_header;
    long m_view_type;
    long m_view_datum_index;
    s_simulation_distributed_view *m_distributed_view;
    c_simulation_world *m_world;
    long m_view_index;
    s_machine_identifier m_machine_identifier;
    long m_machine_index;
    c_network_observer *m_observer;
    long m_observer_channel_index;
    long m_death_reason;
    e_simulation_view_establishment_mode m_establishment_mode;
    long m_establishment_identifier;
    long m_remote_establishment_mode;
    long m_remote_establishment_identifier;
    c_network_channel *m_channel;
    unsigned long m_channel_connection_identifier;
    c_network_channel_simulation_interface m_simulation_interface;
    bool m_active;
    unsigned long m_acknowledged_player_mask;
    long m_synchronous_action_number;
    long m_synchronous_update_number;
    long m_synchronous_catchup_attempts;
    long m_catchup_start;
    long m_catchup_end;
    long m_catchup_finish_time;
    long m_catchup_attempt;
    long m_catchup_data;
    union {
        long m_catchup_word0;
        struct {
            unsigned char m_catchup_message_type;
        };
    };
    unsigned long m_catchup_data1;
    long m_catchup_data2;
    unsigned long m_catchup_data3;
    long m_action_number;
    void attach_observer_channel(c_network_observer *observer, long observer_channel_index, const s_transport_secure_address *address);
    void detach_observer_channel();
    void send_message(e_network_message_type message_type, long size, const void *message, bool check_backlog) const;
    void update();
    void set_view_establishment(e_simulation_view_establishment_mode mode, long identifier);
    bool handle_remote_establishment(e_simulation_view_establishment_mode mode, long identifier, long validation_identifier, const void *validation_data);
    bool handle_player_acknowledge(unsigned long player_exists_mask, unsigned long player_in_game_mask, const s_player_identifier *identifiers);
    bool handle_synchronous_actions(long action_number, long update_number, unsigned long action_mask, const player_action *actions, unsigned long checksum);
    void dispatch_synchronous_actions(unsigned long action_mask, const player_action *actions, unsigned long checksum);
    bool synchronous_catchup_initiate(void);
    bool handle_synchronous_gamestate(const s_network_message_synchronous_gamestate *message, const void *chunk);

    bool exists() const { return m_view_type != 0; }
    bool active() const { return m_active; }
    bool established() const { bool result = false; if (m_channel) result = m_simulation_interface.established(); return result; }
    bool is_distributed() const { return m_view_type == 3 || m_view_type == 4; }
    bool is_client_view() const { return ((1 << m_view_type) & 20U) != 0; }
    bool is_dead(long *reason = 0) const { if (reason) *reason = m_death_reason; return m_death_reason != 0; }
    c_simulation_world *get_world() const { return m_world; }
    long get_world_view_index() const { return m_view_index; }
    long get_machine_index() const { return m_machine_index; }
    void get_machine_identifier(s_machine_identifier *identifier) const { *identifier = m_machine_identifier; }
    c_network_channel *get_channel() const { return m_channel; }
    long get_observer_channel() const { return m_observer_channel_index; }
    unsigned long get_connection_identifier() const { return m_channel_connection_identifier; }
    e_simulation_view_establishment_mode get_view_establishment_mode() const { return m_establishment_mode; }
    long get_view_establishment_identifier() const { return m_establishment_identifier; }
    bool synchronous_catchup_in_progress() const { return m_catchup_attempt != -1; }
    unsigned long get_acknowledged_player_mask() const { return m_acknowledged_player_mask; }
    void force_unacknowledge_player(long player_index) { m_acknowledged_player_mask &= ~(1 << player_index); }
    bool client_in_game() const
    {
        bool result;
        if (m_view_type == 2)
            result = m_establishment_mode >= 5;
        else
            result = m_establishment_mode >= 3;
        return result;
    }
    void notify_closed() { kill_view(_simulation_view_reason_channel_closed); detach_observer_channel(); }
    c_replication_control_view *get_control_view();
    bool observer_channel_backlogged(e_network_message_type message_type) const;
    void observer_channel_set_waiting_on_backlog(e_network_message_type message_type);
private:
    void kill_view(e_simulation_view_reason simulation_view_reason);
    friend class c_simulation_watcher;
    friend class c_simulation_world;
    friend void set_joining_view_establishment(c_simulation_view *, e_simulation_view_establishment_mode, long);
    friend void simulation_world_update_view_establishment(c_simulation_world *, c_simulation_view *);
    friend void simulation_world_update_all_view_establishments(c_simulation_world *);
    friend void simulation_view_handle_join_timeout(c_simulation_view *view);
    friend void simulation_view_send_catchup_state(c_simulation_view *view);
public:
    bool client_join_is_finished(void) const;
private:
    void update_view_activation_state(void);
};

typedef char simulation_view_size[(sizeof(void *) != 4 || sizeof(c_simulation_view) == 0xb8) ? 1 : -1];
typedef char simulation_view_datum_offset[(sizeof(void *) != 4 || offsetof(c_simulation_view, m_view_datum_index) == 8) ? 1 : -1];
typedef char simulation_view_interface_offset[(sizeof(void *) != 4 || offsetof(c_simulation_view, m_simulation_interface) == 0x48) ? 1 : -1];
typedef char simulation_view_machine_offset[(sizeof(void *) != 4 || offsetof(c_simulation_view, m_machine_identifier) == 0x18) ? 1 : -1];
typedef char simulation_view_active_offset[(sizeof(void *) != 4 || offsetof(c_simulation_view, m_active) == 0x7c) ? 1 : -1];
typedef char simulation_view_catchup_offset[(sizeof(void *) != 4 || offsetof(c_simulation_view, m_catchup_attempt) == 0x9c) ? 1 : -1];

#include "networking/session/network_session_parameter_observer.h"

__forceinline bool c_simulation_view::observer_channel_backlogged(e_network_message_type message_type) const
{
    if (m_observer_channel_index != NONE)
        return m_observer->observer_channel_backlogged(_network_observer_owner_simulation, m_observer_channel_index, message_type);
    return true;
}
__forceinline void c_simulation_view::observer_channel_set_waiting_on_backlog(e_network_message_type message_type)
{
    if (m_observer_channel_index != NONE)
        m_observer->observer_channel_set_waiting_on_backlog(_network_observer_owner_simulation, m_observer_channel_index, message_type);
}

#endif // SIMULATION_SIMULATION_VIEW_H
