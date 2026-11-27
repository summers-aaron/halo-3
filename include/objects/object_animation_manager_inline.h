#ifndef OBJECTS_OBJECT_ANIMATION_MANAGER_INLINE_H
#define OBJECTS_OBJECT_ANIMATION_MANAGER_INLINE_H

#include "cseries/cseries.h"
#include "objects/object_header_access.h"

// objects.obj: true when the object owns an animation manager block.
static __forceinline bool object_has_animation_manager(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
    return object->animation.offset != -1;
}

#endif
