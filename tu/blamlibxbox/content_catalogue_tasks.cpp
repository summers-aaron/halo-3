// blamlibxbox/content_catalogue_tasks.obj: from the records 0x2f6f88 0x2f6f98 0x2f7008 0x2f71d8 0x2f7258 0x2f72a8 0x2f7330 0x2f7450 0x2f75e8 0x2f76e0 0x2f76e8 0x2f7990 0x2f79a0 0x5808b0 0x580910 0x5809b0 0x5809c0 0x580ac8 0x580b30 0x580b80 0x580bf8
// @flags /TP /O2 /Oc
#include "saved_games/content_catalogue_types.h"
#include "cseries/atomic_intrinsics.h"
#include "shell/shell.h"
#include "main/levels_globals.h"
// @address 0x2f7990 ??1exception@std@@UAA@XZ
// @address 0x2f79a0 ?_Doraise@bad_alloc@std@@MBAXXZ
// @address 0x5808b0 ??_Gc_marketplace_asset_enumeration_overlapped_task@@UAAPAXI@Z
// @address 0x1ac700 ?set_bit@c_synchronized_long@@QAAJJ_N@Z
// @address 0x2f6348 ?flush@s_content_item@@IAA_N_N@Z
// @address 0x2f6640 ?close@s_content_item@@QAAX_N@Z
// @address 0x2f6ac8 ?get_container_full_path@s_content_item@@QBAPBDPADJ_N@Z
// @address 0x285770 ?dispose_content_item_array@c_content_catalogue@@IAAXXZ
// @address 0x2838b8 ?dispose@c_content_catalogue@@QAAXXZ
// @address 0x285260 ?content_enumeration_success@c_content_catalogue@@IAAXPAU_XCONTENT_DATA@@JJ@Z
// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z
// @address 0x20e770 ?overlapped_task_block_until_finished@@YAXPAVc_overlapped_task@@@Z
// @address 0x20e4e8 ?reset@c_overlapped_task@@UAAXXZ
// @address 0x59fbf8 XEnumerate
// @address 0x59fbd0 XContentDelete
// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x5bec98 ?complete@c_overlapped_task@@UAAXXZ
// @address 0x5bec98 ?success@c_overlapped_task@@UAAXK@Z
// @address 0x20e550 ?is_result_successful@c_overlapped_task@@UAA_NKKK@Z
// @address 0x3a4d80 ?complete@c_marketplace_asset_enumeration_overlapped_task@@UAAXXZ
// @address 0x3d26d8 ?reset@c_marketplace_asset_enumeration_overlapped_task@@UAAXXZ
// @address 0x5bec98 ?update@c_overlapped_task@@UAAXK@Z
// @address 0x5bec98 ?failure@c_overlapped_task@@UAAXKKK@Z
// @address 0x8af98 ?is_signed_in_to_live@c_controller_interface@@QBA_NXZ
// @address 0x8a4c8 ?is_signed_in_to_machine@c_controller_interface@@QBA_NXZ
#include <ppcintrinsics.h>
#include <stdlib.h>
#include <new>
#include <string.h>
#include "multithreading/synchronization.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "saved_games/content_item_task_types.h"
#include "saved_games/marketplace_asset_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "cseries/crt_xdk.h"
#include "multithreading/synchronization_types.h"

extern "C" {

#include "xbox/online_services.h"
#include "xbox/content_api.h"




}
struct s_synchronization_globals;

#include "interface/controller_types.h"
#include "interface/controller_inline.h"

#include "cseries/async_xoverlapped_types.h"
#include "multithreading/synchronized_value.h"


class c_content_enumeration_overlapped_task;
struct _XCONTENT_DATA;

#include "saved_games/content_item_inline.h"



#include "saved_games/marketplace_asset_task_types.h"
#include "interface/marketplace_consume_assets_task.h"

struct s_content_item_array {
    unsigned __int64 count;
    s_content_item items[151];
};

long c_content_item_overlapped_task::start_content_delete(void *overlapped)
{
    s_content_item *item = m_content_item;
    long controller_index = item->get_controller_index();
    if (controller_get((e_controller_index)controller_index)->is_signed_in_to_machine())
        return XContentDelete(controller_index, &item->content_data, overlapped);
    return 0x80004005UL;
}
long c_content_item_overlapped_task::start_content_flush(void *overlapped)
{
    char container_path[256];
    m_content_item->get_container_full_path(container_path, 256, false);
    long controller_index = m_content_item->get_controller_index();
    if (controller_get((e_controller_index)controller_index)->is_signed_in_to_machine())
        return XContentFlush(container_path, overlapped);
    return 0x80004005UL;
}
long c_content_item_overlapped_task::start_content_close(void *overlapped)
{
    char container_path[256];
    s_content_item *item = m_content_item;
    long controller_index = item->get_controller_index();
    if (controller_get((e_controller_index)controller_index)->is_signed_in_to_machine())
        return XContentClose(item->get_container_full_path(container_path, 256, false), overlapped);
    return 0x80004005UL;
}

