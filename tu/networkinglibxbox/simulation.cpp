// networkinglibxbox/simulation.obj: from the records 0x80618 0x90958 0x96d90 0x97fa8 0x98258 0x98308 0xa4098 0xa4320 0xa4a40 0xa5240 0xa5fd8 0xa6118 0xa6830 0x11aac8 0x198250 0x1ad340 0x1ad408 0x1ce428 0x281228 0x39f290 0x39f300 0x39f410 0x39f458 0x39f5b0 0x39f6c0 0x39f790 0x39f7a8 0x39faa0 0x39fb18 0x39fc68 0x39fce0 0x39fda0 0x39fdd8 0x39ff30 0x3a0038 0x3a0128 0x3a0350 0x3a03f8 0x3a0550 0x3a0680 0x3a07c8 0x3a0a50
// @flags /TP /O2
#include "tag_files/files.h"
#include "simulation/simulation_watcher_types.h"
#include "saved_games/saved_film_work_types.h"
#include "cseries/crt_xdk.h"
#include "simulation/simulation_services.h"
#include "game/game_simulation_types.h"
#include "simulation/simulation_queue_global_event_types.h"
#include "memory/data_types.h"
#include <stdio.h>
#include "memory/static_arrays.h"
#include "game/game_time_types.h"
#include "game/player_types.h"
#include "shell/shell.h"
#include "objects/object_iterator.h"
#include "units/unit_control.h"
#include "game/game_globals_storage.h"
#include <stdarg.h>
#include <stdlib.h>
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "objects/objects.h"
#include "simulation/simulation.h"
#include "units/units.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "game/game_results.h"
#include "main/main_game.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/logic/network_life_cycle.h"
#include "simulation/game_interface/simulation_game_interface.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/simulation_players.h"
#include "simulation/simulation_actors.h"
#include "simulation/simulation_queue_global_events.h"
#include "saved_games/game_state_procs.h"
#include "saved_games/saved_film_snippet.h"
#include "saved_games/saved_film_runtime.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_queue_types.h"
#include "networking/online/voice_observer_types.h"
#include "networking/delivery/network_channel.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "game/players_globals.h"
#include "memory/data.h"
#include "memory/crc.h"
#include "networking/network_memory.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "interface/c_controller.h"
#include "simulation/simulation_world.h"
#include "memory/bitstream.h"
#include "simulation/simulation_connection_statistics_types.h"
#include "simulation/watcher_machine_collection_types.h"
#include "simulation/simulation_machine_update_entry_types.h"
#include "simulation/simulation_view_reason_types.h"
#include "saved_games/saved_film_update_types.h"
#include "simulation/simulation_player_netdebug_data_types.h"
#include "networking/online/voice_xenon.h"
#include "networking/network_memory_types.h"
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/session/network_session_parameter_observer.h"
#include "simulation/simulation_globals.h"
#include "simulation/simulation_world_types.h"
#include "networking/replication/replication_motion.h"
#include "simulation/simulation_machine_update_types.h"
#include "simulation/simulation_queue.h"
#include "saved_games/saved_film_globals.h"
#include "saved_games/saved_film_recording_state.h"
#include "saved_games/saved_film_tick.h"
#include "simulation/simulation_update_types.h"
#include "simulation/simulation_update_metadata_types.h"
#include "simulation/simulation_distributed_view.h"
#include "simulation/simulation_view.h"
#include "networking/replication/replication_encoding.h"
#include "simulation/simulation_players_types.h"
#include "networking/simulation/simulation_type_collection.h"
#include "networking/network_memory_base_types.h"
#include <string.h>
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "game/game_engine_scoring_globals.h"
#include "objects/object_datum.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_match_internals.h"
#include "networking/network_time_types.h"
#include "saved_games/saved_film_recording_types.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "multithreading/compiler_barrier.h"
#include "memory/game_state_globals.h"
#include "cseries/async_helpers_internal.h"
#include "multithreading/threads.h"
#include "game/players_globals_types.h"
#include "networking/network_time_access.h"
#include "cseries/cseries.h"
#include "game/game_simulation_predicates.h"
class c_async_double_buffer;
class c_simulation_watcher;
class c_simulation_type_collection;
class c_simulation_world;
class c_simulation_distributed_world;
class c_game_results;
struct s_simulation_distributed_view;
struct s_world_update_queue;

// @address 0x11df780 g_simulation_gamestate_entity_data
// @address 0x198250 ??_H@YAXPAXIHP6APAX0@Z@Z
// @address 0x80618 ?simulation_starting_up@@YA_NXZ
// @address 0x1ad408 ?print@?$c_static_string@$0BAE@@@QAAPBDPBDZZ
// @address 0x39f790 ?simulation_prepare_to_load_saved_game@@YAXJ@Z

// @address 0x808560 g_simulation_globals

__declspec(thread) s_data_array *g_simulation_gamestate_entity_data;

bool simulation_starting_up(void)
{
  bool result = false;
  game_globals_storage *game_globals = tls_game_globals();
  if (game_globals && game_globals->game_in_progress && !game_globals->initializing &&
      game_globals->map_active)
    result = true;
  return result;
}

// @address 0x90958 ?simulation_aborted@@YA_NXZ
bool simulation_aborted()
{
    return g_simulation_globals.initialized && g_simulation_globals.aborted;
}

// @address 0x98258 simulation_is_pending_start
extern "C" bool simulation_is_pending_start(void)
{
  bool result = false;
  if (g_simulation_globals.initialized && !g_simulation_globals.aborted && g_simulation_globals.world->m_world_type != 0)
    result = g_simulation_globals.world->m_world_state != 4;
  return result;
}

// @address 0x98308 ?simulation_in_progress@@YA_NXZ
static __forceinline bool game_step_a(void)
{
  return simulation_starting_up() && tls_game_globals()->active_structure_bsp_mask != 0;
}
static __forceinline bool game_step_b(void)
{
  return game_step_a() && !g_simulation_globals.aborted;
}
static __forceinline bool game_step_c(void)
{
  return game_step_b() && !g_simulation_globals.film_update_pending;
}
bool simulation_in_progress(void)
{
  bool result = false;
  if (g_simulation_globals.initialized)
    result = game_step_c() && g_simulation_globals.world->m_world_type != 0 && g_simulation_globals.world->m_world_state == 4;
  return result;
}


// @address 0x94e3b0 g_saved_film_timer_globals
// @address 0x94e3ac g_saved_film_playback_tick


// @address 0xa5240 simulation_build_player_and_actor_update
extern "C" void simulation_build_player_and_actor_update(c_simulation_world *world, simulation_update *update)
{
    update->player_valid_mask = 0;
    update->action_test_flags = 0;
    update->actor_valid_mask = 0;
    update->prediction_mask = 0;
    for (long player_index = 0; player_index < 16; player_index++)
    {
        if (world->m_players[player_index].exists())
        {
            unsigned long action_flags = 0;
            if (world->m_players[player_index].generate_action(&update->actions[player_index], &action_flags))
            {
                update->player_valid_mask |= 1 << player_index;
                update->action_test_flags |= action_flags;
            }
        }
    }
    for (long player_index = 0; player_index < 16; player_index++)
    {
        c_simulation_player *player = &world->m_players[player_index];
        if (player->exists() && player->is_predictive())
        {
            s_player_prediction *prediction = &update->predictions[player_index];
            if (player->generate_prediction(prediction))
            {
                prediction->identifier = simulation_entity_get_gamestate_index(prediction->entity);
                update->prediction_mask |= 1 << player_index;
            }
        }
    }
    for (long actor_index = 0; actor_index < 16; actor_index++)
    {
        if (world->m_actors[actor_index].exists() &&
            world->m_actors[actor_index].generate_control(&update->actor_unit_indices[actor_index], &update->actor_controls[actor_index]))
            update->actor_valid_mask |= 1 << actor_index;
    }
}
// @address 0x11aac8 ?simulation_destroy_update@@YAXPAUsimulation_update@@@Z
void simulation_destroy_update(simulation_update *update)
{
  c_simulation_queue *queue = &update->bookkeeping;
  if (queue->initialized) {
    queue->clear();
    queue->initialized = false;
  }
  queue = &update->simulation;
  if (queue->initialized) {
    queue->clear();
    queue->initialized = false;
  }
}

