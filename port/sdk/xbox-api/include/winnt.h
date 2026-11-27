// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// winnt.h: NT-level Win32 types, big-endian Xbox layouts (LARGE_INTEGER, CONTEXT), SLISTs, critical sections.
#ifndef _WINNT_
#define _WINNT_

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>

#define ANYSIZE_ARRAY 1
#define RESTRICTED_POINTER

// The Xbox 360 is a PowerPC target: UNALIGNED is the __unaligned qualifier (it reaches mangled names).
#if !defined(UNALIGNED) && !defined(_NOUNALIGNED_)
#define UNALIGNED __unaligned
#endif
#ifndef UNALIGNED64
#define UNALIGNED64
#endif

#define MAX_NATURAL_ALIGNMENT sizeof(DWORD)
#ifndef __cplusplus
#define TYPE_ALIGNMENT(t) FIELD_OFFSET(struct { char x; t test; }, test)
#endif
#define PROBE_ALIGNMENT(_s) TYPE_ALIGNMENT(DWORD)
#define C_ASSERT(e) typedef char __C_ASSERT__[(e) ? 1 : -1]

#define POINTER_64
#define POINTER_32
#define FIRMWARE_PTR POINTER_32
typedef unsigned long POINTER_64_INT;

#include <basetsd.h>

// No x86/IA64 machine macro is defined, so import declarations are plain declarations.
#define DECLSPEC_IMPORT
#ifndef DECLSPEC_NORETURN
#define DECLSPEC_NORETURN __declspec(noreturn)
#endif
#ifndef DECLSPEC_ALIGN
#define DECLSPEC_ALIGN(x) __declspec(align(x))
#endif
#ifndef DECLSPEC_UUID
#ifdef __cplusplus
#define DECLSPEC_UUID(x) __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif
#ifndef DECLSPEC_NOVTABLE
#ifdef __cplusplus
#define DECLSPEC_NOVTABLE __declspec(novtable)
#else
#define DECLSPEC_NOVTABLE
#endif
#endif
#ifndef DECLSPEC_SELECTANY
#define DECLSPEC_SELECTANY __declspec(selectany)
#endif
#ifndef NOP_FUNCTION
#define NOP_FUNCTION __noop
#endif
#ifndef DECLSPEC_ADDRSAFE
#define DECLSPEC_ADDRSAFE
#endif
#ifndef DECLSPEC_NOINLINE
#define DECLSPEC_NOINLINE __declspec(noinline)
#endif

typedef void *PVOID;
typedef void *PVOID64;

#define FASTCALL
#define FORCEINLINE static __forceinline
#define NTAPI __stdcall
#define NTSYSAPI DECLSPEC_IMPORT
#define NTSYSCALLAPI DECLSPEC_IMPORT

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

typedef wchar_t WCHAR;
typedef WCHAR *PWCHAR, *LPWCH, *PWCH, *NWPSTR, *LPWSTR, *PWSTR;
typedef CONST WCHAR *LPCWCH, *PCWCH, *LPCWSTR, *PCWSTR;
typedef CHAR *PCHAR, *LPCH, *PCH, *NPSTR, *LPSTR, *PSTR;
typedef CONST CHAR *LPCCH, *PCCH, *LPCSTR, *PCSTR;

#ifdef UNICODE
#ifndef _TCHAR_DEFINED
#define _TCHAR_DEFINED
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE, *PTBYTE;
#endif
typedef LPWSTR LPTCH, PTCH, PTSTR, LPTSTR, LP;
typedef LPCWSTR PCTSTR, LPCTSTR;
#define __TEXT(quote) L##quote
#else
#ifndef _TCHAR_DEFINED
#define _TCHAR_DEFINED
typedef char TCHAR, *PTCHAR;
typedef unsigned char TBYTE, *PTBYTE;
#endif
typedef LPSTR LPTCH, PTCH, PTSTR, LPTSTR;
typedef LPCSTR PCTSTR, LPCTSTR;
#define __TEXT(quote) quote
#endif
#define TEXT(quote) __TEXT(quote)

