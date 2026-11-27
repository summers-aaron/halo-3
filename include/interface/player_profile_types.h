#ifndef INTERFACE_PLAYER_PROFILE_TYPES_H
#define INTERFACE_PLAYER_PROFILE_TYPES_H

// Profile input presets, shared by the profile interface and its storage format.
enum e_button_presets {
    button_preset_default = 0,
    button_preset_south_paw,
    button_preset_boxer,
    button_preset_green_thumb,
    button_preset_professional,
    button_preset_walkie_talkie,
    button_preset_count
};
enum e_joystick_presets { joystick_preset_default = 0, joystick_preset_count = 4 };

// XStringVerify's six-byte string descriptor, also used by profile verification.
#ifdef __XONLINE__
typedef STRING_DATA s_player_profile_string_data;
#else
#pragma pack(push, 1)
struct s_player_profile_string_data
{
    unsigned short wStringSize;
    wchar_t *pszString;
};
#pragma pack(pop)
#endif

typedef char player_profile_string_data_size[(sizeof(void *) != 4 || sizeof(s_player_profile_string_data) == 6) ? 1 : -1];

#endif // INTERFACE_PLAYER_PROFILE_TYPES_H
