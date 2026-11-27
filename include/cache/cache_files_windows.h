#ifndef CACHE_CACHE_FILES_WINDOWS_H
#define CACHE_CACHE_FILES_WINDOWS_H

#include "saved_games/game_state_xenon_globals.h"
#include "cache/map_file_index_types.h"
#include "cache/sound_cache_hash_source.h"
#include "memory/basic_buffer.h"
#include "memory/lruv_cache_internal.h"
#include "saved_games/game_state_procs_types.h"
#include <stddef.h>

// Functions defined in tu/blamlibxbox/cache_files_windows.cpp.

extern "C" long cache_files_windows_get_map_status(const char *path);

void cache_file_block_until_not_busy();
extern "C" void cache_files_windows_idle(void);

struct s_cache_file_header;
struct s_cache_files_windows_load_status;

extern "C" void cache_files_windows_update_load_status(s_cache_files_windows_load_status *status, short progress, const char *path);
extern "C" bool cached_map_file_exists(const char *path);
extern "C" long cache_files_windows_get_active_map_level(const char *path);
extern "C" long cache_files_windows_get_pending_map_level(const char *path);
extern "C" void cache_files_windows_call_pending_load_procs();
extern "C" bool cache_files_windows_get_active_copy_progress(float *progress_out);
extern "C" float cache_files_windows_get_map_copy_progress(const char *path);
extern "C" void cached_map_file_close(long index);
extern "C" void cache_files_windows_initialize(void);
extern "C" bool cache_files_windows_set_active_map(const char *path, s_cache_file_header *header);

struct s_cache_files_windows_stream_state;

extern "C" unsigned char cache_files_windows_test_wait_request(s_cache_files_windows_stream_state *request);
extern "C" void cache_files_windows_update_language(void);
extern "C" void cache_files_windows_update_build_number(void);
extern "C" void cache_files_windows_cancel_active_copy(void);
extern "C" bool cache_files_windows_test_map_paths_equal(const char *path_a, const char *path_b);
extern "C" void cache_files_windows_scan_cached_maps(bool *p0);
void cached_map_files_delete(enum e_map_file_index _sym_p0, enum e_map_file_index _sym_p1);
enum e_map_file_index cached_map_files_find_free_map(long size, short type);
unsigned long cached_map_file_get_size(enum e_map_file_index _sym_p0);
void cached_map_file_get_path(e_map_file_index index, char *path, unsigned int size);


// Shared declarations used by cache_files_windows.cpp.
struct s_cache_files_windows_pending_load;
struct s_cache_files_windows_completion
{
    s_lruv_completion result;
    long size;
};

// The map-copy stream source handed to the bulk reader (vtable 0x3d56c).
struct s_cache_files_windows_stream_state : public c_sound_cache_hash_source
{
#ifdef HALO3_64BIT
    virtual bool open(c_basic_buffer<void> source_position);
#else
    virtual bool open(unsigned __int64 source_position);
#endif
    virtual bool read(c_basic_buffer<void> request, c_basic_buffer<void> *result);
    virtual bool finish(c_basic_buffer<void> *result);

    void *file_handle;
    c_synchronized_long bytes_done;
    c_synchronized_long io_pending;
    long checksum;
    bool valid;
    bool active;
    s_cache_files_windows_completion completion;
    s_lruv_completion_context overlapped;
    c_basic_buffer<void> buffer;
};

typedef char cache_stream_completion_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_stream_state, completion) == 0x18))) ? 1 : -1];
typedef char cache_stream_overlapped_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_stream_state, overlapped) == 0x28))) ? 1 : -1];
typedef char cache_stream_bytes_done_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_stream_state, bytes_done) == 8))) ? 1 : -1];
typedef char cache_stream_buffer_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_stream_state, buffer) == 0x3c))) ? 1 : -1];
typedef char cache_stream_size[(sizeof(void *) != 4 || ((sizeof(s_cache_files_windows_stream_state) == 0x44))) ? 1 : -1];

// Game-state proc listener of the map-copy buffer (slot 0, vtable 0x3d560).
class c_cache_files_pending_load_listener : public c_game_state_proc_listener
{
public:
    virtual void set_mode(long mode, s_game_state_buffer_description *description);
    virtual void update(void *);
    virtual void dispose(void *);
};

extern "C" s_cache_files_windows_pending_load g_cache_files_windows_pending_load;
extern "C" s_cache_files_windows_stream_state g_cache_files_windows_stream_state;
extern "C" c_cache_files_pending_load_listener g_cache_files_pending_load_listener;

// Shared storage read by the Windows cache implementation.
struct s_xenon_game_state_globals;
extern "C" {
long cache_files_windows_request_map_load(const char *, long);
void cache_files_windows_process_loading_requests(void);
}

#endif // CACHE_CACHE_FILES_WINDOWS_H
