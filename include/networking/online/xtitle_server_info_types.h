#ifndef NETWORKING_ONLINE_XTITLE_SERVER_INFO_TYPES_H
#define NETWORKING_ONLINE_XTITLE_SERVER_INFO_TYPES_H

#ifndef _XTL_
struct XTITLE_SERVER_INFO
{
    unsigned long address;
    unsigned long flags;
    char description[200];
};
#endif

typedef char xtitle_server_info_size[(sizeof(unsigned long) != 4 || sizeof(XTITLE_SERVER_INFO) == 0xd0) ? 1 : -1];

#endif // NETWORKING_ONLINE_XTITLE_SERVER_INFO_TYPES_H
