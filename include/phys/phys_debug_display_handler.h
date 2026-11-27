#ifndef INCLUDE_PHYS_DEBUG_DISPLAY_HANDLER_H
#define INCLUDE_PHYS_DEBUG_DISPLAY_HANDLER_H

class phys_display_geometry;
class phys_transform;
class phys_vec4;
template <typename T> class phys_array;
enum phys_result;

class phys_debug_display_handler {
public:
    enum debug_command {
        PHYS_STEP = 0x00,
        PHYS_ADD_GEOMETRY,
        PHYS_UPDATE_GEOMETRY,
        PHYS_SET_COLOR_GEOMETRY,
        PHYS_REMOVE_GEOMETRY,
        PHYS_DISPLAY_GEOMETRY_WITH_TRANSFORM,
        PHYS_DISPLAY_GEOMETRY,
        PHYS_DISPLAY_POINT,
        PHYS_DISPLAY_LINE,
        PHYS_DISPLAY_TEXT,
        PHYS_SEND_STATISTICS_DUMP_DEPRECATED,
        PHYS_HOLD_IMMEDIATE,
        PHYS_ADD_CAMERA,
        PHYS_UPDATE_CAMERA,
        PHYS_REMOVE_CAMERA,
        PHYS_SEND_MEMSTATS_DUMP,
        PHYS_ADD_GEOMETRY_INSTANCE,
        PHYS_DISPLAY_TEXT_3D
    };

    virtual ~phys_debug_display_handler() {}
    virtual phys_result add_geometry(const phys_array<phys_display_geometry *> &geometry, const phys_transform &transform, unsigned long id, int tag) = 0;
    virtual phys_result add_geometry_instance(unsigned long original_id, const phys_transform &transform, unsigned long id, int tag) = 0;
    virtual phys_result set_geometry_color(int color, unsigned long id, int tag) = 0;
    virtual phys_result update_geometry(const phys_transform &transform, unsigned long id, int tag) = 0;
    virtual phys_result remove_debug_geometry(unsigned long id, int tag) = 0;
    virtual phys_result update_camera(const phys_vec4 &from, const phys_vec4 &to, const phys_vec4 &up,
        float camera_near_plane, float camera_far_plane, float camera_fov_rad, const char *name) = 0;
    virtual phys_result display_point(const phys_vec4 &position, int color, int tag) = 0;
    virtual phys_result display_line(const phys_vec4 &from, const phys_vec4 &to, int color, int tag) = 0;
    virtual phys_result debug_display_text(const char *text, int color, int tag) = 0;
    virtual phys_result display_3d_text(const char *text, const phys_vec4 &position, int color, int tag) = 0;
    virtual phys_result display_geometry(const phys_array<phys_display_geometry *> &geometry, const phys_transform &transform, int color, int tag) = 0;
    virtual phys_result display_geometry(const phys_array<phys_display_geometry *> &geometry, int color, int tag) = 0;
    virtual phys_result send_mem_stats_dump(const char *data, int length) = 0;
    virtual phys_result hold_immediate() = 0;
    virtual void lock_for_update() {}
    virtual void unlock_after_update() {}
};

#endif
