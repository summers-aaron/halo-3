// blamlibxbox/hs_runtime.obj: from the records 0xaacd8 0xaada8 0xaaf90 0xab1d8 0xab278 0xab580 0xab6c8 0xab760 0xab8a0 0xaba10 0xabb28 0xabbb8 0xd43f0 0xd4560 0xd45d0 0xd4648 0xd4a60 0x111c60 0x120138 0x24b9e0 0x24b9f8 0x24ba10 0x24ba28 0x24ba50 0x24bd38 0x24bf70 0x24c1e8 0x24c430 0x24c510 0x24c7f8 0x24c8c0 0x24c930 0x24c970 0x24c9b0 0x24c9e8 0x24ca38 0x24cab8 0x24cc40 0x24ce10 0x24cfe8 0x24d098 0x24d160 0x24d250 0x24d2f0 0x24d348 0x24d840 0x24d940 0x24dac0 0x24dc70 0x24dd40 0x24de30 0x24de50 0x24de78 0x24deb0 0x24ded8 0x24def8 0x24df28 0x24df50 0x24df70 0x24df80 0x24df98 0x24e020 0x24e078 0x24e1f8 0x24e598 0x24e988 0x24e9f8 0x24ebc0 0x24ec60 0x24ec80 0x24ed20 0x24ed40 0x24ede0 0x24ee00 0x24eea0 0x24eec0 0x24ef60 0x24ef78 0x2b5e50 0x2b64a8 0x2bc4b8 0x2dc210 0x32fe48
// @address 0x11df66c g_hs_thread_deterministic_data
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x11df750 g_hs_runtime_globals
// @address 0x11df88c g_hs_thread_tracking_data
// @address 0x11df8c4 g_hs_thread_non_deterministic_data
// @address 0x11df7c8 g_hs_global_data
// @address 0x11df888 g_hs_distributed_global_data
// @address 0x24e988 ?hs_scripting_kill_running_thread@@YAXJ@Z
// @address 0x24dd40 ?hs_find_thread_by_name@@YAJPBD@Z
// @address 0x32fe48 ?__tls_set_g_hs_runtime_globals_allocator@@YAXPAX@Z

extern "C" {
    #include "objects/object_scripting_tls.h"
}


#include "hs/hs_thread_deterministic_data.h"
#include "objects/object_name_list.h"
#include "objects/object_list_header_types.h"
#include "objects/list_reference_types.h"
#include "ai/cl_engine.h"
#include "memory/tls_data_arrays.h"
#include "hs/hs_types.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "ai/actor_globals.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "simulation/simulation.h"
#include "ai/ai_script.h"
#include "hs/hs.h"
#include "scenario/scenario_tag.h"
#include "hs/hs_runtime.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "hs/object_lists.h"
#include "structures/structure_seams.h"
#include "memory/data.h"
#include "cseries/cseries.h"
#include "cache/restricted_memory_layout.h"
#include "hs/hs_thread_iterator_types.h"
#include "hs/hs_function.h"
#include "hs/hs_value_types.h"
#include "hs/hs_thread_types.h"
#include "hs/hs_runtime_data.h"
#include "effects/particle_location.h"
#include "memory/tls_globals.h"
#include "ai/actor_datum.h"
#include "ai/command_script_data.h"
#include "cache/restricted_memory.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "ai/command_script_types.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"
#include "hs/hs_script_types.h"
#include "hs/hs_function_definition.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "game/game_time_inline.h"
#include "game/game_simulation_predicates.h"
#include "tag_files/tag_groups.h"
#include "hs/object_list_inlines.h"
#include "objects/object_header_access.h"
#include "cseries/pointer_utilities.h"

namespace {
template<typename datum_type>
class c_hs_data_iterator {
public:
    __forceinline void begin(s_data_array *data) {
        m_iterator.absolute_index = -1;
        m_iterator.index = -1;
        m_iterator.data = data;
    }
    __forceinline bool next() {
        m_datum = (datum_type *)::data_iterator_next(&m_iterator);
        return m_datum != 0;
    }
    __forceinline datum_type *get_datum() { return m_datum; }
    __forceinline long get_index() { return m_iterator.index; }
private:
    datum_type *m_datum;
    s_data_iterator m_iterator;
};
}

namespace {
static __forceinline hs_thread *hs_runtime_thread_get(long thread_index)
{
    hs_thread *result = &((hs_thread *)g_hs_thread_deterministic_data->data)[datum_index_to_absolute_index(thread_index)];
    return result;
}
}


namespace {
static __forceinline void hs_runtime_data_make_invalid(s_data_array *data) {
    data->valid = false;
}
static __forceinline void hs_runtime_delete_internal_global_datums() {
    c_hs_data_iterator<hs_global_runtime> iterator;
    iterator.begin(tls_hs_global_data());
    while (iterator.next()) {
        if ((datum_index_to_absolute_index(iterator.get_index())) >= 1262)
            ::datum_delete(tls_hs_global_data(), iterator.get_index());
    }
}
static __forceinline void hs_runtime_cs_handle_thread_delete(long thread_index) {
    if (tls_actor_globals()->enabled) {
        c_hs_data_iterator<command_script_datum> iterator;
        iterator.begin((s_data_array *)tls_command_script_data());
        while (iterator.next()) {
            command_script_datum *script = iterator.get_datum();
            if (script->thread_index == thread_index)
                script->thread_index = -1;
        }
    }
}
static __forceinline long hs_runtime_global_index_to_datum_index(long global_index) {
    if (global_index & 0x8000)
        return global_index & 0x7fff;
    else
        return (global_index & 0x7fff) + 1262;
}
static __forceinline hs_global_runtime *hs_runtime_global_get(long global_index) {
    long absolute_index = hs_runtime_global_index_to_datum_index(global_index);
    return &((hs_global_runtime *)tls_hs_global_data()->data)[absolute_index];
}
}

// @address 0xaada8 ?hs_thread_new@@YAJW4e_hs_thread_type@@J_N@Z

// @address 0xab1d8 ?hs_thread_format@@YAPBDJ@Z

// @address 0xabb28 ?hs_thread_delete@@YAXJ_N@Z



// @address 0x24cfe8 ?hs_runtime_dispose_from_old_map@@YAXXZ

// @address 0x24ef60 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_hs_runtime_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z

// grown: 0xd45d0 0x24c8c0 0x24ca38 0x24d840 0x24d940 0x24de50 0x24de78 0x24deb0 0x24def8 0x24df28 0x24df50 0x24df70





// @address 0x24c930 ?hs_inspect_real@@YAXFJPADJ@Z

// @address 0x24c9b0 ?hs_inspect_long_integer@@YAXFJPADJ@Z

static __forceinline hs_thread *hs_runtime_script_thread_get(long thread_index)
{
    return &((hs_thread *)g_hs_thread_deterministic_data->data)[datum_index_to_absolute_index(thread_index)];
}

static __forceinline hs_stack_frame *hs_runtime_script_frame(hs_thread *thread, short offset)
{
    return (hs_stack_frame *)(thread->stack + offset);
}

static __forceinline void hs_runtime_script_pop_frame(long thread_index)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    thread->stack_offset = hs_runtime_script_frame(thread, thread->stack_offset)->previous;
}

static __forceinline bool hs_runtime_script_node_is_type_21(int node_index)
{
    return ((hs_syntax_node *)g_hs_syntax_data->data)[datum_index_to_absolute_index(node_index)].constant_type == 0x15;
}

static __forceinline long hs_runtime_long_to_boolean(long value)
{
    hs_value result;
    result.long_value = 0;
    result.boolean_value = !value;
    return result.long_value;
}

// @address 0x24e1f8 ?hs_global_reconcile_read@@YAXF@Z

// natural: 0xd45d0 (grown)


static __forceinline hs_syntax_node *hs_runtime_syntax_get(long index)
{
    return &((hs_syntax_node *)g_hs_syntax_data->data)[datum_index_to_absolute_index(index)];
}

static __forceinline float hs_runtime_ticks_to_seconds(long ticks)
{
    return (float)ticks / 30.0f;
}




extern "C" {
// @address 0x7ead10 g_hs_typecasting_table
long (*g_hs_typecasting_table[83][83])(long);
}

// @address 0x7ead0a debug_globals
bool debug_globals;
// @address 0x7ead0b debug_scripting
bool debug_scripting;
static __forceinline long hs_runtime_object_name_to_object(long thread, short name)
{
    long result;
    if (name >= 0 && name < 2048)
        result = ((long *)g_object_name_list)[name];
    else result = -1;
    return result;
}
// @address 0xaacd8 hs_runtime_internal_evaluate
extern "C" long hs_runtime_internal_evaluate(long expression_index)
{
    long result = -1;
    if (g_hs_runtime_globals->initialized && expression_index != -1) {
        long thread_index = hs_thread_new(_hs_thread_type_runtime_evaluate, -1, true);
        if (thread_index != -1) {
            hs_thread *thread = hs_runtime_thread_get(thread_index);
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 3;
            hs_evaluate(thread_index, expression_index, destination, 0);
            if (thread->flags & 1)
                hs_thread_main(thread_index);
            result = thread->result;
            hs_thread_delete(thread_index, true);
        }
    }
    return result;
}

long hs_thread_new(e_hs_thread_type type, long script_index, bool deterministic)
{
    long thread_index = datum_new((s_data_array *)g_hs_thread_deterministic_data);
    if (thread_index != -1) {
        hs_thread *thread = hs_runtime_thread_get(thread_index);
        thread->stack_offset = 0;
        hs_stack_frame *frame = (hs_stack_frame *)thread->stack;
        memset(frame, 0, sizeof(*frame));
        frame->previous = -1;
        frame->parameters_offset = -1;
        frame->script_index = -1;
        frame->expression_index = -1;
        thread->type = (unsigned char)type;
        thread->script_index = (short)script_index;
        thread->cleanup_script_index = -1;
        thread->flags = 0;
        thread->thread_index = thread_index;
        thread->ai_index = 0;
        thread->flags2 = 0;
        thread->abort_combat_status = 0;
        if (script_index != -1 && ((s_hs_script_definition *)global_scenario_get()->scripts.address)[script_index].type == 1)
            thread->sleep_until = -2;
        else
            thread->sleep_until = 0;
    }
    return thread_index;
}

// @address 0xaaf90 ?hs_evaluate@@YA_NJJUhs_destination_pointer@@PAJ@Z
namespace {
static __forceinline hs_thread *hs_runtime_evaluate_thread_get(long thread_index)
{
    return &((hs_thread *)g_hs_thread_deterministic_data->data)[datum_index_to_absolute_index(thread_index)];
}
static __forceinline hs_stack_frame *hs_runtime_evaluate_stack_frame(hs_thread *thread, short offset)
{
    return (hs_stack_frame *)(thread->stack + offset);
}
static __forceinline bool hs_runtime_stack_push(long thread_index)
{
    hs_thread *thread = hs_runtime_evaluate_thread_get(thread_index);
    bool result = false;
    short stack_offset = thread->stack_offset + hs_runtime_evaluate_stack_frame(thread, thread->stack_offset)->size + sizeof(hs_stack_frame);
    if (stack_offset + sizeof(hs_stack_frame) <= sizeof(thread->stack)) {
        hs_stack_frame *frame = hs_runtime_evaluate_stack_frame(thread, stack_offset);
        result = true;
        memset(frame, 0, sizeof(hs_stack_frame));
        frame->previous = thread->stack_offset;
        thread->stack_offset = stack_offset;
        frame->parameters_offset = -1;
        frame->script_index = -1;
        frame->expression_index = -1;
    } else {
        thread->stack_offset = 0;
        thread->flags |= 0x40;
    }
    return result;
}
}

