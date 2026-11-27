#ifndef PHYSICS_CHARACTER_PHYSICS_OBJECT_ACCESS_H
#define PHYSICS_CHARACTER_PHYSICS_OBJECT_ACCESS_H
#include "objects/object_datum.h"
#include "objects/object_header_access.h"
#include "memory/tls_globals.h"

static __forceinline unsigned char character_physics_mode_ground_object_type(long index)
{
    return ((const object_header_datum *)object_header_data->data)[(unsigned short)index].object_type;
}

static __forceinline s_object_header *character_physics_mode_ground_get_object_header(long object_index)
{
    return object_header_get(object_index);
}

static __forceinline e_object_type character_physics_mode_ground_contact_object_type(long index)
{
    // match: 0xe7498: zero-bit rotate keeps the header-data TLS base in r10 at the contact-type load; removal recolors it to r9
    return (e_object_type)((const s_object_header *)object_header_data->data)[(unsigned short)_rotl(index, 0)].object_type;
}
#endif
