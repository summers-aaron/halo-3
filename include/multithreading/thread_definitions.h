#ifndef MULTITHREADING_THREAD_DEFINITIONS_H
#define MULTITHREADING_THREAD_DEFINITIONS_H

struct s_thread_default_row {
    const char *name;
    long processor;
    long stack_size;
    long default_priority;
    unsigned long (*start_routine)(void *parameter);
    void *parameter;
};

extern "C" const s_thread_default_row k_thread_default_definitions[];

#endif
