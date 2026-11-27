#include <xtl.h>
#include "memory/game_state_globals.h"
// blamlibxbox/game_state_procs.obj: from the records 0x19bb40 0x19c388 0x19c418 0x19c7f0 0x1d67a0 0x1d6890 0x1ebb68 0x1ebb90 0x1ebc40 0x1ebcb8 0x1ebd10 0x1ebda8 0x1f4628 0x222d88 0x222da0 0x7079e8 0x707cc0 0x70b758 0x70b8c8
// @flags /TP /O2 /Oc
#include "saved_games/game_state_procs_externs.h"
#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory.h"
#include "cseries/crt_xdk.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "main/main.h"
#include "saved_games/game_state_procs.h"
#include "cseries/async_xoverlapped_memory.h"
#include "cache/cache_files_windows.h"
#include "saved_games/game_state.h"
#include "multithreading/synchronized_value.h"
#include <ppcintrinsics.h>
#include <stdlib.h>

#include "saved_games/game_state_lock.h"
#include "saved_games/game_state_procs_types.h"
#include "objects/object_broadphase.h"
#include "physics/havok.h"
#include "camera/observer.h"
#include "game/player_training.h"
#include "cache/cache_files_pending_load_types.h"


// @address 0x19bb40 ?game_state_call_before_load_procs@@YAXXZ
void game_state_call_before_load_procs()
{
    for (long region_index = 0; region_index < 5; region_index++) {
        c_restricted_memory *region = &g_restricted_regions[region_index];
        if (region->m_primary_section->m_owner_thread != -1) {
            region->set_user_addresses_internal((unsigned char *)region->m_primary_section->m_address);
        }
        for (long mirror_index = 0; mirror_index < 4; mirror_index++) {
            region->m_mirrors[mirror_index].readers.peek();
        }
    }
}

// The default handler game_state_procs_initialize installs as handlers[0]: its allocator hands out
// memory stage 4, and set_mode builds its pool.
// @address 0x75de54 ?g_game_state_proc_default_handler@@3Us_async_xoverlapped_memory@@A
s_async_xoverlapped_memory g_game_state_proc_default_handler = s_async_xoverlapped_memory((memory_stage)4);

// @address 0x19c388 game_state_try_and_get_compressed_buffer
extern "C" unsigned char game_state_try_and_get_compressed_buffer(s_game_state_compressed_buffer *buffer)
{
    bool result = false;
    if (buffer->active == 0) {
        void *token = game_state_procs_try_and_get_slot(5, 2, 0x400000, &g_game_state_compressor_listener);
        buffer->compressed_buffer = static_cast<s_game_state_compression_buffer *>(token);
        if (token != 0) {
            buffer->compressed_size = 0x400000;
        }
        result = (token != 0);
        if (result != 0) {
            buffer->active = 1;
            buffer->acquire_requested = 0;
        }
    }
    return result;
}

// @address 0x19c418 game_state_dispose_compressed_buffer
extern "C" void game_state_dispose_compressed_buffer(s_game_state_compressed_buffer *buffer)
{
    if (buffer->active != 0) {
        ::game_state_call_after_load_procs(5, buffer->compressed_buffer);
        buffer->compressed_buffer = 0;
        buffer->compressed_size = 0;
        buffer->active = 0;
        buffer->extra = 0;
        buffer->last = 0;
    }
}

// @address 0x19c7f0 ?dispose@c_game_state_compressor_listener@@UAAXPAX@Z
void c_game_state_compressor_listener::dispose(void *)
{
    if (game_state_globals.compressed.active != 0) {
        game_state_dispose_compressed_buffer(&game_state_globals.compressed);
    }
}

// The image's three after-unlock procs in slot order (0x74b130..0x74b13c). March 2007's debug table
// holds only the first two; the third slot is the shared empty body at 0x5bec98, a proc compiled
// empty in release whose name did not survive.
// @address 0x74b130 g_game_state_unlock_procs
#if !defined(_XBOX) || defined(__clang__)
static void game_state_unlock_proc_empty(long)
{
}

