// blamlibxbox/wind.obj: from the records 0x2962d0
// @flags /TP /O2 /Oc

#include "scenario/wind.h"
static __forceinline float wind_interpolate_component(float p0, float p1, float p2, float p3, float start, float step, float time)
{
    float d01 = p1 - p0;
    float dd012 = p2 - p1;
    float ddd = p3 - p2;
    ddd -= dd012;
    dd012 -= d01;
    ddd -= dd012;
    return (((time - (step * 2.0f + start)) * ddd / (step * 3.0f) + dd012)
            * (time - (start + step)) / (step * 2.0f) + d01)
           * (1.0f / step) * (time - start) + p0;
}

// @address 0x2962d0 wind_interpolate_vector
extern "C" void wind_interpolate_vector(float *out, const float *p0, const float *p1, const float *p2, const float *p3, float start, float step, float time)
{
    out[0] = wind_interpolate_component(p0[0], p1[0], p2[0], p3[0], start, step, time);
    out[1] = wind_interpolate_component(p0[1], p1[1], p2[1], p3[1], start, step, time);
    out[2] = wind_interpolate_component(p0[2], p1[2], p2[2], p3[2], start, step, time);
}
