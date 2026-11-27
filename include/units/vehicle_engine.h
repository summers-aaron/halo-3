#ifndef UNITS_VEHICLE_ENGINE_H
#define UNITS_VEHICLE_ENGINE_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// Note: "toruqe" is the established spelling used by this interface; keep it as is.
struct s_toruqe_curve_definition
{
    float start;
    float peak;
    float peak_rpm_fraction;
    float exponent;
    float end;
    float overrun;
};

struct s_vehicle_gear_definition
{
    s_toruqe_curve_definition curves[2];
    float up_time;
    float up_scale;
    float ratio;
    float down_time;
    float down_scale;
};

struct s_vehicle_engine_definition
{
    float engine_moment;
    float maximum_rpm;
    s_tag_block gears;
    s_tag_reference shift_sound;
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char vehicle_torque_curve_size[(sizeof(s_toruqe_curve_definition) == 0x18) ? 1 : -1];
typedef char vehicle_gear_definition_size[(sizeof(s_vehicle_gear_definition) == 0x44) ? 1 : -1];
typedef char vehicle_gear_ratio_offset[(offsetof(s_vehicle_gear_definition, ratio) == 0x38) ? 1 : -1];
typedef char vehicle_engine_definition_size[(sizeof(s_vehicle_engine_definition) == 0x24) ? 1 : -1];
typedef char vehicle_engine_gears_offset[(offsetof(s_vehicle_engine_definition, gears) == 8) ? 1 : -1];
typedef char vehicle_engine_shift_sound_offset[(offsetof(s_vehicle_engine_definition, shift_sound) == 0x14) ? 1 : -1];
#endif

// March vehicle_engine_reset clears 20 bytes and initializes gear at 0xc and desired_gear at 0xd.
// The leading words hold throttle, rpm and load torque, followed by gears and shift timing counters.
struct s_vehicle_engine
{
    float throttle;
    float rpm;
    float load_torque;
    signed char gear;
    signed char desired_gear;
    unsigned char fade_ticks;
    unsigned char clutch_in_ticks;
    unsigned char idle_ticks;
    unsigned char up_ticks;
    unsigned char down_ticks;
};

typedef char vehicle_engine_size[(sizeof(s_vehicle_engine) == 0x14) ? 1 : -1];
typedef char vehicle_engine_gear_offset[(offsetof(s_vehicle_engine, gear) == 0xc) ? 1 : -1];
typedef char vehicle_engine_fade_ticks_offset[(offsetof(s_vehicle_engine, fade_ticks) == 0xe) ? 1 : -1];
typedef char vehicle_engine_up_ticks_offset[(offsetof(s_vehicle_engine, up_ticks) == 0x11) ? 1 : -1];

// Functions defined in tu/objectslibxbox/vehicle_engine.cpp that other units call.

struct s_vehicle_engine;
struct s_vehicle_engine_definition;

float vehicle_engine_get_rpm_function_scale(const s_vehicle_engine *fade);
short vehicle_engine_get_desired_gear(const s_vehicle_engine *engine,const s_vehicle_engine_definition *engine_definition,float throttle,bool braking,
    bool airborne,bool locked);
void vehicle_engine_update(long object_index, s_vehicle_engine *engine, const s_vehicle_engine_definition *engine_definition,
    float throttle, short requested_gear, bool clutch_disengaged);
float vehicle_get_torque_from_torque_curve_definition(const s_vehicle_engine_definition *engine, float rpm, const s_toruqe_curve_definition *curve);

#endif // UNITS_VEHICLE_ENGINE_H
