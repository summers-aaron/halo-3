// blamlibxbox/saved_game_files.obj: from the records 0x8ad68 0x1a80d8 0x1ceba0 0x1d6a58 0x1e0b68 0x1e14f0 0x1e72f8 0x1e73f8 0x1e7490 0x1e74f0 0x1e75c8 0x1e76f8 0x1e7820 0x1e7b48 0x1e7be8 0x1e7ca8 0x1e7d88 0x1e7e70 0x1e7f08 0x1e7fc0 0x1e8170 0x1e81d0 0x1e82e8 0x1e8370 0x1e83e0 0x1e84a8 0x1e8578 0x1e8618 0x1e86b8 0x1e8750 0x1e88a8 0x1e8980 0x1e8a58 0x1e8ce0 0x1e8d88 0x1e9240 0x1e9350 0x1e94a8 0x1e9518 0x1e97e0 0x1e9c20 0x1e9ca8 0x1e9d80 0x1e9e28 0x1e9f60 0x1ea1c0 0x1ea230 0x1ea3f8 0x1ea490 0x1ea650 0x1ea8e8 0x1ea9f0 0x1eab50 0x1eada8 0x1eae38 0x1eaea0 0x1eaef8 0x1eaf50 0x1eafa8 0x1eb020 0x1eb290 0x1eb338 0x1eba38 0x1eba98 0x1ebb00 0x1ed3e0 0x205db8 0x228138 0x228908 0x228998 0x228ba0 0x228e68 0x229270 0x265a38 0x284bf0 0x284e08 0x4a4390 0x4a47d0 0x4a48e8 0x4e0d30
#include "saved_games/saved_game_collect_files.h"
#include "text/text_group.h"
#include "saved_games/saved_film_work_types.h"
#include "interface/user_interface_time.h"
#include "saved_games/content_catalogue_types.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
#include "saved_games/game_content.h"
#include "tag_files/files_windows_types.h"
#include "memory/static_arrays.h"
#include "saved_games/game_content_types.h"
#include "multithreading/thread_tls.h"
#include "interface/c_player_profile.h"
#include "interface/controller_types.h"
#include "cseries/async_helpers.h"
#include "tag_files/tag_types.h"
#include "shell/shell.h"
#include "game/game_variant.h"
#include "game/game_globals_storage.h"
#include "saved_games/saved_game_item_metadata.h"
#include "interface/gui_selected_item_metadata.h"
#include "interface/game_tag_parser.h"
#include "cseries/async.h"
#include "networking/online/online_file_string_specializations.h"
#include "interface/user_interface_error_manager.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include <string.h>
#include <ppcintrinsics.h>
#include <wctype.h>
#include "multithreading/synchronization.h"
#include "game/game.h"
#include "interface/user_interface.h"
#include "text/unicode.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "interface/user_interface_text_parser.h"
#include "main/main.h"
#include "networking/tools/network_blf.h"
#include "saved_games/saved_game_files.h"
#include "saved_games/saved_game_files_operations.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "cache/cache_files.h"
#include "game/game_engine_sandbox.h"
#include "interface/user_interface_utilities.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/saved_film.h"
#include "bink/bink_sdk.h"
#include "interface/user_interface_shared_globals.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "tag_files/file_reference.h"
#include "saved_games/saved_game_file_category_types.h"
#include "saved_games/marketplace_asset_types.h"
#include "saved_games/marketplace_asset_task_types.h"
#include "saved_games/content_synchronization_globals_types.h"
#include "saved_games/text_stream_types.h"
#include "saved_games/saved_game_files_context_types.h"
#include "saved_games/buffered_file_writer_types.h"
#include "interface/parser_entry_types.h"
#include "saved_games/content_critical_section_types.h"
#include "multithreading/threads.h"
#include "saved_games/saved_game_file_types.h"
#include "networking/tools/blf_header_types.h"
#include "memory/gameplay_tls_globals.h"
#include "memory/physical_memory_allocation.h"
#include "saved_games/saved_film_storage.h"
#include "memory/tls_globals.h"
#include "saved_games/content_catalogue_queries.h"
#include "saved_games/content_catalogue_iteration.h"
#include "main/main_game.h"
#include "saved_games/saved_film_globals.h"
#include "memory/bitstream_wide_strings.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "main/thread_globals_lock.h"
#include "main/game_preferences_types.h"
#include "main/game_preferences_inlines.h"
#include "saved_games/saved_game_files_globals.h"
#include "saved_games/saved_film_recording_types.h"
#include "multithreading/synchronized_value.h"
#include "saved_games/game_state_after_load.h"
#include "main/main_time.h"
#include "objects/object_broadphase.h"
#include "camera/director.h"
#include "physics/havok.h"
#include "sound/sound_manager.h"
#include "sound/game_sound.h"
#include "camera/observer.h"
#include "game/players.h"
#include "simulation/simulation.h"
#include "render/render_objects.h"
#include "saved_games/game_state.h"
#include "ai/ai.h"
#include "effects/decals.h"
#include "render/water_renderer.h"
#include "game/campaign_metagame.h"
#include "saved_games/content_catalogue_operations.h"
#include "cseries/async_file_write.h"
#include <time.h>
#include "text/utf8_conversion.h"
#include "cseries/xenon_platform_api.h"
#include "saved_games/game_content_conversion.h"
#include "memory/bitstream.h"
#include "networking/session/network_session_interface_types.h"
#include "text/ascii_wchar_conversion.h"
#include "tag_files/c_file_reference.h"
#include "saved_games/saved_game_variant_file_types.h"
#include "game/game_engine_base_variant.h"
#include "networking/tools/blf_authentication_types.h"
#include "game/game_options.h"
#include "cseries/crt_xdk.h"
#include "cseries/cseries.h"
#include "saved_games/saved_game_scope_lock.h"
#include "saved_games/content_item_types.h"
#include "saved_games/game_state_header_types.h"
#include "saved_games/game_state_saved_header.h"
#include "interface/c_player_marketplace.h"
#include "interface/marketplace_consume_assets_task.h"
#include "networking/online/online_file_game_api_parser.h"
#include "interface/controller_marketplace_types.h"
#include "game/game_engine_scoring_globals.h"
#include <wchar.h>
#include "game/map_variant_files.h"
#include "sound/sound_cache_inline.h"
#include "tag_files/string_ids.h"
#ifdef HALO3_64BIT
#include "game/game_state_accessors.h"
#include "interface/user_interface_session.h"
#endif

// @flags /TP /O2 /Oc
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x8af98 ?is_signed_in_to_live@c_controller_interface@@QBA_NXZ
// @address 0x1e83e0 ?saved_game_file_type_to_game_content_type@@YA?AW4e_game_content_type@@W4e_saved_game_file_type@@@Z
// @address 0x1e8578 ?game_engine_index_to_saved_game_file_type@@YA?AW4e_saved_game_file_type@@W4e_game_engine_type@@@Z
// @address 0x8a698 ?is_guest@c_controller_interface@@QBA_NXZ
// @address 0x229270 saved_game_files_copy_file_callback

struct s_file_reference;
struct s_user_interface_shared_globals;
struct s_file_reference;
struct s_file_reference;


// ---- saved game files: pending thread-global values ----
// @address 0x19f3b0 ??0c_thread_globals_lock@@QAA@XZ
// @address 0x19f408 ??1c_thread_globals_lock@@QAA@XZ
extern "C" {
// @address 0x8a1f75 g_saved_game_files_pending_dirty
signed char g_saved_game_files_pending_dirty;
// @address 0x8c297c g_saved_game_files_name_counters
long g_saved_game_files_name_counters[14];


// @address 0x8e3dd8 g_global_preferences_storage

}
extern "C" {
// @address 0x7e3634 g_sound_cache_globals
s_sound_cache_globals g_sound_cache_globals;
}
class c_interlocked_long;

struct s_catalogue_async_task
{
    long catalogue_index;
    c_synchronized_long *completion;
    c_synchronized_long *done;
    long content_item_index;
#ifdef HALO3_64BIT
    char payload[512];
#else
    char payload[280];
#endif
};

struct s_bitmap_load_task
{
    long catalogue_index;
    long content_item_index;
    const wchar_t *name;
    const wchar_t *description;
    c_synchronized_long *completion;
    c_synchronized_long *done;
#ifdef HALO3_64BIT
    char payload[512];
#else
    char payload[264];
#endif
};

struct s_file_delete_task
{
    long controller_index;
    long file_type;
    c_synchronized_long *completion;
    c_synchronized_long *done;
    c_synchronized_long *status;
    unsigned short path[128];
#ifdef HALO3_64BIT
    char task_payload_storage[256];
#endif
};

#pragma pack(push, 4)
struct s_saved_game_content_header {
    unsigned short build_number;
    unsigned short scenario_slot;
    s_saved_game_item_metadata_packed metadata;
};
#pragma pack(pop)

struct s_saved_game_files_copy_task
{
    long state;
    void *source_handle;
    void *destination_handle;
    unsigned long file_size;
    unsigned long count;
    unsigned long copied;
    char *buffer;
    unsigned long buffer_size;
    bool flush;
    c_synchronized_long *done;
    long last_state;
    long catalogue_index;
    long content_item_index;
    long validate;
#ifdef HALO3_64BIT
    char payload[512];
#else
    char payload[288 - 0x38];
#endif
};

struct s_saved_game_files_write_buffer_task
{
    long state;
    long controller_index;
    wchar_t *display_name;
    const void *buffer;
    unsigned long buffer_size;
    s_file_reference *file;
    long *content_item_index;
    c_synchronized_long *success;
};

struct s_saved_game_files_buffered_file_writer {
    s_buffered_file_writer writer;
    char buffer[0x1000];
    __forceinline s_saved_game_files_buffered_file_writer(s_file_reference *file)
    {
        writer.file = file;
        writer.buffer = buffer;
        writer.buffer_size = sizeof(buffer);
        writer.buffer_position = 0;
    }
    __forceinline void flush()
    {
        if (writer.buffer_position > 0)
        {
            file_write(writer.file, writer.buffer_size, writer.buffer);
            writer.buffer_position = 0;
        }
    }
};

typedef char saved_game_files_copy_task_size[(sizeof(void *) != 4 || ((sizeof(s_saved_game_files_copy_task) == 288))) ? 1 : -1];

typedef char saved_game_files_copy_done_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_game_files_copy_task, done) == 0x24))) ? 1 : -1];

typedef char saved_game_files_copy_last_state_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_game_files_copy_task, last_state) == 0x28))) ? 1 : -1];


static __forceinline bool saved_game_files_requests_blocked()
{
    return g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 3;
}

__forceinline void c_saved_film::dispose()
{
    close();
    m_async_buffer.release_storage(&g_saved_film_storage_allocator);
}


namespace {
static __forceinline void autosave_queue_dispose()
{
}

static __forceinline s_saved_film_timer_globals &saved_films_get_globals()
{
    return g_saved_film_timer_globals;
}

static __forceinline void saved_films_dispose()
{
    saved_films_get_globals().film.dispose();
    saved_films_get_globals().initialized = false;
    saved_films_get_globals().delete_on_level_load = false;
}
}

// The image's 27 after-load procs in slot order (0x74b150..0x74b1bc). The eight 0x5bec98 slots are
// the shared empty body: procs compiled empty in release whose names did not survive (March 2007's
// debug table has 20 entries in a different order). The void(void) and unsigned-int callees sit in
// the (long) table as converted function pointers, with no adapter between.
// @address 0x74b150 g_game_state_after_load_handlers
#if !defined(_XBOX) || defined(__clang__)
static void game_state_after_load_proc_empty(long)
{
}

void (*g_game_state_after_load_handlers[27])(long) =
{
    main_time_restore,
    object_broaphase_load_from_game_state,
    game_state_after_load_proc_empty,
    (void (*)(long))game_options_validate_active_variants,
    (void (*)(long))director_reset_all_directors,
    havok_load_from_game_state,
    game_state_after_load_proc_empty,
    sound_stop_all,
    game_sound_restore,
    observer_initialize_for_saved_game,
    players_remap_local_players_from_flags,
    simulation_initialize_for_saved_game,
    render_objects_reset_render_state,
    game_state_after_load_proc_empty,
    game_state_after_load_proc_empty,
    game_state_after_load_proc_empty,
    game_state_set_revert_time,
    (void (*)(long))director_set_saved_film_modes,
    ai_initialize_for_saved_game,
    (void (*)(long))decal_system_destroy_all,
    observer_initialize_after_load_saved_game,
    game_state_after_load_proc_empty,
    game_state_after_load_proc_empty,
    game_state_after_load_proc_empty,
    (void (*)(long))c_water_renderer::initialize_for_new_map,
    (void (*)(long))campaign_metagame_set_disabled_flag,
    main_reset_zone_requests
};
#endif

extern "C" {

// @address 0x94a4fc g_game_state_shared_lock

}
// @address 0x192420 ?unlock_resources_and_resume_render_thread@@YAXK@Z

// @address 0x1d6a58 game_state_handle_after_load
extern "C" void game_state_handle_after_load(long arg)
{
    _InterlockedExchange(&g_game_state_lock.value, 0);
    int token = ::tag_resources_lock_game();
    for (short i = 0; (unsigned short)i < 27; ++i) {
        g_game_state_after_load_handlers[i](arg);
    }
    ::tag_resources_unlock_game((*(long *)(&token)));
    __lwsync();
    _InterlockedExchange(&g_game_state_shared_lock.lock.value, 0);
    unlock_resources_and_resume_render_thread(g_game_state_shared_lock.render_thread_lock);
}

// @address 0x1e14f0 saved_game_files_find_latest_film_path
bool saved_game_files_find_latest_film_path(char *path)
{
    s_file_reference directory;
    s_find_file_data find_data;
    s_file_reference file;
    s_file_reference latest_file;
    s_file_last_modification_date file_time;
    s_file_last_modification_date latest_time;
    bool result = false;

    file_reference_create_from_path(&directory, k_saved_game_autosave_directory, true);
    find_files_start(&find_data, 0, &directory, "*.film");

    bool found = false;
    bool first = true;
    while (find_files_next(&find_data, &file, &file_time)) {
        if (!first && CompareFileTime((const FILETIME *)&file_time, (const FILETIME *)&latest_time) <= 0) {
            continue;
        }
        latest_time = file_time;
        latest_file = file;
        found = true;
        first = false;
    }

    find_files_end(&find_data);

    if (found) {
        file_location_get_full_path(latest_file.location, latest_file.path, path, 256);
        result = true;
    }
    return result;
}

// ---- saved game file i/o completion, text parsers ----
// @address 0x75ddec g_saved_game_files_globals
s_saved_game_files_globals g_saved_game_files_globals =
{
    0,
    { -1, NULL, NULL, NULL, -1 },
    {
        c_magic_string_game_tag_parser(L"<map_variant_name", &g_saved_game_files_globals.context, saved_game_files_parse_first_string),
        c_magic_string_game_tag_parser(L"<game_variant_name", &g_saved_game_files_globals.context, saved_game_files_parse_second_string),
        c_magic_string_game_tag_parser(L"<campaign_map_name", &g_saved_game_files_globals.context, saved_game_files_parse_third_string),
        c_magic_string_game_tag_parser(L"<difficulty", &g_saved_game_files_globals.context, saved_game_files_parse_difficulty_string),
    },
};

// @address 0x1e72f8 saved_game_files_initialize
void saved_game_files_initialize(void)
{
    long index = 0;
    g_saved_game_files_globals.flags = 0;
    memset(&g_saved_game_files_globals.context, 0, sizeof(g_saved_game_files_globals.context));
    for (; index < 14; index++) {
        long value = 0;
        if (!saved_game_files_requests_blocked()) {
            c_thread_globals_lock lock(_inline_thread_globals_lock);
            if ((unsigned long)index <= 13) {
                s_global_preferences *globals = global_preferences_thread_storage();
                value = globals->primary_file.data.saved_game_file_name_counters[index];
            }
        }
        g_saved_game_files_name_counters[index] = value;
    }
    saved_film_initialize();
}


// @address 0x1e73f8 saved_game_files_reset
void saved_game_files_reset(void)
{
    g_saved_game_files_globals.flags = 0;
    g_saved_game_files_globals.context.user_index = 0;
    g_saved_game_files_globals.context.first_string = 0;
    g_saved_game_files_globals.context.second_string = 0;
    g_saved_game_files_globals.context.third_string = 0;
    g_saved_game_files_globals.context.campaign_difficulty = 0;
    saved_game_files_reset_film_state();
}


extern "C" {

}


