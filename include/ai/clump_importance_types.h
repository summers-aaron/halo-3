#ifndef AI_CLUMP_IMPORTANCE_TYPES_H
#define AI_CLUMP_IMPORTANCE_TYPES_H

enum e_clump_importance
{
    _clump_importance_none = 0,
    _clump_importance_background,
    _clump_importance_low,
    _clump_importance_normal,
    _clump_importance_elevated,
    _clump_importance_high,
    _clump_importance_critical,
    _clump_importance_player,

    k_clump_importance_count
};

#endif // AI_CLUMP_IMPORTANCE_TYPES_H
