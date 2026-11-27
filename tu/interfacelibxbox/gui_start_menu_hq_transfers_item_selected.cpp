// interfacelibxbox/gui_start_menu_hq_transfers_item_selected.obj: from the records 0x4b8d60 0x4b8d68 0x4b8dc8 0x4b8e08 0x4b8ef0 0x4b8f48
// @flags /TP /O2 /Oc
#include "interface/gui_widget.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers.h"
// @address 0x52020 ??_7c_gui_transfers_selected_item_datasource@@6B@
// @address 0x4b4128 ?get_column_names@c_gui_transfers_selected_item_datasource@@UAAXQAJPAJ@Z
// @address 0x4b8d68 ?initialize@c_gui_transfers_selected_item_datasource@@UAA_NJ@Z
// @address 0x4b8f48 ?handle_list_item_chosen@c_gui_start_menu_hq_transfers_item_selected@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "interface/user_interface_memory.h"
#include "networking/online/online_files.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#include "interface/gui_screen_widget_legacy.h"
#include "cseries/crt_xdk.h"
#include "interface/c_gui_text_widget.h"
#include "interface/user_interface_messages.h"
#include "interface/c_gui_list_item_widget.h"

enum e_transfer_menu_string_id
{
    _transfer_menu_name = 0x29c,
    _transfer_menu_actions = 0x10010,
    _transfer_menu_cancel = 0x1029a,
    _transfer_menu_prioritize = 0x1029b
};

// @address 0x4b8d60 ?get_current_item_count_internal@c_gui_transfers_selected_item_datasource@@MAAJXZ
long c_gui_transfers_selected_item_datasource::get_current_item_count_internal()
{
    return item_count;
}

bool c_gui_transfers_selected_item_datasource::initialize(long parameter)
{
    m_name = parameter;
    item_count = 0;
    item_ids[0] = _transfer_menu_cancel;
    long count = item_count;
    unsigned char has_secondary_item = this->has_secondary_item;
    count++;
    item_count = count;
    if (has_secondary_item != 0)
        return true;
    item_ids[count] = _transfer_menu_prioritize;
    item_count++;
    return true;
}

void c_gui_transfers_selected_item_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _transfer_menu_name;
    (*column_count)++;
}

// @address 0x4b8dc8 ?get_string_id_value@c_gui_transfers_selected_item_datasource@@UAA_NJJPAJ@Z
bool c_gui_transfers_selected_item_datasource::get_string_id_value(long element_handle, long value_name, long *out_value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < item_count && value_name == _transfer_menu_name) {
        *out_value = item_ids[element_handle];
        result = true;
    }
    return result;
}

// @address 0x4b8e08 ?initialize_datasource@c_gui_start_menu_hq_transfers_item_selected@@UAAXXZ
void c_gui_start_menu_hq_transfers_item_selected::initialize_datasource()
{
    gui_screen_widget_initialize_datasource(this);
    c_gui_transfers_selected_item_datasource *task = (c_gui_transfers_selected_item_datasource *)
        ::user_interface_malloc_tracked(sizeof(c_gui_transfers_selected_item_datasource), 0, 0);
    if (task) {
        new (task) c_gui_transfers_selected_item_datasource(filename, &status, enabled);
        if (task->initialize(_transfer_menu_actions)) {
            m_datasource[m_datasource_count] = task;
            m_datasource_count++;
            return;
        }
        task->~c_gui_transfers_selected_item_datasource();
        ::user_interface_free((void const *)(task));
    }
}

// @address 0x4b8ef0 ?update@c_gui_start_menu_hq_transfers_item_selected@@UAAXK@Z
void c_gui_start_menu_hq_transfers_item_selected::update(unsigned long current_milliseconds)

{
  gui_screen_widget_update(this, current_milliseconds);
  c_gui_text_widget *child_text_widget = static_cast<c_gui_text_widget *>(get_child_widget(_gui_text_widget, 0x29d));
  if (child_text_widget != 0)
      child_text_widget->set_text(status.description.metadata.name);

}

bool c_gui_start_menu_hq_transfers_item_selected::handle_list_item_chosen(
    const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled=false;
    if(list==_transfer_menu_actions) {
        long chosen;
        if(data->get_string_id_value(item->get_element_handle(),_transfer_menu_name,&chosen)) {
            switch(chosen) {
            case _transfer_menu_cancel:
                online_file_manager_delete_transfer(&g_online_file_manager, filename);
                transition_out_with_transition_type(0, _screen_transition_normal);
                break;
            case _transfer_menu_prioritize:
                g_online_file_manager.bump_transfer_priority(filename);
                transition_out_with_transition_type(0, _screen_transition_normal);
                break;
            }
        }
        handled=true;
    }
    if(!handled) handled=false;
    return handled;
}
