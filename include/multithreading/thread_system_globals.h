#ifndef MULTITHREADING_THREAD_SYSTEM_GLOBALS_H
#define MULTITHREADING_THREAD_SYSTEM_GLOBALS_H

#include <stddef.h>

struct s_thread_system_globals
{
    void *handles[9];
    unsigned long thread_ids[9];
    void *thread_parameters[9];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char thread_system_globals_size[(sizeof(void *) != 4 || sizeof(s_thread_system_globals) == 0x6c) ? 1 : -1];
typedef char thread_system_globals_ids_offset[(sizeof(void *) != 4 || offsetof(s_thread_system_globals, thread_ids) == 0x24) ? 1 : -1];
typedef char thread_system_globals_parameters_offset[(sizeof(void *) != 4 || offsetof(s_thread_system_globals, thread_parameters) == 0x48) ? 1 : -1];
#endif

#endif // MULTITHREADING_THREAD_SYSTEM_GLOBALS_H
