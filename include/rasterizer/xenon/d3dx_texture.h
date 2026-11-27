#ifndef RASTERIZER_XENON_D3DX_TEXTURE_H
#define RASTERIZER_XENON_D3DX_TEXTURE_H

struct s_gui_custom_bitmap_storage_xenon_rect;
struct s_gui_custom_bitmap_storage_xenon_surface_desc
{
    unsigned long format;
    unsigned long type;
    unsigned long usage;
    unsigned long pool;
    unsigned long multisample_type;
    unsigned long multisample_quality;
    unsigned long width;
    unsigned long height;
};

struct s_gui_custom_bitmap_storage_xenon_image_info
{
    unsigned long width;
    unsigned long height;
    unsigned long depth;
    unsigned long mip_levels;
    unsigned long format;
    unsigned long resource_type;
    unsigned long image_file_format;
};

struct ID3DXBuffer;
extern "C" void D3DResource_BlockUntilNotBusy(void *resource);
extern "C" long D3DXSaveSurfaceToFileInMemory(ID3DXBuffer **dest_buffer, unsigned long format, void *src_surface, void *palette, void *rect);
extern "C" void D3DSurface_GetDesc(void *surface, s_gui_custom_bitmap_storage_xenon_surface_desc *desc);
extern "C" long D3DXGetImageInfoFromFileInMemory(const void *data, unsigned long size, s_gui_custom_bitmap_storage_xenon_image_info *info);

extern "C" long D3DXLoadSurfaceFromFileInMemory(void *dest_surface, void *dest_palette,
    s_gui_custom_bitmap_storage_xenon_rect *dest_rect, const void *src_data,
    unsigned long src_size, void *src_rect, unsigned long filter, unsigned long color_key,
    s_gui_custom_bitmap_storage_xenon_image_info *src_info);

#endif
