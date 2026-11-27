#ifndef MATH_PERIODIC_FUNCTIONS_H
#define MATH_PERIODIC_FUNCTIONS_H

// Functions defined in tu/blamlibxbox/periodic_functions.cpp that other units call.

float transition_function_evaluate(short function_type, float t);
float periodic_function_evaluate(short function, float time);

extern "C" {
extern unsigned char k_transition_function_tables[][1024];
extern unsigned char k_periodic_function_tables[][1024];
}

#endif // MATH_PERIODIC_FUNCTIONS_H
