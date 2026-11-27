#ifndef CSERIES_DATA_MINING_EVENT_TYPES_H
#define CSERIES_DATA_MINING_EVENT_TYPES_H

#include "cseries/datamine_parameter_types.h"

class c_datamine
{
public:
    c_datamine(long level, const char *event_name, long category_count, ...);
    ~c_datamine();
    s_datamine_parameter *fill_and_get_parameter(const char *name, long type, long size);
    void add_long(const char *name, long value);
    void add_wchar_string(const char *name, const wchar_t *value);

    long level;
    long category_count;
    char *format;
    char *output;
    void *event;
    char *position;
};

typedef char datamine_size[(sizeof(void *) != 4 || sizeof(c_datamine) == 0x18) ? 1 : -1];

// Storage used by the players event-initialization adapter.
struct s_event
{
    long level;
    long category_count;
    void *data[4];
};
typedef char data_mining_event_size[(sizeof(s_event) == sizeof(c_datamine)) ? 1 : -1];

#endif // CSERIES_DATA_MINING_EVENT_TYPES_H
