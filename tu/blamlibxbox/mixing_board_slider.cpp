// blamlibxbox/mixing_board_slider.obj: from the records 0xd7278 0xdbed0 0xdbf90 0xf0868 0x2fd460 0x708ff8 0x70c110
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "cseries/cseries.h"
// @address 0xdbed0 ?move_to_target@c_mixing_board_slider@@QAAXW4e_slider_target_positions@@W4e_slider_movement_pattern@@M@Z
// @address 0xdbf90 ?move_to_target@c_mixing_board_slider@@QAAXW4e_slider_target_positions@@W4e_slider_movement_pattern@@V?$t_short_fixed@$07@@@Z
// @address 0xf0868 ?export_real@?$t_short_fixed@$07@@QBAMXZ
// @address 0x2fd460 ?snap_to_position@c_mixing_board_slider@@QAAXM@Z

#include "animations/mixing_board/mixing_board_slider.h"
#include "units/slider_movement_pattern_types.h"
#include "memory/tls_globals.h"
#include "units/slider_target_position_types.h"
#include "animations/mixing_board/mixing_board_slider_types.h"


template<typename t_type> inline t_type const &mixing_board_slider_min(t_type const &a, t_type const &b)
{
    return a > b ? b : a;
}
inline float clamp_to_unit_space(float value)
{
    return MIN(MAX(value, 0.0f), 1.0f);
}

// 0xdbed0
void c_mixing_board_slider::move_to_target(e_slider_target_positions target_position, e_slider_movement_pattern movement_pattern, float duration)
{
    t_short_fixed<8> scaled;
    movement_pattern = (e_slider_movement_pattern)PIN(movement_pattern, 0, 6);
    duration = PIN(duration, 0.0f, 127.0f);
    if ((g_short_fixed_8_scale_initialized & 1) == 0)
    {
        g_short_fixed_8_scale_initialized |= 1;
        g_short_fixed_8_scale = literal_256_0f;
    }
    scaled.m_value = (short)(g_short_fixed_8_scale * duration);
    move_to_target(target_position, movement_pattern, scaled);
}

// 0xdbf90
void c_mixing_board_slider::move_to_target(e_slider_target_positions target_position, e_slider_movement_pattern movement_pattern, t_short_fixed<8> duration)
{
    c_mixing_board_slider *slider = this;
    movement_pattern = (e_slider_movement_pattern)PIN(movement_pattern, 0, 6);
    if (target_position == slider->m_target_position && movement_pattern == slider->m_movement_pattern)
    {
        slider->m_initial_velocity = slider->m_initial_velocity > duration ? duration : slider->m_initial_velocity;
    }
    else
    {
        slider->m_target_position = target_position;
        slider->m_movement_pattern = movement_pattern;
        slider->m_velocity = duration;
        slider->m_initial_velocity = duration;
        slider->m_start_position = slider->m_position;
    }
}

// 0xf0868
template float t_short_fixed<8>::export_real() const;

void c_mixing_board_slider::snap_to_position(float position)
{
    freeze();
    position = clamp_to_unit_space(position);
    m_position.import_real(position);
}

// @address 0x79d7dc k_mixing_board_slider_targets
// @address 0x708ff8 ??__Ek_mixing_board_slider_targets@@YAXXZ

static __forceinline float mixing_board_slider_seconds_per_tick()
{
    return tls_game_time_globals()->tick_length;
}

static __forceinline void mixing_board_slider_import_duration(t_short_fixed<8> *duration, float value)
{
    if ((g_short_fixed_8_scale_initialized & 1) == 0)
    {
        g_short_fixed_8_scale_initialized |= 1;
        g_short_fixed_8_scale = literal_256_0f;
    }
    duration->m_value = (short)(g_short_fixed_8_scale * value);
}

