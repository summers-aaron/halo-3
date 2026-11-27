// blamlibxbox/bink_playback.obj: from the records 0x8e208 0x8e2a8 0x8e310 0x8e388 0x8e4c0 0x20ed10 0x20ed18 0x20ed20 0x20ed70 0x20ee18 0x20ee78 0x20eec0 0x20eee0 0x20ef48 0x20ef60 0x20ef68 0x20ef98 0x20f028 0x20f0c0 0x20f130 0x20f220 0x20f2d0 0x20f320 0x20f328 0x20f4b8 0x20f578 0x20f778 0x20fa18 0x20fd78 0x20ff60 0x210048 0x210130 0x210208 0x210300 0x2103a0 0x210510 0x210598 0x2106b8 0x210730 0x3a7ef0 0x3a7ff8 0x3a8c48 0x3a9150 0x47b838 0x47b9a8 0x711038
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "render/render_texture_camera_globals.h"
#include <time.h>
#define XVIDEO_MODE bink_playback_sdk_video_mode
#include <xtl.h>
#include "multithreading/thread_tls.h"
#undef XVIDEO_MODE
#include "math/real_math_types.h"
#include "math/real_math_constants.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "memory/datum_index.h"
#include "tag_files/tag_types.h"
#include "game/game_globals.h"
// @address 0x20ed18 ?deallocate@c_bink_allocation@@UAAXPAX@Z
// @address 0x20ed20 ?bink_get_memory_available@@YAXPBD_N@Z
// @address 0x20ee18 ?bink_playback_dispose@@YAXXZ
// @address 0x20eec0 bink_playback_set_recording_enabled
// @address 0x20ef48 bink_playback_clear_playing
// @address 0x20ef68 bink_playback_is_playing
// @address 0x20f028 bink_playback_get_time
// @address 0x20f2d0 ?set_mode@c_bink_game_state_listener@@UAAXJPAUs_game_state_buffer_description@@@Z
// @address 0x20f328 ?bink_alloc@@YAPAXI@Z
// @address 0x20f4b8 ?bink_free@@YAXPAX@Z
// @address 0x20ff60 ?bink_query_analog_controller_buttons@@YA_NXZ
// @address 0x210130 bink_playback_play_by_tag_index
// @address 0x210208 predict_bink_movie_fullpath
// @address 0x210300 bink_get_plane_allocation_description
// @address 0x210510 ?bink_thread_get_prediction_message@@YA_NPAUs_bink_runtime@@PAUs_bink_prediction_message@@@Z
// @address 0x210598 bink_playback_start_movie

#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"
#include "bink/bink_playback.h"
#include "bink/bink_sdk.h"
// @address 0x710070 BinkGetKeyFrame
#include "bink/bink_playback_externs.h"
#include "rasterizer/rasterizer_device_states.h"
// @address 0x96e760 g_game_state_procs
// @address 0x74b898 g_geometry_globals
// @address 0x73a248 ?render_globals@c_rasterizer@@2Us_rasterizer_render_globals@@A
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @address 0x6f34f0 XMPOverrideBackgroundMusic
// @address 0x6f35c8 XMPRestoreBackgroundMusic
// @address 0x59f738 XEnableScreenSaver
// @address 0x70dd64 XAudioGetSpeakerConfig
#include "saved_games/saved_film.h"
#include "bink/bink_playback_globals.h"
#include "bink/binkread.h"
#include "rasterizer/rasterizer_dx9_dynamic_geometry.h"
#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry.h"
#include "rasterizer/rasterizer_states.h"
#include "memory/data.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "main/main_game.h"
#include "math/color_math.h"
#include "multithreading/threads.h"
#include "rasterizer/rasterizer_main.h"
#include "saved_games/game_state_procs.h"
#include "saved_games/game_state_procs_types.h"
#include "tag_files/files.h"
#include "text/font_cache.h"
#include "cache/cache_files_windows.h"
#include "interface/chud/chud.h"
#include "main/main.h"
#include "input/input.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "bink/allocation_description_types.h"
#include "networking/online/online_globals_types.h"
#include "interface/user_interface_global_flags_types.h"
#include "tag_files/tag_resource_types.h"
#include "bink/bink_playback_types.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer_vertex_screen_types.h"




#include "bink/bink_texture_api.h"
// @address 0x59e3b0 GlobalMemoryStatus
// @address 0x8c29bc g_bink_allocation_count
#include "cache/restricted_memory_regions.h"
#include "ai/ai_orders.h"
_bink_globals bink_globals;

// @address 0x7c6008 g_bink_available_physical_memory
unsigned long g_bink_available_physical_memory = 0xFFFFFFFF;

// @address 0x7c6010 g_bink_pixel_shader_constants
real_vector4d g_bink_pixel_shader_constants[4] = {
    { 1.16412354f, 1.59579468f, 0.0f, -0.87065506f },
    { 1.16412354f, -0.81347656f, -0.39144897f, 0.52970505f },
    { 1.16412354f, 0.0f, 2.01782227f, -1.08166885f },
    { 1.0f, 0.0f, 0.0f, 0.03125f },
};

// @address 0x7c8740 g_bink_idle_movie_index
long g_bink_idle_movie_index = NONE;

// @address 0x7e86e0 g_bink_allocations
void *g_bink_allocations[16];

// @address 0x8c4928 g_bink_last_activity_time
unsigned long g_bink_last_activity_time;

#include "interface/user_interface_clock.h"

#include "bink/bink_allocation.h"
#include "tag_files/file_reference.h"
#include "bink/bink_file_requests.h"
#include "bink/bink_playback_requests.h"
#include "networking/tools/network_debug_dump.h"
#include "networking/tools/network_debug_dump_system.h"
#include <xam.h>
#include "game/game_time_access.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cache/geometry_storage.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "networking/online/online_globals_types.h"
#include "tag_files/tag_groups.h"
#include "bink/bink_texture_api.h"
#include "memory/contiguous_allocation.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "scenario/scenario.h"
#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry_globals.h"

struct s_bink_xmp_message_parameters
{
    unsigned long message;
    void *parameters[2];
};

struct playback_control_result { int value; };

struct s_stamp { __int64 now; __int64 pad2; unsigned long random[7]; };

struct s_bink_tag_definition
{
    long count;
    s_tag_resource resource;
};

typedef D3DLOCKED_RECT s_bink_locked_rect;

enum e_bink_playback_vertex_index
{
    _bink_playback_vertex_first = 0,
    k_bink_playback_vertex_count = 4
};

typedef t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_trigger_activations_allocator> t_bink_restricted_allocation_manager;

static __forceinline void bink_critical_section_leave(long index)
{
    s_critical_section_handle *object = &g_synch_globals.critical_sections[index];
    _InterlockedDecrement(&object->lock_count.value);
    RtlLeaveCriticalSection(&object->critical_section);
}

static __forceinline bool bink_prediction_active()
{
    return bink_globals.message.state != 0;
}
static __forceinline bool bink_predicting_movie(const char *path, long index)
{
    long match = 0;
    if (bink_prediction_active()) {
        if ((long)(bink_globals.message.flags & 0x1000) != 0)
            match = bink_globals.message.index == index;
        else
            match = bink_globals.message.name.is_equal(path);
    }
    if (match != 0)
        return true;
    return false;
}


// 0x20ed10
// @address 0x20ed10 ?allocate@c_bink_allocation@@UAAPAXIPBD@Z
void *c_bink_allocation::allocate(unsigned int size, const char *name)
{
    return bink_alloc(size);
}

// 0x20ed18
void c_bink_allocation::deallocate(void *address)
{
    bink_free(address);
}

// 0x20ed20
void bink_get_memory_available(const char *path, bool flag)
{
    MEMORYSTATUS status;
    memset(&status, 0, sizeof(status));
    status.dwLength = sizeof(status);
    GlobalMemoryStatus(&status);
    g_bink_available_physical_memory = status.dwAvailPhys;
}

