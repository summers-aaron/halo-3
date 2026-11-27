// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3d9.h: Xbox 360 Direct3D: resource and device structures, the D3DDevice_* API and the inline state setters.
#ifndef _D3D9_H_
#define _D3D9_H_

#define DIRECT3D_VERSION 0x0900
#define D3D_SDK_VERSION 0

// Most Xbox 360 D3D calls cannot fail; with D3DCOMPILE_USEVOIDS defined the C++ methods return void instead of S_OK.
#ifdef D3DCOMPILE_USEVOIDS
#define D3DVOID void
#define D3DVOIDRETURN return
#else
#define D3DVOID HRESULT
#define D3DVOIDRETURN return S_OK
#endif

#define D3DINLINE static __forceinline
#define D3DMINLINE __forceinline

#ifndef DECLSPEC_SELECTANY
#define DECLSPEC_SELECTANY __declspec(selectany)
#endif

#if !defined(_NORESTRICT_) && !defined(_PREFAST_)
#define D3DRESTRICT __restrict
#else
#define D3DRESTRICT
#endif

// The Xbox compiler folds __IsIntConst(x) to 1 for a compile-time constant; the inline setters below keep a constant
// fast path and a table-driven path behind it.
#define D3DISINTCONST(x) __IsIntConst((int) x)

#include <stdlib.h>
#include <PPCIntrinsics.h>

// ---- Object types --------------------------------------------------------------------------------------------

typedef struct Direct3D Direct3D;
typedef struct D3DDevice D3DDevice;
typedef struct D3DVertexDeclaration D3DVertexDeclaration;
typedef struct D3DVertexShader D3DVertexShader;
typedef struct D3DPixelShader D3DPixelShader;
typedef struct D3DResource D3DResource;
typedef struct D3DBaseTexture D3DBaseTexture;
typedef struct D3DTexture D3DTexture;
typedef struct D3DVolumeTexture D3DVolumeTexture;
typedef struct D3DCubeTexture D3DCubeTexture;
typedef struct D3DArrayTexture D3DArrayTexture;
typedef struct D3DLineTexture D3DLineTexture;
typedef struct D3DVertexBuffer D3DVertexBuffer;
typedef struct D3DIndexBuffer D3DIndexBuffer;
typedef struct D3DSurface D3DSurface;
typedef struct D3DVolume D3DVolume;
typedef struct D3DQuery D3DQuery;
typedef struct D3DConstantBuffer D3DConstantBuffer;
typedef struct D3DCommandBuffer D3DCommandBuffer;

// PC Direct3D 9 interface names.
#define IDirect3D9 Direct3D
#define IDirect3DDevice9 D3DDevice
#define IDirect3DVertexDeclaration9 D3DVertexDeclaration
#define IDirect3DVertexShader9 D3DVertexShader
#define IDirect3DPixelShader9 D3DPixelShader
#define IDirect3DResource9 D3DResource
#define IDirect3DBaseTexture9 D3DBaseTexture
#define IDirect3DTexture9 D3DTexture
#define IDirect3DVolumeTexture9 D3DVolumeTexture
#define IDirect3DCubeTexture9 D3DCubeTexture
#define IDirect3DArrayTexture9 D3DArrayTexture
#define IDirect3DLineTexture9 D3DLineTexture
#define IDirect3DVertexBuffer9 D3DVertexBuffer
#define IDirect3DIndexBuffer9 D3DIndexBuffer
#define IDirect3DSurface9 D3DSurface
#define IDirect3DVolume9 D3DVolume
#define IDirect3DQuery9 D3DQuery
#define IDirect3DConstantBuffer9 D3DConstantBuffer
#define IDirect3DCommandBuffer9 D3DCommandBuffer

typedef struct Direct3D *LPDIRECT3D9, *PDIRECT3D9;
typedef struct D3DDevice *LPDIRECT3DDEVICE9, *PDIRECT3DDEVICE9;
typedef struct D3DVertexDeclaration *LPDIRECT3DVERTEXDECLARATION9, *PDIRECT3DVERTEXDECLARATION9;
typedef struct D3DVertexShader *LPDIRECT3DVERTEXSHADER9, *PDIRECT3DVERTEXSHADER9;
typedef struct D3DPixelShader *LPDIRECT3DPIXELSHADER9, *PDIRECT3DPIXELSHADER9;
typedef struct D3DResource *LPDIRECT3DRESOURCE9, *PDIRECT3DRESOURCE9;
typedef struct D3DBaseTexture *LPDIRECT3DBASETEXTURE9, *PDIRECT3DBASETEXTURE9;
typedef struct D3DTexture *LPDIRECT3DTEXTURE9, *PDIRECT3DTEXTURE9;
typedef struct D3DVolumeTexture *LPDIRECT3DVOLUMETEXTURE9, *PDIRECT3DVOLUMETEXTURE9;
typedef struct D3DCubeTexture *LPDIRECT3DCUBETEXTURE9, *PDIRECT3DCUBETEXTURE9;
typedef struct D3DArrayTexture *LPDIRECT3DARRAYTEXTURE9, *PDIRECT3DARRAYTEXTURE9;
typedef struct D3DLineTexture *LPDIRECT3DLINETEXTURE9, *PDIRECT3DLINETEXTURE9;
typedef struct D3DVertexBuffer *LPDIRECT3DVERTEXBUFFER9, *PDIRECT3DVERTEXBUFFER9;
typedef struct D3DIndexBuffer *LPDIRECT3DINDEXBUFFER9, *PDIRECT3DINDEXBUFFER9;
typedef struct D3DSurface *LPDIRECT3DSURFACE9, *PDIRECT3DSURFACE9;
typedef struct D3DVolume *LPDIRECT3DVOLUME9, *PDIRECT3DVOLUME9;
typedef struct D3DQuery *LPDIRECT3DQUERY9, *PDIRECT3DQUERY9;
typedef struct D3DConstantBuffer *LPDIRECT3DCONSTANTBUFFER9, *PDIRECT3DCONSTANTBUFFER9;
typedef struct D3DCommandBuffer *LPDIRECT3DCOMMANDBUFFER9, *PDIRECT3DCOMMANDBUFFER9;

typedef struct _D3DSURFACES {
    D3DSurface* pDepthStencilSurface;
    D3DSurface* pRenderTarget[4];
} D3DSURFACES;

#include "d3d9types.h"
#include "d3d9caps.h"
#include "d3d9xps.h"

