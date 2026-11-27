#ifndef EFFECTS_EFFECT_MESSAGE_QUEUE_TLS_H
#define EFFECTS_EFFECT_MESSAGE_QUEUE_TLS_H

extern "C" {
extern __declspec(thread) void *g_effect_message_queue;
}

#endif
