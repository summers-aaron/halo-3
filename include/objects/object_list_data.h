#ifndef OBJECTS_OBJECT_LIST_DATA_H
#define OBJECTS_OBJECT_LIST_DATA_H

struct s_data_array;

extern "C" {
extern __declspec(thread) s_data_array *g_object_list_header_data;
extern __declspec(thread) s_data_array *g_list_object_reference_data;
}

#endif // OBJECTS_OBJECT_LIST_DATA_H
