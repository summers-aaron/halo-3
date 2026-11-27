#ifndef OBJECTS_OBJECT_MULTIPLAYER_ACCESS_H
#define OBJECTS_OBJECT_MULTIPLAYER_ACCESS_H

#include "objects/object_header.h"
#include "objects/multiplayer_object_types.h"

static __forceinline s_multiplayer_object_properties *object_try_and_get_multiplayer(long object_index)
{
    s_multiplayer_object_properties *multiplayer = 0;
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, NONE);
    if (object && object->multiplayer_data_offset != NONE)
        multiplayer = (s_multiplayer_object_properties *)object_header_block_get(object, &object->multiplayer_properties);
    return multiplayer;
}

#endif // OBJECTS_OBJECT_MULTIPLAYER_ACCESS_H
