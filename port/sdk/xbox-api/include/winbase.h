// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// winbase.h: the Win32 base API subset of the Xbox 360 title runtime (memory, heaps, threads, sync, files, time).
#ifndef _WINBASE_
#define _WINBASE_

#ifndef WINBASEAPI
#define WINBASEAPI
#endif
#ifndef XWINBASEAPI
#define XWINBASEAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define GetCurrentTime() GetTickCount()
#define Yield()

#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)

#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2

#define TIME_ZONE_ID_INVALID ((DWORD)0xFFFFFFFF)

#define WAIT_FAILED ((DWORD)0xFFFFFFFF)
#define WAIT_OBJECT_0 ((STATUS_WAIT_0) + 0)
#define WAIT_ABANDONED ((STATUS_ABANDONED_WAIT_0) + 0)
#define WAIT_ABANDONED_0 ((STATUS_ABANDONED_WAIT_0) + 0)
#define WAIT_IO_COMPLETION STATUS_USER_APC
#define STILL_ACTIVE STATUS_PENDING

#define EXCEPTION_ACCESS_VIOLATION STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE STATUS_INVALID_HANDLE
#define CONTROL_C_EXIT STATUS_CONTROL_C_EXIT

// Memory helpers and the heap / critical-section entry points are the Rtl routines under their Win32 names.
#define MoveMemory RtlMoveMemory
#define CopyMemory RtlCopyMemory
#define FillMemory RtlFillMemory
#define ZeroMemory RtlZeroMemory
#define SecureZeroMemory RtlSecureZeroMemory
#define HeapAlloc RtlAllocateHeap
#define HeapReAlloc RtlReAllocateHeap
#define HeapFree RtlFreeHeap
#define HeapSize RtlSizeHeap
#define HeapValidate RtlValidateHeap
#define HeapCompact RtlCompactHeap
#define HeapLock RtlLockHeap
#define HeapUnlock RtlUnlockHeap
#define InitializeCriticalSection RtlInitializeCriticalSection
#define InitializeCriticalSectionAndSpinCount RtlInitializeCriticalSectionAndSpinCount
#define DeleteCriticalSection RtlDeleteCriticalSection
#define EnterCriticalSection RtlEnterCriticalSection
#define LeaveCriticalSection RtlLeaveCriticalSection
#define TryEnterCriticalSection RtlTryEnterCriticalSection

#define FILE_FLAG_WRITE_THROUGH 0x80000000
#define FILE_FLAG_OVERLAPPED 0x40000000
#define FILE_FLAG_NO_BUFFERING 0x20000000
#define FILE_FLAG_RANDOM_ACCESS 0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE 0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000

#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef LPVOID PSECURITY_ATTRIBUTES;
typedef LPVOID LPSECURITY_ATTRIBUTES;

