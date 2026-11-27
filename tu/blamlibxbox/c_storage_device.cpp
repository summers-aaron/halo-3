// blamlibxbox/c_storage_device.obj: from the records 0x2f5a00 0x2f5a10 0x2f5ab0 0x2f5ab8 0x2f5bb8 0x2f5cc0 0x2f5e40
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "saved_games/storage_device.h"
#include "saved_games/content_catalogue_types.h"
#include "saved_games/content_catalogue_externs.h"
#include <string.h>
#include "interface/user_interface_messages.h"
#include "interface/c_deletable.h"
// @address 0x59ffb8 XShowDeviceSelectorUI
// @address 0x483310 ??0c_load_dialog_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJJ@Z
// @address 0x8a4c8 ?is_signed_in_to_machine@c_controller_interface@@QBA_NXZ
// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x2f5e40 ?get_device_data@c_storage_device@@SA_NKPAW4e_storage_device_type@@PA_K1PA_W@Z
// @address 0x284ff0 ?begin_enumeration@c_content_catalogue@@QAA_NXZ
// @address 0x494790 ?post_error_with_custom_message@c_gui_error_manager@@QAAXJPB_W0W4e_controller_index@@_N2@Z
#include <stdlib.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/controller_state_flags.h"
#include "saved_games/storage_device_xdk.h"
class c_storage_device;

#include "cseries/async_xoverlapped_types.h"
#include <new>
#include "interface/user_interface.h"
#include "text/unicode_inline.h"
namespace {
static __forceinline unsigned long online_guide_show_device_selector_ui(
    e_controller_index controller_index, unsigned long storage_bytes, unsigned long *device_id, bool force_selection, void *overlapped)
{
    union { unsigned __int64 value; struct { unsigned long high; unsigned long low; } parts; } bytes;
    bytes.parts.high = 0;
    bytes.parts.low = storage_bytes;
    unsigned long flags = 0;
    if (force_selection) flags = 0x200;
    if (controller_get(controller_index)->is_signed_in_to_machine())
        return XShowDeviceSelectorUI(controller_index, 1, flags, bytes.value, device_id, overlapped);
    return 0x80004005;
}
}
// storage_device_selection_succeeded calls get_device_data with the name buffer length as a sixth argument
typedef bool (*c_storage_device_device_data_query)(unsigned long, e_storage_device_type *, unsigned __int64 *, unsigned __int64 *, wchar_t *, long);
const char *c_storage_device_selector_overlapped_task::get_context_string() const {
    return "XShowDeviceSelectorUI";
}
unsigned long c_storage_device_selector_overlapped_task::start(void *overlapped) {
    unsigned long result = 0;
    m_storage_device->set_device_selection_cancelled(false);
    result = online_guide_show_device_selector_ui(m_controller_index, m_storage_bytes, m_device_id, m_force_selection, overlapped);
    return result;
}
void c_storage_device_selector_overlapped_task::success(unsigned long return_result) {
    m_storage_device->storage_device_selection_succeeded(return_result);
}
static __forceinline long c_storage_device_controller_range_bad(long x) { if (x < 0) return 1; if (x >= 4) return 1; return 0; }
static __forceinline void c_storage_device_clear_controller_flag(long index)
{
    controller_get((e_controller_index)index)->flags.set(_controller_state_storage_device_selected, false);
}
// @address 0x2f5ab8 ?failure@c_storage_device_selector_overlapped_task@@UAAXKKK@Z
void c_storage_device_selector_overlapped_task::failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error) {
    if (overlapped_error == 0x65b && overlapped_extended_error == 0x4c7) {
        m_storage_device->cancel_and_release();
        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
        if (memory == 0) return;
        c_load_dialog_screen_message *message = new (memory) c_load_dialog_screen_message(m_controller_index, _console_window, 0x1000c, 0x103dc, 0x103dc);
        if (message == 0) return;
        ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
        return;
    }
    c_storage_device *storage_device = m_storage_device;
    long controller_index = storage_device->get_controller_index();
    if (!c_storage_device_controller_range_bad(controller_index)) {
        c_storage_device_clear_controller_flag(controller_index);
    }
    storage_device->release();
}

void c_storage_device::select_device(unsigned long storage_bytes)
{
    // match: 0x2f5bb8: task-field store folds storage_bytes to 0xa00000 (lis r7,160); without __assume c2 stores the r4 parameter
    __assume(storage_bytes == 0xa00000);
    if (!is_being_selected()) {
        release();
        m_task.initialize(this, get_controller_index(), storage_bytes, &m_device_id);
        if (overlapped_task_start_internal(&m_task, "..\\saved_games\\c_storage_device.cpp", 138)) {
            s_controller *controller = controller_get(get_controller_index());
            controller->flags.set_unsafe(controller->flags.get_unsafe() | 24);
        }
    }
}

void c_storage_device::storage_device_selection_succeeded(unsigned long return_result)
{
    e_storage_device_type device_type = _storage_device_type_none;
    unsigned __int64 total_bytes;
    unsigned __int64 free_bytes;
    wchar_t name[128];
    bool success = ((c_storage_device_device_data_query)get_device_data)(m_device_id, &device_type, &total_bytes, &free_bytes, name, 128);
    if (g_user_interface_is_alpha && device_type != _storage_device_type_hard_drive) {
        g_gui_error_manager.post_error_with_custom_message(0x20003, 0, 0, get_controller_index(), false, true);
        c_storage_device_clear_controller_flag(get_controller_index());
        release();
        success = false;
    }
    set_valid(success);
    if (is_valid()) {
        char ascii_name[128];
        long written;
        wchar_string_to_ascii_string(name, ascii_name, 128, &written);
        g_content_catalogue_globals.catalogues[get_controller_index()].begin_enumeration();
    }
}

bool c_storage_device::get_device_data(unsigned long device_id, e_storage_device_type *storage_device_type, unsigned __int64 *total_bytes,
    unsigned __int64 *free_bytes, wchar_t *name)
{
    bool result = false;
    s_storage_device_data data;
    if (device_id != 0xcacacaca) {
        memset(&data.device_type, 0, sizeof(data) - offsetof(s_storage_device_data, device_type));
        data.device_id = device_id;
        if (XContentGetDeviceData(device_id, &data) == 0) {
            result = true;
            if (storage_device_type) {
                unsigned long type = data.device_type;
                if (type != 1) *storage_device_type = (e_storage_device_type)((type == 2) - 1);
                else *storage_device_type = _storage_device_type_hard_drive;
            }
            if (total_bytes) *total_bytes = data.total_bytes;
            if (free_bytes) *free_bytes = data.free_bytes;
            if (name) ustrnzcpy(name, data.name, 128);
        }
    }
    return result;
}

