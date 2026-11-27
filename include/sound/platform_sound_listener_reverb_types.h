#ifndef SOUND_PLATFORM_SOUND_LISTENER_REVERB_TYPES_H
#define SOUND_PLATFORM_SOUND_LISTENER_REVERB_TYPES_H

#include <stddef.h>
#include "sound/sound_manager_types.h"

// March update_reverb compares and copies exactly 72 bytes per sound_environment.
struct sound_environment {
    unsigned int : 32;
    short priority;
    unsigned short : 16;
    float room, room_hf, room_rolloff, decay_time, decay_hf_ratio;
    float reflections, reflections_delay, reverb, reverb_delay, diffusion, density, hf_reference;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
};

typedef char sound_environment_size_check[(sizeof(sound_environment) == 0x48) ? 1 : -1];
typedef char sound_environment_priority_offset[(offsetof(sound_environment, priority) == 0x4) ? 1 : -1];
typedef char sound_environment_room_offset[(offsetof(sound_environment, room) == 0x8) ? 1 : -1];

struct s_platform_sound_listener_reverb
{
    const sound_environment *environment;
    s_sound_azimuth_image azimuth_image;
    float wet_gain;
};
#endif
