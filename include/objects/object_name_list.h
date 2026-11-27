#ifndef OBJECTS_OBJECT_NAME_LIST_H
#define OBJECTS_OBJECT_NAME_LIST_H

// g_object_name_list: the object index placed under each scenario object name, indexed by name index
// (objects.cpp; March __tls_set_g_object_name_list_allocator, object_set_object_index_for_name_index).
extern "C" {
extern __declspec(thread) long *g_object_name_list;
}

static __forceinline long *tls_object_name_list() { return g_object_name_list; }

#endif // OBJECTS_OBJECT_NAME_LIST_H