// 0x20ed70 (CLEAN RECORD morph-0x20ed70.cpp, adapted to TU decls)
// @address 0x20ed70 ?bink_playback_initialize@@YAXXZ
void bink_playback_initialize()
{
    g_game_state_procs.entries[1].handler = &g_bink_game_state_listener;
    int *prediction_frames = (int *)g_trigger_activations_allocation.reserve_memory("bink game state", 0, 8, 0);
    *prediction_frames = 0;
    int **tls = &g_bink_prediction_frames;
    *tls = prediction_frames;
    memset(&bink_globals, 0, sizeof(bink_globals));
    BinkSetMemory(bink_alloc, bink_free);
    bink_globals.active = true;
}

// 0x20ee18
void bink_playback_dispose(void)
{
    if (bink_globals.active) {
        bink_playback_stop_and_cleanup();
        memset(&bink_globals, 0, sizeof(bink_globals));
    }
    g_game_state_procs.entries[1].handler = 0;
}

// 0x20eec0
extern "C" void bink_playback_set_recording_enabled(e_map_memory_configuration configuration)
{
    bink_globals.recording_enabled = (configuration - 1 == 0);
}

// @address 0x20eee0 bink_playback_cancel_prediction
extern "C" void bink_playback_cancel_prediction(void)
{
    s_bink_prediction_message message;
    bink_thread_get_prediction_message(&bink_globals.runtime, &message);
    bink_playback_stop_movie();
    if (bink_globals.load_procs_pending != 0) {
        ::game_state_call_after_load_procs(1, bink_globals.pending_load_procs);
        bink_globals.pending_load_procs = 0;
        bink_globals.load_procs_pending = 0;
    }
}

// 0x20ef48
extern "C" void bink_playback_clear_playing(void)
{
    bink_globals.playing = 0;
}

// @address 0x20ef60 ?bink_playback_dispose_from_old_map@@YAXXZ
void bink_playback_dispose_from_old_map()
{
    bink_playback_cancel_prediction();
}

// 0x20ef68
extern "C" bool bink_playback_is_playing(void)
{
    return bink_globals.active && bink_globals.enabled;
}


static __forceinline bool bink_playback_playing_unpaused()
{
    return bink_globals.active && bink_globals.enabled && (bink_globals.control_flags & 0x1000) == 0;
}

// @address 0x20ef98 bink_playback_render_first_frame
extern "C" void bink_playback_render_first_frame(void)
{
    bool playing = bink_globals.active && bink_globals.enabled;
    if (!playing)
        return;
    if (!bink_playback_playing_unpaused())
        return;
    if (bink_globals.runtime.frame_index != 0)
        return;
    if (bink_globals.runtime.frames_decoded != 0)
        return;
    bink_playback_render_centered_frame();
}


// 0x20f028
extern "C" float bink_playback_get_time(void)
{
    float result = k_real_zero_storage;
    bool playing = bink_globals.active && bink_globals.enabled;
    if (playing) {
        s_bink_info *info = bink_globals.runtime.bink;
        if (info->frame_rate_divisor != 0)
            result = (float)info->frame_rate * (float)info->current_frame / (float)info->frame_rate_divisor;
    }
    return result;
}


// @address 0x20f0c0 bink_playback_close_movie
extern "C" void bink_playback_close_movie(s_bink_runtime *runtime)
{
    if (runtime->bink) {
        for (long i = 0; i < 2; i++) {
            if (runtime->video.texture_sets[i].fence_pending)
                D3DDevice_BlockOnFence(runtime->video.texture_sets[i].fence);
            bink_free(runtime->video.texture_memory[i]);
        }
        BinkClose(runtime->bink);
        runtime->bink = 0;
    }
}


// @address 0x20f130 bink_playback_stop_movie
extern "C" void bink_playback_stop_movie(void)
{
    if (bink_globals.active) {
        s_bink_runtime *runtime = &bink_globals.runtime;
        if (runtime->thread_running) {
            runtime->exit_requested = 1;
            __lwsync();
            while (!runtime->thread_finished)
                __lwsync();
        }
        runtime->playing = false;
        runtime->thread_finished = 0;
        runtime->thread_running = 0;
        runtime->frame_index = 0;
        runtime->frames_decoded = 0;
        runtime->frames_skipped = 0;
        runtime->exit_requested = 0;
        runtime->playback_requested = 0;
        runtime->stop_forever_requested = 0;
        bink_playback_close_movie(runtime);
        bink_globals.message.state = 0;
        if (bink_globals.memory_pool) {
            bink_globals.memory_pool = 0;
            bink_globals.memory_pool_size = 0;
            if (bink_globals.load_procs_pending) {
                ::game_state_call_after_load_procs(1, bink_globals.pending_load_procs);
                bink_globals.pending_load_procs = 0;
                bink_globals.load_procs_pending = 0;
            }
        }
        bink_globals.control_flags = 0;
        bink_globals.enabled = false;
    }
    XMPRestoreBackgroundMusic();
    XEnableScreenSaver(1);
}

// @address 0x20f220 bink_playback_stop_and_cleanup
extern "C" void bink_playback_stop_and_cleanup(void)
{
    bool result = false;
    bool active = bink_globals.active && bink_globals.enabled;
    if (active) {
        unsigned long control_flags = bink_globals.control_flags;
        bink_playback_stop_movie();
        if ((_rotl(control_flags, 27) & 1) != 0)
            main_menu_launch();
        long milliseconds = g_user_interface_globals.milliseconds.peek();
        s_render_texture_camera_globals *globals = g_render_texture_camera_globals;
        g_bink_last_activity_time = milliseconds;
        if (globals->dynamic_lights) {
            chud_set_texture_cam_enabled(0);
            globals->enabled = result;
            globals->dynamic_lights = result;
        }
    }
    bink_globals.playing = result;
}

// 0x20f2d0
void c_bink_game_state_listener::set_mode(long mode, s_game_state_buffer_description *description)
{
    if (mode == 1) {
        description->type = 2;
        description->size = 0x1d00000;
        return;
    }
    if (mode == 6) {
        description->type = 2;
        description->size = 0x8fffe0;
        return;
    }
    description->size = 0;
    description->type = 0;
}

// @address 0x20f320 bink_playback_abort_movie
extern "C" void bink_playback_abort_movie(void)
{
    bink_playback_stop_movie();
}

// @address 0x7614cc g_bink_game_state_listener
// PC build only: on Xbox update() is the shared empty body at 0x5bec98, dispose() is
// bink_playback_abort_movie itself, and the image holds the object's vtable word (0x3e298).
#if !defined(_XBOX) || defined(__clang__)
void c_bink_game_state_listener::update(void *)
{
}

void c_bink_game_state_listener::dispose(void *)
{
    bink_playback_abort_movie();
}

c_bink_game_state_listener g_bink_game_state_listener;
#endif

void bink_free(void *memory_block)
{
    MEMORYSTATUS status;
    memset(&status, 0, sizeof(status));
    status.dwLength = sizeof(status);
    GlobalMemoryStatus(&status);
    g_bink_available_physical_memory = status.dwAvailPhys;
    for (long i = 0; i < g_bink_allocation_count; i++) {
        if (g_bink_allocations[i] == memory_block) {
            g_bink_allocations[i] = 0;
            break;
        }
    }
    memset(&status, 0, sizeof(status));
    status.dwLength = sizeof(status);
    GlobalMemoryStatus(&status);
    g_bink_available_physical_memory = status.dwAvailPhys;
}

// 0x20ff60
bool bink_query_analog_controller_buttons(void)
{
    bool pressed = false;
    int buttons[10] = {10, 11, 12, 13, 14, 15, 0, 1, 6, 7};
    if (bink_globals.control_flags & 1024) {
        if (input_has_button_pressed(10))
            pressed = true;
    } else {
        for (unsigned int index = 0; index < 10; index = index + 1) {
            if (input_has_button_pressed(buttons[index])) {
                pressed = true;
                break;
            }
        }
    }
    return pressed;
}

// 0x210048
// @address 0x210048 bink_playback_play_by_name
extern "C" void bink_playback_play_by_name(long index)
{
    c_static_string<256> path;
    path.print("d:\\bink\\%s_%d.bik", index, 60);
    bool playing = bink_globals.active && bink_globals.enabled;
    if (playing)
        bink_playback_stop_and_cleanup();
    playing = bink_globals.active && bink_globals.enabled;
    if (!playing && predict_bink_movie_fullpath(path.get_string(), -1, 30)) {
        bink_prediction_update();
        bink_globals.runtime.playback_requested = 1;
        bink_playback_start_movie(30);
    }
}