#ifdef __cplusplus
extern "C" {
#endif

// The Direct3D object is a singleton without state; any non-null pointer stands for it.
D3DINLINE Direct3D* WINAPI Direct3DCreate9(UINT SDKVersion) { return (Direct3D*) 1; }

// Direct3D::CreateDevice behavior flags.
#define D3DCREATE_NO_SHADER_PATCHING                0x00000008L

// ---- Errors --------------------------------------------------------------------------------------------------

#define _FACD3D                                     0x876
#define MAKE_D3DHRESULT(code)                       MAKE_HRESULT(1, _FACD3D, code)
#define MAKE_D3DSTATUS(code)                        MAKE_HRESULT(0, _FACD3D, code)

#define D3D_OK                                      S_OK
#define D3DERR_NOTAVAILABLE                         MAKE_D3DHRESULT(2154)
#define D3DERR_INVALIDCALL                          MAKE_D3DHRESULT(2156)

// ---- Primitive / vertex counts -------------------------------------------------------------------------------

// Vertices of N primitives = N * [type][0] + [type][1].  One COMDAT copy per image (selectany).
extern CONST __declspec(selectany) UINT D3DPRIMITIVEVERTEXCOUNT[][2] =
{
    {0, 0}, {1, 0}, {2, 0}, {1, 1}, {3, 0}, {1, 2}, {1, 2}, {0, 0}, {3, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {4, 0},
};

#define D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount) \
    ((PrimitiveCount) * D3DPRIMITIVEVERTEXCOUNT[PrimitiveType][0] + D3DPRIMITIVEVERTEXCOUNT[PrimitiveType][1])
#define D3DPRIMCOUNT(PrimitiveType, VertexCount) \
    (((VertexCount) - D3DPRIMITIVEVERTEXCOUNT[PrimitiveType][1]) / D3DPRIMITIVEVERTEXCOUNT[PrimitiveType][0])

// The same table for tessellated primitive types (D3DTESSPRIMITIVETYPE).
extern CONST __declspec(selectany) UINT D3DTESSPRIMITIVEVERTEXCOUNT[][2] =
{
    {0, 0}, {0, 0}, {2, 0}, {1, 1}, {3, 0}, {1, 2}, {1, 2}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {4, 0},
    {0, 0}, {0, 0}, {1, 0}, {1, 0}, {1, 0},
};

// ---- The device's constant shadow ----------------------------------------------------------------------------

typedef struct _D3DConstants
{
    union {
        GPUFETCH_CONSTANT Fetch[GPU_FETCH_CONSTANTS];
        struct {
            GPUTEXTURE_FETCH_CONSTANT TextureFetch[GPU_D3D_TEXTURE_FETCH_CONSTANT_COUNT];
            GPUVERTEX_FETCH_CONSTANT VertexFetch[3*GPU_D3D_VERTEX_FETCH_CONSTANT_COUNT];
        };
    };
    union {
        D3DVECTOR4 Alu[GPU_ALU_CONSTANTS];
        struct {
            D3DVECTOR4 VertexShaderF[GPU_D3D_VERTEX_CONSTANTF_COUNT];
            D3DVECTOR4 PixelShaderF[GPU_D3D_PIXEL_CONSTANTF_COUNT];
        };
    };
    union {
        DWORD Flow[GPU_FLOW_CONSTANTS];
        struct {
            DWORD VertexShaderB[GPU_D3D_VERTEX_CONSTANTB_COUNT/32];
            DWORD PixelShaderB[GPU_D3D_PIXEL_CONSTANTB_COUNT/32];
            DWORD VertexShaderI[GPU_D3D_VERTEX_CONSTANTI_COUNT];
            DWORD PixelShaderI[GPU_D3D_PIXEL_CONSTANTI_COUNT];
        };
    };
} D3DConstants;

// Entries of the device's per-state call tables.
typedef void (WINAPI *D3DSETRENDERSTATECALL)(D3DDevice* pDevice, DWORD Value);
typedef void (WINAPI *D3DSETSAMPLERSTATECALL)(D3DDevice* pDevice, DWORD Sampler, DWORD Value);
typedef DWORD (WINAPI *D3DGETRENDERSTATECALL)(D3DDevice* pDevice);
typedef DWORD (WINAPI *D3DGETSAMPLERSTATECALL)(D3DDevice* pDevice, DWORD Sampler);

// ---- Register tags -------------------------------------------------------------------------------------------

D3DINLINE DWORD D3DTag_Index(D3DTAG Tag)
{
    return D3DTAG_INDEX(Tag);
}

// Bits StartIndex..EndIndex (inclusive) of the tag's group.
D3DINLINE UINT64 D3DTag_SubsetMask(D3DTAG Tag, DWORD StartIndex, DWORD EndIndex)
{
    DWORD first = D3DTAG_START(Tag);
    return D3DTAG_MASKENCODE(first + StartIndex, first + EndIndex);
}

// One pending bit covers four float constants (vertex and pixel constant groups share the bit layout).
D3DINLINE UINT64 D3DTag_ShaderConstantMask(DWORD StartConstant, DWORD ConstantCount)
{
    return D3DTag_SubsetMask(D3DTAG_VERTEXSHADERCONSTANTS, StartConstant / 4, (StartConstant + ConstantCount - 1) / 4);
}

D3DINLINE UINT64 D3DTag_SamplerMask(DWORD Sampler)
{
    return D3DTag_SubsetMask(D3DTAG_TEXTUREFETCHCONSTANTS, Sampler, Sampler);
}

// One bit per GPU context register (group): 5 x 64 bits.
typedef struct _D3DTAGCOLLECTION
{
    UINT64 m_Mask[5];
} D3DTAGCOLLECTION;

D3DINLINE void D3DTagCollection_Set(D3DTAGCOLLECTION* pCollection, DWORD Index, UINT64 Mask)
{
    pCollection->m_Mask[Index] |= Mask;
}

D3DINLINE void D3DTagCollection_SetAll(D3DTAGCOLLECTION* pCollection)
{
    pCollection->m_Mask[0] = ~(UINT64) 0;
    pCollection->m_Mask[1] = ~(UINT64) 0;
    pCollection->m_Mask[2] = ~(UINT64) 0;
    pCollection->m_Mask[3] = ~(UINT64) 0;
    pCollection->m_Mask[4] = ~(UINT64) 0;
}

D3DINLINE void D3DTagCollection_Clear(D3DTAGCOLLECTION* pCollection, DWORD Index, UINT64 Mask)
{
    pCollection->m_Mask[Index] &= ~Mask;
}

D3DINLINE void D3DTagCollection_SetPixelShaderConstantFTag(D3DTAGCOLLECTION* pCollection, DWORD StartConstant, DWORD ConstantCount)
{
    D3DTagCollection_Set(pCollection, D3DTag_Index(D3DTAG_PIXELSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartConstant, ConstantCount));
}

// ---- Callbacks -----------------------------------------------------------------------------------------------

typedef void (*D3DVBLANKCALLBACK)(D3DVBLANKDATA *pData);

// ---- Queries -------------------------------------------------------------------------------------------------

#ifdef __cplusplus
struct D3DQuery
{
    ULONG   WINAPI Release();
    D3DVOID WINAPI Issue(DWORD IssueFlags);
    HRESULT WINAPI GetData(void* pData, DWORD Size, DWORD GetDataFlags);
};
#endif

// ---- Resources -----------------------------------------------------------------------------------------------

// D3DResource::Common: the D3DRTYPE in the low nibble, lock id and count above it, flags in the upper bits.
#define D3DCOMMON_TYPE_MASK                         0x0000000F
#define D3DCOMMON_TYPE_SHIFT                        0
#define D3DCOMMON_TYPE_VERTEXBUFFER                 (D3DRTYPE_VERTEXBUFFER << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_TYPE_INDEXBUFFER                  (D3DRTYPE_INDEXBUFFER << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_TYPE_TEXTURE                      (D3DRTYPE_TEXTURE << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_TYPE_SURFACE                      (D3DRTYPE_SURFACE << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_TYPE_VERTEXDECLARATION            (D3DRTYPE_VERTEXDECLARATION << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_TYPE_VERTEXSHADER                 (D3DRTYPE_VERTEXSHADER << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_TYPE_PIXELSHADER                  (D3DRTYPE_PIXELSHADER << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_TYPE_CONSTANTBUFFER               (D3DRTYPE_CONSTANTBUFFER << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_TYPE_COMMANDBUFFER                (D3DRTYPE_COMMANDBUFFER << D3DCOMMON_TYPE_SHIFT)
#define D3DCOMMON_D3DCREATED                        0x00100000
#define D3DCOMMON_CPU_CACHED_MEMORY                 0x00200000

// Initial BaseFlush / MipFlush: nothing to flush.
#define D3DFLUSH_INITIAL_VALUE                      0xffff0000

// Fields every resource starts with (C has no inheritance; C++ derives from D3DResource).
#define _D3D9_RESOURCE_FIELDS \
    DWORD Common;           /* type and flags */ \
    DWORD ReferenceCount;   /* external references */ \
    DWORD Fence;            /* last ring-buffer fence that used the resource */ \
    DWORD ReadFence;        /* fence after which the CPU may read GPU writes */ \
    DWORD Identifier;       /* the title's tag */ \
    DWORD BaseFlush;        /* cache range flushed at Unlock */

#define _D3D9_BASETEXTURE_FIELDS \
    DWORD MipFlush; \
    GPUTEXTURE_FETCH_CONSTANT Format;

struct D3DResource
{
    _D3D9_RESOURCE_FIELDS
};

#ifdef __cplusplus
#define _D3D9_DERIVED(Base) : public Base
#define _D3D9_INHERITED(Fields)
#else
#define _D3D9_DERIVED(Base)
#define _D3D9_INHERITED(Fields) Fields
#endif

struct D3DBaseTexture _D3D9_DERIVED(D3DResource)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
    _D3D9_BASETEXTURE_FIELDS
};

struct D3DTexture _D3D9_DERIVED(D3DBaseTexture)
{
#ifdef __cplusplus
    D3DVOID WINAPI LockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
    D3DVOID WINAPI UnlockRect(UINT Level);
#endif
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS _D3D9_BASETEXTURE_FIELDS)
};

struct D3DVolumeTexture _D3D9_DERIVED(D3DBaseTexture)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS _D3D9_BASETEXTURE_FIELDS)
};

struct D3DCubeTexture _D3D9_DERIVED(D3DBaseTexture)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS _D3D9_BASETEXTURE_FIELDS)
};

struct D3DArrayTexture _D3D9_DERIVED(D3DBaseTexture)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS _D3D9_BASETEXTURE_FIELDS)
};

struct D3DLineTexture _D3D9_DERIVED(D3DBaseTexture)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS _D3D9_BASETEXTURE_FIELDS)
};

struct D3DVertexBuffer _D3D9_DERIVED(D3DResource)
{
#ifdef __cplusplus
    D3DVOID WINAPI Unlock();
#endif
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
    GPUVERTEX_FETCH_CONSTANT Format;
};

// D3DIndexBuffer::Common: 32-bit indices, and the GPUENDIAN the GPU reads them with.
#define D3DINDEXBUFFER_INDEX32                      0x80000000
#define D3DINDEXBUFFER_ENDIAN_SHIFT                 29

struct D3DIndexBuffer _D3D9_DERIVED(D3DResource)
{
#ifdef __cplusplus
    D3DVOID WINAPI Unlock();
#endif
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
    DWORD Address;
    DWORD Size;
};

// D3DSurface::Common: the surface aliases a texture level (Parent / MipLevel / ArrayIndex are valid).
#define D3DCOMMON_SURFACE_FROM_TEXTURE              0x40000000

struct D3DSurface _D3D9_DERIVED(D3DResource)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
    union {
        struct {                            // an EDRAM surface
            GPU_SURFACEINFO SurfaceInfo;
            union {
                GPU_DEPTHINFO DepthInfo;
                GPU_COLORINFO ColorInfo;
            };
            GPU_HICONTROL HiControl;
            struct {
                DWORD Width : 14, Height : 15, : 3;
            };
            D3DFORMAT Format;
            DWORD Size;
        };
        struct {                            // a view of a texture level
            D3DBaseTexture *Parent;
            struct {
                DWORD MipLevel : 4, ArrayIndex : 6, : 22;
            };
        };
    };
};

