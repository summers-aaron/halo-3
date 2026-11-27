// blamlibxbox/sound_classes.obj: from the records 0xf8500 0x1fae50 0x1faf90 0x1fafe8 0x1fb050 0x1fb060 0x1fb078 0x1fb1f0 0x1fb270 0x1fb2f8 0x1fb398 0x1fb3b0
// @compiler xdk6130
// @flags /TP /O2
#include "shell/shell.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "game/game_globals_storage.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "camera/observer.h"
#include "sound/sound_classes.h"
#include "game/game_globals.h"
#include "scenario/scenario.h"
#include "sound/game_sound_definitions.h"
#include "sound/sound_classes_types.h"
#include "cache/cache_files.h"
#include "sound/sound_globals_reference_types.h"
#include "sound/sound_class_definitions.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory.h"
#include "interface/c_gui_screen_widget.h"
#include "networking/online/online_globals_types.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_window_manager.h"

// @address 0xf8500 ?sound_class_get@@YAPBUsound_class_definition@@F@Z
// @address 0x1fae50 sound_class_is_valid
// @address 0x1faf90 ?sound_classes_initialize@@YAXXZ
// @address 0x1fafe8 ?sound_classes_initialize_for_new_map@@YAXXZ
// @address 0x1fb050 ?sound_classes_dispose_from_old_map@@YAXXZ
// @address 0x1fb060 ?sound_classes_dispose@@YAXXZ
// @address 0x1fb270 ?sound_class_set_gain_db@@YAXPBDUreal_decibel@@MJ@Z
// @address 0x2251c k_sound_class_minimum_fade_time
#if !defined(_XBOX) || defined(__clang__)
const float k_sound_class_minimum_fade_time = 0.0f;
#endif

enum { k_sound_class_count = 54, k_sound_class_channel_count = 5 };

enum { k_sound_class_ducking_channel = 2, k_game_mode_multiplayer = 3 };



const sound_class_definition *sound_class_get(short index)
{
  sound_globals_reference *reference = global_game_globals->sound_globals.count() ? global_game_globals->sound_globals.begin() : 0;
  unsigned short tag_index = (unsigned short)reference->sound_classes.index;
  void *definition = global_tag_instances[tag_index].definition;
  return &((sound_classes *)definition)->classes[index];
}

// 0x1fae50
extern "C" unsigned char sound_class_is_valid(unsigned int index)
{
    sound_globals_reference *holder = global_game_globals->sound_globals.count() ? global_game_globals->sound_globals.begin() : 0;
    bool valid;
    if (holder)
        valid = true;
    else
        valid = false;
    valid = valid && holder->sound_classes.index != -1;
    valid = valid && index <= 53;
    valid = valid && *sound_class_names[index] != 0;
    if (valid)
    {
        const sound_class_definition *entry = &TAG_GET('sncl', sound_classes, holder->sound_classes.index)->classes[index];
        bool ok = entry->maximum_sounds_per_tag <= 16;
        ok = ok && entry->maximum_sounds_per_object <= 16;
        ok = ok && (entry->internal_flags & 1) != 0;
        return ok;
    }
    return valid;
}

// 0x1faf90
void sound_classes_initialize(void)
{
    void *data = (&g_sound_class_data_allocator)
                     ->reserve_memory("sound classes", 0, sizeof(sound_class_datum) * k_sound_class_count, 0);
    sound_class_data_set(data);
    memset(data, 0, sizeof(sound_class_datum) * k_sound_class_count);
}





static const real_decibel k_sound_classes_zero_gain_db = { 0.0f };


// 0x1fafe8
void sound_classes_initialize_for_new_map(void)
{
    sound_classes_set_initialized(true);

    for (long sound_class_index = 0; sound_class_index < k_sound_class_count; sound_class_index++)
    {
        sound_class_datum *datum = &sound_class_data()[sound_class_index];

        for (long channel_index = 0; channel_index < k_sound_class_channel_count; channel_index++)
        {
            datum->target_gain_db[channel_index] = k_sound_classes_zero_gain_db;
            datum->gain_db[channel_index] = k_sound_classes_zero_gain_db;
            datum->time[channel_index] = 0.0f;
        }
    }
}

// 0x1fb050
void sound_classes_dispose_from_old_map(void)
{
    sound_classes_set_initialized(false);
}

// 0x1fb060
void sound_classes_dispose(void)
{
  sound_class_data_set(0);
}

