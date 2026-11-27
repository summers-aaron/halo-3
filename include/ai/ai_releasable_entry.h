#ifndef AI_AI_RELEASABLE_ENTRY_H
#define AI_AI_RELEASABLE_ENTRY_H

struct s_ai_releasable_entry
{
    bool is_actor;
    long index;
    long release_index;
};

struct s_ai_releasable_storage
{
    short count;
    short released_count;
    s_ai_releasable_entry entries[256];
};

extern "C" int compare_releasable_inactive_squads(const s_ai_releasable_entry *a, const s_ai_releasable_entry *b);

#endif // AI_AI_RELEASABLE_ENTRY_H
