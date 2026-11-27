#ifndef CSERIES_ASYNC_TYPES_H
#define CSERIES_ASYNC_TYPES_H

enum e_async_completion
{
    _async_completion_pending,
    _async_completion_done
};

enum e_async_priority {
    _async_priority_none = 0,
    _async_priority_predicted = 1,
    _async_priority_content_enumeration = 3,
    _async_priority_blocking_generic = 5,
    _async_priority_blocking_below_sound = 9
};

enum e_async_category {
    _async_category_none = 0,
    _async_category_cache_files = 1,
    _async_category_fonts = 7,
    _async_category_saved_games = 8,
    _async_category_online_files = 9
};

#endif // CSERIES_ASYNC_TYPES_H
