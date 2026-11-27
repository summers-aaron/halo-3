// port/sdk/runtime: Windows process entry (lld-link /entry:port_entry). msvcrt.dll initializes itself when it
// loads; this runs the C initializers (.CRT$XI*, which return non-zero on failure) and the C++ ones (.CRT$XC*, the
// game's static constructors) in section order, then the game's main(). lld-link merges .CRT$X?A..Z by suffix.
#include "../input/port_input.h"
#include "port_config.h"
#include "port_runtime.h"

typedef int(__cdecl *port_c_init)(void);
typedef void(__cdecl *port_cpp_init)(void);

__declspec(dllimport) int __cdecl _write(int fd, const void *buffer, unsigned int count);
__declspec(dllimport) void __cdecl exit(int code);
__declspec(dllimport) void __cdecl _exit(int code);

#pragma section(".CRT$XIA", read)
#pragma section(".CRT$XIZ", read)
#pragma section(".CRT$XCA", read)
#pragma section(".CRT$XCZ", read)
__declspec(allocate(".CRT$XIA")) static port_c_init port_xi_a = 0;
__declspec(allocate(".CRT$XIZ")) static port_c_init port_xi_z = 0;
__declspec(allocate(".CRT$XCA")) static port_cpp_init port_xc_a = 0;
__declspec(allocate(".CRT$XCZ")) static port_cpp_init port_xc_z = 0;

// Hint NVIDIA Optimus / AMD PowerXpress laptops to bind the discrete GPU rather than the iGPU.
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001u;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

int main(void);

static unsigned int port_length(const char *text)
{
    unsigned int length = 0;
    while (text[length] != 0)
        length++;
    return length;
}

void port_write_error(const char *text)
{
    _write(2, text, port_length(text));
}

// Every way out of the process but a return from main: no controller is left vibrating.
void port_exit(int code)
{
    port_input_hardware_shutdown();
    _exit(code);
}

__declspec(dllimport) char *__cdecl getenv(const char *name);
__declspec(dllimport) int __cdecl _itoa_s(int value, char *buffer, __SIZE_TYPE__ size, int radix);

static void port_write_number(int value)
{
    char text[16];
    _itoa_s(value, text, sizeof(text), 10);
    port_write_error(text);
}

void __cdecl port_entry(void)
{
    // PORT_TRACE_INIT=1 reports each static constructor before it runs. The settings (halo3.ini) set the HALO3_*
    // variables the launch left unset before any initializer or subsystem reads them.
    port_install_crash_handler();
    port_protect_poisoned_data();
    port_config_init();
    int trace = getenv("PORT_TRACE_INIT") != 0;
    for (port_c_init *init = &port_xi_a + 1; init < &port_xi_z; init++) {
        if (*init != 0 && (*init)() != 0)
            port_exit(255);
    }
    int count = (int)(&port_xc_z - (&port_xc_a + 1)), index = 0;
    for (port_cpp_init *init = &port_xc_a + 1; init < &port_xc_z; init++, index++) {
        if (trace) {
            port_write_error("port: static constructor ");
            port_write_number(index + 1);
            port_write_error(" of ");
            port_write_number(count);
            port_write_error("\n");
        }
        if (*init != 0)
            (*init)();
    }
    port_write_error("port: entering main()\n");
    int code = main();
    port_exit_flush();
    port_input_hardware_shutdown();
    port_run_atexit();
    exit(code);
}
