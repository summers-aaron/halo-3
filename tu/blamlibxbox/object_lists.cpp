// blamlibxbox/object_lists.obj: from the records 0xd5218 0x120380 0x120400 0x120570 0x263c48 0x263c60 0x263c78 0x263d40 0x263e38 0x263ed8 0x263ef8 0x263f98 0x2b3798 0x2b4530 0x509510 0x708360 0x708370
// @flags /TP /O2
// @address 0x11df7cc g_object_list_header_data
// @address 0x11df86c g_list_object_reference_data
#include "memory/data_types.h"
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "cache/restricted_memory_regions.h"
#include "hs/object_lists.h"
#include "objects/reference_lists.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "objects/object_list_header_types.h"
#include "objects/list_reference_types.h"
#include "hs/hs_runtime_data.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "cache/restricted_memory_registration.h"
#include "camera/director.h"
#include "memory/tls_effect_data.h"
#include "cache/restricted_memory_layout.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_region_members.h"
#include "memory/datum_index_inlines.h"

__declspec(thread) s_data_array *g_object_list_header_data;
__declspec(thread) s_data_array *g_list_object_reference_data;

namespace {
struct s_object_list_header_data_publisher
{
    static __forceinline void publish(void *address)
    {
        data_set_new_base_address(&tls_object_list_header_data(), (s_data_array *)address);
    }
};

struct s_list_object_reference_data_publisher
{
    static __forceinline void publish(void *address)
    {
        data_set_new_base_address(&tls_list_object_reference_data(), (s_data_array *)address);
    }
};
}

// @address 0xd5218 object_lists_dispose_empty_lists
void object_lists_dispose_empty_lists(void)
{
    long object_list_index = ::data_next_index(tls_object_list_header_data(), -1);
    while (object_list_index != -1)
    {
        s_object_list_header *object_list = datum_get<s_object_list_header>(tls_object_list_header_data(), object_list_index);
        if (object_list->count == 0 && object_list_index != -1)
        {
            long reference_index = object_list->first_list_reference;
            s_data_array *reference_data = tls_list_object_reference_data();
            while (reference_index != -1)
            {
                long next_reference_index = ((s_list_reference *)datum_get_absolute(reference_data,
                    datum_index_to_absolute_index(reference_index)))->next_index;
                ::datum_delete(reference_data, reference_index);
                reference_index = next_reference_index;
            }
            ::datum_delete(tls_object_list_header_data(), object_list_index);
        }
        s_data_array *object_list_data = tls_object_list_header_data();
        object_list_index = datum_absolute_index_to_index(object_list_data,
            data_next_absolute_index(object_list_data, (object_list_index + 1) & 0xffff));
    }
}

long object_list_new(void)
{
  long object_list_index = ::datum_new(tls_object_list_header_data());
  if (object_list_index != -1) {
    s_object_list_header *list = (s_object_list_header *)(tls_object_list_header_data()->data + 12 * (unsigned short)object_list_index);
    list->reference_count = 0;
    list->first_list_reference = -1;
    tls_hs_runtime_globals()->require_object_list_gc = true;
  }
  return object_list_index;
}

// @address 0x120570 ?object_list_get_first@@YAJJPAJ@Z
long object_list_get_first(long object_list_index, long *reference_index)
{
  s_data_array *array = tls_list_object_reference_data();
  if (*reference_index != -1) {
    s_list_reference *datum = (s_list_reference *)(array->data + (unsigned short)*reference_index * array->size);
    long next_index = datum->next_index;
    if (next_index != -1)
      __dcbt(0, array->data + (unsigned short)next_index * array->size);
    long object_index = datum->value;
    *reference_index = next_index;
    return object_index;
  }
  return -1;
}

void __tls_set_g_object_list_data_allocator(void *object)
{
    data_set_new_base_address(tls_particle_data_address(), (s_data_array *)object);
}

void __tls_set_g_object_list_header_data_allocator(void *object)
{
    data_set_new_base_address(tls_contrail_data_address(), (s_data_array *)object);
}



// @address 0x120400 object_list_add
void object_list_add(long object_list_index, long object_index)
{
    s_object_list_header *object_list = datum_get<s_object_list_header>(tls_object_list_header_data(), object_list_index);
    reference_list_add(tls_list_object_reference_data(), &object_list->first_list_reference, object_index, 0, 0);
    object_list->reference_count++;
}


static __forceinline long object_list_count_next_object(s_data_array *array, long *reference_index)
{
    long object_index;
    if (*reference_index != -1)
    {
        s_list_reference *datum = DATUM_GET(array, s_list_reference, *reference_index);
        long next_index = datum->next_index;
        if (next_index != -1)
            __dcbt(0, datum_get(array, next_index));
        object_index = datum->value;
        *reference_index = next_index;
    }
    else
    {
        object_index = -1;
    }
    return object_index;
}

