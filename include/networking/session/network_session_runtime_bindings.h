#ifndef NETWORKING_SESSION_RUNTIME_BINDINGS_H
#define NETWORKING_SESSION_RUNTIME_BINDINGS_H

// The pooled interface pointers network_initialize stores, read on Xbox as
// offsets from these two base symbols.
#if defined(_XBOX) && !defined(__clang__)
extern "C" void g_network_session_manager();
extern "C" void network_globals_message_handler();
#endif

#endif
