#ifndef INTERFACE_CONTROLLER_TYPES_H
#define INTERFACE_CONTROLLER_TYPES_H

#include "shell/shell.h"
#include "cseries/cseries_wrapped.h"
#include "interface/controller_state_flags.h"
#include "interface/c_player_profile.h"
#include "memory/static_arrays_flags.h"
#include <stddef.h>

class c_content_catalogue;
struct s_player_identifier;
struct s_achievment_bit_vector;
// Physical pad buttons, in the order the button glyphs map them
// (parse_controller_button_string); NUMBER_OF_GAMEPAD_BUTTONS is the count.
enum e_gamepad_button
{
    _gamepad_button_left_trigger = 0,
    _gamepad_button_right_trigger,
    _gamepad_button_dpad_up,
    _gamepad_button_dpad_down,
    _gamepad_button_dpad_left,
    _gamepad_button_dpad_right,
    _gamepad_button_start,
    _gamepad_button_back,
    _gamepad_button_left_thumb,
    _gamepad_button_right_thumb,
    _gamepad_button_a,
    _gamepad_button_b,
    _gamepad_button_x,
    _gamepad_button_y,
    _gamepad_button_left_bumper,
    _gamepad_button_right_bumper,
    NUMBER_OF_GAMEPAD_BUTTONS
};
enum e_achievement;

// March's controller methods assert in interface\c_controller.cpp; no header
// assertion survives. This leaf sits beside c_controller.h to avoid pulling the
// record into callers which still have private controller/profile definitions.
// The record is 0x9d0 bytes (March: 0x9a0); the display-name offset grew by 0x48.
struct c_controller_interface
{
public:
    c_controller_interface();
    ~c_controller_interface();

    void set_user_index(short user_index);
    // Stores NONE; March 83653340 stored the controller index instead.
    void reset_user_index() { user_index = -1; }
    e_controller_index get_controller_index() const;
    short get_user_index() const { return user_index; }
    const s_player_identifier *get_player_identifier(s_player_identifier *identifier) const;
    const unsigned __int64 get_player_xuid() const;
    const wchar_t *get_display_name() const { return display_name; }
    // March 826fb7a0 returns the profile at +8.
    c_player_profile_interface *get_player_profile_interface() { return &player_profile; }
    c_content_catalogue *get_content_catalogue();
    long get_button_down_frames(e_gamepad_button button);
    bool storage_device_valid();
    void set_as_unsigned_in_user(bool unsigned_in);
    void sign_in_state_changed();
    bool in_use() const;
    bool is_signed_in_to_machine() const;
    bool is_signed_in_to_live() const;
    bool is_silver_or_gold_live() const;
    bool is_guest() const;
    bool is_unsigned_in_user() const { return flags.test(_controller_state_unsigned_in_user); }
    bool is_attached() const { return flags.test(_controller_state_attached); }
    bool get_storage_device_selection_performed() { return flags.test(_controller_state_storage_device_selected); }
    void set_storage_device_selection_performed(bool performed)
    {
        flags.set(_controller_state_storage_device_selected, performed);
    }
    bool is_allowed_online_multiplayer_gaming() const;
    bool is_allowed_communications(bool *friends_only) const;
    bool is_allowed_profile_viewing(bool *friends_only) const;
    bool is_allowed_user_created_content(bool *friends_only) const;
    bool is_allowed_presence(bool *friends_only) const;
    unsigned __int64 get_hash_bits(long value);
    void update(unsigned long milliseconds);
    bool controller_has_achievement(e_achievement achievement) const;
    void controller_get_achievements(s_achievment_bit_vector *achievements) const;

protected:
    void controller_reset_achievements();
    void controller_grant_achievement(e_achievement achievement);

private:
    void update_controller_properties();
    void sign_in_controller(const s_player_identifier *identifier, bool unsigned_in);
    void sign_out_controller(bool value);
    void remove_controller_from_network_session();
    void generate_random_display_name();

public:
    // March constructs this member as c_flags<e_controller_state_flags, short, 13>; user_index is signed.
    c_flags<e_controller_state_flags, short, 13> flags;
    short user_index;
    c_player_profile_interface player_profile;
    wchar_t display_name[16];
    unsigned __int64 hash_bits;
    unsigned __int64 hash_key;
    c_static_flags_no_init<64> achievements;   // March: c_static_flags (bit_vector of 64 achievements)
    void *m_user;                                // controller update state
    unsigned long sign_out_time;
};

// The catalog identifies s_controller as the old name of this same record,
// rather than a second object. Retain it as an alias, with one storage definition.
typedef c_controller_interface s_controller;

struct s_controller_globals
{
    s_controller_globals();
    ~s_controller_globals();

    c_controller_interface controllers[4];
    long controller_windows[4];
    bool controller_event_pending;
};

extern "C" s_controller_globals g_controller_globals;

static __forceinline s_controller_globals &controller_globals() { return g_controller_globals; }

inline e_controller_index c_controller_interface::get_controller_index() const
{
    return (e_controller_index)(this - g_controller_globals.controllers);
}

typedef char controller_size_check[(sizeof(void *) != 4 || ((sizeof(c_controller_interface) == 0x9d0) )) ? 1 : -1];
typedef char controller_user_index_offset_check[(offsetof(c_controller_interface, user_index) == 2) ? 1 : -1];
typedef char controller_profile_offset_check[(offsetof(c_controller_interface, player_profile) == 8) ? 1 : -1];
typedef char controller_user_offset_check[(sizeof(void *) != 4 || ((offsetof(c_controller_interface, m_user) == 0x9c8))) ? 1 : -1];
typedef char controller_display_name_offset_check[(sizeof(void *) != 4 || ((offsetof(c_controller_interface, display_name) == 0x990))) ? 1 : -1];
typedef char controller_hash_bits_offset_check[(sizeof(void *) != 4 || ((offsetof(c_controller_interface, hash_bits) == 0x9b0))) ? 1 : -1];
typedef char controller_hash_key_offset_check[(sizeof(void *) != 4 || ((offsetof(c_controller_interface, hash_key) == 0x9b8))) ? 1 : -1];
typedef char controller_achievements_offset_check[(sizeof(void *) != 4 || ((offsetof(c_controller_interface, achievements) == 0x9c0))) ? 1 : -1];
typedef char controller_sign_out_time_offset_check[(sizeof(void *) != 4 || ((offsetof(c_controller_interface, sign_out_time) == 0x9cc) )) ? 1 : -1];
typedef char controller_windows_offset_check[(sizeof(void *) != 4 || ((offsetof(s_controller_globals, controller_windows) == 0x2740) )) ? 1 : -1];

#endif // INTERFACE_CONTROLLER_TYPES_H
