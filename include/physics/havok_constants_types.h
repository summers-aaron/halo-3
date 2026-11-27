#ifndef PHYSICS_HAVOK_CONSTANTS_TYPES_H
#define PHYSICS_HAVOK_CONSTANTS_TYPES_H

#include <stddef.h>

struct s_havok_constants
{
    float minimum_radius;
    float linear_cast_early_out_distance;
    long environment_type;
    long debug_mode;
    float collision_tolerance;
    bool disable_deactivation;
    float deactivation_reference_distance;
    bool jumping_beans;
    bool weld_environment;
    bool cache_environment_shapes;
    bool shape_cache_debug;
    bool enable_back_stepping;
    bool enable_sweep_shapes;
    bool display_stats;
    bool initialize_profiling;
    bool render_impacts;
    bool render_proxies;
    bool render_collision_damage;
    bool enable_impact_effects;
    bool play_biped_impact_effects;
    bool render_shape_properties;
    bool cleanup_inactive_agents;
    bool batch_add_entities_disabled;
    float expected_max_linear_velocity;
    float minimum_velocity_threshold; // Minimum allowed penetration depth.
};

typedef char havok_constants_environment_offset[(offsetof(s_havok_constants, environment_type) == 0x08) ? 1 : -1];
typedef char havok_constants_debug_offset[(offsetof(s_havok_constants, debug_mode) == 0x0c) ? 1 : -1];
typedef char havok_constants_deactivation_distance_offset[(offsetof(s_havok_constants, deactivation_reference_distance) == 0x18) ? 1 : -1];
typedef char havok_constants_jumping_beans_offset[(offsetof(s_havok_constants, jumping_beans) == 0x1c) ? 1 : -1];
typedef char havok_constants_weld_environment_offset[(offsetof(s_havok_constants, weld_environment) == 0x1d) ? 1 : -1];
typedef char havok_constants_cache_offset[(offsetof(s_havok_constants, cache_environment_shapes) == 0x1e) ? 1 : -1];
typedef char havok_constants_back_stepping_offset[(offsetof(s_havok_constants, enable_back_stepping) == 0x20) ? 1 : -1];
typedef char havok_constants_impact_effects_offset[(offsetof(s_havok_constants, enable_impact_effects) == 0x27) ? 1 : -1];
typedef char havok_constants_batch_add_offset[(offsetof(s_havok_constants, batch_add_entities_disabled) == 0x2b) ? 1 : -1];
typedef char havok_constants_size[(sizeof(s_havok_constants) == 0x34) ? 1 : -1];

#endif // PHYSICS_HAVOK_CONSTANTS_TYPES_H
