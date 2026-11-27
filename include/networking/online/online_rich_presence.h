#ifndef NETWORKING_ONLINE_ONLINE_RICH_PRESENCE_H
#define NETWORKING_ONLINE_ONLINE_RICH_PRESENCE_H

#include <stddef.h>

// Data defined in tu/networkinglibxbox/online_rich_presence.cpp that other units use.

struct s_presence_contexts { long game_mode, difficulty, hopper, game_engine, map, state; };
class c_online_rich_presence_manager {
public:
    c_online_rich_presence_manager()
    {
        m_last_update_time = 0;
        long *current = &m_contexts[0].game_mode;
        do {
            for (unsigned long context = 0; context < 6; ++context, ++current) {
                current[24] = -1;
                *current = -1;
            }
        // match: 0x709d50: the end test compares the cursor addresses as signed integers.
        } while ((intptr_t)current < (intptr_t)&m_last_contexts[0].game_mode);
    }
    void update();
private:
    void set_presence_state_to_game();
    unsigned long m_last_update_time;
    s_presence_contexts m_contexts[4];
    s_presence_contexts m_last_contexts[4];
};


extern c_online_rich_presence_manager g_rich_presence_manager;

#endif // NETWORKING_ONLINE_ONLINE_RICH_PRESENCE_H
