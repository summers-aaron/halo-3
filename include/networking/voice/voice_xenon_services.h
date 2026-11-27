#ifndef NETWORKING_VOICE_VOICE_XENON_SERVICES_H
#define NETWORKING_VOICE_VOICE_XENON_SERVICES_H

extern "C++" {
struct s_voice_hopper_settings {
    long voice_mode;
    long voice_option_40;
    long voice_option_44;
    unsigned char voice_option_48;
    unsigned char teams;
};

extern "C" bool voice_try_and_get_hopper_settings(s_voice_hopper_settings * settings);
}

#endif // NETWORKING_VOICE_VOICE_XENON_SERVICES_H
