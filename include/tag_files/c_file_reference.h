#ifndef TAG_FILES_C_FILE_REFERENCE_H
#define TAG_FILES_C_FILE_REFERENCE_H

#include "tag_files/file_reference.h"

class c_file_reference : public s_file_reference
{
public:
    c_file_reference(const char *path, unsigned long flags);
    bool m_open;
private:
    __forceinline bool find_or_create(unsigned long flags);
};

#endif // TAG_FILES_C_FILE_REFERENCE_H
