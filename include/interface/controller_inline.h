#ifndef INTERFACE_CONTROLLER_INLINE_H
#define INTERFACE_CONTROLLER_INLINE_H

#include "interface/controller_types.h"
#include "cseries/rotate_intrinsics.h"

// Returns the controller interface for the given index without a bounds check.
__forceinline c_controller_interface *controller_get(e_controller_index controller_index)
{
    return &g_controller_globals.controllers[controller_index];
}

static __forceinline e_controller_index first_controller()
{
    return _controller0;
}

static __forceinline e_controller_index next_controller(e_controller_index index)
{
    e_controller_index result = _controller_none;
    if (index >= 0 && index < 3)
        result = (e_controller_index)(index + 1);
    return result;
}

static __forceinline bool controller_in_use(const c_controller_interface *controller)
{
    return controller->is_signed_in_to_machine() || (_rotl(controller->flags, 30) & 1);
}

#endif // INTERFACE_CONTROLLER_INLINE_H
