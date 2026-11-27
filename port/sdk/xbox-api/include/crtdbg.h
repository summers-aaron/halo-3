// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// crtdbg.h: debug-heap and report hooks. The game is a retail (NDEBUG) build: the assertion and report macros
// compile to nothing and the debug allocators are the ordinary ones.
#pragma once
#ifndef _INC_CRTDBG
#define _INC_CRTDBG
#include <crtdefs.h>

#ifndef _ASSERT
#define _ASSERT(expr) ((void)0)
#endif
#ifndef _ASSERTE
#define _ASSERTE(expr) ((void)0)
#endif
#ifndef _ASSERT_EXPR
#define _ASSERT_EXPR(expr, msg) ((void)0)
#endif
#define _RPT0(rptno, msg)
#define _RPT1(rptno, msg, arg1)
#define _RPT2(rptno, msg, arg1, arg2)
#define _RPTF0(rptno, msg)
#define _RPTF1(rptno, msg, arg1)
#define _RPTF2(rptno, msg, arg1, arg2)
#define _CrtSetReportMode(t, f) ((int)0)
#define _CrtSetDbgFlag(f) ((int)0)
#define _CrtCheckMemory() ((int)1)
#define _malloc_dbg(s, t, f, l) malloc(s)
#define _calloc_dbg(c, s, t, f, l) calloc(c, s)
#define _realloc_dbg(p, s, t, f, l) realloc(p, s)
#define _free_dbg(p, t) free(p)
#define _CRT_WARN 0
#define _CRT_ERROR 1
#define _CRT_ASSERT 2
#define _CRT_ERRCNT 3
#define _NORMAL_BLOCK 1
#define _CRT_BLOCK 2

#endif // _INC_CRTDBG
