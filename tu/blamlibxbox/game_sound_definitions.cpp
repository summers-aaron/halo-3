// blamlibxbox/game_sound_definitions.obj: from the records 0xf9528 0xf9790
// @flags /TP /O2 /Oc
#include <math.h>

#include "sound/game_sound_definitions.h"
static inline bool is_negative_gain(const real_decibel &a) { return a.value < 0.0f; }
static const real_decibel k_minimum_gain_db = { -64.0f };

// @address 0xf9528 ?real_gain_to_decibels@@YA?AUreal_decibel@@M@Z
real_decibel real_gain_to_decibels(float gain)
{
    float clamped_gain = ((gain > 0.0f ? gain : 0.0f) < 1.0f)
        ? (gain > 0.0f ? gain : 0.0f) : 1.0f;
    float decibels;
    if (gain > 0.0f)
    {
        decibels = 20.0f * log10(clamped_gain);
        decibels = ((decibels > -64.0f ? decibels : -64.0f) < 0.0f)
            ? (decibels > -64.0f ? decibels : -64.0f) : 0.0f;
    }
    else
        decibels = -64.0f;
    real_decibel result;
    result.value = decibels;
    return pin_gain_db(result);
}

real_decibel pin_gain_db(real_decibel gain_db)
{
    const real_decibel *lower_clamped_gain = (gain_db > k_minimum_gain_db) ? &gain_db : &k_minimum_gain_db;
    if (is_negative_gain(*lower_clamped_gain)) {
        return (gain_db > k_minimum_gain_db) ? gain_db : k_minimum_gain_db;
    }
    union { int bits; real_decibel decibels; } zero_gain;
    zero_gain.bits = 0;
    return zero_gain.decibels;
}
