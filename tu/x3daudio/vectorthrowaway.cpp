// x3daudio/vectorthrowaway.obj: from the records 0x13be28 0x13be60 0x13c7f8 0x13d010
// @flags /TP /O2 /Oc
#include <xtl.h>
#include <math.h>

// @address 0x13be28 ?SubtractVector@@YAXPBU_D3DVECTOR@@0PAU1@@Z
void SubtractVector(const _D3DVECTOR *a, const _D3DVECTOR *b, _D3DVECTOR *result)
{
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}

// @address 0x13be60 ?Magnitude@@YAMPBU_D3DVECTOR@@@Z
float Magnitude(const _D3DVECTOR *vector)
{
    double x = vector->x;
    double y = vector->y;
    double z = vector->z;
    double squared_length = x * x + y * y + z * z;
    return (float)sqrt(squared_length);
}

// @address 0x13c7f8 ?CrossProduct@@YAXPBU_D3DVECTOR@@0PAU1@@Z
void CrossProduct(const _D3DVECTOR *a, const _D3DVECTOR *b, _D3DVECTOR *result)
{
    const _D3DVECTOR * b_loc = b;
    const _D3DVECTOR * a_loc = a;
    {
        float by = b_loc->y;
        float bz = b_loc->z;
        float ay = a_loc->y;
        result->x = ay * bz - by * (a_loc->z);
    }
    result->y = a_loc->z * b_loc->x - a_loc->x * b_loc->z;
    {
        float bx = b_loc->x;
        float ay = a_loc->y;
        float ax = a_loc->x;
        float by = b_loc->y;
        result->z = by * ax - ay * bx;
    }
}

// @address 0x13d010 ?DotProduct@@YAMPBU_D3DVECTOR@@0@Z
float DotProduct(const _D3DVECTOR *a, const _D3DVECTOR *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

