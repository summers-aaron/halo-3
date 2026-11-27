#ifndef SCENARIO_SCENARIO_INTERPOLATORS_H
#define SCENARIO_SCENARIO_INTERPOLATORS_H

// Functions defined in tu/blamlibxbox/scenario_interpolators.cpp.

struct s_scenario_interpolator_state;

float scenario_interpolator_time_fraction_smooth(float fraction);
void scenario_interpolators_frame_update(float delta);
s_scenario_interpolator_state *scenario_get_interpolator_state(long name, long *out_index);
float scenario_interpolator_evaluate_at(long index, float value);
long hs_interpolator_start(long name, float target, float duration);
float hs_interpolator_get_current_value(long name, bool evaluate);
float hs_interpolator_evaluate_at_time_fraction(long name, float time_fraction, bool evaluate);
float hs_interpolator_evaluate_at_time(long name, float time, bool evaluate);
float hs_interpolator_evaluate_at_time_delta(long name, float time_delta, bool evaluate);
void __tls_set_g_scenario_interpolator_globals_allocator(void *allocation);
void scenario_interpolators_initialize(void);
void scenario_interpolators_dispose(void);
void scenario_interpolators_initialize_for_new_map(void);

// Named allocator objects owned by this unit.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_interpolator_globals_allocator> g_scenario_interpolator_globals_allocator;

void scenario_interpolators_dispose_from_old_map(void);

#endif // SCENARIO_SCENARIO_INTERPOLATORS_H
