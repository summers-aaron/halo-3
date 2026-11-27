// probe only: cache_files_windows_idle (retail 0x82081ac8), not in tu/ yet (metadata: "prebuilt" between library
// functions). Decompiled from the PPC listing (recomp oracle sub_82081AC8):
//   - copying mode (no DVD streaming override, and cache files enabled with the utility drive mounted, or disabled):
//     when the pending copy task is done, advance the map copy;
//   - streaming mode: take the queued cached-map request (m_cached_maps[1]); unless it has failed 3 times or is
//     already loaded (status 3), close the cached map slot it displaces (lower level) and open it into a free one.
#include <string.h>
#include "cseries/cseries.h"
#include "cache/cache_files_globals_types.h"
#include "cache/cache_files_pending_load_types.h"
#include "saved_games/game_state_xenon_types.h"

extern "C" bool g_utility_drive_mounted;
extern "C" s_cache_files_windows_pending_load g_cache_files_windows_pending_load;
extern s_xenon_game_state_globals xenon_game_state_globals;
extern "C" void cache_files_windows_update_map_copy();
s_failed_map *find_failed_map(const char *path);
extern "C" long cache_files_windows_get_map_status(const char *path);
extern "C" void cached_map_file_close(long index);
enum e_map_file_index cached_map_files_find_free_map(long size, short type);
extern "C" bool game_state_xenon_open_file(long file_index, const char *path);
void cache_file_map_has_failed(const char *path, long load_action);

extern "C" void cache_files_windows_idle()
{
    bool streaming = CACHE_FILES_OVERRIDE || (CACHE_FILES_ENABLED && !g_utility_drive_mounted);
    if (!streaming) {
        if (g_cache_files_windows_pending_load.task_done.peek() != 0)
            cache_files_windows_update_map_copy();
        return;
    }
    s_cache_files_windows_cached_map *request = &g_cache_files_windows_pending_load.m_cached_maps[1];
    if (request->name.m_string[0] == 0)
        return;
    s_failed_map *failed = find_failed_map(request->name.m_string);
    if (!(failed && failed->failure_count >= 3) && cache_files_windows_get_map_status(request->name.m_string) != 3) {
        long level = request->level;
        long cached = xenon_game_state_globals.cached_map_file_index;
        bool open = true;
        if (xenon_game_state_globals.active_file_index != cached && cached != -1) {
            if (cached == xenon_game_state_globals.pending_file_index || xenon_game_state_globals.cached_map_level > level)
                open = false;
            else
                cached_map_file_close(cached);
        }
        if (open) {
            if (xenon_game_state_globals.active_file_index == cached || cached != -1) {
                xenon_game_state_globals.cached_map_file_index = -1;
                xenon_game_state_globals.cached_map_level = -1;
            }
            long index = cached_map_files_find_free_map(0, 1);
            if (game_state_xenon_open_file(index, request->name.m_string)) {
                xenon_game_state_globals.cached_map_file_index = index;
                xenon_game_state_globals.cached_map_level = level;
            } else {
                cache_file_map_has_failed(request->name.m_string, level);
            }
        }
    }
    memset(request, 0, sizeof(*request));
}
