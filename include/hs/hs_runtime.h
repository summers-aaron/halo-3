#include "hs/hs_runtime_data.h"
#ifndef HS_HS_RUNTIME_H
#define HS_HS_RUNTIME_H

#include "hs/hs_script_types.h"
#include "hs/hs_thread_types.h"
#include "hs/hs_value_types.h"

enum e_hs_destination_type {
    _hs_destination_none,
    _hs_destination_thread_stack,
    _hs_destination_global,
    _hs_destination_thread_result
};

struct hs_destination_pointer {
    short destination_type;
    short stack_offset;
};

struct hs_global_runtime {
    short identifier;
    short type;
    hs_value value;
};

struct hs_stack_frame {
    long expression_index;
    long script_index;
    hs_destination_pointer destination;
    short parameters_offset;
    short previous;
    short size;
};

// Functions defined in tu/blamlibxbox/hs_runtime.cpp.

struct s_hs_thread_iterator;

extern "C" void hs_wake(long thread_index, long waking_thread_index);
void hs_return(long thread_index, long value);
void hs_thread_delete(long thread_index, bool internal);
// match: 0xd4560: __restrict iterator keeps the field stores from reloading through an alias
void hs_thread_iterator_new(s_hs_thread_iterator *__restrict iterator);
void hs_runtime_update(void);
void hs_runtime_initialize();
void hs_runtime_dispose_from_old_map();
long *hs_arguments_evaluate(long thread_index, short formal_parameter_count, const short *formal_parameters, bool initialize);
void hs_runtime_initialize_for_new_map();
void hs_scripting_kill_running_thread(long thread_index);
void __tls_set_g_hs_thread_non_deterministic_data_allocator(void *object);
void __tls_set_g_hs_runtime_globals_allocator(void *allocation);
long hs_find_thread_by_name(const char *name);
extern "C" long hs_runtime_internal_evaluate(long expression_index);
extern "C" long hs_thread_iterator_next(s_hs_thread_iterator *iterator);
extern "C" long hs_runtime_evaluate(long expression_index, bool, bool);
extern "C" void hs_scripting_kill_all_threads();
void hs_thread_main(long thread_index);
long hs_cast(long thread, short actual_type, short desired_type, long value);
void __tls_set_g_hs_thread_tracking_data_allocator(void *allocation);
void __tls_set_g_hs_global_data_allocator(void *allocation);
void __tls_set_g_hs_distributed_global_data_allocator(void *allocation);
long hs_global_evaluate(short global_index);
extern "C" char *hs_stack_allocate(long thread_index, unsigned int size, __int64 alignment_bits, short *out_offset);
extern "C" void hs_initialize_type_conversion_table();
void hs_global_reconcile_read(short global_index);
long hs_find_thread_by_script(short script_index);
void hs_script_evaluate(short script_index, long thread_index, bool initialize);
long hs_thread_new(e_hs_thread_type type, long script_index, bool deterministic);
long hs_runtime_script_begin(short script_index, e_hs_script_type script_type, e_hs_thread_type thread_type);

struct hs_enum_definition
{
    // Both builds store the count as a big-endian short (game difficulty reads
    // `00 04 00 00`), not a long; identifiers stays at offset 4 either way.
    short count;
    const char **identifiers;
};
typedef char hs_enum_definition_size[(sizeof(void *) != 4 || sizeof(hs_enum_definition) == 8) ? 1 : -1];
typedef void (*hs_inspect_procedure)(short type, long value, char *buffer, long buffer_size);
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;

extern const hs_enum_definition hs_enum_table[];
bool hs_evaluate(long thread_index, long expression_index, hs_destination_pointer destination_pointer, long *out_value);
extern "C" long *hs_destination_get_pointer(hs_thread *thread, hs_destination_pointer destination);
extern "C" hs_inspect_procedure k_hs_inspect_functions[];
extern "C" const short k_hs_value_type_sizes[];
extern "C" long (*g_hs_typecasting_table[83][83])(long);
extern bool debug_globals;
extern bool debug_scripting;
void __tls_set_g_prop_ref_data_allocator(void *object);
extern "C" void hs_thread_set_nondet_data_base(void *allocation);
extern t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_prop_ref_data_allocator> g_hs_thread_deterministic_data_allocator;
extern t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_thread_tracking_data_allocator> g_hs_thread_tracking_data_allocator;
extern t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &hs_thread_set_nondet_data_base> g_hs_thread_non_deterministic_data_allocator;
extern t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_global_data_allocator> g_hs_global_data_allocator;
extern t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_distributed_global_data_allocator> g_hs_distributed_global_data_allocator;
extern t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_runtime_globals_allocator> g_hs_runtime_globals_allocator;
long hs_data_to_void(long value);
extern "C" bool g_hs_set_inspect_override;


#endif // HS_HS_RUNTIME_H
