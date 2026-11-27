// interfacelibxbox/gui_start_menu_hq_service_record.obj: from the records 0x4ae1e0 0x4aea40 0x4aeb28 0x4aeb68 0x4aeba8 0x4aec80 0x4af090 0x4af688 0x4af700 0x4af738 0x4af7b0 0x4af810 0x4af870
// @flags /TP /O2 /Oc
#include "networking/online/online_url_version.h"
#include "cseries/cseries_unicode.h"
#include <stddef.h>
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_widget.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/gui_data.h"
#include "memory/static_arrays.h"
#include "game/player_types.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "interface/user_interface_utilities.h"
#include "game/multiplayer_game_hopper.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "networking/online/online_url.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_text_widget.h"
#include "networking/session/network_session_interface_types.h"
#include "interface/experience_rank_types.h"
#include "interface/experience_grade_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "networking/tools/network_blf.h"
#include "interface/user_interface_allocation.h"
#include "text/text_group.h"
#include "interface/user_interface_controller.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "interface/gui_model_widget_types.h"
#include "interface/gui_start_menu_hq_service_record_externs.h"
#include "cseries/language.h"
#include "interface/c_player_marketplace.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/gui_text_widget_colors.h"
#include "networking/session/network_session_interface_user_externs.h"
#include "interface/gui_screen_widget_legacy.h"
#include "interface/controller_runtime_storage.h"
#include "shell/utility_drive.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/service_record_pane.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "networking/tools/blf_authentication_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record/gui_start_menu_hq_service_record.h"
#include "interface/gui_roster_data.h"
#include "main/player_profile_summary_types.h"
#include "networking/network_configuration.h"
//  0x74b540 g_current_language
// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x8a1ead g_utility_drive_mounted
// @address 0x4842c0 ?tint_widget_to_change_color@@YAXPAVc_gui_widget@@J_N@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x4e8c08 ?handle_list_item_chosen@c_start_menu_pane_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
// @address 0x4a8838 ??_Ec_gui_start_menu_hq_service_record_pane@@UAAPAXI@Z
// @address 0x42d380 ?controller_event_should_go_to_parent@c_gui_start_menu_hq_service_record@@MAA_NPBVc_controller_input_message@@@Z
// @address 0x42d298 ?controller_event_should_go_to_parent@c_gui_start_menu_hq_service_record_pane@@MAA_NPBVc_controller_input_message@@@Z

struct s_service_record_url
{
    c_static_string<256> m_string;
    long m_service_type;
    long m_cache_mode;
    long m_lifetime_seconds;
    long m_queue_type;
};


typedef char gui_service_record_user_consume_pending_offset[(offsetof(s_player_marketplace_user, consume_pending) == 0x104c) ? 1 : -1];

typedef char gui_service_record_user_size[(sizeof(s_player_marketplace_user) == 0x1050) ? 1 : -1];

typedef char gui_service_record_motd_tag_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_service_record, m_motd_tag) == 0x122dc) )) ? 1 : -1];

static __forceinline unsigned __int64 get_controller_player_xuid(const c_controller_interface *controller)
{
    unsigned __int64 xuid = 0;
    if (controller->user_index >= 0 && controller->user_index < 4)
        xuid = g_network_session_interface_local_users[controller->user_index].xuid;
    return xuid;
}

// @address 0x4aea40 ?get_owner_xuid@c_start_menu_pane_screen_widget@@IBA_KXZ
unsigned __int64 c_start_menu_pane_screen_widget::get_owner_xuid() const
{
    return get_controller_player_xuid(
        controller_get(get_any_responding_controller()));
}

// @address 0x4aeb28 ?get_integer_value@c_gui_insignia_data@@UAA_NJJPAJ@Z
bool c_gui_insignia_data::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if ((unsigned long)element_handle <= 3)
        result = m_data[element_handle]->get_integer_value(m_selected_items[element_handle], value_name, value);
    return result;
}

// @address 0x4aeb68 ?get_string_id_value@c_gui_insignia_data@@UAA_NJJPAJ@Z
bool c_gui_insignia_data::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if ((unsigned long)element_handle <= 3)
        result = m_data[element_handle]->get_string_id_value(m_selected_items[element_handle], value_name, value);
    return result;
}

