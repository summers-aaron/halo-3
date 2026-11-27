// blamlibxbox/cseries.obj: from the records 0x820d0 0x822a8 0x1858b8 0x1989e0 0x198c98 0x19a388 0x19a4a0 0x19a540 0x19a608 0x19a6c0 0x19a738 0x19a7c0 0x19a800 0x1bec80 0x1bed70 0x1bedb8 0x1beed0 0x1bf0a8 0x1bf5e8
// @flags /TP /O2 /Oc
// @address 0x820d0 ?csstrtok@@YAPADPADPBD_NPAUcsstrtok_data@@@Z
// @address 0x19a540 ?csstricmp@@YAHPBD0@Z
// @address 0x19a608 ?csstrnicmp@@YAHPBD0I@Z
// @address 0x19a738 ?csnzappendf@@YAPADPADIPBDZZ
// @address 0x1bec80 ?get_language_suffix@@YAPBDW4e_language@@_N@Z
// @address 0x1bedb8 ?get_language_iso_639_1_name@@YAPBDW4e_language@@@Z
// @address 0x1bf5e8 ?get_e_language_from_xbox_language@@YA?AW4e_language@@K@Z
#include <xtl.h>
#include "game/game_globals.h"
#include "cseries/cseries.h"
#include "cseries/pointer_utilities.h"
#include <stddef.h>
#include "cseries/crt_xdk.h"
#include "scenario/scenario.h"
#include "cache/cache_files.h"
#include "cache/cache_file_allocation_range_types.h"
#include "multithreading/threads.h"
#include "multithreading/synchronization.h"
#include "memory/normal_allocation.h"
#include "tag_files/tag_groups.h"
#include "multithreading/synchronization_types.h"
#include "cache/cache_file_globals.h"
#include "cseries/language.h"
#include "scenario/scenario_tag.h"
#include "multithreading/synchronized_value.h"
#include "multithreading/thread_system_globals.h"

#include "cseries/locale.h"
#include "math/real_math_types.h"
#include "units/biped_globals.h"

// @address 0x74b540 g_current_language
e_language g_current_language = static_cast<e_language>(-1);

// @address 0x74b544 g_current_locale
long g_current_locale = -1;

// @address 0x265bc k_biped_aiming_vector_minimum_magnitude
// @rdata 0x820265bc..0x820265c0
const float k_biped_aiming_vector_minimum_magnitude = 0.9f;
// @rdata-end

// @address 0x3acd0 k_argb_green
// @rdata 0x8203acd0..0x8203ace0
static const real_argb_color k_argb_green = { 1.0f, 0.0f, 1.0f, 0.0f };
// @rdata-end

// @address 0x3ace0 k_argb_blue
// @rdata 0x8203ace0..0x8203acf0
static const real_argb_color k_argb_blue = { 1.0f, 0.0f, 0.0f, 1.0f };
// @rdata-end

// @address 0x3ad10 k_argb_magenta
// @rdata 0x8203ad10..0x8203ad20
static const real_argb_color k_argb_magenta = { 1.0f, 1.0f, 0.0f, 1.0f };
// @rdata-end

// @address 0x3ad20 k_argb_pink
// @rdata 0x8203ad20..0x8203ad30
static const real_argb_color k_argb_pink = { 1.0f, 1.0f, 0.41f, 0.7f };
// @rdata-end

// @address 0x3ad40 k_argb_orange
// @rdata 0x8203ad40..0x8203ad50
static const real_argb_color k_argb_orange = { 1.0f, 1.0f, 0.5f, 0.0f };
// @rdata-end

// @address 0x74b754 ?global_real_argb_green@@3PBTreal_argb_color@@B
const real_argb_color *global_real_argb_green = &k_argb_green;
// @address 0x74b758 ?global_real_argb_blue@@3PBTreal_argb_color@@B
const real_argb_color *global_real_argb_blue = &k_argb_blue;
// @address 0x74b764 ?global_real_argb_magenta@@3PBTreal_argb_color@@B
const real_argb_color *global_real_argb_magenta = &k_argb_magenta;
// @address 0x74b768 ?global_real_argb_pink@@3PBTreal_argb_color@@B
const real_argb_color *global_real_argb_pink = &k_argb_pink;
// @address 0x74b770 ?global_real_argb_orange@@3PBTreal_argb_color@@B
const real_argb_color *global_real_argb_orange = &k_argb_orange;

