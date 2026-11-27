#ifndef PHYSICS_HAVOK_PROXIES_GLOBALS_H
#define PHYSICS_HAVOK_PROXIES_GLOBALS_H

struct s_data_array;
extern "C" {
extern __declspec(thread) s_data_array *g_havok_proxy_data;
}

#endif // PHYSICS_HAVOK_PROXIES_GLOBALS_H
