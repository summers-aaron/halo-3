#ifndef TAG_FILES_FILE_REFERENCE_H
#define TAG_FILES_FILE_REFERENCE_H

#include <stddef.h>

// File reference shared layout. March file_reference_get_info (829636f8) names
// signature, flags and location, followed by the path, handle and position.
// This leaf remains separate from files.h while held units carry private copies.
struct s_file_reference
{
    unsigned long signature; // 'filo'
    unsigned short flags;
    short location;          // NONE or a file-reference location
    char path[256];
    void *handle;
    unsigned long position;
};

typedef char file_reference_signature_offset[(offsetof(s_file_reference, signature) == 0x000) ? 1 : -1];
typedef char file_reference_flags_offset[(offsetof(s_file_reference, flags) == 0x004) ? 1 : -1];
typedef char file_reference_location_offset[(offsetof(s_file_reference, location) == 0x006) ? 1 : -1];
typedef char file_reference_path_offset[(offsetof(s_file_reference, path) == 0x008) ? 1 : -1];
typedef char file_reference_handle_offset[(offsetof(s_file_reference, handle) == 0x108) ? 1 : -1];
typedef char file_reference_position_offset[(sizeof(void *) != 4 || ((offsetof(s_file_reference, position) == 0x10c) )) ? 1 : -1];
typedef char file_reference_size[(sizeof(void *) != 4 || ((sizeof(s_file_reference) == 0x110) )) ? 1 : -1];

#endif // TAG_FILES_FILE_REFERENCE_H
