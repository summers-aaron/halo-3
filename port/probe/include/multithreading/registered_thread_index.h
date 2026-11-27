#ifndef MULTITHREADING_REGISTERED_THREAD_INDEX_H
#define MULTITHREADING_REGISTERED_THREAD_INDEX_H

// This thread's registered index (TLS slot 0x831df674), owned by threads.cpp:
// March 2007 debug name and type; initialize_thread_management touches only this slot in both builds.
// The MSVC-ABI ELF target also defines _WIN32.
extern __declspec(thread)
#if defined(__ELF__) && __SIZEOF_POINTER__ == 8
    __attribute__((tls_model("local-exec")))
#endif
    long g_registered_thread_index;

#endif // MULTITHREADING_REGISTERED_THREAD_INDEX_H
