// tmp/probe/extra (GAME-CHANGES F50): normalize3d under C linkage. ai_vehicles.cpp and props.cpp declare it
// extern "C" (props_battle_vector), the rest of the source C++ (real_math.h); retail has one function (0x9b308, the
// C++ one is its out-of-line copy). Its own unit: the C and C++ declarations cannot meet in one translation unit.
#include <math.h>
union real_vector3d;
struct probe_vector3d { float i, j, k; };
extern "C" float normalize3d(real_vector3d *vector_)
{
    probe_vector3d *vector = (probe_vector3d *)vector_;
    const float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (fabsf(length) < 0.0001f)
        return 0.0f;
    const float inverse = 1.0f / length;
    vector->i *= inverse;
    vector->j *= inverse;
    vector->k *= inverse;
    return length;
}
