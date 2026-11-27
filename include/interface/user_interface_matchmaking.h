#ifndef INTERFACE_USER_INTERFACE_MATCHMAKING_H
#define INTERFACE_USER_INTERFACE_MATCHMAKING_H

#include "interface/hopper_category_id_types.h"

// Functions defined in tu/interfacelibxbox/user_interface_matchmaking.cpp that other units call.

struct s_hopper_information;
enum e_hopper_load_status;
enum e_matchmaking_search_preference;

bool user_interface_matchmaking_get_hopper_information(unsigned short id, s_hopper_information *out);
bool user_interface_matchmaking_set_hopper(short value);
extern "C" long user_interface_matchmaking_get_category_ids(long *category_ids);
extern "C" bool user_interface_matchmaking_get_category_name(long category_identifier, unsigned short *category_name);
extern "C" bool user_interface_matchmaking_get_category_description(long category_identifier, unsigned short *description);
extern "C" int user_interface_matchmaking_get_category_value(long category);
extern "C" bool user_interface_matchmaking_squad_set_search_preference(long value);
long user_interface_matchmaking_get_hopper_ids(e_hopper_category_id category, unsigned short *const ids);
e_hopper_load_status user_interface_matchmaking_hopper_catalog_load_status(void);
e_matchmaking_search_preference user_interface_matchmaking_squad_get_search_preference(void);

template<typename t_blf_type> class c_http_blf_simple_downloader;
struct s_dynamic_matchmaking_hopper_statistics;
struct s_matchmaking_single_hopper_statistics;

void user_interface_matchmaking_update_hopper_statistics_for_screen(
    c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics> *downloader,
    unsigned short *last_hopper,
    s_matchmaking_single_hopper_statistics *statistics);

#endif // INTERFACE_USER_INTERFACE_MATCHMAKING_H
