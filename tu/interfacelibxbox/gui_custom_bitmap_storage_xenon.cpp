// interfacelibxbox/gui_custom_bitmap_storage_xenon.obj: from the records 0x4e0be0 0x4ef838 0x4ef958 0x4ef9c8 0x4efd98 0x4eff20
// @flags /TP /O2 /Oc
// @address 0x86d80 XGSetTextureHeader
// @address 0x5a7618 D3DResource_Release
#include <stddef.h>
#include "shell/shell_xenon.h"
#include "bitmaps/bitmap_data.h"
// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x847d0 ?internal_critical_section_leave@@YAXJ@Z
// @address 0x4ef838 ?initialize@c_gui_custom_bitmap_storage_item@@QAA_NJJ_N@Z
// @address 0x4efff8 ?unload_rendered_bitmap@c_gui_custom_bitmap_storage_item@@QAAXXZ
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x16b798 D3DTexture_LockRect
// @address 0x4ef9c8 gui_custom_bitmap_storage_decode_image
#include <string.h>
#include "multithreading/synchronization.h"
#include "interface/user_interface_memory.h"
#include "text/font_cache.h"
#include "interface/gui_custom_bitmap_storage_xenon.h"
#include "rasterizer/rasterizer.h"
#include "interface/screenshot_capture_storage_types.h"
#include "interface/gui_screens/director/gui_screen_take_screenshot.h"
#include "bitmaps/bitmaps.h"

#include "rasterizer/xenon/rasterizer_xenon_resource.h"

enum
{
    k_gui_custom_bitmap_storage_xenon_critical_section = 0x14
};

#include "interface/gui_custom_bitmap_storage_item.h"

struct s_gui_custom_bitmap_storage_xenon : c_gui_custom_bitmap_storage_item
{
    char m_texture_header[0x34];
};

#ifdef HALO3_64BIT
static_assert(offsetof(s_gui_custom_bitmap_storage_xenon, m_texture_header) == sizeof(c_gui_custom_bitmap_storage_item),
    "the D3D header must immediately follow the storage item");
static_assert(sizeof(s_gui_custom_bitmap_storage_xenon) == 0x94 - 0xc,
    "the storage item and D3D header must fit after the UI datum's 12-byte prefix");
#endif

struct s_gui_custom_bitmap_storage_xenon_rect
{
    long left;
    long top;
    long right;
    long bottom;
};

struct D3DTexture;
struct tagRECT;
#include "rasterizer/xenon/rasterizer_xenon_d3d.h"
#include "rasterizer/xenon/d3dx_texture.h"
//  0x5b4a68 D3DXLoadSurfaceFromFileInMemory

struct ID3DXBuffer
{
    virtual long QueryInterface(void *riid, void **ppvObj) = 0;
    virtual unsigned long AddRef() = 0;
    virtual unsigned long Release() = 0;
    virtual void *GetBufferPointer() = 0;
    virtual unsigned long GetBufferSize() = 0;
};




// @address 0x5a3ae0 D3DResource_BlockUntilNotBusy
// @address 0x5b4a60 D3DXSaveSurfaceToFileInMemory
// @address 0x5a4d88 D3DSurface_GetDesc
// @address 0x5b39b0 D3DXGetImageInfoFromFileInMemory

#include "rasterizer/xenon/xg_texture.h"

