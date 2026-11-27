// blamlibxbox/game_time.obj: from the records 0x80668 0x983e0 0xd5d60 0xd5e80 0xf1968 0x17a670 0x1d6ed8 0x1d6f30 0x1d6fb8 0x1d6fe8 0x1d7098 0x1d7148 0x1d7400 0x1d74a0 0x1e1d88 0x23f330 0x2416a0
// @flags /TP /O2
#include "bink/bink_playback_types.h"
#include "interface/controller_inline.h"
#include "render/water_interaction_tls.h"
#include "sound/sound_pause.h"
#include "memory/tls_data_arrays.h"
#include "sound/sound_manager_types.h"
#include "game/game_time_types.h"
#include "memory/static_arrays.h"
#include "game/game_globals_storage.h"
#include <ppcintrinsics.h>
#include <string.h>
#include <stdlib.h>
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "cutscene/cinematics.h"
#include "game/game_time.h"
#include "math/integer_math.h"
#include "networking/logic/network_session_interface.h"
#include "render/render_water.h"
#include "simulation/simulation_world.h"
#include "game/game_globals.h"
#include "bink/bink_playback.h"
#include "interface/controller_types.h"
#include "simulation/simulation_globals.h"
#include "sound/sound_pause_state_types.h"
#include "cutscene/cinematic_title_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "game/game_time_runtime.h"
#include "cache/restricted_memory_layout.h"
#include "game/game_time_access.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"
#include "game/game_time_allocator.h"
#include "interface/user_interface_messages.h"
#include "networking/online/online_globals_types.h"
#include "sound/sound_manager.h"
#include "saved_games/saved_film_timing.h"
#include "saved_games/saved_film_tick.h"
#include "simulation/simulation_world_types.h"
#include "memory/data_inline.h"
// @address 0x1d7098 ?game_time_set_rate_scale_direct@@YAXM@Z


// @address 0x11df680 game_globals
// @address 0x11df6bc game_time_globals
__declspec(thread) game_time_globals_definition *game_time_globals;
// @address 0x11401e8 g_controller_globals
// @address 0x808560 g_simulation_globals
// @address 0x94e3b0 g_saved_film_timer_globals
// @address 0x94e3ac g_saved_film_playback_tick



struct s_flags_iterator {
    int word_index;
    int flags[1];
};


// 0x80668

// @address 0x80668 game_time_is_paused
extern "C" bool game_time_is_paused(void)
{
    bool result = false;
    if (game_time_globals->initialized) {
        // match: 0x291b40: preserve the original volatile TLS read in game_time_is_paused.
        bool campaign = static_cast<game_globals_storage *volatile &>(game_globals)->options.game_mode == 1;
        bool paused = game_is_cooperative() && !game_is_film_playback_active();
        const bool allowed = campaign && !paused;
        unsigned short flags = game_time_globals->flags;
        result = flag_test(flags, 1);
        if (flag_test(flags, 2) || flag_test(flags, 8))
            result = allowed | result;
        if (flag_test(flags, 3) || flag_test(flags, 4) || flag_test(flags, 5) || flag_test(flags, 6))
            result = allowed | result;
        if (flag_test(flags, 7))
            result = allowed | result;
    }
    return result;
}

// @address 0xd5d60 game_time_update_water_ripples
extern "C" void game_time_update_water_ripples(void)
{
    s_water_interaction_data *data = g_water_interaction_data_address;
    s_flags_iterator iterator;
    iterator.word_index = 0;
    iterator.flags[0] = data->active_ripples.m_storage[0];

    int ripple_index = flags_iterator_next(&iterator);
    if (ripple_index != -1) {
        do {
            s_water_interaction_ripple *ripple = &data->ripples[ripple_index];
            if (game_time_globals->time - ripple->start_time > ripple->period) {
                data->active_ripples.set(ripple_index, false);
            }
            ripple_index = flags_iterator_next(&iterator);
        } while (ripple_index != -1);
    }

    memset(data->ripple_amplitudes, 0, sizeof(data->ripple_amplitudes));

    iterator.word_index = 0;
    iterator.flags[0] = data->active_ripples.m_storage[0];

    ripple_index = flags_iterator_next(&iterator);
    if (ripple_index != -1) {
        do {
            s_water_interaction_ripple *ripple = &data->ripples[ripple_index];
            long offset = ripple->offset;
            if (offset < 30) {
                do {
                    data->ripple_amplitudes[offset] += ripple->amplitude;
                    offset += ripple->period;
                } while (offset < 30);
            }
            ripple_index = flags_iterator_next(&iterator);
        } while (ripple_index != -1);
    }
}

