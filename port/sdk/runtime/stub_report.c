// port/sdk/runtime: what a stage-1 stub (port/tools/stubgen) calls. Reports the symbol the game reached that the
// port does not implement yet, then stops the process. A poisoned datum (retail data the game does not define in
// source yet) is reported the same way, from the fault handler, on its first access.
//
// Diagnostic mode, PORT_STUB_CONTINUE=1: a stub the caller cleans up after (cdecl) reports its name once and
// returns 0, and a poisoned datum reports its name once and reads as zeros from then on, so one run lists every
// such gap on the way; other stubs still stop. Results after a skipped call or datum are not meaningful.
#include "port_runtime.h"

#ifdef _WIN32
__declspec(dllimport) char *__cdecl getenv(const char *name);
#else
char *getenv(const char *name);
#endif

#define PORT_REPORTED_CAPACITY 4096
static const char *port_reported[PORT_REPORTED_CAPACITY];
static int port_reported_count = 0;

static int port_continue_mode(void)
{
    return getenv("PORT_STUB_CONTINUE") != 0;
}

// 1 the first time a name is reported in the diagnostic mode.
static int port_first_report(const char *name)
{
    for (int i = 0; i < port_reported_count; i++) {
        if (port_reported[i] == name)
            return 0;
    }
    if (port_reported_count < PORT_REPORTED_CAPACITY)
        port_reported[port_reported_count++] = name;
    return 1;
}

void port_stub_called(const char *name, int can_return)
{
    if (can_return && port_continue_mode()) {
        if (port_first_report(name)) {
            port_write_error("port: skipped: ");
            port_write_error(name);
            port_write_error("\n");
        }
        return;
    }
    port_write_error("port: not implemented: ");
    port_write_error(name);
    port_write_error("\n");
    port_exit(3);
}

void port_protect_poisoned_data(void)
{
    for (__SIZE_TYPE__ i = 0; i < port_poison_count; i++)
        port_set_page_access(port_poison_table[i].address, (unsigned long)port_poison_table[i].size, 0);
}

int port_poisoned_access(const void *address)
{
    const unsigned char *where = (const unsigned char *)address;
    for (__SIZE_TYPE__ i = 0; i < port_poison_count; i++) {
        const port_poisoned_datum *datum = &port_poison_table[i];
        const unsigned char *start = (const unsigned char *)datum->address;
        if (where < start || where >= start + datum->size)
            continue;
        if (port_continue_mode()) {
            if (port_first_report(datum->name)) {
                port_write_error("port: skipped datum: ");
                port_write_error(datum->name);
                port_write_error("\n");
            }
            port_set_page_access(datum->address, (unsigned long)datum->size, 1);
            return 1;
        }
        port_write_error("port: not defined in source: ");
        port_write_error(datum->name);
        port_write_error(" (retail data)\n");
        port_exit(3);
    }
    return 0;
}
