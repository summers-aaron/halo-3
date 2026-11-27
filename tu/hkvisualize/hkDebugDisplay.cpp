// hkvisualize/hkDebugDisplay.obj: from the records 0x6b5858 0x6b5920 0x6b5990 0x6b5a58 0x6b5b20 0x6b5be0 0x6b5ca0 0x6b5d58 0x6b5e50 0x6b5f10 0x6b5fd8 0x6b6030 0x6b6098 0x6b6120 0x6b61c8 0x6b6218 0x6b6490 0x6b6540 0x6b6590 0x6b65e0 0x6b68b8 0x6b6978 0x6b6a40 0x6b6b08 0x6b6bc8 0x6b6c70 0x6b6df0 0x6b6e50 0x6b6f28 0x6b6fb8 0x70b468 0x6b6ff8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x6b5858 ?remove_debug_display_handler@phys_debug_display@@QAAXPAVphys_debug_display_handler@@@Z
// @address 0x6b5920 ?clear@phys_debug_display@@QAAXXZ
// @address 0x6b5990 ?add_geometry@phys_debug_display@@QAAXABV?$phys_array@PAVphys_display_geometry@@@@ABVphys_transform@@KH@Z
// @address 0x6b5a58 ?add_geometry_instance@phys_debug_display@@QAAXKABVphys_transform@@KH@Z
// @address 0x6b5b20 ?set_geometry_color@phys_debug_display@@QAAXHKH@Z
// @address 0x6b5be0 ?update_geometry@phys_debug_display@@QAAXABVphys_transform@@KH@Z
// @address 0x6b5ca0 ?remove_debug_geometry@phys_debug_display@@QAAXKH@Z
// @address 0x6b5d58 ?update_camera@phys_debug_display@@QAAXABVphys_vec4@@00MMMPBD@Z
// @address 0x6b5e50 ?display_point@phys_debug_display@@QAAXABVphys_vec4@@HH@Z
// @address 0x6b5f10 ?display_line@phys_debug_display@@QAAXABVphys_vec4@@0HH@Z
// @address 0x6b5fd8 ?display_line_local@phys_debug_display@@QAAXABVphys_qs_transform@@ABVphys_vec4@@1HH@Z
// @address 0x6b6030 ?display_line_local@phys_debug_display@@QAAXABVphys_transform@@ABVphys_vec4@@1HH@Z
// @address 0x6b6098 ?display_ray@phys_debug_display@@QAAXABVphys_vec4@@0HH@Z
// @address 0x6b6120 ?display_ray_local@phys_debug_display@@QAAXABVphys_qs_transform@@ABVphys_vec4@@1HH@Z
// @address 0x6b61c8 ?display_ray_local@phys_debug_display@@QAAXABVphys_transform@@ABVphys_vec4@@1HH@Z
// @address 0x6b6218 ?display_arrow@phys_debug_display@@QAAXABVphys_vec4@@0HH@Z
// @address 0x6b6490 ?display_star@phys_debug_display@@QAAXABVphys_vec4@@MHH@Z
// @address 0x6b6540 ?display_star_local@phys_debug_display@@QAAXABVphys_qs_transform@@ABVphys_vec4@@MHH@Z
// @address 0x6b6590 ?display_star_local@phys_debug_display@@QAAXABVphys_transform@@ABVphys_vec4@@MHH@Z
// @address 0x6b68b8 ?debug_display_text@phys_debug_display@@QAAXPBDHH@Z
// @address 0x6b6978 ?display_3d_text@phys_debug_display@@QAAXPBDABVphys_vec4@@HH@Z
// @address 0x6b6a40 ?display_geometry@phys_debug_display@@QAAXABV?$phys_array@PAVphys_display_geometry@@@@ABVphys_transform@@HH@Z
// @address 0x6b6b08 ?display_geometry@phys_debug_display@@QAAXABV?$phys_array@PAVphys_display_geometry@@@@HH@Z
// @address 0x6b6bc8 ?add_debug_display_handler@phys_debug_display@@QAAXPAVphys_debug_display_handler@@@Z
// @address 0x6b6df0 ?display_frame@phys_debug_display@@QAAXABVphys_transform@@MH@Z
// @address 0x6b6fb8 ?phys_debug_display_create@@YAPAVphys_ref_object@@XZ
// @address 0x70b468 ??__Ephys_singleton_reg_debug_display@@YAXXZ
// @address 0x6b6e50 ??0phys_debug_display@@QAA@XZ
// @address 0x6b6ff8 ??_Gphys_debug_display@@UAAPAXI@Z
// @address 0x6b6f28 ??1phys_debug_display@@UAA@XZ
// @address 0x5bec98 ?gather_memory_stats@phys_ref_object@@UBAXPAVphys_statistics_collector@@@Z
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "physics/havok_memory.h"

