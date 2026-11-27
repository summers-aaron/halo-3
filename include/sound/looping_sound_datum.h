#ifndef SOUND_LOOPING_SOUND_DATUM_H
#define SOUND_LOOPING_SOUND_DATUM_H

#include <stddef.h>
#include "cseries/cseries.h"
#include "sound/sound_source_types.h"
#include "objects/object_reference_types.h"

struct looping_sound_flags16
{
    unsigned short value;
    __forceinline unsigned char test(long bit) const { unsigned char result = (value >> bit) & 1; return result; }
    __forceinline bool test_bool(long bit) const { return (value & (1UL << bit)) != 0; }
    __forceinline void set(long bit, bool on) { if (on) value |= 1UL << bit; else value &= ~(1UL << bit); }
};

struct looping_sound_flags8
{
    unsigned char value;
    __forceinline unsigned char test(long bit) const { unsigned char result = (value >> bit) & 1; return result; }
};

enum e_looping_sound_playback_state
{
    _looping_sound_playback_start,
    _looping_sound_playback_loop,
    _looping_sound_playback_end,
    _looping_sound_playback_alternate_start,
    _looping_sound_playback_alternate_loop,
    _looping_sound_playback_alternate_transition,
    _looping_sound_playback_alternate_end
};
struct looping_sound_track_datum
{
    long sound;
    unsigned long range_permutation_flags;
    unsigned long random_seed;
    c_enum<e_looping_sound_playback_state, char, 0, 7> test_kind;
    c_enum<e_looping_sound_playback_state, char, 0, 7> check_kind;
    unsigned char pending_count;
    signed char fading;
    float pitch;
    float gains[4];
};
typedef looping_sound_track_datum looping_sound_track;
STATIC_ASSERT(sizeof(looping_sound_track_datum) == 0x24, looping_sound_track_datum_size);
STATIC_ASSERT(offsetof(looping_sound_track_datum, test_kind) == 0xc, looping_sound_track_current_offset);
STATIC_ASSERT(offsetof(looping_sound_track_datum, check_kind) == 0xd, looping_sound_track_queued_offset);

struct looping_sound_datum                    // 0x124 bytes
{
    short salt;
    unsigned short update_flags;
    long definition_index;
    long sound_identifier;
    long owner;
    looping_sound_flags16 flags;
    unsigned char volume_type, location;
    float volume;
    real_decibel gain_offset;
    real_point3d position;
    real_quaternion orientation;
    real_vector3d velocity;
    s_location bsp_location;
    float length;
    float minimum, maximum;
    short inner, outer;
    real_decibel outer_cone_gain;
    looping_sound_flags8 state_flags;
    signed char components;
    unsigned char state;
    unsigned char flip_flop;
    long controller;
    long detail_times[12];
    looping_sound_track tracks[4];
public:
  __forceinline bool wait_until_components_finish() const { return state_flags.test(1) != 0; }
public:
  __forceinline bool can_play_details() const { return state_flags.test(2) == 0; }
public:
  __forceinline bool alternate() const { return state_flags.test(0) != 0; }
};




STATIC_ASSERT(offsetof(looping_sound_datum, update_flags) == 2, looping_sound_update_flags_offset);
STATIC_ASSERT(offsetof(looping_sound_datum, length) == 0x48, looping_sound_length_offset);
STATIC_ASSERT(sizeof(looping_sound_datum) == 0x124, looping_sound_runtime_datum_size);

#endif