// 0x210130
extern "C" void bink_playback_play_by_tag_index(long index)
{
    bool playing = bink_globals.active && bink_globals.enabled;
    if (playing)
        bink_playback_stop_and_cleanup();
    playing = bink_globals.active && bink_globals.enabled;
    if (!playing && predict_bink_movie_fullpath("", index, 0x1000)) {
        bink_prediction_update();
        bink_globals.runtime.playback_requested = 1;
        bink_playback_start_movie(0x1000);
    }
}

extern "C" bool predict_bink_movie_fullpath(const char *path, long index, unsigned long flags)
{
    bool success = false;
    if (bink_prediction_active()) {
        if (bink_predicting_movie(path, index))
            success = true;
    } else {
        success = true;
        c_static_string<256> name(path);
        bink_globals.message.name = name;
        bink_globals.message.index = index;
        bink_globals.message.recording = bink_globals.recording_enabled && (flags & 128) != 0;
        bink_globals.message.state = 1;
        bink_globals.message.flags = flags;
    }
    return success;
}


extern "C" s_allocation_description bink_get_plane_allocation_description(void *texture, BINKPLANE *plane, bool force_allocate)
{
    unsigned long size = 0;
    if (plane->Allocate || force_allocate) {
        XGTEXTURE_DESC description;
        XGGetTextureDesc((D3DBaseTexture *)texture, 0, &description);
        plane->BufferPitch = description.RowPitch;
        size = description.Height * description.RowPitch;
    } else {
        plane->BufferPitch = 0;
    }
    s_allocation_description result;
    result.destination = &plane->Buffer;
    result.element_size = 1;
    result.element_count = size;
    result.size = size + 0xfff;
    result.alignment_bits = 12;
    return result;
}

// @address 0x2103a0 bink_prediction_update
extern "C" void bink_prediction_update(void)
{
    if (bink_globals.message.state != 1)
        return;
    bool active = bink_globals.active && bink_globals.enabled;
    if (active)
        return;
    internal_critical_section_enter(33);
    if (bink_globals.runtime.queued_prediction_message.state == 0) {
        unsigned long size = bink_globals.recording_enabled ? 0x1d00000 : 0x8fffe0;
        bink_globals.memory_pool_size = size;
        bink_globals.memory_pool_offset = 0;
        bink_globals.memory_pool = 0;
        void *token = game_state_procs_try_and_get_slot(1, 0, size, &g_bink_game_state_listener);
        if (token != 0) {
            bink_globals.pending_load_procs = token;
            bink_globals.load_procs_pending = size;
        }
        if (bink_globals.load_procs_pending >= bink_globals.memory_pool_size)
            bink_globals.memory_pool = static_cast<char *>(bink_globals.pending_load_procs);
        if (bink_globals.memory_pool != 0) {
            bink_globals.runtime.queued_prediction_message = bink_globals.message;
            if (prepare_bink_movie_for_playback_async(&bink_globals.runtime, &bink_globals.runtime.queued_prediction_message))
                bink_globals.message.state = 0;
            else
                bink_playback_stop_movie();
            bink_globals.runtime.queued_prediction_message.state = 0;
        }
    }
    bink_critical_section_leave(33);
}

static __forceinline bool bink_all_pointers_freed() {
    bool all_freed = true;
    for (short i = 0; i < g_bink_allocation_count; i++)
        if (g_bink_allocations[i]) all_freed = false;
    return all_freed;
}
void *bink_alloc(unsigned int size) {
    void *result = 0;
    MEMORYSTATUS status;
    memset(&status, 0, sizeof(status));
    status.dwLength = sizeof(status);
    GlobalMemoryStatus(&status);
    g_bink_available_physical_memory = status.dwAvailPhys;
    if (g_bink_allocation_count > 0 && g_bink_allocations[0] == 0) {
        if (bink_all_pointers_freed()) {
            g_bink_allocation_count = 0;
            bink_globals.memory_pool_offset = 0;
        }
    }
    bool failed = false;
    failed = failed || (bink_globals.memory_pool_offset + size > (unsigned long)bink_globals.memory_pool_size);
    failed = failed || g_bink_allocation_count >= 16;
    failed = failed || bink_globals.memory_pool == 0;
    if (!failed) {
        result = bink_globals.memory_pool + bink_globals.memory_pool_offset;
        bink_globals.memory_pool_offset += size;
        memset(result, 0, size);
        g_bink_allocations[g_bink_allocation_count] = result;
        g_bink_allocation_count++;
        bink_get_memory_available("end bink_alloc", false);
        long slack = 0x30000;
        bink_globals.memory_pool_offset += slack;
        if (bink_globals.memory_pool_offset > bink_globals.memory_pool_size)
            bink_globals.memory_pool_offset = bink_globals.memory_pool_size;
    }
    return result;
}

// @address 0x47b9a8 bink_playback_get_movie_path
extern "C" bool bink_playback_get_movie_path(int movie, c_static_string<256> *name)
{
    const char *movie_name;
    switch (movie) {
        case 3: movie_name = "intro"; break;
        case 4: movie_name = "credits"; break;
        case 0: movie_name = "attract_1"; break;
        case 1: movie_name = "attract_2"; break;
        case 2: movie_name = "attract_3"; break;
        // match: 0x47b9a8: drops the cmplwi 4/bgt range check before the jump table and the movie_name stack slot
        default: __assume(0);
    }
    name->print("d:\\bink\\%s_%d.bik", movie_name, 60);
    return true;
}




bool bink_thread_get_prediction_message(s_bink_runtime *runtime, s_bink_prediction_message *message)
{
    bool result = false;
    internal_critical_section_enter(33);
    s_bink_prediction_message *queued = &runtime->queued_prediction_message;
    if (queued->state == _bink_prediction_message_queued)
    {
        *message = *queued;
        result = true;
        queued->state = _bink_prediction_message_none;
    }
    bink_critical_section_leave(33);
    return result;
}


// @address 0x3a7ef0 ?netdebug_initialize@@YA_NPBD00@Z
bool netdebug_initialize(const char *project_name, const char *build_version, const char *system_name)
{
    char *stored_number_text;
    char formatted_number_buffer[256];
    char *table = (char *)&g_netdebug_globals;
    strncpy(g_netdebug_globals.project, project_name, 64);
    g_netdebug_globals.project[63] = 0;
    strncpy(g_netdebug_globals.build_version, build_version, 64);
    g_netdebug_globals.build_version[63] = 0;
    strncpy(g_netdebug_globals.system_name, system_name, 160);
    g_netdebug_globals.system_name[159] = 0;
    table[8668] = 0;
    network_debug_dump_initialize_system_name();
    strncpy(g_netdebug_globals.flash_version, "UNKNOWN", 0x100);
    g_netdebug_globals.flash_version[255] = 0;
    formatted_number_buffer[0] = 0;
    memset(formatted_number_buffer + 1, 0, sizeof(formatted_number_buffer) - 1);
    csnzprintf(formatted_number_buffer, 0x100, "%d", 0x17f4);
    stored_number_text = strncpy(g_netdebug_globals.xdk_version, formatted_number_buffer, 0x100);
    g_netdebug_globals.xdk_version[255] = 0;
    bink_playback_build_log_filename(stored_number_text);
    main_set_status_value("session_id", "%s", g_netdebug_globals.session_id);
    start_thread(4);
    g_netdebug_globals.enabled = true;
    return true;
}

// @address 0x3a7ff8 bink_playback_queue_file_request
extern "C" void bink_playback_queue_file_request(const char *, const char *path, void (*progress)(long, long),
    void (*completion)(bool, void *), void *)
{
    bool success = false;
    if (g_netdebug_globals.enabled) {
        internal_critical_section_enter(4);
        s_bink_file_request request;
        memset(&request.stream_name, 0, sizeof(request) - 1);
        request.stream_name.clear();
        request.progress_callback = progress;
        request.completion_callback = completion;
        request.callback_data = NULL;
        request.active = true;
        file_reference_create_from_path(&request.file, path, false);
        bool found = false;
        for (long index = 0; index < 10; index++) {
            if (!g_netdebug_globals.requests[index].active) {
                memcpy(&g_netdebug_globals.requests[index], &request, sizeof(request));
                found = true;
                break;
            }
        }
        if (found)
            success = true;
        internal_critical_section_leave(4);
    }
    if (!success && completion != NULL)
        completion(false, NULL);
}


