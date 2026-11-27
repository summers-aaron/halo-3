#ifndef EFFECTS_EFFECT_DATA_H
#define EFFECTS_EFFECT_DATA_H

extern "C++" {
struct s_data_array;
extern __declspec(thread) s_data_array *effect_data;
}

#endif // EFFECTS_EFFECT_DATA_H
