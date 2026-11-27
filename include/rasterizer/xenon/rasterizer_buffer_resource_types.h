#ifndef RASTERIZER_XENON_RASTERIZER_BUFFER_RESOURCE_TYPES_H
#define RASTERIZER_XENON_RASTERIZER_BUFFER_RESOURCE_TYPES_H

#include <stddef.h>

#include "xbox/d3d_resource_types.h"

// Xbox 360 XDK d3d9.h: buffer headers.
struct D3DVertexBuffer : D3DResource
{
    // GPUVERTEX_FETCH_CONSTANT, including its low two type/endian bits.
    unsigned long Format[2];
};

struct D3DIndexBuffer : D3DResource
{
    unsigned long Address;
    unsigned long Size;
};

typedef char d3d_vertex_buffer_size[(sizeof(D3DVertexBuffer) == 0x20) ? 1 : -1];
typedef char d3d_index_buffer_size[(sizeof(D3DIndexBuffer) == 0x20) ? 1 : -1];
typedef char d3d_index_buffer_address_offset[(offsetof(D3DIndexBuffer, Address) == 0x18) ? 1 : -1];

#endif
