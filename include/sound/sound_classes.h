#ifndef SOUND_SOUND_CLASSES_H
#define SOUND_SOUND_CLASSES_H

// Functions defined in tu/blamlibxbox/sound_classes.cpp that other units call.

struct sound_class_definition;

const sound_class_definition *sound_class_get(short index);

#include "game/game_globals.h"
#include "sound/sound_class_definitions.h"
#include "sound/sound_classes_types.h"
#include "sound/sound_globals_reference_types.h"
#include "tag_files/tag_groups.h"

// Local inline body for inlined sound_class_get call sites, named apart from the
// out-of-line function: this build emits bl for cross-unit calls, so inlined sites
// need this copy.
static __forceinline const sound_class_definition *inl_sound_class_get(short index) {
    sound_globals_reference *reference = global_game_globals->sound_globals.count() ? global_game_globals->sound_globals.begin() : 0;
    return &(TAG_GET('sncl', sound_classes, reference->sound_classes.index))->classes[index];
}
extern "C" unsigned char sound_class_is_valid(unsigned int index);
extern "C" void sound_class_update_ducking_gain(short sound_class_index);

#include "cache/restricted_memory_regions.h"

// Named allocator objects owned by this unit.
void __tls_set_g_sound_class_data_allocator(void *allocation);
typedef t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_sound_class_data_allocator> t_sound_class_data_allocator;
template<>
void *t_sound_class_data_allocator::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);
template<>
void *t_sound_class_data_allocator::allocate(unsigned int size, const char *name);
template<>
void t_sound_class_data_allocator::deallocate(void *memory);
extern t_sound_class_data_allocator g_sound_class_data_allocator;

// Data defined in tu/blamlibxbox/sound_classes.cpp: the name of each sound class (k_sound_class_count, 54 entries).

extern const char *sound_class_names[];

struct real_decibel;

extern "C" void sound_class_set_gain_by_index(long sound_class_index, real_decibel gain_db, float time, long channel_index);
void sound_class_set_gain_db(const char *name, real_decibel gain_db, float time, long channel_index);

void sound_classes_initialize(void);
void sound_classes_dispose(void);
void sound_classes_initialize_for_new_map(void);
void sound_classes_dispose_from_old_map(void);

#endif // SOUND_SOUND_CLASSES_H
