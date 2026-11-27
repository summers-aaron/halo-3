#include "saved_games/game_content.h"
// blamlibxbox/content_item.obj: from the records 0x284630 0x2846d0 0x2847d0 0x284858 0x2f5f20 0x2f61f8 0x2f6270 0x2f6348 0x2f63e8 0x2f6640 0x2f6750 0x2f67c0 0x2f6830 0x2f6900 0x2f69b8 0x2f6ac8 0x2f6b50 0x2f6bd0 0x2f6cb0 0x2f6f20
// @flags /TP /O2
#include "saved_games/content_item_status_types.h"
#include "shell/shell.h"
#include "saved_games/game_content_types.h"
#include <string.h>
// @address 0x2f6830 ?get_display_name@s_content_item@@QBAPA_WPA_WJ@Z
// @address 0x2f6f20 content_item_get_string_length
// @address 0x2f6bd0 content_item_try_and_get_metadata
// @address 0x2f6270 content_item_delete_container
// @address 0x2f61f8 ?create_from_enumeration@s_content_item@@IAAXW4e_game_content_type@@PAVc_content_catalogue@@PBU_XCONTENT_DATA@@@Z
// @address 0x2f6348 ?flush@s_content_item@@IAA_N_N@Z
// @address 0x2f6900 ?get_directory_reference@s_content_item@@QAA_NPAUs_file_reference@@@Z
// @address 0x2f6ac8 ?get_container_full_path@s_content_item@@QBAPBDPADJ_N@Z
// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x1aec40 ?file_reference_create_from_path@@YAPAUs_file_reference@@PAU1@PBD_N@Z
// @address 0x2f63e8 ?open@s_content_item@@QAA_N_N0J0@Z
// @address 0x2f6640 ?close@s_content_item@@QAAX_N@Z
// @address 0x2f69b8 ?get_file_reference@s_content_item@@QBA_NPAUs_file_reference@@@Z
// @address 0x1e84a8 ?game_content_type_to_saved_game_file_type@@YA?AW4e_saved_game_file_type@@W4e_game_content_type@@@Z
// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z

#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "saved_games/saved_game_files.h"
#include "tag_files/files.h"
#include "saved_games/content_item.h"
#include "interface/controller_types.h"
#include "saved_games/content_synchronization_globals_types.h"
#include "saved_games/content_critical_section_types.h"
#include "saved_games/content_item_task_types.h"

#include "saved_games/saved_game_file_types.h"
class c_content_catalogue;
struct s_file_reference;
// @address 0x59fbe0 XContentFlush

#include "saved_games/content_item_types.h"

struct s_synchronization_globals;

#include "multithreading/synchronized_value.h"

struct s_content_item;
#include "cseries/async_xoverlapped_types.h"
#include "saved_games/content_catalogue_types.h"
#include "saved_games/storage_device.h"
#include "memory/data_types.h"
#include "xbox/content_api.h"
// @address 0x59fb68 XContentCreateEx
// @address 0x59fbd8 XContentClose

#include "saved_games/saved_game_scope_lock.h"

#include "saved_games/saved_game_item_metadata_packed.h"

struct s_content_item;

union ULARGE_INTEGER
{
    struct
    {
        unsigned long HighPart;
        unsigned long LowPart;
    };
    unsigned __int64 QuadPart;
};




typedef char content_item_size_check[(sizeof(void *) != 4 || ((sizeof(s_content_item) == 0x240))) ? 1 : -1];
typedef char content_item_flags_offset_check[(offsetof(s_content_item, flags) == 4) ? 1 : -1];
typedef char content_item_data_offset_check[(sizeof(void *) != 4 || ((offsetof(s_content_item, content_data) == 0x108))) ? 1 : -1];


#include "saved_games/content_item_inlines.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/content_item_storage.h"
#include "interface/controller_inline.h"
#include "tag_files/file_reference.h"
#include "saved_games/saved_game_files_operations.h"
#include "saved_games/content_item_inline.h"

__forceinline void s_content_item::block_until_finished()
{
    get_content_catalogue()->block_until_finished(this, false, true);
}



__forceinline void s_content_item::set_valid(bool valid)
{
    flags.set_bit(0, valid);
}




static __forceinline unsigned long content_item_storage_device_id(long controller_index)
{
    c_storage_device *storage_device = (unsigned long)controller_index <= 3 ? &g_content_catalogue_globals.storage_devices[controller_index] : 0;
    return storage_device ? storage_device->m_device_id : 0xcacacaca;
}

