#ifndef EFFECTS_EFFECT_MESSAGE_H
#define EFFECTS_EFFECT_MESSAGE_H

#include "objects/object_reference_types.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_index.h"
#include "effects/effect_location_types.h"

struct s_effect_message_system
{
    s_location location;
    long reserved1c;
    float duration;
    float lightprobe_slot_index;
    float scale_a;
    float scale_b;
    long object_index;
    long first_person_flags;
    long part_index;
};
struct s_effect_message_particle
{
    s_location location;
    unsigned long random_seed;
    long reserved20;
    float count;
    float scale;
    float rate;
    long owner_index;
    long owner_type;
    long sub_index;
    long value;
};
struct s_effects_message_location { real_matrix4x3 matrix; real_vector3d velocity; unsigned long random_seed; };
struct s_effects_message_decal {
    real_point3d position;
    real_vector3d velocity;
    long definition_index;
    long object_index;
    unsigned char limit_type;
};
enum e_effect_message_type
{
    _effect_message_create_particle_system = 0,
    _effect_message_add_particle_location = 1,
    _effect_message_finalize_particle_system = 2,
    _effect_message_create_contrail_system = 3,
    _effect_message_add_contrail_location = 4,
    _effect_message_finalize_contrail_system = 5,
    _effect_message_create_contrail_profile = 6,
    _effect_message_add_contrail_profile_location = 7,
    _effect_message_finalize_contrail_profile = 8,
    _effect_message_create_light_volume_system = 9,
    _effect_message_add_light_volume_location = 10,
    _effect_message_finalize_light_volume_system = 11,
    _effect_message_create_decal_system = 12,
    _effect_message_effect_xsynced_bit = 13,
    _effect_message_global_xsynced_bit = 14,
    _effect_message_add_render_location = 15,
    _effect_message_refresh_particles = 16,
    _effect_message_refresh_event_particles = 17,
    _effect_message_sample_lightmap = 18
};
struct s_effect_message {
    long type;
    long effect_index;
    c_tag_index definition_index;
    long event_index;
    unsigned char event_block_index;
    unsigned char limit_type;
    unsigned short event_counter;
    long location_index;
    union {
        struct { long location_index; real_matrix4x3 matrix; } lens_flare;
        s_effects_message_decal decal;
        s_effects_message_location light_volume;
        s_effect_message_system system;
        s_effect_message_particle particle;
        s_effect_render_location_data render_location;
    };
    s_effect_message() {}
    s_effect_message(long effect_index, long event_index);
    s_effect_message(long effect_index);
    s_effect_message(long effect_index, long event_index, unsigned char limit_type);
};
typedef char effect_message_size[(sizeof(s_effect_message) == 92) ? 1 : -1];
#endif // EFFECTS_EFFECT_MESSAGE_H
