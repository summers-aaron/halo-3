#ifndef CAMERA_DIRECTOR_GLOBALS_TYPES_H
#define CAMERA_DIRECTOR_GLOBALS_TYPES_H

#include "camera/debug_director_internals.h"
#include <stddef.h>

#include "camera/director_storage_types.h"

#include "memory/tls_globals.h"
static __forceinline s_controllers *tls_director_globals() { return g_director_globals; }
__forceinline c_director *director_get(long user_index) { return g_director_globals->directors[user_index].get_director(); }

#endif
