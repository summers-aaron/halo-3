#ifndef NETWORKING_TRANSPORT_TRANSPORT_ADDRESS_H
#define NETWORKING_TRANSPORT_TRANSPORT_ADDRESS_H

// transport_address: an IPv4 or IPv6 address and a port, 20 bytes. The header March 2007's asserts name
// (networking\transport\transport_address.cpp). Member names from March: http_client's asserts
// "m_address.address.ipv4_address" and "m_address.address_length == sizeof(unsigned long)" (the union is the
// member "address"); transport_address_valid reads address_length as a short (4, 16; -1 is taken as IPv4) and the
// IPv6 form as eight unsigned halfwords; transport_address_equivalent compares the port as an unsigned short.

struct transport_address
{
    union
    {
        unsigned long ipv4_address;
        unsigned short ipv6_address[8];
    } address;
    unsigned short port;
    short address_length;           // sizeof the address in use (4 or 16)
};
typedef char transport_address_size_check[(sizeof(transport_address) == 0x14) ? 1 : -1];

// Address helpers: validity check and equality comparison, optionally including the port.
bool transport_address_valid(const transport_address *address);
bool transport_address_equivalent(const transport_address *a, const transport_address *b, bool include_port);

#endif // NETWORKING_TRANSPORT_TRANSPORT_ADDRESS_H