// @address 0x737703 g_catch_exceptions
extern "C" {
bool g_catch_exceptions = true;
}

static unsigned char ascii_isupper(int c)
{
    return c >= 'A' && c <= 'Z';
}
static int ascii_tolower(int c)
{
    if (ascii_isupper(c)) return c + 32;
    return c;
}

char *csstrnlwr(char *string, unsigned int size)
{
    unsigned int remaining = 256;
    unsigned int *sizeAddress = &size;
    char *character = string;
    while (*character != 0 && remaining-- > 0) {
        char lowercase = *character;
        if (*character >= 'A' && *character <= 'Z')
            lowercase = lowercase - 'A' + 'a';
        *character = lowercase;
        character++;
    }
    return string;
}

/* csnzprintf: formats into a sized buffer and always terminates it. */
char *csnzprintf(char *buffer, unsigned int size, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  _vsnprintf(buffer, size - 1, format, args);
  buffer[size - 1] = 0;
  return buffer;
}

// @address 0x1989e0 ??2@YAPAXI@Z
void *operator new(size_t size)
{
    return RtlAllocateHeap(GetProcessHeap(), 0, size);
}



// @address 0x19a388 ?cseries_dispose@@YAXXZ
void cseries_dispose(void)
{
    for (long thread_index = 2; thread_index < 9; ++thread_index) {
        if (g_thread_globals.handles[thread_index] != (void *)-1) {
            g_thread_should_exit[thread_index].set(1);
            wait_for_single_object_internal(g_thread_globals.handles[thread_index], 1000);
            CloseHandle(g_thread_globals.handles[thread_index]);
            g_thread_globals.handles[thread_index] = (void *)-1;
            g_thread_globals.thread_ids[thread_index] = (unsigned long)-1;
        }
    }
    g_thread_system_initialized.set(0);

    g_synch_globals.initialized = false;
    for (long mutex_index = 0; mutex_index < 3; ++mutex_index) {
        CloseHandle(g_synch_globals.mutexes[mutex_index].handle);
    }
    for (long event_index = 0; event_index < 7; ++event_index) {
        CloseHandle(g_synch_globals.events[event_index].handle);
    }
    for (long semaphore_index = 0; semaphore_index < 5; ++semaphore_index) {
        CloseHandle(g_synch_globals.semaphores[semaphore_index].handle);
    }
}

int ascii_stricmp(const char *first, const char *second)
{
    int index;
    for (index = 0;; index++) {
        int firstCharacter = ascii_tolower(first[index]);
        int secondCharacter = ascii_tolower(second[index]);
        if (firstCharacter == 0) return secondCharacter != 0 ? -1 : 0;
        if (secondCharacter == 0) return firstCharacter != 0;
        if (firstCharacter != secondCharacter) {
            if (firstCharacter > secondCharacter) return 1;
            return -1;
        }
    }
}

