#ifndef CSERIES_PLACEMENT_NEW_FWD_H
#define CSERIES_PLACEMENT_NEW_FWD_H

#include <stddef.h>

// Forward declaration for callers that include the XDK <new> definition later.
inline void *operator new(size_t size, void *memory);

#endif
