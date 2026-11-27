#ifndef EFFECTS_DECAL_MESSAGE_QUEUE_TLS_H
#define EFFECTS_DECAL_MESSAGE_QUEUE_TLS_H

template<class t_message, long k_count> class t_message_queue;
class c_decal_message_union;

extern "C" { extern __declspec(thread) t_message_queue<c_decal_message_union, 256> *g_decal_message_queue; }

#endif