typedef SHORT *PSHORT;
typedef LONG *PLONG;

#ifdef STRICT
typedef void *HANDLE;
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#else
typedef PVOID HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name
#endif
typedef HANDLE *PHANDLE;

typedef BYTE FCHAR;
typedef WORD FSHORT;
typedef DWORD FLONG;

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;
#endif

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif

#define STDMETHODCALLTYPE __stdcall
#define STDMETHODVCALLTYPE __cdecl
#define STDAPICALLTYPE __stdcall
#define STDAPIVCALLTYPE __cdecl
#define STDAPI EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type) EXTERN_C type STDAPICALLTYPE
#define STDMETHODIMP HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type) type STDMETHODCALLTYPE
#define STDAPIV EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type) EXTERN_C type STDAPIVCALLTYPE
#define STDMETHODIMPV HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_(type) type STDMETHODVCALLTYPE

typedef char CCHAR;
typedef DWORD LCID;
typedef PDWORD PLCID;
typedef WORD LANGID;

#define APPLICATION_ERROR_MASK 0x20000000
#define ERROR_SEVERITY_SUCCESS 0x00000000
#define ERROR_SEVERITY_INFORMATIONAL 0x40000000
#define ERROR_SEVERITY_WARNING 0x80000000
#define ERROR_SEVERITY_ERROR 0xC0000000

typedef struct _FLOAT128 {
    __int64 LowPart;
    __int64 HighPart;
} FLOAT128, *PFLOAT128;

#define _ULONGLONG_
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
#define MAXLONGLONG (0x7fffffffffffffff)
#define MAXULONGLONG (0xffffffffffffffff)
typedef LONGLONG *PLONGLONG;
typedef ULONGLONG *PULONGLONG;
typedef LONGLONG USN;

