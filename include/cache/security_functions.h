#ifndef CACHE_SECURITY_FUNCTIONS_H
#define CACHE_SECURITY_FUNCTIONS_H

// Functions defined in tu/blamlibxbox/security_functions.cpp that other units call.

struct s_network_http_request_hash;
struct _XHASH_STATE;

bool security_calculate_hash(const char *data, unsigned long size, bool salted, s_network_http_request_hash *hash);
extern "C" void security_get_section_data(long index, const void **address, unsigned long *size);
extern "C" void *security_get_module_handle(const char *module_name);
extern "C" bool security_initialize_hash(_XHASH_STATE *state, int size, bool salted);

#endif // CACHE_SECURITY_FUNCTIONS_H
