// blamlibxbox/game_preferences.obj: from the records 0x81500 0x84468 0x119cd8 0x119d60 0x191288 0x195908 0x1984d8 0x198780 0x19f298 0x19f2f0 0x19f340 0x19f3b0 0x19f408 0x19f488 0x19f5d8 0x19f660 0x19f6e8 0x19f780 0x19f818 0x19f938 0x19fb40 0x19fd78 0x4a2c08 0x4d6500 0x70b618 0x70b640 0x19fbd0 0x19fe28
// @flags /TP /O2 /J /Oc
#include "interface/gui_network_session_advertisement_types.h"
#include "multithreading/thread_tls.h"
#include "game/game_types.h"
#include "interface/gui_game_mode_types.h"
#include "cseries/async_helpers.h"
// @address 0x84468 global_preferences_get_game_setup_storage
// @address 0x191288 global_preferences_get_subtitle_setting
// @address 0x195908 global_preferences_get_brightness_setting
// @address 0x1984d8 shell_mount_utility_drive
// @address 0x198780 shell_initialize_utility_drive
// @address 0x119cd8 ?global_preferences_get_betrayal_count@@YAJXZ
// @address 0x19fb40 ?global_preferences_set_xbox_live_private_privacy_setting@@YAXW4e_gui_network_session_advertisement_mode@@@Z
// @address 0x19f298 ??0s_game_preferences_file_image@@QAA@XZ
// @address 0x19f2f0 ??0s_gui_game_setup_storage@@QAA@XZ
// @address 0x4d6500 ?global_preferences_get_last_game_setup@@YA_NJ@Z
// @address 0x119d60 ?global_preferences_get_ejection_count@@YAJXZ
// @address 0x19f6e8 ?global_preferences_set_betrayal_count@@YAXJ@Z
// @address 0x19f780 ?global_preferences_set_ejection_count@@YAXJ@Z
// @address 0x19f3b0 ??0c_thread_globals_lock@@QAA@XZ
// @address 0x19f408 ??1c_thread_globals_lock@@QAA@XZ
// @address 0x19f938 global_preferences_set_mode_setup
// @address 0x19fd78 global_preferences_reset_to_defaults
// @address 0x4a2c08 global_preferences_get_mode_setup

#include <ppcintrinsics.h>
#include <string.h>
#include "multithreading/synchronization.h"
#include "interface/user_interface_session.h"
#include "cache/security_functions.h"
#include "main/game_preferences.h"
#include "main/game_preferences_globals.h"
#include "networking/logic/network_life_cycle.h"
#include "main/main.h"
#include "saved_games/saved_film.h"
#include "main/loading.h"
#include "game/map_variant.h"
#include "cseries/async.h"
#include "game/game_variant.h"
#include "networking/session/network_session_parameters.h"
#include "input/input_xinput.h"
#include "cseries/file_handle_types.h"
#include "cseries/create_file_task_types.h"
#include "multithreading/threads.h"
#include "cseries/crt_xdk.h"
#include "shell/utility_drive.h"
// @address 0x582158 XMountUtilityDriveEx
// @address 0x5821a8 XUnmountUtilityDrive
// @address 0x5821e8 XMountUtilityDriveUseStfc
// @address 0x8a1ead g_utility_drive_mounted
#include "cseries/async_completion.h"
#include "main/game_preferences_inlines.h"
#include "multithreading/synchronized_value.h"
#include "multithreading/synchronization_types.h"

extern "C" {
// @address 0x8a1eae g_utility_drive_was_formatted
bool g_utility_drive_was_formatted;
}
struct s_game_preferences_controller_state {
    s_xinput_state previous_state;
    s_xinput_state state;
};

struct s_synchronization_globals;

#include "main/game_preferences_types.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "interface/gui_pregame_setup_manager_types.h"

typedef char preferences_mode_alignment[(__alignof(s_game_preferences_mode_settings) == __alignof(s_gui_game_setup_storage)) ? 1 : -1];
typedef char preferences_snapshot_size[(sizeof(s_game_preferences_snapshot) == sizeof(s_pregame_setup_params)) ? 1 : -1];

s_global_preferences_close_state g_global_preferences_close_state = { false, -1 };
// @address 0x8e3dd8 g_global_preferences
s_global_preferences g_global_preferences;

// @address 0x11df698 thread_global_preferences
#if !defined(_XBOX) || defined(__clang__)
__declspec(thread) s_global_preferences *thread_global_preferences;
#endif

