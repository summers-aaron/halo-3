#ifndef PHYSICS_COLLISION_BSP_TEST_CONTEXT_H
#define PHYSICS_COLLISION_BSP_TEST_CONTEXT_H

struct structure_bsp;
struct s_structure_bsp_resources;
struct pathfinding_data;

struct s_collision_bsp_test_context
{
    long structure_bsp_index;
    structure_bsp *structure_bsp;
    s_structure_bsp_resources *resources;
    void *unknown;
    pathfinding_data *runtime;
};

void collision_bsp_test_context_initialize(long structure_bsp_index, s_collision_bsp_test_context *context);

#if !defined(_WIN64) && !defined(__LP64__)
typedef char collision_bsp_test_context_size[(sizeof(s_collision_bsp_test_context) == 20) ? 1 : -1];
#endif

#endif // PHYSICS_COLLISION_BSP_TEST_CONTEXT_H
