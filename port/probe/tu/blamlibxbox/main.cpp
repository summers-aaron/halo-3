// blamlibxbox/main.obj: from the records 0x80278 0x84820 0x84af0 0xa3ea0 0x126b38 0x13f858 0x13f958 0x141ea0 0x142720 0x192420 0x1a8388 0x1a8398 0x1a83e8 0x1a8480 0x1a8838 0x1a8a08 0x1a8ed8 0x1a9678 0x1a9790 0x1a9940 0x1a9a68 0x1a9c08 0x1a9c70 0x1a9cd0 0x1a9da8 0x1a9e28 0x1a9f50 0x1a9fd8 0x1aa040 0x1aa0f0 0x1aa1d0 0x1aa240 0x1aa2e8 0x1aa350 0x1aa498 0x1aab08 0x1aac60 0x1aad00 0x1aade0 0x1aae88 0x1aaec0 0x1aaf60 0x1aaf78 0x1acce8 0x1acf10 0x1ad0f0 0x1ad158 0x1ad2d8 0x1ad470 0x1b2f48 0x1ba700 0x1ba720 0x1ba740 0x1e4828 0x1e4900 0x1e4a28 0x1e4a40 0x1e4c00 0x1e4ca8 0x1e4e30 0x1e7420 0x1f0630 0x211690 0x211810 0x211ab8 0x2197d0 0x2261a8 0x2263b8 0x2a7050 0x2a7188 0x2a74f0 0x2a7678 0x2a83c8 0x2a84e8 0x2a8798 0x2a8a60 0x2a8cc0 0x2a8da0 0x2a8e70 0x2a8f10 0x2a9058 0x2a9198 0x2a9308 0x2a9458 0x2a95a0 0x2a9708 0x2a9798 0x2a9898 0x2a9938 0x2a9b70 0x2a9e58 0x2a9f28 0x2aa040 0x2ff870 0x2ff8d8 0x44a9d8 0x46b000
// @address 0x81ac8 cache_files_windows_idle
// @address 0x81dc8 cache_files_windows_process_loading_requests
// @address 0x11401e8 g_controller_globals
// @address 0x808560 g_simulation_globals
// @address 0x8a1ed4 g_active_designer_zone_mask
// @address 0x8a1ed8 g_active_cinematic_zone_mask
// @address 0x94e3b0 g_saved_film_timer_globals
// @address 0x96e760 g_game_state_procs
// @address 0x11df680 game_globals
// @address 0x11df6dc x_persistent_user_data
// @address 0x11df75c g_chud_scripting_globals
// @address 0x11df878 g_object_message_queue
#include "simulation/simulation_watcher_types.h"
// @address 0x73e318 g_widget_types
// @address 0x8c29c0 g_saved_film_playback
// @address 0x1142940 g_window_manager
#include "render/water_interaction_tls.h"
#include "memory/game_state_globals.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
// @flags /TP /O2 /Oz
#include "render/camera_fx_globals_types.h"
#include "objects/objects_tracked_object_types.h"
#include "main/main_debug_upload.h"
// @split_bss
// @sparse_bss 0x828c29c0..0x828c4928
#include <xtl.h>
#include "memory/physical_memory_map_types.h"
#include "effects/player_effect_tls.h"
#include "cseries/crt_xdk.h"
#include "text/unicode_inline.h"
#include <time.h>
#include "memory/static_arrays.h"
#include <stdio.h>
#include "cseries/cseries_wrapped.h"
#include "game/game_time_types.h"
#include "main/game_tick_time_samples.h"
#include <math.h>
#include "shell/shell.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
// @address 0x1e4c00 main_crash_set_user_input
// @address 0x80278 ?publish_waiting_gamestate@@YAXXZ
// @address 0x192420 ?unlock_resources_and_resume_render_thread@@YAXK@Z
// @address 0x1a8398 ?main_thread_combine_timing_data@@YAXPAX@Z
// @address 0x2261a8 ?__tls_set_g_physics_constants_allocator@@YAXPAX@Z
// @address 0x44a9d8 ?__tls_set_g_main_render_timing_data_allocator@@YAXPAX@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x1ad2d8 ??0c_main_crash_report_path@@QAA@XZ
// @address 0x1a9f50 ?main_switch_zone_set@@YAXJ@Z
// @address 0x47a860 ?chud_messaging_special_load@@YAX_N@Z
// @address 0x1aad00 ?main_startup_sequence@@YA_NXZ
// @address 0x142720 ?_internal_halt_render_thread_and_lock_resources@@YAKPBDJ@Z
// @address 0x84820 main_update_startup_menu_launch
// @address 0x84af0 main_has_pending_state_changes
// @address 0x1e7420 main_reset_saved_film_state
#include <stdarg.h>
#include <stddef.h>
#include <float.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "cseries/crt_xdk.h"
#include "multithreading/synchronization.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "game/players.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "simulation/simulation.h"
#include "sound/game_sound.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "camera/director.h"
#include "camera/observer.h"
#include "cseries/async.h"
#include "cseries/async_xoverlapped.h"
#include "game/game_time.h"
#include "game/player_control.h"
#include "game/player_rumble.h"
#include "input/input_abstraction.h"
#include "input/input_abstraction_types.h"
#include "input/input_xenon.h"
#include "interface/chud/chud.h"
#include "interface/chud/chud_manager.h"
#include "interface/chud/chud_messaging.h"
#include "interface/first_person_weapons.h"
#include "main/main.h"
#include "main/main_game.h"
#include "main/main_render.h"
#include "main/main_time.h"
#include "multithreading/threads.h"
#include "profiler/profiler_stopwatch.h"
#include "networking/network_globals.h"
#include "networking/online/online_achievements.h"
#include "networking/tools/network_webstats.h"
#include "networking/transport/transport.h"
#include "networking/transport/transport_security.h"
#include "rasterizer/rasterizer_globals.h"
#include "render/camera_fx_settings.h"
#include "saved_games/game_state.h"
#include "saved_games/game_state_procs_externs.h"
#include "saved_games/saved_film_snippet.h"
#include "saved_games/saved_game_files.h"
#include "scenario/scenario_interpolators.h"
#include "sound/sound_manager.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "text/font_cache.h"
#include "bink/bink_playback.h"
#include "game/game_engine.h"
#include "interface/c_controller.h"
#include "main/game_preferences.h"
#include "main/game_preferences_globals.h"
#include "objects/objects.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "rasterizer/xenon/rasterizer_xenon_main.h"
#include "render/render.h"
#include "saved_games/saved_film.h"
#include "scenario/scenario.h"
#include "cseries/upload_debug.h"
#include "game/campaign_metagame.h"
#include "tag_files/tag_resource_cache_external_access.h"
#include "tag_files/tag_resource_cache_paging.h"
#include "tag_files/tag_resource_cache_paging.h"
#include "cseries/cseries.h"
#include "cache/physical_memory_map.h"
#include "main/loading.h"
#include "main/loading_progress.h"
#include "networking/network_configuration.h"
#include "cseries/async_helpers.h"
#include "simulation/simulation_globals.h"
#include "main/player_profile_summary_types.h"
#include "interface/user_interface_session.h"
#include "scenario/scenario_bits_change_types.h"
#include "networking/online/online_lsp.h"
#include "cseries/data_mining_event_types.h"
#include "objects/widgets/widget_type.h"
#include "game/game_achievements_types.h"
#include "interface/c_player_profile.h"

enum e_controller_index;
enum e_window_index;
#include "interface/user_interface_messages.h"
#include "tag_files/string_ids.h"
#include "interface/gui_screens/campaign/campaign_select_difficulty_message.h"
#include <new>
struct game_state_header;
class c_restricted_memory;
#include "cache/restricted_memory.h"
class c_tag_resources_game_lock
{
public:
    c_tag_resources_game_lock() { m_lock = tag_resources_lock_game(); }
    ~c_tag_resources_game_lock() { tag_resources_unlock_game(m_lock); }
private:
    long m_lock;
};
class c_message;
struct s_file_reference;
struct D3DDevice;
#include "main/main_loop_globals.h"
#include "main/main_internal.h"
#include "interface/gui_screens/sandbox_menu/gui_screen_sandbox_menu.h"
extern "C" {


#include "main/main_global_declarations.h"


// match: 0x59ffd0: SDK header adoption is blocked by this unit's current types/API arguments: '<unnamed-tag>' : 'enum' type redefinition.

#include "cseries/xenon_platform_api.h"

}

struct s_sound_location;
#include "physics/physics_constants_types.h"
#include "physics/physics_constants_tls.h"
#include "render/camera_fx_globals.h"
#include "interface/controller_types.h"

// @address 0x11df6d0 g_main_gamestate_timing_data
__declspec(thread) s_game_tick_time_samples *g_main_gamestate_timing_data;
// @address 0x11df7d8 g_main_render_timing_data
__declspec(thread) s_game_tick_time_samples *g_main_render_timing_data;

#include "cache/restricted_memory_layout.h"

extern "C" {

}
class c_restricted_memory;
#include "cache/restricted_memory_regions.h"

#include "tag_files/file_reference.h"
extern "C" {
#include "bink/bink_playback_globals.h"


#include "bink/bink_playback_requests.h"

}



#include "main/main_runtime_globals.h"


#include "main/main_flag_state.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_definitions.h"

extern "C" {


}
class c_interlocked_long;
struct scenario;

// The added functions have no surviving dev-build names or paired dev bodies.

#include "game/game_options.h"


#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_cache_new.h"
#include "cseries/bit_operations.h"
#include "game/game_state_accessors.h"
#include "saved_games/saved_film_recording_types.h"

// Hoisted file-scope includes and TU-private types; original relative order kept.
// Delay-load failure codes the XDK does not name: SEVERITY_ERROR | FACILITY_VISUALCPP | ERROR_MOD_NOT_FOUND (126) / ERROR_PROC_NOT_FOUND (127).
enum e_vc_delay_load_exception {
    _vc_exception_delayload_module_not_found = 0xc06d007e,
    _vc_exception_delayload_proc_not_found = 0xc06d007f
};

extern "C" {
#include "main/main_resource_demand_processing.h"

}

#include "tag_files/tag_resource_cache_controller_demand_collector.h"

#include "main/main_render_resources.h"

#include "rasterizer/rasterizer.h"

#include "rasterizer/rasterizer_main_globals.h"

#include "sound/game_sound_message_queue.h"

#include "multithreading/synchronization_types.h"

#include "cseries/async_helpers_internal.h"

#include "bink/bink_playback_types.h"

#include "cache/cache_files_windows.h"

#include "main/main_crash_report.h"

#include "main/main_render_types.h"

#include "main/main_loop_dependencies.h"

#include "networking/network_configuration_types.h"

#include "main/main_crash_report_progress.h"

#include "main/main_crash_support.h"

struct s_main_crash_report_writer
{
    s_file_reference *file;
    char *buffer;
    long buffer_size;
    long buffer_position;
    char m_buffer[4096];
};

#include "physics/physics_constants.h"

#include "cseries/minidump_streams.h"

#include "cseries/minidump_interfaces.h"
#include "cseries/pointer_utilities.h"

struct s_main_block_section
{
    long offset;
    long size;
};

#include "cseries/minidump_directory_types.h"

struct s_main_writer_context
{
    EXCEPTION_POINTERS *exception_pointers;
    unsigned long thread_id;
};

struct s_main_block_writer
{
    c_minidump_file_output_interface *device;
    s_main_writer_context context;
    c_minidump_provider_interface *source;
    long mode;
    long byte_swap;
    void (*progress)(float percent);
    s_minidump_entry headers[8];
    s_main_block_section sections[64];
    unsigned long entry_count;
    long context_rva;
    unsigned long section_count;

    unsigned char write_section_memory64_descriptors();
};

struct s_main_stream_block
{
    unsigned __int64 first;
    unsigned int second;
    unsigned int third;
};

struct s_main_pair_header { __int64 first; unsigned __int64 second; };


union u_main_word_pair
{
    unsigned __int64 value;
    struct
    {
        unsigned int high;
        unsigned int low;
    } words;
};

#include "interface/controller_signin_cache.h"

#include "game/game_achievements.h"

#include "interface/c_window_manager.h"

#include "interface/c_gui_screen_widget.h"

#include "effects/player_effect_globals_types.h"

#include "saved_games/saved_film_globals.h"

#include "game/game_system_types.h"

#include "cseries/progress.h"

#include "effects/decal_message_types.h"

#include "objects/object_render_thread_message.h"

#include "multithreading/message_queue.h"

#include "main/main_time_globals.h"

#include "main/main_time_allocator.h"


#include "networking/network_update.h"

#include "networking/network_globals_types.h"

extern "C" {
#include "saved_games/game_state_runtime.h"

}

#if defined(_XBOX) && !defined(__clang__)
#include "interface/closed_caption_types.h"
#else
#include "interface/closed_caption.h"
#endif

#include "interface/user_interface_clock.h"

#include "simulation/simulation_update_types.h"

#include "simulation/simulation_update_metadata_types.h"

#include "game/game_time_inline.h"

#include "simulation/simulation_world_types.h"

#include "game/player_mapping_types.h"

#include "simulation/simulation_update.h"



#include "cache/cache_file_tag_resource_runtime_interface.h"

extern "C" {
#include "saved_games/saved_film_camera_states.h"

#include "cache/tag_resource_update.h"

}

#include "networking/online/minidump_file_output_secure.h"

#include "multithreading/threads_externs.h"

#include "cseries/retail_minidump_info_provider.h"

typedef bool (*t_main_region_callback)(void *);

union u_main_callback_storage
{
    t_main_region_callback callbacks[4];
    __int64 counter;
};

extern "C" {
#include "main/xenon_minidump_regions.h"
}

#include "cache/cache_file_memory_region.h"


#include "networking/online/online_lsp_manager.h"

#include "networking/tools/http_client.h"

#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"

#include "networking/tools/network_webstats_globals.h"

#include "networking/transport/transport_globals.h"

#include "networking/online/online_guide.h"

#include "multithreading/thread_system_globals.h"

#include "multithreading/thread_definitions.h"

struct s_main_delay_load_information
{
    unsigned long size;
    void *descriptor;
    void *function;
    const char *library_name;
};



static __forceinline bool main_request_bit_is_set(unsigned int flags, int bit_index)
{
    return (flags & (1U << bit_index)) != 0;
}
static __forceinline void main_request_set_bit(unsigned int &flags, int bit_index)
{
    flags |= 1U << bit_index;
}
static __forceinline unsigned long main_status_strnlen(const char *s, unsigned long maximum)
{
    unsigned long length = 0;
    while (length < maximum && *s++)
        ++length;
    return length;
}


static __forceinline bool main_render_thread_enabled()
{
    return g_render_thread_enabled.peek() == 1;
}
static __forceinline bool main_restricted_region_publish_to_mirror(long index)
{
    return g_restricted_regions[index].mirror_contents();
}

void publish_waiting_gamestate()
{
    if (main_render_thread_enabled() && !g_main_gamestate_timing_data->publishing_flags_empty()) {
        main_time_update_sync_milliseconds();
        if (main_restricted_region_publish_to_mirror(3)) {
            rasterizer_main_wake_render_thread();
            g_main_gamestate_timing_data->reset();
            ::main_render_purge_pending_messages();
            main_render_update_last_milliseconds();
        }
    }
}


extern "C" void main_update_startup_menu_launch(void)
{
    if (main_game_globals.status != 2 && main_game_globals.status != 6)
    {
        game_options options;
        if (main_game_globals.game_change_in_progress == 0)
        {
            main_menu_launch();
            return;
        }
        main_globals.startup_masking_pending = 0;
    }
}

extern "C" {

}

extern "C" unsigned char main_has_pending_state_changes(void)
{
    bool startup = false;
    if (main_game_globals.game_change_in_progress != 0)
        startup = true;
    do {
        if (main_globals.map_change_requested != 0) break;
        if (main_globals.save_requested != 0) break;
        if (main_globals.revert_requested != 0) break;
        if (main_globals.request58 != 0) break;
        if ((unsigned char)saved_film_is_update_pending() != 0) break;
        if (main_globals.play_requested != 0) break;
        if (game_state_globals.compressed.acquire_requested != 0) break;
        if (main_globals.stop_requested != 0) break;
        if (main_globals.active_flag != 0) break;
        if (main_globals.secondary_flag != 0) break;
        if (main_globals.reset_requested != 0) break;
        if (main_globals.shutdown_requested != 0) break;
        if (main_globals.request5 != 0) break;
        if (main_globals.request6 != 0) break;
        if (main_globals.campaign_progress_saved) break;
        if (main_globals.sync_requested != 0) break;
        if (main_globals.dirty != 0) break;
        bool busy = false;
        c_cache_file_tag_resource_runtime_manager *manager = g_geometry_globals.manager;
        if (manager != 0 && manager->m_active_zone_changed != 0)
            busy = true;
        if (busy)
            break;
        return startup;
    } while (0);
    return 1;
}



