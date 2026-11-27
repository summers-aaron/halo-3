// binkxenon/radcb.obj: from the records 0x7122f0 0x712358 0x712400 0x712588 0x712780 0x7127f0 0x7128e8 0x712990 0x7129b8 0x712a10 0x712a30 0x712a40 0x712a60 0x712b08 0x712b10
// @flags /TP /O2 /Oc
// @compiler xdk3424
// @address 0x7122f0 remove_cb_from_list
// @address 0x712358 my_wait_for_2
// @address 0x712588 RAD_callback_thread
// @address 0x7128e8 RADCB_suspend_2_handlers
// @address 0x712990 RADCB_resume_handler
// @address 0x7129b8 RADCB_try_to_suspend_callback
// @address 0x712780 RADCB_register_callback
// @address 0x7127f0 request_free
// @address 0x712a10 RADCB_suspend_callback
// @address 0x712a30 RADCB_suspend_2_callbacks
// @address 0x712a40 RADCB_resume_callback
// @address 0x712b08 RADCB_idle_on_callbacks
#include "bink/radcb.h"
#include <xtl.h>
// @address 0x712868 RADCB_release_handler_resources

int remove_cb_from_list(s_callback_thread *list, s_registered_callback *node)
{
    s_registered_callback *p;
    if (list == 0) return 0;
    p = list->callbacks;
    if (p == node) {
        list->callbacks = p->next;
        return 1;
    }
    for (; p->next != 0; p = p->next) {
        if (p->next == node) {
            p->next = node->next;
            return 1;
        }
    }
    return 0;
}

void my_wait_for_2(void *a, void *b)
{
    if (a == 0) {
        if (b != 0)
            WaitForSingleObject(b, -1);
        return;
    }
    if (b == 0) {
        WaitForSingleObject(a, -1);
        return;
    }
    void *handles[2];
    handles[0] = a;
    handles[1] = b;
    unsigned long timeout = -1;
    unsigned long result = WaitForMultipleObjects(2, handles, 0, timeout);
    if (result >= 1) {
        if (result == 1)
            WaitForSingleObject(a, timeout);
    } else
        WaitForSingleObject(b, timeout);
}

static __forceinline s_registered_callback *radcb_prepare_callback_insertion(
    s_registered_callback *sorted, s_registered_callback *registration, unsigned int priority)
{
    s_registered_callback *previous = sorted;
    s_registered_callback **next_link = &sorted->sorted_next;
    if (*next_link)
    {
        do
        {
            if (priority >= (*next_link)->priority)
            {
                registration->sorted_next = previous->sorted_next;
                return previous;
            }
            previous = *next_link;
            next_link = &previous->sorted_next;
        } while (*next_link);
    }
    registration->sorted_next = 0;
    return previous;
}

// @address 0x712400 call_callbacks
extern "C" void call_callbacks(s_callback_thread *thread)
{
    ++thread->iteration;
    for (;;)
    {
        s_registered_callback *sorted = 0;
        for (s_registered_callback *registration = thread->callbacks; registration; registration = registration->next)
        {
            if (thread->state != 1)
                return;
            unsigned int priority = registration->prepare(registration, thread->iteration);
            registration->priority = priority;
            if (priority)
            {
                if (!sorted)
                {
                    sorted = registration;
                    registration->sorted_next = 0;
                }
                else if (priority >= sorted->priority)
                {
                    registration->sorted_next = sorted;
                    sorted = registration;
                }
                else
                {
                    s_registered_callback *previous = radcb_prepare_callback_insertion(sorted, registration, priority);
                    previous->sorted_next = registration;
                }
            }
        }
        void *handles[16];
        s_registered_callback *registrations[16];
        s_registered_callback *registration = sorted;
        unsigned int count = 0;
        for (; registration && count < 16; registration = registration->sorted_next)
        {
            handles[count] = registration->mutex;
            registrations[count] = registration;
            ++count;
        }
        if (!count)
            return;
        unsigned long selected = WaitForMultipleObjects(count, handles, 0, 1);
        if (selected < count)
        {
            registrations[selected]->complete(registrations[selected], thread->iteration);
            ReleaseMutex(handles[selected]);
        }
    }
}

int RAD_callback_thread(s_callback_thread *thread)
{
    unsigned long timeout = -1;
    while (WaitForSingleObject(thread->event, thread->timeout)) {
        WaitForSingleObject(thread->mutex, timeout);
        if (thread->state != 1) {
            if (thread->state == 3) {
                ReleaseMutex(thread->mutex);
                return 0;
            }
        } else if (thread->has_callbacks)
            call_callbacks(thread);
        ReleaseMutex(thread->mutex);
    }
    return 0;
}

