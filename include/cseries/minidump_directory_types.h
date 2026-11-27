#ifndef CSERIES_MINIDUMP_DIRECTORY_TYPES_H
#define CSERIES_MINIDUMP_DIRECTORY_TYPES_H

#include <stddef.h>

// The 12-byte stream directory entry shared by the minidump writers.
struct s_minidump_entry
{
    long stream_type;
    long data_size;
    long rva;
};

typedef char minidump_directory_entry_size[(sizeof(s_minidump_entry) == 12) ? 1 : -1];
typedef char minidump_directory_rva_offset[(offsetof(s_minidump_entry, rva) == 8) ? 1 : -1];

#endif // CSERIES_MINIDUMP_DIRECTORY_TYPES_H
