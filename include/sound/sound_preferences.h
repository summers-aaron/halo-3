#ifndef SOUND_SOUND_PREFERENCES_H
#define SOUND_SOUND_PREFERENCES_H

#include <stddef.h>

struct s_sound_preferences
{
    short source_counts[5];
    short channel_counts[5];
    short voice_count;
    float speed_of_sound;
};

typedef char sound_preferences_size[(sizeof(s_sound_preferences) == 0x1c) ? 1 : -1];
typedef char sound_preferences_channel_counts_offset[(offsetof(s_sound_preferences, channel_counts) == 0xa) ? 1 : -1];
typedef char sound_preferences_voice_count_offset[(offsetof(s_sound_preferences, voice_count) == 0x14) ? 1 : -1];
typedef char sound_preferences_speed_of_sound_offset[(offsetof(s_sound_preferences, speed_of_sound) == 0x18) ? 1 : -1];

#endif // SOUND_SOUND_PREFERENCES_H
