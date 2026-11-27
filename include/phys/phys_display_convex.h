#ifndef INCLUDE_PHYS_DISPLAY_CONVEX_H
#define INCLUDE_PHYS_DISPLAY_CONVEX_H

#include "phys/phys_display_geometry.h"

class phys_display_convex : public phys_display_geometry {
public:
    phys_display_convex(phys_geometry* wire_body_cap);
    virtual void build_display_geometry();
    virtual void get_wireframe_geometry(phys_array<phys_vec4>& lines);
};

#endif // INCLUDE_HAVOK_HK_DISPLAY_CONVEX_H
