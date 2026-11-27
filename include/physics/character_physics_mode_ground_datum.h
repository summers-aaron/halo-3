#ifndef PHYSICS_CHARACTER_PHYSICS_MODE_GROUND_DATUM_H
#define PHYSICS_CHARACTER_PHYSICS_MODE_GROUND_DATUM_H

#include "math/matrix_math_types.h"
#include "game/materials.h"

struct s_character_physics_move_output_datum;
struct s_character_physics_move_input_datum;
struct s_character_physics_update_output_datum;
struct s_character_physics_update_input_datum;
struct s_havok_contact_point_reference;

// Persistent state for the ground character-physics mode: contact, support and landing data.
class c_character_physics_mode_ground_datum {
public:
    // March 82c38590 and 82c38748: initialization precedes the shared reset; disposal is empty.
    __forceinline void initialize()
    {
        // Keep the datum independent of the math helper header's CRT declarations.
        extern const real_vector3d *global_zero_vector3d;
        extern const real_vector3d *global_up3d;
        last_update_time = -1;
        m_localized_origin = *global_zero_vector3d;
        m_update_count = 0;
        m_ground_plane = *global_up3d;
        m_contact_flags = 0;
        initialize_internal(true);
    }
    __forceinline void dispose() {}
    void move(s_character_physics_move_output_datum *, const s_character_physics_move_input_datum *);
    void update(s_character_physics_update_output_datum *, const s_character_physics_update_input_datum *);
    void snap_facing(real_vector3d *, real_vector3d *, const s_character_physics_update_input_datum *);
    __forceinline c_global_material_type get_ground_material_type() const { return m_contact_material; }
    __forceinline bool ground_contact_offset_disabled() const { return (m_contact_flags & 1) != 0; }
    float get_ground_plane_contact_offset() const;
    __forceinline void set_ground_plane(const real_vector3d *plane) { m_ground_plane = *plane; }
    __forceinline long get_support_havok_component_index() const { return m_ground_component_index; }
    __forceinline long get_support_rigid_body_index() const { return m_ground_body_index; }
    __forceinline const real_matrix4x3 *get_support_havok_component_matrix() const { return &m_ground_transform; }

    // March 82c3a8f8, 82c3c498, 82c3c548 and 82c3c580: direct field return/set, no named locals.
    __forceinline float get_landing_velocity() { return m_landing_velocity; }
    __forceinline const real_vector3d *get_ground_plane() const { return &m_ground_plane; }
    __forceinline long get_support_object_index() const { return m_ground_object_index; }
    __forceinline void notify_in_slip_surface_volume() { m_contact_flags |= 2; }

private:
    void initialize_internal(bool);
    void update_animation(s_character_physics_update_output_datum *, const s_character_physics_update_input_datum *);
    bool update_localized_physics_space_linear_velocity(const s_character_physics_update_input_datum *, real_vector3d *, real_vector3d *, real_matrix3x3 *);
    bool update_ground_velocity(const s_character_physics_update_input_datum *, real_vector3d *, real_matrix3x3 *);
    void update_internal(s_character_physics_update_output_datum *, const s_character_physics_update_input_datum *, bool, const real_vector3d *);
    void set_contact(long, const s_havok_contact_point_reference *, const real_vector3d *);
    bool calculate_ground_contact(const s_character_physics_move_input_datum *, bool, const real_vector3d *,
        real_vector3d *, s_havok_contact_point_reference *, float *, long *);
    void build_composite_ground_plane(const s_character_physics_move_input_datum *, const real_matrix3x3 *,
        bool, const real_vector3d *, const s_havok_contact_point_reference *, long, real_vector3d *);

    real_vector3d m_localized_origin;
    long last_update_time;
    real_vector3d last_acceleration;
    long m_ground_component_index;
    long m_ground_body_index;
    real_matrix4x3 m_ground_transform;
    long m_ground_index;
    long m_ground_object_index;
    c_global_material_type m_contact_material;
    unsigned char m_update_count;
    unsigned char m_contact_flags;
    real_vector3d m_ground_plane;
    float m_landing_velocity;
};

typedef char character_physics_ground_size[(sizeof(c_character_physics_mode_ground_datum) == 0x74) ? 1 : -1];

#endif // PHYSICS_CHARACTER_PHYSICS_MODE_GROUND_DATUM_H
