#ifndef MAIN_MAIN_RUNTIME_GLOBALS_H
#define MAIN_MAIN_RUNTIME_GLOBALS_H

struct s_main_status_value
{
    char status_type[256];
    char status_value[512];
};

typedef char main_status_value_size[(sizeof(s_main_status_value) == 768) ? 1 : -1];
struct s_main_flag_state;
struct s_main_crash_report_globals
{
    char spare_before_user_input[2];
    char user_input[1024];
    bool user_input_available;
    bool finished;
};
class c_interlocked_long;

extern "C" s_main_status_value g_status_values[32];
extern s_main_flag_state main_globals;
extern "C" s_main_crash_report_globals g_main_crash_report_globals;
extern c_interlocked_long g_single_thread_request_flags;

#endif // MAIN_MAIN_RUNTIME_GLOBALS_H
