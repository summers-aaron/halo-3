#ifndef CACHE_INDIRECT_CACHE_FILE_READER_TYPES_H
#define CACHE_INDIRECT_CACHE_FILE_READER_TYPES_H

#include "memory/basic_buffer.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "cache/cache_file_bulk_reader_types.h"

// Locates data by file index, file-relative offset and compressed size.
// March c_cache_file_tag_resource_runtime_manager::get_location has the same layout.
struct s_indirect_cache_file_location
{
 long file_index;
 unsigned long offset;
 unsigned long size;
};

STATIC_ASSERT(sizeof(s_indirect_cache_file_location) == 0xc, indirect_cache_file_location_size);
STATIC_ASSERT(offsetof(s_indirect_cache_file_location, offset) == 4, indirect_cache_file_location_offset);
STATIC_ASSERT(offsetof(s_indirect_cache_file_location, size) == 8, indirect_cache_file_location_size_offset);

// March runtime manager implements this location-atlas interface.
class c_indirect_cache_file_location_atlas
{
public:
 virtual bool get_location(unsigned __int64 position, s_indirect_cache_file_location *location) = 0;
};

struct s_indirect_cache_file_read_request {
 // The caller always assigns the buffer before the request is used; skip the default zero-fill.
 s_indirect_cache_file_read_request() : buffer(c_basic_buffer<void>::_uninitialized) {}
 c_basic_buffer<void> buffer;
 unsigned __int64 position;
 unsigned long offset, size;
 unsigned long uncompressed_size;
 long hash_source_index;
 bool validate_hash;
 s_network_http_request_hash expected_hash;
};
#include "cache/indirect_cache_file_bulk_read_iterator.h"

typedef char indirect_cache_file_read_request_size[(sizeof(void *) != 4 || ((sizeof(s_indirect_cache_file_read_request) == 56))) ? 1 : -1];

#endif
