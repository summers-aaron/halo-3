// LIBCMT/strncpy_s.obj: from the records 0x948e8
// @compiler xdk6130
// @flags /TC /O1 /Oc
// @address 0x948e8 strncpy_s
/* XDK CRT strncpy_s.c (strncpy_s); libcMT.lib builds it /O1 /Gy */
#include <stdlib.h>
#include "cseries/crt_internal.h"
__forceinline
void _invoke_watson_if_error(
    errno_t _ExpressionError,
    const wchar_t *_Expression,
    const wchar_t *_Function,
    const wchar_t *_File,
    unsigned int _Line,
    uintptr_t _Reserved
    )
{
    if (_ExpressionError == 0)
    {
        return;
    }
    _invoke_watson(_Expression, _Function, _File, _Line, _Reserved);
}
errno_t __cdecl strncpy_s(char *_Dst, size_t _SizeInBytes, const char *_Src, size_t _Count)
{
    char *p;
    size_t available;
    if (_Count == 0 && _Dst == ((void *)0) && _SizeInBytes == 0)
    {
        return 0;
    }
    {
        int _Expr_val = !!(((_Dst)) != ((void *)0) && ((_SizeInBytes)) > 0);
        ((void)0);
        if (!(_Expr_val))
        {
            (*_errno()) = 22;
            _invalid_parameter(((void *)0), ((void *)0), ((void *)0), 0, 0);
            return ((22));
        }
    };
    if (_Count == 0)
    {
        *(_Dst) = 0; ;;
        return 0;
    }
    if (((_Src)) == ((void *)0))
    {
        *(((_Dst))) = 0;
        ;
        ;
        {
            int _Expr_val = !!((((_Src))) != ((void *)0));
            ((void)0);
            if (!(_Expr_val))
            {
                (*_errno()) = (22);
                _invalid_parameter(((void *)0), ((void *)0), ((void *)0), 0, 0);
                return (((22)));
            }
        }
    };
    p = _Dst;
    available = _SizeInBytes;
    if (_Count == ((size_t)-1))
    {
        while ((*p++ = *_Src++) != 0 && --available > 0)
        {
        }
    }
    else
    {
        ((void)0);
        while ((*p++ = *_Src++) != 0 && --available > 0 && --_Count > 0)
        {
        }
        if (_Count == 0)
        {
            *p = 0;
        }
    }
    if (available == 0)
    {
        if (_Count == ((size_t)-1))
        {
            _Dst[_SizeInBytes - 1] = 0;
            return 80;
        }
        *(_Dst) = 0; ;;
        {
            int _Expr_val = !!((L"Buffer is too small" && 0));
            ((void)0);
            if (!(_Expr_val))
            {
                (*_errno()) = 34;
                _invalid_parameter(((void *)0), ((void *)0), ((void *)0), 0, 0);
                return (34);
            }
        };
    }
    ;
    return 0;
}