// 0x8e208
static __forceinline bool bink_playback_is_active()
{
    return bink_globals.active != 0 && bink_globals.enabled != 0;
}
// @address 0x8e208 bink_is_idle_and_drive_empty
extern "C" bool bink_is_idle_and_drive_empty(void)
{
    bool result = false;
    playback_control_result control;
    control.value = 0;
    if ((unsigned int)bink_xmp_query_drive_empty((int *)&control) == 0)
        result = control.value == 0 && !bink_playback_is_active();
    return result;
}

// 0x8e2a8
// match: 0x8e2a8: /Os keeps the bink_xmp_send_message call out of line (frame 0x70, no inlined parameters block at 0x80)
#pragma optimize("s", on)
// @address 0x8e2a8 bink_xmp_query_drive_empty
extern "C" unsigned long bink_xmp_query_drive_empty(int *empty)
{
    long bounds[4];
    unsigned long result = bink_xmp_send_message(2, &bounds[0], &bounds[1]);
    if (result == 0)
        *empty = bounds[0] == 0 && bounds[1] == 0;
    return result;
}
// match: 0x8e2a8: ends the /Os region opened for bink_xmp_query_drive_empty
#pragma optimize("", on)

// 0x8e310
// match: 0x8e310: /Os emits cmpwi on cr0 and one shared epilogue branch instead of cr6 and an early blr
#pragma optimize("s", on)
// @address 0x8e310 bink_xmp_send_message
extern "C" unsigned long bink_xmp_send_message(unsigned long message, void *parameter0, void *parameter1)
{
    s_bink_xmp_message_parameters parameters;
    parameters.message = message;
    parameters.parameters[0] = parameter0;
    parameters.parameters[1] = parameter1;
    long result = XMsgInProcessCall((HXAMAPP)0xfa, 0x7001b, (UINT_PTR)&parameters, 0);
    if (result >= 0)
        return 0;
    if ((result & 0x1fff0000) == 0x70000)
        return result & 0xffff;
    return 1627;
}
// match: 0x8e310: ends the /Os region opened for bink_xmp_send_message
#pragma optimize("", on)

// 0x210598
extern "C" void bink_playback_start_movie(unsigned int flags)
{
    if (!bink_globals.active || !bink_globals.runtime.bink)
        return;
    if ((flags & 0x100) != 0)
        bink_globals.ready = false;
    else {
        bool ready = true;
        if (cache_files_windows_get_active_copy_progress(0) != 0)
            ready = false;
        bink_globals.ready = ready;
    }
    bink_globals.control_flags = flags;
    if ((flags & 4) != 0) {
        bink_get_memory_available("begin event_manager", false);
        bink_get_memory_available("end event_manager", false);
    }
    for (long i = 0; i < 2; i++)
        bink_globals.runtime.video.started[i] = true;
    *g_bink_prediction_frames = -1;
    unsigned long stored_flags = bink_globals.control_flags;
    bink_globals.runtime.playback_rate = 1.0f;
    bink_globals.enabled = true;
    if ((stored_flags & 0x200) != 0)
        bink_globals.start_tick = game_time_globals->time;
    else
        bink_globals.start_tick = GetTickCount();
    XMPOverrideBackgroundMusic();
    if ((flags & 0x400) != 0)
        XEnableScreenSaver(0);
    cache_files_windows_get_active_copy_progress(0);
}
// @address 0x2106b8 bink_playback_play_movie
extern "C" void bink_playback_play_movie(const char *path, long index, unsigned long flags)
{
    if (((flags & 0x4000) == 0 || bink_globals.recording_enabled) &&
        predict_bink_movie_fullpath(path, index, flags)) {
        bink_prediction_update();
        bink_globals.runtime.playback_requested = 1;
        bink_playback_start_movie(flags);
    }
}

// 0x20ee78 (identical: tu-check 0 words, bus 0 words)
// @address 0x20ee78 bink_playback_handle_after_load
extern "C" void bink_playback_handle_after_load(unsigned long, unsigned long load_flags)
{
    if (load_flags != 0)
        return;
    bool is_active = bink_globals.active != 0 && bink_globals.enabled != 0;
    if (!is_active)
        return;
    bink_playback_cancel_prediction();
}

// 0x47b838 (identical: tu-check 0 words)
// @address 0x47b838 bink_playback_play_idle_movie
extern "C" void bink_playback_play_idle_movie(void)
{
    long state = g_bink_idle_movie_index;
    long index;
    if (state == -1) {
        random_seed_next(get_local_random_seed_address());
        unsigned short low = (unsigned short)(*get_local_random_seed_address() >> 16);
        index = (short)(((((unsigned int)low << 1) | ((unsigned int)low >> 31)) + low) >> 16);
    } else {
        random_seed_next(get_local_random_seed_address());
        index = ((short)(((unsigned short)(*get_local_random_seed_address() >> 16) >> 15) + 1) + state) % 3;
    }
    if (index < 0 || index >= 3)
        index = 0;
    g_bink_idle_movie_index = index;
    c_static_string<256> name;
    if (bink_playback_get_movie_path(index, &name) && predict_bink_movie_fullpath(name.get_string(), -1, 30)) {
        bink_prediction_update();
        bink_globals.runtime.playback_requested = 1;
        bink_playback_start_movie(30);
    }
    bool active = bink_globals.active && bink_globals.enabled;
    if (!active) {
        unsigned long milliseconds = g_user_interface_globals.milliseconds.peek();
        g_bink_last_activity_time = milliseconds;
    }
}

static __forceinline unsigned long bink_playback_sequenced_remainder(unsigned long number, unsigned long divisor)
{
    unsigned long result = divisor ? number % divisor : 0UL;
    return result;
}
// @address 0x3a8c48 bink_playback_format_timestamp
extern "C" void bink_playback_format_timestamp(void)
{
    s_stamp stamp;
    stamp.now = _time64(0);
    char *string = _ctime64(&stamp.now);
    struct tm *local = _localtime64(&stamp.now);
    if (string != 0 && strlen(string) == 25 && local->tm_year + 1900 >= 2006) {
        char *month = string + 4;
        if (month[0] == 'J' && month[1] == 'a' && month[2] == 'n') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '1';
        } else if (month[0] == 'F' && month[1] == 'e' && month[2] == 'b') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '2';
        } else if (month[0] == 'M' && month[1] == 'a' && month[2] == 'r') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '3';
        } else if (month[0] == 'A' && month[1] == 'p' && month[2] == 'r') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '4';
        } else if (month[0] == 'M' && month[1] == 'a' && month[2] == 'y') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '5';
        } else if (month[0] == 'J' && month[1] == 'u' && month[2] == 'n') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '6';
        } else if (month[0] == 'J' && month[1] == 'u' && month[2] == 'l') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '7';
        } else if (month[0] == 'A' && month[1] == 'u' && month[2] == 'g') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '8';
        } else if (month[0] == 'S' && month[1] == 'e' && month[2] == 'p') {
            month[0] = '_';
            month[1] = '0';
            month[2] = '9';
        } else if (month[0] == 'O' && month[1] == 'c' && month[2] == 't') {
            month[0] = '_';
            month[1] = '1';
            month[2] = '0';
        } else if (month[0] == 'N' && month[1] == 'o' && month[2] == 'v') {
            month[0] = '_';
            month[1] = '1';
            month[2] = '1';
        } else if (month[0] == 'D' && month[1] == 'e' && month[2] == 'c') {
            month[0] = '_';
            month[1] = '1';
            month[2] = '2';
        }
        char *text = g_netdebug_globals.session_id;
        text[0] = string[0];
        text[1] = string[1];
        text[2] = string[3];
        text[3] = string[4];
        text[4] = string[5];
        text[5] = string[6];
        text[6] = string[8];
        text[7] = string[9];
        text[8] = string[20];
        text[9] = string[21];
        text[10] = string[22];
        text[11] = string[23];
        text[12] = '_';
        text[13] = string[11];
        text[14] = string[12];
        text[15] = string[14];
        text[16] = string[15];
        text[17] = string[17];
        text[18] = string[18];
        text[19] = 0;
        if (text[2] == ' ') {
            char first = text[0];
            char second = text[1];
            if ((first == 'S' && second == 'u') || (first == 'M' && second == 'o'))
                text[2] = 'n';
            else if (first == 'T' && second == 'u')
                text[2] = 'e';
            else if (first == 'W' && second == 'e')
                text[2] = 'd';
            else if (first == 'T' && second == 'h')
                text[2] = 'u';
            else if (first == 'F' && second == 'r')
                text[2] = 'i';
            else if (first == 'S' && second == 'a')
                text[2] = 't';
        }
    } else {
        XNetRandom((unsigned char *)stamp.random, 28);
        unsigned long v[6];
        long index = 0;
        v[index] = bink_playback_sequenced_remainder(stamp.random[index], (index == 2 ? 10000 : 100));
        ++index;
        v[index] = bink_playback_sequenced_remainder(stamp.random[index], (index == 2 ? 10000 : 100));
        ++index;
        v[index] = bink_playback_sequenced_remainder(stamp.random[index], (index == 2 ? 10000 : 100));
        ++index;
        v[index] = bink_playback_sequenced_remainder(stamp.random[index], (index == 2 ? 10000 : 100));
        ++index;
        v[index] = stamp.random[index] % (index == 2 ? 10000 : 100);
        ++index;
        v[index] = stamp.random[index] % (index == 2 ? 10000 : 100);
        ++index;
        csnzprintf(g_netdebug_globals.session_id, 128, "FAK_%02d%02d%04d_%02d%02d%02d", (v[0] ? v[0] : 0UL), (v[1] ? v[1] : 0UL), (v[2] ? v[2] : 0UL),
            (v[3] ? v[3] : 0UL), v[4], (v[5] ? v[5] : 0UL));
    }
}