// @address 0x1fb1f0 sound_class_set_gain_by_index
extern "C" void sound_class_set_gain_by_index(long sound_class_index, real_decibel gain_db, float time, long channel_index)
{
    sound_class_datum *datum = &sound_class_data()[sound_class_index];

    datum->gain_db[channel_index] = pin_gain_db(gain_db);
    datum->time[channel_index] = time < k_sound_class_minimum_fade_time ? k_sound_class_minimum_fade_time : time;
    if (time == k_sound_class_minimum_fade_time)
        datum->target_gain_db[channel_index] = datum->gain_db[channel_index];
}

// @address 0x1fb078 sound_class_update_ducking_gain
extern "C" void sound_class_update_ducking_gain(short sound_class_index)
{
    if (!sound_class_is_valid(sound_class_index))
        return;

    const sound_class_definition *definition = sound_class_get(sound_class_index);
    if (definition->flags & 1)
        return;

    sound_class_datum *datum = &sound_class_data()[sound_class_index];
    c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, 0x10081);
    float time;
    float gain;

    if (online_guide_is_active())
    {
        if (datum->state == 1)
            return;
        datum->state = 1;
        time = tls_game_globals()->options.game_mode == k_game_mode_multiplayer ? 5.0f : 1.0f;
        gain = 0.15f;
    }
    else if (screen && tls_game_globals()->options.game_mode != k_game_mode_multiplayer)
    {
        if (datum->state == 2)
            return;
        datum->state = 2;
        time = 1.0f;
        gain = 0.2f;
    }
    else
    {
        if (datum->state == 0)
            return;
        datum->state = 0;
        time = tls_game_globals()->options.game_mode == k_game_mode_multiplayer ? 5.0f : 1.0f;
        gain = 1.0f;
    }

    datum->time[k_sound_class_ducking_channel] = time;
    datum->gain_db[k_sound_class_ducking_channel] = real_gain_to_decibels(gain);
}

// 0x1fb270
void sound_class_set_gain_db(const char *name, real_decibel gain_db, float time, long channel_index)
{
    for (long sound_class_index = 0; sound_class_index < k_sound_class_count; sound_class_index++)
    {
        if (*sound_class_names[sound_class_index] && strstr(sound_class_names[sound_class_index], name))
            sound_class_set_gain_by_index(sound_class_index, gain_db, time, channel_index);
    }
}


/* Propagated from the exact source of sub_1be748, a member of the same template family
   (identical code up to constants and targets; Byte-for-Byte Haskell 0.8.24). */

// Registers a member of field layout 1 (see restricted_memory_register_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.

// @address 0x1fb2f8 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_sound_class_data_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_sound_class_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(
      &g_restricted_regions[1], name, type, size, 0, __tls_set_g_observer_gamestate_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(1) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  sound_class_data_set(address);
  return restricted_region_get_member_address(1, member_index);
}

// 0x1fb398
template<> void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_sound_class_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x1fb3b0
template<> void t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_sound_class_data_allocator>::deallocate(void *memory)
{
    long member_index = m_member_index;
    sound_class_data_set(0);
    restricted_region_free_member(1, member_index);
}


// @address 0x745f58 ?sound_class_names@@3PAPBDA
const char *sound_class_names[] = {
    "projectile_impact",
    "projectile_detonation",
    "projectile_flyby",
    "projectile_detonation_lod",
    "weapon_fire",
    "weapon_ready",
    "weapon_reload",
    "weapon_empty",
    "weapon_charge",
    "weapon_overheat",
    "weapon_idle",
    "weapon_melee",
    "weapon_animation",
    "object_impacts",
    "particle_impacts",
    "weapon_fire_lod",
    "unused1_impacts!",
    "unused2_impacts!",
    "unit_footsteps",
    "unit_dialog",
    "unit_animation",
    "unit_unused!",
    "vehicle_collision",
    "vehicle_engine",
    "vehicle_animation",
    "vehicle_engine_lod",
    "device_door",
    "device_unused0!",
    "device_machinery",
    "device_stationary",
    "device_unused1!",
    "device_unused2!",
    "music",
    "ambient_nature",
    "ambient_machinery",
    "ambient_stationary",
    "huge_ass",
    "object_looping",
    "cinematic_music",
    "unknown_unused0!",
    "unknown_unused1!",
    "ambient_flock",
    "no_pad",
    "no_pad_stationary",
    "mission_unused0!",
    "cortana_mission",
    "cortana_gravemind_channel",
    "mission_dialog",
    "cinematic_dialog",
    "scripted_cinematic_foley",
    "game_event",
    "ui",
    "test",
    "multilingual_test",
};

t_sound_class_data_allocator g_sound_class_data_allocator;

// @address 0x11df828 g_sound_class_data
extern "C" {
__declspec(thread) sound_class_datum *g_sound_class_data;
}

// @address 0x7e3631 g_sound_classes_initialized
extern "C" {
bool g_sound_classes_initialized;
}
