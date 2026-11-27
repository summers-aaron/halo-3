// interfacelibxbox/gui_custom_bitmap_widget.obj: from the records 0x1aaf98 0x491eb0 0x491f30 0x491f68 0x492028 0x492078 0x4920c8 0x492118 0x4923b0 0x492410 0x492460 0x492550 0x492608 0x4926a8 0x4927d8 0x4efff8
// @flags /TP /O2
// @address 0x4927d8 ?set_map_image@c_gui_custom_bitmap_widget@@QAAXW4e_custom_map_image_type@@W4e_map_id@@_N@Z
// @address 0x4efff8 ?unload_rendered_bitmap@c_gui_custom_bitmap_storage_item@@QAAXXZ

#include <stddef.h>
#include "interface/custom_map_image_types.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/custom_map_image_formats.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "memory/static_arrays.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "interface/user_interface_memory.h"
#include "tag_files/files.h"
#include "text/font_cache.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "networking/logic/storage/network_storage_cache.h"
#include "cseries/async.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "cseries/async_helpers.h"
#include "multithreading/synchronized_value.h"
#include "networking/logic/storage/network_storage_bitmap_load_task_types.h"
#include "interface/gui_custom_bitmap_storage_manager.h"
#include "bitmaps/bitmap_data.h"
#include "main/levels_globals.h"
#include "main/levels.h"
#include "interface/core_widget_definition.h"
#include "tag_files/file_reference.h"
#include "cseries/async_block.h"
#include "cseries/compiler_intrinsics.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "interface/gui_custom_bitmap_storage_item.h"

union s_gui_custom_bitmap_widget_load_task
{
    s_async_task storage;
    s_network_storage_bitmap_load_task load;
};

struct s_gui_custom_bitmap_widget_file_load_task
{
    long state;
    s_file_reference *file;
    long offset;
    void *buffer;
    unsigned long buffer_size;
    long storage_index;
    long image_type;
    c_synchronized_long *cancel;
    c_synchronized_long *success;
    bool flag;
};


template<long k_maximum_count> long c_static_string<k_maximum_count>::length() const
{
    const char *string = m_string;
    unsigned long count;
    for (count = 0; count < (unsigned long)k_maximum_count; count++) {
        if (*string++ == '\0')
            break;
    }
    return count;
}
template long c_static_string<256>::length() const;

// @address 0x491eb0 ??0c_gui_custom_bitmap_widget@@QAA@XZ
c_gui_custom_bitmap_widget::c_gui_custom_bitmap_widget() :
    c_gui_bitmap_widget(),
    m_storage_index(-1),
    m_async_load_buffer(0),
    m_async_load_size(0),
    bitmap_index(-1),
    m_async_task_done(0),
    m_async_task_cancel(0),
    m_async_task_success(0)
{
    memset(m_map_image, 0, sizeof(m_map_image));
    memset(m_loaded_map_image, 0, sizeof(m_loaded_map_image));
}

static __forceinline const bitmap_data *gui_custom_bitmap_widget_ready_bitmap(c_gui_custom_bitmap_storage_item *item);

// @address 0x492028 ?get_current_bitmap@c_gui_custom_bitmap_widget@@UBAPBUbitmap_data@@XZ
const bitmap_data *c_gui_custom_bitmap_widget::get_current_bitmap() const
{
    c_gui_custom_bitmap_storage_item *item = gui_custom_bitmap_storage_try_and_get(&g_gui_custom_bitmap_storage_data.data, m_storage_index);
    if (item != 0)
        return gui_custom_bitmap_widget_ready_bitmap(item);
    return 0;
}

// @address 0x492078 ?get_allocated_size@c_gui_custom_bitmap_widget@@QAAJXZ
long c_gui_custom_bitmap_widget::get_allocated_size()
{
    c_gui_custom_bitmap_storage_item *item = gui_custom_bitmap_storage_try_and_get(
        &g_gui_custom_bitmap_storage_data.data, m_storage_index);
    if (item != 0)
        return item->m_allocated_size;
    return 0;
}