static __forceinline unsigned long static_string_length(const char *string)
{
    unsigned long length;
    for (length = 0; length < 128; length++) {
        if (*string++ == '\0')
            break;
    }
    return length;
}

// @address 0x3a9150 bink_playback_build_log_filename
extern "C" void bink_playback_build_log_filename(const char *)
{
    bink_playback_format_timestamp();
    unsigned long length = static_string_length(g_netdebug_globals.session_id);
    strncpy(g_netdebug_globals.session_id + length, "@", 128 - length);
    g_netdebug_globals.session_id[127] = '\0';
    length = static_string_length(g_netdebug_globals.session_id);
    strncpy(g_netdebug_globals.session_id + length, g_netdebug_globals.system_name, 128 - length);
    g_netdebug_globals.session_id[127] = '\0';
}







static __forceinline void bink_playback_initialize_plane_description(
    s_allocation_description *description, void *texture, BINKPLANE *plane, bool force_allocate)
{
    typedef void (*get_description)(s_allocation_description *, void *, BINKPLANE *, bool);
    ((get_description)bink_get_plane_allocation_description)(description, texture, plane, force_allocate);
}

static __forceinline void bink_playback_texture_header(s_bink_texture *texture, unsigned long width, unsigned long height)
{
    XGSetTextureHeader(width, height, 1, 4, (D3DFORMAT)0x28000002, D3DPOOL_DEFAULT, 0, -1, 0, texture, 0, 0);
}

static __forceinline void bink_playback_clear_alpha(void *data, const unsigned long &size)
{
    memset(data, -1, size);
}

// @address 0x8e4c0 ?prepare_bink_movie_for_playback_async@@YA_NPAUs_bink_runtime@@PBUs_bink_prediction_message@@@Z
bool prepare_bink_movie_for_playback_async(s_bink_runtime *runtime, const s_bink_prediction_message *message)
{
    s_bink_runtime *state = (s_bink_runtime *)runtime;
    _bink_globals *globals = &bink_globals;
    bool success = false;
    if (globals->memory_pool) {
        bink_get_memory_available("begin BinkOpen", false);
        // match: 0x8e4c0: __restrict hoists the lbz of request->recording above the BinkOpenXAudio address load
        const s_bink_prediction_message *__restrict request = message;
        state->decoded_texture = 0;
        state->have_decoded_texture = false;
        state->thread_started = request->recording;
        state->next_frame = 1;
        BinkSetSoundSystem(BinkOpenXAudio, 0);
        // match: 0x8e4c0: the sound-track list and the allocation list share one stack slot (a local union, s_bink_needs_unions);
        // a union member cannot have constructors, so the list is the _no_init form
        union { int values[3]; c_wrapped_array_no_init<s_allocation_description> list; } tracks = {{0, 1, 2}};
        unsigned long speakers[2];
        s_bink_locked_rect locked;
        BinkSetSoundTrack(3, tracks.values);
        const void *data = message;
        unsigned long flags = 0x104400;
        if (state->thread_started)
            flags = 0x106400;
        globals->movie_tag_index = -1;
        globals->movie_data = 0;
        if ((long)(message->flags & 0x1000) != 0) {
            long index = message->index;
            if (index != -1) {
                s_bink_tag_definition *definition = (s_bink_tag_definition *)tag_get(0, index);
                if (definition) {
                    const s_tag_resource *resource = &definition->resource;
                    s_tag_data *available = (s_tag_data *)tag_resource_available(resource);
                    if (available) {
                        data = available->address;
                        globals->movie_tag_index = index;
                        flags |= 0xc000000;
                        globals->movie_data = available->address;
                        globals->movie_resource_flags = 
                            g_geometry_globals.manager->m_resource_cache.m_render_access.m_per_thread_access_cache[get_current_thread_index()]
                            ->m_resource_pages.begin()[datum_index_to_absolute_index(resource->resource_handle)];
                    }
                }
            }
        }
        state->bink = BinkOpen(data, flags);
        bink_get_memory_available("end BinkOpen", false);
        if (state->bink) {
            success = true;
            XAudioGetSpeakerConfig(speakers);
            tracks.values[0] = 0; tracks.values[1] = 1;
            BinkSetMixBins(state->bink, 0, tracks.values, 2);
            tracks.values[1] = 5; tracks.values[0] = 4;
            BinkSetMixBins(state->bink, 1, tracks.values, 2);
            tracks.values[0] = 2; tracks.values[1] = 3;
            BinkSetMixBins(state->bink, 2, tracks.values, 2);
            state->movie_width = state->bink->width;
            state->movie_height = state->bink->height;
            s_bink_frame_buffers *buffers = &state->video.buffers;
            BinkGetFrameBuffersInfo(state->bink, buffers);
            for (long i = 0; i < state->video.buffers.count; i++) {
                s_bink_prepare_texture_set *textures = &state->video.texture_sets[i];
                textures->fence_pending = false;
                bink_playback_texture_header(&textures->y, state->video.buffers.y_width, state->video.buffers.y_height);
                XGOffsetResourceAddress(&textures->y, state->video.buffers.frames[i].y.Buffer);
                if (state->video.buffers.frames[i].alpha.Allocate) {
                    bink_playback_texture_header(&textures->alpha, state->video.buffers.y_width, state->video.buffers.y_height);
                    XGOffsetResourceAddress(&textures->alpha, state->video.buffers.frames[i].alpha.Buffer);
                } else if ((long)(message->flags & 0x100) != 0) {
                    bink_playback_texture_header(&textures->alpha, 16, 16);
                    XGOffsetResourceAddress(&textures->alpha, state->video.buffers.frames[i].alpha.Buffer);
                }
                bink_playback_texture_header(&textures->cr, state->video.buffers.chroma_width, state->video.buffers.chroma_height);
                XGOffsetResourceAddress(&textures->cr, state->video.buffers.frames[i].cr.Buffer);
                bink_playback_texture_header(&textures->cb, state->video.buffers.chroma_width, state->video.buffers.chroma_height);
                XGOffsetResourceAddress(&textures->cb, state->video.buffers.frames[i].cb.Buffer);
            }
            c_bink_allocation allocator;
            for (long i = 0; success && i < state->video.buffers.count; i++) {
                s_bink_frame_planes *planes = &state->video.buffers.frames[i];
                s_bink_prepare_texture_set *textures = &state->video.texture_sets[i];
                bool force_allocate = (message->flags & 0x100) != 0;
                s_allocation_description descriptions[4];
                bink_playback_initialize_plane_description(&descriptions[0], &textures->y, &planes->y, force_allocate);
                bink_playback_initialize_plane_description(&descriptions[1], &textures->cr, &planes->cr, force_allocate);
                bink_playback_initialize_plane_description(&descriptions[2], &textures->cb, &planes->cb, force_allocate);
                bink_playback_initialize_plane_description(&descriptions[3], &textures->alpha, &planes->alpha, force_allocate);
                tracks.list.set_elements(descriptions, 4);
                // match: 0x8e4c0: the by-value list is copied through a pointer to the union member
                c_wrapped_array<s_allocation_description> *list = static_cast<c_wrapped_array<s_allocation_description> *>(&tracks.list);
                success = allocate_list_contiguous(&allocator, *list, "bink!", &state->video.texture_memory[i],
                    &state->video.texture_memory_size[i]);
            }
            if (success) {
                for (long i = 0; i < state->video.buffers.count; i++) {
                    s_bink_prepare_texture_set *textures = &state->video.texture_sets[i];
                    XGOffsetResourceAddress(&textures->y, state->video.buffers.frames[i].y.Buffer);
                    if (state->video.buffers.frames[i].alpha.Allocate) {
                        XGOffsetResourceAddress(&textures->alpha, state->video.buffers.frames[i].alpha.Buffer);
                    } else if ((long)(message->flags & 0x100) != 0) {
                        XGOffsetResourceAddress(&textures->alpha, state->video.buffers.frames[i].alpha.Buffer);
                        XGTEXTURE_DESC description;
                        XGGetTextureDesc(&textures->alpha, 0, &description);
                        unsigned long size = description.RowPitch * description.Height;
                        D3DTexture_LockRect(&textures->alpha, 0, &locked, 0, 0);
                        bink_playback_clear_alpha(locked.pBits, size);
                        D3DTexture_UnlockRect(&textures->alpha, 0);
                    }
                    XGOffsetResourceAddress(&textures->cr, state->video.buffers.frames[i].cr.Buffer);
                    XGOffsetResourceAddress(&textures->cb, state->video.buffers.frames[i].cb.Buffer);
                }
                bink_register_frame_buffers(state->bink, buffers);
            } else {
                bink_playback_close_movie(runtime);
                state->bink = 0;
            }
        }
    }
    return success;
}

