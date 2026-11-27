#ifndef TAG_FILES_FILE_LAST_MODIFICATION_DATE_H
#define TAG_FILES_FILE_LAST_MODIFICATION_DATE_H

// Keep the byte alignment of the file timestamp record.
#pragma pack(push, 1)
struct s_file_last_modification_date
{
    // The file APIs copy native FILETIME values into this record.
#ifdef _M_PPCBE
    unsigned long high_date_time;
    unsigned long low_date_time;
#else
    unsigned long low_date_time;
    unsigned long high_date_time;
#endif
};
#pragma pack(pop)

#endif
