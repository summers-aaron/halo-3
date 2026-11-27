#ifndef CACHE_RESTRICTED_MEMORY_LAYOUT_H
#define CACHE_RESTRICTED_MEMORY_LAYOUT_H

#include <stddef.h>

// Plain storage layouts used by restricted-memory registration. The s_* names are data views of
// c_restricted_memory.
// Owner evidence: March cache\restricted_memory.cpp:677-715 and
// cache\restricted_memory_regions.h:146,155,156. This leaf header keeps the layouts independent
// of the owning classes' inline code while their remaining private definitions are migrated.
class c_restricted_section;

typedef void (*field_proc_t)(void *field);

struct s_layout_field
{
    unsigned int unaligned_offset;
    unsigned int offset;
    unsigned int size;
    field_proc_t construct;                    // updates the user's address (historical view name)
    field_proc_t post_copy;
    field_proc_t pre_overwrite;
};

// Storage of c_restricted_memory::s_mirror_slot. These words are synchronization state;
// no constructors or synchronization operations belong to this plain layout.
struct s_layout_mirror_slot
{
    volatile long readers;
    volatile long valid;
    volatile long locked;
    c_restricted_section *section;
};

struct s_field_layout
{
    volatile long primary_locked;
    volatile long mirror_locked;
    s_layout_mirror_slot mirrors[4];
    volatile long primary_readers;
    volatile long mirror_readers;
    volatile long primary_valid;
    volatile long mirror_valid;               // also used as the alias count
    c_restricted_section *primary_section;
    unsigned long flags;                      // (threads_unlock_restricted_region)
    unsigned long pad;                        // matches c_restricted_memory::m_pad
    unsigned int subsection_size;
    unsigned int reserved_size;
    long mirror_count;
    long critical_section;
    unsigned int field_count;
    s_layout_field fields[128];
    void *listener;
    void *primary_allocation;
    void *mirror_allocation;
    long region_index;
};

// These layouts hold pointers; the size checks below apply when pointers are 4 bytes.
typedef char s_layout_field_size[(sizeof(void *) != 4 || sizeof(s_layout_field) == 0x18) ? 1 : -1];
typedef char s_layout_field_unaligned_offset[(offsetof(s_layout_field, unaligned_offset) == 0) ? 1 : -1];
typedef char s_layout_field_offset[(offsetof(s_layout_field, offset) == 4) ? 1 : -1];
typedef char s_layout_field_requested_size[(offsetof(s_layout_field, size) == 8) ? 1 : -1];
typedef char s_layout_field_construct[(sizeof(void *) != 4 || offsetof(s_layout_field, construct) == 0xc) ? 1 : -1];
typedef char s_layout_field_post_copy[(sizeof(void *) != 4 || offsetof(s_layout_field, post_copy) == 0x10) ? 1 : -1];
typedef char s_layout_field_pre_overwrite[(sizeof(void *) != 4 || offsetof(s_layout_field, pre_overwrite) == 0x14) ? 1 : -1];
typedef char s_layout_mirror_slot_size[(sizeof(void *) != 4 || sizeof(s_layout_mirror_slot) == 0x10) ? 1 : -1];
typedef char s_field_layout_size[(sizeof(void *) != 4 || sizeof(s_field_layout) == 0xc88) ? 1 : -1];
typedef char s_field_layout_flags[(sizeof(void *) != 4 || offsetof(s_field_layout, flags) == 0x5c) ? 1 : -1];
typedef char s_field_layout_subsection_size[(sizeof(void *) != 4 || offsetof(s_field_layout, subsection_size) == 0x64) ? 1 : -1];
typedef char s_field_layout_reserved_size[(sizeof(void *) != 4 || offsetof(s_field_layout, reserved_size) == 0x68) ? 1 : -1];
typedef char s_field_layout_count[(sizeof(void *) != 4 || offsetof(s_field_layout, field_count) == 0x74) ? 1 : -1];
typedef char s_field_layout_fields[(sizeof(void *) != 4 || offsetof(s_field_layout, fields) == 0x78) ? 1 : -1];
typedef char s_field_layout_listener[(sizeof(void *) != 4 || offsetof(s_field_layout, listener) == 0xc78) ? 1 : -1];
typedef char s_field_layout_region[(sizeof(void *) != 4 || offsetof(s_field_layout, region_index) == 0xc84) ? 1 : -1];

#endif // CACHE_RESTRICTED_MEMORY_LAYOUT_H
