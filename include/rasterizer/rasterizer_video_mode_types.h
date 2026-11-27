#ifndef RASTERIZER_RASTERIZER_VIDEO_MODE_TYPES_H
#define RASTERIZER_RASTERIZER_VIDEO_MODE_TYPES_H

// The 48-byte XDK video-mode record. Keep the struct identity used by the
// accepted rasterizer_main_get_video_mode signature.
struct XVIDEO_MODE
{
    unsigned int dwDisplayWidth;
    unsigned int dwDisplayHeight;
    int fIsInterlaced;
    int fIsWideScreen;
    int fIsHiDef;
    float RefreshRate;
    unsigned int VideoStandard;
    unsigned int Reserved[5];
};

typedef char rasterizer_video_mode_size_check[(sizeof(XVIDEO_MODE) == 48) ? 1 : -1];

#endif // RASTERIZER_RASTERIZER_VIDEO_MODE_TYPES_H
