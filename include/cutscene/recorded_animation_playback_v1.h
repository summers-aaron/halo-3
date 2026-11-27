#ifndef CUTSCENE_RECORDED_ANIMATION_PLAYBACK_V1_H
#define CUTSCENE_RECORDED_ANIMATION_PLAYBACK_V1_H

#include "math/real_math_types.h"

struct unit_control_data;
struct animation_playback_controller;

struct animation_event_v1
{
    short type;
    unsigned short ticks;
};
typedef void (*animation_event_handler_v1)(unit_control_data *, const animation_event_v1 *, const char **);

struct recorded_seat_event { short type; unsigned short ticks; signed char seat; };
struct recorded_aiming_speed_event { short type; unsigned short ticks; signed char speed; };
struct recorded_flags_event { short type; unsigned short ticks; unsigned short flags; };
struct recorded_weapon_event { short type; unsigned short ticks; unsigned short index; };
struct recorded_throttle_event { short type; unsigned short ticks; real_vector2d throttle; };
struct recorded_angles_event { unsigned short type; unsigned short ticks; real_euler_angles2d angles; };
struct recorded_vector_event { unsigned short type; unsigned short ticks; real_vector3d vector; };

typedef char recorded_animation_event_v1_size[(sizeof(animation_event_v1) == 4) ? 1 : -1];
typedef char recorded_animation_vector_event_size[(sizeof(recorded_vector_event) == 16) ? 1 : -1];
typedef char recorded_animation_angles_event_size[(sizeof(recorded_angles_event) == 12) ? 1 : -1];

extern "C" {
extern animation_event_handler_v1 k_recorded_animation_apply_functions_v1[];
}

void apply_animation_seat(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_aiming_speed(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_control_flags(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_primary_weapon_index(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_secondary_weapon_index(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_throttle(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_facing_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_aiming_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_looking_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_angle_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void apply_multi_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream);
void recorded_animation_initialize_event_stream_v1(
    animation_playback_controller *controller, unit_control_data *control, const char **stream, unsigned char version);
bool recorded_animation_apply_event_stream_v1(
    animation_playback_controller *controller, unit_control_data *control, long *ticks, const char **stream);

#endif // CUTSCENE_RECORDED_ANIMATION_PLAYBACK_V1_H
