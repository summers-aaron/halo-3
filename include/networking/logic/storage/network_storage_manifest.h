#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_MANIFEST_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_MANIFEST_H

// Functions defined in tu/networkinglibxbox/network_storage_manifest.cpp that other units call.

class c_network_storage_manifest;

extern "C" c_network_storage_manifest *network_storage_manifest_get(void);
extern "C" void network_storage_manifest_update(c_network_storage_manifest *manifest);

#include "networking/logic/storage/network_storage_cache_key_types.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
class c_network_storage_manifest {
public:
    enum e_entry_state { _entry_unknown, _entry_tracked, _entry_untracked_hash, _entry_untracked_missing, _entry_unavailable };
    struct s_tracked_entry {
        s_network_storage_cache_key key;
        s_network_http_request_hash hash;
    };
    struct s_manifest {
        long entry_count;
        s_tracked_entry entries[1];
    };
    struct s_untracked_entry {
        e_entry_state state;
        s_network_storage_cache_key key;
        s_network_http_request_hash hash;
        long expiration_time;
    };
    e_entry_state get_entry_state(const s_network_storage_cache_key *key);
    void add_untracked_entry(const s_network_storage_cache_key *key, const s_network_http_request_hash *hash, long lifetime_seconds);
    __forceinline bool slot_valid(long entry_index) {
        return m_untracked_entries[entry_index].state == _entry_untracked_hash
            || m_untracked_entries[entry_index].state == _entry_untracked_missing;
    }
public:
    long m_expiration_time;
    s_manifest *m_manifest;
    bool m_failed;
    long m_times[128];
    c_http_blf_simple_downloader<s_online_file_manifest> m_downloader;
    s_untracked_entry m_untracked_entries[32];
};

extern "C" bool network_storage_manifest_copy_hash(c_network_storage_manifest *manifest,
    const s_network_storage_cache_key *key, s_network_http_request_hash *hash);

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_MANIFEST_H
