#ifndef SIMULATION_SIMULATION_WORLD_TYPES_H
#define SIMULATION_SIMULATION_WORLD_TYPES_H

#include <stddef.h>
#include "simulation/simulation_players_types.h"
#include "simulation/simulation_actors.h"
#include "simulation/simulation_queue.h"
#include "simulation/simulation_view.h"

// March assertions locate the class in simulation\simulation_world.h. This
// sibling holds its definition until users of the declaration header adopt it.
// Playback worlds use types 5 and 6; distributed worlds use 7 and 8.
class c_simulation_watcher;
class c_simulation_distributed_world;
class c_simulation_type_collection;
class c_simulation_view;
class c_simulation_queue;
class c_simulation_entity_database;
class c_simulation_event_handler;
class c_replication_entity_manager;
class c_replication_event_manager;
class c_network_channel;
struct s_machine_identifier;
struct player_action;
struct unit_control_data;
struct simulation_update;
struct s_simulation_update_metadata;
struct s_simulation_queue_element;
struct s_server_update_queue_element;
enum e_simulation_queue_element_type;
enum e_game_simulation_type;
class c_simulation_world;
extern "C" void simulation_world_update_active(c_simulation_world *);
extern "C" bool simulation_world_handle_synchronous_update(c_simulation_world *, const simulation_update *, const s_simulation_update_metadata *);

enum e_simulation_world_type
{
    _simulation_world_type_none,
    _simulation_world_type_local,
    _simulation_world_type_local_playback,
    _simulation_world_type_synchronous_authority,
    _simulation_world_type_synchronous_client,
    _simulation_world_type_synchronous_playback_authority,
    _simulation_world_type_synchronous_playback_client,
    _simulation_world_type_distributed_authority,
    _simulation_world_type_distributed_client
};
enum e_simulation_world_state
{
    _simulation_world_state_none,
    _simulation_world_state_dead,
    _simulation_world_state_disconnected,
    _simulation_world_state_joining,
    _simulation_world_state_active,
    _simulation_world_state_handoff,
    _simulation_world_state_leaving,
    k_simulation_world_state_count
};
struct s_simulation_world_view_iterator
{
    unsigned long type_mask;
    long index;
};

class c_simulation_world
{
public:
    c_simulation_world() { m_world_type = _simulation_world_type_none; }
    ~c_simulation_world() {}
    bool exists() const { return m_world_type != _simulation_world_type_none; }
    bool is_local() const
    {
        bool is_local = m_world_type == 1 || m_world_type == 2;
        return is_local;
    }
    bool is_authority() const { return m_world_type != 4 && m_world_type != 6 && m_world_type != 8; }
    bool is_playback() const { return m_world_type == 2 || m_world_type == 5 || m_world_type == 6; }
    bool is_connected() const
    {
        bool result = false;
        long state = m_world_state;
        if (state >= _simulation_world_state_active && state <= _simulation_world_state_leaving)
            result = true;
        return result;
    }
    bool is_joining() const { return m_world_state == _simulation_world_state_joining; }
    bool is_active() const { return m_world_state == _simulation_world_state_active; }
    bool is_dead() const { return m_world_state == _simulation_world_state_dead; }
    bool runs_simulation() const
    {
        bool result = true;
        long simulation_type = m_world_type;
        if (simulation_type == 2 || (unsigned long)(simulation_type - 4) <= 2)
            result = false;
        return result;
    }
    bool is_out_of_sync() const
    {
        bool result = false;
        if (!is_authority() || is_playback())
            result = m_out_of_sync;
        return result;
    }
    e_simulation_world_type get_world_type() const { return (e_simulation_world_type)m_world_type; }
    bool is_distributed() const { return m_world_type == 7 || m_world_type == 8; }
    long get_next_update_number() const { return next_update_number; }
    // March 824a9a48 calls game_time_get(); its shared inline helper is not available yet.
    long get_time() const;
    e_simulation_world_state get_state() const { return (e_simulation_world_state)m_world_state; }
    bool attached_to_map() const { return m_attached_to_map; }
    // Attaching an already attached world leaves it unchanged.
    void attach_to_map() { if (!m_attached_to_map) m_attached_to_map = true; }
    bool time_running() { return m_time_running; }
    // These accessors need the distributed-world definition owned by another unit.
    c_replication_entity_manager *get_entity_manager();
    c_replication_event_manager *get_event_manager();
    c_simulation_entity_database *get_entity_database();
    c_simulation_event_handler *get_event_handler();

