#ifndef GAME_GAME_TIME_TYPES_H
#define GAME_GAME_TIME_TYPES_H

// Global game time state: tick rate, current time, pause tick, and time-scale blending.
struct game_time_globals_definition {
    unsigned char initialized;
    char pad1;
    unsigned short flags;
    short ticks_per_second;
    float tick_length;
    long time;
    float rate_scale;
    float leftover_ticks;
    float rate_scale_timer;
    float rate_scale_duration;
    float rate_scale_initial;
    float rate_scale_final;
    long pause_game_tick;
};

typedef char game_time_globals_size_must_be_44[(sizeof(game_time_globals_definition) == 44) ? 1 : -1];

// Thread-local game time state (TLS slot 0x831df6bc), owned by game_time.cpp (March 2007 debug name and type).
extern __declspec(thread) game_time_globals_definition *game_time_globals;

#endif // GAME_GAME_TIME_TYPES_H
