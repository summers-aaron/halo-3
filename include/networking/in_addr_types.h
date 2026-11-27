#ifndef NETWORKING_IN_ADDR_TYPES_H
#define NETWORKING_IN_ADDR_TYPES_H

#ifndef _WINSOCKAPI_
struct in_addr {
    union {
        unsigned long s_addr;
        unsigned char bytes[4];
    };
};

#endif

#endif // NETWORKING_IN_ADDR_TYPES_H