// @address 0x81500 ?global_preferences_update@@YAXXZ
void global_preferences_update(void)
{
    c_thread_globals_lock lock(_inline_thread_globals_lock);
    if (global_preferences_thread_storage()->dirty)
        global_preferences_save();
    s_global_preferences *preferences = global_preferences_thread_storage();
    if (preferences->async_write_pending)
    {
        if (preferences->io_completion.peek() != 0)
        {
            g_global_preferences_close_state.requested = true;
            preferences->async_write_pending = false;
        }
    }
}

// 0x84468
extern "C" s_gui_game_setup_storage *global_preferences_get_game_setup_storage(void)
{
    s_gui_game_setup_storage *result = 0;
    bool skip = g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 3;
    if (!skip) {
        c_thread_globals_lock lock;
        result = &global_preferences_thread_storage()->primary_file.data.mode_settings;
    }
    return result;
}

// 0x119cd8
long global_preferences_get_betrayal_count()
{
    long result = 0;
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        result = global_preferences_thread_storage()->primary_file.data.betrayal_count;
    }
    return result;
}

// 0x119d60
long global_preferences_get_ejection_count(void)
{
    long result = 0;
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        result = global_preferences_thread_storage()->primary_file.data.ejection_count;
    }
    return result;
}

// 0x191288
int global_preferences_get_subtitle_setting(void)
{
    int result = 0;
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        result = global_preferences_thread_storage()->primary_file.data.subtitle_setting;
    }
    return result;
}

// 0x195908
int global_preferences_get_brightness_setting(void)
{
    int result = 2;
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        result = global_preferences_thread_storage()->primary_file.data.brightness_setting;
    }
    return result;
}

unsigned char shell_mount_utility_drive(unsigned char format)
{
    if (g_utility_drive_mounted) {
        XUnmountUtilityDrive();
        g_utility_drive_mounted = 0;
    }
    if (XMountUtilityDriveEx(XMountUtilityDriveUseStfc(0) | (format ? XMOUNTUTILITYDRIVE_FORMAT_ALL : 0),
                             0x10000, 0xff000) == 0) {
        if (XFlushUtilityDrive() == 0 && GetDiskFreeSpaceExA("cache0:\\", 0, 0, 0) != 0) {
            void *handle = CreateFileA("cache0:\\test.dat", 0x40000000, 0, 0, 2, 128, 0);
            if (handle != (void *)-1) {
                CloseHandle(handle);
                if (XFlushUtilityDrive() == 0)
                    g_utility_drive_mounted = 1;
            }
        }
        if (!g_utility_drive_mounted)
            XUnmountUtilityDrive();
    }
    return g_utility_drive_mounted;
}

// 0x198780
void shell_initialize_utility_drive(void)
{
    bool format = false;
    for (long user_index = 0; user_index < 4; user_index++) {
        s_game_preferences_controller_state controller_state;
        if (XInputGetState(user_index, &controller_state.state) == 0 && controller_state.state.buttons == 0x1001) {
            format = true;
            break;
        }
    }

    if (format || !shell_mount_utility_drive(0)) {
        if (shell_mount_utility_drive(1)) {
            g_utility_drive_was_formatted = true;
        }
    }

    if (g_utility_drive_mounted) {
        saved_film_delete_crash_reports();
    }
    const char *status = g_utility_drive_mounted ? "yes" : "no";
    main_set_status_value("using utility drive", status);
}

// 0x19f298
s_game_preferences_file_image::s_game_preferences_file_image()
{
}

// 0x19f2f0
s_gui_game_setup_storage::s_gui_game_setup_storage()
{
}

s_gui_game_setup_storage::s_multiplayer_settings::s_multiplayer_settings()
{
}

// 0x19f3b0
c_thread_globals_lock::c_thread_globals_lock()
{
    this->m_acquired = false;
    if (global_preferences_thread_storage() == 0) {
        internal_critical_section_enter(57);
        this->m_acquired = true;
        global_preferences_thread_storage() = &g_global_preferences;
    }
}

// 0x19f408
c_thread_globals_lock::~c_thread_globals_lock()
{
    if (this->m_acquired) {
        global_preferences_thread_storage() = 0;
        s_synchronization_globals *globals = &g_synch_globals;
        _InterlockedDecrement(&globals->critical_sections[57].lock_count.value);
        RtlLeaveCriticalSection(&globals->critical_sections[57].critical_section);
        this->m_acquired = false;
    }
}


