#ifndef MAIN_GAME_TICK_TIME_SAMPLES_H
#define MAIN_GAME_TICK_TIME_SAMPLES_H

#include "cseries/cseries_wrapped.h"

// main.cpp reserves 20 bytes for each timing accumulator; main_render.cpp reads
// the accumulated seconds at +0xc. Wind consumes the same published samples.
struct s_game_tick_time_samples
{
    union
    {
        long count;
        c_flags_no_init<long, unsigned long, 12> flags;
    };
    float minimum;
    float maximum;
    float total;
    long last_sample;

    __forceinline bool publishing_flags_empty() const { return ((unsigned long)count & 0xfff) == 0; }
    __forceinline void reset() { count = 0; minimum = 0.0f; maximum = 0.0f; total = 0.0f; last_sample = 0; }
    __forceinline void initialize() { reset(); }
    __forceinline void accum(const s_game_tick_time_samples *samples)
    {
        count = samples->count;
        minimum += samples->minimum;
        maximum += samples->maximum;
        total += samples->total;
        last_sample = samples->last_sample;
    }
};

typedef char game_tick_time_samples_size[(sizeof(s_game_tick_time_samples) == 20) ? 1 : -1];

// Published render timing accumulator in TLS.
extern "C" {
extern __declspec(thread) s_game_tick_time_samples *g_main_gamestate_timing_data;
extern __declspec(thread) s_game_tick_time_samples *g_main_render_timing_data;
}

static __forceinline s_game_tick_time_samples *tls_game_tick_time_samples()
{
    return g_main_render_timing_data;
}

#endif // MAIN_GAME_TICK_TIME_SAMPLES_H
