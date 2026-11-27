// interfacelibxbox/gui_start_menu_hq_service_record_file_share.obj: from the records 0x492858 0x4aeaa0 0x4aee50 0x4af8c8 0x4af8f8 0x4af930 0x4af9e8 0x4af9f0 0x4afb30 0x4afd08 0x4afd68 0x4afe88 0x4affd8 0x4b0060 0x4b03f0 0x4b04d8 0x4b0510 0x4b0ca0 0x4b0da0 0x4b1278 0x4b1508 0x4b1790 0x4b18e0 0x4b1930 0x4b1ee0 0x4b1f50 0x4b1fd0 0x4b2068 0x4b2110 0x4b21b8 0x4b2260
#include "interface/c_gui_screen_widget.h"
#include "saved_games/saved_game_item_metadata.h"
#include "saved_games/saved_game_files.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/c_player_marketplace.h"
#include "interface/gui_file_share_selection.h"
// @flags /TP /O2 /Oc
#include "interface/gui_widget_tree.h"
#include "interface/user_interface_utilities.h"
#include <stddef.h>
#include "interface/custom_map_image_types.h"
#include "math/real_math_types.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_animation_transform.h"
#include "memory/static_arrays.h"
#include "multithreading/synchronization.h"
#include "multithreading/critical_section_scope.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "text/font_cache.h"
#include "interface/gui_screens/start_menu/panes/settings/start_menu_settings.h"
#include "interface/c_controller.h"
#include "interface/controller_types.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_category.h"
#include "interface/user_interface_messages.h"
#include "interface/c_gui_list_widget.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/c_gui_widget.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_item_selected.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share.h"
#include "networking/online/online_files.h"
#include "networking/online/online_file_work_item_types.h"
#include "interface/user_interface_allocation.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_list_item_widget.h"
#include "cseries/cseries.h"
#include "interface/gui_start_menu_hq_service_record_file_share_externs.h"
class c_gui_data;
// @address 0x484370 ?user_interface_get_bytes_size_string@@YA_NJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x8a3c0 ?ustrnzcpy@@YAPA_WPA_WPB_WJ@Z
// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x847d0 ?internal_critical_section_leave@@YAXJ@Z
// @address 0x484d20 ?print@?$c_static_wchar_string@$0EAA@@@QAAPB_WPB_WZZ
// @address 0x3bb708 ?get_transfer_status_by_slot@c_online_files_active_transfer_tracker@@QBA_N_KJPAUs_queued_work_item_status@@@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x487450 ?parse_tag@c_magic_string_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4874c0 ?get_magic_string@c_magic_string_game_tag_parser@@UAA_NPA_WJ@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
// @address 0x4afe88 ?get_string_id_value@c_gui_file_share_slots_datasource@@UAA_NJJPAJ@Z
// @address 0x4af9e8 ?get_current_item_count_internal@c_gui_file_share_slots_datasource@@MAAJXZ
// @address 0x4af8c8 ?apply_initial_state@c_load_file_share_choose_category_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
// @address 0x4af8f8 ?apply_initial_state@c_load_file_share_choose_item_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
// @address 0x4afb30 ?update@c_gui_file_share_slots_datasource@@UAAXXZ
// @address 0x4af930 ?apply_initial_state@c_load_file_share_item_selected_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
// @address 0x4b0060 ?initialize@c_gui_start_menu_hq_service_record_file_share@@UAAXXZ
// @address 0x4b03f0 ?initialize_datasource@c_gui_start_menu_hq_service_record_file_share@@UAAXXZ
// @address 0x4b2068 parse_xml_file_share_progress
// @address 0x485008 ??0c_gui_screen_widget@@QAA@J@Z
// @address 0x485350 ?dispose@c_gui_screen_widget@@UAAXXZ
// @address 0x4927d8 ?set_map_image@c_gui_custom_bitmap_widget@@QAAXW4e_custom_map_image_type@@W4e_map_id@@_N@Z
// @address 0x3bb7c0 ?stop@c_online_files_enumerator@@QAAXXZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z
// @address 0x48b228 ?set_full_animation_state@c_gui_widget@@QAAXPBUs_animation_transform@@_N@Z

#include "saved_games/saved_game_file_types.h"

// A slot holds either an enumerated file description or an active transfer.
typedef char file_share_slot_size[(sizeof(s_gui_file_share_slot) == 0x130) ? 1 : -1];
typedef char file_share_slot_transfer_offset[(offsetof(s_gui_file_share_slot, transfer) == 8) ? 1 : -1];
typedef char file_share_slot_transfer_state_offset[(offsetof(s_gui_file_share_slot, transfer) + offsetof(s_queued_work_item_status, state) == 0x11c) ? 1 : -1];

#include "networking/online/online_file_manager.h"



#include "networking/online/online_files_enumerator.h"
typedef s_file_share_slot s_file_share_enumerator_slot;



class c_online_file_manager;


extern "C" {





// @address 0x514e8 ??_7c_gui_start_menu_hq_service_record_file_share@@6B@
// @address 0x4a8838 ??_Ec_gui_start_menu_hq_service_record_file_share@@UAAPAXI@Z
// @address 0x4a8838 ??_Gc_gui_start_menu_hq_service_record_file_share@@UAAPAXI@Z
// @address 0x515f8 ??_7c_gui_file_share_slots_datasource@@6B@




}


#include "interface/gui_data.h"
class c_gui_file_share_slots_datasource;


#include "interface/gui_file_share_screen_messages.h"





#include "interface/game_tag_parser.h"







#include "interface/gui_widget.h"

#include "interface/c_gui_list_item_widget.h"





#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/service_record_pane.h"



#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share_choose_item.h"





// @address 0x513d8 ??_7c_gui_insignia_data@@6B@
// @address 0x4c0ee0 ??_Gc_gui_insignia_data@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_insignia_data@@UAAPAXI@Z

#include <new>



#include <string.h>



#include "interface/gui_custom_bitmap_widget.h"
#include "multithreading/synchronization_types.h"
#include "multithreading/synchronization_runtime.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#include "interface/user_interface_error_manager.h"

static __forceinline void gui_start_menu_hq_service_record_file_share_start_enumerator(
    s_file_share_enumerator_state *enumerator, unsigned __int64 owner_xuid, unsigned __int64 target_xuid)
{
    enumerator->m_xuid = owner_xuid;
    enumerator->m_state = 0;
    enumerator->m_quota_bytes = 0;
    enumerator->m_quota_slots = 0;
    enumerator->m_owner_xuid = target_xuid;
    enumerator->m_visible_slots = 0;
    enumerator->started = true;
}


