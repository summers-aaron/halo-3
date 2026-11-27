// blamlibxbox/sound_playback_controller.obj: from the records 0x129d58 0x129f90 0x13f6c0 0x1f7968 0x1f7a30 0x1f7ad8 0x1f7bb0
// @flags /TP /O2 /Oc
#define NOD3D
#include "bink/bink_playback_globals.h"
#include "sound/sound_xaudio_xenon_source_manager_types.h"
#include "sound/sound_source_types.h"
#include "sound/sound_manager_types.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "memory/data_absolute_index.h"
#include <string.h>
#include "game/players.h"
#include "cseries/runtime_state.h"
#include "sound/sound_manager.h"
#include "sound/sound_playback_controller.h"
#include "sound/sound_playback_controller_types.h"
#include "cseries/cseries.h"
#include "sound/sound_xaudio_xenon.h"
#include "sound/game_sound_types.h"
#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"
#include <xaudio.h>
#include "math/random_math.h"
// @address 0x1f7bb0 ?sound_playback_controller_release_reference@@YAXJ@Z


struct s_sound_location;
struct s_sound_source;




c_smart_data_array<c_sound_playback_controller> g_sound_playback_controller_data;







void sound_playback_controller_release_reference(long index)
{
    if (index == -1) return;
    s_data_array *data = g_sound_playback_controller_data;
    c_sound_playback_controller *controller = datum_get<c_sound_playback_controller>(data, index);
    controller->reference_count--;
    if (controller->reference_count == 0)
        controller->flags &= 0xfd;
    if (controller->reference_count == 0)
        ::datum_delete(g_sound_playback_controller_data, index);
}


c_sound_playback_controller *g_default_playback_controller;

// @address 0x8c29bc g_bink_allocation_count
long g_bink_allocation_count;

static __forceinline bool sound_playback_controller_sound_is_active()
{
    return g_sound_manager_globals->initialized && g_sound_manager_globals->enabled && g_sound_manager_globals->active;
}
static __forceinline bool sound_playback_controller_manager_is_active(s_sound_manager_globals *manager)
{
    return manager->initialized && manager->enabled && manager->active;
}
static __forceinline unsigned char sound_playback_controller_sound_manager_get_flip_flop()
{
    return g_sound_manager_globals->flip_flop;
}
static __forceinline void sound_playback_controller3_default_controller_initialize(
    c_sound_playback_controller *controller, long identifier, unsigned long seed, bool flip_flop)
{
    controller->owner_identifier = identifier;
    controller->random_seed = seed;
    controller->flag_a = !flip_flop;
    controller->flags = 1;
    memset(controller->sources, 0, sizeof(controller->sources));
}
static __forceinline c_sound_playback_controller *sound_playback_controller_get(long controller_index)
{
    c_sound_playback_controller *controller = g_default_playback_controller;
    if (controller_index != NONE)
        controller = datum_get<c_sound_playback_controller>(g_sound_playback_controller_data.get(), controller_index);
    return controller;
}
static __forceinline void sound_playback_controller_platform_sound_synch_playback(long playback_controller_index)
{
    bool synch_start = false;
    long source_count = g_xaudio_source_manager.source_count;
    for (long source_index = 0; source_index < source_count; source_index++)
    {
        c_platform_source_datum *source = &g_xaudio_source_manager.sources[source_index];
        if (source->channel == playback_controller_index && source->state != 0)
        {
            if (source->is_start_pending())
            {
                XAudioSourceVoice_Start(source->voice, 1);
                source->flags &= ~(1 << 7);
            }
            synch_start = true;
        }
    }
    if (synch_start)
        XAudioSynchStart();
}


static __forceinline long sound_playback_controller_first_active_user()
{
    long result = NONE;
    for (long user_index = 0; user_index < 4; user_index++)
    {
        if (tls_player_mapping_globals()->player_indices[user_index] != NONE)
        {
            result = user_index;
            break;
        }
    }
    return result;
}

// @address 0x129d58 ?update_controller_source@c_sound_playback_controller@@QAAXPBUs_sound_source@@_N@Z
void c_sound_playback_controller::update_controller_source(const s_sound_source *source, bool value)
{
    if (owner_identifier != NONE && flag_a != value)
    {
        if ((source->spatialization & 0xf0) == 0x10)
        {
            for (long user = sound_playback_controller_first_active_user(); user != NONE; user = players_next_active_user(user))
            {
                doppler_shifts[user] = calculate_doppler_shift_cents(user, (const s_sound_location *)&source->position);
            }
        }
        else
        {
            memset(sources, 0, sizeof(sources));
        }
        flag_a = value;
    }
}

