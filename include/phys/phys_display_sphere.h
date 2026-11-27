#ifndef INCLUDE_PHYS_DISPLAY_SPHERE_H
#define INCLUDE_PHYS_DISPLAY_SPHERE_H

#include "phys/phys_display_geometry.h"
#include "phys/phys_collide.h"

class phys_display_sphere : public phys_display_geometry {
public:
    phys_display_sphere(const phys_sphere& sphere, int x_res, int y_res);
    virtual void build_display_geometry();
    virtual void get_wireframe_geometry(phys_array<phys_vec4>& lines);
    inline void get_sphere(phys_sphere& s) { s = m_display_sphere; }
    inline int get_x_res() { return m_x_res; }
    inline int get_y_res() { return m_y_res; }
protected:
    phys_sphere m_display_sphere;
    int m_x_res;
    int m_y_res;
};

#endif // INCLUDE_HAVOK_HK_DISPLAY_SPHERE_H
