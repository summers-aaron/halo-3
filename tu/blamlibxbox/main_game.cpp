// @address 0x59f080 XTLGetLanguage
// blamlibxbox/main_game.obj: from the records 0xd5310 0x198e00 0x199618 0x199b30 0x1b5590 0x1e3358 0x1e3508 0x1e35b0 0x1e38b8 0x1e3a10 0x1e3e38 0x1e3f70 0x1e4018 0x1e40f8 0x1e4188 0x1e4238 0x1e4320 0x1e43f0 0x1e45b0 0x1e4640 0x1e4708 0x1e4770 0x1f06c0 0x580c08
// @flags /TP /O2 /Oc
#include "main/main_game_externs.h"
#include "shell/utility_drive.h"
#include "interface/user_interface_squad_session.h"
#include "effects/effects.h"
#include "tag_files/tag_types.h"
#include "input/input.h"
#include "game/game_types.h"
#include "multithreading/synchronization.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "game/game.h"
#include "game/players.h"
#include "objects/objects.h"
#include "simulation/simulation.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "cache/cache_file_tag_resource_vtable_list.h"
#include "cache/cache_file_globals.h"
#include "cache/xbox_sound_cache.h"
#include "game/game_engine_variant.h"
#include "input/input_xenon.h"
#include "main/game_preferences.h"
#include "main/main.h"
#include "main/main_game.h"
#include "networking/logic/network_life_cycle.h"
#include "text/font_cache.h"
#include "ai/ai.h"
#include "bink/bink_playback.h"
#include "game/game_engine.h"
#include "main/levels.h"
#include "main/levels_globals.h"
#include "game/game_state_accessors.h"
#include "bink/bink_playback_types.h"
#include "main/main_render.h"
#include "networking/logic/network_session_interface.h"
#include "scenario/scenario.h"
#include "main/loading.h"
#include "main/loading_progress.h"
#include "cseries/cseries.h"
#include "game/game_globals.h"
#include "cache/physical_memory_map.h"
#include "memory/physical_memory_map_types.h"
#include "saved_games/game_state.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_bsp_state_out_types.h"
#include "scenario/scenario_bits_state_types.h"
#include "cseries/data_mining_event_types.h"
#include "input/input_abstraction.h"
#include "cseries/language.h"
#include "memory/data_types.h"
#include "input/input_abstraction_types.h"
#include "game/game_options.h"
#include "text/text_group.h"
#include <ppcintrinsics.h>
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "interface/marketplace_consume_assets_task.h"
#include "memory/data.h"
#include "sound/sound_cache_inline.h"
#include <string.h>
#include "shell/shell.h"
#include "main/main_flag_state.h"
#include "math/random_math.h"
#include "cache/geometry_storage.h"
#include "saved_games/game_state_xenon_types.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "saved_games/game_state_procs.h"
#include "networking/network_memory.h"
#include "interface/user_interface_memory.h"
#include "cseries/async.h"
#include "networking/online/online_files.h"
#include "saved_games/saved_film.h"
// @address 0x1ce960 ?create_default@c_map_variant@@QAAXW4e_map_id@@@Z
// @address 0x243980 ?game_engine_variant_validate@@YA_NPAVc_game_variant@@@Z
// @address 0x1f6c58 ?generate_random_seed@@YAKXZ
// @address 0x845a8 ?levels_get_campaign_next_map_id@@YA?AW4e_map_id@@W4e_campaign_id@@W41@@Z
// @address 0x1e45b0 ?main_menu_build_game_options@@YAXPAUgame_options@@@Z
// @address 0x3b0fa0 ?network_squad_session_set_coop_game_options@@YA_NW4e_campaign_id@@W4e_map_id@@PBDFPBX@Z
// @address 0x1def00 ?players_build_persistent_player_options@@YAXPAUgame_options@@@Z
// @address 0x1df158 ?players_store_persistent_campaign_data_on_game_won@@YAXPAUs_campaign_armaments@@@Z
// @address 0x80618 ?simulation_starting_up@@YA_NXZ
// @address 0x8af98 ?is_signed_in_to_live@c_controller_interface@@QBA_NXZ
// @address 0x1cee08 ?map_variant_is_valid@@YA_NPBUs_map_variant_validation@@@Z
// @address 0x1e3f70 ?main_game_internal_pregame_load@@YAXXZ
// @address 0x1e40f8 ?main_game_internal_close_caches@@YAXXZ
// @address 0x1e3e38 ?main_game_unload_and_prepare_for_next_game@@YAXPBUgame_options@@@Z
// @address 0x1e4188 ?main_game_set_map_memory_stage@@YAXJ@Z
// @address 0x1b5590 main_game_sort_heap
// @address 0x74b7a0 ?g_loaded_language_index@@3JA
long g_loaded_language_index = -1;


