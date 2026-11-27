#ifndef CACHE_CACHE_FILES_WINDOWS_ASYNC_H
#define CACHE_CACHE_FILES_WINDOWS_ASYNC_H

#include "cseries/async_types.h"
#include "interface/gui_custom_bitmap_storage_hash_types.h"
template<typename T> class c_basic_buffer;
class c_sound_cache_hash_source;
class c_synchronized_long;
extern "C" { extern bool async_allocate_file_space(const char *path, void *handle, unsigned long size, __int64 *free_space_used); }
extern "C" { extern long game_state_xenon_write_file(long file_index, long category, long priority, unsigned char *result, void *done); }
extern "C" long gui_custom_bitmap_storage_queue_async_read(e_async_priority priority, void *handle, unsigned long base_offset,
    unsigned long size, const s_gui_custom_bitmap_storage_hash *expected_hash, c_basic_buffer<void> buffer,
    c_sound_cache_hash_source *source, c_basic_buffer<void> destination, bool *success, c_synchronized_long *progress,
    c_synchronized_long *bytes_read, c_synchronized_long *done);

#endif // CACHE_CACHE_FILES_WINDOWS_ASYNC_H