// 0x19f488
void global_preferences_flush(void)
{
    c_thread_globals_lock lock(_inline_thread_globals_lock);
    void *handle = global_preferences_thread_storage()->file_handle;
    bool handle_valid = handle != 0 && handle != (void *)-1;
    if (handle_valid)
    {
        async_block_until_done(&global_preferences_thread_storage()->io_completion, simple_yield_function, 0, 0, 0, 0);
        global_preferences_update();
        async_block_until_done(&global_preferences_thread_storage()->io_completion, simple_yield_function, 0, 0, 0, 0);
        s_async_task task;
        void *file_handle = global_preferences_thread_storage()->file_handle;
        c_synchronized_long done(0);
        memset(&task, 0, sizeof(task));
        task.flush_file.handle = file_handle;
        async_task_add_ex(_async_priority_blocking_below_sound, &task,
            _async_category_saved_games, async_flush_file_callback, &done, true);
        async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
        g_global_preferences_close_state.requested = true;
        global_preferences_thread_storage()->async_write_pending = false;
    }
}

// @address 0x19f5d8 global_preferences_set_font_language
void global_preferences_set_font_language(long value)
{
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        s_global_preferences *preferences = global_preferences_thread_storage();
        preferences->primary_file.data.font_language = value;
        preferences->dirty = true;
    }
}

// @address 0x19f660 global_preferences_get_font_language
int global_preferences_get_font_language(void)
{
    int result = -1;
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        result = global_preferences_thread_storage()->primary_file.data.font_language;
    }
    return result;
}

// 0x19f6e8
void global_preferences_set_betrayal_count(long value)
{
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        volatile s_global_preferences *preferences = global_preferences_thread_storage();
        preferences->dirty = preferences->primary_file.data.betrayal_count != value;
        preferences->primary_file.data.betrayal_count = value;
    }
}

// 0x19f780
void global_preferences_set_ejection_count(long value)
{
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        volatile s_global_preferences *preferences = global_preferences_thread_storage();
        preferences->dirty = preferences->primary_file.data.ejection_count != value;
        preferences->primary_file.data.ejection_count = value;
    }
}

// @address 0x19f818 global_preferences_set_last_menu_selection
void global_preferences_set_last_menu_selection(long value)
{
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        s_global_preferences *preferences = global_preferences_thread_storage();
        preferences->primary_file.data.last_menu_selection = value;
        preferences->dirty = true;
    }
}

// 0x19f938
extern "C" void global_preferences_set_mode_setup(int *p)
{
    s_game_preferences_snapshot *setup = (s_game_preferences_snapshot *)p;
    bool skip = g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 3;
    if (!skip) {
        c_thread_globals_lock lock;
        switch (setup->type) {
        case _gui_game_mode_campaign:
        {
            char *settings = (char *)global_preferences_thread_storage()
                ->primary_file.data.mode_settings.serialized_settings.campaign_settings;
            memcpy(settings, setup->data, sizeof(((s_game_preferences_mode_settings *)0)->campaign_settings));
            break;
        }
        case _gui_game_mode_multiplayer:
            memcpy(global_preferences_thread_storage()->primary_file.data.mode_settings.serialized_settings.multiplayer_settings, setup->data,
                sizeof(((s_game_preferences_mode_settings *)0)->multiplayer_settings));
            break;
        case _gui_game_mode_matchmaking:
        {
            // match: 0x19f938: retain the accepted restrict qualifier for the color copy.
            s_game_preferences_color *__restrict destination = &global_preferences_thread_storage()
                ->primary_file.data.mode_settings.serialized_settings.matchmaking_settings;
            *destination = setup->color;
            break;
        }
        case _gui_game_mode_mapeditor:
            memcpy(global_preferences_thread_storage()->primary_file.data.mode_settings.serialized_settings.map_editor_settings, setup->data,
                sizeof(((s_game_preferences_mode_settings *)0)->map_editor_settings));
            break;
        case _gui_game_mode_theater:
            memcpy(global_preferences_thread_storage()->primary_file.data.mode_settings.serialized_settings.theater_settings, setup->data,
                sizeof(((s_game_preferences_mode_settings *)0)->theater_settings));
            break;
        }
        global_preferences_thread_storage()->dirty = true;
    }
}

// 0x19fb40
void global_preferences_set_xbox_live_private_privacy_setting(e_gui_network_session_advertisement_mode privacy_setting)
{
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        s_global_preferences *preferences = global_preferences_thread_storage();
        preferences->primary_file.data.xbox_live_private_privacy_setting = privacy_setting;
        preferences->dirty = true;
    }
}

