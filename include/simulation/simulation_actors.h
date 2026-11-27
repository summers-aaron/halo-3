#ifndef SIMULATION_SIMULATION_ACTORS_H
#define SIMULATION_SIMULATION_ACTORS_H

#include <stddef.h>
#include "units/unit_control.h"

class c_simulation_world;
struct s_player_motion;

// One of the sixteen unit-driving actors a simulation world tracks.
class c_simulation_actor
{
public:
    c_simulation_actor()
    {
        m_simulation_actor_index = -1;
        m_unit_index = -1;
        m_world = 0;
        m_spare_flag = false;
    }
    ~c_simulation_actor() {}

    void initialize(c_simulation_world *world, long simulation_actor_index, long unit_index)
    {
        m_simulation_actor_index = simulation_actor_index;
        m_unit_index = unit_index;
        m_world = world;
        m_control_time = -1;
        void unit_control_clear(unit_control_data *control);
        unit_control_clear(&m_control);
    }

    void destroy()
    {
        m_simulation_actor_index = -1;
        m_unit_index = -1;
        m_world = 0;
    }

    void handle_authority_control(const unit_control_data *control);
    bool generate_control(long *unit_index, unit_control_data *control);
    bool build_motion_data(const unit_control_data *control, s_player_motion *motion) const;

    bool exists() const { return m_simulation_actor_index != -1; }
    long get_control_index() const { return m_simulation_actor_index + 16; }
    long get_unit_index() const { return m_unit_index; }

    long m_simulation_actor_index;
    long m_unit_index;
    c_simulation_world *m_world;
    bool m_spare_flag; // March constructor initializes this byte to false; no semantic use found.
    long m_control_time;
    unit_control_data m_control;
};

typedef char simulation_actor_size[(sizeof(void *) != 4 || ((sizeof(c_simulation_actor) == 0x90) )) ? 1 : -1];
typedef char simulation_actor_control_time_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_actor, m_control_time) == 0x10) )) ? 1 : -1];
typedef char simulation_actor_control_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_actor, m_control) == 0x14) )) ? 1 : -1];

#endif
