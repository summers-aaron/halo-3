#ifndef OBJECTS_OBJECT_MARKER_ACCESS_H
#define OBJECTS_OBJECT_MARKER_ACCESS_H

#include "objects/object_marker_globals_types.h"
#include "memory/tls_data_arrays.h"
extern "C" s_object_marker_globals g_object_marker_globals[];

// Lvalue aliases preserve the marker helpers' pointer arithmetic and alias summaries.
#define OBJECT_MARKER_GLOBALS g_object_marker_globals

#endif
