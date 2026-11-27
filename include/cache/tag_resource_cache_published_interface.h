#ifndef CACHE_TAG_RESOURCE_CACHE_PUBLISHED_INTERFACE_H
#define CACHE_TAG_RESOURCE_CACHE_PUBLISHED_INTERFACE_H

#include "cache/tag_resource_cache_new.h"
#include "tag_files/tag_resource_cache_external_access.h"

class c_tag_resource_cache_published_interface
{
public:
    c_tag_resource_cache_new *m_cache;
    void prepare_for_delete(long handle);
    __forceinline bool address_is_locked(long handle) const { return m_cache->resource_is_locked_externally(handle); }
    __forceinline void release_address(long handle) { tag_resource_cache_new_release_page(m_cache, handle); }
    __forceinline void prepare_for_move(long handle) { tag_resource_cache_new_handle_page_move(m_cache, handle); }
};

#endif
