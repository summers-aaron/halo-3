#ifndef INCLUDE_PHYS_DISPLAY_GEOMETRY_H
#define INCLUDE_PHYS_DISPLAY_GEOMETRY_H

#include "phys/phys_geometry.h"

enum phys_display_geometry_type {
    PHYS_DISPLAY_RECTANGLE = 0,
    PHYS_DISPLAY_SPHERE,
    PHYS_DISPLAY_BOX,
    PHYS_DISPLAY_AABB,
    PHYS_DISPLAY_CONE,
    PHYS_DISPLAY_SEMICIRCLE,
    PHYS_DISPLAY_CONVEX,
    PHYS_DISPLAY_PLANE,
    PHYS_DISPLAY_CAPSULE,
    PHYS_DISPLAY_CYLINDER
};

class phys_display_geometry : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_DISPLAY);
    inline phys_display_geometry(phys_display_geometry_type type);
    virtual ~phys_display_geometry();
    inline phys_geometry* get_geometry();
    virtual void get_wireframe_geometry(phys_array<phys_vec4>& lines) = 0;
    virtual void build_display_geometry() = 0;
    inline const phys_transform& get_transform() const;
    inline phys_transform& get_transform();
    inline phys_display_geometry_type get_type_id() const;
    phys_geometry* m_geometry;
protected:
    phys_transform m_body_transform;
    phys_display_geometry_type m_type;
};

inline phys_display_geometry::phys_display_geometry(phys_display_geometry_type type) : m_geometry(0), m_type(type) {
    m_body_transform.set_to_identity();
}
inline phys_geometry* phys_display_geometry::get_geometry() { return m_geometry; }
inline const phys_transform& phys_display_geometry::get_transform() const { return m_body_transform; }
inline phys_transform& phys_display_geometry::get_transform() { return m_body_transform; }
inline phys_display_geometry_type phys_display_geometry::get_type_id() const { return m_type; }

#endif // INCLUDE_HAVOK_HK_DISPLAY_GEOMETRY_H
