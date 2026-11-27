#ifndef TAG_FILES_FILE_OPEN_DISPOSITION_TYPES_H
#define TAG_FILES_FILE_OPEN_DISPOSITION_TYPES_H

enum e_file_open_disposition
{
    _file_open_disposition_none = -1,
    _file_open_disposition_open_existing = 0,
    _file_open_disposition_create_always = 1,
    _file_open_disposition_create_new = 2,
    _file_open_disposition_open_always = 3,
    _file_open_disposition_open_always_create_directories = 4,
    _file_open_disposition_truncate_existing = 5
};

#endif