// @address 0x1e0b68 saved_game_files_reset_film_state
void saved_game_files_reset_film_state(void)
{
    int digests_match;
    int map_digest_valid;
    int compare_full_digest;
    unsigned long map_digest_size;
    const void *map_digest_data;
    int map_digest_unused[2];
    if (tls_game_globals()->options.game_mode == 3)
        return;
    if (main_game_reset_in_progress() != 0)
        return;
    saved_film_reset_playback_state();
    if (((unsigned char)game_is_cooperative()) == 0)
        return;
    if (((unsigned char)game_is_film_playback_active()) != 0) {
        map_digest_size = 0;
        map_digest_data = 0;
        map_digest_valid = 0;
        if (((unsigned char)cache_file_try_and_get_map_digest(&map_digest_size, &map_digest_data)) != 0) {
            if (game_is_map_active() && tls_game_globals()->options.game_mode == 1)
                compare_full_digest = 0;
            else
                compare_full_digest = 1;
            if (map_digest_size == 60) {
                digests_match = cache_file_compare_digests((const unsigned char *)map_digest_data,
                    g_saved_film_timer_globals.film.get_content_name_size(),
                    g_saved_film_timer_globals.film.get_content_name(), ((unsigned char)compare_full_digest));
            }
            else {
                digests_match = 0;
            }
            if (((unsigned char)digests_match) != 0) {
                map_digest_valid = 1;
            }
        }
        if (((unsigned char)map_digest_valid) == 0) {
            if (((unsigned int)(signed char)saved_films_get_globals().request_pending == 0)) {
                saved_films_get_globals().request_pending = 1;
                saved_films_get_globals().requested_state = 15;
            }
        }
    }
    saved_films_get_globals().timer_target = tls_game_globals()->options.playback_start_in_ticks;
    saved_film_reset_work_globals();
    g_saved_film_work_globals.playback_length = tls_game_globals()->options.playback_length_in_ticks;
    saved_film_reset_recording_state();
    XEnableScreenSaver(0);
    saved_films_get_globals().screen_saver_enabled = 0;
}

// @address 0x1e7490 ?saved_game_files_dispose@@YAXXZ
void saved_game_files_dispose()
{
    autosave_queue_dispose();
    saved_films_dispose();
}

// @address 0x1e74f0 saved_game_files_update
void saved_game_files_update(void)
{
    if (g_saved_game_files_pending_dirty) {
        for (long index = 0; index < 14; index++) {
            long value = g_saved_game_files_name_counters[index];
            if (!saved_game_files_requests_blocked()) {
                if ((unsigned long)index <= 13) {
                    c_thread_globals_lock lock;
                    s_global_preferences *globals = global_preferences_thread_storage();
                    globals->dirty = value != globals->primary_file.data.saved_game_file_name_counters[index];
                    globals->primary_file.data.saved_game_file_name_counters[index] = value;
                }
            }
        }
        g_saved_game_files_pending_dirty = 0;
    }
}


class c_content_catalogue;


// ---- saved game files: personal storage content item, saved game file reads ----

struct game_state_header;


extern "C" {

}

// 0x1e75c8


#ifdef HALO3_64BIT
extern "C" unsigned long g_saved_game_personal_storage_generation = 1;
extern "C" void saved_game_files_delete_personal_storage_for_player_count(long controller_index, long player_count);
static long saved_game_files_get_active_player_count(void);
#endif

// @address 0x1e7b48 saved_game_files_delete_personal_storage
extern "C" void saved_game_files_delete_personal_storage(long controller_index)
{
#ifdef HALO3_64BIT
    saved_game_files_delete_personal_storage_for_player_count(controller_index, saved_game_files_get_active_player_count());
#else
    if (((c_controller_interface *)&controller_globals().controllers[controller_index])->is_signed_in_to_machine()) {
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
        long content_item = saved_game_files_get_personal_storage_content_item((e_controller_index)controller_index, false);
        if (content_item != -1) {
            content_catalogue_close_item(catalogue, content_item, true);
            content_catalogue_delete_item(catalogue, content_item, true);
        }
    }
#endif
}

// @address 0x1eba38 saved_game_files_delete_content_item
void saved_game_files_delete_content_item(long controller_index, long content_item_index)
{
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
    content_catalogue_close_item(catalogue, content_item_index, true);
    content_catalogue_delete_item(catalogue, content_item_index, true);
#ifdef HALO3_64BIT
    ++g_saved_game_personal_storage_generation;
#endif
}

// 0x1e7be8


// 0x1e7ca8


// 0x1e7e70


// @address 0x1e7e70 saved_game_files_delete_content_item_async
extern "C" long saved_game_files_delete_content_item_async(
    long catalogue_index, long content_item_index, c_synchronized_long *completion, c_synchronized_long *done)
{
    s_catalogue_async_task task;
    completion->set(0);
    done->set(0);
    task.catalogue_index = catalogue_index;
    task.completion = completion;
    task.done = done;
    task.content_item_index = content_item_index;
    return ::async_task_add_ex(_async_priority_blocking_generic, (s_async_task *)&task, _async_category_saved_games,
        saved_game_files_delete_content_item_callback, done, true);
}


// @address 0x1e7f08 saved_game_files_update_metadata_async
extern "C" long saved_game_files_update_metadata_async(long catalogue_index, long content_item_index, const wchar_t *name,
    const wchar_t *description, c_synchronized_long *completion, c_synchronized_long *done)
{
    s_bitmap_load_task task;
    if (name == 0 && description == 0)
        return -1;
    completion->set(0);
    done->set(0);
    task.catalogue_index = catalogue_index;
    task.content_item_index = content_item_index;
    task.name = name;
    task.description = description;
    task.completion = completion;
    task.done = done;
    return ::async_task_add_ex(_async_priority_blocking_generic, (s_async_task *)&task, _async_category_saved_games,
        saved_game_files_update_metadata_callback, done, true);
}


// @address 0x1ea3f8 saved_game_files_delete_content_item_callback
extern "C" e_async_completion saved_game_files_delete_content_item_callback(s_async_task *task)
{
    s_catalogue_async_task *delete_task = (s_catalogue_async_task *)task;
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[delete_task->catalogue_index];
    bool success;
    if (content_catalogue_has_storage_device(catalogue)) {
        content_catalogue_close_item(catalogue, delete_task->content_item_index, true);
        content_catalogue_delete_item(catalogue, delete_task->content_item_index, true);
        success = true;
    } else {
        success = false;
    }
    return (e_async_completion)saved_game_files_set_async_result(success, 1, &delete_task->completion->value, &delete_task->done->value);
}


static __forceinline void saved_game_files_delete_ustrnzcpy(unsigned short *destination, const unsigned short *source, int size)
{
    const unsigned short *s = source;
    for (int i = 0; i <= size - 1; ++i) {
        unsigned short value = *s;
        destination[i] = value;
        if (value == 0)
            break;
        ++s;
    }
    destination[size - 1] = 0;
}


static __forceinline e_game_content_type saved_game_files_file_type_to_content_type(e_saved_game_file_type file_type)
{
    switch (file_type) {
    case _saved_game_file_type_ctf: return _game_content_type_ctf;
    case _saved_game_file_type_slayer: return _game_content_type_slayer;
    case _saved_game_file_type_oddball: return _game_content_type_oddball;
    case _saved_game_file_type_koth: return _game_content_type_koth;
    case _saved_game_file_type_jugg: return _game_content_type_jugg;
    case _saved_game_file_type_terries: return _game_content_type_terries;
    case _saved_game_file_type_assault: return _game_content_type_assault;
    case _saved_game_file_type_zombiez: return _game_content_type_zombiez;
    case _saved_game_file_type_vip: return _game_content_type_vip;
    case _saved_game_file_type_map: return _game_content_type_map;
    case _saved_game_file_type_film: return _game_content_type_film;
    case _saved_game_file_type_clip: return _game_content_type_clip;
    case _saved_game_file_type_shot: return _game_content_type_shot;
    default: return _game_content_type_none;
    }
}


// @address 0x1e7d88 saved_game_files_create_content_item_async
extern "C" long saved_game_files_create_content_item_async(
    int p0, const unsigned short *p1, int p2, c_synchronized_long *p3, c_synchronized_long *p4, c_synchronized_long *p5)
{
    const unsigned short *source = p1;
    p3->set(0);
    p4->set(0);
    p5->set(-1);
    s_file_delete_task task;
    task.controller_index = p0;
    task.file_type = p2;
    unsigned short *destination = task.path;
    task.completion = p3;
    task.done = p4;
    task.status = p5;
    saved_game_files_delete_ustrnzcpy(destination, source, 128);
    return ::async_task_add_ex(_async_priority_blocking_generic, (s_async_task *)&task, _async_category_saved_games,
        saved_game_files_create_content_item_callback, p4, true);
}

// @address 0x1e8170 saved_game_files_get_content_item_file_path
extern "C" bool saved_game_files_get_content_item_file_path(long catalogue_index, long content_item_index, char *path, long max_path = 256)
{
    bool result = false;
    path[0] = 0;
    if (content_catalogue_get_item_file_path((const c_content_catalogue *)&content_catalogues().catalogues[catalogue_index], content_item_index,
        path, 256)) {
        result = true;
    }
    return result;
}

const char *saved_game_filename_extension_by_game_file_type(e_saved_game_file_type file_type, bool without_dot)
{
    const char *extension;
    switch (file_type) {
    case _saved_game_file_type_bmf: extension = ".bmf"; break;
    case _saved_game_file_type_ctf: extension = ".ctf"; break;
    case _saved_game_file_type_slayer: extension = ".slayer"; break;
    case _saved_game_file_type_oddball: extension = ".oddball"; break;
    case _saved_game_file_type_koth: extension = ".koth"; break;
    case _saved_game_file_type_jugg: extension = ".jugg"; break;
    case _saved_game_file_type_terries: extension = ".terries"; break;
    case _saved_game_file_type_assault: extension = ".assault"; break;
    case _saved_game_file_type_zombiez: extension = ".zombiez"; break;
    case _saved_game_file_type_vip: extension = ".vip"; break;
    case _saved_game_file_type_map: extension = ".map"; break;
    case _saved_game_file_type_film: extension = ".film"; break;
    case _saved_game_file_type_clip: extension = ".clip"; break;
    case _saved_game_file_type_shot: extension = ".shot"; break;
    default: extension = 0; break;
    }
    if (extension && without_dot) extension++;
    return extension;
}

struct s_file_reference;
// @address 0x1e82e8 ?saved_game_read_metadata_from_file@@YA_NPAUs_file_reference@@PAUs_saved_game_item_metadata_packed@@@Z
bool saved_game_read_metadata_from_file(s_file_reference *file, s_saved_game_item_metadata_packed *out_metadata)
{
    bool success = false;
    unsigned long error;
    if (file_open(file, 1, &error)) {
        success = saved_game_read_metadata_from_open_file_internal(file, out_metadata) && saved_game_metadata_is_valid(out_metadata);
        file_close(file);
    }
    return success;
}


// 0x1e8370
// @address 0x1e8370 saved_game_write_metadata_to_file
extern "C" int saved_game_write_metadata_to_file(s_file_reference *reference, const s_saved_game_item_metadata_packed *metadata, int file_type)
{
    int result = 0;
    unsigned long error;
    if (file_open(reference, 3, &error)) {
        result = ((int (*)(s_file_reference *, const s_saved_game_item_metadata_packed *, int, int))saved_game_write_metadata_to_open_file)(
            reference, metadata, file_type, 1);
        file_close(reference);
    }
    return result;
}


// 0x1e83e0
e_game_content_type saved_game_file_type_to_game_content_type(e_saved_game_file_type file_type)
{
    switch (file_type) {
    case _saved_game_file_type_ctf: return _game_content_type_ctf;
    case _saved_game_file_type_slayer: return _game_content_type_slayer;
    case _saved_game_file_type_oddball: return _game_content_type_oddball;
    case _saved_game_file_type_koth: return _game_content_type_koth;
    case _saved_game_file_type_jugg: return _game_content_type_jugg;
    case _saved_game_file_type_terries: return _game_content_type_terries;
    case _saved_game_file_type_assault: return _game_content_type_assault;
    case _saved_game_file_type_zombiez: return _game_content_type_zombiez;
    case _saved_game_file_type_vip: return _game_content_type_vip;
    case _saved_game_file_type_map: return _game_content_type_map;
    case _saved_game_file_type_film: return _game_content_type_film;
    case _saved_game_file_type_clip: return _game_content_type_clip;
    case _saved_game_file_type_shot: return _game_content_type_shot;
    default: return _game_content_type_none;
    }
}

e_saved_game_file_type game_content_type_to_saved_game_file_type(e_game_content_type content_type)
{
    switch (content_type) {
    case _game_content_type_map:
        return _saved_game_file_type_map;
    case _game_content_type_film:
        return _saved_game_file_type_film;
    case _game_content_type_clip:
        return _saved_game_file_type_clip;
    case _game_content_type_shot:
        return _saved_game_file_type_shot;
    case _game_content_type_ctf:
        return _saved_game_file_type_ctf;
    case _game_content_type_slayer:
        return _saved_game_file_type_slayer;
    case _game_content_type_oddball:
        return _saved_game_file_type_oddball;
    case _game_content_type_koth:
        return _saved_game_file_type_koth;
    case _game_content_type_jugg:
        return _saved_game_file_type_jugg;
    case _game_content_type_terries:
        return _saved_game_file_type_terries;
    case _game_content_type_assault:
        return _saved_game_file_type_assault;
    case _game_content_type_vip:
        return _saved_game_file_type_vip;
    case _game_content_type_zombiez:
        return _saved_game_file_type_zombiez;
    default:
        return _saved_game_file_type_none;
    }
}

// 0x1e8578
e_saved_game_file_type game_engine_index_to_saved_game_file_type(e_game_engine_type engine_type)
{
    switch (engine_type) {
    case _game_engine_type_ctf: return _saved_game_file_type_ctf;
    case _game_engine_type_slayer: return _saved_game_file_type_slayer;
    case _game_engine_type_oddball: return _saved_game_file_type_oddball;
    case _game_engine_type_koth: return _saved_game_file_type_koth;
    case _game_engine_type_vip: return _saved_game_file_type_vip;
    case _game_engine_type_jugg: return _saved_game_file_type_jugg;
    case _game_engine_type_terries: return _saved_game_file_type_terries;
    case _game_engine_type_assault: return _saved_game_file_type_assault;
    case _game_engine_type_zombiez: return _saved_game_file_type_zombiez;
    default: return _saved_game_file_type_none;
    }
}

// 0x1e8618

// @address 0x1e8618 game_engine_type_to_game_content_type
e_game_content_type game_engine_type_to_game_content_type(e_game_engine_type engine_type)
{
    switch (engine_type) {
    case _game_engine_type_ctf: return _game_content_type_ctf;
    case _game_engine_type_slayer: return _game_content_type_slayer;
    case _game_engine_type_oddball: return _game_content_type_oddball;
    case _game_engine_type_koth: return _game_content_type_koth;
    case _game_engine_type_vip: return _game_content_type_vip;
    case _game_engine_type_jugg: return _game_content_type_jugg;
    case _game_engine_type_terries: return _game_content_type_terries;
    case _game_engine_type_assault: return _game_content_type_assault;
    case _game_engine_type_zombiez: return _game_content_type_zombiez;
    default: return _game_content_type_none;
    }
}

// 0x1e86b8
// @address 0x1e86b8 game_content_type_to_game_engine_type
e_game_engine_type game_content_type_to_game_engine_type(e_game_content_type content_type)
{
    switch (content_type) {
    case _game_content_type_ctf: return _game_engine_type_ctf;
    case _game_content_type_slayer: return _game_engine_type_slayer;
    case _game_content_type_oddball: return _game_engine_type_oddball;
    case _game_content_type_koth: return _game_engine_type_koth;
    case _game_content_type_vip: return _game_engine_type_vip;
    case _game_content_type_jugg: return _game_engine_type_jugg;
    case _game_content_type_terries: return _game_engine_type_terries;
    case _game_content_type_assault: return _game_engine_type_assault;
    case _game_content_type_zombiez: return _game_engine_type_zombiez;
    default: return _game_engine_type_none;
    }
}