/* Appends a string to a 260-character static string. */


// @address 0x1ad340 static_string_append_bounded
extern "C" char *static_string_append_bounded(char *string, const char *source)
{
  unsigned long length = csstrnlen(string, 260);
  strncpy(string + length, source, 260 - length);
  string[259] = '\0';
  return string;
}

template<long k_maximum_count>
const char *c_static_string<k_maximum_count>::print(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  _vsnprintf(m_string, k_maximum_count - 1, format, args);
  va_end(args);
  m_string[k_maximum_count - 1] = 0;
  return m_string;
}
template const char *c_static_string<260>::print(const char *format, ...);
extern template const char *c_static_string<128>::print(const char *format, ...);


// @address 0x1ce428 bitstream_initialize
extern "C" c_bitstream *bitstream_initialize(c_bitstream *bitstream, const unsigned char *data, long data_size)
{
    bitstream->m_data = (unsigned char *)data;
    bitstream->m_data_size_alignment = 1;
    bitstream->m_data_max = (unsigned char *)data + data_size;
    bitstream->m_data_size_bytes = data_size;
    bitstream->reset(0);
    return bitstream;
}

namespace {
static __forceinline void simulation_gamestate_entities_dispose()
{
    if (g_simulation_gamestate_entity_data)
        g_simulation_gamestate_entity_data = 0;
}
}
static __forceinline bool simulation_world_type_is_client(long type)
{
    return type != 4 && type != 6 && type != 8;
}
static __forceinline bool simulation_world_type_is_networked(long type)
{
    return type == 2 || type == 5 || type == 6;
}
// @address 0x39f290 ?is_out_of_sync@c_simulation_world@@QBA_NXZ


// @address 0x39f300 ?simulation_initialize@@YAXXZ
void simulation_initialize(void)
{
  s_data_array *data_new_result;
  c_simulation_type_collection *type_collection;
  long stored_event_count;
  long stored_entity_count;
  struct { long entity_count; long event_count; } out;
  data_new_result = (s_data_array *)data_new("sim. gamestate entities", 2048, 16, 0, (c_allocation_base *)&g_simulation_gamestate_entity_data_allocator);
  g_simulation_gamestate_entity_data = data_new_result;
  g_simulation_globals.world = &network_base_memory_globals.world;
  g_simulation_globals.watcher = &network_base_memory_globals.watcher;
  g_simulation_globals.type_collection = &network_base_memory_globals.types;
  g_simulation_globals.type_collection->initialize();
  simulation_game_register_types((c_simulation_type_collection *)g_simulation_globals.type_collection, &out.entity_count, &out.event_count);
  type_collection = g_simulation_globals.type_collection;
  stored_entity_count = out.entity_count;
  stored_event_count = out.event_count;
  type_collection->finish_types(stored_entity_count, stored_event_count);
  g_simulation_globals.reset_in_progress = false;
  g_simulation_globals.recording_film = false;
  g_simulation_globals.initialized = true;
}

// @address 0x39f410 ?simulation_dispose@@YAXXZ
void simulation_dispose()
{
    simulation_gamestate_entities_dispose();
    if (g_simulation_globals.initialized) {
        g_simulation_globals.world = 0;
        g_simulation_globals.watcher = 0;
        g_simulation_globals.type_collection = 0;
        g_simulation_globals.initialized = false;
    }
}

namespace {
static __forceinline bool simulation_world_exists(const c_simulation_world *world)
{
    return world->m_world_type != 0;
}
static __forceinline void simulation_world_iterator_begin(s_simulation_world_view_iterator *iterator, unsigned int mask)
{
    iterator->type_mask = mask;
    iterator->index = 0;
}
static __forceinline c_simulation_view *simulation_world_get_view_by_channel(c_simulation_world *world, c_network_channel *channel)
{
    c_simulation_view *view = 0;
    s_simulation_world_view_iterator iterator;
    simulation_world_iterator_begin(&iterator, -1);
    c_simulation_view *test_view;
    while (world->iterator_next(&iterator, &test_view)) {
        if (test_view->get_channel() == channel) {
            view = test_view;
            break;
        }
    }
    return view;
}
}

static __forceinline bool simulation_dispose_main_game_reset_in_progress()
{
    return main_game_globals.reset_in_progress;
}
// @address 0x39f5b0 ?simulation_dispose_from_old_map@@YAXXZ
void simulation_dispose_from_old_map()
{
    c_voice_observer *voice = &g_voice_observer;
    for (long i = 0; i < 16; ++i)
        voice->m_player_push_to_talk_sound_index[i] = -1;
    if (!simulation_dispose_main_game_reset_in_progress()) {
        c_simulation_watcher *watcher = g_simulation_globals.watcher;
        c_network_observer *observer = watcher->m_observer;
        if (observer) {
            observer->m_owners[3].owner = 0;
            observer->m_owners[3].session_index = -1;
            watcher->m_observer = 0;
        }
        watcher->m_session = 0;
        watcher->m_world = 0;
        watcher->m_initial_player_update = false;
        ((c_simulation_world *)g_simulation_globals.world)->destroy_world();
        if (game_engine_running()) {
            if (g_game_results_globals.active != 0)
                g_game_results_globals.active = 0;
            char *results = ((char *)&g_current_game_results);
            if (results[1] == 0) {
                results[2] = 1;
                results[1] = 1;
                game_results_grant_top_player_achievement(results);
            }
        }
        if (!simulation_dispose_main_game_reset_in_progress())
            g_simulation_gamestate_entity_data->valid = false;
        if (g_simulation_globals.recording_film != 0) {
            saved_film_dispose();
            g_simulation_globals.recording_film = 0;
        }
        g_simulation_globals.reset_in_progress = false;
    }
}
namespace {
static __forceinline bool simulation_world_player_exists(const c_simulation_world *world, long player_index)
{
    long index = (unsigned short)player_index;
    bool result = false;
    if (index >= 0 && index < 16u) {
        if (world->m_players[index].exists())
            result = true;
    }
    return result;
}
static __forceinline void simulation_delete_player(c_simulation_world *world, long player_index)
{
    unsigned long index = (unsigned short)player_index;
    c_simulation_player *player = &world->m_players[index];
    player->m_control_index = -1;
    player->m_player_index = -1;
    player->m_state = -1;
    player->m_world = 0;
    s_simulation_world_view_iterator iterator;
    simulation_world_iterator_begin(&iterator, 20);
    c_simulation_view *view;
    while (((const c_simulation_world *)world)->iterator_next(&iterator, &view)) {
        if (view->established())
            view->force_unacknowledge_player(index);
    }
}
}
namespace {
static __forceinline long simulation_game_simulation_type_get(void)
{
    return (signed char)tls_game_globals()->options.simulation_type;
}
}

