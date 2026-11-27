// port/sdk/xam (Windows host): the user's language, from the user interface language Windows displays (its ISO 639
// language and ISO 3166 region names).
#include "xam_host.h"
#include "../kernel/win32_host.h"

typedef unsigned int port_dword;

#define PORT_LOCALE_SISO639LANGNAME 0x59ul
#define PORT_LOCALE_SISO3166CTRYNAME 0x5aul

PORT_HOST_IMPORT(unsigned short, GetUserDefaultUILanguage, 0, (void));
PORT_HOST_IMPORT(int, GetLocaleInfoA, 16, (port_dword locale, port_dword type, char *data, int size));

static void port_locale_code(port_dword locale, port_dword type, char code[3])
{
    char text[9];
    code[0] = 0;
    if (host_GetLocaleInfoA(locale, type, text, sizeof(text)) == 3) { // two letters and the terminator
        code[0] = text[0];
        code[1] = text[1];
        code[2] = 0;
    }
}

void port_host_language(char language[3], char region[3])
{
    port_dword locale = host_GetUserDefaultUILanguage(); // a language id is a locale id with the default sort
    port_locale_code(locale, PORT_LOCALE_SISO639LANGNAME, language);
    port_locale_code(locale, PORT_LOCALE_SISO3166CTRYNAME, region);
}
