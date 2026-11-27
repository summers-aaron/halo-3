// Port shim for structured exception handling on targets without an SEH runtime (Linux). Force-included there.
// Five units use __try: the guarded block runs and the handler never does, which is what happens on the Xbox
// whenever nothing faults. A fault crashes instead of being caught; port/sdk/kernel can add signal-based recovery.
#ifndef PORT_SEH_H
#define PORT_SEH_H
#define __try if (1)
#define __except(filter) else if (0)
#define __finally
#define __leave
#define GetExceptionCode() 0
#define GetExceptionInformation() ((void *)0)
#endif
