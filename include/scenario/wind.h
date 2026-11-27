#ifndef SCENARIO_WIND_H
#define SCENARIO_WIND_H

// Functions defined in tu/blamlibxbox/wind.cpp that other units call.

extern "C" void wind_interpolate_vector(float *out, const float *p0, const float *p1, const float *p2, const float *p3, float start, float step, float time);

#endif // SCENARIO_WIND_H