struct D3DVolume _D3D9_DERIVED(D3DResource)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
    D3DBaseTexture *Parent;
    struct {
        DWORD MipLevel : 4, ArrayIndex : 6, : 22;
    };
};

struct D3DVertexDeclaration _D3D9_DERIVED(D3DResource)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
};

struct D3DVertexShader _D3D9_DERIVED(D3DResource)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
};

struct D3DPixelShader _D3D9_DERIVED(D3DResource)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
};

struct D3DConstantBuffer _D3D9_DERIVED(D3DResource)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
    DWORD Address;
    DWORD Size;
};

struct D3DCommandBuffer _D3D9_DERIVED(D3DResource)
{
    _D3D9_INHERITED(_D3D9_RESOURCE_FIELDS)
    D3DTAGCOLLECTION m_Inherited;           // registers taken from the device when the buffer runs
    D3DTAGCOLLECTION m_Persisted;           // registers left set after it ran
};

// ---- The device ----------------------------------------------------------------------------------------------

struct D3DDevice
{
#ifdef __cplusplus
    D3DVOID WINAPI GetDeviceCaps(D3DCAPS9 *pCaps);
    D3DVOID WINAPI GetDisplayMode(UINT UnusedSwapChain, D3DDISPLAYMODE *pMode);
    HRESULT WINAPI Reset(D3DPRESENT_PARAMETERS *pPresentationParameters);
    D3DVOID WINAPI Present(CONST RECT *pUnusedSourceRect, CONST RECT *pUnusedDestRect, void *hUnusedDestWindowOverride, void *pUnusedDirtyRegion);
    HRESULT WINAPI CreateVertexBuffer(UINT Length, DWORD Usage, DWORD UnusedFVF, D3DPOOL UnusedPool, D3DVertexBuffer **ppVertexBuffer, HANDLE* pUnusedSharedHandle);
    HRESULT WINAPI CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DIndexBuffer **ppIndexBuffer, HANDLE* pUnusedSharedHandle);
    HRESULT WINAPI CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedLockable, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters);
    HRESULT WINAPI CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedDiscard, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters);
    D3DVOID WINAPI SetRenderTarget(DWORD RenderTargetIndex, D3DSurface *pRenderTarget);
    D3DVOID WINAPI SetDepthStencilSurface(D3DSurface *pZStencilSurface);
    D3DVOID WINAPI Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
    D3DVOID WINAPI ClearF(DWORD Flags, CONST D3DRECT *pRect, CONST D3DVECTOR4* pColor, float Z, DWORD Stencil);
    D3DVOID WINAPI SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
    D3DVOID WINAPI GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue);
    D3DVOID WINAPI SetTexture(DWORD Sampler, D3DBaseTexture *pTexture);
    D3DVOID WINAPI SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
    D3DVOID WINAPI DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    D3DVOID WINAPI DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT UnusedMinIndex, UINT UnusedNumIndices, UINT StartIndex, UINT PrimitiveCount);
    D3DVOID WINAPI DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
    HRESULT WINAPI CreateVertexShader(CONST DWORD *pFunction, D3DVertexShader** ppShader);
    D3DVOID WINAPI SetVertexShader(D3DVertexShader *pShader);
    D3DVOID WINAPI SetVertexShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
    D3DVOID WINAPI SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
    D3DVOID WINAPI SetVertexShaderConstantI(UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount);
    D3DVOID WINAPI SetStreamSource(UINT StreamNumber, D3DVertexBuffer *pStreamData, UINT OffsetInBytes, UINT Stride);
    D3DVOID WINAPI SetIndices(D3DIndexBuffer *pIndexData);
    HRESULT WINAPI CreatePixelShader(CONST DWORD *pFunction, D3DPixelShader** ppShader);
    D3DVOID WINAPI SetPixelShader(D3DPixelShader* pShader);
    D3DVOID WINAPI SetPixelShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount);
    D3DVOID WINAPI SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount);
    D3DVOID WINAPI SetPixelShaderConstantI(UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount);
    D3DVOID WINAPI SetVertexDeclaration(D3DVertexDeclaration *pDecl);
    D3DVOID WINAPI SetScissorRect(CONST RECT* pRect);
    D3DVOID WINAPI SetClipPlane(DWORD Index, CONST float* pPlane);
    HRESULT WINAPI CreateQueryTiled(D3DQUERYTYPE Type, DWORD TileCount, D3DQuery** ppQuery);
    D3DVOID WINAPI Resolve(DWORD Flags, CONST D3DRECT *pSourceRect, D3DBaseTexture *pDestTexture, CONST D3DPOINT *pDestPoint, UINT DestLevel, UINT DestSliceOrFace, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters);
    D3DVOID WINAPI AcquireThreadOwnership();
    D3DVOID WINAPI ReleaseThreadOwnership();
    D3DVOID WINAPI BlockUntilIdle();
    D3DVOID WINAPI SynchronizeToPresentationInterval();
    D3DVOID WINAPI Swap(D3DBaseTexture* pFrontBuffer, CONST D3DVIDEO_SCALER_PARAMETERS* pParameters);
    D3DVOID WINAPI SetPredication(DWORD PredicationMask);
    D3DVOID WINAPI BeginTiling(DWORD Flags, DWORD Count, CONST D3DRECT* pTileRects, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil);
    HRESULT WINAPI EndTiling(DWORD ResolveFlags, CONST D3DRECT* pResolveRects, D3DBaseTexture* pDestTexture, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters);
    D3DVOID WINAPI BeginExport(DWORD Index, D3DResource* pResource, DWORD Flags);
    D3DVOID WINAPI EndExport(DWORD Index, D3DResource* pResource, DWORD Flags);
    DWORD   WINAPI InsertFence();
    D3DVOID WINAPI SetBlendState(DWORD RenderTargetIndex, D3DBLENDSTATE BlendState);
    D3DVOID WINAPI BeginConditionalRendering(DWORD Identifier);
    D3DVOID WINAPI EndConditionalRendering();
    D3DVOID WINAPI SetShaderGPRAllocation(DWORD Flags, DWORD VertexShaderCount, DWORD PixelShaderCount);
    D3DVOID WINAPI GetShaderGPRAllocation(DWORD* pFlags, DWORD* pVertexShaderCount, DWORD* pPixelShaderCount);
    D3DVOID WINAPI FlushHiZStencil(D3DFHZS_FLUSHTYPE FlushType);
    D3DVOID WINAPI UnsetAll();

    // Unbuffered ("Gpu") APIs: the title writes these registers straight to the GPU, D3D leaves them alone.
    void    WINAPI GpuOwnVertexShaderConstantF(DWORD StartRegister, DWORD Vector4fCount);
    void    WINAPI GpuOwnPixelShaderConstantF(DWORD StartRegister, DWORD Vector4fCount);
    void    WINAPI GpuDisownVertexShaderConstantF(DWORD StartRegister, DWORD Vector4fCount);
    void    WINAPI GpuDisownPixelShaderConstantF(DWORD StartRegister, DWORD Vector4fCount);
    void    WINAPI GpuLoadPixelShaderConstantF4Pointer(UINT StartRegister, CONST void* pConstantData, DWORD Vector4fCount);
    void    WINAPI GpuLoadVertexShaderConstantF4Pointer(UINT StartRegister, CONST void* pConstantData, DWORD Vector4fCount);
    HRESULT WINAPI GpuBeginVertexShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount);
    HRESULT WINAPI GpuBeginPixelShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount);
    void    WINAPI GpuEndPixelShaderConstantF4();
#endif

    // Register groups to send at the next draw (only groups the title does not own through the Gpu APIs).
    D3DTAGCOLLECTION m_Pending;

    // m_Pending.m_Mask[2] bits that must be written predicated (automatic Z pass).
    UINT64 m_Predicated_PendingMask2;

    // The ring / command buffer write pointer sits one dword before the next write (pre-increment stores);
    // below m_pRingGuarantee at least 32 dwords are free before m_pRingLimit.
    PRING m_pRing;
    PRING m_pRingLimit;
    PRING m_pRingGuarantee;

    DWORD m_ReferenceCount;

    // Per-state entry points, indexed by D3DRS_* / D3DSAMP_* (which are byte offsets into these tables).
    D3DSETRENDERSTATECALL m_SetRenderStateCall[D3DRS_MAX/4];
    D3DSETSAMPLERSTATECALL m_SetSamplerStateCall[D3DSAMP_MAX/4];
    D3DGETRENDERSTATECALL m_GetRenderStateCall[D3DRS_MAX/4];
    D3DGETSAMPLERSTATECALL m_GetSamplerStateCall[D3DSAMP_MAX/4];

    // Shadow of the GPU constant files; the ALU constants start on a 128-byte boundary.
    __declspec(align(128)) D3DConstants m_Constants;

    // Shadow of the context registers (set the m_Pending bit after writing one directly).
    float m_ClipPlanes[D3DMAXUSERCLIPPLANES][4];
    GPU_DESTINATIONPACKET m_DestinationPacket;
    GPU_WINDOWPACKET m_WindowPacket;            // written directly, never through m_Pending
    GPU_VALUESPACKET m_ValuesPacket;
    GPU_PROGRAMPACKET m_ProgramPacket;
    GPU_CONTROLPACKET m_ControlPacket;
    GPU_TESSELLATORPACKET m_TessellatorPacket;
    GPU_MISCPACKET m_MiscPacket;
    GPU_POINTPACKET m_PointPacket;
};

