#ifndef INTERFACE_CHUD_CHUD_MESSAGING_MANAGER_H
#define INTERFACE_CHUD_CHUD_MESSAGING_MANAGER_H

#include <stddef.h>
#include "interface/chud/chud_medal_message_types.h"
#include "interface/chud/chud_message_types.h"
#include "game/game_time_types.h"
#include "interface/chud/chud_message_context_types.h"

enum { k_chud_message_medal_count = 4 };
enum { k_chud_message_count = 5 };
enum { k_chud_message_length = 96 };

struct s_chud_text_entry
{
    wchar_t text[k_chud_message_length];
    long : 32;
    long duration;
    unsigned char precached;
    long start_time;
    bool active;
    float scale;
    float alpha;
    float offset;
};

struct c_chud_messaging_manager
{
    s_chud_medal_message m_medals[k_chud_message_medal_count];
    s_chud_message m_messages[k_chud_message_count];
    s_chud_text_entry m_state_messages[4];

    __forceinline long get_new_message_index()
    {
        long index = -1;
        long oldest_time = game_time_globals->time + 1;
        long oldest_index = -1;
        for (long message_index = 0; message_index < k_chud_message_count; message_index++)
        {
            s_chud_message *message = &m_messages[message_index];
            if (!message->active)
            {
                index = message_index;
                break;
            }
            else if (message->time < oldest_time)
            {
                oldest_time = message->time;
                oldest_index = message_index;
            }
        }
        if (index == -1)
        {
            index = oldest_index;
        }
        return index;
    }

    void post(const wchar_t *text, e_chud_message_context context);
    void post_medal(long list_index, long medal_value);
private:
    void update_medals(long);
public:
    void clear_messages();
    void draw(long user_index);
    __forceinline void clear_medals()
    {
        for (long i = 0; i < 4; i++)
            m_medals[i].active = false;
    }
    __forceinline void reset()
    {
        clear_messages();
        clear_medals();
    }
    __forceinline void update(long user_index);
private:
    void update_state_messages(long user_index);

};

typedef char chud_text_entry_size[(sizeof(s_chud_text_entry) == 0xe0) ? 1 : -1];
typedef char chud_text_entry_active_offset[(offsetof(s_chud_text_entry, active) == 0xd0) ? 1 : -1];
typedef char chud_manager_state_messages_offset[(offsetof(c_chud_messaging_manager, m_state_messages) == 0x518) ? 1 : -1];
typedef char chud_messaging_manager_size[(sizeof(c_chud_messaging_manager) == 0x898) ? 1 : -1];

#endif
