// port/sdk/xam (both hosts): the console settings a title reads (XConfig): its language and its video mode.
//
// Language: the console's language is the host user's. The host names it by ISO 639 language and ISO 3166 region
// (port_host_language, locale_linux.c / locale_windows.c); it maps to the Xbox's language codes, and a language the
// Xbox does not have reads as English, the Xbox's own default.
//
// Video mode: 1280x720, progressive, widescreen, high definition, 60 Hz: the HD mode the game was made for. The port
// presents the game's frames in a host window of any size, so the game renders its own 1152x640 for this mode. With a
// custom resolution ([display] resolution = WxH, port_config_resolution) the display is that size instead, in square
// pixels: the game renders at the display's size (port/GAME-CHANGES.md, "Custom resolutions"), and its cameras take
// their aspect ratio from it (Hor+ on 21:9).
#include "xam_host.h"
#include "../runtime/port_config.h"

typedef unsigned int port_dword;

#define PORT_XC_LANGUAGE_ENGLISH 0x01ul
#define PORT_XC_LANGUAGE_JAPANESE 0x02ul
#define PORT_XC_LANGUAGE_GERMAN 0x03ul
#define PORT_XC_LANGUAGE_FRENCH 0x04ul
#define PORT_XC_LANGUAGE_SPANISH 0x05ul
#define PORT_XC_LANGUAGE_ITALIAN 0x06ul
#define PORT_XC_LANGUAGE_KOREAN 0x07ul
#define PORT_XC_LANGUAGE_TCHINESE 0x08ul
#define PORT_XC_LANGUAGE_PORTUGUESE 0x09ul
#define PORT_XC_LANGUAGE_SCHINESE 0x0aul
#define PORT_XC_LANGUAGE_POLISH 0x0bul
#define PORT_XC_LANGUAGE_RUSSIAN 0x0cul

#define PORT_XC_VIDEO_STANDARD_NTSC_M 1ul

static int port_same(const char *a, const char *b)
{
    return a[0] == b[0] && a[1] == b[1];
}

PORT_MS_ABI port_dword XTLGetLanguage(void)
{
    char language[3] = {0}, region[3] = {0};
    port_host_language(language, region);
    static const struct { char code[3]; port_dword xbox; } k_languages[] = {
        {"en", PORT_XC_LANGUAGE_ENGLISH}, {"ja", PORT_XC_LANGUAGE_JAPANESE}, {"de", PORT_XC_LANGUAGE_GERMAN},
        {"fr", PORT_XC_LANGUAGE_FRENCH}, {"es", PORT_XC_LANGUAGE_SPANISH}, {"it", PORT_XC_LANGUAGE_ITALIAN},
        {"ko", PORT_XC_LANGUAGE_KOREAN}, {"pt", PORT_XC_LANGUAGE_PORTUGUESE}, {"pl", PORT_XC_LANGUAGE_POLISH},
        {"ru", PORT_XC_LANGUAGE_RUSSIAN},
    };
    if (port_same(language, "zh")) {
        // Taiwan, Hong Kong and Macau write Traditional Chinese; elsewhere Simplified.
        int traditional = port_same(region, "TW") || port_same(region, "HK") || port_same(region, "MO");
        return traditional ? PORT_XC_LANGUAGE_TCHINESE : PORT_XC_LANGUAGE_SCHINESE;
    }
    for (unsigned int i = 0; i < sizeof(k_languages) / sizeof(k_languages[0]); i++) {
        if (port_same(language, k_languages[i].code))
            return k_languages[i].xbox;
    }
    return PORT_XC_LANGUAGE_ENGLISH;
}

// XVIDEO_MODE (xbox.h).
typedef struct port_video_mode {
    port_dword display_width, display_height;
    int interlaced, wide_screen, high_definition;
    float refresh_rate;
    port_dword video_standard;
    port_dword reserved[5];
} port_video_mode;

PORT_MS_ABI void XGetVideoMode(port_video_mode *mode)
{
    *mode = (port_video_mode){1280, 720, 0, 1, 1, 60.0f, PORT_XC_VIDEO_STANDARD_NTSC_M, {0}};
    unsigned int width, height;
    if (port_config_resolution(&width, &height)) {
        mode->display_width = width;
        mode->display_height = height;
    }
}

// Locale: the console's country, from the host user's region (ISO 3166, port_host_language). A region the Xbox does
// not list, or none, reads as the United States, the Xbox's own default.
PORT_MS_ABI port_dword XGetLocale(void)
{
    char language[3] = {0}, region[3] = {0};
    port_host_language(language, region);
    // XC_LOCALE_* (xbox.h), in their order: 1 AUSTRALIA ... 37 RUSSIAN_FEDERATION.
    static const char k_regions[][3] = {
        "AU", "AT", "BE", "BR", "CA", "CL", "CN", "CO", "CZ", "DK", "FI", "FR", "DE", "GR", "HK", "HU", "IN", "IE",
        "IT", "JP", "KR", "MX", "NL", "NZ", "NO", "PL", "PT", "SG", "SK", "ZA", "ES", "SE", "CH", "TW", "GB", "US",
        "RU",
    };
    for (unsigned int i = 0; i < sizeof(k_regions) / sizeof(k_regions[0]); i++) {
        if (port_same(region, k_regions[i]))
            return i + 1;
    }
    return 36; // XC_LOCALE_UNITED_STATES
}
