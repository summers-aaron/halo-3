#ifndef SOUND_SOUND_CLASSES_TYPES_H
#define SOUND_SOUND_CLASSES_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

struct sound_class_definition;

struct sound_classes { long count; TAG_POINTER(sound_class_definition) classes; };

#include "sound/game_sound_definitions.h"

struct sound_class_datum
{
    real_decibel gain_db[5];
    real_decibel target_gain_db[5];
    float time[5];
    unsigned short flags;
    long state;
};

typedef char sound_class_flags_offset[(offsetof(sound_class_datum, flags) == 0x3c) ? 1 : -1];
typedef char sound_class_state_offset[(offsetof(sound_class_datum, state) == 0x40) ? 1 : -1];
typedef char sound_class_datum_size[(sizeof(sound_class_datum) == 0x44) ? 1 : -1];

#include "sound/sound_class_tls.h"
extern "C" {
extern bool g_sound_classes_initialized;
extern const float k_sound_class_minimum_fade_time;
}
static __forceinline sound_class_datum *sound_class_data()
{
    return g_sound_class_data;
}
static __forceinline bool sound_class_data_is_valid()
{
    return g_sound_class_data != 0;
}
static __forceinline void sound_class_data_set(void *data)
{
    g_sound_class_data = (sound_class_datum *)data;
}
static __forceinline bool sound_classes_initialized()
{
    return g_sound_classes_initialized;
}
static __forceinline void sound_classes_set_initialized(bool initialized)
{
    g_sound_classes_initialized = initialized;
}

#endif // SOUND_SOUND_CLASSES_TYPES_H
