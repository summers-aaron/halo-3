#ifndef RASTERIZER_D3D_DEVICE_PRIVATE_H
#define RASTERIZER_D3D_DEVICE_PRIVATE_H

#include <xtl.h>
#include <stddef.h>

// Private device state follows the public SDK device.
struct initialize_device
{
    D3DDevice public_state;
    unsigned char private_state[0x3f];
    unsigned char flags;
};

typedef char d3d_device_flags_offset[(sizeof(void *) != 4 || ((offsetof(initialize_device, flags) == 0x2abf))) ? 1 : -1];

#endif