bool hs_evaluate(long thread_index, long expression_index, hs_destination_pointer destination_pointer, long *out_value)
{
    hs_thread *thread = hs_runtime_evaluate_thread_get(thread_index);
    hs_syntax_node *expression = hs_runtime_syntax_get(expression_index);
    long value = -1;
    bool result = true;
    if (hs_runtime_syntax_get(expression_index)->flags & 1) {
        if (expression->flags & 4) {
            if (expression->flags & 0x10) {
                hs_stack_frame *frame;
                for (frame = hs_runtime_evaluate_stack_frame(thread, thread->stack_offset); frame;
                    frame = hs_runtime_evaluate_stack_frame(thread, frame->previous)) {
                    if (frame->script_index != -1) {
                        s_hs_script_definition *script = &((s_hs_script_definition *)global_scenario_get()->scripts.address)[frame->script_index];
                        s_hs_script_parameter *parameter = (s_hs_script_parameter *)tag_block_get_element_with_size(
                            &script->parameters, expression->value, sizeof(s_hs_script_parameter));
                        long *parameters = (long *)hs_runtime_evaluate_stack_frame(thread, frame->parameters_offset);
                        value = hs_cast(thread_index, parameter->type, expression->type, parameters[expression->value]);
                        break;
                    }
                }
            } else {
                long global_value = hs_global_evaluate((unsigned long)expression->value);
                value = hs_cast(thread_index, hs_global_get_type((unsigned long)expression->value), expression->type, global_value);
            }
        } else {
            value = hs_cast(thread_index, expression->constant_type, expression->type, expression->value);
        }
        long *destination = hs_destination_get_pointer((hs_thread *)thread, destination_pointer);
        if (destination)
            *destination = value;
    } else {
        hs_runtime_evaluate_stack_frame(thread, thread->stack_offset)->destination = destination_pointer;
        result = hs_runtime_stack_push(thread_index);
        thread->flags |= 1;
        hs_runtime_evaluate_stack_frame(thread, thread->stack_offset)->expression_index = expression_index;
    }
    if (out_value)
        *out_value = value;
    return result;
}

const char *hs_thread_format(long thread_index)
{
    hs_thread *thread = hs_runtime_thread_get(thread_index);
    long type = thread->type;
    switch (type) {
    case _hs_thread_type_script:
    case _hs_thread_type_runtime_evaluate:
    case _hs_thread_type_command_script:
        {
            long script_index = hs_runtime_thread_get(thread_index)->script_index;
            if (script_index != -1)
                return ((s_hs_script_definition *)global_scenario_get()->scripts.address)[script_index].name;
            else
                return "[unknown thread type]";
        }
    case _hs_thread_type_global_initialize:
        return "[global initialize]";
    case _hs_thread_type_console:
        return "[console command]";
    default:
        return "[unknown thread type]";
    }
}

namespace {
static __forceinline long hs_runtime_syntax_get_nth(long expression_index, short argument_index)
{
    while (argument_index > 0) {
        expression_index = hs_runtime_syntax_get(expression_index)->next_node_index;
        argument_index--;
    }
    return expression_index;
}
}
// @address 0xab580 ?hs_evaluate_begin@@YAXFJ_N@Z
void hs_evaluate_begin(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short result_offset;
    long *expression_index = (long *)hs_stack_allocate(thread_index, 4, 2, 0);
    long *result = (long *)hs_stack_allocate(thread_index, 4, 2, &result_offset);
    if (expression_index && result) {
        if (initialize) {
            *expression_index = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_script_frame(thread,
                thread->stack_offset)->expression_index)->value)->next_node_index;
            *result = 0;
        }
        if (*expression_index != -1) {
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            destination.stack_offset = result_offset;
            hs_evaluate(thread_index, *expression_index, destination, 0);
            *expression_index = hs_runtime_syntax_get(*expression_index)->next_node_index;
        } else {
            hs_return(thread_index, *result);
        }
    }
}

// natural: 0xab6c8


static __forceinline hs_thread *hs_runtime_main_thread_get(long thread_index)
{
    return (hs_thread *)hs_runtime_evaluate_thread_get(thread_index);
}
static __forceinline hs_stack_frame *hs_runtime_main_frame_get(hs_thread *thread)
{
    return (hs_stack_frame *)(thread->stack + thread->stack_offset);
}

// @address 0xab278 ?hs_thread_main@@YAXJ@Z
void hs_thread_main(long thread_index)
{
    hs_thread *thread = hs_runtime_main_thread_get(thread_index);
    scenario *scenario = global_scenario_get();
    s_hs_script_definition *script = 0;
    short offset;
    hs_destination_pointer destination;
    tls_hs_runtime_globals()->executing_thread_index = thread_index;
    if (thread->script_index != -1)
        script = &((s_hs_script_definition *)scenario->scripts.address)[thread->script_index];
    thread->sleep_until = 0;
    if (thread->stack_offset == 0) {
        hs_runtime_main_frame_get(thread)->size = 0;
        if (hs_stack_allocate(thread_index, 4, 2, &offset)) {
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            destination.stack_offset = offset;
            hs_evaluate(thread_index, script->root_expression, destination, 0);
            scenario = global_scenario_get();
        }
    }
    if (thread->stack_offset != 0) {
        while (thread->stack_offset != 0 && thread->sleep_until >= 0 &&
            (!simulation_starting_up() || thread->sleep_until <= game_time_get()) &&
            !(thread->flags & 64) && tls_hs_runtime_globals()->initialized) {
            hs_syntax_node *expression = &(
                (hs_syntax_node *)g_hs_syntax_data->data)[datum_index_to_absolute_index(hs_runtime_main_frame_get(thread)->expression_index)];
            bool initialize = (thread->flags & 1) != 0;
            hs_runtime_main_frame_get(thread)->size = 0;
            thread->flags &= ~1;
            if (!(expression->flags & 2)) {
                hs_function_table[expression->constant_type]->evaluate(expression->constant_type, thread_index, initialize);
            } else {
                hs_script_evaluate(expression->constant_type, thread_index, initialize);
            }
        }
        scenario = global_scenario_get();
    }
    hs_runtime_globals *globals = tls_hs_runtime_globals();
    if (globals->require_object_list_gc && !globals->globals_initializing) {
        object_lists_dispose_empty_lists();
        globals->require_object_list_gc = false;
    }
    if (thread->stack_offset == 0 || (thread->flags & 64)) {
        bool finished = true;
        if (thread->flags2 & 16) {
            short saved_script_index = thread->script_index;
            thread->script_index = thread->cleanup_script_index;
            thread->cleanup_script_index = saved_script_index;
            script = thread->script_index != -1 ? &((s_hs_script_definition *)scenario->scripts.address)[thread->script_index] : 0;
        } else if (thread->cleanup_script_index != -1) {
            s_hs_script_definition *cleanup = &((s_hs_script_definition *)scenario->scripts.address)[thread->cleanup_script_index];
            if (cleanup->type == 3 && cleanup->parameters.count <= 0) {
                short saved_script_index = thread->script_index;
                thread->script_index = thread->cleanup_script_index;
                thread->cleanup_script_index = saved_script_index;
                thread->stack_offset = 0;
                thread->flags &= ~64;
                thread->flags |= 128;
                thread->flags2 |= 16;
                finished = false;
            }
        }
        if (finished) {
            if (thread->type == 0) {
                if ((short)script->type == 0 || (short)script->type == 1) {
                    thread->sleep_until = -1;
                }
            } else if (thread->type == 4) {
                hs_thread_delete(thread_index, true);
            } else if (thread->type == 2) {
                hs_thread_delete(thread_index, true);
            }
        }
    }
    globals->executing_thread_index = -1;
}

// @address 0xab6c8 hs_destination_get_pointer
extern "C" long *hs_destination_get_pointer(hs_thread *thread, hs_destination_pointer destination)
{
    switch (destination.destination_type)
    {
    case _hs_destination_none:
        return 0;
    case _hs_destination_thread_stack:
        return (long *)(thread->stack + destination.stack_offset);
    case _hs_destination_global:
        return &hs_runtime_global_get(destination.stack_offset)->value.long_value;
    case _hs_destination_thread_result:
        return &thread->result;
    default:
        // match: 0xab6c8: unreachable hint omits dead branch check
        __assume(0);
    }
}


// @rdata 0x82034174..0x82034188
static const short k_hs_object_cast_type_masks[10] =
{
    -1, 0x1003, 2, 4, 0x190, 0x40, 0x2000, 0, 0, 0
};
// @rdata-end

// @address 0x341c8 k_hs_value_type_sizes
#if !defined(_XBOX) || defined(__clang__)
const short k_hs_value_type_sizes[k_hs_type_count] =
{
    0, 0, 0, 0, 0, 1, 4, 2, 4, 4, 2, 4, 4, 2, 2, 2, 2, 2, 4, 4, 2, 2, 2, 2, 4, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 2, 2,
    2, 2, 2, 2, 2, 4, 4, 4, 4
};
#endif

// @address 0xab760 ?hs_evaluate_equality@@YAXFJ_N@Z
void hs_evaluate_equality(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short formal_parameters[2];
    formal_parameters[0] = formal_parameters[1] = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_syntax_get(
        hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)->next_node_index)->type;
    long *arguments = hs_arguments_evaluate(thread_index, 2, formal_parameters, initialize);
    if (arguments)
    {
        hs_value result;
        result.long_value = 0;
        bool equal = memcmp(arguments, arguments + 1, k_hs_value_type_sizes[formal_parameters[0]]) == 0;
        if (function_index == _hs_function_not_equal)
            equal = !equal;
        result.boolean_value = equal;
        hs_return(thread_index, result.long_value);
    }
}

// @address 0xab8a0 ?hs_return@@YAXJJ@Z
void hs_return(long thread_index, long value)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    hs_stack_frame *frame = hs_runtime_script_frame(thread, thread->stack_offset);
    hs_syntax_node *expression = hs_runtime_syntax_get(frame->expression_index);
    short actual_type;
    if ((expression->flags & 2) == 0) {
        actual_type = hs_function_table[expression->constant_type]->return_type;
    } else if ((short)expression->constant_type != -1) {
        actual_type = ((s_hs_script_definition *)global_scenario_get()->scripts.address)[expression->constant_type].return_type;
    } else {
        actual_type = 4;
    }
    hs_stack_frame *previous = hs_runtime_script_frame(thread, frame->previous);
    long *destination = hs_destination_get_pointer((hs_thread *)thread, previous->destination);
    if (destination) {
        *destination = hs_cast(thread_index, actual_type, expression->type, value);
    }
    hs_runtime_script_pop_frame(thread_index);
}

// @address 0xaba10 ?hs_cast@@YAJJFFJ@Z
long hs_cast(long thread, short actual_type, short desired_type, long value)
{
    long result = 0;
    if (actual_type == desired_type || actual_type == 3)
        result = value;
    else if (desired_type >= 72 && desired_type <= 78)
        result = value;
    else if (desired_type >= 65 && desired_type <= 71)
    {
        long object_index = -1;
        if (actual_type >= 72 && actual_type <= 78)
        {
            hs_value name;
            name.long_value = value;
            object_index = hs_runtime_object_name_to_object(thread, (short)name.short_value);
            result = object_index;
        }
        else if (actual_type == 19)
        {
            object_index = object_index_from_ai(value, (e_hs_type)desired_type);
            result = object_index;
        }
        else
            result = value;
        if (object_index != -1)
        {
            object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
            object->flags |= 1UL << 28;
        }
    }
    else
        result = g_hs_typecasting_table[desired_type][actual_type](value);
    return result;
}

void hs_thread_delete(long thread_index, bool internal)
{
    hs_runtime_cs_handle_thread_delete(thread_index);
    ::datum_delete((s_data_array *)g_hs_thread_deterministic_data, thread_index);
}

// @address 0xd4560 ?hs_thread_iterator_new@@YAXPIAUs_hs_thread_iterator@@@Z
static __forceinline hs_syntax_node *hs_runtime_set_syntax_get(long index)
{
    return (hs_syntax_node *)hs_runtime_syntax_get(index);
}

// @address 0xabbb8 ?hs_evaluate_inequality@@YAXFJ_N@Z