// @address 0x2f5f20 content_item_create
extern "C" bool content_item_create(s_content_item *item, e_game_content_type game_content_type, c_content_catalogue *catalogue,
    const wchar_t *name, const wchar_t *display_name, long flags, long maximum_size, bool synchronous)
{
    bool result = false;
    if (content_catalogue_get_remaining_slots(catalogue, game_content_type, 1) > 0) {
        _XCONTENT_DATA content_data;
        content_data.DeviceID = content_item_storage_device_id(catalogue->get_controller_index());
        content_data.dwContentType = 1;
        ustrnzcpy_inline(content_data.szDisplayName, name, 128);
        if (display_name)
            ustrnzcat(content_data.szDisplayName, display_name, 128);
        content_catalogue_build_new_saved_game_file_type_container_name(game_content_type, content_data.szFileName, 42);
        item->flags.set(0);
        item->set_content_catalogue(catalogue);
        item->set_game_content_type(game_content_type);
        item->set_content_data(&content_data);
        if (synchronous) {
            unsigned long content_flags = (flags & 1 ? 2 : 0) | (flags & 2 ? 3 : 0) | (flags & 4 ? 16 : 0) | (flags & 8 ? 32 : 0) | (flags & 16 ? 64 : 0);
            ULARGE_INTEGER content_size;
            content_size.LowPart = 0;
            if (maximum_size >= 0)
                content_size.LowPart = maximum_size;
            content_size.HighPart = 0;
            char full_path[256];
            item->get_container_full_path(full_path, sizeof(full_path), false);
            if ((long)XContentCreateEx(item->get_controller_index(), full_path, &item->content_data, content_flags, 0, 0, 0, content_size.QuadPart, 0) >= 0) {
                item->set_valid(true);
                item->set_open(true);
                item->flags.set_bit(6, true);
                result = true;
            }
        } else {
            item->block_until_finished();
            c_saved_game_scope_lock lock;
            c_content_item_overlapped_task *task = catalogue->get_content_item_task();
            task->reset();
            task->set_content_item(item);
            task->set_content_creation_flags(flags);
            task->set_maximum_content_size_bytes(maximum_size);
            task->set_task_type(_content_item_task_type_create);
            result = overlapped_task_start_internal(task, "..\\saved_games\\content_item.cpp", 159);
            if (result) {
                item->set_valid(true);
                item->set_busy(true);
                item->flags.set_bit(6, true);
            }
        }
    }
    return result;
}


void s_content_item::create_from_enumeration(e_game_content_type content_type, c_content_catalogue *catalogue, const _XCONTENT_DATA *data)
{
    set_content_data(data);
    flags.set(0);
    set_content_catalogue(catalogue);
    set_game_content_type(content_type);
    set_valid(true);
}

bool s_content_item::flush(bool synchronous)
{
    bool success = false;
    if (is_valid()) {
        char full_path[256];
        long result = XContentFlush(get_container_full_path(full_path, sizeof(full_path), false), 0);
        if (result >= 0)
            success = true;
    }
    return success;
}

bool s_content_item::open(bool create, bool no_profile_transfer, long maximum_content_size_bytes, bool synchronous)
{
    bool success = false;
    if (is_valid()) {
        if (!is_open()) {
            if (is_busy())
                block_until_finished();
            c_saved_game_scope_lock lock;
            if (!is_open()) {
                if (synchronous) {
                    unsigned long content_flags = create ? 2 : 3;
                    unsigned __int64 content_size = maximum_content_size_bytes;
                    if (no_profile_transfer)
                        content_flags |= 16;
                    char full_path[256];
                    get_container_full_path(full_path, sizeof(full_path), false);
                    unsigned long result = XContentCreateEx(get_controller_index(), full_path, &content_data, content_flags, 0, 0, 0, content_size, 0);
                    if ((long)result >= 0) {
                        set_open(true);
                        success = true;
                    } else if (result == 1392) {
                        set_corrupt(true);
                    }
                } else {
                    c_content_item_overlapped_task *task = get_content_catalogue()->get_content_item_task();
                    task->reset();
                    task->set_content_item(this);
                    task->set_content_creation_flags((create ? 1 : 2) | (no_profile_transfer ? 4 : 0));
                    task->set_maximum_content_size_bytes(maximum_content_size_bytes);
                    task->set_task_type(_content_item_task_type_create);
                    success = overlapped_task_start_internal(task, "..\\saved_games\\content_item.cpp", 397);
                    if (success)
                        set_busy(true);
                }
            } else {
                success = true;
            }
        } else {
            success = true;
        }
    }
    return success;
}

