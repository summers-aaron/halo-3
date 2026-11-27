#ifndef SOUND_SOUND_XAUDIO_XENON_PACKET_CONTROLLER_DEFAULT_H
#define SOUND_SOUND_XAUDIO_XENON_PACKET_CONTROLLER_DEFAULT_H

class c_platform_source_datum;
struct s_platform_source_packet;

class c_platform_source_packet_controller_default
{
public:
    static bool acquire_packet(c_platform_source_datum *owner, s_platform_source_packet *out_packet);
};

#endif // SOUND_SOUND_XAUDIO_XENON_PACKET_CONTROLLER_DEFAULT_H
