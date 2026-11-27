#ifndef INCLUDE_OBJECTS_RECYCLING_GROUPS_H
#define INCLUDE_OBJECTS_RECYCLING_GROUPS_H

struct s_data_array;


// March 2007 TLS slot 0x831df6f0.
extern __declspec(thread) s_data_array *recycling_groups;

#endif // INCLUDE_OBJECTS_RECYCLING_GROUPS_H
