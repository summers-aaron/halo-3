// graphicslibxbox/rasterizer_d3d_allocations.obj: from the records 0x468338 0x4683c8 0x468450 0x4684d8 0x468548 0x468660 0x468750
// @flags /TP /O2
// @address 0x468450 ?create_pixel_shader@c_d3d_resource_allocator@@SAJPBKPAPAUD3DPixelShader@@@Z
#include <xtl.h>
#include <xgraphics.h>
#include <string.h>
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_d3d_allocations.h"

// @address 0x8c485c g_d3d_resource_allocator_device
void *g_d3d_resource_allocator_device;


HRESULT c_d3d_resource_allocator::create_d3d_vertex_buffer(UINT length, DWORD usage, DWORD fvf, DWORD pool, D3DVertexBuffer **buffer, void **allocation)
{
  HRESULT result = S_OK;
  if (g_d3d_resource_allocator_device)
    result = c_rasterizer::get_device()->CreateVertexBuffer(length, 0, 0, D3DPOOL_MANAGED, buffer, NULL);
  return result;
}

HRESULT c_d3d_resource_allocator::create_vertex_shader(const DWORD *function, D3DVertexShader **shader)
{
  HRESULT result = S_OK;
  if (g_d3d_resource_allocator_device)
    result = c_rasterizer::get_device()->CreateVertexShader(function, shader);
  return result;
}

// 0x468450
HRESULT c_d3d_resource_allocator::create_pixel_shader(const DWORD *function, D3DPixelShader **shader)
{
  HRESULT result = S_OK;
  if (g_d3d_resource_allocator_device)
    result = c_rasterizer::get_device()->CreatePixelShader(function, shader);
  return result;
}

static __forceinline void *rasterizer_d3d_allocations_allocate_contiguous_memory(unsigned long size)
{
  unsigned long flags;
  unsigned long page_size = XMemGetPageSize(0);
  if (page_size == 0x1000)
    flags = 0;
  else if (page_size == 0x10000)
    flags = 0x20000000;
  else if (page_size == 0x1000000)
    flags = 0x80000000;
  else
    flags = 0;
  return XPhysicalAlloc(size, ~0UL, flags, 0x404);
}

// 0x4684d8
void *allocate_d3d_memory_tracked(unsigned long size)
{
  void *result = rasterizer_d3d_allocations_allocate_contiguous_memory(size);
  return result;
}

HRESULT c_d3d_resource_allocator::create_render_target(UINT width, UINT height, D3DFORMAT format, D3DMULTISAMPLE_TYPE multisample,
    DWORD quality, BOOL lockable, D3DSurface **surface, const void *parameters)
{
  HRESULT result = S_OK;
  if (g_d3d_resource_allocator_device)
    result = c_rasterizer::get_device()->CreateRenderTarget(
        width, height, format, multisample, quality, lockable, surface, static_cast<const D3DSURFACE_PARAMETERS *>(parameters));
  return result;
}

// @address 0x468548 ?create_texture@c_d3d_resource_allocator@@SAJIIIKW4_D3DFORMAT@@KPAPAUD3DTexture@@PAKPAEK@Z
long c_d3d_resource_allocator::create_texture(unsigned int width, unsigned int height, unsigned int levels, unsigned long usage,
    D3DFORMAT format, unsigned long pool, D3DTexture **out_texture, unsigned long *out_size, unsigned char *data, unsigned long flags)
{
    bool user_memory = (flags & 1) != 0;
    bool tiled = user_memory && !(flags & 2);
    unsigned long texture_usage = tiled ? 4 : usage;
    D3DTexture *texture = (D3DTexture *)RtlAllocateHeap(GetProcessHeap(), 0, sizeof(D3DTexture));
    unsigned int size = XGSetTextureHeader(width, height, levels, texture_usage, format, (D3DPOOL)pool, 0, -1, 0, texture, 0, 0);
    if (size) {
        void *address;
        if (user_memory)
            address = data;
        else
            address = allocate_d3d_memory_tracked(size);
        if (address) {
            XGOffsetResourceAddress(texture, address);
            if (!user_memory && data)
                memcpy(address, data, size);
            *out_texture = texture;
        }
    }
    return 0;
}

// @address 0x468660 ?create_cube_texture@c_d3d_resource_allocator@@SAJIIKW4_D3DFORMAT@@KPAPAUD3DCubeTexture@@PAKPAEK@Z
long c_d3d_resource_allocator::create_cube_texture(unsigned int edge, unsigned int levels,
    unsigned long usage, D3DFORMAT format, unsigned long pool, D3DCubeTexture **out,
    unsigned long *out_size, unsigned char *data, unsigned long flags)
{
 // match: 0x468660: __assume folds user_memory ? 4 : usage into a branchless subfic/subfe select (removal emits cmplwi/bne + r5 usage load)
 __assume(usage==0); long result=0;
 bool user_memory=(flags&1)!=0;
 unsigned long texture_usage=user_memory ? 4 : usage;
 D3DCubeTexture *texture=(D3DCubeTexture *)RtlAllocateHeap(GetProcessHeap(),0,sizeof(D3DCubeTexture));
 unsigned long size=XGSetCubeTextureHeader(edge,levels,texture_usage,format,(D3DPOOL)1,0,-1,texture,0,0);
 if(size>0) {
  bool use_data=(flags&1)!=0;
  void *memory=use_data ? data : allocate_d3d_memory_tracked(size);
  if(memory) {
   XGOffsetResourceAddress(texture,memory);
   if(!use_data && data) memcpy(memory,data,size);
   *out=texture;
  }
 }
 return result;
}
