#ifndef CACHE_RESTRICTED_MEMORY_COPY_H
#define CACHE_RESTRICTED_MEMORY_COPY_H

// Unnamed prebuilt copy routines selected by restricted_memory_copy for 128-byte chunks.
extern "C" {
int restricted_memory_copy_aligned_blocks(void *destination, const void *source, unsigned int size);
int restricted_memory_copy_unaligned_blocks(void *destination, const void *source, unsigned int size);
}

#endif // CACHE_RESTRICTED_MEMORY_COPY_H
