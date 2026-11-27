#ifndef TAG_FILES_TAG_API_INTEROP_H
#define TAG_FILES_TAG_API_INTEROP_H

#include "tag_files/tag_types.h"

// March tag_api_interop.h:59..60 and tag_api_interop.cpp:361..362;
// 82c9d678 reads address at +4, and 82cc2408 reads descriptor at +0.
struct s_tag_interop
{
    TAG_POINTER(void) descriptor;
    TAG_POINTER(void) address;
};

typedef char tag_interop_size[(sizeof(s_tag_interop) == 8) ? 1 : -1];

#endif // TAG_FILES_TAG_API_INTEROP_H
