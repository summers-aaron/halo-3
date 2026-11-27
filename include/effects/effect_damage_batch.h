#ifndef EFFECTS_EFFECT_DAMAGE_BATCH_H
#define EFFECTS_EFFECT_DAMAGE_BATCH_H

struct c_render_batch;
struct s_damage_data;
extern "C" bool effect_damage_batch_add_request(c_render_batch *cache, const s_damage_data *request);

#endif // EFFECTS_EFFECT_DAMAGE_BATCH_H
