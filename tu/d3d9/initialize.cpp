// d3d9/initialize.obj: from the records 0x147088
// @flags /TP /O2 /Oc
#include <ppcintrinsics.h>
#include <xtl.h>

#include "rasterizer/d3d_device_private.h"

// @address 0x147088 d3d9_set_device_flag
extern "C" void d3d9_set_device_flag(initialize_device *device)
{
    device->flags = device->flags | 2;
}