#include "phys/phys_critical_section.h"
#include "phys/phys_math.h"
#include "phys/phys_object.h"
#include "phys/phys_base.h"
#include "phys/phys_singleton.h"
namespace phys_math {
}
#include "phys/phys_vec4_util.h"
#include "phys/phys_debug_display_handler.h"
class phys_color {
public:
    enum ExtendedColours { color_red = 0xFFFF0000, color_green = 0xFF008000, color_blue = 0xFF0000FF };
};
#include "phys/phys_debug_display.h"
#include "phys/phys_display_geometry.h"
#include "phys/phys_statistics_collector.h"

template<> phys_debug_display *phys_singleton<phys_debug_display>::s_instance = 0;

// 0x6b6e50
phys_debug_display::phys_debug_display()
{
    m_display_lock = new phys_critical_section(1000);
}

// 0x6b5858
void phys_debug_display::remove_debug_display_handler(phys_debug_display_handler *handler)
{
    m_display_lock->enter();
    int index = m_handlers.array_index_of(handler);
    if (index >= 0) m_handlers.remove_at(index);
    m_display_lock->leave();
}
// 0x6b5920
void phys_debug_display::clear()
{
    m_display_lock->enter();
    m_handlers.clear();
    m_display_lock->leave();
}
// 0x6b5990
void phys_debug_display::add_geometry(const phys_array<phys_display_geometry *> &geometry, const phys_transform &transform, unsigned long id, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->add_geometry(geometry, transform, id, tag);
    }
    m_display_lock->leave();
}
// 0x6b5a58
void phys_debug_display::add_geometry_instance(unsigned long original_id, const phys_transform &transform, unsigned long id, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->add_geometry_instance(original_id, transform, id, tag);
    }
    m_display_lock->leave();
}
// 0x6b5b20
void phys_debug_display::set_geometry_color(int color, unsigned long id, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->set_geometry_color(color, id, tag);
    }
    m_display_lock->leave();
}
// 0x6b5be0
void phys_debug_display::update_geometry(const phys_transform &transform, unsigned long id, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->update_geometry(transform, id, tag);
    }
    m_display_lock->leave();
}
// 0x6b5ca0
void phys_debug_display::remove_debug_geometry(unsigned long id, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->remove_debug_geometry(id, tag);
    }
    m_display_lock->leave();
}
// 0x6b5d58
void phys_debug_display::update_camera(const phys_vec4 &from, const phys_vec4 &to, const phys_vec4 &up, float camera_near_plane,
    float camera_far_plane, float camera_fov_rad, const char *name)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->update_camera(from, to, up, camera_near_plane, camera_far_plane, camera_fov_rad, name);
    }
    m_display_lock->leave();
}
// 0x6b5e50
void phys_debug_display::display_point(const phys_vec4 &position, int color, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->display_point(position, color, tag);
    }
    m_display_lock->leave();
}
// 0x6b5f10
void phys_debug_display::display_line(const phys_vec4 &from, const phys_vec4 &to, int color, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->display_line(from, to, color, tag);
    }
    m_display_lock->leave();
}
// 0x6b5fd8
void phys_debug_display::display_line_local(const phys_qs_transform &transform, const phys_vec4 &from, const phys_vec4 &to, int color, int tag)
{
    phys_vec4 world_from, world_to;
    world_from.vec_set_transformed_pos(transform, from);
    world_to.vec_set_transformed_pos(transform, to);
    display_line(world_from, world_to, color, tag);
}
// 0x6b6030
void phys_debug_display::display_line_local(const phys_transform &transform, const phys_vec4 &from, const phys_vec4 &to, int color, int tag)
{
    phys_qs_transform qs_transform;
    qs_transform.set_from_transform(transform);
    display_line_local(qs_transform, from, to, color, tag);
}
// 0x6b6098
void phys_debug_display::display_ray(const phys_vec4 &from, const phys_vec4 &direction, int color, int tag)
{
    phys_vec4 end;
    end.set_add4(from, direction);
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->display_line(from, end, color, tag);
    }
}
// 0x6b6120
void phys_debug_display::display_ray_local(const phys_qs_transform &transform, const phys_vec4 &from, const phys_vec4 &direction, int color, int tag)
{
    phys_vec4 world_from, world_ray_dir;
    world_from.vec_set_transformed_pos(transform, from);
    world_ray_dir.vec_set_rotated_dir(transform.m_rotation, direction);
    display_ray(world_from, world_ray_dir, color, tag);
}
// 0x6b61c8
void phys_debug_display::display_ray_local(const phys_transform &transform, const phys_vec4 &from, const phys_vec4 &direction, int color, int tag)
{
    phys_qs_transform qs_transform;
    qs_transform.set_from_transform(transform);
    display_ray_local(qs_transform, from, direction, color, tag);
}
// 0x6b6218
void phys_debug_display::display_arrow(const phys_vec4 &from, const phys_vec4 &direction, int color, int tag)
{
    if (direction.length_sq_3d() < 1.1920929e-07f) {
        return;
    }
    phys_vec4 to;
    to.set_add4(from, direction);
    phys_vec4 ortho_dir;
    phys_vec4_util::compute_perpendicular_vec(direction, ortho_dir);
    ortho_dir.normalize_xyz();
    phys_vec4 ortho_dir2;
    ortho_dir2.set_cross(direction, ortho_dir);
    ortho_dir.mul4(direction.length_3d());
    const float c = 0.85f;
    phys_vec4 p;
    p.set_lerp4(from, to, c);
    phys_vec4 p0;
    p0.set_add_mul4(p, ortho_dir, 1.0f - c);
    phys_vec4 p1;
    p1.set_add_mul4(p, ortho_dir, -(1.0f - c));
    phys_vec4 p2;
    p2.set_add_mul4(p, ortho_dir2, 1.0f - c);
    phys_vec4 p3;
    p3.set_add_mul4(p, ortho_dir2, -(1.0f - c));
    display_line(from, to, color, tag);
    display_line(to, p0, color, tag);
    display_line(to, p1, color, tag);
    display_line(to, p2, color, tag);
    display_line(to, p3, color, tag);
}
// 0x6b6490
void phys_debug_display::display_star(const phys_vec4 &position, float scale, int color, int tag)
{
    phys_vec4 offset, from, to;
    for (int axis = 0; axis < 3; ++axis) {
        offset.set_zero4();
        offset(axis) = scale;
        from.set_add4(position, offset);
        to.set_sub4(position, offset);
        display_line(from, to, color, tag);
    }
}
// 0x6b6540
void phys_debug_display::display_star_local(const phys_qs_transform &transform, const phys_vec4 &position, float scale, int color, int tag)
{
    phys_vec4 world_draw_pos;
    world_draw_pos.vec_set_transformed_pos(transform, position);
    display_star(world_draw_pos, scale, color, tag);
}
// 0x6b6590
void phys_debug_display::display_star_local(const phys_transform &transform, const phys_vec4 &position, float scale, int color, int tag)
{
    phys_vec4 world_draw_pos;
    world_draw_pos.vec_set_transformed_pos(transform, position);
    display_star(world_draw_pos, scale, color, tag);
}
// @address 0x6b65e0 ?display_plane@phys_debug_display@@QAAXABVphys_vec4@@0MHH@Z
void phys_debug_display::display_plane(const phys_vec4 &plane, const phys_vec4 &offset, float scale, int color, int tag)
{
    phys_vec4 pos;
    pos.set_add_mul4(offset, plane, -plane(3));
    const phys_vec4 &normal = plane;

    int major = normal.get_major_axis();
    phys_vec4 frame_binormal;
    frame_binormal.set_zero4();
    frame_binormal((major + 1) % 3) = 1;

    frame_binormal.set_cross(normal, frame_binormal);
    frame_binormal.normalize_xyz();
    frame_binormal.mul4(scale);

    phys_vec4 tangent;
    tangent.set_cross(frame_binormal, normal);
    tangent.normalize_xyz();
    tangent.mul4(scale);

    for (int e = 0; e < 4; e++) {
        phys_vec4 line_pt1 = pos;
        (((e + 0) % 4) / 2) ? line_pt1.sub4(tangent) : line_pt1.add4(tangent);
        (((e + 1) % 4) / 2) ? line_pt1.sub4(frame_binormal) : line_pt1.add4(frame_binormal);

        phys_vec4 line_pt2 = pos;
        (((e + 1) % 4) / 2) ? line_pt2.sub4(tangent) : line_pt2.add4(tangent);
        (((e + 2) % 4) / 2) ? line_pt2.sub4(frame_binormal) : line_pt2.add4(frame_binormal);

        display_line(line_pt1, line_pt2, color, tag);
    }

    phys_vec4 scaled_normal;
    scaled_normal.set_mul4(scale, normal);
    display_arrow(pos, scaled_normal, color, tag);
}
// 0x6b68b8
void phys_debug_display::debug_display_text(const char *text, int color, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->debug_display_text(text, color, tag);
    }
    m_display_lock->leave();
}
// 0x6b6978
void phys_debug_display::display_3d_text(const char *text, const phys_vec4 &position, int color, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->display_3d_text(text, position, color, tag);
    }
    m_display_lock->leave();
}
// 0x6b6a40
void phys_debug_display::display_geometry(const phys_array<phys_display_geometry *> &geometry, const phys_transform &transform, int color, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->display_geometry(geometry, transform, color, tag);
    }
    m_display_lock->leave();
}
// 0x6b6b08
void phys_debug_display::display_geometry(const phys_array<phys_display_geometry *> &geometry, int color, int tag)
{
    m_display_lock->enter();
    for (int index = 0; index < m_handlers.getSize(); ++index) {
        phys_debug_display_handler *handler = m_handlers[index];
        handler->display_geometry(geometry, color, tag);
    }
    m_display_lock->leave();
}
// 0x6b6bc8
void phys_debug_display::add_debug_display_handler(phys_debug_display_handler *handler)
{
    m_display_lock->enter();
    m_handlers.array_push_back(handler);
    m_display_lock->leave();
}
// @address 0x6b6c70 ?display_frame@phys_debug_display@@QAAXABVphys_qs_transform@@MH@Z
void phys_debug_display::display_frame(const phys_qs_transform &world_from_local, float size, int tag)
{
    phys_vec4 zero_point;
    phys_vec4 X;
    phys_vec4 Y;
    phys_vec4 Z;
    phys_vec4 vec; vec.set_zero4();
    zero_point.vec_set_transformed_pos(world_from_local, vec);
    vec.set(size, 0, 0, 0);
    X.vec_set_transformed_pos(world_from_local, vec);
    vec.set(0, size, 0, 0);
    Y.vec_set_transformed_pos(world_from_local, vec);
    vec.set(0, 0, size, 0);
    Z.vec_set_transformed_pos(world_from_local, vec);
    phys_vec4 axis_dir_x; axis_dir_x.set_sub4(X, zero_point);
    phys_vec4 axis_dir_y; axis_dir_y.set_sub4(Y, zero_point);
    phys_vec4 axis_dir_z; axis_dir_z.set_sub4(Z, zero_point);
    display_arrow(zero_point, axis_dir_x, phys_color::color_red, tag);
    display_arrow(zero_point, axis_dir_y, phys_color::color_green, tag);
    display_arrow(zero_point, axis_dir_z, phys_color::color_blue, tag);
}
// 0x6b6df0
void phys_debug_display::display_frame(const phys_transform &transform, float scale, int tag)
{
    phys_qs_transform qs_transform;
    qs_transform.set_from_transform(transform);
    display_frame(qs_transform, scale, tag);
}
// 0x6b6fb8
phys_ref_object *phys_debug_display_create()
{
    return new phys_debug_display;
}

// Registers the singleton at the head of hkSingletonInitList.

// 0x70b468 (dynamic initializer of the registration node)
phys_singleton_init_node phys_singleton_reg_debug_display = phys_singleton_init_node(phys_debug_display_create,
    reinterpret_cast<void **>(&phys_singleton<phys_debug_display>::s_instance));

// @address 0x6b6f28 ??1phys_debug_display@@UAA@XZ
phys_debug_display::~phys_debug_display()
{
    delete m_display_lock;
}

