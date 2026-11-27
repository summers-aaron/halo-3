#ifndef CSERIES_PROGRESS_H
#define CSERIES_PROGRESS_H

#include <stddef.h>

// blamlibxbox progress.obj. A caller may install its own callbacks; when it has none the default set reports.
// Field roles are read from progress_new (+0/+16), progress_update_with_description (+4/+20) and
// progress_done (+8/+24), each of which passes the callback set's data pointer as the last argument.
struct progress_callbacks
{
    void (*progress_new_proc)(const char *description, void *data);
    void (*progress_update_proc)(const char *description, const char *update, long percentage, void *data);
    void (*progress_done_proc)(const char *description, void *data);
    void *progress_data;
};

struct s_progress_globals
{
    progress_callbacks default_callbacks;
    progress_callbacks custom_callbacks;
    bool disable_callbacks;
    bool currently_valid;
    char description[256];
    long last_percentage;
    unsigned long last_update_time;
};

typedef char progress_callbacks_size[(sizeof(void *) != 4 || sizeof(progress_callbacks) == 0x10) ? 1 : -1];
typedef char s_progress_globals_size[(sizeof(void *) != 4 || sizeof(s_progress_globals) == 0x12c) ? 1 : -1];
typedef char s_progress_globals_description_offset[(sizeof(void *) != 4 || offsetof(s_progress_globals, description) == 0x22) ? 1 : -1];
typedef char s_progress_globals_last_percentage_offset[(sizeof(void *) != 4 || offsetof(s_progress_globals, last_percentage) == 0x124) ? 1 : -1];

extern "C" s_progress_globals progress_globals;

extern "C" void progress_new(const char *description);
extern "C" long progress_update_with_description(long current, long total);
extern "C" void progress_done(void);

#endif // CSERIES_PROGRESS_H
