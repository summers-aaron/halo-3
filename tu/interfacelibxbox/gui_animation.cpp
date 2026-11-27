// interfacelibxbox/gui_animation.obj: from the records 0x4e4700 0x4e4878 0x4e4ae8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "interface/gui_widget_types.h"
// @address 0x4e4ae8 ?transform_real_point3d@gui_real_rectangle2d@@SAXPATreal_point3d@@PBTrectangle2d@@@Z
#include <math.h>
#include "interface/interface_constants_inline.h"
#include "interface/user_interface.h"
#include "math/real_math.h"

// 0x4e4700
gui_real_rectangle2d *gui_real_rectangle2d::rotate_about_local_point(const real_point2d *origin, float sine, float cosine)
{
    if (sine != 0.0f)
    {
        real_vector2d target_scale = interface_get_authoring_to_target_dimensions_scaling();
        e_authored_ui_aspect_ratio aspect_ratio = (e_authored_ui_aspect_ratio)::interface_get_closest_authored_ui_aspect_ratio();
        real_vector2d conversion = interface_get_authoring_aspect_conversion_scaling(aspect_ratio);
        float center_x = origin->x * conversion.i * target_scale.i + points[0].x;
        float center_y = origin->y * conversion.j * target_scale.j + points[0].y;
        for (unsigned int i = 0; i < 4; i = i + 1)
        {
            real_vector2d offset;
            offset.i = points[i].x - center_x;
            offset.j = points[i].y - center_y;
            float y = sine * offset.i + cosine * offset.j;
            offset.i = cosine * offset.i - sine * offset.j;
            offset.j = y;
            points[i].x = center_x + offset.i;
            points[i].y = center_y + offset.j;
        }
    }
    return this;
}

// 0x4e4878
gui_real_rectangle2d *gui_real_rectangle2d::scale_about_local_point(const real_point2d *local_point, const real_vector2d *scale)
{
    if (scale->i != 1.0f || scale->j != 1.0f)
    {
        real_vector2d target_scale = interface_get_authoring_to_target_dimensions_scaling();
        e_authored_ui_aspect_ratio aspect_ratio = (e_authored_ui_aspect_ratio)::interface_get_closest_authored_ui_aspect_ratio();
        real_vector2d conversion = interface_get_authoring_aspect_conversion_scaling(aspect_ratio);
        real_point2d center;
        center.x = points[0].x + local_point->x * target_scale.i * conversion.i;
        center.y = points[0].y + local_point->y * target_scale.j * conversion.j;
        for (unsigned int i = 0; i < 4; i++)
        {
            real_vector2d offset;
            offset.i = (points[i].x - center.x) * scale->i;
            offset.j = (points[i].y - center.y) * scale->j;
            points[i].x = center.x + offset.i;
            points[i].y = center.y + offset.j;
        }
    }
    return this;
}

// 0x4e4ae8
void gui_real_rectangle2d::transform_real_point3d(real_point3d *point, const rectangle2d *rectangle)
{
    float half_width = (float)(rectangle->x1 - rectangle->x0) * 0.5f;
    float half_height = (float)(rectangle->y1 - rectangle->y0) * 0.5f;
    float near_distance;
    float unused_far;
    float unused_far2;
    user_interface_get_projection_plane_distances(&unused_far, &near_distance, &unused_far2);
    if (fabsf(near_distance) < 0.0001f)
        near_distance = 1.0f;
    point->z = point->z + near_distance;
    if (point->z >= 0.0f)
        point->z = point->z > 1.0f ? point->z : 1.0f;
    else
        point->z = point->z > -1.0f ? -1.0f : point->z;
    float scale = 1.0f / (point->z * (1.0f / near_distance));
    point->x = point->x * scale;
    point->y = point->y * scale;
    point->x = point->x + half_width;
    point->y = point->y + half_height;
    point->z = point->z - near_distance;
}