// ---- Direct3D ------------------------------------------------------------------------------------------------

HRESULT WINAPI Direct3D_CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
HRESULT WINAPI Direct3D_CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
HRESULT WINAPI Direct3D_CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);
HRESULT WINAPI Direct3D_CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, void* hUnusedFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDevice** ppReturnedDeviceInterface);

// ---- D3DDevice_* ---------------------------------------------------------------------------------------------

D3DVertexShader* WINAPI D3DDevice_CreateVertexShader(CONST DWORD* pFunction);
D3DPixelShader* WINAPI D3DDevice_CreatePixelShader(CONST DWORD* pFunction);
D3DBaseTexture* WINAPI D3DDevice_CreateTexture(DWORD Width, DWORD Height, DWORD Depth, DWORD Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DRESOURCETYPE D3DType);
D3DSurface* WINAPI D3DDevice_CreateSurface(DWORD Width, DWORD Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, CONST D3DSURFACE_PARAMETERS* pParameters);
D3DVertexBuffer* WINAPI D3DDevice_CreateVertexBuffer(UINT Length, DWORD Usage, D3DPOOL UnusedPool);
D3DIndexBuffer* WINAPI D3DDevice_CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool);
D3DVertexDeclaration* WINAPI D3DDevice_CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements);
D3DConstantBuffer* WINAPI D3DDevice_CreateConstantBuffer(UINT VectorCount, DWORD Usage);
D3DCommandBuffer* WINAPI D3DDevice_CreateCommandBuffer(UINT Size, DWORD Flags);
D3DQuery* WINAPI D3DDevice_CreateQuery(D3DDevice* pDevice, D3DQUERYTYPE Type);
D3DQuery* WINAPI D3DDevice_CreateQueryTiled(D3DDevice* pDevice, D3DQUERYTYPE Type, DWORD TileCount);
D3DSurface* WINAPI D3DDevice_GetRenderTarget(D3DDevice* pDevice, DWORD RenderTargetIndex);
D3DSurface* WINAPI D3DDevice_GetDepthStencilSurface(D3DDevice* pDevice);
D3DSurface* WINAPI D3DDevice_GetBackBuffer(D3DDevice* pDevice);
ULONG WINAPI D3DDevice_AddRef(D3DDevice* pDevice);
ULONG WINAPI D3DDevice_Release(D3DDevice* pDevice);
void WINAPI D3DDevice_GetDeviceCaps(D3DDevice* pDevice, D3DCAPS9* pCaps);
void WINAPI D3DDevice_GetDisplayMode(D3DDevice* pDevice, UINT UnusedSwapChain, D3DDISPLAYMODE* pMode);
HRESULT WINAPI D3DDevice_Reset(D3DDevice* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
void WINAPI D3DDevice_SetGammaRamp(D3DDevice* pDevice, DWORD UnusedFlags, CONST D3DGAMMARAMP* pRamp);
void WINAPI D3DDevice_GetGammaRamp(D3DDevice* pDevice, D3DGAMMARAMP* pRamp);
void WINAPI D3DDevice_SetPWLGamma(D3DDevice* pDevice, DWORD Flags, CONST D3DPWLGAMMA* pRamp);
void WINAPI D3DDevice_GetPWLGamma(D3DDevice* pDevice, D3DPWLGAMMA* pRamp);
void WINAPI D3DDevice_SetRenderTarget_External(D3DDevice* pDevice, DWORD RenderTargetIndex, D3DSurface* pRenderTarget);
void WINAPI D3DDevice_SetDepthStencilSurface(D3DDevice* pDevice, D3DSurface* pZStencilSurface);
void WINAPI D3DDevice_Clear(D3DDevice* pDevice, DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil, BOOL EDRAMClear);
void WINAPI D3DDevice_ClearF(D3DDevice* pDevice, DWORD Flags, CONST D3DRECT* pRect, CONST D3DVECTOR4* pColor, float Z, DWORD Stencil);
void WINAPI D3DDevice_SetViewport(D3DDevice* pDevice, CONST D3DVIEWPORT9* pViewport);
void WINAPI D3DDevice_GetViewport(D3DDevice* pDevice, D3DVIEWPORT9* pViewport);
void WINAPI D3DDevice_SetTexture(D3DDevice* pDevice, DWORD Sampler, D3DBaseTexture* pTexture, UINT64 PendingMask3);
void WINAPI D3DDevice_DrawVertices(D3DDevice* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT VertexCount);
void WINAPI D3DDevice_DrawIndexedVertices(D3DDevice* pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT IndexCount);
void WINAPI D3DDevice_DrawVerticesUP(D3DDevice* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
void WINAPI D3DDevice_DrawIndexedVerticesUP(D3DDevice* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT IndexCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
void WINAPI D3DDevice_SetVertexDeclaration(D3DDevice* pDevice, D3DVertexDeclaration* pDecl);
void WINAPI D3DDevice_SetScissorRect(D3DDevice* pDevice, CONST RECT* pRect);
void WINAPI D3DDevice_SetVertexShader(D3DDevice* pDevice, D3DVertexShader* pShader);
void WINAPI D3DDevice_SetVertexShaderConstantB(D3DDevice* pDevice, UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount);
void WINAPI D3DDevice_SetVertexShaderConstantFN(D3DDevice* pDevice, UINT StartRegister, CONST float* pConstantData, DWORD Vector4fCount, UINT64 PendingMask0);
void WINAPI D3DDevice_SetVertexShaderConstantI(D3DDevice* pDevice, UINT StartRegister, CONST int* pConstantData, DWORD Vector4iCount);
void WINAPI D3DDevice_SetStreamSource(D3DDevice* pDevice, UINT StreamNumber, D3DVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride, UINT64 PendingMask3);
void WINAPI D3DDevice_SetIndices(D3DDevice* pDevice, D3DIndexBuffer* pIndexData);
void WINAPI D3DDevice_SetPixelShader(D3DDevice* pDevice, D3DPixelShader* pShader);
void WINAPI D3DDevice_SetPixelShaderConstantB(D3DDevice* pDevice, UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount);
void WINAPI D3DDevice_SetPixelShaderConstantFN(D3DDevice* pDevice, UINT StartRegister, CONST float* pConstantData, DWORD Vector4fCount, UINT64 PendingMask1);
void WINAPI D3DDevice_SetPixelShaderConstantI(D3DDevice* pDevice, UINT StartRegister, CONST int* pConstantData, DWORD Vector4iCount);
void WINAPI D3DDevice_SetClipPlane(D3DDevice* pDevice, DWORD Index, CONST float* pPlane);
void WINAPI D3DDevice_GetClipPlane(D3DDevice* pDevice, DWORD Index, float* pPlane);
void WINAPI D3DDevice_Resolve(D3DDevice* pDevice, DWORD Flags, CONST D3DRECT* pSourceRect, D3DBaseTexture* pDestTexture, CONST D3DPOINT* pDestPoint, UINT DestLevel, UINT DestSliceOrFace, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters);
void WINAPI D3DDevice_AcquireThreadOwnership(D3DDevice* pDevice);
void WINAPI D3DDevice_ReleaseThreadOwnership(D3DDevice* pDevice);
void WINAPI D3DDevice_Present(D3DDevice* pDevice);
void WINAPI D3DDevice_BlockUntilIdle(D3DDevice* pDevice);
void WINAPI D3DDevice_SetVerticalBlankCallback(D3DDevice* pDevice, D3DVBLANKCALLBACK pCallback);
void WINAPI D3DDevice_SynchronizeToPresentationInterval(D3DDevice* pDevice);
void WINAPI D3DDevice_Swap(D3DDevice* pDevice, D3DBaseTexture* pFrontBuffer, CONST D3DVIDEO_SCALER_PARAMETERS* pParameters);
void WINAPI D3DDevice_SetPredication(D3DDevice* pDevice, DWORD PredicationMask);
void WINAPI D3DDevice_BeginTiling(D3DDevice* pDevice, DWORD Flags, DWORD Count, CONST D3DRECT* pTileRects, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil);
HRESULT WINAPI D3DDevice_EndTiling(D3DDevice* pDevice, DWORD ResolveFlags, CONST D3DRECT* pResolveRects, D3DBaseTexture* pDestTexture, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters);
void WINAPI D3DDevice_BeginExport(D3DDevice* pDevice, DWORD Index, D3DResource* pResource, DWORD Flags);
void WINAPI D3DDevice_EndExport(D3DDevice* pDevice, DWORD Index, D3DResource* pResource, DWORD Flags);
void WINAPI D3DDevice_DrawIndexedTessellatedVertices(D3DDevice* pDevice, D3DTESSPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT IndexCount);
DWORD WINAPI D3DDevice_InsertFence(D3DDevice* pDevice);
void WINAPI D3DDevice_BlockOnFence(DWORD Fence);
BOOL WINAPI D3DDevice_IsFencePending(DWORD Fence);
void WINAPI D3DDevice_SetBlendState(D3DDevice* pDevice, DWORD RenderTargetIndex, D3DBLENDSTATE BlendState);
void WINAPI D3DDevice_GetBlendState(D3DDevice* pDevice, DWORD RenderTargetIndex, D3DBLENDSTATE* pBlendState);
void WINAPI D3DDevice_EndConditionalSurvey(D3DDevice* pDevice, DWORD Flags);
void WINAPI D3DDevice_BeginConditionalRendering(D3DDevice* pDevice, DWORD Identifier);
void WINAPI D3DDevice_EndConditionalRendering(D3DDevice* pDevice);
void WINAPI D3DDevice_Suspend(D3DDevice* pDevice);
void WINAPI D3DDevice_SetShaderGPRAllocation(D3DDevice* pDevice, DWORD Flags, DWORD VertexShaderCount, DWORD PixelShaderCount);
void WINAPI D3DDevice_GetShaderGPRAllocation(D3DDevice* pDevice, DWORD* pFlags, DWORD* pVertexShaderCount, DWORD* pPixelShaderCount);
void WINAPI D3DDevice_FlushHiZStencil(D3DDevice* pDevice, D3DFHZS_FLUSHTYPE FlushType);
void WINAPI D3DDevice_UnsetAll(D3DDevice* pDevice);
DWORD WINAPI D3DDevice_GetDeviceState(D3DDevice* pDevice);
void WINAPI D3DDevice_SetSurfaces(D3DDevice* pDevice, CONST D3DSURFACES* pSurfaces, DWORD Flags);
void WINAPI D3DDevice_BeginCommandBuffer(D3DDevice* pDevice, D3DCommandBuffer* pCommandBuffer, DWORD Flags, CONST D3DTAGCOLLECTION* pInheritTags, CONST D3DTAGCOLLECTION* pPersistTags, CONST D3DRECT* pTilingRects, DWORD TileCount);
HRESULT WINAPI D3DDevice_EndCommandBuffer(D3DDevice* pDevice);
void WINAPI D3DDevice_RunCommandBuffer(D3DDevice* pDevice, D3DCommandBuffer* pD3dCommandBuffer, DWORD PredicationSelect);
void WINAPI D3DDevice_SetViewportF(D3DDevice* pDevice, CONST D3DVIEWPORTF9* pViewportF);
D3DVECTOR4* WINAPI D3DDevice_GpuBeginShaderConstantF4(D3DDevice* pDevice, BOOL PixelShader, UINT StartRegister, DWORD Vector4fCount);
void WINAPI D3DDevice_GpuLoadShaderConstantF4Pointer(D3DDevice* pDevice, BOOL PixelShader, UINT StartRegister, CONST void* pConstantData, DWORD Vector4fCount);

// Render states in the order of the device's state tables: X(D3DRS_ suffix, function suffix).
#define _D3D9_RENDER_STATES(X) \
    X(ZENABLE, ZEnable) X(ZFUNC, ZFunc) X(ZWRITEENABLE, ZWriteEnable) X(FILLMODE, FillMode) \
    X(CULLMODE, CullMode) X(ALPHABLENDENABLE, AlphaBlendEnable) \
    X(SEPARATEALPHABLENDENABLE, SeparateAlphaBlendEnable) X(BLENDFACTOR, BlendFactor) X(SRCBLEND, SrcBlend) \
    X(DESTBLEND, DestBlend) X(BLENDOP, BlendOp) X(SRCBLENDALPHA, SrcBlendAlpha) \
    X(DESTBLENDALPHA, DestBlendAlpha) X(BLENDOPALPHA, BlendOpAlpha) X(ALPHATESTENABLE, AlphaTestEnable) \
    X(ALPHAREF, AlphaRef) X(ALPHAFUNC, AlphaFunc) X(STENCILENABLE, StencilEnable) \
    X(TWOSIDEDSTENCILMODE, TwoSidedStencilMode) X(STENCILFAIL, StencilFail) X(STENCILZFAIL, StencilZFail) \
    X(STENCILPASS, StencilPass) X(STENCILFUNC, StencilFunc) X(STENCILREF, StencilRef) \
    X(STENCILMASK, StencilMask) X(STENCILWRITEMASK, StencilWriteMask) X(CCW_STENCILFAIL, CCWStencilFail) \
    X(CCW_STENCILZFAIL, CCWStencilZFail) X(CCW_STENCILPASS, CCWStencilPass) X(CCW_STENCILFUNC, CCWStencilFunc) \
    X(CCW_STENCILREF, CCWStencilRef) X(CCW_STENCILMASK, CCWStencilMask) \
    X(CCW_STENCILWRITEMASK, CCWStencilWriteMask) X(CLIPPLANEENABLE, ClipPlaneEnable) X(POINTSIZE, PointSize) \
    X(POINTSIZE_MIN, PointSizeMin) X(POINTSPRITEENABLE, PointSpriteEnable) X(POINTSIZE_MAX, PointSizeMax) \
    X(MULTISAMPLEANTIALIAS, MultiSampleAntiAlias) X(MULTISAMPLEMASK, MultiSampleMask) \
    X(SCISSORTESTENABLE, ScissorTestEnable) X(SLOPESCALEDEPTHBIAS, SlopeScaleDepthBias) \
    X(DEPTHBIAS, DepthBias) X(COLORWRITEENABLE, ColorWriteEnable) X(COLORWRITEENABLE1, ColorWriteEnable1) \
    X(COLORWRITEENABLE2, ColorWriteEnable2) X(COLORWRITEENABLE3, ColorWriteEnable3) \
    X(TESSELLATIONMODE, TessellationMode) X(MINTESSELLATIONLEVEL, MinTessellationLevel) \
    X(MAXTESSELLATIONLEVEL, MaxTessellationLevel) X(WRAP0, Wrap0) X(WRAP1, Wrap1) X(WRAP2, Wrap2) \
    X(WRAP3, Wrap3) X(WRAP4, Wrap4) X(WRAP5, Wrap5) X(WRAP6, Wrap6) X(WRAP7, Wrap7) X(WRAP8, Wrap8) \
    X(WRAP9, Wrap9) X(WRAP10, Wrap10) X(WRAP11, Wrap11) X(WRAP12, Wrap12) X(WRAP13, Wrap13) X(WRAP14, Wrap14) \
    X(WRAP15, Wrap15) X(VIEWPORTENABLE, ViewportEnable) X(HIGHPRECISIONBLENDENABLE, HighPrecisionBlendEnable) \
    X(HIGHPRECISIONBLENDENABLE1, HighPrecisionBlendEnable1) \
    X(HIGHPRECISIONBLENDENABLE2, HighPrecisionBlendEnable2) \
    X(HIGHPRECISIONBLENDENABLE3, HighPrecisionBlendEnable3) X(HALFPIXELOFFSET, HalfPixelOffset) \
    X(PRIMITIVERESETENABLE, PrimitiveResetEnable) X(PRIMITIVERESETINDEX, PrimitiveResetIndex) \
    X(ALPHATOMASKENABLE, AlphaToMaskEnable) X(ALPHATOMASKOFFSETS, AlphaToMaskOffsets) \
    X(GUARDBAND_X, GuardBand_X) X(GUARDBAND_Y, GuardBand_Y) X(DISCARDBAND_X, DiscardBand_X) \
    X(DISCARDBAND_Y, DiscardBand_Y) X(HISTENCILENABLE, HiStencilEnable) \
    X(HISTENCILWRITEENABLE, HiStencilWriteEnable) X(HISTENCILFUNC, HiStencilFunc) \
    X(HISTENCILREF, HiStencilRef) X(PRESENTINTERVAL, PresentInterval) \
    X(PRESENTIMMEDIATETHRESHOLD, PresentImmediateThreshold) X(HIZENABLE, HiZEnable) \
    X(HIZWRITEENABLE, HiZWriteEnable) X(LASTPIXEL, LastPixel) X(LINEWIDTH, LineWidth) \
    X(BUFFER2FRAMES, Buffer2Frames)

#define _D3D9_SAMPLER_STATES(X) \
    X(MinFilter) X(MinFilterZ) X(MagFilter) X(MagFilterZ) X(MipFilter) X(SeparateZFilterEnable) X(MaxAnisotropy) \
    X(AnisotropyBias) X(MipMapLodBias) X(MaxMipLevel) X(MinMipLevel) X(BorderColor) X(AddressU) X(AddressV) \
    X(AddressW) X(TrilinearThreshold) X(HGradientExpBias) X(VGradientExpBias) X(WhiteBorderColorW) \
    X(PointBorderEnable)

#define _D3D9_DECLARE_RENDER_STATE(Enum, Name) \
    void WINAPI D3DDevice_SetRenderState_##Name(D3DDevice* pDevice, DWORD Value); \
    DWORD WINAPI D3DDevice_GetRenderState_##Name(D3DDevice* pDevice);
#define _D3D9_DECLARE_SAMPLER_STATE(Name) \
    void WINAPI D3DDevice_SetSamplerState_##Name(D3DDevice* pDevice, DWORD Sampler, DWORD Value); \
    DWORD WINAPI D3DDevice_GetSamplerState_##Name(D3DDevice* pDevice, DWORD Sampler);
_D3D9_RENDER_STATES(_D3D9_DECLARE_RENDER_STATE)
_D3D9_SAMPLER_STATES(_D3D9_DECLARE_SAMPLER_STATE)

// ---- Inline sampler state setters: texture fetch constant fields set straight into the shadow ------------------

// Marks texture fetch constant Sampler pending after one of its fields was written.
#define _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler) \
    D3DTagCollection_Set(&(pDevice)->m_Pending, D3DTag_Index(D3DTAG_TEXTUREFETCHCONSTANTS), D3DTag_SamplerMask(Sampler))

D3DINLINE void WINAPI D3DDevice_SetSamplerState_AddressU_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ClampX = (GPUCLAMP) Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_AddressV_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ClampY = (GPUCLAMP) Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_AddressW_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ClampZ = (GPUCLAMP) Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_MipFilter_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].MipFilter = Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

// Any nonzero border color selects white.
D3DINLINE void WINAPI D3DDevice_SetSamplerState_BorderColor_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].BorderColor = (Value == 0) ? GPUBORDERCOLOR_ABGR_BLACK : GPUBORDERCOLOR_ABGR_WHITE;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_TrilinearThreshold_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].TriClamp = (GPUTRICLAMP) Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_HGradientExpBias_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].GradExpAdjustH = (INT) Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_VGradientExpBias_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].GradExpAdjustV = (INT) Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

