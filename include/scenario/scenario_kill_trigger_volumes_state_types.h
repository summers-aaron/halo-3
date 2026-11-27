#ifndef SCENARIO_KILL_TRIGGER_VOLUMES_STATE_TYPES_H
#define SCENARIO_KILL_TRIGGER_VOLUMES_STATE_TYPES_H

struct s_kill_trigger_volume_runtime_state
{
    unsigned long disabled_volumes[11];
    bool map_initialized;
};

extern "C" {
extern __declspec(thread) s_kill_trigger_volume_runtime_state *g_scenario_kill_trigger_volumes_state;
}

#endif // SCENARIO_KILL_TRIGGER_VOLUMES_STATE_TYPES_H
