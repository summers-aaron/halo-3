#ifndef EFFECTS_EFFECT_LIGHTPROBES_H
#define EFFECTS_EFFECT_LIGHTPROBES_H

#include "math/real_math_types.h"
struct s_effect_lightprobe_result;
extern "C" long effect_sample_lightprobes(const real_point3d *point, s_effect_lightprobe_result *result);

#endif
