#ifndef NETWORKING_ONLINE_ONLINE_FILES_DELETER_H
#define NETWORKING_ONLINE_ONLINE_FILES_DELETER_H

#include "cseries/cseries.h"

// Shared by the online file manager and the file-share deletion screen.
class c_online_files_deleter
{
public:
    __forceinline c_online_files_deleter() :
        m_xuid(0),
        m_owner_xuid(0),
        m_slot_index(NONE),
        m_request_cookie(0),
        m_status(0)
    {
    }
    void update();
    unsigned __int64 m_xuid;
    unsigned __int64 m_owner_xuid;
    long m_slot_index;
    char m_name[36];
    long m_request_cookie;
    long m_status;
    char m_results[0x400];
};

typedef char online_files_deleter_size[(sizeof(c_online_files_deleter) == 0x440) ? 1 : -1];
#endif