// @address 0x19fbd0 global_preferences_save
extern "C" void global_preferences_save(void)
{
    if (!global_preferences_thread_storage()->dirty)
        return;
    if (global_preferences_thread_storage()->io_completion.peek() == 0)
        return;
    if (memcmp(&global_preferences_thread_storage()->primary_file.data,
        &global_preferences_thread_storage()->shadow_file.data, sizeof(s_global_preferences_data)) != 0) {
        memcpy(&global_preferences_thread_storage()->shadow_file.data,
            &global_preferences_thread_storage()->primary_file.data, sizeof(s_global_preferences_data));
        s_network_http_request_hash hash;
        bool hash_ok = security_calculate_hash((const char *)&global_preferences_thread_storage()->shadow_file.data,
            sizeof(s_global_preferences_data), true, &hash);
        void *handle = global_preferences_thread_storage()->file_handle;
        unsigned int handle_valid = handle != 0 && handle != (void *)-1;
        if ((handle_valid & 0xff) && hash_ok) {
            memcpy(global_preferences_thread_storage()->shadow_file.hash.hash, hash.hash, sizeof(hash));
            s_async_task task;
            task.write_position.flags = 0;
            c_synchronized_long *done = &global_preferences_thread_storage()->io_completion;
            void *file_handle = global_preferences_thread_storage()->file_handle;
            memset(&task, 0, sizeof(task));
            task.write_position.handle = file_handle;
            task.write_position.offset = 0;
            task.write_position.buffer = global_preferences_thread_storage()->shadow_file.hash.hash;
            task.write_position.bytes_written = 0;
            task.write_position.flags = 1;
            task.write_position.unbuffered = true;
            task.write_position.size = sizeof(s_game_preferences_file_image);
            global_preferences_thread_storage()->async_task_id = async_task_add_ex(_async_priority_blocking_generic, &task,
            _async_category_saved_games, async_write_position_callback, done, true);
            global_preferences_thread_storage()->async_write_pending = true;
        }
    }
    global_preferences_thread_storage()->dirty = false;
}

// 0x19fd78
extern "C" void *global_preferences_reset_to_defaults(void)
{
    s_global_preferences *preferences_globals;
    void *cleared_shadow;
    preferences_globals = global_preferences_thread_storage();
    memset(&preferences_globals->primary_file, 0, sizeof(s_game_preferences_file_image));
    preferences_globals->primary_file.data.version = 34;
    preferences_globals->primary_file.data.last_language = -1;
    preferences_globals->primary_file.data.font_language = -1;
    preferences_globals->primary_file.data.map_count = 0;
    preferences_globals->primary_file.data.xbox_live_private_privacy_setting = _network_advertise_xbox_live_friends_only;
    long *subtitle = &preferences_globals->primary_file.data.subtitle_setting;
    *subtitle = 0;
    preferences_globals->primary_file.data.brightness_setting = 2;
    cleared_shadow = memset(&preferences_globals->shadow_file.data, 0, sizeof(s_global_preferences_data));
    preferences_globals->dirty = true;
    return cleared_shadow;
}

// 0x4a2c08
extern "C" void global_preferences_get_mode_setup(c_gui_pregame_setup_manager *manager, int type)
{
    const s_game_preferences_mode_settings &source = global_preferences_get_game_setup_storage()->serialized_settings;
    s_game_preferences_snapshot snapshot;
    snapshot.type = type;
    switch (type) {
    case _gui_game_mode_campaign: {
        int i;
        for (i = 0; i < 7; i++)
            snapshot.words[i] = source.campaign_words[i];
        break;
    }
    case _gui_game_mode_matchmaking:
        snapshot.color = source.matchmaking_settings;
        break;
    case _gui_game_mode_multiplayer:
        memcpy(snapshot.data, source.multiplayer_settings, sizeof(source.multiplayer_settings));
        break;
    case _gui_game_mode_mapeditor:
        memcpy(snapshot.data, source.map_editor_settings, sizeof(source.map_editor_settings));
        break;
    case _gui_game_mode_theater:
        memcpy(snapshot.data, source.theater_settings, sizeof(source.theater_settings));
        break;
    }
    manager->set_pending_setup(&snapshot);
}


// 0x4d6500
bool global_preferences_get_last_game_setup(long mode)
{
    bool leader;
    if (user_interface_squad_exists()) {
        leader = ::network_life_cycle_squad_local_peer_is_leader();
    } else {
        leader = true;
    }
    return leader && mode != k_gui_game_mode_count;
}

