// interfacelibxbox/gui_roster_list_widget.obj: from the records 0x4ea728 0x4ea7b0 0x4eb2a8
// @flags /TP /O2 /Oc
#include "interface/gui_widget_types.h"
#include "interface/gui_roster_list_externs.h"
#include "game/game_engine_display.h"
#include "interface/gui_data.h"
#include "cseries/cseries.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_utilities.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
// c_gui_roster_list_widget::update_render_state (gui_roster_list_widget.obj)
// @address 0x4eb2a8 ?update_render_state@c_gui_roster_list_widget@@UAAXK@Z
// @address 0x564e0 ??_7c_gui_roster_list_widget@@6B@
// @address 0x4ea728 ??0c_gui_roster_list_widget@@QAA@_N0@Z
// @address 0x4e1958 ??0c_gui_list_widget@@QAA@XZ
// @address 0x489b58 ?get_element_handle@c_gui_widget@@UAAJXZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z
// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
// @address 0x4842c0 ?tint_widget_to_change_color@@YAXPAVc_gui_widget@@J_N@Z

c_gui_roster_list_widget::c_gui_roster_list_widget(bool game_roster_is_local_party, bool is_team_game)
{
    m_game_roster_is_local_party = game_roster_is_local_party;
    m_team_game = false;
    m_show_party_indicators = false;
    for (long i = 0; i < 16; i = i + 1)
    {
        m_teams[i].active = false;
        m_teams[i].color = -1;
        m_teams[i].change_start_time = 0;
        m_teams[i].display_until = 0;
    }
}

__forceinline void c_gui_roster_list_widget::update_team_mode()
{
    if (m_game_roster_is_local_party)
    {
        m_team_game = user_interface_networking_should_show_teams();
        m_team_switching_enabled = user_interface_networking_is_team_switching_allowed();
    }
    if (!m_team_switching_enabled)
    {
        for (long team_index = 0; team_index < 16; team_index++)
        {
            if (m_teams[team_index].active)
            {
                m_teams[team_index].active = false;
                m_teams[team_index].change_start_time = 0;
                m_teams[team_index].display_until = 0;
            }
        }
    }
}

