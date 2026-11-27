#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_HASH_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_HASH_H

// SHA-1 hash of an HTTP request.
struct s_network_http_request_hash
{
    unsigned char hash[20];
};
typedef char network_http_request_hash_size_check[(sizeof(s_network_http_request_hash) == 20) ? 1 : -1];

#endif
