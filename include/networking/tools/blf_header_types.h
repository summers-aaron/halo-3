#ifndef NETWORKING_TOOLS_BLF_HEADER_TYPES_H
#define NETWORKING_TOOLS_BLF_HEADER_TYPES_H

#pragma pack(push, 1)
struct s_blf_header
{
    long chunk_type;
    long chunk_size;
    short major_version;
    short minor_version;

    __forceinline void setup(long type, long size, long major, long minor)
    {
        chunk_type = type;
        chunk_size = size;
        major_version = (short)major;
        minor_version = (short)minor;
    }
};
#pragma pack(pop)

typedef char blf_header_size[(sizeof(long) != 4 || sizeof(s_blf_header) == 0x0c) ? 1 : -1];

#endif // NETWORKING_TOOLS_BLF_HEADER_TYPES_H
