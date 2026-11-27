#ifndef NETWORKING_ONLINE_ONLINE_SESSION_RESULTS_H
#define NETWORKING_ONLINE_ONLINE_SESSION_RESULTS_H

struct s_online_search_results;

extern "C" bool online_session_build_search_result_from_results_buffer(const void *buffer, s_online_search_results *out);

#endif // NETWORKING_ONLINE_ONLINE_SESSION_RESULTS_H
