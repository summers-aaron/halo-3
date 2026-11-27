#ifndef EFFECTS_PARTICLE_SYSTEM_LIMITS_OWNER_TYPES_H
#define EFFECTS_PARTICLE_SYSTEM_LIMITS_OWNER_TYPES_H

#include "tag_files/tag_types.h"

struct s_particle_system_limits;

struct s_particle_system_limits_owner { long reserved; TAG_POINTER(s_particle_system_limits) table; };

#endif // EFFECTS_PARTICLE_SYSTEM_LIMITS_OWNER_TYPES_H
