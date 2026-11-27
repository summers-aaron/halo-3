#ifndef GAME_GAME_ENGINE_MULTIPLAYER_SOUNDS_H
#define GAME_GAME_ENGINE_MULTIPLAYER_SOUNDS_H

// Multiplayer announcer queue and functions owned by game_engine_multiplayer_sounds.cpp.

struct s_game_engine_event_data;
struct s_sound_tracker;
enum e_sound_event_stop_reason;

struct s_game_engine_multiplayer_sound_globals
{
    long sound_index;
    long sound_event_index;
    long queued_sound_time;
    bool playing;
    short queued_sound_delay_ticks;
};

struct s_recent_sound_event
{
    long event_hash;
    long listeners[4];
    long timer;
};

struct s_game_engine_multiplayer_sound_queue
{
    s_game_engine_multiplayer_sound_globals entries[5];
    s_recent_sound_event slots[4];
    long count;
};

typedef char multiplayer_sound_entry_size[(sizeof(s_game_engine_multiplayer_sound_globals) == 0x10) ? 1 : -1];
typedef char recent_sound_event_size[(sizeof(s_recent_sound_event) == 0x18) ? 1 : -1];
typedef char multiplayer_sound_queue_size[(sizeof(s_game_engine_multiplayer_sound_queue) == 0xb4) ? 1 : -1];

extern "C" {
extern s_game_engine_multiplayer_sound_queue g_game_engine_multiplayer_sound_queue;
extern const s_sound_tracker k_game_engine_multiplayer_sound_tracker;
}

void game_engine_on_announcer_sound_stop_proc(long sound_index, long sound_event_index, e_sound_event_stop_reason reason);

extern "C" void game_engine_queue_multiplayer_sound(long sound_index, long sound_param, const s_game_engine_event_data *event, bool queue);
extern "C" void game_engine_initialize_multiplayer_sounds();
void game_engine_update_multiplayer_sound(void);

extern "C" void game_engine_remove_expired_queued_sounds();
s_recent_sound_event *get_sound_event(const s_game_engine_event_data *event);

#endif // GAME_GAME_ENGINE_MULTIPLAYER_SOUNDS_H
