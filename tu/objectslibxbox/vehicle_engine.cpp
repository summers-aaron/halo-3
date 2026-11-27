// objectslibxbox/vehicle_engine.obj: from the records 0x394b78 0x394c00 0x394d68 0x394f98
// @flags /TP /O2 /Oc
// @address 0x394b78 ?vehicle_engine_get_rpm_function_scale@@YAMPBUs_vehicle_engine@@@Z
// @address 0x394d68 ?vehicle_engine_get_desired_gear@@YAFPBUs_vehicle_engine@@PBUs_vehicle_engine_definition@@M_N22@Z
#include <math.h>
#include "memory/tls_globals.h"
#include "tag_files/tag_types.h"
#include "sound/game_sound.h"
#include "units/vehicle_engine.h"
#include "units/bipeds.h"
#include "math/real_math.h"
#include "game/game_time_conversion.h"

const float k_fade_duration = 0.45f;
enum
{
 _vehicle_gear_none = -2,
 _vehicle_gear_neutral = -1,
 _vehicle_gear_reverse = 0,
 _vehicle_gear_first = 1,
};

static __forceinline float fade_elapsed(const s_vehicle_engine *fade)
{
  float ticks = fade->fade_ticks;
  return ticks * tls_game_time_globals()->tick_length;
}

float vehicle_engine_get_rpm_function_scale(const s_vehicle_engine *fade)
{
  float result = k_real_one;
  if (fade_elapsed(fade) < k_fade_duration) {
    float t = fade_elapsed(fade) * (1.0f / 0.45f);
    float u;
    if (t < k_real_half)
      u = k_real_one - t * k_real_two;
    else
      u = (t - k_real_half) * k_real_two;
    result = (u + k_real_one) * k_real_half;
  }
  return result;
}

#define real_abs(x) ((x)>=0.0f ? (x) : -(x))
__forceinline float vehicle_engine_power(float x, float y) { return (float)pow(x, y); }
// @address 0x394c00 ?vehicle_get_torque_from_torque_curve_definition@@YAMPBUs_vehicle_engine_definition@@MPBUs_toruqe_curve_definition@@@Z
float vehicle_get_torque_from_torque_curve_definition(
    struct s_vehicle_engine_definition const *engine, float rpm, struct s_toruqe_curve_definition const *curve)
{
 float rpm_fraction = curve->peak_rpm_fraction > 0.001f ? curve->peak_rpm_fraction : 0.001f;
 float peak_rpm = engine->maximum_rpm * rpm_fraction;
 float result = 0.0f;
 if (rpm < peak_rpm) {
  if (peak_rpm > 0.001f) {
   float slope = (curve->peak - curve->start) / peak_rpm;
   float intercept = curve->start;
   result = slope * rpm + intercept;
  } else result = 0.0f;
 } else if (rpm >= engine->maximum_rpm) {
  float slope = (curve->overrun - curve->end) / engine->maximum_rpm;
  float intercept = curve->end - slope * engine->maximum_rpm;
  result = slope * rpm + intercept;
 } else {
  if (real_abs(engine->maximum_rpm - peak_rpm) > 0.0001f) {
   float slope = (curve->end - curve->peak) / (engine->maximum_rpm - peak_rpm);
   float intercept = curve->peak - peak_rpm * slope;
   if (real_abs(curve->peak) > 0.001f) {
    float multiplier = curve->peak;
    float fraction = (slope * rpm + intercept) / multiplier;
    if (fraction >= 0.0f && fraction <= 1.0f) fraction = vehicle_engine_power(fraction, curve->exponent);
    result = fraction * multiplier;
   } else result = 0.0f;
  } else result = 0.0f;
 }
 return result;
}

short vehicle_engine_get_desired_gear(const s_vehicle_engine *engine, const s_vehicle_engine_definition *engine_definition, float throttle,
    bool braking, bool airborne, bool locked)
{
 short desired=engine->gear;
 if (!(engine_definition->maximum_rpm<0.001f)) {
  float effective_throttle=braking ? 0.0f : throttle;
  float rpm=engine->rpm;
  float rpm_scale=PIN(rpm/engine_definition->maximum_rpm,0.0f,1.0f);
  if(effective_throttle==0.0f) {
   float neutral_delay=1.0f;
   if(rpm<0.25f*engine_definition->maximum_rpm || (!airborne && game_ticks_to_seconds((float)engine->idle_ticks)>neutral_delay)) desired=-1;
  } else if (!locked) {
   if(effective_throttle<0.0f) {
    if(!braking) desired=0;
   } else if(engine->gear<1) {
    if(!braking) desired=1;
   } else if(engine->gear>=0 && engine->gear<engine_definition->gears.count) {
    const s_vehicle_gear_definition *current_gear=&((const s_vehicle_gear_definition *)engine_definition->gears.address)[engine->gear];
    if(game_ticks_to_seconds((float)engine->up_ticks)>current_gear->up_time && effective_throttle>=rpm_scale && rpm_scale>current_gear->up_scale)
     desired=(short)PIN(engine->gear+1,1,engine_definition->gears.count-1);
    if(game_ticks_to_seconds((float)engine->down_ticks)>current_gear->down_time && rpm_scale<current_gear->down_scale)
     desired=(short)(desired<=0 ? 1 : PIN(engine->gear-1,1,engine_definition->gears.count-1));
   } else desired=-1;
  }
 }
 return desired;
}




