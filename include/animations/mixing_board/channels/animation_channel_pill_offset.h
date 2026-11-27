#ifndef ANIMATIONS_ANIMATION_CHANNEL_PILL_OFFSET_H
#define ANIMATIONS_ANIMATION_CHANNEL_PILL_OFFSET_H
class c_animation_data;
union real_point3d;
union real_vector3d;
// Gets the pill offset position and delta for an animation frame.
extern "C" void animation_channel_get_pill_offset(const c_animation_data *animation,
    long frame, real_point3d *position, real_vector3d *delta);
#endif