// @address 0x4ea7b0
void c_gui_roster_list_widget::update(unsigned long current_milliseconds)
{
    c_gui_data *data = get_data();
    long selectable_item_count = 0;
    update_team_mode();
    if (data)
    {
        for (c_gui_widget *list_item = gui_widget_find_child_by_name(this, _gui_list_item_widget); list_item;
            list_item = gui_widget_get_next_list_item(list_item, 1))
        {
            c_gui_bitmap_widget *base_color_bitmap = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 295);
            c_gui_bitmap_widget *rank_bitmap = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 952);
            c_gui_text_widget *name_text = (c_gui_text_widget *)list_item->get_child_widget(_gui_text_widget, 668);
            c_gui_text_widget *name_text_focused = (c_gui_text_widget *)list_item->get_child_widget(_gui_text_widget, 0x10000 + 949);
            c_gui_bitmap_widget *status_bitmap = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 11);
            c_gui_widget *searching_text = list_item->get_child_widget(_gui_text_widget, 280);
            c_gui_widget *searching_bitmap = list_item->get_child_widget(_gui_bitmap_widget, 280);
            c_gui_widget *joining_text = list_item->get_child_widget(_gui_text_widget, 0x10000 + 962);
            c_gui_widget *joining_bitmap = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 962);
            c_gui_widget *joining_bitmap_1 = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 963);
            c_gui_widget *joining_bitmap_2 = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 964);
            c_gui_widget *empty_text = list_item->get_child_widget(_gui_text_widget, 0x10000 + 965);
            c_gui_widget *empty_bitmap = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 965);
            c_gui_widget *voice_talking_bitmap = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 984);
            c_gui_bitmap_widget *voice_bitmap = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 985);
            c_gui_widget *player_bitmap_0 = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 950);
            c_gui_widget *player_bitmap_1 = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 951);
            c_gui_bitmap_widget *skill_bitmap_0 = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 954);
            c_gui_bitmap_widget *skill_bitmap_1 = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 955);
            c_gui_bitmap_widget *experience_bitmap_0 = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 956);
            c_gui_bitmap_widget *experience_bitmap_1 = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 957);
            c_gui_widget *player_bitmap_2 = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 966);
            c_gui_widget *player_bitmap_3 = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 967);
            long element_handle = list_item->get_element_handle();
            long team_index = -1;
            long player_state = 0;
            long status_frame = -1;
            long status_type = 0;
            long voice_state = 0;
            long experience_frame = -1;
            long skill_frame = -1;
            long rank_flags = 0;
            if (base_color_bitmap && rank_bitmap && name_text && name_text_focused && status_bitmap && voice_bitmap &&
                searching_text && searching_bitmap && empty_text && empty_bitmap && voice_talking_bitmap &&
                joining_text && joining_bitmap && joining_bitmap_1 && joining_bitmap_2 &&
                player_bitmap_0 && player_bitmap_1 && skill_bitmap_0 && skill_bitmap_1 &&
                experience_bitmap_0 && experience_bitmap_1 && player_bitmap_2 && player_bitmap_3 &&
                data->get_integer_value(element_handle, 0x10000 + 20, &team_index) &&
                data->get_integer_value(element_handle, 0x10000 + 936, &player_state) &&
                data->get_integer_value(element_handle, 0x10000 + 22, &status_frame) &&
                data->get_integer_value(element_handle, 0x10000 + 491, &status_type) &&
                data->get_integer_value(element_handle, 0x10000 + 983, &voice_state) &&
                data->get_integer_value(element_handle, 0x10000 + 956, &experience_frame) &&
                data->get_integer_value(element_handle, 0x10000 + 954, &skill_frame) &&
                data->get_integer_value(element_handle, 0x10000 + 986, &rank_flags))
            {
                long state = player_state;
                bool player_present = state == 0;
                bool player_joining = state == 2;
                bool player_empty = state == 1;
                bool player_searching = state == 3;
                long status = status_type;
                long voice = voice_state;
                bool text_set = false;
                bool team_game = get_is_team_game();
                long rank_frame = 0;
                if (player_present)
                    selectable_item_count++;

                for (c_gui_widget *child = list_item->get_children(); child; child = child->get_next())
                    gui_widget_set_alternate_state(child, voice == 1);

                list_item->set_enabled(player_present);
                searching_bitmap->set_visible(player_searching);
                searching_text->set_visible(player_searching);
                joining_bitmap->set_visible(player_joining);
                joining_bitmap_1->set_visible(player_joining);
                joining_bitmap_2->set_visible(player_joining);
                joining_text->set_visible(player_joining);
                empty_bitmap->set_visible(player_empty);
                empty_text->set_visible(player_empty);
                voice_bitmap->set_visible(voice == 2 || voice == 3 || voice == 4);
                voice_talking_bitmap->set_visible(voice == 3 || voice == 4);

                if (TEST_BIT(rank_flags, 2))
                    rank_frame = 3;
                else if (TEST_BIT(rank_flags, 1))
                    rank_frame = 2;
                else if (TEST_BIT(rank_flags, 0))
                    rank_frame = 1;
                base_color_bitmap->set_sprite_frame(rank_frame);
                rank_bitmap->set_sprite_frame(rank_frame);

                name_text->set_visible(player_present);
                name_text_focused->set_visible(player_present);
                player_bitmap_0->set_visible(player_present);
                player_bitmap_1->set_visible(player_present);

                skill_bitmap_0->set_visible(skill_frame != -1);
                skill_bitmap_1->set_visible(skill_frame != -1);
                if (skill_frame != -1)
                {
                    skill_bitmap_0->set_sprite_frame(skill_frame);
                    skill_bitmap_1->set_sprite_frame(skill_frame);
                }

                experience_bitmap_0->set_visible(experience_frame != -1);
                experience_bitmap_1->set_visible(experience_frame != -1);
                if (experience_frame != -1)
                {
                    experience_bitmap_0->set_sprite_frame(experience_frame);
                    experience_bitmap_1->set_sprite_frame(experience_frame);
                }

                player_bitmap_2->set_visible(player_present);
                player_bitmap_3->set_visible(player_present);

                if (voice == 2)
                    voice_bitmap->set_sprite_frame(0);
                else if (voice == 3)
                    voice_bitmap->set_sprite_frame(1);
                else if (voice == 4)
                    voice_bitmap->set_sprite_frame(2);

                if (status == 2)
                {
                    status_bitmap->set_visible(true);
                    status_bitmap->set_sprite_frame(5);
                }
                else if (status == 3)
                {
                    status_bitmap->set_visible(true);
                    status_bitmap->set_sprite_frame(6);
                }
                else if (status_frame != -1)
                {
                    status_bitmap->set_visible(true);
                    status_bitmap->set_sprite_frame(status_frame);
                }
                else if (status == 1)
                {
                    status_bitmap->set_visible(true);
                    status_bitmap->set_sprite_frame(4);
                }
                else if (status == 4)
                {
                    status_bitmap->set_visible(true);
                    status_bitmap->set_sprite_frame(7);
                }
                else
                {
                    status_bitmap->set_visible(false);
                }

                if (team_game)
                {
                    bool team_valid = team_index != -1 && m_teams[team_index].active;
                    if (team_valid)
                    {
                        c_static_wchar_string<1024> team_text;
                        c_static_wchar_string<256> team_name;
                        wchar_t team_prefix[2] = { 0xe104, 0 };
                        wchar_t team_suffix[2] = { 0xe105, 0 };
                        team_text.append(team_prefix);
                        team_text.append(L" ");
                        game_engine_get_team_name(m_teams[team_index].color, &team_name);
                        team_text.append(team_name.get_string());
                        team_text.append(L" ");
                        team_text.append(team_suffix);
                        name_text->set_text(team_text.get_string());
                        name_text_focused->set_text(team_text.get_string());
                        text_set = true;
                    }
                }
                if (!text_set)
                {
                    c_static_wchar_string<1024> player_name;
                    if (data->get_text_value(element_handle, 686, &player_name))
                    {
                        name_text->set_text(player_name.get_string());
                        name_text_focused->set_text(player_name.get_string());
                    }
                }
            }
        }
    }
    set_selectable_item_cap_count(selectable_item_count);
    c_gui_list_widget::update(current_milliseconds);
}



