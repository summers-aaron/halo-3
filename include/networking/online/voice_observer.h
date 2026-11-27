#ifndef NETWORKING_ONLINE_VOICE_OBSERVER_H
#define NETWORKING_ONLINE_VOICE_OBSERVER_H

// Functions defined in tu/networkinglibxbox/voice_observer.cpp.

extern "C" bool voice_observer_is_push_to_talk_active(void *observer, long player_index);

class c_voice_observer;

extern "C" unsigned long voice_observer_get_team_chat_mask(void *observer, long player_index);
extern "C" long voice_observer_new_multiplayer_sound(c_voice_observer *observer, long player_index, long sound_type);

#endif // NETWORKING_ONLINE_VOICE_OBSERVER_H
