#ifndef HS_HS_THREAD_DETERMINISTIC_DATA_H
#define HS_HS_THREAD_DETERMINISTIC_DATA_H

struct s_data_array;

// hs_runtime_initialize allocates the "det hs thread" data array.
extern "C" {
extern __declspec(thread) s_data_array *g_hs_thread_deterministic_data;
}

#endif // HS_HS_THREAD_DETERMINISTIC_DATA_H