// @address 0x492858 gui_start_menu_hq_service_record_file_share_set_map_image
extern "C" void gui_start_menu_hq_service_record_file_share_set_map_image(c_gui_custom_bitmap_widget *widget, c_gui_data *data, long index, long unknown)
{
    bool loaded = false;
    long map_id;
    long kind;
    if (index >= 0 && index < data->get_current_item_count() &&
        data->get_integer_value(index, _start_menu_string_map_id, &map_id) &&
        data->get_integer_value(index, _start_menu_string_type, &kind))
    {
        e_custom_map_image_type type = _custom_map_image_type_none;
        switch ((unsigned long)kind)
        {
        case 0:
            type = _custom_map_image_type_film;
            break;
        case 1:
            type = _custom_map_image_type_film_clip;
            break;
        case 2:
            type = _custom_map_image_type_variant;
            break;
        }
        if (type != _custom_map_image_type_none)
        {
            widget->set_map_image(type, (e_map_id)map_id, true);
            loaded = true;
        }
    }
    if (!loaded)
        memset(widget->m_map_image, 0, sizeof(widget->m_map_image));
}

// @address 0x4aeaa0 ?get_column_names@c_gui_insignia_data@@UAAXQAJPAJ@Z
void c_gui_insignia_data::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _start_menu_string_name;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_image;
    ++*column_count;
    column_names[*column_count] = _file_share_item_string_title;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_message;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_enabled;
    ++*column_count;
}

// @address 0x4aee50 ?initialize_datasource@c_gui_start_menu_hq_service_record_pane@@UAAXXZ
void c_gui_start_menu_hq_service_record_pane::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    c_gui_data *training = find_data(_start_menu_string_insignia_training, 0);
    c_gui_data *spartan = find_data(_start_menu_string_insignia_spartan, 0);
    c_gui_data *officer = find_data(_start_menu_string_insignia_officer, 0);
    c_gui_data *campaign = find_data(_start_menu_string_insignia_campaign, 0);
    if (training && spartan && officer && campaign)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_insignia_data), 0, 0);
        if (memory != 0)
        {
            c_gui_insignia_data *datasource = new (memory) c_gui_insignia_data(training, spartan, officer, campaign);
            if (datasource->initialize(_start_menu_string_insignia_data))
            {
                m_datasource[m_datasource_count] = datasource;
                m_datasource_count++;
                return;
            }
            datasource->~c_gui_insignia_data();
            user_interface_free(datasource);
        }
    }
}

void c_load_file_share_choose_category_screen_message::apply_initial_state(c_gui_screen_widget *screen_widget) const
{
    // match: 0x4af8c8: __restrict lets the four message loads hoist above the screen stores
    c_gui_start_menu_hq_service_record_file_share_choose_category *__restrict destination =
        (c_gui_start_menu_hq_service_record_file_share_choose_category *)screen_widget;
    destination->m_e = m_quota_total;
    destination->m_d = m_quota_used;
    destination->m_c = m_slot_index;
    destination->m_b = m_player_xuid;
    destination->m_a = m_owner_xuid;
}

void c_load_file_share_choose_item_screen_message::apply_initial_state(c_gui_screen_widget *screen_widget) const
{
    ((c_gui_start_menu_hq_service_record_file_share_choose_item *)screen_widget)->setup(m_owner_xuid, m_player_xuid,
        m_slot_index, m_quota_used, m_quota_total, (e_saved_game_file_type)m_file_type);
}

void c_load_file_share_item_selected_screen_message::apply_initial_state(c_gui_screen_widget *screen_widget) const
{
    // match: 0x4af930: __restrict lets the message loads hoist above the screen stores
    c_gui_start_menu_hq_service_record_file_share_item_selected *__restrict destination =
        (c_gui_start_menu_hq_service_record_file_share_item_selected *)screen_widget;
    destination->m_size = m_slot_index;
    destination->m_player_xuid = m_player_xuid;
    destination->m_owner_xuid = m_owner_xuid;
    memcpy(&destination->m_file_share_slot, &m_slot, sizeof(m_slot));
    destination->m_identity.identity = m_identity;
}

long c_gui_file_share_slots_datasource::get_current_item_count_internal()
{
    return m_slot_count;
}

// @address 0x4af9f0 ?get_column_names@c_gui_file_share_slots_datasource@@UAAXQAJPAJ@Z
void c_gui_file_share_slots_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _start_menu_string_name;
    ++*column_count;
    column_names[*column_count] = _file_share_item_string_description;
    ++*column_count;
    column_names[*column_count] = _file_share_item_string_author;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_filename;
    ++*column_count;
    column_names[*column_count] = _file_share_item_string_size;
    ++*column_count;
    column_names[*column_count] = _file_share_item_string_date;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_type;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_game_engine_index;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_map_id;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_bnet_pro;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_bnet_pro_important;
    ++*column_count;
}

void c_gui_file_share_slots_datasource::update()
{
    if (!m_enumerator->is_busy())
    {
        m_slot_count = 0;
        for (long slot_index = 0; slot_index < m_enumerator->get_quota_slot_count(); ++slot_index)
        {
            const s_file_share_enumerator_slot *enumerator_slot = m_enumerator->get_file_share_slot(slot_index);
            m_slots[m_slot_count].type = 0;
            if ((&((c_online_file_manager *)&g_online_file_manager)->m_active_transfer_tracker)
                    ->get_transfer_status_by_slot(
                        m_enumerator->get_file_share_xuid(), slot_index, &m_slots[m_slot_count].transfer))
            {
                m_slots[m_slot_count].type = 1;
            }
            else if (enumerator_slot && enumerator_slot->state == 2)
            {
                m_slots[m_slot_count].type = 2;
                m_slots[m_slot_count].file = enumerator_slot->description;
            }
            ++m_slot_count;
        }
        if (m_show_quota && m_slot_count > 0)
        {
            s_player_marketplace_user *user = &g_player_marketplace_users[m_controller_index];
            g_player_marketplace_users[m_controller_index].controller_index = m_controller_index;
            if (user->signed_in)
            {
                bool is_local = user->catalogue_state == 2;
                bool is_online = !user->consume_pending && user->offer_id != 0;
                if (is_local || is_online)
                {
                    m_slots[m_slot_count].type = 3;
                    ++m_slot_count;
                }
            }
        }
    }
}



// @address 0x4afd08 ?get_text_value@c_gui_file_share_slots_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_file_share_slots_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_slot_count)
    {
        const s_gui_file_share_slot *slot = &m_slots[element_handle];
        if (slot->type == 1)
            result = (&slot->transfer.description.metadata)->get_metadata_text(value_name, value);
        else if (slot->type == 2)
            result = (&slot->file.metadata)->get_metadata_text(value_name, value);
    }
    return result;
}



