#ifndef EFFECTS_EFFECT_MESSAGE_QUEUE_H
#define EFFECTS_EFFECT_MESSAGE_QUEUE_H

#include "effects/effect_message.h"
#include "multithreading/message_queue.h"

// effects.obj owns this specialization; callers must not instantiate the primary template.
template<> bool t_message_queue<s_effect_message, 512>::try_add_message(const s_effect_message *new_message);

#endif // EFFECTS_EFFECT_MESSAGE_QUEUE_H