// @address 0x4e0be0 gui_custom_bitmap_storage_capture_screenshot
extern "C" void gui_custom_bitmap_storage_capture_screenshot(void)
{
    s_gui_custom_bitmap_storage_xenon storage;

    storage.m_width = 0;
    storage.m_height = 0;
    storage.m_use_compressed_format = false;
    storage.m_initialized = false;
    storage.m_bitmap_ready = false;
    storage.m_requested_size = 0;
    storage.m_allocated_size = 0;
    storage.m_overhang_size = 0;
    storage.m_bitmap.texture_ref = -1;
    storage.m_hardware_format_bitmap.clear();

    long data_trailing = g_screenshot_capture_storage.texture_size;
    short width = (short)g_screenshot_capture_storage.width;
    long data_address = g_screenshot_capture_storage.buffer;
    long aligned_width = (width + 63) & ~63;
    long height = (short)g_screenshot_capture_storage.height;

    internal_critical_section_enter(k_gui_custom_bitmap_storage_xenon_critical_section);

    unsigned long format = c_rasterizer::bitmap_format_to_hardware_format(11, false, false, false);

    storage.m_use_compressed_format = false;

    if (XGSetTextureHeader(aligned_width, height, 1, 4, format, 1, 0, -1, 0, storage.m_texture_header, 0, 0) != 0)
    {
        gui_custom_bitmap_storage_set_buffer(&storage, aligned_width, height, data_address, data_trailing);
        storage.m_bitmap_ready = true;
    }

    internal_critical_section_leave(k_gui_custom_bitmap_storage_xenon_critical_section);

    char *destination = g_screenshot_capture_storage.scratch_buffer + 328;
    long limit = g_screenshot_capture_storage.secondary_size;
    void *base = g_screenshot_capture_storage.secondary_buffer;
    unsigned long output_size = g_screenshot_capture_storage.scratch_size;

    internal_critical_section_enter(k_gui_custom_bitmap_storage_xenon_critical_section);

    bool result = gui_custom_bitmap_storage_encode_image(&storage, 1, 0, destination, output_size, base, limit, &g_screenshot_capture_storage.data_size);

    internal_critical_section_leave(k_gui_custom_bitmap_storage_xenon_critical_section);

    g_screenshot_capture_storage.valid = result;

    storage.unload_rendered_bitmap();
}

bool c_gui_custom_bitmap_storage_item::initialize(long width, long height, bool use_compressed_format)
{
    s_gui_custom_bitmap_storage_xenon *storage = static_cast<s_gui_custom_bitmap_storage_xenon *>(this);

    internal_critical_section_enter(k_gui_custom_bitmap_storage_xenon_critical_section);

    int format = ((use_compressed_format ? -1 : 0) & 5) + 11;
    bool result = false;
    unsigned long hardware_format = c_rasterizer::bitmap_format_to_hardware_format((short)format, true, false, false);

    m_use_compressed_format = use_compressed_format;

    bitmap_2d_initialize(&m_bitmap, (short)width, (short)height, 0, (short)format, 48, 1, 0);
    m_bitmap.curve = 3;

    unsigned int texture_size = XGSetTextureHeader((short)m_bitmap.width, (short)m_bitmap.height, 1, 4, hardware_format, 1, 0, -1, 0,
        storage->m_texture_header, 0, 0);

    if (texture_size != 0)
    {
        unsigned int allocation_size = texture_size + 0x1000;
        uintptr_t data_address = reinterpret_cast<uintptr_t>(user_interface_malloc_tracked(allocation_size, 0, 0));

#ifdef HALO3_64BIT
        m_requested_size = reinterpret_cast<void *>(data_address);
#else
        m_requested_size = data_address;
#endif

        if (data_address != 0)
        {
            gui_custom_bitmap_storage_set_buffer(storage, width, height, data_address, allocation_size);
            result = true;
        }
    }

    internal_critical_section_leave(k_gui_custom_bitmap_storage_xenon_critical_section);

    return result;
}

// @address 0x4ef958 gui_custom_bitmap_storage_set_buffer
extern "C" void gui_custom_bitmap_storage_set_buffer(
    s_gui_custom_bitmap_storage_xenon *storage, unsigned long width, unsigned long height, uintptr_t data_address, long trailing_size)
{
    unsigned long allocated_size;

    internal_critical_section_enter(k_gui_custom_bitmap_storage_xenon_critical_section);
#ifdef HALO3_64BIT
    storage->m_requested_size = reinterpret_cast<void *>(data_address);
#else
    storage->m_requested_size = data_address;
#endif
    storage->m_width = width;
    allocated_size = (data_address + 0xfff) & 0xfffff000;
    storage->m_height = height;
    storage->m_allocated_size = allocated_size;
    storage->m_overhang_size = (data_address - allocated_size) + trailing_size;
    XGOffsetResourceAddress(storage->m_texture_header, reinterpret_cast<void *>(allocated_size));
    storage->m_initialized = true;
    internal_critical_section_leave(k_gui_custom_bitmap_storage_xenon_critical_section);
}