// Big-endian order of the two halves (as LARGE_INTEGER).
typedef struct _FILETIME {
    DWORD dwHighDateTime;
    DWORD dwLowDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR cFileName[MAX_PATH];
    WCHAR cAlternateFileName[14];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
typedef VOID (WINAPI *PFIBER_START_ROUTINE)(LPVOID lpFiberParameter);
typedef PFIBER_START_ROUTINE LPFIBER_START_ROUTINE;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

#define MUTEX_MODIFY_STATE MUTANT_QUERY_STATE
#define MUTEX_ALL_ACCESS MUTANT_ALL_ACCESS

typedef struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

#define CREATE_SUSPENDED 0x00000004

#define THREAD_PRIORITY_LOWEST THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL (THREAD_PRIORITY_LOWEST + 1)
#define THREAD_PRIORITY_NORMAL 0
#define THREAD_PRIORITY_HIGHEST THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL (THREAD_PRIORITY_HIGHEST - 1)
#define THREAD_PRIORITY_ERROR_RETURN (MAXLONG)
#define THREAD_PRIORITY_TIME_CRITICAL THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE THREAD_BASE_PRIORITY_IDLE

#if !defined(MIDL_PASS)
typedef PCONTEXT LPCONTEXT;
typedef PEXCEPTION_RECORD LPEXCEPTION_RECORD;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;
#endif

#define IGNORE 0
#define INFINITE 0xFFFFFFFF

#ifndef _NTOS_
// The Interlocked API is the compiler's interlocked intrinsics: each name maps onto the underscore form.
#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedExchange _InterlockedExchange
#define InterlockedExchangeAdd _InterlockedExchangeAdd
#define InterlockedIncrement _InterlockedIncrement
#define InterlockedOr _InterlockedOr
#define InterlockedXor _InterlockedXor
#define InterlockedAnd _InterlockedAnd
#define InterlockedCompareExchange64 _InterlockedCompareExchange64
#define InterlockedDecrement64 _InterlockedDecrement64
#define InterlockedExchange64 _InterlockedExchange64
#define InterlockedExchangeAdd64 _InterlockedExchangeAdd64
#define InterlockedIncrement64 _InterlockedIncrement64
#define InterlockedOr64 _InterlockedOr64
#define InterlockedXor64 _InterlockedXor64
#define InterlockedAnd64 _InterlockedAnd64

XWINBASEAPI LONG WINAPI InterlockedIncrement(IN OUT LONG volatile *lpAddend);
XWINBASEAPI LONG WINAPI InterlockedDecrement(IN OUT LONG volatile *lpAddend);
XWINBASEAPI LONG WINAPI InterlockedExchange(IN OUT LONG volatile *Target, IN LONG Value);
#define InterlockedExchangePointer(Target, Value) (PVOID)InterlockedExchange((LONG volatile *)(Target), (LONG)(Value))
XWINBASEAPI LONG WINAPI InterlockedExchangeAdd(IN OUT LONG volatile *Addend, IN LONG Value);
XWINBASEAPI LONG WINAPI InterlockedCompareExchange(IN OUT LONG volatile *Destination, IN LONG Exchange, IN LONG Comperand);

#ifdef __cplusplus
__inline PVOID __cdecl __InlineInterlockedCompareExchangePointer(IN OUT PVOID volatile *Destination, IN PVOID ExChange,
                                                                 IN PVOID Comperand)
{
    LONG previous = InterlockedCompareExchange((LONG volatile *)Destination, (LONG)ExChange, (LONG)Comperand);
    return (PVOID)previous;
}
#define InterlockedCompareExchangePointer __InlineInterlockedCompareExchangePointer
#else
#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    (PVOID)InterlockedCompareExchange((LONG volatile *)(Destination), (LONG)(ExChange), (LONG)(Comperand))
#endif

XWINBASEAPI LONG WINAPI InterlockedOr(IN OUT LONG volatile *Target, IN LONG Set);
XWINBASEAPI LONG WINAPI InterlockedXor(IN OUT LONG volatile *Target, IN LONG Set);
XWINBASEAPI LONG WINAPI InterlockedAnd(IN OUT LONG volatile *Target, IN LONG Set);
XWINBASEAPI LONG64 WINAPI InterlockedIncrement64(IN OUT LONG64 volatile *lpAddend);
XWINBASEAPI LONG64 WINAPI InterlockedDecrement64(IN OUT LONG64 volatile *lpAddend);
XWINBASEAPI LONG64 WINAPI InterlockedExchange64(IN OUT LONG64 volatile *Target, IN LONG64 Value);
XWINBASEAPI LONG64 WINAPI InterlockedExchangeAdd64(IN OUT LONG64 volatile *Addend, IN LONG64 Value);
XWINBASEAPI LONG64 WINAPI InterlockedCompareExchange64(IN OUT LONG64 volatile *Destination, IN LONG64 Exchange,
                                                       IN LONG64 Comperand);
XWINBASEAPI LONG64 WINAPI InterlockedOr64(IN OUT LONG64 volatile *Target, IN LONG64 Set);
XWINBASEAPI LONG64 WINAPI InterlockedXor64(IN OUT LONG64 volatile *Target, IN LONG64 Set);
XWINBASEAPI LONG64 WINAPI InterlockedAnd64(IN OUT LONG64 volatile *Target, IN LONG64 Set);

#if defined(_XBOX)
// Emits one raw PowerPC instruction word (the port supplies it as an ordinary function).
void (__emit)(unsigned int opcode);
#pragma intrinsic(__emit)
#if defined(HALO3_64BIT)
#define AcquireLockBarrier() __atomic_signal_fence(__ATOMIC_SEQ_CST)
#define ReleaseLockBarrier() __atomic_signal_fence(__ATOMIC_SEQ_CST)
#else
#define AcquireLockBarrier() __emit(0x7C2004AC)
#define ReleaseLockBarrier() __emit(0x7C2004AC)
#endif
#else
#define AcquireLockBarrier()
#define ReleaseLockBarrier()
#endif

#if defined(_SLIST_HEADER_)
#define InitializeSListHead(ListHead) ((ListHead)->Alignment = 0)
#define QueryDepthSList(ListHead) ((USHORT)(ListHead)->Depth)
XWINBASEAPI PSLIST_ENTRY WINAPI InterlockedPopEntrySList(PSLIST_HEADER ListHead);
XWINBASEAPI PSLIST_ENTRY WINAPI InterlockedPushEntrySList(PSLIST_HEADER ListHead, PSLIST_ENTRY ListEntry);
XWINBASEAPI PSLIST_ENTRY WINAPI InterlockedFlushSList(PSLIST_HEADER ListHead);
#endif
#endif // _NTOS_

WINBASEAPI BOOL WINAPI FreeLibrary(IN OUT HMODULE hLibModule);
WINBASEAPI FARPROC WINAPI GetProcAddress(IN HMODULE hModule, IN LPCSTR lpProcName);
WINBASEAPI VOID WINAPI GlobalMemoryStatus(IN OUT LPMEMORYSTATUS lpBuffer);

WINBASEAPI LPVOID WINAPI VirtualAlloc(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flAllocationType, IN DWORD flProtect);
WINBASEAPI BOOL WINAPI VirtualFree(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD dwFreeType);
WINBASEAPI BOOL WINAPI VirtualProtect(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flNewProtect, OUT PDWORD lpflOldProtect);
WINBASEAPI DWORD WINAPI VirtualQuery(IN LPCVOID lpAddress, OUT PMEMORY_BASIC_INFORMATION lpBuffer, IN DWORD dwLength);

WINBASEAPI HANDLE WINAPI HeapCreate(IN DWORD flOptions, IN SIZE_T dwInitialSize, IN SIZE_T dwMaximumSize);
WINBASEAPI BOOL WINAPI HeapDestroy(IN OUT HANDLE hHeap);
#if !defined(_NTURTL_)
// (Declared under the Win32 names, which the macros above turn into the Rtl ones.)
XWINBASEAPI LPVOID WINAPI HeapAlloc(IN HANDLE hHeap, IN DWORD dwFlags, IN SIZE_T dwBytes);
XWINBASEAPI LPVOID WINAPI HeapReAlloc(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem, IN SIZE_T dwBytes);
XWINBASEAPI SIZE_T WINAPI HeapSize(IN HANDLE hHeap, IN DWORD dwFlags, IN LPCVOID lpMem);
XWINBASEAPI BOOL WINAPI HeapFree(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem);
XWINBASEAPI BOOL WINAPI HeapValidate(IN HANDLE hHeap, IN DWORD dwFlags, IN LPCVOID lpMem);
XWINBASEAPI SIZE_T WINAPI HeapCompact(IN HANDLE hHeap, IN DWORD dwFlags);
XWINBASEAPI BOOL WINAPI HeapLock(IN HANDLE hHeap);
XWINBASEAPI BOOL WINAPI HeapUnlock(IN HANDLE hHeap);
#endif
XWINBASEAPI HANDLE WINAPI GetProcessHeap(VOID);

#ifndef GetCurrentProcess
#define GetCurrentProcess() NtCurrentProcess()
#endif

WINBASEAPI VOID WINAPI RaiseException(IN DWORD dwExceptionCode, IN DWORD dwExceptionFlags, IN DWORD nNumberOfArguments,
                                      IN CONST ULONG_PTR *lpArguments);
WINBASEAPI LONG WINAPI UnhandledExceptionFilter(IN struct _EXCEPTION_POINTERS *ExceptionInfo);
typedef LONG (WINAPI *PTOP_LEVEL_EXCEPTION_FILTER)(struct _EXCEPTION_POINTERS *ExceptionInfo);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;
WINBASEAPI LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter(IN LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

WINBASEAPI LPVOID WINAPI GetCurrentFiber(VOID);
#define GetFiberData() (*(LPVOID *)(GetCurrentFiber()))
WINBASEAPI LPVOID WINAPI CreateFiber(IN DWORD dwStackSize, IN LPFIBER_START_ROUTINE lpStartAddress, IN LPVOID lpParameter);
WINBASEAPI VOID WINAPI DeleteFiber(IN LPVOID lpFiber);
WINBASEAPI LPVOID WINAPI ConvertThreadToFiber(IN LPVOID lpParameter);
WINBASEAPI BOOL WINAPI ConvertFiberToThread(VOID);
WINBASEAPI VOID WINAPI SwitchToFiber(IN LPVOID lpFiber);
WINBASEAPI BOOL WINAPI SwitchToThread(VOID);

WINBASEAPI HANDLE WINAPI CreateThread(IN LPSECURITY_ATTRIBUTES lpThreadAttributes, IN DWORD dwStackSize,
                                      IN LPTHREAD_START_ROUTINE lpStartAddress, IN LPVOID lpParameter,
                                      IN DWORD dwCreationFlags, OUT LPDWORD lpThreadId);
#ifndef GetCurrentThread
#define GetCurrentThread() NtCurrentThread()
#endif
WINBASEAPI DWORD WINAPI GetCurrentThreadId(VOID);
WINBASEAPI DWORD WINAPI GetCurrentProcessorNumber(VOID);
WINBASEAPI BOOL WINAPI SetThreadPriority(IN HANDLE hThread, IN int nPriority);
WINBASEAPI BOOL WINAPI SetThreadPriorityBoost(IN HANDLE hThread, IN BOOL bDisablePriorityBoost);
WINBASEAPI int WINAPI GetThreadPriority(IN HANDLE hThread);
WINBASEAPI DECLSPEC_NORETURN VOID WINAPI ExitThread(IN DWORD dwExitCode);
WINBASEAPI BOOL WINAPI GetExitCodeThread(IN HANDLE hThread, OUT LPDWORD lpExitCode);

XWINBASEAPI DWORD WINAPI GetLastError(VOID);
XWINBASEAPI VOID WINAPI SetLastError(IN DWORD dwErrCode);

#define HasOverlappedIoCompleted(lpOverlapped) (*((volatile ULONG_PTR *)(&(lpOverlapped)->Internal)) != STATUS_PENDING)
WINBASEAPI BOOL WINAPI GetOverlappedResult(IN HANDLE hFile, IN LPOVERLAPPED lpOverlapped, OUT LPDWORD lpNumberOfBytesTransferred,
                                           IN BOOL bWait);
WINBASEAPI HANDLE WINAPI CreateIoCompletionPort(IN HANDLE FileHandle, IN HANDLE ExistingCompletionPort,
                                                IN ULONG_PTR CompletionKey, IN DWORD NumberOfConcurrentThreads);
WINBASEAPI BOOL WINAPI GetQueuedCompletionStatus(IN HANDLE CompletionPort, OUT LPDWORD lpNumberOfBytesTransferred,
                                                 OUT PULONG_PTR lpCompletionKey, OUT LPOVERLAPPED *lpOverlapped,
                                                 IN DWORD dwMilliseconds);
WINBASEAPI BOOL WINAPI PostQueuedCompletionStatus(IN HANDLE CompletionPort, IN DWORD dwNumberOfBytesTransferred,
                                                  IN ULONG_PTR dwCompletionKey, IN LPOVERLAPPED lpOverlapped);

WINBASEAPI DWORD WINAPI SuspendThread(IN HANDLE hThread);
WINBASEAPI DWORD WINAPI ResumeThread(IN HANDLE hThread);
WINBASEAPI VOID WINAPI DebugBreak(VOID);

WINBASEAPI BOOL WINAPI SetEvent(IN HANDLE hEvent);
WINBASEAPI BOOL WINAPI ResetEvent(IN HANDLE hEvent);
WINBASEAPI BOOL WINAPI PulseEvent(IN HANDLE hEvent);
WINBASEAPI BOOL WINAPI ReleaseSemaphore(IN HANDLE hSemaphore, IN LONG lReleaseCount, OUT LPLONG lpPreviousCount);
WINBASEAPI BOOL WINAPI ReleaseMutex(IN HANDLE hMutex);
WINBASEAPI DWORD WINAPI WaitForSingleObject(IN HANDLE hHandle, IN DWORD dwMilliseconds);
WINBASEAPI DWORD WINAPI WaitForMultipleObjects(IN DWORD nCount, IN CONST HANDLE *lpHandles, IN BOOL bWaitAll,
                                               IN DWORD dwMilliseconds);
WINBASEAPI VOID WINAPI Sleep(IN DWORD dwMilliseconds);

WINBASEAPI DWORD WINAPI GetFileSize(IN HANDLE hFile, OUT LPDWORD lpFileSizeHigh);
WINBASEAPI BOOL WINAPI GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize);
WINBASEAPI BOOL WINAPI WriteFile(IN HANDLE hFile, IN LPCVOID lpBuffer, IN DWORD nNumberOfBytesToWrite,
                                 OUT LPDWORD lpNumberOfBytesWritten, IN LPOVERLAPPED lpOverlapped);
WINBASEAPI BOOL WINAPI ReadFile(IN HANDLE hFile, OUT LPVOID lpBuffer, IN DWORD nNumberOfBytesToRead,
                                OUT LPDWORD lpNumberOfBytesRead, IN LPOVERLAPPED lpOverlapped);
WINBASEAPI BOOL WINAPI FlushFileBuffers(IN HANDLE hFile);
WINBASEAPI BOOL WINAPI SetEndOfFile(IN HANDLE hFile);
WINBASEAPI DWORD WINAPI SetFilePointer(IN HANDLE hFile, IN LONG lDistanceToMove, IN PLONG lpDistanceToMoveHigh,
                                       IN DWORD dwMoveMethod);
WINBASEAPI BOOL WINAPI SetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer,
                                        DWORD dwMoveMethod);
