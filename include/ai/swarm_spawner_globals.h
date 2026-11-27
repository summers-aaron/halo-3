#ifndef AI_SWARM_SPAWNER_GLOBALS_H
#define AI_SWARM_SPAWNER_GLOBALS_H

// The nondeterministic swarm count occupies the shared TLS slot at 0x831df880.
extern "C" {
extern __declspec(thread) unsigned short *g_swarm_nondeterministic_counter;
}

#endif
