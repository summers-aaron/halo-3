// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xobjbase.h: COM interface-declaration macros, IUnknown and the IStream family.
#if !defined(_XOBJBASE_H_)
#define _XOBJBASE_H_
#pragma once

#define __RPC_FAR

typedef unsigned char byte;
typedef unsigned char boolean;
typedef double DOUBLE;

#ifdef CONST_VTABLE
#define CONST_VTBL const
#else
#define CONST_VTBL
#endif

#ifndef DECLSPEC_SELECTANY
#define DECLSPEC_SELECTANY __declspec(selectany)
#endif
#ifndef DECLSPEC_NOVTABLE
#ifdef __cplusplus
#define DECLSPEC_NOVTABLE __declspec(novtable)
#else
#define DECLSPEC_NOVTABLE
#endif
#endif
#ifndef DECLSPEC_UUID
#ifdef __cplusplus
#define DECLSPEC_UUID(x) __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif

#define MIDL_INTERFACE(x) struct DECLSPEC_UUID(x) DECLSPEC_NOVTABLE
#define EXTERN_GUID(itf, l1, s1, s2, c1, c2, c3, c4, c5, c6, c7, c8) \
    EXTERN_C const IID DECLSPEC_SELECTANY itf = {l1, s1, s2, {c1, c2, c3, c4, c5, c6, c7, c8}}

#include <pshpack8.h>

#define WINOLEAPI EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define WINOLEAPI_(type) EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE

// Interface declaration macros: in C++ an interface is a struct of pure virtual methods; in C (or with CINTERFACE)
// it is a struct holding a pointer to a table of function pointers taking an explicit This.
#if defined(__cplusplus) && !defined(CINTERFACE)
#define interface struct
#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type, method) virtual type STDMETHODCALLTYPE method
#define STDMETHODV(method) virtual HRESULT STDMETHODVCALLTYPE method
#define STDMETHODV_(type, method) virtual type STDMETHODVCALLTYPE method
#define PURE = 0
#define THIS_
#define THIS void
#define DECLARE_INTERFACE(iface) interface DECLSPEC_NOVTABLE iface
#define DECLARE_INTERFACE_(iface, baseiface) interface DECLSPEC_NOVTABLE iface : public baseiface
#if !defined(BEGIN_INTERFACE)
#define BEGIN_INTERFACE
#define END_INTERFACE
#endif
#else
#define interface struct
#define STDMETHOD(method) HRESULT(STDMETHODCALLTYPE *method)
#define STDMETHOD_(type, method) type(STDMETHODCALLTYPE *method)
#define STDMETHODV(method) HRESULT(STDMETHODVCALLTYPE *method)
#define STDMETHODV_(type, method) type(STDMETHODVCALLTYPE *method)
#if !defined(BEGIN_INTERFACE)
#define BEGIN_INTERFACE
#define END_INTERFACE
#endif
#define PURE
#define THIS_ INTERFACE FAR *This,
#define THIS INTERFACE FAR *This
#ifdef CONST_VTABLE
#undef CONST_VTBL
#define CONST_VTBL const
#define DECLARE_INTERFACE(iface)                    \
    typedef interface iface {                       \
        const struct iface##Vtbl FAR *lpVtbl;       \
    } iface;                                        \
    typedef const struct iface##Vtbl iface##Vtbl;   \
    const struct iface##Vtbl
#else
#undef CONST_VTBL
#define CONST_VTBL
#define DECLARE_INTERFACE(iface)                    \
    typedef interface iface {                       \
        struct iface##Vtbl FAR *lpVtbl;             \
    } iface;                                        \
    typedef struct iface##Vtbl iface##Vtbl;         \
    struct iface##Vtbl
#endif
#define DECLARE_INTERFACE_(iface, baseiface) DECLARE_INTERFACE(iface)
#endif

#ifndef FARSTRUCT
#ifdef __cplusplus
#define FARSTRUCT FAR
#else
#define FARSTRUCT
#endif
#endif
#ifndef HUGEP
#define HUGEP
#endif

#include <stdlib.h>

#define LISet32(li, v) ((li).HighPart = (v) < 0 ? -1 : 0, (li).LowPart = (v))
#define ULISet32(li, v) ((li).HighPart = 0, (li).LowPart = (v))

#if defined(_WIN32) && !defined(OLE2ANSI)
typedef WCHAR OLECHAR;
typedef OLECHAR __RPC_FAR *LPOLESTR;
typedef const OLECHAR __RPC_FAR *LPCOLESTR;
#define OLESTR(str) L##str
#else
typedef char OLECHAR;
typedef LPSTR LPOLESTR;
typedef LPCSTR LPCOLESTR;
#define OLESTR(str) str
#endif