#pragma code_seg(push, "BINK")
// @address 0x711038 bink_thread_work_function
extern "C" void bink_thread_work_function(s_bink_info *bink, unsigned long frame, long flags)
{
    if (bink) {
        if (!frame)
            frame = 1;
        bink->seeking = 1;
        bink->seek_time = 0;
        if (frame > bink->frame_count)
            frame = bink->frame_count;
        unsigned long start;
        if (!(flags & 2) && bink->has_sound) {
            unsigned long lead = (bink->frame_rate_divisor + bink->frame_rate - 1) / bink->frame_rate;
            if (lead >= frame)
                start = 1;
            else
                start = frame - lead;
        } else {
            start = frame;
        }
        unsigned long current = bink->current_frame;
        if (current != frame) {
            if ((flags & 1) && (flags & 2)) {
                GotoFrame(bink, frame);
            } else {
                unsigned long key;
                if (flags & 1)
                    key = frame;
                else
                    key = BinkGetKeyFrame(bink, frame, 0);
                long restore_sound;
                unsigned long seek;
                if (key > start) {
                    restore_sound = bink->sound_enabled;
                    bink->sound_enabled = 0;
                    seek = start;
                } else {
                    seek = key;
                    restore_sound = 0;
                }
                if (frame < current || seek > current) {
                    GotoFrame(bink, seek);
                    if (seek == frame) {
                        bink->seeking = 0;
                        return;
                    }
                }
                {
                    long *callback = &bink->sound_callback;
                    RADCB_suspend_callback(g_bink_sound_thread, callback);
                    long sound_on = bink->sound_on;
                    if (sound_on)
                        BinkSetSoundOnOff(bink, 0);
                    if (bink->current_frame != bink->decoded_frame) {
                        BinkDoFrame(bink);
                        bink->frame_decoded = 1;
                    }
                    BinkNextFrame(bink);
                    while (bink->current_frame != frame) {
                        if (restore_sound && bink->current_frame == key) {
                            restore_sound = 0;
                            bink->sound_enabled = 1;
                        }
                        BinkDoFrame(bink);
                        bink->frame_decoded = 1;
                        BinkNextFrame(bink);
                    }
                    bink->frame_time = 0;
                    bink->sound_time = 0;
                    if (restore_sound)
                        bink->sound_enabled = 1;
                    RADCB_resume_callback(g_bink_sound_thread, callback);
                    if (sound_on)
                        BinkSetSoundOnOff(bink, 1);
                }
            }
        }
        bink->seeking = 0;
    }
}
#pragma code_seg(pop)

static __forceinline long bink_playback_next_decompression_frame(const s_bink_video_state *frame)
{
    long index = frame->buffers.current_frame + 1;
    if (index >= frame->buffers.count || !frame->texture_memory[index]) index = 0;
    return index;
}
// @address 0x20f578 ?bink_try_to_decompress_frame@@YAXPAUs_bink_runtime@@@Z
void bink_try_to_decompress_frame(s_bink_runtime *runtime)
{
    s_bink_runtime *frame = runtime;
    s_bink_video_state *video = &runtime->video;
    _bink_globals *globals = &bink_globals;
    long index = bink_playback_next_decompression_frame(video);
    if (index != -1 && globals->runtime.video.started[index] && !BinkWait(runtime->bink)) {
        long next = bink_playback_next_decompression_frame(video);
        if (video->texture_memory[next] && video->texture_sets[next].fence_pending)
            D3DDevice_BlockOnFence(video->texture_sets[next].fence);
        bool decompressed = false;
        if ((globals->control_flags & 0x200) && frame->next_frame >= runtime->bink->current_frame + 1) {
            bink_thread_work_function(runtime->bink, frame->next_frame - 1, 0);
            decompressed = true;
        } else if (!(globals->control_flags & 0x200) || frame->next_frame >= runtime->bink->current_frame) {
            BinkDoFrame(runtime->bink);
            if ((globals->control_flags & 1) || (runtime->bink && runtime->bink->current_frame != runtime->bink->frame_count))
                BinkNextFrame(runtime->bink);
            decompressed = true;
        }
        if (decompressed) {
            char *data = (char *)video->texture_memory[index];
            unsigned long size = video->texture_memory_size[index];
            for (unsigned long offset = 0; offset < size; offset += 128) {
                __dcbst(0, data);
                data += 128;
            }
            if (!(globals->control_flags & 0x100)) {
                int *prediction = g_bink_prediction_frames;
                prediction[0] = frame->video.buffers.current_frame;
                prediction[1] = runtime->bink->current_frame;
            }
            runtime->video.started[index] = false;
        }
    }
}



struct D3DDevice;
struct D3DBaseTexture;
struct D3DTexture;




// @address 0x737504 g_tiled_render_x_scale
// @address 0x737508 g_tiled_render_y_scale
// @address 0x8c4830 g_tiled_render_x_offset
// @address 0x8c4834 g_tiled_render_y_offset


static __forceinline long bink_playback_vertex_index(c_enum<e_bink_playback_vertex_index, long, 0, 4> index)
{
    return index;
}