#include "cache/physical_memory_map.h"


/* The release event builder fills a 24-byte local event. */

#include "game/game_system_types.h"
#include "physics/havok_monitor_storage.h"

// Slot 3 of the map memory callbacks has nothing to initialize.
// @address 0x5bec98 ?map_memory_initialize_empty@@YAXW4e_map_memory_configuration@@@Z
void map_memory_initialize_empty(e_map_memory_configuration)
{
}

// @address 0x394f8 k_main_game_map_memory_callbacks
// @rdata 0x820394f8..0x82039538
const main_game_callback k_main_game_map_memory_callbacks[8] =
{
    { game_state_procs_reset_current_handler, game_state_procs_set_mode_handler },
    { network_memory_shared_dispose, network_memory_shared_initialize },
    { user_interface_memory_dispose, user_interface_memory_initialize },
    { async_block_until_content_finished, map_memory_initialize_empty },
    { online_files_memory_dispose, online_files_memory_initialize },
    { bink_playback_cancel_prediction, bink_playback_set_recording_enabled },
    { saved_film_close, saved_film_initialize_history_storage },
    { saved_film_dispose_playback, saved_film_new_playback }
};
// @rdata-end

static __forceinline void main_game_physical_memory_stage_push(void)
{
    long next_stage = g_physical_memory_globals.current_stage + 1;
    g_physical_memory_globals.stages[next_stage] = g_physical_memory_globals.stages[g_physical_memory_globals.current_stage];
    g_physical_memory_globals.current_stage = next_stage;
}


// @address 0x1e4238 ?main_game_configure_map_memory@@YAXPBUgame_options@@@Z

struct s_datum;
struct s_map_variant_validation;
// generate_random_seed stays an out-of-line call here.
// @address 0x1f6c58 ?generate_random_seed@@YAKXZ

static __forceinline const game_options *main_game_options_get()
{
    return &tls_game_globals()->options;
}
// @address 0x1e38b8 ?main_game_goto_next_level@@YA_NXZ

static __forceinline void main_game_user_interface_enter_game_shell()
{
    long milliseconds = g_user_interface_globals.milliseconds.peek();
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.last_input.value, milliseconds);
}

// @address 0x1e4640 ?main_menu_launch@@YAXXZ


bool main_game_load_map(const game_options *)
{
    bool result;
    s_game_globals *tr11;
    tr11 = global_game_globals;
    int tr29;
    tr29 = g_current_language;
    s_game_globals *tr28;
    tr28 = tr11;
    if (tr29 == -1) {
        tr29 = ::get_e_language_from_xbox_language(XTLGetLanguage());
        tr11 = global_game_globals;
        g_current_language = (e_language)tr29;
    }
    result = false;
    int tr10;
    tr10 = g_loaded_language_index;
    if (tr10 != -1) {
        if (tr11 != 0) {
            c_language_pack *pack = &tr11->language_packs[tr10];
            if (pack->string_count != 0) {
                pack->strings = 0;
                pack->data = 0;
                pack->string_count = 0;
            }
        }
        g_loaded_language_index = -1;
    }
    if (tr29 != -1) {
        if ((unsigned char)tr28->language_packs[tr29].load_data() != 0) {
            g_loaded_language_index = tr29;
            result = true;
        }
    } else {
        result = true;
    }
    return result;
}

bool main_game_goto_next_level(void)
{
    bool result = false;
    game_options options;
    e_map_id next_map_id = _map_id_none;
    char next_scenario_path[256];
    next_scenario_path[0] = 0;
    ::data_mine_insert_single_player_game_options("level completed");
    (game_options &)options = *main_game_options_get();
    if (options.campaign_id != _campaign_id_none) {
        next_map_id = levels_get_campaign_next_map_id(options.campaign_id, options.map_id);
        if (next_map_id != _map_id_none) {
            s_datum next_level;
            if (levels_try_and_get_datum(g_level_globals.map_levels, next_map_id, (s_datum *)&next_level))
                csstrnzcpy(next_scenario_path, next_level.name, sizeof(next_scenario_path));
        }
    }
    if (next_map_id != _map_id_none && next_scenario_path[0]) {
        options.map_id = next_map_id;
        options.campaign_insertion_point = 0;
        csstrnzcpy(options.scenario_path, next_scenario_path, sizeof(options.scenario_path));
        options.survival_mode_enabled = false;
        options.random_seed = generate_random_seed();
        players_build_persistent_player_options(&options);
        players_store_persistent_campaign_data_on_game_won(&options.campaign_armaments);
        if (!network_squad_session_set_coop_game_options(
                options.campaign_id, next_map_id, next_scenario_path, options.campaign_insertion_point, &options.campaign_armaments))
        {
            options.initial_zone_set_index = 0;
            game_options_validate_variants(&options);
            main_game_set_pending_options(&options);
        }
        result = true;
    }
    return result;
}

