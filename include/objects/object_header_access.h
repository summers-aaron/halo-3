#ifndef OBJECTS_OBJECT_HEADER_ACCESS_H
#define OBJECTS_OBJECT_HEADER_ACCESS_H

#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"

static __forceinline s_object_header *object_header_get(datum_index object_index) {
    return &((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)];
}

static __forceinline const s_object_header *object_header_get_const(datum_index object_index) {
    return &((const s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)];
}

static __forceinline object_datum *object_get(datum_index object_index) {
    return object_header_get(object_index)->datum;
}

static __forceinline void *object_get_and_verify_type(datum_index object_index, unsigned long type_mask) {
    (void)type_mask;
    return object_header_get(object_index)->datum;
}

static __forceinline void object_get_bounding_sphere(datum_index object_index, real_point3d *center, float *radius) {
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 0xffffffff);
    *center = object->bounding_center;
    *radius = object->bounding_radius;
}

// The object a handle names when its header is live and its type is in the mask, else null.
static __forceinline void *object_try_and_get_and_verify_type_inline(datum_index object_index, unsigned long type_mask)
{
    s_data_array *data = object_header_data;
    void *result = 0;
    if (object_index != -1) {
        long absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);
        if (absolute_index < data->first_unallocated) {
            s_object_header *header = (s_object_header *)(data->data + data->size * absolute_index);
            short identifier = header->identifier;
            if (identifier != 0 && identifier == (object_index >> 16) && ((1 << header->object_type) & type_mask) != 0)
                result = header->datum;
        }
    }
    return result;
}

#endif // OBJECTS_OBJECT_HEADER_ACCESS_H
