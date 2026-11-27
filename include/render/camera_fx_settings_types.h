#ifndef RENDER_CAMERA_FX_SETTINGS_TYPES_H
#define RENDER_CAMERA_FX_SETTINGS_TYPES_H
#include <stddef.h>
#include "math/real_math_types.h"
class c_exposure;
class c_camera_fx_settings {
public:
struct s_real_parameter {
    unsigned short flags;
    unsigned short reserved;
    float value;
    float max_change;
    float interpolation;

    void set(float v, float m, float i, unsigned short f)
    {
        value = v;
        max_change = m;
        interpolation = i;
        flags = f;
    }

    float calculate_new_value(float current_value, float target_value) const;
};
struct s_real_exposure_parameter : s_real_parameter {
    float minimum;
    float maximum;
    float adjustment;
    float history_seconds;
    void calculate_new_value(c_exposure &exposure, unsigned long flags, float target, float adjustment, float minimum, float maximum) const;
};
struct s_real_instant_parameter {
    unsigned short flags;
    unsigned short reserved;
    float value;
    void set(float v, unsigned short f)
    {
        value = v;
        flags = f;
    }
};
struct s_color_parameter {
    unsigned short flags;
    unsigned short reserved;
    union
    {
        real_rgb_color color;
        struct { float red, green, blue; };
    };
    void set(float r, float g, float b, unsigned short f)
    {
        red = r;
        green = g;
        blue = b;
        flags = f;
    }
};
struct s_word_parameter {
    unsigned short flags;
    unsigned short value;
    void set(unsigned short v, unsigned short f)
    {
        value = v;
        flags = f;
    }
};
s_real_exposure_parameter exposure;
s_real_instant_parameter exposure_sensitivity;
s_real_instant_parameter white_point;
s_real_parameter bloom_intensity;
s_real_parameter bloom_threshold;
s_real_parameter bloom_brightness;
s_color_parameter color_balance;
s_color_parameter color_contrast;
s_color_parameter color_saturation;
s_real_parameter parameter144;
s_real_parameter parameter160;
s_real_parameter parameter176;
s_word_parameter parameter192;
s_real_parameter exposure_offset;
s_real_parameter exposure_scale;
};
typedef char camera_fx_settings_white_point_offset[(offsetof(c_camera_fx_settings, white_point) == 40) ? 1 : -1];
typedef char camera_fx_settings_color_balance_offset[(offsetof(c_camera_fx_settings, color_balance) == 96) ? 1 : -1];
typedef char camera_fx_settings_word_parameter_offset[(offsetof(c_camera_fx_settings, parameter192) == 192) ? 1 : -1];
#endif
