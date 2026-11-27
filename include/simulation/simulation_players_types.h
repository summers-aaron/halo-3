#ifndef SIMULATION_SIMULATION_PLAYERS_TYPES_H
#define SIMULATION_SIMULATION_PLAYERS_TYPES_H

#include "game/player_types.h"
#include "units/unit_control.h"
#include <stddef.h>
#include <string.h>

#include "simulation/player_action.h"

struct s_player_motion;
struct s_player_prediction;
class c_simulation_world;

class c_simulation_player
{
public:
    c_simulation_player()
    {
        m_control_index = -1;
        m_player_index = -1;
        m_state = -1;
        memset(&m_identifier, 0, sizeof(m_identifier));
        m_world = 0;
        m_active = false;
        m_pending_deletion = false;
    }
    ~c_simulation_player() {}
    bool generate_action(player_action *, unsigned long *);
    bool generate_prediction(s_player_prediction *prediction);
    bool build_motion_data(const player_action *, bool, s_player_motion *) const;
    void handle_local_input(const player_action *, unsigned long);
    // March 824fd010, 82500bb8 and 82507098, assertions stripped.
    bool exists() const { return m_control_index != -1; }
    // March 824fd060, assertions stripped.
    bool is_predictive() const { return m_state == 4; }
    bool active() const { return m_active; }
    bool pending_deletion() const { return m_pending_deletion; }
    long get_player_index() const { return m_player_index; }
    void get_machine_identifier(s_machine_identifier *identifier) const { *identifier = m_machine_identifier; }
    void set_active(bool value) { m_active = value; }
    void set_pending_deletion(bool value) { m_pending_deletion = value; }

    long m_control_index;
    long m_player_index;
    long m_state;
    s_player_identifier m_identifier;
    s_machine_identifier m_machine_identifier;
    c_simulation_world *m_world;
    bool m_pending_deletion;
    bool m_active;
    long m_last_time;
    player_action m_last_action;
    unsigned long m_last_action_flags;
};

typedef char simulation_player_size[(sizeof(void *) != 4 || ((sizeof(c_simulation_player) == 0xa0) )) ? 1 : -1];
typedef char simulation_player_active_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_player, m_active) == 0x21) )) ? 1 : -1];
typedef char simulation_player_world_offset[(sizeof(void *) != 4 || offsetof(c_simulation_player, m_world) == 0x1c) ? 1 : -1];
typedef char simulation_player_last_time_offset[(sizeof(void *) != 4 || offsetof(c_simulation_player, m_last_time) == 0x24) ? 1 : -1];
typedef char simulation_player_action_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_player, m_last_action) == 0x28) )) ? 1 : -1];

#endif
