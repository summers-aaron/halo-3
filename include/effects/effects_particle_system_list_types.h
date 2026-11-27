#ifndef EFFECTS_EFFECTS_PARTICLE_SYSTEM_LIST_TYPES_H
#define EFFECTS_EFFECTS_PARTICLE_SYSTEM_LIST_TYPES_H

#include "effects/effects_particle_system_entry_types.h"

struct s_effects_particle_system_list {
    s_effects_particle_system_entry entries[320];
    long count;
};

#endif // EFFECTS_EFFECTS_PARTICLE_SYSTEM_LIST_TYPES_H
