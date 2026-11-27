#ifndef INCLUDE_PHYS_DISPLAY_BOX_H
#define INCLUDE_PHYS_DISPLAY_BOX_H

#include "phys/phys_display_geometry.h"

class phys_display_box : public phys_display_geometry {
public:
    phys_display_box(const phys_vec4& box_half_extents);
    phys_display_box();
    void set_box_params(const phys_vec4& box_half_extents, const phys_transform& t);
    virtual void build_display_geometry();
    virtual void get_wireframe_geometry(phys_array<phys_vec4>& lines);
    inline const phys_vec4& get_half_extents() { return m_half_extents; }
protected:
    phys_vec4 m_half_extents;
};

#endif // INCLUDE_HAVOK_HK_DISPLAY_BOX_H
