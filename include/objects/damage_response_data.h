#ifndef OBJECTS_DAMAGE_RESPONSE_DATA_H
#define OBJECTS_DAMAGE_RESPONSE_DATA_H

#include <stddef.h>

struct s_damage_model_material;
struct s_model_damage_info;
union real_vector3d;

struct s_damage_response_data {
    union {
        const s_damage_model_material *section_ref;
        const s_damage_model_material *material;
    };
    const s_model_damage_info *definition;
    short node_index;
    long region_index;
    union { long aftermath_node_raw; long effect_index; };
    const real_vector3d *direction;
    float maximum_body_damage;
    float body_damage;
    union { float body_damage_scale; float shield_response; };
    float scaled_body_damage;
    float shield_damage_scale;
    union { unsigned short first_constraint; unsigned short broken_constraints; };
    union { unsigned short second_constraint; unsigned short loosened_constraints; };
    unsigned long flags;
    long response_metadata;
    bool silent;
};

typedef char damage_response_data_size[(sizeof(void *) != 4 || ((sizeof(s_damage_response_data) == 0x3c))) ? 1 : -1];
typedef char damage_response_data_flags_offset[(sizeof(void *) != 4 || ((offsetof(s_damage_response_data, flags) == 0x30))) ? 1 : -1];
typedef char damage_response_data_silent_offset[(sizeof(void *) != 4 || ((offsetof(s_damage_response_data, silent) == 0x38))) ? 1 : -1];

#endif // OBJECTS_DAMAGE_RESPONSE_DATA_H
