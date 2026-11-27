#ifndef INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_ITEM_H
#define INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_ITEM_H

#include "bitmaps/bitmap_data.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include <stddef.h>
#include <stdlib.h>

class c_gui_custom_bitmap_storage_item
{
public:
    void unload_rendered_bitmap();
    bool initialize(long width, long height, bool use_compressed_format);

    long m_width;
    long m_height;
    bool m_use_compressed_format;
    bitmap_data m_bitmap;
    bool m_initialized;
    c_rasterizer_texture_ref m_hardware_format_bitmap;
    bool m_bitmap_ready;
#ifdef HALO3_64BIT
    // The UI pool lives below 4 GiB, as required by the Xenos texture addresses.
    // Keep the allocation pointer in its four-byte slot in the datum.
    TAG_POINTER(void) m_requested_size;
#else
    uintptr_t m_requested_size;
#endif
    unsigned long m_allocated_size;
    long m_overhang_size;
};

typedef char gui_bitmap_storage_state_offset[(sizeof(void *) != 4 || offsetof(c_gui_custom_bitmap_storage_item, m_requested_size) == 0x48) ? 1 : -1];
typedef char gui_bitmap_storage_size[(sizeof(void *) != 4 || sizeof(c_gui_custom_bitmap_storage_item) == 0x54) ? 1 : -1];

#endif
