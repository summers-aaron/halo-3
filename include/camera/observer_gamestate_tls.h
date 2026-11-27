#ifndef CAMERA_OBSERVER_GAMESTATE_TLS_H
#define CAMERA_OBSERVER_GAMESTATE_TLS_H

// The observer owns this thread-local slot.
extern "C" {
extern __declspec(thread) void *g_observer_gamestate_globals;
}

#endif