static __forceinline char *main_status_strncpy(char *destination, const char *source, unsigned int size)
{
    strncpy(destination, source, size);
    destination[size - 1] = 0;
    return destination;
}
static __forceinline int main_status_vsnprintf(char *buffer, unsigned int size, const char *format, va_list list)
{
    int result = _vsnprintf(buffer, size - 1, format, list);
    buffer[size - 1] = 0;
    return result;
}

struct s_tag_resource_paging_controller;
class c_tag_resource_page_table_control_interface;
struct s_main_resource_paging_cache;
extern "C" {


}
// @address 0x1180 ??_7c_tag_resource_cache_controller_demand_collector@@6B@


// @address 0x13f958 main_update_resource_demands
extern "C" void main_update_resource_demands(c_tag_resource_cache_new *globals, s_tag_resource_cache_external_access_tables *tables,
    c_tag_resource_demand_callback *demand_interface)
{
    if (!tag_resource_cache_is_loaded(&globals->m_page_table))
        return;
    c_tag_resource_page_table_control_interface *control_interface = (c_tag_resource_page_table_control_interface *)&globals->m_resource_address_cache;
    c_tag_resource_cache_controller *controller = &globals->m_controller;
    tag_resource_paging_controller_reset(controller, control_interface);
    bool active = true;
    long first_count = demand_interface->get_maximum_priority_count();
    long second_count = demand_interface->get_inactive_priority_count();
    long index = 0;
    while (true) {
        if (active ? index >= first_count : index >= second_count)
            break;
        c_tag_resource_cache_controller_demand_collector collector(globals->m_use_demand_policy ? tables : 0);
        demand_interface->demand_callback(index, &collector);
        if (globals->m_incremental_publishing) {
            c_wrapped_flags flags;
            flags.set_bits(collector.m_demanded_resources, 0xffff);
            main_render_process_demanded_resources((s_tag_resource_paging_controller *)controller, &flags, control_interface,
                &globals->m_page_io_listener_owner, &active);
        }
        index++;
    }
    if (controller->m_can_predict_optional) {
        globals->m_flag303 = true;
        controller->m_busy = false;
    } else {
        controller->m_busy = false;
    }
}

// @address 0xa3ea0 main_set_status_value
extern "C" void main_set_status_value(const char *status_type, const char *format, ...)
{
    long index = -1;
    char buffer[512];
    va_list list;
    va_start(list, format);
    if (!format)
        main_status_strncpy(buffer, "", sizeof(buffer));
    else
        main_status_vsnprintf(buffer, sizeof(buffer), format, list);
    for (unsigned long i = 0; i < 32; i++) {
        s_main_status_value *status_value = &g_status_values[i];
        if (strncmp(status_value->status_type, status_type, sizeof(status_value->status_type)) == 0) {
            index = i;
            break;
        }
    }
    if (format) {
        if (index == -1) {
            for (unsigned long i = 0; i < 32; i++) {
                s_main_status_value *status_value = &g_status_values[i];
                if (!main_status_strnlen(status_value->status_type, sizeof(status_value->status_type))) {
                    index = i;
                    main_status_strncpy(status_value->status_type, status_type, sizeof(status_value->status_type));
                    break;
                }
            }
        }
    }
    if (index != -1) {
        s_main_status_value *status_value = &g_status_values[index];
        if (format) {
            main_status_strncpy(status_value->status_value, buffer, sizeof(status_value->status_value));
        } else {
            main_status_strncpy(status_value->status_type, "", sizeof(status_value->status_type));
            main_status_strncpy(status_value->status_value, "", sizeof(status_value->status_value));
        }
    }
    va_end(list);
}


// match: 0x1aa498: the shipped game inlines c_rasterizer::setup_targets_simple (rasterizer_states.cpp in March;
// the tree has no out-of-line body yet); this is its body.
static __forceinline void main_setup_targets_simple()
{
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)13);
    c_rasterizer::set_render_target(1, c_rasterizer::_surface_none);
    c_rasterizer::set_render_target(2, c_rasterizer::_surface_none);
    c_rasterizer::set_render_target(3, c_rasterizer::_surface_none);
}

void unlock_resources_and_resume_render_thread(unsigned long flags)
{
    if ((flags & 1) != 0) {
        c_rasterizer::rasterizer_device_release_thread();
        restricted_region_unlock_primary(4);
        restricted_region_unlock_primary(2);
    }
    if ((flags & 2) != 0)
        g_render_thread_enabled.set_if_equal(1, 0);
    if ((flags & 3) != 0)
        rasterizer_main_wake_render_thread();
}

// @address 0x1a8388 main_set_render_timing_data
void main_set_render_timing_data(void *address)
{
    g_main_render_timing_data = (s_game_tick_time_samples *)address;
}

void main_thread_combine_timing_data(void *address)
{
    s_game_tick_time_samples *samples = (s_game_tick_time_samples *)address;
    g_main_gamestate_timing_data->accum(samples);
}


static __forceinline bool main_rasterizer_thread_owns_device()
{
    return g_thread_owning_device.peek() == get_current_thread_index();
}

#if !defined(_XBOX) || defined(__clang__)
// PC (tmp/probe/extra/high_hz_interpolation.cpp, GAME-CHANGES M2): above 30 frames a second, blends the published
// mirror's tick-driven poses between the previous tick and this one before it is drawn.
void render_interpolation_apply();
// PC (port/sdk/d3d/null_device.cpp): whether frames between ticks are drawn (HALO3_FPS above 30).
extern "C" unsigned long port_d3d_frames_between_ticks(void);
#endif

// @address 0x1a8480 ?process_published_game_state@@YAX_N@Z
void process_published_game_state(bool value)
{
    g_main_render_timing_data->accum(g_main_gamestate_timing_data);
    if (value && main_rasterizer_thread_owns_device()) {
#if !defined(_XBOX) || defined(__clang__)
        render_interpolation_apply();
#endif
        main_render();
        return;
    }
    main_render_process_messages();
}


extern "C" {
// @address 0x9e4c40 g_game_sound_message_queue
}

static __forceinline game_sound_render_message *main_reverse_pending_list(game_sound_render_message *list)
{
    game_sound_render_message *result = list;
    if (list)
    {
        game_sound_render_message *previous = 0;
        do
        {
            game_sound_render_message *next = list->next;
            list->next = previous;
            previous = list;
            list = next;
        } while (list);
        result = previous;
    }
    return result;
}

// @address 0x1a83e8 ?main_thread_process_pending_messages@@YAXXZ
void main_thread_process_pending_messages(void)
{
    process_game_engine_globals_messages();
    game_sound_render_message *list = (game_sound_render_message *)InterlockedFlushSList((slist_header_ptr)&g_game_sound_message_queue.pending_list);
    __lwsync();
    game_sound_render_message *entry = main_reverse_pending_list(list);
    while (entry)
    {
        ::unattached_impulse_sound_new(entry->handle, (s_sound_location const *)&entry->location, entry->value);
        game_sound_render_message *next = entry->next;
        __lwsync();
        InterlockedPushEntrySList((slist_header_ptr)&g_game_sound_message_queue.free_list, (slist_entry_ptr)entry);
        entry = next;
    }
}



struct s_synchronization_globals;

static __forceinline void main_m1_critical_section_leave(int lock)
{
    _InterlockedDecrement(&g_synch_globals.critical_sections[lock].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[lock].critical_section);
}

unsigned long _internal_halt_render_thread_and_lock_resources(char const * file, long line)
{
    int result = 0;
    if (g_render_thread_enabled.peek() != 2 && g_thread_system_initialized.peek() != 0 &&
        g_restricted_regions[2].initialized() && get_current_thread_index() != 2)
    {
        result = (g_render_thread_enabled.set_if_equal(0, 1) == 1) ? 2 : 0;
        if (result != 0)
            rasterizer_main_wake_render_thread();
        if (!g_restricted_address[2])
        {
            ::restricted_region_lock_primary(2);
            ::restricted_region_lock_primary(4);
            ::c_rasterizer::rasterizer_device_acquire_thread();
            result |= 1;
            if (g_thread_crashed[2].peek() != 1)
            {
                restricted_region_unlock_primary(3);
                c_restricted_memory *queue = &g_restricted_regions[3];
                if (queue->m_critical_section != -1)
                    internal_critical_section_enter(queue->m_critical_section);
                long count = queue->m_mirror_count;
                if (queue->m_critical_section != -1)
                    main_m1_critical_section_leave(queue->m_critical_section);
                for (long i = count; i != 0; i--)
                {
                    g_restricted_address[3] = (char *)queue->lock_mirror();
                    if (g_restricted_address[3])
                    {
                        int time = ::tag_resources_lock_game();
                        process_published_game_state(false);
                        ::tag_resources_unlock_game((*(long *)(&time)));
                        if (::restricted_region_mirror_locked_for_current_thread(3))
                        {
                            g_restricted_address[3] = 0;
                            queue->unlock_mirror();
                        }
                    }
                }
                ::restricted_region_lock_primary(3);
            }
        }
        if (get_current_thread_index() == 1 && g_restricted_address[1])
            main_thread_process_pending_messages();
    }
    return (unsigned long)(result);
}

// @address 0x1a9c08 main_set_idle_recursion
extern "C" void main_set_idle_recursion(unsigned char enter)
{
    bool main_thread = g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 1;
    if (!main_thread) return;
    if (enter) {
        main_globals.recursion_count++;
        return;
    }
    main_globals.recursion_count--;
}

extern "C" {


}

// @address 0x1a9c70 main_render_blank_frame
extern "C" void main_render_blank_frame(const wchar_t *message)
{
    unsigned long lock = ((int)::_internal_halt_render_thread_and_lock_resources("..\\main\\main.cpp", 1448));
    c_rasterizer::begin_frame();
    rasterizer_set_default_render_targets();
    main_render_draw_message_screen(L"");
    main_time_throttle(0);
    c_rasterizer::end_frame();
    unlock_resources_and_resume_render_thread(lock);
}

// @address 0x1a9cd0 main_set_designer_zone_active
extern "C" void main_set_designer_zone_active(int bit_index)
{
    unsigned int forbidden_bits;
    unsigned int required_bits;
    if (global_zone_set_index != -1) {
        const s_scenario_zone_set *zone_set = &((s_scenario_zone_set *)global_scenario->zone_sets.address)[global_zone_set_index];
        forbidden_bits = zone_set->designer_zone_forbidden_mask;
        required_bits = zone_set->designer_zone_required_mask;
    } else {
        forbidden_bits = 0;
        required_bits = 0;
    }
    unsigned int mask = 1U << bit_index;
    if ((mask & forbidden_bits) == 0 && (mask & required_bits) != 0 &&
        !main_request_bit_is_set(g_active_designer_zone_mask, bit_index)) {
        main_request_set_bit(main_globals.activate_bits, bit_index);
        main_globals.deactivate_bits &= ~mask;
        bool idle = main_globals.deactivate_bits == 0 && main_globals.activate_bits == 0 &&
                    main_globals.enable_bits == 0 && main_globals.disable_bits == 0;
        main_globals.dirty = !idle;
    }
}

// @address 0x1a9da8 ?main_set_cinematic_zone_inactive@@YAXH@Z
void main_set_cinematic_zone_inactive(int bit_index)
{
    if (((1 << bit_index) & g_active_cinematic_zone_mask) == 0) return;
    main_globals.enable_bits = main_globals.enable_bits | (1 << bit_index);
    main_globals.disable_bits = main_globals.disable_bits & ~(1 << bit_index);
    bool idle = main_globals.deactivate_bits == 0 && main_globals.activate_bits == 0 &&
                main_globals.enable_bits == 0 && main_globals.disable_bits == 0;
    main_globals.dirty = !idle;
}


// @address 0x1a9e28 main_set_cinematic_zone_active
extern "C" void main_set_cinematic_zone_active(int bit_index)
{
    unsigned int cinematic_zone_mask;
    if (global_zone_set_index != -1) {
        const s_scenario_zone_set *zone_set = &((s_scenario_zone_set *)global_scenario->zone_sets.address)[global_zone_set_index];
        cinematic_zone_mask = zone_set->cinematic_zones;
    } else {
        cinematic_zone_mask = 0;
    }
    if (((1U << bit_index) & cinematic_zone_mask) == 0) {
        for (int i = 0; i < global_scenario->zone_sets.count; i++) {
            const s_scenario_zone_set *zone_set = &((s_scenario_zone_set *)global_scenario->zone_sets.address)[i];
            if ((zone_set->cinematic_zones & (1U << bit_index)) != 0) {
                main_switch_zone_set(i);
                break;
            }
        }
    }
    if (((1U << bit_index) & g_active_cinematic_zone_mask) == 0) {
        main_globals.disable_bits = (1U << bit_index) | main_globals.disable_bits;
        main_globals.enable_bits = main_globals.enable_bits & ~(1U << bit_index);
        bool idle = main_globals.deactivate_bits == 0 && main_globals.activate_bits == 0 &&
                    main_globals.enable_bits == 0 && main_globals.disable_bits == 0;
        main_globals.dirty = !idle;
    }
}


void main_switch_zone_set(long zone_set_index)
{
    if (global_scenario == 0) return;
    if (zone_set_index < 0) return;
    if (zone_set_index >= global_scenario->zone_sets.count) return;
    bool load;
    if (scenario_zone_set_is_active(zone_set_index)) {
        if (main_globals.secondary_flag == 0) return;
        main_globals.secondary_flag = 0;
        main_globals.secondary_index = zone_set_index;
        load = false;
    } else {
        main_globals.secondary_flag = 1;
        main_globals.secondary_index = zone_set_index;
        load = true;
    }
    chud_messaging_special_load(load);
}

// @address 0x1a9fd8 ?main_set_active_zone_set@@YAXH@Z
void main_set_active_zone_set(int actor_index)
{
    if (global_scenario == 0) return;
    if (actor_index < 0) return;
    if (actor_index >= global_scenario->zone_sets.count) return;
    if (actor_index == global_zone_set_index) {
        if (main_globals.active_flag == 0) return;
        main_globals.active_flag = 0;
        main_globals.active_index = actor_index;
        return;
    }
    main_globals.active_flag = 1;
    main_globals.active_index = actor_index;
}


// @address 0x1aa1d0 ?main_switch_zone_set_private@@YAXXZ
void main_switch_zone_set_private()
{
    bool success = scenario_switch_zone_set(main_globals.secondary_index, true);
    chud_messaging_special_load(false);
    if (!success)
        main_game_load_panic();
    main_globals.secondary_flag = 0;
    main_globals.secondary_index = -1;
}

// @address 0x1aa2e8 ?main_reset_zone_requests@@YAXJ@Z
void main_reset_zone_requests(long zone_set_index)
{
    if (main_globals.active_flag != 0) {
        main_globals.active_flag = 0;
        main_globals.active_index = -1;
    }
    if (main_globals.secondary_flag != 0) {
        main_globals.secondary_flag = 0;
        main_globals.secondary_index = -1;
    }
    if (main_globals.dirty == 0) return;
    main_globals.dirty = 0;
    main_globals.deactivate_bits = 0;
    main_globals.activate_bits = 0;
    main_globals.enable_bits = 0;
    main_globals.disable_bits = 0;
}

// @address 0x1aa350 main_reset_global_requests
extern "C" void main_reset_global_requests(void)
{
    if (main_globals.map_change_requested) main_globals.map_change_requested = 0;
    if (main_globals.save_requested) main_globals.save_requested = 0;
    if (main_globals.revert_requested) main_globals.revert_requested = 0;
    if (main_globals.flush_requested) main_globals.flush_requested = 0;
    if (main_globals.play_requested) main_globals.play_requested = 0;
    if (main_globals.stop_requested) main_globals.stop_requested = 0;
    if (main_globals.active_flag) main_globals.active_flag = 0;
    if (main_globals.secondary_flag) main_globals.secondary_flag = 0;
    if (main_globals.reset_requested) main_globals.reset_requested = 0;
    if (main_globals.shutdown_requested) main_globals.shutdown_requested = 0;
    if (main_globals.game_change_requested) main_globals.game_change_requested = 0;
    if (main_globals.zone_change_requested) main_globals.zone_change_requested = 0;
    if (main_globals.sync_requested) main_globals.sync_requested = 0;
    if (main_globals.dirty) main_globals.dirty = 0;
    main_globals.deactivate_bits = 0;
    main_globals.activate_bits = 0;
    main_globals.enable_bits = 0;
    main_globals.disable_bits = 0;
    if (main_globals.pending_action) main_globals.pending_action = 0;
}

