// blamlibxbox/game_state_xenon.obj: from the records 0x1d6308 0x1f3318 0x1f35f8 0x1f3730 0x1f3798 0x1f5428 0x1f5530 0x1f55f0 0x1f56e0 0x1f5bc0 0x707c88
// @flags /TP /O2 /Oc
// @address 0x1f5530 game_state_read_from_file
// @address 0x1f5bc0 game_state_xenon_write_file
#include "saved_games/game_state_xenon_externs.h"
#include "saved_games/game_state.h"
#include <ppcintrinsics.h>
#include <string.h>

#include "cache/cache_files.h"
#include "saved_games/game_state_xenon.h"
#include "cache/cache_files_windows.h"
#include "cseries/cseries.h"
#include "cseries/async.h"
#include "cseries/async_helpers.h"
#include "cseries/file_handle_types.h"
#include "cache/map_file_index_types.h"
#include "cseries/create_file_task_types.h"
#include "multithreading/synchronized_value.h"
#include "cache/cache_file_header.h"
#include "saved_games/game_state_memory_mapping_types.h"
#include "saved_games/game_state_xenon_types.h"
#include <new>
#include "cseries/async_completion.h"
#include "cseries/xdk_filetime.h"
#include "cache/cache_files_pending_load_types.h"
#include "cseries/crt_xdk.h"
#include "main/main_halt.h"
// @address 0x1f5af0 ?cached_map_file_get_path@@YAXW4e_map_file_index@@PADI@Z

// @address 0x84f90 ?simple_yield_function@@YA_NPAVc_synchronized_long@@@Z
// @address 0x59eb68 CompareFileTime
// @address 0x1f3318 ??0s_xenon_game_state_globals@@QAA@XZ
s_xenon_game_state_globals::s_xenon_game_state_globals()
    : s_xenon_game_state_globals_base(this)
{
}

// @address 0x9ae8c0 ?xenon_game_state_globals@@3Us_xenon_game_state_globals@@A
s_xenon_game_state_globals xenon_game_state_globals;

static __forceinline bool game_state_xenon_header_matches(long file_index)
{
    unsigned long mismatch_mask;
    bool result = true;
    if (!game_state_xenon_compare_file_times(
            &xenon_game_state_globals.files[file_index].header, &mismatch_mask))
        result = false;
    return result;
}

// @address 0x1f35f8 game_state_xenon_open_file
extern "C" bool game_state_xenon_open_file(long file_index, const char *path)
{
    unsigned long start_time = GetTickCount();
    char clean_path[256];
    clean_path[0] = 0;
    bool success = false;
    s_xenon_game_state_file *file = &xenon_game_state_globals.files[file_index];

    cache_files_windows_get_canonical_map_path(path, clean_path);
    if (cached_map_file_open((s_file_handle *)&file->file_handle, clean_path) && game_state_read_from_file(file_index) &&
        game_state_xenon_header_matches(file_index))
    {
        char *file_path = &file->header.source_path[0];
        strncpy(file_path, clean_path, sizeof(clean_path));
        success = true;
        file_path[sizeof(clean_path) - 1] = 0;
    }
    else
    {
        cached_map_file_close(file_index);
    }
    if (success)
    {
        unsigned long elapsed = GetTickCount() - start_time;
    }
    return success;
}

// @address 0x1f3730 game_state_xenon_try_and_get_data
extern "C" char *game_state_xenon_try_and_get_data(const char *path)
{
    char *result = NULL;
    long file_index = game_state_xenon_find_file_by_path(path);
    if (game_state_xenon_is_file_valid(file_index))
    {
        s_xenon_game_state_file *file = &xenon_game_state_globals.files[file_index];
        if (file->header.saved_game_data)
            result = (char *)file->header.trailing_fields;
    }
    return result;
}

static __forceinline bool game_state_xenon_file_index_valid(long file_index)
{
    return file_index >= 0 && file_index <= 14;
}

// @address 0x1f3798 game_state_xenon_is_file_valid
bool game_state_xenon_is_file_valid(long file_index)
{
    bool result = false;

    if (file_index != -1 && game_state_xenon_file_index_valid(file_index))
    {
        if (xenon_game_state_globals.files[file_index].header.source_path[0])
        {
            if (file_index == xenon_game_state_globals.active_file_index ||
                file_index == xenon_game_state_globals.pending_file_index ||
                file_index == 0 || file_index == 1 || file_index == 2)
                result = true;
        }
    }
    return result;
}

// @address 0x9dc200 g_cache_files_windows_pending_load

// @address 0x1f5428 game_state_xenon_get_file_version
extern "C" long game_state_xenon_get_file_version(const char *path)
{
    long result = 0;
    long file_index = game_state_xenon_find_file_by_path(path);
    if (file_index != -1 && game_state_xenon_file_index_valid(file_index))
        result = xenon_game_state_globals.files[file_index].header.file_size;
    else if (cache_files_windows_get_map_status(path) == 2 && g_cache_files_windows_pending_load.countdown >= 7)
        result = g_cache_files_windows_pending_load.bytes_total;
    return result;
}