void hs_evaluate_inequality(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_evaluate_thread_get(thread_index);
    hs_stack_frame *block = (hs_stack_frame *)(thread->stack + thread->stack_offset);
    hs_syntax_node *expression = hs_runtime_set_syntax_get(block->expression_index);
    expression = hs_runtime_set_syntax_get(expression->value);
    expression = hs_runtime_set_syntax_get(expression->next_node_index);
    short type = expression->type;
    short parameter_types[2];
    parameter_types[1] = type;
    parameter_types[0] = parameter_types[1];
    const hs_value *arguments = (const hs_value *)hs_arguments_evaluate(thread_index, 2, parameter_types, initialize);
    if (arguments) {
        bool result = false;
        hs_value value;
        value.long_value = 0;
        switch (type) {
       
        case _hs_type_real: {
            float a = arguments[0].real_value;
            float b = arguments[1].real_value;
            switch (function_index) {
            case _hs_function_greater: result = a > b; break;
            case _hs_function_less: result = a < b; break;
            case _hs_function_greater_or_equal: result = a >= b; break;
            case _hs_function_less_or_equal: result = a <= b; break;
            default: result = false; break;
            }
            break;
        }
        case _hs_type_long_integer: {
            float a = (float)arguments[0].long_value;
            float b = (float)arguments[1].long_value;
            switch (function_index) {
            case _hs_function_greater: result = a > b; break;
            case _hs_function_less: result = a < b; break;
            case _hs_function_greater_or_equal: result = a >= b; break;
            case _hs_function_less_or_equal: result = a <= b; break;
            default: result = false; break;
            }
            break;
        }
        default: {
            float a = (float)(short)arguments[0].short_value;
            float b = (float)(short)arguments[1].short_value;
            switch (function_index) {
            case _hs_function_greater: result = a > b; break;
            case _hs_function_less: result = a < b; break;
            case _hs_function_greater_or_equal: result = a >= b; break;
            case _hs_function_less_or_equal: result = a <= b; break;
            default: result = false; break;
            }
            break;
        }
        }
        value.boolean_value = result;
        hs_return(thread_index, value.long_value);
    }
}

// match: 0xd4560: pointer __restrict qualifier prevents alias reload
void hs_thread_iterator_new(s_hs_thread_iterator *__restrict iterator)
{
    if (g_hs_runtime_globals->initialized) {
        iterator->active_only = true;
        iterator->include_remote = true;
        iterator->index = -1;
        iterator->finished = false;
        iterator->index = ::data_next_index((s_data_array *)g_hs_thread_deterministic_data, iterator->index);
    }
}

// @address 0xd45d0 hs_thread_iterator_next
extern "C" long hs_thread_iterator_next(s_hs_thread_iterator *iterator)
{
    long result = -1;
    if (g_hs_runtime_globals->initialized && iterator->index != -1) {
        result = iterator->index;
        iterator->index = ::data_next_index((s_data_array *)g_hs_thread_deterministic_data, iterator->index);
    }
    return result;
}

static __forceinline hs_thread *hs_runtime_update_thread_get(long thread_index)
{
    return &((hs_thread *)g_hs_thread_deterministic_data->data)[datum_index_to_absolute_index(thread_index)];
}
static __forceinline actor_datum *hs_runtime_update_actor_get(long actor_index)
{
    return &((actor_datum *)tls_actor_data()->data)[datum_index_to_absolute_index(actor_index)];
}
static __forceinline void hs_runtime_update_clear_script_context()
{
    global_script_context.ai_index = 0;
    global_script_context.object_index = 0;
}
static __forceinline void hs_runtime_update_setup_script_context(long thread_index)
{
    hs_thread *thread = hs_runtime_update_thread_get(thread_index);
    hs_runtime_update_clear_script_context();
    if (thread->type == 4) {
        long ai_index = thread->ai_index;
        long actor_index = ::ai_index_get_actor(ai_index);
        if (actor_index != -1) {
            actor_datum *actor = hs_runtime_update_actor_get(actor_index);
            global_script_context.ai_index = ai_index;
            global_script_context.object_index = actor->squad_index != -1 ? ((
                unsigned short)actor->squad_index | 0x20000000) : 0;
        }
    }
}
// @address 0xd43f0 ?hs_runtime_update@@YAXXZ
void hs_runtime_update(void)
{
    if (g_hs_runtime_globals->initialized) {
        long time = tls_game_time_globals()->time;
        bool has_runtime_evaluate_threads = false;
        s_hs_thread_iterator iterator;
        // match: 0xd43f0: pointer __restrict qualifier prevents alias reload
        ((void (*)(s_hs_thread_iterator *__restrict, bool, bool))hs_thread_iterator_new)(&iterator, true, true);
        long thread_index;
        while ((thread_index = hs_thread_iterator_next(&iterator)) != -1) {
            hs_thread *thread = hs_runtime_update_thread_get(thread_index);
            bool run = true;
            if (thread->sleep_until == -3 && !cs_blocked(thread_index))
                thread->sleep_until = 0;
            switch (thread->type) {
            case 4: hs_runtime_update_setup_script_context(thread_index); break;
            case 2: has_runtime_evaluate_threads = true; break;
            default: run = true; break;
            }
            if (run && thread->sleep_until >= 0 && thread->sleep_until <= time)
                hs_thread_main(thread_index);
            hs_runtime_update_clear_script_context();
        }
        object_lists_dispose_empty_lists();
    }
}


// @address 0xd4a60 ?hs_evaluate_logical@@YAXFJ_N@Z
void hs_evaluate_logical(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short operand_offset;
    long *expression = (long *)hs_stack_allocate(thread_index, 4, 2, 0);
    hs_value *operand = (hs_value *)hs_stack_allocate(thread_index, 4, 2, &operand_offset);
    bool *accumulator = (bool *)hs_stack_allocate(thread_index, 1, 0, 0);
    if (expression && operand && accumulator) {
        bool is_and = function_index == 5;
        if (initialize) {
            *expression = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_script_frame(thread,
                thread->stack_offset)->expression_index)->value)->next_node_index;
            *accumulator = is_and;
        } else {
            bool operand_value = operand->boolean_value;
            if (is_and) *accumulator = *accumulator && operand_value;
            else *accumulator = *accumulator || operand_value;
        }
        if (*expression != -1 && *accumulator == is_and) {
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            destination.stack_offset = operand_offset;
            hs_evaluate(thread_index, *expression, destination, 0);
            *expression = hs_runtime_syntax_get(*expression)->next_node_index;
        } else {
            hs_value result;
            result.long_value = 0;
            result.boolean_value = *accumulator;
            hs_return(thread_index, result.long_value);
        }
    }
}

// @address 0x120138 ?hs_evaluate_object_cast_up@@YAXFJ_N@Z
void hs_evaluate_object_cast_up(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short value_offset;
    long *value = (long *)hs_stack_allocate(thread_index, 4, 2, &value_offset);
    if (value) {
        if (initialize) {
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            destination.stack_offset = value_offset;
            hs_evaluate(thread_index, hs_runtime_syntax_get(hs_runtime_syntax_get(
                hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)->next_node_index,
                destination, 0);
        } else if (*value != -1) {
            object_datum *object = (object_datum *)object_get_and_verify_type(*value, ~0UL);
            short object_type = function_index - 23;
            if (k_hs_object_cast_type_masks[object_type] & (1 << object->type))
                hs_return(thread_index, *value);
            else
                hs_return(thread_index, -1);
        } else {
            hs_return(thread_index, -1);
        }
    }
}

// @address 0x24b9e0 ?__tls_set_g_hs_thread_tracking_data_allocator@@YAXPAX@Z
void __tls_set_g_hs_thread_tracking_data_allocator(void *allocation)
{
    data_set_new_base_address(&tls_hs_thread_tracking_data(), (s_data_array *)allocation);
}

// @address 0x24b9f8 hs_thread_set_nondet_data_base
extern "C" void hs_thread_set_nondet_data_base(void *allocation)
{
    data_set_new_base_address(&tls_hs_thread_non_deterministic_data(), (s_data_array *)allocation);
}

// @address 0x24ba10 ?__tls_set_g_hs_global_data_allocator@@YAXPAX@Z
void __tls_set_g_hs_global_data_allocator(void *allocation)
{
    data_set_new_base_address(&tls_hs_global_data(), (s_data_array *)allocation);
}

// @address 0x24ba28 ?__tls_set_g_hs_distributed_global_data_allocator@@YAXPAX@Z
void __tls_set_g_hs_distributed_global_data_allocator(void *allocation)
{
    data_set_new_base_address(&tls_hs_distributed_global_data(), (s_data_array *)allocation);
}

// @address 0x24ba50 ?hs_evaluate_begin_random@@YAXFJ_N@Z
void hs_evaluate_begin_random(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short result_offset;
    short *argument_count = (short *)hs_stack_allocate(thread_index, 2, 1, 0);
    unsigned long *evaluated = (unsigned long *)hs_stack_allocate(thread_index, 4, 2, 0);
    long *result = (long *)hs_stack_allocate(thread_index, 4, 2, &result_offset);
    if (argument_count && evaluated && result) {
        if (initialize) {
            long expression_index = hs_runtime_syntax_get(
                hs_runtime_syntax_get(hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)
                                        ->next_node_index;
            *argument_count = 0;
            for (; expression_index != -1; (*argument_count)++)
                expression_index = hs_runtime_syntax_get(expression_index)->next_node_index;
            memset(evaluated, 0, ((*argument_count + 31) >> 5) * sizeof(long));
        }
        short first_argument = random_range(tls_random_seed(), 0, *argument_count);
        short argument_index;
        for (argument_index = 0; argument_index < *argument_count; argument_index++) {
            short selected_argument = (argument_index + first_argument) % *argument_count;
            if (!(evaluated[selected_argument >> 5] & (1 << (selected_argument & 31)))) {
                hs_destination_pointer destination;
                memset(&destination, 0, sizeof(destination));
                destination.destination_type = 1;
                destination.stack_offset = result_offset;
                hs_evaluate(thread_index, hs_runtime_syntax_get_nth(hs_runtime_syntax_get(hs_runtime_syntax_get(
                    hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)->next_node_index,
                    selected_argument), destination, 0);
                evaluated[selected_argument >> 5] |= 1 << (selected_argument & 31);
                break;
            }
        }
        if (argument_index == *argument_count)
            hs_return(thread_index, *result);
    }
}

// @address 0x24c1e8 ?hs_evaluate_arithmetic@@YAXFJ_N@Z
void hs_evaluate_arithmetic(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    unsigned short *argument_index = (unsigned short *)hs_stack_allocate(thread_index, 2, 1, 0);
    long *expression_index = (long *)hs_stack_allocate(thread_index, 4, 2, 0);
    short value_offset;
    float *value = (float *)hs_stack_allocate(thread_index, 4, 2, &value_offset);
    float *result = (float *)hs_stack_allocate(thread_index, 4, 2, 0);
    if (argument_index && expression_index && value && result) {
        if (initialize) {
            *argument_index = 0;
            *expression_index = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_script_frame(thread,
                thread->stack_offset)->expression_index)->value)->next_node_index;
        } else {
            float argument_value = *value;
            if (*argument_index == 0)
                *result = argument_value;
            else {
                switch (function_index) {
                case _hs_function_add: *result = *result + argument_value; break;
                case _hs_function_subtract: *result = *result - argument_value; break;
                case _hs_function_multiply: *result = *result * argument_value; break;
                case _hs_function_divide: *result = *result / argument_value; break;
                case _hs_function_min: *result = *result > argument_value ? argument_value : *result; break;
                case _hs_function_max: *result = *result > argument_value ? *result : argument_value; break;
                }
            }
            (*argument_index)++;
        }
        if (*expression_index != -1) {
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            destination.stack_offset = value_offset;
            hs_evaluate(thread_index, *expression_index, destination, 0);
            *expression_index = hs_runtime_syntax_get(*expression_index)->next_node_index;
        } else {
            hs_value result_value;
            result_value.real_value = *result;
            hs_return(thread_index, result_value.long_value);
        }
    }
}

// @address 0x24c430 ?hs_evaluate_sleep_forever@@YAXFJ_N@Z
void hs_evaluate_sleep_forever(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    long expression_index = hs_runtime_syntax_get(
        hs_runtime_syntax_get(hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)
                                ->next_node_index;
    long sleep_thread_index = thread_index;
    if (expression_index != -1) {
        hs_value script_value;
        hs_destination_pointer destination;
        memset(&destination, 0, sizeof(destination));
        destination.destination_type = 0;
        hs_evaluate(thread_index, expression_index, destination, &script_value.long_value);
        short script_index = script_value.short_value;
        sleep_thread_index = hs_find_thread_by_script(script_index);
    }
    if (sleep_thread_index != -1) {
        hs_thread *sleep_thread = hs_runtime_script_thread_get(sleep_thread_index);
        sleep_thread->sleep_until = -2;
    }
    hs_return(thread_index, 0);
}

// @address 0x24c510 ?hs_evaluate_sleep@@YAXFJ_N@Z
void hs_evaluate_sleep(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short sleep_offset;
    short script_offset;
    short *sleep_ticks = (short *)hs_stack_allocate(thread_index, 4, 2, &sleep_offset);
    long *script_value = (long *)hs_stack_allocate(thread_index, 4, 2, &script_offset);
    unsigned short *argument_index = (unsigned short *)hs_stack_allocate(thread_index, 2, 1, 0);
    if (sleep_ticks && script_value && argument_index) {
        long sleep_thread_index = thread_index;
        if (initialize) {
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            destination.stack_offset = sleep_offset;
            hs_evaluate(thread_index,
                hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_script_frame(thread,
                    thread->stack_offset)->expression_index)->value)->next_node_index,
                destination, 0);
            *argument_index = 0;
        } else {
            if (*argument_index == 0) {
                long script_expression = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_syntax_get(
                    hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)->next_node_index)->next_node_index;
                (*argument_index)++;
                if (script_expression != -1) {
                    hs_destination_pointer destination;
                    memset(&destination, 0, sizeof(destination));
                    destination.destination_type = 1;
                    destination.stack_offset = script_offset;
                    hs_evaluate(thread_index, script_expression, destination, 0);
                    return;
                }
                *script_value = -1;
            }
            if (*argument_index != 0) {
                short script_index = *(short *)script_value;
                if (script_index != -1)
                    sleep_thread_index = hs_find_thread_by_script(script_index);
                if (sleep_thread_index != -1) {
                    long ticks = 0;
                    long hs_ticks = *sleep_ticks;
                    if (hs_ticks >= 0) {
                        float seconds = hs_runtime_ticks_to_seconds(hs_ticks);
                        ticks = game_seconds_to_ticks_round(seconds);
                    }
                    if (ticks > 0) {
                        hs_thread *sleep_thread = hs_runtime_script_thread_get(sleep_thread_index);
                        long sleep_until = game_time_get() + ticks;
                        if (sleep_thread->sleep_until != -1) {
                            if (sleep_thread_index != thread_index && (sleep_thread->flags & 2) == 0) {
                                sleep_thread->flags |= 2;
                                sleep_thread->previous_sleep_until = sleep_thread->sleep_until;
                            }
                            hs_runtime_script_thread_get(sleep_thread_index)->sleep_until = sleep_until;
                        }
                    }
                }
                hs_return(thread_index, 0);
            }
        }
    }
}

