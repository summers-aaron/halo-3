#ifndef EFFECTS_EFFECT_LIGHTPROBE_MEMORY_H
#define EFFECTS_EFFECT_LIGHTPROBE_MEMORY_H

struct s_effect_lightprobe;

extern "C++" {
void __tls_set_g_havok_game_state_allocator(void *value);
}
extern "C" {
extern __declspec(thread) s_effect_lightprobe *g_effect_lightprobe_memory;
}

#endif
