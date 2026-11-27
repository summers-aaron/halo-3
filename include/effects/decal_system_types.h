#ifndef EFFECTS_DECAL_SYSTEM_TYPES_H
#define EFFECTS_DECAL_SYSTEM_TYPES_H

#include "effects/decals.h"
#include "effects/decal_definition_types.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_index.h"
#include "memory/static_arrays.h"
#include "objects/cluster_reference.h"
#include "structures/cluster_bit_vectors.h"
#include <stddef.h>

struct s_decals_system_definition;
struct s_decal_collision_result;

class c_decal_system {
    friend long decal_system_create(unsigned char, long, long, long, bool, const real_point3d *, const real_vector3d *, const real_vector3d *);
public:
    static void initialize();
    static void initialize_for_new_map();
    static void initialize_for_new_structure_bsp(unsigned long activating_structure_bsp_mask);
    static void dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
    static void adopt_old_render_message_queue(void *);
    static void dispose_from_old_map();
    static void add_cluster_to_pvs(const s_cluster_reference *cluster_reference);
    static __forceinline void reset_render_message_queue();
    static void change_pvs(const s_game_cluster_bit_vectors *old_pvs, const s_game_cluster_bit_vectors *new_pvs, bool update);
    static void destroy(long index);
    static void frame_advance_all(float dt);
private:
    bool check_overlap(const real_point3d *);
    bool collide(const real_point3d *point, const real_vector3d *vector, long ignore_object_index,
        c_static_sized_dynamic_array<s_decal_collision_result, 16> *collisions);
    __forceinline const s_decals_system_definition *get_definition() const;
    __forceinline const real_matrix4x3 *get_projection() const { return &m_projection; }
    __forceinline const real_point3d *get_center() const { return &m_projection.position; }
    __forceinline void set_center(const real_point3d *center) { m_projection.position = *center; }
    __forceinline void set_preplaced(bool preplaced)
    {
        if (preplaced) m_flags |= 1;
        else m_flags &= ~1UL;
    }
    __forceinline void set_projection_forward(const real_vector3d *forward) { m_projection.rotation.forward = *forward; }
    __forceinline void set_projection_up(const real_vector3d *up) { m_projection.rotation.up = *up; }
public:
    long m_datum_header;
    c_tag_index m_definition_index;
    long m_first_child_index;
    unsigned long m_flags;
    c_static_array<s_cluster_reference, 4> m_cluster_references;
    real_matrix4x3 m_projection;
    float m_rotation;
public:
    static void render_all(c_decal_definition::e_pass pass);
};

typedef char decal_system_types_size[(sizeof(c_decal_system) == 0x50) ? 1 : -1];
typedef char decal_system_types_projection_offset[(offsetof(c_decal_system, m_projection) == 0x18) ? 1 : -1];

#endif // EFFECTS_DECAL_SYSTEM_TYPES_H
