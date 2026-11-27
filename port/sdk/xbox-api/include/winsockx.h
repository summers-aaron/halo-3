// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// winsockx.h: the Xbox 360 WinSock 2 subset and the XNet secure-network API (XNADDR, XNKID, QoS).
#ifndef _WINSOCK2API_
#define _WINSOCK2API_
#define _WINSOCKAPI_

// The Xbox socket entry points carry XSocket* names; titles call them by their BSD/WinSock names.
#ifndef XNET_DONT_REMAP_SOCKET_CALLS
#define XSocketCreate socket
#define XSocketClose closesocket
#define XSocketShutdown shutdown
#define XSocketIOCTLSocket ioctlsocket
#define XSocketSetSockOpt setsockopt
#define XSocketGetSockOpt getsockopt
#define XSocketGetSockName getsockname
#define XSocketGetPeerName getpeername
#define XSocketBind bind
#define XSocketConnect connect
#define XSocketListen listen
#define XSocketAccept accept
#define XSocketSelect select
#define XWSAGetOverlappedResult WSAGetOverlappedResult
#define XWSACancelOverlappedIO WSACancelOverlappedIO
#define XSocketRecv recv
#define XWSARecv WSARecv
#define XSocketRecvFrom recvfrom
#define XWSARecvFrom WSARecvFrom
#define XSocketSend send
#define XWSASend WSASend
#define XSocketSendTo sendto
#define XWSASendTo WSASendTo
#define XWSAGetLastError WSAGetLastError
#define XWSASetLastError WSASetLastError
#define __XWSAFDIsSet __WSAFDIsSet
#define XWSAEventSelect WSAEventSelect
#define XWSACleanup WSACleanup
#define XWSACloseEvent WSACloseEvent
#define XWSACreateEvent WSACreateEvent
#define XWSAResetEvent WSAResetEvent
#define XWSASetEvent WSASetEvent
#define XWSAStartup WSAStartup
#define XWSAWaitForMultipleEvents WSAWaitForMultipleEvents
#define XSocketHTONL htonl
#define XSocketHTONS htons
#define XSocketInet_Addr inet_addr
#define XSocketNTOHL ntohl
#define XSocketNTOHS ntohs
#endif

#ifndef _WIN64
#include <pshpack4.h>
#endif

#ifndef INCL_WINSOCK_API_PROTOTYPES
#define INCL_WINSOCK_API_PROTOTYPES 1
#endif
#ifndef INCL_WINSOCK_API_TYPEDEFS
#define INCL_WINSOCK_API_TYPEDEFS 0
#endif

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#if !defined(MAKEWORD)
#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | ((WORD)((BYTE)(high))) << 8))
#endif
#ifndef WINSOCK_VERSION
#define WINSOCK_VERSION MAKEWORD(2, 2)
#endif
#ifndef WINSOCK_API_LINKAGE
#define WINSOCK_API_LINKAGE
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef UINT_PTR SOCKET;

#ifndef FD_SETSIZE
#define FD_SETSIZE 64
#endif

typedef struct fd_set {
    u_int fd_count;
    SOCKET fd_array[FD_SETSIZE];
} fd_set;

extern int PASCAL FAR __WSAFDIsSet(SOCKET, fd_set FAR *);

// Remove fd from the set, shifting the later entries down.
#define FD_CLR(fd, set)                                                                         \
    do {                                                                                        \
        u_int __i;                                                                              \
        for (__i = 0; __i < ((fd_set FAR *)(set))->fd_count; __i++) {                           \
            if (((fd_set FAR *)(set))->fd_array[__i] == fd) {                                   \
                while (__i < ((fd_set FAR *)(set))->fd_count - 1) {                             \
                    ((fd_set FAR *)(set))->fd_array[__i] = ((fd_set FAR *)(set))->fd_array[__i + 1]; \
                    __i++;                                                                      \
                }                                                                               \
                ((fd_set FAR *)(set))->fd_count--;                                              \
                break;                                                                          \
            }                                                                                   \
        }                                                                                       \
    } while (0)