// @address 0x24c7f8 ?hs_evaluate_wake@@YAXFJ_N@Z
void hs_evaluate_wake(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    long expression_index = hs_runtime_syntax_get(
        hs_runtime_syntax_get(hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)
                                ->next_node_index;
    hs_syntax_node *script_name_node = hs_runtime_syntax_get(expression_index);
    long wake_thread_index = hs_find_thread_by_script(script_name_node->short_value);
    if (wake_thread_index != -1)
        hs_wake(wake_thread_index, thread_index);
    hs_return(thread_index, 0);
}

// @address 0x24c8c0 ?hs_inspect_boolean@@YAXFJPADJ@Z
void hs_inspect_boolean(short type, long value, char *buffer, long size)
{
    hs_value argument;
    argument.long_value = value;
    if (argument.boolean_value)
        csstrnzcpy(buffer, "true", size);
    else
        csstrnzcpy(buffer, "false", size);
}

// @address 0x24c970 ?hs_inspect_short_integer@@YAXFJPADJ@Z
void hs_inspect_short_integer(short type, long value, char *buffer, long buffer_size)
{
    hs_value argument;
    argument.long_value = value;
    csnzprintf(buffer, buffer_size, "%d", (short)argument.short_value);
}

void hs_inspect_real(short type, long value, char *buffer, long buffer_size)
{
    csnzprintf(buffer, buffer_size, "%f", ((hs_value *)&value)->real_value);
}

void hs_inspect_long_integer(short type, long value, char *buffer, long buffer_size)
{
    csnzprintf(buffer, buffer_size, "%ld", value);
}

// @address 0x24c9e8 ?hs_inspect_string@@YAXFJPADJ@Z
void hs_inspect_string(short type, long value, char *buffer, long buffer_size)
{
    hs_value argument;
    argument.long_value = value;
    csstrnzcpy(buffer, argument.string_value, buffer_size);
}

// @address 0x24ca38 ?hs_inspect_enum@@YAXFJPADJ@Z
void hs_inspect_enum(short type, long value, char *buffer, long buffer_size)
{
    hs_value argument;
    argument.long_value = value;
    strncpy(buffer, hs_enum_table[type - _hs_type_enum_first].identifiers[(short)argument.short_value], buffer_size);
    buffer[buffer_size - 1] = 0;
}

extern "C" {
// @address 0x744f90 k_hs_inspect_functions
hs_inspect_procedure k_hs_inspect_functions[83] =
{
    0, 0, 0, 0, 0, hs_inspect_boolean, hs_inspect_real, hs_inspect_short_integer, hs_inspect_long_integer, hs_inspect_string,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    hs_inspect_enum, hs_inspect_enum, hs_inspect_enum, hs_inspect_enum, hs_inspect_enum,
    hs_inspect_enum, hs_inspect_enum, hs_inspect_enum, hs_inspect_enum, hs_inspect_enum,
    hs_inspect_enum, hs_inspect_enum, hs_inspect_enum, hs_inspect_enum, hs_inspect_enum, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0
};
}

// @address 0x24cab8 ?hs_evaluate_inspect@@YAXFJ_N@Z
void hs_evaluate_inspect(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short value_offset;
    long *value = (long *)hs_stack_allocate(thread_index, 4, 2, &value_offset);
    if (value) {
        if (initialize) {
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            destination.stack_offset = value_offset;
            hs_evaluate(thread_index, hs_runtime_syntax_get(hs_runtime_syntax_get(
                hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)->next_node_index,
                destination, 0);
        } else {
            hs_syntax_node *expression = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_syntax_get(
                hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)->next_node_index);
            if ((thread->flags & 4) && k_hs_inspect_functions[(short)expression->type]) {
                char buffer[1024];
                k_hs_inspect_functions[(short)expression->type](expression->type, *value, buffer, sizeof(buffer));
            }
            hs_return(thread_index, 0);
        }
    }
}

// @address 0x24cc40 ?hs_runtime_initialize@@YAXXZ
namespace {
}

template<>
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_runtime_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x24eec0: value range assumption emits the li r7,0 that zeroes alignment_bits
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_register_member(1, name, type, size, alignment_bits,
        __tls_set_g_structure_seam_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

void hs_runtime_initialize()
{
    g_hs_thread_deterministic_data = data_new("det hs thread", 320, 1316, 0, (c_allocation_base *)&g_hs_thread_deterministic_data_allocator);
    tls_hs_thread_tracking_data() = data_new("tracking hs thread", 320, 12, 0, (c_allocation_base *)&g_hs_thread_tracking_data_allocator);
    tls_hs_thread_non_deterministic_data() = data_new("non-det hs thread", 4, 1316, 0,
        (c_allocation_base *)&g_hs_thread_non_deterministic_data_allocator);
    tls_hs_global_data() = data_new("hs globals", 3072, sizeof(hs_global_runtime), 0, (c_allocation_base *)&g_hs_global_data_allocator);
    tls_hs_distributed_global_data() = data_new("hs dist. globals", 280, 44, 0, (c_allocation_base *)&g_hs_distributed_global_data_allocator);
    g_hs_runtime_globals = static_cast<hs_runtime_globals *>(
        g_hs_runtime_globals_allocator.reserve_memory("hs runtime globals", 0, sizeof(hs_runtime_globals), 0));
    s_data_array *global_data;
    if (tls_hs_thread_tracking_data() && g_hs_thread_deterministic_data && tls_hs_thread_non_deterministic_data() && (global_data =
        tls_hs_global_data()) && tls_hs_distributed_global_data()) {
        data_make_valid(global_data);
        data_make_valid(tls_hs_distributed_global_data());
        for (long global_index = 0; global_index < 1262; global_index++) {
            if (datum_new_at_absolute_index(tls_hs_global_data(), global_index) != -1)
                hs_runtime_global_get((global_index & 0x7fff) | 0x8000)->type = -1;
        }
    }
    hs_initialize_type_conversion_table();
}

void hs_runtime_dispose_from_old_map()
{
    hs_runtime_data_make_invalid((s_data_array *)tls_hs_thread_tracking_data());
    hs_runtime_data_make_invalid((s_data_array *)g_hs_thread_deterministic_data);
    hs_runtime_data_make_invalid((s_data_array *)tls_hs_thread_non_deterministic_data());
    hs_runtime_delete_internal_global_datums();
    data_delete_all((s_data_array *)tls_hs_distributed_global_data());
    g_hs_runtime_globals->initialized = false;
}

// @address 0x24d098 ?hs_runtime_initialize_threads@@YAXXZ
void hs_runtime_initialize_threads(void)
{
    struct scenario *scenario;
    int count;
    int index;
    s_hs_script_definition *thread;
    short thread_type;
    bool create_thread;

    if (global_scenario_index == -1)
        return;
    index = 0;
    scenario = global_scenario;
    count = scenario->scripts.count;
    if (count <= 0)
        return;

    do {
        thread = &((s_hs_script_definition *)scenario->scripts.address)[index];
        if (game_is_predicted())
            create_thread = (short)thread->type == 1;
        else {
            thread_type = (short)thread->type;
            create_thread = thread_type == 0 || thread_type == 1 || thread_type == 2;
        }
        if (create_thread)
            hs_thread_new(_hs_thread_type_script, index, true);
        count = scenario->scripts.count;
        index++;
    } while (index < count);
}

// @address 0x24d160 hs_runtime_evaluate
extern "C" long hs_runtime_evaluate(long expression_index, bool, bool)
{
    long result = -1;
    if (expression_index != -1) {
        bool initialized_here = false;
        if (!g_hs_runtime_globals->initialized) {
            ::hs_runtime_initialize_for_new_map();
            initialized_here = true;
        }
        if (g_hs_runtime_globals->initialized) {
            long thread_index = hs_thread_new(_hs_thread_type_console, -1, true);
            if (thread_index != -1) {
                hs_thread *thread = hs_runtime_thread_get(thread_index);
                hs_destination_pointer destination;
                memset(&destination, 0, sizeof(destination));
                destination.destination_type = 3;
                thread->flags &= ~4;
                hs_evaluate(thread_index, expression_index, destination, 0);
                if (thread->flags & 1)
                    hs_thread_main(thread_index);
                else
                    result = thread->result;
            }
        }
        if (initialized_here)
            hs_runtime_dispose_from_old_map();
    }
    return result;
}

// @address 0x24d250 ?hs_runtime_script_begin@@YAJFW4e_hs_script_type@@W4e_hs_thread_type@@@Z
long hs_runtime_script_begin(short script_index, e_hs_script_type script_type, e_hs_thread_type thread_type)
{
    s_hs_script_definition *script = &((s_hs_script_definition *)global_scenario_get()->scripts.address)[script_index];
    long thread_index = -1;
    if ((short)script->type == script_type) {
        thread_index = hs_thread_new(thread_type, script_index, true);
        if (thread_index != -1) {
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 3;
            hs_evaluate(thread_index, script->root_expression, destination, 0);
        }
    }
    return thread_index;
}

// @address 0x24d2f0 ?hs_global_evaluate@@YAJF@Z
long hs_global_evaluate(short global_index)
{
    hs_global_reconcile_read(global_index);
    return hs_runtime_global_get(global_index)->value.long_value;
}

// @address 0x24d840 hs_stack_allocate
extern "C" char *hs_stack_allocate(long thread_index, unsigned int size, __int64 alignment_bits, short *out_offset)
{
    hs_thread *datum = &((hs_thread *)g_hs_thread_deterministic_data->data)[datum_index_to_absolute_index(thread_index)];
    char *result = 0;
    hs_stack_frame *block = (hs_stack_frame *)(datum->stack + datum->stack_offset);
    int used = block->size + datum->stack_offset + 0x14;
    char *current = datum->stack + used;
    unsigned int mask = (1 << alignment_bits) - 1;
    int pad = ((mask + address_from_pointer(current)) & ~mask) - address_from_pointer(current);
    unsigned int offset = pad + used;
    if (offset + size <= 0x500) {
        result = datum->stack + offset;
        block->size = block->size + pad + size;
        if (pad > 0)
            memset(current, 0, pad);
        if ((offset & 3) == 0 && size < 4)
            memset(result + size, 0, 4 - size);
    } else {
        offset = -1;
        datum->flags |= 0x40;
    }
    if (out_offset)
        *out_offset = offset;
    return result;
}

// @address 0x24d940 hs_wake
extern "C" void hs_wake(long thread_index, long waking_thread_index)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);

    if (thread->sleep_until == -1 || thread->sleep_until == -3)
        return;
    thread->sleep_until = 0;
    if (thread->flags & 2) {
        thread->flags &= ~2;
        thread->sleep_until = thread->previous_sleep_until;
    }
    else {
        hs_stack_frame *frame = hs_runtime_script_frame(thread, thread->stack_offset);
        if (frame->expression_index != -1 && hs_runtime_script_node_is_type_21(frame->expression_index)) {
            hs_runtime_script_pop_frame(thread_index);
        }
        else if (frame->previous != -1) {
            hs_stack_frame *parent = hs_runtime_script_frame(thread, frame->previous);
            if (parent->expression_index != -1 && hs_runtime_script_node_is_type_21(parent->expression_index)) {
                hs_runtime_script_pop_frame(thread_index);
                hs_runtime_script_pop_frame(thread_index);
                thread->flags &= ~1;
            }
        }
    }
    thread->flags |= 0x20;
}

