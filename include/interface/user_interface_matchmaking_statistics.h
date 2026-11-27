#ifndef INTERFACE_USER_INTERFACE_MATCHMAKING_STATISTICS_H
#define INTERFACE_USER_INTERFACE_MATCHMAKING_STATISTICS_H

template<typename t_blf_type> class c_http_blf_simple_downloader;
struct s_dynamic_matchmaking_hopper_statistics;
struct s_matchmaking_single_hopper_statistics;

// Defined in user_interface_matchmaking.cpp.
void user_interface_matchmaking_update_hopper_statistics_for_screen(
    c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics> *downloader,
    unsigned short *last_hopper,
    s_matchmaking_single_hopper_statistics *statistics);

#endif // INTERFACE_USER_INTERFACE_MATCHMAKING_STATISTICS_H
