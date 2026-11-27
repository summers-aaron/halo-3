#ifndef BINK_RADCB_H
#define BINK_RADCB_H

#include <stddef.h>

struct s_registered_callback
{
    s_registered_callback *next;
    void *mutex;
    s_registered_callback *sorted_next;
    unsigned int priority;
    union {
        void (*callback)(void *);
        unsigned int (*prepare)(s_registered_callback *, unsigned int);
    };
    union {
        void *context;
        void (*complete)(s_registered_callback *, unsigned int);
    };
};

struct s_callback_thread
{
    int state;
    int timeout;
    void *event;
    union { int thread; void *thread_handle; };
    unsigned long thread_id;
    void *mutex;
    union {
        unsigned int has_callbacks;
        s_registered_callback *callbacks;
    };
    unsigned int iteration;
    int callback_count;
};

typedef char radcb_registration_mutex_offset[(sizeof(void *) != 4 || ((offsetof(s_registered_callback, mutex) == 4) )) ? 1 : -1];
typedef char radcb_registration_size[(sizeof(void *) != 4 || ((sizeof(s_registered_callback) == 24) )) ? 1 : -1];
typedef char radcb_thread_callbacks_offset[(sizeof(void *) != 4 || ((offsetof(s_callback_thread, callbacks) == 24) )) ? 1 : -1];
typedef char radcb_thread_iteration_offset[(sizeof(void *) != 4 || ((offsetof(s_callback_thread, iteration) == 28) )) ? 1 : -1];
typedef char radcb_thread_size[(sizeof(void *) != 4 || ((sizeof(s_callback_thread) == 36) )) ? 1 : -1];

extern "C" {
int remove_cb_from_list(s_callback_thread *thread, s_registered_callback *registration);
void my_wait_for_2(void *first, void *second);
void call_callbacks(s_callback_thread *thread);
int RAD_callback_thread(s_callback_thread *thread);
void RADCB_register_callback(s_callback_thread *thread, s_registered_callback *registration, void (*callback)(void *), void *context);
void request_free(s_callback_thread *thread);
void RADCB_suspend_2_handlers(s_callback_thread *first, s_callback_thread *second);
void RADCB_resume_handler(s_callback_thread *thread);
int RADCB_try_to_suspend_callback(unsigned int thread, s_registered_callback *registration);
void RADCB_release_handler_resources(s_callback_thread *thread);
}

#endif // BINK_RADCB_H