// 0x1e8750
// @address 0x1e8750 saved_game_files_get_file_type_display_name
void saved_game_files_get_file_type_display_name(e_saved_game_file_type file_type, wchar_t *output, long capacity)
{
    output[0] = 0;
    long string_id;
    switch (file_type) {
    case _saved_game_file_type_bmf: string_id = _string_id_dash_metadata_saved_game_campaign; break;
    case _saved_game_file_type_ctf: string_id = _string_id_dash_metadata_saved_game_variant_ctf; break;
    case _saved_game_file_type_slayer: string_id = _string_id_dash_metadata_saved_game_variant_slayer; break;
    case _saved_game_file_type_oddball: string_id = _string_id_dash_metadata_saved_game_variant_oddball; break;
    case _saved_game_file_type_koth: string_id = _string_id_dash_metadata_saved_game_variant_king; break;
    case _saved_game_file_type_jugg: string_id = _string_id_dash_metadata_saved_game_variant_juggernaut; break;
    case _saved_game_file_type_terries: string_id = _string_id_dash_metadata_saved_game_variant_territories; break;
    case _saved_game_file_type_assault: string_id = _string_id_dash_metadata_saved_game_variant_assault; break;
    case _saved_game_file_type_zombiez: string_id = _string_id_dash_metadata_saved_game_variant_infection; break;
    case _saved_game_file_type_vip: string_id = _string_id_dash_metadata_saved_game_variant_vip; break;
    case _saved_game_file_type_map: string_id = _string_id_dash_metadata_saved_game_map_variant; break;
    case _saved_game_file_type_film: string_id = _string_id_dash_metadata_saved_game_film; break;
    case _saved_game_file_type_clip: string_id = _string_id_dash_metadata_saved_game_film_clip; break;
    case _saved_game_file_type_shot: string_id = _string_id_dash_metadata_saved_game_screen_shot; break;
    default: return;
    }
    c_static_wchar_string<1024> name;
    if (user_interface_global_string_get(string_id, (unsigned short *)name.m_string))
        ustrnzcpy(output, name.get_string(), 128);
}


static __forceinline unsigned int saved_game_files_ustrnlen(const wchar_t *string, unsigned int size)
{
    unsigned int length;
    for (length = 0; length < size; length++) {
        if (string[length] == 0)
            break;
    }
    return length;
}


static __forceinline e_saved_game_file_category saved_game_files_file_type_category(e_saved_game_file_type file_type)
{
    e_saved_game_file_category category = _saved_game_file_category_none;
    if (file_type >= _saved_game_file_type_ctf && file_type <= _saved_game_file_type_vip) {
        category = _saved_game_file_category_game_variant;
    } else {
        switch (file_type) {
        case _saved_game_file_type_film:
            category = _saved_game_file_category_film;
            break;
        case _saved_game_file_type_clip:
            category = _saved_game_file_category_clip;
            break;
        case _saved_game_file_type_map:
            category = _saved_game_file_category_map;
            break;
        case _saved_game_file_type_shot:
            category = _saved_game_file_category_screenshot;
            break;
        }
    }
    return category;
}

// @address 0x1e88a8 saved_game_files_get_film_name
bool saved_game_files_get_film_name(long controller_index, unsigned long type, const wchar_t *first_string, const wchar_t *second_string,
    long campaign_difficulty, long unused, wchar_t *output)
{
    s_saved_game_files_context context;
    context.first_string = first_string;
    context.second_string = second_string;
    context.third_string = first_string;
    context.campaign_difficulty = campaign_difficulty;
    short user_index = controller_globals().controllers[controller_index].user_index;
    context.user_index = user_index;
    saved_game_files_set_text_parse_context(&context);

    long string_id;
    switch (type) {
    case 2:
        string_id = _string_id_default_film_save_name_format_custom_game;
        break;
    case 1:
        string_id = _string_id_default_film_save_name_format_matchmaking;
        break;
    case 0:
        string_id = _string_id_default_film_save_name_format_campaign;
        break;
    case 3:
        string_id = _string_id_default_film_save_name_format_editor;
        break;
    default:
        string_id = 0;
        break;
    }

    bool result = user_interface_get_formatted_string(user_index, 'save', string_id, output, 16);
    saved_game_files_reset_text_parse_context();
    return result;
}

// @address 0x1e8980 saved_game_files_get_film_description
bool saved_game_files_get_film_description(long controller_index, unsigned long type, const wchar_t *first_string, const wchar_t *second_string,
    long campaign_difficulty, wchar_t *output, long output_capacity)
{
    s_saved_game_files_context context;
    context.first_string = first_string;
    context.second_string = second_string;
    context.third_string = first_string;
    context.campaign_difficulty = campaign_difficulty;
    short user_index = controller_globals().controllers[controller_index].user_index;
    context.user_index = user_index;
    saved_game_files_set_text_parse_context(&context);

    long string_id;
    switch (type) {
    case 2:
        string_id = _string_id_default_film_save_description_format_custom_game;
        break;
    case 1:
        string_id = _string_id_default_film_save_description_format_matchmaking;
        break;
    case 0:
        string_id = _string_id_default_film_save_description_format_campaign;
        break;
    case 3:
        string_id = _string_id_default_film_save_description_format_editor;
        break;
    default:
        string_id = 0;
        break;
    }

    bool result = user_interface_get_formatted_string(user_index, 'save', string_id, output, 128);
    saved_game_files_reset_text_parse_context();
    return result;
}

// 0x1e8a58


struct c_static_wchar_string_1024;

extern "C" {


}

static __forceinline void saved_game_files_metadata_ustrnzcpy(wchar_t *destination, const wchar_t *source, int count)
{
    for (int i = 0; i <= count - 1; i++, source++) {
        if ((destination[i] = *source) == 0)
            break;
    }
    destination[count - 1] = 0;
}

// @address 0x1e8a58 saved_game_files_initialize_metadata
void saved_game_files_initialize_metadata(s_saved_game_item_metadata_record *out, unsigned int index, int value, const wchar_t *name,
    const wchar_t *description, unsigned __int64 identifier, unsigned __int64 size_in_bytes)
{
    wchar_t buffer[1024];

    memset(out, 0, sizeof(*out));
    s_controller *player = index <= 3 ? &controller_globals().controllers[index] : 0;
    buffer[0] = 0;
    const wchar_t *short_name = (const wchar_t *)L"";
    unsigned __int64 author_id = 0;
    bool author_is_online = false;
    if (identifier == 0) {
        bool ok = false;
        buffer[0] = 0;
        const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
        if (globals) {
            int handle = globals->global_strings.index;
            if (handle != -1)
                ok = text_group_get_string(handle, _string_id_bungie, (c_static_wchar_string<1024> *)buffer);
        }
        if (ok)
            short_name = buffer;
    } else if (player) {
        if (((c_controller_interface *)player)->in_use()) {
            if (((c_controller_interface *)player)->is_signed_in_to_live())
                short_name = player->display_name;
            unsigned __int64 xuid = 0;
            short slot = player->user_index;
            if (slot >= 0 && slot < 4)
                xuid = g_network_session_interface_local_users[slot].xuid;
            author_id = xuid;
            author_is_online = ((c_controller_interface *)player)->is_signed_in_to_live();
        }
    }

    out->unique_id = identifier;
    saved_game_files_metadata_ustrnzcpy(out->name, name, 16);
    wchar_string_to_utf8_string(description, out->description, 128, 0);
    {
        const wchar_t *s = short_name;
        char *d = out->author;
        int n = 16;
        wchar_t c;
        do {
            c = *s;
            if (n == 1)
                *d = 0;
            else if (c <= 127)
                *d = (char)c;
            else
                *d = 63;
            s++;
            d++;
            n--;
        } while (c != 0 && n > 0);
    }
    out->file_type = value;
    out->author_is_online = author_is_online;
    out->author_id = author_id;
    out->size_in_bytes = size_in_bytes;
    out->date = _time64(0);
    out->length_seconds = 0;
    out->game_engine_type = 0;
    out->campaign_survival_enabled = 0;
    out->game_id = 0;
    out->campaign_id = -1;
    out->map_id = -1;
    out->campaign_difficulty = -1;
    out->campaign_insertion_point = static_cast<unsigned short>(NONE);
    if (s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)value) == _saved_game_file_category_game_variant)
        out->game_engine_type = game_content_type_to_game_engine_type(saved_game_file_type_to_game_content_type((e_saved_game_file_type)value));
}


// @address 0x1e8ce0 saved_game_files_initialize_metadata_from_game
extern "C" void saved_game_files_initialize_metadata_from_game(s_saved_game_item_metadata_record *out, unsigned int controller_index,
    int file_type, const wchar_t *name, const wchar_t *description, unsigned __int64 size_in_bytes)
{
    game_globals_storage *globals = tls_game_globals();
    unsigned __int64 identifier;
    XNetRandom((unsigned char *)&identifier, sizeof(identifier));
    saved_game_files_initialize_metadata(out, controller_index, file_type, name, description, identifier, size_in_bytes);
    out->campaign_id = globals->options.campaign_id;
    out->map_id = globals->options.map_id;
    out->game_engine_type = globals->options.game_variant.engine_type;
    out->game_id = globals->options.game_instance;
    out->campaign_difficulty = globals->options.campaign_difficulty;
    if (globals->options.matchmade_game != 0) {
        out->campaign_insertion_point = globals->options.matchmaking_options.hopper_identifier;
    }
}

// @address 0x1e8d88 saved_game_metadata_is_valid
bool saved_game_metadata_is_valid(const s_saved_game_item_metadata_packed *metadata)
{
    const char zero[sizeof(metadata->author_flag_zero_fill)] = { 0 };
    bool valid = true;
    for (unsigned int index = saved_game_files_ustrnlen(metadata->name, 16); index < 16; index++)
        valid = valid && metadata->name[index] == 0;
    for (unsigned int index = csstrnlen(metadata->description, 128); index < 128; index++)
        valid = valid && metadata->description[index] == 0;
    for (unsigned int index = csstrnlen(metadata->author, 16); index < 16; index++)
        valid = valid && metadata->author[index] == 0;
    valid = valid && memcmp(metadata->author_flag_zero_fill, zero, sizeof(zero)) == 0;
    valid = valid && metadata->campaign_survival_enabled == 0;
    valid = valid && metadata->game_engine_type >= 0 && metadata->game_engine_type < 11;
    valid = valid && metadata->file_type >= 0 && metadata->file_type < 14;
    valid = valid && metadata->campaign_difficulty >= 0 && metadata->campaign_difficulty < 4 || metadata->campaign_difficulty == -1;
    switch (saved_game_files_file_type_category((e_saved_game_file_type)metadata->file_type)) {
    case _saved_game_file_category_film:
    case _saved_game_file_category_clip:
        if (metadata->campaign_id != -1) {
            valid = valid && metadata->game_engine_type == 0;
            valid = valid && metadata->campaign_difficulty != -1;
        }
        break;
    case _saved_game_file_category_map:
        valid = valid && metadata->campaign_difficulty == -1;
        valid = valid && metadata->campaign_insertion_point == static_cast<unsigned short>(NONE);
        break;
    case _saved_game_file_category_screenshot:
        if (metadata->campaign_id != -1) {
            valid = valid && metadata->game_engine_type == 0;
            valid = valid && metadata->campaign_difficulty != -1;
        }
        break;
    case _saved_game_file_category_game_variant:
        valid = valid && metadata->game_engine_type != 0;
        valid = valid && metadata->campaign_difficulty == -1;
        valid = valid && metadata->campaign_insertion_point == static_cast<unsigned short>(NONE);
        break;
    }
    return valid;
}

// @address 0x1e9240 ?get_file_type_from_saved_game_type@s_saved_game_item_metadata@@SA?AW4e_metadata_file_type@@W4e_saved_game_file_type@@@Z
e_metadata_file_type s_saved_game_item_metadata::get_file_type_from_saved_game_type(e_saved_game_file_type file_type)
{
    return (e_metadata_file_type)saved_game_files_file_type_category(file_type);
}

// 0x1e9350
// @address 0x1e9350 saved_game_files_try_and_get_metadata_property
bool saved_game_files_try_and_get_metadata_property(const s_saved_game_item_metadata *metadata, long property, long *out_value)
{
    bool result = false;
    *out_value = -1;

    switch (property) {
    case _string_id_game_engine_index:
        if ((metadata->file_type >= _saved_game_file_type_ctf && metadata->file_type <= _saved_game_file_type_vip) ||
            ((metadata->file_type == _saved_game_file_type_film || metadata->file_type == _saved_game_file_type_clip) && metadata->campaign_id == -1))
        {
            *out_value = metadata->game_engine_type;
        }
        result = true;
        break;

    case _string_id_difficulty:
        if ((metadata->file_type == _saved_game_file_type_film || metadata->file_type == _saved_game_file_type_clip) && metadata->campaign_id != -1) {
            *out_value = metadata->campaign_difficulty;
        }
        result = true;
        break;

    case _string_id_map_id:
        if (metadata->file_type == _saved_game_file_type_map || metadata->file_type == _saved_game_file_type_film ||
            metadata->file_type == _saved_game_file_type_clip)
        {
            *out_value = metadata->map_id;
        }
        result = true;
        break;

    case _string_id_type: {
        e_metadata_file_type category = s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)metadata->file_type);
        if (category != _saved_game_file_category_none) {
            *out_value = category;
            result = true;
        }
        break;
    }
    }

    return result;
}

// 0x1e94a8
// @address 0x1e94a8 saved_game_files_compute_film_type
long saved_game_files_compute_film_type(const s_saved_game_item_metadata_packed *metadata)
{
    long result = -1;
    if (metadata->campaign_id != -1) {
        if (metadata->campaign_difficulty >= 0 && metadata->campaign_difficulty < 4) {
            return 0;
        }
    }
    if (metadata->game_engine_type >= _game_engine_type_ctf && metadata->game_engine_type <= _game_engine_type_zombiez) {
        result = 2;
        if (metadata->campaign_insertion_point != static_cast<unsigned short>(NONE)) {
            return 1;
        }
        if (metadata->game_engine_type == _game_engine_type_sandbox) {
            return 3;
        }
    }
    return result;
}

// ---- saved game files: bitstream reader for the packed metadata (c_bitstream as read here) ----
class c_bitstream;


// 0x1e9518
// @address 0x1e9518 saved_game_files_encode_metadata
void saved_game_files_encode_metadata(const s_saved_game_item_metadata_packed *metadata, c_bitstream *stream)
{
    stream->write_value_internal(metadata->unique_id, 64);
    stream->write_string_wchar("name", (const unsigned short *)metadata->name, 16);
    {
        unsigned long length = csstrnlen(metadata->description, 128);
        stream->m_encoded_value_bits += ((length ? 128 - 1 : 0) + 1) * 8;
        for (long index = 0; index < 128; index++) {
            unsigned char character = (unsigned char)metadata->description[index];
            stream->write_integer("description", character, 8);
            if (!character)
                break;
        }
    }
    stream->write_string("author", metadata->author, 16);
    stream->write_integer("file_type", metadata->file_type + 1, 5);
    stream->write_bool("author_is_online", metadata->author_is_online);
    stream->write_value_internal(metadata->author_id, 64);
    stream->write_value_internal(metadata->size_in_bytes, 64);
    stream->write_value_internal(metadata->date, 64);
    stream->write_integer("length_seconds", metadata->length_seconds, 32);
    stream->write_integer("campaign_id", metadata->campaign_id, 32);
    stream->write_integer("map_id", metadata->map_id, 32);
    stream->write_integer("game_engine_type", metadata->game_engine_type, 4);
    stream->write_integer("campaign_difficulty", metadata->campaign_difficulty + 1, 3);
    stream->write_integer("campaign_insertion_point", metadata->campaign_insertion_point, 16);
    stream->write_value_internal(metadata->game_id, 64);
}

// 0x1e97e0
// @address 0x1e97e0 saved_game_files_decode_metadata
void saved_game_files_decode_metadata(s_saved_game_item_metadata_packed *metadata, c_bitstream *stream)
{
    metadata->unique_id = bitstream_read_window_bits(stream, 64);
    stream->read_string_wchar("name", (unsigned short *)metadata->name, 16);
    stream->read_string("description", metadata->description, 128);
    stream->read_string("author", metadata->author, 16);
    metadata->file_type = stream->read_integer("file_type", 5) - 1;
    metadata->author_is_online = stream->read_bool("author_is_online");
    metadata->author_id = bitstream_read_window_bits(stream, 64);
    metadata->size_in_bytes = bitstream_read_window_bits(stream, 64);
    metadata->date = bitstream_read_window_bits(stream, 64);
    metadata->length_seconds = stream->read_integer("length_seconds", 32);
    metadata->campaign_id = stream->read_integer("campaign_id", 32);
    metadata->map_id = stream->read_integer("map_id", 32);
    metadata->game_engine_type = (e_game_engine_type)stream->read_integer("game_engine_type", 4);
    metadata->campaign_difficulty = stream->read_integer("campaign_difficulty", 3) - 1;
    metadata->campaign_insertion_point = static_cast<unsigned short>(stream->read_integer("campaign_insertion_point", 16));
    metadata->game_id = bitstream_read_window_bits(stream, 64);
}

