#ifndef AI_FIRING_POSITION_REF_H
#define AI_FIRING_POSITION_REF_H

// A reference to one firing position (ai\actor_firing_position.cpp; FP_REF_NONE and FP_REF_CURRENT live there in
// the March 2007 debug build). Passed and compared by value as one word (the asserts compare with
// fp_ref_equals); a set top bit of block marks a dynamic firing position.
#pragma pack(push, 2)
struct firing_position_ref
{
    union
    {
        struct { short index; short block; };
        long value;
    };
};
#pragma pack(pop)

typedef char firing_position_ref_size_must_be_4[(sizeof(firing_position_ref) == 4) ? 1 : -1];

// Data defined in tu/ailibxbox/actor_firing_position.cpp.
extern firing_position_ref FP_REF_NONE;
extern firing_position_ref FP_REF_CURRENT;

#endif // AI_FIRING_POSITION_REF_H
