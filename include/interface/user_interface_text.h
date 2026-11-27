#ifndef INTERFACE_USER_INTERFACE_TEXT_H
#define INTERFACE_USER_INTERFACE_TEXT_H

// Functions defined in tu/interfacelibxbox/user_interface_text.cpp that other units call.

template<long k_maximum_count> class c_static_wchar_string;
enum e_utf32;

struct s_user_interface_string_parser
{
    const wchar_t *name;
    long character;
    void (*parse)(long, e_utf32, c_static_wchar_string<1024> *);
};

extern "C" s_user_interface_string_parser k_user_interface_string_parsers[127];

bool online_user_get_gamertag(long controller_index, c_static_wchar_string<16> *gamertag);
bool string_contains_special_characters(const wchar_t *string);
bool user_interface_parse_string(long user_index, wchar_t *string, long buffer_size);
extern "C" void parse_player_gamertag_string();
extern "C" bool user_interface_text_get_map_name(long user_index, wchar_t *output, unsigned long capacity);

// Data defined in tu/interfacelibxbox/user_interface_text.cpp that other units use.

#include "memory/static_arrays.h"

struct s_last_known_session_state
{
    unsigned short hopper_identifier;
    c_static_wchar_string<256> map_name;
    c_static_wchar_string<256> variant_name;
    // No parser reads or writes this slot; kept for struct size.
    c_static_wchar_string<256> unused_session_string;
    c_static_wchar_string<256> squad_leader_name;
    long last_player_count;
};

extern s_last_known_session_state g_last_known_session_state;

#endif // INTERFACE_USER_INTERFACE_TEXT_H