// @address 0x4aeba8 ?set_selected_item@c_gui_insignia_data@@QAAXJJ@Z
void c_gui_insignia_data::set_selected_item(long element_handle, long value)
{
    if (element_handle < 4)
    {
        c_gui_data *data = m_data[element_handle];
        if (data)
        {
            long item = data->get_first_element_handle();
            if (item != -1)
            {
                long integer;
                while (!data->get_integer_value(item, 266, &integer) || integer != value)
                {
                    item = data->get_next_element_handle(item);
                    if (item == -1)
                        return;
                }
                if (item != -1)
                    m_selected_items[element_handle] = item;
            }
        }
    }
}

// @address 0x4af688 ?update_render_state@c_gui_start_menu_hq_service_record_pane@@UAAXK@Z
void c_gui_start_menu_hq_service_record_pane::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    for (c_gui_widget *child = get_child_widget(_gui_bitmap_widget, _start_menu_string_base_color); child != 0;
        child = gui_widget_get_next_sibling_of_type(child, _gui_bitmap_widget))
    {
        if (child->m_name == _start_menu_string_base_color)
            tint_widget_to_change_color(child, m_service_record_identity.appearance.primary_color, false);
    }
}

// @address 0x4af700 ?handle_list_item_chosen@c_gui_start_menu_hq_service_record_pane@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_start_menu_hq_service_record_pane::handle_list_item_chosen(const c_controller_input_message *message, long list,
    c_gui_list_item_widget *item, c_gui_data *data)
{
    bool result = false;
    bool service_record_list = false;
    if (list == _start_menu_string_insignia_data)
        service_record_list = true;
    if (service_record_list == 0)
        result = c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list, item, data);
    return result;
}

