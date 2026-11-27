#ifndef INTERFACE_USER_INTERFACE_NETWORKING_ALLOCATION_H
#define INTERFACE_USER_INTERFACE_NETWORKING_ALLOCATION_H
#include <stddef.h>

// Force inline even at the networking dialog sites' inline_depth(0).
struct s_user_interface_networking_placement {};
__forceinline void *operator new(size_t, void *memory, s_user_interface_networking_placement)
{
    return memory;
}

#endif
