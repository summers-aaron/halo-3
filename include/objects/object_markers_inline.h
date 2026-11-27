#ifndef OBJECTS_OBJECT_MARKERS_INLINE_H
#define OBJECTS_OBJECT_MARKERS_INLINE_H

#include "objects/objects.h"

static __forceinline short object_get_markers_by_string_id(long object_index, long marker_name, object_marker *markers,
    short maximum_count)
{
    return internal_object_get_markers_by_string_id(object_index, marker_name, markers, maximum_count, false);
}

#endif
