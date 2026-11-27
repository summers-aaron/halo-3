#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_SORT_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_SORT_H

#include "cache/cache_file_tag_resource_relocation.h"

struct s_cache_file_tag_resource_sort_predicate
{
    bool operator()(const s_cache_file_tag_resource_relocation &a, const s_cache_file_tag_resource_relocation &b) const
    {
        return a.buffer.m_address < b.buffer.m_address;
    }
};

extern "C" void main_game_sort_heap(s_cache_file_tag_resource_relocation *first, s_cache_file_tag_resource_relocation *last,
    s_cache_file_tag_resource_sort_predicate predicate);

extern "C" void cache_file_tag_resource_sort_adjust_heap(s_cache_file_tag_resource_relocation *first, long hole, long bottom,
    s_cache_file_tag_resource_relocation value, s_cache_file_tag_resource_sort_predicate predicate);

#endif // CACHE_CACHE_FILE_TAG_RESOURCE_SORT_H