// @address 0x39f6c0 ?simulation_reset_world_for_authority@@YAXXZ
void simulation_reset_world_for_authority(void)
{
    if (g_simulation_globals.skip_next_world_reset) {
        g_simulation_globals.skip_next_world_reset = false;
        return;
    }
    c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
    g_simulation_globals.reset_in_progress = true;
    world->reset_world();
    long world_type = ((c_simulation_world *)g_simulation_globals.world)->m_world_type;
    bool proceed = true;
    if (world_type == 2 || (unsigned long)(world_type - 4) <= 2)
        proceed = false;
    if (proceed) {
        s_simulation_queue_gamestate_clear_data clear_data;
        simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_main_save_map);
        simulation_gamestate_entities_build_clear_flags(&clear_data);
        simulation_queue_gamestates_delete_insert(&clear_data);
        simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_main_reset_map_complete);
        return;
    }
    g_simulation_globals.reset_in_progress = false;
}

void simulation_prepare_to_load_saved_game(long)
{
    g_simulation_globals.loading_saved_game = true;
}

// @address 0x39faa0 ?simulation_player_joined_game@@YAXJ@Z
void simulation_player_joined_game(long player_index)
{
    if (g_simulation_globals.initialized && !g_simulation_globals.loading_saved_game) {
        c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
        if (!world->is_playback())
            ((c_simulation_world *)world)->create_player(player_index, (e_game_simulation_type)simulation_game_simulation_type_get());
    }
}
// @address 0x39fb18 simulation_delete_world_player
extern "C" void simulation_delete_world_player(long player_index)
{
    if (g_simulation_globals.initialized && !g_simulation_globals.loading_saved_game) {
        c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
        if (!world->is_playback()) {
            if (simulation_world_player_exists(world, player_index))
                simulation_delete_player(world, player_index);
        }
    }
}

// @address 0x39fc68 ?simulation_stop@@YAXXZ
void simulation_stop(void)
{
    if (g_simulation_globals.initialized) {
        c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
        if (world->m_attached_to_map && !g_simulation_globals.reset_in_progress && !main_game_globals.reset_in_progress) {
            world->remove_all_views();
            world->m_attached_to_map = false;
        }
    }
}

// @address 0x39fce0 ?simulation_get_remote_view_by_channel@@YAPAVc_simulation_view@@PAVc_network_channel@@@Z
c_simulation_view *simulation_get_remote_view_by_channel(c_network_channel *channel)
{
    c_simulation_view *view = 0;
    if (g_simulation_globals.initialized && simulation_world_exists(g_simulation_globals.world))
        view = simulation_world_get_view_by_channel((c_simulation_world *)g_simulation_globals.world, channel);
    return view;
}

// The distributed-view storage network_memory owns (the vtable members its constructors set).


// @address 0xa4a40 ?simulation_build_player_updates@@YAXXZ
void simulation_build_player_updates(void)
{
    long update_count = 0;
    simulation_player_update updates[64];
    ((c_simulation_watcher *)g_simulation_globals.watcher)->generate_player_updates(
        (long *)(&update_count), 64, updates);
    long remaining = update_count;
    if (remaining > 0) {
        simulation_player_update *update = updates;
        do {
            ::simulation_queue_player_update_insert(update);
            update++;
            remaining--;
        } while ((unsigned long)remaining != 0);
    }
}
namespace {
static __forceinline void simulation_gamestate_entities_initialize_for_new_map()
{
    s_data_array *entities = (s_data_array *)g_simulation_gamestate_entity_data;
    entities->valid = true;
    data_delete_all(entities);
    data_delete_all((s_data_array *)g_simulation_gamestate_entity_data);
}
}

// @address 0x39f458 ?simulation_initialize_for_new_map@@YAXXZ
void simulation_initialize_for_new_map(void)
{
    if (!simulation_dispose_main_game_reset_in_progress()) {
        c_simulation_distributed_world *distributed_world = 0;
        simulation_gamestate_entities_initialize_for_new_map();
        g_simulation_globals.fatal_error = false;
        g_simulation_globals.aborted = false;
        g_simulation_globals.film_update_pending = false;
        g_simulation_globals.shutdown_requested = false;
        simulation_film_start_recording();
        if (game_simulation::game_is_distributed() && !game_is_cooperative())
            distributed_world = (c_simulation_distributed_world *)network_shared_memory_globals.distributed_world;
        c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
        c_simulation_watcher *watcher = (c_simulation_watcher *)g_simulation_globals.watcher;
        world->initialize_world((e_game_simulation_type)simulation_game_simulation_type_get(), tls_game_globals()->options.game_playback, true,
            (c_simulation_type_collection *)g_simulation_globals.type_collection, watcher, distributed_world);
        ((c_simulation_watcher *)g_simulation_globals.watcher)->initialize_watcher((c_simulation_world *)g_simulation_globals.world);
        ((c_simulation_watcher *)g_simulation_globals.watcher)->setup_connection();
        game_results_initialize_for_new_map();
        g_simulation_globals.skip_next_world_reset = true;
    }
}
namespace {
static __forceinline bool simulation_remove_view_exists(const c_simulation_view *view)
{
    return view->m_view_type != 0;
}
static __forceinline c_simulation_world *simulation_remove_view_get_world(const c_simulation_view *view)
{
    return view->m_world;
}
static __forceinline void simulation_remove_view_terminate_catchup(c_simulation_view *view)
{
    if (view->m_catchup_attempt != -1) {
        view->m_catchup_attempt = -1;
        view->m_catchup_data = 0;
        view->m_catchup_start = -1;
        view->m_catchup_end = -1;
    }
}
static __forceinline void simulation_remove_view_detach_from_world(c_simulation_view *view)
{
    if (view->m_view_type == 2 || view->m_view_type == 1)
        simulation_remove_view_terminate_catchup(view);
    c_simulation_world *world = view->m_world;
    world->views[view->m_view_index] = 0;
    world->view_count--;
    view->m_view_index = -1;
    view->m_world = 0;
}
static __forceinline void simulation_remove_view_destroy_view(c_simulation_view *view)
{
    view->m_view_type = 0;
}
static __forceinline void simulation_remove_view_memory_delete(c_simulation_view *view)
{
    s_simulation_distributed_view *distributed_view = (s_simulation_distributed_view *)view->m_distributed_view;
    long datum_index = view->m_view_datum_index;
    ::datum_delete(network_shared_memory_globals.views, datum_index);
    if (distributed_view != 0) {
        s_data_array *distributed_views = network_shared_memory_globals.distributed_views;
        distributed_view->~s_simulation_distributed_view();
        ::datum_delete(distributed_views, datum_index);
    }
}
}

// @address 0x39fda0 ?simulation_notify_channel_closure@@YAXPAX@Z
void simulation_notify_channel_closure(void *channel)
{
    c_simulation_view *view = (c_simulation_view *)channel;
    view->notify_closed();
}

// @address 0x39ff30 ?simulation_remove_view_from_world@@YAXPAVc_simulation_view@@@Z
void simulation_remove_view_from_world(c_simulation_view *view)
{
    if (view != 0) {
        if (simulation_remove_view_exists(view)) {
            if (view->get_observer_channel() != -1)
                view->detach_observer_channel();
            if (simulation_remove_view_get_world(view) != 0)
                simulation_remove_view_detach_from_world(view);
            simulation_remove_view_destroy_view(view);
        }
        simulation_remove_view_memory_delete(view);
    }
}

