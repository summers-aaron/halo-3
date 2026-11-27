#ifndef NETWORKING_ONLINE_ONLINE_SESSION_XENON_H
#define NETWORKING_ONLINE_ONLINE_SESSION_XENON_H

// Functions defined in tu/networkinglibxbox/online_session_xenon.cpp that other units call.

struct s_online_session;

unsigned long calculate_slot_counts(s_online_session *session, s_online_session *current_session, long *private_slot_count, long *public_slot_count);

#endif // NETWORKING_ONLINE_ONLINE_SESSION_XENON_H
