// port/sdk/audio (both hosts): the XAudio debug switches and the music player (XMP). XAudioInitialize and the rest
// of the engine are xaudio_engine.cpp; the title's own music is not played (no XMP playlist on a PC), so the player
// reports itself idle.
typedef unsigned int port_dword;

void PORT_MS_ABI XAudioSetDebugFlags(port_dword flags)
{
    (void)flags;
}

void PORT_MS_ABI XAudioSetDebugPrintLevel(int level)
{
    (void)level;
}

port_dword PORT_MS_ABI XMPGetStatus(int *status)
{
    if (status != 0)
        *status = 0; // XMP_STATE_IDLE
    return 0;
}