// @address 0x1aab08 main_encode_status_file
extern "C" void main_encode_status_file(s_file_reference *output)
{
    char buffer[256];
    if (output != 0) {
        const char *header = "fatal error detected, current status was:";
        file_write(output, strlen(header), header);
        file_write(output, 2, "\r\n");
    }
    for (unsigned long i = 0; i < 32; i++) {
        s_main_status_value *status_value = &g_status_values[i];
        if (main_status_strnlen(status_value->status_type, 256) != 0) {
            buffer[0] = 0;
            ((c_static_string<256> *)buffer)->print("    %s: %s", status_value->status_type, status_value->status_value);
            if (output != 0) {
                file_write(output, ((const c_static_string<256> *)buffer)->length(), buffer);
                file_write(output, 2, "\r\n");
            }
        }
    }
}

extern "C" {





















}

// @address 0x1acce8 main_get_exception_name
const char *main_get_exception_name(unsigned long status)
{
    const char *name = 0;
    switch (status) {
    case STATUS_DATATYPE_MISALIGNMENT: name = "EXCEPTION_DATATYPE_MISALIGNMENT"; break;
    case STATUS_BREAKPOINT: name = "EXCEPTION_BREAKPOINT"; break;
    case STATUS_SINGLE_STEP: name = "EXCEPTION_SINGLE_STEP"; break;
    case STATUS_ACCESS_VIOLATION: name = "EXCEPTION_ACCESS_VIOLATION"; break;
    case STATUS_ILLEGAL_INSTRUCTION: name = "EXCEPTION_ILLEGAL_INSTRUCTION"; break;
    case STATUS_NONCONTINUABLE_EXCEPTION: name = "EXCEPTION_NONCONTINUABLE_EXCEPTION"; break;
    case STATUS_ARRAY_BOUNDS_EXCEEDED: name = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"; break;
    case STATUS_FLOAT_DENORMAL_OPERAND: name = "EXCEPTION_FLT_DENORMAL_OPERAND"; break;
    case STATUS_FLOAT_DIVIDE_BY_ZERO: name = "EXCEPTION_FLT_DIVIDE_BY_ZERO"; break;
    case STATUS_FLOAT_INEXACT_RESULT: name = "EXCEPTION_FLT_INEXACT_RESULT"; break;
    case STATUS_FLOAT_INVALID_OPERATION: name = "EXCEPTION_FLT_INVALID_OPERATION"; break;
    case STATUS_FLOAT_OVERFLOW: name = "EXCEPTION_FLT_OVERFLOW"; break;
    case STATUS_FLOAT_STACK_CHECK: name = "EXCEPTION_FLT_STACK_CHECK"; break;
    case STATUS_FLOAT_UNDERFLOW: name = "EXCEPTION_FLT_UNDERFLOW"; break;
    case STATUS_INTEGER_DIVIDE_BY_ZERO: name = "EXCEPTION_INT_DIVIDE_BY_ZERO"; break;
    case STATUS_INTEGER_OVERFLOW: name = "EXCEPTION_INT_OVERFLOW"; break;
    case STATUS_PRIVILEGED_INSTRUCTION: name = "EXCEPTION_PRIV_INSTRUCTION"; break;
    case STATUS_FLOAT_MULTIPLE_FAULTS: name = "STATUS_FLOAT_MULTIPLE_FAULTS"; break;
    case STATUS_FLOAT_MULTIPLE_TRAPS: name = "STATUS_FLOAT_MULTIPLE_TRAPS"; break;
    case _vc_exception_delayload_module_not_found: name = "VCPPEXCEPTION_MODULE_NOT_FOUND"; break;
    case _vc_exception_delayload_proc_not_found: name = "VCPPEXCEPTION_PROC_NOT_FOUND"; break;
    }
    return name;
}

#if !defined(_XBOX) || defined(__clang__)
extern "C" char *getenv(const char *name); // the host C library's
#endif

// @address 0x1aac60 ?main_kick_startup_masking_sequence@@YAEE@Z
unsigned char main_kick_startup_masking_sequence(unsigned char use_alpha_texture)
{
    bool result = false;
    int movie_path[66];
#if !defined(_XBOX) || defined(__clang__)
    // PC: HALO3_SKIP_INTRO=1 (development launches) skips the opening movie, as when no masking movie is pending
    const char *skip_intro = getenv("HALO3_SKIP_INTRO");
    if (skip_intro && skip_intro[0] == '1')
        main_globals.startup_masking_pending = 0;
#endif
    if (main_globals.startup_masking_pending != 0)
    {
        *(unsigned char *)movie_path = 0;
        ((c_static_string<256> *)movie_path)->print("d:\\bink\\%s_%d.bik", "intro", 60);
        bink_playback_play_movie((char *)movie_path, -1, ((((unsigned char)use_alpha_texture != 0) ? -1 : 0) & 0x100) | (0xde & -0x101));
        result = true;
        main_globals.startup_masking_pending = 0;
    }
    main_globals.startup_masking_done = 1;
    return result;
}

// @address 0x8a1eb0 g_main_startup_movie_path
extern "C" {
char g_main_startup_movie_path[8];
}

struct _bink_globals;
struct s_input_abstraction_globals;

bool main_startup_sequence(void)
{
    bool result = false;
    if (main_globals.startup_masking_pending != 0)
    {
        if (::csstricmp(g_main_startup_movie_path, "xdemo") != 0)
        {
            bool bink_ready = bink_globals.active != 0 && bink_globals.enabled != 0;
            if (!bink_ready)
            {
                ::main_kick_startup_masking_sequence(0);
                input_abstraction_globals.last_update_milliseconds = GetTickCount();
                input_update();
                bink_globals.ready = 1;
            }
            result = bink_globals.active != 0 && bink_globals.enabled != 0;
        }
        input_abstraction_globals.last_update_milliseconds = GetTickCount();
        main_globals.startup_masking_pending = 0;
    }
    return result;
}

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *), void (*k_set_allocator_function)(void *)>
void *t_restricted_allocation_manager<k_region_index, k_adopt_function, k_post_copy_function, k_set_allocator_function>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    int member_index = g_restricted_regions[k_region_index].add_member(name, type, size, 0,
        k_region_index == 3 ? __tls_set_g_main_gamestate_timing_data_allocator : main_set_render_timing_data, k_adopt_function, k_post_copy_function);
    unsigned int offset = ((s_field_layout *)g_restricted_regions)[k_region_index].fields[member_index].offset;
    void *address = g_restricted_address[k_region_index] + offset;
    int owner = get_current_thread_index();
    m_thread_id = owner;
    m_member_index = member_index;
    if (k_region_index == 3)
        g_main_gamestate_timing_data = (s_game_tick_time_samples *)address;
    else
        g_main_render_timing_data = (s_game_tick_time_samples *)address;
    return g_restricted_address[k_region_index] + ((s_field_layout *)g_restricted_regions)[k_region_index].fields[member_index].offset;
}

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *), void (*k_set_allocator_function)(void *)>
void *t_restricted_allocation_manager<k_region_index, k_adopt_function, k_post_copy_function, k_set_allocator_function>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// @address 0x1aade0
template void *
t_restricted_allocation_manager<3, &main_thread_combine_timing_data, static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_gamestate_timing_data_allocator>::reserve_memory(
    const char *, const char *, unsigned int, long);
// @address 0x1aae88
template void *
t_restricted_allocation_manager<3, &main_thread_combine_timing_data, static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_gamestate_timing_data_allocator>::allocate(
    unsigned int, const char *);
// @address 0x1aaec0
template void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_render_timing_data_allocator>::reserve_memory(
    const char *, const char *, unsigned int, long);
// @address 0x1aaf60
template void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_render_timing_data_allocator>::allocate(unsigned int, const char *);

// @address 0x1aaf78 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_main_render_timing_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_render_timing_data_allocator>::deallocate(void *memory)
{
    int value = m_member_index;
    g_main_render_timing_data = 0;
    restricted_region_free_member(2, value);
}







extern "C" {




}

// @address 0x1acf10 main_crash_handle_exception
extern "C" void main_crash_handle_exception(long exception_code, s_thread_exception_record *exception_record)
{
    if (g_main_crash_recursion_lock)
        return;

    s_file_reference file;
    // match: 0x1acf10: the aggregate initializer replaces the byte store at stack offset 416 and the 271-byte tail clear with a 272-byte memset.
    unsigned char *file_bytes = reinterpret_cast<unsigned char *>(&file);
    file_bytes[0] = 0;
    memset(file_bytes + 1, 0, sizeof(file) - 1);
    char text[0x6000] = "";
    bool enabled = g_network_configuration.crash_reporting_enabled || g_main_crash_reporting_enabled;
    c_main_crash_report_path crash_report_path;

    g_main_crash_recursion_lock = true;
    if (enabled)
    {
        text[0] = 0;
        csstrnzcat(text, "\r\nbuilding crash report, please wait...", sizeof(text));
        main_crash_render_message(text);
    }
    input_clear_gamepads_rumble();
    crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.m_string, crash_report_path.m_folder.m_string,
        "minidump.sdmp");
    if (file_reference_create_from_path(&file, crash_report_path.m_path.m_string, false) && file_exists(&file))
        file_delete(&file);
    if (!g_fake_minidump_creation)
    {
        long mode = g_network_configuration.crash_minidump_mode;
        if (mode != 0 || g_main_crash_reporting_enabled)
        {
            unsigned long flags = 0;
            if (mode == 2)
                flags |= 2;
            else
                flags &= ~2;
            if (mode == 1)
                flags |= 1;
            else
                flags &= ~1;
            if (enabled)
                flags |= 4;
            else
                flags &= ~4;
            crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.m_string,
                crash_report_path.m_folder.m_string, "minidump.sdmp");
            main_write_crash_minidump(flags, crash_report_path.m_path.m_string, exception_code, exception_record);
        }
    }
    if (enabled)
    {
        text[0] = 0;
        csstrnzcat(text, "\r\nCompressing crash report, please wait...", sizeof(text));
        main_crash_render_message(text);
    }
}

// @address 0x1ad0f0 main_crash_render_message
extern "C" void main_crash_render_message(const char *text)
{
    if (text)
    {
        unsigned long lock = ((int)::_internal_halt_render_thread_and_lock_resources("..\\cseries\\cseries_windows_debug.cpp", 497));
        main_render_pregame(_main_pregame_frame_crash, text);
        c_rasterizer::end_frame();
        unlock_resources_and_resume_render_thread(lock);
    }
}

// @address 0x1ad158 main_crash_initialize_report_directory
extern "C" void main_crash_initialize_report_directory(void)
{
    s_file_reference file;
    c_main_crash_report_path crash_report_path;
    c_static_string<260> path;
    path.print("%s%s", crash_report_path.m_root.m_string, crash_report_path.m_folder.m_string);
    if (file_reference_create_from_path(&file, path, true) != 0 && !file_exists(&file))
    {
        file_create_parent_directories_if_not_present(&file);
        ::file_create(&file);
    }
}

c_main_crash_report_path::c_main_crash_report_path()
{
    clear();
    main_status_strncpy(m_root.m_string, "cache1:\\", 32);
    main_status_strncpy(m_folder.m_string, "crash_report\\", 32);
}

// @address 0x1ad470 ?print@?$c_static_string@$0EIE@@@QAAPBDPBDZZ
template <long k_maximum_count>
const char *c_static_string<k_maximum_count>::print(const char *format, ...)
{
    va_list list;
    va_start(list, format);
    main_status_vsnprintf(m_string, k_maximum_count, format, list);
    va_end(list);
    return m_string;
}
template const char *c_static_string<1156>::print(const char *format, ...);
extern template const char *c_static_string<256>::print(const char *format, ...);
extern template const char *c_static_string<260>::print(const char *format, ...);


// @address 0x1b2f48 main_loop_handle_loading_transition
extern "C" void main_loop_handle_loading_transition(void)
{
    main_update_idle_tasks();
}


// @address 0x96df30 g_main_crash_upload_progress



// @address 0x1e4828 main_crash_update_upload_progress
extern "C" void main_crash_update_upload_progress(long current, long total)
{
    g_main_crash_upload_progress.current.set(current);
    g_main_crash_upload_progress.total.set(total);
}





// @address 0x1e4900 main_crash_get_upload_status
extern "C" bool main_crash_get_upload_status(char *buffer, long buffer_size)
{
    if (buffer != NULL)
    {
        *buffer = '\0';
        if (g_main_crash_upload_progress.complete.peek() != 0)
        {
            if (g_main_crash_upload_progress.failed.peek() != 0)
            {
                csnzappendf(buffer, 1024,
                    "\r\nFile upload to server complete.  Visit http://www.bungie.net for more information.  (Safe to reboot)");
                return true;
            }
            else
            {
                csnzappendf(buffer, 1024,
                    "\r\nFile upload to server failed.  Please visit http://www.bungie.net and report this.  (Safe to reboot)");
                return true;
            }
        }
        else
        {
            float percent;
            if (g_main_crash_upload_progress.total.peek() == 0)
            {
                percent = 0.0f;
            }
            else
            {
                percent = ((float)g_main_crash_upload_progress.current.peek() /
                              (float)g_main_crash_upload_progress.total.peek()) *
                    100.0f;
            }
            csnzappendf(buffer, 1024, "\r\nUploading files to server, please wait... %i %%", (int)percent);
            return true;
        }
    }
    return true;
}



// @address 0x1e4a28 main_crash_set_user_input_finished
extern "C" void main_crash_set_user_input_finished(bool, void *)
{
    g_main_crash_report_globals.finished = true;
}









// @address 0x1e4ca8 main_crash_initialize_upload
extern "C" void main_crash_initialize_upload(void)
{
    c_main_crash_report_path crash_report_path;

    g_main_crash_upload_progress.active.set(1);
    g_main_crash_upload_progress.current.set(0);
    g_main_crash_upload_progress.total.set(100);

    progress_new("uploading crash files");
    unsigned long tick = GetTickCount();

    s_progress_globals *globals = &progress_globals;
    if (globals->currently_valid)
    {
        globals->last_percentage = 0;
        globals->last_update_time = tick;
        if (!globals->disable_callbacks)
        {
            if (globals->custom_callbacks.progress_update_proc != NULL)
            {
                globals->custom_callbacks.progress_update_proc(globals->description, 0, 0, globals->custom_callbacks.progress_data);
            }
            else if (globals->default_callbacks.progress_update_proc != NULL)
            {
                globals->default_callbacks.progress_update_proc(globals->description, 0, 0, globals->default_callbacks.progress_data);
            }
        }
    }

    if (g_network_configuration.crash_minidump_mode == 0)
    {
        crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.m_string,
            crash_report_path.m_folder.m_string, "crash_screenshot.dds");
        bink_playback_queue_file_request(NULL, crash_report_path.m_path.m_string, main_crash_update_upload_progress, progress_finish_crash_report, NULL);
    }
    else
    {
        crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.m_string,
            crash_report_path.m_folder.m_string, "minidump.sdmp");
        bink_playback_queue_file_request(
            NULL, crash_report_path.m_path.m_string, main_crash_update_upload_progress, main_crash_handle_minidump_upload, NULL);
    }
}

// @address 0x1e4e30 main_crash_handle_minidump_upload
extern "C" void main_crash_handle_minidump_upload(bool minidump_success, void *)
{
    if (minidump_success != 0)
    {
        c_main_crash_report_path crash_report_path;

        g_main_crash_upload_progress.current.set(0);
        g_main_crash_upload_progress.total.set(100);
        crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.m_string,
            crash_report_path.m_folder.m_string, "crash_screenshot.dds");
        bink_playback_queue_file_request(NULL, crash_report_path.m_path.m_string, main_crash_update_upload_progress, progress_finish_crash_report, NULL);
        return;
    }

    g_main_crash_upload_progress.failed.set(0);
    g_main_crash_upload_progress.active.set(0);
    g_main_crash_upload_progress.complete.set(1);
    progress_finish_crash_report(false, NULL);
}




// @address 0x1e4a40 main_crash_update_user_input_file
extern "C" int main_crash_update_user_input_file()
{
    bool success = false;
    unsigned long error = 0;
    s_file_reference reference;
    s_main_crash_report_writer writer;
    c_main_crash_report_path crash_report_path;

    writer.buffer = 0;
    writer.buffer_size = 0;
    writer.buffer_position = 0;
    crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.m_string, crash_report_path.m_folder.m_string,
        "crash_user_input.txt");
    file_reference_create_from_path(&reference, crash_report_path.m_path.m_string, false);
    bool opened = ::file_create(&reference) && file_open(&reference, 2, &error);

    writer.file = &reference;
    writer.buffer = writer.m_buffer;
    writer.buffer_size = sizeof(writer.m_buffer);
    writer.buffer_position = 0;
    g_main_crash_report_globals.user_input[1023] = 0;
    bool written = opened && buffered_file_write(&writer, g_main_crash_report_globals.user_input, strlen(g_main_crash_report_globals.user_input));
    success = written && buffered_file_writer_flush(&writer);
    file_close(&reference);
    return success;
}

