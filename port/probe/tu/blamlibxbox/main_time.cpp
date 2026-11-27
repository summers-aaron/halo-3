// blamlibxbox/main_time.obj: from the records 0x88640 0x88800 0x1421b8 0x1423e8 0x1924c0 0x192768 0x1a81a8 0x1a81b8 0x1a8240 0x1a8248 0x1a8298 0x1a8340 0x1a8358 0x2432f0
// @flags /TP /O2
#include "game/game_save_globals.h"
#include "memory/tls_globals.h"
#include "cseries/crt_xdk.h"
#include "game/game_time_types.h"
#include "game/game_globals_storage.h"
// @address 0x2432f0 ?__tls_set_g_main_time_globals_allocator@@YAXPAX@Z
#include <ppcintrinsics.h>
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "main/main.h"
// match: 0x1925d8: bind the PIX stub this image links in place of the XDK pix.obj implementation.
// @address 0x1925d8 pix_add_named_counter
#include "main/main_time.h"
#include "rasterizer/rasterizer_main.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "rasterizer/rasterizer_globals.h"
#include "multithreading/synchronization.h"

#include "main/main_time_globals.h"
#include "cache/restricted_memory_layout.h"
#include "multithreading/synchronization_types.h"
#include "game/game_state_accessors.h"
#include "cache/restricted_memory_registration.h"
// match: 0x59f9e0: SDK header adoption is blocked by this unit's current types/API arguments: 'DWORD' : redefinition; different basic types.
#include "multithreading/interlocked_slist_push.h"

#if !defined(_XBOX) || defined(__clang__)
// PC (port/sdk/d3d/null_device.cpp): the rate of the D3D device's vertical blanks, which run at the frame rate
// (HALO3_FPS) while the video mode reports the console's 60 Hz (the 30 Hz tick rate derives from it).
extern "C" unsigned long port_d3d_vertical_blank_rate(void);
extern "C" void rasterizer_main_wait_for_mirror_claim(unsigned long timeout_ms);
#endif

// @address 0x11df738 ?g_main_time_globals@@3PAUs_main_time_globals@@A
__declspec(thread) s_main_time_globals *g_main_time_globals;

extern "C" {
// @address 0x8a1ef8 g_main_time_throttle_swap_index
unsigned long g_main_time_throttle_swap_index;
// @address 0xfb0 k_main_time_input_lag_name
char k_main_time_input_lag_name[] = "halo_input_lag";
// @address 0xfc0 k_main_time_camera_lag_name
char k_main_time_camera_lag_name[] = "halo_camera_lag";
}
struct s_rasterizer_globals;
class c_interlocked_long;
struct s_rasterizer_timing_globals;
struct s_synchronization_globals;
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z


#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "main/main_time_allocator.h"
namespace {
static __forceinline __int64 rasterizer_get_vblank_index()
{
    __int64 result = rasterizer_globals.vblank_index.value;
    __lwsync();
    return result;
}
static __forceinline __int64 main_time_get_absolute_milliseconds()
{
    __int64 result = GetTickCount();
    result |= g_main_time_globals->milliseconds & 0xffffffff00000000ui64;
    if (result < g_main_time_globals->milliseconds)
        result += 1i64 << 32;
    return result;
}
static __forceinline long restricted_region_add_main_time_member(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, alignment_bits,
        (field_proc_t)__tls_set_g_main_time_globals, 0, main_time_globals_post_copy_update);
    void *address = restricted_region_get_member_address(3, member_index);
    g_main_time_globals = (s_main_time_globals *)address;
    return member_index;
}
}

// @address 0x1421b8 main_time_update_sync_milliseconds
extern "C" void main_time_update_sync_milliseconds(void)
{
    __int64 milliseconds = GetTickCount();
    milliseconds |= g_main_time_globals->milliseconds & 0xffffffff00000000ui64;
    if (milliseconds < g_main_time_globals->milliseconds)
        milliseconds += 1i64 << 32;
    g_main_time_globals->synchronization_milliseconds = milliseconds;
}

// @address 0x1a81a8 ?__tls_set_g_main_time_globals@@YAXPAUs_main_time_globals@@@Z
void __tls_set_g_main_time_globals(s_main_time_globals *globals)
{
    g_main_time_globals = globals;
}

bool main_time_is_throttled()
{
    bool result = true;
    if (!game_in_progress())
        result = false;
    else if (g_main_time_globals->custom_time)
        result = g_main_time_globals->throttled;
    return result;
}

