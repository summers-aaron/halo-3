#ifndef INTERFACE_C_PLAYER_PROFILE_STORAGE_H
#define INTERFACE_C_PLAYER_PROFILE_STORAGE_H

#include "shell/shell.h"

// String ids of the profile read failure errors, one per controller (set 2 of the string_ids
// table, decoded from mainmenu.map).
enum e_player_profile_storage_string_id
{
	_player_profile_storage_string_player_profile_read_failed_controller0 = 0x2004a,
	_player_profile_storage_string_player_profile_read_failed_controller1 = 0x2004b,
	_player_profile_storage_string_player_profile_read_failed_controller2 = 0x2004c,
	_player_profile_storage_string_player_profile_read_failed_controller3 = 0x2004d,
};

// Functions defined in tu/interfacelibxbox/c_player_profile_storage.cpp.

void xenon_player_profile_storage_initialize();
void xenon_player_profile_storage_dispose();

class c_player_profile_interface;
struct _XUSER_READ_PROFILE_SETTING_RESULT;

extern "C" void player_profile_interface_apply_settings(c_player_profile_interface *profile,
	_XUSER_READ_PROFILE_SETTING_RESULT *results, const unsigned long *setting_ids);
struct s_storage_block_version15;
extern "C" void initialize_interface_with_xenon_global_gamercard_settings(const s_storage_block_version15 *block,
	c_player_profile_interface *profile);
void read_player_profile_interface_from_xenon_profile(e_controller_index controller_index, c_player_profile_interface *profile);
void write_player_profile_interface_to_xenon_profile(e_controller_index controller_index, c_player_profile_interface *profile);

struct s_async_profile_io_storage;
extern "C" s_async_profile_io_storage g_async_profile_io_storage[];
#include "interface/player_profile_global_settings.h"

#endif // INTERFACE_C_PLAYER_PROFILE_STORAGE_H
