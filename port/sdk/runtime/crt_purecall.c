// port/sdk/runtime (both hosts): pure virtual calls, as the MSVC runtime the game was built against (LIBCMT) handles
// them. A vtable slot of an abstract method points at _purecall. It calls the handler the game installed with
// _set_purecall_handler (the shell installs one that halts with a report), then ends the process; a handler that
// returns does not resume the call.
//
// One definition for both hosts: on Windows the game's own _purecall must not be msvcrt.dll's, which ignores the
// handler set here.
#include "port_runtime.h"

typedef void (PORT_MS_ABI *port_purecall_handler)(void);

static port_purecall_handler port_installed_purecall_handler;

port_purecall_handler PORT_MS_ABI _set_purecall_handler(port_purecall_handler handler)
{
    return __atomic_exchange_n(&port_installed_purecall_handler, handler, __ATOMIC_ACQ_REL);
}

int PORT_MS_ABI _purecall(void)
{
    port_purecall_handler handler = __atomic_load_n(&port_installed_purecall_handler, __ATOMIC_ACQUIRE);
    if (handler != 0)
        handler();
    port_write_error("port: pure virtual function call\n");
    port_exit(6);
    return 0;
}
