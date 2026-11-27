#ifndef INCLUDE_PHYS_HALF_H
#define INCLUDE_PHYS_HALF_H

#include <string.h>

class phys_half {
public:
    inline phys_half() {}
    inline phys_half &operator=(const float &f) {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x642058: a word copy moves the second rotation threshold load ahead of the first threshold store.
        int bits = ((const int *)&f)[0];
#else
        unsigned int bits;
        memcpy(&bits, &f, sizeof(bits));
#endif
        m_value = short(bits >> 16);
        return *this;
    }
    inline operator float() const {
        union {
            unsigned int i;
            float f;
        } u;
        u.i = static_cast<unsigned int>(m_value) << 16;
        return u.f;
    }

private:
    short m_value;
};


typedef char phys_half_size[(sizeof(phys_half) == 2) ? 1 : -1];

#endif