// @address 0x20fa18 bink_playback_render_rect
extern "C" void bink_playback_render_rect(const rectangle2d *rect)
{
    long frame;
    if (bink_globals.control_flags & 0x100)
        frame = bink_globals.runtime.video.buffers.current_frame;
    else
        frame = *g_bink_prediction_frames;
    bool playing = bink_globals.active && bink_globals.enabled;
    if (playing && (frame != -1) == true)
    {
        s_bink_runtime *prepare = &bink_globals.runtime;
        for (long i = 0; i < NUMBEROF(bink_globals.runtime.video.started); i++)
        {
            if (!bink_globals.runtime.video.started[i] && i != frame)
            {
                __lwsync();
                _InterlockedExchange(&prepare->prediction_state_lock.value, i);
            }
        }
        rasterizer_vertex_screen vertices[4];
        for (long i = 0; i < 4; i++)
        {
            long edge = (i + 1) & 2;
            float u = edge != 0 ? 1.0f : 0.0f;
            float v = i > 1 ? 1.0f : 0.0f;
            float x;
            if (edge != 0)
            {
                __int64 sx = rect->x1;
                x = (float)sx;
            }
            else
            {
                __int64 sx = rect->x0;
                x = (float)sx;
            }
            float y;
            if (i > 1)
            {
                __int64 sy = rect->y1;
                y = (float)sy;
            }
            else
            {
                __int64 sy = rect->y0;
                y = (float)sy;
            }
            vertices[i].x = x;
            vertices[i].y = (float)y;
            vertices[i].u = u;
            vertices[i].v = v;
            vertices[i].color = -1;
        }
        bink_playback_bind_frame_textures(frame);
        short width = (short)c_rasterizer::render_globals.width;
        short height = (short)c_rasterizer::render_globals.height;
        short display_width;
        short display_height;
        if (rasterizer_main_get_video_mode()->fIsWideScreen != 0)
        {
            display_width = (short)c_rasterizer::render_globals.width;
            display_height = (short)c_rasterizer::render_globals.height;
        }
        else
        {
            display_width = 640;
            display_height = 480;
        }
        __int64 width64 = width;
        __int64 height64 = height;
        float inv_width = 1.0f / (float)width64;
        float inv_height = 1.0f / (float)height64;
        float x_scale = inv_width * 2.0f;
        float y_scale = inv_height * 2.0f;
        float x_offset = inv_width * -0.0f;
        float y_offset = inv_height * 0.0f;
        long count = 3;
        do
        {
            long index = bink_playback_vertex_index((e_bink_playback_vertex_index)count);
            vertices[index].x = (vertices[index].x * g_tiled_render_x_scale + g_tiled_render_x_offset - 0.5f) * x_scale - 1.0f + x_offset;
            vertices[index].y = y_offset - ((vertices[index].y * g_tiled_render_y_scale + g_tiled_render_y_offset - 0.5f) * y_scale - 1.0f);
            count--;
        } while (count >= 0);
        c_rasterizer::draw_textured_screen_quad(vertices, false);
        if ((bink_globals.control_flags & 0x100) == 0 && (bink_globals.control_flags & 0x400) == 0)
        {
            real_argb_color color;
            color.red = 0.0f;
            color.green = 0.0f;
            color.blue = 0.0f;
            color.alpha = 1.0f - bink_globals.runtime.playback_rate;
            point2d points[4];
            points[0].x = 0;
            points[0].y = 0;
            points[1].x = display_width;
            points[1].y = 0;
            points[2].x = display_width;
            points[2].y = display_height;
            points[3].x = 0;
            points[3].y = display_height;
            unsigned long pixel = real_argb_color_to_pixel32(&color);
            rasterizer_quad_screenspace(points, pixel, 0, 0, 0);
        }
        c_rasterizer::clear_sampler_textures(15);
        bink_globals.runtime.video.texture_sets[frame].fence = D3DDevice_InsertFence(g_rasterizer_device);
        bink_globals.runtime.video.texture_sets[frame].fence_pending = true;
    }
}

// @address 0x20fd78 bink_playback_render_centered_frame
extern "C" void bink_playback_render_centered_frame(void)
{
    unsigned long control_flags = bink_globals.control_flags;
    bool skip = false;
    if (TEST_BIT(control_flags, 9) && bink_globals.runtime.frames_decoded != 0)
        skip = true;
    if (skip)
        return;
    short display_width = (short)c_rasterizer::render_globals.width;
    short display_height = (short)c_rasterizer::render_globals.height;
    float scaled_width;
    float scaled_height;
    if (TEST_BIT(control_flags, 4)) {
        float movie_aspect = (float)bink_globals.runtime.movie_width / (float)bink_globals.runtime.movie_height;
        float video_aspect = rasterizer_main_get_video_aspect_ratio();
        scaled_width = (float)display_width;
        scaled_height = (float)display_height;
        if (movie_aspect > video_aspect)
            scaled_width = scaled_width / video_aspect * movie_aspect;
        else if (movie_aspect < video_aspect)
            scaled_height = scaled_height / movie_aspect * video_aspect;
    } else {
        scaled_width = (float)bink_globals.runtime.movie_width;
        scaled_height = (float)bink_globals.runtime.movie_height;
        if (rasterizer_main_get_video_mode()->fIsWideScreen == 0)
            scaled_height = scaled_height * 0.75f;
    }
    rectangle2d rect;
    rect.x0 = (short)(((float)display_width - scaled_width) * 0.5f);
    rect.x1 = (short)(((float)display_width + scaled_width) * 0.5f);
    rect.y0 = (short)(((float)display_height - scaled_height) * 0.5f);
    rect.y1 = (short)(((float)display_height + scaled_height) * 0.5f);
    bink_playback_render_rect(&rect);
}



static __forceinline float bink_playback_fade_fraction(float elapsed)
{
    return 1.0f - elapsed / 0.3f;
}

static __forceinline void bink_playback_set_fade_volume(s_bink_runtime *runtime, float value)
{
    if (MAX(value, 0.0f) < 1.0f) {
        if (value > 0.0f)
            runtime->playback_rate = value;
        else
            runtime->playback_rate = 0.0f;
    } else
        runtime->playback_rate = 1.0f;
}

static __forceinline bool bink_playback_update_active()
{
    long enabled = bink_globals.enabled != 0;
    return (enabled != 0) && true;
}

static __forceinline c_tag_resource_cache_file_access_cache *bink_playback_current_resource_table()
{
    return g_geometry_globals.manager->m_resource_cache.m_render_access.m_per_thread_access_cache[get_current_thread_index()];
}

static __forceinline bool bink_playback_resource_present(const c_tag_resource_cache_file_access_cache *table, long handle)
{
    bool present = false;
    if (handle != -1)
        present = table->m_resource_handles.begin()[datum_index_to_absolute_index(handle)] + 1 != 0;
    return present;
}

static __forceinline bool bink_playback_resource_key_matches(const c_tag_resource_cache_file_access_cache *table, unsigned __int64 key)
{
    return table->m_resource_pages.begin()[(key >> 15) & 0x7fff] == key;
}

