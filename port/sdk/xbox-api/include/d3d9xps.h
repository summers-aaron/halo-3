// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3d9xps.h: XPS (CPU-generated geometry) callback types.
#ifndef _D3D9D3DXPS_H_
#define _D3D9D3DXPS_H_

// The per-thread context D3D hands an XPS routine, and the routine's signature.
typedef struct D3DXpsThread D3DXpsThread;
typedef void (*D3DXpsCallback)(D3DXpsThread* pThreadContext, void* pCallbackContext, CONST void* pSubmitData, DWORD InstanceIndex);

#endif // _D3D9D3DXPS_H_
