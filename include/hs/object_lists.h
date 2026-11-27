#ifndef HS_OBJECT_LISTS_H
#define HS_OBJECT_LISTS_H

// Functions defined in tu/blamlibxbox/object_lists.cpp.

#include "memory/data.h"
#include "objects/object_list_data.h"
#include "objects/object_list_header_types.h"

long object_list_new(void);
long object_list_get_first(long object_list_index, long *reference_index);

// Prime *reference_index from the list header, then fetch the first object; -1 when there is no list.
static __forceinline long object_list_get_first_or_none(long object_list_index, long *reference_index)
{
	long object_index = -1;
	if (object_list_index != -1) {
		*reference_index = datum_get<s_object_list_header>(g_object_list_header_data, object_list_index)->first_list_reference;
		object_index = object_list_get_first(object_list_index, reference_index);
	}
	return object_index;
}
void __tls_set_g_object_list_data_allocator(void *object);
void __tls_set_g_object_list_header_data_allocator(void *object);
extern "C" short object_list_get_count_not_dead(long object_list_index);
void object_list_set_ignored(long cluster_index, unsigned char set);
extern "C" void object_lists_dispose_empty_lists(void);
extern "C" void object_list_add(long object_list_index, long object_index);
extern "C" unsigned char object_list_delete_object(unsigned short object_list_index, long object_index);
extern "C" void object_lists_initialize(void);
extern "C" void director_set_globals_allocation(void);
extern "C" void director_set_flag_allocation(void);

void object_lists_set_header_data_allocator(void *object);
void object_lists_set_data_allocator(void *object);
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;

typedef t_restricted_allocation_manager<3,
	static_cast<void (*)(void *)>(0),
	static_cast<void (*)(void *)>(0),
	&object_lists_set_header_data_allocator> c_hs_object_list_header_allocator;
typedef t_restricted_allocation_manager<3,
	static_cast<void (*)(void *)>(0),
	static_cast<void (*)(void *)>(0),
	&object_lists_set_data_allocator> c_hs_object_list_reference_allocator;

extern c_hs_object_list_header_allocator g_hs_object_list_header_allocator;
extern c_hs_object_list_reference_allocator g_hs_object_list_reference_allocator;

#endif // HS_OBJECT_LISTS_H
