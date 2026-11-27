#ifndef SOUND_SOUND_MANAGER_TYPES_H
#define SOUND_SOUND_MANAGER_TYPES_H

#include <stddef.h>
#include "math/matrix_math_types.h"
#include "objects/object_reference_types.h"
#include "memory/static_arrays.h"

// Listener records shared by game_sound.cpp and sound_manager.cpp (s_sound_manager_globals::listeners).
// The location/valid prefix is also used by PAS and tracking.
struct s_sound_listener
{
    long header_signature; // 'lshd', set by sound_initialize
    s_location location;
    short water_material_type;
    bool valid;
    bool default_acoustics;
    real_matrix4x3 matrix;
    real_vector3d velocity;
    long footer_signature; // 'lsft'
};

// Transmission queries use a compact listener, distinct from manager records.
struct s_game_sound_transmission_listener
{
    long listener_index;
    real_point3d position;
    s_location location;
};

struct s_sound_azimuth_image
{
    real_bounds azimuth;
    float spread;
    float base_volume;
};

struct s_sound_manager_reverb
{
    float inverse_cutoff;
    float scale;
    long definition_index;
    s_sound_azimuth_image image;
};

struct s_sound_manager_reverbs
{
    s_sound_manager_reverb reverbs[2];
    long count;
};

struct s_sound_manager_environment
{
    s_sound_manager_reverb reverbs[2];
    long reverb_count;
};

// Status bitmaps and runtime reverb state are read by sound_manager.cpp.
struct s_sound_manager_globals
{
    unsigned long free_sources[7];
    long channel_type_first_source[5];
    long channel_type_last_source[5];
    unsigned long free_voices[7];
    unsigned long used_sources[7];
    unsigned long used_voices[7];
    bool initialized;
    bool enabled;
    bool active;
    bool paused;
    bool sounds_locked;
    bool updating;
    bool update_frame;
    union { bool ducking_suppressed; bool bink_playing; };
    union { unsigned char update_flags; unsigned char flip_flop; };
    long reset_time;
    long time;
    s_sound_listener listeners[4];
    union
    {
        s_sound_manager_reverbs reverbs;
        s_sound_manager_environment environment;
    };
    c_static_array<s_sound_manager_reverb, 2> runtime_reverbs;
    float reverb_rate;
    bool started;
    float master_gain;
    // Campaign master-gain fades. The delays count down in milliseconds; the fade-out delay is -1 until a lost game
    // arms it. The three times (seconds) are the sound mix's global solo-player fade settings.
    long fade_in_delay_milliseconds;
    long fade_out_delay_milliseconds;
    float solo_player_fade_out_delay;
    float solo_player_fade_out_time;
    float solo_player_fade_in_time;
    long ducker;
    float ducker_time;
    long previous_ducker;
    float previous_ducker_time;
    short channel_count;
    short voice_count;
    float timeline_duck;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char sound_listener_size[(sizeof(s_sound_listener) == 0x50) ? 1 : -1];
typedef char sound_listener_location_offset[(offsetof(s_sound_listener, location) == 4) ? 1 : -1];
typedef char sound_listener_valid_offset[(offsetof(s_sound_listener, valid) == 0xa) ? 1 : -1];
typedef char sound_listener_matrix_offset[(offsetof(s_sound_listener, matrix) == 0xc) ? 1 : -1];
typedef char sound_manager_reset_time_offset[(offsetof(s_sound_manager_globals, reset_time) == 0xa4) ? 1 : -1];
typedef char sound_manager_listeners_offset[(offsetof(s_sound_manager_globals, listeners) == 0xac) ? 1 : -1];
typedef char sound_manager_reverbs_offset[(offsetof(s_sound_manager_globals, reverbs) == 0x1ec) ? 1 : -1];
typedef char sound_manager_started_offset[(offsetof(s_sound_manager_globals, started) == 0x264) ? 1 : -1];
typedef char sound_manager_ducker_offset[(offsetof(s_sound_manager_globals, ducker) == 0x280) ? 1 : -1];
typedef char sound_manager_timeline_offset[(offsetof(s_sound_manager_globals, timeline_duck) == 0x294) ? 1 : -1];
#endif

typedef char sound_manager_globals_size[(sizeof(s_sound_manager_globals) == 0x298) ? 1 : -1];
typedef char sound_manager_used_sources_offset[(offsetof(s_sound_manager_globals, used_sources) == 0x60) ? 1 : -1];
typedef char sound_manager_used_voices_offset[(offsetof(s_sound_manager_globals, used_voices) == 0x7c) ? 1 : -1];
typedef char sound_manager_runtime_reverbs_offset[(offsetof(s_sound_manager_globals, runtime_reverbs) == 0x228) ? 1 : -1];
typedef char sound_manager_master_gain_offset[(offsetof(s_sound_manager_globals, master_gain) == 0x268) ? 1 : -1];

#endif // SOUND_SOUND_MANAGER_TYPES_H
