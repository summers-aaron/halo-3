#ifndef CACHE_CACHE_FILE_BULK_READER_TYPES_H
#define CACHE_CACHE_FILE_BULK_READER_TYPES_H

#include <stddef.h>
template <typename t_type> class c_basic_buffer;
class c_asynchronous_io_marker;

class c_indirect_cache_file_location_atlas;
struct s_indirect_cache_file_read_request;
#include "cseries/cseries.h"
#include "memory/static_arrays_flags.h"
#include "cache/async_io_file_types.h"
#include "cache/async_io_last_read_types.h"

class c_asynchronous_io_arena {
public:
    void *completion_port;
    c_static_flags_no_init<32> valid_files;
    s_async_io_file files[32];
    unsigned int outstanding_io_requests;
    s_async_io_last_read last_read;
    bool submit_async_read(long file_index, unsigned long offset, unsigned long size, c_basic_buffer<void> buffer, c_asynchronous_io_marker *marker);
    bool submit_indirect_async_read(c_indirect_cache_file_location_atlas *atlas, const s_indirect_cache_file_read_request *request,
        c_asynchronous_io_marker *marker);
    void close_file(long file_index);
    void update_with_quantum_entanglement(bool block_on_completion_port);
};

STATIC_ASSERT_ABI32(sizeof(c_asynchronous_io_arena) == 0x19c, resource_io_arena_size);
STATIC_ASSERT_ABI32(offsetof(c_asynchronous_io_arena, files) == 8, resource_io_arena_files_offset);

#endif // CACHE_CACHE_FILE_BULK_READER_TYPES_H
