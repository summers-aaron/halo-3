// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xtl.h: master include for Xbox 360 titles (Win32 subset, Xbox APIs, Direct3D unless NOD3D, networking unless NONET).
#pragma once

#ifndef _XTL_
#define _XTL_

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _INC_XTL
#define _INC_XTL

#include <xdk.h>
#define _XTL_VER _XDK_VER

#ifndef _INC_WINDOWS
#define _INC_WINDOWS
#endif

#ifndef _PPC_
#define _PPC_
#endif

// Must precede every CRT header pulled in below: the CRT only marks its functions dllimport for _XBOX_VER < 200.
#ifndef _XBOX_VER
#define _XBOX_VER 200
#endif

#include <excpt.h>
#include <stdarg.h>

#include <windef.h>
#include <winbase.h>
#include <xbox.h>
#include <xam.h>
#include <pix.h>

#include <stdlib.h>

#ifndef NOD3D
#include <d3d9.h>
#include <d3dx9.h>
#endif

#ifndef NONET
#include <winsockx.h>
#include <xonline.h>
#endif

#endif // _INC_XTL
#endif // _XTL_