// @address 0xd5e80 flags_iterator_next
extern "C" int flags_iterator_next(s_flags_iterator *iterator)
{
    int result = -1;
    while (iterator->word_index < 1) {
        int mask = iterator->flags[iterator->word_index];
        result = 31 - _CountLeadingZeros(-mask & mask);
        if (result != -1) {
            iterator->flags[iterator->word_index] &= ~(1 << result);
            break;
        }
        iterator->word_index++;
    }
    return result;
}

// @address 0xf1968 game_time_add_water_ripple
extern "C" int game_time_add_water_ripple(int key, int period, float amplitude)
{
    int result = -1;
    if (period >= 30)
        period = 30;
    if (period > 1) {
        s_water_interaction_data *data = g_water_interaction_data_address;
        unsigned long *used = data->active_ripples.m_storage;
        s_flags_iterator iterator;
        iterator.word_index = 0;
        iterator.flags[0] = data->active_ripples.m_storage[0];
        int ripple_index = flags_iterator_next(&iterator);
        while (ripple_index != -1) {
            if (data->ripples[ripple_index].key == key)
                break;
            ripple_index = flags_iterator_next(&iterator);
        }
        if (ripple_index == -1) {
            iterator.word_index = ~used[0];
            ripple_index = bit_vector_lowest_bit_set((const unsigned long *)&iterator, 32);
            if (ripple_index != -1) {
                used[ripple_index >> 5] = 1 << (ripple_index & 31) | used[ripple_index >> 5];
                data->ripples[ripple_index].key = key;
                data->ripples[ripple_index].offset = -1;
            }
        }
        if (ripple_index != -1) {
            s_water_interaction_ripple *ripple = &data->ripples[ripple_index];
            short old_offset = ripple->offset;
            int new_offset = water_interaction_ripples_find_best_offset(old_offset, period, amplitude);
            if (old_offset != -1) {
                int offset = old_offset;
                if (offset < 30) {
                    float *amplitude_at = &data->ripple_amplitudes[offset];
                    do {
                        *amplitude_at -= amplitude;
                        offset += period;
                        amplitude_at += period;
                    } while (offset < 30);
                }
            }
            int offset = new_offset;
            if (new_offset < 30) {
                int stride = period * sizeof(float);
                char *address = (char *)&data->ripple_amplitudes[new_offset];
                do {
                    *(float *)address += amplitude;
                    offset += period;
                    address += stride;
                } while (offset < 30);
            }
            ripple->offset = (short)new_offset;
            long start_time = game_time_globals->time;
            ripple->amplitude = amplitude;
            ripple->period = (short)period;
            ripple->start_time = start_time;
            result = new_offset;
        }
    }
    return result;
}

float game_time_get_safe_in_seconds()
{
    if (game_time_globals != 0 && game_time_globals->initialized)
        return game_ticks_to_seconds((float)game_time_globals->time);
    return 0.0f;
}