// @address 0x24dac0 ?hs_arguments_evaluate@@YAPAJJFPBF_N@Z
long *hs_arguments_evaluate(long thread_index, short formal_parameter_count, const short *formal_parameters, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short parameters_offset;
    long *actual_parameters;
    {
        short stack_offset;
        actual_parameters = (long *)hs_stack_allocate(thread_index, formal_parameter_count * sizeof(long), 2, &stack_offset);
        parameters_offset = stack_offset;
    }
    short *argument_index = (short *)hs_stack_allocate(thread_index, 2, 1, 0);
    long *expression_index = (long *)hs_stack_allocate(thread_index, 4, 2, 0);
    if (actual_parameters && argument_index && expression_index) {
        s_data_array *syntax = g_hs_syntax_data;
        if (initialize) {
            *argument_index = 0;
            hs_syntax_node *call_expression = &((hs_syntax_node *)syntax->data)[
                datum_index_to_absolute_index(hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)];
            *expression_index = ((hs_syntax_node *)syntax->data)[
                datum_index_to_absolute_index(call_expression->value)].next_node_index;
        }
        if (*argument_index < formal_parameter_count) {
            if ((short)((hs_syntax_node *)syntax->data)[datum_index_to_absolute_index(*expression_index)].type ==
                formal_parameters[*argument_index]) {
                hs_destination_pointer destination;
                memset(&destination, 0, sizeof(destination));
                destination.destination_type = 1;
                destination.stack_offset = parameters_offset + *argument_index * sizeof(long);
                hs_evaluate(thread_index, *expression_index, destination, 0);
                *expression_index = hs_runtime_syntax_get(*expression_index)->next_node_index;
                (*argument_index)++;
                actual_parameters = 0;
            }
        }
        return actual_parameters;
    }
    return actual_parameters;
}
// @address 0x24de30 ?hs_long_to_boolean@@YAJJ@Z
long hs_long_to_boolean(long value)
{
    return hs_runtime_long_to_boolean(value);
}

// @address 0x24de50 ?hs_short_to_boolean@@YAJJ@Z
long hs_short_to_boolean(long value)
{
    hs_value argument;
    argument.long_value = value;
    hs_value result;
    result.long_value = 0;
    result.boolean_value = (short)argument.short_value == 0;
    return result.long_value;
}

// @address 0x24de78 ?hs_string_to_boolean@@YAJJ@Z
long hs_string_to_boolean(long value)
{
    hs_value argument;
    argument.long_value = value;
    return hs_runtime_long_to_boolean(strlen(argument.string_value));
}

// @address 0x24deb0 ?hs_short_to_real@@YAJJ@Z
long hs_short_to_real(long value)
{
    hs_value argument;
    argument.long_value = value;
    hs_value result;
    result.real_value = (float)(short)argument.short_value;
    return result.long_value;
}

// @address 0x24ded8 ?hs_long_to_real@@YAJJ@Z
long hs_long_to_real(long value)
{
    hs_value result;
    result.real_value = (float)value;
    return result.long_value;
}

// @address 0x24def8 ?hs_enum_to_real@@YAJJ@Z
long hs_enum_to_real(long value)
{
    hs_value argument;
    argument.long_value = value;
    hs_value result;
    result.real_value = (float)((short)argument.short_value + 1);
    return result.long_value;
}

// @address 0x24df28 ?hs_real_to_short@@YAJJ@Z
long hs_real_to_short(long value)
{
    hs_value result;
    result.long_value = 0;
    result.short_value = (short)*(float *)&value;
    return result.long_value;
}

// @address 0x24df50 ?hs_real_to_long@@YAJJ@Z
long hs_real_to_long(long value)
{
    return (long)*(float *)&value;
}

// @address 0x24df70 ?hs_short_to_long@@YAJJ@Z
long hs_short_to_long(long value)
{
    hs_value argument;
    argument.long_value = value;
    return (short)argument.short_value;
}

// @address 0x24df80 ?hs_long_to_short@@YAJJ@Z
long hs_long_to_short(long value)
{
    hs_value result;
    result.long_value = 0;
    result.short_value = (short)value;
    return result.long_value;
}


namespace {
static __forceinline long hs_runtime_object_index_from_name(short name_index)
{
    long result = -1;
    if (name_index >= 0 && name_index < 2048)
        result = ((long *)g_object_name_list)[name_index];
    return result;
}
}

// @address 0x24df98 ?hs_object_name_to_object_list@@YAJJ@Z
long hs_object_name_to_object_list(long object_name)
{
    long result = -1;
    hs_value name;
    name.long_value = object_name;
    long object_index = hs_runtime_object_index_from_name((short)name.short_value);
    if (object_index != -1) {
        result = object_list_new();
        if (result != -1)
            object_list_add(result, object_index);
    }
    return result;
}

// @address 0x24e020 ?hs_object_to_object_list@@YAJJ@Z
long hs_object_to_object_list(long object_index)
{
    long result = -1;
    if (object_index != -1) {
        result = object_list_new();
        if (result != -1)
            object_list_add(result, object_index);
    }
    return result;
}

// @address 0x42d298 ?hs_data_to_void@@YAJJ@Z
long hs_data_to_void(long)
{
    return 0;
}

// natural: 0x24e078
// @address 0x24e078 hs_initialize_type_conversion_table
extern "C" void hs_initialize_type_conversion_table()
{
    long type;
    for (type = _hs_type_void + 1; type < k_hs_type_count; type++)
        g_hs_typecasting_table[_hs_type_void][type] = hs_data_to_void;
    g_hs_typecasting_table[_hs_type_boolean][_hs_type_real] = hs_long_to_boolean;
    g_hs_typecasting_table[_hs_type_boolean][_hs_type_short_integer] = hs_short_to_boolean;
    g_hs_typecasting_table[_hs_type_boolean][_hs_type_long_integer] = hs_long_to_boolean;
    g_hs_typecasting_table[_hs_type_boolean][_hs_type_string] = hs_string_to_boolean;
    g_hs_typecasting_table[_hs_type_real][_hs_type_short_integer] = hs_short_to_real;
    g_hs_typecasting_table[_hs_type_real][_hs_type_long_integer] = hs_long_to_real;
    for (type = _hs_type_enum_first; type <= _hs_type_enum_last; type++)
        g_hs_typecasting_table[_hs_type_real][type] = hs_enum_to_real;
    g_hs_typecasting_table[_hs_type_short_integer][_hs_type_real] = hs_real_to_short;
    g_hs_typecasting_table[_hs_type_short_integer][_hs_type_long_integer] = hs_long_to_short;
    g_hs_typecasting_table[_hs_type_long_integer][_hs_type_real] = hs_real_to_long;
    g_hs_typecasting_table[_hs_type_long_integer][_hs_type_short_integer] = hs_short_to_long;
    for (type = _hs_type_object; type <= _hs_type_effect_scenery; type++)
        g_hs_typecasting_table[_hs_type_object_list][type] = hs_object_to_object_list;
    for (type = _hs_type_object_name; type <= _hs_type_effect_scenery_name; type++)
        g_hs_typecasting_table[_hs_type_object_list][type] = hs_object_name_to_object_list;
    g_hs_typecasting_table[_hs_type_object_list][_hs_type_ai] = object_list_from_ai_reference;
}

void hs_global_reconcile_read(short global_index)
{
    if (global_index & 0x8000) {
        hs_global_runtime *global = hs_runtime_global_get(global_index);
        hs_global_external *definition = hs_external_globals[global_index & 0x7fff];
        switch (hs_global_get_type(global_index)) {
       
        case _hs_type_boolean:
            global->value.boolean_value = definition->value ? *(bool *)definition->value : false;
            break;
       
        case _hs_type_real:
            global->value.real_value = definition->value ? *(float *)definition->value : 0.0f;
            break;
       
        case _hs_type_short_integer:
        case _hs_type_mp_team:
        case _hs_type_controller:
        case _hs_type_button_preset:
        case _hs_type_joystick_preset:
        case _hs_type_player_color:
        case _hs_type_voice_output_setting:
        case _hs_type_subtitle_setting:
        case _hs_type_character_physics:
            global->value.short_value = definition->value ? *(unsigned short *)definition->value : 0;
            break;
       
        case _hs_type_long_integer:
            global->value.long_value = definition->value ? *(long *)definition->value : 0;
            break;
       
        case _hs_type_string:
            global->value.string_value = definition->value ? *(char **)definition->value : (char *)"";
            break;
       
        case _hs_type_script:
        case _hs_type_trigger_volume:
        case _hs_type_cutscene_flag:
        case _hs_type_cutscene_camera_point:
        case _hs_type_cutscene_title:
        case _hs_type_cutscene_recording:
        case _hs_type_ai_command_list:
        case _hs_type_ai_command_script:
        case _hs_type_ai_behavior:
        case _hs_type_ai_orders:
       
        case _hs_type_starting_profile:
        case _hs_type_conversation:
        case _hs_type_zone_set:
        case _hs_type_enum_first:
        case _hs_type_team:
        case _hs_type_actor_type:
        case _hs_type_model_state:
        case _hs_type_event:
        case _hs_type_object_name:
            global->value.short_value = definition->value ? *(unsigned short *)definition->value : -1;
            break;
       
        case _hs_type_string_id:
        case _hs_type_ai:
            global->value.long_value = definition->value ? *(long *)definition->value : 0;
            break;
       
        case _hs_type_player_character_type:
            global->value.short_value = definition->value ? *(unsigned short *)definition->value : 2;
            break;
       
        case _hs_type_unit_seat_mapping:
        case _hs_type_device_group:
        case _hs_type_ai_line:
        case _hs_type_point_reference:
        case _hs_type_style:
        case _hs_type_object_list:
        case _hs_type_folder:
        case _hs_type_sound:
        case _hs_type_effect:
        case _hs_type_damage:
        case _hs_type_looping_sound:
       
        case _hs_type_animation_graph:
        case _hs_type_damage_effect:
        case _hs_type_object_definition:
        case _hs_type_bitmap:
        case _hs_type_shader:
        case _hs_type_render_model:
        case _hs_type_structure_definition:
        case _hs_type_lightmap_definition:
        case _hs_type_cinematic_definition:
        case _hs_type_cinematic_scene_definition:
        case _hs_type_bink_definition:
       
        case _hs_type_any_tag:
       
        case _hs_type_object:
       
        case _hs_type_unit:
       
        case _hs_type_vehicle:
       
        case _hs_type_weapon:
       
        case _hs_type_device:
       
        case _hs_type_scenery:
       
        case _hs_type_effect_scenery:
       
        case _hs_type_cinematic_lightprobe:
       
        case _hs_type_animation_budget_reference:
       
        case _hs_type_looping_sound_budget_reference:
       
        case _hs_type_sound_budget_reference:
            global->value.long_value = definition->value ? *(long *)definition->value : -1;
            break;
        }
    }
}

