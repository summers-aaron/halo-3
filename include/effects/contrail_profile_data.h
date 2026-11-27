#ifndef EFFECTS_CONTRAIL_PROFILE_DATA_H
#define EFFECTS_CONTRAIL_PROFILE_DATA_H

struct s_data_array;

// Shared with the folded non-deterministic HS thread allocator callback.
extern "C" { extern __declspec(thread) s_data_array *g_contrail_profile_data; }

#endif // EFFECTS_CONTRAIL_PROFILE_DATA_H