// @address 0x263c48 ?object_lists_set_header_data_allocator@@YAXPAX@Z
void object_lists_set_header_data_allocator(void *object)
{
    data_set_new_base_address(&tls_object_list_header_data(), (s_data_array *)object);
}

// @address 0x263c60 ?object_lists_set_data_allocator@@YAXPAX@Z
void object_lists_set_data_allocator(void *object)
{
    data_set_new_base_address(&tls_list_object_reference_data(), (s_data_array *)object);
}

// @address 0x263c78 object_list_delete_object
unsigned char object_list_delete_object(unsigned short object_list_index, long object_index)
{
    bool result = false;
    bool found = false;
    s_data_array *array = tls_list_object_reference_data();
    s_object_list_header *object_list = (s_object_list_header *)&tls_object_list_header_data()->data[(unsigned short)object_list_index
        * sizeof(s_object_list_header)];
    long *first_reference = &object_list->first_list_reference;
    long reference_index = *first_reference;
    while (reference_index != -1)
    {
        s_list_reference *datum = DATUM_GET(array, s_list_reference, reference_index);
        if (datum->value == object_index)
        {
            found = true;
            break;
        }
        reference_index = datum->next_index;
    }
    if (found)
    {
        ::reference_list_remove(array, (long *)(first_reference), object_index);
        result = true;
        object_list->reference_count--;
    }
    return result;
}

// @address 0x263d40 object_list_get_count_not_dead
extern "C" short object_list_get_count_not_dead(long object_list_index)
{
    short count = 0;
    if (object_list_index != -1)
    {
        s_object_list_header *object_list = datum_get<s_object_list_header>(tls_object_list_header_data(), object_list_index);
        long reference_index = object_list->first_list_reference;
        long object_index = object_list_get_first(object_list_index, &reference_index);
        if (object_index != -1)
        {
            do
            {
                s_object_header *header = object_header_get(object_index);
                if ((_rotl(header->datum->damage_flags, 30) & 1) == 0)
                    count = (short)(count + 1);
                object_index = object_list_count_next_object(tls_list_object_reference_data(), &reference_index);
            } while (object_index != -1);
        }
    }
    return count;
}

// @address 0x263e38 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?object_lists_set_header_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &object_lists_set_header_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<s_object_list_header_data_publisher>(3, name, "", size, 0,
        object_lists_set_header_data_allocator);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x263ed8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?object_lists_set_header_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &object_lists_set_header_data_allocator>::deallocate(void *address)
{
    tls_object_list_header_data() = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x263ef8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?object_lists_set_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &object_lists_set_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<s_list_object_reference_data_publisher>(3, name, "", size, 0,
        object_lists_set_data_allocator);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x263f98 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?object_lists_set_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &object_lists_set_data_allocator>::deallocate(void *address)
{
    tls_list_object_reference_data() = 0;
    restricted_region_free_member(3, m_member_index);
}

static __inline long object_lists_cluster_reference_next(long *iterator)
{
    s_data_array *array = tls_list_object_reference_data();
    long value;
    if (*iterator != -1) {
        s_list_reference *datum = (s_list_reference *)(array->data + (unsigned short)*iterator * array->size);
        long next_index = datum->next_index;
        if (next_index != -1)
            __dcbt(0, array->data + (unsigned short)next_index * array->size);
        value = datum->value;
        *iterator = next_index;
    }
    else
        value = -1;
    return value;
}

// @address 0x509510 ?object_list_set_ignored@@YAXJE@Z
void object_list_set_ignored(long cluster_index, unsigned char set)
{
    long iterator;
    long object_index = -1;
    long reference;
    long current;
    if (cluster_index != -1) {
        iterator = datum_get<s_object_list_header>(tls_object_list_header_data(), cluster_index)->first_list_reference;
        object_index = object_list_get_first(cluster_index, &iterator);
    }
    current = object_index;
    if (current != -1) {
        reference = iterator;
        do {
            s_data_array *object_array = tls_object_header_data();
            s_object_header *header = 0;
            if (current != -1) {
                long absolute_index = current & 0xffff;
                if (absolute_index < object_array->first_unallocated) {
                    s_object_header *candidate = (s_object_header *)(object_array->data + object_array->size * absolute_index);
                    long identifier = candidate->identifier;
                    if (identifier != 0 && identifier == (current >> 16))
                        header = candidate;
                }
            }
            if (header && ((1U << header->object_type) & 0x1003) != 0) {
                unit_datum *object = (unit_datum *)header->datum;
                if (object) {
                    if (set)
                        object->unit.unit_flags |= 128;
                    else
                        object->unit.unit_flags &= ~128;
                }
            }
            current = object_lists_cluster_reference_next(&reference);
        } while (current != -1);
    }
}


c_hs_object_list_header_allocator g_hs_object_list_header_allocator;
c_hs_object_list_reference_allocator g_hs_object_list_reference_allocator;