// @address 0x4af738 ?handle_controller_input_message@c_gui_start_menu_hq_service_record_pane@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_start_menu_hq_service_record_pane::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->get_event_type() == _event_type_button_press && message->get_component() == _controller_component_button_b)
    {
        close_current_subpane();
        handled = true;
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4af7b0 service_record_get_next_rank_experience
extern "C" bool service_record_get_next_rank_experience(c_start_menu_pane_screen_widget *screen, wchar_t *buffer, int size)
{
    bool result = false;
    long value = multiplayer_game_hopper_get_next_rank_experience(screen->m_service_record_identity.experience, 0,
        screen->m_service_record_identity.highest_skill);
    if (value != -1)
    {
        usnzprintf(buffer, size, L"%d", value);
        result = true;
    }
    return result;
}

// @address 0x4af810 service_record_get_next_rank_skill
extern "C" bool service_record_get_next_rank_skill(c_start_menu_pane_screen_widget *screen, wchar_t *buffer, int size)
{
    bool result = false;
    long value = multiplayer_game_hopper_get_next_rank_skill(screen->m_service_record_identity.experience, 0, screen->m_service_record_identity.highest_skill);
    if (value != -1)
    {
        usnzprintf(buffer, size, L"%d", value);
        result = true;
    }
    return result;
}

// @address 0x4af870 ?get_emblem_info_for_widget@c_gui_start_menu_hq_service_record_pane@@UAA_NPAVc_gui_widget@@PAUs_emblem_info@@@Z
bool c_gui_start_menu_hq_service_record_pane::get_emblem_info_for_widget(c_gui_widget *widget, s_emblem_info *emblem_info)
{
    bool result = false;
    if (widget->m_name == _start_menu_string_player_emblem)
    {
        result = true;
        *emblem_info = m_service_record_identity.appearance.emblem;
    }
    if ((unsigned char)result == 0)
        return false;
    return result;
}

// @address 0x4ae1e0 ?update@c_gui_start_menu_hq_service_record@@UAAXK@Z
void c_gui_start_menu_hq_service_record::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    bool motd_changed = false;
    if (m_motd_tag != g_online_url_version)
    {
        motd_changed = true;
        m_motd_tag = g_online_url_version;
    }
    if (motd_changed)
    {
        s_service_record_url url;
        url.m_service_type = 0;
        url.m_cache_mode = 0;
        url.m_lifetime_seconds = 0x258;
        url.m_queue_type = 0;
        online_url_set_base_path((c_online_url_string *)&url);
        long language = get_current_language();
        url.m_string.append_print("%s/motd.bin", get_language_suffix((e_language)language, false));
        url.m_cache_mode = 2;
        url.m_queue_type = 1;
        http_buffer_downloader_set_url(m_motd_downloader.buffer_downloader(), (const s_network_http_buffer_url *)&url);
    }
    long motd_status;
    {
        c_service_record_motd_downloader *motd_fetcher = &m_motd_downloader;
        long motd_size = 0;
        const char *motd_data = 0;
        motd_status = motd_fetcher->get_data(&motd_data, &motd_size);
        if (motd_status == 2 && motd_data != 0)
        {
            if (motd_fetcher->m_last_retry_count != motd_fetcher->get_retry_count())
                motd_fetcher->m_data = (const s_service_record_motd_chunk *)network_blf_find_chunk_in_file(
                    motd_data, motd_size, 0x6d6f7464, 1, _blf_file_authentication_none, (unsigned long *)&motd_fetcher->m_data_size);
        }
        else
        {
            motd_fetcher->m_data = 0;
            motd_fetcher->m_data_size = 0;
        }
        const s_service_record_motd_chunk *motd_chunk = motd_fetcher->m_data;
        if (motd_status == 2 && motd_chunk != 0)
        {
            unsigned long text_size = network_blf_payload_unsigned_long(motd_chunk->text_size);
            char motd_text[512];
            wchar_t wide_text[1028];
            if (text_size > 511)
                text_size = 511;
            memcpy(motd_text, motd_chunk->text, text_size);
            motd_text[text_size] = 0;
            ::utf8_string_to_wchar_string((utf8 const *)(const unsigned char *)motd_text, (wchar_t *)((unsigned short *)wide_text),
                (int)(text_size + 1), (long *)(0));
            gui_screen_widget_set_text_widget_text((c_gui_screen_widget *)this, 0x10005, wide_text);
        }
    }
    c_gui_custom_bitmap_widget *image_widget = (c_gui_custom_bitmap_widget *)get_child_widget(_gui_bitmap_widget, 0x10325);
    if (g_utility_drive_mounted == 0 || image_widget == 0)
    {
        m_image_applied = false;
        c_gui_widget *spinner = get_child_widget(_gui_group_widget, 0x100b3);
        if (spinner != 0)
            spinner->set_visible(false);
        c_gui_widget *busy = get_child_widget(_gui_bitmap_widget, 0x10247);
        if (busy != 0)
            busy->set_visible(true);
    }
    else
    {
        long image_size = 0;
        const char *image_data = 0;
        bool image_changed = false;
        if (m_image_tag != g_online_url_version)
        {
            image_changed = true;
            m_image_tag = g_online_url_version;
        }
        if (image_changed)
        {
            s_service_record_url url;
            url.m_service_type = 0;
            url.m_cache_mode = 0;
            url.m_lifetime_seconds = 0x258;
            url.m_queue_type = 0;
            online_url_set_base_path((c_online_url_string *)&url);
            long language = get_current_language();
            url.m_string.append_print("%s/motd_image.jpg", get_language_suffix((e_language)language, false));
            url.m_cache_mode = 2;
            url.m_queue_type = 1;
            http_buffer_downloader_set_url(&m_image_downloader, (const s_network_http_buffer_url *)&url);
        }
        long image_status = m_image_downloader.get_data(&image_data, &image_size);
        if (image_status == 2 && image_data != 0)
        {
            if (!m_image_applied)
            {
                gui_custom_bitmap_widget_load_bitmap_from_buffer(image_widget, true, image_data, image_size, 0);
                image_widget->start_animation_at_time(_animation_state_custom0, 0xffffffff, true);
                m_image_applied = true;
            }
        }
        else
        {
            m_image_applied = false;
        }
        bool loading = (image_status == 1 || image_widget->bitmap_index != -1) ? true : false;
        c_gui_widget *spinner = get_child_widget(_gui_group_widget, 0x100b3);
        if (spinner != 0)
            spinner->set_visible(loading);
        bool failed = (image_status == 3 || image_status == 4) ? true : false;
        c_gui_widget *busy = get_child_widget(_gui_bitmap_widget, 0x10247);
        if (busy != 0)
            busy->set_visible(failed);
    }
    for (long datasource_index = 0; datasource_index < m_datasource_count; datasource_index++)
    {
        if (m_datasource[datasource_index]->get_name() == 0x1010f)
        {
            c_gui_data *datasource = m_datasource[datasource_index];
            if (datasource != 0)
            {
                e_controller_index controller_index = get_any_responding_controller();
                const c_controller_interface *controller = controller_get(controller_index);
                datasource->clear_disabled_elements();
                long user_index = controller->get_controller_index();
                s_player_marketplace_user *user = &g_player_marketplace_users[user_index];
                user->controller_index = user_index;
                if (!controller->is_signed_in_to_live() || !user->pending || user->consume_pending || user->offer_id == 0)
                    datasource->set_disabled_element(0x29c, 0x10250);
                if (!controller->is_signed_in_to_live())
                    datasource->set_disabled_element(0x29c, 0x1021d);
            }
            break;
        }
    }
}

