#ifndef HS_HS_MACRO_FUNCTION_H
#define HS_HS_MACRO_FUNCTION_H

#include "hs/hs_function.h"
#include "hs/hs_runtime.h"

// March hs_runtime.obj: evaluates a script function's arguments as its definition's formal parameters declare them.
static __forceinline long *hs_macro_function_evaluate(short function_index, long thread_index, bool initialize)
{
    const hs_function_definition *definition = hs_function_table[function_index];
    return hs_arguments_evaluate(thread_index, definition->formal_parameter_count, definition->formal_parameters, initialize);
}

#endif // HS_HS_MACRO_FUNCTION_H