#define FindClose(hFindFile) CloseHandle(hFindFile)
WINBASEAPI BOOL WINAPI GetFileTime(IN HANDLE hFile, OUT LPFILETIME lpCreationTime, OUT LPFILETIME lpLastAccessTime,
                                   OUT LPFILETIME lpLastWriteTime);
WINBASEAPI BOOL WINAPI SetFileTime(IN HANDLE hFile, IN CONST FILETIME *lpCreationTime, IN CONST FILETIME *lpLastAccessTime,
                                   IN CONST FILETIME *lpLastWriteTime);
WINBASEAPI BOOL WINAPI CloseHandle(IN OUT HANDLE hObject);

WINBASEAPI VOID WINAPI GetSystemTime(OUT LPSYSTEMTIME lpSystemTime);
WINBASEAPI VOID WINAPI GetSystemTimeAsFileTime(OUT LPFILETIME lpSystemTimeAsFileTime);
WINBASEAPI VOID WINAPI GetLocalTime(OUT LPSYSTEMTIME lpSystemTime);
WINBASEAPI BOOL WINAPI SystemTimeToFileTime(IN CONST SYSTEMTIME *lpSystemTime, OUT LPFILETIME lpFileTime);
WINBASEAPI BOOL WINAPI FileTimeToLocalFileTime(IN CONST FILETIME *lpFileTime, OUT LPFILETIME lpLocalFileTime);
WINBASEAPI BOOL WINAPI LocalFileTimeToFileTime(IN CONST FILETIME *lpLocalFileTime, OUT LPFILETIME lpFileTime);
WINBASEAPI BOOL WINAPI FileTimeToSystemTime(IN CONST FILETIME *lpFileTime, OUT LPSYSTEMTIME lpSystemTime);
WINBASEAPI LONG WINAPI CompareFileTime(IN CONST FILETIME *lpFileTime1, IN CONST FILETIME *lpFileTime2);
WINBASEAPI DWORD WINAPI GetTickCount(VOID);

