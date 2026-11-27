#ifndef OBJECTS_OBJECT_HEADER_DATA_H
#define OBJECTS_OBJECT_HEADER_DATA_H

struct s_data_array;

// Thread-local object header datum array (TLS slot 0x831df6a8), owned by objects.cpp (March 2007 debug name and type).
extern __declspec(thread) s_data_array *object_header_data;

#endif // OBJECTS_OBJECT_HEADER_DATA_H
