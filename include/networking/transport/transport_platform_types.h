#ifndef NETWORKING_TRANSPORT_TRANSPORT_PLATFORM_TYPES_H
#define NETWORKING_TRANSPORT_TRANSPORT_PLATFORM_TYPES_H

// The platform a session's secure transport runs on; transport_secure_key_create asks XNet for a key only on Xenon.
enum e_transport_platform
{
    _transport_platform_xenon = 0,
};

// transport_secure_connection_status: XNetGetConnectStatus's idle/pending/connected/lost, or unknown when the
// address is not an Xbox address.
enum e_transport_secure_connection
{
    _transport_secure_connection_idle,
    _transport_secure_connection_pending,
    _transport_secure_connection_connected,
    _transport_secure_connection_lost,
    _transport_secure_connection_unknown
};

#endif // NETWORKING_TRANSPORT_TRANSPORT_PLATFORM_TYPES_H
