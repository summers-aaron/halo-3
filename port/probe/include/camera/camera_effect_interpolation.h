#ifndef PORT_CAMERA_EFFECT_INTERPOLATION_H
#define PORT_CAMERA_EFFECT_INTERPOLATION_H

#include "math/matrix_math_types.h"

// Main-thread samples of a tick-driven, camera-local effect. The published
// observer pose includes the blend, before collision and render interpolation.
struct s_camera_effect_interpolation
{
    bool valid;
    long tick;
    long previous_tick;
    real_matrix4x3 previous;
    real_matrix4x3 current;

    void sample(long time, float leftover, real_matrix4x3 *effect);
};

#endif
