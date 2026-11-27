#ifndef SOUND_GAME_SOUND_DEFINITIONS_H
#define SOUND_GAME_SOUND_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/game_sound_definitions.cpp that other units call, and the decibel type they
// take (a struct holding a decibel value, declared here beside its two
// functions until one is).

struct real_decibel
{
    float value;

    // March 2007 sound_manager.obj, 0x8289d1c8/0x8289d220; to_real at 0x82a353f0.
    float to_real() const { return value; }
    real_decibel &operator+=(const real_decibel &other)
    {
        value += other.value;
        return *this;
    }
    real_decibel operator+(const real_decibel &other) const
    {
        real_decibel result = *this;
        return result += other;
    }
    bool operator<(const real_decibel &other) const { return value < other.value; }
    bool operator<=(const real_decibel &other) const { return value <= other.value; }
    // March 2007 sound_manager.obj, 0x8289d270.
    bool operator>(const real_decibel &other) const { return value > other.value; }
};

typedef char real_decibel_size[(sizeof(real_decibel) == 4) ? 1 : -1];

// Shared default for scripted sound gain and distance overrides.
extern "C" const real_decibel k_scripted_sound_default_decibels;

struct real_decibel_volumes
{
    real_decibel v[4];
};

typedef char real_decibel_volumes_size[(sizeof(real_decibel_volumes) == 16) ? 1 : -1];

real_decibel pin_gain_db(real_decibel gain_db);
real_decibel real_gain_to_decibels(float gain);

#endif // SOUND_GAME_SOUND_DEFINITIONS_H
