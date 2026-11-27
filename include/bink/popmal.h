#ifndef BINK_POPMAL_H
#define BINK_POPMAL_H

struct s_popmalloc_state
{
    unsigned int total;
    unsigned int count;
    void ***destinations;
    unsigned int *sizes;
};
extern "C" {
extern s_popmalloc_state g_popmalloc_state;
unsigned int popmalloctotal(void);
void *popmalloc(int size, int allocator_arg_2, int allocator_arg_3, int allocator_arg_4, int allocator_arg_5, int allocator_arg_6);
}

#endif // BINK_POPMAL_H