// @address 0x24e598 ?hs_global_reconcile_write@@YAXF@Z
namespace {
static __forceinline short hs_runtime_global_get_type(short global_index)
{
    if (global_index & 0x8000)
        return (short)hs_external_globals[global_index & 0x7fff]->type;
    return ((s_hs_global_definition *)global_scenario->globals.address)[global_index & 0x7fff].type;
}
static __forceinline long object_list_get_first_from_header(long object_list_index, long *reference_index)
{
    long object_index = -1;
    if (object_list_index != -1) {
        *reference_index = ((s_object_list_header *)g_object_list_header_data->data)[datum_index_to_absolute_index(
            object_list_index)].first_list_reference;
        object_index = object_list_get_first(object_list_index, reference_index);
    }
    return object_index;
}
}

void hs_global_reconcile_write(short global_index)
{
    short type = hs_runtime_global_get_type(global_index);
    hs_global_runtime *global = hs_runtime_global_get(global_index);
    if (global_index & 0x8000) {
        hs_global_external *definition = hs_external_globals[global_index & 0x7fff];
        switch (type) {
       
        case _hs_type_boolean:
            if (definition->value)
                *(bool *)definition->value = global->value.boolean_value;
            break;
       
        case _hs_type_real:
            if (definition->value)
                *(float *)definition->value = global->value.real_value;
            break;
       
        case _hs_type_short_integer:
        case _hs_type_script:
        case _hs_type_trigger_volume:
        case _hs_type_cutscene_flag:
        case _hs_type_cutscene_camera_point:
        case _hs_type_cutscene_title:
        case _hs_type_cutscene_recording:
        case _hs_type_ai_command_list:
        case _hs_type_ai_command_script:
        case _hs_type_ai_behavior:
        case _hs_type_ai_orders:
       
        case _hs_type_starting_profile:
       
        case _hs_type_conversation:
       
        case _hs_type_zone_set:
       
        case _hs_type_enum_first:
       
        case _hs_type_team:
       
        case _hs_type_mp_team:
       
        case _hs_type_controller:
       
        case _hs_type_actor_type:
       
        case _hs_type_model_state:
       
        case _hs_type_event:
       
        case _hs_type_enum_last:
       
        case _hs_type_object_name:
            if (definition->value)
                *(unsigned short *)definition->value = global->value.short_value;
            break;
       
        case _hs_type_button_preset:
        case _hs_type_joystick_preset:
        case _hs_type_player_color:
        case _hs_type_voice_output_setting:
        case _hs_type_subtitle_setting:
            global->value.short_value = definition->value ? *(unsigned short *)definition->value : 0;
            break;
       
        case _hs_type_player_character_type:
            global->value.short_value = definition->value ? *(unsigned short *)definition->value : 2;
            break;
       
        case _hs_type_long_integer:
       
        case _hs_type_string:
       
        case _hs_type_string_id:
       
        case _hs_type_unit_seat_mapping:
       
        case _hs_type_device_group:
       
        case _hs_type_ai:
       
        case _hs_type_ai_line:
       
        case _hs_type_point_reference:
       
        case _hs_type_style:
       
        case _hs_type_object_list:
       
        case _hs_type_folder:
       
        case _hs_type_sound:
       
        case _hs_type_effect:
        case _hs_type_damage:
        case _hs_type_looping_sound:
        case _hs_type_animation_graph:
        case _hs_type_damage_effect:
        case _hs_type_object_definition:
        case _hs_type_bitmap:
        case _hs_type_shader:
        case _hs_type_render_model:
        case _hs_type_structure_definition:
        case _hs_type_lightmap_definition:
        case _hs_type_cinematic_definition:
       
        case _hs_type_cinematic_scene_definition:
       
        case _hs_type_bink_definition:
       
        case _hs_type_any_tag:
       
        case _hs_type_object:
       
        case _hs_type_unit:
       
        case _hs_type_vehicle:
       
        case _hs_type_weapon:
       
        case _hs_type_device:
       
        case _hs_type_scenery:
       
        case _hs_type_effect_scenery:
       
        case _hs_type_cinematic_lightprobe:
       
        case _hs_type_animation_budget_reference:
       
        case _hs_type_looping_sound_budget_reference:
        case _hs_type_sound_budget_reference:
            if (definition->value)
                *(long *)definition->value = global->value.long_value;
            break;
        }
    }
    if (type >= _hs_type_object && type <= _hs_type_effect_scenery) {
        if (global->value.long_value != -1)
            ((object_datum *)object_get_and_verify_type(global->value.long_value, -1))->flags |= 1UL << 28;
    } else if (type == _hs_type_object_list) {
        long reference_index;
        long object_index = object_list_get_first_from_header(global->value.long_value, &reference_index);
        while (object_index != -1) {
            ((object_datum *)object_get_and_verify_type(object_index, -1))->flags |= 1UL << 28;
            object_index = object_list_get_next(global->value.long_value, &reference_index);
        }
    }
}

static __forceinline scenario *hs_runtime_initialize_scenario_get()
{
    return global_scenario;
}
static __forceinline s_hs_global_definition *hs_runtime_initialize_global_get(long index)
{
    return &((s_hs_global_definition *)hs_runtime_initialize_scenario_get()->globals.address)[index];
}
// @address 0x24ce10 ?hs_runtime_initialize_for_new_map@@YAXXZ
void hs_runtime_initialize_for_new_map()
{
    data_make_valid(tls_hs_thread_tracking_data());
    data_make_valid(g_hs_thread_deterministic_data);
    data_make_valid(tls_hs_thread_non_deterministic_data());
    hs_runtime_globals *globals = tls_hs_runtime_globals();
    tls_hs_runtime_globals()->initialized = true;
    tls_hs_runtime_globals()->require_object_list_gc = false;
    tls_hs_runtime_globals()->require_gc = false;
    tls_hs_runtime_globals()->globals_initializing = false;
    tls_hs_runtime_globals()->executing_thread_index = -1;
    if (global_scenario_index == -1)
        return;
    scenario *scenario = hs_runtime_initialize_scenario_get();
    long thread_index = hs_thread_new(_hs_thread_type_global_initialize, -1, true);
    hs_thread *thread = hs_runtime_thread_get(thread_index);
    globals->globals_initializing = true;
    int index = 0;
    for (; index < scenario->globals.count; index++) {
        s_hs_global_definition *global = hs_runtime_initialize_global_get(index);
        long datum = hs_runtime_global_index_to_datum_index(index);
        int masked = index & 0x7fff;
        datum_new_at_absolute_index(tls_hs_global_data(), datum);
        thread->script_index = -1;
        hs_destination_pointer destination;
        memset(&destination, 0, sizeof(destination));
        destination.destination_type = _hs_destination_global;
        destination.stack_offset = (short)index;
        hs_runtime_main_frame_get((hs_thread *)thread)->size = 0;
        hs_evaluate(thread_index, global->root_expression, destination, 0);
        if (thread->flags & 1) {
            hs_thread_main(thread_index);
            if (global->type == _hs_type_object_list) {
                long object_list = hs_global_evaluate((short)index);
                if (object_list != -1) {
                    unsigned short list_index = (unsigned short)object_list;
                    s_object_list_header *entry = &((s_object_list_header *)g_object_list_header_data->data)[list_index];
                    entry->count++;
                }
            }
        }
        hs_global_reconcile_write((short)masked);
    }
    tls_hs_runtime_globals()->globals_initializing = false;
    hs_thread_delete(thread_index, true);
    hs_runtime_initialize_threads();
}

void hs_scripting_kill_running_thread(long thread_index)
{
    hs_thread *thread = &((hs_thread *)g_hs_thread_deterministic_data->data)[datum_index_to_absolute_index(thread_index)];
    bool kill_thread = false;
    if (thread->type < 5) {
        if (thread->sleep_until == -1 || thread->sleep_until == -2)
            kill_thread = false;
        else
            kill_thread = true;
    }
    if (kill_thread) {
        thread->sleep_until = 0;
        thread->flags |= 0x40;
    }
}

template<>
// @address 0x24ebc0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_hs_thread_tracking_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_thread_tracking_data_allocator>::allocate(
    unsigned int size, const char *name)
{
    m_member_index = restricted_region_register_member(1, name, "", size, 0,
        __tls_set_g_hs_thread_tracking_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
// @address 0x24ec60 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_hs_thread_tracking_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
void t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_thread_tracking_data_allocator>::deallocate(void *memory)
{
    tls_hs_thread_tracking_data() = 0;
    restricted_region_free_member(1, m_member_index);
}

template<>
// @address 0x24ec80 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?hs_thread_set_nondet_data_base@@YAXPAX@Z@@UAAPAXIPBD@Z
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),&hs_thread_set_nondet_data_base>::allocate(
    unsigned int size, const char *name)
{
    m_member_index = restricted_region_register_member(1, name, "", size, 0,
        hs_thread_set_nondet_data_base, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
// @address 0x24ed20 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?hs_thread_set_nondet_data_base@@YAXPAX@Z@@UAAXPAX@Z
void t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &hs_thread_set_nondet_data_base>::deallocate(void *memory)
{
    tls_hs_thread_non_deterministic_data() = 0;
    restricted_region_free_member(1, m_member_index);
}

template<>
// @address 0x24ed40 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_hs_global_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),&__tls_set_g_hs_global_data_allocator>::allocate(
    unsigned int size, const char *name)
{
    m_member_index = restricted_region_register_member(1, name, "", size, 0,
        __tls_set_g_hs_global_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
// @address 0x24ede0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_hs_global_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
void t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_global_data_allocator>::deallocate(void *memory)
{
    tls_hs_global_data() = 0;
    restricted_region_free_member(1, m_member_index);
}

template<>
// @address 0x24ee00 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_hs_distributed_global_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_distributed_global_data_allocator>::allocate(
    unsigned int size, const char *name)
{
    m_member_index = restricted_region_register_member(1, name, "", size, 0,
        __tls_set_g_hs_distributed_global_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
// @address 0x24eea0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_hs_distributed_global_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
void t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_distributed_global_data_allocator>::deallocate(void *memory)
{
    tls_hs_distributed_global_data() = 0;
    restricted_region_free_member(1, m_member_index);
}

template<>
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),&__tls_set_g_hs_runtime_globals_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

template<>
// @address 0x24ef78 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_hs_runtime_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
void t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_runtime_globals_allocator>::deallocate(void *memory)
{
    g_hs_runtime_globals = 0;
    restricted_region_free_member(1, m_member_index);
}

void __tls_set_g_hs_thread_non_deterministic_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_contrail_profile_data, (s_data_array *)object);
}

// @address 0x2b64a8 ?__tls_set_g_particle_location_data_array_allocator@@YAXPAX@Z
void __tls_set_g_particle_location_data_array_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_particle_location_data_array, (s_data_array *)object);
}

