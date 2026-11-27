#ifndef XBOX_XNET_SERVICES_H
#define XBOX_XNET_SERVICES_H

struct s_transport_secure_identifier;
struct XNADDR;
struct s_transport_qos;

extern "C" int XNetXnAddrToMachineId(const XNADDR *address, unsigned __int64 *machine_id);

extern "C" int XNetQosListen(const s_transport_secure_identifier *, const void *, long, long, long);
extern "C" long XNetQosServiceLookup(unsigned long flags, unsigned long reserved, void **qos);
extern "C" int XNetQosLookup(int count, const void **xnaddrs, const void **kids, const void **keys,
    int, int, int, int probes, int bits_per_second, int, int, s_transport_qos **qos);
extern "C" int XNetQosRelease(void *qos);
extern "C" int XNetServerToInAddr(unsigned long address, unsigned long service_id, long *ip_address);
extern "C" unsigned long inet_addr(const char *address);
extern "C" int XNetUnregisterInAddr(long ip_address);

#endif // XBOX_XNET_SERVICES_H