template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_time_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x1d6ed8: assumption keeps the zero alignment-bits load (li r7,0) in reserve_memory.
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_register_member(3, name, type, size, alignment_bits, __tls_set_g_cinematic_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

void game_time_initialize()
{
    
game_time_globals = (game_time_globals_definition *)g_game_time_globals_allocator.reserve_memory(
        "game time globals", 0, sizeof(game_time_globals_definition), 0);
    memset(game_time_globals, 0, sizeof(game_time_globals_definition));
}
void game_time_initialize_for_new_map()
{
    memset(game_time_globals, 0, sizeof(game_time_globals_definition));
    // match: 0x1d6f30: preserve the original volatile TLS read during new-map initialization.
    game_time_globals->ticks_per_second = static_cast<game_globals_storage *volatile &>(game_globals)->options.game_tick_rate;
    game_time_globals->tick_length = 1.0f / (float)game_time_globals->ticks_per_second;
    game_time_globals->rate_scale = 1.0f;
    game_time_globals->initialized = true;
}
void game_time_dispose_from_old_map()
{
    if (game_time_globals != 0 && game_time_globals->initialized)
        game_time_globals->initialized = false;
}

// @address 0x5bec98 ?game_time_dispose@@YAXXZ
void game_time_dispose()
{
}
void game_time_set_rate_scale_direct(float rate_scale)
{
    // match: 0x1d7098: preserve the original volatile TLS read for the direct rate-scale options.
    const s_game_options *options = &static_cast<game_globals_storage *volatile &>(game_globals)->options;
    float scale = rate_scale > 0.01f ? rate_scale : 0.01f;
    int ticks_per_second = (int)((float)options->game_tick_rate / scale + 0.5f);
    game_time_globals->ticks_per_second = (short)ticks_per_second;
    game_time_globals->tick_length = 1.0f / (float)ticks_per_second;
    game_time_globals->rate_scale = (float)options->game_tick_rate / (float)ticks_per_second;
}

// @address 0x3cc64 k_game_time_controller_pause_reasons
// @rdata 0x8203cc64..0x8203cc74
extern "C" {
const long k_game_time_controller_pause_reasons[4] = { 3, 4, 5, 6 };
}
// @rdata-end

static __forceinline bool game_time_pause_reason_test(long reason)
{
    if (game_time_globals->initialized)
        return (game_time_globals->flags >> reason) & 1;
    return false;
}

static __forceinline void game_time_pause_flag_set(long bit, bool set)
{
    long mask = 1 << bit;
    unsigned short flags;
    if (set)
        flags = mask | game_time_globals->flags;
    else
        flags = game_time_globals->flags & ~mask;
    game_time_globals->flags = flags;
}

static __forceinline long game_time_pausing_allowed(game_globals_storage *game_globals)
{
    if (game_globals->options.game_mode == 1 && game_campaign_has_multiple_players()) {
        bool local_simulation = true;
        if (game_globals->options.game_simulation == 2 || game_globals->options.game_simulation == 4)
            local_simulation = false;
        if (local_simulation && network_squad_session_get_peer_count() <= 1)
            return true;
        return false;
    }
    return true;
}


static __forceinline bool game_time_pause_reason_mask_test(long reason)
{
    if (game_time_globals->initialized)
        return (game_time_globals->flags & (1 << reason)) != 0;
    return false;
}





// 0x1d6fe8
static __forceinline bool game_time_user_interface_active()
{
    return bink_globals.active && bink_globals.enabled;
}

// @address 0x1d6fe8 game_time_set_pause_reason
extern "C" void game_time_set_pause_reason(bool set, long reason)
{
    game_time_globals_definition *globals = game_time_globals;
    if (set)
        globals->flags = (1 << reason) | globals->flags;
    else
        globals->flags = globals->flags & ~(1 << reason);
    game_time_update_pause_state();
    cinematic_set_paused(game_time_is_paused());
    if (game_time_is_paused()) {
        if (g_sound_manager_globals->paused != 1)
            sound_pause(_sound_pause_paused, true);
    } else if (g_sound_manager_globals->paused != 0) {
        sound_pause(_sound_pause_unpaused, false);
    }
}

// @address 0x1d7148 game_time_update_pause_state
extern "C" void game_time_update_pause_state(void)
{
    game_time_globals_definition *globals = game_time_globals;
    if (!globals->initialized || (globals->flags & 1))
        return;
    // match: 0x1d7148: preserve the original volatile TLS-slot read before setting the pause flags.
    game_globals_storage *game = static_cast<game_globals_storage *volatile &>(game_globals);
    // match: 0x1d7148: volatile pointer cast preserves memory access order
    ((volatile game_time_globals_definition *)globals)->flags |= 1;

    unsigned char can_pause = game_time_pausing_allowed(game);
    if (game_time_user_interface_active())
        can_pause = false;

    if (flag_test(globals->flags, 2) && !can_pause)
        game_time_set_pause_reason(0, 2);

    const long *pause_reasons = k_game_time_controller_pause_reasons;
    e_controller_index controller_index = _controller0;
    do {
        c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
        // match: 0x1d7148: unsigned-short read keeps the controller flag test an unsigned compare (cmplwi).
        if (controller_in_use(controller) && !((unsigned short)controller->flags.m_storage & 1) && can_pause && controller->user_index != -1) {
            game_time_set_pause_reason(1, pause_reasons[controller_index]);
        } else if (game_time_pause_reason_mask_test(pause_reasons[controller_index])) {
            game_time_set_pause_reason(0, pause_reasons[controller_index]);
        }
        controller_index = next_controller(controller_index);
    } while (controller_index != _controller_none);

    if (g_message_globals.flag) {
        if (can_pause && !game_time_pause_reason_test(7))
            game_time_set_pause_reason(1, 7);
    } else if (game_time_pause_reason_test(7)) {
        game_time_set_pause_reason(0, 7);
    }
    game_time_pause_flag_set(0, false);
}
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_time_globals_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
void __tls_set_g_game_time_globals_allocator(void *allocation)
{
    cinematic_globals = (s_cinematic_globals_definition *)allocation;
}

template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_time_globals_allocator>::deallocate(
    void *allocation)
{
    long member_index = m_member_index;
    __tls_set_g_game_time_globals_allocator(0);
    restricted_region_free_member(3, member_index);
}



static __forceinline bool game_time_saved_film_in_progress()
{
    // match: 0x1e1d88: preserve the original volatile TLS read in the saved-film predicate.
    game_globals_storage *globals = static_cast<game_globals_storage *volatile &>(game_globals);
    bool result = false;
    if (globals && globals->in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}

// 0x1e1d88
// @address 0x1e1d88 game_time_get_film_ticks_remaining
extern "C" unsigned char game_time_get_film_ticks_remaining(long *out)
{
    bool result = false;
    s_saved_film_timer_globals *timers = &g_saved_film_timer_globals;
    if (game_time_saved_film_in_progress() && game_is_cooperative() && timers->film.get_mode() == 0) {
        long limit = timers->timer_limit;
        long now = g_saved_film_playback_tick.peek();
        if (timers->timer_limit != -1 && limit >= now) {
            *out = limit - now;
            result = 1;
        }
    }
    return result;
}

class c_simulation_world;



// match: 0x983e0: helper boundary retains the networked predicate materialization.
static __forceinline bool game_time_is_networked(const s_game_options *options)
{
    bool result = false;
    if ((unsigned long)(options->game_simulation - 2) <= 3)
        result = true;
    return result;
}

static __forceinline long game_time_simulation_time_get_maximum_available(bool *match_remote_time)
{
    long maximum_available = 0x7fffffff;
    *match_remote_time = false;
    if (g_simulation_globals.initialized)
    {
        c_simulation_world *world = g_simulation_globals.world;
        if (world->m_world_type != 0)
        {
            if (world->is_out_of_sync())
                maximum_available = 0;
            else
                maximum_available = simulation_world_get_available_updates(world, match_remote_time, 0);
        }
    }
    return maximum_available;
}

static __forceinline bool game_time_update_in_progress(const game_globals_storage *globals)
{
    bool result = false;
    if (globals && globals->in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}

static __forceinline void game_time_advance_rate_scale_timer(float &timer, float seconds)
{
    const float &previous_timer = timer;
    timer = previous_timer + seconds;
}

// @address 0x983e0
bool game_time_update(float world_seconds_elapsed, float *game_seconds_elapsed_out, long *game_ticks_elapsed_out)
{
    bool result = false;
    float game_seconds_elapsed = 0.0f;
    float game_ticks = 0.0f;
    long game_ticks_target = 0;
    long game_ticks_limit = 0;
    long game_ticks_elapsed = 0;
    float game_ticks_leftover = 0.0f;
    bool clamped = false;

    game_time_update_pause_state();
    if (game_is_cooperative())
        game_time_globals->rate_scale = g_saved_film_timer_globals.playback_speed;

    // match: 0x983e0: preserve the original volatile TLS snapshot read in game_time_update.
    const game_globals_storage *game = static_cast<game_globals_storage *volatile &>(game_globals);
    if (game_time_update_in_progress(game) && game_time_globals->rate_scale_duration > 0.0f)
    {
        game_time_advance_rate_scale_timer(game_time_globals->rate_scale_timer, world_seconds_elapsed);
        if (game_time_globals->rate_scale_timer < game_time_globals->rate_scale_duration)
        {
            float fraction = game_time_globals->rate_scale_timer / game_time_globals->rate_scale_duration;
            float scale = (1.0f - fraction) * game_time_globals->rate_scale_initial + fraction * game_time_globals->rate_scale_final;
            game_time_set_rate_scale_direct(scale);
        }
        else
        {
            game_time_set_rate_scale_direct(game_time_globals->rate_scale_final);
            game_time_globals->rate_scale_duration = 0.0f;
        }
    }

    if (game_time_update_in_progress(game) && !game_time_is_paused() && game_time_globals->rate_scale > 0.0f)
    {
        game_time_globals_definition *globals = game_time_globals;
        bool match_remote_time = false;
        bool unrestricted_time = false;
        if (inl_cinematic_in_progress())
            unrestricted_time = true;

        if (game_time_is_networked(&game->options) || game_is_cooperative())
        {
            game_ticks_limit = game_time_simulation_time_get_maximum_available(&match_remote_time);
            long film_ticks = 0;
            if (game_time_get_film_ticks_remaining(&film_ticks))
                game_ticks_limit = game_ticks_limit <= film_ticks ? game_ticks_limit : film_ticks;
        }
        else
            game_ticks_limit = globals->ticks_per_second * 5;

        if (!match_remote_time && !unrestricted_time)
        {
            float speed = (globals->rate_scale > 1.0f ? globals->rate_scale : 1.0f) < 5.0f
                ? (globals->rate_scale > 1.0f ? globals->rate_scale : 1.0f) : 5.0f;
            long maximum_ticks = (long)(2.0f * speed + 0.5f);
            game_ticks_limit = game_ticks_limit <= maximum_ticks ? game_ticks_limit : maximum_ticks;
        }

        game_seconds_elapsed = world_seconds_elapsed * globals->rate_scale;
        game_ticks = game_seconds_elapsed * (float)globals->ticks_per_second + globals->leftover_ticks;
        game_ticks_target = (long)game_ticks;
        if (match_remote_time)
        {
            if (game_ticks_target > game_ticks_limit)
            {
                game_ticks_elapsed = game_ticks_limit - 1 < 0 ? 0 : game_ticks_limit - 1;
                clamped = true;
            }
            else if (game_ticks_target + 7 < game_ticks_limit)
            {
                game_ticks_elapsed = game_ticks_limit - 1 < 0 ? 0 : game_ticks_limit - 1;
                clamped = true;
            }
            else if (game_ticks_target + 1 < game_ticks_limit)
            {
                game_ticks_elapsed = game_ticks_target + 1;
                clamped = true;
            }
            else if (game_ticks_target + 1 > game_ticks_limit)
            {
                game_ticks_elapsed = game_ticks_target - 1 < 0 ? 0 : game_ticks_target - 1;
                clamped = true;
            }
            else
                game_ticks_elapsed = game_ticks_target;
        }
        else if (game_ticks_target > game_ticks_limit)
        {
            game_ticks_elapsed = game_ticks_limit;
            clamped = true;
        }
        else
            game_ticks_elapsed = game_ticks_target;

        if (clamped)
        {
            game_ticks_elapsed = game_ticks_limit;
            game_seconds_elapsed = (float)game_ticks_elapsed * globals->tick_length;
        }
        else
            game_ticks_leftover = game_ticks - (float)game_ticks_elapsed;
        result = true;
    }
    game_time_globals->leftover_ticks = game_ticks_leftover;
    if (game_seconds_elapsed_out)
        *game_seconds_elapsed_out = game_seconds_elapsed;
    if (game_ticks_elapsed_out)
        *game_ticks_elapsed_out = game_ticks_elapsed;
    return result;
}


game_time_globals_allocator_type g_game_time_globals_allocator;