// @address 0x1e9c20 saved_game_files_get_metadata_string
extern "C" bool saved_game_files_get_metadata_string(const s_gui_selected_items_block *metadata, long string_id, c_static_wchar_string<1024> *string)
{
    return saved_game_files_try_and_get_metadata_string(metadata->unique_id, metadata->name, metadata->description, metadata->author,
        metadata->file_type, metadata->author_is_online, metadata->author_id, metadata->size_in_bytes,
        metadata->date, metadata->length_seconds, metadata->campaign_id, metadata->map_id,
        metadata->game_engine_type, metadata->campaign_difficulty, metadata->campaign_insertion_point,
        metadata->game_id, string_id, string);
}

// @address 0x1e9ca8 ?saved_game_files_get_saved_game_file_name@@YAPBDW4e_saved_game_file_type@@@Z
char const * saved_game_files_get_saved_game_file_name(enum e_saved_game_file_type index)
{
    const char *table[14] = {
        "mmiof.bmf", "variant.ctf", "variant.slayer", "variant.oddball",
        "variant.koth", "variant.jugg", "variant.terries", "variant.assault",
        "variant.zombiez", "variant.vip", "sandbox.map", "feature.film",
        "snippet.clip", "screen.shot"
    };
    if ((unsigned long)index <= 13)
        return table[(unsigned long)index];
    return "<unknown>";
}


// @address 0x1e9d80 ?saved_game_files_get_personal_storage_content_item@@YAJW4e_controller_index@@_N@Z
long saved_game_files_get_personal_storage_content_item(e_controller_index controller_index, bool create)
{
    c_controller_interface *controller = (c_controller_interface *)&controller_globals().controllers[controller_index];
    long result = -1;

    if (controller->is_signed_in_to_machine()) {
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
        const wchar_t *display_name = controller->get_display_name();
        result = catalogue->try_and_get_item_by_type_and_display_name(display_name);
        if (result == -1 && create)
            result = content_catalogue_new_item(catalogue, display_name, 0, 2, 1, 0xa00000, true);
    }
    return result;
}

// 0x1ea1c0
// @address 0x1ea1c0 saved_game_files_set_async_result
long saved_game_files_set_async_result(unsigned char success, long unused, volatile long *result, volatile long *complete)
{
    if (success == 0) {
        __lwsync();
        _InterlockedExchange(result, 0);
    } else {
        __lwsync();
        _InterlockedExchange(result, success);
    }
    __lwsync();
    _InterlockedExchange(complete, 1);
    return 1;
}


struct s_file_reference;

// @address 0x1ea8e8 ?saved_game_read_metadata_from_open_file_internal@@YA_NPAUs_file_reference@@PAUs_saved_game_item_metadata_packed@@@Z

bool saved_game_read_metadata_from_open_file_internal(s_file_reference *reference, s_saved_game_item_metadata_packed *metadata)
{
    bool result = false;
    char buffer[0x240];
    if (file_set_position(reference, 0, false) && file_read(reference, sizeof(buffer), false, buffer)) {
        const char *current = buffer;
        long chunk_size = 0;
        bool byte_swap;
        if (network_blf_verify_start_of_file(current, sizeof(buffer) - (current - buffer), &byte_swap, &chunk_size)) {
            const char *chunk_buffer = 0;
            current += chunk_size;
            if (network_blf_read_for_known_chunk(
                    current, sizeof(buffer) - (current - buffer), byte_swap, 'chdr', 9, &chunk_size, &chunk_buffer, 0, 0, 0) &&
                chunk_buffer && !byte_swap)
            {
                const s_saved_game_content_header *content_header = (const s_saved_game_content_header *)chunk_buffer;
                *metadata = content_header->metadata;
                result = saved_game_metadata_is_valid(metadata);
            }
        }
    }
    return result;
}


// @address 0x1ea9f0 saved_game_write_metadata_to_open_file
extern "C" int saved_game_write_metadata_to_open_file(s_file_reference *reference, const s_saved_game_item_metadata_packed *metadata, int file_type)
{
    int result = 0;
    char buffer[0x240];
    if (saved_game_metadata_is_valid(metadata) && file_set_position(reference, 0, false) && file_read(reference, sizeof(buffer), false, buffer)) {
        const char *current = buffer;
        long chunk_size = 0;
        bool byte_swap;
        if (network_blf_verify_start_of_file(current, sizeof(buffer) - (current - buffer), &byte_swap, &chunk_size)) {
            const char *chunk_buffer = 0;
            current += chunk_size;
            if (network_blf_read_for_known_chunk(
                    current, sizeof(buffer) - (current - buffer), byte_swap, 'chdr', 9, &chunk_size, &chunk_buffer, 0, 0, 0) &&
                chunk_buffer && !byte_swap)
            {
                s_saved_game_content_header *content_header = (s_saved_game_content_header *)chunk_buffer;
                content_header->metadata = *metadata;
                if (file_set_position(reference, 0, false) && file_write(reference, sizeof(buffer), buffer)) {
                    result = saved_game_files_update_variant_metadata(reference, metadata, file_type);
                }
            }
        }
    }
    return result;
}

// 0x1eada8
// @address 0x1eada8 saved_game_files_set_text_parse_context
void saved_game_files_set_text_parse_context(const s_saved_game_files_context *context)
{
    g_saved_game_files_globals.context = *context;
    g_parse_manager.register_parser(&g_saved_game_files_globals.parsers[0], 'save');
    g_parse_manager.register_parser(&g_saved_game_files_globals.parsers[1], 'save');
    g_parse_manager.register_parser(&g_saved_game_files_globals.parsers[2], 'save');
    g_parse_manager.register_parser(&g_saved_game_files_globals.parsers[3], 'save');
}

// 0x1eae38
// @address 0x1eae38 saved_game_files_reset_text_parse_context
void saved_game_files_reset_text_parse_context(void)
{
    user_interface_text_parser_unregister(&g_saved_game_files_globals.parsers[0], 'save');
    user_interface_text_parser_unregister(&g_saved_game_files_globals.parsers[1], 'save');
    user_interface_text_parser_unregister(&g_saved_game_files_globals.parsers[2], 'save');
    user_interface_text_parser_unregister(&g_saved_game_files_globals.parsers[3], 'save');
    g_saved_game_files_globals.context.user_index = 0;
    g_saved_game_files_globals.context.first_string = 0;
    g_saved_game_files_globals.context.second_string = 0;
    g_saved_game_files_globals.context.third_string = 0;
    g_saved_game_files_globals.context.campaign_difficulty = 0;
}
// 0x1eaea0
// @address 0x1eaea0 saved_game_files_parse_first_string
bool saved_game_files_parse_first_string(void *context, wchar_t *output, long capacity)
{
    const s_saved_game_files_context *files_context = static_cast<const s_saved_game_files_context *>(context);
    if (files_context->first_string) {
        ustrnzcpy(output, files_context->first_string, capacity);
        return true;
    }
    output[0] = 0;
    return true;
}

// 0x1eaef8
// @address 0x1eaef8 saved_game_files_parse_second_string
bool saved_game_files_parse_second_string(void *context, wchar_t *output, long capacity)
{
    const s_saved_game_files_context *files_context = static_cast<const s_saved_game_files_context *>(context);
    if (files_context->second_string) {
        ustrnzcpy(output, files_context->second_string, capacity);
        return true;
    }
    output[0] = 0;
    return true;
}

// 0x1eaf50
// @address 0x1eaf50 saved_game_files_parse_third_string
bool saved_game_files_parse_third_string(void *context, wchar_t *output, long capacity)
{
    const s_saved_game_files_context *files_context = static_cast<const s_saved_game_files_context *>(context);
    if (files_context->third_string) {
        ustrnzcpy(output, files_context->third_string, capacity);
        return true;
    }
    output[0] = 0;
    return true;
}

// 0x1eafa8
// @address 0x1eafa8 saved_game_files_parse_difficulty_string
bool saved_game_files_parse_difficulty_string(void *context, wchar_t *output, long capacity)
{
    const s_saved_game_files_context *files_context = static_cast<const s_saved_game_files_context *>(context);
    long string_id;
    switch (files_context->campaign_difficulty) {
    case 0:
        string_id = _string_id_easy;
        break;
    case 1:
        string_id = _string_id_normal;
        break;
    case 2:
        string_id = _string_id_heroic;
        break;
    case 3:
        string_id = _string_id_legendary;
        break;
    default:
        string_id = 0;
        break;
    }
    return user_interface_get_formatted_string(files_context->user_index, 'save', string_id, output, capacity);
}

// 0x1eb290

// @address 0x1eb290 saved_game_files_has_unique_id
bool saved_game_files_has_unique_id(long controller_index, unsigned __int64 unique_id)
{
    bool result = false;
    s_saved_game_item_metadata_packed metadata;
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
    if (content_catalogue_has_storage_device(catalogue)) {
        long index = content_catalogue_get_first_item_index(catalogue);
        while (index != -1) {
            if (content_catalogue_item_metadata_matches(catalogue, index, &metadata) && metadata.unique_id == unique_id) {
                result = true;
                break;
            }
            index = content_catalogue_get_next_item_index(catalogue, index);
        }
    }
    return result;
}

// 0x1eb338
extern "C" {


}
static __forceinline bool saved_game_files_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    string->m_string[0] = 0;
    if (string_id != -1) {
        const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
        if (globals && globals->global_strings.index != -1)
            result = text_group_get_string(globals->global_strings.index, string_id, string);
    }
    return result;
}

// @address 0x1eb338 saved_game_files_try_and_get_metadata_string
bool saved_game_files_try_and_get_metadata_string(unsigned __int64 unique_id, const wchar_t *name, const wchar_t *description, const char *author,
    long file_type, bool author_is_online, unsigned __int64 author_id, unsigned __int64 size_in_bytes,
    unsigned __int64 date, long length_seconds, long campaign_id, long map_id, long game_engine_type,
    long campaign_difficulty, short campaign_insertion_point, unsigned __int64 game_id,
    long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    switch (string_id) {
    case _string_id_name:
    case _string_id_title:
        string->set(name);
        result = true;
        break;
    case _string_id_description:
        string->set(description);
        result = true;
        break;
    }
    if (!result) {
        if (string_id == _string_id_metadata_line_1_label) {
            result = saved_game_files_global_string_get(0x1020f, string);
        } else if (string_id == _string_id_metadata_line_1) {
            string->set_char(author);
        } else if (size_in_bytes == 0) {
            string->set(L" ");
            result = true;
        } else {
            switch (string_id) {
            case _string_id_size:
                result = user_interface_get_bytes_size_string((long)size_in_bytes, string);
                break;
            case _string_id_metadata_line_1_label:
            case _string_id_metadata_line_2_label:
            case _string_id_metadata_line_3_label:
            case _string_id_metadata_line_4_label: {
                long global_string_id = -1;
                if (file_type >= 1 && file_type <= 9) {
                    if (string_id == _string_id_metadata_line_2_label)
                        global_string_id = _string_id_metadata_date;
                    else if (string_id == _string_id_metadata_line_3_label)
                        global_string_id = _string_id_metadata_size;
                } else if (file_type == 10) {
                    if (string_id == _string_id_metadata_line_2_label)
                        global_string_id = _string_id_metadata_date;
                    else if (string_id == _string_id_metadata_line_3_label)
                        global_string_id = _string_id_metadata_size;
                } else if (file_type == 11) {
                    if (string_id == _string_id_metadata_line_2_label)
                        global_string_id = _string_id_metadata_date;
                    else if (string_id == _string_id_metadata_line_3_label)
                        global_string_id = _string_id_metadata_length;
                    else if (string_id == _string_id_metadata_line_4_label)
                        global_string_id = _string_id_metadata_size;
                } else if (file_type == 12) {
                    if (string_id == _string_id_metadata_line_2_label)
                        global_string_id = _string_id_metadata_date;
                    else if (string_id == _string_id_metadata_line_3_label)
                        global_string_id = _string_id_metadata_length;
                    else if (string_id == _string_id_metadata_line_4_label)
                        global_string_id = _string_id_metadata_size;
                } else if (file_type == 13) {
                    if (string_id == _string_id_metadata_line_2_label)
                        global_string_id = _string_id_metadata_date;
                    else if (string_id == _string_id_metadata_line_3_label)
                        global_string_id = _string_id_metadata_size;
                }
                if (global_string_id != -1)
                    result = user_interface_global_string_get(global_string_id, (unsigned short *)string->m_string);
                break;
            }
            case _string_id_metadata_line_1:
            case _string_id_metadata_line_2:
            case _string_id_metadata_line_3:
            case _string_id_metadata_line_4:
                string->m_string[0] = 0;
                if (file_type >= 1 && file_type <= 9) {
                    if (string_id == _string_id_metadata_line_2)
                        result = user_interface_get_date_string(date, string);
                    else if (string_id == _string_id_metadata_line_3)
                        result = user_interface_get_bytes_size_string((long)size_in_bytes, string);
                } else if (file_type == 10) {
                    if (string_id == _string_id_metadata_line_2)
                        result = user_interface_get_date_string(date, string);
                    else if (string_id == _string_id_metadata_line_3)
                        result = user_interface_get_bytes_size_string((long)size_in_bytes, string);
                } else if (file_type == 11) {
                    if (string_id == _string_id_metadata_line_2)
                        result = user_interface_get_date_string(date, string);
                    else if (string_id == _string_id_metadata_line_3)
                        result = user_interface_get_timespan_string(length_seconds, 0, false, string);
                    else if (string_id == _string_id_metadata_line_4)
                        result = user_interface_get_bytes_size_string((long)size_in_bytes, string);
                } else if (file_type == 12) {
                    if (string_id == _string_id_metadata_line_2)
                        result = user_interface_get_date_string(date, string);
                    else if (string_id == _string_id_metadata_line_3)
                        result = user_interface_get_timespan_string(length_seconds, 0, false, string);
                    else if (string_id == _string_id_metadata_line_4)
                        result = user_interface_get_bytes_size_string((long)size_in_bytes, string);
                } else if (file_type == 13) {
                    if (string_id == _string_id_metadata_line_2)
                        result = user_interface_get_date_string(date, string);
                    else if (string_id == _string_id_metadata_line_3)
                        result = user_interface_get_bytes_size_string((long)size_in_bytes, string);
                }
                if (string->length() > 0)
                    result = true;
                break;
            }
        }
    }
    return result;
}


// @address 0x1eba98 ?set_char@?$c_static_wchar_string@$0EAA@@@QAAXPBD@Z
template<>
void c_static_wchar_string<1024>::set_char(const char *string)
{
    ascii_string_to_wchar_string(string, m_string, 1024, 0);
}

// @address 0x1ebb00 ?length@?$c_static_wchar_string@$0EAA@@@QBAJXZ
// Length of a wide string, capped at 1024 characters.
template<long k_maximum_count>
long c_static_wchar_string<k_maximum_count>::length() const
{
  unsigned int length;
  for (length = 0; length < k_maximum_count; length++) {
    if (m_string[length] == 0) {
      break;
    }
  }
  return length;
}
template long c_static_wchar_string<1024>::length() const;

// @address 0x205db8 saved_game_write_metadata_to_open_file_internal
extern "C" bool saved_game_write_metadata_to_open_file_internal(s_buffered_file_writer *writer)
{
    bool success = true;
    unsigned long read_size = 0;
    unsigned long position = writer->file->position;
    if (position != file_get_eof(writer->file))
    {
        position = writer->file->position;
        unsigned long eof = file_get_eof(writer->file);
        if (eof > writer->buffer_size)
            read_size = writer->buffer_size;
        else
            read_size = file_get_eof(writer->file);
        if (!file_read(writer->file, read_size, true, writer->buffer) || !file_set_position(writer->file, position, true))
            success = false;
    }
    memset(writer->buffer, 0, writer->buffer_size - read_size);
    return success;
}
// @address 0x1a80d8 buffered_file_write
extern "C" bool buffered_file_write(s_buffered_file_writer *writer, const void *data, long size)
{
    const char *source = (const char *)data;
    bool success = true;
    while (success && size > 0) {
        long write_size = writer->buffer_size - writer->buffer_position;
        if (size <= write_size)
            write_size = size;
        if (writer->buffer_position == 0 && !saved_game_write_metadata_to_open_file_internal(writer)) {
            success = false;
            break;
        }
        memcpy(writer->buffer + writer->buffer_position, source, write_size);
        writer->buffer_position += write_size;
        if (writer->buffer_position == writer->buffer_size) {
            success = file_write(writer->file, writer->buffer_size, writer->buffer);
            writer->buffer_position = 0;
        }
        size -= write_size;
        source += write_size;
    }
    return success;
}

