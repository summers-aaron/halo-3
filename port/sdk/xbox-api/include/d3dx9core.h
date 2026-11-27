// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3dx9core.h: the D3DX memory buffer interface (ID3DXBuffer).
#include "d3dx9.h"

#ifndef __D3DX9CORE_H__
#define __D3DX9CORE_H__

typedef interface ID3DXBuffer ID3DXBuffer;
typedef interface ID3DXBuffer *LPD3DXBUFFER;

// A reference-counted block of memory D3DX hands back (vtable: IUnknown, then pointer and size).
#undef INTERFACE
#define INTERFACE ID3DXBuffer
DECLARE_INTERFACE_(ID3DXBuffer, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
    STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
};

#endif // __D3DX9CORE_H__