void s_content_item::close(bool synchronous)
{
    if (is_open()) {
        if (synchronous) {
            char full_path[256];
            XContentClose(get_container_full_path(full_path, sizeof(full_path), false), 0);
            set_open(false);
        } else {
            block_until_finished();
            c_saved_game_scope_lock lock;
            c_content_item_overlapped_task *task = get_content_catalogue()->get_content_item_task();
            task->reset();
            task->set_content_item(this);
            task->set_task_type(_content_item_task_type_close);
            if (overlapped_task_start_internal(task, "..\\saved_games\\content_item.cpp", 466))
                set_busy(true);
        }
    }
}

void s_content_item::set_game_content_type(e_game_content_type content_type)
{
    c_saved_game_scope_lock lock;
    game_content_type = content_type;
}

void s_content_item::set_content_catalogue(c_content_catalogue *catalogue)
{
    c_saved_game_scope_lock lock;
    content_catalogue = catalogue;
}

wchar_t *s_content_item::get_display_name(wchar_t *display_name, long display_name_size) const
{
    *display_name = 0;
    if (is_valid()) {
        c_saved_game_scope_lock lock;
        ustrnzcpy(display_name, content_data.szDisplayName, 128);
    }
    return display_name;
}

bool s_content_item::get_directory_reference(s_file_reference *reference)
{
    bool success = false;
    if (is_valid()) {
        char full_path[256];
        file_reference_create_from_path(reference, get_container_full_path(full_path, sizeof(full_path), true), true);
        success = true;
    }
    return success;
}

bool s_content_item::get_file_reference(s_file_reference *reference) const
{
    bool success = false;
    e_saved_game_file_type file_type = game_content_type_to_saved_game_file_type(get_game_content_type());
    if (file_type >= _saved_game_file_type_bmf && file_type <= _saved_game_file_type_shot) {
        char full_path[256];
        get_container_full_path(full_path, sizeof(full_path), true);
        csstrnzcat(full_path, saved_game_files_get_saved_game_file_name(file_type));
        file_reference_create_from_path(reference, full_path, false);
        success = true;
    }
    return success;
}

const char *s_content_item::get_container_full_path(char *buffer, long buffer_size, bool drive_only) const
{
    if (drive_only && strstr(content_data.szFileName, ":\\") <= content_data.szFileName) {
        csnzprintf(buffer, 256, "%s:\\", content_data.szFileName);
        return buffer;
    }
    strncpy(buffer, content_data.szFileName, 256);
    buffer[255] = 0;
    return buffer;
}

void s_content_item::set_content_data(const _XCONTENT_DATA *data)
{
    c_saved_game_scope_lock lock;
    memmove(&content_data, data, sizeof(content_data));
}



extern "C" void content_item_delete_container(s_content_item *content_item)
{
    if (content_item->is_valid()) {
        content_item->block_until_finished();
        c_content_catalogue *catalogue = content_item->content_catalogue;
        e_controller_index controller_index = catalogue != 0 ? catalogue->get_controller_index() : k_no_controller;
        if (controller_get(controller_index)->is_signed_in_to_machine())
            XContentDelete(controller_index, &content_item->content_data, 0);
    }
}

extern "C" bool content_item_try_and_get_metadata(const s_content_item *content_item, s_saved_game_item_metadata_packed *metadata)
{
    bool metadata_valid = content_item->is_valid() && content_item->flags.test_bit(4) && saved_game_metadata_is_valid(&content_item->metadata);
    if (metadata_valid) {
        *metadata = content_item->metadata;
        return true;
    }
    return false;
}

// Length of a zero-terminated wide string, counted up to 128 characters.
extern "C" unsigned int content_item_get_string_length(const unsigned short *string)
{
    unsigned int length;
    for (length = 0; length < 128; length++) {
        if (string[length] == 0)
            break;
    }
    return length;
}

