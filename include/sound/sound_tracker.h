#ifndef SOUND_SOUND_TRACKER_H
#define SOUND_SOUND_TRACKER_H

#include "sound/sound_event_callbacks.h"

struct s_sound_attachment_data;
struct s_cache_file_sound_permutation;
struct s_sound_output_parameters;

struct s_sound_tracker
{
    union
    {
        void *callbacks[8];
        struct
        {
            bool (*update)(long tracker_data, long definition_index, void *data, unsigned short *flags);
            void (*playing_in_channel)(long, long, long, void *);
            void (*notify)(long object_index, const s_sound_attachment_data *attachment, long sound_index, long pitch_range_index,
                const s_cache_file_sound_permutation *permutation, float time);
            bool (*update_output)(long object_index, long sound_index, const s_sound_attachment_data *attachment, s_sound_output_parameters *output);
            void (*stop)(long sound_index, long sound_event_index, e_sound_event_stop_reason reason);
        };
    };
};
typedef char sound_tracker_layout_size[(sizeof(void *) != 4 || sizeof(s_sound_tracker) == 0x20) ? 1 : -1];

#endif