// @address 0x210730 bink_playback_update_frame
extern "C" void bink_playback_update_frame(long unused)
{
    _bink_globals *globals = &bink_globals;
    s_bink_runtime *runtime = &bink_globals.runtime;
    if (!bink_globals.active || !bink_playback_update_active() || runtime->frame_index != 0)
        return;
    bool available = false;
    if (!TEST_BIT(bink_globals.control_flags, 12))
        available = true;
    else if (globals->movie_tag_index != -1)
    {
        s_bink_tag_definition *definition = (s_bink_tag_definition *)tag_get(0, globals->movie_tag_index);
        if (definition)
        {
            long handle = definition->resource.resource_handle;
            const s_tag_resource *resource = &definition->resource;
            c_tag_resource_cache_file_access_cache *table = bink_playback_current_resource_table();
            available = table && bink_playback_resource_present(table, handle);
            if (available)
            {
                s_tag_data *data = (s_tag_data *)tag_resource_available(resource);
                bool resource_matches = true;
                if (data)
                {
                    unsigned __int64 *key = &globals->movie_resource_flags;
                    unsigned __int64 *end = &globals->movie_resource_flags + 1;
                    do {
                        unsigned __int64 value = *key;
                        resource_matches = bink_playback_resource_key_matches(table, value);
                        if (!resource_matches)
                            break;
                        ++key;
                    } while (
#if defined(__clang__)
                        key < end
#else
                        // match: 0x210730: signed cmpw on the key pointer; a plain pointer compare emits cmplw
                        (long)key < (long)end
#endif
                    );
                }
                if (data && !resource_matches)
                {
                    s_bink_info *fields = runtime->bink;
                    char *old_base = (char *)globals->movie_data;
                    char *new_base = (char *)data->address;
                    fields->data = fields->data - old_base + new_base;
                    fields->audio0 = fields->audio0 - old_base + new_base;
                    fields->plane0 = fields->plane0 - old_base + new_base;
                    fields->plane1 = fields->plane1 - old_base + new_base;
                    fields->plane2 = fields->plane2 - old_base + new_base;
                    fields->audio1 = fields->audio1 - old_base + new_base;
                    globals->movie_data = data->address;
                    globals->movie_resource_flags = 
                        g_geometry_globals.manager->m_resource_cache.m_render_access.m_per_thread_access_cache[get_current_thread_index()]
                            ->m_resource_pages.begin()[datum_index_to_absolute_index(resource->resource_handle)];
                }
            }
        }
    }
    if (available)
    {
        s_bink_runtime *prepare = &globals->runtime;
        if (TEST_BIT(bink_globals.control_flags, 9))
            prepare->next_frame = game_time_globals->time - (long)bink_globals.start_tick + 1;
        else
            prepare->next_frame = (long)prepare->next_frame + 1;
        if (runtime->playing)
        {
            float time = bink_playback_get_time();
            float floored = MAX(bink_playback_fade_fraction(time - runtime->fade_start_time), 0.0f);
            float volume = floored < 1.0f ? MAX(floored, 0.0f) : 1.0f;
            if (runtime->bink)
            {
                long level = (long)(PIN(volume, 0.0f, 1.0f) * 32768.0f);
                for (long i = 0; i < 3; ++i)
                    bink_set_volume(runtime->bink, i, level);
            }
            bink_playback_set_fade_volume(runtime, volume);
            if (volume <= 0.0f)
                runtime->frame_index = 1;
        }
        bool skip = false;
        if (TEST_BIT(bink_globals.control_flags, 1) && (!TEST_BIT(bink_globals.control_flags, 7) || (bink_globals.ready && bink_playback_get_time() > 5.0f)))
            skip = bink_query_analog_controller_buttons() || (online_guide_is_active() && !TEST_BIT(bink_globals.control_flags, 10));
        if (skip && !runtime->playing)
        {
            runtime->playing = true;
            runtime->fade_start_time = bink_playback_get_time();
            if (TEST_BIT(bink_globals.control_flags, 13))
                main_menu_launch();
        }
        else if ((!runtime->bink || runtime->bink->current_frame == runtime->bink->frame_count) && !TEST_BIT(bink_globals.control_flags, 0))
            runtime->frame_index = 1;
        else
            bink_try_to_decompress_frame(runtime);
        if (TEST_BIT(bink_globals.control_flags, 8) || !bink_globals.active || !bink_playback_update_active() || runtime->frame_index == 0)
            return;
    }
    bink_playback_stop_and_cleanup();
}
class c_interlocked_long;
static __forceinline bool bink_playback_is_main_thread()
{
    return g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 1;
}

// @address 0x8e388 bink_playback_update
extern "C" void bink_playback_update(void)
{
    if (bink_playback_is_main_thread())
    {
        s_bink_prediction_message message;
        s_bink_runtime *runtime = &bink_globals.runtime;
        __lwsync();
        long index = _InterlockedExchange(&runtime->prediction_state_lock.value, -1);
        if (index != -1)
        {
            long frame;
            if (bink_globals.control_flags & 0x100)
                frame = bink_globals.runtime.video.buffers.current_frame;
            else
                frame = *g_bink_prediction_frames;
            if (index != frame && index >= 0 && (unsigned long)index < 2)
                bink_globals.runtime.video.started[index] = true;
        }
        if (bink_thread_get_prediction_message(&bink_globals.runtime, &message))
        {
            if (!prepare_bink_movie_for_playback_async(&bink_globals.runtime, &message))
            {
                bink_globals.message.state = 0;
                bink_playback_stop_movie();
            }
            bink_playback_start_movie(message.flags);
        }
        bink_prediction_update();
        bink_playback_update_frame(0);
    }
}

struct D3DPixelShader;
struct D3DVertexShader;
struct D3DBaseTexture;
// @address 0x836908 g_rasterizer_shared_shader_cache
// match: 0x20f778: c_rasterizer::set_sampler_filter_mode (0x167cc8) is expanded inline at these four sites only;
// this is its body.
static __forceinline void bink_playback_set_sampler_filter_mode(long sampler, c_rasterizer::e_sampler_filter_mode mode)
{
    if (g_rasterizer_sampler_filter_modes[sampler] != mode)
    {
        if (VALID_INDEX(sampler, NUMBEROF(g_rasterizer_sampler_filter_modes)))
            g_rasterizer_sampler_filter_modes[sampler] = mode;
        if ((unsigned long)mode <= 7)
            rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), sampler, mode, mode);
    }
}


// @address 0x20f778 bink_playback_bind_frame_textures
extern "C" void bink_playback_bind_frame_textures(long index)
{
    s_bink_runtime *runtime = &bink_globals.runtime;
    s_bink_frame_buffers *frame_buffers = &runtime->video.buffers;
    D3DDevice_SetPixelShader(c_rasterizer::get_device(), g_rasterizer_shared_shader_cache.pixel_shaders[1]);
    D3DDevice_SetVertexShader(c_rasterizer::get_device(), g_rasterizer_shared_shader_cache.vertex_shaders[1]);

    c_rasterizer::set_sampler_texture_direct(0, &bink_globals.runtime.video.texture_sets[index].y);
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    bink_playback_set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer::set_sampler_texture_direct(1, &bink_globals.runtime.video.texture_sets[index].cr);
    c_rasterizer::set_sampler_address_mode(1, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    bink_playback_set_sampler_filter_mode(1, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer::set_sampler_texture_direct(2, &bink_globals.runtime.video.texture_sets[index].cb);
    c_rasterizer::set_sampler_address_mode(2, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    bink_playback_set_sampler_filter_mode(2, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer::e_alpha_blend_mode alpha_blend_mode = c_rasterizer::_alpha_blend_alpha_blend;
    if (frame_buffers->frames[index].alpha.Allocate != 0 || TEST_BIT(bink_globals.control_flags, 8))
    {
        // match: 0x20f778: rotated byte offset puts add r5 (texture address) before li r4,3; plain &alpha swaps them
        const long &frame_offset = (long)_rotl(index * (long)sizeof(s_bink_prepare_texture_set), 0);
        // match: 0x20f778: second rotation of the byte offset; with one rotation add r5 still follows li r4,3
        D3DTexture *alpha_texture = reinterpret_cast<D3DTexture *>(reinterpret_cast<char *>(&runtime->video.texture_sets[0].alpha) + _rotl(frame_offset, 0));
        c_rasterizer::set_sampler_texture_direct(3, alpha_texture);
    }
    else
    {
        const c_rasterizer_globals *globals;
        long tag_index = global_game_globals->rasterizer_globals_ref.index;
        if (tag_index != -1)
            globals = (const c_rasterizer_globals *)tag_get(0, tag_index);
        else
            globals = 0;
        const c_rasterizer_texture_ref texture_ref = globals->get_default_texture_rasterizer_texture_ref(0);
        c_rasterizer::set_sampler_texture(3, texture_ref);
    }
    c_rasterizer::set_sampler_address_mode(3, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    bink_playback_set_sampler_filter_mode(3, c_rasterizer::_sampler_filter_mode_bilinear);

    float alpha_multiplier = c_rasterizer::get_render_target_alpha_multiplier(0);
    real_vector4d *constants = g_bink_pixel_shader_constants;
    unsigned long control_flags = bink_globals.control_flags;
    constants[3].w = alpha_multiplier;
    if (TEST_BIT(control_flags, 8) && !TEST_BIT(control_flags, 10))
        constants[3].w = bink_globals.runtime.playback_rate * alpha_multiplier;
    D3DDevice_SetPixelShaderConstantFN(c_rasterizer::get_device(), 3, constants[0].n, 4, (unsigned __int64)3 << 62);
    c_rasterizer::set_alpha_blend_mode(alpha_blend_mode);
}

t_bink_restricted_allocation_manager g_trigger_activations_allocation;
