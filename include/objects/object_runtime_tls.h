#ifndef OBJECTS_OBJECT_RUNTIME_TLS_H
#define OBJECTS_OBJECT_RUNTIME_TLS_H
#include "objects/object_runtime_globals_types.h"

struct s_object_globals;
static __forceinline s_object_globals *tls_object_runtime_globals() { return object_globals; }

static __forceinline void object_runtime_globals_set(s_object_globals *globals) { object_globals = globals; }
#endif