extern "C" bool gui_custom_bitmap_storage_decode_image(s_gui_custom_bitmap_storage_xenon *storage, long unused, const void *image_data,
    unsigned long image_size, void *base, long limit, long aspect_index)
{
    bool success = false;

    if (storage->m_initialized)
    {
        storage->m_hardware_format_bitmap = c_rasterizer_texture_ref((D3DBaseTexture *)storage->m_texture_header);
        if (storage->m_hardware_format_bitmap.valid())
        {
            // match: 0x4ef9c8: retain the texture-reference assignment boundary for instruction scheduling.
            (c_rasterizer_texture_ref &)storage->m_bitmap.texture_ref = c_rasterizer_texture_ref(storage->m_hardware_format_bitmap);
            storage->m_bitmap.flags |= 128;

            if (base != 0)
            {
                g_shell_allocator_hook.allocator_id = 129;
                g_shell_allocator_hook.base = reinterpret_cast<uintptr_t>(base);
                g_shell_allocator_hook.limit = limit;
                g_shell_allocator_hook.used = 0;
                g_shell_allocator_hook.enabled = 1;
            }

            void *surface = D3DTexture_GetSurfaceLevel((D3DTexture *)storage->m_texture_header, 0);
            if (surface != 0)
            {
                float aspect_ratios[3] = { 0.0f, 1.77777f, 1.77777f };
                s_gui_custom_bitmap_storage_xenon_surface_desc desc;
                D3DSurface_GetDesc(surface, &desc);

                s_gui_custom_bitmap_storage_xenon_rect rect;
                rect.left = rect.top = 0;
                rect.right = desc.width;
                rect.bottom = desc.height;

                _D3DLOCKED_RECT locked;
                float target_aspect = aspect_ratios[aspect_index];
                s_gui_custom_bitmap_storage_xenon_image_info info;

                if (target_aspect > 0.0f && D3DXGetImageInfoFromFileInMemory(image_data, image_size, &info) >= 0)
                {
                    unsigned long image_height = info.height;
                    if (image_height <= 1)
                        image_height = 1;
                    float image_aspect = (float)info.width / (float)image_height;

                    if (image_aspect > 0.0f)
                    {
                        bool needs_clear = false;

                        if (image_aspect > target_aspect)
                        {
                            float ratio = 1.0f - target_aspect / image_aspect;
                            // match: 0x4ef958: keeps crop = fmuls fr0,fr13,fr0 (dimension first); without it the operands swap
                            __assume(ratio > 0.0f);
                            float crop = (float)desc.height * ratio;
                            rect.top = (rect.top - (long)(crop * -0.5f) + 3) & ~3;
                            rect.bottom = (rect.bottom - (long)(crop * 0.5f)) & ~3;
                            needs_clear = true;
                        }
                        else if (image_aspect < target_aspect)
                        {
                            float ratio = 1.0f - image_aspect / target_aspect;
                            // match: 0x4ef958: keeps crop = fmuls fr0,fr13,fr0 (dimension first); without it the operands swap
                            __assume(ratio > 0.0f);
                            float crop = (float)desc.width * ratio;
                            rect.left = (rect.left - (long)(crop * -0.5f) + 3) & ~3;
                            rect.right = (rect.right - (long)(crop * 0.5f)) & ~3;
                            needs_clear = true;
                        }

                        if (needs_clear)
                        {
                            D3DTexture_LockRect((D3DTexture *)storage->m_texture_header, 0, &locked, 0, 0);
                            if (locked.pBits != 0)
                            {
                                memset(locked.pBits, 0, storage->m_overhang_size);
                                D3DTexture_UnlockRect((D3DTexture *)storage->m_texture_header, 0);
                            }
                        }
                    }
                }

                long result = D3DXLoadSurfaceFromFileInMemory(surface, 0, &rect, image_data, image_size, 0, -1, 0, &info);
                D3DResource_Release(surface);

                if (base != 0)
                {
                    g_shell_allocator_hook.allocator_id = 129;
                    g_shell_allocator_hook.base = 0;
                    g_shell_allocator_hook.limit = 0;
                    g_shell_allocator_hook.used = 0;
                    g_shell_allocator_hook.enabled = 0;
                }

                if (result >= 0)
                {
                    int format = ((storage->m_use_compressed_format ? -1 : 0) & 5) + 11;
                    unsigned long hardware_format = c_rasterizer::bitmap_format_to_hardware_format((short)format, true, true, false);
                    XGSetTextureHeader((short)storage->m_bitmap.width, (short)storage->m_bitmap.height, 1, 4, hardware_format, 1, 0, -1, 0,
                        storage->m_texture_header, 0, 0);
                    XGOffsetResourceAddress(storage->m_texture_header, reinterpret_cast<void *>(storage->m_allocated_size));
                    storage->m_bitmap_ready = true;
                    success = true;
                }
            }
        }
    }

    return success;
}

