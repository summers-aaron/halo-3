#ifndef CSERIES_TYPES_H
#define CSERIES_TYPES_H

typedef signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned __int64 qword;

typedef float real;
typedef long datum_index;
typedef long string_id;
typedef unsigned long tag;

typedef char cseries_types_size_int8[(sizeof(int8) == 1) ? 1 : -1];
typedef char cseries_types_size_int16[(sizeof(int16) == 2) ? 1 : -1];
typedef char cseries_types_size_int32[(sizeof(int32) == 4) ? 1 : -1];
typedef char cseries_types_size_int64[(sizeof(int64) == 8) ? 1 : -1];
typedef char cseries_types_size_real[(sizeof(real) == 4) ? 1 : -1];
typedef char cseries_types_size_datum_index[(sizeof(datum_index) == 4) ? 1 : -1];
typedef char cseries_types_size_string_id[(sizeof(string_id) == 4) ? 1 : -1];
typedef char cseries_types_size_tag[(sizeof(tag) == 4) ? 1 : -1];

#endif // CSERIES_TYPES_H
