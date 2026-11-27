#ifndef NETWORKING_TRANSPORT_TRANSPORT_UNIQUE_IDENTIFIER_TYPES_H
#define NETWORKING_TRANSPORT_TRANSPORT_UNIQUE_IDENTIFIER_TYPES_H

struct s_transport_unique_identifier
{
    // XNADDR::abEnet, copied from bytes 10..15 of the secure address (March
    // transport_secure_address_extract_identifier); printed one byte at a time as %02X.
    unsigned char ethernet_address[6];
};

typedef char transport_unique_identifier_size[(sizeof(s_transport_unique_identifier) == 6) ? 1 : -1];

#endif // NETWORKING_TRANSPORT_TRANSPORT_UNIQUE_IDENTIFIER_TYPES_H
