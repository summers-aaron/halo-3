#ifndef EFFECTS_EFFECT_DATUM_H
#define EFFECTS_EFFECT_DATUM_H

#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_index.h"
#include "math/real_math_types.h"
#include "objects/object_types.h"
#include "objects/object_reference_types.h"

enum e_effect_flags
{
    _effect_looping_bit = 0,
    _effect_stopping_bit = 1,
    _effect_detach_bit = 3,
    _effect_culled_bit = 2,
    _effect_marked_for_deletion_bit = 5,
    _effect_first_person_bit = 7,
    _effect_pinged_bit = 11,
    _effect_has_plane_bit = 18,
    _effect_ping_from_impact_bit = 9,
    _effect_event_started_bit = 13,
    _effect_deterministic_bit = 14,
    _effect_attached_to_camera_bit = 15,
    _effect_first_person_only_bit = 16,
    _effect_vector_bit = 21,
    _effect_nonviolent_bit = 4,
    _effect_object_deleted_bit = 6,
    _effect_no_objects_bit = 10,
    _effect_no_decals_bit = 17,
    _effect_no_sound_bit = 19,
    _effect_model_state_changed_bit = 20,
    _effect_cannot_be_restarted_bit = 22,
    _effect_underwater_bit = 23
};
typedef s_cluster_reference s_effect_cluster_reference;


struct effect_datum {
    short identifier;
    union {
        c_flags_no_init<e_effect_flags, unsigned long, 20> flags;
        unsigned short flag_words[2]; // BSP initialization reads the high halfword at +4.
    };
    c_tag_index definition_index;
    long looping_sound_index;
    long input_marker_name;
    unsigned char explicit_marker_index;
    unsigned char part_counter;
    union { long scale_a_function; struct { unsigned char particle_scale_a_state; char state_a_padding[3]; }; };
    union { long scale_b_function; struct { unsigned char particle_scale_b_state; char state_b_padding[3]; }; };
    union { s_location location; struct { s_effect_cluster_reference cluster_reference; short leaf_index; }; };
    union { real_point3d position; real_vector3d velocity; };
    long object_index;
    s_damage_owner damage_owner;
    unsigned char damage_reporting_type;
    char first_person_user_flags;
    union {
        bool first_person;
        signed char first_person_users;
    };
    signed char first_person_user_index;
    long attachment_index;
    int event_index;
    float scale_a;
    float scale_b;
    real_point2d impact_coordinates;
    long location_indices[8];
    long lightprobe_slot_index;
    float death_delay;
    unsigned long random_seed;
    long owner_type;
    real_plane3d plane;

    __forceinline bool is_looping() const
    {
        return flags.test(_effect_looping_bit) || flags.test(_effect_ping_from_impact_bit);
    }
};

typedef char effect_datum_size[(sizeof(effect_datum) == 0x9c) ? 1 : -1];

#endif // EFFECTS_EFFECT_DATUM_H
