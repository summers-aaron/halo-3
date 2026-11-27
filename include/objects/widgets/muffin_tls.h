#ifndef OBJECTS_WIDGETS_MUFFIN_TLS_H
#define OBJECTS_WIDGETS_MUFFIN_TLS_H

#include "physics/havok_proxies_globals.h"

// Muffin map state lives in the shared havok-proxy TLS array.
static __forceinline s_data_array *&tls_muffin_map_data() { return g_havok_proxy_data; }

#endif // OBJECTS_WIDGETS_MUFFIN_TLS_H
