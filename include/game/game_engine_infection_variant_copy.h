#ifndef GAME_GAME_ENGINE_INFECTION_VARIANT_COPY_H
#define GAME_GAME_ENGINE_INFECTION_VARIANT_COPY_H

// Copies an infection variant from source to destination.
// Keep that accepted two-register call signature.
extern "C" void game_engine_infection_variant_copy(void *destination, const void *source);

#endif // GAME_GAME_ENGINE_INFECTION_VARIANT_COPY_H