extern "C" void main_crash_set_user_input(const char *source, long size)
{
    char *destination = g_main_crash_report_globals.user_input;
    strncpy(destination, source, size);
    g_main_crash_report_globals.user_input[size - 1] = 0;
    main_crash_update_user_input_file();
    c_main_crash_report_path crash_report_path;
    g_main_crash_report_globals.user_input_available = true;
    crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.m_string, crash_report_path.m_folder.m_string,
        "crash_user_input.txt");
    bink_playback_queue_file_request(NULL, crash_report_path.m_path.m_string, NULL, main_crash_set_user_input_finished, NULL);
}

void __tls_set_g_physics_constants_allocator(void *allocator)
{
    g_physics_constants = (s_physics_constants *)allocator;
}



// @address 0x2263b8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_physics_constants_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_physics_constants_allocator>::deallocate(
    void *memory)
{
    int value = m_member_index;
    g_physics_constants = 0;
    restricted_region_free_member(3, value);
}






typedef char main_writer_entry_count_offset[(sizeof(void *) != 4 || ((offsetof(s_main_block_writer, entry_count) == 0x27c))) ? 1 : -1];
typedef char main_writer_context_rva[(sizeof(void *) != 4 || ((offsetof(s_main_block_writer, context_rva) == 0x280))) ? 1 : -1];
typedef char main_writer_sections_offset[(sizeof(void *) != 4 || ((offsetof(s_main_block_writer, sections) == 0x7c))) ? 1 : -1];
typedef char main_writer_section_count_offset[(sizeof(void *) != 4 || ((offsetof(s_main_block_writer, section_count) == 0x284))) ? 1 : -1];



// @address 0x2a74f0 main_write_minidump_header
extern "C" bool main_write_minidump_header(s_main_block_writer *writer)
{
    bool result = false;
    if (writer->device->is_valid())
    {
        if (writer->source != NULL)
        {
            _MINIDUMP_HEADER header;
            memset(&header, 0, sizeof(header));
            writer->source->get_minidump_header(&header, 8);
            if (writer->mode == 3)
            {
                header.fields.flags.QuadPart = 2;
            }
            if (writer->byte_swap == 0)
            {
                unsigned long flags_high = header.words[7];
                unsigned long flags_low = header.words[6];
                unsigned long checksum = header.words[4];
                unsigned long stream_count = header.words[2];
                unsigned long signature = header.words[0];
                unsigned long directory_rva = header.words[3];
                unsigned long timestamp = header.words[5];
                unsigned long version = header.words[1];
                header.words[4] = _byteswap_ulong(checksum);
                header.words[2] = _byteswap_ulong(stream_count);
                header.words[0] = _byteswap_ulong(signature);
                header.words[3] = _byteswap_ulong(directory_rva);
                header.words[5] = _byteswap_ulong(timestamp);
                header.words[1] = _byteswap_ulong(version);
                header.fields.flags.QuadPart = (((unsigned __int64)_byteswap_ulong(flags_high)) << 32) | (unsigned int)_byteswap_ulong(flags_low);
            }
            result = writer->device->write(&header, sizeof(header));
        }
    }
    return result;
}

static __forceinline void main_swap_first(unsigned int *p)
{
    if (p)
    {
        unsigned int z = _byteswap_ulong(p[2]);
        unsigned int y = _byteswap_ulong(p[1]);
        p[0] = _byteswap_ulong(p[0]);
        p[2] = z;
        p[1] = y;
    }
}

static __forceinline void main_swap_last(unsigned int *p)
{
    if (p)
    {
        unsigned int x = _byteswap_ulong(p[0]);
        unsigned int y = _byteswap_ulong(p[1]);
        p[2] = _byteswap_ulong(p[2]);
        p[0] = x;
        p[1] = y;
    }
}

// @address 0x2a7678 main_write_minidump_directory
extern "C" bool main_write_minidump_directory(s_main_block_writer *writer)
{
    bool result = false;
    if (writer->device->is_valid() && writer->source != NULL)
    {
        result = true;
        for (long index = 0; result && index < 8; ++index)
        {
            unsigned int *header = (unsigned int *)&writer->headers[index];
            if (writer->byte_swap == 0)
                main_swap_first(header);
            result = writer->device->write(header, 12);
            if (writer->byte_swap == 0)
                main_swap_last(header);
        }
    }
    return result;
}

// @address 0x2a8cc0 main_write_section_memory_descriptors
extern "C" unsigned char main_write_section_memory_descriptors(s_main_block_writer *writer, long offset)
{
    bool result = false;
    s_main_stream_block block;
    if (writer->device->is_valid())
    {
        if (writer->source != NULL)
        {
            result = true;
            unsigned long index = 0;
            long q0 = 0;
            long q1 = 0;
            long q2 = 0;
            long *section_size = &writer->sections[0].size;
            memset(&block, 0, sizeof(block));
            do
            {
                if (index >= writer->section_count) break;
                block.third = offset;
                block.second = *section_size;
                block.first = section_size[-1];
                if (writer->byte_swap == 0)
                    main_encode_stream_block(&block);
                result = writer->device->write(&block, 16);
                q0 = offset;
                q1 = offset;
                q2 = offset;
                offset += *section_size;
                section_size += 2;
                index++;
            } while (result);
        }
    }
    return result;
}


// @address 0x2a8da0 ?write_section_memory64_descriptors@s_main_block_writer@@QAAEXZ
unsigned char s_main_block_writer::write_section_memory64_descriptors()
{
    unsigned int i = 0;
    bool result = false;
    if (device->is_valid() && source != 0)
    {
        result = true;
        long *pair = &sections[0].size;
        s_main_pair_header header;
        memset(&header, 0, sizeof(header));
        do
        {
            if (i >= section_count) break;
            int first = (int)pair[-1];
            header.second = (unsigned long)*pair;
            header.first = first;
            if (byte_swap == 0)
            {
                main_encode_pair_entry((unsigned __int64 *)&header);
            }
            result = device->write(&header, 16);
            i++;
            pair += 2;
        } while (result);
    }
    return result;
}

// @address 0x2a8e70 main_write_section_data
extern "C" bool main_write_section_data(s_main_block_writer *writer)
{
    bool success = false;
    if (writer->device->is_valid() && writer->source) {
        success = true;
        for (unsigned long i = 0; success && i < writer->section_count; i++)
            success = writer->device->write(writer->sections[i].offset, writer->sections[i].size);
    }
    return success;
}


struct s_minidump_writer;



// @address 0x2a83c8 main_write_memory_list_stream
extern "C" int main_write_memory_list_stream(s_main_block_writer *writer)
{
    int result = 0;
    if (writer->device->is_valid() && writer->source != NULL)
    {
        long data_size = (writer->section_count << 4) + 4;
#if !defined(_XBOX) || defined(__clang__)
        minidump_writer_add_directory_entry(
            (s_minidump_writer *)writer, writer->device->tell(), data_size, 5);
        unsigned char ok = main_write_uint32_value(writer, writer->section_count);
#else
        // The Xbox directory-entry leaf leaves the writer in r3 for the next call.
        s_main_block_writer *w2 = minidump_writer_add_directory_entry(
            (s_minidump_writer *)writer, writer->device->tell(), data_size, 5);
        unsigned char ok = main_write_uint32_value(w2, writer->section_count);
#endif
        long offset = writer->device->tell() + (writer->section_count << 4);
        unsigned char descriptors_written = (ok != 0) && (main_write_section_memory_descriptors(writer, offset) != 0);
        unsigned char section_data_written = (descriptors_written != 0) && (main_write_section_data(writer) != 0);
        result = section_data_written;
    }
    return result;
}

// @address 0x2a8f10 main_write_entry_memory_descriptors
extern "C" bool main_write_entry_memory_descriptors(s_main_block_writer *writer, long offset)
{
    bool result = false;
    unsigned long start = 0;
    unsigned long length = 0;
    if (writer->device->is_valid() && writer->source) {
        s_main_stream_block entry;
        memset(&entry, 0, sizeof(entry));
        if (writer->source->start_committed_region_query()) {
            bool success = true;
            do {
                if (writer->source->get_next_committed_region_info(&start, &length)) {
                    long size = length;
                    unsigned long position = start;
                    if (!main_test_range_excluded(writer, position, size)) {
                        long native_order = writer->byte_swap;
                        entry.second = size;
                        entry.third = offset;
                        entry.first = position;
                        if (native_order == 0)
                            main_encode_stream_block(&entry);
                        success = writer->device->write(&entry, sizeof(entry));
                        offset = length + offset;
                    }
                } else {
                    break;
                }
            } while (success);
            writer->source->end_committed_region_query();
            result = success;
        }
    }
    return result;
}


// @address 0x2a9058 main_write_entry_memory64_descriptors
extern "C" bool main_write_entry_memory64_descriptors(s_main_block_writer *writer)
{
    unsigned long start = 0;
    unsigned long length = 0;
    bool result = false;
    if (writer->device->is_valid() && writer->source) {
        unsigned __int64 entry[2];
        memset(entry, 0, sizeof(entry));
        if (writer->source->start_committed_region_query()) {
            bool ok = true;
            do {
                if (!writer->source->get_next_committed_region_info(&start, &length)) break;
                if (!main_test_range_excluded(writer, start, length)) {
                    entry[0] = start;
                    entry[1] = length;
                    if (writer->byte_swap == 0) main_encode_pair_entry(entry);
                    ok = writer->device->write(entry, 16);
                }
            } while (ok);
            writer->source->end_committed_region_query();
            result = ok;
        }
    }
    return result;
}

// @address 0x2a9308 main_write_item_memory_descriptors
extern "C" bool main_write_item_memory_descriptors(s_main_block_writer *writer, long offset, unsigned long count)
{
    bool ok = false;
    s_main_stream_block block;
    unsigned long key = 0;
    unsigned long value = 0;

    block.first = 0;
    memset(&block.second, 0, sizeof(block.second) + sizeof(block.third));
    if (writer->device->is_valid() && writer->source) {
        ok = true;
        unsigned long index = 0;
        do {
            if (index >= count) return ok;
            if (writer->source->start_module_section_memory_query(index)) {
                while (ok && writer->source->get_next_module_section_memory_info(&key, &value)) {
                    block.third = offset;
                    block.first = key;
                    block.second = value;
                    if (writer->byte_swap == 0) main_encode_stream_block(&block);
                    ok = writer->device->write(&block, 16);
                    offset += value;
                }
                writer->source->end_module_section_memory_query();
            }
            index++;
        } while (ok);
    }
    return ok;
}

// @address 0x2a9458 main_write_item_memory64_descriptors
extern "C" unsigned char main_write_item_memory64_descriptors(s_main_block_writer *writer, unsigned long count)
{
    bool result = false;
    unsigned __int64 entry[2] = {0};
    unsigned long first = 0;
    unsigned long second = 0;
    if (writer->device->is_valid() && writer->source)
    {
        bool success = true;
        unsigned long index = 0;
        do
        {
            if (index >= count) break;
            if (writer->source->start_module_section_memory_query(index))
            {
                while (success && writer->source->get_next_module_section_memory_info(&first, &second))
                {
                    entry[0] = first;
                    entry[1] = second;
                    if (writer->byte_swap == 0) main_encode_pair_entry(entry);
                    success = writer->device->write(entry, sizeof(entry));
                }
                writer->source->end_module_section_memory_query();
            }
            ++index;
        } while (success);
        result = success;
    }
    return result;
}

// @address 0x2a9708 main_compute_section_data_size
extern "C" long main_compute_section_data_size(s_main_block_writer *writer)
{
    long total = 0;
    if (writer->device->is_valid() && writer->source != NULL)
    {
        for (unsigned long i = 0; i < writer->section_count; i++)
        {
            total += writer->sections[i].size;
        }
    }
    return total;
}

// @address 0x2a9798 main_compute_item_data_size
extern "C" unsigned long main_compute_item_data_size(s_main_block_writer *writer, unsigned long count)
{
    unsigned long total = 0;
    unsigned long key = 0;
    unsigned long value = 0;
    if (writer->device->is_valid() && writer->source != NULL)
    {
        for (unsigned long index = 0; index < count; index++)
        {
            if (writer->source->start_module_section_memory_query(index))
            {
                while (writer->source->get_next_module_section_memory_info(&key, &value))
                    total += value;
                writer->source->end_module_section_memory_query();
            }
        }
    }
    return total;
}

// @address 0x2a9898 main_write_uint32_value
extern "C" bool main_write_uint32_value(s_main_block_writer *writer, unsigned long value)
{
    bool result = false;
    if (writer->device->is_valid())
    {
        if (writer->byte_swap == 0)
        {
            value = _byteswap_ulong(value);
        }
        result = writer->device->write(&value, 4);
    }
    return result;
}

// @address 0x2a9938 main_write_uint64
extern "C" bool main_write_uint64(s_main_block_writer *writer, unsigned __int64 value)
{
    bool result = false;
    if (writer->device->is_valid())
    {
        if (writer->byte_swap == 0)
        {
            unsigned long low = ((const ULARGE_INTEGER *)&value)->LowPart;
            unsigned long high = ((const ULARGE_INTEGER *)&value)->HighPart;
            value = ((unsigned __int64)_byteswap_ulong(low) << 32) | (unsigned int)_byteswap_ulong(high);
        }
        result = writer->device->write(&value, 8);
    }
    return result;
}

static __forceinline bool main_in_range(unsigned long value, unsigned long low, unsigned long high)
{
    bool result = false;
    if (value >= low && value <= high)
        result = true;
    return result;
}

// @address 0x2a9e58 main_test_range_excluded
extern "C" bool main_test_range_excluded(s_main_block_writer *writer, unsigned long start, long length)
{
    bool result = true;
    unsigned long end = start + length;
    long mode = writer->mode;
    if (mode == 0 || mode == 1 || mode == 3)
    {
        result = false;
    }
    else if (mode == 2)
    {
        unsigned long count = writer->section_count;
        result = false;
        for (unsigned long i = 0; i < count; i++)
        {
            unsigned long low = writer->sections[i].offset;
            unsigned long high = low + writer->sections[i].size;
            if (main_in_range(start, low, high))
                return true;
            if (main_in_range(end, low, high))
                return true;
        }
    }
    return result;
}

// @address 0x2a9f28 main_compute_entry_count
extern "C" unsigned __int64 main_compute_entry_count(s_main_block_writer *writer)
{
    unsigned __int64 count = 0;
    unsigned long start = 0;
    unsigned long length = 0;

    if (writer->device->is_valid() && writer->source != NULL)
    {
        if (writer->source->start_committed_region_query())
        {
            while (writer->source->get_next_committed_region_info(&start, &length))
            {
                if (!main_test_range_excluded(writer, start, length))
                    count++;
            }
            writer->source->end_committed_region_query();
        }
    }
    return count;
}


static __forceinline unsigned __int64 main_byte_swap64(unsigned __int64 value)
{
    u_main_word_pair source;
    source.value = value;
    return ((unsigned __int64)_byteswap_ulong(source.words.low) << 32) | _byteswap_ulong(source.words.high);
}

// @address 0x2ff870 main_encode_stream_block
extern "C" void main_encode_stream_block(s_main_stream_block *data)
{
    if (data)
    {
        unsigned __int64 first = data->first;
        data->second = _byteswap_ulong(data->second);
        data->third = _byteswap_ulong(data->third);
        data->first = main_byte_swap64(first);
    }
}

// @address 0x2ff8d8 main_encode_pair_entry
extern "C" void main_encode_pair_entry(unsigned __int64 *data)
{
    if (data)
    {
        unsigned __int64 second = data[1];
        unsigned __int64 first = data[0];
        data[1] = main_byte_swap64(second);
        data[0] = main_byte_swap64(first);
    }
}

void __tls_set_g_main_render_timing_data_allocator(void *allocator)
{
    g_water_interaction_events = (s_water_interaction_event *)allocator;
}

// 0x211810: one pass over the four controllers. Each signed-in controller's cached profile summary is compared with the
// live one; when the live element handle is better, a load screen message is posted and the profile gets the handle.
// @address 0x50cb0 ??_7c_load_campaign_select_difficulty_screen_message@@6B@


// @address 0x9e7e60 g_controller_signin_cache





