#ifndef AI_PUREFORM_TYPES_H
#define AI_PUREFORM_TYPES_H

// Each pureform selects its matching morph-definition slot.
enum e_pureform
{
    _pureform_none = -1,
    _pureform_ranged = 0,
    _pureform_tank = 1,
    _pureform_stalker = 2,
    k_pureform_count = 3
};

enum e_morph_priority
{
    _morph_priority_none = 0
};

#endif // AI_PUREFORM_TYPES_H