// @address 0x4afd68 ?get_integer_value@c_gui_file_share_slots_datasource@@UAA_NJJPAJ@Z
bool c_gui_file_share_slots_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_slot_count)
    {
        const s_gui_file_share_slot *slot = &m_slots[element_handle];
        switch (value_name)
        {
        case _start_menu_string_display_name:
            *value = element_handle + 1;
            result = true;
            break;
        case _start_menu_string_status:
            if (slot->type == 1)
            {
                *value = slot->transfer.state;
                result = true;
            }
            break;
        case _start_menu_string_bnet_pro:
            *value = slot->type == 3;
            result = true;
            break;
        case _start_menu_string_bnet_pro_important:
        {
            long controller_index = m_controller_index;
            g_player_marketplace_users[controller_index].controller_index = controller_index;
            *value = slot->type == 3 && g_player_marketplace_users[controller_index].catalogue_state == 3;
            result = true;
            break;
        }
        default:
            if (slot->type == 1)
                result = saved_game_files_try_and_get_metadata_property(&slot->transfer.description.metadata, value_name, value);
            else if (slot->type == 2)
                result = saved_game_files_try_and_get_metadata_property(&slot->file.metadata, value_name, value);
            break;
        }
    }
    return result;
}

bool c_gui_file_share_slots_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_slot_count)
    {
        const s_gui_file_share_slot *slot = &m_slots[element_handle];
        if (value_name == _start_menu_string_status)
        {
            long type = slot->type;
            if (type == 1)
            {
                *value = -1;
                switch (slot->transfer.state)
                {
                case 1: *value = _start_menu_string_upload_pending; break;
                case 2: *value = _start_menu_string_preparing_before_upload; break;
                case 3:
                case 4: *value = _start_menu_string_uploading; break;
                case 5: *value = _start_menu_string_download_pending; break;
                case 6: *value = _start_menu_string_downloading; break;
                case 7:
                case 8: *value = _start_menu_string_preparing_after_download; break;
                }
                result = *value != -1;
            }
            else if (type == 2)
            {
                result = true;
                *value = _start_menu_string_download;
            }
            else if (type == 3)
            {
                result = true;
                *value = _start_menu_string_bnet_pro_help;
            }
            else
            {
                result = true;
                *value = m_show_quota ? _start_menu_string_empty_owner : _start_menu_string_empty;
            }
        }
    }
    return result;
}

// @address 0x4affd8 ??0c_gui_start_menu_hq_service_record_file_share@@QAA@J@Z
c_gui_start_menu_hq_service_record_file_share::c_gui_start_menu_hq_service_record_file_share(long name) :
    c_gui_start_menu_hq_service_record_pane(name)
{
    m_controller_state = 0;
    m_controller_state_changed = false;
}

// @address 0x4c0f98 ?post_initialize@c_gui_start_menu_hq_service_record_file_share@@UAAXXZ
void c_gui_start_menu_hq_service_record_file_share::post_initialize()
{
    c_gui_screen_widget::post_initialize();
}

// @address 0x4b2e50 ?create_bitmap_widget@c_gui_start_menu_hq_service_record_file_share@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_start_menu_hq_service_record_file_share::create_bitmap_widget(
    const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _start_menu_string_game_variant)
        return new(0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    return new(0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}

// @address 0x4b1ed8 ?handle_controller_input_message@c_gui_start_menu_hq_service_record_file_share@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_start_menu_hq_service_record_file_share::handle_controller_input_message(const c_controller_input_message *message)
{
    return c_start_menu_pane_screen_widget::handle_controller_input_message(message);
}

static bool parse_xml_bnet_pro_button(void *context, wchar_t *buffer, long buffer_length);
static bool parse_xml_bnet_pro_message(void *context, wchar_t *buffer, long buffer_length);
static bool parse_xml_file_share_user_name(void *context, wchar_t *buffer, long buffer_length);

void c_gui_start_menu_hq_service_record_file_share::initialize()
{
    m_owner_xuid = get_owner_xuid();
    if (m_name == _start_menu_string_start_menu_file_share)
    {
        add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<quota-used", this, parse_xml_quota_used));
        add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<quota-total", this, parse_xml_quota_total));
        add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<quota-remaining", this, parse_xml_quota_remaining));
        add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
                c_magic_string_game_tag_parser(L"<current-progress", this, (bool (*)(void *, wchar_t *, long))parse_xml_file_share_progress));
        add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
                c_magic_string_game_tag_parser(L"<bnet-pro-button", this, parse_xml_bnet_pro_button));
        add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
                c_magic_string_game_tag_parser(L"<bnet-pro-message", this, parse_xml_bnet_pro_message));
        add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
                c_magic_string_game_tag_parser(L"<bnet-pro-help", this, parse_xml_file_share_user_name));
        m_player_xuid = m_target_xuid;
    }
    else if (m_name == _start_menu_string_start_menu_bungie_file_share)
    {
        add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
                c_magic_string_game_tag_parser(L"<current-progress", this, (bool (*)(void *, wchar_t *, long))parse_xml_file_share_progress));
        m_player_xuid = -255;
    }

    if (m_owner_xuid && m_player_xuid)
    {
        gui_start_menu_hq_service_record_file_share_start_enumerator(
            &m_enumerator, m_owner_xuid, m_player_xuid);
    }
    else
    {
        transition_out_with_transition_type(0, _screen_transition_normal);
    }

    c_online_file_manager *manager = (c_online_file_manager *)(&g_online_file_manager);
    m_enumerator.m_active_transfer_token = manager->m_active_transfer_tracker.m_refresh_count;
    e_controller_index controller_index = get_any_responding_controller();
    g_player_marketplace_users[controller_index].controller_index = controller_index;
    m_controller_state = g_player_marketplace_users[controller_index].controller_state;
    c_gui_start_menu_hq_service_record_pane::initialize();
}


static __forceinline void gui_start_menu_hq_service_record_file_share_update_stop_enumerator(s_file_share_enumerator_state *enumerator)
{
    ((c_online_files_enumerator *)enumerator)->stop();
    enumerator->m_quota_bytes = 0;
    enumerator->m_quota_slots = 0;
    enumerator->m_visible_slots = 0;
}