XWINBASEAPI DWORD WINAPI TlsAlloc(VOID);
#define TLS_OUT_OF_INDEXES (DWORD)0xFFFFFFFF
XWINBASEAPI LPVOID WINAPI TlsGetValue(IN DWORD dwTlsIndex);
XWINBASEAPI BOOL WINAPI TlsSetValue(IN DWORD dwTlsIndex, IN LPVOID lpTlsValue);
XWINBASEAPI BOOL WINAPI TlsFree(IN DWORD dwTlsIndex);

typedef VOID (WINAPI *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered,
                                                       LPOVERLAPPED lpOverlapped);
WINBASEAPI DWORD WINAPI SleepEx(IN DWORD dwMilliseconds, IN BOOL bAlertable);
WINBASEAPI DWORD WINAPI WaitForSingleObjectEx(IN HANDLE hHandle, IN DWORD dwMilliseconds, IN BOOL bAlertable);
WINBASEAPI DWORD WINAPI WaitForMultipleObjectsEx(IN DWORD nCount, IN CONST HANDLE *lpHandles, IN BOOL bWaitAll,
                                                 IN DWORD dwMilliseconds, IN BOOL bAlertable);
WINBASEAPI DWORD WINAPI SignalObjectAndWait(IN HANDLE hObjectToSignal, IN HANDLE hObjectToWaitOn, IN DWORD dwMilliseconds,
                                            IN BOOL bAlertable);