// @address 0x211810 main_update_profile_rank_grade
extern "C" void main_update_profile_rank_grade(void)
{
    s_controller_signin_cache_entry *signin_cache;
    c_player_profile_interface *player_profile;
    int global_rank;
    int global_grade;
    int cached_rank;
    int cached_grade;
    void *message_storage;
    int message_previous_grade;
    int message_previous_rank;
    int next_rank_controller;
    int controller_index;
    int current_rank_grade;
    int cached_rank_grade;
    int promoted_rank_grade;
    int profile_rank_grade;
    s_player_profile_summary profile_summary;
    controller_index = 0;
    do {
        signin_cache = &g_controller_signin_cache[controller_index];
        if (((unsigned int)(signed char)signin_cache->valid != 0)) {
            player_profile = g_controller_globals.controllers[controller_index].get_player_profile_interface();
            user_interface_session_get_player_profile_summary((e_controller_index)controller_index, &profile_summary);
            // match: 0x211810: volatile reads load rank and grade straight into r31/r28; plain reads load r11/r10 and copy
            const volatile s_player_profile_summary &live_summary = profile_summary;
            global_rank = live_summary.global_rank;
            global_grade = live_summary.global_grade;
            current_rank_grade = -1;
            if (((unsigned int)global_rank <= 13) && ((unsigned int)global_grade <= 3))
                current_rank_grade = g_experience_rank_icon_element_handles[global_rank][global_grade];
            cached_rank = signin_cache->info_key;
            cached_rank_grade = -1;
            cached_grade = signin_cache->global_grade;
            if (((unsigned int)cached_rank <= 13) && ((unsigned int)cached_grade <= 3))
                cached_rank_grade = g_experience_rank_icon_element_handles[cached_rank][cached_grade];
            if ((current_rank_grade > cached_rank_grade) &&
                ((((unsigned char)window_manager_is_profile_screen_present(g_controller_signin_cache)) == 0))) {
                promoted_rank_grade = -1;
                if (((unsigned int)global_rank <= 13) && ((unsigned int)global_grade <= 3))
                    promoted_rank_grade = g_experience_rank_icon_element_handles[global_rank][global_grade];
                if ((promoted_rank_grade > player_profile->last_played.unknown_setting_c4)) {
                    message_storage = user_interface_malloc_tracked(sizeof(c_load_campaign_select_difficulty_screen_message), 0, 0);
                    if ((message_storage != 0)) {
                        message_previous_grade = signin_cache->global_grade;
                        message_previous_rank = signin_cache->info_key;
                        c_load_campaign_select_difficulty_screen_message *message =
                            new (message_storage) c_load_campaign_select_difficulty_screen_message(_string_id_spartan_rank_dialog,
                                (e_controller_index)(controller_index), _console_window, _string_id_top_most,
                                message_previous_rank, message_previous_grade, global_rank, global_grade);
                        ::user_interface_messaging_post(message);
                    }
                    profile_rank_grade = -1;
                    if (((unsigned int)global_rank <= 13) && ((unsigned int)global_grade <= 3))
                        profile_rank_grade = g_experience_rank_icon_element_handles[global_rank][global_grade];
                    player_profile->set_unknown_setting_c4(profile_rank_grade, true);
                }
                signin_cache->info_key = global_rank;
                signin_cache->global_grade = global_grade;
            }
        }
        next_rank_controller = -1;
        if (((unsigned int)controller_index <= 2))
            next_rank_controller = (controller_index + 1);
        controller_index = next_rank_controller;
    } while ((next_rank_controller != -1));
}

// 0x211690: one pass over the four controllers. Each signed-in controller's cached campaign difficulty is compared
// with the live summary element handle; when the live handle is better, a load screen message is posted,
// achievements are granted, the profile gets the handle, and the cache takes the handle.
// @address 0x211690 main_update_campaign_achievements
extern "C" void main_update_campaign_achievements(void)
{
    s_controller_signin_cache_entry *signin_cache;
    c_player_profile_interface *player_profile;
    int campaign_difficulty;
    int cached_difficulty;
    void *message_storage;
    int next_slot;
    int controller_index;
    long *best_difficulty;
    long difficulty_storage;
    s_player_profile_summary profile_summary;
    controller_index = 0;
    do {
        signin_cache = &g_controller_signin_cache[controller_index];
        if (((unsigned int)(signed char)signin_cache->valid != 0)) {
            player_profile = g_controller_globals.controllers[controller_index].get_player_profile_interface();
            user_interface_session_get_player_profile_summary((e_controller_index)controller_index, &profile_summary);
            campaign_difficulty = network_configuration_get_player_class(&profile_summary);
            cached_difficulty = signin_cache->key;
            difficulty_storage = campaign_difficulty;
            if ((campaign_difficulty > cached_difficulty)) {
                // match: 0x211690: keeps g_controller_globals in r27 and the message vtable address in r26; without it they swap
                __assume(&g_controller_globals != 0);
                if ((((unsigned char)window_manager_is_profile_screen_present(g_controller_signin_cache)) == 0)) {
                    if ((campaign_difficulty > player_profile->last_played.campaign_difficulty)) {
                        message_storage = user_interface_malloc_tracked(sizeof(c_load_object_creation_screen_message), 0, 0);
                        if (message_storage != 0) {
                            c_load_object_creation_screen_message *message = new (message_storage) c_load_object_creation_screen_message(
                                _string_id_spartan_milestone_dialog, (e_controller_index)controller_index,
                                _console_window, _string_id_top_most, campaign_difficulty);
                            ::user_interface_messaging_post(message);
                        }
                        switch (campaign_difficulty) {
                        case 2:
                            game_grant_achievement_to_local_player((e_controller_index)controller_index, _achievement_graduate, true);
                            break;
                        case 3:
                            game_grant_achievement_to_local_player((e_controller_index)controller_index, _achievement_unsc_spartan, true);
                            break;
                        case 4:
                            game_grant_achievement_to_local_player((e_controller_index)controller_index, _achievement_spartan_officer, true);
                            break;
                        }
                        if ((campaign_difficulty >= 0))
                            player_profile->set_campaign_difficulty((e_campaign_difficulty_level)campaign_difficulty, true);
                    }
                    if ((campaign_difficulty > signin_cache->key)) best_difficulty = &difficulty_storage;
                    else best_difficulty = &signin_cache->key;
                    signin_cache->key = *best_difficulty;
                }
            }
        }
        next_slot = -1;
        if (((unsigned int)controller_index <= 2))
            next_slot = (controller_index + 1);
        controller_index = next_slot;
    } while ((next_slot != -1));
}

// 0x211ab8: refresh the controller sign-in cache, then post campaign-difficulty
// and profile-summary load-screen messages unless the topmost screen or the
// current game mode hides them.
extern "C" {


}

// 0x126b38 natural source
// @address 0x126b38 main_update_effects_and_sound
struct s_global_modifier;


extern "C" void main_update_effects_and_sound(float delta_time)
{
    if (delta_time > 0.0f)
    {
        for (short i = 0; i < 4; i++)
        {
            s_widget_type *type = &g_widget_types[i];
            if (type->update != 0)
                type->update(delta_time);
        }
        ::game_sound_update(delta_time);
        scenario_interpolators_frame_update(delta_time);
        animate_global_exposure_modifier(camera_fx_globals, delta_time);
    }
    c_render_globals::set_frame_time(c_render_globals::get_frame_time() + delta_time);
    g_player_effect_globals->enabled = delta_time;
    tls_game_globals()->game_had_an_update_tick_this_frame = false;
}



// @address 0x1aa040 ?main_reset_map_private@@YAXXZ
void main_reset_map_private()
{
    if (game_in_progress() && !game_time_is_paused())
    {
        main_reset_zone_requests(-1);
        main_game_reset_map(main_globals.reset_map_parameter);
        main_globals.save_requested = 0;
        main_globals.reset_map_parameter = false;
    }
}

// @address 0x1aa240 main_update_scenario_bits
extern "C" void main_update_scenario_bits(void)
{
    main_globals.dirty = 0;
    if (game_in_progress())
        scenario_update_designer_zones((const s_scenario_bits_change *)&main_globals.deactivate_bits);
    main_globals.deactivate_bits = 0;
    main_globals.activate_bits = 0;
    main_globals.enable_bits = 0;
    main_globals.disable_bits = 0;
}


// @address 0x211ab8 main_update_profile_messages
extern "C" void main_update_profile_messages(void *signin_cache)
{
    bool show_first = true;
    bool show_second = true;
    c_gui_screen_widget *node = g_window_manager.get_topmost_screen(_console_window);
    game_globals_storage *globals = tls_game_globals();
    bool active = globals && (globals->initializing || globals->map_active);
    if (!active || globals->options.game_mode != 3) {
        show_second = false;
        show_first = false;
    }
    if (node) {
        switch (node->get_name()) {
        case _string_id_spartan_milestone_dialog:
        case _string_id_spartan_rank_dialog:
        case _string_id_gui_alert_toast:
        case _string_id_gui_alert_ingame_full:
        case _string_id_gui_alert_ingame_split:
        case _string_id_gui_dialog_two_items:
        case _string_id_gui_dialog_four_items:
        case _string_id_in_progress:
            show_second = false;
            show_first = false;
            break;
        }
    }
    ((void (*)(void *))controller_update_signin_cache)(g_controller_signin_cache);
    if (show_first) {
        ((void (*)(void *))main_update_campaign_achievements)(g_controller_signin_cache);
    }
    if (show_second) {
        ((void (*)(void *))main_update_profile_rank_grade)(g_controller_signin_cache);
    }
}

extern "C" {

}

extern "C" void main_reset_saved_film_state(void)
{
    if (tls_game_globals()->options.game_mode != 3 && main_game_globals.reset_in_progress == 0)
    {
        saved_film_dispose();
        saved_film_reset_work_globals();
        XEnableScreenSaver(1);
        g_saved_film_timer_globals.screen_saver_enabled = true;
    }
}


// @address 0x2a9b70 main_encode_block_range
extern "C" bool main_encode_block_range(s_main_block_writer *writer, const char *memory, long length)
{
    bool result = false;
    if (writer->source != NULL)
    {
        if (writer->device->is_valid())
        {
            if (memory != NULL && length > 0)
            {
                if (writer->source->should_resolve_memory())
                {
                    unsigned long offset = 0;
                    result = true;
                    do
                    {
                        if (offset >= (unsigned long)length)
                            break;
                        long chunk = length - (long)offset;
                        if ((unsigned long)chunk >= 0x400)
                            chunk = 0x400;
                        unsigned long actual = 0;
                        if (!writer->source->resolve_memory(memory + offset, chunk, g_main_minidump_copy_buffer, &actual))
                        {
                            const char *fallback = "XXminidump could not resolve this memoryXX";
                            long fallback_length = strlen(fallback);
                            if (chunk > 0)
                            {
                                long copied = 0;
                                do
                                {
                                    long piece = chunk - copied;
                                    if (piece >= fallback_length)
                                        piece = fallback_length;
                                    strncpy(g_main_minidump_copy_buffer + copied, fallback, piece);
                                    copied += piece;
                                } while (copied < chunk);
                            }
                            actual = chunk;
                        }
                        result = writer->device->write(g_main_minidump_copy_buffer, actual);
                        offset += actual;
                    } while (result);
                }
                else
                    result = writer->device->write(memory, length);
            }
        }
    }
    return result;
}

// @address 0x2a9198 main_encode_stream_entries
extern "C" bool main_encode_stream_entries(s_main_block_writer *writer, unsigned long count)
{
    bool result = false;
    unsigned long index = 0;
    unsigned long start = 0;
    unsigned long length = 0;
    if (writer->device->is_valid() && writer->source && count)
    {
        if (writer->source->start_committed_region_query())
        {
            bool success = true;
            do
            {
                if (!writer->source->get_next_committed_region_info(&start, &length)) break;
                long size = length;
                unsigned long position = start;
                if (!main_test_range_excluded(writer, position, size))
                    success = main_encode_block_range(writer, static_cast<const char *>(pointer_from_address(position)), size);
                ++index;
                if (writer->progress)
                    writer->progress((float)index * 50.0f / (float)count + 50.0f);
            } while (success);
            writer->source->end_committed_region_query();
            result = success;
        }
    }
    return result;
}


static __forceinline bool main21_write_blocks(s_main_block_writer *writer, unsigned long count)
{
    bool result = false;
    unsigned long start = 0;
    unsigned long length = 0;
    if (writer->device->is_valid() && writer->source)
    {
        bool success = true;
        unsigned long index = 0;
        do
        {
            if (index >= count) break;
            if (writer->source->start_module_section_memory_query(index))
            {
                while (success && writer->source->get_next_module_section_memory_info(&start, &length))
                    success = main_encode_block_range(writer, static_cast<const char *>(pointer_from_address(start)), length);
                writer->source->end_module_section_memory_query();
            }
            if (writer->progress)
                writer->progress((float)(index + 1) * 50.0f / (float)count);
            ++index;
        } while (success);
        return success;
    }
    return result;
}
// @address 0x2a95a0 main_encode_indexed_blocks
extern "C" bool main_encode_indexed_blocks(s_main_block_writer *writer, unsigned long count)
{
    return main21_write_blocks(writer, count);
}

// @address 0x2a84e8 main_write_minidump_memory_list
extern "C" int main_write_minidump_memory_list(s_main_block_writer *writer)
{
    int result = 0;
    long total = 0;
    unsigned long size = 0;
    unsigned long count = 0;
    unsigned long item_size = 0;
    if (writer->device->is_valid() && writer->source) {
        writer->source->get_committed_region_count(&size);
        writer->source->get_loaded_module_count(&count);
        total = writer->section_count + size;
        for (unsigned long i = 0; i < count; i++) {
            writer->source->get_loaded_module_section_count(i, &item_size);
            total = item_size + total;
        }
        int base = writer->device->tell();
        if (writer->entry_count < 8) {
            writer->headers[writer->entry_count].rva = base;
            writer->headers[writer->entry_count].data_size = total * 16 + 4;
            writer->headers[writer->entry_count].stream_type = 5;
            writer->entry_count++;
        }
        unsigned char ok = main_write_uint32_value((s_main_block_writer *)writer, total);
        long offset = writer->device->tell() + total * 16;
        ok = ok && main_write_section_memory_descriptors((s_main_block_writer *)writer, offset);
        offset = main_compute_section_data_size((s_main_block_writer *)writer) + offset;
        ok = ok && main_write_item_memory_descriptors((s_main_block_writer *)writer, offset, count);
        offset = main_compute_item_data_size((s_main_block_writer *)writer, count) + offset;
        ok = ok && main_write_entry_memory_descriptors((s_main_block_writer *)writer, offset);
        ok = ok && main_write_section_data((s_main_block_writer *)writer);
        ok = ok && main_encode_indexed_blocks((s_main_block_writer *)writer, count);
        ok = ok && main_encode_stream_entries((s_main_block_writer *)writer, size);
        result = ok;
    }
    return result;
}

// @address 0x2a8798 main_write_minidump_memory64_list
extern "C" int main_write_minidump_memory64_list(s_main_block_writer *writer)
{
    int result = 0;
    unsigned __int64 total = 0;
    unsigned long size = 0;
    unsigned long count = 0;
    unsigned long item_size = 0;
    if (writer->device->is_valid() && writer->source) {
        writer->source->get_committed_region_count(&size);
        writer->source->get_loaded_module_count(&count);
        total = (unsigned __int64)(unsigned long)writer->section_count + size;
        for (unsigned long i = 0; i < count; i++) {
            writer->source->get_loaded_module_section_count(i, &item_size);
            total = item_size + total;
        }
        int base = writer->device->tell();
        if (writer->entry_count < 8) {
            writer->headers[writer->entry_count].rva = base;
            writer->headers[writer->entry_count].data_size = (total + 1) * 16;
            writer->headers[writer->entry_count].stream_type = 9;
            writer->entry_count++;
        }
        unsigned char ok = main_write_uint64((s_main_block_writer *)writer, total);
        unsigned __int64 offset = (unsigned long)(writer->device->tell() + total * 16 + 8);
        ok = ok && main_write_uint64((s_main_block_writer *)writer, offset);
        ok = ok && writer->write_section_memory64_descriptors();
        main_compute_section_data_size((s_main_block_writer *)writer);
        ok = ok && main_write_item_memory64_descriptors((s_main_block_writer *)writer, count);
        ok = ok && main_write_entry_memory64_descriptors((s_main_block_writer *)writer);
        ok = ok && main_write_section_data((s_main_block_writer *)writer);
        ok = ok && main_encode_indexed_blocks((s_main_block_writer *)writer, count);
        ok = ok && main_encode_stream_entries((s_main_block_writer *)writer, size);
        result = ok;
    }
    return result;
}

