// blamlibxbox/minidump_secure_writer_helper.obj: from the records 0x205cc0
// @flags /TP /O2 /Oc
// @compiler xdk6130
#include <string.h>

// @address 0x205cc0 minidump_test_memory_readable
extern "C" int minidump_test_memory_readable(const void *memory, unsigned long size)
{
    int result = 0;
    unsigned long stride = 0;
    {
        unsigned long strides[4] = { 8, 4, 2, 1 };
        for (unsigned long i = 0; i < 4; ++i)
        {
            if (size % strides[i] == 0)
            {
                stride = strides[i];
                break;
            }
        }
    }
    if (stride != 0)
    {
        unsigned __int64 buffer = 0;
        __try
        {
            for (unsigned long index = 0; index < size / stride; ++index)
                memmove(&buffer, (const char *)memory + index * stride, stride);
        }
        __except (1)
        {
            result = 1;
        }
    }
    else
    {
        result = 1;
    }
    return result;
}