static __forceinline wchar_t *saved_game_files_ustrncpy(wchar_t *dest, const wchar_t *source, long count)
{
    for (long index = 0; index <= count; ++index) {
        if (!(dest[index] = *source))
            break;
        ++source;
    }
    return dest;
}
// @address 0x1ceba0 map_variant_set_metadata
extern "C" void map_variant_set_metadata(c_map_variant *variant, const s_saved_game_item_metadata_packed *metadata)
{
    variant->m_metadata = *(const s_saved_game_item_metadata *)metadata;
    wchar_t name[16];
    ustrnzcpy_inline(name, variant->m_metadata.name, 16);
    memset(variant->m_metadata.name, 0, sizeof(variant->m_metadata.name));
    ustrnzcpy_inline(variant->m_metadata.name, name, 16);
    char description[128];
    csstrnzcpy(description, variant->m_metadata.description, 128);
    memset(variant->m_metadata.description, 0, sizeof(variant->m_metadata.description));
    csstrnzcpy(variant->m_metadata.description, description, 128);
    char author[16];
    csstrnzcpy(author, variant->m_metadata.author, 16);
    memset(variant->m_metadata.author, 0, sizeof(variant->m_metadata.author));
    csstrnzcpy(variant->m_metadata.author, author, 16);
    variant->m_metadata.unique_id = 0;
    variant->m_built_in = false;
}

__forceinline bool c_file_reference::find_or_create(unsigned long flags)
    {
        bool result = false;
        if (file_exists(this)) {
            result = true;
        } else if (flags & 2) {
            file_create_parent_directories_if_not_present(this);
            result = ::file_create(this);
        }
        return result;
    }
// @address 0x228138 ??0c_file_reference@@QAA@PBDK@Z
c_file_reference::c_file_reference(const char *path, unsigned long flags)
{
    // match: 0x228138: value range assumption guides branch lowering
    __assume(flags == 2);
    file_reference_create_from_path(this, path, false);
    unsigned long error;
    m_open = find_or_create(flags) && file_open(this, flags, &error);
}


// @address 0x94a58 ?append_print@?$c_static_string@$0BAA@@@QAAPADPBDZZ


// @address 0x228908 saved_game_files_get_autosave_path
extern "C" void saved_game_files_get_autosave_path(e_saved_game_file_type file_type, c_static_string<256> *path)
{
    strncpy(path->m_string, k_saved_game_autosave_directory, 256);
    path->m_string[255] = 0;
    path->append("\\");
    __int64 timestamp = _time64(0);
    const char *extension = saved_game_filename_extension_by_game_file_type(file_type, false);
    path->append_print("asq%016lX%s", timestamp, extension);
}

__forceinline s_blf_chunk_game_variant::s_blf_chunk_game_variant()
{
    header.setup('mpvr', sizeof(*this), 3, 1);
}
// @address 0x228e68 ??0s_blffile_game_variant@@QAA@XZ
s_blffile_game_variant::s_blffile_game_variant()
{
}

s_blf_chunk_map_variant::s_blf_chunk_map_variant()
{
    header.setup('mapv', sizeof(*this), 12, 1);
}
// @address 0x228ba0 ??0s_blffile_map_variant@@QAA@XZ
s_blffile_map_variant::s_blffile_map_variant()
{
}


static __forceinline void saved_game_files_apply_variant_metadata(
    c_game_engine_base_variant *variant, const s_saved_game_item_metadata_packed *metadata)
{
    variant->set_built_in(false);
    game_engine_variant_set_metadata((s_game_engine_base_variant_metadata_view *)variant, metadata);
}

static __forceinline int saved_game_files_file_type_is_special(int x)
{
    if ((x <= 2)) return 1;
    if ((x > 11)) return 1;
    return 0;
}


// @address 0x1eab50 saved_game_files_update_variant_metadata
extern "C" int saved_game_files_update_variant_metadata(s_file_reference *file, const s_saved_game_item_metadata_packed *metadata, int file_type)
{
    void *buffer;
    int size = 0;
    int result = size;
    unsigned long file_size;
    if (file_type != 1) {
        if (!saved_game_files_file_type_is_special(file_type)) {
            if (file_set_position(file, 0, false)) {
                buffer = ::overlapped_malloc_tracked(0x3bc, (char const *)(0), 0);
                if (buffer) {
                    file_size = 0;
                    memset((void *)buffer, 0, sizeof(s_blffile_game_variant));
                    if (file_get_size(file, &file_size)) {
                        size = (int)file_size;
                        if ((unsigned long)size >= 0x3bc) size = 0x3bc;
                        if (file_read(file, size, false, (void *)buffer)
                            && network_blf_find_chunk_in_file((const char *)buffer, size, 'mpvr', 3, _blf_file_authentication_none, 0)) {
                            (&((s_blffile_game_variant *)buffer)->game_variant.game_variant)->recreate_variant_vtable_for_game_engine_index(
                                (e_game_engine_type)metadata->game_engine_type);
                            saved_game_files_apply_variant_metadata(
                                (c_game_engine_base_variant *)
                                    ((s_blffile_game_variant *)buffer)->game_variant.game_variant.m_variant_storage, metadata);
                            if (file_set_position(file, 0, false) && file_write(file, 0x3bc, (void *)buffer))
                                result = 1;
                        }
                    }
                    overlapped_free(buffer);
                }
            }
            return result;
        }
        return 1;
    }
    if (file_set_position(file, 0, false)) {
        buffer = ::overlapped_malloc_tracked(0xe1f0, (char const *)(0), 0);
        if (buffer) {
            file_size = 0;
            memset((void *)buffer, 0, sizeof(s_blffile_map_variant));
            if (file_get_size(file, &file_size)) {
                size = (int)file_size;
                if ((unsigned long)size >= 0xe1f0) size = 0xe1f0;
                if (file_read(file, size, false, (void *)buffer)
                    && network_blf_find_chunk_in_file((const char *)buffer, size, 'mapv', 12, _blf_file_authentication_none, 0)) {
                    map_variant_set_metadata(&((s_blffile_map_variant *)buffer)->map_variant.map_variant, metadata);
                    if (file_set_position(file, 0, false) && file_write(file, 0xe1f0, (void *)buffer))
                        result = 1;
                }
            }
            overlapped_free(buffer);
        }
    }
    return result;
}

// @address 0x265a38 game_engine_variant_set_description
extern "C" void game_engine_variant_set_description(s_game_engine_base_variant_metadata_view *variant, const char *description)
{
    memset(((c_game_engine_base_variant *)variant)->m_metadata.description, 0,
        sizeof(((c_game_engine_base_variant *)variant)->m_metadata.description));
    csstrnzcpy(((c_game_engine_base_variant *)variant)->m_metadata.description, description,
        sizeof(((c_game_engine_base_variant *)variant)->m_metadata.description));
}

// ---- content catalogue: saved game file paths ----

struct s_synchronization_globals;


class c_content_catalogue;
#ifdef HALO3_64BIT
static void saved_game_files_get_slot_file_names(
    long player_count,
    const char **out_header_name,
    const char **out_state_name,
    const char **out_tmp_header_name,
    const char **out_tmp_state_name)
{
    const char *header_name = "gamestate.hdr";
    const char *state_name = "mmiof.bmf";
    const char *tmp_header_name = "gamestate.tmp";
    const char *tmp_state_name = "mmiof.tmp";
    switch (player_count) {
    case 2:
        header_name = "gamestate_2p.hdr";
        state_name = "mmiof_2p.bmf";
        tmp_header_name = "gamestate_2p.tmp";
        tmp_state_name = "mmiof_2p.tmp";
        break;
    case 3:
        header_name = "gamestate_3p.hdr";
        state_name = "mmiof_3p.bmf";
        tmp_header_name = "gamestate_3p.tmp";
        tmp_state_name = "mmiof_3p.tmp";
        break;
    case 4:
        header_name = "gamestate_4p.hdr";
        state_name = "mmiof_4p.bmf";
        tmp_header_name = "gamestate_4p.tmp";
        tmp_state_name = "mmiof_4p.tmp";
        break;
    default:
        break;
    }
    if (out_header_name)
        *out_header_name = header_name;
    if (out_state_name)
        *out_state_name = state_name;
    if (out_tmp_header_name)
        *out_tmp_header_name = tmp_header_name;
    if (out_tmp_state_name)
        *out_tmp_state_name = tmp_state_name;
}

static long saved_game_files_get_active_player_count(void)
{
    if (game_in_progress() && tls_game_globals() && tls_game_globals()->options.game_mode == 1) {
        long active_players = 0;
        for (long player_index = 0; player_index < 16; ++player_index) {
            if (tls_game_globals()->options.players[player_index].valid)
                ++active_players;
        }
        if (active_players >= 1 && active_players <= 4)
            return active_players;
    }
    if (user_interface_squad_exists()) {
        long squad_players = user_interface_squad_get_player_count();
        if (squad_players >= 1 && squad_players <= 4)
            return squad_players;
    }
    long signed_in_players = 0;
    for (long controller_index = 0; controller_index < 4; ++controller_index) {
        if (((c_controller_interface *)&controller_globals().controllers[controller_index])->in_use())
            ++signed_in_players;
    }
    if (signed_in_players >= 1 && signed_in_players <= 4)
        return signed_in_players;
    return 1;
}

static bool saved_game_files_container_has_any_save(e_controller_index controller_index)
{
    if ((unsigned long)controller_index >= 4)
        return false;
    s_controller *controller = &controller_globals().controllers[controller_index];
    for (long slot = 1; slot <= 4; ++slot) {
        const char *header_name = 0;
        saved_game_files_get_slot_file_names(slot, &header_name, 0, 0, 0);
        s_file_reference file;
        if (::saved_game_files_create_saved_game_file_reference(
                controller_index, (wchar_t const *)controller->display_name, header_name, &file, false, false) &&
            file_exists(&file))
        {
            return true;
        }
    }
    return false;
}

static void saved_game_files_delete_slot_files_in_ready_container(
    e_controller_index controller_index,
    s_controller *controller,
    c_content_catalogue *catalogue,
    long content_item,
    long player_count)
{
    const char *names[4] = { 0, 0, 0, 0 };
    saved_game_files_get_slot_file_names(player_count, &names[0], &names[1], &names[2], &names[3]);
    for (long i = 0; i < 4; ++i) {
        s_file_reference file;
        if (names[i] &&
            ::saved_game_files_create_saved_game_file_reference(
                controller_index, (wchar_t const *)controller->display_name, names[i], &file, false, false) &&
            file_exists(&file))
        {
            file_delete(&file);
        }
    }
    if (!saved_game_files_container_has_any_save(controller_index)) {
        saved_game_files_delete_content_item(controller_index, content_item);
    } else {
        content_catalogue_flush_item(catalogue, content_item, true);
        ++g_saved_game_personal_storage_generation;
    }
}

extern "C" void saved_game_files_delete_personal_storage_for_player_count(long controller_index, long player_count)
{
    if ((unsigned long)controller_index >= 4 || player_count < 1 || player_count > 4)
        return;
    s_controller *controller = &controller_globals().controllers[controller_index];
    if (!((c_controller_interface *)controller)->is_signed_in_to_machine())
        return;
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
    long content_item = saved_game_files_get_personal_storage_content_item((e_controller_index)controller_index, false);
    if (content_item != -1 && ((s_content_item *)catalogue)->make_ready_for_access(content_item, true)) {
        saved_game_files_delete_slot_files_in_ready_container(
            (e_controller_index)controller_index, controller, catalogue, content_item, player_count);
    }
}

static bool saved_game_files_get_game_state_options_for_player_count(
    e_controller_index controller_index,
    long player_count,
    void *buffer,
    long buffer_size)
{
    (void)buffer_size;
    if ((unsigned long)controller_index >= 4 || player_count < 1 || player_count > 4 || !buffer)
        return false;
    s_controller *controller = &controller_globals().controllers[controller_index];
    if (!((c_controller_interface *)controller)->is_signed_in_to_machine())
        return false;

    long content_item = saved_game_files_get_personal_storage_content_item(controller_index, false);
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
    if (content_item == -1 || !((s_content_item *)catalogue)->make_ready_for_access(content_item, true))
        return false;

    const char *header_name = 0;
    const char *state_name = 0;
    saved_game_files_get_slot_file_names(player_count, &header_name, &state_name, 0, 0);

    s_file_reference header_file;
    s_file_reference state_file;
    bool have_refs =
        ::saved_game_files_create_saved_game_file_reference(
            controller_index, (wchar_t const *)controller->display_name, header_name, &header_file, false, false) &&
        ::saved_game_files_create_saved_game_file_reference(
            controller_index, (wchar_t const *)controller->display_name, state_name, &state_file, false, false);
    if (!have_refs)
        return false;

    if (!file_exists(&header_file)) {
        if (file_exists(&state_file)) {
            saved_game_files_delete_slot_files_in_ready_container(
                controller_index, controller, catalogue, content_item, player_count);
        } else if (!saved_game_files_container_has_any_save(controller_index)) {
            saved_game_files_delete_content_item(controller_index, content_item);
        }
        return false;
    }

    bool result = false;
    unsigned long open_error = 0;
    if (file_exists(&state_file) && file_open(&header_file, 1, &open_error)) {
        result = file_read(&header_file, 0xf998, false, buffer);
        file_close(&header_file);
        if (result && !game_state_saved_game_header_valid((const game_state_header *)buffer))
            result = false;
    }
    if (!result) {
        saved_game_files_delete_slot_files_in_ready_container(
            controller_index, controller, catalogue, content_item, player_count);
    }
    return result;
}

extern "C" bool saved_game_files_read_game_options_for_player_count(
    e_controller_index controller_index,
    long player_count,
    game_options *options)
{
    if (!options || player_count < 1 || player_count > 4)
        return false;
    game_state_header header;
    if (!saved_game_files_get_game_state_options_for_player_count(
            controller_index, player_count, &header, sizeof(header)))
    {
        return false;
    }
    if (header.game_options.game_mode != 1)
        return false;
    long valid_players = 0;
    for (long player_index = 0; player_index < 16; ++player_index) {
        if (header.game_options.players[player_index].valid)
            ++valid_players;
    }
    if (valid_players != player_count)
        return false;
    memcpy(options, &header.game_options, sizeof(*options));
    return true;
}
#endif

// @address 0x1e75c8 saved_game_files_get_game_state_options
bool saved_game_files_get_game_state_options(e_controller_index controller_index, void *buffer, long buffer_size)
{
#ifdef HALO3_64BIT
    return saved_game_files_get_game_state_options_for_player_count(
        controller_index, saved_game_files_get_active_player_count(), buffer, buffer_size);
#else
    s_controller *controller = &controller_globals().controllers[controller_index];
    s_file_reference file;
    unsigned long open_error;
    long content_item;
    bool result = false;

    if (((c_controller_interface *)controller)->is_signed_in_to_machine()) {
        content_item = saved_game_files_get_personal_storage_content_item(controller_index, false);
        if (content_item != -1 &&
            ((s_content_item *)((c_content_catalogue *)&content_catalogues().catalogues[
                controller - controller_globals().controllers]))->make_ready_for_access(content_item, true))
            {
            if (::saved_game_files_create_saved_game_file_reference(controller_index, (wchar_t const *)(controller->display_name),
                (char const *)("gamestate.hdr"), &file, false, false) &&
                file_open(&file, 1, &open_error)) {
                result = file_read(&file, 0xf998, false, buffer);
                file_close(&file);
            }
            if (!result)
                saved_game_files_delete_content_item(controller_index, content_item);
        }
    }
    return result;
#endif
}
// @address 0x1e76f8 ?saved_game_files_get_saved_game_state_header@@YA_NW4e_controller_index@@PAUgame_state_header@@J@Z
bool saved_game_files_get_saved_game_state_header(e_controller_index controller_index, game_state_header *header, long header_size)
{
#ifdef HALO3_64BIT
    (void)header_size;
    if ((unsigned long)controller_index >= 4 || !header)
        return false;
    long player_count = saved_game_files_get_active_player_count();
    const char *state_name = 0;
    saved_game_files_get_slot_file_names(player_count, 0, &state_name, 0, 0);

    s_controller *controller = &controller_globals().controllers[controller_index];
    s_file_reference file;
    unsigned long open_error = 0;
    bool result = false;

    if (((c_controller_interface *)controller)->is_signed_in_to_machine()) {
        long content_item = saved_game_files_get_personal_storage_content_item(controller_index, false);
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
        if (content_item != -1 && ((s_content_item *)catalogue)->make_ready_for_access(content_item, true)) {
            if (::saved_game_files_create_saved_game_file_reference(
                    controller_index, (wchar_t const *)controller->display_name, state_name, &file, false, false) &&
                file_exists(&file) &&
                file_open(&file, 1, &open_error))
            {
                result = file_read(&file, 0x7e0000, false, header);
                file_close(&file);
            }
            if (!result) {
                saved_game_files_delete_slot_files_in_ready_container(
                    controller_index, controller, catalogue, content_item, player_count);
            }
        }
    }
    return result;
#else
    s_controller *controller = &controller_globals().controllers[controller_index];
    s_file_reference file;
    unsigned long open_error;
    long content_item;
    bool result = false;

    if (((c_controller_interface *)controller)->is_signed_in_to_machine()) {
        content_item = saved_game_files_get_personal_storage_content_item(controller_index, false);
        if (content_item != -1 &&
            ((s_content_item *)((c_content_catalogue *)&content_catalogues().catalogues[
                controller - controller_globals().controllers]))->make_ready_for_access(content_item, true))
            {
            if (::saved_game_files_create_saved_game_file_reference(controller_index, (wchar_t const *)(controller->display_name),
                (char const *)("mmiof.bmf"), &file, false, false) &&
                file_open(&file, 1, &open_error)) {
                result = file_read(&file, 0x7e0000, false, header);
                file_close(&file);
            }
            if (!result)
                saved_game_files_delete_content_item(controller_index, content_item);
        }
    }
    return result;
#endif
}


