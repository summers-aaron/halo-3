#ifndef NETWORKING_SESSION_NETWORK_SESSION_CHECKSUM_H
#define NETWORKING_SESSION_NETWORK_SESSION_CHECKSUM_H

struct s_network_session_shared_membership;

template <class T> unsigned long fast_checksum(unsigned long checksum, const T *data);
template <> unsigned long fast_checksum<s_network_session_shared_membership>(unsigned long checksum, const s_network_session_shared_membership *data);

#endif
