#ifndef OBJECTS_REFERENCE_LISTS_H
#define OBJECTS_REFERENCE_LISTS_H

// Functions defined in tu/objectslibxbox/reference_lists.cpp that other units call.

#include "memory/data.h"
#include "memory/data_typed_access.h"
#include "memory/datum_index.h"
#include "cseries/crt_xdk.h"

struct s_reference_list_datum
{
    short identifier;
    short padding;
    long element;
    long next_reference;
};

void reference_list_remove(s_data_array *references, long *reference_index, long element);
s_data_array *reference_list_new(const char *name, long payload_size, long maximum_count, c_allocation_base *allocator);

static __forceinline void reference_list_update_payload(
    s_data_array *array, long *head, long datum_index, long payload_size, const void *payload)
{
    long reference_index = *head;
    void *destination;
    while (reference_index != -1) {
        long absolute_index = datum_index_to_absolute_index(reference_index);
        void *allocation = datum_get_absolute(array, absolute_index);
        s_reference_list_datum *element = (s_reference_list_datum *)allocation;
        if (element->element == datum_index) {
            destination = element + 1;
            memcpy(destination, payload, payload_size);
            break;
        } else {
            reference_index = element->next_reference;
        }
    }
}

static __forceinline void reference_list_add(s_data_array *array, long *first, long datum_index, long payload_size, const void *payload)
{
    long index = datum_new(array);
    if (index != -1) {
        long absolute_index = datum_index_to_absolute_index(index);
        void *allocation = datum_get_absolute(array, absolute_index);
        s_reference_list_datum *element = (s_reference_list_datum *)allocation;
        element->element = datum_index;
        element->next_reference = *first;
        *first = index;
        if (payload)
            memcpy(element + 1, payload, payload_size);
    }
}

#endif // OBJECTS_REFERENCE_LISTS_H
