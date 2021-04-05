#include "../include/libraries.h"

void setExcCode(state_t* exceptionStatus, unsigned int toSet)
{
    unsigned int to_subtract = exceptionStatus->cause & 124;
    toSet <<= 2;                                    
    exceptionStatus->cause += toSet - to_subtract; // deletes previous ExeCode and adds new one
}

int checkMode(unsigned int status_register)
{
    /*
    unsigned int statuscode = status_register & 8;
    statuscode >>= 3;
    return statuscode; //if the KUp bit is 0 then kernel mode is on
    */
    return BitExtractor(status_register,8,3);
}

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