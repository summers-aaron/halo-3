#ifndef SOUND_SOUND_DATA_H
#define SOUND_SOUND_DATA_H

template<typename T> class c_smart_data_array;
struct sound_datum;
extern c_smart_data_array<sound_datum> g_sound_data;
struct looping_sound_datum;
extern c_smart_data_array<looping_sound_datum> g_looping_sound_data;

#endif // SOUND_SOUND_DATA_H
