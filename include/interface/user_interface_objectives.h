#ifndef INTERFACE_USER_INTERFACE_OBJECTIVES_H
#define INTERFACE_USER_INTERFACE_OBJECTIVES_H

// Functions defined in tu/interfacelibxbox/user_interface_objectives.cpp.

template<long k_maximum_count> class c_static_wchar_string;
void user_interface_build_primary_objectives_text(c_static_wchar_string<256> *text);
void user_interface_build_secondary_objectives_text(c_static_wchar_string<256> *text);

void __tls_set_g_objectives_allocator(void *allocation);
extern "C" void __tls_set_g_objectives_state_allocator(void *address);
long primary_objectives_string_id_from_index(long index);
long secondary_objectives_string_id_from_index(long index);

// Named allocator object defined in user_interface_objectives.cpp.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_objectives_allocator> g_objectives_allocator;

extern "C" __declspec(thread) signed char *g_objectives_state;
extern "C" const wchar_t k_objectives_line_break[];
#endif // INTERFACE_USER_INTERFACE_OBJECTIVES_H
