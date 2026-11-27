// blamlibxbox/loading.obj: from the records 0x843f0 0x1efd58 0x1efe08 0x1f0188 0x1f0370 0x1f0998
// @flags /TP /O2 /Oc
#include "bink/bink_playback_types.h"
#include "game/game_types.h"
#include "math/real_math_constants.h"
#include "shell/utility_drive.h"
#include "memory/tls_globals.h"
#include <string.h>
#include "game/game_globals_storage.h"
#include "networking/logic/network_session_interface.h"
#include "cache/cache_files_windows.h"
#include "main/main.h"
#include "saved_games/game_state_xenon.h"
#include "main/loading.h"
#include "main/loading_progress.h"
#include "tag_files/tag_resource_cache_paging.h"
#include "bink/bink_playback.h"
#include "main/levels.h"
#include "main/levels_globals.h"
#include "cseries/crt_xdk.h"
#include "scenario/scenario_tag.h"
#include "cache/geometry_globals.h"
#include "scenario/scenario.h"
#include "cache/cache_files_windows_paths.h"
#include "interface/user_interface_global_flags_types.h"
#include "main/levels_map_datum_types.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_cache_new.h"
#include "main/game_preferences_types.h"
#include "main/game_preferences_game_setup.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "game/game_state_accessors.h"
// @address 0x8a1ead g_utility_drive_mounted
// @address 0x74b898 g_geometry_globals
// @address 0x96e058 ?g_level_globals@@3Us_levels_globals@@A
// @address 0x1f0998 loading_update_progress
// @address 0x81ac8 cache_files_windows_idle

struct s_cache_files_windows_load_status;
extern "C" {
// @address 0x7db5ec g_loading_progress
loading_progress_globals g_loading_progress = {};
}

// @address 0x8973c0 g_loading_map_path_cache
extern "C" { loading_map_path_cache g_loading_map_path_cache; }

