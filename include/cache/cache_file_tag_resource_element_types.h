#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_ELEMENT_TYPES_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_ELEMENT_TYPES_H

#include <stddef.h>

#include "networking/logic/storage/network_http_request_hash.h"
#include "tag_files/tag_types.h"

// Shared 88-byte resource element used by the fixed and dynamic page arrays.
struct s_cache_file_tag_resource_element
{
    short salt;
    unsigned char flags;
    signed char file_index;
    short slot;
    short global_page_index;
    long offset;
    long compressed_size;
    union
    {
        long size;
        TAG_POINTER(void) runtime_address;
    };
    union
    {
        unsigned char payload[64];
        struct
        {
            // Same word as s_tag_resource_page_hash_pair::status: get_page_hash copies
            // the whole 64-byte payload into that pair, so payload[0:4] is its status.
            long status;
            s_network_http_request_hash hash;
        };
    };
    unsigned short first_handle;
    short shared_page_index;
};
typedef char cache_resource_element_size[(sizeof(void *) != 4 || ((sizeof(s_cache_file_tag_resource_element) == 88))) ? 1 : -1];
typedef char cache_resource_element_runtime_address_offset[(offsetof(s_cache_file_tag_resource_element, runtime_address) == 16) ? 1 : -1];
typedef char cache_resource_element_payload_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_file_tag_resource_element, payload) == 20))) ? 1 : -1];
typedef char cache_resource_element_hash_offset[(sizeof(void *) != 4 || ((offsetof(s_cache_file_tag_resource_element, hash) == 24))) ? 1 : -1];
typedef char cache_resource_element_first_handle_offset[
    (sizeof(void *) != 4 || ((offsetof(s_cache_file_tag_resource_element, first_handle) == 84))) ? 1 : -1];
typedef char cache_resource_element_shared_page_index_offset[
    (sizeof(void *) != 4 || ((offsetof(s_cache_file_tag_resource_element, shared_page_index) == 86))) ? 1 : -1];


#endif