void c_gui_roster_list_widget::update_render_state(unsigned long current_milliseconds)
{
    c_gui_data *data = get_data();
    this->c_gui_widget::update_render_state(current_milliseconds);
    offset_horizontal_list_item_indicators();
    if (!data)
        return;

    for (c_gui_widget *list_item = gui_widget_find_child_by_name(this, _gui_list_item_widget); list_item;
        list_item = gui_widget_get_next_list_item(list_item, 1))
    {
        c_gui_widget *base_color_bitmap = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 295);
        c_gui_widget *rank_bitmap = list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 952);
        c_gui_bitmap_widget *party_bitmap = (c_gui_bitmap_widget *)list_item->get_child_widget(_gui_bitmap_widget, 0x10000 + 969);
        c_gui_widget *name_text = list_item->get_child_widget(_gui_text_widget, 668);
        c_gui_widget *service_tag_text = list_item->get_child_widget(_gui_text_widget, 0x10000 + 973);
        long element_handle = list_item->get_element_handle();
        long team_index = -1;
        long player_missing = 0;
        long party_index = 0;
        long base_color;
        if (base_color_bitmap && rank_bitmap && party_bitmap && name_text && service_tag_text &&
            data->get_integer_value(element_handle, 0x10000 + 20, &team_index) &&
            data->get_integer_value(element_handle, 0x10000 + 936, &player_missing) &&
            data->get_integer_value(element_handle, 0x10000 + 971, &party_index) &&
            data->get_integer_value(element_handle, 0x10000 + 295, &base_color))
        {
            bool player_present = player_missing == 0;
            bool team_game = m_team_game;
            long team_color = -1;
            base_color_bitmap->set_visible(player_present);
            rank_bitmap->set_visible(player_present);
            party_bitmap->set_visible(player_present && m_show_party_indicators && party_index > 0);
            if (team_game)
            {
                bool team_active = team_index != -1 && (m_teams[team_index].active || m_teams[team_index].display_until >= current_milliseconds);
                if (team_active)
                {
                    team_color = m_teams[team_index].color;
                }
                else
                {
                    long color = -1;
                    if (data->get_integer_value(list_item->get_element_handle(), 278, &color))
                        team_color = color;
                }
            }
            long color = team_game ? team_color : base_color;
            tint_widget_to_change_color(base_color_bitmap, color, team_game);
            tint_widget_to_change_color(rank_bitmap, color, team_game);
            tint_widget_to_change_color(party_bitmap, color, team_game);
            tint_widget_to_change_color(name_text, color, team_game);
            tint_widget_to_change_color(service_tag_text, color, team_game);
            party_bitmap->m_sequence_override = party_index - 1;
        }
    }
}
