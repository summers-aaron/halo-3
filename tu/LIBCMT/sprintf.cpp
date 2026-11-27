// LIBCMT/sprintf.obj: from the records 0x150fe8
// @compiler xdk6130
// @flags /TC /O1
// @address 0x150fe8 sprintf_s
// @address 0x151030 _vsprintf_s_l
// XDK CRT sprintf.c (sprintf_s); libcMT.lib builds it /O1 /Gy
#include <stdio.h>
#include <stdarg.h>

extern __inline int __cdecl sprintf_s(
        char *string,
        size_t sizeInBytes,
        const char *format,
        ...
        )
{
        va_list arglist;
        va_start(arglist, format);
        return _vsprintf_s_l(string, sizeInBytes, format, ((void *)0), arglist);
}