// @address 0x512a4 k_next_rank_experience_tag
// @address 0x512d0 k_next_rank_skill_tag
// @rdata 0x820512a4..0x820512f4
extern "C" {
const wchar_t k_next_rank_experience_tag[] = {
    '<', 'n', 'e', 'x', 't', '-', 'r', 'a', 'n', 'k', '-', 'e', 'x', 'p', 'e', 'r', 'i', 'e', 'n', 'c', 'e', 0
};
const wchar_t k_next_rank_skill_tag[] = {
    '<', 'n', 'e', 'x', 't', '-', 'r', 'a', 'n', 'k', '-', 's', 'k', 'i', 'l', 'l', 0, 0
};
}
// @rdata-end

// @address 0x4aec80 ?initialize@c_gui_start_menu_hq_service_record_pane@@UAAXXZ
void c_gui_start_menu_hq_service_record_pane::initialize()
{
    gui_screen_widget_initialize(this);
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_next_rank_experience_tag, this, (bool (*)(void *, wchar_t *, long))service_record_get_next_rank_experience));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_next_rank_skill_tag, this, (bool (*)(void *, wchar_t *, long))service_record_get_next_rank_skill));

    if (get_owner_xuid() == get_target_xuid() && m_service_record_identity.name[0] == 0)
    {
        s_player_configuration player_configuration;
        s_service_record_identity identity;
        user_interface_controller_get_player_configuration(get_any_responding_controller(), 0, &player_configuration);
        service_record_identity_initialize_from_player(&identity, &player_configuration);
        set_service_record_identity(&identity);
    }

    bool is_owner = get_owner_xuid() == get_target_xuid() ? true : false;
    c_gui_widget *owner_group = get_child_widget(_gui_group_widget, 0x10248);
    if (owner_group != 0)
        owner_group->set_visible(is_owner);

    bool is_not_owner = get_owner_xuid() != get_target_xuid() ? true : false;
    c_gui_widget *visitor_group = get_child_widget(_gui_group_widget, 0x10249);
    if (visitor_group != 0)
        visitor_group->set_visible(is_not_owner);
}

static __forceinline bool get_global_string(long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    string->clear();
    if (string_id != -1)
    {
        const s_user_interface_shared_globals *globals = user_interface_shared_tag_globals_try_and_get();
        if (globals != 0 && globals->global_strings.index != -1)
            result = text_group_get_string(globals->global_strings.index, string_id, string);
    }
    return result;
}

static __forceinline void set_named_text_widget(c_gui_widget *screen, long name, const wchar_t *text)
{
    c_gui_text_widget *widget = static_cast<c_gui_text_widget *>(screen->get_child_widget(_gui_text_widget, name));
    if (widget != 0)
        widget->set_text(text);
}

