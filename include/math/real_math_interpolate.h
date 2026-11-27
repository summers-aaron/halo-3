#ifndef MATH_REAL_MATH_INTERPOLATE_H
#define MATH_REAL_MATH_INTERPOLATE_H

#include "math/real_math_types.h"

// match: 0x1440c8: __restrict on result schedules the point loads ahead of the lerp stores; the declaration spells it so every unit names the one definition
template<class t_point> void real_math_interpolate_points(const t_point *a, const t_point *b, float fraction, t_point *__restrict result);
#ifndef REAL_MATH_DEFINING_TU
// match: 0x1440c8: __restrict on result schedules the point loads ahead of the lerp stores; the instantiation spells it so callers name the one definition
extern template void real_math_interpolate_points<real_point3d>(const real_point3d *, const real_point3d *, float, real_point3d *__restrict);
#endif

#endif // MATH_REAL_MATH_INTERPOLATE_H
