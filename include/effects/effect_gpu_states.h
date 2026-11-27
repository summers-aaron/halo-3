#ifndef EFFECTS_EFFECT_GPU_STATES_H
#define EFFECTS_EFFECT_GPU_STATES_H
#include "effects/particle_instance_types.h"
#include "effects/effect_contrail_profile_types.h"
#include "effects/effect_contrail_location_types.h"
// Legacy C entry points use these tags; the definition has the effect-profile layout.
struct s_definition_14 : s_effect_contrail_profile {};
struct s_object_14 {
    s_definition_14 *definition;
    s_effect_contrail_location *location;
    s_particle_instance *source;
    float unused_state0;
    float time;
    float age;
    float unused_state3;
    float first_random_value;
    float second_random_value;
    float references;
    float lifespan;
    float interpolation;
    float pad30;
    unsigned long valid;
    unsigned long dirty;
};
typedef char effect_gpu_states_size[(sizeof(void*) != 4 || sizeof(s_object_14) == 0x3c) ? 1 : -1];
#endif
