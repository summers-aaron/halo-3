#ifndef INTERFACE_GUI_FILE_SHARE_SCREEN_MESSAGES_H
#define INTERFACE_GUI_FILE_SHARE_SCREEN_MESSAGES_H

#include "interface/user_interface_messages.h"
#include "saved_games/saved_game_file_types.h"
#include "networking/online/file_share_slot_types.h"
#include "networking/online/online_service_record.h"
#include <string.h>

struct c_load_file_share_item_selected_screen_message : c_load_screen_message
{
    virtual void apply_initial_state(c_gui_screen_widget *screen_widget) const;
    __forceinline c_load_file_share_item_selected_screen_message(e_controller_index controller, e_window_index window, long layered_position,
        unsigned __int64 owner_xuid, unsigned __int64 player_xuid, long slot_index, const s_file_share_slot *slot,
        const s_service_record_identity *identity)
        : c_load_screen_message(0x1008c, controller, window, layered_position)
    {
        m_owner_xuid = owner_xuid;
        m_player_xuid = player_xuid;
        m_slot_index = slot_index;
        memcpy(&m_slot, slot, sizeof(m_slot));
        m_identity = *identity;
    }

    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_player_xuid;
    long m_slot_index;
    __declspec(align(8)) s_file_share_slot m_slot;
    s_service_record_identity m_identity;
};

struct c_load_file_share_choose_category_screen_message : c_load_screen_message
{
    virtual void apply_initial_state(c_gui_screen_widget *screen_widget) const;
    __forceinline c_load_file_share_choose_category_screen_message(e_controller_index controller, e_window_index window, long layered_position,
        unsigned __int64 owner_xuid, unsigned __int64 player_xuid, long slot_index, long quota_used, long quota_total)
        : c_load_screen_message(0x1008a, controller, window, layered_position)
    {
        m_owner_xuid = owner_xuid;
        m_player_xuid = player_xuid;
        m_slot_index = slot_index;
        m_quota_used = quota_used;
        m_quota_total = quota_total;
    }

    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_player_xuid;
    long m_slot_index;
    long m_quota_used;
    long m_quota_total;
};

struct c_load_file_share_choose_item_screen_message : c_load_screen_message
{
    virtual void apply_initial_state(c_gui_screen_widget *screen_widget) const;
    __forceinline c_load_file_share_choose_item_screen_message(e_controller_index controller, e_window_index window, long layered_position,
        unsigned __int64 owner_xuid, unsigned __int64 player_xuid, long slot_index, long quota_used, long quota_total,
        e_saved_game_file_type file_type)
        : c_load_screen_message(0x1008b, controller, window, layered_position)
    {
        m_owner_xuid = owner_xuid;
        m_player_xuid = player_xuid;
        m_slot_index = slot_index;
        m_quota_used = quota_used;
        m_quota_total = quota_total;
        m_file_type = file_type;
    }

    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_player_xuid;
    long m_slot_index;
    long m_quota_used;
    long m_quota_total;
    e_saved_game_file_type m_file_type;
};

#endif