D3DINLINE void WINAPI D3DDevice_SetSamplerState_WhiteBorderColorW_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ForceBCWToMax = Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

// Point border on = D3D clamp policy (0); off = OpenGL policy (1).
D3DINLINE void WINAPI D3DDevice_SetSamplerState_PointBorderEnable_Inline(D3DDevice* pDevice, DWORD Sampler, DWORD Value)
{
    Sampler = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    pDevice->m_Constants.TextureFetch[Sampler].ClampPolicy = (GPUCLAMPPOLICY) !Value;
    _D3D9_TEXTURE_FETCH_PENDING(pDevice, Sampler);
}

// ---- Streams and textures: the pending fetch-constant bit is computed inline ----------------------------------

// Stream s uses vertex fetch constant 95 - s, i.e. fetch slot (95 - s) / 3 (the multiply-shift divides by three).
D3DINLINE void WINAPI D3DDevice_SetStreamSource_Inline(D3DDevice *pDevice, UINT StreamNumber, D3DVertexBuffer *pStreamData, UINT OffsetInBytes, UINT Stride)
{
    DWORD vertexFetch = GPU_CONVERT_D3D_TO_HARDWARE_VERTEXFETCHCONSTANT(StreamNumber);
    DWORD slot = (vertexFetch * (65536/3 + 1)) >> 16;
    DWORD bit = D3DTAG_START(D3DTAG_FETCHCONSTANTS) + slot;
    D3DDevice_SetStreamSource(pDevice, StreamNumber, pStreamData, OffsetInBytes, Stride, D3DTAG_MASKENCODE(bit, bit));
}