// @address 0x39fdd8 simulation_add_view_to_world
void simulation_add_view_to_world(int view_type, const unsigned char *machine_identifier, long machine_index, const char *name)
{
    struct { s_simulation_distributed_view *distributed; c_simulation_view *view; } allocation;
    int view_datum_index;
    view_datum_index = network_shared_memory_new_view(&allocation.view, &allocation.distributed);
    if (view_datum_index != -1) {
        c_simulation_view *view = allocation.view;
        view->m_view_type = view_type;
        view->m_view_datum_index = view_datum_index;
        view->m_distributed_view = allocation.distributed;
        memcpy(&view->m_machine_identifier, machine_identifier, sizeof(view->m_machine_identifier));
        view->m_machine_index = machine_index;
        view->m_world = 0;
        view->m_view_index = -1;
        view->m_observer = 0;
        view->m_observer_channel_index = -1;
        view->m_death_reason = 0;
        view->m_establishment_mode = _simulation_view_establishment_none;
        view->m_establishment_identifier = -1;
        view->m_remote_establishment_mode = 0;
        view->m_remote_establishment_identifier = -1;
        view->m_synchronous_catchup_attempts = 0;
        view->m_catchup_attempt = -1;
        view->m_catchup_start = -1;
        view->m_catchup_end = -1;
        view->m_catchup_finish_time = -1;
        view->m_catchup_data = 0;
        view->m_channel = 0;
        view->m_channel_connection_identifier = -1;
        view->m_active = 0;
        if ((unsigned int)(1 << view_type & 20) != 0) {
            view->m_acknowledged_player_mask = 0;
        }
        if (view_type == 2) {
            view->m_synchronous_action_number = -1;
            view->m_synchronous_update_number = -1;
        } else if (view_type == 1) {
            view->m_action_number = 0;
        }
        c_simulation_world *world = g_simulation_globals.world;
        c_simulation_view **view_slot = world->views;
        long view_index = 0;
        do {
            if (*view_slot == 0) {
                world->views[view_index] = view;
                ++world->view_count;
                break;
            }
            ++view_index;
            ++view_slot;
        } while (view_index < 15);
        view->m_view_index = view_index;
        view->m_world = world;
    }
}

// @address 0x3a0038 simulation_get_machine_bandwidth_events
extern "C" bool simulation_get_machine_bandwidth_events(const s_machine_identifier *machine)
{
    bool active = false;
    if (g_simulation_globals.initialized) {
        const c_simulation_world *world = (const c_simulation_world *)g_simulation_globals.world;
        if (world->exists()) {
            s_machine_identifier local_machine;
            world->get_machine_identifier(&local_machine);
            if (memcmp(machine, &local_machine, sizeof(local_machine)) == 0)
                active = true;
            else {
                c_simulation_view *client = ((c_simulation_world *)g_simulation_globals.world)->get_client_view_by_machine_identifier(machine);
                if (client) active = client->active();
            }
        }
    } else active = true;
    return active;
}
// @address 0x97fa8 simulation_player_left_game
extern "C" unsigned long simulation_player_left_game(const c_simulation_world *w)
{
    unsigned long mask = 0;
    const c_simulation_world *world = (const c_simulation_world *)w;
    for (long i = 0; i < 16; i++) { if (world->m_players[i].exists()) mask |= 1 << i; }
    s_simulation_world_view_iterator it;
    simulation_world_iterator_begin(&it, 20);
    c_simulation_view *v;
    while (w->iterator_next(&it, &v)) {
        if (v->client_in_game()) mask &= v->get_acknowledged_player_mask();
        
    }
    return mask;
}


namespace {
static __forceinline void simulation_game_engine_detach_from_simulation()
{
  s_game_engine_globals *engine_globals = game_engine_globals;
  engine_globals->long_2c = -1;
  engine_globals->long_30 = -1;
  for (long i = 0; i < 16; ++i)
    engine_globals->longs_34[i] = -1;
}
static __forceinline void simulation_objects_detach_from_simulation()
{
  c_object_iterator<object_datum> iterator;
  iterator.begin(0, 0);
  while (iterator.next()) {
    object_datum *object = iterator.get_datum();
    if (object->gamestate_index != -1)
      object_detach_gamestate_entity(iterator.get_index(), object->gamestate_index);
  }
}
static __forceinline void simulation_clear_errors()
{
  g_simulation_globals.aborted = false;
  g_simulation_globals.fatal_error = false;
}
}

// @address 0x39f7a8 ?simulation_initialize_for_saved_game@@YAXJ@Z
void simulation_initialize_for_saved_game(long flags)
{
  if (flags & 2) {
    if (network_life_cycle_globals_get()->manager.get_current_state() == _life_cycle_state_in_game)
      network_life_cycle_globals_get()->in_game.get_flags().bits |= 2;
    network_life_cycle_end();
    if (simulation_game_simulation_type_get() == 1 && ((c_simulation_world *)g_simulation_globals.world)->is_local()) {
      ((c_simulation_world *)g_simulation_globals.world)->disconnect();
      simulation_clear_errors();
    } else {
      simulation_dispose_from_old_map();
      if (game_simulation::game_is_distributed() && !game_is_cooperative()) {
        simulation_game_engine_detach_from_simulation();
        simulation_objects_detach_from_simulation();
      }
      game_simulation_set(_game_simulation_local);
      simulation_initialize_for_new_map();
    }
  }
  switch (((c_simulation_world *)g_simulation_globals.world)->m_world_type) {
  case 3:
    game_simulation_set(_game_simulation_synchronous_server);
    break;
  case 4:
    game_simulation_set(_game_simulation_synchronous_client);
    break;
  case 7:
    game_simulation_set(_game_simulation_distributed_server);
    break;
  case 8:
    game_simulation_set(_game_simulation_distributed_client);
    break;
  case 1:
    game_simulation_set(_game_simulation_local);
    break;
  case 0:
    game_simulation_set(_game_simulation_none);
    break;
  case 2:
  case 5:
  case 6:
    break;
  default:
    // match: 0x39f7a8: unreachable hint omits dead branch check
    __assume(0);
  }
  switch (((c_simulation_world *)g_simulation_globals.world)->m_world_type) {
  case 0:
  case 1:
  case 3:
  case 4:
  case 7:
  case 8:
    tls_game_globals()->options.game_playback = 0;
    break;
  case 2:
    tls_game_globals()->options.game_playback = 1;
    break;
  case 5:
    tls_game_globals()->options.game_playback = 2;
    break;
  case 6:
    tls_game_globals()->options.game_playback = 3;
    break;
  default:
    // match: 0x39f7a8: unreachable hint omits dead branch check
    __assume(0);
  }
  ((c_simulation_world *)g_simulation_globals.world)->delete_all_players();
  ((c_simulation_watcher *)g_simulation_globals.watcher)->handle_new_gamestate(flags);
  ((c_simulation_world *)g_simulation_globals.world)->recreate_players((e_game_simulation_type)simulation_game_simulation_type_get());
  if (game_is_cooperative())
    players_remap_cooperative_users(1);
  g_simulation_globals.loading_saved_game = false;
}


