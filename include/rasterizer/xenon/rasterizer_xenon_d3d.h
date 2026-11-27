#ifndef RASTERIZER_XENON_RASTERIZER_XENON_D3D_H
#define RASTERIZER_XENON_RASTERIZER_XENON_D3D_H

struct D3DDevice;
struct D3DVertexBuffer;
struct D3DTexture;
struct tagRECT;

struct _D3DLOCKED_RECT
{
    long Pitch;
    void *pBits;
};

// Shared Direct3D declarations.
extern "C" {
void D3DDevice_SetPixelShaderConstantFN(D3DDevice *device, unsigned int start_register, const float *constants,
    unsigned long count, unsigned long long pending_mask);
void D3DDevice_SetPixelShaderConstantB(D3DDevice *device, unsigned long start, const int *values, unsigned long count);
void D3DTexture_LockRect(D3DTexture *texture, unsigned int level, _D3DLOCKED_RECT *locked, const tagRECT *rectangle, unsigned long flags);
void *D3DTexture_GetSurfaceLevel(D3DTexture *texture, unsigned long level);
void D3DTexture_UnlockRect(D3DTexture *texture, unsigned int level);
void D3DDevice_SetVertexShaderConstantB(D3DDevice *device, unsigned long start, const int *values, unsigned long count);
D3DVertexBuffer *D3DDevice_CreateVertexBuffer(unsigned int length, unsigned long usage, unsigned long pool);
void *D3DVertexBuffer_Lock(D3DVertexBuffer *buffer, unsigned int offset, unsigned int size, unsigned long flags);
void D3DVertexBuffer_Unlock(D3DVertexBuffer *buffer);
void D3DDevice_FlushHiZStencil(D3DDevice *device, long flush_type);
void D3DDevice_SetRenderState_AlphaBlendEnable(D3DDevice *device, unsigned long enable);
void D3DDevice_SetRenderState_ColorWriteEnable(D3DDevice *device, unsigned long mask);
void D3DDevice_SetRenderState_ColorWriteEnable1(D3DDevice *device, unsigned long mask);
void D3DDevice_SetRenderState_BlendOp(D3DDevice *device, unsigned long operation);
void D3DDevice_SetRenderState_SrcBlend(D3DDevice *device, unsigned long blend);
void D3DDevice_SetRenderState_DestBlend(D3DDevice *device, unsigned long blend);
unsigned long D3DDevice_GetDeviceState(D3DDevice *device);
void D3DDevice_BlockOnFence(unsigned long fence);
void D3DDevice_SetRenderState_ZEnable(D3DDevice *device, unsigned long enable);
void D3DDevice_SetRenderState_ZWriteEnable(D3DDevice *device, unsigned long enable);
void D3DDevice_SetRenderState_ZFunc(D3DDevice *device, unsigned long function);
void D3DDevice_SetRenderState_SlopeScaleDepthBias(D3DDevice *device, unsigned long bias);
void D3DDevice_SetRenderState_DepthBias(D3DDevice *device, unsigned long bias);
void D3DDevice_SetRenderState_CullMode(D3DDevice *device, unsigned long mode);
void D3DDevice_BeginExport(D3DDevice *device, unsigned long index, D3DVertexBuffer *buffer, unsigned long flags);
void D3DDevice_EndExport(D3DDevice *device, unsigned long index, D3DVertexBuffer *buffer, unsigned long flags);
void D3DDevice_SetRenderState_TessellationMode(D3DDevice *device, unsigned long mode);
void D3DDevice_SetRenderState_MaxTessellationLevel(D3DDevice *device, unsigned long level);
void D3DDevice_SetRenderState_MinTessellationLevel(D3DDevice *device, unsigned long level);
void D3DDevice_DrawIndexedTessellatedVertices(D3DDevice *device, long primitive_type, long base_vertex_index, long start_index, long index_count);
void D3DDevice_SetRenderState_FillMode(D3DDevice *device, unsigned long mode);
}

extern "C" void D3DDevice_SetRenderState_BlendFactor(D3DDevice *device, unsigned long value);
extern "C" void D3DDevice_EndTiling(D3DDevice *device, unsigned long flags, const void *rectangles, const void *resolve_data, const void *clear_color,
    float depth, unsigned long stencil, unsigned long unused);

#endif // RASTERIZER_XENON_RASTERIZER_XENON_D3D_H
