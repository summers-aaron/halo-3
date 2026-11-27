#ifndef PHYSICS_HAVOK_COLLISION_DAMAGE_EXTERNS_H
#define PHYSICS_HAVOK_COLLISION_DAMAGE_EXTERNS_H

class c_havok_component;

extern "C" { unsigned char havok_component_has_active_entity(c_havok_component *component); }

#include "game/game_time_types.h"
#include "multithreading/registered_thread_index.h"
#include "math/real_math_constants.h"
extern "C" {
extern bool havok_collision_damage_disabled;
}

#endif // PHYSICS_HAVOK_COLLISION_DAMAGE_EXTERNS_H