// @address 0x13f6c0 ?sound_playback_controllers_synch_playback@@YAX_N@Z
void sound_playback_controllers_synch_playback(bool synch)
{
    for (long controller_index = ::data_next_index(g_sound_playback_controller_data, NONE); controller_index != NONE;
        controller_index = datum_absolute_index_to_index(g_sound_playback_controller_data,
            data_next_absolute_index(g_sound_playback_controller_data, (controller_index + 1) & 0xffff)))
    {
        c_sound_playback_controller *controller = sound_playback_controller_get(controller_index);
        bool can_synch = synch ? controller->can_restart_synchronized_sound() : controller->can_start_synchronized_sounds();
        if (can_synch)
        {
            sound_playback_controller_platform_sound_synch_playback(controller_index);
            if (!synch)
                controller->on_synch_playback();
        }
    }
}

// @address 0x1f7968 ?sound_playback_controller_find_existing@@YAJJ@Z
long sound_playback_controller_find_existing(long owner_identifier)
{
    long index = NONE;
    if (sound_playback_controller_sound_is_active() && owner_identifier != NONE)
    {
        index = ::data_next_index(g_sound_playback_controller_data, NONE);
        if (index != NONE)
        {
            c_sound_playback_controller *default_controller = g_default_playback_controller;
            for (; index != NONE; index = ::data_next_index(g_sound_playback_controller_data, index))
            {
                c_sound_playback_controller *controller = default_controller;
                if (index != NONE)
                    controller = datum_get<c_sound_playback_controller>(g_sound_playback_controller_data.get(), index);
                if (controller->matches_identifier(owner_identifier))
                    break;
            }
        }
    }
    return index;
}

// @address 0x129f90 ?sound_playback_controller_find_or_create@@YAJJ@Z
long sound_playback_controller_find_or_create(long owner_identifier)
{
    long index = NONE;
    s_sound_manager_globals *manager = g_sound_manager_globals;
    if (sound_playback_controller_manager_is_active(manager) && owner_identifier != NONE)
    {
        index = sound_playback_controller_find_existing(owner_identifier);
        if (index == NONE)
            index = datum_new(g_sound_playback_controller_data);
        if (index != NONE)
        {
            c_sound_playback_controller *controller = sound_playback_controller_get(index);
            controller->reference_count++;
            if (controller->reference_count == 1)
                controller->initialize(owner_identifier, generate_random_seed(), manager->flip_flop);
        }
    }
    return index;
}

// @address 0x1f7a30 ?sound_playback_controller_find_add_reference@@YAJJ@Z
long sound_playback_controller_find_add_reference(long owner_identifier)
{
    long index = NONE;
    if (sound_playback_controller_sound_is_active() && owner_identifier != NONE)
    {
        index = sound_playback_controller_find_existing(owner_identifier);
        if (index != NONE)
        {
            c_sound_playback_controller *controller = sound_playback_controller_get(index);
            controller->reference_count++;
        }
    }
    return index;
}

// @address 0x1f7ad8 ?sound_playback_controllers_initialize@@YA_NXZ
bool sound_playback_controllers_initialize(void)
{
    s_data_array *data = ::runtime_state_data_new((char const *)("sound playback controllers"), 64, sizeof(c_sound_playback_controller));
    g_sound_playback_controller_data = data;
    g_default_playback_controller = (c_sound_playback_controller *)::runtime_state_malloc(
        (char const *)(""), (char const *)(""), sizeof(c_sound_playback_controller));
    c_sound_playback_controller *controller = g_default_playback_controller;
    if (data && controller)
    {
        data_make_valid(data);
        bool flip_flop = (sound_playback_controller_sound_manager_get_flip_flop() & 1) != 0;
        unsigned long seed = generate_random_seed();
        controller = g_default_playback_controller;
        c_sound_playback_controller *const &controller_ref = controller;
        sound_playback_controller3_default_controller_initialize(controller_ref, NONE, seed, flip_flop);
        data = g_sound_playback_controller_data;
    }
    return data && controller;
}
