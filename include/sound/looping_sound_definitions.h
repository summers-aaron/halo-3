#ifndef SOUND_LOOPING_SOUND_DEFINITIONS_H
#define SOUND_LOOPING_SOUND_DEFINITIONS_H
#include <stddef.h>
#include "tag_files/tag_types.h"
#include "sound/game_sound_definitions.h"
struct looping_sound_definition
{
    unsigned long flags;
    float fade_in_time;
    float music_time_upper_bound;
    float minimum_distance;
    float maximum_distance;
    s_tag_reference unused_sound;
    short entry_index;
    s_tag_block tracks;
    s_tag_block details;
};
struct looping_sound_track_definition
{
    long name;
    unsigned long flags;
    union { real_decibel gain; long gain_bits; };
    float fade_in_seconds, fade_out_seconds;
    s_tag_reference start, loop, end, alternate_loop, alternate_end;
    unsigned short effect;
    s_tag_reference alternate_start, alternate_transition;
    float crossfade_seconds, alternate_fade_out_seconds;
};
typedef char looping_sound_definition_tracks_offset[(offsetof(looping_sound_definition, tracks) == 0x28) ? 1 : -1];
typedef char looping_sound_definition_details_offset[(offsetof(looping_sound_definition, details) == 0x34) ? 1 : -1];
typedef char looping_sound_definition_maximum_offset[(offsetof(looping_sound_definition, maximum_distance) == 0x10) ? 1 : -1];
typedef char looping_sound_definition_class_offset[(offsetof(looping_sound_definition, entry_index) == 0x24) ? 1 : -1];
typedef char looping_sound_track_loop_offset[(offsetof(looping_sound_track_definition, loop) == 0x24) ? 1 : -1];
typedef char looping_sound_track_definition_size[(sizeof(looping_sound_track_definition) == 0x90) ? 1 : -1];
typedef char looping_sound_definition_size[(sizeof(looping_sound_definition) == 0x40) ? 1 : -1];
typedef char looping_sound_minimum_offset[(offsetof(looping_sound_definition, minimum_distance) == 0xc) ? 1 : -1];
typedef char looping_sound_unused_sound_offset[(offsetof(looping_sound_definition, unused_sound) == 0x14) ? 1 : -1];
typedef char looping_sound_track_alternate_start_offset[(offsetof(looping_sound_track_definition, alternate_start) == 0x68) ? 1 : -1];
#endif
