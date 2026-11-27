#ifndef CAMERA_DIRECTOR_STORAGE_TYPES_H
#define CAMERA_DIRECTOR_STORAGE_TYPES_H

#include "camera/director_vtables.h"
#include <stddef.h>

// Each slot holds the active director mode, constructed in place.
struct s_director_storage
{
#ifdef HALO3_64BIT
    __declspec(align(8)) unsigned char bytes[0x190];
#else
    __declspec(align(4)) unsigned char bytes[0x160];
#endif

    __forceinline c_director *get_director() { return static_cast<c_director *>(static_cast<void *>(bytes)); }
    __forceinline const c_director *get_director() const { return static_cast<const c_director *>(static_cast<const void *>(bytes)); }
};

struct s_director_slot
{
    int director_type;
    int perspective;
    int camera_type;
};

struct s_controllers
{
    s_director_storage directors[4];
    s_director_slot slots[4];
    float dt;
    float transition;
    float transition_remaining;
};

typedef char director_storage_size[(sizeof(void *) != 4 || (sizeof(s_director_storage) == 0x160)) ? 1 : -1];
typedef char director_storage_alignment[(sizeof(void *) != 4 || (__alignof(s_director_storage) == 4)) ? 1 : -1];
typedef char director_slot_table_offset[(sizeof(void *) != 4 || (offsetof(s_controllers, slots) == 0x580)) ? 1 : -1];
typedef char director_transition_offset[(sizeof(void *) != 4 || (offsetof(s_controllers, transition) == 0x5b4)) ? 1 : -1];

#endif
