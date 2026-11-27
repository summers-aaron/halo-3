#ifndef AI_PROP_REF_FLAGS_H
#define AI_PROP_REF_FLAGS_H
// Dialogue flag at bit 1; perception also names bit 0 salient.
enum e_prop_ref_flags {
    prop_ref_unused = 0,
    _prop_ref_flag_salient = 0,
    prop_ref_dialogue_flag = 1,
    _prop_ref_flag_3 = 3,
    _prop_ref_unboardable_bit = 4
};
#endif