void main_game_internal_pregame_load(void)
{
    main_game_physical_memory_stage_push();
    main_game_physical_memory_stage_push();
    main_game_globals.status = 6;
}

void main_game_set_map_memory_stage(long state)
{
    main_game_physical_memory_stage_push();
    main_game_globals.state = state;
    for (unsigned long index = 0; index < 8; index++) {
        if (k_main_game_map_memory_callbacks[index].initialize) {
            k_main_game_map_memory_callbacks[index].initialize(static_cast<e_map_memory_configuration>(state));
        }
    }
}

void main_game_configure_map_memory(const game_options *options)
{
    long stage = 0;
    if (options)
    {
        long mode = options->game_mode;
        if (mode != 1)
        {
            if (mode != 2)
            {
                if (mode == 3)
                    stage = 1;
            }
            else
            {
                stage = 2;
                if (options->game_playback != 0)
                    stage = 3;
            }
        }
        else
        {
            stage = 4;
            if (options->game_playback != 0)
                stage = 5;
            else if (options->map_id == 0xc3a)
                stage = 6;
        }
    }
    if (main_game_globals.state != stage)
    {
        if (main_game_globals.state > 0)
        {
            for (long index = NUMBEROF(k_main_game_map_memory_callbacks) - 1; index >= 0; index--)
            {
                if (k_main_game_map_memory_callbacks[index].dispose)
                    k_main_game_map_memory_callbacks[index].dispose();
            }
            physical_memory_pop_stage(4);
            main_game_globals.state = 0;
        }
        if (stage > 0)
            main_game_set_map_memory_stage(stage);
    }
}

void main_menu_build_game_options(game_options *options)
{
    game_options_initialize_default(options);
    options->game_mode = 3;
    internal_critical_section_enter(22);
    levels_map_datum map = g_level_globals.default_map;
    internal_critical_section_leave(22);
    csstrnzcpy(options->scenario_path, map.name, sizeof(options->scenario_path));
    game_options_setup_default_players(1, options);
}

void main_menu_launch(void)
{
    if (!main_globals.main_menu_disabled) {
        if (simulation_starting_up() && main_game_options_get()->game_mode == 3) {
            main_game_user_interface_enter_game_shell();
        } else {
            game_options options;
            main_menu_build_game_options(&options);
            game_options_validate_variants(&options);
            main_game_set_pending_options(&options);
        }
    }
}

// @address 0x1e4708 main_menu_update_launch_options
extern "C" void main_menu_update_launch_options(void)
{
    game_options options;
    main_menu_build_game_options(&options);
    game_engine_variant_validate(&options.game_variant);
    if (!map_variant_is_valid((const s_map_variant_validation *)&options.map_variant))
        options.map_variant.create_default((e_map_id)options.map_variant.map_id);
    main_game_set_pending_options(&options);
}

// @address 0x1e4770 ?data_mine_insert_single_player_game_options@@YAXPBD@Z
void data_mine_insert_single_player_game_options(char const * value)
{
  if (game_in_progress() && tls_game_globals()->options.game_mode == 1 && tls_game_globals()->options.local_machine_exists) {
    c_datamine event(0, value, 2, "main", "game");
  }
}

// @address 0x580c08 ?start@c_marketplace_consume_assets_task@@UAAKPAX@Z
unsigned long c_marketplace_consume_assets_task::start(void *overlapped)
{
    unsigned long result = 0x80004005;
    if (controller_get(controller_index)->is_signed_in_to_live())
        result = main_game_submit_live_request(controller_index, 0x3e9, &offer_id, 1, &this->result, overlapped);
    return result;
}


// std::sort_heap over 24-byte elements
static __forceinline void main_game_pop_heap(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_predicate predicate)
{
    if (1 < last - first)
    {
        s_cache_file_tag_resource_relocation value = *(last - 1);
        *(last - 1) = *first;
        cache_file_tag_resource_sort_adjust_heap(first, 0, (last - 1) - first, value, predicate);
    }
}
extern "C" void main_game_sort_heap(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_predicate predicate)
{
    for (; 1 < last - first; --last)
        main_game_pop_heap(first, last, predicate);
}

