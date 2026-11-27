#ifndef CSERIES_XMEM_H
#define CSERIES_XMEM_H
#include <stddef.h>
extern "C" {
void *XMemCpy(void *destination, const void *source, size_t size);
void *XMemSet(void *destination, int value, size_t size);
}
#endif