// @address 0x3a0128 simulation_film_read_updates
extern "C" unsigned char simulation_film_read_updates(long maximum_updates, long *updates_read)
{
  bool success = true;
  long update_count = 0;
  long update_limit = (long)((float)game_time_globals->ticks_per_second * game_time_globals->rate_scale * 0.25f + 0.5f);
  {
    long queued_updates;
    simulation_world_get_available_updates((c_simulation_world *)g_simulation_globals.world, 0, &queued_updates);
    if (queued_updates == 0 && update_limit < 1)
      update_limit = 1;
  }
  if (update_limit > maximum_updates)
    update_limit = maximum_updates;
  do {
    long queued_updates;
    simulation_world_get_available_updates((c_simulation_world *)g_simulation_globals.world, 0, &queued_updates);
    if (queued_updates >= update_limit)
      break;
    long size = g_saved_film_timer_globals.film.get_async_buffer()->get_position();
    s_saved_film_update film_update;
    if (saved_film_try_and_get_update_header(&film_update.type) && film_update.type == 0) {
      simulation_update update;
      s_simulation_update_metadata metadata;
      memset(&update, 0, sizeof(update));
      memset(&metadata, 0, sizeof(metadata));
      metadata.async_buffer_position = size;
      metadata.flags |= 4;
      metadata.film_storage_used = g_saved_film_timer_globals.film.get_storage_used();
      c_saved_film *film = &g_saved_film_timer_globals.film;
      if (film->read_simulation_update(&film_update, &update)) {
        c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
        bool handled = false;
        long expected_update_number;
        if (world->update_queue_reset_value == 1)
          expected_update_number = world->last_update_number;
        else if (world->update_queue_reset_value == 0)
          expected_update_number = world->last_update_number + 1;
        else
          expected_update_number = world->update_queue_applied_number;
        if (expected_update_number != -1 && update.update_number != expected_update_number)
          simulation_world_set_out_of_sync(world, false);
        else if (world->update_queue_handle_server_update(&update, &metadata))
          handled = true;
        else
          g_simulation_globals.fatal_error = true;
        if (!handled) {
          simulation_destroy_update(&update);
          success = 0;
        }
      } else {
        success = 0;
      }
      update_count++;
    } else {
      success = 0;
    }
  } while (success && update_count < 30);
  *updates_read = update_count;
  return success;
}

// @address 0x3a0350 ?simulation_abort_immediate@@YAXW4e_simulation_abort_reason@@@Z
void simulation_abort_immediate(e_simulation_abort_reason reason)
{
    if (simulation_world_exists(g_simulation_globals.world) && !g_simulation_globals.aborted) {
        g_simulation_globals.aborted = true;
        g_simulation_globals.abort_reason = reason;
        g_simulation_globals.abort_time = network_time_get();
        c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
        if (world->m_world_state != _simulation_world_state_dead) {
            world->disconnect();
            world->change_state_internal(_simulation_world_state_dead);
        }
    }
}

static __forceinline void simulation_get_game_description(c_static_string<260> *description)
{
    const game_options *options = &tls_game_globals()->options;
    const char *scenario_path = options->scenario_path;
    const char *scenario_name = strrchr(scenario_path, '\\');
    scenario_name = scenario_name ? scenario_name + 1 : scenario_path;
    char short_name[8];
    strncpy(short_name, scenario_name, sizeof(short_name));
    short_name[sizeof(short_name) - 1] = 0;
    description->print("%s_%08X", short_name, options->game_instance >> 32);
}
static __forceinline bool simulation_saved_film_write_open(const char *description, const game_options *options)
{
    bool success = false;
    if (g_saved_film_timer_globals.film.get_mode() == -1)
    {
        c_static_string<128> file_path;
        file_path.print("%s\\%s_%s.temp", k_saved_game_autosave_directory, "asq", description);
        success = saved_film_new_recording(
            &g_saved_film_timer_globals.film, file_path.get_string(), options, controller_get_first_signed_in_non_guest());
        if (success)
        {
            strncpy(g_saved_film_timer_globals.name, description, sizeof(g_saved_film_timer_globals.name));
            g_saved_film_timer_globals.name[sizeof(g_saved_film_timer_globals.name) - 1] = 0;
        }
        else
        {
            g_saved_film_timer_globals.name[0] = 0;
        }
    }
    return success;
}
// @address 0x3a03f8 ?simulation_film_start_recording@@YA_NXZ
bool simulation_film_start_recording()
{
    const game_options *options = &tls_game_globals()->options;
    bool success = false;
    if (g_simulation_globals.recording_film)
    {
        saved_film_dispose();
        g_simulation_globals.recording_film = false;
    }
    if (options->record_saved_film)
    {
        c_static_string<260> description;
        simulation_get_game_description(&description);
        bool recording = simulation_saved_film_write_open(description.get_string(), options);
        g_simulation_globals.recording_film = recording;
        if (recording)
            success = true;
        else
            saved_film_dispose();
    }
    return success;
}

static __forceinline bool simulation_update_abort_check_squad_session(c_network_session **session)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    c_network_session *current = (c_network_session *)network_life_cycle_globals_get()->manager.m_active_squad_session;
    if (current->m_local_state != 0) {
      if (session)
        *session = current;
      result = true;
    }
  }
  return result;
}

static __forceinline bool simulation_update_abort_check_game_state_allows_update(const c_simulation_world *state)
{
  return state->m_world_type != 4 && state->m_world_type != 6 && state->m_world_type != 8;
}

// @address 0x3a0550 ?simulation_update_abort_check@@YAXXZ
void simulation_update_abort_check(void)
{
  bool update = false;
  c_simulation_world *state = g_simulation_globals.world;
  if (state->is_out_of_sync() && simulation_update_abort_check_game_state_allows_update(state))
    update = true;
  c_network_session *session = 0;
  if (simulation_update_abort_check_squad_session(&session)) {
    const c_network_session_parameters *parameters = (const c_network_session_parameters *)session->m_session_parameters;
    if (parameters->m_parameters.synchronous_out_of_sync.get_allowed() && parameters->m_parameters.synchronous_out_of_sync.get()->report_out_of_sync)
      update = true;
  }
  if (update) {
    saved_film_dispose();
    simulation_abort_immediate(_simulation_abort_reason_7);
  }
}

// @address 0x3a0680 ?simulation_get_view_netdebug_data@@YA_NPAVc_simulation_view@@PAJ111@Z
bool simulation_get_view_netdebug_data(c_simulation_view *view, long *rtt_msec, long *packet_rate, long *bandwidth_bps, long *packet_loss)
{
    bool result = false;
    c_network_observer *observer = g_simulation_globals.watcher->m_observer;
    if (view != 0 && observer != 0) {
        long rtt;
        float rate;
        long bandwidth;
        long loss;
        if (observer->get_simple_status(view->get_observer_channel(), &rtt, &rate, &bandwidth, &loss)) {
            *rtt_msec = rtt;
            *packet_rate = (long)(rate * 10.0f + 0.5f);
            *bandwidth_bps = bandwidth * 10 / 1024;
            *packet_loss = (long)((float)loss + 0.5f);
            result = true;
        }
    }
    return result;
}

// 0x3a0a50

static __forceinline float simulation_range_fraction(float value, const s_simulation_range *range)
{
    long first = range->first;
    long second = range->second;
    float fraction;
    if (first > second) {
        if (value >= (float)first) {
            fraction = 1.0f;
        } else if (value <= (float)second) {
            fraction = 0.0f;
        } else {
            fraction = (value - (float)second) / (float)(first - second);
        }
    } else {
        if (value <= (float)first) {
            fraction = 1.0f;
        } else if (value >= (float)second) {
            fraction = 0.0f;
        } else {
            fraction = ((float)second - value) / (float)(second - first);
        }
    }
    return fraction;
}