// @address 0xd5310 main_game_dispose_from_old_structure_bsp
extern "C" void main_game_dispose_from_old_structure_bsp(int flags)
{
    game_globals_storage *game_globals = tls_game_globals();
    for (long index = 0; index < 16; index++)
    {
        if ((flags & (1u << index)) != 0)
        {
            for (long word = 0; word < 8; word++)
                game_globals->cluster_pvs[index][word] = 0;
        }
    }
    for (long index = 0; index < 16; index++)
    {
        if ((flags & (1u << index)) != 0)
        {
            for (long word = 0; word < 8; word++)
                game_globals->cluster_pvs_local[index][word] = 0;
        }
    }
    for (long index = 0; index < 16; index++)
    {
        if ((flags & (1u << index)) != 0)
        {
            for (long word = 0; word < 8; word++)
                game_globals->cluster_activation[index][word] = 0;
        }
    }
    g_havok_monitor_globals.lock_count_4c++;
    for (long system_index = NUMBEROF(g_game_systems) - 1; system_index >= 0; system_index--)
    {
        if (g_game_systems[system_index].dispose_from_old_structure_bsp_proc)
            g_game_systems[system_index].dispose_from_old_structure_bsp_proc(flags);
    }
    ::objects_purge_deleted_objects();
    g_havok_monitor_globals.lock_count_4c--;
    tls_game_globals()->active_structure_bsp_mask = 0;
}

// @address 0x198e00 main_game_reset_scenario_globals
extern "C" void main_game_reset_scenario_globals(void)
{
    cache_file_tag_resource_reset_active_state();
    scenario_tags_unload();
    global_scenario_index = -1;
    global_scenario = 0;
    global_game_globals = 0;
    global_zone_set_index = -1;
    g_active_structure_bsp_mask = 0;
}

// match: 0x199618: keeps the 256-byte map_file_name memcpy an out-of-line call
#pragma function(memcpy)

// @address 0x1e3508 main_game_set_pending_options
extern "C" void main_game_set_pending_options(const game_options *options)
{
    if (options != 0)
        memcpy(&main_game_globals.options, options, sizeof(main_game_globals.options));
    main_game_globals.game_change_in_progress = 1;
    main_game_globals.game_unload_pending = (options == 0);
    main_game_globals.game_change_time = GetTickCount();
    if (options == 0) {
        if (!network_life_cycle_globals_get()->initialized) network_life_cycle_end();
        else if (network_life_cycle_globals_get()->manager.m_current_state == 0) network_life_cycle_end();
        else user_interface_leave_squad(0);
    }
}

// @address 0x199618 main_game_reset_zone_state_for_unload
extern "C" void main_game_reset_zone_state_for_unload(void)
{
    c_cache_file_tag_resource_runtime_manager *manager = g_geometry_globals.manager;
    manager->m_locked = true;
    if (manager->m_game_mode == 3) {
        manager->m_first_cursor = manager->m_zone_names.m_first;
    } else {
        manager->m_second_cursor.m_value = -1;
        long index = xenon_game_state_globals.active_file_index;
        bool valid = true;
        if (index < 0 || index >= 15)
            valid = false;
        if (valid)
            memcpy(manager->m_second_cursor.m_name.get_buffer(), xenon_game_state_globals.files[index].header.source_path,
                sizeof(manager->m_second_cursor.m_name));
    }
    game_globals_storage *game = tls_game_globals();
    s_scenario_bits_state state;
    state.previous[0] = game->active_designer_zone_mask;
    state.previous[1] = game->active_cinematic_zone_mask;
    state.current[0] = 0;
    state.current[1] = 0;
    scenario_apply_zone_set_change(-1, game->active_structure_bsp_mask, 0, 0, &state, 0, true);
    cache_file_tag_resource_reset_active_state();
}


struct tag_header;
// @address 0x3bbb0 ??_7s_cache_file_tag_resource_handler_table_resolver@@6B@

