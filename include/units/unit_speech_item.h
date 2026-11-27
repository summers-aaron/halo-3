#ifndef UNITS_UNIT_SPEECH_ITEM_H
#define UNITS_UNIT_SPEECH_ITEM_H

#include <stddef.h>

// Speech request carrying
// the dialogue/sound indices, delay and information packet at these offsets.
struct unit_speech_item
{
    short vocalization;
    signed char type;
    bool deterministic;
    union
    {
        unsigned short permutation;
        // Permutation split into separate pitch-range and index bytes.
        struct
        {
            unsigned char pitch_range;
            unsigned char permutation_index;
        };
    };
    short pad6;
    long dialogue_index;
    long sound_index;
    short pad10;
    short delay;
    struct s_information
    {
        short type;
        short flags;
        long subject;
        long cause;
        long target;
        long extra0;
        long extra1;
        long extra2;
        long extra3;
    } information;
};

typedef char unit_speech_item_size[(sizeof(unit_speech_item) == 0x34) ? 1 : -1];
typedef char unit_speech_item_information_offset[(offsetof(unit_speech_item, information) == 0x14) ? 1 : -1];

typedef char unit_speech_item_permutation_offset[(offsetof(unit_speech_item, permutation) == 4) ? 1 : -1];

#endif // UNITS_UNIT_SPEECH_ITEM_H
