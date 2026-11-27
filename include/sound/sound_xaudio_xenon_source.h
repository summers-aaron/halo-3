#ifndef SOUND_SOUND_XAUDIO_XENON_SOURCE_H
#define SOUND_SOUND_XAUDIO_XENON_SOURCE_H

// Functions defined in tu/blamlibxbox/sound_xaudio_xenon_source.cpp that other units call.

class c_xenon_audio_source_manager;
struct XAUDIOVOICEPACKETCALLBACK;

extern "C" void xaudio_source_handle_packet_complete(
    c_xenon_audio_source_manager *source_manager, const XAUDIOVOICEPACKETCALLBACK *callback_data);

#endif // SOUND_SOUND_XAUDIO_XENON_SOURCE_H