// @address 0x843f0 loading_get_campaign_map
extern "C" void loading_get_campaign_map(int *campaign_id, int *map_id)
{
    const s_gui_game_setup_storage::s_campaign_settings *setup =
        &global_preferences_get_game_setup_storage()->campaign_settings;
    int current_campaign_id = setup->campaign_id;
    int current_map_id = setup->map_id;
    char valid = setup->valid;
    *campaign_id = current_campaign_id;
    *map_id = current_map_id;
    if (valid == 0 || current_campaign_id == -1 || current_map_id == -1) {
        *campaign_id = 1;
        *map_id = 0xbbd;
    }
}
// @address 0x1efd58 get_next_scenario_path
char *get_next_scenario_path(char *path,long path_size)
{
    const game_options *options = &tls_game_globals()->options;
    path[0] = 0;
    if (options && options->game_mode == 1) {
        long campaign = options->campaign_id;
        long map = options->map_id;
        if (campaign == -1) campaign = levels_find_campaign_for_map_name(options->scenario_path);
        if (campaign != -1) {
            e_map_id next_map = levels_get_campaign_next_map_id((e_campaign_id)campaign,(e_map_id)map);
            if (next_map != _map_id_none) {
                levels_map_datum level;
                if (levels_try_and_get_datum((s_data_array *)g_level_globals.map_levels,next_map,(s_datum *)&level)) csstrnzcpy(path,level.name,256);
            }
        }
    }
    return path;
}
static __forceinline bool loading_bink_playing()
{
    return bink_globals.active && bink_globals.enabled;
}
static __forceinline long loading_cached_level(const char *path)
{
    long cached = cache_files_windows_get_active_map_level(path);
    if (cached == -1)
        cached = cache_files_windows_get_pending_map_level(path);
    return cached;
}
static __forceinline void next_scenario_path_set(s_next_scenario_path *path, const char *source, long type)
{
    strncpy(path->path, source, 256);
    path->type = type;
    path->path[255] = 0;
}
// @address 0x1efe08 loading_get_next_scenario_path
extern "C" bool loading_get_next_scenario_path(s_next_scenario_path *result)
{
    scenario *current = global_scenario;
    bool has_scenario = current != 0;
    bool is_mainmenu = false;
    bool is_campaign = false;
    long intro_state = cache_files_windows_get_map_status("levels\\solo\\005_intro\\005_intro");
    bool is_intro;
    {
        int campaign_id;
        int map_id;
        loading_get_campaign_map(&campaign_id, &map_id);
        is_intro = campaign_id == 1 && map_id == 0xbbd;
    }
    bool matchmaking = network_life_cycle_in_matchmaking();
    {
        e_campaign_id session_campaign_id;
        e_map_id session_map_id;
        if (network_life_cycle_session_get_map(&session_campaign_id, &session_map_id)) {
            if (session_map_id != 0xbbd)
                is_intro = false;
            if (!matchmaking) {
                levels_map_datum level;
                matchmaking = levels_try_and_get_datum((s_data_array *)g_level_globals.multiplayer_levels, session_map_id, (s_datum *)&level);
            }
        }
    }
    if (has_scenario) {
        is_mainmenu = current->type == 2;
        is_campaign = 0 == current->type;
    }
    s_next_scenario_path path;
    path.has_path = true;
    path.matchmaking = false;
    path.path[0] = 0;
    path.type = 0;
    if (cache_files_windows_get_map_status("levels\\shared\\shared\\shared") == 0)
        next_scenario_path_set(&path, "levels\\shared\\shared\\shared", 5);
    else if (cache_files_windows_get_map_status("levels\\ui\\mainmenu\\mainmenu") == 0)
        next_scenario_path_set(&path, "levels\\ui\\mainmenu\\mainmenu", 6);
    else if (matchmaking)
        path.matchmaking = true;
    else if ((is_mainmenu || !has_scenario) && is_intro && (intro_state == 0 || intro_state == 1))
        next_scenario_path_set(&path, "levels\\solo\\005_intro\\005_intro", 4);
    else if (is_mainmenu && !is_intro && loading_cached_level("levels\\solo\\005_intro\\005_intro") == 4) {
        path.has_path = false;
        next_scenario_path_set(&path, "levels\\solo\\005_intro\\005_intro", 0);
    }
    else if (cache_files_windows_get_map_status("levels\\shared\\shared\\campaign") == 0)
        next_scenario_path_set(&path, "levels\\shared\\shared\\campaign", 3);
    else if (is_campaign) {
        char name[256];
        get_next_scenario_path(name, 256);
        if (name[0] != 0)
            next_scenario_path_set(&path, name, 0);
    }
    else if (is_mainmenu && !loading_bink_playing()) {
        char map_name[256];
        int campaign;
        int map;
        loading_get_campaign_map(&campaign, &map);
        if (campaign != -1 && map != -1) {
            if (campaign == g_loading_map_path_cache.campaign && map == g_loading_map_path_cache.map) {
                strncpy(map_name, g_loading_map_path_cache.path, 260);
            }
            else {
                map_name[0] = 0;
                levels_get_map_scenario_path(campaign, map, map_name, 256);
                strncpy(g_loading_map_path_cache.path, map_name, 260);
                g_loading_map_path_cache.campaign = campaign;
                g_loading_map_path_cache.map = map;
            }
        }
        if (map_name[0] != 0)
            next_scenario_path_set(&path, map_name, 0);
    }
    if (result)
        *result = path;
    return true;
}
// @rdata 0x8202490c k_loading_progress_epsilon
static const float k_loading_progress_epsilon = 0.0001f;

