#ifndef XBOX_D3D_RESOURCE_TYPES_H
#define XBOX_D3D_RESOURCE_TYPES_H

#include <stddef.h>

// Public XDK d3d9.h resource header, without the SDK method declarations.
struct D3DResource
{
    unsigned long Common;
    unsigned long ReferenceCount;
    unsigned long Fence;
    unsigned long ReadFence;
    unsigned long Identifier;
    unsigned long BaseFlush;
};
typedef char d3d_resource_size[(sizeof(D3DResource) == 0x18) ? 1 : -1];
typedef char d3d_resource_identifier_offset[(offsetof(D3DResource, Identifier) == 0x10) ? 1 : -1];

#endif // XBOX_D3D_RESOURCE_TYPES_H