static __forceinline void gui_start_menu_hq_service_record_file_share_update_reset_enumerator(
    s_file_share_enumerator_state *enumerator, unsigned __int64 owner_xuid, unsigned __int64 target_xuid)
{
    enumerator->m_xuid = owner_xuid;
    enumerator->m_state = 0;
    enumerator->m_quota_bytes = 0;
    enumerator->m_quota_slots = 0;
    enumerator->m_owner_xuid = target_xuid;
    enumerator->m_visible_slots = 0;
}

// @address 0x4b04d8 ?dispose@c_gui_start_menu_hq_service_record_file_share@@UAAXXZ
void c_gui_start_menu_hq_service_record_file_share::dispose()
{
    ((c_online_files_enumerator *)&m_enumerator)->stop();
    c_gui_screen_widget::dispose();
}

__forceinline c_gui_file_share_slots_datasource::c_gui_file_share_slots_datasource(
    c_online_files_enumerator *enumerator, long controller_index, bool show_quota)
    : m_enumerator(enumerator), m_controller_index(controller_index), m_slot_count(0), m_show_quota(show_quota)
{
}

void c_gui_start_menu_hq_service_record_file_share::initialize_datasource()
{
    c_gui_start_menu_hq_service_record_pane::initialize_datasource();
    void *storage = operator new(sizeof(c_gui_file_share_slots_datasource), 0, 0, _ui_allocation_marker_dummy);
    c_gui_file_share_slots_datasource *datasource = 0;
    if (storage)
    {
        long controller_index = get_any_responding_controller();
        bool show_quota = gui_start_menu_hq_service_record_file_share_is_owner(this);
        datasource = new (storage) c_gui_file_share_slots_datasource(
            (c_online_files_enumerator *)&m_enumerator, controller_index, show_quota);
    }
    if (datasource)
    {
        if (((c_gui_data *)datasource)->initialize(_start_menu_string_slots))
        {
            m_datasource[m_datasource_count++] = (c_gui_data *)datasource;
            return;
        }
        ((c_gui_data *)datasource)->~c_gui_data();
        user_interface_free(datasource);
    }
}



// @address 0x4b0ca0 ?get_quota_flags@c_gui_start_menu_hq_service_record_file_share@@AAAXPAVc_gui_list_widget@@JPA_N1@Z
void c_gui_start_menu_hq_service_record_file_share::get_quota_flags(
    c_gui_list_widget *list, long element_handle, bool *has_first, bool *has_second)
{
    c_gui_data *data = list ? (list)->get_data() : 0;
    if (has_first)
        *has_first = false;
    if (has_second)
        *has_second = false;
    if (data)
    {
        long first = 0;
        long second = 0;
        if (!data->get_integer_value(element_handle, _start_menu_string_bnet_pro, &first))
            return;
        if (!data->get_integer_value(element_handle, _start_menu_string_bnet_pro_important, &second))
            return;
        if (has_first)
            *has_first = first != 0;
        if (!has_second)
            return;
        *has_second = second != 0;
    }
}



static __forceinline bool gui_start_menu_hq_service_record_file_share_is_in_progress(long state)
{
    return state == 1 || state == 2 || state == 3 || state == 4;
}

static __forceinline void gui_start_menu_hq_service_record_file_share_set_child_flag(c_gui_widget *widget, e_gui_widget_type type, long name, bool value)
{
    c_gui_widget *child = widget->get_child_widget(type, name);
    if (child)
        gui_widget_set_alternate_state(child, value);
}

static __forceinline void gui_start_menu_hq_service_record_file_share_show_child(c_gui_widget *widget, e_gui_widget_type type, long name, bool visible)
{
    c_gui_widget *child = widget->get_child_widget(type, name);
    if (child)
        child->set_visible(visible);
}

// @address 0x4b0da0 ?update_slots@c_gui_start_menu_hq_service_record_file_share@@AAAXXZ
void c_gui_start_menu_hq_service_record_file_share::update_slots()
{
    c_gui_widget *list = get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
    if (!list)
        return;
    c_gui_data *data = ((c_gui_list_widget *)list)->get_data();
    if (!data)
        return;
    bool image_requested = false;
    s_queued_work_item_status status;
    for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget); item; item = gui_widget_get_next_list_item(item, true))
    {
        c_gui_custom_bitmap_widget *bitmap = (c_gui_custom_bitmap_widget *)item->get_child_widget(_gui_bitmap_widget, _start_menu_string_game_variant);
        long element_handle = item->get_element_handle();
        const s_file_share_enumerator_slot *slot;
        if (element_handle != -1)
            slot = ((const c_online_files_enumerator *)&m_enumerator)->get_file_share_slot(element_handle);
        else
            slot = 0;
        bool transferring = slot &&
            (&((c_online_file_manager *)&g_online_file_manager)->m_active_transfer_tracker)
                ->get_transfer_status_by_slot(m_player_xuid, element_handle, &status);
        bool occupied = !transferring && slot && slot->state == 2;
        bool has_first = false;
        bool has_second = false;
        get_quota_flags((c_gui_list_widget *)list, element_handle, &has_first, &has_second);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget, _start_menu_string_type, occupied && !has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget, _start_menu_string_map, occupied && !has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget, _start_menu_string_game_variant, occupied && !has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget, _start_menu_string_transfers, transferring && !has_first);
        long frame = transferring && gui_start_menu_hq_service_record_file_share_is_in_progress(status.state);
        c_gui_bitmap_widget *icon =
            (c_gui_bitmap_widget *)item->get_child_widget(_gui_bitmap_widget, _start_menu_string_transfers);
        if (icon)
            icon->m_bitmap_override = frame;
        gui_start_menu_hq_service_record_file_share_set_child_flag(item, _gui_bitmap_widget,
            _start_menu_string_transfers, transferring && status.state != 1 && status.state != 5);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget,
            _start_menu_string_background_color, !occupied && !transferring && !has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget, _start_menu_string_empty, !occupied && !transferring && !has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_text_widget,
            _start_menu_string_display_name, !occupied && !transferring && !has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_text_widget, _start_menu_string_bnet_pro_text, has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget, _start_menu_string_bnet_pro_background, has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget, _start_menu_string_bnet_pro_logo, has_first);
        gui_start_menu_hq_service_record_file_share_show_child(item, _gui_bitmap_widget, _start_menu_string_bnet_pro_important, has_first && has_second);
        if (bitmap && !image_requested && bitmap->bitmap_index == -1)
        {
            gui_start_menu_hq_service_record_file_share_set_map_image(bitmap, data, element_handle, 1);
            image_requested = bitmap->bitmap_index != -1;
        }
    }
}


// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z


