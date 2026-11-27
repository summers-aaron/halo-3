#ifndef BINK_BINK_TEXTURE_API_H
#define BINK_BINK_TEXTURE_API_H

#include <xtl.h>
#include "rasterizer/xenon_d3d_library.h"

// XDK xgraphics.h declarations without its header-defined format lookup table.
extern "C" {
UINT WINAPI XGSetTextureHeader(UINT width, UINT height, UINT levels, DWORD usage, D3DFORMAT format,
    D3DPOOL pool, UINT base_offset, UINT mip_offset, UINT pitch, D3DTexture *texture, UINT *base_size, UINT *mip_size);
VOID WINAPI XGOffsetResourceAddress(D3DResource *resource, VOID *base_address);
}

#endif // BINK_BINK_TEXTURE_API_H