// @address 0x2f7008 ?start@c_content_item_overlapped_task@@UAAKPAX@Z
unsigned long c_content_item_overlapped_task::start(void *overlapped)
{
    unsigned long result;
    internal_critical_section_enter(21);
    switch (m_task_type) {
    case _content_item_task_type_create:
        result = start_content_create(overlapped);
        break;
    case _content_item_task_type_delete:
        result = start_content_delete(overlapped);
        break;
    case _content_item_task_type_flush:
        result = start_content_flush(overlapped);
        break;
    case _content_item_task_type_close:
        result = start_content_close(overlapped);
        break;
    default:
        result = -1;
        break;
    }
    _InterlockedDecrement(&g_synch_globals.critical_sections[21].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[21].critical_section);
    return result;
}
// @address 0x2f71d8 ?success@c_content_item_overlapped_task@@UAAXK@Z
void c_content_item_overlapped_task::success(unsigned long return_result)
{
    if (m_task_type != _content_item_task_type_create) {
        if (m_task_type == _content_item_task_type_close)
            m_content_item->flags.set_bit(2, false);
    } else {
        m_content_item->flags.set_bit(0, true);
        m_content_item->flags.set_bit(2, true);
        if (return_result == 0x570)
            m_content_item->flags.set_bit(1, true);
    }
}
// @address 0x2f7258 ?failure@c_content_item_overlapped_task@@UAAXKKK@Z
void c_content_item_overlapped_task::failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error)
{
    s_content_item *content_item = m_content_item;
    if (content_item == 0)
        return;
    switch ((int)m_task_type) {
    case _content_item_task_type_create:
        return;
    case _content_item_task_type_delete:
        return;
    case _content_item_task_type_flush:
        content_item->flush(true);
        return;
    case _content_item_task_type_close:
        content_item->close(true);
        return;
    default:
        // match: 0x2f7258: unreachable hint omits dead branch check
        __assume(0);
    }
}

// @address 0x2f72a8 ?complete@c_content_item_overlapped_task@@UAAXXZ
void c_content_item_overlapped_task::complete()
{
    s_content_item *content_item = m_content_item;
    content_item->flags.set_bit(3, false);
    if (m_task_type == _content_item_task_type_delete)
        content_item->flags.set(0);
    m_task_type = _content_item_task_type_none;
    m_content_item = 0;
    reset();
}

