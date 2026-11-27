#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_WINDOW_ALLOCATOR_STORAGE_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_WINDOW_ALLOCATOR_STORAGE_H

#include "cache/tag_resource_page_allocator.h"

struct s_cache_file_resource_element_set;

// The runtime's page allocator: paging asks it for the location, size, hash, position and request priority of a
// datum index's page in the runtime element set.
class c_cache_file_tag_resource_runtime_window_allocator_storage : public c_tag_resource_page_allocator
{
public:
    virtual bool get_page_location(long handle, s_tag_resource_page_location *location);
    virtual long get_page_size(unsigned int handle);
    virtual bool get_page_hash(long handle, s_tag_resource_page_hash_pair *hash);
    virtual __int64 get_page_position(long handle);
    virtual unsigned long get_request_priority(long handle);

    s_cache_file_resource_element_set *m_element_set;
};

#endif