e_locale get_e_locale_from_xbox_locale(long xbox_locale)
{
    e_locale result = _locale_default;
    switch (xbox_locale) {
    case XC_LOCALE_AUSTRALIA: result = _locale_australia; break;
    case XC_LOCALE_AUSTRIA: result = _locale_austria; break;
    case XC_LOCALE_BELGIUM: result = _locale_belgium; break;
    case XC_LOCALE_BRAZIL: result = _locale_brazil; break;
    case XC_LOCALE_CANADA: result = _locale_canada; break;
    case XC_LOCALE_CHILE: result = _locale_chile; break;
    case XC_LOCALE_CHINA: result = _locale_china; break;
    case XC_LOCALE_COLOMBIA: result = _locale_colombia; break;
    case XC_LOCALE_CZECH_REPUBLIC: result = _locale_czech_republic; break;
    case XC_LOCALE_DENMARK: result = _locale_denmark; break;
    case XC_LOCALE_FINLAND: result = _locale_finland; break;
    case XC_LOCALE_FRANCE: result = _locale_france; break;
    case XC_LOCALE_GERMANY: result = _locale_germany; break;
    case XC_LOCALE_GREECE: result = _locale_greece; break;
    case XC_LOCALE_HONG_KONG: result = _locale_hong_kong; break;
    case XC_LOCALE_HUNGARY: result = _locale_hungary; break;
    case XC_LOCALE_INDIA: result = _locale_india; break;
    case XC_LOCALE_IRELAND: result = _locale_ireland; break;
    case XC_LOCALE_ITALY: result = _locale_italy; break;
    case XC_LOCALE_JAPAN: result = _locale_japan; break;
    case XC_LOCALE_KOREA: result = _locale_korea; break;
    case XC_LOCALE_MEXICO: result = _locale_mexico; break;
    case XC_LOCALE_NETHERLANDS: result = _locale_netherlands; break;
    case XC_LOCALE_NEW_ZEALAND: result = _locale_new_zealand; break;
    case XC_LOCALE_NORWAY: result = _locale_norway; break;
    case XC_LOCALE_POLAND: result = _locale_poland; break;
    case XC_LOCALE_PORTUGAL: result = _locale_portugal; break;
    case XC_LOCALE_SINGAPORE: result = _locale_singapore; break;
    case XC_LOCALE_SLOVAK_REPUBLIC: result = _locale_slovak_republic; break;
    case XC_LOCALE_SOUTH_AFRICA: result = _locale_south_africa; break;
    case XC_LOCALE_SPAIN: result = _locale_spain; break;
    case XC_LOCALE_SWEDEN: result = _locale_sweden; break;
    case XC_LOCALE_SWITZERLAND: result = _locale_switzerland; break;
    case XC_LOCALE_TAIWAN: result = _locale_taiwan; break;
    case XC_LOCALE_GREAT_BRITAIN: result = _locale_great_britain; break;
    case XC_LOCALE_UNITED_STATES: result = _locale_default; break;
    }
    return result;
}

int csstricmp(const char *s1, const char *s2)
{
    for (long i = 0; ; i++)
    {
        long c1 = towlower(s1[i]);
        long c2 = towlower(s2[i]);
        if (c1 == 0)
            return c2 ? -1 : 0;
        if (c2 == 0)
            return c1 != 0;
        if (c1 != c2)
            return c1 > c2 ? 1 : -1;
    }
}

int csstrnicmp(const char *s1, const char *s2, unsigned int n)
{
    for (unsigned int i = 0; ; i++)
    {
        if (i == n)
            return 0;
        long c1 = towlower(s1[i]);
        long c2 = towlower(s2[i]);
        if (c1 == 0)
            return c2 ? -1 : 0;
        if (c2 == 0)
            return c1 != 0;
        if (c1 != c2)
            return c1 > c2 ? 1 : -1;
    }
}


char *csstrtok(char *data, const char *delimiters, bool skip_delimiters, csstrtok_data *state)
{
    char *current;
    if (data == 0)
        current = state->next;
    else
        current = data;

    if (current != 0) {
        current += strspn(current, "\\");
        if (*current == 0)
            current = 0;
    }

    char *token = current;
    if (current != 0) {
        current = strpbrk(current, "\\");
        if (current != 0) {
            *current = 0;
            current++;
        }
    }
    state->next = current;
    return token;
}

// 0x19a6c0
char *csstrnzcat(char *destination, const char *source, unsigned int size)
{
    unsigned int length = csstrnlen(destination, size);
    char *result = strncpy(destination + length, source, size - length);
    destination[size - 1] = 0;
    return result;
}

char *csnzappendf(char *buffer, unsigned int size, const char *format, ...)
{
    va_list args;
    unsigned int length = strlen(buffer);
    unsigned int remaining = size - length;
    char *end = buffer + length;
    va_start(args, format);
    _vsnprintf(end, remaining - 1, format, args);
    end[remaining - 1] = 0;
    return buffer;
}

static __forceinline void *system_malloc(unsigned int size)
{
    return RtlAllocateHeap(GetProcessHeap(), 0, size);
}
static __forceinline void system_free(void *memory)
{
    if (memory)
        RtlFreeHeap(GetProcessHeap(), 0, memory);
}

// 0x19a7c0
void c_normal_allocation::deallocate(void *memory)
{
    system_free(memory);
}

// 0x19a800
void *c_normal_allocation::allocate(unsigned int size, const char *name)
{
    return system_malloc(size);
}


