#ifndef NETWORKING_TOOLS_BLF_CHUNK_AUTHOR_TYPES_H
#define NETWORKING_TOOLS_BLF_CHUNK_AUTHOR_TYPES_H

#include <string.h>
#include "networking/tools/blf_header_types.h"

#pragma pack(push, 4)
// Wire storage is also used by campaign files initialized explicitly in place.
struct s_blf_chunk_author_data
{
    s_blf_header header;
    char build_name[16];
    unsigned __int64 build_identifier;
    char build_string[28];
    char author_name[16];
};

struct s_blf_chunk_author : s_blf_chunk_author_data
{
    s_blf_chunk_author();
    __forceinline void initialize()
    {
        header.setup('athr', sizeof(*this), 3, 1);
        memset(build_name, 0, sizeof(build_name));
        build_identifier = 0;
        memset(build_string, 0, sizeof(build_string));
        memset(author_name, 0, sizeof(author_name));
    }
};
#pragma pack(pop)

typedef char blf_chunk_author_size[(sizeof(long) != 4 || sizeof(s_blf_chunk_author) == 0x50) ? 1 : -1];
typedef char blf_chunk_author_data_size[(sizeof(long) != 4 || sizeof(s_blf_chunk_author_data) == 0x50) ? 1 : -1];

#endif // NETWORKING_TOOLS_BLF_CHUNK_AUTHOR_TYPES_H