void main_time_globals_post_copy_update(void *allocation)
{
    s_main_time_globals *globals = (s_main_time_globals *)allocation;
    globals->last_system_milliseconds = GetTickCount();
}

void main_time_reset()
{
    __int64 vblank_index = rasterizer_get_vblank_index();
    g_main_time_globals->vblank_index = vblank_index;
    g_main_time_globals->initial_vblank_index = vblank_index;
    g_main_time_globals->target_vblank_index = vblank_index;
    g_main_time_globals->input_collection_milliseconds = main_time_get_absolute_milliseconds();
}


// @address 0x192768 main_time_update_throttle_sample
extern "C" void main_time_update_throttle_sample(__int64 target)
{
    s_rasterizer_timing_globals *globals = &g_rasterizer_timing_globals;
    globals->current_record.vblank_index = target;
    globals->current_record.render_present_vblank_delta = (long)rasterizer_get_vblank_index() - (long)target;
    globals->current_record.render_present_milliseconds = GetTickCount();
    void *entry = InterlockedPopEntrySList(&globals->present_free_records);
    __lwsync();
    if (entry) {
        __int64 * dst = (__int64 *)entry;
        // match: 0x192768: pointer __restrict qualifier prevents alias reload
        __int64 * __restrict src = (__int64 *)&globals->current_record;
        for (long count = 12; count-- > 0; )
            *dst++ = *src++;
        __lwsync();
        InterlockedPushEntrySList(&globals->present_pending_overflow_records, entry);
    }
    long input_collection_milliseconds = (long)globals->last_presented_record.input_collection_milliseconds;
    long presented_milliseconds = (long)globals->last_presented_record.milliseconds;
    long input_adjustment_time = (long)globals->last_presented_record.input_adjustment_time;
    long input_lag = presented_milliseconds - input_collection_milliseconds;
    long camera_lag = presented_milliseconds - input_adjustment_time;
    float lag = (float)input_lag;
    pix_add_named_counter(lag, k_main_time_input_lag_name);
    lag = (float)camera_lag;
    pix_add_named_counter(lag, k_main_time_camera_lag_name);
}

// @address 0x1a8240 ?main_time_restore@@YAXJ@Z
void main_time_restore(long saved_time)
{
    main_time_reset();
}

// @address 0x1a8248 ?rasterizer_get_refresh_rate@@YAHXZ
int rasterizer_get_refresh_rate()
{
    int rate = (int)(rasterizer_main_get_video_mode()->RefreshRate + 0.5f);
    if (rate <= 0)
        rate = 60;
    return rate;
}

