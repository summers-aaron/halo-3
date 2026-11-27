#ifndef OBJECTS_DAMAGE_RESPONSE_TYPES_H
#define OBJECTS_DAMAGE_RESPONSE_TYPES_H

#include <stddef.h>

struct s_damage_model_material;
struct s_model_damage_info;
struct s_damage_owner;
struct s_damage_data;
union real_vector3d;

// The 0x3c-byte object_kill response, as accessed by damage.cpp.
struct s_destroy_routine_response
{
    const s_damage_model_material *material;
    const s_model_damage_info *definition;
    short node_index;
    long region_index;
    long effect_index;
    const real_vector3d *direction;
    float maximum_body_damage;
    float body_damage;
    float body_damage_scale;
    float scaled_body_damage;
    float shield_damage_scale;
    unsigned short broken_constraints;
    unsigned short loosened_constraints;
    unsigned long flags;
    long response_metadata;
    bool silent;
};

typedef char destroy_routine_response_size[(sizeof(void *) != 4 || ((sizeof(s_destroy_routine_response) == 0x3c))) ? 1 : -1];
typedef char destroy_routine_response_effect_offset[(sizeof(void *) != 4 || ((offsetof(s_destroy_routine_response, effect_index) == 0x10))) ? 1 : -1];
typedef char destroy_routine_response_flags_offset[(sizeof(void *) != 4 || ((offsetof(s_destroy_routine_response, flags) == 0x30))) ? 1 : -1];
typedef char destroy_routine_response_silent_offset[(sizeof(void *) != 4 || ((offsetof(s_destroy_routine_response, silent) == 0x38))) ? 1 : -1];

extern "C" void object_kill(long object_index, const s_damage_data *damage_data, const s_damage_owner *owner,
    s_destroy_routine_response *response, bool silent, long argument);

#endif // OBJECTS_DAMAGE_RESPONSE_TYPES_H