D3DINLINE void WINAPI D3DDevice_SetTexture_Inline(D3DDevice *pDevice, DWORD Sampler, D3DBaseTexture *pTexture)
{
    DWORD bit = D3DTAG_START(D3DTAG_FETCHCONSTANTS) + GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(Sampler);
    D3DDevice_SetTexture(pDevice, Sampler, pTexture, D3DTAG_MASKENCODE(bit, bit));
}

// ---- Render / sampler state dispatch -------------------------------------------------------------------------

// A non-constant state goes through the device's table (the state value is the byte offset of its entry);
// a constant one resolves at compile time to the state's own entry point.
D3DINLINE void D3DDevice_SetRenderState(D3DDevice *pDevice, D3DRENDERSTATETYPE State, DWORD Value)
{
    if (!D3DISINTCONST(State))
    {
        pDevice->m_SetRenderStateCall[State / 4](pDevice, Value);
    }
    else
    {
        switch (State)
        {
#define _D3D9_SET_CASE(Enum, Name) case D3DRS_##Enum: D3DDevice_SetRenderState_##Name(pDevice, Value); break;
        _D3D9_RENDER_STATES(_D3D9_SET_CASE)
#undef _D3D9_SET_CASE
        }
    }
}

D3DINLINE void D3DDevice_GetRenderState(D3DDevice *pDevice, D3DRENDERSTATETYPE State, DWORD* pValue)
{
    if (!D3DISINTCONST(State))
    {
        *pValue = pDevice->m_GetRenderStateCall[State / 4](pDevice);
    }
    else
    {
        switch (State)
        {
#define _D3D9_GET_CASE(Enum, Name) case D3DRS_##Enum: *pValue = D3DDevice_GetRenderState_##Name(pDevice); break;
        _D3D9_RENDER_STATES(_D3D9_GET_CASE)
#undef _D3D9_GET_CASE
        }
    }
}

D3DINLINE void D3DDevice_SetSamplerState(D3DDevice *pDevice, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    if (!D3DISINTCONST(Type))
    {
        pDevice->m_SetSamplerStateCall[Type / 4](pDevice, Sampler, Value);
    }
    else
    {
        switch (Type)
        {
        case D3DSAMP_ADDRESSU:              D3DDevice_SetSamplerState_AddressU_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_ADDRESSV:              D3DDevice_SetSamplerState_AddressV_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_ADDRESSW:              D3DDevice_SetSamplerState_AddressW_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_BORDERCOLOR:           D3DDevice_SetSamplerState_BorderColor_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_MAGFILTER:             D3DDevice_SetSamplerState_MagFilter(pDevice, Sampler, Value); break;
        case D3DSAMP_MINFILTER:             D3DDevice_SetSamplerState_MinFilter(pDevice, Sampler, Value); break;
        case D3DSAMP_MIPFILTER:             D3DDevice_SetSamplerState_MipFilter_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_MIPMAPLODBIAS:         D3DDevice_SetSamplerState_MipMapLodBias(pDevice, Sampler, Value); break;
        case D3DSAMP_MAXMIPLEVEL:           D3DDevice_SetSamplerState_MaxMipLevel(pDevice, Sampler, Value); break;
        case D3DSAMP_MAXANISOTROPY:         D3DDevice_SetSamplerState_MaxAnisotropy(pDevice, Sampler, Value); break;
        case D3DSAMP_MAGFILTERZ:            D3DDevice_SetSamplerState_MagFilterZ(pDevice, Sampler, Value); break;
        case D3DSAMP_MINFILTERZ:            D3DDevice_SetSamplerState_MinFilterZ(pDevice, Sampler, Value); break;
        case D3DSAMP_SEPARATEZFILTERENABLE: D3DDevice_SetSamplerState_SeparateZFilterEnable(pDevice, Sampler, Value); break;
        case D3DSAMP_MINMIPLEVEL:           D3DDevice_SetSamplerState_MinMipLevel(pDevice, Sampler, Value); break;
        case D3DSAMP_TRILINEARTHRESHOLD:    D3DDevice_SetSamplerState_TrilinearThreshold_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_ANISOTROPYBIAS:        D3DDevice_SetSamplerState_AnisotropyBias(pDevice, Sampler, Value); break;
        case D3DSAMP_HGRADIENTEXPBIAS:      D3DDevice_SetSamplerState_HGradientExpBias_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_VGRADIENTEXPBIAS:      D3DDevice_SetSamplerState_VGradientExpBias_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_WHITEBORDERCOLORW:     D3DDevice_SetSamplerState_WhiteBorderColorW_Inline(pDevice, Sampler, Value); break;
        case D3DSAMP_POINTBORDERENABLE:     D3DDevice_SetSamplerState_PointBorderEnable_Inline(pDevice, Sampler, Value); break;
        }
    }
}

// ---- Shader constants ----------------------------------------------------------------------------------------

// One float4 constant: copied component by component into the shadow (so the optimizer can drop a temporary the
// caller built), then its pending bit is set.
D3DINLINE void D3DDevice_SetVertexShaderConstantF1(D3DDevice* pDevice, UINT StartRegister, CONST float* D3DRESTRICT pConstantData)
{
    pDevice->m_Constants.VertexShaderF[StartRegister].x = pConstantData[0];
    pDevice->m_Constants.VertexShaderF[StartRegister].y = pConstantData[1];
    pDevice->m_Constants.VertexShaderF[StartRegister].z = pConstantData[2];
    pDevice->m_Constants.VertexShaderF[StartRegister].w = pConstantData[3];
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, 1));
}

D3DINLINE void D3DDevice_SetPixelShaderConstantF1(D3DDevice* D3DRESTRICT pDevice, UINT StartRegister, CONST float* D3DRESTRICT pConstantData)
{
    pDevice->m_Constants.PixelShaderF[StartRegister].x = pConstantData[0];
    pDevice->m_Constants.PixelShaderF[StartRegister].y = pConstantData[1];
    pDevice->m_Constants.PixelShaderF[StartRegister].z = pConstantData[2];
    pDevice->m_Constants.PixelShaderF[StartRegister].w = pConstantData[3];
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_PIXELSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, 1));
}

