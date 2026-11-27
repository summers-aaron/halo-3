#ifndef AI_AI_SCRATCH_H
#define AI_AI_SCRATCH_H

struct s_ai_scratch_block {
    unsigned char in_use;
    void *address;
    int size;
};
typedef char ai_scratch_block_size[(sizeof(void *) != 4 || ((sizeof(s_ai_scratch_block) == 12))) ? 1 : -1];

// Two 12-byte descriptors and the overflow-pool restoration/initialization flags.
extern "C" {
extern s_ai_scratch_block g_ai_scratch_buffers[2];
extern char g_ai_scratch_flags[];
}

#endif
