#ifndef CACHE_TAG_RESOURCE_LRUV_CACHE_CALLS_H
#define CACHE_TAG_RESOURCE_LRUV_CACHE_CALLS_H

// Include after the page table definition, which owns the nested cache type.
extern "C" void tag_resource_lruv_cache_allocate_buffer(c_tag_resource_page_table::c_tag_resource_lruv_cache *cache, unsigned long size);

#endif // CACHE_TAG_RESOURCE_LRUV_CACHE_CALLS_H
