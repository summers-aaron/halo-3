#ifndef OBJECTS_OBJECT_REGION_INFORMATION_INLINE_H
#define OBJECTS_OBJECT_REGION_INFORMATION_INLINE_H

#include "objects/object_header.h"

static __forceinline void object_get_region_information_inline(long object_index, long *region_count,
    char **region_permutations, char **visible_permutations, object_region_information **region_information)
{
    object_datum *object = object_get(object_index);
    *region_count = object->region_information.size;
    char *base = (char *)object + object->region_information.offset;
    if ((bool)((object->flags >> _object_has_model_instances_bit) & 1))
        *region_count -= 16;
    *region_count /= (long)(sizeof(object_region_information) + 2 * sizeof(char));
    if (region_information)
        *region_information = (object_region_information *)base;
    if (region_permutations)
        *region_permutations = base + *region_count * (long)sizeof(object_region_information);
    if (visible_permutations)
        *visible_permutations = base + *region_count * (long)(sizeof(object_region_information) + sizeof(char));
}

#endif