// @address 0x4b1278 gui_start_menu_hq_service_record_file_share_update_quota
extern "C" void gui_start_menu_hq_service_record_file_share_update_quota(c_gui_start_menu_hq_service_record_file_share *screen)
{
    bool quota_text_visible = false;
    bool quota_widget_value;
    int quota_bytes_remaining = 0;
    c_gui_widget *quota_widget;
    screen->m_quota_used = -1;
    screen->m_file_count = 0;
    if (screen->m_enumerator.m_request_cookie == 0 && (unsigned char)gui_start_menu_hq_service_record_file_share_is_owner(screen) != 0) {
        long *quota_slot_count = &((c_online_files_enumerator *)&screen->m_enumerator)->m_visible_slots;
        int slot_count = *quota_slot_count;
        int slot_index = 0;
        screen->m_quota_used = 0;
        if (slot_count > 0) {
            const s_file_share_enumerator_slot *file_share_slot = ((const c_online_files_enumerator *)&screen->m_enumerator)->m_slots;
            do {
                if (slot_index >= 0 && (unsigned int)slot_index < 25 && file_share_slot != 0 && file_share_slot->state == 2) {
                    screen->m_quota_used = (&file_share_slot->description.metadata)->size_in_bytes + (unsigned long)screen->m_quota_used;
                    screen->m_file_count = screen->m_file_count + 1;
                }
                slot_index = slot_index + 1;
                file_share_slot = file_share_slot + 1;
            } while (slot_index < *quota_slot_count);
        }
        int transfer_index = 0;
        if (((c_online_file_manager *)&g_online_file_manager)->m_active_transfer_tracker.m_transfer_count > 0) {
            s_queued_work_item_status transfer_status;
            do {
                if ((&((c_online_file_manager *)&g_online_file_manager)->m_active_transfer_tracker)
                        ->get_transfer_status(transfer_index, &transfer_status))
                {
                    int transfer_uploading = transfer_status.state;
                    transfer_uploading = transfer_uploading == 1 || transfer_uploading == 2 || transfer_uploading == 3 || transfer_uploading == 4;
                    if ((unsigned int)(transfer_uploading & 255) != 0) {
                        screen->m_quota_used = transfer_status.description.metadata.size_in_bytes + (unsigned long)screen->m_quota_used;
                        screen->m_file_count = screen->m_file_count + 1;
                    }
                }
                transfer_index = transfer_index + 1;
            } while (transfer_index < ((c_online_file_manager *)&g_online_file_manager)->m_active_transfer_tracker.m_transfer_count);
        }
        quota_bytes_remaining = (int)(screen->m_enumerator.m_quota_bytes - (unsigned long)screen->m_quota_used);
    }
    if (screen->m_quota_used == -1 || screen->m_content_available == 0 || (quota_widget_value = 1, screen->m_content_busy != 0)) {
        quota_widget_value = 0;
    }
    quota_widget = screen->get_child_widget(_gui_bitmap_widget, _start_menu_string_progress);
    if (quota_widget != 0) {
        quota_widget->set_visible(quota_widget_value);
    }
    if (screen->m_quota_used == -1 || screen->m_content_available == 0 || (quota_widget_value = 1, screen->m_content_busy != 0)) {
        quota_widget_value = 0;
    }
    quota_widget = screen->get_child_widget(_gui_bitmap_widget, _start_menu_string_progress_border);
    if (quota_widget != 0) {
        quota_widget->set_visible(quota_widget_value);
    }
    quota_text_visible = false;
    if (screen->m_quota_used != -1 && screen->m_content_available != 0 && screen->m_content_busy == 0)
        quota_text_visible = true;
    quota_widget = screen->get_child_widget(_gui_text_widget, _start_menu_string_quota_remaining);
    if (quota_widget != 0) {
        quota_widget->set_visible(quota_text_visible);
    }
    long quota_text_name = quota_bytes_remaining > 0 ? _start_menu_string_quota_remaining : _start_menu_string_no_quota_remaining;
    c_gui_text_widget *quota_text_widget = (c_gui_text_widget *)screen->get_child_widget(_gui_text_widget, _start_menu_string_quota_remaining);
    if (quota_text_widget != 0) {
        quota_text_widget->set_text_from_string_id(screen, quota_text_name);
    }
}

// @address 0x4b1790 ?update_render_state@c_gui_start_menu_hq_service_record_file_share@@UAAXK@Z

static __forceinline float c_gui_start_menu_hq_service_record_file_share_quota_ratio(const c_gui_start_menu_hq_service_record_file_share *screen)
{
    float ratio = 0.0f;
    if ((long)screen->m_enumerator.m_quota_bytes != 0)
        ratio = (float)screen->m_quota_used / (float)(long)screen->m_enumerator.m_quota_bytes;
    return ratio;
}

void c_gui_start_menu_hq_service_record_file_share::update_render_state(unsigned long current_milliseconds)
{
    c_gui_start_menu_hq_service_record_pane::update_render_state(current_milliseconds);
    c_gui_widget *quota_bar = get_child_widget(_gui_bitmap_widget, _start_menu_string_progress);
    long quota_used = get_quota_used();
    if (quota_used != -1 && quota_bar)
    {
        float ratio = MIN(MAX(c_gui_start_menu_hq_service_record_file_share_quota_ratio(this), 0.0f), 1.0f);
        s_animation_transform state;
        memcpy(&state, quota_bar->get_animated_state(), sizeof(state));
        state.scale.i *= ratio;
        quota_bar->set_full_animation_state(&state, false);
        quota_bar->set_visible(ratio > 0.0f);
    }
}
#undef MIN
#undef MAX

// @address 0x4b18e0 gui_start_menu_hq_service_record_file_share_is_owner
bool gui_start_menu_hq_service_record_file_share_is_owner(void *screen)
{
    c_gui_start_menu_hq_service_record_file_share *file_share_screen = (c_gui_start_menu_hq_service_record_file_share *)screen;
    unsigned __int64 owner_xuid = file_share_screen->get_owner_xuid();
    return owner_xuid != 0 && owner_xuid == file_share_screen->m_player_xuid;
}

// @address 0x4b1ee0 ?parse_xml_quota_used@@YA_NPAXPA_WJ@Z
bool parse_xml_quota_used(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_static_wchar_string<1024> string;
    c_gui_start_menu_hq_service_record_file_share *screen = (c_gui_start_menu_hq_service_record_file_share *)context;
    long quota_used = screen->get_quota_used();
    if (quota_used != -1 && user_interface_get_bytes_size_string(quota_used, &string))
    {
        string.copy_to(buffer, buffer_length);
        success = true;
    }
    return success;
}