// @address 0x2a8a60 main_write_minidump_memory_list_excluding_sections
extern "C" int main_write_minidump_memory_list_excluding_sections(s_main_block_writer *writer)
{
    int result = 0;
    unsigned long size = 0;
    unsigned long count = 0;
    unsigned long item_size = 0;
    if (writer->device->is_valid() && writer->source) {
        writer->source->get_committed_region_count(&size);
        unsigned __int64 extra = ((unsigned __int64 (*)(s_main_block_writer *, unsigned long))main_compute_entry_count)((s_main_block_writer *)writer, size);
        writer->source->get_loaded_module_count(&count);
        unsigned __int64 total = extra;
        for (unsigned long i = 0; i < count; i++) {
            writer->source->get_loaded_module_section_count(i, &item_size);
            total = item_size + total;
        }
        int base = writer->device->tell();
        if (writer->entry_count < 8) {
            writer->headers[writer->entry_count].rva = base;
            writer->headers[writer->entry_count].data_size = total * 16 + 4;
            writer->headers[writer->entry_count].stream_type = 5;
            writer->entry_count++;
        }
        unsigned char ok = ((unsigned char (*)(s_main_block_writer *, unsigned __int64))main_write_uint32_value)((s_main_block_writer *)writer, total);
        unsigned long offset = writer->device->tell() + total * 16;
        ok = ok && main_write_item_memory_descriptors((s_main_block_writer *)writer, offset, count);
        unsigned long next = main_compute_item_data_size((s_main_block_writer *)writer, count) + offset;
        ok = ok && main_write_entry_memory_descriptors((s_main_block_writer *)writer, next);
        ok = ok && main_encode_indexed_blocks((s_main_block_writer *)writer, count);
        ok = ok && ((unsigned char (*)(s_main_block_writer *, unsigned __int64))main_encode_stream_entries)((s_main_block_writer *)writer, extra);
        result = ok;
    }
    return result;
}


// @address 0x1f0630 main_loading_update_progress
extern "C" void main_loading_update_progress(void *context, void *user_data, long percent_complete)
{
    if (g_loading_progress.visible && (unsigned long)g_loading_progress.stage <= 2)
        loading_update_progress((unsigned int)((1.0f - (float)percent_complete * 0.01f) * g_loading_progress.maximum[g_loading_progress.stage]));
    main_update_idle_tasks();
}

struct game_options;


class c_decal_message_union;
struct s_object_render_thread_message;
extern "C" {


}

static __forceinline void main_render_purge_pending_messages_inline()
{
    ((t_message_queue<s_object_render_thread_message, 2048> *)g_object_message_queue)->clear();
    main_render_purge_effect_messages(false);
    g_decal_message_queue->clear();
}

static __forceinline void main_physical_memory_resize_region_dispose()
{
    physical_memory_pop_stage(3);
    if (g_physical_memory_globals.listener)
        g_physical_memory_globals.listener->dispose_resize_buffer(physical_memory_current_stage_buffer());
    physical_memory_pop_stage(2);
}

static __forceinline void main_game_dispose()
{
    game_globals = NULL;
    for (long system_index = 91; system_index >= 0; system_index--)
        g_game_systems[system_index].dispose_proc();
}

static __forceinline void main_progress_set_default_callbacks(const progress_callbacks *callbacks)
{
    if (callbacks)
        progress_globals.default_callbacks = *callbacks;
    else
        memset((&progress_globals.default_callbacks), 0, sizeof(progress_callbacks));
}

static __forceinline void main_loading_dispose()
{
    g_loading_progress.active = false;
    main_progress_set_default_callbacks(NULL);
}

void main_loop_exit()
{
    main_render_purge_pending_messages_inline();
    g_thread_should_exit[2].set(1);
    rasterizer_main_wake_render_thread();
    wait_for_thread_to_exit(2, 0xffffffff);
    main_game_unload_and_prepare_for_next_game(NULL);
    main_physical_memory_resize_region_dispose();
    main_game_dispose();
    main_loading_dispose();
}


static __forceinline void main_loop_loading_initialize()
{
    progress_callbacks callbacks = {};
    callbacks.progress_new_proc = (void (*)(const char *, void *))main_loop_handle_loading_transition;
    callbacks.progress_update_proc = (void (*)(const char *, const char *, long, void *))main_loading_update_progress;
    callbacks.progress_done_proc = (void (*)(const char *, void *))main_loop_handle_loading_transition;
    main_globals.startup_masking_done = false;
    g_loading_progress.active = false;
    progress_globals.default_callbacks = callbacks;
}
static __forceinline void main_loop_time_initialize()
{
    g_main_time_globals = (s_main_time_globals *)g_main_time_globals_allocator.reserve_memory("main_timing",
        "globals", 64, 0);
    unsigned long now = GetTickCount();
    s_main_time_globals *globals = g_main_time_globals;
    globals->milliseconds = now;
    __int64 *timing = &globals->vblank_index;
    timing[0] = 0;
    timing[1] = 0;
    timing[2] = 0;
    timing[3] = 0;
}
// @address 0x1a8838 ?main_loop_enter@@YAXXZ
void main_loop_enter()
{
    main_globals.startup_masking_pending = true;
    main_loop_loading_initialize();
    main_game_globals.game_won = false;
    main_loop_time_initialize();
    rasterizer_globals_initialize();
    game_initialize();
    g_main_gamestate_timing_data = (s_game_tick_time_samples *)g_main_gamestate_timing_data_allocator.reserve_memory("timing samples",
        "global", 20, 0);
    g_main_render_timing_data = (s_game_tick_time_samples *)g_main_render_timing_data_allocator.reserve_memory("timing samples",
        "global", 20, 0);
    physical_memory_resize_region_initialize();
    g_main_gamestate_timing_data->initialize();
    g_main_render_timing_data->initialize();
    main_render_purge_pending_messages_inline();
    main_time_update_sync_milliseconds();
    main_render_update_last_milliseconds();
    rasterizer_main_initialize_render_thread_wake();
    c_rasterizer::rasterizer_device_release_thread();
    start_thread(2);
    start_thread(5);
}



struct s_network_globals;
static __forceinline bool main_bink_playback_enabled()
{
    bool result = false;
    if (bink_globals.active)
        result = bink_globals.enabled != 0;
    return result;
}
// @address 0x1a9940 main_update_idle_tasks
extern "C" void main_update_idle_tasks()
{
    unsigned long now = GetTickCount();
    bool main_thread = g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 1;
    if (main_thread && (int)main_globals.recursion_count == 0) {
        unsigned long last = main_globals.last_idle_tick;
        if (last != 0 && now - last >= 32) {
            main_globals.recursion_count = 1;
            main_globals.last_idle_tick = now;
            main_thread_process_pending_messages();
            overlapped_update();
            if (network_globals.entered == 0) {
                network_receive();
                network_update();
                network_send();
            }
            if (g_render_thread_enabled.peek() == 0) {
                main_render_loading_frame();
            }
            sound_idle();
            if (bink_globals.active && main_bink_playback_enabled() &&
                bink_globals.runtime.frame_index != 0) {
                bink_playback_stop_and_cleanup();
            }
            main_globals.recursion_count = main_globals.recursion_count - 1;
        }
    }
}

extern "C" {
// @address 0x74b7fc g_main_save_map_enabled
char g_main_save_map_enabled = 1;



}
static __forceinline bool main_save_map_should_write_state()
{
    return tls_game_globals()->options.game_mode == 1 && game_campaign_has_multiple_players() == false;
}
// @address 0x1aa0f0 ?main_save_map_and_exit_private@@YAXXZ
void main_save_map_and_exit_private()
{
    if (!game_is_cooperative()) {
        saved_film_dispose();
        if (g_main_save_map_enabled != 0 && main_save_map_should_write_state()) {
            game_state_revert(0);
            game_state_lock_and_halt_render_thread(0);
            ::saved_game_files_write_saved_game_state((game_state_header const *)&game_state_globals.headers[game_state_globals.index],
                0xf998, (void const *)(game_state_globals.game_state_buffer), 0x7e0000);
            game_state_unlock_and_resume_render_thread(0);
        }
        game_finish();
    }
    main_globals.shutdown_requested = 0;
}

enum e_async_priority;
enum e_async_category;
enum e_async_completion;

static __forceinline float game_time_get_speed()
{
    return game_time_globals->rate_scale;
}
// @rdata 0x8202490c
static const float main_loop_rate_scale_epsilon = 0.0001f;
struct s_chud_globals_definition;
struct main_loop_waypoints;
struct main_loop_signin_cache;
struct s_saved_film_playback;
extern "C" {

static s_saved_film_playback *g_saved_film_playback;
// @address 0x8c48e0 g_closed_caption_globals
#if defined(_XBOX) && !defined(__clang__)
static s_closed_caption_globals g_closed_caption_globals;
#else
s_closed_caption_globals g_closed_caption_globals;
#endif












}
static __forceinline long main_loop_first_active_user()
{
    long result = -1;
    for (long i = 0; i < 4; ++i)
        if (tls_player_mapping_globals()->player_indices[i] != -1)
        {
            result = i;
            break;
        }
    return result;
}


static __forceinline bool shell_application_is_paused()
{
    return g_shell_application_paused;
}
static __forceinline bool main_time_halted()
{
    bool halted = shell_application_is_paused();
    return halted;
}

static __forceinline unsigned long user_interface_milliseconds()
{
    return g_user_interface_globals.milliseconds.peek();
}
static __forceinline void closed_caption_update()
{
    unsigned long current_time = user_interface_milliseconds();
    for (unsigned long index = 0; index < 5; index++)
    {
        s_closed_caption_entry *caption = &g_closed_caption_globals.captions[index];
        if (caption->flags & 1)
        {
            unsigned long elapsed = current_time - caption->creation_time;
            if (elapsed >= 5000)
                memset(caption, 0, sizeof(*caption));
        }
    }
}
// @address 0x1a8ed8 ?main_loop_body@@YAXXZ
void main_loop_body()
{
    input_update();
    input_abstraction_update();
    global_preferences_update();
    async_idle();
    if (g_global_preferences_close_state.task != -1)
    {
        if (g_global_preferences_close_state.done.peek())
            g_global_preferences_close_state.task = -1;
    }
    else if (g_global_preferences_close_state.requested)
    {
        s_async_task task;
        memset(&task, 0, sizeof(task));
        g_global_preferences_close_state.task = async_task_add_ex(_async_priority_blocking_generic, &task,
            _async_category_none, close_file_callback,
            &g_global_preferences_close_state.done, true);
        if (g_global_preferences_close_state.task != -1)
            g_global_preferences_close_state.requested = false;
    }
    cache_files_windows_idle();
    cache_files_windows_process_loading_requests();
    long listener_index = 0;
    // match: 0x1a8ed8: signed address comparison keeps cmpw at the listener loop back edge.
    for (s_game_state_proc_entry *entry = g_game_state_procs.entries;
        (intptr_t)entry < (intptr_t)(g_game_state_procs.entries + NUMBEROF(g_game_state_procs.entries)); ++entry, ++listener_index)
        if ((1 << (listener_index & 31)) & g_game_state_procs.active[listener_index >> 5])
            entry->handler->update(entry->buffer);
    main_loop_process_global_state_changes();
    tag_resource_update(g_geometry_globals.manager);
    if (!shell_application_is_paused())
    {
        float seconds_elapsed = 0.0f;
        float world_seconds_elapsed = 0.0f;
        float game_seconds_elapsed = 0.0f;
        long game_ticks_elapsed = 0;
        seconds_elapsed = main_time_update();
        overlapped_update();
        c_tag_resources_game_lock lock;
        // match: 0x1a8ed8: March calls main_time_halted() twice here (ternary, then set(6)); one cached call keeps
        // the four late TLS address temps aligned so the minimum/maximum/total/tail adds keep the shipped order
        bool halted = main_time_halted();
        world_seconds_elapsed = halted ? 0.0f : seconds_elapsed;
        g_main_gamestate_timing_data->minimum += seconds_elapsed;
        g_main_gamestate_timing_data->maximum += world_seconds_elapsed;
        g_main_gamestate_timing_data->flags.set(6, halted);
        user_interface_update(seconds_elapsed);
        closed_caption_update();
        bink_playback_update();
        if (g_saved_film_playback)
            saved_film_update_playback_download(g_saved_film_playback);
        main_update_profile_messages(g_controller_signin_cache);
        saved_film_update();
        network_receive();
        network_update();
        simulation_update();
        if (simulation_in_progress())
        {
            bool time_advanced = game_time_update(world_seconds_elapsed, &game_seconds_elapsed, &game_ticks_elapsed);
            g_main_gamestate_timing_data->flags.set(2, !time_advanced);
#if !defined(_XBOX) || defined(__clang__)
            // tmp/probe (GAME-CHANGES M2): a frame is published (drawn) only when one of these flags is set, and a
            // frame between two ticks sets none (flag 0 is "ticks ran"), so the frame rate never exceeds the tick rate.
            // Above 30 frames a second every frame in which game time advanced is published (flag 4, unused by the
            // game).
            if (time_advanced && port_d3d_frames_between_ticks())
                g_main_gamestate_timing_data->flags.set(4, true);
#endif
            g_main_gamestate_timing_data->total += game_seconds_elapsed;
            g_main_gamestate_timing_data->last_sample += game_ticks_elapsed;
            g_main_gamestate_timing_data->flags.set(0,
                g_main_gamestate_timing_data->last_sample != 0);
            player_control_update(world_seconds_elapsed, game_seconds_elapsed);
            game_update_ticks(game_ticks_elapsed, &game_seconds_elapsed);
            network_send();
            main_update_effects_and_sound(game_seconds_elapsed);
            director_update(world_seconds_elapsed);
            observer_update(world_seconds_elapsed);
            game_engine_update_scoreboard(world_seconds_elapsed);
            if (chud_globals)
            {
                game_engine_update_waypoint_timers(g_chud_scripting_globals);
                long user = main_loop_first_active_user();
                while (user != -1)
                {
                    c_chud_manager::x_persistent_user_data[user].update(user, world_seconds_elapsed);
                    user = players_next_active_user(user);
                }
            }
            rumble_update(world_seconds_elapsed);
            achievements_update(world_seconds_elapsed);
            for (long i = 0; i < 4; ++i)
                first_person_weapons_update_camera_estimate(i);
            if (main_time_halted()) sound_idle(); else sound_render();
        }
        else
        {
            if (g_simulation_globals.initialized && simulation_starting_up() && g_simulation_globals.watcher->need_to_generate_updates())
            {
                struct simulation_update update;
                update.flags = 0;
                s_simulation_update_metadata metadata;
                simulation_build_game_update(0, &update, &metadata);
                saved_film_update_recording(&update);
                g_tracked_object_globals->active = true;
                simulation_apply_before_game(&update);
                objects_flush_tracked_accelerations();
                c_simulation_world::distribute_update(&update, &metadata);
                simulation_destroy_update(&update);
            }
            network_send();
            rumble_clear_all_now();
            g_main_gamestate_timing_data->flags.set(5, true);
            if (main_time_halted()) sound_idle(); else sound_render();
        }
#if !defined(_XBOX) || defined(__clang__)
        {
            extern void port_sim_scenario_tick(long game_ticks_elapsed, float world_seconds_elapsed);
            port_sim_scenario_tick(game_ticks_elapsed, world_seconds_elapsed);
        }
#endif
    }
    else
        g_main_gamestate_timing_data->flags.set(1, true);
    if (game_time_is_paused())
        g_main_gamestate_timing_data->flags.set(3, true);
    if (game_time_initialized() && fabs(game_time_get_speed() - 1.0f) >= main_loop_rate_scale_epsilon)
        g_main_gamestate_timing_data->flags.set(7, true);
    if (game_in_progress() && game_is_cooperative() && !game_is_film_playback_active())
        g_main_gamestate_timing_data->flags.set(10, true);
    s_game_tick_time_samples *timing = g_main_gamestate_timing_data;
    float elapsed = timing->maximum;
    if (!(elapsed < 0.06666667f))
        timing->flags.set(11, true);
    else if (simulation_starting_up() && game_is_cooperative() && elapsed > 0.0f && elapsed < 0.025f && !timing->flags.test(8) &&
        !timing->flags.test(9) && !timing->flags.test(0))
        timing->flags.clear();
    timing->flags.set(8, !main_time_is_throttled());
    main_demand_update();
    main_exception_handle_crash();
}

extern "C" {
// match: 0x80278: winbase.h adoption is blocked by this unit's types/API arguments; keep the intrinsic prototype and pragma.

}

struct s_rasterizer_globals;

