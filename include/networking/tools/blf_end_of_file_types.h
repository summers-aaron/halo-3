#ifndef NETWORKING_TOOLS_BLF_END_OF_FILE_TYPES_H
#define NETWORKING_TOOLS_BLF_END_OF_FILE_TYPES_H

#include "networking/tools/blf_header_types.h"

#pragma pack(push, 1)
struct s_blf_chunk_end_of_file
{
    s_blf_header header;
    long total_file_size;
    unsigned char authentication_type;

    __forceinline s_blf_chunk_end_of_file()
    {
        initialize();
    }

    __forceinline void initialize()
    {
        header.setup('_eof', sizeof(*this), 1, 1);
        total_file_size = 0;
        authentication_type = 0;
    }
};
#pragma pack(pop)

#endif