// @address 0x3a0a50 simulation_compute_connection_rating
bool simulation_compute_connection_rating(long index, const s_simulation_connection_statistics *statistics, long *result)
{
    bool valid = false;
    long maximum_value = g_network_configuration.maximum_value;
    long value = maximum_value;
    if (statistics->metrics[0] != 0 && g_simulation_globals.in_online_networked_session != 0) {
        valid = true;
    }
    for (unsigned long metric = 0; metric < 4; ++metric) {
        const s_simulation_range *range = &g_network_configuration.ranges[metric];
        float measured;
        switch (metric) {
        case 0:
            measured = (float)statistics->metrics[0];
            break;
        case 1:
            measured = (float)statistics->metrics[1] * 0.1f;
            break;
        case 2:
            measured = (float)statistics->metrics[2] * 0.1f;
            measured *= 1024.0f;
            break;
        case 3:
            measured = (float)statistics->metrics[3];
            break;
        default:
            // match: 0x3a0a50: unreachable hint omits dead branch check
            __assume(0);
        }
        float fraction = simulation_range_fraction(measured, range);
        long candidate = (long)((float)(maximum_value - 1) * fraction + 0.5f) + 1;
        if (value > candidate) {
            value = candidate;
        }
    }
    *result = value;
    return valid;
}

// @address 0xa6118 simulation_record_snippet_marker
extern "C" void simulation_record_snippet_marker(long *p0, long *p1)
{
  game_globals_storage *game_globals = tls_game_globals();
  bool valid = false;
  if (game_globals && game_globals->game_in_progress && !game_globals->initializing &&
      game_globals->map_active)
    valid = true;
  if (!valid)
    return;
  if (game_globals->options.game_mode != 2)
    return;
  if (!game_is_cooperative())
    return;
  if (saved_film_snippet_find_last_recorded_marker(g_saved_film_work_globals.markers) != -1) {
    _ReadWriteBarrier();
    long index = saved_film_snippet_find_last_recorded_marker(g_saved_film_work_globals.markers);
    s_saved_film_snippet_marker *marker = &g_saved_film_work_globals.markers[index];
    marker->identifier = p1[1];
    marker->tick = p1[2];
    marker->reserved1 = *p0;
    index = saved_film_snippet_find_last_recorded_marker(g_saved_film_work_globals.markers);
    g_saved_film_work_globals.markers[index].recorded = false;
    g_saved_film_work_globals.markers[index].valid = true;
  }
  g_saved_film_work_globals.recording_pending = false;
}


static __forceinline bool simulation_async_usable()
{
    bool result;
    switch (g_thread_crashed[3].peek()) {
    case 1:
        result = false;
        break;
    default:
        result = true;
        break;
    }
    return result;
}

// @address 0xa4320 simulation_write_snippet_snapshot
extern "C" void simulation_write_snippet_snapshot(bool force)
{
  bool success = false;
  game_globals_storage *game_globals = tls_game_globals();
  bool valid = false;
  if (game_globals && game_globals->game_in_progress && !game_globals->initializing &&
      game_globals->map_active)
    valid = true;
  if (!valid || game_globals->options.game_mode != 2)
    return;
  bool due = g_simulation_globals.world->next_update_number % 300 == 0 && !g_saved_film_work_globals.recording_pending;
  if (!due)
    return;
  bool exists = false;
  long tick = g_saved_film_playback_tick.value;
  if (tick > 0)
    tick++;
  bool cooperative = game_is_cooperative();
  if (cooperative) {
    long found = -1;
    for (long index = 0; index < 16; index++) {
      if (g_saved_film_work_globals.markers[index].valid && g_saved_film_work_globals.markers[index].tick == tick)
        found = index;
    }
    if (found != -1)
      exists = true;
  }
  if (!exists && !force && cooperative) {
    if (!g_saved_film_work_globals.buffer)
      g_saved_film_work_globals.buffer = saved_film_get_async_buffer();
    if (g_saved_film_work_globals.buffer->ready_to_write()) {
      bool seekable = false;
      if (saved_film_get_marker_count(g_saved_film_work_globals.markers) == 0 ||
        tick - g_saved_film_work_globals.markers[saved_film_snippet_find_best_marker_for_tick(g_saved_film_work_globals.markers, tick, true)].tick >=
          g_saved_film_work_globals.playback_length / 9)
        seekable = true;
      s_saved_film_snippet_marker *entry = saved_film_new_work_entry(g_saved_film_work_globals.markers, seekable);
      g_saved_film_work_globals.buffer->set_position(entry->buffer_offset);
      game_state_lock_and_halt_render_thread(0x100);
      long bytes_written = 0;
      char *buffer = game_state_globals.game_state_buffer;
      entry->checksum = fast_checksum_buffer(
          fast_checksum_buffer(fast_checksum_buffer(fast_checksum_buffer(-1, buffer, 4), buffer + 0x1f8000, 4), buffer + 0x3f0000, 4),
          buffer + 0x7dfffc, 4);
      simulation_async_usable();
      g_saved_film_work_globals.buffer->write(buffer, 0x10000, &bytes_written);
      if (bytes_written == 0x10000) {
        g_saved_film_work_globals.buffer->write(buffer, 0x7e0000, &bytes_written);
        if (bytes_written == 0x7e0000 && g_saved_film_work_globals.buffer->flush())
          success = true;
      }
      game_state_unlock_and_resume_render_thread(0x100);
    }
    if (!success && !g_saved_film_timer_globals.request_pending) {
      g_saved_film_timer_globals.request_pending = true;
      g_saved_film_timer_globals.requested_state = 10;
    }
  } else {
    game_state_lock_and_halt_render_thread(0x100);
    game_state_unlock_and_resume_render_thread(0x100);
  }
}


// The release c_simulation_watcher::describe_status keeps only the empty-string copy.
static __forceinline void simulation_watcher_describe_status(char *buffer, long length)
{
  csstrnzcpy(buffer, "", length);
}

// Describing an initialized simulation delegates to its watcher.
static __forceinline void simulation_describe_status(char *buffer, long length)
{
  if (g_simulation_globals.initialized)
    simulation_watcher_describe_status(buffer, length);
  else
    csstrnzcpy(buffer, "", length);
}

