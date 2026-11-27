#ifndef INCLUDE_PHYS_SOLVER_CHECK_H
#define INCLUDE_PHYS_SOLVER_CHECK_H

// Functions defined in tu/hkconstraintsolver/hkSolverCheck.cpp that other units call.

extern "C" signed char process_flying_colors(int product);

extern "C" {
extern int g_phys_solver_check_count;
extern const int k_phys_solver_key_masks[12];
extern const char k_havok_prime_component_not_enabled_message[];
}

extern const char PHYS_KEYCODE[];
extern const char *phys_component_strings[];
extern const unsigned int PHYS_KEYVALUE;

#endif // HAVOK_HK_SOLVER_CHECK_H