const char *get_language_iso_639_1_name(e_language language)
{
    const char *result = "";
    switch (language)
    {
    case _language_english: result = "en-US"; break;
    case _language_japanese: result = "jp-JP"; break;
    case _language_german: result = "de-DE"; break;
    case _language_french: result = "fr-FR"; break;
    case _language_spanish: result = "es-ES"; break;
    case _language_mexican: result = "es-MX"; break;
    case _language_italian: result = "it-IT"; break;
    case _language_korean: result = "ko-KR"; break;
    case _language_chinese_traditional: result = "zh-TW"; break;
    case _language_chinese_simplified: result = "zh-CN"; break;
    case _language_portuguese: result = "pt-PT"; break;
    case _language_polish: result = "pl-PL"; break;
    }
    return result;
}

// @address 0x1bf0a8 get_dense_index_from_xbox_value
extern "C" int get_dense_index_from_xbox_value(int xbox_value)
{
    int result = 100;
    switch (xbox_value) {
    case XONLINE_COUNTRY_UNITED_ARAB_EMIRATES: result = 0; break;
    case XONLINE_COUNTRY_ALBANIA: result = 1; break;
    case XONLINE_COUNTRY_ARMENIA: result = 2; break;
    case XONLINE_COUNTRY_ARGENTINA: result = 3; break;
    case XONLINE_COUNTRY_AUSTRIA: result = 4; break;
    case XONLINE_COUNTRY_AUSTRALIA: result = 5; break;
    case XONLINE_COUNTRY_AZERBAIJAN: result = 6; break;
    case XONLINE_COUNTRY_BELGIUM: result = 7; break;
    case XONLINE_COUNTRY_BULGARIA: result = 8; break;
    case XONLINE_COUNTRY_BAHRAIN: result = 9; break;
    case XONLINE_COUNTRY_BRUNEI_DARUSSALAM: result = 10; break;
    case XONLINE_COUNTRY_BOLIVIA: result = 11; break;
    case XONLINE_COUNTRY_BRAZIL: result = 12; break;
    case XONLINE_COUNTRY_BELARUS: result = 13; break;
    case XONLINE_COUNTRY_BELIZE: result = 14; break;
    case XONLINE_COUNTRY_CANADA: result = 15; break;
    case XONLINE_COUNTRY_SWITZERLAND: result = 16; break;
    case XONLINE_COUNTRY_CHILE: result = 17; break;
    case XONLINE_COUNTRY_CHINA: result = 18; break;
    case XONLINE_COUNTRY_COLOMBIA: result = 19; break;
    case XONLINE_COUNTRY_COSTA_RICA: result = 20; break;
    case XONLINE_COUNTRY_CZECH_REPUBLIC: result = 21; break;
    case XONLINE_COUNTRY_GERMANY: result = 22; break;
    case XONLINE_COUNTRY_DENMARK: result = 23; break;
    case XONLINE_COUNTRY_DOMINICAN_REPUBLIC: result = 24; break;
    case XONLINE_COUNTRY_ALGERIA: result = 25; break;
    case XONLINE_COUNTRY_ECUADOR: result = 26; break;
    case XONLINE_COUNTRY_ESTONIA: result = 27; break;
    case XONLINE_COUNTRY_EGYPT: result = 28; break;
    case XONLINE_COUNTRY_SPAIN: result = 29; break;
    case XONLINE_COUNTRY_FINLAND: result = 30; break;
    case XONLINE_COUNTRY_FAROE_ISLANDS: result = 31; break;
    case XONLINE_COUNTRY_FRANCE: result = 32; break;
    case XONLINE_COUNTRY_GREAT_BRITAIN: result = 33; break;
    case XONLINE_COUNTRY_GEORGIA: result = 34; break;
    case XONLINE_COUNTRY_GREECE: result = 35; break;
    case XONLINE_COUNTRY_GUATEMALA: result = 36; break;
    case XONLINE_COUNTRY_HONG_KONG: result = 37; break;
    case XONLINE_COUNTRY_HONDURAS: result = 38; break;
    case XONLINE_COUNTRY_CROATIA: result = 39; break;
    case XONLINE_COUNTRY_HUNGARY: result = 40; break;
    case XONLINE_COUNTRY_INDONESIA: result = 41; break;
    case XONLINE_COUNTRY_IRELAND: result = 42; break;
    case XONLINE_COUNTRY_ISRAEL: result = 43; break;
    case XONLINE_COUNTRY_INDIA: result = 44; break;
    case XONLINE_COUNTRY_IRAQ: result = 45; break;
    case XONLINE_COUNTRY_IRAN: result = 46; break;
    case XONLINE_COUNTRY_ICELAND: result = 47; break;
    case XONLINE_COUNTRY_ITALY: result = 48; break;
    case XONLINE_COUNTRY_JAMAICA: result = 49; break;
    case XONLINE_COUNTRY_JORDAN: result = 50; break;
    case XONLINE_COUNTRY_JAPAN: result = 51; break;
    case XONLINE_COUNTRY_KENYA: result = 52; break;
    case XONLINE_COUNTRY_KYRGYZSTAN: result = 53; break;
    case XONLINE_COUNTRY_KOREA: result = 54; break;
    case XONLINE_COUNTRY_KUWAIT: result = 55; break;
    case XONLINE_COUNTRY_KAZAKHSTAN: result = 56; break;
    case XONLINE_COUNTRY_LEBANON: result = 57; break;
    case XONLINE_COUNTRY_LIECHTENSTEIN: result = 58; break;
    case XONLINE_COUNTRY_LITHUANIA: result = 59; break;
    case XONLINE_COUNTRY_LUXEMBOURG: result = 60; break;
    case XONLINE_COUNTRY_LATVIA: result = 61; break;
    case XONLINE_COUNTRY_LIBYA: result = 62; break;
    case XONLINE_COUNTRY_MOROCCO: result = 63; break;
    case XONLINE_COUNTRY_MONACO: result = 64; break;
    case XONLINE_COUNTRY_MACEDONIA: result = 65; break;
    case XONLINE_COUNTRY_MONGOLIA: result = 66; break;
    case XONLINE_COUNTRY_MACAU: result = 67; break;
    case XONLINE_COUNTRY_MALDIVES: result = 68; break;
    case XONLINE_COUNTRY_MEXICO: result = 69; break;
    case XONLINE_COUNTRY_MALAYSIA: result = 70; break;
    case XONLINE_COUNTRY_NICARAGUA: result = 71; break;
    case XONLINE_COUNTRY_NETHERLANDS: result = 72; break;
    case XONLINE_COUNTRY_NORWAY: result = 73; break;
    case XONLINE_COUNTRY_NEW_ZEALAND: result = 74; break;
    case XONLINE_COUNTRY_OMAN: result = 75; break;
    case XONLINE_COUNTRY_PANAMA: result = 76; break;
    case XONLINE_COUNTRY_PERU: result = 77; break;
    case XONLINE_COUNTRY_PHILIPPINES: result = 78; break;
    case XONLINE_COUNTRY_PAKISTAN: result = 79; break;
    case XONLINE_COUNTRY_POLAND: result = 80; break;
    case XONLINE_COUNTRY_PUERTO_RICO: result = 81; break;
    case XONLINE_COUNTRY_PORTUGAL: result = 82; break;
    case XONLINE_COUNTRY_PARAGUAY: result = 83; break;
    case XONLINE_COUNTRY_QATAR: result = 84; break;
    case XONLINE_COUNTRY_ROMANIA: result = 85; break;
    case XONLINE_COUNTRY_RUSSIAN_FEDERATION: result = 86; break;
    case XONLINE_COUNTRY_SAUDI_ARABIA: result = 87; break;
    case XONLINE_COUNTRY_SWEDEN: result = 88; break;
    case XONLINE_COUNTRY_SINGAPORE: result = 89; break;
    case XONLINE_COUNTRY_SLOVENIA: result = 90; break;
    case XONLINE_COUNTRY_SLOVAK_REPUBLIC: result = 91; break;
    case XONLINE_COUNTRY_EL_SALVADOR: result = 92; break;
    case XONLINE_COUNTRY_SYRIA: result = 93; break;
    case XONLINE_COUNTRY_THAILAND: result = 94; break;
    case XONLINE_COUNTRY_TUNISIA: result = 95; break;
    case XONLINE_COUNTRY_TURKEY: result = 96; break;
    case XONLINE_COUNTRY_TRINIDAD_AND_TOBAGO: result = 97; break;
    case XONLINE_COUNTRY_TAIWAN: result = 98; break;
    case XONLINE_COUNTRY_UKRAINE: result = 99; break;
    case XONLINE_COUNTRY_UNITED_STATES: result = 100; break;
    case XONLINE_COUNTRY_URUGUAY: result = 101; break;
    case XONLINE_COUNTRY_UZBEKISTAN: result = 102; break;
    case XONLINE_COUNTRY_VENEZUELA: result = 103; break;
    case XONLINE_COUNTRY_VIET_NAM: result = 104; break;
    case XONLINE_COUNTRY_YEMEN: result = 105; break;
    case XONLINE_COUNTRY_SOUTH_AFRICA: result = 106; break;
    case XONLINE_COUNTRY_ZIMBABWE: result = 107; break;
    }
    return result;
}

