#ifndef PHYSICS_COLLISION_MODEL_DEFINITIONS_H
#define PHYSICS_COLLISION_MODEL_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/collision_model_definitions.cpp that other units call.

void collision_model_fixup(long index);
extern "C" bool collision_model_instance_list_fixup(long tag_index, void *resources);

#ifdef HALO3_64BIT
struct structure_bsp;
void collision_model_unpack_bsp_instances_64(structure_bsp *bsp);
#endif

#endif // PHYSICS_COLLISION_MODEL_DEFINITIONS_H
