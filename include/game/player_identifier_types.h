#ifndef GAME_PLAYER_IDENTIFIER_TYPES_H
#define GAME_PLAYER_IDENTIFIER_TYPES_H

#include <string.h>

struct s_player_identifier {
    union {
        unsigned char data[8];
        unsigned char bytes[8];
    };

    // March 836610c0: tests the complete eight-byte identifier against zero.
    __forceinline bool is_empty() const { return *reinterpret_cast<const unsigned __int64 *>(data) == 0; }
};

__forceinline bool player_identifier_compare(const s_player_identifier *a, const s_player_identifier *b)
{
    return 0 == memcmp(a, b, sizeof(s_player_identifier));
}

struct s_machine_identifier {
    union {
        unsigned char data[6];
        unsigned char bytes[6];
    };
};

#endif // GAME_PLAYER_IDENTIFIER_TYPES_H
