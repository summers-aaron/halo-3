#ifndef INCLUDE_TAG_FILES_TAG_GROUPS_H
#define INCLUDE_TAG_FILES_TAG_GROUPS_H

#include "cseries/cseries.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"

typedef s_tag_block tag_block;
typedef s_tag_reference tag_reference;
typedef s_tag_data tag_data;

// 0x827cff0c (?global_tag_instances@@3PBUcache_file_tag_instance@@B)
extern const cache_file_tag_instance *global_tag_instances;

// Out-of-line cache_files.obj functions
void *tag_try_and_get_unsafe(tag group_tag, datum_index tag_index);
datum_index tag_iterator_next(tag_iterator *iterator);

static __forceinline void *tag_get(tag group_tag, datum_index tag_index) {
    (void)group_tag;
    return global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index)].definition;
}

static __forceinline void *tag_block_get_element_with_size(const s_tag_block *block, long index, long element_size) {
    return (char *)block->address + index * element_size;
}

static __forceinline void *tag_data_get_pointer(const s_tag_data *data, long offset, long size) {
    (void)size;
    return (char *)data->address + offset;
}

#define TAG_GET(group_tag, type, tag_index) ((type *)tag_get((group_tag), (tag_index)))
#define TAG_TRY_AND_GET(group_tag, type, tag_index) ((type *)tag_try_and_get_unsafe((group_tag), (tag_index)))
#define TAG_BLOCK_GET_ELEMENT(block, index, type) ((type *)tag_block_get_element_with_size((block), (index), sizeof(type)))

#endif // INCLUDE_TAG_FILES_TAG_GROUPS_H