static __forceinline float vehicle_get_gear_ratio(const s_vehicle_engine *engine, const s_vehicle_engine_definition *engine_definition, short gear_index)
{
 float ratio = 0.0f;
 if (gear_index != _vehicle_gear_neutral) {
  if (gear_index >= 0 && gear_index < engine_definition->gears.count)
   ratio = ((const s_vehicle_gear_definition *)engine_definition->gears.address)[gear_index].ratio;
 } else
  ratio = 0.0f;
 return ratio;
}

static __forceinline float vehicle_engine_get_torque(
    const s_vehicle_engine *engine, const s_vehicle_engine_definition *engine_definition, short gear_index, float rpm)
{
 float torque = 0.0f;
 if (gear_index >= 0 && gear_index < engine_definition->gears.count) {
  const s_vehicle_gear_definition *gear_definition = &((const s_vehicle_gear_definition *)engine_definition->gears.address)[gear_index];
  float throttle_torque = vehicle_get_torque_from_torque_curve_definition(engine_definition, rpm, &gear_definition->curves[0]);
  float coast_torque = vehicle_get_torque_from_torque_curve_definition(engine_definition, rpm, &gear_definition->curves[1]);
  torque = (1.0f - engine->throttle) * coast_torque + engine->throttle * throttle_torque;
 }
 return torque;
}

static __forceinline bool vehicle_engine_shift(s_vehicle_engine *engine, signed char next_gear, short requested_gear)
{
 long pg = engine->gear;
 engine->gear = next_gear;
 engine->desired_gear = _vehicle_gear_none;
 engine->idle_ticks = 0;
 engine->up_ticks = 0;
 engine->down_ticks = 0;
 engine->clutch_in_ticks = 0;
 return pg != requested_gear;
}

// @address 0x394f98 ?vehicle_engine_update@@YAXJPAUs_vehicle_engine@@PBUs_vehicle_engine_definition@@MF_N@Z
void vehicle_engine_update(long object_index, s_vehicle_engine *engine, const s_vehicle_engine_definition *engine_definition,
    float throttle, short requested_gear, bool clutch_disengaged)
{
 if (engine_definition->maximum_rpm > 0.001f) {
  if (engine->clutch_in_ticks != 0 || clutch_disengaged)
   throttle = 0.0f;
  float gear_ratio = vehicle_get_gear_ratio(engine, engine_definition, engine->gear);
  float rpm_scale = PIN(engine->rpm, -0.5f * engine_definition->maximum_rpm, 1.5f * engine_definition->maximum_rpm) / engine_definition->maximum_rpm;
  bool change_direction;
  if (requested_gear == _vehicle_gear_none || (requested_gear >= _vehicle_gear_first) == (engine->gear >= _vehicle_gear_first))
   change_direction = false;
  else
   change_direction = true;
  bool shifted = false;
  engine->throttle += (throttle - rpm_scale) * (game_tick_length() / 0.022f);
  engine->throttle = PIN(engine->throttle, 0.0f, 1.0f);
  engine->fade_ticks = MIN(engine->fade_ticks + 1, 254);

  if (change_direction || (engine->gear == _vehicle_gear_neutral && requested_gear != engine->gear)) {
   shifted = vehicle_engine_shift(engine, (signed char)requested_gear, requested_gear);
  } else if (engine->desired_gear == _vehicle_gear_none && requested_gear == engine->gear) {
   engine->clutch_in_ticks = 0;
   engine->idle_ticks = MIN(engine->idle_ticks + 1, 255);
   if (engine->gear >= 0 && engine->gear < engine_definition->gears.count) {
    const s_vehicle_gear_definition *gear_definition = &((const s_vehicle_gear_definition *)engine_definition->gears.address)[engine->gear];
    if (rpm_scale > gear_definition->up_scale)
     engine->up_ticks = MIN(engine->up_ticks + 1, 255);
    else
     engine->up_ticks = 0;
    if (rpm_scale < gear_definition->down_scale)
     engine->down_ticks = MIN(engine->down_ticks + 1, 255);
    else
     engine->down_ticks = 0;
   }
  } else {
   long clutch_shift_ticks = (long)((float)game_tick_rate() * 0.05f);
   if (clutch_shift_ticks <= 1) clutch_shift_ticks = 1;
   if (engine->clutch_in_ticks == 0) {
    engine->desired_gear = (signed char)requested_gear;
    engine->clutch_in_ticks = 1;
   } else if (engine->clutch_in_ticks == clutch_shift_ticks) {
    shifted = vehicle_engine_shift(engine, engine->desired_gear, requested_gear);
    if (game_ticks_to_seconds((float)engine->fade_ticks) > 0.45f)
     engine->fade_ticks = 0;
   } else {
    engine->clutch_in_ticks++;
   }
  }

  if (engine->gear >= 0) {
   float torque = vehicle_engine_get_torque(engine, engine_definition, engine->gear, engine->rpm);
   engine->rpm += (torque + gear_ratio * engine->load_torque) * (game_tick_length() / engine_definition->engine_moment);
   engine->rpm = PIN(engine->rpm, 0.05f * engine_definition->maximum_rpm, global_lean_pivot_offset * engine_definition->maximum_rpm);
  } else {
   engine->rpm = engine->rpm * 0.95f;
  }
  if (shifted && engine_definition->shift_sound.index != -1)
   object_impulse_sound_new(object_index, engine_definition->shift_sound.index, -1, global_origin3d, global_forward3d, 1.0f);
 }
}