// @address 0x4920c8 ?get_overhang_size@c_gui_custom_bitmap_widget@@QAAJXZ
long c_gui_custom_bitmap_widget::get_overhang_size()
{
    c_gui_custom_bitmap_storage_item *item = gui_custom_bitmap_storage_try_and_get(
        &g_gui_custom_bitmap_storage_data.data, m_storage_index);
    if (item != 0)
        return item->m_overhang_size;
    return 0;
}

// @address 0x492608 ?get_dimensions@c_gui_custom_bitmap_widget@@AAA_NPAJ0@Z
// match: 0x492608: __restrict on height keeps li r3,1 ahead of the width/height stores
bool c_gui_custom_bitmap_widget::get_dimensions(long *width, long *__restrict height)
{
    bool result = false;
    s_runtime_core_widget_definition *bounds = get_core_definition();
    if ((bounds->flags & 8) == 0)
    {
        int width_value = (int)(bounds->authored_bounds.x1 - bounds->authored_bounds.x0);
        int height_value = (int)(bounds->authored_bounds.y1 - bounds->authored_bounds.y0);
        if (width_value > 0 && height_value > 0)
        {
            *width = width_value;
            *height = height_value;
            result = true;
        }
    }
    return result;
}

// @address 0x4923b0 ?is_ready@c_gui_custom_bitmap_widget@@QAA_NPA_N@Z
bool c_gui_custom_bitmap_widget::is_ready(bool *loading)
{
    int state = m_type;
    unsigned char visible = m_visible;
    if (loading)
    {
        bool is_loading = state == 0 || state == 1 || state == 2;
        *loading = is_loading;
    }
    return visible && bitmap_index == -1;
}

// match: 0x492550: signed handle preserves the separate invalid-handle check.
static __forceinline void gui_custom_bitmap_widget_add_reference(int handle)
{
    internal_critical_section_enter(20);
    if (handle != -1)
    {
        s_data_array *array = g_gui_custom_bitmap_storage_data.data;
        if ((handle & 0xffff) < array->first_unallocated)
        {
            gui_custom_bitmap_storage_slot *datum =
                (gui_custom_bitmap_storage_slot *)(array->data + array->element_size * (handle & 0xffff));
            if (datum->identifier != 0 && datum->identifier == handle >> 16)
                datum->reference_count = datum->reference_count + 1;
        }
    }
    internal_critical_section_leave(20);
}

// @address 0x491f30 ?dispose@c_gui_custom_bitmap_widget@@UAAXXZ
void c_gui_custom_bitmap_widget::dispose()
{
    c_gui_widget::dispose();
    reset();
}

// @address 0x491f68 ?reset@c_gui_custom_bitmap_widget@@AAAXXZ
void c_gui_custom_bitmap_widget::reset()
{
    if (bitmap_index != -1)
    {
        m_async_task_cancel.set(1);
        async_block_until_done(&m_async_task_done, simple_yield_function, true, false, 0, 0);
        bitmap_index = -1;
    }
    if (m_storage_index != -1)
    {
        gui_custom_bitmap_storage_release(&g_gui_custom_bitmap_storage_data.data, m_storage_index);
        m_storage_index = -1;
    }
    if (m_async_load_buffer)
    {
        user_interface_free(m_async_load_buffer);
        m_async_load_buffer = 0;
    }
}

// @address 0x492460 gui_custom_bitmap_widget_load_bitmap_from_buffer
extern "C" unsigned char gui_custom_bitmap_widget_load_bitmap_from_buffer(
    c_gui_custom_bitmap_widget *widget, bool use_compressed_format, const char *buffer, long size, long aspect_index)
{
    bool success = false;
    widget->reset();
    long width;
    long height;
    if (widget->get_dimensions(&width, &height))
    {
        widget->m_storage_index =
            g_gui_custom_bitmap_storage_data.manager.allocate_bitmap(
                width, height, use_compressed_format);
        if (widget->m_storage_index != -1)
        {
            s_gui_custom_bitmap_widget_load_task task;
            task.load.image_data = buffer;
            task.load.image_size = size;
            task.load.bitmap_index = widget->m_storage_index;
            task.load.aspect_index = aspect_index;
            task.load.success = &widget->m_async_task_success;
            widget->m_async_task_success.set(0);
            widget->bitmap_index = async_task_add_ex(_async_priority_blocking_generic, &task.storage, static_cast<e_async_category>(9),
                reinterpret_cast<e_async_completion (*)(s_async_task *)>(network_storage_cache_handle_bitmap_load_task), &widget->m_async_task_done, true);
            success = widget->bitmap_index != -1;
        }
    }
    if (!success)
        widget->reset();
    return success;
}

