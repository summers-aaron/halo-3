#ifndef SAVED_GAMES_SAVED_FILM_GLOBALS_H
#define SAVED_GAMES_SAVED_FILM_GLOBALS_H

#include "saved_games/saved_film_recording_types.h"

struct s_film_replay_metadata
{
    union {
        unsigned long words[9];
        struct {
            long type;
            long player_index;
            real_point3d position;
            real_vector3d forward;
            bool following;
        };
    };
};

struct s_saved_film_timer_globals
{
    __forceinline s_saved_film_timer_globals() {}
    ~s_saved_film_timer_globals() {}

    union {
        bool initialized;
        char name[64];
    };
    c_saved_film film;
    bool game_state_load_pending;
    long game_state_stream_position;
    bool dispose_in_progress;
    bool player_remap_enabled;
    float playback_speed;
    unsigned char playback_flag;
    bool unknown_10201;
    bool timer_active;
    bool sound_fade_complete;
    unsigned long timer_start;
    long timer_limit;
    unsigned char playback_sub_flag;
    long timer_pending;
    long timer_other;
    long timer_target;
    c_static_flags<32> authored_camera_users;
    unsigned long replay_flags;
    s_film_replay_metadata replay_metadata;
    bool request_pending;
    long requested_state;
    bool ignore_version;
    bool unknown_10251;
    bool scoreboard_message_shown;
    bool screen_saver_enabled;
    bool delete_on_level_load;
};

typedef char saved_film_globals_playback_speed_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_timer_globals, playback_speed) == 0x101fc))) ? 1 : -1];
typedef char saved_film_globals_film_offset[(offsetof(s_saved_film_timer_globals, film) == 0x40) ? 1 : -1];
typedef char saved_film_globals_size[(sizeof(void *) != 4 || ((sizeof(s_saved_film_timer_globals) == 0x10258))) ? 1 : -1];

typedef char saved_film_globals_flags_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_timer_globals, authored_camera_users) == 0x1021c))) ? 1 : -1];
typedef char saved_film_globals_reset_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_timer_globals, screen_saver_enabled) == 0x10253))) ? 1 : -1];
typedef char saved_film_replay_metadata_size[(sizeof(s_film_replay_metadata) == 0x24) ? 1 : -1];

extern "C" s_saved_film_timer_globals g_saved_film_timer_globals;


#endif
