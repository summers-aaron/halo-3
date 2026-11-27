#ifndef TEXT_FONT_CACHE_THREAD_STATE_TYPES_H
#define TEXT_FONT_CACHE_THREAD_STATE_TYPES_H

// font_cache_lock/font_cache_unlock and their UI callers store one byte.
struct s_font_cache_thread_state
{
    signed char active;
};

typedef char font_cache_thread_state_size[(sizeof(s_font_cache_thread_state) == 1) ? 1 : -1];

#endif // TEXT_FONT_CACHE_THREAD_STATE_TYPES_H
