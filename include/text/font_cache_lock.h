#ifndef TEXT_FONT_CACHE_LOCK_H
#define TEXT_FONT_CACHE_LOCK_H

struct s_font_cache_thread_state;

extern "C" void font_cache_unlock(s_font_cache_thread_state *state);

#endif // TEXT_FONT_CACHE_LOCK_H