// @address 0x96d90 ?simulation_update@@YAXXZ
void simulation_update(void)
{
  if (g_simulation_globals.initialized && g_simulation_globals.aborted && simulation_starting_up()
      && tls_game_globals()->options.game_mode == 3)
    main_menu_update_launch_options();
  if (g_simulation_globals.initialized && g_simulation_globals.film_update_pending)
    g_simulation_globals.film_update_pending = saved_film_is_update_pending();
  if (g_simulation_globals.initialized && !g_simulation_globals.aborted && simulation_starting_up()) {
    if (g_simulation_globals.fatal_error)
      simulation_abort_immediate(_simulation_abort_reason_fatal_error);
    if (!g_simulation_globals.aborted && !((c_simulation_watcher *)g_simulation_globals.watcher)->maintain_connection())
      simulation_abort_immediate(_simulation_abort_reason_connection_lost);
    if (!g_simulation_globals.aborted) {
      long world_type = ((c_simulation_world *)g_simulation_globals.world)->m_world_type;
      if (simulation_world_type_is_networked(world_type) && g_saved_film_timer_globals.request_pending)
        simulation_abort_immediate(_simulation_abort_reason_film_request_pending);
    }
    if (!g_simulation_globals.aborted && !g_simulation_globals.film_update_pending)
      ((c_simulation_world *)g_simulation_globals.world)->update();
    simulation_update_abort_check();
    if (!g_simulation_globals.aborted && !g_simulation_globals.film_update_pending) {
      long world_type = ((c_simulation_world *)g_simulation_globals.world)->m_world_type;
      bool flush_queue = true;
      if (world_type == 2 || (world_type >= 4 && world_type <= 6))
        flush_queue = false;
      if (!flush_queue && ((c_simulation_world *)g_simulation_globals.world)->m_time_running
          && ((c_simulation_world *)g_simulation_globals.world)->m_world_state == 4
          && !g_simulation_globals.world->is_out_of_sync())
        update_queue_delete_ready_head((s_world_update_queue *)g_simulation_globals.world);
    }
    if (!g_simulation_globals.aborted && !g_simulation_globals.film_update_pending) {
      long world_type = ((c_simulation_world *)g_simulation_globals.world)->m_world_type;
      if (simulation_world_type_is_networked(world_type)
          && simulation_world_type_is_client(world_type)
          && ((c_simulation_world *)g_simulation_globals.world)->m_time_running
          && ((c_simulation_world *)g_simulation_globals.world)->m_world_state == 4
          && !g_simulation_globals.world->is_out_of_sync()) {
        long playback_type = tls_game_globals()->options.game_playback;
        bool update_failed = false;
        if (playback_type == 1 || playback_type == 2) {
          long film_ticks_remaining = 0;
          if (g_saved_film_timer_globals.film.get_mode() == 0) {
            film_ticks_remaining = g_saved_film_timer_globals.film.get_current_tick();
            if (film_ticks_remaining > 0) {
              long updates_read = 0;
              if (!simulation_film_read_updates(film_ticks_remaining, &updates_read) && updates_read != film_ticks_remaining)
                update_failed = true;
            }
          }
          long updates_required = (long)((float)game_time_globals->ticks_per_second * game_time_globals->rate_scale * 0.25f + 0.5f);
          if (updates_required > 0) {
            long update_queue_length;
            simulation_world_get_available_updates((c_simulation_world *)g_simulation_globals.world, 0, &update_queue_length);
            if (film_ticks_remaining != 0 && update_queue_length == 0)
              update_failed = true;
          }
        }
        else {
          update_failed = true;
        }
        if (update_failed) {
          if (!g_saved_film_timer_globals.request_pending) {
            g_saved_film_timer_globals.request_pending = true;
            g_saved_film_timer_globals.requested_state = 1;
          }
          if (!network_life_cycle_globals_get()->initialized || network_life_cycle_globals_get()->manager.get_current_state() == _life_cycle_state_none)
            main_menu_launch();
        }
      }
    }
    if (simulation_is_pending_start())
      simulation_describe_status(g_simulation_globals.describe_buffer, 256);
  }
  if (g_simulation_globals.initialized && g_simulation_globals.aborted && g_simulation_globals.recording_film) {
    saved_film_dispose();
    g_simulation_globals.recording_film = false;
  }
  if (g_simulation_globals.initialized && g_simulation_globals.aborted) {
    long world_type = ((c_simulation_world *)g_simulation_globals.world)->m_world_type;
    if (world_type != 0 && simulation_world_type_is_networked(world_type) && g_saved_film_timer_globals.film.get_mode() == 0)
      saved_film_dispose();
  }
  if (g_simulation_globals.initialized && g_simulation_globals.saved_film_dispose_pending) {
    saved_film_dispose();
    g_simulation_globals.saved_film_dispose_pending = false;
  }
  g_simulation_globals.in_online_networked_session = g_simulation_globals.initialized
      && ((c_simulation_watcher *)g_simulation_globals.watcher)->in_online_networked_session();
}

// @address 0xa6830 ?simulation_apply_before_game@@YAXPBUsimulation_update@@@Z
void simulation_apply_before_game(const struct simulation_update *update)
{
    const long *unit_indices = update->actor_unit_indices;
    long actor_index = 0;
    for (; actor_index < 16; actor_index++) {
        if (update->actor_valid_mask & (1 << actor_index)) {
            long unit_index = unit_indices[actor_index];
            if (object_try_and_get_and_verify_type(unit_index, 0x1003))
                unit_control(unit_index, &update->actor_controls[actor_index]);
        }
    }
    if (update->machine_update_exists)
        players_set_machines(update->machine_update.machine_valid_mask, update->machine_update.machine_identifiers);
    c_data_iterator<player_datum> player_iterator;
    player_iterator.begin(player_data);
    while (player_iterator.next()) {
        long player_index = player_iterator.get_index();
        if (update->prediction_mask & (1 << (unsigned short)player_index))
            simulation_game_process_player_prediction(player_index, &update->predictions[(unsigned short)player_index]);
    }
    c_simulation_world::apply_simulation_queue(&update->bookkeeping);
    const c_simulation_queue *simulation = &update->simulation;
    long lock_token = 0;
    if (simulation->initialized && simulation->count > 0) {
        if ((((unsigned short)update->flags) & 1) == 0) {
            if (!game_is_cooperative())
                lock_token = tag_resources_lock_game();
        }
        c_simulation_world::apply_simulation_queue(simulation);
        if ((((unsigned short)update->flags) & 1) == 0) {
            objects_purge_deleted_objects();
            if (!game_is_cooperative())
                tag_resources_unlock_game(lock_token);
        }
    }
    if ((_rotl(((unsigned short)update->flags), 31) & 1) != 0) {
        c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
        if (!simulation_world_type_is_client(world->m_world_type)) {
            if (!world->m_gamestate_received) {
                game_state_lock_and_halt_render_thread(0x100);
                game_state_unlock_and_resume_render_thread(0x100);
            }
            world->m_gamestate_received = false;
        }
    }
}

static __forceinline bool simulation_apply_game_in_progress()
{
    bool valid = false;
    game_globals_storage *globals = tls_game_globals();
    if (globals && globals->game_in_progress && !globals->initializing && globals->map_active)
        valid = true;
    return valid;
}
// @address 0xa5fd8 simulation_apply_update_to_film
extern "C" void simulation_apply_update_to_film(struct simulation_update *update, s_simulation_update_metadata *metadata)
{
    simulation_record_snippet_marker(reinterpret_cast<long *>(update), reinterpret_cast<long *>(metadata));
    if (simulation_apply_game_in_progress() && game_is_cooperative())
    {
        _InterlockedExchange(&g_saved_film_playback_tick.value, metadata->film_storage_used);
        saved_film_set_replay_metadata(update->camera_valid_mask, &update->film_replay_metadata);
        if (game_is_film_playback_active())
            saved_film_update_playback_speed(metadata->film_storage_used);
        if (saved_film_is_recording() &&
            !simulation_film_retrieve_updates(update, metadata) &&
            !g_saved_film_timer_globals.request_pending)
        {
            g_saved_film_timer_globals.request_pending = true;
            g_saved_film_timer_globals.requested_state = 3;
        }
    }
}


