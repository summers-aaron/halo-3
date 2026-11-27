#ifndef OBJECTS_OBJECT_ATTACHMENT_TYPES_H
#define OBJECTS_OBJECT_ATTACHMENT_TYPES_H

#include <stddef.h>

// object_datum::attached_lights is an array of type/datum pairs (objects.cpp).
struct s_object_datum_attachment
{
    signed char type;
    char alignment[3];
    long datum_index;
};

typedef char object_datum_attachment_size[(sizeof(s_object_datum_attachment) == 8) ? 1 : -1];
typedef char object_datum_attachment_index_offset[(offsetof(s_object_datum_attachment, datum_index) == 4) ? 1 : -1];

#endif
