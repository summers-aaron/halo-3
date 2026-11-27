// objectslibxbox/reference_lists.obj: from the records 0xe3468 0x35df98
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/data.h"
#include "objects/reference_lists.h"

void reference_list_remove(s_data_array *references, long *reference_index, long element)
{
    while (*reference_index != -1)
    {
        s_reference_list_datum *reference = DATUM_GET(references, s_reference_list_datum, *reference_index);
        if (reference->element == element)
        {
            long next_reference = reference->next_reference;
            ::datum_delete(references, *reference_index);
            *reference_index = next_reference;
            return;
        }
        reference_index = &reference->next_reference;
    }
}

// @address 0x35df98 ?reference_list_new@@YAPAUs_data_array@@PBDJJPAVc_allocation_base@@@Z
s_data_array *reference_list_new(const char *name, long payload_size,
    long maximum_count, c_allocation_base *allocator)
{
    char reference_name[256];
    s_data_array *result;
    csnzprintf(reference_name, sizeof(reference_name), "%s reference", name);
    if (payload_size + sizeof(s_reference_list_datum) == 32)
    {
        result = data_new(reference_name, maximum_count,
            payload_size + sizeof(s_reference_list_datum), 5, allocator);
    }
    else
    {
        result = data_new(reference_name, maximum_count,
            payload_size + sizeof(s_reference_list_datum), 0, allocator);
    }
    return result;
}
