#ifndef RASTERIZER_RASTERIZER_LUT_CONTROL_H
#define RASTERIZER_RASTERIZER_LUT_CONTROL_H

#include "math/real_math_types.h"

// The gamma lookup-table control and the functions defined in tu/graphicslibxbox/rasterizer_LUT_control.cpp.

struct _D3DGAMMARAMP;
struct _D3DPWLGAMMA;

enum e_gamma_ramp_type
{
    _gamma_ramp_true_sRGB = 0,
    _gamma_ramp_piecewise_linear_sRGB,
    _gamma_ramp_7e3,
};

enum
{
    k_gamma_setting_count = 5,
};

// Scale and black-offset values for each of the five gamma settings
extern "C" float k_gamma_setting_scales[k_gamma_setting_count];
extern "C" float k_gamma_setting_black_offsets[k_gamma_setting_count];

class c_rasterizer_LUT_control
{
public:
    void compute_LUT(_D3DGAMMARAMP &ramp);
    void compute_LUT(_D3DPWLGAMMA &ramp);

    e_gamma_ramp_type m_type;
    float m_brightness;
    float m_gamma;
    long m_setting;
    real_rgb_color m_white;
    real_rgb_color m_black;
};

float get_linear_buffer_value_from_true_sRGB(long index, long maximum, float brightness);
float get_linear_buffer_value_from_piecewise_linear_sRGB(long index, long maximum, float brightness);
float get_linear_buffer_value_from_7e3(long index, long maximum, float brightness);

#endif // RASTERIZER_RASTERIZER_LUT_CONTROL_H
