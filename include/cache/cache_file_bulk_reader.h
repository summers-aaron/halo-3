#ifndef CACHE_CACHE_FILE_BULK_READER_H
#define CACHE_CACHE_FILE_BULK_READER_H

// Functions defined in tu/blamlibxbox/cache_file_bulk_reader.cpp.

#include "cseries/async_types.h"

struct s_sound_cache_hash_state;
struct s_async_task;

e_async_completion cache_file_bulk_reader_handle_async_read(s_async_task *task, void *data, long data_size);
extern "C" void cache_file_bulk_reader_update_hash(s_sound_cache_hash_state *state);

class c_sound_cache_file_reader;
class c_sound_cache_reader;

extern "C" bool cache_file_bulk_reader_test_expected_hash(s_sound_cache_hash_state *state, const void *expected_digest);
extern "C" void cache_file_bulk_reader_finish_pending_reads(s_sound_cache_hash_state *state, void *reader);
extern "C" void asynchronous_io_arena_dispose(void);
extern "C" void cache_file_bulk_reader_update_hash_state(s_sound_cache_hash_state *state, c_sound_cache_reader *reader);

class c_asynchronous_io_arena;

extern "C" c_asynchronous_io_arena g_asynchronous_io_arena;

#endif // CACHE_CACHE_FILE_BULK_READER_H