void loading_update_progress(unsigned int units)
{
    if (!g_loading_progress.visible) return;
    if ((unsigned long)g_loading_progress.stage > 2) return;
    float weight[3][2] = { {0.0f, 8388608.0f}, {8388608.0f, 16777216.0f}, {8388608.0f, 16777216.0f} };
    float maximum = g_loading_progress.maximum[g_loading_progress.stage];
    float value = (float)units;
    g_loading_progress.maximum[g_loading_progress.stage] = value > maximum ? value : maximum;
    g_loading_progress.current[g_loading_progress.stage] = value;
    float total = 0.0f;
    float loaded = 0.0f;
    for (unsigned long i = 0; i < 3; i++) {
        float w = weight[i][g_utility_drive_mounted];
        total += g_loading_progress.maximum[i] * (1.0f / w);
        loaded += g_loading_progress.current[i] * (1.0f / w);
    }
    if (total > k_loading_progress_epsilon) {
        g_loading_progress.progress = 1.0f - loaded / total;
        return;
    }
    g_loading_progress.progress = 0.0f;
}
// @address 0x1f0188 loading_load_map_with_progress
extern "C" long loading_load_map_with_progress(short progress,const char *path,long type)
{
    long result = 1;
    long state = cache_files_windows_get_map_status(path);
    if (state != 3 && state != 4) {
        bool full = false;
        long level;
        switch (type) {
        case 0:
            level = 0;
            break;
        case 1:
            level = 1;
            break;
        default:
            level = 2;
            full = true;
            break;
        }
        long cached = cache_files_windows_get_active_map_level(path);
        if (cached == -1)
            cached = cache_files_windows_get_pending_map_level(path);
        if (cached < level)
            cache_files_windows_request_map_load(path,level);
        if (full) {
            long total = game_state_xenon_get_file_version(path);
            if (g_loading_progress.visible) {
                g_loading_progress.stage = 0;
                loading_update_progress((unsigned int)total);
            }
            float one = 1.0f;
            do {
                float done_fraction = cache_files_windows_get_map_copy_progress(path);
                float remain = one - done_fraction;
                unsigned int done = (unsigned int)game_state_xenon_get_file_version(path);
                loading_update_progress((unsigned int)((float)done * remain));
                cache_files_windows_process_loading_requests();
                state = cache_files_windows_get_map_status(path);
                if (state == 0)
                    cache_files_windows_request_map_load(path,level);
                cache_files_windows_idle();
                SwitchToThread();
                main_update_idle_tasks();
                state = cache_files_windows_get_map_status(path);
            } while (state == 2 || state == 1 || state == 0);
            if (g_loading_progress.visible) {
                loading_update_progress(0);
                g_loading_progress.stage = (unsigned long)-1;
            }
            state = cache_files_windows_get_map_status(path);
        }
    }
    if (type == 2) {
        if (state == 4)
            result = 0;
    }
    else if (type == 1 && g_geometry_globals.manager != 0) {
        cache_files_windows_update_load_status((s_cache_files_windows_load_status *)g_geometry_globals.storage,progress,path);
    }
    return result;
}

static __forceinline unsigned char loading_resources_loaded(c_cache_file_tag_resource_runtime_manager *manager, short map_index, const char *path)
{
    unsigned char result = false;
    if (manager->m_cache_file_resource_gestalt_index != -1) {
        char name[256];
        name[0] = 0;
        cache_files_windows_get_canonical_map_path(path, (c_static_string<256> *)name);
        if (!manager->m_active_zone_changed && strncmp(manager->m_zone_names.m_first.m_name.get_string(), name, 256) == 0 &&
            (map_index == -1 || manager->m_zone_names.m_first.m_value == map_index))
            result = tag_resource_cache_is_loaded(&manager->m_resource_cache.m_geometry_storage.m_page_table);
    }
    return result;
}
// @address 0x1f0370 loading_get_map_load_state
extern "C" long loading_get_map_load_state(short map_index, const char *path)
{
    long result;
    switch (cache_files_windows_get_map_status(path)) {
    case 0:
        result = 0;
        break;
    case 1:
    case 2:
        result = 1;
        break;
    case 3:
        result = 3;
        break;
    case 4:
        result = 4;
        break;
    default:
        // match: 0x1f0370: unreachable hint omits dead branch check
        __assume(0);
    }
    if (game_in_progress() && result == 3) {
        unsigned char loaded = true;
        if (g_geometry_globals.manager)
            loaded = loading_resources_loaded(g_geometry_globals.manager, map_index, path);
        result = (loaded != 0) + 2;
    }
    return result;
}
