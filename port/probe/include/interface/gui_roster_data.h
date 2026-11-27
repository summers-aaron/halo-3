#include "interface/user_interface_matchmaking_progress.h"
#ifndef INTERFACE_GUI_ROSTER_DATA_H
#define INTERFACE_GUI_ROSTER_DATA_H

// Functions defined in tu/interfacelibxbox/gui_roster_data.cpp.

#include "interface/gui_data.h"
#include "game/player_configuration.h"
#include "game/player_identifier_types.h"
#include "shell/shell.h"

class c_gui_roster_data;
class c_gui_game_details;
extern "C" void gui_roster_data_merge_duplicate_rows(c_gui_roster_data *roster);
extern "C" void gui_roster_data_add_joinable_controllers(c_gui_roster_data *roster);
extern "C" void gui_roster_data_compute_team_maxima(c_gui_roster_data *roster);

static __forceinline void gui_roster_data_add_slot(c_gui_roster_data *roster, int type);

enum e_roster_data_column
{
    _roster_data_column_type = 0x103a8,
    _roster_data_column_player_index = 0x10014,
    _roster_data_column_team_index = 0x116,
    _roster_data_column_base_color = 0x10127,
    _roster_data_column_controller = 0x10016,
    _roster_data_column_voice = 0x101eb,
    _roster_data_column_button_x = 0x1004f,
    _roster_data_column_name = 0x2ae,
    _roster_data_column_service_tag = 0x103cd,
    _roster_data_column_press_a_to_join = 0x118,
    _roster_data_column_looking_for_player = 0x103c2,
    _roster_data_column_player_found = 0x103c5,
    _roster_data_column_rank = 0x103bc,
    _roster_data_column_skill = 0x103ba,
    _roster_data_column_state = 0x103cb,
    _roster_data_column_disabled = 0x103d7,
    _roster_data_column_bungienet_user = 0x103da,
};

class c_gui_roster_data : public c_gui_ordered_data {
public:
    enum e_player_row_type { _player_row_player, _player_row_joinable_controller = 3 };
    enum e_voice_talking_state
    {
        _voice_not_talking,
        _voice_available,
        _voice_talking,
        _voice_private_chat,
        _voice_muted
    };
    static e_voice_talking_state get_voice_for_player(e_controller_index controller_index, long player_index);
    struct s_player_row {
        e_player_row_type type;
        long player_index;
        s_player_identifier identifier;
        bool identifier_valid;
        s_player_configuration configuration;
        bool configuration_valid;
        long team_index;
        long game_team_index;
        long peer_index;
        long player_sequence_number;
        unsigned __int64 party_nonce;
        long leader_team_index;
        e_controller_index controller_index;
        e_voice_talking_state voice;
        long state;
        bool is_squad_leader;
        bool show_teams;
        long flags;
        long party_skill;
        long party_experience;
    };
    friend class c_gui_game_details;
    friend void gui_roster_data_merge_duplicate_rows(c_gui_roster_data *roster);
    friend void gui_roster_data_add_joinable_controllers(c_gui_roster_data *roster);
    friend void gui_roster_data_compute_team_maxima(c_gui_roster_data *roster);
    friend void gui_roster_data_add_slot(c_gui_roster_data *roster, int type);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_player_appearance(long element_handle, s_player_appearance *appearance);
    virtual bool get_player_identifier(long element_handle, s_player_identifier *identifier);
    virtual bool get_player_configuration(long element_handle, s_player_configuration *configuration);
protected:
    c_gui_roster_data(e_controller_index controller_index);
    virtual long get_current_item_count_internal();
    s_player_row *add_player_internal(e_player_row_type row_type, long player_index,
        const s_player_identifier *identifier, const s_player_configuration *configuration,
        e_controller_index controller, e_voice_talking_state voice, long flags, bool use_team_overrides);
    void update_matchmaking_slots(void);
    long m_wanted_player_slot_count;
    long m_party_player_slot_count;
    s_player_row m_players[16];
    long m_player_count;
    e_controller_index m_controller_index;
    bool m_player_list_update_paused;
};

class c_gui_squad_roster_data : public c_gui_roster_data {
public:
    c_gui_squad_roster_data(e_controller_index controller_index);
    virtual void update();
};


extern "C" long g_experience_rank_icon_element_handles[14][4];
// PC port: network_session_interface.cpp defines extern "C" bool; declaring int reads dirty upper %eax on i686.
extern "C" bool network_squad_session_boot_player(int, int);

extern "C" long user_interface_session_get_controller_index(long player_index);
extern "C" bool user_interface_networking_has_found_game(void);

#endif // INTERFACE_GUI_ROSTER_DATA_H
