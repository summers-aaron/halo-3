#ifndef EFFECTS_EFFECT_PENDING_POOL_H
#define EFFECTS_EFFECT_PENDING_POOL_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "tag_files/tag_index.h"
#include "objects/object_reference_types.h"

struct effects_async_payload {
    long object_index;
    unsigned char lightprobe_slot, user_mask;
    real_point3d point;
    real_vector3d forward, up;
    float scale_a, scale_b;
    s_location location_hint;
};
struct s_effect_pool_node {
    s_effect_pool_node *free;
    union { char storage[76]; struct { long reserved; c_tag_index definition_index; effects_async_payload creation; }; };
    s_effect_pool_node() {}
};
struct __declspec(align(8)) s_effect_pool_slist { unsigned __int64 value; };
struct s_effect_pool {
    s_effect_pool_node nodes[128];
    s_effect_pool_slist pending;
    s_effect_pool_slist free_list;
    bool initialized;
    long free_count;
};
extern "C" s_effect_pool g_effect_pending_pool;

#if defined(_XBOX)
typedef char s_effect_pool_node_size_check[(sizeof(void *) != 4 || ((sizeof(s_effect_pool_node) == 80))) ? 1 : -1];
typedef char s_effect_pool_pending_check[(sizeof(void *) != 4 || ((offsetof(s_effect_pool, pending) == 0x2800))) ? 1 : -1];
typedef char s_effect_pool_free_list_check[(sizeof(void *) != 4 || ((offsetof(s_effect_pool, free_list) == 0x2808))) ? 1 : -1];
typedef char s_effect_pool_initialized_check[(sizeof(void *) != 4 || ((offsetof(s_effect_pool, initialized) == 0x2810))) ? 1 : -1];
typedef char s_effect_pool_size_check[(sizeof(void *) != 4 || ((sizeof(s_effect_pool) == 0x2818))) ? 1 : -1];
#endif

#endif // EFFECTS_EFFECT_PENDING_POOL_H
