#ifndef RENDER_RENDER_WATER_XENON_H
#define RENDER_RENDER_WATER_XENON_H

struct D3DDevice;
struct D3DArrayTexture;
struct D3DIndexBuffer;

extern "C" {
unsigned long D3DArrayTexture_GetArraySize(D3DArrayTexture *texture);
void D3DDevice_SetIndices(D3DDevice *device, D3DIndexBuffer *index_buffer);
void XGSetIndexBufferHeader(unsigned long length, unsigned long usage, unsigned long format, unsigned long pool,
    unsigned long base_offset, D3DIndexBuffer *index_buffer);
void D3DDevice_SetVertexShaderConstantFN(D3DDevice *device, unsigned int start_register, const float *constant_data,
    unsigned long vector4f_count, unsigned __int64 mask);
}

extern "C" void D3DDevice_SetPixelShaderConstantFN(D3DDevice *device, unsigned int start_register, const float *constant_data,
    unsigned long vector4f_count, unsigned __int64 mask);

struct _XMMATRIX;
extern "C" _XMMATRIX g_water_view_projection_matrix;
#endif