static __forceinline bool main_loop_render_thread_requested()
{
    return g_single_thread_request_flags.peek() != 0;
}
static __forceinline void *main_loop_event_handle()
{
    return g_synch_globals.events[3].handle;
}
static __forceinline __int64 main_loop_rasterizer_frame()
{
    return rasterizer_globals.vblank_index.peek();
}
#if !defined(_XBOX) || defined(__clang__)
extern "C" unsigned long port_d3d_vertical_blank_rate(void);
#endif
// @address 0x1a9790 ?main_loop@@YAXXZ
void main_loop()
{
    unsigned long lock_token;

    main_loop_enter();
    restricted_region_unlock_primary(4);
    restricted_region_unlock_primary(2);
    rasterizer_main_wake_render_thread();
    __int64 last_frame = main_loop_rasterizer_frame();
    if (main_globals.loop_exit_requested == 0) {
    do {
        __int64 frame = main_loop_rasterizer_frame();
#if !defined(_XBOX) || defined(__clang__)
        if (!disable_main_loop_throttle && frame == last_frame && port_d3d_vertical_blank_rate() < 1000) {
#else
        if (!disable_main_loop_throttle && frame == last_frame) {
#endif
            void *event = main_loop_event_handle();
            if (event != 0) {
                WaitForSingleObjectEx(event, 0xFFFFFFFF, 0);
            }
        }
        last_frame = main_loop_rasterizer_frame();
        main_globals.last_idle_tick = GetTickCount();
        bool single_threaded = false;
        g_single_thread_request_flags.bitwise_and(~1);
        if (g_render_thread_enabled.peek() != 1 && g_render_thread_enabled.peek() != 2) {
            single_threaded = true;
            main_thread_process_pending_messages();
            c_rasterizer::dispose();
        } else {
            main_thread_process_pending_messages();
            publish_waiting_gamestate();
            main_loop_body();
            publish_waiting_gamestate();
        }
        bool render_thread_requested = main_loop_render_thread_requested();
        if (render_thread_requested != single_threaded) {
            if (!single_threaded) {
                lock_token = ((int)::_internal_halt_render_thread_and_lock_resources("..\\main\\main.cpp", 0x506));
            } else {
                unlock_resources_and_resume_render_thread(lock_token);
            }
        }
    } while (main_globals.loop_exit_requested == 0);
    }
    if (g_render_thread_enabled.set_if_equal(0, 1) == 1)
        rasterizer_main_wake_render_thread();
    restricted_region_lock_primary(2);
    restricted_region_lock_primary(4);
    main_loop_exit();
}


// @address 0x74b898 g_geometry_globals
// @address 0x8c4694 g_rasterizer_device
// @address 0x5a80c8 D3DDevice_Suspend
// @address 0x46b000 ?main_halt_and_display_errors@@YAXXZ
void main_halt_and_display_errors()
{
    bool render_thread = g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 2;
    if (render_thread)
    {
        if (main_rasterizer_thread_owns_device())
            c_rasterizer::rasterizer_device_release_thread();
        g_main_halt_requested = 1;
        for (;;)
            Sleep((unsigned long)-1);
    }
    shell_mount_utility_drive(1);
    if (g_rasterizer_device)
    {
        ((int)::_internal_halt_render_thread_and_lock_resources("..\\rasterizer\\xenon\\rasterizer_xenon_main.cpp", 951));
        if (!g_rasterizer_frame_has_ended)
            main_render_blank_frame(L"");
        D3DDevice_Suspend(g_rasterizer_device);
    }
    e_controller_index controller = controller_get_first_signed_in_controller();
    if (controller == _controller_none)
        controller = _controller0;
    XShowDirtyDiscErrorUI(controller);
}

// @address 0x1ba700 ?report_page_failure@c_tag_resource_error_receiver@@UAAXXZ
void c_tag_resource_error_receiver::report_page_failure()
{
    if (g_main_halt_on_state_change)
    {
        main_halt_and_display_errors();
        return;
    }
    m_page_failed = true;
}

// @address 0x1ba720 ?report_read_failure@c_tag_resource_error_receiver@@UAAXXZ
void c_tag_resource_error_receiver::report_read_failure()
{
    if (g_main_halt_on_state_change)
    {
        main_halt_and_display_errors();
        return;
    }
    m_read_failed = true;
}

// @address 0x1ba740 ?report_hash_failure@c_tag_resource_error_receiver@@UAAXXZ
void c_tag_resource_error_receiver::report_hash_failure()
{
    if (g_main_halt_on_state_change)
    {
        main_halt_and_display_errors();
        return;
    }
    m_hash_failed = true;
}

// @address 0x1a9a68 main_render_loading_frame
extern "C" void main_render_loading_frame()
{
    char text[12288];
    unsigned short wide_text[12292];
    long mode;

    wide_text[0] = 0;
    bool bink = bink_globals.active &&
                bink_globals.enabled;
    if (bink) {
        mode = 1;
    } else {
        if (!g_loading_progress.visible)
            return;
        mode = 8;
    }
    unsigned long lock = _internal_halt_render_thread_and_lock_resources("..\\main\\main.cpp", 1387);
    c_rasterizer::begin_frame();
    rasterizer_set_default_render_targets();
    if (mode == 8) {
        if (!g_main_render_loading_globals.initialized)
            render_initialize_loading_screen();
        if (main_rasterizer_thread_owns_device() && g_main_render_loading_globals.owner_thread == get_current_thread_index())
            main_render_update_loading_progress((1.0f - g_loading_progress.progress) * 100.0f);
        main_render_draw_loading_screen();
    } else if (mode == 3) {
        main_render_draw_message_screen((const wchar_t *)wide_text);
    } else {
        unsigned short *source = wide_text;
        char *destination = text;
        long remaining = 12288;
        unsigned short c;
        do {
            c = *source;
            if (remaining == 1)
                *destination = 0;
            else if (c <= 127)
                *destination = (char)c;
            else
                *destination = '?';
            source++;
            destination++;
            remaining--;
        } while (c != 0 && remaining > 0);
        main_render_pregame((e_main_pregame_frame)mode, text);
    }
    if (mode != 2)
        main_time_throttle(0);
    c_rasterizer::end_frame();
    unlock_resources_and_resume_render_thread(lock);
}

// --- 0x1a8a08 support types (TU-private) ---
extern "C" {











}

// @address 0x1a8a08 ?main_loop_process_global_state_changes@@YAXXZ
void main_loop_process_global_state_changes()
{
    s_saved_film_snippet_camera film_views[4];

    bool finished_seek;

    if (main_globals.request3 != 0)
        main_globals.request3 = 0;
    main_update_startup_menu_launch();
    game_state_process_pending_changes();
    if ((unsigned char)main_has_pending_state_changes() == 0)
        return;

    unsigned long lock = (int)::_internal_halt_render_thread_and_lock_resources((char const *)"..\\main\\main.cpp", 758);

    if (main_globals.map_change_requested != 0)
    {
        main_globals.revert_requested = 1;
        main_globals.game_change_requested = 1;
        main_globals.zone_change_requested = 0;
        main_globals.map_change_requested = 0;
    }
    if (main_globals.revert_requested != 0)
    {
        unsigned long flags = 0;
        SET_BIT(flags, 0, main_globals.game_change_requested);
        SET_BIT(flags, 1, main_globals.zone_change_requested);
        campaign_metagame_update_scores_before_revert();
        game_state_revert(flags);
        campaign_metagame_update_scores_after_revert();
        c_datamine event(0, "checkpoint load", 1, "main");
        main_globals.game_change_requested = 0;
        main_globals.zone_change_requested = 0;
        main_globals.revert_requested = 0;
    }
    if (main_globals.save_requested != 0)
        main_reset_map_private();
    if (main_globals.request58 != 0)
        main_globals.request58 = 0;
    if ((unsigned char)saved_film_is_update_pending() != 0)
    {
        saved_film_get_player_views(&film_views);
        if (g_saved_film_timer_globals.game_state_load_pending)
        {
            saved_film_handle_game_state_load();
            g_saved_film_timer_globals.game_state_load_pending = false;
        }
        finished_seek = false;
        if (saved_film_has_pending_seek())
            saved_film_snippet_finished_seek(&finished_seek);
        if (!finished_seek)
            saved_film_snippet_set_cameras_from_states(&film_views);
    }
    if (main_globals.play_requested != 0)
    {
        game_state_load_from_compressed_buffer(&game_state_globals.compressed, 1, 1, 0);
        main_globals.play_requested = 0;
    }
    if (game_state_globals.compressed.acquire_requested != 0)
        game_state_try_and_get_compressed_buffer(&game_state_globals.compressed);
    if (main_globals.stop_requested != 0)
        main_globals.stop_requested = 0;
    if (main_globals.active_flag != 0)
    {
        if (scenario_activate_zone_set(main_globals.active_index) == 0)
            main_game_load_panic();
        main_globals.active_flag = 0;
        main_globals.active_index = -1;
    }
    if (main_globals.secondary_flag != 0)
        main_switch_zone_set_private();
    if (main_globals.dirty != 0)
        main_update_scenario_bits();
    if (main_globals.reset_requested != 0)
    {
        chud_messaging_special_save(false);
        if (game_in_progress())
        {
            game_state_save();
            c_datamine event(0, "checkpoint save", 1, "main");
        }
        main_globals.reset_requested = 0;
    }
    if (main_globals.shutdown_requested != 0)
        main_save_map_and_exit_private();
    if (main_globals.request5 != 0)
    {
        if (game_in_progress())
        {
            game_state_write_scratch_file((const char *)&main_globals.request7);
            main_globals.request5 = 0;
        }
    }
    if (main_globals.request6 != 0)
    {
        if (game_in_progress())
            main_globals.request6 = 0;
    }
    if (main_globals.campaign_progress_saved)
    {
        main_globals.campaign_progress_saved = false;
        if (game_in_progress())
            chud_messaging_special_progress(false);
    }
    if (main_globals.sync_requested != 0)
        main_globals.sync_requested = 0;
    bool busy = false;
    if (g_geometry_globals.manager != 0 && g_geometry_globals.manager->m_active_zone_changed != 0)
        busy = true;
    c_cache_file_tag_resource_runtime_manager *manager = g_geometry_globals.manager;
    if (busy && manager != 0 && manager->m_active_zone_changed)
    {
        manager->commit_active_zones();
        tag_resource_cache_new_commit_dirty(manager->get_resource_cache());
        manager->m_active_zone_changed = false;
        manager->get_resource_cache()->m_page_table.m_include_unreferenced = true;
        tag_resource_update_active_map_file(manager);
    }
    unlock_resources_and_resume_render_thread(lock);
}

#if defined(_XBOX) && !defined(__clang__)
STATIC_ASSERT(offsetof(c_minidump_file_output_secure, buffered_writer) == 0x120, main_online_buffered_writer_offset);
STATIC_ASSERT(offsetof(c_minidump_file_output_secure, encrypted_writer) == 0x1130, main_online_encrypted_writer_offset);
#endif

// @address 0x428d8 ??_7c_minidump_file_output_secure@@6B@
static __forceinline bool main_write_minidump_modules(s_main_block_writer *writer)
{
    bool result = false;
    if (writer->device->is_valid() && writer->source != NULL)
    {
        result = minidump_writer_write_module_list((s_minidump_writer *)writer);
        result = result && minidump_writer_write_module_names((s_minidump_writer *)writer);
    }
    return result;
}



#define const_seg data_seg
// @rdata 0x82022510..0x82022ba0
static float main_minidump_progress_pad0[3] = { 0.8f, 0.6f, 0.3f };
static float main_minidump_progress_start = 0.0f;
static float main_minidump_progress_pad1[416] = { 1.0f };
// @rdata-end
// @rdata 0x82022ba0..0x82022bb0
static float main_minidump_progress_pad2 = 16.0f;
static float main_minidump_progress_end = 100.0f;
static float main_minidump_progress_pad3[2] = { 2.0f, 6.0f };
// @rdata-end
#undef const_seg

static __forceinline void main_report_minidump_progress(s_main_block_writer *writer, const float &percent)
{
    if (writer->progress)
        writer->progress(percent);
}

// @address 0x2a7188 main_write_minidump_streams
extern "C" bool main_write_minidump_streams(s_main_block_writer *writer)
{
    bool result = false;
    if (writer->device->is_valid() && writer->source != NULL)
    {
        __try
        {
            writer->source->suspend_non_calling_threads();
            if (writer->source->minidump_begin(writer->context.exception_pointers))
            {
                main_report_minidump_progress(writer, main_minidump_progress_start);
                bool success = true;
                success = success && main_write_minidump_header(writer);
                success = success && main_write_minidump_directory(writer);
                success = success && minidump_writer_write_system_info((s_minidump_writer *)writer);
                success = success && minidump_writer_write_thread_list((s_minidump_writer *)writer);
                success = success && main_write_minidump_modules(writer);
                success = success && minidump_writer_write_exception_stream((s_minidump_writer *)writer);
                success = success && minidump_writer_write_version_block((s_minidump_writer *)writer);
                success = success && (unsigned char)minidump_writer_write_memory_list((s_minidump_writer *)writer);
                success = success && writer->device->seek(32);
                success = success && main_write_minidump_directory(writer);
                writer->source->minidump_end();
                result = success;
                main_report_minidump_progress(writer, main_minidump_progress_end);
            }
        }
        __finally
        {
            writer->source->resume_non_calling_threads();
        }
    }
    return result;
}

// @address 0x2a7050 main_write_minidump_file
// match: 0x2a7050: pointer __restrict qualifier prevents alias reload
extern "C" bool main_write_minidump_file(s_main_block_writer *writer, const char *path, s_main_writer_context *__restrict context)
{
    bool result = false;
    if (path && writer->source && context->exception_pointers) {
#if defined(_XBOX) && !defined(__clang__)
        c_minidump_file_output_secure upload;
        upload.m_open = false;
        upload.m_count = 0;
        upload.m_value = 0;
        upload.buffered_writer.buffer = NULL;
        upload.buffered_writer.buffer_size = 0;
        upload.buffered_writer.buffer_position = 0;
        memset(&upload.encrypted_writer, 0, offsetof(s_online_files_encrypted_writer, writer));
        upload.encrypted_writer.writer = NULL;
#else
        c_minidump_file_output_secure upload{};
#endif
        writer->device = &upload;
        writer->device->open(path);
        if (writer->device->is_valid()) {
            writer->context = *context;
            bool opened = main_write_minidump_streams(writer);
            bool finished = writer->device->close();
            result = opened && finished;
        }
        writer->device = NULL;
        data_compress_encrypted_writer_dispose(&upload.encrypted_writer);
    }
    return result;
}

// Crash-dump data shared with the exception handler.



// @address 0x1d70 ??_7c_retail_minidump_info_provider@@6B@
// @address 0x3e3c0 ??_7c_minidump_provider_interface@@6B@
// @address 0x2aa040 ??_Gc_retail_minidump_info_provider@@UAAPAXI@Z
// @address 0x2aa040 ??_Ec_retail_minidump_info_provider@@UAAPAXI@Z
// @address 0x2aa040 ??_Gc_minidump_provider_interface@@UAAPAXI@Z
// @address 0x2aa040 ??_Ec_minidump_provider_interface@@UAAPAXI@Z
// @address 0x2197d0 main_write_crash_minidump
extern "C" long main_write_crash_minidump(unsigned long flags, const char *path, long exception_code, s_thread_exception_record *data)
{
    u_main_callback_storage storage;
    EXCEPTION_RECORD exception_record;
    s_main_block_writer writer;

    writer.device = NULL;
    writer.source = NULL;
    writer.context.exception_pointers = NULL;
    writer.context.thread_id = 0;
    writer.context_rva = 0;
    writer.entry_count = 0;
    for (long i = 0; i < 8; i++)
    {
        writer.headers[i].data_size = 0;
        writer.headers[i].rva = 0;
        writer.headers[i].stream_type = 0;
    }
    writer.section_count = 0;
    for (long i = 0; i < 64; i++)
    {
        writer.sections[i].offset = 0;
        writer.sections[i].size = 0;
    }

    s_main_writer_context context = {NULL};
    EXCEPTION_POINTERS exception_pointers = {NULL};
    memset(&exception_record.ExceptionFlags, 0, sizeof(exception_record) - sizeof(exception_record.ExceptionCode));

    storage.callbacks[0] = xenon_minidump_get_game_state_region;
    storage.callbacks[1] = network_shared_memory_get_bounds;
    storage.callbacks[2] = xenon_minidump_get_geometry_data;
    storage.callbacks[3] = cache_file_try_and_get_memory_region;

    if (g_main_crash_reporting_enabled)
        flags = 5;

    bool test = ((flags >> 1) & 1) != 0;
    if (test && cache_files_enable_memory_protection)
        cache_files_enable_memory_protection = false;

    bool report_progress = ((flags >> 2) & 1) != 0;
    void (*progress)(float) = report_progress ? upload_debug_report_progress : NULL;

    context.exception_pointers = &exception_pointers;
    exception_record.ExceptionAddress = data->exception_address;
    exception_pointers.ContextRecord = &data->context;
    exception_record.ExceptionCode = data->exception_code;
    exception_pointers.ExceptionRecord = &exception_record;
    exception_record.ExceptionFlags = data->exception_flags;
    exception_record.NumberParameters = data->parameter_count;
    if (data->parameter_count != 0)
    {
        unsigned int size = data->parameter_count * 4;
        if (size <= sizeof(exception_record.ExceptionInformation))
            memcpy(exception_record.ExceptionInformation, data->parameters, size);
    }

    writer.byte_swap = 0;
    writer.progress = progress;
    context.thread_id = data->thread_id;
    writer.mode = test == 0;

    if (!test)
    {
        for (long i = 0; i < 4; i++)
        {
            s_main_block_section section = {0};
            t_main_region_callback callback = storage.callbacks[i];
            if (!callback(&section))
                continue;
            if ((unsigned long)section.offset != 0 && (unsigned long)section.size != 0 && writer.section_count < 64)
            {
                writer.sections[writer.section_count] = section;
                writer.section_count++;
            }
        }
    }

    if (!QueryPerformanceCounter((_LARGE_INTEGER *)&storage.counter))
        GetTickCount();

    unsigned long lock = _internal_halt_render_thread_and_lock_resources("..\\cseries\\xenon_minidump.cpp", 175);
    {
        c_retail_minidump_info_provider provider;
        writer.source = &provider;
        main_write_minidump_file(&writer, path, &context);
        writer.source = NULL;
    }
    if (!QueryPerformanceCounter((_LARGE_INTEGER *)&storage.counter))
        GetTickCount();
    unlock_resources_and_resume_render_thread(lock);
    return -1;
}

extern "C" {
// @address 0x72f782 g_main_resource_demand_update_enabled
bool g_main_resource_demand_update_enabled = true;

}

// @address 0xd30 ??_7c_main_render_demand_callback@@6B@

// @address 0x45fc50
unsigned long c_main_render_demand_callback::get_maximum_priority_count()
{
    return 10;
}

// @address 0x3fc550
unsigned long c_main_render_demand_callback::get_inactive_priority_count()
{
    return 4;
}


static __forceinline long main_demand_get_storage(c_cache_file_tag_resource_runtime_manager *&manager)
{
    if (g_main_resource_demand_update_enabled)
    {
        manager = g_geometry_globals.manager;
        if (manager->get_resource_cache()->m_controller.m_can_predict_optional)
            return true;
    }
    else
        manager = g_geometry_globals.manager;
    return false;
}

// @address 0x13f858 main_demand_update
extern "C" void main_demand_update()
{
    c_cache_file_tag_resource_runtime_manager *manager;
    const unsigned long &enabled = ((unsigned long)main_demand_get_storage(manager));
    long scenario_index = global_scenario_index;
    if ((unsigned char)enabled && scenario_index != -1)
    {
        c_main_render_demand_callback context(scenario_index, global_scenario_game_globals_index, global_zone_set_index, g_main_resource_demands_invalidated);
        s_tag_resource_cache_external_access_tables tables = { &manager->m_external_access, 0 };
        main_update_resource_demands(manager->get_resource_cache(),
            tables.original_table ? &tables : 0,
            &context);
        g_main_resource_demands_invalidated = false;
    }
}







extern "C" {

// @address 0x737703 g_catch_exceptions

// @address 0x8a1f74 g_suppress_upload_debug

// @address 0x119a484 g_main_last_ethernet_link_status










// match: 0x8aa70: SDK header adoption is blocked by this unit's current types/API arguments: '<unnamed-tag>' : 'enum' type redefinition.

// match: 0x589dd8: SDK header adoption is blocked by this unit's current types/API arguments: '<unnamed-tag>' : 'enum' type redefinition.

}

static __forceinline unsigned int main_crash_csstrnlen(const char *string, unsigned int size)
{
    unsigned int length = 0;
    while (length < size)
    {
        if (*string++ == 0)
            break;
        ++length;
    }
    return length;
}

static __forceinline char *main_crash_csstrnzcat(char *destination, const char *source, unsigned int size)
{
    unsigned int length = main_crash_csstrnlen(destination, size);
    char *result = strncpy(destination + length, source, size - length);
    destination[size - 1] = 0;
    return result;
}

static __forceinline unsigned long system_seconds()
{
    return (unsigned long)_time64(NULL);
}

static void main_crash_set_test_options()
{
    g_catch_exceptions = true;
    g_set_never_a_debugger_present = true;
    g_force_upload_even_if_untracked = true;
    g_suppress_keyboard_for_minidump = true;
    g_suppress_upload_debug = false;
    main_set_status_value("test-type", "%s", "default");
}

static __forceinline void network_webstats_force_reset()
{
    if (g_webstats_enabled && g_webstats_globals.upload_state != 0)
        upload_failed(true);
}

static __forceinline void network_webstats_update()
{
    if (g_webstats_enabled && !update_lsp() && g_webstats_globals.upload_state != 0)
        upload_failed(false);
}

static __forceinline void transport_global_update()
{
    if (transport_startup_state.initialized)
    {
        bool available = transport_startup_state.initialized;
        unsigned long link_status = XNetGetEthernetLinkStatus();
        if (g_main_last_ethernet_link_status != link_status)
            g_main_last_ethernet_link_status = link_status;
        if (!(link_status & 1))
            available = false;
        if (transport_startup_state.network_available != available)
        {
            transport_startup_state.network_available = available;
            if (available)
                transport_startup();
            else
                transport_shutdown();
        }
        transport_secure_address_resolve();
    }
}

static __forceinline bool main_crash_upload_start()
{
    g_main_crash_upload_progress.complete.set(0);
    g_main_crash_upload_progress.active.set(0);
    g_main_crash_upload_progress.failed.set(0);
    if (!g_suppress_upload_debug)
        main_crash_initialize_upload();
    return g_main_crash_upload_progress.active.peek() != 0;
}

static __forceinline bool main_crash_upload_complete(bool *out_success)
{
    if (out_success)
        *out_success = g_main_crash_upload_progress.complete.peek() ? g_main_crash_upload_progress.failed.peek() != 0 : false;
    return !g_main_crash_upload_progress.active.peek() && g_main_crash_upload_progress.complete.peek() == 1;
}


// main_halt_and_catch_fire: the spin is an inline-asm nop (unique in the image); with /Oz c2 keeps
// global optimisation for this asm function but skips the post-RA li dedup (FUN_108744d0)
// @address 0x1aa498 main_halt_and_spin_forever
extern "C" void main_halt_and_spin_forever(void)
{
    main_set_idle_recursion(true);
    if (!g_main_halt_recursion_lock)
    {
        bool upload_started;
        bool upload_complete;
        bool upload_success;
        bool keyboard_active;
        wchar_t keyboard_result[256];
        char text[0x6000];
        _XOVERLAPPED overlapped;
        c_static_string<256> user_description;

        main_set_idle_recursion(true);
        synchronization_leave_registered_sections();
        main_crash_initialize_report_directory();
        GetTickCount();
        upload_complete = false;
        upload_success = false;
        bool enabled = g_network_configuration.crash_reporting_enabled || g_main_crash_reporting_enabled;
        g_main_halt_recursion_lock = true;
        main_set_status_value("system_milliseconds", "time %d", GetTickCount());
        main_encode_status_file(NULL);
        font_cache_initialize();
        transport_initialize();
        input_clear_gamepads_rumble();
        main_progress_set_default_callbacks(NULL);
        saved_film_dispose();
        g_online_lsp_manager.go_into_crash_mode();
        network_webstats_force_reset();
        g_online_file_manager.set_upstream_quota(-1);
        if (g_main_halt_test_options)
            main_crash_set_test_options();

        user_description.clear();
        upload_started = main_crash_upload_start();
        keyboard_active = false;
        if (g_fake_minidump_creation)
        {
            keyboard_active = true;
        }
        else if (enabled && !g_suppress_upload_debug && !g_suppress_keyboard_for_minidump)
        {
            memset(&overlapped, 0, sizeof(overlapped));
            keyboard_active = online_guide_show_virtual_keyboard_ui(_controller0, 2, L"", L"Help us out!",
                                  L"Please enter a description of what you were doing right before this event occurred.  "
                                  L"You can plug a USB keyboard into your Xbox 360 to help enter text, or you can use your controller.",
                                  keyboard_result, sizeof(keyboard_result) / sizeof(keyboard_result[0]),
                                  &overlapped) == 997;
        }

        for (;;)
        {
            unsigned long lock = _internal_halt_render_thread_and_lock_resources("..\\main\\main.cpp", 2828);
            GetTickCount();
            if (enabled)
            {
                c_rasterizer::begin_frame();
                main_setup_targets_simple();
            }

            text[0] = 0;
            char upload_output[1024];
            if (upload_started && main_crash_get_upload_status(upload_output, sizeof(upload_output)))
                main_crash_csstrnzcat(text, upload_output, sizeof(text));

            if (keyboard_active)
            {
                if (g_fake_minidump_creation)
                {
                    c_static_string<256> crash_fast("crash fast");
                    keyboard_active = false;
                    main_crash_set_user_input(crash_fast.get_string(), crash_fast.length() + 1);
                }
                else if (overlapped.InternalLow != 997)
                {
                    keyboard_active = false;
                    if (XGetOverlappedResult(&overlapped, NULL, false) == 0)
                    {
                        long length = 0;
                        char ascii_result[256];
                        wchar_string_to_ascii_string(keyboard_result, ascii_result, sizeof(ascii_result), &length);
                        if (length > 1)
                            main_crash_set_user_input(ascii_result, length);
                    }
                }
            }

            e_main_pregame_frame pregame_frame = _main_pregame_frame_crash;
            if (upload_complete && upload_success)
                pregame_frame = _main_pregame_frame_crash_upload_succeeded;
            else if (!upload_complete)
            {
                if (system_seconds() & 1)
                    csnzappendf(text, sizeof(text), "\r\n\r\nOops, something bad happened!  Visit http://www.bungie.net for more information.");
                else
                    csnzappendf(text, sizeof(text),
                        "\r\n\r\nOops, something bad happened!  Visit http://www.bungie.net for more information.  (Please don't reboot)");
            }

            if (enabled)
            {
                main_render_pregame(pregame_frame, text);
                c_rasterizer::end_frame();
            }
            overlapped_update();
            input_update();
            g_online_lsp_manager.update();
            network_webstats_update();
            transport_global_update();
            if (upload_started && !upload_complete)
                upload_complete = main_crash_upload_complete(&upload_success);
            Sleep(0);
            unlock_resources_and_resume_render_thread(lock);
        }
        main_crash_set_user_input(user_description.get_string(), user_description.length() + 1);
    }

    if (!g_main_halt_exit_spin_disabled)
    {
        for (;;)
            __asm nop;
    }
    abort();
    main_set_idle_recursion(false);
}






extern "C" {















}

static const char *main_exception_registered_thread_name(long thread_index)
{
    return k_thread_default_definitions[thread_index].name;
}

static const char *main_exception_thread_name(unsigned long thread_id)
{
    const char *name = "unknown thread";
    for (unsigned long thread_index = 0; thread_index < 9; thread_index++)
    {
        if (g_thread_globals.thread_ids[thread_index] == thread_id)
        {
            name = main_exception_registered_thread_name(thread_index);
            break;
        }
    }
    return name;
}

static __forceinline void main_exception_build_screenshot_path(c_main_crash_report_path *path)
{
    ((c_static_string<260> *)&path->m_path)->print("%s%s%s", path->m_root.m_string, path->m_folder.m_string, "crash_screenshot.dds");
}

static __forceinline void main_exception_screenshot(bool release_sections)
{
    c_main_crash_report_path crash_report_path;
    if (release_sections)
        synchronization_leave_registered_sections();
    main_exception_build_screenshot_path(&crash_report_path);
    rasterizer_save_screenshot(crash_report_path.m_path.m_string, true);
}

// @address 0x141ea0 main_exception_handle_crash
extern "C" long main_exception_handle_crash(void)
{
    long result = 0;
    if (g_main_crash_data.state.peek())
    {
        main_set_idle_recursion(true);
        unsigned long exception_code = g_main_crash_data.exception_code;
        const char *exception_name = main_get_exception_name(exception_code);
        if (g_catch_exceptions)
        {
            if (exception_code != 0x73746b)
            {
                c_static_string<1156> text;
                const char *thread_name = main_exception_thread_name(g_main_crash_data.thread_id);
                unsigned long thread_id = g_main_crash_data.thread_id;
                _clearfp();
                synchronization_leave_registered_sections();
                main_crash_initialize_report_directory();
                void *exception_address = g_main_crash_data.exception_address;
                text.print("version:\r\n%s\r\n", "halo3 cache release xenon 11855.07.08.20.2317.halo3_ship  Aug 20 2007 23:19:46");
                text.append_print("thread information:\r\n thread_name: %s, thread_id: %08lx\r\n", thread_name, thread_id);
                if (exception_code == 0x61737274 && g_main_crash_data.parameter_count >= 4)
                {
                    const char *information = (const char *)g_main_crash_data.parameters[0];
                    const char *file = (const char *)g_main_crash_data.parameters[1];
                    long line = g_main_crash_data.parameters[2];
                    bool fatal = g_main_crash_data.parameters[3] != 0;
                    text.append_print("halt:\r\n%s at %s,#%d\r\n", fatal ? "### ASSERTION FAILED: " : "### RUNTIME WARNING: ", file, line);
                    if (information)
                        text.append_print("halt information:\r\n  %s\r\n", information);
                }
                else
                {
                    if (exception_name)
                    {
                        text.append_print("halt:\r\n### RUNTIME ERROR: %s at %08lX\r\n", exception_name, exception_address);
                        text.append_print("halt information:\r\n  (%s)\r\n", "");
                    }
                    else
                    {
                        text.append_print("halt:\r\n### RUNTIME ERROR: UNKNOWN EXCEPTION %08lX at %08lX\r\n", exception_code, exception_address);
                        text.append_print("halt information:\r\n  (%s)\r\n", "");
                    }
                    switch (exception_code)
                    {
                    case STATUS_ACCESS_VIOLATION:
                        if (g_main_crash_data.parameter_count >= 2)
                            text.append_print("crash:   tried to %s address %08lx\r\n",
                                g_main_crash_data.parameters[0] == 0 ? "read" : "write",
                                g_main_crash_data.parameters[1]);
                        break;
                    case _vc_exception_delayload_module_not_found:
                        if (g_main_crash_data.parameter_count >= 1)
                        {
                            const s_main_delay_load_information *information = (const s_main_delay_load_information *)g_main_crash_data.parameters[0];
                            text.append_print("  tried to load %s\r\n", information->library_name);
                        }
                        break;
                    }
                }
                main_exception_screenshot(false);
                main_crash_handle_exception(exception_code, &g_main_crash_data);
                main_halt_and_spin_forever();
                result = 1;
            }
            else
            {
                main_exception_screenshot(true);
                main_crash_handle_exception(exception_code, &g_main_crash_data);
                result = -1;
            }
        }
        else
            result = 0;
        g_main_crash_data.state.set(0);
        main_set_idle_recursion(false);
    }
    else
        result = 0;
    return result;
}

c_interlocked_long g_render_thread_enabled = c_interlocked_long(1);
c_interlocked_long g_rasterizer_device_reset_flag;
c_interlocked_long g_single_thread_request_flags;
s_main_flag_state main_globals;
t_restricted_allocation_manager<3, &main_thread_combine_timing_data, static_cast<void (*)(void *)>(0), &__tls_set_g_main_gamestate_timing_data_allocator>
    g_main_gamestate_timing_data_allocator;
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_render_timing_data_allocator> g_main_render_timing_data_allocator;
c_stop_watch g_main_status_timer;
c_stop_watch g_uber_startup_timer;
c_synchronized_long g_first_exception_flag;
s_thread_exception_record g_main_crash_data;


// @address 0x7d4f18 g_status_values
s_main_status_value g_status_values[32];
// @address 0x7db1e8 g_main_crash_report_globals
s_main_crash_report_globals g_main_crash_report_globals;
// @address 0x8a1ef3 disable_main_loop_throttle
#if !defined(_XBOX) || defined(__clang__)
bool disable_main_loop_throttle;
#endif
// @address 0x8a1efc g_force_upload_even_if_untracked
bool g_force_upload_even_if_untracked;
// @address 0x8a1efd g_fake_minidump_creation
bool g_fake_minidump_creation;
// @address 0x8a1efe g_suppress_keyboard_for_minidump
bool g_suppress_keyboard_for_minidump;
// @address 0x8a1f00 g_set_never_a_debugger_present
bool g_set_never_a_debugger_present;
// @address 0x8a1f03 g_main_halt_on_state_change
unsigned char g_main_halt_on_state_change;
// @address 0x8c29c6 g_main_crash_reporting_enabled
bool g_main_crash_reporting_enabled;
// @address 0x8c484f g_main_halt_requested
unsigned char g_main_halt_requested;
// @address 0x119a3e4 g_main_halt_recursion_lock
bool g_main_halt_recursion_lock;
// @address 0x119a3e5 g_main_halt_test_options
bool g_main_halt_test_options;
// @address 0x119a3e6 g_main_halt_exit_spin_disabled
bool g_main_halt_exit_spin_disabled;
// @address 0x119a3ec g_main_crash_recursion_lock
bool g_main_crash_recursion_lock;
// @address 0x119a40c g_main_resource_demands_invalidated
bool g_main_resource_demands_invalidated;
// @address 0x119a5c0 g_main_minidump_copy_buffer
char g_main_minidump_copy_buffer[1024];
