// blamlibxbox/shell_xenon.obj: from the records 0x197c70 0x197f30 0x1982e8 0x198338 0x198490 0x198670 0x1988a8 0x198970 0x198a18 0x1aeb68
// @flags /TP /O2 /Oc
// @shared_bss 0x828a1ee0..0x828a1ef8
// @address 0x8a1eb8 g_shell_allocator_hook
// @address 0x7db1d8 g_shell_timing_globals
// @address 0x96e760 g_game_state_procs
// @address 0x74b80c g_runtime_state_allocation
// @address 0x7d1da0 input_globals

/* The game's XMemFree hook: frees through XMemFreeDefault unless the block belongs to an allocator the game
   handles itself (allocator id 130 with physical, protect-2 memory, or the id registered in g_shell_allocator_hook). */

#include <xtl.h>
#include <stdlib.h>
#include "memory/runtime_state_allocation.h"
#include "memory/physical_memory_map_types.h"
#include "memory/physical_memory_map.h"
#include "multithreading/synchronization.h"
#include "bink/bink_playback.h"
#include "cseries/async.h"
#include "input/input_xenon.h"
#include "main/game_preferences.h"
#include "main/main.h"
#include "multithreading/threads.h"
#include "saved_games/game_state.h"
#include "sound/sound_manager.h"
#include "text/font_cache.h"
#include "text/font_group.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "cache/cache_files_windows.h"
#include "memory/data.h"
#include "cseries/cseries.h"
#include "shell/shell_xenon.h"
#include "memory/data_types.h"
#include "text/font_cache_globals_types.h"
#include "shell/shell_runtime_dependencies.h"
#include "cache/geometry_storage.h"
#include "cache/cache_file_tag_resource_runtime_manager_storage.h"
#include "cseries/placement_new.h"
#include "input/input_globals.h"
#include "cseries/cseries_windows.h"
#include "cseries/pointer_utilities.h"
#include <excpt.h>
#include <string.h>
#include "shell/shell_runtime_globals.h"
#include "cache/cache_file_runtime_globals.h"
#include <ppcintrinsics.h>
#include <float.h>
#include "multithreading/xenon_intrinsics.h"
#include "memory/runtime_state.h"
#include "text/font_package_types.h"
#include "text/font_cache_internal_types.h"
#include "networking/tools/network_debug_dump.h"
#include "multithreading/synchronization_types.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_dynamic_render_target_types.h"
#include "main/thread_globals_lock_inlines.h"
#include "saved_games/game_state_procs_externs.h"
#include "saved_games/game_state_procs_types.h"
extern "C" {
#include "sound/sound_xaudio_xenon_imports.h"
#include "cseries/xenon_platform_api.h"
}

s_runtime_state_globals g_runtime_state_globals;

struct s_shell_tag_resource_physical_globals { void *listener; };
struct s_shell_font_cache_thread_state { signed char active; };
#if defined(_XBOX) && !defined(__clang__)
static bool g_shell_floating_point_exceptions_enabled;
static void *g_shell_havok_memory;
#else
/* PC-port definitions: with external linkage each use loads through r11, but
   the static form above lets this file's .bss accesses share one r27 base. */
extern "C" {
bool g_shell_floating_point_exceptions_enabled;
void *g_shell_havok_memory;
}
#endif

// @address 0x93ddf0 g_font_globals
// @address 0x96e7b8 g_font_cache_channels
// @address 0x8c46bd g_rasterizer_main_initialized

struct D3DDevice;

class c_shell_font_cache_scope_lock
{
public:
    __forceinline c_shell_font_cache_scope_lock()
    {
        m_state.active = 0;
        if (g_font_cache_globals == 0) {
            internal_critical_section_enter(37);
            m_state.active = 1;
            g_font_cache_globals = &g_internal_font_cache_globals;
        }
    }
    __forceinline ~c_shell_font_cache_scope_lock() { font_cache_unlock(&m_state); }
private:
    s_shell_font_cache_thread_state m_state;
};

