// OBJ: from the records 0x28da88 0x28dbc0 0x28dc28 0x28dd08 0x28de08 0x28ded0 0x28df00
// @compiler xdk6130
#include "game/game_time_inline.h"
// @flags /TP /O2 /Oc
// @address 0x28de08 game_engine_initialize_multiplayer_sounds

#include <string.h>
#include "sound/game_sound.h"
#include "game/game_engine_multiplayer_sounds.h"
#include "game/sound_tracking_data_types.h"
#include "sound/game_sound_impulse_parameters.h"
#include "saved_games/game_state_lock.h"

#include "game/game_time_types.h"
#include "game/game_engine_event_data.h"
#include "sound/sound_tracker.h"
// @address 0x1fd070 ?unspatialized_impulse_sound_new@@YAJJM@Z
// @address 0x1fda88 ?game_sound_impulse_new_internal@@YAJJPAUs_game_sound_impulse_internal_parameters@@@Z

// @address 0x7f1958 g_game_engine_multiplayer_sound_queue
s_game_engine_multiplayer_sound_queue g_game_engine_multiplayer_sound_queue;
// @address 0x3602c k_game_engine_multiplayer_sound_tracker
// @rdata 0x8203602c..0x8203604c
const s_sound_tracker k_game_engine_multiplayer_sound_tracker =
{
    { 0, 0, 0, 0, (void *)game_engine_on_announcer_sound_stop_proc, 0, 0, 0 }
};
// @rdata-end

// 0x28da88: records sound_index as a listener of the event's recent-sound slot, then queues the sound
// (or plays it now when queue is false) if it is new and valid.
// @address 0x28da88 game_engine_queue_multiplayer_sound
extern "C" void game_engine_queue_multiplayer_sound(long sound_index, long sound_param, const s_game_engine_event_data *event, bool queue)
{
    s_recent_sound_event *recent = get_sound_event(event);
    bool inserted = false;
    long index = 0;
    for (; index < 4; index++)
    {
        if (recent->listeners[index] == sound_index)
            break;
        if (recent->listeners[index] == -1)
        {
            inserted = true;
            recent->listeners[index] = sound_index;
            break;
        }
    }
    bool add = inserted;
    if (event->type == 0 && (event->event == 20 || event->event == 21))
        add = true;
    if (add && sound_index != -1)
    {
        if (queue)
        {
            long count = g_game_engine_multiplayer_sound_queue.count;
            if (count >= 5)
            {
                game_engine_remove_expired_queued_sounds();
                count = g_game_engine_multiplayer_sound_queue.count;
            }
            if ((unsigned long)count > 4)
                return;
            g_game_engine_multiplayer_sound_queue.entries[count].sound_index = sound_index;
            g_game_engine_multiplayer_sound_queue.entries[g_game_engine_multiplayer_sound_queue.count].queued_sound_delay_ticks = (short)sound_param;
            g_game_engine_multiplayer_sound_queue.entries[g_game_engine_multiplayer_sound_queue.count].queued_sound_time = game_time_get();
            g_game_engine_multiplayer_sound_queue.count = g_game_engine_multiplayer_sound_queue.count + 1;
            return;
        }
        unspatialized_impulse_sound_new(sound_index, 1.0f);
    }
}

// @address 0x28dbc0 game_engine_delete_queued_multiplayer_sound
extern "C" long game_engine_delete_queued_multiplayer_sound(long index)
{
    g_game_engine_multiplayer_sound_queue.count--;
    for (; index < g_game_engine_multiplayer_sound_queue.count; index++)
        g_game_engine_multiplayer_sound_queue.entries[index] = g_game_engine_multiplayer_sound_queue.entries[index + 1];
    return index;
}

// 0x28dc28: the queued sounds that have waited out their delay are dropped.
// @address 0x28dc28 game_engine_remove_expired_queued_sounds
extern "C" void game_engine_remove_expired_queued_sounds()
{
    long index = 0;
    long count = g_game_engine_multiplayer_sound_queue.count;
    while (index < count)
    {
        s_game_engine_multiplayer_sound_globals *entry = &g_game_engine_multiplayer_sound_queue.entries[index];
        if (!entry->playing && game_time_get() - entry->queued_sound_time + entry->queued_sound_delay_ticks >= (long)((float)game_tick_rate() * 5.0f + 0.5f))
        {
            game_engine_delete_queued_multiplayer_sound(index);
            count = g_game_engine_multiplayer_sound_queue.count;
        }
        else
            index++;
    }
}

