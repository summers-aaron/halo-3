#ifndef BINK_BINKXA_H
#define BINK_BINKXA_H

struct XAUDIOSOURCEVOICE {
    unsigned int handle;
};
struct SNDCHANNEL {
    XAUDIOSOURCEVOICE voice;
    char private_data[88];
    unsigned char *buffer;
    unsigned int buffer_size;
    char parameters[8];
    int volume;
    int pan;
    int paused;
    int closing;
    unsigned int write_position;
    unsigned int locked_bytes;
    unsigned int initial_bytes;
    int : 32;
    int : 32;
    volatile unsigned int start_time;
    unsigned int reserved_time;
    int silence;
};
struct BINKSND {
    char private_data[sizeof(void *) == 8 ? 160 : 100];
    int on;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    union {
        SNDCHANNEL channel;
        struct {
            char channel_prefix[108];
            int volume;
            int pan;
            char playback_state[40];
        };
    };
    unsigned int mix_bins_enabled;
    unsigned int reserved;
    unsigned int mix_bin_count;
    unsigned int reserved_mix;
    unsigned int mix_bins[12];
};
struct XMAPACKETPOSITION {
    unsigned long ReadOffset;
    unsigned short SampleIndex;
};
union XAUDIOPACKETPOSITION {
    unsigned long Position;
    XMAPACKETPOSITION XMAPosition[6];
};
extern "C" {
// match: 0x1380e0: SDK header adoption is blocked by this unit's current types/API arguments: 'XAUDIOPACKETPOSITION' : 'union' type redefinition.
long XAudioSourceVoice_Stop(XAUDIOSOURCEVOICE voice, unsigned char immediate);
// match: 0x137bd8: SDK header adoption is blocked by this unit's current types/API arguments: 'XAUDIOPACKETPOSITION' : 'union' type redefinition.
int XAudioVoice_GetVoiceState(int voice, unsigned char *state);
// match: 0x6c02d0: SDK header adoption is blocked by this unit's current types/API arguments: 'XAUDIOPACKETPOSITION' : 'union' type redefinition.
int XAudioSourceVoice_SetPacketPosition(int voice, const XAUDIOPACKETPOSITION *position);
// match: 0x13ec40: SDK header adoption is blocked by this unit's current types/API arguments: 'XAUDIOPACKETPOSITION' : 'union' type redefinition.
int XAudioSourceVoice_Start(int voice, unsigned char flags);
}

void start_streaming(SNDCHANNEL *channel);
int Unlock(BINKSND *sound, unsigned int byte_count);

struct BINK;
void set_rate_volume_and_pan(BINKSND *sound);
int Open(BINKSND *sound, unsigned int frequency, int bits, int channels,
         unsigned int flags, BINK *bink);
typedef int (*BINKSNDOPEN)(BINKSND *, unsigned int, int, int, unsigned int, BINK *);


extern "C" {
unsigned long XAudioVoice_Release(int voice);
extern int g_bink_xaudio_voice_count;
}

#endif
