#ifndef NETWORKING_ONLINE_ONLINE_FILES_ENUMERATOR_H
#define NETWORKING_ONLINE_ONLINE_FILES_ENUMERATOR_H
#include "memory/static_arrays.h"
#include "networking/online/file_share_slot_types.h"

class c_online_files_enumerator
{
public:
    __forceinline c_online_files_enumerator() :
        m_xuid(0),
        m_owner_xuid(0),
        m_request_cookie(0),
        m_state(0),
        m_quota_bytes(0),
        m_quota_slots(0),
        m_visible_slots(0)
    {
    }

    void stop();
    void update();
    __forceinline bool is_busy() { return m_request_cookie != 0; }
    __forceinline long get_quota_slot_count() const { return m_visible_slots; }
    __forceinline unsigned __int64 get_file_share_xuid() const { return m_owner_xuid; }
    __forceinline unsigned __int64 get_quota_byte_count() const { return m_quota_bytes; }
    __forceinline const s_file_share_slot *get_file_share_slot(long index) const
    {
        if (index >= 0 && (unsigned long)index < 25)
            return &m_slots[index];
        return 0;
    }
private:
    bool process_request_result();
public: // Shared with the file-share screen.
    unsigned __int64 m_xuid;
    unsigned __int64 m_owner_xuid;
    long m_request_cookie;
    long m_state;
    char m_results[0x4000];
    unsigned __int64 m_quota_bytes;
    long m_quota_slots;
    long m_visible_slots;
    long m_subscription_hash;
    c_static_wchar_string<256> m_message;
    s_file_share_slot m_slots[25];
};

struct s_file_share_enumerator_state : c_online_files_enumerator
{
    __forceinline s_file_share_enumerator_state() : started(false), m_active_transfer_token(0) {}
    bool started;
    long m_active_transfer_token;
};

typedef char online_files_enumerator_slots_offset[(offsetof(c_online_files_enumerator, m_slots) == 0x4230) ? 1 : -1];
typedef char online_files_enumerator_size[(sizeof(c_online_files_enumerator) == 0x5fe0) ? 1 : -1];
#endif