namespace {
static __forceinline bool hs_runtime_script_started(const char *name)
{
    long thread_index = hs_find_thread_by_name(name);
    bool started = false;
    if (thread_index != -1)
    {
        hs_thread *thread = hs_runtime_thread_get(thread_index);
        if (thread->sleep_until == -2 && (thread->flags & 32) == 0)
            started = false;
        else
            started = true;
    }
    return started;
}
}

// @address 0x2bc4b8 ?hs_script_started@@YAXFJ_N@Z
void hs_script_started(short function_index, long thread_index, bool initialize)
{
    const hs_function_definition *definition = hs_function_table[function_index];
    hs_value result;
    result.long_value = 0;
    const hs_value *arguments = (const hs_value *)hs_arguments_evaluate(thread_index, definition->formal_parameter_count,
        definition->formal_parameters, initialize);
    if (arguments)
    {
        result.boolean_value = hs_runtime_script_started(arguments[0].string_value);
        hs_return(thread_index, result.long_value);
    }
}

namespace {
static __forceinline void hs_runtime_reset_scripts()
{
    if (game_simulation::game_is_authoritative())
    {
        s_hs_thread_iterator iterator;
        ((void (*)(s_hs_thread_iterator *, bool, bool))hs_thread_iterator_new)(&iterator, true, true);
        long thread_index;
        while ((thread_index = hs_thread_iterator_next(&iterator)) != -1)
        {
            hs_thread *thread = hs_runtime_thread_get(thread_index);
            if (thread->type != _hs_thread_type_console)
                hs_thread_delete(thread_index, false);
        }
        hs_runtime_initialize_threads();
        ai_handle_script_verification(true);
    }
}
}

// @address 0x2dc210 ?hs_reset_scripts@@YAXFJ_N@Z
void hs_reset_scripts(short function_index, long thread_index, bool initialize)
{
    hs_runtime_reset_scripts();
    hs_return(thread_index, 0);
}

void __tls_set_g_hs_runtime_globals_allocator(void *allocation)
{
    g_object_scripting_state = (s_object_scripting_state *)allocation;
}

 

static __forceinline long hs_runtime_find_datum_index(const s_data_array *data, long absolute_index)
{
    long result = -1;
    if (absolute_index != -1)
        result = (*(unsigned short *)(data->data + data->size * absolute_index) << 16) | absolute_index;
    return result;
}

static __forceinline long hs_runtime_find_next_index(s_data_array *data, long index)
{
    long absolute_index = ::data_next_absolute_index((const ::s_data_array *)(data), datum_index_to_absolute_index(index + 1));
    long next_index = -1;
    if (absolute_index != -1)
        next_index = hs_runtime_find_datum_index(data, absolute_index);
    return next_index;
}

static __forceinline void hs_runtime_find_iterator_new(s_hs_thread_iterator *iterator)
{
    if (g_hs_runtime_globals->initialized) {
        iterator->active_only = true;
        iterator->include_remote = true;
        iterator->index = -1;
        iterator->finished = false;
        iterator->index = hs_runtime_find_next_index((s_data_array *)g_hs_thread_deterministic_data, iterator->index);
    }
}

static __forceinline long hs_runtime_find_iterator_next(s_hs_thread_iterator *iterator)
{
    long result = -1;
    if (g_hs_runtime_globals->initialized)
    {
        if (iterator->index != -1)
        {
            result = iterator->index;
            iterator->index = hs_runtime_find_next_index((s_data_array *)g_hs_thread_deterministic_data, iterator->index);
        }
    }
    return result;
}

static __forceinline hs_thread *hs_runtime_find_thread_get(long thread_index)
{
    return (hs_thread *)(((s_data_array *)g_hs_thread_deterministic_data)->data + (datum_index_to_absolute_index(thread_index)) * sizeof(hs_thread));
}

static __forceinline void hs_runtime_find_kill_running_thread(long thread_index)
{
    hs_thread *thread = hs_runtime_find_thread_get(thread_index);
    bool update = false;
    switch (thread->type)
    {
    case 0: case 1: case 2: case 3: case 4:
        if (thread->sleep_until == -1 || thread->sleep_until == -2)
            update = false;
        else
            update = true;
        break;
    }
    if (update)
    {
        thread->flags |= 0x40;
        thread->sleep_until = 0;
    }
}

// @address 0x24dc70 ?hs_find_thread_by_script@@YAJF@Z
long hs_find_thread_by_script(short script_index)
{
    s_hs_thread_iterator iterator;
    // match: 0x24dc70: pointer __restrict qualifier prevents alias reload
    ((void (*)(s_hs_thread_iterator *__restrict, bool, bool))hs_thread_iterator_new)(&iterator, true, true);
    long thread_index;
    while ((thread_index = hs_runtime_find_iterator_next(&iterator)) != -1)
    {
        hs_thread *thread = hs_runtime_find_thread_get(thread_index);
        if (thread->script_index == script_index)
            return thread_index;
    }
    return -1;
}

long hs_find_thread_by_name(const char *name)
{
    s_hs_thread_iterator iterator;
    ((void (*)(s_hs_thread_iterator *, bool, bool))hs_thread_iterator_new)(&iterator, true, true);
    long thread_index;
    while ((thread_index = hs_runtime_find_iterator_next(&iterator)) != -1)
    {
        hs_thread *thread = hs_runtime_find_thread_get(thread_index);
        if (thread->script_index != -1)
        {
            s_hs_script_definition *script = &((s_hs_script_definition *)global_scenario_get()->scripts.address)[thread->script_index];
            if (ascii_stricmp(script->name, name) == 0)
                return thread_index;
        }
    }
    return -1;
}

// @address 0x24e9f8 hs_scripting_kill_all_threads
extern "C" void hs_scripting_kill_all_threads()
{
    s_hs_thread_iterator iterator;
    // match: 0x24e9f8: pointer __restrict qualifier prevents alias reload
    ((void (*)(s_hs_thread_iterator *__restrict, bool, bool))hs_thread_iterator_new)(&iterator, true, true);
    long index;
    while ((index = hs_runtime_find_iterator_next(&iterator)) != -1)
    {
        if (index != tls_hs_runtime_globals()->executing_thread_index)
            hs_runtime_find_kill_running_thread(index);
    }
}


// @address 0x24bd38 ?hs_evaluate_if@@YAXFJ_N@Z
void hs_evaluate_if(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short condition_offset;
    short result_offset;
    long *condition = (long *)hs_stack_allocate(thread_index, 4, 2, &condition_offset);
    long *expression = (long *)hs_stack_allocate(thread_index, 4, 2, 0);
    long *result = (long *)hs_stack_allocate(thread_index, 4, 2, &result_offset);
    if (condition && expression && result) {
        if (initialize) {
            *condition = 0;
            *expression = -1;
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            long node = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_script_frame(thread,
                thread->stack_offset)->expression_index)->value)->next_node_index;
            destination.stack_offset = condition_offset;
            hs_evaluate(thread_index, node, destination, 0);
        } else if (*expression == -1) {
            if (*(bool *)condition) {
                *expression = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_script_frame(thread,
                    thread->stack_offset)->expression_index)->value)->next_node_index)->next_node_index;
            } else {
                *expression = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_syntax_get(
                    hs_runtime_script_frame(thread,
                        thread->stack_offset)->expression_index)->value)->next_node_index)->next_node_index)->next_node_index;
                if (*expression == -1) {
                    hs_return(thread_index, 0);
                    return;
                }
            }
            long next_expression = *expression;
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 1;
            destination.stack_offset = result_offset;
            hs_evaluate(thread_index, next_expression, destination, 0);
        } else {
            hs_return(thread_index, *result);
        }
    }
}

extern "C" {
// @address 0x8c4258 g_hs_set_inspect_override
bool g_hs_set_inspect_override;
}
static __forceinline s_object_list_header *hs_runtime_set_list_get(long index)
{
    return &((s_object_list_header *)g_object_list_header_data->data)[datum_index_to_absolute_index(index)];
}
static __forceinline hs_global_runtime *hs_runtime_set_global_get(long index)
{
    long absolute_index = (index & 0x8000) ? (index & 0x7fff) : (index & 0x7fff) + 1262;
    return &((hs_global_runtime *)tls_hs_global_data()->data)[absolute_index];
}

// @address 0x24bf70 ?hs_evaluate_set@@YAXFJ_N@Z
void hs_evaluate_set(short function_index, long thread_index, bool initialize)
{
    char buffer[1024];
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    long expression_index = hs_runtime_syntax_get(hs_runtime_syntax_get(
        hs_runtime_script_frame(thread, thread->stack_offset)->expression_index)->value)->next_node_index;
    hs_syntax_node *variable = hs_runtime_syntax_get(expression_index);
    long *value = (long *)hs_stack_allocate(thread_index, 4, 2, 0);
    if (value) {
        int global_type = ((int (*)(unsigned short))hs_global_get_type)((unsigned long)variable->value);
        if (initialize) {
            hs_destination_pointer destination;
            memset(&destination, 0, sizeof(destination));
            destination.destination_type = 2;
            destination.stack_offset = (unsigned long)variable->value;
            if ((short)global_type == 31) {
                long list_index = hs_global_evaluate((unsigned long)variable->value);
                if (list_index != -1)
                    --(short &)hs_runtime_set_list_get(list_index)->count;
            }
            hs_evaluate(thread_index, hs_runtime_syntax_get(expression_index)->next_node_index, destination, 0);
        } else {
            hs_global_reconcile_write((unsigned long)variable->value);
            if ((short)global_type == 31) {
                long list_index = hs_global_evaluate((unsigned long)variable->value);
                if (list_index != -1)
                    ++(short &)hs_runtime_set_list_get(list_index)->count;
            }
            if (g_hs_set_inspect_override || (thread->flags & 16)) {
                if (k_hs_inspect_functions[(short)variable->type]) {
                    k_hs_inspect_functions[(short)variable->type](variable->type,
                        hs_runtime_set_global_get(variable->value)->value.long_value, buffer, sizeof(buffer));
                } else {
                    strncpy(buffer, "[value unavailable]", sizeof(buffer));
                    buffer[sizeof(buffer) - 1] = 0;
                }
            }
            hs_return(thread_index, hs_global_evaluate((unsigned long)variable->value));
        }
    }
}

