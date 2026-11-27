// blamlibxbox/animation_interpolation.obj: from the records 0xd63f8 0x10fb00 0x11a2e8
// @flags /TP /O2 /Oc
#include <math.h>
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "memory/tls_globals.h"
#include "animations/animation_interpolation.h"


#include "cseries/cseries.h"

// @address 0x2aee0 k_standard_interpolation_time
#if !defined(_XBOX) || defined(__clang__)
const float k_standard_interpolation_time = 0.26666668f;
#endif
static __forceinline bool interpolator_enabled(const c_interpolator_control *control)
{
    return control->m_duration != 0.0f;
}

static __forceinline float ease_in_ease_out(float ratio)
{
    float result = 3.0f * ratio * ratio - 2.0f * ratio * ratio * ratio;
    return PIN(result, 0.0f, 1.0f);
}

static __forceinline float ease_in_ease_out_heavy(float ratio)
{
    float result = 6.0f * ratio * ratio * ratio * ratio * ratio
        - 15.0f * ratio * ratio * ratio * ratio + 10.0f * ratio * ratio * ratio;
    return PIN(result, 0.0f, 1.0f);
}

bool c_interpolator_control::update()
{
    bool result = false;
    if (enabled() && !finished()) {
        if ((m_flags & 1) != 0) {
            m_time += game_tick_length();
            if (m_time >= m_duration - 0.0001f) {
                m_time = m_duration;
                m_flags |= 2;
            }
        } else {
            m_time -= game_tick_length();
            if (m_time <= 0.0001f) {
                m_time = 0.0f;
                m_flags |= 2;
            }
        }
        result = true;
    }
    return result;
}

float c_interpolator_control::get_ratio() const
{
    float ratio = 0.0f;
    if (interpolator_enabled(this))
    {
        ratio = m_duration > 0.0001f ? m_time / m_duration : 0.0f;
        switch (m_interpolation_type)
        {
        case 1:
            ratio = ease_in_ease_out(ratio);
            break;
        case 2:
            ratio = ease_in_ease_out_heavy(ratio);
            break;
        case 3:
            ratio = 1.0f - (ratio - 1.0f) * (ratio - 1.0f);
            break;
        case 4:
            ratio = 1.0f - (1.0f - ratio) * (1.0f - ratio) * (1.0f - ratio);
            break;
        }
        ratio = PIN(ratio, 0.0f, 1.0f);
    }
    return ratio;
}
static __forceinline bool interpolator_interpolating_on(const c_interpolator_control *control)
{
    return interpolator_enabled(control) && (control->m_flags & 1) != 0;
}
static __forceinline void interpolator_disable(c_interpolator_control *control)
{
    control->m_time = 0.0f;
    control->m_duration = 0.0f;
    control->m_flags = 0;
}

void c_interpolator_control::set_status(bool on, float duration)
{
    c_interpolator_control *control = this;
    if (duration > 0.0001f)
    {
        if (!interpolator_enabled(control) || on != interpolator_interpolating_on(control) || fabsf(duration - control->m_duration) >= 0.0001f)
        {
            float elapsed = on ? 0.0f : duration;
            if (interpolator_enabled(control))
                elapsed = duration * control->get_ratio();
            control->m_time = elapsed;
            control->m_duration = duration;
            control->m_flags = (on != 0);
            if (!control->update())
                interpolator_disable(control);
        }
    }
    else
    {
        interpolator_disable(control);
    }
}
