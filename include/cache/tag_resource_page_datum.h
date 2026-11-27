#ifndef CACHE_TAG_RESOURCE_PAGE_DATUM_H
#define CACHE_TAG_RESOURCE_PAGE_DATUM_H

#include <stddef.h>
#include "memory/basic_buffer.h"

struct s_tag_resource_page_buffer
{
    const char *data;
    unsigned long size;
};

// Page-table datum; byte offsets are shared by paging and external access.
#pragma pack(push, 4)
struct s_tag_resource_page_datum
{
    unsigned short salt;
    unsigned char flags;
    unsigned char state;
    short references;
    short io_references;
    short io_lock_count;
    union {
        struct { c_basic_buffer<void> buffer; };
        __int64 address;
    };
};
#pragma pack(pop)

typedef char tag_resource_page_size[(sizeof(void *) != 4 || ((sizeof(s_tag_resource_page_datum) == 0x14))) ? 1 : -1];
typedef char tag_resource_page_state_offset[(offsetof(s_tag_resource_page_datum, state) == 3) ? 1 : -1];
typedef char tag_resource_page_external_references_offset[(offsetof(s_tag_resource_page_datum, io_lock_count) == 8) ? 1 : -1];
typedef char tag_resource_page_buffer_offset[(offsetof(s_tag_resource_page_datum, buffer) == 0xc) ? 1 : -1];

#endif
