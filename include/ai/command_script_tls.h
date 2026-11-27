#ifndef AI_COMMAND_SCRIPT_TLS_H
#define AI_COMMAND_SCRIPT_TLS_H

extern "C++" {
struct s_data_array;
extern __declspec(thread) s_data_array *command_script_data;
}

#endif // AI_COMMAND_SCRIPT_TLS_H
