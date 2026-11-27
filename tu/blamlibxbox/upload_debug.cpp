// blamlibxbox/upload_debug.obj: from the records 0x219c38 0x219cc0
// @compiler xdk6130
// @flags /TP /O1
#include "memory/static_arrays.h"
#include <stdio.h>
// @address 0x219cc0 ?print@?$c_static_string@$0BO@@@QAAPBDPBDZZ
// @address 0x82370 _vsnprintf

#include <stdarg.h>
#include "main/main.h"
#include "main/main_debug_upload.h"
#include "cseries/upload_debug.h"
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"



template<long k_maximum_count> const char *c_static_string<k_maximum_count>::print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnprintf(m_string, k_maximum_count - 1, format, args);
    m_string[k_maximum_count - 1] = 0;
    return m_string;
}

// @address 0x8a1f74 g_suppress_upload_debug
bool g_suppress_upload_debug;


// @address 0x219c38 upload_debug_report_progress
extern "C" void upload_debug_report_progress(float value)
{
    c_static_string<30> string;
    string.print("\r\nwriting report... %.2f", value);
    char message[0x6000];
    message[0] = 0;
    csstrnzcat(message, string.get_string(), sizeof(message));
    _time64(0);
    csstrnzcat(message, "\r\n\r\nOops, something bad happened!  Visit http://www.bungie.net for more information.  (Please don't reboot)", sizeof(message));
    main_crash_render_message(message);
}
