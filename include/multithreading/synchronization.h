#ifndef MULTITHREADING_SYNCHRONIZATION_H
#define MULTITHREADING_SYNCHRONIZATION_H

// Functions defined in tu/blamlibxbox/synchronization.cpp
// that other units call. Signatures as the definitions spell them.

void initialize_synchronization_objects();
void internal_critical_section_enter(long index);
bool internal_critical_section_try_and_enter(long critical_section_id);
void internal_mutex_take(long mutex_id);
long internal_semaphore_release(long semaphore_id);
long sempahore_get_max_signal_count(long semaphore_id);
void internal_semaphore_take(long semaphore_id);
void synchronization_leave_registered_sections();
bool wait_for_single_object_internal(void *handle, unsigned long timeout_in_milliseconds);

// Functions defined in tu/blamlibxbox/synchronization.cpp that other units call.

extern "C" void synchronization_release_thread_locks();

// Data defined in tu/blamlibxbox/synchronization.cpp that other units use.

struct s_synchronization_globals;

extern s_synchronization_globals g_synch_globals;

#endif // MULTITHREADING_SYNCHRONIZATION_H
