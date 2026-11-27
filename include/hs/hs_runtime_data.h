#ifndef HS_HS_RUNTIME_DATA_H
#define HS_HS_RUNTIME_DATA_H
#include "objects/object_list_data.h"
#include "effects/particle_array_globals.h"

#include "effects/contrail_profile_data.h"

#include "hs/hs_thread_deterministic_data.h"

#include "hs/hs_thread_types.h"

// The script runtime's thread-local data. Each slot is named after the allocation hs_runtime_initialize
// (hs_runtime.cpp) or hs_initialize (hs.cpp) makes for it.

struct s_data_array;

// "hs runtime globals" (8 bytes). Field roles from March hs_runtime_initialized (+0), hs_runtime_require_gc (+1)
// and hs_runtime_require_object_list_gc (+2); hs_runtime.cpp clears +3 while global initializers run and keeps the
// executing thread at +4.
struct hs_runtime_globals
{
    bool initialized;
    bool require_gc;
    bool require_object_list_gc;
    bool globals_initializing;
    long executing_thread_index;
};

typedef char hs_runtime_globals_size[(sizeof(hs_runtime_globals) == 8) ? 1 : -1];

struct s_particle_data_array;

extern "C" {
extern __declspec(thread) s_data_array *g_hs_thread_tracking_data;          // "tracking hs thread"
extern __declspec(thread) s_data_array *g_hs_thread_non_deterministic_data; // "non-det hs thread"
extern __declspec(thread) s_data_array *g_hs_distributed_global_data;       // "hs dist. globals"
extern __declspec(thread) hs_runtime_globals *g_hs_runtime_globals;                    // hs runtime globals (hs_runtime_globals *)
extern __declspec(thread) s_data_array *g_hs_global_data;                   // "hs globals"
}

static __forceinline s_data_array *&tls_hs_thread_deterministic_data() { return g_hs_thread_deterministic_data; }
static __forceinline hs_runtime_globals *tls_hs_runtime_globals() { return g_hs_runtime_globals; }
static __forceinline s_data_array *&tls_hs_global_data() { return g_hs_global_data; }
static __forceinline s_data_array *&tls_hs_thread_tracking_data() { return g_hs_thread_tracking_data; }
static __forceinline s_data_array *&tls_hs_thread_non_deterministic_data() { return g_hs_thread_non_deterministic_data; }
static __forceinline s_data_array *&tls_hs_distributed_global_data() { return g_hs_distributed_global_data; }
static __forceinline s_data_array *&tls_object_list_header_data() { return g_object_list_header_data; }
static __forceinline s_data_array *&tls_list_object_reference_data() { return g_list_object_reference_data; }

// Whether the script runtime globals have been initialized.
static __forceinline bool hs_runtime_initialized()
{
    return tls_hs_runtime_globals()->initialized;
}

#endif // HS_HS_RUNTIME_DATA_H
