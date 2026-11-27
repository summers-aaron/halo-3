#ifndef SOUND_GAME_LOOPING_SOUND_DATA_H
#define SOUND_GAME_LOOPING_SOUND_DATA_H

extern "C++" {
#include "memory/data.h"
struct game_looping_sound_datum;
extern __declspec(thread) c_smart_data_array<game_looping_sound_datum> game_looping_sound_data;
}

#endif // SOUND_GAME_LOOPING_SOUND_DATA_H
