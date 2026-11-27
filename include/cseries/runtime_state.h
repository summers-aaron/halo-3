#ifndef CSERIES_RUNTIME_STATE_H
#define CSERIES_RUNTIME_STATE_H

// Functions defined in tu/blamlibxbox/runtime_state.cpp that other units call.

struct s_data_array;

void * runtime_state_malloc(char const *name, char const *type, long requested_size);
struct s_data_array * runtime_state_data_new(char const *array_name, long maximum_count, long datum_size);

void runtime_state_initialize(void);
void runtime_state_dispose(void);
void runtime_state_initialize_for_new_map(void);
void runtime_state_dispose_from_old_map(void);

#endif // CSERIES_RUNTIME_STATE_H