extern "C" void RADCB_register_callback(s_callback_thread *l, s_registered_callback *n, void (*callback_function)(void *), void *callback_data)
{
    s_registered_callback *head;
    n->context = callback_data; n->callback = callback_function; n->mutex = CreateMutexA(0,0,0);
    head = l->callbacks; n->next = head; l->callbacks = n; l->callback_count++;
}

// 0x7127f0 natural source
extern "C" void request_free(s_callback_thread *thread)
{
    int previous_state = thread->state;
    SetEvent(thread->event);
    thread->state = 3;
    SetThreadPriority(thread->thread_handle, 15);
    if (previous_state != 0) {
        if (previous_state == 2)
            ReleaseMutex(thread->mutex);
    } else {
        ResumeThread(thread->thread_handle);
    }
}

extern "C" {
void RADCB_suspend_2_handlers(s_callback_thread *a, s_callback_thread *b)
{
    if (a != 0) {
        a->state = 4;
        if (b != 0) {
            b->state = 4;
            my_wait_for_2(a->mutex, b->mutex);
            a->state = 2;
            b->state = 2;
        } else {
            WaitForSingleObject(a->mutex, -1);
            a->state = 2;
        }
    } else if (b != 0) {
        b->state = 4;
        WaitForSingleObject(b->mutex, -1);
        b->state = 2;
    }
}
}

// 0x712990
extern "C" void RADCB_resume_handler(s_callback_thread *context)
{
    if (context->state == 0) {
        context->state = 1;
        ResumeThread(context->thread_handle);
        return;
    }
    context->state = 1;
    ReleaseMutex(context->mutex);
}

extern "C" {
int RADCB_try_to_suspend_callback(unsigned int thread, s_registered_callback *registration)
{
  int wait_result;

  if ((registration != 0) && (registration->mutex != 0)) {
    wait_result = WaitForSingleObject(registration->mutex, 0);
    return wait_result == 0U;
  }
  return true;
}
}
// 0x712a10 natural source
extern "C" void RADCB_suspend_callback(s_callback_thread *thread, s_registered_callback *registration)
{
    if (registration && registration->mutex)
        WaitForSingleObject(registration->mutex, -1);
}

// 0x712a30 natural source
extern "C" void RADCB_suspend_2_callbacks(
    s_callback_thread *first_thread, s_registered_callback *first, s_callback_thread *second_thread, s_registered_callback *second)
{
    my_wait_for_2(first->mutex, second->mutex);
}

// 0x712a40 natural source
extern "C" void RADCB_resume_callback(s_callback_thread *thread, s_registered_callback *registration)
{
    if (registration && registration->mutex)
        ReleaseMutex(registration->mutex);
}

// @address 0x712a60 RADCB_free_2_handlers
extern "C" void RADCB_free_2_handlers(s_callback_thread *first, s_callback_thread *second)
{
    if (!first)
    {
        if (!second)
            return;
        request_free(second);
        WaitForSingleObject(second->thread_handle, -1);
        RADCB_release_handler_resources(second);
    }
    else if (!second)
    {
        request_free(first);
        WaitForSingleObject(first->thread_handle, -1);
        RADCB_release_handler_resources(first);
    }
    else
    {
        request_free(first);
        request_free(second);
        my_wait_for_2(first->thread_handle, second->thread_handle);
        RADCB_release_handler_resources(first);
        RADCB_release_handler_resources(second);
    }
}

// 0x712b08 natural source
extern "C" void RADCB_idle_on_callbacks()
{
    Sleep(0);
}

// @address 0x712b10 do_unregister
extern "C" int do_unregister(s_callback_thread *thread, s_registered_callback *registration, unsigned int flags)
{
    --thread->callback_count;
    int freed;
    if ((flags & 1) && !thread->callbacks)
    {
        request_free(thread);
        WaitForSingleObject(thread->thread_handle, -1);
        RADCB_release_handler_resources(thread);
        freed = 1;
    }
    else
    {
        thread->state = 4;
        WaitForSingleObject(thread->mutex, -1);
        thread->state = 1;
        ReleaseMutex(thread->mutex);
        freed = 0;
    }
    CloseHandle(registration->mutex);
    registration->mutex = 0;
    return freed;
}

