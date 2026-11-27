#ifndef RASTERIZER_XENON_RASTERIZER_DEVICE_TYPES_H
#define RASTERIZER_XENON_RASTERIZER_DEVICE_TYPES_H

#include "math/real_math_types.h"
#include <stddef.h>

struct D3DSurface;
struct D3DTexture;

struct D3DTAGCOLLECTION { unsigned __int64 m_Mask[5]; };
struct D3DConstants
{
    unsigned long Fetch[0x300 / sizeof(unsigned long)];
    real_vector4d VertexShaderF[256];
    real_vector4d PixelShaderF[256];
    unsigned long Flow[256 / 32 + 32]; // GPU_FLOW_CONSTANTS: boolean words, then integer constants
};

// XDK d3d9gpu.h register packets shadowed in D3DDevice, held as raw register words.
struct s_d3d_destination_packet {
    unsigned long SurfaceInfo;
    unsigned long Color0Info;
    unsigned long DepthInfo;
    unsigned long Color1Info;
    unsigned long Color2Info;
    unsigned long Color3Info;
    unsigned long CoherDestBase[8];
    unsigned long ScreenScissorTL;
    unsigned long ScreenScissorBR;
};

struct s_d3d_window_packet {
    unsigned long WindowOffset;
    unsigned long WindowScissorTL;
    unsigned long WindowScissorBR;
};

struct s_d3d_values_packet {
    unsigned long MaxVtxIndx;
    unsigned long MinVtxIndx;
    unsigned long IndxOffset;
    unsigned long MultiPrimIbResetIndx;
    unsigned long ColorMask;
    float BlendRed;
    float BlendGreen;
    float BlendBlue;
    float BlendAlpha;
    unsigned long Unused[3];
    unsigned long StencilRefMaskBF;
    unsigned long StencilRefMask;
    float AlphaRef;
    float VportXScale;
    float VportXOffset;
    float VportYScale;
    float VportYOffset;
    float VportZScale;
    float VportZOffset;
};

struct s_d3d_program_packet {
    unsigned long ProgramControl;
    unsigned long ContextMisc;
    unsigned long InterpolatorControl;
    unsigned long Wrapping0;
    unsigned long Wrapping1;
};

// XDK public register shadow at device +0x2934; d3d9gpu.h packet 4.
struct s_d3d_control_packet {
    unsigned long DepthControl;
    unsigned long BlendControl0;
    unsigned long ColorControl;
    union { unsigned long dword; unsigned char bytes[4]; } HiControl;
    unsigned long ClipControl;
    unsigned long ModeControl;
    unsigned long VteControl;
    unsigned long Unused;
    unsigned long EdramModeControl;
    unsigned long BlendControl1;
    unsigned long BlendControl2;
    unsigned long BlendControl3;
};

struct D3DDevice {
    // Public XDK d3d9.h device prefix; D3DRS_MAX=404, D3DSAMP_MAX=80.
    D3DTAGCOLLECTION m_Pending;
    unsigned __int64 m_Predicated_PendingMask2;
    unsigned long *m_pRing;
    unsigned long *m_pRingLimit;
    unsigned long *m_pRingGuarantee;
    unsigned long m_ReferenceCount;
    void (*m_SetRenderStateCall[404 / 4])(D3DDevice *, unsigned long);
    void (*m_SetSamplerStateCall[80 / 4])(D3DDevice *, unsigned long, unsigned long);
    unsigned long (*m_GetRenderStateCall[404 / 4])(D3DDevice *);
    unsigned long (*m_GetSamplerStateCall[80 / 4])(D3DDevice *, unsigned long);
    __declspec(align(128)) D3DConstants m_Constants;
    float m_ClipPlanes[6][4]; // D3DMAXUSERCLIPPLANES
    s_d3d_destination_packet m_DestinationPacket;
    s_d3d_window_packet m_WindowPacket;
    s_d3d_values_packet m_ValuesPacket;
    s_d3d_program_packet m_ProgramPacket;
    s_d3d_control_packet m_ControlPacket;
    __forceinline void SetPixelShaderConstantF(unsigned int start_register, const float *constants, unsigned long count);
    __forceinline long SetDepthStencilSurface(D3DSurface *surface);
    __forceinline long SetTexture(unsigned long sampler, D3DTexture *texture);
    __forceinline long SetRenderTarget(unsigned long index, D3DSurface *surface);
};
typedef char d3d_device_constants_offset[(sizeof(void *) != 4 || offsetof(D3DDevice, m_Constants) == 0x480) ? 1 : -1];

typedef char d3d_device_pixel_constants_offset[(sizeof(void *) != 4 || offsetof(D3DDevice, m_Constants.PixelShaderF) == 0x1780) ? 1 : -1];
typedef char d3d_device_control_packet_offset[(sizeof(void *) != 4 || offsetof(D3DDevice, m_ControlPacket) == 0x2934) ? 1 : -1];
typedef char d3d_device_hi_control_offset[(sizeof(void *) != 4 || offsetof(D3DDevice, m_ControlPacket.HiControl) == 0x2940) ? 1 : -1];
typedef char d3d_device_edram_offset[(sizeof(void *) != 4 || offsetof(D3DDevice, m_ControlPacket.EdramModeControl) == 0x2954) ? 1 : -1];

#endif
