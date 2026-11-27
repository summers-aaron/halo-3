#ifndef SOUND_SOUND_CLASS_TLS_H
#define SOUND_SOUND_CLASS_TLS_H

struct sound_class_datum;

// Thread-local pointer to the sound-class data array.
extern "C" {
extern __declspec(thread) sound_class_datum *g_sound_class_data;
}

#endif
