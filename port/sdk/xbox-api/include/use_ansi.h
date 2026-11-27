// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// use_ansi.h: names the C++ runtime library for the linker (static libcpmt unless a C++ runtime DLL is wanted).
#pragma once
#ifndef _USE_ANSI_CPP
#define _USE_ANSI_CPP
#undef _CRT_NOPRAGMA_LIBS
#if _XBOX_VER < 200 && defined(_DLL) && !defined(_STATIC_CPPLIB)
#ifdef _DEBUG
#pragma comment(lib, "msvcprtd")
#else
#pragma comment(lib, "msvcprt")
#endif
#include <crtassem.h>
#else
#ifdef _DEBUG
#pragma comment(lib, "libcpmtd")
#else
#pragma comment(lib, "libcpmt")
#endif
#endif
#endif // _USE_ANSI_CPP
