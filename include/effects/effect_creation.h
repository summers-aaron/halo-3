#ifndef EFFECTS_EFFECT_CREATION_H
#define EFFECTS_EFFECT_CREATION_H

class c_tag_index;
struct s_damage_owner;
struct s_location;
union real_point3d;
union real_vector3d;
union real_rgb_color;
struct effect_vector_field;

long effect_new_from_object(c_tag_index tag_index, const s_damage_owner *damage_owner, long flags, long object_index, float scale_a,
    float scale_b, const real_rgb_color *color, const effect_vector_field *vector_field, long mode);

extern "C" long effect_new_unattached_from_point(c_tag_index definition_index, const real_point3d *point, const real_vector3d *forward,
    const real_vector3d *up, long match_all_markers, long deterministic, const void *plane, const s_location *hint);

struct s_effect_vector;
struct real_plane3d;
enum e_effect_deterministic;
extern "C" long effect_new_with_damage_owner(c_tag_index definition_index, const s_damage_owner *damage_owner,
    unsigned char damage_reporting_type, long vector_count, const s_effect_vector *vectors, const real_plane3d *plane,
    const s_location *hint, e_effect_deterministic deterministic);

extern "C" void effect_create_damage_effect(c_tag_index definition_index, const s_damage_owner *damage_owner, long parent_effect_index,
    const real_vector3d *velocity, long vector_count, const s_effect_vector *vectors, const real_plane3d *plane, const s_location *hint,
    float scale_a, float scale_b, const real_rgb_color *change_color, const effect_vector_field *vector_field, long flags,
    long deterministic);

extern "C" void effect_create_on_object(c_tag_index definition_index, const s_damage_owner *owner, long object_index, float scale_a, float scale_b,
    const real_rgb_color *color, const effect_vector_field *vector_field, long flags, long marker_index);

#endif // EFFECTS_EFFECT_CREATION_H
