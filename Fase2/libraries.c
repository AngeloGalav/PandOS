#include "../include/libraries.h"

void memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}

unsigned int BitExtractor(unsigned int reg, int mask, int shift)
{
    reg = reg & mask;
    reg >>= shift;
    return reg;
}