// Append fd unless it is already present or the set is full.
#define FD_SET(fd, set)                                                                         \
    do {                                                                                        \
        u_int __i;                                                                              \
        for (__i = 0; __i < ((fd_set FAR *)(set))->fd_count; __i++) {                           \
            if (((fd_set FAR *)(set))->fd_array[__i] == (fd)) {                                 \
                break;                                                                          \
            }                                                                                   \
        }                                                                                       \
        if (__i == ((fd_set FAR *)(set))->fd_count) {                                           \
            if (((fd_set FAR *)(set))->fd_count < FD_SETSIZE) {                                 \
                ((fd_set FAR *)(set))->fd_array[__i] = (fd);                                    \
                ((fd_set FAR *)(set))->fd_count++;                                              \
            }                                                                                   \
        }                                                                                       \
    } while (0)
#define FD_ZERO(set) (((fd_set FAR *)(set))->fd_count = 0)
#define FD_ISSET(fd, set) __WSAFDIsSet((SOCKET)(fd), (fd_set FAR *)(set))

struct timeval {
    long tv_sec;
    long tv_usec;
};

#define IOCPARM_MASK 0x7f
#define IOC_VOID 0x20000000
#define IOC_OUT 0x40000000
#define IOC_IN 0x80000000
#define IOC_INOUT (IOC_IN | IOC_OUT)
#define _IO(x, y) (IOC_VOID | ((x) << 8) | (y))
#define _IOR(x, y, t) (IOC_OUT | (((long)sizeof(t) & IOCPARM_MASK) << 16) | ((x) << 8) | (y))
#define _IOW(x, y, t) (IOC_IN | (((long)sizeof(t) & IOCPARM_MASK) << 16) | ((x) << 8) | (y))
#define FIONREAD _IOR('f', 127, u_long)
#define FIONBIO _IOW('f', 126, u_long)

#define IPPROTO_IP 0
#define IPPROTO_ICMP 1
#define IPPROTO_IGMP 2
#define IPPROTO_GGP 3
#define IPPROTO_TCP 6
#define IPPROTO_PUP 12
#define IPPROTO_UDP 17
#define IPPROTO_IDP 22
#define IPPROTO_ND 77
#define IPPROTO_RAW 255
#define IPPROTO_MAX 256

struct in_addr {
    union {
        struct {
            u_char s_b1, s_b2, s_b3, s_b4;
        } S_un_b;
        struct {
            u_short s_w1, s_w2;
        } S_un_w;
        u_long S_addr;
    } S_un;
#define s_addr S_un.S_addr
#define s_host S_un.S_un_b.s_b2
#define s_net S_un.S_un_b.s_b1
#define s_imp S_un.S_un_w.s_w2
#define s_impno S_un.S_un_b.s_b4
#define s_lh S_un.S_un_b.s_b3
};

#define INADDR_ANY (u_long)0x00000000
#define INADDR_LOOPBACK 0x7f000001
#define INADDR_BROADCAST (u_long)0xffffffff
#define INADDR_NONE 0xffffffff
#define ADDR_ANY INADDR_ANY

struct sockaddr_in {
    short sin_family;
    u_short sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};

#define WSADESCRIPTION_LEN 256
#define WSASYS_STATUS_LEN 128

typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN + 1];
    char szSystemStatus[WSASYS_STATUS_LEN + 1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char FAR *lpVendorInfo;
} WSADATA, FAR *LPWSADATA;

#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)

#define SOCK_STREAM 1
#define SOCK_DGRAM 2

#define SO_ACCEPTCONN 0x0002
#define SO_REUSEADDR 0x0004
#define SO_BROADCAST 0x0020
#define SO_LINGER 0x0080
#define SO_DONTLINGER (int)(~SO_LINGER)
#define SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR))
#define SO_SNDBUF 0x1001
#define SO_RCVBUF 0x1002
#define SO_SNDTIMEO 0x1005
#define SO_RCVTIMEO 0x1006
#define SO_TYPE 0x1008
#define TCP_NODELAY 0x0001

#define AF_UNSPEC 0
#define AF_INET 2
#define AF_MAX 29

struct sockaddr {
    u_short sa_family;
    char sa_data[14];
};

#define PF_UNSPEC AF_UNSPEC
#define PF_INET AF_INET
#define PF_MAX AF_MAX

struct linger {
    u_short l_onoff;
    u_short l_linger;
};

#define SOL_SOCKET 0xffff
#define SOMAXCONN 0x7fffffff

#define WSAAPI FAR PASCAL
#define WSAEVENT HANDLE
#define LPWSAEVENT LPHANDLE
#define WSAOVERLAPPED OVERLAPPED
typedef struct _OVERLAPPED *LPWSAOVERLAPPED;

