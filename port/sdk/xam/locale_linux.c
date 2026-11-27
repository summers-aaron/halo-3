// port/sdk/xam (Linux host): the user's language, from the POSIX locale variables in their order of precedence for
// messages (LC_ALL, LC_MESSAGES, LANG), whose values read `language[_REGION][.encoding][@modifier]`.
#include "xam_host.h"

#include <stdlib.h>

static int port_lower(char c) { return c >= 'a' && c <= 'z'; }
static int port_upper(char c) { return c >= 'A' && c <= 'Z'; }

void port_host_language(char language[3], char region[3])
{
    static const char *const k_variables[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
    language[0] = region[0] = 0;
    for (unsigned int i = 0; i < sizeof(k_variables) / sizeof(k_variables[0]); i++) {
        const char *value = getenv(k_variables[i]);
        if (value == 0 || value[0] == 0)
            continue;
        // The first one set decides, even when it is "C" or "POSIX" (no language).
        if (port_lower(value[0]) && port_lower(value[1]) && (value[2] == 0 || value[2] == '_' || value[2] == '.'
                || value[2] == '@')) {
            language[0] = value[0];
            language[1] = value[1];
            language[2] = 0;
            if (value[2] == '_' && port_upper(value[3]) && port_upper(value[4])) {
                region[0] = value[3];
                region[1] = value[4];
                region[2] = 0;
            }
        }
        return;
    }
}