extern "C" void game_engine_initialize_multiplayer_sounds()
{
    memset(&g_game_engine_multiplayer_sound_queue, 0, sizeof(g_game_engine_multiplayer_sound_queue));
    g_game_engine_multiplayer_sound_queue.count = 1;
    g_game_engine_multiplayer_sound_queue.entries[0].sound_index = -1;
    g_game_engine_multiplayer_sound_queue.entries[0].playing = false;
    g_game_engine_multiplayer_sound_queue.entries[0].queued_sound_delay_ticks = (short)(long)((float)game_tick_rate() * 2.0f + 0.5f);
    g_game_engine_multiplayer_sound_queue.entries[0].queued_sound_time = game_time_get();
    for (long slot_index = 0; slot_index < 4; slot_index++)
        g_game_engine_multiplayer_sound_queue.slots[slot_index].timer = -1000;
}

void game_engine_on_announcer_sound_stop_proc(long sound_index, long sound_event_index, e_sound_event_stop_reason reason)
{
    if (!game_state_is_locked())
    {
        if (g_game_engine_multiplayer_sound_queue.entries[0].sound_event_index == sound_event_index)
            g_game_engine_multiplayer_sound_queue.entries[0].playing = false;
    }
}

// @address 0x28df00 ?get_sound_event@@YAPAUs_recent_sound_event@@PBUs_game_engine_event_data@@@Z


static __forceinline long get_event_hash(const s_game_engine_event_data *event)
{
    return event->type ^ (event->event << 16) ^ (event->cause_team_index << 4)
        ^ ~(event->effect_team_index << 8) ^ event->cause_player_index ^ ~event->effect_player_index;
}


s_recent_sound_event *get_sound_event(const s_game_engine_event_data *event)
{
    long lifetime = game_seconds_to_ticks_round(0.9f);
    long hash = get_event_hash(event);
    long current_time = game_time_get();
    long event_index = -1;
    long oldest_elapsed_time = -1;
    bool new_event = true;
    for (long index = 0; index < 4; ++index) {
        if (g_game_engine_multiplayer_sound_queue.slots[index].event_hash == hash &&
            current_time - g_game_engine_multiplayer_sound_queue.slots[index].timer < lifetime) {
            event_index = index;
            new_event = false;
            break;
        } else {
            long elapsed_time = current_time - g_game_engine_multiplayer_sound_queue.slots[index].timer;
            if (elapsed_time > oldest_elapsed_time) {
                oldest_elapsed_time = elapsed_time;
                event_index = index;
            }
        }
    }
    g_game_engine_multiplayer_sound_queue.slots[event_index].timer = current_time;
    if (new_event) {
        g_game_engine_multiplayer_sound_queue.slots[event_index].event_hash = hash;
        memset(g_game_engine_multiplayer_sound_queue.slots[event_index].listeners, -1,
            sizeof(g_game_engine_multiplayer_sound_queue.slots[event_index].listeners));
    }
    return &g_game_engine_multiplayer_sound_queue.slots[event_index];
}

// 0x28dd08: the queued sound at the head of the queue plays once its delay has run out (its impulse handle goes into
// the entry); otherwise the delay counts down, and an idle head entry is dropped from the queue.
static __forceinline long game_engine_multiplayer_sound_impulse_new(long sound_index, float scale, const s_sound_tracker *tracking)
{
    s_game_sound_impulse_internal_parameters parameters;
    parameters.additional_override_flags = 0;
    parameters.source.volume_type = 0;
    parameters.source.flags = 0;
    parameters.source.spatialization = 0;
    parameters.source.volume = scale;
    parameters.source.name = 0;
    parameters.object_index = -1;
    parameters.playback_label = -1;
    parameters.tracking_data = 0;
    parameters.tracking = tracking;
    parameters.permutation = 0;
    return game_sound_impulse_new_internal(sound_index, &parameters);
}

// @address 0x28dd08 ?game_engine_update_multiplayer_sound@@YAXXZ
void game_engine_update_multiplayer_sound(void)
{
    game_engine_remove_expired_queued_sounds();
    if (g_game_engine_multiplayer_sound_queue.count != 0)
    {
        short delay = g_game_engine_multiplayer_sound_queue.entries[0].queued_sound_delay_ticks;
        if (delay >= 0)
        {
            short next_delay = (short)(delay - 1);
            g_game_engine_multiplayer_sound_queue.entries[0].queued_sound_delay_ticks = next_delay;
            if (next_delay == -1)
            {
                long sound_index = g_game_engine_multiplayer_sound_queue.entries[0].sound_index;
                long impulse = -1;
                if (sound_index != -1)
                    impulse = game_engine_multiplayer_sound_impulse_new(sound_index, 1.0f, &k_game_engine_multiplayer_sound_tracker);
                g_game_engine_multiplayer_sound_queue.entries[0].sound_event_index = impulse;
                if (impulse != -1)
                    g_game_engine_multiplayer_sound_queue.entries[0].playing = true;
            }
            return;
        }
        if (!g_game_engine_multiplayer_sound_queue.entries[0].playing)
            game_engine_delete_queued_multiplayer_sound(0);
    }
}