// 0x1bf5e8
e_language get_e_language_from_xbox_language(unsigned long xbox_language)
{
  switch (xbox_language) {
  case XC_LANGUAGE_ENGLISH:
    return _language_english;
  case XC_LANGUAGE_JAPANESE:
    return _language_japanese;
  case XC_LANGUAGE_GERMAN:
    return _language_german;
  case XC_LANGUAGE_FRENCH:
    return _language_french;
  case XC_LANGUAGE_SPANISH:
    if (g_current_locale == -1)
      g_current_locale = get_e_locale_from_xbox_locale((long)XGetLocale());
    switch (g_current_locale) {
    case _locale_canada:
    case _locale_chile:
    case _locale_colombia:
    case _locale_mexico:
    case _locale_default:
      return _language_mexican;
    default:
      return _language_spanish;
    }
  case XC_LANGUAGE_ITALIAN:
    return _language_italian;
  case XC_LANGUAGE_KOREAN:
    return _language_korean;
  case XC_LANGUAGE_TCHINESE:
    return _language_chinese_traditional;
  case XC_LANGUAGE_SCHINESE:
    return _language_chinese_simplified;
  case XC_LANGUAGE_PORTUGUESE:
    return _language_portuguese;
  default:
    return _language_english;
  }
}

const char *get_language_suffix(e_language language, bool english_no_suffix)
{
    switch (language)
    {
    case _language_english: return english_no_suffix ? "" : "en";
    case _language_japanese: return "jpn";
    case _language_german: return "de";
    case _language_french: return "fr";
    case _language_spanish: return "sp";
    case _language_mexican: return "mx";
    case _language_italian: return "it";
    case _language_korean: return "kor";
    case _language_chinese_traditional: return "cht";
    case _language_chinese_simplified: return "chs";
    case _language_portuguese: return "pt";
    case _language_polish: return "pl";
    // match: 0x1bec80: without __assume(0) the jump table gains a cmplwi r3,11 + bgtlr range check and the shared blr moves
    default: __assume(0);
    }
}

