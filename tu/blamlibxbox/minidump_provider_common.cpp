// blamlibxbox/minidump_provider_common.obj: from the records 0x2ff968 0x2ff9e8 0x2ffa20 0x2ffa78
// @flags /TP /O2 /Oc

#include <time.h>
#include "cseries/minidump_provider_common.h"
#include "cseries/minidump_streams.h"
#include "cseries/minidump_writer.h"

// @address 0x2ff968 ?get_minidump_header@c_minidump_provider_common@@UAAXPAU_MINIDUMP_HEADER@@K@Z
void c_minidump_provider_common::get_minidump_header(_MINIDUMP_HEADER *header, unsigned long stream_count)
{
    if (header != 0)
    {
        __int64 current_time;
        _time64(&current_time);

        header->fields.signature = 0x504d444d;
        header->fields.version = 0x600ea793;
        header->fields.stream_count = stream_count;
        header->fields.directory_rva = sizeof(*header);
        header->fields.checksum = 0;
        header->fields.timestamp = (unsigned long)current_time;
        header->fields.flags.QuadPart = 0;
    }
}

// @address 0x2ff9e8 ?get_misc_information@c_minidump_provider_common@@UAAXPAU_MINIDUMP_MISC_INFO@@@Z
void c_minidump_provider_common::get_misc_information(_MINIDUMP_MISC_INFO *info)
{
    if (info == 0)
        return;

    info->size_of_info = 24;
    info->flags1 = 1;
    info->process_id = 0xa5c0b46f;
    info->process_create_time = 0;
    info->process_user_time = 0;
    info->process_kernel_time = 0;
}

// @address 0x2ffa20 ?get_system_information@c_minidump_provider_common@@UAAXPAU_MINIDUMP_SYSTEM_INFO@@@Z
void c_minidump_provider_common::get_system_information(_MINIDUMP_SYSTEM_INFO *info)
{
    if (info == 0)
        return;

    info->processor_architecture = 3;
    info->processor_level = 1;
    info->processor_revision = 1;
    info->number_of_processors = 2;
    info->product_type = 1;
    info->major_version = 5;
    info->minor_version = 1;
    info->build_number = 0xb68;
    info->platform_id = 2;
    info->suite_mask = 0;
    info->cpu.other.processor_features[0] = 0;
    info->cpu.other.processor_features[1] = 0;
    info->csd_version_rva = 0;
}

// @address 0x2ffa78 ?get_system_version_string@c_minidump_provider_common@@UAAXPADK@Z
void c_minidump_provider_common::get_system_version_string(char *name, unsigned long size)
{
    if (name != 0)
    {
        strncpy(name, "Xenon", size);
        name[size - 1] = 0;
    }
}