static __forceinline player_datum *simulation_netdebug_player_get(long player_index)
{
    long absolute_index = player_index & 0xffff;
    return &((player_datum *)player_data->data)[absolute_index];
}
// @address 0x3a07c8 ?simulation_update_player_netdebug_data@@YA_NJPAUs_simulation_player_netdebug_data@@@Z
bool simulation_update_player_netdebug_data(long player_index, s_simulation_player_netdebug_data *netdebug_data)
{
    bool changed = false;
    if (g_simulation_globals.initialized) {
        player_datum *player = simulation_netdebug_player_get(player_index);
        s_simulation_player_netdebug_data previous = *netdebug_data;
        memset(netdebug_data, 0, sizeof(*netdebug_data));
        if ((player->flags & 2) == 0) {
            const s_machine_identifier *identifier = &tls_players_globals()->machines[player->machine_index];
            long machine_index = ((c_simulation_watcher *)g_simulation_globals.watcher)->get_machine_index_by_identifier(identifier);
            if (machine_index != -1) {
                c_simulation_world *world = (c_simulation_world *)g_simulation_globals.world;
                if (simulation_world_type_is_client(world->m_world_type)) {
                    if (machine_index == world->m_local_machine_index) {
                        s_simulation_world_view_iterator iterator;
                        simulation_world_iterator_begin(&iterator, -1);
                        c_simulation_view *view;
                        long client_count = 0;
                        long rtt[16];
                        long packet_rate[16];
                        long bandwidth[16];
                        long loss[16];
                        while (world->iterator_next(&iterator, &view)) {
                            if (simulation_get_view_netdebug_data(
                                    view, &rtt[client_count], &packet_rate[client_count], &bandwidth[client_count], &loss[client_count]))
                                ++client_count;
                        }
                        if (client_count > 0) {
                            netdebug_data->values[4] = (short)network_bandwidth_compute_average(client_count, rtt);
                            netdebug_data->values[5] = (short)network_bandwidth_compute_average(client_count, packet_rate);
                            netdebug_data->values[6] = (short)network_bandwidth_compute_average(client_count, bandwidth);
                            netdebug_data->values[7] = (short)network_bandwidth_compute_average(client_count, loss);
                        }
                    } else {
                        long rtt;
                        long packet_rate;
                        long bandwidth;
                        long loss;
                        if (simulation_get_view_netdebug_data(world->get_client_view_by_machine_index(machine_index), &rtt, &packet_rate, &bandwidth, &loss)) {
                            netdebug_data->values[4] = (short)rtt;
                            netdebug_data->values[5] = (short)packet_rate;
                            netdebug_data->values[6] = (short)bandwidth;
                            netdebug_data->values[7] = (short)loss;
                        }
                    }
                }
            }
        }
        changed = memcmp(&previous, netdebug_data, sizeof(previous)) != 0;
    }
    return changed;
}


// @address 0x281228 ?simulation_film_retrieve_updates@@YA_NPBUsimulation_update@@PAUs_simulation_update_metadata@@@Z
bool simulation_film_retrieve_updates(const struct simulation_update *update, s_simulation_update_metadata *)
{
    bool success = false;
    long tick = g_saved_film_playback_tick.value;
    switch (g_saved_film_recording_globals.state) {
    case 0:
        success = true;
        break;
    case 1:
    case 5:
        break;
    case 4:
        if (g_saved_film_timer_globals.playback_speed == 0.0f)
            success = true;
        break;
    case 2:
        if (tick == g_saved_film_recording_globals.first_tick) {
            saved_film_snippet_set_cameras_from_states(g_saved_film_recording_globals.camera_states);
            g_saved_film_recording_globals.state = 3;
        }
    case 3: {
        unsigned long bytes_written = 0;
        union { s_film_replay_metadata film_metadata; s_simulation_camera_update camera_update; } film_camera = {};
        saved_film_get_replay_metadata(&film_camera.film_metadata);
        unsigned char buffer[256];
        c_bitstream stream;
        bitstream_initialize(&stream, buffer, sizeof(buffer));
        stream.begin_writing(1, 0, false);
        simulation_machine_update_entry_encode(&stream, &film_camera.camera_update);
        stream.finish_writing(0);
        stream.reset(3);
        simulation_camera_update_decode(&stream, &film_camera.camera_update);
        g_saved_film_recording_globals.async_buffer.write(&film_camera.film_metadata, sizeof(film_camera), reinterpret_cast<long *>(&bytes_written));
        if (bytes_written == sizeof(film_camera)) {
            struct simulation_update update_copy = *update;
            update_copy.camera_valid_mask |= 1;
            update_copy.film_replay_metadata = film_camera.film_metadata;
            if (saved_film_update_storage(&g_saved_film_recording_globals.film, &update_copy))
                success = true;
        }
        break;
    }
    case 6:
        if (tick >= 0 && tick <= g_saved_film_recording_globals.last_tick) {
            if (tick == g_saved_film_recording_globals.last_tick) {
                saved_film_set_playback_rate(0.0f, 1);
                g_saved_film_recording_globals.state = 4;
                success = true;
            } else {
                unsigned long bytes_read = 0;
                s_film_replay_metadata metadata = {};
                g_saved_film_recording_globals.async_buffer.read(&metadata, sizeof(metadata), reinterpret_cast<long *>(&bytes_read));
                if (bytes_read == sizeof(metadata)) {
                    saved_film_set_replay_metadata(1, &metadata);
                    success = true;
                }
            }
        }
        break;
    case _saved_film_recording_waiting_for_playback:
        if (tick == g_saved_film_recording_globals.first_tick) {
            saved_film_snippet_set_cameras_from_states(g_saved_film_recording_globals.camera_states);
            saved_film_reset_recording_state();
        }
        success = true;
        break;
    default:
        // match: 0x281228: unreachable hint omits dead branch check
        __assume(0);
        break;
    }
    return success;
}
static __forceinline c_simulation_world *simulation_build_update_get_world()
{
    return (c_simulation_world *)g_simulation_globals.world;
}
static __forceinline bool simulation_world_type_is_playback(long type)
{
    return type == 7 || type == 8;
}
__forceinline void c_simulation_world::gamestate_flush()
{
    if (!m_gamestate_received) {
        game_state_lock_and_halt_render_thread(0x100);
        game_state_unlock_and_resume_render_thread(0x100);
    }
    m_gamestate_received = false;
}
static __forceinline bool simulation_update_flags_test(short flags, long bit)
{
    return (flags & (1 << bit)) != 0;
}
// @address 0xa4098 simulation_build_game_update
extern "C" void simulation_build_game_update(bool in_progress, struct simulation_update *update, s_simulation_update_metadata *metadata)
{
    memset(update, 0, sizeof(struct simulation_update));
    memset(metadata, 0, sizeof(s_simulation_update_metadata));
    bool recording = false;
    if (saved_film_is_recording() && saved_film_get_save_state() != 0)
        recording = true;
    simulation_write_snippet_snapshot(recording);
    simulation_build_update_get_world()->build_update(in_progress, update, metadata);
    long world_type = simulation_build_update_get_world()->m_world_type;
    if ((!simulation_world_type_is_client(world_type) || simulation_world_type_is_networked(world_type))
        && (!simulation_world_type_is_playback(world_type) || simulation_world_type_is_networked(world_type))
        && !simulation_build_update_get_world()->is_out_of_sync())
    {
        bool determinism_failure = false;
        if (simulation_update_flags_test(((unsigned short)update->flags), 2)) {
            simulation_build_update_get_world()->gamestate_flush();
        }
        if (update->update_number != simulation_build_update_get_world()->next_update_number) {
            determinism_failure = true;
        } else {
            const game_time_globals_definition *const time_state = (const game_time_globals_definition *)game_time_globals;
            const unsigned long world_time = (unsigned long)time_state->time;
            if (update->verify_game_time != (long)world_time) {
                determinism_failure = true;
            } else {
                const unsigned long *const random_state = (const unsigned long *)tls_random_seed();
                const long world_seed = (long)*random_state;
                if (update->verify_random != (unsigned long)world_seed)
                    determinism_failure = true;
            }
        }
        if (determinism_failure)
            simulation_world_set_out_of_sync(simulation_build_update_get_world(), true);
    }
    simulation_apply_update_to_film(update, metadata);
}
