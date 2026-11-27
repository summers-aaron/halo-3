// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// new.h: the runtime's allocation-failure handlers (new mode, _set_new_handler) beside the standard new_handler.
#pragma once
#ifndef _INC_NEW
#define _INC_NEW
#ifdef __cplusplus

// Under MS extensions this header stands alone (it declares what it needs of <new> itself).
#ifndef _MSC_EXTENSIONS
#include <new>
#endif
#include <crtdefs.h>

#pragma push_macro("new")
#undef new

#ifndef _CRTIMP2
#if defined(_DLL) && !defined(_STATIC_CPPLIB) && _XBOX_VER < 200
#define _CRTIMP2 __declspec(dllimport)
#else
#define _CRTIMP2
#endif
#endif

#ifdef _MSC_EXTENSIONS
namespace std {
typedef void(__cdecl *new_handler)();
_CRTIMP2 new_handler __cdecl set_new_handler(new_handler _NewHandler) throw();
}
using std::new_handler;
using std::set_new_handler;
#endif

#ifndef __NOTHROW_T_DEFINED
#define __NOTHROW_T_DEFINED
namespace std {
struct nothrow_t
{
};
extern const nothrow_t nothrow;
}
void *__CRTDECL operator new(size_t _Size, const std::nothrow_t &) throw();
void *__CRTDECL operator new[](size_t _Size, const std::nothrow_t &) throw();
void __CRTDECL operator delete(void *, const std::nothrow_t &) throw();
void __CRTDECL operator delete[](void *, const std::nothrow_t &) throw();
#endif

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void *__CRTDECL operator new(size_t, void *where)
{
    return where;
}
inline void __CRTDECL operator delete(void *, void *)
{
}
#endif

// New mode: when set, malloc failures also call the new handler.
_CRTIMP int __cdecl _query_new_mode(void);
_CRTIMP int __cdecl _set_new_mode(int _NewMode);
#ifndef _PNH_DEFINED
#define _PNH_DEFINED
typedef int(__cdecl *_PNH)(size_t);
#endif
_CRTIMP _PNH __cdecl _query_new_handler(void);
_CRTIMP _PNH __cdecl _set_new_handler(_PNH _NewHandler);

#ifndef _NO_ANSI_NH_DEFINED
#define _NO_ANSI_NH_DEFINED
#define _NO_ANSI_NEW_HANDLER ((new_handler)-1)
#define _NO_ANSI_NEW_HANDLER_M ((_new_handler_m)-1)
#endif

#pragma pop_macro("new")
#endif // __cplusplus
#endif // _INC_NEW