// @address 0x284630 ?make_ready_for_access@s_content_item@@QAA_NJ_N@Z
bool s_content_item::make_ready_for_access(long content_item_index, bool synchronous)
{
    bool success = false;
    if (content_catalogue_has_storage_device(((c_content_catalogue *)this))) {
        c_saved_game_scope_lock lock;
        s_content_item *content_item = content_catalogue_try_and_get_item(((c_content_catalogue *)this), content_item_index);
        if (content_item != 0)
            success = content_item->open(false, false, 0, synchronous);
    }
    return success;
}

 

// @address 0x2846d0 content_catalogue_open_or_create_item
extern "C" long content_catalogue_open_or_create_item(c_content_catalogue *catalogue, const wchar_t *display_name, const wchar_t *name,
    e_game_content_type content_type, bool no_profile_transfer, long maximum_content_size, long unused)
{
    int item_index = -1;
    if (content_catalogue_has_storage_device(catalogue)) {
        c_saved_game_scope_lock lock;
        item_index = catalogue->try_and_get_personal_storage_item(display_name, (e_content_item_status)content_type);
        if (item_index != -1) {
            unsigned short index = (unsigned short)item_index;
            s_data_array *storage = catalogue->get_personal_storage_link();
            s_content_item *item = &((s_content_item *)storage->data)[index];
            if (!item->open(true, no_profile_transfer, maximum_content_size, false))
                item_index = -1;
        } else {
            item_index = (int)content_catalogue_new_item(catalogue, display_name, name, content_type, no_profile_transfer, maximum_content_size, false);
        }
    }
    return item_index;
}


// @address 0x2847d0 content_catalogue_close_item
extern "C" void content_catalogue_close_item(c_content_catalogue *catalogue, long content_item_index, bool synchronous)
{
    if (content_catalogue_has_storage_device(catalogue)) {
        c_saved_game_scope_lock lock;
        s_content_item *content_item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (content_item != 0)
            content_item->close(synchronous);
    }
}

// @address 0x284858 ?content_catalogue_get_game_content_type@@YA?AW4e_game_content_type@@PAVc_content_catalogue@@J@Z
e_game_content_type content_catalogue_get_game_content_type(c_content_catalogue *catalogue, long content_item_index)
{
    e_game_content_type game_content_type = _game_content_type_none;
    c_saved_game_scope_lock lock;
    if (content_catalogue_has_storage_device(catalogue)) {
        s_content_item *content_item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (content_item != 0)
            game_content_type = content_item->get_game_content_type();
    }
    return game_content_type;
}



static __forceinline long content_item_name_index_of(const c_static_wchar_string<128> *name, const wchar_t *substring)
{
    long index = -1;
    if ((long)content_item_get_string_length((const unsigned short *)name->get_string()) > 0) {
        const wchar_t *found = wcsstr(name->get_string(), substring);
        if (found)
            index = found - name->get_string();
    }
    return index;
}

// @address 0x2f6cb0 content_item_load_metadata
extern "C" bool content_item_load_metadata(s_content_item *content_item)
{
    bool success = false;
    e_game_content_type content_type = content_item->get_game_content_type();
    if (content_type != _game_content_type_savegame && content_type != _game_content_type_personal) {
        c_saved_game_scope_lock lock;
        if (content_item->is_valid()) {
            bool was_open = content_item->is_open();
            if (content_item->is_busy())
                content_item->block_until_finished();
            if (!was_open)
                content_item->open(false, false, 0, true);
            if (content_item->is_open()) {
                e_saved_game_file_type file_type = game_content_type_to_saved_game_file_type(content_type);
                s_file_reference file;
                if ((unsigned long)file_type <= _saved_game_file_type_shot && content_item->get_file_reference(&file)) {
                    if (!saved_game_read_metadata_from_file(&file, &content_item->metadata)) {
                        wchar_t display_name[128];
                        c_static_wchar_string<128> name;
                        content_item->get_display_name(display_name, 128);
                        name.set(display_name);
                        long index = content_item_name_index_of(&name, L"(");
                        if (index != -1) {
                            if (index > 0 && display_name[index - 1] == L' ')
                                index--;
                            display_name[index] = 0;
                        }
                        saved_game_files_initialize_metadata((s_saved_game_item_metadata_record *)&content_item->metadata,
                            content_item->content_catalogue->get_controller_index(), file_type, display_name, L"", 0, 0);
                        content_item->flags.set_bit(4, true);
                        content_item->flags.set_bit(1, true);
                    }
                    success = true;
                }
                if (!was_open)
                    content_item->close(true);
            }
        }
    }
    content_item->flags.set_bit(4, success);
    return success;
}
