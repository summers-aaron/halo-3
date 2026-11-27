// networkinglibxbox/online_url.obj: from the records 0x8fc38 0x8fcd0 0x8fd80 0x94860
// @flags /TP /O2 /Oc /noGL
#include "memory/static_arrays.h"
#include "cseries/crt_xdk.h"
#include "cseries/cseries.h"
// @address 0x8fc38 online_url_make_cache_key
// @address 0x8fcd0 ?starts_with@?$c_static_string@$0BAA@@@QBA_NPBD@Z
// @address 0x8fd80 ?substring@?$c_static_string@$0BAA@@@QBA_NJJPAV1@@Z
// @address 0x94860 online_url_set_base_path
// @address 0x82308 ?print@?$c_static_string@$0BAA@@@QAAPBDPBDZZ
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "networking/online/online_url.h"
#include "networking/online/online_url_name_globals.h"


struct c_online_url_string
{
    c_online_url_path m_string;
    long m_service_type;
    long m_cache_mode;
    long m_lifetime_seconds;
};



extern "C" {
// @address 0x80a238 g_online_url_name_globals
#include "networking/online/online_url_name_globals.h"
// @address 0x1014 k_online_url_base_path_format
char k_online_url_base_path_format[] = "/storage/title/%s/";
// @address 0x1028 k_online_default_hopper_name
char k_online_default_hopper_name[] = "default_hoppers";
}

template<> bool c_online_url_path::starts_with(const char *prefix) const
{
    return memcmp(prefix, m_string, csstrnlen(prefix, 256)) == 0;
}

void online_url_make_cache_key(const char *source_url, char *cache_key)
{
    char key_string[256];
    char source_url_string[256];
    strncpy(key_string, source_url, 0x100);
    key_string[255] = 0;
    if ((unsigned char)((const c_online_url_path *)key_string)->starts_with("/storage") != 0) {
        strncpy(source_url_string, source_url, 0x100);
        source_url_string[255] = 0;
        ((const c_online_url_path *)source_url_string)->substring(8, ((const c_online_url_path *)key_string)->length() - 8, (c_online_url_path *)key_string);
    }
    strncpy(cache_key, key_string, 80);
    cache_key[79] = 0;
    return;
}

template<> bool c_online_url_path::substring(long start_index, long substring_length, c_online_url_path *output_string) const
{
    int copy_size;
    bool result;
    result = 0;
    if (start_index >= 0 && substring_length > 0 &&
        start_index + substring_length <= length()) {
        copy_size = substring_length + 1;
        if (copy_size > 0x100)
            copy_size = 0x100;
        strncpy(output_string->m_string, m_string + start_index, copy_size);
        output_string->m_string[copy_size - 1] = 0;
        result = 1;
    }
    return result;
}

// url base path, the name global first, then the default hopper name
extern "C" {
void online_url_set_base_path(c_online_url_string *url)
{
    char name[256];
    const char *source;
    size_t count;
    if (g_online_url_name_globals.base_path[0] != 0) {
        source = g_online_url_name_globals.base_path;
        count = 64;
        strncpy_s(name, 256, source, count);
        url->m_string.print(k_online_url_base_path_format, name);
        return;
    } else if (g_online_url_name_globals.name[0] != 0) {
        source = g_online_url_name_globals.name;
        count = 64;
    } else {
        source = k_online_default_hopper_name;
        count = 16;
    }
    strncpy_s(name, 256, source, count);
    url->m_string.print(k_online_url_base_path_format, name);
}
}
