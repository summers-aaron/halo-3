// blamlibxbox/recorded_animation_playback_v1.obj: from the records 0x2b9e88 0x2b9ea8 0x2b9ec8 0x2b9ee0 0x2b9ef8 0x2b9f10 0x2b9f40 0x2b9f70 0x2b9fa0 0x2b9fd0 0x2ba068 0x2ba100 0x2ba110
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cutscene/recorded_animation_initialize.h"
#include "cutscene/recorded_animation_playback_v1.h"
#include "math/real_math.h"
#include "units/unit_control.h"

// @address 0x2b9e88 ?apply_animation_seat@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_animation_seat(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    control->animation_state = ((const recorded_seat_event *)event)->seat;
    *stream += 6;
}

// @address 0x2b9ea8 ?apply_aiming_speed@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_aiming_speed(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    control->aiming_speed = ((const recorded_aiming_speed_event *)event)->speed;
    *stream += 6;
}

// @address 0x2b9ec8 ?apply_control_flags@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_control_flags(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    control->control_flags = ((const recorded_flags_event *)event)->flags;
    *stream += 6;
}

// @address 0x2b9ee0 ?apply_primary_weapon_index@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_primary_weapon_index(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    control->weapon_set_indices[0] = ((const recorded_weapon_event *)event)->index;
    *stream += 6;
}

// @address 0x2b9ef8 ?apply_secondary_weapon_index@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_secondary_weapon_index(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    control->weapon_set_indices[1] = ((const recorded_weapon_event *)event)->index;
    *stream += 6;
}

// @address 0x2b9f10 ?apply_throttle@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_throttle(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    const char *next = *stream;
    control->throttle.i = ((const recorded_throttle_event *)event)->throttle.i;
    control->throttle.j = ((const recorded_throttle_event *)event)->throttle.j;
    control->throttle.k = 0.0f;
    *stream = next + 12;
}

// @address 0x2b9f40 ?apply_facing_vector@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_facing_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    control->facing_vector = ((const recorded_vector_event *)event)->vector;
    *stream += 16;
}

// @address 0x2b9f70 ?apply_aiming_vector@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_aiming_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    control->aiming_vector = ((const recorded_vector_event *)event)->vector;
    *stream += 16;
}

// @address 0x2b9fa0 ?apply_looking_vector@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_looking_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    control->looking_vector = ((const recorded_vector_event *)event)->vector;
    *stream += 16;
}

// @address 0x2b9fd0 ?apply_angle_vector@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_angle_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    real_vector3d vector;
    vector3d_from_euler_angles2d(&vector, &((const recorded_angles_event *)event)->angles);
    if (((const recorded_angles_event *)event)->type != 21) control->facing_vector = vector;
    if (((const recorded_angles_event *)event)->type != 20) control->aiming_vector = vector;
    if (((const recorded_angles_event *)event)->type != 19) control->looking_vector = vector;
    *stream += sizeof(recorded_angles_event);
}

// @address 0x2ba068 ?apply_multi_vector@@YAXPAUunit_control_data@@PBUanimation_event_v1@@PAPBD@Z
void apply_multi_vector(unit_control_data *control, const animation_event_v1 *event, const char **stream)
{
    if (((const recorded_vector_event *)event)->type != 14)
        control->facing_vector = ((const recorded_vector_event *)event)->vector;
    if (((const recorded_vector_event *)event)->type != 13)
        control->aiming_vector = ((const recorded_vector_event *)event)->vector;
    if (((const recorded_vector_event *)event)->type != 12)
        control->looking_vector = ((const recorded_vector_event *)event)->vector;
    *stream += sizeof(recorded_vector_event);
}

// @address 0x2ba100 ?recorded_animation_initialize_event_stream_v1@@YAXPAUanimation_playback_controller@@PAUunit_control_data@@PAPBDE@Z
void recorded_animation_initialize_event_stream_v1(
    animation_playback_controller *controller, unit_control_data *control, const char **playback_stream, unsigned char version)
{
    recorded_animation_initialize_unit_control(control, playback_stream, version);
}

// @address 0x2ba110 ?recorded_animation_apply_event_stream_v1@@YA_NPAUanimation_playback_controller@@PAUunit_control_data@@PAJPAPBD@Z
bool recorded_animation_apply_event_stream_v1(
    animation_playback_controller *controller, unit_control_data *control, long *ticks, const char **playback_stream)
{
    bool result = true;
    const animation_event_v1 *event = (const animation_event_v1 *)*playback_stream;
    while (*ticks >= event->ticks && event->type != 1) {
        if (k_recorded_animation_apply_functions_v1[event->type])
            k_recorded_animation_apply_functions_v1[event->type](control, event, playback_stream);
        else
            *playback_stream += sizeof(animation_event_v1);
        *ticks -= event->ticks;
        event = (const animation_event_v1 *)*playback_stream;
    }
    result = event->type != 1 || *ticks != event->ticks;
    return result;
}

extern "C" {
// @address 0x73fe50 k_recorded_animation_apply_functions_v1
animation_event_handler_v1 k_recorded_animation_apply_functions_v1[24] =
{
    0, 0, apply_animation_seat, apply_aiming_speed, apply_control_flags, apply_primary_weapon_index,
    apply_throttle, 0, 0, apply_facing_vector, apply_aiming_vector, apply_looking_vector,
    apply_multi_vector, apply_multi_vector, apply_multi_vector, apply_multi_vector,
    apply_secondary_weapon_index,
    apply_angle_vector, apply_angle_vector, apply_angle_vector,
    apply_angle_vector, apply_angle_vector, apply_angle_vector, apply_angle_vector
};
}
