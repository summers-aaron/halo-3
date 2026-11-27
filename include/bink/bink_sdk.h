#ifndef BINK_BINK_SDK_H
#define BINK_BINK_SDK_H

struct s_bink_info;
struct s_bink_frame_buffers;
struct s_bink_info;

// Bink/RAD entry points used by the playback runtime.
extern "C" {
void BinkSetMemory(void *(*allocator)(unsigned int), void (*deallocator)(void *));
void BinkClose(s_bink_info *bink);
void BinkOpenXAudio();
void BinkSetSoundSystem(void (*)(), long);
void BinkSetSoundTrack(unsigned int, const int *);
s_bink_info *BinkOpen(const void *, unsigned long);
void BinkSetMixBins(s_bink_info *, int, int *, int);
void BinkGetFrameBuffersInfo(s_bink_info *, s_bink_frame_buffers *);
void bink_register_frame_buffers(s_bink_info *, s_bink_frame_buffers *);
void bink_set_volume(s_bink_info *bink, long channel, long volume);
void GotoFrame(s_bink_info *, unsigned long);
void BinkSetSoundOnOff(s_bink_info *, long);
void BinkDoFrame(s_bink_info *);
void BinkNextFrame(s_bink_info *);
void RADCB_suspend_callback(long, long *);
void RADCB_resume_callback(long, long *);
long BinkWait(s_bink_info *);

unsigned long __stdcall XEnableScreenSaver(int enable);

// Platform and Bink library entry points used during playback.
unsigned long XMPRestoreBackgroundMusic(void);
unsigned long XMPOverrideBackgroundMusic(void);
long XAudioGetSpeakerConfig(unsigned long *speaker_config);
unsigned long BinkGetKeyFrame(s_bink_info *, unsigned long, long);
}

#endif // BINK_BINK_SDK_H
