#ifndef NETWORKING_TOOLS_NETWORK_WEB_EVENTS_H
#define NETWORKING_TOOLS_NETWORK_WEB_EVENTS_H

// Functions defined in tu/networkinglibxbox/network_web_events.cpp that other units call.

struct cheating_player;
struct s_network_web_event_local_player;

extern "C" bool network_web_event_report_local_cheater(const cheating_player *cheating,const s_network_web_event_local_player *players,long count);
extern "C" bool network_web_event_report_remote_cheater(unsigned __int64 remote_machine_id, const cheating_player *cheating,
    const s_network_web_event_local_player *players, long count);

#endif // NETWORKING_TOOLS_NETWORK_WEB_EVENTS_H