// @address 0x492550 ?share_bitmap@c_gui_custom_bitmap_widget@@QAAHPAU1@@Z
int c_gui_custom_bitmap_widget::share_bitmap(c_gui_custom_bitmap_widget *other)
{
    reset();
    unsigned long handle = other->m_storage_index;
    if (handle != 0xffffffff)
    {
        m_storage_index = handle;
        gui_custom_bitmap_widget_add_reference((int)handle);
    }
    return 0;
}

// @rdata 0x8203e2b4..0x8203e2bc
// @address 0x3e2b4 k_custom_map_image_filename_format
extern "C" {
const char k_custom_map_image_filename_format[8] = "%s.blf";
}
// @rdata-end

// @address 0x4926a8 gui_custom_bitmap_widget_get_map_image_path
extern "C" bool gui_custom_bitmap_widget_get_map_image_path(
    e_custom_map_image_type type, int map_id, c_static_string<256> *path)
{
    bool result = false;
    if (map_id != -1)
    {
        s_datum definition;
        c_static_string<256> name;
        name.m_string[0] = '\0';
        if (levels_try_and_get_datum(g_level_globals.map_levels, map_id, &definition) ||
            levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, &definition))
        {
            strncpy(name.m_string, definition.image_file_base, sizeof(name.m_string));
            name.m_string[sizeof(name.m_string) - 1] = '\0';
        }
        if (name.length() > 0)
        {
            switch (type)
            {
            case _custom_map_image_type_normal:
                path->print(k_custom_map_image_filename_format, name.m_string);
                break;
            case _custom_map_image_type_small:
                path->print("%s_sm.blf", name.m_string);
                break;
            case _custom_map_image_type_variant:
                path->print("%s_variant.blf", name.m_string);
                break;
            case _custom_map_image_type_film:
                path->print("%s_film.blf", name.m_string);
                break;
            case _custom_map_image_type_film_clip:
                path->print("%s_clip.blf", name.m_string);
                break;
            }
            result = path->length() > 0;
        }
    }
    return result;
}

void c_gui_custom_bitmap_widget::set_map_image(e_custom_map_image_type type, e_map_id map_id, bool loading)
{
    bool loaded = false;
    c_static_string<256> path;
    path.m_string[0] = '\0';
    if (gui_custom_bitmap_widget_get_map_image_path(type, map_id, &path))
    {
        strncpy(m_map_image, path.m_string, sizeof(m_map_image));
        loaded = true;
        m_loading = loading;
        m_task_id = 0;
    }
    if (!loaded)
        memset(m_map_image, 0, sizeof(m_map_image));
}

void c_gui_custom_bitmap_storage_item::unload_rendered_bitmap()
{
    internal_critical_section_enter(20);
    if (m_hardware_format_bitmap.valid())
    {
        c_rasterizer_texture_ref::release(m_hardware_format_bitmap);
        m_bitmap.texture_ref = m_hardware_format_bitmap.get_datum_ref();
    }
    m_bitmap_ready = false;
    internal_critical_section_leave(20);
}

static __forceinline const bitmap_data *gui_custom_bitmap_widget_ready_bitmap(c_gui_custom_bitmap_storage_item *item)
{
    if (item->m_bitmap_ready)
        return &item->m_bitmap;
    return 0;
}

