#ifndef EFFECTS_EFFECT_LIGHTPROBE_SLOT_DATA_H
#define EFFECTS_EFFECT_LIGHTPROBE_SLOT_DATA_H

#include "cseries/cseries_wrapped.h"
#include "geometry/geometry_sampling_types.h"

struct s_data_array;
extern "C" {
extern __declspec(thread) s_data_array *g_effect_lightprobe_slot_data;
}

struct s_effect_lightprobe {
    c_flags_no_init<long, unsigned char, 2> flags;
    s_geometry_sample sample;
};

#endif // EFFECTS_EFFECT_LIGHTPROBE_SLOT_DATA_H