#ifdef HALO3_64BIT
struct s_saved_game_async_autosave_target
{
    long controller_index;
    long content_item;
    const char *final_header_name;
    const char *final_state_name;
    s_file_reference tmp_header_file;
    s_file_reference tmp_state_file;
    s_file_reference header_file;
    s_file_reference state_file;
    bool succeeded;
};

struct s_saved_game_async_autosave_state
{
    long target_count;
    const void *buffer;
    c_synchronized_long done;
    s_saved_game_async_autosave_target targets[4];
};

static s_saved_game_async_autosave_state g_saved_game_async_autosave_state;

static bool saved_game_files_write_atomic_target(
    const s_saved_game_async_autosave_target *target,
    const game_state_header *header,
    const void *buffer)
{
    s_file_reference tmp_header = target->tmp_header_file;
    s_file_reference tmp_state = target->tmp_state_file;
    s_file_reference final_header = target->header_file;
    s_file_reference final_state = target->state_file;
    unsigned long open_error = 0;
    bool wrote_tmp = false;

    if (file_exists(&tmp_header))
        file_delete(&tmp_header);
    if (file_exists(&tmp_state))
        file_delete(&tmp_state);

    if (file_create(&tmp_header) && file_open(&tmp_header, 2, &open_error)) {
        wrote_tmp = file_write(&tmp_header, sizeof(game_state_header), header);
        file_close(&tmp_header);
    }
    if (wrote_tmp) {
        wrote_tmp = false;
        if (file_create(&tmp_state) && file_open(&tmp_state, 2, &open_error)) {
            wrote_tmp = file_write(&tmp_state, 0x7e0000, buffer);
            file_close(&tmp_state);
        }
    }

    if (!wrote_tmp) {
        if (file_exists(&tmp_header))
            file_delete(&tmp_header);
        if (file_exists(&tmp_state))
            file_delete(&tmp_state);
        return false;
    }

    if (file_exists(&final_header))
        file_delete(&final_header);
    if (file_exists(&final_state))
        file_delete(&final_state);

    if (!file_rename(&tmp_state, target->final_state_name)) {
        if (file_exists(&tmp_header))
            file_delete(&tmp_header);
        if (file_exists(&tmp_state))
            file_delete(&tmp_state);
        return false;
    }
    if (!file_rename(&tmp_header, target->final_header_name)) {
        if (file_exists(&tmp_header))
            file_delete(&tmp_header);
        if (file_exists(&final_state))
            file_delete(&final_state);
        return false;
    }
    return true;
}

static long saved_game_files_collect_autosave_targets(
    const game_state_header *header,
    s_saved_game_async_autosave_target (&targets)[4])
{
    long target_count = 0;
    long signed_in_count = 0;
    if (!game_state_saved_game_header_valid(header))
        return 0;

    long valid_players = 0;
    for (unsigned long player_index = 0; player_index < 16; ++player_index) {
        if (header->game_options.players[player_index].valid)
            ++valid_players;
    }
    if (valid_players < 1 || valid_players > 4)
        return 0;

    const char *header_name = 0;
    const char *state_name = 0;
    const char *tmp_header_name = 0;
    const char *tmp_state_name = 0;
    saved_game_files_get_slot_file_names(
        valid_players, &header_name, &state_name, &tmp_header_name, &tmp_state_name);

    for (long controller_index = 0; controller_index < 4; ++controller_index) {
        if (((c_controller_interface *)&controller_globals().controllers[controller_index])->is_signed_in_to_machine())
            ++signed_in_count;
    }

    for (long controller_index = 0; controller_index < 4; ++controller_index) {
        s_controller *controller = &controller_globals().controllers[controller_index];
        if (!((c_controller_interface *)controller)->is_signed_in_to_machine())
            continue;

        s_player_identifier identifier_storage;
        const s_player_identifier *identifier = ((c_controller_interface *)controller)->get_player_identifier(&identifier_storage);
        bool matches = false;
        for (unsigned long player_index = 0; player_index < 16; ++player_index) {
            const game_player_options *player = &header->game_options.players[player_index];
            if (player->valid && memcmp(&player->player_identifier, identifier, sizeof(s_player_identifier)) == 0) {
                matches = true;
                break;
            }
        }
        if (!matches)
            continue;

        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
        long content_item = saved_game_files_get_personal_storage_content_item((e_controller_index)controller_index, true);
        if (content_item != -1 && ((s_content_item *)catalogue)->make_ready_for_access(content_item, true)) {
            s_saved_game_async_autosave_target &target = targets[target_count];
            target.controller_index = controller_index;
            target.content_item = content_item;
            target.final_header_name = header_name;
            target.final_state_name = state_name;
            target.succeeded = false;
            if (saved_game_files_create_saved_game_file_reference((e_controller_index)controller_index, controller->display_name,
                    tmp_header_name, &target.tmp_header_file, false, true) &&
                saved_game_files_create_saved_game_file_reference((e_controller_index)controller_index, controller->display_name,
                    tmp_state_name, &target.tmp_state_file, false, true) &&
                saved_game_files_create_saved_game_file_reference((e_controller_index)controller_index, controller->display_name,
                    header_name, &target.header_file, false, true) &&
                saved_game_files_create_saved_game_file_reference((e_controller_index)controller_index, controller->display_name,
                    state_name, &target.state_file, false, true))
            {
                ++target_count;
            }
        }
    }
    (void)signed_in_count;
    return target_count;
}

static void saved_game_files_finalize_autosave_targets(
    const s_saved_game_async_autosave_target *targets,
    long target_count)
{
    for (long index = 0; index < target_count; ++index) {
        const s_saved_game_async_autosave_target &target = targets[index];
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[target.controller_index];
        if (target.succeeded) {
            content_catalogue_flush_item(catalogue, target.content_item, true);
        } else if (!file_exists(&target.header_file) &&
                   !saved_game_files_container_has_any_save((e_controller_index)target.controller_index))
        {
            saved_game_files_delete_content_item(target.controller_index, target.content_item);
        }
    }
    if (target_count > 0)
        ++g_saved_game_personal_storage_generation;
}

static e_async_completion saved_game_files_commit_async_autosave_callback(s_async_task *)
{
    const void *buffer = g_saved_game_async_autosave_state.buffer;
    const game_state_header *header = (const game_state_header *)buffer;
    if (buffer != 0) {
        for (long index = 0; index < g_saved_game_async_autosave_state.target_count; ++index) {
            s_saved_game_async_autosave_target &target = g_saved_game_async_autosave_state.targets[index];
            target.succeeded = saved_game_files_write_atomic_target(&target, header, buffer);
        }
    }
    __lwsync();
    _InterlockedExchange(&g_saved_game_async_autosave_state.done.value, 1);
    return (e_async_completion)1;
}

extern "C" void saved_game_files_prepare_async_autosave(const game_state_header *header)
{
    g_saved_game_async_autosave_state.buffer = 0;
    __lwsync();
    _InterlockedExchange(&g_saved_game_async_autosave_state.done.value, 0);
    saved_game_files_write_saved_game_state(header, 0, 0, -1);
}

extern "C" bool saved_game_files_has_prepared_async_autosave(void)
{
    return g_saved_game_async_autosave_state.target_count > 0;
}

extern "C" void saved_game_files_cancel_async_autosave(void)
{
    g_saved_game_async_autosave_state.target_count = 0;
    g_saved_game_async_autosave_state.buffer = 0;
    __lwsync();
    _InterlockedExchange(&g_saved_game_async_autosave_state.done.value, 0);
}

extern "C" bool saved_game_files_start_async_autosave_commit(const void *hashed_buffer)
{
    if (g_saved_game_async_autosave_state.target_count <= 0 || hashed_buffer == 0) {
        saved_game_files_cancel_async_autosave();
        return false;
    }
    g_saved_game_async_autosave_state.buffer = hashed_buffer;
    __lwsync();
    _InterlockedExchange(&g_saved_game_async_autosave_state.done.value, 0);
    s_async_task task;
    memset(&task, 0, sizeof(task));
    long task_id = ::async_task_add_ex(_async_priority_blocking_generic, &task, _async_category_saved_games,
        saved_game_files_commit_async_autosave_callback, &g_saved_game_async_autosave_state.done, true);
    if (task_id == -1) {
        saved_game_files_commit_async_autosave_callback(&task);
        saved_game_files_finalize_autosave_targets(
            g_saved_game_async_autosave_state.targets, g_saved_game_async_autosave_state.target_count);
        saved_game_files_cancel_async_autosave();
        return false;
    }
    return true;
}

extern "C" bool saved_game_files_is_async_autosave_commit_done(void)
{
    long done = g_saved_game_async_autosave_state.done.value;
    __lwsync();
    return done != 0;
}

extern "C" void saved_game_files_finish_async_autosave(void)
{
    if (g_saved_game_async_autosave_state.target_count > 0) {
        saved_game_files_finalize_autosave_targets(
            g_saved_game_async_autosave_state.targets, g_saved_game_async_autosave_state.target_count);
    }
    saved_game_files_cancel_async_autosave();
}

extern "C" void saved_game_files_delete_campaign_session_saved_game_state(const game_options *options)
{
    if (!options || options->game_mode != 1)
        return;
    long session_player_count = 0;
    for (long player_index = 0; player_index < 16; ++player_index) {
        if (options->players[player_index].valid)
            ++session_player_count;
    }
    if (session_player_count < 1 || session_player_count > 4)
        return;

    for (long controller_index = 0; controller_index < 4; ++controller_index) {
        saved_game_files_delete_personal_storage_for_player_count(controller_index, session_player_count);
    }
}
#endif

// @address 0x1e7820 ?saved_game_files_write_saved_game_state@@YAXPBUgame_state_header@@JPBXJ@Z
void saved_game_files_write_saved_game_state(const game_state_header *header, long header_size, const void *buffer, long buffer_size)
{
#ifdef HALO3_64BIT
    (void)header_size;
    s_saved_game_async_autosave_target local_targets[4];
    s_saved_game_async_autosave_target (&targets)[4] =
        (buffer == 0 && buffer_size == -1) ? g_saved_game_async_autosave_state.targets : local_targets;
    long target_count = saved_game_files_collect_autosave_targets(header, targets);

    long valid_players = 0;
    long signed_in_count = 0;
    if (header) {
        for (unsigned long player_index = 0; player_index < 16; ++player_index) {
            if (header->game_options.players[player_index].valid)
                ++valid_players;
        }
    }
    for (long controller_index = 0; controller_index < 4; ++controller_index) {
        if (((c_controller_interface *)&controller_globals().controllers[controller_index])->is_signed_in_to_machine())
            ++signed_in_count;
    }
    for (long index = 0; index < target_count; ++index) {
        s_controller *controller = &controller_globals().controllers[targets[index].controller_index];
        c_player_profile_interface *profile = controller->get_player_profile_interface();
        if (profile) {
            __int64 time = _time64(0);
            long cooperative = (valid_players > 1 || signed_in_count > 1) ? 1 : 0;
            profile->dirty_or(false);
            profile->campaign.last_campaign_played_time[cooperative] = time;
        }
    }

    if (buffer == 0 && buffer_size == -1) {
        g_saved_game_async_autosave_state.target_count = target_count;
        return;
    }

    for (long index = 0; index < target_count; ++index) {
        targets[index].succeeded = saved_game_files_write_atomic_target(&targets[index], header, buffer);
    }
    saved_game_files_finalize_autosave_targets(targets, target_count);
#else
    long signed_in_count = 0;
    if (game_state_saved_game_header_valid(header)) {
        long controller_index = 0;
        do {
            if (((c_controller_interface *)&controller_globals().controllers[controller_index])->is_signed_in_to_machine())
                signed_in_count++;
            long next_controller_index = -1;
            if ((unsigned long)controller_index <= 2)
                next_controller_index = controller_index + 1;
            controller_index = next_controller_index;
        } while (controller_index != -1);
        controller_index = 0;
        do {
            s_controller *controller = &controller_globals().controllers[controller_index];
            if (((c_controller_interface *)controller)->is_signed_in_to_machine()) {
                s_player_identifier identifier_storage;
                const s_player_identifier *identifier = ((c_controller_interface *)controller)->get_player_identifier(&identifier_storage);
                bool matches = false;
                for (unsigned long player_index = 0; player_index < 16; player_index++) {
                    const game_player_options *player = &header->game_options.players[player_index];
                    if (player->valid && memcmp(&player->player_identifier, identifier, sizeof(s_player_identifier)) == 0) {
                        matches = true;
                        break;
                    }
                }
                if (matches) {
                    const char *header_name = "gamestate.hdr";
                    const char *state_name = "mmiof.bmf";
                    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller - controller_globals().controllers];
                    long content_item = saved_game_files_get_personal_storage_content_item((e_controller_index)controller_index, true);
                    if (content_item != -1 &&
                        ((s_content_item *)catalogue)->make_ready_for_access(content_item, true)) {
                        s_file_reference header_file;
                        s_file_reference state_file;
                        unsigned long open_error;
                        bool success = false;
                        if (saved_game_files_create_saved_game_file_reference((e_controller_index)controller_index, controller->display_name,
                            header_name, &header_file, false, true) &&
                            saved_game_files_create_saved_game_file_reference((e_controller_index)controller_index, controller->display_name,
                                state_name, &state_file, false, true))
                            {
                            file_delete(&header_file);
                            file_delete(&state_file);
                            content_catalogue_flush_item(catalogue, content_item, true);
                            if (file_create(&header_file) && file_open(&header_file, 2, &open_error)) {
                                success = file_write(&header_file, sizeof(game_state_header), header);
                                file_close(&header_file);
                            }
                            if (success) {
                                success = false;
                                if (file_create(&state_file) && file_open(&state_file, 2, &open_error)) {
                                    success = file_write(&state_file, 0x7e0000, buffer);
                                    file_close(&state_file);
                                }
                            }
                        }
                        if (!success)
                            saved_game_files_delete_content_item(controller_index, content_item);
                    }
                    c_player_profile_interface *profile = controller->get_player_profile_interface();
                    if (profile) {
                        __int64 time = _time64(0);
                        long cooperative = signed_in_count > 1 ? 1 : 0;
                        profile->dirty_or(false);
                        profile->campaign.last_campaign_played_time[cooperative] = time;
                    }
                }
            }
            long next_controller_index = -1;
            if ((unsigned long)controller_index <= 2)
                next_controller_index = controller_index + 1;
            controller_index = next_controller_index;
        } while (controller_index != -1);
    }
