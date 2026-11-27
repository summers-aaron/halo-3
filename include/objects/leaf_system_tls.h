#ifndef OBJECTS_LEAF_SYSTEM_TLS_H
#define OBJECTS_LEAF_SYSTEM_TLS_H

struct s_data_array;

extern "C" __declspec(thread) s_data_array * g_leaf_system_data;

#endif
