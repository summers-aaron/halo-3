#ifndef SOUND_SOUND_ENVIRONMENT_PARAMETERS_H
#define SOUND_SOUND_ENVIRONMENT_PARAMETERS_H
#include <stddef.h>

struct s_sound_environment_parameters
{
    float obstruction;
    float occlusion;
    float transmission;
    float low_frequency_transmission;
    short material_index;
    short padding;
    float transmission_multiplier;
};

typedef char sound_environment_size[(sizeof(s_sound_environment_parameters) == 0x18) ? 1 : -1];
typedef char sound_environment_multiplier_offset[(offsetof(s_sound_environment_parameters, transmission_multiplier) == 0x14) ? 1 : -1];
#endif
