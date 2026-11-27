// networkinglibxbox/network_storage_manifest.obj: from the records 0x8ffc0 0x90140 0x90238 0x4046a8 0x95c10
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/logic/storage/network_storage_cache.h"
#include "memory/static_arrays.h"
// @address 0x90238 network_storage_manifest_copy_hash
// @address 0x4046a8 ?add_untracked_entry@c_network_storage_manifest@@QAAXPBUs_network_storage_cache_key@@PBUs_network_http_request_hash@@J@Z
// @address 0x95c10 network_storage_manifest_update
// @address 0x8ffc0 network_storage_manifest_get
#include "cseries/crt_xdk.h"
#include "cache/security_functions.h"
#include "networking/logic/storage/network_storage_manifest.h"
#include "networking/logic/storage/network_storage_manifest_internals.h"
#include "networking/online/online_url_version.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/storage/network_storage_cache_key_types.h"
#include "networking/tools/blf_authentication_types.h"
#include "networking/tools/network_blf.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"

struct s_network_storage_cache_key;

// @address 0x11db09c g_network_storage_manifest_last_version
long g_network_storage_manifest_last_version;
// @address 0x7b0ecc g_network_storage_cache

c_network_storage_manifest::e_entry_state c_network_storage_manifest::get_entry_state(const s_network_storage_cache_key *key)
{
    e_entry_state result = _entry_unknown;
    c_static_string<80> key_string(key->path);
    if (m_manifest == 0)
    {
        result = _entry_unavailable;
    }
    else
    {
        for (long entry_index = 0; entry_index < network_blf_payload_long(m_manifest->entry_count); ++entry_index)
        {
            if (key_string.is_equal(m_manifest->entries[entry_index].key.path))
            {
                result = _entry_tracked;
                break;
            }
        }
        if (result == _entry_unknown)
        {
            for (unsigned long entry_index = 0; entry_index < 32; ++entry_index)
            {
                if (m_untracked_entries[entry_index].state != _entry_unknown
                    && m_untracked_entries[entry_index].state != _entry_tracked
                    && key_string.is_equal(m_untracked_entries[entry_index].key.path))
                {
                    result = m_untracked_entries[entry_index].state;
                    break;
                }
            }
        }
    }
    return result;
}

void c_network_storage_manifest::add_untracked_entry(const s_network_storage_cache_key *key, const s_network_http_request_hash *hash, long lifetime_seconds) {
    long entry_to_use_index = -1;
    long oldest_expiration_time = 0x7fffffff;
    c_static_string<80> key_string(key->path);
    for (unsigned long entry_index = 0; entry_index < 32; entry_index++) {
        if (slot_valid(entry_index) && key_string.is_equal(m_untracked_entries[entry_index].key.path)) {
            entry_to_use_index = entry_index;
            break;
        }
    }
    if (entry_to_use_index == -1) {
        for (unsigned long entry_index = 0; entry_index < 32; entry_index++) {
            if (!slot_valid(entry_index)) {
                entry_to_use_index = entry_index;
                break;
            } else if (m_untracked_entries[entry_index].expiration_time < oldest_expiration_time) {
                entry_to_use_index = entry_index;
                oldest_expiration_time = m_untracked_entries[entry_index].expiration_time;
            }
        }
    }
    m_untracked_entries[entry_to_use_index].key = *key;
    if (hash != 0) {
        m_untracked_entries[entry_to_use_index].hash = *hash;
        m_untracked_entries[entry_to_use_index].state = _entry_untracked_hash;
    } else {
        m_untracked_entries[entry_to_use_index].state = _entry_untracked_missing;
    }
    m_untracked_entries[entry_to_use_index].expiration_time = GetTickCount() + lifetime_seconds * 1000;
}

extern "C" bool network_storage_manifest_copy_hash(c_network_storage_manifest *m, const s_network_storage_cache_key *key, s_network_http_request_hash *out)
{
    bool result = false;
    c_static_string<80> name(key->path);
    long now = GetTickCount();
    c_network_storage_manifest::e_entry_state state = m->get_entry_state(key);
    if (state == c_network_storage_manifest::_entry_tracked)
    {
        if (m->m_manifest)
        {
            for (long i = 0; i < network_blf_payload_long(m->m_manifest->entry_count); i++)
            {
                if (name.is_equal(m->m_manifest->entries[i].key.path))
                {
                    memcpy(out, &m->m_manifest->entries[i].hash, sizeof(*out));
                    result = true;
                    break;
                }
            }
        }
    }
    else if (state == c_network_storage_manifest::_entry_untracked_hash)
    {
        for (unsigned long i = 0; i < 32; i++)
        {
            if (m->slot_valid(i) && now <= m->m_untracked_entries[i].expiration_time
                && name.is_equal(m->m_untracked_entries[i].key.path))
            {
                memcpy(out, &m->m_untracked_entries[i].hash, sizeof(*out));
                result = true;
                break;
            }
        }
    }
    return result;
}

static __forceinline bool network_storage_manifest_version_changed()
{
    bool result = false;
    volatile long *last_version = &g_network_storage_manifest_last_version;
    long previous = *last_version;
    long current = g_online_url_version;
    if (previous != current)
    {
        g_network_storage_manifest_last_version = current;
        result = true;
    }
    return result;
}
extern "C" c_network_storage_manifest *network_storage_manifest_get(void)
{
    c_network_storage_manifest *result = 0;
    const void *manifest = 0;
    unsigned long size = 0;
    security_get_section_data(1, &manifest, &size);
    if (manifest != 0 && size >= 0x50cc)
        result = (c_network_storage_manifest *)manifest;
    return result;
}
extern "C" void network_storage_manifest_update(c_network_storage_manifest *manifest) {
    long now = (long)GetTickCount();
    if ((manifest->m_expiration_time != 0 && now > manifest->m_expiration_time) || network_storage_manifest_version_changed())
        network_storage_cache_reset_manifest(manifest);
    if (!manifest->m_manifest) {
        const void *data;
        c_http_blf_simple_downloader<s_online_file_manifest> *downloader = &manifest->m_downloader;
        const char *buffer;
        long size = 0;
        buffer = 0;
        long status = downloader->get_data(&buffer, &size);
        if (status == 2 && buffer) {
            if (downloader->m_last_retry_count != downloader->get_retry_count())
                downloader->m_data = static_cast<const s_online_file_manifest *>(network_blf_find_chunk_in_file(buffer,
                    (unsigned long)size, 0x6f6e666d, 1,
                    _blf_file_authentication_none, (unsigned long *)&downloader->m_data_size));
        } else {
            downloader->m_data = 0;
            downloader->m_data_size = 0;
        }
        data = downloader->m_data;
        if (status == 2 && data) {
            manifest->m_manifest = (c_network_storage_manifest::s_manifest *)data;
            manifest->m_failed = false;
            manifest->m_expiration_time = now + 900000;
        }
        else if (status != 1) manifest->m_failed = true;
    }
    for (long i = 0; i < 32; i++) {
        if (manifest->slot_valid(i)) {
            long *expiration_time = &manifest->m_untracked_entries[i].expiration_time;
            if (*expiration_time != -1 && now > *expiration_time) {
                g_network_storage_cache.remove(&manifest->m_untracked_entries[i].key);
                memset(&manifest->m_untracked_entries[i], 0, sizeof(manifest->m_untracked_entries[i]));
            }
        }
    }
}
