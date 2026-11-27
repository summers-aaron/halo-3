#ifndef HS_HS_FUNCTION_DEFINITION_H
#define HS_HS_FUNCTION_DEFINITION_H

typedef void (*hs_evaluate_function_definition)(short function_index, long thread_index, bool initialize);

// The formal parameter count and types follow the evaluate and parse callbacks.
struct hs_function_definition
{
    short return_type;
    unsigned short flags;
    hs_evaluate_function_definition evaluate;
    void *parse;
    short formal_parameter_count;
    short formal_parameters[1];
};

#endif // HS_HS_FUNCTION_DEFINITION_H
