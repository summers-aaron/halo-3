#ifndef OBJECTS_DAMAGE_OWNER_H
#define OBJECTS_DAMAGE_OWNER_H

// Functions defined in tu/objectslibxbox/damage_owner.cpp that other units call.

struct s_damage_owner;
extern "C" { extern const s_damage_owner k_damage_owner_unknown; }

void damage_compute_damage_owner_from_object_index(long object_index, s_damage_owner *owner);

// Data defined in tu/objectslibxbox/damage_owner.cpp that other units use.

extern const s_damage_owner *global_damage_owner_unknown;

#endif // OBJECTS_DAMAGE_OWNER_H
