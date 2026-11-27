#ifndef SOUND_SCRIPTED_SOUND_CONSTANTS_H
#define SOUND_SCRIPTED_SOUND_CONSTANTS_H

struct real_decibel;
extern "C" const real_decibel k_scripted_sound_default_gain;

// match: 0x567dd8: a code-address view preserves constant-pool address scheduling.
extern "C" void scripted_sound_constant_pool_anchor();

#endif