    // March 824ffc50: only the two stores remain after assertions are stripped.
    void iterator_begin(s_simulation_world_view_iterator *iterator, unsigned long mask) const
    {
        iterator->type_mask = mask;
        iterator->index = 0;
    }
    bool iterator_next(s_simulation_world_view_iterator *, c_simulation_view **) const;
    // March 8250a4a8: view, then iterator; assertion-only traversal is stripped.
    c_simulation_view *get_authority_view() const
    {
        c_simulation_view *view = 0;
        s_simulation_world_view_iterator iterator;
        iterator_begin(&iterator, 10);
        iterator_next(&iterator, &view);
        return view;
    }
    static void apply_simulation_queue(const c_simulation_queue *);
    static void distribute_update(const simulation_update *, const s_simulation_update_metadata *);
    static void claim_authority_gameworld();
    // match: 0x3bfe40: callers keep the allocated element in a register across the call only with noalias.
    __declspec(noalias) void simulation_queue_allocate(e_simulation_queue_element_type, long, s_simulation_queue_element **);
    void simulation_queue_free(s_simulation_queue_element *);
    void simulation_queue_enqueue(s_simulation_queue_element *);
    void initialize_world(e_game_simulation_type, long, bool, c_simulation_type_collection *, c_simulation_watcher *, c_simulation_distributed_world *);
    void reset_world();
    void destroy_world();
    void update();
    void process_input(unsigned long, const player_action *, unsigned long);
    void build_player_actions(simulation_update *);
    void build_update(bool, simulation_update *, s_simulation_update_metadata *);
    void change_state_internal(e_simulation_world_state);
    void change_state_disconnected();
    void change_state_joining(unsigned long);
    void change_state_active();
    void change_state_leaving()
    {
        if ((is_connected() || m_world_state == _simulation_world_state_joining) && m_world_state != _simulation_world_state_leaving)
            change_state_internal(_simulation_world_state_leaving);
    }
    void change_state_handoff()
    {
        if (m_world_state == _simulation_world_state_active)
            change_state_internal(_simulation_world_state_handoff);
        else
            change_state_leaving();
    }
    void time_start(long, bool);
    void time_set_immediate_update(bool);
    unsigned long get_player_active_mask() const;
    // March 82507280; defined below where c_simulation_view is complete.
    bool view_has_acknowledged_active_players(const c_simulation_view *) const;
    void delete_all_players();
    void send_player_acknowledgements(bool);
    void drop_simulation_from_active_to_joining();
    long create_actor(long, long);
    void recreate_players(e_game_simulation_type);
    void disconnect();
    bool claim_authority();
    bool relinquish_authority();
    void create_player(long, e_game_simulation_type);
    bool can_generate_updates() const;
    bool synchronous_gamestate_write_in_progress() const;
    c_simulation_view *get_client_view_by_machine_index(long);
    c_simulation_view *get_client_view_by_machine_identifier(const s_machine_identifier *);
    void remove_all_views();
    long update_queue_get_available_updates() const;
    void gamestate_flush();

    bool player_is_active(long index) const
    {
        long player_absolute_index = (unsigned short)index;
        const c_simulation_player *player = &m_players[player_absolute_index];
        bool result = false;
        if (player->exists()) result = player->active();
        return result;
    }
    void get_player_machine(long index, s_machine_identifier *identifier) const { *identifier = m_players[index].m_machine_identifier; }
    void mark_player_pending_deletion(long index)
    {
        c_simulation_player *player = &m_players[index];
        if (player->active()) player->set_active(false);
        player->set_pending_deletion(true);
    }
    void deactivate_all_players()
    {
        for (long player_index = 0; player_index < 16; player_index++)
            if (m_players[player_index].exists() && m_players[player_index].active())
                m_players[player_index].set_active(false);
    }
    void set_machine_identifier(const s_machine_identifier *identifier)
    {
        m_machine_identifier = *identifier;
        m_machine_identifier_valid = true;
    }
    void get_machine_identifier(s_machine_identifier *identifier) const { *identifier = m_machine_identifier; }
    void set_machine_index(long index) { m_local_machine_index = index; }