extern "C" {
void (*g_game_state_unlock_procs[3])(long) =
{
    havok_load_from_game_state,
    object_broaphase_load_from_game_state,
    game_state_unlock_proc_empty
};
}
#endif
// @address 0x192420 ?unlock_resources_and_resume_render_thread@@YAXK@Z

// @address 0x1d6890 game_state_unlock_and_resume_render_thread
extern "C" void game_state_unlock_and_resume_render_thread(long arg)
{
    _InterlockedExchange(&g_game_state_lock.value, 0);
    long token = ::tag_resources_lock_game();
    for (short i = 0; (unsigned short)i < 3; ++i) {
        g_game_state_unlock_procs[i](arg);
    }
    ::tag_resources_unlock_game(token);
    g_game_state_shared_lock.lock.set(0);
    unlock_resources_and_resume_render_thread(g_game_state_shared_lock.render_thread_lock);
}

// The image's four before-save procs in slot order (0x74b120..0x74b130); March 2007's
// game_state_procs.obj table holds the same four functions in the same order.
// @address 0x74b120 g_game_state_lock_procs
extern "C" {
void (*g_game_state_lock_procs[4])(long) =
{
    object_broaphase_save_to_game_state,
    havok_save_to_game_state,
    observer_initialize_before_saved_game,
    player_training_handle_saved_game
};
}

// @address 0x1d67a0 game_state_lock_and_halt_render_thread
extern "C" {
// @address 0x85690 D3DDevice_IsFencePending
// @address 0x1950c8 D3DDevice_BlockOnFence
// @address 0x737664 g_game_state_render_fence
// @address 0x96e760 g_game_state_procs
}

extern "C" void game_state_lock_and_halt_render_thread(int lock_mode)
{
    int fence;
    short current;
    short next;
    long token;
    g_game_state_shared_lock.render_thread_lock = _internal_halt_render_thread_and_lock_resources("..\\saved_games\\game_state_procs.cpp", 219);
    fence = g_game_state_render_fence;
    D3DDevice_IsFencePending(fence);
    D3DDevice_BlockOnFence(fence);
    g_game_state_shared_lock.lock.set(2);
    token = ::tag_resources_lock_game();
    next = 0;
    do {
        current = (short)next;
        g_game_state_lock_procs[current](lock_mode);
        next = (short)(current + 1);
    } while ((unsigned short)next < 4);
    ::tag_resources_unlock_game(token);
    _InterlockedExchange(&g_game_state_lock.value, 1);
    if ((lock_mode & 0x200) == 0) {
        game_state_update_hash((unsigned char)(((unsigned int)lock_mode >> 8) & 1), 0);
    }
}

// @address 0x1ebb68 game_state_procs_initialize
extern "C" void game_state_procs_initialize(void)
{
    g_game_state_procs.handlers[0] = &g_game_state_proc_default_handler;
    g_game_state_procs.current = 0;
    g_game_state_procs.active[0] = 0;
}

// @address 0x1ebb90 game_state_procs_set_mode_handler
extern "C" void game_state_procs_set_mode_handler(e_map_memory_configuration configuration)
{
    s_game_state_buffer_description descriptions[6];

    c_game_state_proc_handler *handler = 0;
    if (configuration != 1 && configuration != 6) {
        handler = g_game_state_procs.handlers[1];
    }
    if (handler == 0) {
        handler = g_game_state_procs.handlers[0];
    }
    g_game_state_procs.current = handler;

    for (long index = 0; index < 6; index++) {
        if (g_game_state_procs.entries[index].handler != 0) {
            g_game_state_procs.entries[index].handler->set_mode(configuration, &descriptions[index]);
        } else {
            descriptions[index].type = 0;
            descriptions[index].size = 0;
        }
    }

    g_game_state_procs.current->set_mode(configuration, descriptions);
}

