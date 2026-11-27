#ifndef INCLUDE_EFFECTS_PARTICLE_GPU_QUEUE_H
#define INCLUDE_EFFECTS_PARTICLE_GPU_QUEUE_H

struct s_particle_emitter_gpu_block;
class c_particle;

extern "C" unsigned long particle_block_queue_gpu_particle(s_particle_emitter_gpu_block *reference, int type,
    const c_particle *particle, float elapsed_time, bool temporary);

#endif // INCLUDE_EFFECTS_PARTICLE_GPU_QUEUE_H