bool game_state_read_from_file(long file_index)
{
    bool success = false;
    c_synchronized_long done(0);
    c_synchronized_long bytes_read(0);
    char file_name[256];
    s_xenon_game_state_file *file = &xenon_game_state_globals.files[file_index];

    cached_map_file_get_path((e_map_file_index)file_index, file_name, sizeof(file_name));
    async_read_position(file->file_handle, file->buffer, sizeof(file->buffer), 0, _async_category_none,
        _async_priority_blocking_below_sound, &bytes_read, &done);
    async_block_until_done(&done, simple_yield_function, false, false, 0, 0);
    if (bytes_read.peek() == sizeof(file->buffer) &&
        ::cache_file_header_verify((s_cache_file_header const *)file->buffer, (char const *)(file_name), false))
        success = true;
    return success;
}

// @address 0x1f55f0 game_state_xenon_compare_file_times
bool game_state_xenon_compare_file_times(const s_cache_file_header *snapshot, unsigned long *mismatch_mask)
{
    bool all_match = true;

    mismatch_mask[0] = 0;
    for (long slot = 0; slot < 3; slot++)
    {
        if (snapshot->level_mask & (1 << slot))
        {
            bool match = false;
            long file_index = -1;

            switch (slot)
            {
            case 0: file_index = 0; break;
            case 1: file_index = 1; break;
            case 2: file_index = 2; break;
            }

            if (xenon_game_state_globals.files[file_index].header.source_path[0] &&
                CompareFileTime(&xenon_game_state_globals.files[file_index].header.timestamp, &snapshot->level_timestamps[slot]) ==
                    0)
                match = true;
            if (!match)
            {
                all_match = false;
                mismatch_mask[slot >> 5] |= 1 << (slot & 31);
            }
        }
    }
    return all_match;
}

// @address 0x1f56e0 game_state_xenon_find_file_by_path
long game_state_xenon_find_file_by_path(const char *path)
{
    long result = -1;
    char clean_path[256];

    clean_path[0] = 0;
    cache_files_windows_get_canonical_map_path(path, clean_path);
    for (long file_index = 0; file_index < 15; file_index++)
    {
        if (csstricmp(clean_path, xenon_game_state_globals.files[file_index].header.source_path) == 0)
        {
            result = file_index;
            break;
        }
    }
    return result;
}

// 0x1f5bc0
extern "C" long game_state_xenon_write_file(long file_index, long category, long priority, unsigned char *result, void *done)
{
    s_async_task task;
    s_xenon_game_state_file *file = &xenon_game_state_globals.files[file_index];

    memset(file->buffer, 0, sizeof(file->buffer));
    task.write_file.handle = file->file_handle;
    task.write_file.count = sizeof(file->buffer);
    task.write_file.buffer = file->buffer;
    task.write_file.result = result;
    return async_task_add_ex((e_async_priority)priority, &task, (e_async_category)category,
        (e_async_completion (*)(s_async_task *))async_write_file_callback, (c_synchronized_long *)done, true);
}

// @address 0x94a4ac g_game_state_async_file_state

// @address 0x1d6308 ?game_state_create_or_open_file@@YAXXZ
void game_state_create_or_open_file(void)
{
    char filename[256];
    s_create_file_task task;
    long file_index = 0;
    void **handle_ptr = g_game_state_async_file_state.file_handles;
    long done_value;
    do {
        c_synchronized_long done0(0);
        c_synchronized_long done1(0);
        csnzprintf(filename, sizeof(filename), "cache1:\\savegame%d.bin", file_index);
        memset(&task, 0, sizeof(task));
        strncpy(task.file_name, filename, sizeof(task.file_name));
        task.file_name[sizeof(task.file_name) - 1] = 0;
        task.access = 0xc0000000;
        task.share_mode = 0;
        task.disposition = 4;
        task.flags = 0;
        task.file = (s_file_handle *)handle_ptr;
        task.create_directories = false;
        async_task_add_ex(_async_priority_blocking_below_sound, (s_async_task *)&task,
            _async_category_saved_games, async_create_file_callback, &done0, true);
        async_block_until_done(&done0, simple_yield_function, false, false, 0, 0);
        bool file_open = (*handle_ptr != NULL) && (*handle_ptr != (void *)-1);
        if (file_open) {
            async_set_file_size(*(s_file_handle *)handle_ptr, 0x7e0000, _async_category_saved_games,
                _async_priority_blocking_below_sound, &done1, &done0);
            async_block_until_done(&done0, simple_yield_function, false, false, 0, 0);
        }
        done_value = done1.peek();
        if (done_value != 1)
            main_halt_and_display_errors();
        file_index++;
        handle_ptr++;
    } while (file_index < 2);
    g_game_state_async_file_state.slot_valid[2] = done_value;
}
