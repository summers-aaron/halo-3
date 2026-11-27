#ifndef PHYSICS_HAVOK_COLLISION_GEOMETRY_SHAPE_H
#define PHYSICS_HAVOK_COLLISION_GEOMETRY_SHAPE_H

#include "phys/phys_collide.h"
#include "physics/havok_shape_fixup.h"

struct collision_bsp;

// hkVector4 members supply this class's natural alignment.
class c_collision_geometry_shape : public phys_compound_shape {
public:
    __forceinline c_collision_geometry_shape(phys_fixup_tag flag, const collision_bsp *bsp)
        : phys_compound_shape(flag)
    {
        m_bsp = bsp;
        initialize_tag_shape_reference_count_and_user_data(this, 0);
        m_edge_weld_disabled = false;
    }
    c_collision_geometry_shape(long, const collision_bsp *, long, long, long, float, long, const phys_vec4 *, const phys_vec4 *);
    virtual int get_child_shape_count() const;
    virtual unsigned int get_first_key() const;
    virtual unsigned int get_next_key(unsigned int old_key) const;
    virtual const phys_shape *get_child_shape(unsigned int key, char (&buffer)[512]) const;
#if !defined(_XBOX) || defined(__clang__)
    // GAME-CHANGES F120: the Xbox hkShape vtable 0x820405e4 slot 6 (getAabbImpl) is 0x25feb0, the box AABB from
    // m_aabbCenter / m_aabbHalfExtents that c_environment_surface_shape::getAabbImpl shares (one body in the image).
    // Without it the PC kept hkShapeCollection::getAabbImpl, which walks every child key. Defined in havok_shapes.cpp.
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
#endif
public:
    phys_vec4 m_aabb_center;
    phys_vec4 m_aabb_half_extents;
    long m_geometry_index;
    const collision_bsp *m_bsp;
    signed char m_structure_bsp_index;
    unsigned char m_type;
    short m_instance_index;
    float m_radius;
};

typedef char collision_geometry_shape_size[(sizeof(void *) != 4 || sizeof(c_collision_geometry_shape) == 0x50) ? 1 : -1];

#endif
