#ifndef CACHE_SYNCHRONIZATION_SEMAPHORE_TYPES_H
#define CACHE_SYNCHRONIZATION_SEMAPHORE_TYPES_H

enum e_synchronization_semaphore
{
    semaphore_none = -1,
    k_semaphore_resource_threading_access_rw_lock = 2,
    k_semaphore_simple_resource_cache_publish_rw_lock = 3
};

#endif // CACHE_SYNCHRONIZATION_SEMAPHORE_TYPES_H
