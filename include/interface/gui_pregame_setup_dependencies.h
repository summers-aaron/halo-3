#ifndef GUI_PREGAME_SETUP_DEPENDENCIES_H
#define GUI_PREGAME_SETUP_DEPENDENCIES_H

struct s_data_array;
#include "main/levels.h"
struct s_user_interface_session_privacy_request;
extern "C" unsigned char user_interface_global_string_get(int handle, unsigned short *out);
extern "C" short levels_get_campaign_map_index(long campaign_id, long map_id);
extern "C" long levels_get_default_multiplayer_map_id(void);
extern "C" char *levels_get_map_scenario_path(long campaign_id, long map_id, char *name, long name_size);
extern "C" bool user_interface_squad_set_map_variant_with_verify(s_user_interface_session_privacy_request *request);
extern "C" bool user_interface_squad_set_film(void *setup_data, bool use_default);
extern "C" bool user_interface_squad_restart(int session_class);
extern "C" unsigned char map_variant_test_equal(const void *first, const void *second);

#endif // GUI_PREGAME_SETUP_DEPENDENCIES_H
