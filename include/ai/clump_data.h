#ifndef AI_CLUMP_DATA_H
#define AI_CLUMP_DATA_H

extern "C++" {
struct s_data_array;
extern __declspec(thread) s_data_array *clump_data;
}

#endif // AI_CLUMP_DATA_H
