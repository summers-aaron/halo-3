#ifndef RENDER_RENDER_PLAYER_VIEW_OBSERVER_FLAGS_TYPES_H
#define RENDER_RENDER_PLAYER_VIEW_OBSERVER_FLAGS_TYPES_H

struct render_player_view_observer_flags
{
    long values[5];
};

typedef char render_player_view_observer_flags_size[(sizeof(void *) != 4 || sizeof(render_player_view_observer_flags) == 20) ? 1 : -1];

#endif // RENDER_RENDER_PLAYER_VIEW_OBSERVER_FLAGS_TYPES_H