#define SD_RECEIVE 0x00
#define SD_SEND 0x01
#define SD_BOTH 0x02

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr FAR *LPSOCKADDR;

#if INCL_WINSOCK_API_PROTOTYPES
WINSOCK_API_LINKAGE SOCKET WSAAPI accept(IN SOCKET s, OUT struct sockaddr FAR *addr, IN OUT int FAR *addrlen);
WINSOCK_API_LINKAGE int WSAAPI bind(IN SOCKET s, IN const struct sockaddr FAR *name, IN int namelen);
WINSOCK_API_LINKAGE int WSAAPI closesocket(IN SOCKET s);
WINSOCK_API_LINKAGE int WSAAPI connect(IN SOCKET s, IN const struct sockaddr FAR *name, IN int namelen);
WINSOCK_API_LINKAGE int WSAAPI ioctlsocket(IN SOCKET s, IN long cmd, IN OUT u_long FAR *argp);
WINSOCK_API_LINKAGE int WSAAPI getpeername(IN SOCKET s, OUT struct sockaddr FAR *name, IN OUT int FAR *namelen);
WINSOCK_API_LINKAGE int WSAAPI getsockname(IN SOCKET s, OUT struct sockaddr FAR *name, IN OUT int FAR *namelen);
WINSOCK_API_LINKAGE int WSAAPI getsockopt(IN SOCKET s, IN int level, IN int optname, OUT char FAR *optval,
                                          IN OUT int FAR *optlen);
// Network byte order is the console's native (big-endian) order: the conversions are identities.
#ifdef _PPC_
#define htonl(hostlong) ((u_long)(hostlong))
#define htons(hostshort) ((u_short)(hostshort))
#else
WINSOCK_API_LINKAGE u_long WSAAPI htonl(IN u_long hostlong);
WINSOCK_API_LINKAGE u_short WSAAPI htons(IN u_short hostshort);
#endif
WINSOCK_API_LINKAGE unsigned long WSAAPI inet_addr(IN const char FAR *cp);
WINSOCK_API_LINKAGE int WSAAPI listen(IN SOCKET s, IN int backlog);
#ifdef _PPC_
#define ntohl(netlong) ((u_long)(netlong))
#define ntohs(netshort) ((u_short)(netshort))
#else
WINSOCK_API_LINKAGE u_long WSAAPI ntohl(IN u_long netlong);
WINSOCK_API_LINKAGE u_short WSAAPI ntohs(IN u_short netshort);
#endif
WINSOCK_API_LINKAGE int WSAAPI recv(IN SOCKET s, OUT char FAR *buf, IN int len, IN int flags);
WINSOCK_API_LINKAGE int WSAAPI recvfrom(IN SOCKET s, OUT char FAR *buf, IN int len, IN int flags,
                                        OUT struct sockaddr FAR *from, IN OUT int FAR *fromlen);
WINSOCK_API_LINKAGE int WSAAPI select(IN int nfds, IN OUT fd_set FAR *readfds, IN OUT fd_set FAR *writefds,
                                      IN OUT fd_set FAR *exceptfds, IN const struct timeval FAR *timeout);
WINSOCK_API_LINKAGE int WSAAPI send(IN SOCKET s, IN const char FAR *buf, IN int len, IN int flags);
WINSOCK_API_LINKAGE int WSAAPI sendto(IN SOCKET s, IN const char FAR *buf, IN int len, IN int flags,
                                      IN const struct sockaddr FAR *to, IN int tolen);
WINSOCK_API_LINKAGE int WSAAPI setsockopt(IN SOCKET s, IN int level, IN int optname, IN const char FAR *optval,
                                          IN int optlen);
WINSOCK_API_LINKAGE int WSAAPI shutdown(IN SOCKET s, IN int how);
WINSOCK_API_LINKAGE SOCKET WSAAPI socket(IN int af, IN int type, IN int protocol);
WINSOCK_API_LINKAGE int WSAAPI WSAStartup(IN WORD wVersionRequested, OUT LPWSADATA lpWSAData);
WINSOCK_API_LINKAGE int WSAAPI WSACleanup(void);
WINSOCK_API_LINKAGE void WSAAPI WSASetLastError(IN int iError);
WINSOCK_API_LINKAGE int WSAAPI WSAGetLastError(void);
#endif // INCL_WINSOCK_API_PROTOTYPES

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in FAR *LPSOCKADDR_IN;
typedef struct in_addr IN_ADDR;
typedef struct in_addr *PIN_ADDR;
typedef struct in_addr FAR *LPIN_ADDR;
typedef struct fd_set FD_SET;
typedef struct fd_set *PFD_SET;
typedef struct fd_set FAR *LPFD_SET;
typedef struct timeval TIMEVAL;
typedef struct timeval *PTIMEVAL;
typedef struct timeval FAR *LPTIMEVAL;

