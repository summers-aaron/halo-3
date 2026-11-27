#ifndef CACHE_TAG_RESOURCE_PAGE_ALLOCATOR_H
#define CACHE_TAG_RESOURCE_PAGE_ALLOCATOR_H

#include <stddef.h>
#include "networking/logic/storage/network_http_request_hash.h"

#pragma pack(push, 1)
struct s_tag_resource_page_hash_pair
{
    long status;
    s_network_http_request_hash complete;
    s_network_http_request_hash head;
    s_network_http_request_hash tail;
};
#pragma pack(pop)

struct s_tag_resource_page_location
{
    unsigned long offset;
    unsigned long size;
    unsigned long uncompressed_size;
    long hash_source_index;
    bool validate_hash;
    s_network_http_request_hash expected_hash;
};

// Paging uses one interface for locations, hashes, and synchronous request priorities, in this slot order.
class c_tag_resource_page_allocator
{
public:
    virtual bool get_page_location(long handle, s_tag_resource_page_location *location) = 0;
    virtual long get_page_size(unsigned int handle) = 0;
    virtual bool get_page_hash(long handle, s_tag_resource_page_hash_pair *hash) = 0;
    virtual __int64 get_page_position(long handle) = 0;
    virtual unsigned long get_request_priority(long handle) = 0;
};

typedef char paging_page_location_size[(sizeof(s_tag_resource_page_location) == 40) ? 1 : -1];
typedef char paging_page_location_hash_offset[(offsetof(s_tag_resource_page_location, expected_hash) == 17) ? 1 : -1];
typedef char paging_page_hash_pair_size[(sizeof(s_tag_resource_page_hash_pair) == 64) ? 1 : -1];

#endif
