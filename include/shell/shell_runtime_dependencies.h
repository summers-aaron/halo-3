#ifndef SHELL_SHELL_RUNTIME_DEPENDENCIES_H
#define SHELL_SHELL_RUNTIME_DEPENDENCIES_H

struct s_font_globals;
struct s_shell_font_cache_thread_state;

extern "C" {

extern "C++" s_font_globals g_font_globals;
void font_cache_unlock(s_shell_font_cache_thread_state *state);
}

#endif