WINBASEAPI BOOL WINAPI ReadFileEx(IN HANDLE hFile, OUT LPVOID lpBuffer, IN DWORD nNumberOfBytesToRead,
                                  IN LPOVERLAPPED lpOverlapped, IN LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
WINBASEAPI BOOL WINAPI WriteFileEx(IN HANDLE hFile, IN LPCVOID lpBuffer, IN DWORD nNumberOfBytesToWrite,
                                   IN LPOVERLAPPED lpOverlapped, IN LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[MAX_PATH];
    CHAR cAlternateFileName[14];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
typedef WIN32_FIND_DATAA WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAA PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAA LPWIN32_FIND_DATA;

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

// Only the ANSI ("A") forms exist; the neutral names map onto them unconditionally.
WINBASEAPI HANDLE WINAPI CreateMutexA(IN LPSECURITY_ATTRIBUTES lpMutexAttributes, IN BOOL bInitialOwner, IN LPCSTR lpName);
#define CreateMutex CreateMutexA
WINBASEAPI HANDLE WINAPI OpenMutexA(IN DWORD dwDesiredAccess, IN BOOL bInheritHandle, IN LPCSTR lpName);
#define OpenMutex OpenMutexA
WINBASEAPI HANDLE WINAPI CreateEventA(IN LPSECURITY_ATTRIBUTES lpEventAttributes, IN BOOL bManualReset, IN BOOL bInitialState,
                                      IN LPCSTR lpName);
#define CreateEvent CreateEventA
WINBASEAPI HANDLE WINAPI OpenEventA(IN DWORD dwDesiredAccess, IN BOOL bInheritHandle, IN LPCSTR lpName);
#define OpenEvent OpenEventA
WINBASEAPI HANDLE WINAPI CreateSemaphoreA(IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, IN LONG lInitialCount,
                                          IN LONG lMaximumCount, IN LPCSTR lpName);
#define CreateSemaphore CreateSemaphoreA
WINBASEAPI HANDLE WINAPI OpenSemaphoreA(IN DWORD dwDesiredAccess, IN BOOL bInheritHandle, IN LPCSTR lpName);
#define OpenSemaphore OpenSemaphoreA

WINBASEAPI HMODULE WINAPI LoadLibraryA(IN LPCSTR lpLibFileName);
#define LoadLibrary LoadLibraryA
WINBASEAPI HMODULE WINAPI GetModuleHandleA(IN LPCSTR lpModuleName);
#define GetModuleHandle GetModuleHandleA

XWINBASEAPI VOID WINAPI OutputDebugStringA(IN LPCSTR lpOutputString);
XWINBASEAPI VOID WINAPI OutputDebugStringW(IN LPCWSTR lpOutputString);
#ifdef UNICODE
#define OutputDebugString OutputDebugStringW
#else
#define OutputDebugString OutputDebugStringA
#endif

WINBASEAPI BOOL WINAPI GetDiskFreeSpaceExA(IN LPCSTR lpDirectoryName, OUT PULARGE_INTEGER lpFreeBytesAvailableToCaller,
                                           OUT PULARGE_INTEGER lpTotalNumberOfBytes,
                                           OUT PULARGE_INTEGER lpTotalNumberOfFreeBytes);
#define GetDiskFreeSpaceEx GetDiskFreeSpaceExA
WINBASEAPI BOOL WINAPI CreateDirectoryA(IN LPCSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
#define CreateDirectory CreateDirectoryA
WINBASEAPI BOOL WINAPI RemoveDirectoryA(IN LPCSTR lpPathName);
#define RemoveDirectory RemoveDirectoryA
WINBASEAPI HANDLE WINAPI CreateFileA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode,
                                     IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition,
                                     IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile);
#define CreateFile CreateFileA
WINBASEAPI BOOL WINAPI SetFileAttributesA(IN LPCSTR lpFileName, IN DWORD dwFileAttributes);
#define SetFileAttributes SetFileAttributesA
WINBASEAPI DWORD WINAPI GetFileAttributesA(IN LPCSTR lpFileName);
#define GetFileAttributes GetFileAttributesA

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

WINBASEAPI BOOL WINAPI GetFileAttributesExA(IN LPCSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                            OUT LPVOID lpFileInformation);
#define GetFileAttributesEx GetFileAttributesExA
WINBASEAPI BOOL WINAPI DeleteFileA(IN LPCSTR lpFileName);
#define DeleteFile DeleteFileA
WINBASEAPI HANDLE WINAPI FindFirstFileA(IN LPCSTR lpFileName, OUT LPWIN32_FIND_DATA lpFindFileData);
#define FindFirstFile FindFirstFileA
WINBASEAPI BOOL WINAPI FindNextFileA(IN HANDLE hFindFile, OUT LPWIN32_FIND_DATAA lpFindFileData);
#define FindNextFile FindNextFileA
WINBASEAPI BOOL WINAPI CopyFileA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN BOOL bFailIfExists);
#define CopyFile CopyFileA
WINBASEAPI BOOL WINAPI MoveFileA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName);
#define MoveFile MoveFileA
WINBASEAPI BOOL WINAPI MoveFileExA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN DWORD dwFlags);
#define MoveFileEx MoveFileExA
#define MOVEFILE_REPLACE_EXISTING 0x00000001
#define MOVEFILE_COPY_ALLOWED 0x00000002
#define MOVEFILE_WRITE_THROUGH 0x00000008
WINBASEAPI BOOL WINAPI GetVolumeInformationA(IN LPCSTR lpRootPathName, OUT LPSTR lpVolumeNameBuffer, IN DWORD nVolumeNameSize,
                                             OUT LPDWORD lpVolumeSerialNumber, OUT LPDWORD lpMaximumComponentLength,
                                             OUT LPDWORD lpFileSystemFlags, OUT LPSTR lpFileSystemNameBuffer,
                                             IN DWORD nFileSystemNameSize);
