#ifndef CACHE_TAG_RESOURCE_EXTERNAL_ACCESS_STATE_H
#define CACHE_TAG_RESOURCE_EXTERNAL_ACCESS_STATE_H

#include <stddef.h>

struct s_tag_resource_cache_external_access_key {
    long value;
    long first;
    long second;
    long third;
};
struct s_tag_resource_cache_external_access_hash_table {
    long count;
    s_tag_resource_cache_external_access_key keys[8192];
    short bucket_counts[8192];
    short slot_buckets[8192];
};

struct s_range {
    unsigned short unused;
    unsigned short count;
    int offset;
};
struct s_range_set {
    unsigned short range_count;
    short first_range;
    unsigned short index_count;
    short first_index;
};
struct s_tag_resource_index_buffer { int *elements; };
struct s_ranges {
    int index_count;
    union { int *indices; s_tag_resource_index_buffer index_buffer; };
    int range_count;
    s_range *ranges;
    int range_index_count;
    int *range_indices;
    unsigned long set_count;
    s_range_set *sets;
};
struct tag_resource_external_access_key_list {
    long data[0x4000];
    long count;
};
struct s_hash_entry { int value, key0, key1, key2; };
struct s_tag_resource_cache_group_hash_table {
    int entry_count;
    s_hash_entry entries[2048];
    short hash_entry_count[2048];
    short original_hash_index[2048];
};
struct s_tag_resource_cache_external_access_pair { long count; void *elements; };
struct s_tag_resource_cache_external_access_allocation_slot { unsigned short owner; short generation; unsigned short count; short next; };
struct s_tag_resource_cache_external_access_group { short identifier; unsigned short count; long first; };
template <typename t_type, long k_maximum_count>
struct t_tag_resource_cache_external_access_list {
    t_type elements[k_maximum_count];
    long count;
};
class c_tag_resource_cache_external_access_iterator;
struct s_tag_resource_cache_external_access_published_table {
    s_tag_resource_cache_external_access_hash_table hash_table;
    union { s_tag_resource_cache_external_access_pair pairs[4]; s_ranges ranges; };
};
struct s_tag_resource_cache_external_access_state : s_tag_resource_cache_external_access_published_table {
    s_tag_resource_cache_group_hash_table group_hash_table;
    c_tag_resource_cache_external_access_iterator *iterator;
    unsigned short group_identifier;
    tag_resource_external_access_key_list key_list;
    s_tag_resource_cache_external_access_group groups[0x9000];
    long group_count;
    t_tag_resource_cache_external_access_list<long, 0x9000> value_list;
    struct s_slot_list {
        s_tag_resource_cache_external_access_allocation_slot slots[0x2000];
        long count;
    } slot_list;
    unsigned char key_list_overflow_reported;
    unsigned char group_overflow_reported;
    unsigned char value_overflow_reported;
    unsigned char slot_overflow_reported;
    unsigned char key_list_overflow;
    unsigned char group_overflow;
    unsigned char value_overflow;
    unsigned char slot_overflow;
};

typedef char external_access_state_key_list_offset[
    (sizeof(void *) != 4 || ((offsetof(s_tag_resource_cache_external_access_state, key_list) == 0x32030))) ? 1 : -1];
typedef char external_access_state_groups_offset[
    (sizeof(void *) != 4 || ((offsetof(s_tag_resource_cache_external_access_state, groups) == 0x42034))) ? 1 : -1];
typedef char external_access_state_slots_offset[
    (sizeof(void *) != 4 || ((offsetof(s_tag_resource_cache_external_access_state, slot_list) == 0xae03c))) ? 1 : -1];
typedef char external_access_state_flags_offset[
    (sizeof(void *) != 4 || ((offsetof(s_tag_resource_cache_external_access_state, key_list_overflow_reported) == 0xbe040))) ? 1 : -1];

typedef char external_access_state_overflow_offset[
    (sizeof(void *) != 4 || ((offsetof(s_tag_resource_cache_external_access_state, key_list_overflow) == 0xbe044))) ? 1 : -1];
typedef char external_access_state_size[(sizeof(void *) != 4 || ((sizeof(s_tag_resource_cache_external_access_state) == 0xbe048))) ? 1 : -1];

#endif
