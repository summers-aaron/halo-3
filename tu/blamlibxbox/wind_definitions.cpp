// blamlibxbox/wind_definitions.obj: from the records 0x57ed30
// @flags /TP /O2 /Oc


#include <math.h>
// @address 0x81dc34 ?g_wind_values@@3Us_wind_values@@A
#include "scenario/wind_definitions.h"
#include "main/game_tick_time_samples.h"
// @address 0x58968 k_wind_texture_offset_scale
// @rdata 0x82058968..0x8205896c
extern "C" {
const float k_wind_texture_offset_scale = 1048576.0f;
}
// @rdata-end
// @address 0x5896c k_wind_speed_to_feet_per_second
// @rdata 0x8205896c..0x82058970
extern "C" {
const float k_wind_speed_to_feet_per_second = 0.14666666f;
}
// @rdata-end
// @address 0x57ed30 ?update@s_wind_definition@@QBAXPAUs_wind_values@@@Z
void s_wind_definition::update(s_wind_values *values) const
{
    s_wind_values *state = &g_wind_values;
    float elapsed = tls_game_tick_time_samples()->total;
    float advance = elapsed * 65536.0f;
    state->fractional_seconds = (long)((float)state->fractional_seconds + advance);
    if (state->fractional_seconds > 65536)
    {
        long carry = state->fractional_seconds / 65536;
        state->fractional_seconds %= 65536;
        state->seconds += carry;
    }
    float fraction = (float)state->fractional_seconds * (1.0f / 65536.0f);
    float time = (float)state->seconds + fraction;
    const c_function_definition *direction_function = &direction;
    float direction_degrees = 0.0f;
    if (direction_function->m_data.address != 0 && direction_function->m_data.size > 0)
        direction_degrees = direction_function->map_to_output_range_legacy(direction_function->evaluate_legacy(time, 0.5f));
    float angle = direction_degrees * 0.017453292f;
    const c_function_definition *speed_function = &speed;
    float wind_speed = 0.0f;
    if (speed_function->m_data.address != 0 && speed_function->m_data.size > 0)
        wind_speed = speed_function->map_to_output_range_legacy(speed_function->evaluate_legacy(time, 0.5f));
    float sine = (float)sin(angle);
    float cosine = (float)cos(angle);
    float feet_per_second = wind_speed * k_wind_speed_to_feet_per_second;
    float input = feet_per_second * 0.005f;
    const c_function_definition *bend_function = &bend;
    float bend_value = 0.0f;
    if (bend_function->m_data.address != 0 && bend_function->m_data.size > 0)
        bend_value = bend_function->map_to_output_range_legacy(bend_function->evaluate_legacy(input, 0.5f));
    const c_function_definition *turbulence_function = &turbulence;
    float turbulence_value = 0.0f;
    if (turbulence_function->m_data.address != 0 && turbulence_function->m_data.size > 0)
        turbulence_value = turbulence_function->map_to_output_range_legacy(turbulence_function->evaluate_legacy(input, 0.5f));
    const c_function_definition *texture_function = &texture_speed;
    float texture_value = 0.0f;
    if (texture_function->m_data.address != 0 && texture_function->m_data.size > 0)
        texture_value = texture_function->map_to_output_range_legacy(texture_function->evaluate_legacy(input, 0.5f));
    float scroll_x = (texture_value * sine) * elapsed;
    float scroll_y = (texture_value * cosine) * elapsed;
    state->offset_x = (long)((float)state->offset_x + scroll_x * k_wind_texture_offset_scale);
    state->offset_y = (long)((float)state->offset_y + scroll_y * k_wind_texture_offset_scale);
    state->offset_x &= 0xFFFFF;
    state->offset_y &= 0xFFFFF;
    float texture_x = (float)state->offset_x * (1.0f / 1048576.0f);
    float texture_y = (float)state->offset_y * (1.0f / 1048576.0f);
    state->texture_x = texture_x;
    state->texture_y = texture_y;
    float inverse_frequency = 1.0f / frequency;
    state->frequency = inverse_frequency;
    float zero = 0.0f;
    state->zero = zero;
    float velocity_x = bend_value * sine;
    float velocity_y = bend_value * cosine;
    state->velocity_x = velocity_x;
    state->velocity_y = velocity_y;
    state->turbulence = turbulence_value;
    state->time = time;
}