// XNet: byte-packed wire/config structures.
#include <pshpack1.h>

#define XNET_STARTUP_BYPASS_SECURITY 0x01
#define XNET_STARTUP_ALLOCATE_MAX_DGRAM_SOCKETS 0x02
#define XNET_STARTUP_ALLOCATE_MAX_STREAM_SOCKETS 0x04

typedef struct {
    BYTE cfgSizeOfStruct;
    BYTE cfgFlags;
    BYTE cfgSockMaxDgramSockets;
    BYTE cfgSockMaxStreamSockets;
    BYTE cfgSockDefaultRecvBufsizeInK;
    BYTE cfgSockDefaultSendBufsizeInK;
    BYTE cfgKeyRegMax;
    BYTE cfgSecRegMax;
    BYTE cfgQosDataLimitDiv4;
    BYTE cfgQosProbeTimeoutInSeconds;
    BYTE cfgQosProbeRetries;
    BYTE cfgQosSrvMaxSimultaneousResponses;
    BYTE cfgQosPairWaitTimeInSeconds;
} XNetStartupParams;

typedef struct {
    IN_ADDR ina;
    IN_ADDR inaOnline;
    WORD wPortOnline;
    BYTE abEnet[6];
    BYTE abOnline[20];
} XNADDR;

typedef struct {
    BYTE ab[8];
} XNKID;

typedef XNADDR TSADDR;

#define XNET_XNKID_MASK 0xF0
#define XNET_XNKID_SYSTEM_LINK 0x00
#define XNET_XNKID_SYSTEM_LINK_XPLAT 0x40
#define XNET_XNKID_ONLINE_PEER 0x80
#define XNET_XNKID_ONLINE_SERVER 0xC0
#define XNET_XNKID_ONLINE_TITLESERVER 0xE0
#define XNetXnKidIsSystemLinkXbox(pxnkid) (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_SYSTEM_LINK)
#define XNetXnKidIsSystemLinkXPlat(pxnkid) (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_SYSTEM_LINK_XPLAT)
#define XNetXnKidIsSystemLink(pxnkid) (XNetXnKidIsSystemLinkXbox(pxnkid) || XNetXnKidIsSystemLinkXPlat(pxnkid))
#define XNetXnKidIsOnlinePeer(pxnkid) (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_ONLINE_PEER)
#define XNetXnKidIsOnlineServer(pxnkid) (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_ONLINE_SERVER)
#define XNetXnKidIsOnlineTitleServer(pxnkid) (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_ONLINE_TITLESERVER)

typedef struct {
    BYTE ab[16];
} XNKEY;

typedef struct {
    INT iStatus;
    UINT cina;
    IN_ADDR aina[8];
} XNDNS;

#define XNET_XNQOSINFO_COMPLETE 0x01
#define XNET_XNQOSINFO_TARGET_CONTACTED 0x02
#define XNET_XNQOSINFO_TARGET_DISABLED 0x04
#define XNET_XNQOSINFO_DATA_RECEIVED 0x08
#define XNET_XNQOSINFO_PARTIAL_COMPLETE 0x10

typedef struct {
    BYTE bFlags;
    BYTE bReserved;
    WORD cProbesXmit;
    WORD cProbesRecv;
    WORD cbData;
    BYTE *pbData;
    WORD wRttMinInMsecs;
    WORD wRttMedInMsecs;
    DWORD dwUpBitsPerSec;
    DWORD dwDnBitsPerSec;
} XNQOSINFO;

typedef struct {
    UINT cxnqos;
    UINT cxnqosPending;
    XNQOSINFO axnqosinfo[1];
} XNQOS;

typedef struct {
    DWORD dwSizeOfStruct;
    DWORD dwNumDataRequestsReceived;
    DWORD dwNumProbesReceived;
    DWORD dwNumSlotsFullDiscards;
    DWORD dwNumDataRepliesSent;
    DWORD dwNumDataReplyBytesSent;
    DWORD dwNumProbeRepliesSent;
} XNQOSLISTENSTATS;