template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &main_time_globals_post_copy_update,
    &__tls_set_g_main_time_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x1a8298: caller constant specialization for alignment_bits
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_main_time_member(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x1a8340 ?allocate@?$t_restricted_allocation_manager@$02$0A@$1?main_time_globals_post_copy_update@@YAXPAX@Z$1?__tls_set_g_main_time_globals_allocator@@YAX0@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &main_time_globals_post_copy_update,
    &__tls_set_g_main_time_globals_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x1a8358 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$1?main_time_globals_post_copy_update@@YAXPAX@Z$1?__tls_set_g_main_time_globals_allocator@@YAX0@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &main_time_globals_post_copy_update,
    &__tls_set_g_main_time_globals_allocator>::deallocate(void *address)
{
    int value = m_member_index;
    g_main_time_globals = 0;
    restricted_region_free_member(3, value);
}

void __tls_set_g_main_time_globals_allocator(void *allocator)
{
    g_game_save_globals = (s_game_save_globals *)allocator;
}

static __forceinline unsigned long main_time_swap_index_get()
{
    unsigned long result = rasterizer_globals.swap_index.value;
    __lwsync();
    return result;
}
static __forceinline void main_time_wait_for_vblank()
{
    s_synchronization_globals *globals = &g_synch_globals;
    if (globals->events[4].handle)
        WaitForSingleObjectEx(globals->events[4].handle, 0xffffffff, 0);
}

// @address 0x1924c0 ?main_time_throttle@@YAX_J@Z
void main_time_throttle(__int64 target)
{
    __int64 limit = target ? target - 2 : 0;
#if !defined(_XBOX) || defined(__clang__)
    if (port_d3d_vertical_blank_rate() >= 1000) {
        g_main_time_throttle_swap_index = main_time_swap_index_get();
        main_time_update_throttle_sample(target);
        return;
    }
#endif
    if (main_time_is_throttled()) {
        volatile __int64 current = rasterizer_get_vblank_index();
        while (g_main_time_throttle_swap_index > 0 && current == g_main_time_throttle_swap_index) {
            main_time_wait_for_vblank();
            current = rasterizer_get_vblank_index();
        }
        g_main_time_throttle_swap_index = main_time_swap_index_get();
        if (limit > 0 && limit - current <= 8) {
            while (current < limit) {
                main_time_wait_for_vblank();
                current = rasterizer_get_vblank_index();
            }
        }
    }
    main_time_update_throttle_sample(target);
}

#define MAIN_TIME_MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAIN_TIME_MIN(a, b) ((a) < (b) ? (a) : (b))
// @address 0x88640 ?main_time_update@@YAMXZ
float main_time_update()
{
    unsigned long system_milliseconds = GetTickCount();
    s_main_time_globals *globals = g_main_time_globals;
    __int64 last_milliseconds = globals->milliseconds;
    __int64 milliseconds = (last_milliseconds & 0xffffffff00000000ui64) | system_milliseconds;
    if (milliseconds < last_milliseconds)
        milliseconds += 0x100000000ui64;
    __int64 initial_vblank = rasterizer_get_vblank_index();
    __int64 vblank = initial_vblank;
    long game_time = game_in_progress() ? tls_game_time_globals()->time : 0;
    __int64 target_vblank = globals->vblank_index + 1;
#if !defined(_XBOX) || defined(__clang__)
    const unsigned long vblank_rate = port_d3d_vertical_blank_rate();
    float elapsed = 0.0f;
    static long long s_uncapped_freq = 0;
    static long long s_uncapped_last = 0;
    if (vblank_rate >= 1000) {
        publish_waiting_gamestate();
        __int64 now = 0;
        if (!s_uncapped_freq)
            QueryPerformanceFrequency((_LARGE_INTEGER *)&s_uncapped_freq);
        QueryPerformanceCounter((_LARGE_INTEGER *)&now);
        if (g_render_thread_enabled.peek() != 0 && g_restricted_regions[3].has_available_mirror()) {
            rasterizer_main_wait_for_mirror_claim(2);
            QueryPerformanceCounter((_LARGE_INTEGER *)&now);
        }
        if (s_uncapped_last > 0 && s_uncapped_freq > 0 && now > s_uncapped_last)
            elapsed = (float)((double)(now - s_uncapped_last) / (double)s_uncapped_freq);
        s_uncapped_last = now;
        vblank = rasterizer_get_vblank_index();
    } else {
        s_uncapped_last = 0;
        while (main_time_is_throttled() && vblank < target_vblank) {
            publish_waiting_gamestate();
            void *event = g_synch_globals.events[3].handle;
            if (event)
                WaitForSingleObjectEx(event, 0xffffffff, 0);
            vblank = rasterizer_get_vblank_index();
        }
        float elapsed_vblanks = (float)(vblank - g_main_time_globals->vblank_index);
        elapsed = elapsed_vblanks / (float)vblank_rate;
    }
#else
    while (main_time_is_throttled() && vblank < target_vblank) {
        publish_waiting_gamestate();
        void *event = g_synch_globals.events[3].handle;
        if (event)
            WaitForSingleObjectEx(event, 0xffffffff, 0);
        vblank = rasterizer_get_vblank_index();
    }
    float elapsed_vblanks = (float)(vblank - g_main_time_globals->vblank_index);
    float elapsed = elapsed_vblanks / (float)rasterizer_get_refresh_rate();
#endif
    __int64 next_vblank = vblank;
    if (g_render_thread_enabled.peek() != 0)
        next_vblank += 4;
    else
        next_vblank += 2;
    elapsed = MAIN_TIME_MIN(MAIN_TIME_MAX(elapsed, 0.0f), 10.0f);
    s_main_time_globals *updated_globals = g_main_time_globals;
    updated_globals->game_time = game_time;
    updated_globals->milliseconds = milliseconds;
    updated_globals->vblank_index = vblank;
    updated_globals->target_vblank_index = next_vblank;
    updated_globals->initial_vblank_index = initial_vblank;
    return elapsed;
}


// @address 0x74b834 ?g_main_time_globals_allocator@@3V?$t_restricted_allocation_manager@$02$0A@$1?main_time_globals_post_copy_update@@YAXPAX@Z$1?__tls_set_g_main_time_globals_allocator@@YAX0@Z@@A
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &main_time_globals_post_copy_update,
    &__tls_set_g_main_time_globals_allocator> g_main_time_globals_allocator;
