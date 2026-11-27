#ifndef GPU_PARTICLE_EFFECT_PARAMETERS_TYPES_H
#define GPU_PARTICLE_EFFECT_PARAMETERS_TYPES_H

struct s_definition_14;
struct s_effect_contrail_location;
struct s_particle_instance;

// Upload prefix of the effect states object: both callers pass s_object_14,
// whose leading definition/location/source pointers sit under values[10].
struct s_effect_parameters {
    s_definition_14 *definition;
    s_effect_contrail_location *location;
    s_particle_instance *source;
    float values[10];
};

#endif // GPU_PARTICLE_EFFECT_PARAMETERS_TYPES_H
