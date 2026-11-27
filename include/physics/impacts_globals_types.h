#ifndef PHYSICS_IMPACTS_GLOBALS_TYPES_H
#define PHYSICS_IMPACTS_GLOBALS_TYPES_H
#include <stddef.h>

// The orphaned-impacts resume cursor follows the Havok-visible fields.
struct impacts_globals_type
{
    long highest_score_index;
    long sorted_indexes[32];
    bool disconnected_from_physics;
    long orphaned_impacts_resume_index;
};
typedef char impacts_globals_resume_offset[(offsetof(impacts_globals_type, orphaned_impacts_resume_index) == 0x88) ? 1 : -1];
typedef char impacts_globals_size[(sizeof(impacts_globals_type) == 0x8c) ? 1 : -1];
extern "C" { extern __declspec(thread) impacts_globals_type *g_impacts_globals; }

#endif