// @address 0x4b1f50 ?parse_xml_quota_total@@YA_NPAXPA_WJ@Z
bool parse_xml_quota_total(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_static_wchar_string<1024> string;
    c_gui_start_menu_hq_service_record_file_share *screen = (c_gui_start_menu_hq_service_record_file_share *)context;
    long quota_total = screen->get_quota_total();
    if (quota_total != -1 && user_interface_get_bytes_size_string(quota_total, &string))
    {
        string.copy_to(buffer, buffer_length);
        success = true;
    }
    return success;
}

// @address 0x4b1fd0 ?parse_xml_quota_remaining@@YA_NPAXPA_WJ@Z
bool parse_xml_quota_remaining(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_static_wchar_string<1024> string;
    c_gui_start_menu_hq_service_record_file_share *screen = (c_gui_start_menu_hq_service_record_file_share *)context;
    long quota_used = screen->get_quota_used();
    long quota_total = screen->get_quota_total();
    if (quota_total != -1 && quota_used != -1 && user_interface_get_bytes_size_string((quota_total - quota_used > 0) ? quota_total - quota_used : 0, &string))
    {
        string.copy_to(buffer, buffer_length);
        success = true;
    }
    return success;
}

extern "C" bool parse_xml_file_share_progress(void *context, wchar_t *buffer, unsigned long buffer_length)
{
    s_queued_work_item_status status;
    c_static_wchar_string<1024> text;

    c_online_file_manager *manager = (c_online_file_manager *)(&g_online_file_manager);
    if (manager->m_active_transfer_tracker.m_transfer_count > 0)
    {
        internal_critical_section_enter(29);
        status = manager->m_active_transfer_tracker.m_transfers[0].status;
        internal_critical_section_leave(29);
    }

    text.print(L"%d", (long)(status.progress * 100.0f));
    text.copy_to(buffer, buffer_length);
    return true;
}

// Post-March parsers (the March 2007 build has no bnet pro or file share user name tags): only initialize()
// registers them, for the <bnet-pro-button, <bnet-pro-message and <bnet-pro-help tags. Named by
// this file's parse_xml_<tag> convention.

