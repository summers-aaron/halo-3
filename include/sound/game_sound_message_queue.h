#ifndef SOUND_GAME_SOUND_MESSAGE_QUEUE_H
#define SOUND_GAME_SOUND_MESSAGE_QUEUE_H

#include <stddef.h>

#include <ppcintrinsics.h>

#include "objects/object_reference_types.h"
#include "sound/game_sound_types.h"

// Sound location plus the scenario location the queued message carries.
struct game_sound_location_block { s_sound_location sound; s_location location; };
struct game_sound_render_message {
    game_sound_render_message *next;
    long handle;
    game_sound_location_block location;
    float value;
    __int64 padding_after_value;
};
struct game_sound_render_message_queue {
    game_sound_render_message messages[128];
    __int64 pending_list;
    __int64 free_list;
    bool valid;
    long count;
    ~game_sound_render_message_queue();
    __forceinline void initialize(long message_count);
};
extern "C" game_sound_render_message_queue g_game_sound_message_queue;

#if defined(_XBOX)
typedef char game_sound_render_message_size_check[(sizeof(void *) != 4 || sizeof(game_sound_render_message) == 64) ? 1 : -1];
typedef char game_sound_render_message_queue_pending_list_check[(sizeof(void *) != 4 || offsetof(game_sound_render_message_queue,
    pending_list) == 0x2000) ? 1 : -1];
typedef char game_sound_render_message_queue_free_list_check[(sizeof(void *) != 4 || offsetof(game_sound_render_message_queue, free_list) == 0x2008) ? 1 : -1];
typedef char game_sound_render_message_queue_size_check[(sizeof(void *) != 4 || sizeof(game_sound_render_message_queue) == 0x2018) ? 1 : -1];
#endif

#endif // SOUND_GAME_SOUND_MESSAGE_QUEUE_H
