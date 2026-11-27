#ifndef MULTITHREADING_THREAD_TLS_H
#define MULTITHREADING_THREAD_TLS_H

#include "memory/tls_data_arrays.h"

static __forceinline long get_current_thread_index()
{
    return g_registered_thread_index;
}

#endif // MULTITHREADING_THREAD_TLS_H