#endif
}
// @address 0x1e7be8 saved_game_files_read
extern "C" long saved_game_files_read(long catalogue_index, long content_item_index, void *buffer, unsigned long size,
    c_synchronized_long *completion, c_synchronized_long *done)
{
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[catalogue_index];
    long result = -1;
    char path[264];
    if (((s_content_item *)catalogue)->make_ready_for_access(content_item_index, true)) {
        if (saved_game_files_get_content_item_file_path(catalogue_index, content_item_index, path, 256)) {
            result = async_read_entire_file(path, buffer, size, 0, _async_category_saved_games, _async_priority_blocking_generic, completion, done);
        } else {
            content_catalogue_close_item(catalogue, content_item_index, true);
        }
    }
    return result;
}
// @address 0x1e7ca8 saved_game_files_write_content_item
extern "C" long saved_game_files_write_content_item(long catalogue_index, long content_item_index, long file_type, const void *buffer,
    unsigned long size, c_synchronized_long *completion, void *done)
{
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[catalogue_index];
    long result = -1;
    char path[264];
    if (((s_content_item *)catalogue)->make_ready_for_access(content_item_index, true)) {
        if (!saved_game_files_get_content_item_file_path(catalogue_index, content_item_index, path, 256) ||
            !content_catalogue_set_item_metadata(catalogue, content_item_index, &((const s_blffile_saved_game_file *)buffer)->get_metadata()) ||
            (result = async_write_buffer_to_file(path, buffer, size, 0, 8, (void *)5, completion, done)) == -1) {
            content_catalogue_close_item(catalogue, content_item_index, true);
        }
    }
    return result;
}
// @address 0x1ea490 saved_game_files_update_metadata_callback
extern "C" e_async_completion saved_game_files_update_metadata_callback(s_async_task *task)
{
    s_bitmap_load_task *bitmap_task = (s_bitmap_load_task *)task;
    int result = 0;
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[bitmap_task->catalogue_index];
    if (content_catalogue_has_storage_device(catalogue)) {
        e_game_content_type content_type = content_catalogue_get_game_content_type(catalogue, bitmap_task->content_item_index);
        if ((unsigned int)game_content_type_to_saved_game_file_type(content_type) <= 13) {
            s_file_reference file;
            if (::c_start_menu_game_editor::load_progress_ui_for_catalogue((c_start_menu_game_editor *)catalogue,
                    (e_controller_index)bitmap_task->content_item_index, (s_file_reference *)&file))
            {
                if (((s_content_item *)catalogue)->make_ready_for_access(bitmap_task->content_item_index, true)) {
                    s_saved_game_item_metadata_packed metadata;
                    unsigned __int64 identifier;
                    memset(&metadata, 0, sizeof(metadata));
                    if (saved_game_read_metadata_from_file(&file, &metadata)) {
                        const wchar_t *name = bitmap_task->name;
                        if (name != 0) {
                            memset(metadata.name, 0, sizeof(metadata.name));
                            saved_game_files_delete_ustrnzcpy((unsigned short *)metadata.name, (const unsigned short *)name, 16);
                        }
                        const wchar_t *author = bitmap_task->description;
                        if (author != 0) {
                            memset(metadata.description, 0, sizeof(metadata.description));
                            wchar_string_to_utf8_string(author, metadata.description, sizeof(metadata.description), 0);
                        }
                        XNetRandom((unsigned char *)&identifier, sizeof(identifier));
                        metadata.unique_id = identifier;
                        metadata.date = _time64(0);
                        if (content_catalogue_set_item_metadata(catalogue, bitmap_task->content_item_index, &metadata))
                            result = ((int (*)(s_file_reference *, const s_saved_game_item_metadata_packed *, int,
                                int))saved_game_write_metadata_to_file)(&file, &metadata, content_type, 1);
                    }
                    content_catalogue_close_item(catalogue, bitmap_task->content_item_index, true);
                }
            }
        }
    }
    return (e_async_completion)((long (*)(int, long, volatile long *, volatile long *))saved_game_files_set_async_result)(
        result, 1, &bitmap_task->completion->value, &bitmap_task->done->value);
}
// @address 0x1ea230 saved_game_files_create_content_item_callback
extern "C" e_async_completion saved_game_files_create_content_item_callback(s_async_task *task)
{
    s_file_delete_task *create_task = (s_file_delete_task *)task;
    bool success = false;
    e_game_content_type content_type = saved_game_files_file_type_to_content_type((e_saved_game_file_type)create_task->file_type);
    if (content_type != _game_content_type_none) {
        const char *file_name = (const char *)::saved_game_files_get_saved_game_file_name((e_saved_game_file_type)create_task->file_type);
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[create_task->controller_index];
        s_file_reference file;
        long content_item_index;
        if (saved_game_files_create_content_item_file(create_task->controller_index, content_type, (const wchar_t *)create_task->path, file_name,
            &file, &content_item_index) &&
            content_item_index != -1 && ((s_content_item *)catalogue)->make_ready_for_access(content_item_index, true)) {
            if (::file_create(&file))
                success = true;
            content_catalogue_close_item(catalogue, content_item_index, true);
            if (success)
                create_task->status->set(content_item_index);
            else
                content_catalogue_delete_item(catalogue, content_item_index, true);
        }
    }
    return (e_async_completion)saved_game_files_set_async_result(success, 1, &create_task->completion->value, &create_task->done->value);
}
// @address 0x229270 saved_game_files_copy_file_callback
extern "C" e_async_completion saved_game_files_copy_file_callback(s_saved_game_file_copy_task *task)
{
    bool success = false;
    long controller_index = task->controller_index;
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
    bool footer_written = false;
    if (content_catalogue_has_storage_device(catalogue)) {
        if (content_catalogue_has_pending_tasks(catalogue)) {
            success = true;
        } else {
            if (task->state == 0) {
                s_saved_game_item_metadata_packed *metadata = task->metadata;
                long file_type = metadata->file_type;
                e_game_content_type content_type = saved_game_file_type_to_game_content_type((e_saved_game_file_type)file_type);
                if ((unsigned long)content_type <= (unsigned long)_game_content_type_shot) {
                    const char *file_name = (const char *)::saved_game_files_get_saved_game_file_name((e_saved_game_file_type)metadata->file_type);
                    saved_game_files_find_unique_name(controller_index, file_type, metadata->name, 16);
                    if (saved_game_files_create_content_item_file(task->controller_index, content_type, task->metadata->name, file_name,
                        task->destination, &task->content_item_index) &&
                        task->content_item_index != -1) {
                        success = true;
                        task->state = 1;
                    }
                }
            } else if (task->state == 1) {
                if (((s_content_item *)catalogue)->make_ready_for_access(task->content_item_index, false)) {
                    success = true;
                    task->state = 2;
                }
            } else if (task->state == 2) {
                unsigned long open_error;
                if (::file_create(task->destination) &&
                    file_open(task->destination, 2, &open_error) &&
                    file_open(task->source, 1, &open_error) &&
                    file_get_size(task->source, &task->file_size)) {
                    success = true;
                    task->state = 3;
                }
            } else if (task->state == 3) {
                s_blf_header file_header;
                if ((unsigned long)(task->file_size - task->source->position) >= 12 &&
                    file_read(task->source, sizeof(file_header), true, &file_header)) {
                    if (file_header.chunk_type == '_eof' && file_header.major_version == 1) {
                        success = true;
                        task->state = 4;
                    } else if (file_header.chunk_type == 'chdr' && file_header.major_version == 9) {
                        s_blf_chunk_content_header content_header;
                        memset(&content_header, 0, sizeof(content_header));
                        memcpy(&content_header, &file_header, sizeof(file_header));
                        if ((unsigned long)file_header.chunk_size >= sizeof(content_header) &&
                            file_read(task->source, sizeof(content_header) - sizeof(file_header), true,
                                (char *)&content_header + sizeof(file_header))) {
                            memset(content_header.metadata.name, 0, sizeof(content_header.metadata.name));
                            saved_game_files_ustrncpy(content_header.metadata.name, task->metadata->name, 16);
                            content_catalogue_set_item_metadata(catalogue, task->content_item_index, &content_header.metadata);
                            if (file_write(task->destination, sizeof(content_header), &content_header))
                                success = true;
                        }
                    } else {
                        if (file_write(task->destination, sizeof(file_header), &file_header)) {
                            success = true;
                            long remaining = file_header.chunk_size - sizeof(file_header);
                            if (remaining > 0) {
                                char copy_buffer[0x2000];
                                while (remaining > 0) {
                                    long chunk_size = remaining < sizeof(copy_buffer) ? remaining : sizeof(copy_buffer);
                                    if ((unsigned long)(task->file_size - task->source->position) < (unsigned long)chunk_size ||
                                        !file_read(task->source, chunk_size, true, copy_buffer) ||
                                        !file_write(task->destination, chunk_size, copy_buffer)) {
                                        success = false;
                                        break;
                                    }
                                    remaining -= chunk_size;
                                }
                            }
                        }
                    }
                }
            } else if (task->state == 4) {
                s_blf_chunk_end_of_file footer;
                footer.initialize();
                footer.total_file_size = file_get_eof(task->destination);
                if (file_write(task->destination, sizeof(footer), &footer)) {
                    success = true;
                    footer_written = true;
                }
                if (!file_close(task->destination))
                    success = false;
                if (!file_close(task->source))
                    success = false;
            }
            if (footer_written || !success) {
                if (task->content_item_index != -1) {
                    content_catalogue_close_item(catalogue, task->content_item_index, true);
                    if (!success)
                        content_catalogue_delete_item(catalogue, task->content_item_index, true);
                }
            }
        }
    }
    task->completion->set(success && footer_written);
    return (e_async_completion)(footer_written || !success);
}

// @address 0x284bf0 ?content_catalogue_set_item_metadata@@YA_NPBVc_content_catalogue@@JPBUs_saved_game_item_metadata_packed@@@Z
bool content_catalogue_set_item_metadata(const c_content_catalogue *catalogue, long content_item_index,
    const s_saved_game_item_metadata_packed *metadata)
{
    bool result = false;
    if (saved_game_metadata_is_valid(metadata)) {
        c_saved_game_scope_lock lock;
        if (content_catalogue_has_storage_device(catalogue)) {
            s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
            if (item) {
                s_saved_game_item_metadata_packed *item_metadata = &item->metadata;
                memcpy(item_metadata, metadata, sizeof(*metadata));
                item->flags.set_bit(4, saved_game_metadata_is_valid(item_metadata));
                result = true;
            }
        }
    }
    return result;
}

// @address 0x284e08 content_catalogue_get_item_file_path
extern "C" bool content_catalogue_get_item_file_path(const c_content_catalogue *catalogue, long content_item_index, char *path, long path_size)
{
    bool result = false;
    path[0] = 0;
    c_saved_game_scope_lock lock;
    if (content_catalogue_has_storage_device(catalogue)) {
        s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (item) {
            e_saved_game_file_type file_type = game_content_type_to_saved_game_file_type(item->game_content_type);
            if (file_type >= _saved_game_file_type_bmf && file_type <= _saved_game_file_type_shot) {
                item->get_container_full_path(path, 256, true);
                csstrnzcat(path, (const char *)::saved_game_files_get_saved_game_file_name(file_type), 256);
            }
            result = path[0] != 0;
        }
    }
    return result;
}

// Marketplace asset check.
extern "C" {
// @address 0x7c33f0 g_marketplace_asset_consume_task
c_marketplace_consume_assets_task g_marketplace_asset_consume_task;

}

// ---- marketplace subscription status ----


// @address 0x4a4390 saved_game_files_parse_subscription_response
extern "C" int saved_game_files_parse_subscription_response(s_controller_update *update, void *unused, unsigned char active, int result)
{
    int completion_result = result;
    if (active) {
        c_online_file_game_api_parser parser;
        do {
            {
                const char *data = 0;
                long size = 0;
                long data_result = g_marketplace_http_downloader.get_data(&data, &size);
                if (data_result == 2) {
                    if (data == 0) {
                        completion_result = 0;
                        break;
                    }
                    parser.begin = (char *)data;
                    parser.next = (char *)data;
                    parser.size = size;
                } else if (data_result == 1) {
                    completion_result = result;
                    break;
                } else {
                    completion_result = 0;
                    break;
                }
            }
            {
                long previous_status = update->subscription_status;
                unsigned __int64 seconds = 0;
                c_static_wchar_string<256> status;
                bool valid = true;
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "Status: ", &status);
                if (valid) {
                    if (wcsncmp(status.get_string(), L"NeverSubscribed", 256) == 0)
                        update->subscription_status = 1;
                    else if (wcsncmp(status.get_string(), L"Subscribed", 256) == 0)
                        update->subscription_status = 2;
                    else if (wcsncmp(status.get_string(), L"SubscribedRenew", 256) == 0)
                        update->subscription_status = 3;
                    else if (wcsncmp(status.get_string(), L"Expired", 256) == 0)
                        update->subscription_status = 4;
                    else {
                        valid = false;
                        update->subscription_status = 0;
                    }
                }
                valid = valid && parser.read_qword("NextOfferID: ", &update->offer_id);
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "HQButton: ", &update->hq_button);
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "HQMessage: ", &update->hq_message);
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "FileShareButton: ", &update->file_share_button);
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "FileShareMessage: ", &update->file_share_message);
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "FileShareHelp: ", &update->file_share_help);
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "JustSubscribedMessage: ", &update->just_subscribed_message);
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "CurrentlySubscribedMessage: ", &update->currently_subscribed_message);
                valid = valid && online_file_game_api_parser_read_wstring(&parser, "OverQuotaMessage: ", &update->over_quota_message);
                valid = valid && parser.read_qword("SubscriptionSecondsPast19700101: ", &seconds);
                valid = valid && parser.read_long("SubscriptionHash: ", &update->subscription_hash);
                update->subscription_seconds = seconds;
                update->active_14 = valid;
                completion_result = 0;
                update->update_count++;
                if ((previous_status == 1 || previous_status == 4) && (update->subscription_status == 2 || update->subscription_status == 3)) {
                    if (update->just_subscribed_message.length() > 0)
                        g_gui_error_manager.post_error_with_custom_message(0x20007, 0, update->just_subscribed_message.get_string(),
                            update->controller_index, false, true);
                }
            }
        } while (false);
    } else {
        g_marketplace_http_downloader.reset();
        update->active_14 = false;
        update->subscription_status = 0;
        memset(&update->state_10, 0, sizeof(update->state_10));
        completion_result = 0;
    }
    return completion_result;
}
// @address 0x4a47d0 saved_game_files_handle_marketplace_assets
extern "C" long saved_game_files_handle_marketplace_assets(s_player_marketplace_user *user, void *controller, bool signed_in, long result)
{
    if (signed_in) {
        short task_state = g_marketplace_asset_enumeration_overlapped_task.get_task_state();
        if (task_state == _overlapped_task_state_success) {
            for (long index = 0; index < g_marketplace_asset_enumeration_overlapped_task.m_asset_count; index++) {
                if (g_marketplace_asset_enumeration_overlapped_task.m_assets[index].quantity > 0) {
                    g_gui_error_manager.clear_error(0x2008c, g_marketplace_asset_consume_task.controller_index, 0);
                    user->owns_asset = true;
                    user->asset = g_marketplace_asset_enumeration_overlapped_task.m_assets[index];
                    break;
                }
            }
            result = user->owns_asset ? 2 : 0;
        } else if (task_state == _overlapped_task_state_failure) {
            result = 0;
        }
    } else {
        overlapped_task_terminate(&g_marketplace_asset_enumeration_overlapped_task);
        user->enumeration_state = 0;
        result = 0;
    }
    return result;
}

// @address 0x4a48e8 marketplace_consume_assets
extern "C" bool marketplace_consume_assets(s_player_marketplace_user *user, unsigned __int64 offer_id)
{
    bool result = false;
    if (!g_marketplace_asset_consume_task.busy()) {
        g_marketplace_asset_consume_task.initialize((e_controller_index)user->controller_index, offer_id);
        result = overlapped_task_start_internal(&g_marketplace_asset_consume_task, "..\\interface\\c_player_marketplace.cpp", 659);
        user->consume_pending = true;
    }
    return result;
}

// 0x1eb020
static __forceinline unsigned long saved_game_files_unique_name_length(const wchar_t *string, unsigned long maximum)
{
    unsigned long length = 0;
    while (length < maximum && string[length])
        length++;
    return length;
}

static __forceinline wchar_t *saved_game_files_unique_name_copy(wchar_t *dest, const wchar_t *src, long count)
{
    const wchar_t *s = src;
    for (long i = 0; i <= count - 1; i++) {
        wchar_t c = *s;
        dest[i] = c;
        if (c == 0)
            break;
        ++s;
    }
    dest[count - 1] = 0;
    return dest;
}


// @address 0x1eb020 saved_game_files_find_unique_name
extern "C" void saved_game_files_find_unique_name(long controller_index, long file_type, wchar_t *name, unsigned long name_size)
{
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
    unsigned long length = saved_game_files_unique_name_length(name, name_size);
    if (content_catalogue_has_storage_device(catalogue)) {
        long content_type = (long)saved_game_file_type_to_game_content_type((e_saved_game_file_type)file_type);
        if (!(unsigned char)content_catalogue_is_name_available(catalogue, name, content_type)) {
            if (iswdigit(name[length - 1]) && iswdigit(name[length - 2]) && iswdigit(name[length - 3])) {
                unsigned long space_index = length - 4;
                if (name[space_index] == L' ') {
                    length = space_index;
                    name[space_index] = 0;
                }
            }

            wchar_t *suffix = length + 4 < name_size ? &name[length] : &name[name_size - 6];
            if (suffix < name)
                suffix = name;

            long first_number = g_saved_game_files_name_counters[file_type] < 1 ? 1 : g_saved_game_files_name_counters[file_type];
            long number = first_number;
            bool wrapped = false;
            while (true) {
                if (number > 999) {
                    number = 1;
                    wrapped = true;
                }
                if (number == first_number && wrapped)
                    break;

                wchar_t number_string[5];
                number_string[0] = L' ';
                number_string[1] = (wchar_t)(L'0' + number / 100);
                number_string[2] = (wchar_t)(L'0' + number % 100 / 10);
                number_string[3] = (wchar_t)(L'0' + number % 10);
                number_string[4] = 0;
                saved_game_files_unique_name_copy(suffix, number_string, 5);
                if ((unsigned char)content_catalogue_is_name_available(catalogue, name, content_type))
                    break;
                number++;
            }

            if (g_saved_game_files_name_counters[file_type] != number) {
                g_saved_game_files_name_counters[file_type] = number;
                g_saved_game_files_pending_dirty = true;
            }
        }
    }
}


