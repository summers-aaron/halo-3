#ifndef INCLUDE_PHYS_DISPLAY_PLANE_H
#define INCLUDE_PHYS_DISPLAY_PLANE_H
#include "phys/phys_display_geometry.h"
class phys_display_plane : public phys_display_geometry {
public:
    phys_display_plane();
    phys_display_plane(const phys_vec4 &normal, const phys_vec4 &perp_to_normal, const phys_vec4 &center, const phys_vec4 &extent);
    phys_vec4 &get_contact_normal();
    phys_vec4 &get_plane_center();
    phys_vec4 &get_perp_to_normal();
    virtual void build_display_geometry();
    void set_box_params(const phys_vec4 &normal, const phys_vec4 &perp_to_normal, const phys_vec4 &center, const phys_vec4 &extent);
protected:
    void make_plane_points(phys_vec4 *points);
public:

    phys_vec4 m_normal;
    phys_vec4 m_plane_center;
    phys_vec4 m_perp_to_normal;
    phys_vec4 m_plane_extent;
public:
  void virtual get_wireframe_geometry(phys_array<phys_vec4> &lines);
};
#endif
