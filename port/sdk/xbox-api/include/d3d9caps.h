// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3d9caps.h: the device capability record (D3DCAPS9) and the presentation-interval values.
#ifndef _d3d9CAPS_H
#define _d3d9CAPS_H

#ifndef DIRECT3D_VERSION
#define DIRECT3D_VERSION 0x0900
#endif

typedef struct _D3DVSHADERCAPS2_0
{
    DWORD Caps;
    INT DynamicFlowControlDepth, NumTemps, StaticFlowControlDepth;
} D3DVSHADERCAPS2_0;

typedef struct _D3DPSHADERCAPS2_0
{
    DWORD Caps;
    INT DynamicFlowControlDepth, NumTemps, StaticFlowControlDepth, NumInstructionSlots;
} D3DPSHADERCAPS2_0;

typedef struct _D3DCAPS9
{
    D3DDEVTYPE DeviceType;
    UINT AdapterOrdinal;
    DWORD Caps, Caps2, Caps3, PresentationIntervals, CursorCaps, DevCaps, PrimitiveMiscCaps, RasterCaps, ZCmpCaps;
    DWORD SrcBlendCaps, DestBlendCaps, AlphaCmpCaps, ShadeCaps, TextureCaps, TextureFilterCaps;
    DWORD CubeTextureFilterCaps, VolumeTextureFilterCaps, TextureAddressCaps, VolumeTextureAddressCaps, LineCaps;
    DWORD MaxTextureWidth, MaxTextureHeight;
    DWORD MaxVolumeExtent, MaxTextureRepeat, MaxTextureAspectRatio, MaxAnisotropy;
    float MaxVertexW, GuardBandLeft, GuardBandTop, GuardBandRight, GuardBandBottom, ExtentsAdjust;
    DWORD StencilCaps, FVFCaps, TextureOpCaps, MaxTextureBlendStages, MaxSimultaneousTextures, VertexProcessingCaps;
    DWORD MaxActiveLights, MaxUserClipPlanes, MaxVertexBlendMatrices, MaxVertexBlendMatrixIndex;
    float MaxPointSize;
    DWORD MaxPrimitiveCount, MaxVertexIndex, MaxStreams, MaxStreamStride, VertexShaderVersion, MaxVertexShaderConst;
    DWORD PixelShaderVersion;
    float PixelShader1xMaxValue;
    DWORD DevCaps2;
    float MaxNpatchTessellationLevel;
    DWORD Reserved5;
    UINT MasterAdapterOrdinal, AdapterOrdinalInGroup, NumberOfAdaptersInGroup;
    DWORD DeclTypes, NumSimultaneousRTs, StretchRectFilterCaps;
    D3DVSHADERCAPS2_0 VS20Caps;
    D3DPSHADERCAPS2_0 PS20Caps;
    DWORD VertexTextureFilterCaps, MaxVShaderInstructionsExecuted, MaxPShaderInstructionsExecuted;
    DWORD MaxVertexShader30InstructionSlots, MaxPixelShader30InstructionSlots;
} D3DCAPS9;

// D3DPRESENT_PARAMETERS::PresentationInterval
#define D3DPRESENT_INTERVAL_DEFAULT                 0x00000000L
#define D3DPRESENT_INTERVAL_ONE                     0x00000001L
#define D3DPRESENT_INTERVAL_TWO                     0x00000002L
#define D3DPRESENT_INTERVAL_THREE                   0x00000004L
#define D3DPRESENT_INTERVAL_FOUR                    0x00000008L
#define D3DPRESENT_INTERVAL_IMMEDIATE               0x80000000L

// User clip planes (D3DRS_CLIPPLANEENABLE bits, D3DDevice::m_ClipPlanes).
#define D3DMAXUSERCLIPPLANES                        6

#endif // _d3d9CAPS_H
