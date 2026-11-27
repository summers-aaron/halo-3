#ifndef NETWORKING_ONLINE_ONLINE_STATISTICS_TASKS_H
#define NETWORKING_ONLINE_ONLINE_STATISTICS_TASKS_H
#include <xtl.h>
#include "cseries/async_xoverlapped_types.h"

struct s_internal_leaderboard_write
{
    unsigned long leaderboard_id;
    unsigned long property_count;
    XUSER_PROPERTY properties[32];
};

class c_statistics_write_overlapped_task : public c_overlapped_task
{
public:
    __forceinline c_statistics_write_overlapped_task(const char *file, long line) : c_overlapped_task(file, line) {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    void initialize(void *session, unsigned __int64 xuid, long write_count, const s_internal_leaderboard_write *leaderboard_writes);
private:
    void *m_session;
    unsigned __int64 m_xuid;
    long m_write_count;
    s_internal_leaderboard_write m_leaderboard_writes[4];
    XSESSION_VIEW_PROPERTIES m_views[4];
};
#endif
