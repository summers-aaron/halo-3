#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_BULK_READER_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_BULK_READER_H

#include "cache/sound_cache_bulk_reader.h"

class c_indirect_cache_file_location_atlas;
class c_indirect_cache_file_bulk_read_iterator;
struct s_indirect_cache_file_read_request;

class c_cache_file_tag_resource_bulk_reader : public c_sound_cache_bulk_reader
{
public:
    virtual bool start_read(c_basic_buffer<void> buffer, unsigned long offset,
        unsigned long *bytes_read, long request_index);
    virtual bool read_complete(long request_index, unsigned long *bytes_read, bool *success);

    using c_sound_cache_bulk_reader::m_markers;
    using c_sound_cache_bulk_reader::m_pending_requests;
    using c_sound_cache_bulk_reader::m_io_arena;
    c_indirect_cache_file_location_atlas *m_atlas;
    c_indirect_cache_file_bulk_read_iterator *m_iterator;
    const s_indirect_cache_file_read_request *m_request;
    unsigned long *m_total_bytes;
};

typedef char cache_file_tag_resource_bulk_reader_size[
    (sizeof(void *) != 4 || ((sizeof(c_cache_file_tag_resource_bulk_reader) == 0x54))) ? 1 : -1];

#endif // CACHE_CACHE_FILE_TAG_RESOURCE_BULK_READER_H
