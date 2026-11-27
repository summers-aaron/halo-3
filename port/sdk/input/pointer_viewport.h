// Map a client pixel through the centered present rectangle used by Vulkan and D3D12.
// The native 9:5 image represents a 16:9 display except on an exact pixel-for-pixel blit.
#ifndef PORT_INPUT_POINTER_VIEWPORT_H
#define PORT_INPUT_POINTER_VIEWPORT_H

static inline int port_pointer_in_frame(int x, int y, int width, int height, int tw, int th, float *u, float *v)
{
    if (width <= 0 || height <= 0 || tw <= 0 || th <= 0)
        return 0;
    int w = width, h = height;
    if (width != tw || height != th) {
        const int native = (long long)tw * 5 == (long long)th * 9;
        const long long num = native ? 16 : tw, den = native ? 9 : th;
        if (width * den > height * num)
            w = (int)((height * num + den / 2) / den);
        else
            h = (int)((width * den + num / 2) / num);
    }
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    const int left = (width - w) / 2, top = (height - h) / 2;
    if (x < left || x >= left + w || y < top || y >= top + h)
        return 0;
    *u = ((float)(x - left) + 0.5f) / (float)w;
    *v = ((float)(y - top) + 0.5f) / (float)h;
    return 1;
}

#endif
