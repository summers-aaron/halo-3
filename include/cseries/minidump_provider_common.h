#ifndef CSERIES_MINIDUMP_PROVIDER_COMMON_H
#define CSERIES_MINIDUMP_PROVIDER_COMMON_H

#include "cseries/minidump_interfaces.h"

class c_minidump_provider_common : public c_minidump_provider_interface
{
public:
    virtual void get_minidump_header(_MINIDUMP_HEADER *header, unsigned long stream_count);
    virtual void get_misc_information(_MINIDUMP_MISC_INFO *info);
    virtual void get_system_information(_MINIDUMP_SYSTEM_INFO *info);
    virtual void get_system_version_string(char *name, unsigned long size);
};

#endif // CSERIES_MINIDUMP_PROVIDER_COMMON_H