#define GetVolumeInformation GetVolumeInformationA
WINBASEAPI BOOL WINAPI CancelIo(IN HANDLE hFile);

XWINBASEAPI BOOL WINAPI QueryPerformanceCounter(OUT LARGE_INTEGER *lpPerformanceCount);
XWINBASEAPI BOOL WINAPI QueryPerformanceFrequency(OUT LARGE_INTEGER *lpFrequency);

#include <winerror.h>

typedef enum _FILE_INFO_BY_HANDLE_CLASS {
    FileBasicInfo,
    FileStandardInfo,
    FileNameInfo,
    FileRenameInfo,
    FileDispositionInfo,
    FileAllocationInfo,
    FileEndOfFileInfo,
    FileStreamInfo,
    FileCompressionInfo,
    FileAttributeTagInfo,
    FileIdBothDirectoryInfo,
    FileIdBothDirectoryRestartInfo,
    FileIoPriorityHintInfo,
    MaximumFileInfoByHandleClass
} FILE_INFO_BY_HANDLE_CLASS, *PFILE_INFO_BY_HANDLE_CLASS;

typedef struct _FILE_ALLOCATION_INFO {
    LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFO, *PFILE_ALLOCATION_INFO;

typedef struct _FILE_END_OF_FILE_INFO {
    LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFO, *PFILE_END_OF_FILE_INFO;

typedef struct _FILE_DISPOSITION_INFO {
    BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFO, *PFILE_DISPOSITION_INFO;

XWINBASEAPI BOOL WINAPI SetFileInformationByHandle(IN HANDLE hFile, IN FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
                                                   IN LPVOID lpFileInformation, IN DWORD dwBufferSize);

WINBASEAPI int WINAPI wvsprintfA(OUT LPSTR, IN LPCSTR, IN va_list arglist);
WINBASEAPI int WINAPIV wsprintfA(OUT LPSTR, IN LPCSTR, ...);
#define wvsprintf wvsprintfA
#define wsprintf wsprintfA

#define CP_ACP 0
#define CP_UTF8 65001
WINBASEAPI int WINAPI MultiByteToWideChar(IN UINT CodePage, IN DWORD dwFlags, IN LPCSTR lpMultiByteStr, IN int cbMultiByte,
                                          OUT LPWSTR lpWideCharStr, IN int cchWideChar);
WINBASEAPI int WINAPI WideCharToMultiByte(IN UINT CodePage, IN DWORD dwFlags, IN LPCWSTR lpWideCharStr, IN int cchWideChar,
                                          OUT LPSTR lpMultiByteStr, IN int cbMultiByte, IN LPCSTR lpDefaultChar,
                                          OUT LPBOOL lpUsedDefaultChar);

#ifdef __cplusplus
}
#endif

#endif // _WINBASE_
