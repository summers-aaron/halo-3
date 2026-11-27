// blamlibxbox/recorded_animation_playback.obj: from the records 0x2b9788 0x2b97a8 0x2b97c8 0x2b97e8 0x2b9808 0x2b9828 0x2b9858 0x2b98b0 0x2b9920 0x2b9aa8 0x2b9cf8 0x2b9d70
// @flags /TP /O2 /Oc
#include "math/real_math.h"
#include "cutscene/recorded_animation_initialize.h"
// @address 0x2b9788 ?apply_animation_seat@@YAXPAUanimation_playback_controller@@PAUunit_control_data@@PBUanimation_event_header@@PAPBD@Z
// @address 0x2b97a8 ?apply_aiming_speed@@YAXPAUanimation_playback_controller@@PAUunit_control_data@@PBUanimation_event_header@@PAPBD@Z
// @address 0x2b97c8 ?apply_control_flags@@YAXPAUanimation_playback_controller@@PAUunit_control_data@@PBUanimation_event_header@@PAPBD@Z
// @address 0x2b97e8 ?apply_primary_weapon_index@@YAXPAUanimation_playback_controller@@PAUunit_control_data@@PBUanimation_event_header@@PAPBD@Z
// @address 0x2b9808 ?apply_secondary_weapon_index@@YAXPAUanimation_playback_controller@@PAUunit_control_data@@PBUanimation_event_header@@PAPBD@Z
// @address 0x2b9828 ?apply_throttle@@YAXPAUanimation_playback_controller@@PAUunit_control_data@@PBUanimation_event_header@@PAPBD@Z
// @address 0x2b9858 update_controller_char
// @address 0x2b98b0 ?uncompress_vector_from_controller@@YAXPATreal_vector3d@@PBUdirection_playback_controller@@@Z
// @address 0x2b9cf8 recorded_animation_initialize_event_stream
// @address 0x2b9d70 recorded_animation_apply_event_stream
// @address 0x995c8 ?vector3d_from_euler_angles2d@@YAPATreal_vector3d@@PAT1@PBTreal_euler_angles2d@@@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "cutscene/recorded_animation_playback.h"


#include "units/unit_control.h"

struct s_unit_control { short s[6]; };

struct s_recorded_delta { signed char first; signed char second; };

struct s_recorded_short_delta { short first, second; };

void apply_animation_seat(animation_playback_controller *controller, unit_control_data *control, const animation_event_header *header, const char **stream)
{
    control->animation_state = *(const signed char *)*stream;
    *stream += 1;
}

void apply_aiming_speed(animation_playback_controller *controller, unit_control_data *control, const animation_event_header *header, const char **stream)
{
    control->aiming_speed = *(const signed char *)*stream;
    *stream += 1;
}

void apply_control_flags(animation_playback_controller *controller, unit_control_data *control, const animation_event_header *header, const char **stream)
{
    control->control_flags = *(const unsigned long *)*stream;
    *stream += 4;
}

void apply_primary_weapon_index(
    animation_playback_controller *controller, unit_control_data *control, const animation_event_header *header, const char **stream)
{
    control->weapon_set_indices[0] = *(const unsigned short *)*stream;
    *stream += 2;
}

void apply_secondary_weapon_index(
    animation_playback_controller *controller, unit_control_data *control, const animation_event_header *header, const char **stream)
{
    control->weapon_set_indices[1] = *(const unsigned short *)*stream;
    *stream += 2;
}

void apply_throttle(animation_playback_controller *controller, unit_control_data *control, const animation_event_header *header, const char **stream)
{
    const recorded_animation_throttle_payload *value = (const recorded_animation_throttle_payload *)*stream;
    control->throttle.i = value->throttle.i;
    control->throttle.j = value->throttle.j;
    control->throttle.k = 0.0f;
    ++value;
    *stream = (const char *)value;
}

extern "C" void update_controller_char(const s_recorded_delta *delta, direction_playback_controller *pair)
{
    pair->yaw += delta->first;
    if (pair->yaw > 1000)
        pair->yaw -= 1000;
    else if (pair->yaw < -1000)
        pair->yaw += 1000;
    pair->pitch += delta->second;
}


// 0x2b98b0: expand the recorded direction angles, then form the unit vector.
void uncompress_vector_from_controller(real_vector3d *vector, const direction_playback_controller *controller)
{
    real_euler_angles2d angles;
    angles.yaw = controller->yaw * 0.0031415927f;
    angles.pitch = controller->pitch * 0.0031415927f;
    vector3d_from_euler_angles2d(vector, &angles);
}

void recorded_animation_initialize_event_stream(s_unit_control *out,
    unit_control_data *unit_control, const char **stream, unsigned char version)
{
  recorded_animation_initialize_unit_control(unit_control, stream, version);
  *out = *(s_unit_control *)*stream;
  *stream += sizeof(s_unit_control);
}