// @address 0x1e4018 main_game_initialize_map_resources
extern "C" bool main_game_initialize_map_resources(const game_options *options)
{
    bool success = true;
    success &= main_game_load_map(options);
    s_data_array *sounds = g_sound_cache_globals.sounds;
    sounds->valid = true;
    data_delete_all(sounds);
    s_cache_file_tags_header *header = g_cache_file_globals.tags_header;
    long game_mode = options->game_mode;
    long zone_tag_index = -1;
    for (long i = 0; i < header->global_tag_count; i++)
    {
        if (header->global_tags[i].group == 'zone')
            zone_tag_index = header->global_tags[i].index;
    }
    if (zone_tag_index != -1)
    {
        c_wrapped_array<const s_cache_file_tag_resource_vtable *> resources(g_cache_file_tag_resource_vtable_list, g_cache_file_tag_resource_vtable_count);
        s_cache_file_tag_resource_handler_table_resolver resolver;
        g_geometry_globals.manager->initialize_for_new_map((tag_header *)header, game_mode, zone_tag_index,
            resources, &resolver);
    }
    return success;
}
static __forceinline void main_game_dispose_language_pack(void)
{
    long language = g_loaded_language_index;
    if (language != -1)
    {
        s_game_globals *globals = global_game_globals;
        if (globals)
        {
            c_language_pack *pack = &globals->language_packs[language];
            if (pack->string_count)
            {
                pack->strings = 0;
                pack->data = 0;
                pack->string_count = 0;
            }
        }
        g_loaded_language_index = -1;
    }
}

void main_game_internal_close_caches(void)
{
    cache_file_tag_resource_runtime_manager_dispose_from_old_map(g_geometry_globals.manager);
    sound_cache_flush_all_sound_guaranteed();
    g_sound_cache_globals.sounds->valid = false;
    main_game_dispose_language_pack();
}


void main_game_unload_and_prepare_for_next_game(const game_options *options)
{
    unsigned long lock = ((int)::_internal_halt_render_thread_and_lock_resources("..\\main\\main_game.cpp", 970));
    if (game_in_progress())
    {
        ::data_mine_insert_single_player_game_options("game finish");
        game_engine_game_ending();
        simulation_stop();
        main_game_reset_zone_state_for_unload();
        ::game_dispose_from_old_map();
    }
    switch (main_game_globals.status)
    {
    case 0:
        break;
    case 2:
    case 5:
        main_game_internal_close_caches();
        main_game_globals.status = 4;
        csstrnzcpy(main_game_globals.scenario_path, "", sizeof(main_game_globals.scenario_path));
        main_game_reset_scenario_globals();
        physical_memory_pop_stage(5);
        main_game_globals.status = 0;
        break;
    case 6:
        physical_memory_pop_stage(5);
        physical_memory_pop_stage(4);
        main_game_globals.status = 0;
        break;
    default:
        // match: 0x3cfd4: unreachable hint omits dead branch check
        __assume(0);
    }
    main_game_configure_map_memory(options);
    unlock_resources_and_resume_render_thread(lock);
}

template<typename T> static __forceinline const T &main_game_max(const T &a, const T &b) { return a > b ? a : b; }
template<typename T> static __forceinline const T &main_game_min(const T &a, const T &b) { return a < b ? a : b; }
static __forceinline bool main_game_is_playback()
{
    bool result = false;
    if (tls_game_globals()->options.game_simulation == 4)
        result = true;
    return result;
}
static __forceinline void main_game_create_ai(e_game_create_mode mode)
{
    int lock = ::tag_resources_lock_game();
    long game_mode = tls_game_globals()->options.game_mode;
    if (!main_game_is_playback() && (game_mode == 1 || game_mode == 2))
        ai_create_initial_squads();
    ::tag_resources_unlock_game((*(long *)(&lock)));
}

// @address 0x1e43f0 ?main_game_start@@YA_NPBUgame_options@@@Z
bool main_game_start(const game_options *options)
{
    long zone_set_index = main_game_min(
        main_game_max(options->initial_zone_set_index, (short)0), (short)(global_scenario->zone_sets.count - 1));
    unsigned long lock = ((int)::_internal_halt_render_thread_and_lock_resources("..\\main\\main_game.cpp", 1832));
    bool success = false;
    game_initialize_for_new_map(options);
    s_scenario_bits_state state;
    state.previous[0] = 0;
    state.previous[1] = 0;
    state.current[0] = 0;
    state.current[1] = 0;
    scenario_apply_zone_set_change(zone_set_index, 0, 0, 0, &state, 0, true);
    game_create_objects(_game_create_mode_lock);
    game_create_players();
    bool zone_set_switched = false;
    if (scenario_switch_zone_set(zone_set_index, true))
        zone_set_switched = true;
    if (zone_set_switched)
    {
        game_start(_game_create_mode_lock);
        main_game_create_ai(_game_create_mode_lock);
        success = true;
    }
    if (success)
    {
        if (tls_game_globals()->options.game_mode != 3)
        {
            c_datamine event(0, "game start", 1, "main");
        }
    }
    else
    {
        ::game_dispose_from_old_map();
    }
    unlock_resources_and_resume_render_thread(lock);
    return success;
}

