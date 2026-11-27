#ifndef MULTITHREADING_SEMAPHORE_DEFINITIONS_H
#define MULTITHREADING_SEMAPHORE_DEFINITIONS_H

#include <stddef.h>

// Semaphore definition: name, initial and maximum counts, and single-thread policy.
struct s_semaphore_definition
{
    const char *name;
    long initial_count;
    long maximum_count;
    signed char single_thread_reference;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char semaphore_definition_size[(sizeof(void *) != 4 || sizeof(s_semaphore_definition) == 16) ? 1 : -1];
typedef char semaphore_definition_maximum_count_offset[(sizeof(void *) != 4 || offsetof(s_semaphore_definition, maximum_count) == 8) ? 1 : -1];
#endif

#endif // MULTITHREADING_SEMAPHORE_DEFINITIONS_H
