#ifndef RENDER_OBJECT_RENDER_MESSAGE_QUEUE_TLS_H
#define RENDER_OBJECT_RENDER_MESSAGE_QUEUE_TLS_H

struct s_object_render_thread_message;
template<class t_message, long k_count> class t_message_queue;

extern "C" {
extern __declspec(thread) t_message_queue<s_object_render_thread_message, 2048> *g_object_message_queue;
}

#endif