static __forceinline bool hs_runtime_deleted_list_remove(long list_index, long object_index)
{
    return list_index != -1 && ((unsigned char (*)(long, long))object_list_delete_object)(list_index, object_index) &&
        ((s_object_list_header *)g_object_list_header_data->data)[datum_index_to_absolute_index(list_index)].reference_count == 0;
}
static __forceinline long hs_runtime_deleted_global_evaluate(short global_index)
{
    hs_global_reconcile_read(global_index);
    return hs_runtime_global_get(global_index)->value.long_value;
}
static __forceinline short hs_runtime_deleted_global_index(long absolute_index)
{
    bool is_external = (short)absolute_index < 1262;
    long index = is_external ? (short)absolute_index : (short)absolute_index - 1262;
    short narrowed = (short)index;
    unsigned long flag = is_external ? 0x8000 : 0;
    return (narrowed & 0x7fff) | flag;
}
static __forceinline bool hs_runtime_deleted_object_referenced(object_datum *object)
{
    return (object->flags >> 28) & 1;
}
static __forceinline long hs_runtime_deleted_iterator_next(s_hs_thread_iterator *iterator, const hs_runtime_globals *globals)
{
    long result = -1;
    if (globals->initialized && iterator->index != -1) {
        result = iterator->index;
        iterator->index = hs_runtime_find_next_index(g_hs_thread_deterministic_data, iterator->index);
    }
    return result;
}
// @address 0x111c60 ?hs_handle_deleted_object@@YAXJ@Z
void hs_handle_deleted_object(long object_index)
{
    if (hs_runtime_deleted_object_referenced((object_datum *)object_get_and_verify_type(object_index, ~0UL))) {
        c_hs_data_iterator<hs_global_runtime> globals;
        globals.begin(tls_hs_global_data());
        bool more_globals = globals.next();
        scenario *scenario = global_scenario;
        while (more_globals) {
            short global_index = hs_runtime_deleted_global_index(globals.get_index());
            short type;
            if (global_index & 0x8000)
                type = (short)hs_external_globals[global_index & 0x7fff]->type;
            else
                type = ((s_hs_global_definition *)scenario->globals.address)[global_index & 0x7fff].type;
            if (type >= 65 && type <= 71) {
                if (hs_runtime_deleted_global_evaluate(global_index) == object_index) {
                    globals.get_datum()->value.long_value = -1;
                    hs_global_reconcile_write(global_index);
                }
            } else if (type == 31) {
                long list_index = hs_runtime_deleted_global_evaluate(global_index);
                if (list_index != -1 && ((unsigned char (*)(long, long))object_list_delete_object)(list_index, object_index)) {
                    s_object_list_header *list = &((s_object_list_header *)g_object_list_header_data->data)[
                        datum_index_to_absolute_index(list_index)];
                    if (list->reference_count == 0) {
                    --(short &)list->count;
                    globals.get_datum()->value.long_value = -1;
                    hs_global_reconcile_write(global_index);
                    }
                }
            }
            more_globals = globals.next();
        }
        s_hs_thread_iterator threads;
        // match: 0x111c60: pointer __restrict qualifier prevents alias reload
        ((void (*)(s_hs_thread_iterator *__restrict, bool, bool))hs_thread_iterator_new)(&threads, true, true);
        const hs_runtime_globals *runtime = tls_hs_runtime_globals();
        long thread_index;
        while ((thread_index = hs_runtime_deleted_iterator_next(&threads, runtime)) != -1) {
            hs_thread *thread = hs_runtime_thread_get(thread_index);
            for (short offset = thread->stack_offset; offset != -1; ) {
                hs_stack_frame *frame = (hs_stack_frame *)(thread->stack + offset);
                if (frame->script_index >= 0 && frame->script_index < scenario->scripts.count) {
                    s_hs_script_definition *script = &((s_hs_script_definition *)scenario->scripts.address)[frame->script_index];
                    if (script->parameters.count > 0) {
                        long *parameters = (long *)(thread->stack + frame->parameters_offset);
                        for (short i = 0; i < script->parameters.count; ++i) {
                            short type = ((s_hs_script_parameter *)script->parameters.address)[i].type;
                            if (type >= 65 && type <= 71) {
                                if (parameters[i] == object_index)
                                    parameters[i] = -1;
                            } else if (type == 31) {
                                long list_index = parameters[i];
                                if (list_index != -1 && ((unsigned char (*)(long, long))object_list_delete_object)(list_index, object_index)) {
                                    s_object_list_header *list = &((s_object_list_header *)g_object_list_header_data->data)[
                                        datum_index_to_absolute_index(list_index)];
                                    if (list->reference_count == 0) {
                                        --(short &)list->count;
                                        parameters[i] = -1;
                                    }
                                }
                            }
                        }
                    }
                }
                offset = frame->previous;
            }
        }
        object_lists_dispose_empty_lists();
    }
}

static __forceinline hs_stack_frame *hs_runtime_evaluate_frame(hs_thread *thread)
{
    return (hs_stack_frame *)(&thread->stack[(short)thread->stack_offset]);
}
static __forceinline void hs_runtime_script_inspect(short type, long value, char *buffer)
{
    const char *unavailable = "[value unavailable]";
    if (k_hs_inspect_functions[type])
        k_hs_inspect_functions[type](type, value, buffer, 100);
    else
        csstrnzcpy(buffer, unavailable, 100);
}
// @address 0x24d348 ?hs_script_evaluate@@YAXFJ_N@Z
void hs_script_evaluate(short script_index, long thread_index, bool initialize)
{
    if (script_index == -1) {
        hs_return(thread_index, 0);
    } else {
        s_hs_script_definition *script = &((s_hs_script_definition *)global_scenario_get()->scripts.address)[script_index];
        hs_thread *thread = hs_runtime_thread_get(thread_index);
        short *argument_index = (short *)hs_stack_allocate(thread_index, 2, 1, 0);
        long *expression_index = (long *)hs_stack_allocate(thread_index, 4, 2, 0);
        short value_offset;
        long *value = (long *)hs_stack_allocate(thread_index, 4, 2, &value_offset);
        if (argument_index && expression_index && value) {
            short parameters_offset = -1;
            long *parameters = 0;
            if (script->parameters.count <= 0 ||
                (parameters = (long *)hs_stack_allocate(thread_index, script->parameters.count * 4, 2, &parameters_offset)) != 0) {
                if (initialize) {
                    *argument_index = 0;
                    *expression_index = hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_evaluate_frame(
                        thread)->expression_index)->value)->next_node_index;
                }
                if (*argument_index < script->parameters.count) {
                    hs_destination_pointer destination;
                    memset(&destination, 0, sizeof(destination));
                    destination.destination_type = 1;
                    destination.stack_offset = parameters_offset + *argument_index * 4;
                    hs_evaluate(thread_index, *expression_index, destination, 0);
                    *expression_index = hs_runtime_syntax_get(*expression_index)->next_node_index;
                } else if (*argument_index == script->parameters.count) {
                    long call_expression = hs_runtime_evaluate_frame(thread)->expression_index;
                    hs_destination_pointer destination;
                    memset(&destination, 0, sizeof(destination));
                    destination.destination_type = 1;
                    destination.stack_offset = value_offset;
                    if (hs_evaluate(thread_index, script->root_expression, destination, 0)) {
                        if (thread->flags & 16) {
                            char buffer[10240];
                            char value_buffer[100];
                            csnzprintf(buffer, sizeof(buffer), "%s: %s ", hs_thread_format(thread_index), script->name);
                            if (parameters) {
                                for (short i = 0; i < script->parameters.count; ++i) {
                                    short type = ((s_hs_script_parameter *)script->parameters.address)[i].type;
                                    hs_runtime_script_inspect(type, parameters[i], value_buffer);
                                    csnzappendf(buffer, sizeof(buffer), "%s ", value_buffer);
                                }
                            }
                            if (call_expression != -1) {
                                short line = (short)((hs_syntax_node *)g_hs_syntax_data->data)[datum_index_to_absolute_index(
                                    call_expression)].line_number;
                                csnzappendf(buffer, sizeof(buffer), "   (line #%i)", line);
                            }
                        }
                        for (short i = 0; i < script->parameters.count; ++i) {
                            short type = ((s_hs_script_parameter *)script->parameters.address)[i].type;
                            if (type >= 65 && type <= 71) {
                                if (parameters[i] != -1)
                                    ((object_datum *)object_get_and_verify_type(parameters[i], ~0UL))->flags |= 1UL << 28;
                            } else if (type == 31) {
                                if (parameters[i] != -1)
                                    ++(short &)hs_runtime_set_list_get(parameters[i])->count;
                            }
                        }
                        hs_runtime_evaluate_frame(thread)->parameters_offset = parameters_offset;
                        hs_runtime_evaluate_frame(thread)->script_index = script_index;
                    }
                } else {
                    for (short i = 0; i < script->parameters.count; ++i) {
                        if (((s_hs_script_parameter *)script->parameters.address)[i].type == 31 && parameters[i] != -1)
                            --(short &)hs_runtime_set_list_get(parameters[i])->count;
                    }
                    hs_return(thread_index, *value);
                }
                ++*argument_index;
            }
        }
    }
}

static __forceinline long hs_runtime_sleep_argument(hs_thread *thread)
{
    return hs_runtime_syntax_get(hs_runtime_syntax_get(hs_runtime_script_frame(thread,
        thread->stack_offset)->expression_index)->value)->next_node_index;
}
// @address 0xd4648 ?hs_evaluate_sleep_until@@YAXFJ_N@Z
void hs_evaluate_sleep_until(short function_index, long thread_index, bool initialize)
{
    hs_thread *thread = hs_runtime_script_thread_get(thread_index);
    short condition_offset, period_offset, timeout_offset;
    hs_value *condition = (hs_value *)hs_stack_allocate(thread_index, 4, 2, &condition_offset);
    short *period = (short *)hs_stack_allocate(thread_index, 4, 2, &period_offset);
    long *timeout = (long *)hs_stack_allocate(thread_index, 4, 2, &timeout_offset);
    long *start_time = (long *)hs_stack_allocate(thread_index, 4, 2, 0);
    short *state = (short *)hs_stack_allocate(thread_index, 2, 1, 0);
    if (condition && period && timeout && start_time && state) {
        long period_expression = hs_runtime_syntax_get(hs_runtime_sleep_argument(thread))->next_node_index;
        if (initialize) {
            condition->boolean_value = false;
            *start_time = game_time_get();
            *state = 0;
            *period = 30;
            *timeout = -1;
            if (period_expression != -1) {
                hs_destination_pointer destination;
                memset(&destination, 0, sizeof(destination));
                destination.destination_type = 1;
                destination.stack_offset = period_offset;
                hs_evaluate(thread_index, period_expression, destination, 0);
                return;
            }
        }
        if (*state == 0) {
            *state = 1;
            if (period_expression != -1) {
                long timeout_expression = hs_runtime_syntax_get(period_expression)->next_node_index;
                if (timeout_expression != -1) {
                    hs_destination_pointer destination;
                    memset(&destination, 0, sizeof(destination));
                    destination.destination_type = 1;
                    destination.stack_offset = timeout_offset;
                    hs_evaluate(thread_index, timeout_expression, destination, 0);
                    return;
                }
            }
        }
        if (*state == 1) {
            long timeout_ticks = -1;
            if (*timeout != -1)
                timeout_ticks = game_seconds_to_ticks_round(hs_runtime_ticks_to_seconds(*timeout));
            hs_value result;
            if (condition->boolean_value) {
                result.long_value = 0;
                result.boolean_value = true;
            } else if (timeout_ticks != -1 && game_time_get() >= *start_time + timeout_ticks) {
                result.long_value = 0;
                result.boolean_value = false;
            } else {
                hs_destination_pointer destination;
                memset(&destination, 0, sizeof(destination));
                destination.destination_type = 1;
                destination.stack_offset = condition_offset;
                hs_evaluate(thread_index, hs_runtime_sleep_argument(thread), destination, 0);
                long period_ticks = game_seconds_to_ticks_round(hs_runtime_ticks_to_seconds(*period));
                if (period_ticks < 1)
                    period_ticks = 1;
                thread->sleep_until = game_time_get() + period_ticks;
                if (timeout_ticks != -1) {
                    long deadline = *start_time + timeout_ticks;
                    long wake_time = thread->sleep_until;
                    if (deadline <= wake_time)
                        wake_time = deadline;
                    thread->sleep_until = wake_time;
                }
                return;
            }
            hs_return(thread_index, result.long_value);
        }
    }
}

__declspec(thread) s_data_array *g_hs_thread_deterministic_data;
__declspec(thread) s_data_array *g_hs_thread_tracking_data;
__declspec(thread) s_data_array *g_hs_thread_non_deterministic_data;
__declspec(thread) s_data_array *g_hs_distributed_global_data;
__declspec(thread) hs_runtime_globals *g_hs_runtime_globals;
__declspec(thread) s_data_array *g_hs_global_data;

t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_prop_ref_data_allocator> g_hs_thread_deterministic_data_allocator;
t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_thread_tracking_data_allocator> g_hs_thread_tracking_data_allocator;
t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &hs_thread_set_nondet_data_base> g_hs_thread_non_deterministic_data_allocator;
t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_global_data_allocator> g_hs_global_data_allocator;
t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_distributed_global_data_allocator> g_hs_distributed_global_data_allocator;
t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_hs_runtime_globals_allocator> g_hs_runtime_globals_allocator;