// A compile-time count of one is set inline; anything else goes to the out-of-line setter with its pending mask.
D3DINLINE void D3DDevice_SetVertexShaderConstantF(D3DDevice* pDevice, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount)
{
    if (D3DISINTCONST(Vector4fCount))
    {
        if (Vector4fCount == 1)
            D3DDevice_SetVertexShaderConstantF1(pDevice, StartRegister, pConstantData);
        else
            D3DDevice_SetVertexShaderConstantFN(pDevice, StartRegister, pConstantData, Vector4fCount, D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
    }
    else
    {
        D3DDevice_SetVertexShaderConstantFN(pDevice, StartRegister, pConstantData, Vector4fCount, D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
    }
}

D3DINLINE void D3DDevice_SetPixelShaderConstantF(D3DDevice* pDevice, UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount)
{
    if (D3DISINTCONST(Vector4fCount))
    {
        if (Vector4fCount == 1)
            D3DDevice_SetPixelShaderConstantF1(pDevice, StartRegister, pConstantData);
        else
            D3DDevice_SetPixelShaderConstantFN(pDevice, StartRegister, pConstantData, Vector4fCount, D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
    }
    else
    {
        D3DDevice_SetPixelShaderConstantFN(pDevice, StartRegister, pConstantData, Vector4fCount, D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
    }
}

// ---- Gpu ownership: an owned register group is never pending -------------------------------------------------

D3DINLINE void D3DDevice_GpuOwnVertexShaderConstantF(D3DDevice *pDevice, DWORD StartRegister, DWORD Vector4fCount)
{
    D3DTagCollection_Clear(&pDevice->m_Pending, D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
}

D3DINLINE void D3DDevice_GpuOwnPixelShaderConstantF(D3DDevice *pDevice, DWORD StartRegister, DWORD Vector4fCount)
{
    D3DTagCollection_Clear(&pDevice->m_Pending, D3DTag_Index(D3DTAG_PIXELSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
}

// Handing registers back marks them pending so the next draw rewrites them from the shadow.
D3DINLINE void D3DDevice_GpuDisownVertexShaderConstantF(D3DDevice *pDevice, DWORD StartRegister, DWORD Vector4fCount)
{
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
}

D3DINLINE void D3DDevice_GpuDisownPixelShaderConstantF(D3DDevice *pDevice, DWORD StartRegister, DWORD Vector4fCount)
{
    D3DTagCollection_Set(&pDevice->m_Pending, D3DTag_Index(D3DTAG_PIXELSHADERCONSTANTS), D3DTag_ShaderConstantMask(StartRegister, Vector4fCount));
}

// Nothing to do outside debug builds.
D3DINLINE void D3DDevice_GpuEndShaderConstantF4(D3DDevice *pDevice)
{
}

// ---- D3DDevice methods: thin forwards to D3DDevice_* ----------------------------------------------------------

#ifdef __cplusplus

D3DMINLINE D3DVOID WINAPI D3DDevice::GetDeviceCaps(D3DCAPS9 *pCaps) { D3DDevice_GetDeviceCaps(this, pCaps); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetDisplayMode(UINT UnusedSwapChain, D3DDISPLAYMODE *pMode) { D3DDevice_GetDisplayMode(this, UnusedSwapChain, pMode); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters) { return D3DDevice_Reset(this, pPresentationParameters); }
D3DMINLINE D3DVOID WINAPI D3DDevice::Present(CONST RECT *pUnusedSourceRect, CONST RECT *pUnusedDestRect, void *hUnusedDestWindowOverride, void *pUnusedDirtyRegion) { D3DDevice_Present(this); D3DVOIDRETURN; }

// The Create* methods report failure as E_OUTOFMEMORY.
#define _D3D9_CREATED(Expression) (((Expression) != NULL) ? S_OK : E_OUTOFMEMORY)
D3DMINLINE HRESULT WINAPI D3DDevice::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD UnusedFVF, D3DPOOL UnusedPool, D3DVertexBuffer **ppVertexBuffer, HANDLE* pUnusedSharedHandle) { return _D3D9_CREATED(*ppVertexBuffer = D3DDevice_CreateVertexBuffer(Length, Usage, UnusedPool)); }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool, D3DIndexBuffer **ppIndexBuffer, HANDLE* pUnusedSharedHandle) { return _D3D9_CREATED(*ppIndexBuffer = D3DDevice_CreateIndexBuffer(Length, Usage, Format, UnusedPool)); }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedLockable, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters) { return _D3D9_CREATED(*ppSurface = D3DDevice_CreateSurface(Width, Height, Format, MultiSample, pParameters)); }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD UnusedMultisampleQuality, BOOL UnusedDiscard, D3DSurface **ppSurface, CONST D3DSURFACE_PARAMETERS* pParameters) { return _D3D9_CREATED(*ppSurface = D3DDevice_CreateSurface(Width, Height, Format, MultiSample, pParameters)); }
D3DMINLINE HRESULT WINAPI D3DDevice::CreateVertexShader(CONST DWORD *pFunction, D3DVertexShader** ppShader) { return _D3D9_CREATED(*ppShader = D3DDevice_CreateVertexShader(pFunction)); }
D3DMINLINE HRESULT WINAPI D3DDevice::CreatePixelShader(CONST DWORD *pFunction, D3DPixelShader** ppShader) { return _D3D9_CREATED(*ppShader = D3DDevice_CreatePixelShader(pFunction)); }

// With no query pointer, CreateQueryTiled only reports whether the type is supported (event and occlusion are).
D3DMINLINE HRESULT WINAPI D3DDevice::CreateQueryTiled(D3DQUERYTYPE Type, DWORD TileCount, D3DQuery** ppQuery)
{
    if (ppQuery == NULL)
        return (Type == D3DQUERYTYPE_EVENT || Type == D3DQUERYTYPE_OCCLUSION) ? S_OK : D3DERR_NOTAVAILABLE;
    return _D3D9_CREATED(*ppQuery = D3DDevice_CreateQueryTiled(this, Type, TileCount));
}

D3DMINLINE D3DVOID WINAPI D3DDevice::SetRenderTarget(DWORD RenderTargetIndex, D3DSurface *pRenderTarget) { D3DDevice_SetRenderTarget_External(this, RenderTargetIndex, pRenderTarget); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetDepthStencilSurface(D3DSurface *pZStencilSurface) { D3DDevice_SetDepthStencilSurface(this, pZStencilSurface); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) { D3DDevice_Clear(this, Count, pRects, Flags, Color, Z, Stencil, FALSE); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::ClearF(DWORD Flags, CONST D3DRECT *pRect, CONST D3DVECTOR4* pColor, float Z, DWORD Stencil) { D3DDevice_ClearF(this, Flags, pRect, pColor, Z, Stencil); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) { D3DDevice_SetRenderState(this, State, Value); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue) { D3DDevice_GetRenderState(this, State, pValue); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetTexture(DWORD Sampler, D3DBaseTexture *pTexture) { D3DDevice_SetTexture_Inline(this, Sampler, pTexture); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) { D3DDevice_SetSamplerState(this, Sampler, Type, Value); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { D3DDevice_DrawVertices(this, PrimitiveType, StartVertex, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT UnusedMinIndex, UINT UnusedNumIndices, UINT StartIndex, UINT PrimitiveCount) { D3DDevice_DrawIndexedVertices(this, PrimitiveType, BaseVertexIndex, StartIndex, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount)); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) { D3DDevice_DrawVerticesUP(this, PrimitiveType, D3DVERTEXCOUNT(PrimitiveType, PrimitiveCount), pVertexStreamZeroData, VertexStreamZeroStride); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShader(D3DVertexShader *pShader) { D3DDevice_SetVertexShader(this, pShader); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount) { D3DDevice_SetVertexShaderConstantB(this, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount) { D3DDevice_SetVertexShaderConstantF(this, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexShaderConstantI(UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount) { D3DDevice_SetVertexShaderConstantI(this, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetStreamSource(UINT StreamNumber, D3DVertexBuffer *pStreamData, UINT OffsetInBytes, UINT Stride) { D3DDevice_SetStreamSource_Inline(this, StreamNumber, pStreamData, OffsetInBytes, Stride); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetIndices(D3DIndexBuffer *pIndexData) { D3DDevice_SetIndices(this, pIndexData); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShader(D3DPixelShader* pShader) { D3DDevice_SetPixelShader(this, pShader); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount) { D3DDevice_SetPixelShaderConstantB(this, StartRegister, pConstantData, BoolCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, DWORD Vector4fCount) { D3DDevice_SetPixelShaderConstantF(this, StartRegister, pConstantData, Vector4fCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPixelShaderConstantI(UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount) { D3DDevice_SetPixelShaderConstantI(this, StartRegister, pConstantData, Vector4iCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetVertexDeclaration(D3DVertexDeclaration *pDecl) { D3DDevice_SetVertexDeclaration(this, pDecl); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetScissorRect(CONST RECT* pRect) { D3DDevice_SetScissorRect(this, pRect); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetClipPlane(DWORD Index, CONST float* pPlane) { D3DDevice_SetClipPlane(this, Index, pPlane); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::Resolve(DWORD Flags, CONST D3DRECT *pSourceRect, D3DBaseTexture *pDestTexture, CONST D3DPOINT *pDestPoint, UINT DestLevel, UINT DestSliceOrFace, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters) { D3DDevice_Resolve(this, Flags, pSourceRect, pDestTexture, pDestPoint, DestLevel, DestSliceOrFace, pClearColor, ClearZ, ClearStencil, pParameters); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::AcquireThreadOwnership() { D3DDevice_AcquireThreadOwnership(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::ReleaseThreadOwnership() { D3DDevice_ReleaseThreadOwnership(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BlockUntilIdle() { D3DDevice_BlockUntilIdle(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SynchronizeToPresentationInterval() { D3DDevice_SynchronizeToPresentationInterval(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::Swap(D3DBaseTexture* pFrontBuffer, CONST D3DVIDEO_SCALER_PARAMETERS* pParameters) { D3DDevice_Swap(this, pFrontBuffer, pParameters); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetPredication(DWORD PredicationMask) { D3DDevice_SetPredication(this, PredicationMask); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginTiling(DWORD Flags, DWORD Count, CONST D3DRECT* pTileRects, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil) { D3DDevice_BeginTiling(this, Flags, Count, pTileRects, pClearColor, ClearZ, ClearStencil); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DDevice::EndTiling(DWORD ResolveFlags, CONST D3DRECT* pResolveRects, D3DBaseTexture* pDestTexture, CONST D3DVECTOR4* pClearColor, float ClearZ, DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS* pParameters) { return D3DDevice_EndTiling(this, ResolveFlags, pResolveRects, pDestTexture, pClearColor, ClearZ, ClearStencil, pParameters); }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginExport(DWORD Index, D3DResource* pResource, DWORD Flags) { D3DDevice_BeginExport(this, Index, pResource, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndExport(DWORD Index, D3DResource* pResource, DWORD Flags) { D3DDevice_EndExport(this, Index, pResource, Flags); D3DVOIDRETURN; }
D3DMINLINE DWORD   WINAPI D3DDevice::InsertFence() { return D3DDevice_InsertFence(this); }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetBlendState(DWORD RenderTargetIndex, D3DBLENDSTATE BlendState) { D3DDevice_SetBlendState(this, RenderTargetIndex, BlendState); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::BeginConditionalRendering(DWORD Identifier) { D3DDevice_BeginConditionalRendering(this, Identifier); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::EndConditionalRendering() { D3DDevice_EndConditionalRendering(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::SetShaderGPRAllocation(DWORD Flags, DWORD VertexShaderCount, DWORD PixelShaderCount) { D3DDevice_SetShaderGPRAllocation(this, Flags, VertexShaderCount, PixelShaderCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::GetShaderGPRAllocation(DWORD* pFlags, DWORD* pVertexShaderCount, DWORD* pPixelShaderCount) { D3DDevice_GetShaderGPRAllocation(this, pFlags, pVertexShaderCount, pPixelShaderCount); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::FlushHiZStencil(D3DFHZS_FLUSHTYPE FlushType) { D3DDevice_FlushHiZStencil(this, FlushType); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DDevice::UnsetAll() { D3DDevice_UnsetAll(this); D3DVOIDRETURN; }

D3DMINLINE void    WINAPI D3DDevice::GpuOwnVertexShaderConstantF(DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuOwnVertexShaderConstantF(this, StartRegister, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuOwnPixelShaderConstantF(DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuOwnPixelShaderConstantF(this, StartRegister, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuDisownVertexShaderConstantF(DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuDisownVertexShaderConstantF(this, StartRegister, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuDisownPixelShaderConstantF(DWORD StartRegister, DWORD Vector4fCount) { D3DDevice_GpuDisownPixelShaderConstantF(this, StartRegister, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuLoadPixelShaderConstantF4Pointer(UINT StartRegister, CONST void* pConstantData, DWORD Vector4fCount) { D3DDevice_GpuLoadShaderConstantF4Pointer(this, TRUE, StartRegister, pConstantData, Vector4fCount); }
D3DMINLINE void    WINAPI D3DDevice::GpuLoadVertexShaderConstantF4Pointer(UINT StartRegister, CONST void* pConstantData, DWORD Vector4fCount) { D3DDevice_GpuLoadShaderConstantF4Pointer(this, FALSE, StartRegister, pConstantData, Vector4fCount); }
D3DMINLINE HRESULT WINAPI D3DDevice::GpuBeginVertexShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount) { return _D3D9_CREATED(*ppConstantData = D3DDevice_GpuBeginShaderConstantF4(this, FALSE, StartRegister, Vector4fCount)); }
D3DMINLINE HRESULT WINAPI D3DDevice::GpuBeginPixelShaderConstantF4(UINT StartRegister, D3DVECTOR4** ppConstantData, DWORD Vector4fCount) { return _D3D9_CREATED(*ppConstantData = D3DDevice_GpuBeginShaderConstantF4(this, TRUE, StartRegister, Vector4fCount)); }
D3DMINLINE void    WINAPI D3DDevice::GpuEndPixelShaderConstantF4() { D3DDevice_GpuEndShaderConstantF4(this); }

#endif // __cplusplus

// ---- Resource APIs -------------------------------------------------------------------------------------------

ULONG WINAPI D3DQuery_Release(D3DQuery* pThis);
void WINAPI D3DQuery_Issue(D3DQuery* pThis, DWORD IssueFlags);
HRESULT WINAPI D3DQuery_GetData(D3DQuery* pThis, void* pData, DWORD Size, DWORD GetDataFlags);
ULONG WINAPI D3DResource_AddRef(D3DResource* pThis);
ULONG WINAPI D3DResource_Release(D3DResource* pThis);
D3DRESOURCETYPE WINAPI D3DResource_GetType(D3DResource* pThis);
BOOL WINAPI D3DResource_IsBusy(D3DResource* pThis);
void WINAPI D3DResource_BlockUntilNotBusy(D3DResource* pThis);
DWORD WINAPI D3DBaseTexture_GetLevelCount(D3DBaseTexture* pThis);
void WINAPI D3DTexture_GetLevelDesc(D3DTexture* pThis, UINT Level, D3DSURFACE_DESC* pDesc);
D3DSurface* WINAPI D3DTexture_GetSurfaceLevel(D3DTexture* pThis, UINT Level);
void WINAPI D3DTexture_LockRect(D3DTexture* pThis, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);
void WINAPI D3DTexture_UnlockRect(D3DTexture* pThis, UINT Level);
void WINAPI D3DVolumeTexture_LockBox(D3DVolumeTexture* pThis, UINT Level, D3DLOCKED_BOX* pLockedVolume, CONST D3DBOX* pBox, DWORD Flags);
void WINAPI D3DCubeTexture_GetLevelDesc(D3DCubeTexture* pThis, UINT Level, D3DSURFACE_DESC* pDesc);
D3DSurface* WINAPI D3DCubeTexture_GetCubeMapSurface(D3DCubeTexture* pThis, D3DCUBEMAP_FACES FaceType, UINT Level);
void WINAPI D3DCubeTexture_LockRect(D3DCubeTexture* pThis, D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);
DWORD WINAPI D3DArrayTexture_GetArraySize(D3DArrayTexture* pThis);
D3DSurface* WINAPI D3DArrayTexture_GetArraySurface(D3DArrayTexture* pThis, UINT ArrayIndex, UINT Level);
void WINAPI D3DArrayTexture_LockRect(D3DArrayTexture* pThis, UINT ArrayIndex, UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);
void* WINAPI D3DVertexBuffer_Lock(D3DVertexBuffer* pThis, UINT OffsetToLock, UINT SizeToLock, DWORD Flags);
void WINAPI D3DVertexBuffer_Unlock(D3DVertexBuffer* pThis);
void* WINAPI D3DIndexBuffer_Lock(D3DIndexBuffer* pThis, UINT OffsetToLock, UINT SizeToLock, DWORD Flags);
void WINAPI D3DIndexBuffer_Unlock(D3DIndexBuffer* pThis);
void WINAPI D3DSurface_GetDesc(D3DSurface* pThis, D3DSURFACE_DESC* pDesc);
void WINAPI D3DSurface_LockRect(D3DSurface* pThis, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);
void WINAPI D3DVertexShader_Bind(D3DVertexShader* pThis, DWORD Flags, D3DVertexDeclaration* pVertexDeclaration, CONST DWORD* pStreamStrides, D3DPixelShader* pPixelShader);
void WINAPI D3DCommandBuffer_GetClone(D3DCommandBuffer* pThis, DWORD Flags, D3DCommandBuffer* pHeaderPart, DWORD* pHeaderSize, void* pPhysicalPart, DWORD* pPhysicalSize);

#ifdef __cplusplus
D3DMINLINE ULONG   WINAPI D3DQuery::Release() { return D3DQuery_Release(this); }
D3DMINLINE D3DVOID WINAPI D3DQuery::Issue(DWORD IssueFlags) { D3DQuery_Issue(this, IssueFlags); D3DVOIDRETURN; }
D3DMINLINE HRESULT WINAPI D3DQuery::GetData(void* pData, DWORD Size, DWORD GetDataFlags) { return D3DQuery_GetData(this, pData, Size, GetDataFlags); }
D3DMINLINE D3DVOID WINAPI D3DTexture::LockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags) { D3DTexture_LockRect(this, Level, pLockedRect, pRect, Flags); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DTexture::UnlockRect(UINT Level) { D3DTexture_UnlockRect(this, Level); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DVertexBuffer::Unlock() { D3DVertexBuffer_Unlock(this); D3DVOIDRETURN; }
D3DMINLINE D3DVOID WINAPI D3DIndexBuffer::Unlock() { D3DIndexBuffer_Unlock(this); D3DVOIDRETURN; }
#endif

#ifdef __cplusplus
}
#endif

#endif // _D3D9_H_
