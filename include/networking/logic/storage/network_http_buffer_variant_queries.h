#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BUFFER_VARIANT_QUERIES_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BUFFER_VARIANT_QUERIES_H

struct s_network_http_buffer_blf_downloader_a;
struct s_network_http_buffer_blf_downloader_b;
extern "C" long network_http_buffer_downloader_get_game_variant(
    s_network_http_buffer_blf_downloader_a *downloader, void **out_data, unsigned long *out_data_size);
extern "C" long network_http_buffer_downloader_get_map_variant(
    s_network_http_buffer_blf_downloader_b *downloader, void **out_data, unsigned long *out_data_size);

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BUFFER_VARIANT_QUERIES_H