// @address 0x1bed70 get_current_language_suffix
extern "C" const char *get_current_language_suffix(bool)
{
    e_language language = get_current_language();
    const char *(*suffix)(e_language, bool) = get_language_suffix;
    return suffix(language, false);
}

// @address 0x198c98 scenario_tags_try_and_load
extern "C" unsigned char scenario_tags_try_and_load(long campaign_id, long map_id, char const *scenario_path)
{
    bool result = false;
    if (scenario_tags_load(scenario_path)) {
        scenario *definition = TAG_GET('scnr', scenario, global_scenario_index);
        unsigned char campaign_matches;
        if (definition->campaign_id == campaign_id) goto campaign_true;
        if (campaign_id == -1) goto campaign_true;
        campaign_matches = 0; goto campaign_done;
campaign_true:
        campaign_matches = 1;
campaign_done:;
        unsigned char matches;
        if (campaign_matches != 0 && (definition->map_id == map_id || map_id == -1 || map_id == -2))
            matches = 1;
        else
            matches = 0;
        if (matches != 0) {
            scenario_tags_fixup();
            unsigned long address = (unsigned long)g_cache_file_globals.header.ranges[5].start;
            if (((((address >> 20) + 0x200) & 0x1000) + (address & 0x1fffffff) - 0x40000000) != address)
                XPhysicalProtect(pointer_from_address(address), (unsigned long)g_cache_file_globals.header.ranges[5].size, 2);
            address = (unsigned long)g_cache_file_globals.header.ranges[1].start;
            if (((((address >> 20) + 0x200) & 0x1000) + (address & 0x1fffffff) - 0x40000000) != address)
                XPhysicalProtect(pointer_from_address(address), (unsigned long)g_cache_file_globals.header.ranges[1].size, 2);
            result = true;
        }
        else
        {
            scenario_tags_unload();
            global_scenario_index = -1;
            global_scenario = 0;
            global_game_globals = 0;
            global_zone_set_index = -1;
            g_active_structure_bsp_mask = 0;
        }
    }
    return result;
}
