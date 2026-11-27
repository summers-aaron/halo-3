#ifndef NETWORKING_ONLINE_ONLINE_GUIDE_STATE_TYPES_H
#define NETWORKING_ONLINE_ONLINE_GUIDE_STATE_TYPES_H

#include <string.h>
#include <stddef.h>
#include "cseries/cseries.h"
#include "cseries/async_xoverlapped.h"
#include "interface/controller_types.h"
#include "memory/static_arrays.h"
#include "networking/online/online_service_record.h"

struct s_compose_option
{
    unsigned long type;
    wchar_t text[24];
};
struct s_compose_button
{
    unsigned long type;
    wchar_t text[24];
    unsigned long option_count;
    s_compose_option options[11];
};

struct s_compose_string
{
    unsigned long type;
    c_static_wchar_string<256> text;
};

struct s_compose_text
{
    c_static_wchar_string<256> title;
    c_static_wchar_string<256> text;
    c_static_wchar_string<256> description;
    s_compose_string strings[12];
};


#pragma pack(push, 4)
struct s_compose_data
{
    __forceinline s_compose_data()
        : xuid(0),
          value(0xffffffff)
    {
        name[0] = 0;
    }
    __forceinline s_compose_data(unsigned __int64 in_xuid, unsigned long in_value, const char *in_name, const s_service_record_identity *in_identity)
        : xuid(in_xuid),
          value(in_value)
    {
        csstrnzcpy(name, in_name, sizeof(name));
        payload = *in_identity;
    }
    unsigned __int64 xuid;
    unsigned long value;
    char name[33];
    s_service_record_identity payload;
};
struct s_compose_custom_data
{
    unsigned __int64 xuid;
    unsigned long value;
    char name[33];
    s_service_record_identity payload;
    long reserved;
};
#pragma pack(pop)
class c_show_custom_message_compose_task : public c_overlapped_task
{
public:
    __forceinline c_show_custom_message_compose_task()
        : c_overlapped_task(0, 0),
          controller_index(-1)
    {
    }
    __forceinline c_show_custom_message_compose_task(const char *file, long line, long controller, unsigned __int64 xuid, long value,
        const char *name, const s_service_record_identity *identity, const s_compose_text *texts)
        : c_overlapped_task(file, line),
          controller_index(controller),
          source(xuid, value, name, identity),
          m_texts(*texts)
    {
    }
    __forceinline virtual ~c_show_custom_message_compose_task() {}
    __forceinline c_show_custom_message_compose_task &operator=(const c_show_custom_message_compose_task &other)
    {
        c_overlapped_task::operator=(other);
        controller_index = other.controller_index;
        source.xuid = other.source.xuid;
        source.value = other.source.value;
        memcpy(source.name, other.source.name, sizeof(source.name));
        source.payload = other.source.payload;
        m_texts = other.m_texts;
        for (long button_index = 0; button_index < 4; button_index++)
            buttons[button_index] = other.buttons[button_index];
        button_count = other.button_count;
        custom_data = other.custom_data;
        return *this;
    }
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *platform_handle);
private:
    long controller_index;
    unsigned long padding;
    s_compose_data source;
    s_compose_text m_texts;
    s_compose_button buttons[4];
    unsigned long button_count;
    s_compose_custom_data custom_data;
};

#include "networking/online/online_friend_xenon.h"
#pragma pack(push, 4)
struct s_online_guide_state;
#include "networking/online/online_guide_xenon_services.h"

struct s_online_guide_state_base
{
    __forceinline s_online_guide_state_base(s_online_guide_state *self)
    {
        // match: 0x39df30: singleton global constructor specialization
        __assume(self == &g_online_guide_globals);
    }
};

struct s_online_guide_state : s_online_guide_state_base
{
    s_online_guide_state();
    __forceinline ~s_online_guide_state() {}
    c_show_custom_message_compose_task compose;
    bool actions;
    e_controller_index controller_index;        // 10548
    unsigned __int64 xuid;                      // 10552
    bool find_friend_active;                    // 10560
    c_online_find_friend_task find_friend;     // 10564
    unsigned __int64 session;                   // 30184
    long player_details_pending;                // 30192
    long qos_attempt;                           // 30196
    long service_record_mode;                    // 30200
};
#pragma pack(pop)

typedef char online_guide_actions_offset[(sizeof(void *) != 4 || ((offsetof(s_online_guide_state, actions) == 0x2930))) ? 1 : -1];
typedef char online_guide_friend_active_offset[(sizeof(void *) != 4 || ((offsetof(s_online_guide_state, find_friend_active) == 0x2940))) ? 1 : -1];
typedef char online_guide_details_offset[(sizeof(void *) != 4 || ((offsetof(s_online_guide_state, player_details_pending) == 0x75f0))) ? 1 : -1];
typedef char online_guide_service_record_offset[(sizeof(void *) != 4 || ((offsetof(s_online_guide_state, service_record_mode) == 0x75f8))) ? 1 : -1];

#endif
