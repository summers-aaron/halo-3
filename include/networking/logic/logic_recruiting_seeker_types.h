#ifndef NETWORKING_LOGIC_RECRUITING_SEEKER_TYPES_H
#define NETWORKING_LOGIC_RECRUITING_SEEKER_TYPES_H

#include "networking/transport/transport_security_types.h"


#include "networking/logic/logic_matchmaking_seeker.h"
struct s_recruiting_search_statistics {
    long count;                     /* (session_search.count in the search) */
    unsigned long start;
    unsigned long finish;
    long failures;                  /* (0x10 in the search) */
    long unknown;
    unsigned long failure_time;
    long c1, c2, c3, c4;
    c_static_array<long, 12> by_a;
    long c5;
    c_static_array<long, 2> by_b;
    long join_failures[20];
};
struct s_recruiting_session_search {
    long unused_first;
    s_recruiting_search_statistics stats;
    s_online_session_search_parameters online_query;
    long unused_last;
};

struct s_recruiting_session_search_query { long unused_first; long flags; long controller_index; };

#include <string.h>
#include "interface/user_interface_controller.h"
class c_matchmaking_quality;
enum e_recruiting_seeker_flags {
    _recruiting_seeker_flag_bit0,
    _recruiting_seeker_flag_bit1,
    _recruiting_seeker_flag_bit2,
    _recruiting_seeker_flag_bit3,
    _recruiting_seeker_flag_bit4,
    _recruiting_seeker_flag_bit5,
    k_recruiting_seeker_flags_count
};
class c_recruiting_seeker {
public:
 c_flags<e_recruiting_seeker_flags, unsigned char, k_recruiting_seeker_flags_count> flags;
 long playlist;
 e_controller_index controller;
 long a,m_current_session_search_index,m_failure_count,d,e,m_contacted_count,g;
 unsigned long start_time;
 long time;
 s_recruiting_session_search m_searches[16];
 long h,i;
 __forceinline void initialize() {
  flags.clear(); playlist=0; controller=::user_interface_controller_get_first_xbox_live_signed_in_non_guest();
  a=0; m_current_session_search_index=-1; m_failure_count=0; d=0; g=0; e=0; m_contacted_count=0;
  memset(m_searches,0,sizeof(m_searches)); start_time=0; time=0; h=0; i=0;
 }
 bool search_begin(e_controller_index controller_index, long playlist_index, bool, c_matchmaking_quality *quality);
private:
 friend void network_recruiting_search_update(void);
 void update_online_session_search_initiate();
 void build_online_query_from_query(const s_recruiting_session_search_query *query, s_online_session_search_parameters *parameters);
 void update_session_search();
 void update_online_session_search();
 void update_tracked_session_statistics();
 void mark_current_session_search_query_failed();
};
#endif
