#ifndef HS_HS_VALUE_TYPES_H
#define HS_HS_VALUE_TYPES_H

#include "tag_files/tag_types.h"

union hs_value
{
    long long_value;
    bool boolean_value;
    unsigned char byte_value;
    float real_value;
    unsigned short short_value;
    TAG_POINTER(char) string_value;
};

typedef char hs_value_size[(sizeof(void *) != 4 || sizeof(hs_value) == 4) ? 1 : -1];

#endif // HS_HS_VALUE_TYPES_H