// @address 0x4af090 ?update@c_gui_start_menu_hq_service_record_pane@@UAAXK@Z
void c_gui_start_menu_hq_service_record_pane::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    const s_service_record_identity *dynamic_identity = &m_service_record_identity;
    set_named_text_widget(this, 0x2ae, dynamic_identity->name);

    bool model_name_valid = false;
    c_static_wchar_string<1024> model_name;
    if (dynamic_identity->appearance.player_model_choice == 1)
        model_name_valid = get_global_string(198, &model_name);
    else
        model_name_valid = get_global_string(197, &model_name);
    if (model_name_valid)
    {
        c_static_wchar_string<256> model_and_service_tag;
        model_and_service_tag.print(L"%s - %s", model_name.get_string(), dynamic_identity->appearance.service_tag);
        set_named_text_widget(this, 0x103cd, model_and_service_tag.get_string());
    }

    c_gui_model_widget *model = static_cast<c_gui_model_widget *>(get_child_widget(_gui_model_widget, 0x10467));
    if (model != 0)
        model->set_player_appearance(&dynamic_identity->appearance, -1);

    c_gui_bitmap_widget *rank_bitmap = static_cast<c_gui_bitmap_widget *>(get_child_widget(_gui_bitmap_widget, 0x103bc));
    if (rank_bitmap != 0)
    {
        long rank_image = -1;
        e_experience_rank rank = dynamic_identity->rank;
        e_experience_grade grade = dynamic_identity->grade;
        if (static_cast<unsigned long>(rank) <= 13 && static_cast<unsigned long>(grade) <= 3)
            rank_image = g_experience_rank_icon_element_handles[rank][grade];
        rank_bitmap->set_sprite_frame(rank_image);
    }
    c_gui_bitmap_widget *skill_bitmap = static_cast<c_gui_bitmap_widget *>(get_child_widget(_gui_bitmap_widget, 0x103ba));
    if (skill_bitmap != 0)
        skill_bitmap->set_sprite_frame(dynamic_identity->highest_skill);

    c_gui_text_widget *rank_text = static_cast<c_gui_text_widget *>(get_child_widget(_gui_text_widget, 0x103af));
    if (rank_text != 0)
    {
        c_static_wchar_string<1024> rank_string;
        if (user_interface_get_player_rank_and_grade_string(dynamic_identity->rank, dynamic_identity->grade, true, &rank_string))
        {
            rank_text->set_visible(true);
            rank_text->set_text(rank_string.get_string());
        }
        else
            rank_text->set_visible(false);
    }
    c_gui_text_widget *experience_text = static_cast<c_gui_text_widget *>(get_child_widget(_gui_text_widget, 0x103ac));
    if (experience_text != 0)
    {
        c_static_wchar_string<1024> experience_string;
        if (user_interface_get_player_total_xp(dynamic_identity->experience, &experience_string))
        {
            experience_text->set_visible(true);
            experience_text->set_text(experience_string.get_string());
        }
        else
            experience_text->set_visible(false);
    }
    c_gui_text_widget *next_rank_text = static_cast<c_gui_text_widget *>(get_child_widget(_gui_text_widget, 0x103b0));
    if (next_rank_text != 0)
    {
        c_static_wchar_string<1024> next_rank_string;
        long experience = dynamic_identity->experience;
        long highest_skill = dynamic_identity->highest_skill;
        long next_rank_experience = multiplayer_game_hopper_get_next_rank_experience(experience, 0, highest_skill);
        long next_rank_skill = multiplayer_game_hopper_get_next_rank_skill(experience, 0, highest_skill);
        if (next_rank_experience != -1 && next_rank_skill != -1)
            get_string_by_string_id(0x103b3, &next_rank_string);
        else if (next_rank_experience != -1)
            get_string_by_string_id(0x103b1, &next_rank_string);
        else if (next_rank_skill != -1)
            get_string_by_string_id(0x103b2, &next_rank_string);
        else
            get_string_by_string_id(0x103b4, &next_rank_string);
        next_rank_text->set_visible(true);
        next_rank_text->set_text(next_rank_string.get_string());
    }

    c_gui_insignia_data *data = 0;
    for (long i = 0; i < m_datasource_count; ++i)
    {
        if (m_datasource[i]->get_name() == 0x10204)
        {
            data = static_cast<c_gui_insignia_data *>(m_datasource[i]);
            break;
        }
    }
    c_gui_widget *list = get_child_widget(_gui_list_widget, 0x10204);
    if (data != 0 && list != 0)
    {
        s_player_profile_summary profile;
        profile.global_rank = dynamic_identity->rank;
        profile.global_grade = dynamic_identity->grade;
        profile.highest_skill = dynamic_identity->highest_skill;
        profile.games_completed = dynamic_identity->games_played;
        profile.experience_base = dynamic_identity->hopper_weight;
        long player_class = network_configuration_get_player_class(&profile);
        bool has_rank = player_class >= 2;
        bool has_grade = player_class >= 3;
        const long &has_skill = static_cast<unsigned char>(player_class >= 4);
        long color_index = dynamic_identity->color_index;
        data->set_selected_item(0, has_rank);
        data->set_selected_item(1, has_grade);
        const unsigned char &skill_value = static_cast<unsigned char>(has_skill);
        data->set_selected_item(2, skill_value);
        data->set_selected_item(3, color_index);
        list->set_visible(true);
        c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget);
        while (item != 0)
        {
            for (long i = 0; i < 4; ++i)
            {
                c_gui_bitmap_widget *bitmap = static_cast<c_gui_bitmap_widget *>(item->get_child_widget(_gui_bitmap_widget, data->get_bitmap_name(i)));
                if (bitmap != 0)
                {
                    long name = bitmap->get_name();
                    bool visible = data->bitmap_is_visible(item->get_element_handle(), name);
                    bitmap->set_visible(visible);
                    if (visible)
                    {
                        long frame = -1;
                        if (data->get_integer_value(item->get_element_handle(), 0x3f5, &frame))
                            bitmap->set_sprite_frame(frame);
                    }
                }
            }
            long frame;
            gui_widget_set_alternate_state(item, !data->get_sprite_frame(item->get_element_handle(), &frame));
            item = gui_widget_get_next_sibling_of_type(item, _gui_list_item_widget);
        }
    }
}

