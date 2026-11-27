#ifndef NETWORKING_SIMULATION_SIMULATION_VIEW_TELEMETRY_INTERNALS_H
#define NETWORKING_SIMULATION_SIMULATION_VIEW_TELEMETRY_INTERNALS_H

struct s_simulation_world_actor_stride
{
    char stride_bytes[144];
};

struct s_simulation_world_actor_control_view
{
    char world_header_bytes[0xa94];
    long actor_index;
    long unit_index;
};

#endif // NETWORKING_SIMULATION_SIMULATION_VIEW_TELEMETRY_INTERNALS_H
