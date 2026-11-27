#ifndef SOUND_SYNCHRONIZATION_EVENT_TYPES_H
#define SOUND_SYNCHRONIZATION_EVENT_TYPES_H

struct s_synchronization_event {
    long locking_thread;
    long lock_count;
    void *handle;
};

#endif // SOUND_SYNCHRONIZATION_EVENT_TYPES_H