// @address 0x2f7330 ?start_content_create@c_content_item_overlapped_task@@AAAKPAX@Z
unsigned long c_content_item_overlapped_task::start_content_create(void *overlapped)
{
    unsigned long creation_flags = m_create_disposition;
    long maximum_content_size = m_flush_completed;
    s_content_item *content_item = m_content_item;
    unsigned long content_flags = ((creation_flags & 1) << 1) | ((creation_flags & 0x1c) << 2) | ((creation_flags & 2) != 0 ? 3 : 0);
    if (maximum_content_size < 0)
        maximum_content_size = 0;
    unsigned long content_size[2];
    content_size[0] = 0;
    content_size[1] = maximum_content_size;
    char container_path[256];
    content_item->get_container_full_path(container_path, 256, false);
    long controller_index = content_item->get_controller_index();
    if (controller_get((e_controller_index)controller_index)->is_signed_in_to_machine()) {
        unsigned long result = XContentCreateEx(content_item->get_controller_index(), container_path, &content_item->content_data,
            content_flags, 0, 0, 0, *(unsigned __int64 *)content_size, overlapped);
        if (result == 0x570)
            m_content_item->flags.set_bit(1, true);
        return result;
    }
    return 0x80004005UL;
}
// @address 0x2f7450 ?start@c_content_enumeration_overlapped_task@@UAAKPAX@Z
unsigned long c_content_enumeration_overlapped_task::start(void *overlapped)
{
    int result;
    result = 1223;
    int remaining_count;
    long *content_count;
    content_count = &m_content_count;
    int other_content_count;
    other_content_count = m_other_content_count;
    remaining_count = 151 - other_content_count - *content_count;
    if (remaining_count > 0) {
        bool is_marketplace_content;
        is_marketplace_content = (unsigned long)m_content_type == 1;
        int content_type;
        if ((unsigned long)m_content_type >= 1) {
            content_type = 2;
            if (!is_marketplace_content)
                content_type = 1;
        } else {
            content_type = 1;
        }
        if ((unsigned char)controller_get((e_controller_index)m_controller_index)->is_signed_in_to_machine() != 0) {
            void **enumeration_handle = &m_enumeration_handle;
            int buffer_size[2];
            int enumerator_result;
            enumerator_result = XContentCreateEnumerator(m_controller_index, m_device_id, content_type, 0,
                remaining_count, (unsigned long *)&buffer_size, enumeration_handle);
            result = enumerator_result;
            if ((unsigned int)enumerator_result == 0) {
                int enumeration_size;
                enumeration_size = remaining_count * sizeof(_XCONTENT_DATA);
                if (m_content_data == 0 && m_content_type == 0) {
                    s_content_item_array *array = (s_content_item_array *)::overlapped_malloc_tracked(
                        sizeof(s_content_item_array), 0, 0);
                    s_content_item *items = (s_content_item *)array;
                    if (items != 0) {
                        int item_count;
                        item_count = 151;
                        s_content_item_array *header = (s_content_item_array *)items;
                        header->count = item_count;
                        items = header->items;
                        s_content_item *item = items;
                        do {
                            new (item) s_content_item;
                            item_count = item_count - 1;
                            ++item;
                        } while ((unsigned int)item_count != 0);
                    }
                    m_content_data = (_XCONTENT_DATA *)items;
                }
                if (m_content_data != 0) {
                    int other_count;
                    other_count = m_other_content_count;
                    result = XEnumerate(*(void **)enumeration_handle, &m_content_data[other_count + m_content_count],
                        (unsigned long)enumeration_size, 0, overlapped);
                }
            }
        } else {
            result = 0x80004005;
        }
    }
    return result;
}
// @address 0x2f6f88 ?get_context_string@c_content_enumeration_overlapped_task@@UBAPBDXZ
const char *c_content_enumeration_overlapped_task::get_context_string() const
{
    return "XContentCreateEnumerator";
}
// @address 0x2f6f98 ?get_context_string@c_content_item_overlapped_task@@UBAPBDXZ
const char *c_content_item_overlapped_task::get_context_string() const
{
    const char *context_string;
    switch (get_task_type()) {
    case _content_item_task_type_create:
        context_string = "XContentCreateEx";
        break;
    case _content_item_task_type_delete:
        context_string = "XContentDelete";
        break;
    case _content_item_task_type_flush:
        context_string = "XContentFlush";
        break;
    case _content_item_task_type_close:
        context_string = "XContentClose";
        break;
    default:
        context_string = "<unknown content item task type>";
        break;
    }
    return context_string;
}
// @address 0x2f75e8 ?success@c_content_enumeration_overlapped_task@@UAAXK@Z
void c_content_enumeration_overlapped_task::success(unsigned long return_result)
{
    bool finished = false;
    long stage = m_content_type;
    if ((unsigned long)stage >= 1)
        m_other_content_count = return_result;
    else
        m_content_count = return_result;
    if (stage < 1 && m_other_content_count + m_content_count < 151) {
        volatile long *stage_pointer = &m_content_type;
        *stage_pointer = *stage_pointer + 1;
        reset();
        if (m_enumeration_handle != 0 && m_enumeration_handle != (void *)-1) {
            CloseHandle(m_enumeration_handle);
            m_enumeration_handle = 0;
        }
        if (overlapped_task_start_internal(this, "..\\saved_games\\content_catalogue_tasks.cpp", 551))
            task_recycled_during_completion(true);
        else
            finished = true;
    } else {
        finished = true;
    }
    if (finished) {
        if (m_content_data != 0)
            m_catalogue->content_enumeration_success(m_content_data, m_content_count, m_other_content_count);
        m_content_data = 0;
        m_content_count = 0;
        m_other_content_count = 0;
    }
}
// @address 0x2f76e0 ?failure@c_content_enumeration_overlapped_task@@UAAXKKK@Z
void c_content_enumeration_overlapped_task::failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error)
{
    m_catalogue->dispose_content_item_array();
}
static __forceinline void content_catalogue_tasks_delete_item_array(s_content_item *items)
{
    unsigned __int64 *count = (unsigned __int64 *)items - 1;
    s_content_item_array *array = (s_content_item_array *)count;
    for (long index = 0; index < array->count; index++)
        items[index].close(true);
    overlapped_free(array);
}
// @address 0x2f76e8 ?complete@c_content_enumeration_overlapped_task@@UAAXXZ
void c_content_enumeration_overlapped_task::complete()
{
    if (!(bool)(_rotl(m_flags, 31) & 1)) {
        c_content_catalogue *catalogue = m_catalogue;
        if (content_catalogue_has_storage_device(catalogue))
            catalogue->m_flags.value |= 1;
        else
            catalogue->dispose();
        if (m_enumeration_handle != 0 && m_enumeration_handle != (void *)-1) {
            CloseHandle(m_enumeration_handle);
            m_enumeration_handle = 0;
        }
        if (m_content_data != 0)
            content_catalogue_tasks_delete_item_array((s_content_item *)m_content_data);
        m_content_data = 0;
        m_enumeration_handle = 0;
        reset();
        g_level_globals.enumerate_dlc_files = true;
    }
}
// @address 0x580910 ??1c_marketplace_asset_enumeration_overlapped_task@@UAA@XZ
c_marketplace_asset_enumeration_overlapped_task::~c_marketplace_asset_enumeration_overlapped_task()
{
    if (m_enumeration_handle != 0 && m_enumeration_handle != (void *)-1) {
        CloseHandle(m_enumeration_handle);
        m_enumeration_handle = (void *)-1;
    }
}
void c_marketplace_asset_enumeration_overlapped_task::reset()
{
    c_overlapped_task::reset();
    if (m_enumeration_handle != 0 && m_enumeration_handle != (void *)-1) {
        CloseHandle(m_enumeration_handle);
        m_enumeration_handle = (void *)-1;
    }
}
// @address 0x5809b0 ?get_context_string@c_marketplace_asset_enumeration_overlapped_task@@UBAPBDXZ
const char *c_marketplace_asset_enumeration_overlapped_task::get_context_string() const
{
    return "XMarketplaceCreateAssetEnumerator";
}
// @address 0x5809c0 ?start@c_marketplace_asset_enumeration_overlapped_task@@UAAKPAX@Z
unsigned long c_marketplace_asset_enumeration_overlapped_task::start(void *overlapped)
{
    unsigned long result = 0x80004005;
    m_asset_count = 0;
    if (controller_get(m_controller_index)->is_signed_in_to_live()) {
        unsigned long buffer_size = 0;
        result = XMarketplaceCreateAssetEnumerator(m_controller_index, 32, &buffer_size, &m_enumeration_handle);
        if (result == 0) {
            result = 1223;
            m_enumeration_buffer = (XMARKETPLACE_ASSET_ENUMERATE_REPLY *)::overlapped_malloc_tracked(buffer_size, (char const *)(0), 0);
            memset(m_enumeration_buffer, 0, buffer_size);
            if (m_enumeration_buffer != 0)
                result = XEnumerate(m_enumeration_handle, m_enumeration_buffer, buffer_size, 0, overlapped);
        }
        if (result != 997) {
            if (m_enumeration_handle != 0 && m_enumeration_handle != (void *)-1) {
                CloseHandle(m_enumeration_handle);
                m_enumeration_handle = (void *)-1;
            }
        }
    }
    return result;
}
// @address 0x580ac8 ?success@c_marketplace_asset_enumeration_overlapped_task@@UAAXK@Z
void c_marketplace_asset_enumeration_overlapped_task::success(unsigned long return_result)
{
    if (return_result == 1) {
        const XMARKETPLACE_ASSET_PACKAGE *asset_package = &m_enumeration_buffer->asset_package;
        for (unsigned long asset_index = 0; asset_index < asset_package->asset_count; asset_index++) {
            m_assets[m_asset_count] = asset_package->assets[asset_index];
            m_asset_count++;
        }
    }
}
void c_marketplace_asset_enumeration_overlapped_task::complete()
{
    if (m_enumeration_handle && m_enumeration_handle != (void *)-1) {
        CloseHandle(m_enumeration_handle);
        m_enumeration_handle = (void *)-1;
    }
    if (m_enumeration_buffer) {
        overlapped_free(m_enumeration_buffer);
        m_enumeration_buffer = 0;
    }
}
// @address 0x580b30 ?is_result_successful@c_marketplace_asset_enumeration_overlapped_task@@UAA_NKKK@Z
bool c_marketplace_asset_enumeration_overlapped_task::is_result_successful(
    unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error)
{
    return (calling_result == 0 || calling_result == 997) &&
        ((overlapped_error == 0 && overlapped_extended_error == 0) || (overlapped_error == 1627 && overlapped_extended_error == 0x80070012));
}
// @address 0x580b30 ?is_result_successful@c_content_enumeration_overlapped_task@@UAA_NKKK@Z
bool c_content_enumeration_overlapped_task::is_result_successful(
    unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error)
{
    return (calling_result == 0 || calling_result == 997) &&
        ((overlapped_error == 0 && overlapped_extended_error == 0) || (overlapped_error == 1627 && overlapped_extended_error == 0x80070012));
}
// @address 0x580b80 ?get_context_string@c_marketplace_request_consume_assets_task@@UBAPBDXZ
const char *c_marketplace_request_consume_assets_task::get_context_string() const
{
    return "XMarketplaceConsumeAssets";
}
// @address 0x580bf8 ?get_context_string@c_marketplace_consume_assets_task@@UBAPBDXZ
const char *c_marketplace_consume_assets_task::get_context_string() const
{
    return "XShowMarketplaceDownloadItemsUI";
}

c_marketplace_asset_enumeration_overlapped_task g_marketplace_asset_enumeration_overlapped_task;

