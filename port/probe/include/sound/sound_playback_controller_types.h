#ifndef SOUND_SOUND_PLAYBACK_CONTROLLER_TYPES_H
#define SOUND_SOUND_PLAYBACK_CONTROLLER_TYPES_H

#include "cseries/cseries.h"
#include <string.h>

struct s_sound_source;
class c_sound_playback_controller {
public:
    short identifier;
    unsigned char reference_count : 6;
    unsigned char flag_a : 1;
    unsigned char flag_b : 1;
    union
    {
        unsigned char flags;
        // synchronized_sound_count is the high six bits (flags >> 2), synchronized_sounds_ready bit 0 (flags & 1), the
        // unknown flag bit 1 (flags &= 0xfd); MSB-first on the console, LSB-first on x86.
        struct
        {
            // The console allocates bit-fields from the most significant bit, the PC compilers from the least:
            // synchronized_sound_count is the high six bits and synchronized_sounds_ready bit 0 on both.
#if defined(_XBOX) && !defined(__clang__)
            unsigned char synchronized_sound_count : 6;
            unsigned char unknown_flag : 1;
            unsigned char synchronized_sounds_ready : 1;
#else
            unsigned char synchronized_sounds_ready : 1;
            unsigned char unknown_flag : 1;
            unsigned char synchronized_sound_count : 6;
#endif
        };
    };
    long owner_identifier;
    unsigned long random_seed;
    union { long sources[4]; float doppler_shifts[4]; };

    __forceinline void initialize(long identifier, unsigned long seed, unsigned char flip_flop)
    {
        owner_identifier = identifier;
        random_seed = seed;
        flag_a = ~flip_flop;
        flags &= 3;
        flags |= 1;
        flags &= ~2;
        memset(sources, 0, sizeof(sources));
    }
    __forceinline bool matches_identifier(long identifier) const { return owner_identifier == identifier; }
    __forceinline bool can_start_synchronized_sounds() const
    {
        return !(flags & 1) && owner_identifier != NONE && (flags >> 2) == 0;
    }
    __forceinline bool can_restart_synchronized_sound() const
    {
        return (flags & 1) && owner_identifier != NONE && (flags >> 2) == 0;
    }
    __forceinline void on_synch_playback()
    {
        flags |= 1;
        flags |= 2;
    }

    __forceinline void add_synch_count(long count)
    {
        if (owner_identifier != -1)
            synchronized_sound_count += count;
    }
    __forceinline signed char synchronized_sound_ready(signed char synchronized_marker)
    {
        if (owner_identifier != -1)
        {
            if (synchronized_marker == -1)
            {
                synchronized_marker = synchronized_sound_count;
                synchronized_sound_count--;
                synchronized_sounds_ready = false;
            }
        }
        return synchronized_marker;
    }
    __forceinline void synchronized_sound_ready()
    {
        if (owner_identifier != -1)
        {
            synchronized_sound_count--;
            synchronized_sounds_ready = false;
        }
    }

    void update_controller_source(const s_sound_source *source, bool value);
};

#endif
