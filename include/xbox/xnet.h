#ifndef XBOX_XNET_H
#define XBOX_XNET_H

#include "networking/in_addr_types.h"

// The XDK's secure network library (xnet.lib, declared by winsockx.h): the address, key and connection calls the
// game makes. Spelled here because the XDK header cannot be included beside the tree's in_addr.

struct XNADDR
{
    unsigned long ina;
    unsigned long inaOnline;
    unsigned short wPortOnline;
    unsigned char abEnet[6];
    unsigned char abOnline[20];
};
typedef char xnaddr_size[(sizeof(XNADDR) == 36) ? 1 : -1];

// XNetGetTitleXnAddr result flags
#define XNET_GET_XNADDR_PENDING 0x00000000
#define XNET_GET_XNADDR_NONE    0x00000001
#define XNET_GET_XNADDR_ETHERNET 0x00000002
#define XNET_GET_XNADDR_DHCP    0x00000004
#define XNET_GET_XNADDR_PPPOE   0x00000008
#define XNET_GET_XNADDR_STATIC  0x00000010

// XNetGetConnectStatus results
#define XNET_CONNECT_STATUS_IDLE      0x00000000
#define XNET_CONNECT_STATUS_PENDING   0x00000001
#define XNET_CONNECT_STATUS_CONNECTED 0x00000002
#define XNET_CONNECT_STATUS_LOST      0x00000003

struct s_transport_secure_identifier;
struct s_transport_secure_key;

#ifdef __cplusplus
extern "C" {
#endif
// The key identifier and key are the XDK's XNKID (8 bytes) and XNKEY (16 bytes); the game holds them as
// s_transport_secure_identifier and s_transport_secure_key.
unsigned long XNetGetTitleXnAddr(XNADDR *pxna);
int XNetXnAddrToInAddr(const XNADDR *pxna, const s_transport_secure_identifier *pxnkid, in_addr *pina);
int XNetInAddrToXnAddr(in_addr ina, XNADDR *pxna, s_transport_secure_identifier *pxnkid);
int XNetCreateKey(s_transport_secure_identifier *pxnkid, s_transport_secure_key *pxnkey);
int XNetXnAddrToMachineId(const XNADDR *pxnaddr, unsigned __int64 *pqwMachineId);
unsigned long XNetGetConnectStatus(in_addr ina);
int XNetConnect(in_addr ina);
int XNetRandom(unsigned char *pb, unsigned int cb);
#ifdef __cplusplus
}
#endif

#endif // XBOX_XNET_H
