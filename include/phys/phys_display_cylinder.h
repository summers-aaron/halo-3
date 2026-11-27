#ifndef INCLUDE_PHYS_DISPLAY_CYLINDER_H
#define INCLUDE_PHYS_DISPLAY_CYLINDER_H

#include "phys/phys_display_geometry.h"

class phys_display_cylinder : public phys_display_geometry {
public:
    phys_display_cylinder(const phys_vec4& top, const phys_vec4& bottom, phys_real radius, int side_count = 9, int height_segment_count = 1);
    virtual void build_display_geometry();
    virtual void get_wireframe_geometry(phys_array<phys_vec4>& lines);
    const phys_vec4& get_capsule_top() { return m_capsule_top; }
    const phys_vec4& get_capsule_bottom() { return m_capsule_bottom; }
    phys_real get_convex_radius() { return m_radius; }
    inline int get_height_sample_count() { return m_height_segment_count; }
    inline int get_side_count() { return m_side_count; }
protected:
    phys_vec4 m_capsule_top;
    phys_vec4 m_capsule_bottom;
    phys_real m_radius;
    int m_side_count;
    int m_height_segment_count;
};

#endif // INCLUDE_HAVOK_HK_DISPLAY_CYLINDER_H
