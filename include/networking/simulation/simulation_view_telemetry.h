#ifndef NETWORKING_SIMULATION_SIMULATION_VIEW_TELEMETRY_H
#define NETWORKING_SIMULATION_SIMULATION_VIEW_TELEMETRY_H

// Bungie: simulation\simulation_view_telemetry.cpp. c_simulation_view_telemetry_provider::get_telemetry_for_for_view
// fills one of these per view (March 2007 assert: "m_telemetry_data.number_of_players<k_number_of_users").

#include "math/real_math_types.h"

class c_simulation_view_telemetry_provider;

struct s_simulation_view_player_telemetry_data
{
    long player_absolute_index;             // c_simulation_player::get_player_index; compared with an index & 0xffff
    long entity_index;                      // simulation entity of the player's unit
    real_point3d position;
    real_vector3d forward;
    signed char zoom_level;
};

struct s_simulation_view_telemetry_data
{
    c_simulation_view_telemetry_provider *provider;
    bool view_inactive;                                     // client view that is not active yet
    unsigned long acknowledged_player_mask;
    long number_of_players;
    s_simulation_view_player_telemetry_data players[4];     // k_number_of_users
};

typedef char simulation_view_telemetry_size_player[(sizeof(s_simulation_view_player_telemetry_data) == 0x24) ? 1 : -1];
typedef char simulation_view_telemetry_size_data[(sizeof(void *) != 4 || ((sizeof(s_simulation_view_telemetry_data) == 0xa0) )) ? 1 : -1];

class c_simulation_view;

// Per-view telemetry provider. This is a plain class with no vtable;
// it holds the view and the telemetry block filled for that view.
class c_simulation_view_telemetry_provider
{
public:
    const void *get_telemetry_for_for_view();
    void calculate_control_relevance(long control_index, const void *telemetry, float *relevance, long *transmission_period);
    bool entity_is_active(long entity_index) const;

    c_simulation_view *m_view;
    s_simulation_view_telemetry_data m_telemetry_data;
};
typedef char simulation_view_telemetry_size_provider[(sizeof(void *) != 4 || ((sizeof(c_simulation_view_telemetry_provider) == 0xa4) )) ? 1 : -1];

#endif