// @address 0x4efd98 gui_custom_bitmap_storage_encode_image
extern "C" bool gui_custom_bitmap_storage_encode_image(
    s_gui_custom_bitmap_storage_xenon *storage,
    long,
    long,
    void *output_destination,
    unsigned long output_size,
    void *base,
    long limit,
    unsigned long *out_size)
{
    internal_critical_section_enter(k_gui_custom_bitmap_storage_xenon_critical_section);

    bool result = false;

    if (storage->m_initialized && storage->m_bitmap_ready)
    {
        g_shell_allocator_hook.allocator_id = 129;
        g_shell_allocator_hook.base = reinterpret_cast<uintptr_t>(base);
        g_shell_allocator_hook.limit = limit;
        g_shell_allocator_hook.used = 0;
        g_shell_allocator_hook.enabled = 1;

        void *surface = D3DTexture_GetSurfaceLevel((D3DTexture *)storage->m_texture_header, 0);
        if (surface != 0)
        {
            ID3DXBuffer *buffer = 0;

            if (out_size != 0)
            {
                *out_size = 0;
            }

            if (D3DXSaveSurfaceToFileInMemory(&buffer, 1, surface, 0, 0) >= 0)
            {
                if (out_size != 0)
                {
                    *out_size = buffer->GetBufferSize();
                }

                if (output_size >= buffer->GetBufferSize())
                {
                    memcpy(output_destination, buffer->GetBufferPointer(), buffer->GetBufferSize());
                    result = true;
                }

                buffer->Release();
            }

            D3DResource_Release(surface);

            g_shell_allocator_hook.allocator_id = 129;
            g_shell_allocator_hook.base = 0;
            g_shell_allocator_hook.limit = 0;
            g_shell_allocator_hook.used = 0;
            g_shell_allocator_hook.enabled = 0;
        }
    }

    internal_critical_section_leave(k_gui_custom_bitmap_storage_xenon_critical_section);

    return result;
}


// @address 0x4eff20 gui_custom_bitmap_storage_release_texture
extern "C" void gui_custom_bitmap_storage_release_texture(c_gui_custom_bitmap_storage_item *storage)
{
    internal_critical_section_enter(k_gui_custom_bitmap_storage_xenon_critical_section);

    if (storage->m_hardware_format_bitmap.valid())
    {
        D3DResource_BlockUntilNotBusy(storage->m_hardware_format_bitmap.get_d3d_texture());
        c_rasterizer_texture_ref::release(storage->m_hardware_format_bitmap);
        (c_rasterizer_texture_ref &)storage->m_bitmap.texture_ref = storage->m_hardware_format_bitmap;
    }

    storage->m_bitmap_ready = false;

    internal_critical_section_leave(k_gui_custom_bitmap_storage_xenon_critical_section);
}

typedef char gui_bitmap_allocator_id_offset[(offsetof(s_allocator_hook, allocator_id) == 4) ? 1 : -1];
typedef char gui_bitmap_allocator_size[(sizeof(void *) != 4 || ((sizeof(s_allocator_hook) == 0x14) )) ? 1 : -1];

typedef char bitmap_storage_xenon_texture_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_custom_bitmap_storage_item, m_hardware_format_bitmap) == 0x40))) ? 1 : -1];
typedef char bitmap_storage_xenon_request_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_custom_bitmap_storage_item, m_requested_size) == 0x48))) ? 1 : -1];
typedef char bitmap_storage_xenon_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_custom_bitmap_storage_item) == 0x54))) ? 1 : -1];
