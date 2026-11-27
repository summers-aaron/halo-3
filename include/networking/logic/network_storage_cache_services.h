#ifndef NETWORKING_LOGIC_NETWORK_STORAGE_CACHE_SERVICES_H
#define NETWORKING_LOGIC_NETWORK_STORAGE_CACHE_SERVICES_H

extern "C++" {
class c_http_buffer_downloader;
struct s_network_storage_cache_url;

extern "C" void online_url_set_base_path(void *url);
extern "C" void http_buffer_downloader_set_url(c_http_buffer_downloader *downloader, const s_network_storage_cache_url *url);
extern "C" bool content_catalogue_make_ready_for_path(const char *path, bool validate);
}

#endif // NETWORKING_LOGIC_NETWORK_STORAGE_CACHE_SERVICES_H
