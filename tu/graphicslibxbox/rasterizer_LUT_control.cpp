// rasterizer_LUT_control.obj, from the records 0x4687d8 0x468830 0x468908 0x468988 0x468c18
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
// @address 0x468830 ?get_linear_buffer_value_from_piecewise_linear_sRGB@@YAMJJM@Z
// @address 0x468908 ?get_linear_buffer_value_from_7e3@@YAMJJM@Z
// @address 0x468c18 ?compute_LUT@c_rasterizer_LUT_control@@QAAXAAU_D3DPWLGAMMA@@@Z
#include <math.h>

#include "cseries/cseries.h"
#include "rasterizer/rasterizer_LUT_control.h"

struct _D3DGAMMARAMP
{
    unsigned short red[256];
    unsigned short green[256];
    unsigned short blue[256];
};

struct _D3DPWLVALUE
{
    unsigned short base;
    unsigned short delta;
};

struct _D3DPWLGAMMA
{
    _D3DPWLVALUE red[128];
    _D3DPWLVALUE green[128];
    _D3DPWLVALUE blue[128];
};

extern "C" {
// @address 0x73056c k_gamma_setting_scales
float k_gamma_setting_scales[k_gamma_setting_count] = { 0.76f, 0.88f, 1.0f, 1.12f, 1.24f };
// @address 0x8c4860 k_gamma_setting_black_offsets
float k_gamma_setting_black_offsets[k_gamma_setting_count];
}

// @address 0x4687d8 ?get_linear_buffer_value_from_true_sRGB@@YAMJJM@Z
float get_linear_buffer_value_from_true_sRGB(long index, long maximum, float brightness)
{
    return (float)pow((double)((float)index / (float)maximum), (double)brightness);
}

float get_linear_buffer_value_from_piecewise_linear_sRGB(long index, long maximum, float brightness)
{
    float x = (float)index / (float)maximum;
    float a = x * 0.24926686f;
    float b = (x - 0.1245098f) * 0.49853373f;
    float c = (x - 0.2495098f) * 0.99706745f;
    float d = (x - 0.5002451f) * 1.9941349f;
    return MAX(MAX(a, b), MAX(c, d));
}

float get_linear_buffer_value_from_7e3(long index, long maximum, float brightness)
{
    long mantissa = index & 127;
    long exponent = (index >> 7) & 7;
    long biased_exponent = MAX(exponent, 1);
    long implicit_bit = MIN(exponent, 1);
    return (float)ldexp((double)((float)implicit_bit + (float)mantissa / 128.0f), biased_exponent - 8);
}

void c_rasterizer_LUT_control::compute_LUT(_D3DGAMMARAMP &ramp)
{
    m_setting = PIN(m_setting, 0, k_gamma_setting_count - 1);
    float (*get_linear_buffer_value)(long, long, float) = 0;
    switch (m_type)
    {
    case _gamma_ramp_true_sRGB:
        get_linear_buffer_value = get_linear_buffer_value_from_true_sRGB;
        break;
    case _gamma_ramp_piecewise_linear_sRGB:
        get_linear_buffer_value = get_linear_buffer_value_from_piecewise_linear_sRGB;
        break;
    case _gamma_ramp_7e3:
        get_linear_buffer_value = get_linear_buffer_value_from_7e3;
        break;
    }
    float exponent = 1.0f / (k_gamma_setting_scales[m_setting] * m_gamma);
    real_rgb_color black_point = {m_black.red + k_gamma_setting_black_offsets[m_setting],
        m_black.green + k_gamma_setting_black_offsets[m_setting], m_black.blue + k_gamma_setting_black_offsets[m_setting]};
    real_rgb_color range = { m_white.red - black_point.red, m_white.green - black_point.green, m_white.blue - black_point.blue };
    for (long index = 0; index < 256; index++)
    {
        float value = get_linear_buffer_value(index, 255, m_brightness);
        real_rgb_color color = { range.red * value, range.green * value, range.blue * value };
        color.red += black_point.red;
        color.green += black_point.green;
        color.blue += black_point.blue;
        real_rgb_color output;
        output.red = (float)pow((double)PIN(color.red, 0.0f, 1.0f), (double)exponent);
        output.green = (float)pow((double)PIN(color.green, 0.0f, 1.0f), (double)exponent);
        output.blue = (float)pow((double)PIN(color.blue, 0.0f, 1.0f), (double)exponent);
        ramp.red[index] = (unsigned short)PIN((long)(output.red * 65535.0f), 0, 65535);
        ramp.green[index] = (unsigned short)PIN((long)(output.green * 65535.0f), 0, 65535);
        ramp.blue[index] = (unsigned short)PIN((long)(output.blue * 65535.0f), 0, 65535);
    }
}

void c_rasterizer_LUT_control::compute_LUT(_D3DPWLGAMMA &ramp)
{
    float (*get_linear_buffer_value)(long, long, float) = 0;
    switch (m_type)
    {
    case _gamma_ramp_true_sRGB:
        get_linear_buffer_value = get_linear_buffer_value_from_true_sRGB;
        break;
    case _gamma_ramp_piecewise_linear_sRGB:
        get_linear_buffer_value = get_linear_buffer_value_from_piecewise_linear_sRGB;
        break;
    case _gamma_ramp_7e3:
        get_linear_buffer_value = get_linear_buffer_value_from_7e3;
        break;
    }
    float exponent = 1.0f / (k_gamma_setting_scales[m_setting] * m_gamma);
    real_rgb_color black_point = {m_black.red + k_gamma_setting_black_offsets[m_setting],
        m_black.green + k_gamma_setting_black_offsets[m_setting], m_black.blue + k_gamma_setting_black_offsets[m_setting]};
    real_rgb_color range = { m_white.red - black_point.red, m_white.green - black_point.green, m_white.blue - black_point.blue };
    ramp.blue[0].base = 0;
    ramp.green[0].base = 0;
    ramp.red[0].base = 0;
    for (long index = 0; index < 128; index++)
    {
        float value = get_linear_buffer_value(index << 3, 1023, m_brightness);
        real_rgb_color color = { range.red * value, range.green * value, range.blue * value };
        color.red += black_point.red;
        color.green += black_point.green;
        color.blue += black_point.blue;
        real_rgb_color output;
        output.red = (float)pow((double)PIN(color.red, 0.0f, 1.0f), (double)exponent);
        output.green = (float)pow((double)PIN(color.green, 0.0f, 1.0f), (double)exponent);
        output.blue = (float)pow((double)PIN(color.blue, 0.0f, 1.0f), (double)exponent);
        ramp.red[index].base = (unsigned short)PIN((long)(output.red * 65535.0f), 0, 65535);
        ramp.green[index].base = (unsigned short)PIN((long)(output.green * 65535.0f), 0, 65535);
        ramp.blue[index].base = (unsigned short)PIN((long)(output.blue * 65535.0f), 0, 65535);
        if (index > 0)
        {
            ramp.red[index - 1].delta = ramp.red[index].base - ramp.red[index - 1].base;
            ramp.green[index - 1].delta = ramp.green[index].base - ramp.green[index - 1].base;
            ramp.blue[index - 1].delta = ramp.blue[index].base - ramp.blue[index - 1].base;
        }
    }
    ramp.red[127].delta = 65535 - ramp.red[127].base;
    ramp.green[127].delta = 65535 - ramp.red[127].base;
    ramp.blue[127].delta = 65535 - ramp.red[127].base;
}