    c_simulation_watcher *m_watcher;
    c_simulation_distributed_world *m_distributed_world;
    long m_world_type;
    bool m_machine_identifier_valid;
    s_machine_identifier m_machine_identifier;
    long m_local_machine_index;
    long m_world_state;
    unsigned long m_join_start_time;
    unsigned long m_join_flags;
    bool m_time_running;
    bool m_immediate_update;
    long next_update_number;
    long m_last_distributed_update_number;
    bool m_out_of_sync;
    bool m_out_of_sync_determinism_failure;
    char reserved_0032;
    bool m_gamestate_flushed;
    bool m_gamestate_flushed_outside_game_tick;
    // Set when a saved-film game state is loaded; forces synchronous catchup on join.
    unsigned char m_gamestate_loaded_from_saved_film;
    bool m_attached_to_map;
    bool m_gamestate_received;
    long m_join_attempt_count;
    unsigned long m_last_active_time;
    long m_next_establishment_identifier;
    long m_total_join_time;
    long m_maximum_updates;
    long m_handoff_state;
    unsigned long m_handoff_time;
    long view_count;
    c_simulation_view *views[15];
    c_simulation_player m_players[16];
    c_simulation_actor m_actors[16];
    long m_synchronous_catchup_failure_time;
    int update_queue_applied_number;
    int update_queue_reset_value;
    int last_update_number;
    int update_queue_length;
    int update_queue_length_copy;
    s_server_update_queue_element *update_queue_head;
    s_server_update_queue_element *update_queue_tail;
    c_simulation_queue bookkeeping;
    c_simulation_queue simulation;

private:
    friend class c_simulation_view;
    enum e_join_progress
    {
        _join_progress_incomplete,
        _join_progress_complete,
        _join_progress_failed = 3
    };
    e_join_progress update_joining_view(c_simulation_view *);
    void update_client_join_progress();
    void update_authority_join_progress();
    void update_authority_handoff();
    void update_queue_reset();
    void distributed_authority_dispatch_actor_control(unsigned long, const unit_control_data *);
    void distributed_authority_dispatch_player_actions(unsigned long, const player_action *);
    void delete_all_actors();
    bool update_queue_handle_server_update(const simulation_update *, const s_simulation_update_metadata *);
    friend unsigned char simulation_film_read_updates(long, long *);
    friend void simulation_world_update_active(c_simulation_world *);
    friend bool simulation_world_handle_synchronous_update(c_simulation_world *, const simulation_update *, const s_simulation_update_metadata *);
    // match: 0x98160: removing the accepted distribution helper grows its caller by 8B (SW3 checks 2..4).
    friend void simulation_world_distribute_update(c_simulation_world *, const simulation_update *, const s_simulation_update_metadata *);
};

typedef char simulation_world_join_time_offset[(sizeof(void *) != 4 || offsetof(c_simulation_world, m_join_start_time) == 0x1c) ? 1 : -1];
typedef char simulation_world_join_attempts_offset[(sizeof(void *) != 4 || offsetof(c_simulation_world, m_join_attempt_count) == 0x38) ? 1 : -1];
typedef char simulation_world_handoff_time_offset[(sizeof(void *) != 4 || offsetof(c_simulation_world, m_handoff_time) == 0x50) ? 1 : -1];
typedef char simulation_world_type_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, m_world_type) == 8) )) ? 1 : -1];
typedef char simulation_world_state_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, m_world_state) == 0x18) )) ? 1 : -1];
typedef char simulation_world_local_machine_index_offset[(sizeof(void *) != 4 || offsetof(c_simulation_world, m_local_machine_index) == 0x14) ? 1 : -1];
typedef char simulation_world_next_update_offset[(sizeof(void *) != 4 || offsetof(c_simulation_world, next_update_number) == 0x28) ? 1 : -1];
typedef char simulation_world_time_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, m_time_running) == 0x24) )) ? 1 : -1];
typedef char simulation_world_views_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, views) == 0x58) )) ? 1 : -1];
typedef char simulation_world_update_queue_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, update_queue_applied_number) == 0x1398) )) ? 1 : -1];

typedef char simulation_world_size[(sizeof(void *) != 4 || ((sizeof(c_simulation_world) == 0x13ec) )) ? 1 : -1];
typedef char simulation_world_machine_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, m_machine_identifier) == 0x0d) )) ? 1 : -1];
typedef char simulation_world_players_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, m_players) == 0x94) )) ? 1 : -1];
typedef char simulation_world_actors_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, m_actors) == 0xa94) )) ? 1 : -1];
typedef char simulation_world_bookkeeping_queue_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, bookkeeping) == 0x13b4) )) ? 1 : -1];
typedef char simulation_world_simulation_queue_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_world, simulation) == 0x13d0) )) ? 1 : -1];

// March 82507280; defined here where both classes are complete.
inline bool c_simulation_world::view_has_acknowledged_active_players(const c_simulation_view *view) const
{
    unsigned long acknowledged_player_mask = view->m_acknowledged_player_mask;
    unsigned long active_player_mask = get_player_active_mask();
    bool result = (acknowledged_player_mask & active_player_mask) == active_player_mask;
    return result;
}

#endif
