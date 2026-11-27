// blamlibxbox/byte_swapping.obj: from the records 0x21a128 0x21a188
// @flags /TP /O2 /Oc
// @address 0x21a128 byte_swap_array
// @address 0x21a188 byte_swap_element
#include <ppcintrinsics.h>
#include "memory/byte_swapping.h"
#include "memory/byte_swapping_types.h"

// The swapped files (font packages, font cache pages) are little-endian on disc. The console converts them;
// a little-endian host keeps them as they are, and the walk still measures them.
#if defined(_XBOX) && !defined(__clang__)
#define BYTE_SWAP_HOST_IS_BIG_ENDIAN 1
#else
#define BYTE_SWAP_HOST_IS_BIG_ENDIAN 0
#endif


extern "C" void byte_swap_element(element_type_t *type, char *data, void *definition, int *out_size, int *out_count)
{
    int *codes = (int *)definition;
    int count = codes[1];
    int size = 0;
    int index;
    if (count > 0)
    {
        unsigned int remaining = (unsigned int)count;
        do
        {
            index = 2;
            bool done = false;
            do
            {
                int *element = &codes[index];
                int code = *element;
                switch (code)
                {
                case _byte_swap_2byte:
                    if (BYTE_SWAP_HOST_IS_BIG_ENDIAN && data != 0)
                        *(unsigned short *)(data + size) = _byteswap_ushort(*(unsigned short *)(data + size));
                    size += 2;
                    index++;
                    break;
                case _byte_swap_4byte:
                    if (BYTE_SWAP_HOST_IS_BIG_ENDIAN && data != 0)
                    {
                        unsigned long *p = (unsigned long *)(data + size);
                        *p = _byteswap_ulong(*p);
                    }
                    size += 4;
                    index++;
                    break;
                case _byte_swap_8byte:
                    if (BYTE_SWAP_HOST_IS_BIG_ENDIAN && data != 0)
                    {
                        unsigned long *p = (unsigned long *)(data + size);
                        *(unsigned __int64 *)p = ((unsigned __int64)_byteswap_ulong(p[1]) << 32) | _byteswap_ulong(p[0]);
                    }
                    size += 8;
                    index++;
                    break;
                case _byte_swap_array_start:
                    {
                        int local_size;
                        int local_count;
                        byte_swap_element(type, data != 0 ? data + size : 0, element, &local_size, &local_count);
                        index += local_count;
                        size += local_size;
                    }
                    break;
                case _byte_swap_array_end:
                    done = true;
                    index++;
                    break;
                case _byte_swap_external:
                    {
                        element_type_t *field_type = *(element_type_t **)(element + 1);
                        int local_size;
                        byte_swap_element(field_type, data != 0 ? data + size : 0, field_type->definition, &local_size, 0);
                        index += 2;
                        size += local_size;
                    }
                    break;
                default:
                    if (code > 0)
                    {
                        size += code;
                        index++;
                    }
                    break;
                }
            } while (!done);
            remaining--;
        } while (remaining != 0);
    }
    if (out_size != 0)
        *out_size = size;
    if (out_count != 0)
        *out_count = index;
}

extern "C" void byte_swap_array(element_type_t *type, char *data, int count)
{
    if (BYTE_SWAP_HOST_IS_BIG_ENDIAN && data != 0)
    {
        int index = 0;
        if (count > 0)
        {
            do
            {
                byte_swap_element(type, type->size * index + data, type->definition, 0, 0);
                index++;
            } while (index < count);
        }
    }
}