extern "C" {
bool recorded_animation_apply_event_stream(animation_playback_controller *playback_controller,
    unit_control_data *playback_control, long *ticks, const char **playback_stream)
{
    const animation_event_header *event;
    unsigned short decoded_ticks;
    for (;;)
    {
        event = (const animation_event_header *)*playback_stream;
        unsigned char header = event->type;
        unsigned short header_size;
        switch (header >> 6)
        {
        case 0:
            decoded_ticks = 0;
            header_size = 1;
            break;
        case 1:
            decoded_ticks = 1;
            header_size = 1;
            break;
        case 2:
            decoded_ticks = event->byte_ticks;
            header_size = 2;
            break;
        case 3:
            decoded_ticks = event->short_ticks;
            header_size = 3;
            break;
        default:
            // match: 0x2b9d70: the event header has only two time encoding bits.
            __assume(0);
        }
        if (*ticks < decoded_ticks || (header & 63) == 1)
            break;
        *playback_stream = (const char *)event + header_size;
        recorded_animation_apply_function handler = k_recorded_animation_apply_functions[event->type & 63];
        if (handler)
            handler(playback_controller, playback_control, event, playback_stream);
        *ticks -= decoded_ticks;
    }
    return !((event->type & 63) == 1 && *ticks == decoded_ticks);
}
}

// @address 0x2b9920 apply_vector_char_difference
void apply_vector_char_difference(
    animation_playback_controller *controller, unit_control_data *control, const animation_event_header *event, s_recorded_delta **stream)
{
  s_recorded_delta *delta = *stream;
  short mask = (short)((event->type & 0x3f) - 7);
  if (mask & 1) {
    update_controller_char(delta, &controller->pair[0]);
    uncompress_vector_from_controller(&control->facing, &controller->pair[0]);
  }
  if (mask & 2) {
    if (mask & 1) {
      controller->pair[1] = controller->pair[0];
      control->aiming = control->facing;
    } else {
      update_controller_char(delta, &controller->pair[1]);
      uncompress_vector_from_controller(&control->aiming, &controller->pair[1]);
    }
  }
  if (mask & 4) {
    if (mask & 1) {
      controller->pair[2] = controller->pair[0];
      control->looking = control->facing;
    } else if (mask & 2) {
      controller->pair[2] = controller->pair[1];
      control->looking = control->aiming;
    } else {
      update_controller_char(delta, &controller->pair[2]);
      uncompress_vector_from_controller(&control->looking, &controller->pair[2]);
    }
  }
  (*stream)++;
}

static __forceinline void recorded_animation_playback_update_controller_short(const s_recorded_short_delta *delta, direction_playback_controller *pair)
{
  pair->yaw += delta->first;
  if (pair->yaw > 1000)
    pair->yaw -= 1000;
  else if (pair->yaw < -1000)
    pair->yaw += 1000;
  pair->pitch += delta->second;
}

// @address 0x2b9aa8 apply_vector_short_difference
void apply_vector_short_difference(animation_playback_controller *controller, unit_control_data *control,
    const animation_event_header *event, s_recorded_short_delta **stream)
{
  s_recorded_short_delta *delta = *stream;
  short mask = (short)((event->type & 0x3f) - 15);
  if (mask & 1) {
    recorded_animation_playback_update_controller_short(delta, &controller->pair[0]);
    uncompress_vector_from_controller(&control->facing, &controller->pair[0]);
  }
  if (mask & 2) {
    if (mask & 1) {
      controller->pair[1] = controller->pair[0];
      control->aiming = control->facing;
    } else {
      recorded_animation_playback_update_controller_short(delta, &controller->pair[1]);
      uncompress_vector_from_controller(&control->aiming, &controller->pair[1]);
    }
  }
  if (mask & 4) {
    if (mask & 1) {
      controller->pair[2] = controller->pair[0];
      control->looking = control->facing;
    } else if (mask & 2) {
      controller->pair[2] = controller->pair[1];
      control->looking = control->aiming;
    } else {
      recorded_animation_playback_update_controller_short(delta, &controller->pair[2]);
      uncompress_vector_from_controller(&control->looking, &controller->pair[2]);
    }
  }
  (*stream)++;
}

extern "C" {
// @address 0x73feb0 k_recorded_animation_apply_functions
recorded_animation_apply_function k_recorded_animation_apply_functions[24] =
{
    0, 0, apply_animation_seat, apply_aiming_speed, apply_control_flags, apply_primary_weapon_index,
    apply_throttle,
    (recorded_animation_apply_function)apply_vector_char_difference,
    (recorded_animation_apply_function)apply_vector_char_difference,
    (recorded_animation_apply_function)apply_vector_char_difference,
    (recorded_animation_apply_function)apply_vector_char_difference,
    (recorded_animation_apply_function)apply_vector_char_difference,
    (recorded_animation_apply_function)apply_vector_char_difference,
    (recorded_animation_apply_function)apply_vector_char_difference,
    (recorded_animation_apply_function)apply_vector_char_difference,
    (recorded_animation_apply_function)apply_vector_short_difference,
    (recorded_animation_apply_function)apply_vector_short_difference,
    (recorded_animation_apply_function)apply_vector_short_difference,
    (recorded_animation_apply_function)apply_vector_short_difference,
    (recorded_animation_apply_function)apply_vector_short_difference,
    (recorded_animation_apply_function)apply_vector_short_difference,
    (recorded_animation_apply_function)apply_vector_short_difference,
    (recorded_animation_apply_function)apply_vector_short_difference,
    apply_secondary_weapon_index
};
}
