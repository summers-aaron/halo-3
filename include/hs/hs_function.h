#ifndef HS_HS_FUNCTION_H
#define HS_HS_FUNCTION_H

// Functions defined in tu/blamlibxbox/hs_function.cpp.

enum e_hs_function_index
{
    _hs_function_begin = 0,
    _hs_function_begin_random = 1,
    _hs_function_if = 2,
    _hs_function_cond = 3,
    _hs_function_set = 4,
    _hs_function_and = 5,
    _hs_function_or = 6,
    _hs_function_add = 7,
    _hs_function_subtract = 8,
    _hs_function_multiply = 9,
    _hs_function_divide = 10,
    _hs_function_min = 11,
    _hs_function_max = 12,
    _hs_function_equal = 13,
    _hs_function_not_equal = 14,
    _hs_function_greater = 15,
    _hs_function_less = 16,
    _hs_function_greater_or_equal = 17,
    _hs_function_less_or_equal = 18,
};

struct hs_function_definition;

void hs_damage_object_list(long object_list_index, long region_name, float damage_amount);
extern "C" void game_tuning_set_value(const char *name, int player, float value);
extern "C" float game_tuning_get_value(const char *name, int player);
extern "C" long ai_scripting_attach_units_evaluate(long object_list_index, long ai_index);
void ai_scripting_detach_units_evaluate(long object_list_index);
extern "C" long hs_players_new_unit_list(void);

// Data defined in tu/blamlibxbox/hs_function.cpp that other units use.
extern const hs_function_definition *hs_function_table[];

#endif // HS_HS_FUNCTION_H
