#ifndef CAMERA_OBSERVER_PROXIMITY_H
#define CAMERA_OBSERVER_PROXIMITY_H

// Defined in tu/blamlibxbox/observer.cpp: is any active user's camera within a radius of a point (optionally
// scaled by the field of view).

union real_point3d;

enum e_adjust_for_fov_scale
{
    _adjust_for_fov_scale_none,
    _adjust_for_fov_scale,
};

bool active_user_close_to_point_not_deterministic(const real_point3d *point, float radius, e_adjust_for_fov_scale adjust);

#endif // CAMERA_OBSERVER_PROXIMITY_H
