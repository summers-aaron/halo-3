#ifndef NETWORKING_SESSION_NETWORK_MANAGED_SESSION_STATUS_H
#define NETWORKING_SESSION_NETWORK_MANAGED_SESSION_STATUS_H

struct s_managed_session_status {
    unsigned short m_storage;
    __forceinline bool test(long bit) const { return ((m_storage >> bit) & 1) != 0; }
};

extern "C" unsigned short *managed_session_get_status(unsigned short *result, long index);

#endif // NETWORKING_SESSION_NETWORK_MANAGED_SESSION_STATUS_H
