// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// excpt.h: structured exception handling dispositions and filter results.
#pragma once
#ifndef _INC_EXCPT
#define _INC_EXCPT
#include <crtdefs.h>

#pragma pack(push, _CRT_PACKING)
#ifdef __cplusplus
extern "C" {
#endif

typedef enum _EXCEPTION_DISPOSITION
{
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind
} EXCEPTION_DISPOSITION;

struct _EXCEPTION_RECORD;
struct _CONTEXT;
struct _DISPATCHER_CONTEXT;
typedef struct _EXCEPTION_POINTERS *Exception_info_ptr;

// (These replace port_seh.h's Linux placeholders of the same names, as the runtime header always has.)
#define GetExceptionCode _exception_code
#define exception_code _exception_code
#define GetExceptionInformation (struct _EXCEPTION_POINTERS *)_exception_info
#define exception_info (struct _EXCEPTION_POINTERS *)_exception_info
#define AbnormalTermination _abnormal_termination
#define abnormal_termination _abnormal_termination
unsigned long __cdecl _exception_code(void);
void *__cdecl _exception_info(void);
int __cdecl _abnormal_termination(void);

#define EXCEPTION_EXECUTE_HANDLER 1
#define EXCEPTION_CONTINUE_SEARCH 0
#define EXCEPTION_CONTINUE_EXECUTION -1

#ifdef __cplusplus
}
#endif
#pragma pack(pop)
#endif // _INC_EXCPT