#include <poppack.h>

INT WSAAPI XNetStartup(const XNetStartupParams *pxnsp);
INT WSAAPI XNetCleanup();
INT WSAAPI XNetRandom(BYTE *pb, UINT cb);
INT WSAAPI XNetCreateKey(XNKID *pxnkid, XNKEY *pxnkey);
INT WSAAPI XNetRegisterKey(const XNKID *pxnkid, const XNKEY *pxnkey);
INT WSAAPI XNetUnregisterKey(const XNKID *pxnkid);
INT WSAAPI XNetXnAddrToInAddr(const XNADDR *pxna, const XNKID *pxnkid, IN_ADDR *pina);
INT WSAAPI XNetServerToInAddr(const IN_ADDR ina, DWORD dwServiceId, IN_ADDR *pina);
INT WSAAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR *pxna, XNKID *pxnkid);
INT WSAAPI XNetUnregisterInAddr(const IN_ADDR ina);
INT WSAAPI XNetXnAddrToMachineId(const XNADDR *pxnaddr, ULONGLONG *pqwMachineId);

#define XNET_CONNECT_STATUS_IDLE 0x00000000
#define XNET_CONNECT_STATUS_PENDING 0x00000001
#define XNET_CONNECT_STATUS_CONNECTED 0x00000002
#define XNET_CONNECT_STATUS_LOST 0x00000003

INT WSAAPI XNetConnect(const IN_ADDR ina);
DWORD WSAAPI XNetGetConnectStatus(const IN_ADDR ina);

#define XNET_QOS_LISTEN_ENABLE 0x00000001
#define XNET_QOS_LISTEN_DISABLE 0x00000002
#define XNET_QOS_LISTEN_SET_DATA 0x00000004
#define XNET_QOS_LISTEN_SET_BITSPERSEC 0x00000008
#define XNET_QOS_LISTEN_RELEASE 0x00000010

INT WSAAPI XNetQosListen(const XNKID *pxnkid, const BYTE *pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags);
INT WSAAPI XNetQosLookup(UINT cxna, const XNADDR *apxna[], const XNKID *apxnkid[], const XNKEY *apxnkey[], UINT cina,
                         const IN_ADDR aina[], const DWORD adwServiceId[], UINT cProbes, DWORD dwBitsPerSec,
                         DWORD dwFlags, WSAEVENT hEvent, XNQOS **ppxnqos);
INT WSAAPI XNetQosServiceLookup(DWORD dwFlags, WSAEVENT hEvent, XNQOS **ppxnqos);
INT WSAAPI XNetQosRelease(XNQOS *pxnqos);
INT WSAAPI XNetQosGetListenStats(const XNKID *pxnkid, XNQOSLISTENSTATS *pQosListenStats);

#define XNET_GET_XNADDR_PENDING 0x00000000
#define XNET_GET_XNADDR_NONE 0x00000001
#define XNET_GET_XNADDR_ETHERNET 0x00000002
#define XNET_GET_XNADDR_STATIC 0x00000004
#define XNET_GET_XNADDR_DHCP 0x00000008
#define XNET_GET_XNADDR_PPPOE 0x00000010
#define XNET_GET_XNADDR_GATEWAY 0x00000020
#define XNET_GET_XNADDR_DNS 0x00000040
#define XNET_GET_XNADDR_ONLINE 0x00000080
#define XNET_GET_XNADDR_TROUBLESHOOT 0x00008000

DWORD WSAAPI XNetGetTitleXnAddr(XNADDR *pxna);
DWORD WSAAPI XNetGetDebugXnAddr(XNADDR *pxna);

#define XNET_ETHERNET_LINK_ACTIVE 0x00000001
#define XNET_ETHERNET_LINK_100MBPS 0x00000002
#define XNET_ETHERNET_LINK_10MBPS 0x00000004
#define XNET_ETHERNET_LINK_FULL_DUPLEX 0x00000008
#define XNET_ETHERNET_LINK_HALF_DUPLEX 0x00000010
#define XNET_ETHERNET_LINK_WIRELESS 0x00000020

DWORD WSAAPI XNetGetEthernetLinkStatus();

#define IPPROTO_VDP 254

#ifdef __cplusplus
}
#endif

#ifndef _WIN64
#include <poppack.h>
#endif

#endif // _WINSOCK2API_