class c_shell_physical_memory_stage
{
public:
    __forceinline void initialize_buffer(const s_shell_memory_buffer &buffer)
    {
        m_buffers[0] = buffer;
        m_buffers[1] = buffer;
    }
    __forceinline void consume(unsigned long bytes)
    {
        m_next += bytes;
        m_remaining -= bytes;
    }

    __forceinline void *allocate(unsigned long size, long alignment_bits)
    {
        char *next = m_next;
        char *address = (char *)align_pointer(next, alignment_bits);
        unsigned long padding = address - next;
        consume(padding);
        consume(size);
        return address;
    }

private:
    union {
        struct { void *m_base; unsigned long m_size; char *m_next; unsigned long m_remaining; };
        s_shell_memory_buffer m_buffers[2];
    };
};

// @address 0x949a98 g_shell_physical_memory_stage

enum memory_stage;

// @address 0x1988a8 shell_alloc_memory
void *shell_alloc_memory(unsigned long size, unsigned long attributes)
{
  s_alloc_attributes *attr = (s_alloc_attributes *)&attributes;
  void *result = 0;
  if (attr->allocator_id != 130) {
    if (g_shell_allocator_hook.enabled && g_shell_allocator_hook.allocator_id == (long)attr->allocator_id) {
      uintptr_t aligned = (g_shell_allocator_hook.base + g_shell_allocator_hook.used + 15) & ~15;
      long used = aligned - g_shell_allocator_hook.base + size;
      if (used <= g_shell_allocator_hook.limit) {
        g_shell_allocator_hook.used = used;
        result = (void *)aligned;
      }
    }
    else
      result = XMemAllocDefault(size, attributes);
  } else
    result = shell_alloc_physical_memory(size, *attr);
  return result;
}
// @address 0x198970 shell_free_memory
void shell_free_memory(void *address, unsigned long attributes)
{
  s_alloc_attributes *attr = (s_alloc_attributes *)&attributes;
  if (attr->allocator_id != 130) {
    if (g_shell_allocator_hook.enabled && g_shell_allocator_hook.allocator_id == (long)attr->allocator_id)
      return;
  } else if (attr->physical_memory == 1 && attr->memory_protect == 2) {
    return;
  }
  XMemFreeDefault(address, attributes);
}

// @address 0x58d720 GetProcessHeap

// The game's global operator delete: frees through the process heap.
// @address 0x198a18 ??3@YAXPAX@Z
void __cdecl operator delete(void *p)
{
    if (p != 0) {
        RtlFreeHeap(GetProcessHeap(), 0, p);
    }
}

// The game's startup stub: runs main on a 640 KB thread, waits for it, then exits.
// @address 0x1982e8 shell_start_main_thread
int shell_start_main_thread(void)
{
    void *thread = CreateThread(0, 0xA0000, reinterpret_cast<LPTHREAD_START_ROUTINE>(main), 0, 0, 0);
    if (thread != 0)
        WaitForSingleObject(thread, (unsigned long)-1);
    system_exit(0);
}

static __forceinline long shell_xenon_exception_filter(unsigned long exception_code, _EXCEPTION_POINTERS *exception_pointers)
{
    thread_record_first_exception(exception_pointers);
    return main_exception_handle_crash();
}

// @address 0x198338 main
int main(void)
{
    physical_memory_system_malloc(0x1a790000, (void *)0xa5470000, 4);
    memset(&g_physical_memory_globals, 0, sizeof(g_physical_memory_globals));
    g_physical_memory_globals.initialized = 1;
    g_physical_memory_globals.current_stage = 0;
    g_physical_memory_globals.begin = 0x05470000;
    g_physical_memory_globals.end = 0x1fc00000;
    g_physical_memory_globals.stages[0].begin = 0x05470000;
    g_physical_memory_globals.stages[0].end = 0x1fc00000;
    g_physical_memory_globals.stages[0].next_available_zero_allocation = (char *)1;
    g_physical_memory_globals.stages[0].allocation_count = 0;
    g_physical_memory_globals.stages[0].fixed_allocation_count = 0;
    XSetFileCacheSize(0x40000);
    long next_stage = g_physical_memory_globals.current_stage + 1;
    g_physical_memory_globals.stages[next_stage] = g_physical_memory_globals.stages[g_physical_memory_globals.current_stage];
    g_physical_memory_globals.current_stage = next_stage;
    __try
    {
        if (shell_initialize(0))
        {
            main_loop();
            shell_dispose();
        }
    }
    __except (shell_xenon_exception_filter(GetExceptionCode(), GetExceptionInformation()))
    {
        main_halt_and_spin_forever();
    }
    physical_memory_pop_stage(1);
    memset(&g_physical_memory_globals, 0, sizeof(g_physical_memory_globals));
    XPhysicalFree((void *)0xa5470000);
    return 0;
}

