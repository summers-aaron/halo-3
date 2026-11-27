#ifndef AI_AI_ORDERS_H
#define AI_AI_ORDERS_H

extern "C" { extern __declspec(thread) int *g_bink_prediction_frames; }

// Functions defined in tu/ailibxbox/ai_orders.cpp.

void __tls_set_g_trigger_activations_allocator(void *allocator);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_trigger_activations_allocator> g_trigger_activations_allocation;

#endif // AI_AI_ORDERS_H
