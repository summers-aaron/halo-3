#ifndef SOUND_SOUND_DATUM_H
#define SOUND_SOUND_DATUM_H
#include "sound/sound_source_types.h"
#include "sound/sound_event_callbacks.h"
class c_sound_datum_flags
{
public:
    unsigned short m_flags;
    __forceinline void set(long bit, bool value) { if (value) m_flags |= (1 << bit); else m_flags &= ~(1 << bit); }
    __forceinline void set_unsafe(unsigned short flags) { m_flags = flags; }
    __forceinline unsigned char test(long bit) const { unsigned char result = (m_flags >> bit) & 1; return result; }
};
struct sound_datum
{
    short identifier;
    signed char type;
    unsigned char synchronized_playback;
    c_sound_datum_flags flags;
    unsigned short playback_flags;
    long playback_index;
    long definition_index;
    long object_index;
    const s_sound_event_callbacks *callbacks;
    s_sound_source source;
#ifdef HALO3_64BIT
    __declspec(align(8)) char event_parameters[56];
#else
    char event_parameters[48];
#endif
    long start_time;
    float pitch;
    real_decibel gain;
    unsigned long random_seed;
    signed char pitch_range_index;
    signed char permutation_index;
    short chunk_index;
    signed char priority;
    signed char listener_index;
    signed char hardware_voice_index;
    signed char voice_state;
    long playback_controller_index;
    short channel_index;
    signed char fade_type;
    char stop_reason;
    real_decibel fade_gain_db;
    long fade_start_time;
    long fade_end_time;
};

#endif