// @address 0x1f06c0 main_game_initialize_loading_progress
extern "C" void main_game_initialize_loading_progress(const char *scenario_path)
{
    struct main_game_loading_map_sizes
    {
        const char *scenario;
        unsigned long sizes[3];
    };
    g_loading_progress.scenario_path = scenario_path;
    g_loading_progress.visible = true;
    g_loading_progress.stage = -1;
    memset(g_loading_progress.maximum, 0, sizeof(g_loading_progress.maximum));
    memset(g_loading_progress.current, 0, sizeof(g_loading_progress.current));
    g_loading_progress.progress = 0.0f;
    if (scenario_path)
    {
        main_game_loading_map_sizes map_sizes[] = {
            { "005_intro", { 0xec0000, 0x150bd22, 0x3346ef4 } },
            { "010_jungle", { 0x2650000, 0xefce00, 0x418db6d } },
            { "020_base", { 0x2c60000, 0x1e28023, 0x46da671 } },
            { "030_outskirts", { 0x2d30000, 0x119dbe1, 0x25e8f9b } },
            { "040_voi", { 0x3130000, 0x13e45b2, 0x2dcfd50 } },
            { "050_floodvoi", { 0x2e50000, 0x14d678c, 0x2f78539 } },
            { "070_waste", { 0x3430000, 0x19941a3, 0x33c655c } },
            { "100_citadel", { 0x3490000, 0x17d2ac3, 0x1c8117a } },
            { "110_hc", { 0x21c0000, 0x12c183f, 0x22da39b } },
            { "120_halo", { 0x2eb0000, 0x19e8c40, 0x3c77b7f } },
            { "130_epilogue", { 0xe60000, 0x127c005, 0x29ec507 } }
        };
        for (unsigned long i = 0; i < sizeof(map_sizes) / sizeof(map_sizes[0]); ++i)
        {
            main_game_loading_map_sizes *map = &map_sizes[i];
            if (strstr(scenario_path, map->scenario))
            {
                g_loading_progress.maximum[1] = (float)map->sizes[0];
                g_loading_progress.current[1] = (float)map->sizes[0];
                g_loading_progress.maximum[2] = (float)map->sizes[g_utility_drive_mounted + 1];
                g_loading_progress.current[2] = (float)map->sizes[g_utility_drive_mounted + 1];
                break;
            }
        }
    }
}

// @address 0x119a3f4 g_main_game_load_panic_in_progress
char g_main_game_load_panic_in_progress;
// @address 0x1e4320 ?main_game_load_panic@@YAXXZ
void main_game_load_panic()
{
    bool main_menu_loaded = false;
    unsigned long resume_token = ((int)::_internal_halt_render_thread_and_lock_resources("..\\main\\main_game.cpp", 0x5e1));
    g_object_message_queue->clear();
    main_render_purge_effect_messages(0);
    g_decal_message_queue->clear();
    main_game_unload_and_prepare_for_next_game(0);
    if ((unsigned char)g_main_game_load_panic_in_progress == 0)
    {
        game_options options;
        g_main_game_load_panic_in_progress = 1;
        main_menu_build_game_options(&options);
        main_game_change_immediate(&options);
        main_menu_loaded = true;
        g_main_game_load_panic_in_progress = 0;
    }
    if (!main_menu_loaded)
    {
        main_game_internal_pregame_load();
        main_globals.main_menu_disabled = 1;
        main_halt_and_display_errors();
    }
    unlock_resources_and_resume_render_thread(resume_token);
}