// @address 0x19fe28 ?global_preferences_init@@YAXXZ
void global_preferences_init(void)
{
    bool valid = false;
    s_network_http_request_hash hash;
    s_create_file_task create_task;
    s_async_task set_size_task;

    if (global_preferences_thread_storage()->initialized)
        return;
    global_preferences_thread_storage()->initialized = true;
    c_synchronized_long *sync = &global_preferences_thread_storage()->io_completion;
    s_file_handle volatile *file = (s_file_handle *)&global_preferences_thread_storage()->file_handle;

    memset(&create_task, 0, sizeof(create_task));
    strncpy(create_task.file_name, "cache0:\\preferences.dat", sizeof(create_task.file_name));
    create_task.file_name[sizeof(create_task.file_name) - 1] = 0;
    create_task.access = 0xc0000000;
    create_task.share_mode = 0;
    create_task.disposition = 4;
    create_task.flags = 0x8000000;
    create_task.file = file;
    create_task.create_directories = false;
    async_task_add_ex(_async_priority_blocking_below_sound, (s_async_task *)&create_task,
            _async_category_saved_games, async_create_file_callback, sync, true);
    async_block_until_done(&global_preferences_thread_storage()->io_completion, simple_yield_function, 0, 0, 0, 0);

    bool handle_valid = global_preferences_thread_storage()->file_handle != 0
        && global_preferences_thread_storage()->file_handle != (void *)-1;
    if (handle_valid)
    {
        s_game_preferences_file_image buffer;
        c_synchronized_long bytes_read;
        async_read_position(global_preferences_thread_storage()->file_handle,
            (void *)&buffer, sizeof(s_game_preferences_file_image), 0, 8, 9, &bytes_read,
            &global_preferences_thread_storage()->io_completion);
        async_block_until_done(&global_preferences_thread_storage()->io_completion, simple_yield_function, 0, 0, 0, 0);
        bool hash_ok = security_calculate_hash((const char *)&buffer.data.version, sizeof(s_global_preferences_data), true, &hash);
        valid = (buffer.data.version == 34);
        if (valid)
        {
            valid = hash_ok && memcmp(hash.hash, buffer.hash.hash, sizeof(hash.hash)) == 0;
            if (valid)
                valid = (bytes_read.peek() == sizeof(s_game_preferences_file_image));
        }
        if (valid)
        {
            if (buffer.data.last_language < 0 || buffer.data.last_language >= 12)
                buffer.data.last_language = -1;
            if (buffer.data.font_language < 0 || buffer.data.font_language >= 12)
                buffer.data.font_language = -1;
            if (buffer.data.subtitle_setting < 0 || buffer.data.subtitle_setting >= 3)
                buffer.data.subtitle_setting = 0;
            if (buffer.data.brightness_setting < 0 || buffer.data.brightness_setting >= 5)
                buffer.data.brightness_setting = 2;
            if (buffer.data.xbox_live_private_privacy_setting != 0
                && buffer.data.xbox_live_private_privacy_setting != 1
                && buffer.data.xbox_live_private_privacy_setting != 2)
                buffer.data.xbox_live_private_privacy_setting = _network_advertise_xbox_live_friends_only;
            volatile long *sync_value = &global_preferences_thread_storage()->io_completion.value;
            __lwsync();
            _InterlockedExchange(sync_value, 1);
            memcpy(&global_preferences_thread_storage()->primary_file, &buffer, sizeof(s_game_preferences_file_image));
            memcpy(&global_preferences_thread_storage()->shadow_file.data,
                &global_preferences_thread_storage()->primary_file.data, sizeof(s_global_preferences_data));
        }
    }
    if (!valid)
    {
        global_preferences_reset_to_defaults();
        bool handle_valid2 = global_preferences_thread_storage()->file_handle != 0
            && global_preferences_thread_storage()->file_handle != (void *)-1;
        if (handle_valid2)
        {
            global_preferences_flush();
            void *file_handle = global_preferences_thread_storage()->file_handle;
            sync = &global_preferences_thread_storage()->io_completion;
            memset(&set_size_task, 0, sizeof(set_size_task));
            set_size_task.set_file_size_request.handle = file_handle;
            set_size_task.set_file_size_request.size = sizeof(s_game_preferences_file_image);
            set_size_task.set_file_size_request.success_out = 0;
            global_preferences_thread_storage()->async_task_id =
                async_task_add_ex(_async_priority_blocking_generic, &set_size_task,
            _async_category_saved_games, set_file_size_callback, sync, true);
        }
    }
}