extern "C" {
// @address 0x8a1eaf g_cache_files_globals
}

// Detects whether the game is running from the DVD: reads the disc header through the
// library and checks its type string, then reports the result to the status console.
// @address 0x198670 shell_test_dvd_presence
void shell_test_dvd_presence(void)
{
    char buffer[256];
    int active;
    if (GetVolumeInformationA("d:\\", 0, 0, 0, 0, 0, buffer, sizeof(buffer)) != 0) {
        active = strcmp(buffer, "GDFX") == 0 || strcmp(buffer, "UDFX") == 0;
        CACHE_FILES_ENABLED = active;
    } else {
        active = 1;
        CACHE_FILES_ENABLED = active;
    }
    const char *status = CACHE_FILES_ENABLED ? "yes" : "no";
    main_set_status_value("running from dvd", status);
}

/* ---- shell dispose: inlined dispose code of the subsystems torn down at shell exit ---- */

// @address 0x197f30 shell_dispose
extern "C" void shell_dispose(void)
{
    game_state_shell_dispose();
    if (g_shell_havok_memory)
        g_shell_havok_memory = 0;
    sound_dispose();

    data_dispose(g_geometry_globals.manager->m_shared_file_handles);
    g_game_state_procs.handlers[1] = 0;
    g_physical_memory_globals.listener = 0;
    cache_file_tag_resource_runtime_dispose(&g_geometry_globals);
    g_game_state_procs.entries[0].handler = 0;

    if (g_font_globals.initialized) {
        font_cache_close_file();
        font_cache_delete();
        c_shell_font_cache_scope_lock lock;
        if (g_font_cache_channels.active.peek() != 0) {
            font_cache_reset_channels();
            __lwsync();
            _InterlockedExchange(&g_font_cache_channels.active.value, 0);
        }
    }
    memset(&g_font_globals, 0, sizeof(g_font_globals));

    SetEvent(g_synch_globals.events[2].handle);
    wait_for_thread_to_exit(4, 0xffffffff);
    ResetEvent(g_synch_globals.events[2].handle);

    memset(&g_netdebug_globals, 0, sizeof(g_netdebug_globals));
    c_rasterizer::cleanup_before_device_reset();
    if (c_rasterizer::get_device())
        c_dynamic_render_targets::dispose();
    g_rasterizer_main_initialized = false;
    if (g_runtime_state_globals.buffer)
        memset(&g_runtime_state_globals, 0, sizeof(g_runtime_state_globals));
    cseries_dispose();
}

// @address 0x1aeb68 shell_alloc_physical_memory
void *shell_alloc_physical_memory(unsigned long size, s_alloc_attributes attributes)
{
    if (attributes.physical_memory == 1 && attributes.memory_protect == 2) {
        unsigned long alignment = attributes.alignment;
        if (alignment == 0)
            alignment = 12;
        void *address = g_shell_physical_memory_stage.allocate(size, alignment);
        if (address != 0 && attributes.zero_initialize)
            XMemSet(address, 0, size);
        return address;
    }
    return XMemAllocDefault(size, attributes.value);
}

extern "C" {

// @address 0x8e3dd8 g_global_preferences_storage
}

// @address 0x5bec98 ?shell_halt_on_pure_virtual_call@@YAXXZ
void shell_halt_on_pure_virtual_call()
{
    ASSERT("calling pure virtual function!");
}