static __forceinline bool saved_game_files_valid_handle(void *handle)
{
    return handle != 0 && handle != (void *)-1;
}

// @address 0x1ed3e0 saved_game_files_delete_callback
// its one caller (0x1e9f60) passes the source path, the callback and the priority/category constants
extern "C" long saved_game_files_delete_callback(const char *source_path, const char *destination_path, long, long, long,
    long, long, long, long, long, s_async_task *task, void *callback, c_synchronized_long *done,
    void **source_handle, void **destination_handle)
{
    s_file_reference source;
    s_file_reference destination;
    unsigned long error;
    e_async_completion (*work_callback)(s_async_task *) = saved_game_files_enumerate_from_content_catalogue_callback;
    *source_handle = 0;
    *destination_handle = 0;
    long result = -1;
    bool created = false;

    file_reference_create_from_path(&source, "cache1:\\sf_snippet.film", false);
    file_reference_create_from_path(&destination, destination_path, false);
    if (!file_exists(&destination)) {
        file_create_parent_directories_if_not_present(&destination);
        created = ::file_create(&destination);
    }
    if (file_open(&source, 1, &error)) {
        if (file_open(&destination, 2, &error)) {
            *source_handle = source.handle;
            if (*source_handle != 0) {
                *destination_handle = destination.handle;
                if (*destination_handle != 0) {
                    if (saved_game_files_valid_handle(*source_handle) && saved_game_files_valid_handle(*destination_handle)) {
                        result = ::async_task_add_ex(_async_priority_blocking_below_sound, task, _async_category_saved_games, work_callback,
                            done, true);
                        if (result != -1)
                            return result;
                    }
                }
            }
            file_close(&destination);
        }
        file_close(&source);
    }
    if (created)
        file_delete(&destination);
    return result;
}

extern "C" {
// @address 0x8c494c g_marketplace_active_controller_count
long g_marketplace_active_controller_count;
}


static __forceinline long saved_game_files_poll_2(s_controller_update *update, long pending)
{
            struct s_download_output { const char *data; long size; } output;
            output.data = 0;
            output.size = 0;

            long result = g_marketplace_http_downloader.get_data(&output.data, &output.size);
            if (result == 2)
                pending = 3;
            else if (result != 1)
                pending = 0;
            return pending;
        }

static __forceinline long saved_game_files_poll_3(s_controller_update *update, long pending)
{

            short task_state = g_marketplace_asset_enumeration_overlapped_task.get_task_state();
            if (task_state == _overlapped_task_state_success)
                pending = 4;
            else if (task_state == _overlapped_task_state_failure)
                pending = 0;
            return pending;
        }

static __forceinline long saved_game_files_poll_4(s_controller_update *update, long pending)
{
            struct s_download_output { long size; const char *data; } output;
            output.data = 0;
            output.size = 0;

            long result = g_marketplace_http_downloader.get_data(&output.data, &output.size);
            if (result == 2) {
                update->owns_asset = false;
                update->active_14 = false;
                pending = 0;
            } else if (result != 1)
                pending = 0;
            return pending;
        }

// @address 0x8ad68 ?saved_game_files_update_controller@@YAHPAUs_controller_update@@PAUc_controller_interface@@H@Z
// 0x8ad68
int saved_game_files_update_controller(s_controller_update *update, c_controller_interface *controller, int state)
{
    int next_state = state;
    bool signed_in = controller->is_signed_in_to_live() && !controller->is_guest();
    if (signed_in) {
        switch (state) {
        case 0:
            if (g_marketplace_active_controller_count == 0) {
                if (update->marketplace_state == 0)
                    next_state = 1;
                else if (update->state_10 == 0)
                    next_state = 5;
            }
            break;
        case 1:
            next_state = saved_game_files_handle_marketplace_assets((s_player_marketplace_user *)update, controller, signed_in, 1);
            break;
        case 2:
            next_state = saved_game_files_poll_2(update, 2);
            break;
        case 3:
            next_state = saved_game_files_poll_3(update, 3);
            break;
        case 4:
            next_state = saved_game_files_poll_4(update, 4);
            break;
        case 5:
            next_state = saved_game_files_parse_subscription_response(update, controller, signed_in, 5);
            break;
        }
    } else {
        update->active_14 = false;
        update->subscription_status = 0;
        update->offer_id = 0;
        update->marketplace_state = 0;
        update->state_10 = 0;
    }
    if (next_state == 0) {
        if (update->marketplace_state == 1)
            update->marketplace_state = 2;
        if (update->state_10 == 1)
            update->state_10 = 2;
    }
    return next_state;
}

// @address 0x1ea650 ?saved_game_files_enumerate_from_content_catalogue_callback@@YA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion saved_game_files_enumerate_from_content_catalogue_callback(s_async_task *task)
{
    s_saved_game_files_copy_task *enumerate_task = (s_saved_game_files_copy_task *)task;
    e_async_completion result;
    s_saved_game_item_metadata_packed metadata;
    s_file_reference file;
    long state = enumerate_task->state;
    long validate_flag = enumerate_task->validate;
    long last_state = enumerate_task->last_state;
    long catalogue_index = enumerate_task->catalogue_index;
    long content_item_index = enumerate_task->content_item_index;
    bool validate = validate_flag != 0;
    if (enumerate_task->state >= 0 && enumerate_task->state < 5) {
        result = async_copy_file_callback(task);
        if (result == 1) {
            long success = enumerate_task->done->value;
            __lwsync();
            if (success == 1) {
                enumerate_task->state = 5;
                enumerate_task->done->set(0);
                result = _async_completion_pending;
            }
        }
    } else {
        bool failed = false;
        if (state <= last_state) {
            failed = true;
            long file_type = -1;
            c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[catalogue_index];
            if (content_catalogue_has_storage_device(catalogue) && content_item_index != -1) {
                if (validate) {
                    e_game_content_type content_type = content_catalogue_get_game_content_type(catalogue, content_item_index);
                    file_type = game_content_type_to_saved_game_file_type(content_type);
                    if ((unsigned long)file_type <= 13 &&
                        ::c_start_menu_game_editor::load_progress_ui_for_catalogue((c_start_menu_game_editor *)catalogue,
                            (e_controller_index)content_item_index, (s_file_reference *)&file) &&
                        content_catalogue_update_item_metadata(catalogue, content_item_index))
                    {
                        memset(&metadata, 0, sizeof(metadata));
                        if (content_catalogue_item_metadata_matches(catalogue, content_item_index, &metadata)) {
                            content_catalogue_get_item_display_name(catalogue, content_item_index, metadata.name, 16);
                            if (content_catalogue_set_item_metadata(catalogue, content_item_index, &metadata)) {
                                if (((bool (*)(s_file_reference *, const s_saved_game_item_metadata_packed *, int,
                                        int))saved_game_write_metadata_to_file)(&file, &metadata, content_type, 1))
                                    failed = false;
                            }
                        }
                    }
                } else {
                    if (content_catalogue_update_item_metadata(catalogue, content_item_index))
                        failed = false;
                }
                content_catalogue_close_item(catalogue, content_item_index, true);
            }
            if (!failed && file_type == 11 && ((bool (*)(char *, long, long))saved_game_files_find_latest_film_path)((char *)&metadata, 256, 0))
                file_delete(file_reference_create_from_path(&file, (char *)&metadata, false));
        }
        if (failed)
            enumerate_task->done->set(0);
        enumerate_task->done->set(!failed);
        result = _async_completion_done;
    }
    if (result == 1 && enumerate_task->buffer)
        overlapped_free(enumerate_task->buffer);
    return result;
}


// @address 0x1e7fc0 saved_game_files_create_content_item_file
extern "C" bool saved_game_files_create_content_item_file(long controller_index, e_game_content_type content_type,
    const wchar_t *display_name, const char *file_name, s_file_reference *reference, long *out_content_item_index)
{
    long content_item_index = -1;
    bool success = false;
    bool unknown_flag = false;
    c_content_catalogue *catalogue = &content_catalogues().catalogues[controller_index];
    long maximum_size = 0;
    wchar_t name[128];
    saved_game_files_get_file_type_display_name(game_content_type_to_saved_game_file_type(content_type), name, 128);
    if (content_type == 2) {
        unknown_flag = true;
        maximum_size = 0xa00000;
    }
    if (!saved_game_files_requests_blocked() || catalogue->last_content_index == -1) {
        content_item_index = content_catalogue_open_or_create_item(
            (c_content_catalogue *)catalogue, display_name, name, content_type, unknown_flag, maximum_size, 0);
        if (content_item_index != -1) {
            char path[256];
            if (content_catalogue_get_item_container_path((c_content_catalogue *)catalogue, content_item_index, path, 256, 1)) {
                csstrnzcat(path, file_name);
                file_reference_create_from_path(reference, path, false);
                success = true;
            }
        }
    }
    if (out_content_item_index)
        *out_content_item_index = content_item_index;
    return success;
}


// @address 0x1e9f60 saved_game_files_copy_screenshot
extern "C" long saved_game_files_copy_screenshot(
    long catalogue_index, long content_type, const wchar_t *display_name, const char *source_file_name, c_synchronized_long *done,
    c_synchronized_long *completion, long *out_content_item_index)
{
    long result = -1;
    s_controller *controller = &controller_globals().controllers[catalogue_index];
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller - controller_globals().controllers];
    unsigned long size = 0x10000;
    void *buffer = ::overlapped_malloc_tracked(size, (char const *)(0), 0);
    while (buffer == 0 && size >= 0x1000) {
        size >>= 1;
        buffer = ::overlapped_malloc_tracked(size, (char const *)(0), 0);
    }
    unsigned long buffer_size = buffer != 0 ? size : 0;
    done->set(0);
    completion->set(0);
    if (buffer != 0) {
        char path[256];
        if (content_catalogue_has_storage_device(catalogue) && content_catalogue_get_remaining_slots(catalogue, 13, 1) > 0) {
            e_saved_game_file_type file_type = game_content_type_to_saved_game_file_type(_game_content_type_clip);
            long content_item_index;
            {
                wchar_t name[128];
                saved_game_files_get_file_type_display_name(file_type, name, 128);
                content_item_index = content_catalogue_new_item(catalogue, display_name, name, 13, 1, 0, true);
            }
            if (content_item_index != -1 && content_catalogue_get_item_container_path(catalogue, content_item_index, path, 256, 1)) {
                csnzappendf(path, 256, (const char *)::saved_game_files_get_saved_game_file_name(file_type));
                s_saved_game_files_copy_task task;
                memset(&task, 0, sizeof(task));
                task.state = 0;
                task.buffer = (char *)buffer;
                task.buffer_size = buffer_size;
                task.done = done;
                task.last_state = 5;
                task.catalogue_index = catalogue_index;
                task.content_item_index = content_item_index;
                task.validate = 1;
                result = saved_game_files_delete_callback("cache1:\\sf_snippet.film", path, 0, 1, 0, 0, 1, 2, 8, 5,
                    (s_async_task *)&task, (void *)saved_game_files_enumerate_from_content_catalogue_callback,
                    completion, &task.source_handle, &task.destination_handle);
                *out_content_item_index = content_item_index;
            }
        }
        if (result == -1)
            overlapped_free(buffer);
    }
    return result;
}


// @address 0x4e0d30 saved_game_files_write_screenshot
extern "C" long saved_game_files_write_screenshot(s_saved_game_files_write_buffer_task *task)
{
    bool success = false;
    bool done = false;
    if (task->state == 0)
    {
        long controller_index = task->controller_index;
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
        if (content_catalogue_has_storage_device(catalogue))
        {
            saved_game_files_find_unique_name(controller_index, 13, task->display_name, 16);
            if (saved_game_files_create_content_item_file(task->controller_index, _game_content_type_shot, task->display_name,
                (const char *)::saved_game_files_get_saved_game_file_name(_saved_game_file_type_shot), task->file, task->content_item_index) &&
                    *task->content_item_index != -1 &&
                ((s_content_item *)catalogue)->make_ready_for_access(*task->content_item_index, true) &&
                    ::file_create((s_file_reference *)task->file))
            {
                content_catalogue_flush_item(catalogue, *task->content_item_index, true);
                unsigned long error;
                if (file_open(task->file, 2, &error))
                {
                    task->state = 1;
                    success = true;
                }
            }
        }
    }
    else if (task->state == 1)
    {
        if (file_write(task->file, task->buffer_size, task->buffer))
        {
            success = true;
            done = true;
        }
        file_close(task->file);
    }
    if (done || !success)
    {
        long content_item_index = *task->content_item_index;
        if (content_item_index != -1)
        {
            c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[task->controller_index];
            if (content_catalogue_has_storage_device(catalogue))
            {
                content_catalogue_close_item(catalogue, content_item_index, true);
                if (!success)
                    content_catalogue_delete_item(catalogue, *task->content_item_index, true);
            }
        }
    }
    task->success->set(success && done);
    return done || !success;
}

// @address 0x1e9e28 ?saved_game_files_create_saved_game_file_reference@@YA_NW4e_controller_index@@PB_WPBDPAUs_file_reference@@_N4@Z
bool saved_game_files_create_saved_game_file_reference(
    enum e_controller_index p0, wchar_t const *p1, char const *p2, struct s_file_reference *p3, bool p4, bool p5)
{
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[p0];
    long content_item_index = catalogue->try_and_get_item_by_type_and_display_name(p1);
    bool success = false;
    if (content_item_index == -1)
        content_item_index = content_catalogue_new_item(catalogue, p1, 0, 2, 1, 0xa00000, true);
    if (content_item_index != -1)
    {
        char path[256];
        if (content_catalogue_get_item_container_path(catalogue, content_item_index, path, 256, 1))
        {
            csstrnzcat(path, p2);
            file_reference_create_from_path(p3, path, false);
            success = true;
        }
    }
    return success;
}


extern "C" {
// match: noalias declaration preserves caller live registers


}


// @address 0x228998 saved_game_files_write_map_autosave
extern "C" void saved_game_files_write_map_autosave(bool)
{
    game_globals_storage *game_globals = tls_game_globals();
    game_options *options = &game_globals->options;
    s_blffile_map_variant file_data;
    if (game_engine_sandbox_is_active())
    {
        s_game_engine_globals *engine_globals = game_engine_globals;
        if (!engine_globals->sandbox.variant_file_exists)
            return;
        engine_globals->map_info.save_to(&file_data.map_variant.map_variant);
        if (wcslen(file_data.map_variant.map_variant.m_metadata.name) == 0)
            game_engine_set_map_info_name(&file_data.map_variant.map_variant, options->map_variant.m_metadata.name);
    }
    else
    {
        options->map_variant.save_to(&file_data.map_variant.map_variant);
    }

    long count = 0;
    s_saved_game_queue_item items[25];
    if (!saved_game_collect_files(1, items, 0, 25, &count, 0))
        return;

    long newest_index = -1;
    for (long index = 0; index < count; index++)
    {
        if (newest_index == -1 || CompareFileTime((const FILETIME *)&items[newest_index].modification_date,
            (const FILETIME *)&items[index].modification_date) < 0)
            newest_index = index;
    }
    if (newest_index != -1 && items[newest_index].metadata.unique_id == map_variant_compute_unique_id(&file_data.map_variant.map_variant))
        return;

    c_static_string<256> path;
    memcpy(&file_data.get_metadata(), &file_data.map_variant.map_variant.m_metadata, sizeof(s_saved_game_item_metadata_packed));
    file_data.get_metadata().unique_id = map_variant_compute_unique_id(&file_data.map_variant.map_variant);
    file_data.get_metadata().unique_id = map_variant_compute_unique_id(&file_data.map_variant.map_variant);
    file_data.end_of_file.total_file_size = 0xE1D8;
    saved_game_files_get_autosave_path(_saved_game_file_type_map, &path);

    c_file_reference file(path.m_string, 2);
    s_saved_game_files_buffered_file_writer file_writer(&file);
    buffered_file_write(&file_writer.writer, &file_data, sizeof(file_data));
    file_writer.flush();
    if (file.m_open)
        file_close(&file);
}

// @address 0x3e674 k_saved_game_autosave_directory
