#ifndef INCLUDE_PHYS_SYSTEM_H
#define INCLUDE_PHYS_SYSTEM_H

#include "phys/phys_singleton.h"
class phys_streambuf_factory;
template<> void phys_singleton<phys_streambuf_factory>::replace_singleton_instance(phys_streambuf_factory *instance);

class phys_thread_memory;
class phys_memory;
class phys_bool;
enum phys_result;

typedef void (*phys_error_report_func)(const char *, void *);
class phys_dummy_singleton;
namespace phys_system {
    void init_singletons();
    void quit_singletons();
    phys_result init_thread_memory(phys_thread_memory *memory);
    phys_result init(phys_memory *memory_manager, phys_thread_memory *system_thread_memory,
        void (*error_report_fn)(const char *, void *), void *error_report_context);
    phys_bool system_is_initialized();
    phys_result release_thread_resources();
    phys_result quit_system();
}

// Data defined in tu/hkbase/hkBaseSystem.cpp that other units use.

extern phys_bool phys_system_init_verbose;
extern phys_bool phys_system_is_initialized;

extern "C" {
extern const char k_phys_error_stack_trace_header[], k_phys_error_location_separator[], k_phys_error_severity_separator[],
    k_phys_error_description_separator[], k_phys_error_line_suffix[], k_phys_error_id_format[];
extern phys_dummy_singleton *phys_dummy_singleton_instance;
}

#endif // HAVOK_HK_BASE_SYSTEM_H