// @address 0x492410 ?assemble_render_data@c_gui_custom_bitmap_widget@@UAAXPAUs_gui_widget_render_data@@PBTrectangle2d@@W4e_controller_index@@_N33@Z
void c_gui_custom_bitmap_widget::assemble_render_data(s_gui_widget_render_data *render_data, const rectangle2d *window_bounds,
    e_controller_index local_controller_index, bool a, bool b, bool c)
{
    c_gui_bitmap_widget::assemble_render_data(render_data, window_bounds, local_controller_index, a, b, c);
    render_data->flags |= 1;
    static_cast<s_gui_bitmap_widget_render_data *>(render_data)->custom_storage_index = m_storage_index;
}

// @address 0x492118 ?update@c_gui_custom_bitmap_widget@@UAAXK@Z
void c_gui_custom_bitmap_widget::update(unsigned long current_milliseconds)
{
    c_gui_widget::update(current_milliseconds);
    if (bitmap_index == -1)
    {
        if (strncmp(m_map_image, m_loaded_map_image, sizeof(m_map_image)) != 0)
        {
            reset();
            strncpy(m_loaded_map_image, m_map_image, sizeof(m_loaded_map_image));
            if (m_loaded_map_image[0] != '\0')
            {
                long width;
                long height;
                if (get_dimensions(&width, &height))
                {
                    bool started = false;
                    m_storage_index =
                        g_gui_custom_bitmap_storage_data.manager.allocate_bitmap(
                            width, height, m_loading);
                    if (m_storage_index != -1)
                    {
                        m_async_load_buffer = user_interface_malloc_tracked(0xc000, 0, 0);
                        if (m_async_load_buffer)
                        {
                            union
                            {
                                s_async_task storage;
                                s_gui_custom_bitmap_widget_file_load_task file_load;
                            } task;
                            task.file_load.state = 0;
                            task.file_load.file = &m_file_reference;
                            file_reference_create_from_path(&m_file_reference, m_loaded_map_image, false);
                            task.file_load.offset = 0;
                            task.file_load.storage_index = m_storage_index;
                            task.file_load.buffer_size = 0xc000;
                            task.file_load.buffer = m_async_load_buffer;
                            task.file_load.cancel = &m_async_task_cancel;
                            task.file_load.success = &m_async_task_success;
                            __lwsync();
                            _InterlockedExchange(&task.file_load.success->value, 0);
                            task.file_load.image_type = m_task_id;
                            task.file_load.flag = false;
                            m_async_task_cancel.set(0);
                            bitmap_index = async_task_add_ex(_async_priority_blocking_generic, &task.storage, static_cast<e_async_category>(9),
                                reinterpret_cast<e_async_completion (*)(s_async_task *)>(network_storage_cache_load_bitmap_file), &m_async_task_done, true);
                            started = bitmap_index != -1;
                        }
                    }
                    else
                    {
                        m_loaded_map_image[0] = '\0';
                    }
                    if (!started)
                    {
                        unsigned int handle = m_storage_index;
                        if (handle != -1)
                        {
                            internal_critical_section_enter(20);
                            // match: 0x492118: the int copy keeps the second invalid-handle compare.
                            int storage_handle = (int)handle;
                            if (storage_handle != -1)
                            {
                                s_data_array *array = g_gui_custom_bitmap_storage_data.data;
                                if ((storage_handle & 0xffff) < array->first_unallocated)
                                {
                                    gui_custom_bitmap_storage_slot *datum =
                                        (gui_custom_bitmap_storage_slot *)(array->data + array->element_size * (storage_handle & 0xffff));
                                    if (datum->identifier != 0 && datum->identifier == storage_handle >> 16)
                                        datum->item.unload_rendered_bitmap();
                                }
                            }
                            internal_critical_section_leave(20);
                            m_storage_index = -1;
                        }
                        reset();
                    }
                }
            }
        }
    }
    else if (m_async_task_done.peek())
    {
        if (m_async_task_success.peek())
        {
            if (m_async_load_buffer)
            {
                user_interface_free(m_async_load_buffer);
                m_async_load_buffer = 0;
                m_async_load_size = 0;
                start_animation_at_time(_animation_state_custom0, 0xffffffff, true);
            }
        }
        else
        {
            reset();
        }
        bitmap_index = -1;
    }
}

