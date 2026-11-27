#ifndef AI_ACTOR_DATA_H
#define AI_ACTOR_DATA_H

struct s_data_array;

// Thread-local actor datum array (TLS slot 0x831df65c), owned by actors.cpp (March 2007 debug name and type; release allocator g_actor_data_allocator).
extern __declspec(thread) s_data_array *actor_data;

#endif // AI_ACTOR_DATA_H