#ifndef __IUnknown_FWD_DEFINED__
#define __IUnknown_FWD_DEFINED__
typedef interface IUnknown IUnknown;
#endif

#ifndef __IUnknown_INTERFACE_DEFINED__
#define __IUnknown_INTERFACE_DEFINED__
typedef IUnknown __RPC_FAR *LPUNKNOWN;
typedef IUnknown __RPC_FAR *PUNKNOWN;

EXTERN_C const IID IID_IUnknown;
#if defined(__cplusplus) && !defined(CINTERFACE)
extern "C++" {
MIDL_INTERFACE("00000000-0000-0000-C000-000000000046")
IUnknown
{
public:
    BEGIN_INTERFACE
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
    virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
    // Typed convenience form: asks for the interface identified by Q's uuid.
    template <class Q>
    HRESULT STDMETHODCALLTYPE QueryInterface(Q **pp)
    {
        return QueryInterface(__uuidof(Q), (void **)pp);
    }
    END_INTERFACE
};
}
#else
typedef struct IUnknownVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE __RPC_FAR *QueryInterface)(IUnknown __RPC_FAR *This, REFIID riid,
                                                         void __RPC_FAR *__RPC_FAR *ppvObject);
    ULONG(STDMETHODCALLTYPE __RPC_FAR *AddRef)(IUnknown __RPC_FAR *This);
    ULONG(STDMETHODCALLTYPE __RPC_FAR *Release)(IUnknown __RPC_FAR *This);
    END_INTERFACE
} IUnknownVtbl;
interface IUnknown {
    CONST_VTBL struct IUnknownVtbl __RPC_FAR *lpVtbl;
};
#endif
#endif // __IUnknown_INTERFACE_DEFINED__

#ifndef __ISequentialStream_FWD_DEFINED__
#define __ISequentialStream_FWD_DEFINED__
typedef interface ISequentialStream ISequentialStream;
#endif

#ifndef __ISequentialStream_INTERFACE_DEFINED__
#define __ISequentialStream_INTERFACE_DEFINED__
EXTERN_C const IID IID_ISequentialStream;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0c733a30-2a1c-11ce-ade5-00aa0044773d")
ISequentialStream : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Read(void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbRead) = 0;
    virtual HRESULT STDMETHODCALLTYPE Write(const void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbWritten) = 0;
};
#endif
#endif // __ISequentialStream_INTERFACE_DEFINED__

#ifndef __IStream_FWD_DEFINED__
#define __IStream_FWD_DEFINED__
typedef interface IStream IStream;
#endif

#ifndef __IStream_INTERFACE_DEFINED__
#define __IStream_INTERFACE_DEFINED__
typedef IStream __RPC_FAR *LPSTREAM;

typedef struct tagSTATSTG {
    LPOLESTR pwcsName;
    DWORD type;
    ULARGE_INTEGER cbSize;
    FILETIME mtime;
    FILETIME ctime;
    FILETIME atime;
    DWORD grfMode;
    DWORD grfLocksSupported;
    CLSID clsid;
    DWORD grfStateBits;
    DWORD reserved;
} STATSTG;

typedef enum tagSTGTY {
    STGTY_STORAGE = 1,
    STGTY_STREAM = 2,
    STGTY_LOCKBYTES = 3,
    STGTY_PROPERTY = 4
} STGTY;

typedef enum tagSTREAM_SEEK {
    STREAM_SEEK_SET = 0,
    STREAM_SEEK_CUR = 1,
    STREAM_SEEK_END = 2
} STREAM_SEEK;

typedef enum tagLOCKTYPE {
    LOCK_WRITE = 1,
    LOCK_EXCLUSIVE = 2,
    LOCK_ONLYONCE = 4
} LOCKTYPE;

EXTERN_C const IID IID_IStream;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000000c-0000-0000-C000-000000000046")
IStream : public ISequentialStream
{
public:
    virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
                                           ULARGE_INTEGER __RPC_FAR *plibNewPosition) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream __RPC_FAR *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER __RPC_FAR *pcbRead,
                                             ULARGE_INTEGER __RPC_FAR *pcbWritten) = 0;
    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE Revert(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG __RPC_FAR *pstatstg, DWORD grfStatFlag) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IStream __RPC_FAR *__RPC_FAR *ppstm) = 0;
};
#endif
#endif // __IStream_INTERFACE_DEFINED__

#include <guiddef.h>

#ifndef INITGUID
#include <cguid.h>
#endif

#ifndef RC_INVOKED
#include <poppack.h>
#endif

#endif // _XOBJBASE_H_