// @address 0x4b2110 ?parse_xml_bnet_pro_button@@YA_NPAXPA_WJ@Z
static bool parse_xml_bnet_pro_button(void *context, wchar_t *buffer, long buffer_length)
{
    unsigned int controller_index = ((c_gui_widget *)context)->get_any_responding_controller();
    bool success = false;
    if (controller_index <= 3 && g_controller_globals.controllers[controller_index].in_use())
    {
        g_player_marketplace_users[controller_index].controller_index = controller_index;
        s_player_marketplace_user *user = &g_player_marketplace_users[controller_index];
        if (user->signed_in)
        {
            c_static_wchar_string<256> name(user->strings[2].get_string());
            name.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4b21b8 ?parse_xml_bnet_pro_message@@YA_NPAXPA_WJ@Z
static bool parse_xml_bnet_pro_message(void *context, wchar_t *buffer, long buffer_length)
{
    unsigned int controller_index = ((c_gui_widget *)context)->get_any_responding_controller();
    bool success = false;
    if (controller_index <= 3 && g_controller_globals.controllers[controller_index].in_use())
    {
        g_player_marketplace_users[controller_index].controller_index = controller_index;
        s_player_marketplace_user *user = &g_player_marketplace_users[controller_index];
        if (user->signed_in)
        {
            c_static_wchar_string<256> name(user->strings[3].get_string());
            name.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4b2260 ?parse_xml_file_share_user_name@@YA_NPAXPA_WJ@Z
static bool parse_xml_file_share_user_name(void *context, wchar_t *buffer, long buffer_length)
{
    unsigned int controller_index = ((c_gui_widget *)context)->get_any_responding_controller();
    bool success = false;
    if (controller_index <= 3 && g_controller_globals.controllers[controller_index].in_use())
    {
        g_player_marketplace_users[controller_index].controller_index = controller_index;
        s_player_marketplace_user *user = &g_player_marketplace_users[controller_index];
        if (user->signed_in)
        {
            c_static_wchar_string<256> name(user->strings[4].get_string());
            name.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}


static __forceinline void gui_start_menu_hq_service_record_file_share_set_child_visible(c_gui_widget *screen, e_gui_widget_type type, long name, bool visible)
{
    c_gui_widget *child = screen->get_child_widget(type, name);
    if (child)
        child->set_visible(visible);
}

// @address 0x4b1508 ?update_empty@c_gui_start_menu_hq_service_record_file_share@@AAAXXZ
void c_gui_start_menu_hq_service_record_file_share::update_empty()
{
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
    bool has_first = false;
    bool empty_slot = false;
    if (list)
    {
        long element_handle = (list)->get_focused_element_handle();
        const s_file_share_enumerator_slot *slot;
        if (element_handle != -1)
            slot = ((const c_online_files_enumerator *)&m_enumerator)->get_file_share_slot(element_handle);
        else
            slot = 0;
        s_queued_work_item_status status;
        bool transferring = slot &&
            (&((c_online_file_manager *)&g_online_file_manager)->m_active_transfer_tracker)
                ->get_transfer_status_by_slot(m_player_xuid, element_handle, &status);
        bool occupied = !transferring && slot && slot->state == 2;
        get_quota_flags(list, element_handle, &has_first, 0);
        empty_slot = !occupied && !transferring && !has_first;
    }
    gui_start_menu_hq_service_record_file_share_set_child_visible(this, _gui_group_widget, _start_menu_string_empty_owner,
        empty_slot && gui_start_menu_hq_service_record_file_share_is_owner(this) && m_content_available && !m_content_busy);
    gui_start_menu_hq_service_record_file_share_set_child_visible(this, _gui_group_widget, _start_menu_string_empty,
        empty_slot && !gui_start_menu_hq_service_record_file_share_is_owner(this) && m_content_available && !m_content_busy);
    gui_start_menu_hq_service_record_file_share_set_child_visible(this, _gui_group_widget, _start_menu_string_bnet_pro,
        has_first && m_content_available && !m_content_busy);
    gui_start_menu_hq_service_record_file_share_set_child_visible(this, _gui_group_widget, _start_menu_string_file_info_block,
        !empty_slot && !has_first && m_content_available && !m_content_busy);
}



// @address 0x70d3a4 RtlLeaveCriticalSection

static __forceinline void gui_start_menu_hq_service_record_file_share_critical_section_leave(long index)
{
    _InterlockedDecrement(&g_synch_globals.critical_sections[index].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[index].critical_section);
}

static __forceinline long gui_start_menu_hq_service_record_file_share_find_transfer(
    const c_online_files_active_transfer_tracker *tracker, unsigned __int64 owner_xuid, long slot)
{
    long transfer_index = -1;
    internal_critical_section_enter(29);
    for (long index = 0; index < tracker->m_transfer_count; index++)
    {
        if (tracker->m_transfers[index].status.owner_xuid == owner_xuid && tracker->m_transfers[index].status.slot == slot)
        {
            transfer_index = index;
            break;
        }
    }
    gui_start_menu_hq_service_record_file_share_critical_section_leave(29);
    return transfer_index;
}

static __forceinline const s_queued_work_item_status_tracker *gui_start_menu_hq_service_record_file_share_get_transfer(
    const c_online_files_active_transfer_tracker *tracker, long transfer_index)
{
    c_critical_section_scope lock(29);
    return &tracker->m_transfers[transfer_index];
}

// @address 0x3bb638 ?get_transfer_status@c_online_files_active_transfer_tracker@@QBA_NJPAUs_queued_work_item_status@@@Z
// @address 0x483240 ??0c_load_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z


extern "C" {
// @address 0x515b4 ??_7c_load_file_share_choose_category_screen_message@@6B@
// @address 0x515c4 ??_7c_load_file_share_choose_item_screen_message@@6B@
// @address 0x515d4 ??_7c_load_file_share_item_selected_screen_message@@6B@
// @address 0x515e4 ??_7c_load_transfers_item_selected_screen_message@@6B@
}



// @address 0x4e8c08 ?handle_list_item_chosen@c_start_menu_pane_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z

// @address 0x4b1930 ?handle_list_item_chosen@c_gui_start_menu_hq_service_record_file_share@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_start_menu_hq_service_record_file_share::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    long element_handle = item->get_element_handle();
    if (element_handle >= 0 && element_handle < 25 && list_name == _start_menu_string_start_menu_items)
    {
        const s_file_share_enumerator_slot *slot = ((const c_online_files_enumerator *)&m_enumerator)->get_file_share_slot(element_handle);
        long has_saved_film = 0;
        if (!data->get_integer_value(element_handle, _start_menu_string_bnet_pro, &has_saved_film))
            has_saved_film = 0;

        if (!has_saved_film)
        {
            const c_online_files_active_transfer_tracker *tracker = &((c_online_file_manager *)&g_online_file_manager)->m_active_transfer_tracker;
            long transfer_index = gui_start_menu_hq_service_record_file_share_find_transfer(tracker, m_player_xuid, element_handle);
            s_queued_work_item_status status;
            if (tracker->get_transfer_status(transfer_index, &status))
            {
                c_load_transfers_item_selected_screen_message *load_message = new(NULL, 0, _ui_allocation_marker_dummy)
                    c_load_transfers_item_selected_screen_message(message->get_controller(), get_render_window(), get_name(),
                        gui_start_menu_hq_service_record_file_share_get_transfer(tracker, transfer_index)->filename, &status, transfer_index == 0);
                if (load_message)
                {
                    load_message->set_parent_screen_index(get_screen_index());
                    user_interface_messaging_post(load_message);
                }
            }
            else if (slot && slot->state == 2)
            {
                c_load_file_share_item_selected_screen_message *load_message = new(NULL, 0, _ui_allocation_marker_dummy)
                    c_load_file_share_item_selected_screen_message(message->get_controller(), get_render_window(), get_name(),
                        m_owner_xuid, m_player_xuid, element_handle, slot, &m_service_record_identity);
                if (load_message)
                {
                    load_message->set_parent_screen_index(get_screen_index());
                    user_interface_messaging_post(load_message);
                }
            }
            else if (gui_start_menu_hq_service_record_file_share_is_owner(this))
            {
                if (file_share_is_available(message->get_controller()))
                {
                    if (m_quota_used < (long)m_enumerator.get_quota_byte_count() && m_file_count < (long)m_enumerator.m_quota_slots)
                    {
                        if (get_alpha_is_internal_beta())
                        {
                            c_load_file_share_choose_item_screen_message *load_message = new(NULL, 0, _ui_allocation_marker_dummy)
                                c_load_file_share_choose_item_screen_message(message->get_controller(), get_render_window(), get_name(),
                                    m_owner_xuid, m_player_xuid, element_handle, m_quota_used, (long)m_enumerator.m_quota_bytes,
                                    _saved_game_file_type_bmf);
                            if (load_message)
                            {
                                load_message->set_parent_screen_index(get_screen_index());
                                user_interface_messaging_post(load_message);
                            }
                        }
                        else
                        {
                            c_load_file_share_choose_category_screen_message *load_message = new(NULL, 0, _ui_allocation_marker_dummy)
                                c_load_file_share_choose_category_screen_message(message->get_controller(), get_render_window(), get_name(),
                                    m_owner_xuid, m_player_xuid, element_handle, m_quota_used, (long)m_enumerator.m_quota_bytes);
                            if (load_message)
                            {
                                load_message->set_parent_screen_index(get_screen_index());
                                user_interface_messaging_post(load_message);
                            }
                        }
                    }
                    else
                    {
                        e_controller_index controller_index = get_any_responding_controller();
                        g_player_marketplace_users[controller_index].controller_index = controller_index;
                        s_player_marketplace_user *user = &g_player_marketplace_users[controller_index];
                        if (user->signed_in)
                        {
                            c_static_wchar_string<1024> name(user->strings[7].get_string());
                            if (name.length() > 0)
                                g_gui_error_manager.post_error_with_custom_message(0x20008, 0, name.m_string, message->get_controller(), false, true);
                        }
                        else
                            user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
                    }
                }
                else
                    g_gui_error_manager.post_error_with_custom_message(0x20082, 0, 0, message->get_controller(), false, true);
            }
            handled = true;
        }
        else
        {
            e_controller_index controller_index = get_any_responding_controller();
            g_player_marketplace_users[controller_index].controller_index = controller_index;
            s_player_marketplace_user *user = &g_player_marketplace_users[controller_index];
            if (user->signed_in)
            {
                if (!user->consume_pending && user->offer_id != 0)
                {
                    marketplace_consume_assets(user, user->offer_id);
                    handled = true;
                }
                else if (user->catalogue_state == 2)
                {
                    e_controller_index error_controller = get_any_responding_controller();
                    g_gui_error_manager.post_error_with_custom_message(0x20006, 0, user->signed_in ? user->strings[6].get_string() : 0,
                        error_controller, false, true);
                    handled = true;
                }
            }
        }
    }

    bool result;
    if (!handled)
    {
        result = false;
        bool quota_list = false;
        if (list_name == _start_menu_string_insignia_data)
            quota_list = true;
        if (!quota_list)
            return ((::c_start_menu_pane_screen_widget *)this)->c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list_name, item, data);
    }
    else
        result = handled;
    return result;
}

// @address 0x4b0510 ?update@c_gui_start_menu_hq_service_record_file_share@@UAAXK@Z
void c_gui_start_menu_hq_service_record_file_share::update(unsigned long current_milliseconds)
{
    c_online_file_manager *manager = (c_online_file_manager *)&g_online_file_manager;
    long dirty_token = manager->m_active_transfer_tracker.m_refresh_count;
    e_controller_index controller_index = get_any_responding_controller();
    g_player_marketplace_users[controller_index].controller_index = controller_index;
    long controller_state = g_player_marketplace_users[controller_index].controller_state;
    c_gui_widget *preview_group = get_child_widget(_gui_group_widget, _start_menu_string_file_info_block);
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
    c_gui_data *data = list ? list->get_data() : 0;
    long fallback;
    e_controller_index current_controller = get_any_responding_controller();
    m_content_available = controller_is_live_content_available(current_controller, &fallback);
    if (m_content_available)
    {
        if (dirty_token != m_enumerator.m_active_transfer_token ||
            (gui_start_menu_hq_service_record_file_share_is_owner(this) && controller_state != m_controller_state))
        {
            ((c_online_files_enumerator *)&m_enumerator)->stop();
            gui_start_menu_hq_service_record_file_share_update_reset_enumerator(&m_enumerator, m_owner_xuid, m_player_xuid);
            long old_controller_state = m_controller_state;
            m_enumerator.m_active_transfer_token = dirty_token;
            m_controller_state = controller_state;
            m_enumerator.started = true;
            m_controller_state_changed = (old_controller_state - controller_state) != 0;
        }
        gui_start_menu_hq_service_record_file_share_update_enumerator(this);
        m_content_busy = m_enumerator.m_request_cookie == 0 && m_enumerator.m_visible_slots == 0;
        set_child_visible(_gui_group_widget, _start_menu_string_need_live_account, false);
        set_child_visible(_gui_group_widget, _start_menu_string_need_hdd, false);
        set_child_visible(_gui_group_widget, _start_menu_string_need_hdd_and_live_account, false);
        set_child_visible(_gui_group_widget, _start_menu_string_failed, m_content_busy);
    }
    else
    {
        gui_start_menu_hq_service_record_file_share_update_stop_enumerator(&m_enumerator);
        m_content_busy = false;
        m_enumerator.m_active_transfer_token = 0;
        set_child_visible(_gui_group_widget, _start_menu_string_need_live_account, fallback == 0);
        set_child_visible(_gui_group_widget, _start_menu_string_need_hdd, fallback == 1);
        set_child_visible(_gui_group_widget, _start_menu_string_need_hdd_and_live_account, true);
        set_child_visible(_gui_group_widget, _start_menu_string_failed, false);
    }
    update_slots();
    gui_start_menu_hq_service_record_file_share_update_quota(this);
    update_empty();
    set_child_enabled(_gui_list_widget, _start_menu_string_start_menu_items, !m_enumerator.is_busy());
    set_child_visible(_gui_group_widget, _start_menu_string_in_progress, m_enumerator.is_busy());
    unsigned __int64 owner_xuid = get_owner_xuid();
    bool is_owner = owner_xuid != 0 && owner_xuid == m_player_xuid;
    c_gui_widget *owner_options = get_child_widget(_gui_group_widget, _start_menu_string_owner);
    if (owner_options)
        owner_options->set_visible(is_owner);
    owner_xuid = get_owner_xuid();
    is_owner = owner_xuid != 0 && owner_xuid == m_player_xuid;
    c_gui_widget *other_options = get_child_widget(_gui_group_widget, _start_menu_string_not_owner);
    if (other_options)
        other_options->set_visible(!is_owner);
    if (list && data && data->get_current_item_count() > 0)
    {
        long selected_file = -1;
        if (g_file_share_selected_slot != -1 && !m_enumerator.is_busy())
        {
            transfer_focus_to_list(list, g_file_share_selected_slot, false, false);
            g_file_share_selected_slot = -1;
        }
        if (!data->get_integer_value(list->get_focused_element_handle(), _start_menu_string_map_id, &selected_file))
            selected_file = -1;
        if (preview_group)
        {
            c_gui_custom_bitmap_widget *preview = (c_gui_custom_bitmap_widget *)preview_group->get_child_widget(
                _gui_bitmap_widget, _start_menu_string_game_variant);
            if (preview)
                gui_start_menu_hq_service_record_file_share_set_map_image(preview, data, list->get_focused_element_handle(), 1);
        }
    }
    c_gui_start_menu_hq_service_record_pane::update(current_milliseconds);
}

typedef char gui_file_share_user_state_offset[(offsetof(s_player_marketplace_user, catalogue_state) == 0x18) ? 1 : -1];
typedef char gui_file_share_user_offer_offset[(offsetof(s_player_marketplace_user, offer_id) == 0x20) ? 1 : -1];
typedef char gui_file_share_user_size[(sizeof(s_player_marketplace_user) == 0x1050) ? 1 : -1];

typedef char file_share_enumerator_busy_offset[(sizeof(void *) != 4
    || (offsetof(c_gui_start_menu_hq_service_record_file_share, m_enumerator) + offsetof(s_file_share_enumerator_state, m_request_cookie) == 6952)) ? 1 : -1];
typedef char file_share_quota_used_offset[(sizeof(void *) != 4 || (offsetof(c_gui_start_menu_hq_service_record_file_share, m_quota_used) == 31488)) ? 1 : -1];
typedef char file_share_content_available_offset[(sizeof(void *) != 4
    || (offsetof(c_gui_start_menu_hq_service_record_file_share, m_content_available) == 31501)) ? 1 : -1];
typedef char file_share_quota_bytes_offset[(sizeof(void *) != 4
    || (offsetof(c_gui_start_menu_hq_service_record_file_share, m_enumerator) + offsetof(s_file_share_enumerator_state, m_quota_bytes) == 23344)) ? 1 : -1];
typedef char file_share_enumerator_slots_offset[(sizeof(void *) != 4
    || (offsetof(c_gui_start_menu_hq_service_record_file_share, m_enumerator) + offsetof(c_online_files_enumerator, m_slots) == 23880)) ? 1 : -1];

// @address 0x72fc94 g_file_share_selected_slot
extern "C" { long g_file_share_selected_slot = -1; }

