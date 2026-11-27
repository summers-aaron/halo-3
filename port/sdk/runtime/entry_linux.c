// port/sdk/runtime (Linux host): process entry. glibc's start code runs the C and C++ initializers (.init_array,
// the game's static constructors among them) and then main; the link wraps main (ld --wrap=main) so this runs
// first, and the game's main() is __real_main. Static destructors are registered with glibc's atexit and run by
// exit().
#include "../input/port_input.h"
#include "port_config.h"
#include "port_runtime.h"

#include <stdlib.h>
#include <unistd.h>

int PORT_MS_ABI __real_main(void);

void PORT_MS_ABI port_write_error(const char *text)
{
    unsigned int length = 0;
    while (text[length] != 0)
        length++;
    while (length > 0) {
        ssize_t written = write(2, text, length);
        if (written <= 0)
            return;
        text += written;
        length -= (unsigned int)written;
    }
}

// Every way out of the process but a return from main: no controller is left vibrating.
void PORT_MS_ABI port_exit(int code)
{
    port_input_hardware_shutdown();
    _exit(code);
}

void PORT_MS_ABI port_run_atexit(void)
{
}

// Before the game's static constructors: the linker orders prioritized initializers (.init_array.101) ahead of the
// game's plain .init_array entries. The crash handler first, so poisoned data faults are reported by name; then the
// settings (halo3.ini), which set the HALO3_* variables the launch left unset before anything reads them.
__attribute__((constructor(101))) static void port_early_initialize(void)
{
    port_install_crash_handler();
    port_protect_poisoned_data();
    port_config_init();
}

int __wrap_main(int argc, char **argv, char **envp)
{
    (void)argc; (void)argv; (void)envp;
    port_write_error("port: entering main()\n");
    const int code = __real_main();
    port_exit_flush();
    port_input_hardware_shutdown();
    exit(code);
}