// @address 0x1ebc40 game_state_procs_dispose_active_entries
extern "C" void game_state_procs_dispose_active_entries(void)
{
    for (long index = 0; index < 6; index++) {
        if (g_game_state_procs.active[index >> 5] & (1 << (index & 31))) {
            g_game_state_procs.entries[index].handler->dispose(g_game_state_procs.entries[index].buffer);
        }
    }
}

// @address 0x1ebcb8 game_state_procs_reset_current_handler
extern "C" void game_state_procs_reset_current_handler(void)
{
    game_state_procs_dispose_active_entries();
    c_game_state_proc_handler *handler = g_game_state_procs.current;
    if (handler != 0) {
        handler->reset();
        g_game_state_procs.current = 0;
    }
}

// @address 0x1ebd10 game_state_procs_try_and_get_slot
extern "C" void *game_state_procs_try_and_get_slot(long index, long allocation_parameter, long allocation_size, c_game_state_proc_listener *listener)
{
    void *token = 0;
    c_game_state_proc_handler *current = g_game_state_procs.current;
    if (current != 0) {
        token = current->game_state_proc_handler_allocate(index, allocation_parameter, allocation_size, listener);
        if (token != 0) {
            s_game_state_proc_entry *entry = &g_game_state_procs.entries[index];
            entry->buffer = token;
            entry->handler = listener;
            g_game_state_procs.active[index >> 5] |= 1 << (index & 31);
        }
    }
    return token;
}

// @address 0x1ebda8 ?game_state_call_after_load_procs@@YAXJPAX@Z
void game_state_call_after_load_procs(long index, void *buffer)
{
    g_game_state_procs.current->game_state_proc_handler_release(index, buffer);
    g_game_state_procs.active[index >> 5] &= ~(1 << (index & 31));
}


extern "C" {
// @address 0x9dc200 g_cache_files_windows_pending_load
// @address 0x75de68 g_cache_files_pending_load_listener
}

// @address 0x1f4628 cache_files_windows_update_copy_buffer
extern "C" void cache_files_windows_update_copy_buffer(long arg)
{
    if (g_cache_files_windows_pending_load.copy_buffer_size != arg) {
        cache_files_windows_call_pending_load_procs();
        char *buffer = static_cast<char *>(game_state_procs_try_and_get_slot(0, 0, arg, &g_cache_files_pending_load_listener));
        g_cache_files_windows_pending_load.copy_buffer = buffer;
        if (buffer != 0) {
            g_cache_files_windows_pending_load.copy_buffer_size = arg;
        }
    }
}

// @address 0x222d88 game_state_procs_set_autosave_listener
extern "C" void game_state_procs_set_autosave_listener(void)
{
    g_game_state_procs.entries[2].handler = &g_game_state_autosave_listener;
}

// @address 0x222da0 game_state_procs_reset_autosave_listener
extern "C" void game_state_procs_reset_autosave_listener(void)
{
    g_game_state_procs.entries[2].handler = 0;
}


extern "C" {
// @address 0x58dec0 atexit
}


extern "C" {
// @address 0x75de6c g_cache_files_windows_stream_state
}

// @address 0x707cc0 game_state_initialize_stream_state
extern "C" void game_state_initialize_stream_state(void)
{
    g_cache_files_windows_stream_state.bytes_done.value = 0;
    g_cache_files_windows_stream_state.io_pending.value = 0;
    g_cache_files_windows_stream_state.completion.result.done.value = 0;
    __lwsync();
    _InterlockedExchange(&g_cache_files_windows_stream_state.completion.result.done.value, 0);
    g_cache_files_windows_stream_state.buffer.m_address = 0;
    g_cache_files_windows_stream_state.buffer.m_size = 0;
    atexit(game_state_dispose_stream_state);
}

// @address 0x70b8c8 game_state_dispose_stream_state
extern "C" void game_state_dispose_stream_state(void)
{
}

s_game_state_shared_lock g_game_state_shared_lock;
s_game_state_procs_globals g_game_state_procs;



