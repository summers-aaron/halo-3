#ifndef RASTERIZER_RASTERIZER_D3D_ALLOCATIONS_H
#define RASTERIZER_RASTERIZER_D3D_ALLOCATIONS_H

// Functions defined in tu/graphicslibxbox/rasterizer_d3d_allocations.cpp.

struct D3DVertexBuffer;
struct D3DIndexBuffer;
struct D3DVertexShader;
struct D3DPixelShader;
struct D3DSurface;
struct D3DTexture;
struct D3DCubeTexture;
enum _D3DFORMAT;
enum _D3DMULTISAMPLE_TYPE;

class c_d3d_resource_allocator
{
public:
    static long create_d3d_vertex_buffer(unsigned int length, unsigned long usage, unsigned long fvf,
        unsigned long pool, D3DVertexBuffer **buffer, void **allocation);
    static long create_d3d_index_buffer(unsigned int length, unsigned long usage, _D3DFORMAT format,
        unsigned long pool, D3DIndexBuffer **buffer, void **allocation);
    static long create_vertex_shader(const unsigned long *function, D3DVertexShader **shader);
    static long create_pixel_shader(const unsigned long *function, D3DPixelShader **shader);
    static long create_render_target(unsigned int width, unsigned int height, _D3DFORMAT format,
        _D3DMULTISAMPLE_TYPE multisample, unsigned long quality, int lockable, D3DSurface **surface,
        const void *parameters);
    static long create_texture(unsigned int width, unsigned int height, unsigned int levels,
        unsigned long usage, _D3DFORMAT format, unsigned long pool, D3DTexture **texture,
        unsigned long *size, unsigned char *data, unsigned long flags);
    static long create_cube_texture(unsigned int edge, unsigned int levels, unsigned long usage,
        _D3DFORMAT format, unsigned long pool, D3DCubeTexture **texture, unsigned long *size,
        unsigned char *data, unsigned long flags);
};

extern "C" {
extern void *g_d3d_resource_allocator_device;
}

void *allocate_d3d_memory_tracked(unsigned long size);

#endif // RASTERIZER_RASTERIZER_D3D_ALLOCATIONS_H
