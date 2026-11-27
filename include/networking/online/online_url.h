#ifndef NETWORKING_ONLINE_ONLINE_URL_H
#define NETWORKING_ONLINE_ONLINE_URL_H

// Functions defined in tu/networkinglibxbox/online_url.cpp that other units call.

#include "memory/static_arrays.h"
// match: 0x8fd80: URL qualification separates WPL inline decisions from other string capacities.
typedef c_static_string<256> c_online_url_path;
template<> bool c_online_url_path::substring(long start_index, long substring_length, c_static_string<256> *output_string) const;

struct c_online_url_string;

extern "C" char k_online_url_base_path_format[];
extern "C" char k_online_default_hopper_name[];

extern "C" void online_url_make_cache_key(const char *source_url, char *cache_key);
extern "C" void online_url_set_base_path(c_online_url_string *url);

#endif // NETWORKING_ONLINE_ONLINE_URL_H
