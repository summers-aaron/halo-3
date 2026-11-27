#ifndef CUTSCENE_RECORDED_ANIMATION_PLAYBACK_H
#define CUTSCENE_RECORDED_ANIMATION_PLAYBACK_H

// Functions defined in tu/blamlibxbox/recorded_animation_playback.cpp.

#include "math/real_math_types.h"

#pragma pack(push, 1)
struct animation_event_header
{
    unsigned char type;
    union
    {
        unsigned char byte_ticks;
        unsigned short short_ticks;
    };
};
#pragma pack(pop)

struct recorded_animation_throttle_payload
{
    real_vector2d throttle;
};
struct direction_playback_controller { short yaw, pitch; };
struct animation_playback_controller { direction_playback_controller pair[3]; };
struct s_recorded_delta;
struct s_recorded_short_delta;
struct s_unit_control;
struct unit_control_data;

extern "C" {
typedef void (*recorded_animation_apply_function)(animation_playback_controller *, unit_control_data *,
    const animation_event_header *, const char **);
extern recorded_animation_apply_function k_recorded_animation_apply_functions[];
}

extern "C" void recorded_animation_initialize_event_stream(s_unit_control *out,
    unit_control_data *unit_control, const char **stream, unsigned char version);
extern "C" bool recorded_animation_apply_event_stream(animation_playback_controller *controller,
    unit_control_data *control, long *ticks, const char **playback_stream);
extern "C" void apply_vector_char_difference(animation_playback_controller *controller, unit_control_data *control, 
    const animation_event_header *event, s_recorded_delta **stream);
extern "C" void apply_vector_short_difference(animation_playback_controller *controller, unit_control_data *control, 
    const animation_event_header *event, s_recorded_short_delta **stream);

#endif // CUTSCENE_RECORDED_ANIMATION_PLAYBACK_H
