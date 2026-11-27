#ifndef RENDER_SCREEN_POSTPROCESS_BLUR_H
#define RENDER_SCREEN_POSTPROCESS_BLUR_H

#if !defined(_XBOX) || defined(__clang__)
extern "C" void screen_postprocess_apply_large_blur(float sigma_x, float sigma_y);
#else
extern "C" void screen_postprocess_apply_large_blur(long source, long dest, float sigma_x, float sigma_y);
#endif

#endif // RENDER_SCREEN_POSTPROCESS_BLUR_H
