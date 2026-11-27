// port/sdk/runtime (both hosts): std::_Throw, the C++ library's exception hook (the Xbox CRT's, built with
// _HAS_EXCEPTIONS 0 like the game). Library code that would throw (an out-of-range string position, a failed
// allocation in a container) calls it with the exception; with exceptions compiled out nothing can catch it, and the
// CRT reports it and ends the process. Built like the game, against the XDK's <exception>, for the C++ name.
#include <exception>
#include <stdexcept>
#include <xstring>

#include "port_runtime.h"

namespace std {

void __cdecl _Throw(const exception &error)
{
    port_write_error("port: C++ library exception (std::_Throw): ");
    const char *what = error.what();
    port_write_error(what != 0 ? what : "(no message)");
    port_write_error("\n");
    port_exit(3);
}

// The string error reporters (<xstring>; the Xbox CRT's xstring.obj, 0x58dac8 / 0x58dbc0): throw length_error
// "string too long" / out_of_range "invalid string position", which _Throw reports.
void __cdecl _String_base::_Xlen()
{
    _Throw(length_error("string too long"));
}

void __cdecl _String_base::_Xran()
{
    _Throw(out_of_range("invalid string position"));
}

} // namespace std
