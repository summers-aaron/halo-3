#ifndef PHYSICS_IMPACT_H
#define PHYSICS_IMPACT_H

#include <stddef.h>
#include "math/matrix_math_types.h"
#include "game/materials.h"
#include "physics/physics_model_constraint_reference.h"

class c_impact {                                 /* 180 bytes */
public:
    enum e_type { type0,type1,type2,type3 };
    struct s_contact_description;
    bool includes_point(const s_contact_description *contact_description, bool test_space);
    unsigned short salt;
    unsigned char flags;
    signed char sorted_index;
    e_type type;
    float score;
    short physics_reference_count;
    // Unreferenced ticks saturate at 126.
    union { bool active; signed char unreferenced_ticks; };
    signed char sweetener_size;
    signed char activation_state;
    union { signed char contact_type; signed char vehicle_friction_point_index; };
    union { unsigned char contact_flags; bool use_marker; bool sound_only; };
    union { bool effect_pending; bool vehicle_impact; };
    long last_reference_time;
    signed char state;
    char queued_flag;
    float reference_collision_score;
    float queued_score;
    unsigned char effect_state_a;
    union { signed char effect_index_a; unsigned char collision_type; };
    unsigned char effect_state_b;
    signed char effect_index_b;
    unsigned char effect_state_c;
    long object_index_a;
    long object_index_b;
    c_global_material_type material_a;
    c_global_material_type material_b;
    long looping_sound_index_a;
    long looping_sound_a;
    long looping_sound_index_b;
    long looping_sound_b;
    long managed_effect_index_a;
    long managed_effect_index_b;
    long managed_effect_index_c;
    float vehicle_sound_volume_a;
    float vehicle_sound_volume_b;
    float volume_a, volume_b;
    real_vector3d velocity_direction;
    real_vector3d object_position;
    real_point3d origin;
    real_point3d space_position_a;
    real_point3d space_position_b;
    float effect_elapsed_time;
    signed char has_sound_a;
    signed char has_sound_b;
    long queued_tick;
    float queued_score_b;
    union { unsigned short rigid_body_index; s_physics_model_constraint_reference constraint_reference; };
    enum e_states { state0, state1, state2, state3 };
    static bool included_in_impact_space(const real_point3d *, const real_vector3d *, const real_point3d *, const real_vector3d *, float *);
    static __forceinline long create(const s_contact_description *description, e_states state);
    bool is_managing_effect() const;
    friend void impact_delete(long);
    long get_object_index_a() const { return object_index_a; }
    long get_object_index_b() const { return object_index_b; }
    __forceinline c_global_material_type get_global_material_index_a() const { return material_a; }
    __forceinline c_global_material_type get_global_material_index_b() const { return material_b; }
    __forceinline const real_point3d *get_world_space_position() const { return &origin; }
    __forceinline bool is_between_pair_of_dynamic_rigid_bodies() const { return get_object_index_b() != -1; }
    void queue_collision(float score);
    void update_state(long impact_index, long rigid_body_a, long rigid_body_b);
    void update_effects(long impact_index, long rigid_body_a, long rigid_body_b);
    void remove_reference(long impact_index, bool deleting);
    void update(long impact_index);
    void update_vehicle_impact_sound_only(long impact_datum_index);
    void update_vehicle_impact(long impact_index, const s_contact_description *contact_description,
        const real_vector3d *effect_direction, float volume, bool vehicle);
    void update_looping_sound(struct s_sound_location *location, float *volume);
    __forceinline e_type get_type() const { return type; }
    __forceinline long get_reference_count() const { return physics_reference_count; }
    __forceinline void disconnect_from_physics()
    {
        if (get_type() != 3 && get_type() != 0)
            physics_reference_count = 0;
    }
    static __forceinline void initialize_contact_description(s_contact_description *description, e_type type, bool use_marker,
        long object_index_a, long node_index_a, c_global_material_type material_a,
        long object_index_b, long node_index_b, c_global_material_type material_b,
        const real_point3d *position, const real_vector3d *normal, long vehicle_friction_point_index,
        const s_physics_model_constraint_reference *constraint_reference);
    static __forceinline float calculate_score(const real_point3d *point, e_states state)
    {
        return get_closest_observer_distance_squared(point, state);
    }
    __forceinline void add_reference(long impact_index)
    {
        physics_reference_count++;
    }
private:
    static inline float get_impact_state_score_scale(e_states state);
    static float get_closest_observer_distance_squared(const real_point3d *point, e_states state);
    void calculate_rigid_body_indexes(long, long *, long *);
    bool currently_playing_looping_sound(long impact_index, long looping_sound_index);
    void halt_managed_effects(bool, bool stop_looping_sounds);
    void update_rigid_body_space_positions(const real_point3d *world_space_position, long node_index_a, long node_index_b);
    void calculate_impact_matrix(long object_index, real_matrix3x3 *matrix);
public:
    static bool passes_the_character_test(long first, long second, const real_point3d *point, const real_vector3d *normal);
    static void calculate_water_surface_vector(real_point3d *point, real_vector3d *vector);
    void calculate_effect_parameters(long, long, long, real_point3d *, real_vector3d *, real_vector3d *, real_vector3d *, float *, float *, bool);
};

