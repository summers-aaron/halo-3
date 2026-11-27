#ifndef TAG_FILES_FILES_WINDOWS_TYPES_H
#define TAG_FILES_FILES_WINDOWS_TYPES_H

#include <xtl.h>
#include "tag_files/file_enumeration_types.h"

// Shared by the Windows file iterator and the font modification-date reader.
#include "tag_files/file_last_modification_date.h"

typedef char file_modification_date_size[(sizeof(s_file_last_modification_date) == 8) ? 1 : -1];

#endif // TAG_FILES_FILES_WINDOWS_TYPES_H
