#ifndef EFFECTS_PARTICLE_LOCATION_LIMITS_H
#define EFFECTS_PARTICLE_LOCATION_LIMITS_H

#include "effects/effect_budget_limits.h"

enum e_particle_budget_block
{
    _particle_instance_location_budget = 18,
    _particle_instance_budget = 19,
    _particle_system_budget = 21,
    _particle_location_budget = 22
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char particle_instance_location_budget_offset[
    (_particle_instance_location_budget * sizeof(s_effect_limit_block) + offsetof(s_effect_limit_block, limits) == 0x174) ? 1 : -1];
typedef char particle_instance_budget_offset[
    (_particle_instance_budget * sizeof(s_effect_limit_block) + offsetof(s_effect_limit_block, limits) == 0x188) ? 1 : -1];
typedef char particle_system_budget_offset[
    (_particle_system_budget * sizeof(s_effect_limit_block) + offsetof(s_effect_limit_block, limits) == 0x1b0) ? 1 : -1];
typedef char particle_location_budget_offset[
    (_particle_location_budget * sizeof(s_effect_limit_block) + offsetof(s_effect_limit_block, limits) == 0x1c4) ? 1 : -1];
#endif


#endif