typedef char impact_size[(sizeof(c_impact) == 180) ? 1 : -1];
typedef char impact_sweetener_offset[(offsetof(c_impact, sweetener_size) == 0x0f) ? 1 : -1];
typedef char impact_reference_score_offset[(offsetof(c_impact, reference_collision_score) == 0x1c) ? 1 : -1];
typedef char impact_object_a_offset[(offsetof(c_impact, object_index_a) == 0x2c) ? 1 : -1];
typedef char impact_queued_tick_offset[(offsetof(c_impact, queued_tick) == 0xa8) ? 1 : -1];

struct c_impact::s_contact_description
{
    bool use_marker;
    unsigned char type;
    short padding_after_type;
    long object_a;
    long node_index_a;
    c_global_material_type material_a;
    long object_b;
    long node_index_b;
    c_global_material_type material_b;
    real_point3d position;
    real_vector3d normal;
    long vehicle_friction_point_index;
    bool disconnected_from_physics;
    unsigned char padding_after_disconnected_byte;
    short padding_after_disconnected_word;
    union
    {
        struct { short constraint_type, constraint_index; };
        s_physics_model_constraint_reference constraint_reference;
    };
};

typedef char impact_contact_size[(sizeof(c_impact::s_contact_description) == 0x40) ? 1 : -1];
typedef char impact_contact_material_a_offset[(offsetof(c_impact::s_contact_description, material_a) == 0x0c) ? 1 : -1];
typedef char impact_contact_material_b_offset[(offsetof(c_impact::s_contact_description, material_b) == 0x18) ? 1 : -1];
typedef char impact_contact_position_offset[(offsetof(c_impact::s_contact_description, position) == 0x1c) ? 1 : -1];
typedef char impact_contact_normal_offset[(offsetof(c_impact::s_contact_description, normal) == 0x28) ? 1 : -1];
typedef char impact_contact_friction_offset[(offsetof(c_impact::s_contact_description, vehicle_friction_point_index) == 0x34) ? 1 : -1];
typedef char impact_contact_disconnected_offset[(offsetof(c_impact::s_contact_description, disconnected_from_physics) == 0x38) ? 1 : -1];
typedef char impact_contact_constraint_offset[(offsetof(c_impact::s_contact_description, constraint_reference) == 0x3c) ? 1 : -1];

// Clear contact alignment bytes together with the contact fields.
__forceinline void c_impact::initialize_contact_description(s_contact_description *description, e_type type, bool use_marker,
    long object_index_a, long node_index_a, c_global_material_type material_a,
    long object_index_b, long node_index_b, c_global_material_type material_b,
    const real_point3d *position, const real_vector3d *normal, long vehicle_friction_point_index,
    const s_physics_model_constraint_reference *constraint_reference)
{
    description->type = (unsigned char)type;
    description->use_marker = use_marker;
    description->padding_after_type = 0;
    description->object_a = object_index_a;
    description->node_index_a = node_index_a;
    description->material_a = material_a;
    description->object_b = object_index_b;
    description->node_index_b = node_index_b;
    description->material_b = material_b;
    description->position = *position;
    description->normal = *normal;
    description->vehicle_friction_point_index = vehicle_friction_point_index;
    description->disconnected_from_physics = 0;
    description->padding_after_disconnected_byte = 0;
    description->padding_after_disconnected_word = 0;
    if (constraint_reference == 0) {
        description->constraint_reference.type = -1;
        description->constraint_reference.index = -1;
    } else {
        description->constraint_reference = *constraint_reference;
    }
}

#endif
