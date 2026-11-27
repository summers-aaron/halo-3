// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3dx9.h: D3DX utility library umbrella: common macros, error codes, and the D3DX sub-headers.
#ifndef __D3DX9_H__
#define __D3DX9_H__

#include <limits.h>
#include "XObjBase.h"

#define D3DX_DEFAULT                                ((UINT) -1)

#ifndef D3DXINLINE
#define D3DXINLINE __forceinline
#endif

#include "d3d9.h"
#include "d3dx9math.h"
#include "d3dx9core.h"
#include "d3dx9mesh.h"
#include "d3dx9tex.h"
#include "d3dx9shader.h"
#include "d3dx9effect.h"
#include "d3dx9shape.h"
#include "d3dx9anim.h"
#include "d3dx9xof.h"

// D3DX failure codes share Direct3D's facility.
#define _FACDD                                      0x876
#define MAKE_DDHRESULT(code)                        MAKE_HRESULT(1, _FACDD, code)

enum _D3DXERR {
    D3DXERR_INVALIDDATA = MAKE_DDHRESULT(2905),
};

#endif // __D3DX9_H__
