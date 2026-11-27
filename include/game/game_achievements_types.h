#ifndef GAME_GAME_ACHIEVEMENTS_TYPES_H
#define GAME_GAME_ACHIEVEMENTS_TYPES_H

enum e_achievement
{
    _achievement_none = -1,

    _achievement_landfall = 0,
    _achievement_holdout,
    _achievement_the_road,
    _achievement_assault,
    _achievement_cleansing,
    _achievement_refuge,
    _achievement_last_stand,
    _achievement_the_key,
    _achievement_return,
    _achievement_campaign_complete_normal,
    _achievement_campaign_complete_heroic,
    _achievement_campaign_complete_legendary,
    _achievement_marathon_man,
    _achievement_guerilla,
    _achievement_demon,
    _achievement_cavalier,
    _achievement_askar,
    _achievement_exterminator,
    _achievement_ranger,
    _achievement_vanguard,
    _achievement_orpheus,
    _achievement_reclaimer,
    _achievement_iron,
    _achievement_black_eye,
    _achievement_tough_luck,
    _achievement_catch,
    _achievement_fog,
    _achievement_famine,
    _achievement_thunderstorm,
    _achievement_tilt,
    _achievement_mythic,
    _achievement_graduate,
    _achievement_unsc_spartan,
    _achievement_spartan_officer,
    _achievement_two_for_one,
    _achievement_mvp,
    _achievement_too_close_to_the_sun,
    _achievement_triple_kill,
    _achievement_overkill,
    _achievement_lee_r_wilson_memorial,
    _achievement_killing_frenzy,
    _achievement_steppin_razor,
    _achievement_mongoose_mowdown,
    _achievement_maybe_next_time_buddy,
    _achievement_up_close_and_personal,
    _achievement_fear_the_pink_mist,
    _achievement_headshot_honcho,
    _achievement_used_car_salesman,
    _achievement_we_re_in_for_some_chop,

    k_number_of_achievements
};

typedef char game_achievements_count_check[(k_number_of_achievements == 49) ? 1 : -1];

// String identifiers indexed by e_achievement.
extern "C" const long k_achievement_name_ids[k_number_of_achievements];

#endif // GAME_GAME_ACHIEVEMENTS_TYPES_H
