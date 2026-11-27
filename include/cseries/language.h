#ifndef CSERIES_LANGUAGE_H
#define CSERIES_LANGUAGE_H

#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"

// The language the game runs in: -1 until get_current_language first asks the system. March keeps it as a
// cached value that get_current_language fills on first use.
extern "C" e_language g_current_language;

// Cached Xbox locale, -1 until the language selection code first requests it.
extern "C" long g_current_locale;

static __forceinline e_language get_current_language()
{
    e_language language = g_current_language;
    if (language == -1)
    {
        language = get_e_language_from_xbox_language(XTLGetLanguage());
        g_current_language = language;
    }
    return language;
}

#endif // CSERIES_LANGUAGE_H
