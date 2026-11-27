#ifndef SOUND_SOUND_PLAYBACK_CONTROLLER_H
#define SOUND_SOUND_PLAYBACK_CONTROLLER_H

// Functions defined in tu/blamlibxbox/sound_playback_controller.cpp that other units call.

void sound_playback_controller_release_reference(long index);
void sound_playback_controllers_synch_playback(bool synch);
long sound_playback_controller_find_existing(long owner_identifier);
long sound_playback_controller_find_or_create(long owner_identifier);
long sound_playback_controller_find_add_reference(long owner_identifier);
bool sound_playback_controllers_initialize(void);

// Data defined in tu/blamlibxbox/sound_playback_controller.cpp that other units use.

class c_sound_playback_controller;
template<typename t_datum> class c_smart_data_array;

extern c_sound_playback_controller *g_default_playback_controller;
extern c_smart_data_array<c_sound_playback_controller> g_sound_playback_controller_data;

#endif // SOUND_SOUND_PLAYBACK_CONTROLLER_H