// @address 0x197c70 shell_initialize
extern "C" unsigned char shell_initialize(long parameter) {
    _set_purecall_handler(shell_halt_on_pure_virtual_call);
    QueryPerformanceFrequency((LARGE_INTEGER *)&g_shell_timing_globals.frequency);
    if (g_shell_timing_globals.frequency != 0x2f90838i64)
        g_shell_timing_globals.frequency = 0x2f90838i64;
    g_shell_timing_globals.initialized = true;
    initialize_thread_management();
    initialize_synchronization_objects();
    game_state_allocate_buffer();
#ifdef HALO3_64BIT
    g_runtime_state_globals.buffer = _physical_memory_malloc((memory_stage)1, 0, 0x600000, 3, 0, -1);
#else
    g_runtime_state_globals.buffer = _physical_memory_malloc((memory_stage)1, 0, 0x2f0000, 3, 0, -1);
#endif
    g_runtime_state_globals.size = 0;
    unsigned long launch_data_size = 0;
    XGetLaunchDataSize(&launch_data_size);
    shell_test_dvd_presence();
    shell_initialize_utility_drive();
    {
        s_shell_memory_buffer buffer;
        buffer.address = _physical_memory_malloc((memory_stage)1, 0, 0x1b0000, 2, 0, -1);
        buffer.size = 0x1b0000;
        g_shell_physical_memory_stage.initialize_buffer(buffer);
    }
    XNotifyDelayUI(0x7fffffff);
    if (g_shell_floating_point_exceptions_enabled)
        _controlfp(0x100001f, 0xfffff);
    async_initialize();
    char system_name[160];
    strncpy(system_name, "unknown_system", sizeof(system_name));
    system_name[sizeof(system_name) - 1] = 0;
    netdebug_initialize("halo3",
        "11855.07.08.20.2317.halo3_ship__cache_release", system_name);
    {
        c_thread_globals_lock lock(_inline_thread_globals_lock);
        global_preferences_init();
    }
    memset(&g_font_globals, 0, sizeof(g_font_globals));
    g_font_globals.language = -1;
    font_group_initialize_language();
    cache_files_copy_fonts();
    font_cache_open_file(false);
    font_cache_initialize_channels();
    font_cache_initialize_pixels();
    g_font_globals.initialized = 1;
    cache_files_windows_initialize();
    {
        s_shell_memory_buffer buffer;
#ifdef HALO3_64BIT
        const size_t runtime_manager_alloc_size = sizeof(c_cache_file_tag_resource_runtime_manager_storage) + 31;
        buffer.address = g_geometry_globals.allocator->allocate(runtime_manager_alloc_size, 0);
        buffer.size = (long)runtime_manager_alloc_size;
        void *aligned_address = align_pointer(buffer.address, 5);
        g_geometry_globals.allocation = buffer;
        memset(aligned_address, 0, sizeof(c_cache_file_tag_resource_runtime_manager_storage));
#else
        buffer.address = g_geometry_globals.allocator->allocate(0x8b63f, 0);
        buffer.size = 0x8b63f;
        void *aligned_address = align_pointer(buffer.address, 5);
        g_geometry_globals.allocation = buffer;
        memset(aligned_address, 0, sizeof(c_cache_file_tag_resource_runtime_manager));
#endif
        c_cache_file_tag_resource_runtime_manager_storage *manager = aligned_address
            ? new (aligned_address) c_cache_file_tag_resource_runtime_manager_storage : NULL;
        g_geometry_globals.storage = manager;
        cache_file_tag_resource_runtime_initialize(manager,
            &g_runtime_state_allocation);
    }
    game_state_shell_initialize();
    c_rasterizer::shell_initialize(false, false);
    memset(&input_globals, 0, sizeof(input_globals));
    input_globals.initialized = true;
    input_globals.shell_initialization_flags |= 1;
    input_update();
    sound_initialize();
    g_shell_havok_memory = _physical_memory_malloc((memory_stage)1, 0, 0x4b000, 0, 0, -1);
    return true;
}

// @address 0x580c98 GetVolumeInformationA
// @address 0x58dd40 _set_purecall_handler

s_allocator_hook g_shell_allocator_hook;
shell_xenon_timing_globals g_shell_timing_globals;
c_shell_physical_memory_stage g_shell_physical_memory_stage;
