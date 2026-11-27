#ifndef INCLUDE_PHYS_DEBUG_DISPLAY_H
#define INCLUDE_PHYS_DEBUG_DISPLAY_H

#include <stddef.h>

#include "phys/phys_base.h"
#include "phys/phys_singleton.h"

class phys_critical_section;
class phys_display_geometry;
class phys_debug_display_handler;

class phys_debug_display : public phys_singleton<phys_debug_display> {
public:
    phys_array<phys_debug_display_handler *> m_handlers;
    phys_critical_section *m_display_lock;
    phys_debug_display();
    virtual ~phys_debug_display();
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_DEBUG_VIEWER);
    void remove_debug_display_handler(phys_debug_display_handler *handler);
    void clear();
    void add_geometry(const phys_array<phys_display_geometry *> &, const phys_transform &, unsigned long, int);
    void add_geometry_instance(unsigned long, const phys_transform &, unsigned long, int);
    void set_geometry_color(int, unsigned long, int);
    void update_geometry(const phys_transform &, unsigned long, int);
    void remove_debug_geometry(unsigned long, int);
    void update_camera(const phys_vec4 &, const phys_vec4 &, const phys_vec4 &, float, float, float, const char *);
    void display_point(const phys_vec4 &, int, int);
    void display_line(const phys_vec4 &, const phys_vec4 &, int, int);
    void display_line_local(const phys_qs_transform &, const phys_vec4 &, const phys_vec4 &, int, int);
    void display_line_local(const phys_transform &, const phys_vec4 &, const phys_vec4 &, int, int);
    void display_ray(const phys_vec4 &, const phys_vec4 &, int, int);
    void display_ray_local(const phys_qs_transform &, const phys_vec4 &, const phys_vec4 &, int, int);
    void display_ray_local(const phys_transform &, const phys_vec4 &, const phys_vec4 &, int, int);
    void display_arrow(const phys_vec4 &, const phys_vec4 &, int, int);
    void display_plane(const phys_vec4 &, const phys_vec4 &, float, int, int);
    void display_star(const phys_vec4 &, float, int, int);
    void display_star_local(const phys_qs_transform &, const phys_vec4 &, float, int, int);
    void display_star_local(const phys_transform &, const phys_vec4 &, float, int, int);
    void debug_display_text(const char *, int, int);
    void display_3d_text(const char *, const phys_vec4 &, int, int);
    void display_geometry(const phys_array<phys_display_geometry *> &, const phys_transform &, int, int);
    void display_geometry(const phys_array<phys_display_geometry *> &, int, int);
    void add_debug_display_handler(phys_debug_display_handler *handler);
    void display_frame(const phys_qs_transform &, float, int);
    void display_frame(const phys_transform &, float, int);
};

typedef char havok_size_hkDebugDisplay[(sizeof(void *) != 4 || ((sizeof(phys_debug_display) == 24) )) ? 1 : -1];
typedef char havok_offset_hkDebugDisplay_handlers[(sizeof(void *) != 4 || (offsetof(phys_debug_display, m_handlers) == 8)) ? 1 : -1];
typedef char havok_offset_hkDebugDisplay_section[(sizeof(void *) != 4 || (offsetof(phys_debug_display, m_display_lock) == 20)) ? 1 : -1];

#endif