// @address 0x1e3358 ?main_game_reset_map@@YAX_N@Z
void main_game_reset_map(bool reset_random_seed)
{
    unsigned long resume_token = _internal_halt_render_thread_and_lock_resources("..\\main\\main_game.cpp", 250);
    game_options options;
    main_game_globals.reset_in_progress = true;
    simulation_stop();
    players_detach_from_map();
    data_mine_insert_single_player_game_options("map reset");
    memcpy(&options, &tls_game_globals()->options, sizeof(game_options));
    if (reset_random_seed)
        options.random_seed = generate_random_seed();
    game_globals_storage *game_globals = tls_game_globals();
    long zone_set_index = options.initial_zone_set_index;
    unsigned long old_structure_bsp_mask = game_globals->active_structure_bsp_mask;
    unsigned long zone_set_bsp_mask;
    if (zone_set_index != -1)
        zone_set_bsp_mask = ((s_scenario_zone_set *)global_scenario->zone_sets.address)[zone_set_index].bsp_active_mask;
    else
        zone_set_bsp_mask = 0;
    unsigned long active_designer_zone_mask = game_globals->active_designer_zone_mask;
    unsigned long active_cinematic_zone_mask = game_globals->active_cinematic_zone_mask;
    s_scenario_bits_state state;
    state.previous[0] = active_designer_zone_mask;
    state.previous[1] = active_cinematic_zone_mask;
    if (zone_set_index != -1)
        state.current[0] = ((s_scenario_zone_set *)global_scenario->zone_sets.address)[zone_set_index].designer_zone_required_mask;
    else
        state.current[0] = 0;
    state.current[1] = 0;
    scenario_apply_zone_set_change(zone_set_index, old_structure_bsp_mask, zone_set_bsp_mask, 0, &state, 0, true);
    long lock = tag_resources_lock_game();
    main_game_dispose_from_old_structure_bsp(g_active_structure_bsp_mask);
    tag_resources_unlock_game(lock);
    game_dispose_from_old_map();
    memset(input_globals.gamepads, 0, sizeof(input_globals.gamepads));
    g_object_message_queue->clear();
    main_render_purge_effect_messages(0);
    g_decal_message_queue->clear();
    bool started = main_game_start(&options);
    main_game_globals.reset_in_progress = false;
    if (!started)
        main_game_load_panic();
    unlock_resources_and_resume_render_thread(resume_token);
}

// @address 0x119a3f0 g_main_game_previous_game_mode
long g_main_game_previous_game_mode;

// @address 0x1e3a10 main_game_load_scenario
extern "C" unsigned char main_game_load_scenario(game_options *options)
{
    bool result = false;
    long phase = 0;
    char *path = options->scenario_path;
    long load_status = loading_get_map_load_state(-1, path);
    if (load_status == 0 || load_status == 1) {
        main_game_unload_and_prepare_for_next_game(0);
        main_game_set_map_memory_stage(1);
        main_game_physical_memory_stage_push();
        main_game_globals.status = 6;
    }
    if (main_kick_startup_masking_sequence(1) != 0) {
        bool playing_bink = bink_globals.active != 0 && bink_globals.enabled != 0;
        if (!playing_bink) {
            if (g_main_game_previous_game_mode == 1 && options->game_mode == 1)
                main_game_initialize_loading_progress(path);
            g_main_game_previous_game_mode = options->game_mode;
        }
    }
    else {
        if (g_main_game_previous_game_mode == 1 && options->game_mode == 1)
            main_game_initialize_loading_progress(path);
        g_main_game_previous_game_mode = options->game_mode;
    }
    if (cache_files_windows_is_external_map_path(path) != 0)
        content_catalogue_make_ready_for_path(path, true);
    if ((unsigned long)load_status >= 2) {
        if ((unsigned long)load_status == 3)
            phase = 1;
    }
    else {
        phase = loading_load_map_with_progress(-1, path, 2);
        if (load_status == 0 || load_status == 1) {
            physical_memory_pop_stage(5);
            main_game_globals.status = 0;
            main_game_unload_and_prepare_for_next_game(options);
        }
    }
    if ((unsigned char)phase != 0) {
        main_game_physical_memory_stage_push();
        main_game_globals.status = 1;
        if (scenario_tags_try_and_load(options->campaign_id, options->map_id, path) != 0) {
            unsigned long populate = main_game_initialize_map_resources(options);
            main_game_globals.status = 2;
            csstrnzcpy(main_game_globals.scenario_path, path, sizeof(main_game_globals.scenario_path));
            if ((populate & 1) != 0) {
                long zone_set_index = options->initial_zone_set_index;
                bool advance = true;
                scenario *scenario_definition = global_scenario;
                if (zone_set_index >= 0 && zone_set_index < scenario_definition->zone_sets.count) {
                    scenario_bsp_state_out out_state;
                    scenario_get_bsp_state(&out_state);
                    s_scenario_zone_set *entries = (s_scenario_zone_set *)scenario_definition->zone_sets.address;
                    s_scenario_zone_set *entry = &entries[zone_set_index];
                    out_state.required_structure_bsp_mask |= entry->bsp_active_mask;
                    out_state.required_cinematic_zone_mask |= entry->cinematic_zones;
                    out_state.required_designer_zone_mask |= entry->designer_zone_required_mask;
                    long zone_data = scenario_zone_set_get_data();
                    cache_file_tag_resource_set_active_state(g_geometry_globals.manager, global_scenario_index, zone_data, &out_state);
                    advance = scenario_commit_zone_resources(1);
                }
                if (advance != 0)
                    result = true;
                else
                    main_game_globals.status = 3;
            }
            else
                main_game_globals.status = 3;
        }
        if (!result) {
            if (main_game_globals.status == 3) {
                main_game_internal_close_caches();
                main_game_reset_scenario_globals();
            }
            physical_memory_pop_stage(5);
            main_game_globals.status = 0;
        }
    }
    if (!result)
        main_halt_and_display_errors();
    input_abstraction_globals.last_update_milliseconds = GetTickCount();
    input_update();
    bink_globals.ready = 1;
    if (bink_playback_is_playing()) {
        do {
            input_update();
            main_update_idle_tasks();
            cache_files_windows_idle();
        } while (bink_playback_is_playing());
    }
    if (g_loading_progress.visible) {
        g_loading_progress.stage = -1;
        memset(g_loading_progress.current, 0, sizeof(g_loading_progress.current));
        g_loading_progress.progress = 1.0f;
        if (g_main_render_loading_globals.initialized && render_is_loading_complete(1) == 0) {
            do {
                main_update_idle_tasks();
            } while (render_is_loading_complete(1) == 0);
        }
        g_loading_progress.visible = false;
        g_loading_progress.scenario_path = 0;
    }
    return result;
}

