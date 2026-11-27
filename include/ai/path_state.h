#ifndef AI_PATH_STATE_H
#define AI_PATH_STATE_H

#include "ai/ai.h"

// March c_path_state owns a scratch path_state and frees it in its out-of-line destructor.
// Each movement or behavior owner allocates its own scratch path state.
class c_path_state
{
public:
    void *m_data;

    c_path_state() : m_data(ai_scratch_allocate(0x281dc)) {}
    ~c_path_state();
};

typedef char path_state_owner_size[(sizeof(void *) != 4 || sizeof(c_path_state) == 4) ? 1 : -1];

#endif // AI_PATH_STATE_H
