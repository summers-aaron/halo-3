#ifndef STRUCTURES_STRUCTURE_PORTAL_REFERENCE_TYPES_H
#define STRUCTURES_STRUCTURE_PORTAL_REFERENCE_TYPES_H

// bsp_index is the top 6 bits of the short on the console (bit-fields from the
// most significant bit); x86 allocates from the least significant, so clang declares them in reverse.
struct s_portal_reference
{
    union
    {
        struct
        {
#if defined(_XBOX) && !defined(__clang__)
            short bsp_index : 6;
            short portal_index : 10;
#else
            short portal_index : 10;
            short bsp_index : 6;
#endif
        };
        short value;
    };
};

typedef char s_portal_reference_size_assert[(sizeof(s_portal_reference) == 0x2) ? 1 : -1];

#endif // STRUCTURES_STRUCTURE_PORTAL_REFERENCE_TYPES_H
