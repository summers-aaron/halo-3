#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_RELOCATION_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_RELOCATION_H

#include "cseries/cseries.h"
#include "memory/basic_buffer.h"

struct s_cache_file_tag_resource_page_key
{
    unsigned long file;
    unsigned long offset;
    unsigned long compressed_size;
    unsigned long uncompressed_size;
};
class c_cache_file_tag_resource_runtime_buffer : public c_basic_buffer<void>
{
public:
    // match: these constructors initialize the range directly; omit base zeroing.
    __forceinline c_cache_file_tag_resource_runtime_buffer() :
        c_basic_buffer<void>(c_basic_buffer<void>::_uninitialized)
    {
        m_address = NULL;
        m_size = 0;
    }
    __forceinline c_cache_file_tag_resource_runtime_buffer(
        c_basic_buffer<void> buffer, unsigned long offset, unsigned long size) :
        c_basic_buffer<void>(buffer, offset, size) {}
    __forceinline c_cache_file_tag_resource_runtime_buffer(const c_basic_buffer<void> &buffer, void *position) :
        c_basic_buffer<void>(position, buffer.m_size - ((char *)position - (char *)buffer.m_address)) {}
    __forceinline c_cache_file_tag_resource_runtime_buffer &operator=(const c_basic_buffer<void> &buffer)
    {
        c_basic_buffer<void>::operator=(buffer);
        return *this;
    }
};
struct s_cache_file_tag_resource_relocation
{
    s_cache_file_tag_resource_page_key key;
    c_basic_buffer<void> buffer;
};
struct s_cache_file_tag_resource_relocation_list
{
    __forceinline s_cache_file_tag_resource_relocation_list() : count(0) {}
    __forceinline s_cache_file_tag_resource_relocation *begin() { return entries; }
    __forceinline s_cache_file_tag_resource_relocation *end() { return entries + count; }
    __forceinline void reset_cursor() { cursor = 0; }
    __forceinline void sort();
    __forceinline void sort_by_key();
    __forceinline void reset();

    s_cache_file_tag_resource_relocation entries[16384];
    long count;
    c_cache_file_tag_resource_runtime_buffer window;
    unsigned long cursor;
};

#if defined(_M_PPC) || defined(_M_PPCBE) || defined(_XBOX)
STATIC_ASSERT(sizeof(s_cache_file_tag_resource_page_key) == 0x10, resource_relocation_key_size);
STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_relocation) == 0x18, resource_relocation_size);
STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_relocation_list) == 0x60010, resource_relocation_list_size);
#endif

#endif // CACHE_CACHE_FILE_TAG_RESOURCE_RELOCATION_H