// 64-bit integers split into halves. _M_PPCBE is defined, so the halves are in big-endian (high first) order.
typedef union _LARGE_INTEGER {
    struct {
        LONG HighPart;
        DWORD LowPart;
    };
    struct {
        LONG HighPart;
        DWORD LowPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD HighPart;
        DWORD LowPart;
    };
    struct {
        DWORD HighPart;
        DWORD LowPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

typedef struct _LUID {
    DWORD LowPart;
    LONG HighPart;
} LUID, *PLUID;

#define _DWORDLONG_
typedef ULONGLONG DWORDLONG;
typedef DWORDLONG *PDWORDLONG;

#define Int32x32To64(a, b) ((LONGLONG)((LONG)(a)) * (LONGLONG)((LONG)(b)))
#define UInt32x32To64(a, b) ((ULONGLONG)((DWORD)(a)) * (ULONGLONG)((DWORD)(b)))
#define Int64ShllMod32(a, b) ((ULONGLONG)(a) << (b))
#define Int64ShraMod32(a, b) ((LONGLONG)(a) >> (b))
#define Int64ShrlMod32(a, b) ((ULONGLONG)(a) >> (b))

#define ANSI_NULL ((CHAR)0)
#define UNICODE_NULL ((WCHAR)0)
typedef BYTE BOOLEAN;
typedef BOOLEAN *PBOOLEAN;

typedef struct _LIST_ENTRY {
    struct _LIST_ENTRY *Flink;
    struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;

typedef struct _SINGLE_LIST_ENTRY {
    struct _SINGLE_LIST_ENTRY *Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY;

typedef struct LIST_ENTRY32 {
    DWORD Flink;
    DWORD Blink;
} LIST_ENTRY32, *PLIST_ENTRY32;

typedef struct LIST_ENTRY64 {
    ULONGLONG Flink;
    ULONGLONG Blink;
} LIST_ENTRY64, *PLIST_ENTRY64;

#include <guiddef.h>

#define MINCHAR 0x80
#define MAXCHAR 0x7f
#define MINSHORT 0x8000
#define MAXSHORT 0x7fff
#define MINLONG 0x80000000
#define MAXLONG 0x7fffffff
#define MAXBYTE 0xff
#define MAXWORD 0xffff
#define MAXDWORD 0xffffffff

#define FIELD_OFFSET(type, field) ((LONG)(LONG_PTR)&(((type *)0)->field))
#define RTL_FIELD_SIZE(type, field) (sizeof(((type *)0)->field))
#define RTL_SIZEOF_THROUGH_FIELD(type, field) (FIELD_OFFSET(type, field) + RTL_FIELD_SIZE(type, field))
#define RTL_CONTAINS_FIELD(Struct, Size, Field) \
    ((((PCHAR)(&(Struct)->Field)) + sizeof((Struct)->Field)) <= (((PCHAR)(Struct)) + (Size)))
#define RTL_NUMBER_OF_V1(A) (sizeof(A) / sizeof((A)[0]))
#if defined(__cplusplus) && !defined(MIDL_PASS) && !defined(RC_INVOKED) && !defined(_PREFAST_) && !defined(SORTPP_PASS)
// Declared only, never defined: sizeof of the returned array-pointer's target yields the element count.
extern "C++" template <typename RTLP_T, size_t N> char (*RtlpNumberOf(UNALIGNED RTLP_T (&)[N]))[N];
#define RTL_NUMBER_OF_V2(A) (sizeof(*RtlpNumberOf(A)))
#else
#define RTL_NUMBER_OF_V2(A) RTL_NUMBER_OF_V1(A)
#endif
#ifdef ENABLE_RTL_NUMBER_OF_V2
#define RTL_NUMBER_OF(A) RTL_NUMBER_OF_V2(A)
#else
#define RTL_NUMBER_OF(A) RTL_NUMBER_OF_V1(A)
#endif
#define ARRAYSIZE(A) RTL_NUMBER_OF_V2(A)
#define _ARRAYSIZE(A) RTL_NUMBER_OF_V1(A)
#define CONTAINING_RECORD(address, type, field) ((type *)((PCHAR)(address) - (ULONG_PTR)(&((type *)0)->field)))

#define LANG_NEUTRAL 0x00
#define LANG_ENGLISH 0x09
#define SUBLANG_NEUTRAL 0x00
#define SUBLANG_DEFAULT 0x01
#define SUBLANG_SYS_DEFAULT 0x02
#define SORT_DEFAULT 0x0
#define MAKELANGID(p, s) ((((WORD)(s)) << 10) | (WORD)(p))
#define PRIMARYLANGID(lgid) ((WORD)(lgid) & 0x3ff)
#define SUBLANGID(lgid) ((WORD)(lgid) >> 10)
#define MAKELCID(lgid, srtid) ((DWORD)((((DWORD)((WORD)(srtid))) << 16) | ((DWORD)((WORD)(lgid)))))
#define LANGIDFROMLCID(lcid) ((WORD)(lcid))

#define UNREFERENCED_PARAMETER(P) (P)
#define DBG_UNREFERENCED_PARAMETER(P) (P)
#define DBG_UNREFERENCED_LOCAL_VARIABLE(V) (V)
#define DEFAULT_UNREACHABLE default: __assume(0)

#ifndef WIN32_NO_STATUS
#define STATUS_WAIT_0 ((DWORD)0x00000000L)
#define STATUS_ABANDONED_WAIT_0 ((DWORD)0x00000080L)
#define STATUS_USER_APC ((DWORD)0x000000C0L)
#define STATUS_TIMEOUT ((DWORD)0x00000102L)
#define STATUS_PENDING ((DWORD)0x00000103L)
#define DBG_CONTINUE ((DWORD)0x00010002L)
#define DBG_TERMINATE_THREAD ((DWORD)0x40010003L)
#define DBG_TERMINATE_PROCESS ((DWORD)0x40010004L)
#define DBG_CONTROL_C ((DWORD)0x40010005L)
#define DBG_CONTROL_BREAK ((DWORD)0x40010008L)
#define STATUS_GUARD_PAGE_VIOLATION ((DWORD)0x80000001L)
#define STATUS_DATATYPE_MISALIGNMENT ((DWORD)0x80000002L)
#define STATUS_BREAKPOINT ((DWORD)0x80000003L)
#define STATUS_SINGLE_STEP ((DWORD)0x80000004L)
#define DBG_EXCEPTION_NOT_HANDLED ((DWORD)0x80010001L)
#define STATUS_ACCESS_VIOLATION ((DWORD)0xC0000005L)
#define STATUS_IN_PAGE_ERROR ((DWORD)0xC0000006L)
#define STATUS_INVALID_HANDLE ((DWORD)0xC0000008L)
#define STATUS_NO_MEMORY ((DWORD)0xC0000017L)
#define STATUS_ILLEGAL_INSTRUCTION ((DWORD)0xC000001DL)
#define STATUS_NONCONTINUABLE_EXCEPTION ((DWORD)0xC0000025L)
#define STATUS_INVALID_DISPOSITION ((DWORD)0xC0000026L)
#define STATUS_ARRAY_BOUNDS_EXCEEDED ((DWORD)0xC000008CL)
#define STATUS_FLOAT_DENORMAL_OPERAND ((DWORD)0xC000008DL)
#define STATUS_FLOAT_DIVIDE_BY_ZERO ((DWORD)0xC000008EL)
#define STATUS_FLOAT_INEXACT_RESULT ((DWORD)0xC000008FL)
#define STATUS_FLOAT_INVALID_OPERATION ((DWORD)0xC0000090L)
#define STATUS_FLOAT_OVERFLOW ((DWORD)0xC0000091L)
#define STATUS_FLOAT_STACK_CHECK ((DWORD)0xC0000092L)
#define STATUS_FLOAT_UNDERFLOW ((DWORD)0xC0000093L)
#define STATUS_INTEGER_DIVIDE_BY_ZERO ((DWORD)0xC0000094L)
#define STATUS_INTEGER_OVERFLOW ((DWORD)0xC0000095L)
#define STATUS_PRIVILEGED_INSTRUCTION ((DWORD)0xC0000096L)
#define STATUS_STACK_OVERFLOW ((DWORD)0xC00000FDL)
#define STATUS_CONTROL_C_EXIT ((DWORD)0xC000013AL)
#define STATUS_FLOAT_MULTIPLE_FAULTS ((DWORD)0xC00002B4L)
#define STATUS_FLOAT_MULTIPLE_TRAPS ((DWORD)0xC00002B5L)
#define STATUS_REG_NAT_CONSUMPTION ((DWORD)0xC00002C9L)
#endif

#define MAXIMUM_WAIT_OBJECTS 64
#define MAXIMUM_SUSPEND_COUNT MAXCHAR

typedef ULONG_PTR KSPIN_LOCK;
typedef KSPIN_LOCK *PKSPIN_LOCK;

#if defined(_PPC_)
#if !defined(RC_INVOKED)
#define CONTEXT_CONTROL 0x00000001L
#define CONTEXT_FLOATING_POINT 0x00000002L
#define CONTEXT_INTEGER 0x00000004L
#define CONTEXT_VECTOR 0x00000010L
#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER | CONTEXT_VECTOR)
#endif

// Xenon (PowerPC) thread context: control registers, 32 GPRs, FPSCR + 32 FPRs, then VSCR + 128 vector registers.
typedef struct _CONTEXT {
    DWORD ContextFlags;
    DWORD Msr;
    DWORD Iar;
    DWORD Lr;
    ULONGLONG Ctr;
    ULONGLONG Gpr0;
    ULONGLONG Gpr1;
    ULONGLONG Gpr2;
    ULONGLONG Gpr3;
    ULONGLONG Gpr4;
    ULONGLONG Gpr5;
    ULONGLONG Gpr6;
    ULONGLONG Gpr7;
    ULONGLONG Gpr8;
    ULONGLONG Gpr9;
    ULONGLONG Gpr10;
    ULONGLONG Gpr11;
    ULONGLONG Gpr12;
    ULONGLONG Gpr13;
    ULONGLONG Gpr14;
    ULONGLONG Gpr15;
    ULONGLONG Gpr16;
    ULONGLONG Gpr17;
    ULONGLONG Gpr18;
    ULONGLONG Gpr19;
    ULONGLONG Gpr20;
    ULONGLONG Gpr21;
    ULONGLONG Gpr22;
    ULONGLONG Gpr23;
    ULONGLONG Gpr24;
    ULONGLONG Gpr25;
    ULONGLONG Gpr26;
    ULONGLONG Gpr27;
    ULONGLONG Gpr28;
    ULONGLONG Gpr29;
    ULONGLONG Gpr30;
    ULONGLONG Gpr31;
    DWORD Cr;
    DWORD Xer;
    double Fpscr;
    double Fpr0;
    double Fpr1;
    double Fpr2;
    double Fpr3;
    double Fpr4;
    double Fpr5;
    double Fpr6;
    double Fpr7;
    double Fpr8;
    double Fpr9;
    double Fpr10;
    double Fpr11;
    double Fpr12;
    double Fpr13;
    double Fpr14;
    double Fpr15;
    double Fpr16;
    double Fpr17;
    double Fpr18;
    double Fpr19;
    double Fpr20;
    double Fpr21;
    double Fpr22;
    double Fpr23;
    double Fpr24;
    double Fpr25;
    double Fpr26;
    double Fpr27;
    double Fpr28;
    double Fpr29;
    double Fpr30;
    double Fpr31;
    DWORD UserModeControl;
    DWORD Fill;
    float Vscr[4];
    float Vr0[4];
    float Vr1[4];
    float Vr2[4];
    float Vr3[4];
    float Vr4[4];
    float Vr5[4];
    float Vr6[4];
    float Vr7[4];
    float Vr8[4];
    float Vr9[4];
    float Vr10[4];
    float Vr11[4];
    float Vr12[4];
    float Vr13[4];
    float Vr14[4];
    float Vr15[4];
    float Vr16[4];
    float Vr17[4];
    float Vr18[4];
    float Vr19[4];
    float Vr20[4];
    float Vr21[4];
    float Vr22[4];
    float Vr23[4];
    float Vr24[4];
    float Vr25[4];
    float Vr26[4];
    float Vr27[4];
    float Vr28[4];
    float Vr29[4];
    float Vr30[4];
    float Vr31[4];
    float Vr32[4];
    float Vr33[4];
    float Vr34[4];
    float Vr35[4];
    float Vr36[4];
    float Vr37[4];
    float Vr38[4];
    float Vr39[4];
    float Vr40[4];
    float Vr41[4];
    float Vr42[4];
    float Vr43[4];
    float Vr44[4];
    float Vr45[4];
    float Vr46[4];
    float Vr47[4];
    float Vr48[4];
    float Vr49[4];
    float Vr50[4];
    float Vr51[4];
    float Vr52[4];
    float Vr53[4];
    float Vr54[4];
    float Vr55[4];
    float Vr56[4];
    float Vr57[4];
    float Vr58[4];
    float Vr59[4];
    float Vr60[4];
    float Vr61[4];
    float Vr62[4];
    float Vr63[4];
    float Vr64[4];
    float Vr65[4];
    float Vr66[4];
    float Vr67[4];
    float Vr68[4];
    float Vr69[4];
    float Vr70[4];
    float Vr71[4];
    float Vr72[4];
    float Vr73[4];
    float Vr74[4];
    float Vr75[4];
    float Vr76[4];
    float Vr77[4];
    float Vr78[4];
    float Vr79[4];
    float Vr80[4];
    float Vr81[4];
    float Vr82[4];
    float Vr83[4];
    float Vr84[4];
    float Vr85[4];
    float Vr86[4];
    float Vr87[4];
    float Vr88[4];
    float Vr89[4];
    float Vr90[4];
    float Vr91[4];
    float Vr92[4];
    float Vr93[4];
    float Vr94[4];
    float Vr95[4];
    float Vr96[4];
    float Vr97[4];
    float Vr98[4];
    float Vr99[4];
    float Vr100[4];
    float Vr101[4];
    float Vr102[4];
    float Vr103[4];
    float Vr104[4];
    float Vr105[4];
    float Vr106[4];
    float Vr107[4];
    float Vr108[4];
    float Vr109[4];
    float Vr110[4];
    float Vr111[4];
    float Vr112[4];
    float Vr113[4];
    float Vr114[4];
    float Vr115[4];
    float Vr116[4];
    float Vr117[4];
    float Vr118[4];
    float Vr119[4];
    float Vr120[4];
    float Vr121[4];
    float Vr122[4];
    float Vr123[4];
    float Vr124[4];
    float Vr125[4];
    float Vr126[4];
    float Vr127[4];
} CONTEXT, *PCONTEXT;

typedef struct _STACK_FRAME_HEADER {
    DWORD BackChain;
    DWORD CompilerReserved;
    DWORD CalleeOwned1;
    DWORD CalleeOwned2;
    ULONGLONG Parameter0;
    ULONGLONG Parameter1;
    ULONGLONG Parameter2;
    ULONGLONG Parameter3;
    ULONGLONG Parameter4;
    ULONGLONG Parameter5;
    ULONGLONG Parameter6;
    ULONGLONG Parameter7;
} STACK_FRAME_HEADER, *PSTACK_FRAME_HEADER;

VOID __jump_unwind(PVOID Fp, PVOID TargetPc);
#endif // _PPC_

#define EXCEPTION_NONCONTINUABLE 0x1
#define EXCEPTION_MAXIMUM_PARAMETERS 15

typedef struct _EXCEPTION_RECORD {
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    DWORD NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD;
typedef EXCEPTION_RECORD *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

typedef PVOID PACCESS_TOKEN;
typedef PVOID PSECURITY_DESCRIPTOR;
typedef PVOID PSID;
typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK *PACCESS_MASK;

#define DELETE (0x00010000L)
#define READ_CONTROL (0x00020000L)
#define WRITE_DAC (0x00040000L)
#define WRITE_OWNER (0x00080000L)
#define SYNCHRONIZE (0x00100000L)
#define STANDARD_RIGHTS_REQUIRED (0x000F0000L)
#define STANDARD_RIGHTS_READ (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE (READ_CONTROL)
#define STANDARD_RIGHTS_ALL (0x001F0000L)
#define SPECIFIC_RIGHTS_ALL (0x0000FFFFL)
#define ACCESS_SYSTEM_SECURITY (0x01000000L)
#define MAXIMUM_ALLOWED (0x02000000L)
#define GENERIC_READ (0x80000000L)
#define GENERIC_WRITE (0x40000000L)
#define GENERIC_EXECUTE (0x20000000L)
#define GENERIC_ALL (0x10000000L)

#define TLS_MINIMUM_AVAILABLE 32

#define THREAD_BASE_PRIORITY_LOWRT 15
#define THREAD_BASE_PRIORITY_MAX 2
#define THREAD_BASE_PRIORITY_MIN -2
#define THREAD_BASE_PRIORITY_IDLE -15

#define NtCurrentProcess() ((HANDLE)-1)
#define NtCurrentThread() ((HANDLE)-2)

#define EVENT_MODIFY_STATE 0x0002
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3)
#define MUTANT_QUERY_STATE 0x0001
#define MUTANT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | MUTANT_QUERY_STATE)
#define SEMAPHORE_MODIFY_STATE 0x0002
#define SEMAPHORE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3)

typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

#define PAGE_NOACCESS 0x01
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOPY 0x08
#define PAGE_EXECUTE 0x10
#define PAGE_EXECUTE_READ 0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD 0x100
#define PAGE_NOCACHE 0x200
#define PAGE_WRITECOMBINE 0x400
#define PAGE_USER_READONLY 0x1000
#define PAGE_USER_READWRITE 0x2000
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_DECOMMIT 0x4000
#define MEM_RELEASE 0x8000
#define MEM_FREE 0x10000
#define MEM_PRIVATE 0x20000
#define MEM_RESET 0x80000
#define MEM_TOP_DOWN 0x100000
#define MEM_NOZERO 0x800000
#define MEM_LARGE_PAGES 0x20000000
#define MEM_HEAP 0x40000000
#define MEM_16MB_PAGES 0x80000000

#define FILE_READ_DATA (0x0001)
#define FILE_LIST_DIRECTORY (0x0001)
#define FILE_WRITE_DATA (0x0002)
#define FILE_ADD_FILE (0x0002)
#define FILE_APPEND_DATA (0x0004)
#define FILE_ADD_SUBDIRECTORY (0x0004)
#define FILE_CREATE_PIPE_INSTANCE (0x0004)
#define FILE_READ_EA (0x0008)
#define FILE_WRITE_EA (0x0010)
#define FILE_EXECUTE (0x0020)
#define FILE_TRAVERSE (0x0020)
#define FILE_DELETE_CHILD (0x0040)
#define FILE_READ_ATTRIBUTES (0x0080)
#define FILE_WRITE_ATTRIBUTES (0x0100)
#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)
#define FILE_GENERIC_READ (STANDARD_RIGHTS_READ | FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | SYNCHRONIZE)
#define FILE_GENERIC_WRITE \
    (STANDARD_RIGHTS_WRITE | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA | SYNCHRONIZE)
#define FILE_GENERIC_EXECUTE (STANDARD_RIGHTS_EXECUTE | FILE_READ_ATTRIBUTES | FILE_EXECUTE | SYNCHRONIZE)
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_SHARE_DELETE 0x00000004
#define FILE_ATTRIBUTE_READONLY 0x00000001
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_ARCHIVE 0x00000020
#define FILE_ATTRIBUTE_DEVICE 0x00000040
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_TEMPORARY 0x00000100

typedef union _FILE_SEGMENT_ELEMENT {
    PVOID Buffer;
    DWORD Alignment;
} FILE_SEGMENT_ELEMENT, *PFILE_SEGMENT_ELEMENT;

#define IO_COMPLETION_MODIFY_STATE 0x0002
#define IO_COMPLETION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3)
#define DUPLICATE_CLOSE_SOURCE 0x00000001
#define DUPLICATE_SAME_ACCESS 0x00000002

// PE image fields are little-endian; the big-endian console swaps them on read.
#define IMAGE_READ_DWORD(Pointer) (_byteswap_ulong(*(Pointer)))
#define IMAGE_READ_WORD(Pointer) (_byteswap_ushort(*(Pointer)))

#include <string.h>

#ifndef _SLIST_HEADER_
#define _SLIST_HEADER_
#if defined(_WIN64)
typedef struct __declspec(align(16)) _SLIST_ENTRY {
    struct _SLIST_ENTRY *Next;
} SLIST_ENTRY, *PSLIST_ENTRY;

typedef struct __declspec(align(16)) _SLIST_HEADER {
    ULONGLONG Alignment;
    ULONGLONG Region;
} SLIST_HEADER, *PSLIST_HEADER;
#else
// 32-bit SLISTs: entries are plain single-list entries; the header packs next/depth/sequence into one 64-bit word.
#define SLIST_ENTRY SINGLE_LIST_ENTRY
#define _SLIST_ENTRY _SINGLE_LIST_ENTRY
#define PSLIST_ENTRY PSINGLE_LIST_ENTRY

typedef union _SLIST_HEADER {
    ULONGLONG Alignment;
    struct {
        SLIST_ENTRY Next;
        WORD Depth;
        WORD Sequence;
    };
} SLIST_HEADER, *PSLIST_HEADER;
#endif
#endif // _SLIST_HEADER_

NTSYSAPI SIZE_T NTAPI RtlCompareMemory(const VOID *Source1, const VOID *Source2, SIZE_T Length);

#define RtlEqualMemory(Destination, Source, Length) (!memcmp((Destination), (Source), (Length)))
#define RtlMoveMemory(Destination, Source, Length) memmove((Destination), (Source), (Length))
#define RtlCopyMemory(Destination, Source, Length) memcpy((Destination), (Source), (Length))
#define RtlFillMemory(Destination, Length, Fill) memset((Destination), (Fill), (Length))
#define RtlZeroMemory(Destination, Length) memset((Destination), 0, (Length))

#if !defined(MIDL_PASS)
// Byte-wise clear through a volatile pointer, so the stores cannot be optimised away.
FORCEINLINE PVOID RtlSecureZeroMemory(IN PVOID ptr, IN SIZE_T cnt)
{
    volatile char *p = (volatile char *)ptr;
    for (; cnt != 0; --cnt) {
        *p = 0;
        ++p;
    }
    return ptr;
}
#endif

typedef struct _RTL_CRITICAL_SECTION {
    union {
        DWORD RawEvent[4];
    } Synchronization;
    LONG LockCount;
    LONG RecursionCount;
    DWORD OwningThread;
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

NTSYSAPI VOID NTAPI RtlEnterCriticalSection(IN PRTL_CRITICAL_SECTION CriticalSection);
NTSYSAPI VOID NTAPI RtlLeaveCriticalSection(IN PRTL_CRITICAL_SECTION CriticalSection);
NTSYSAPI DWORD NTAPI RtlTryEnterCriticalSection(IN PRTL_CRITICAL_SECTION CriticalSection);
NTSYSAPI VOID NTAPI RtlInitializeCriticalSection(OUT PRTL_CRITICAL_SECTION CriticalSection);
NTSYSAPI VOID NTAPI RtlInitializeCriticalSectionAndSpinCount(OUT PRTL_CRITICAL_SECTION CriticalSection, IN DWORD SpinCount);
#define RtlDeleteCriticalSection(CriticalSection) ((void)0)

#define HEAP_NO_SERIALIZE 0x00000001
#define HEAP_GROWABLE 0x00000002
#define HEAP_GENERATE_EXCEPTIONS 0x00000004
#define HEAP_ZERO_MEMORY 0x00000008
#define HEAP_REALLOC_IN_PLACE_ONLY 0x00000010
#define HEAP_TAIL_CHECKING_ENABLED 0x00000020
#define HEAP_FREE_CHECKING_ENABLED 0x00000040
#define HEAP_DISABLE_COALESCE_ON_FREE 0x00000080
#define HEAP_CREATE_ALIGN_16 0x00010000
#define HEAP_CREATE_ENABLE_TRACING 0x00020000
#define HEAP_MAXIMUM_TAG 0x0FFF
#define HEAP_PSEUDO_TAG_FLAG 0x8000
#define HEAP_TAG_SHIFT 18
#define HEAP_MAKE_TAG_FLAGS(b, o) ((DWORD)((b) + ((o) << 18)))

#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3
#define DLL_PROCESS_DETACH 0

#ifdef __cplusplus
}
#endif

#endif // _WINNT_
