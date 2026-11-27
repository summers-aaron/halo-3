#ifndef EFFECTS_EFFECT_CREATION_DATA_H
#define EFFECTS_EFFECT_CREATION_DATA_H

#include <stddef.h>
#include "effects/effect_creation_flags.h"
#include "effects/effect_datum.h"
#include "effects/effect_vector_types.h"

struct collision_result;
struct s_effect_creation_data {
    union { long flags; c_flags_no_init<e_effect_creation_flags, unsigned long, 5> m_flags; };
    c_tag_index definition_index;
    long structure_bsp_index;
    s_damage_owner damage_owner;
    unsigned char damage_reporting_type;
    long object_index;
    short node_index;
    short marker_index;
    long region_permutation_index;
    union { const real_point3d *position; const s_effect_vector *effect_vectors; };
    long effect_vector_count;
    real_vector3d velocity;
    long parent_effect_index;
    union { long lightprobe_slot_index; struct { unsigned char particle_scale_a_state; char state_a_padding[3]; }; };
    union { long user_mask; struct { unsigned char particle_scale_b_state; char state_b_padding[3]; }; };
    short first_person_user_index;
    float scale_a;
    float scale_b;
    long reserved54;
    long reserved58;
    const real_point2d *impact_coordinates;
    long location_index;
    union { long particle_system_index; s_location location_hint; };
    const collision_result *lightprobe_source;
    const real_plane3d *plane;
};

typedef char effect_creation_data_size[(sizeof(void *) != 4 || sizeof(s_effect_creation_data) == 0x70) ? 1 : -1];
typedef char effect_creation_data_object_offset[(offsetof(s_effect_creation_data, object_index) == 0x1c) ? 1 : -1];

#endif
