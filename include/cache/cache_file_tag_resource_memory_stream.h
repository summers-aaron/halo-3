#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_MEMORY_STREAM_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_MEMORY_STREAM_H

#include "memory/basic_buffer.h"

struct s_cache_file_tag_resource_memory_stream
{
    virtual bool set_buffer(c_basic_buffer<void> buffer);
    virtual int write(c_basic_buffer<void> data, c_basic_buffer<void> *written);
    virtual int is_at_end(c_basic_buffer<void> *out);

    __forceinline s_cache_file_tag_resource_memory_stream() : m_position(0), m_buffer() {}

    int m_position;
    c_basic_buffer<void> m_buffer;
};
STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_memory_stream) == 0x10, runtime_memory_stream_size);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_memory_stream, m_position) == 4, runtime_memory_stream_position_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_memory_stream, m_buffer) == 8, runtime_memory_stream_buffer_offset);

#endif
