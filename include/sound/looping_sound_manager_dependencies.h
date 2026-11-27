#ifndef SOUND_LOOPING_SOUND_MANAGER_DEPENDENCIES_H
#define SOUND_LOOPING_SOUND_MANAGER_DEPENDENCIES_H


extern "C" void sound_stop_and_unlock(unsigned long sound_index, long reason);
extern "C" unsigned char sound_make_permutation_ready_by_index(long sound_index, bool *ready);

#endif // SOUND_LOOPING_SOUND_MANAGER_DEPENDENCIES_H
