#ifndef MULTITHREADING_MESSAGE_QUEUE_H
#define MULTITHREADING_MESSAGE_QUEUE_H

#include "multithreading/synchronized_bitvector.h"

template<class message, long count> class t_message_queue {
public:
    c_synchronized_long message_count;
    c_synchronized_bitvector<count> in_use;
    message messages[count];
public:
    __forceinline void remove_messages_for_object(long object_index);
    void clear();
    bool try_add_message(const message *new_message);
    long try_to_append_message_queue(t_message_queue *source);
    __forceinline long try_to_prepend_message_queue(t_message_queue *source);
    void add_message(const message *new_message);
    void process_messages(void (*callback)(message *, long), long parameter);
};
template<class t_message, long k_count> __forceinline void t_message_queue<t_message, k_count>::remove_messages_for_object(long object_index)
{
    long count = message_count.peek();
    for (long message_index = 0; message_index < count; message_index++) {
        if (in_use.test_bit(message_index) && messages[message_index].object_index == object_index)
            ((c_synchronized_bitvector<512> *)&in_use)->set_bit(message_index, false);
    }
}
template<class t_message, long k_count> __forceinline long t_message_queue<t_message, k_count>::try_to_prepend_message_queue(t_message_queue *source)
{
    long count = source->message_count.peek();
    long failed_count = source->try_to_append_message_queue(this);
    clear();
    failed_count += try_to_append_message_queue(source);
    source->message_count.set(count);
    return failed_count;
}
template<class t_message, long k_count> __forceinline bool t_message_queue<t_message, k_count>::try_add_message(const t_message *message)
{
    if (message_count.peek() + 1 < k_count) {
        add_message(message);
        return true;
    }
    return false;
}

#endif // MULTITHREADING_MESSAGE_QUEUE_H
