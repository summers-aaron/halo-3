#ifndef MAIN_MAIN_TIME_H
#define MAIN_MAIN_TIME_H

// Functions defined in tu/blamlibxbox/main_time.cpp.

bool main_time_is_throttled();
void main_time_globals_post_copy_update(void *allocation);
void main_time_reset();
void main_time_restore(long saved_time);
int rasterizer_get_refresh_rate();
void __tls_set_g_main_time_globals_allocator(void *allocator);
void main_time_throttle(__int64 target);
float main_time_update();
extern "C" void main_time_update_sync_milliseconds(void);

struct s_main_time_globals;

void __tls_set_g_main_time_globals(s_main_time_globals *globals);

// Lag-counter labels and the shared counter submission entry point.
extern "C" {
extern char k_main_time_input_lag_name[];
extern char k_main_time_camera_lag_name[];
void pix_add_named_counter(float lag, const char *name, ...);
}

#endif // MAIN_MAIN_TIME_H