// match: 0xd7278: precise keeps the case-1..6 easing fmuls+fadds chains split (xdk c2 fuses them into fmadds)
#pragma float_control(precise, on, push)
static __forceinline float evaluate_slider_movement_pattern(float fraction, e_slider_movement_pattern movement_pattern)
{
    fraction = PIN(fraction, 0.0001f, 1.0f);
    switch (movement_pattern)
    {
    case 0:
        break;
    case 1:
        fraction = (1.0f - fraction) * fraction + fraction;
        break;
    case 2:
        fraction = (1.0f - fraction) * fraction * fraction + fraction;
        break;
    case 3:
        fraction = fraction * fraction;
        break;
    case 4:
        fraction = fraction * fraction * fraction;
        break;
    case 5:
        if (fraction < 0.5f)
        {
            float t = fraction * 2.0f;
            fraction = t * t * t * 0.5f;
        }
        else
        {
            float t = (fraction - 0.5f) * 2.0f;
            fraction = ((1.0f - t) * t + t) * 0.5f + 0.5f;
        }
        break;
    case 6:
        if (fraction < 0.5f)
        {
            float t = fraction * 2.0f;
            fraction = t * t * 0.5f;
        }
        else
        {
            float t = (fraction - 0.5f) * 2.0f;
            fraction = ((1.0f - t) * t * t + t) * 0.5f + 0.5f;
        }
        break;
    default:
        // match: 0xd7278: __assume(0) omits the movement_pattern range check before the easing jump table
        __assume(0);
    }
    return fraction;
}
// match: 0xd7278: precise keeps the case-1..6 easing fmuls+fadds chains split (xdk c2 fuses them into fmadds)
#pragma float_control(pop)


__forceinline bool c_mixing_board_slider::update()
{
    unsigned char target_position = m_target_position.m_value;
    if (target_position != _slider_target_position_none)
    {
        const t_short_fixed<13> &target_entry = k_mixing_board_slider_targets[target_position];
        t_short_fixed<13> target = target_entry;
        if (m_position.m_value != target.m_value)
        {
            if (m_velocity.m_value != 0)
            {
                t_short_fixed<8> tick;
                mixing_board_slider_import_duration(&tick, mixing_board_slider_seconds_per_tick());
                if (m_initial_velocity > tick)
                {
                    float remaining = m_initial_velocity.export_real();
                    remaining -= mixing_board_slider_seconds_per_tick();
                    float start = m_start_position.export_real();
                    float distance = target.export_real() - start;
                    float fraction = 1.0f - remaining / m_velocity.export_real();
                    fraction = evaluate_slider_movement_pattern(fraction, m_movement_pattern);
                    short old_position = m_position.m_value;
                    float position = fraction * distance + start;
                    m_position.import_real(position);
                    short start_position = m_start_position.m_value;
                    short lower = start_position > target.m_value ? target.m_value : start_position;
                    short upper = start_position > target.m_value ? start_position : target.m_value;
                    m_position.m_value = PIN(m_position.m_value, lower, upper);
                    mixing_board_slider_import_duration(&m_initial_velocity, remaining);
                    if (m_position.m_value == old_position)
                    {
                        if (m_position.m_value < target.m_value)
                            m_position.m_value++;
                        else if (m_position.m_value > target.m_value)
                            m_position.m_value--;
                        else
                            snap_to_target(m_target_position);
                    }
                }
                else
                {
                    snap_to_target((e_slider_target_positions)target_position);
                }
            }
            else
            {
                snap_to_target((e_slider_target_positions)target_position);
            }
        }
        else
        {
            freeze();
        }
    }
    return false;
}

// @address 0xd7278 mixing_board_slider_update
extern "C" bool mixing_board_slider_update(c_mixing_board_slider *slider)
{
    return slider->update();
}

extern "C" {
const t_short_fixed<13> k_mixing_board_slider_targets[4] = { 0, 0, 4096, 8192 };
}

// @address 0x332b0 literal_256_0f
// @address 0x7c888c g_short_fixed_8_scale_initialized
// @address 0x7c8890 g_short_fixed_8_scale
extern "C" {
const float literal_256_0f = 256.0f;
unsigned long g_short_fixed_8_scale_initialized;
float g_short_fixed_8_scale;
}