struct s_effect_message;
// The effect-message flush callback is intentionally empty.


static __forceinline void main_game_effects_flush_messages()
{
    ((t_message_queue<s_effect_message, 512> *)g_effect_message_queue)->process_messages(
        (void (*)(s_effect_message *, long))effects_dispose_from_old_structure_bsp, 0);
}

// @address 0x1e35b0 ?main_game_change_immediate@@YA_NPBUgame_options@@@Z
bool main_game_change_immediate(const game_options *options)
{
    bool success = false;
    unsigned long lock = ::_internal_halt_render_thread_and_lock_resources("..\\main\\main_game.cpp", 526);
    g_object_message_queue->clear();
    main_render_purge_effect_messages(0);
    g_decal_message_queue->clear();
    main_game_effects_flush_messages();
    main_game_unload_and_prepare_for_next_game(options);
    main_game_effects_flush_messages();
    main_reset_global_requests(0);
    main_game_effects_flush_messages();
    main_set_status_value("map", 0);
    if (!options)
    {
        main_game_internal_pregame_load();
        success = true;
    }
    else
    {
        memset(input_globals.gamepads, 0, sizeof(input_globals.gamepads));
        main_set_status_value("map", "loading %s", options->scenario_path);
        main_game_effects_flush_messages();
        if (main_game_load_scenario((game_options *)options) && (main_game_effects_flush_messages(), main_game_start(options)))
            {
                main_set_status_value("map", "loaded %s", options->scenario_path);
                long minor_version = -1;
                if (global_scenario)
                    minor_version = g_cache_file_globals.header.minor_version;
                main_set_status_value("minor_version", "%i", minor_version);
                c_datamine event(0, "map loaded", 2, "main", "game");
                global_preferences_flush();
                if (options->game_mode == 1)
                {
                    data_mine_insert_single_player_game_options("game start");
                    if (options->game_mode == 1 && options->map_id != -1)
                        user_interface_set_last_campaign_level(options->campaign_id, options->map_id);
                    if (!options->game_playback && !game_campaign_has_multiple_players())
                    {
                        long controller_index = g_user_interface_globals.active_controller.peek();
                        if (controller_index != -1)
                        {
                            game_state_try_and_load_from_persistent_storage((e_controller_index)controller_index);
                            __lwsync();
                            _InterlockedExchange(&g_user_interface_globals.active_controller.value, -1);
                        }
                    }
                }
                else if (options->game_mode == 3)
                {
                    if (!main_startup_sequence())
                        main_game_user_interface_enter_game_shell();
                    XNotifyDelayUI(0);
                }
                success = true;
            }
            else
        {
            main_set_status_value("map", "load-failed %s", options->scenario_path);
            main_game_load_panic();
        }
    }
    unlock_resources_and_resume_render_thread(lock);
    return success;
}

// @address 0x19a840 ??0game_options@@QAA@XZ
s_main_game_globals main_game_globals;


// @address 0x948f78 main_globals
// @address 0x74b910 g_havok_monitor_globals
// @address 0x11df878 g_object_message_queue
// @address 0x8a1ead g_utility_drive_mounted
// @address 0x81ac8 cache_files_windows_idle
// @address 0x59ff68 main_game_submit_live_request
