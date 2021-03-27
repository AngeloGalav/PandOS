#include "../Libraries/libraries.h"

void setExcCode(state_t* exceptionStatus, unsigned int toSet)
{
    unsigned int to_subtract = exceptionStatus->cause & 124;
    toSet <<= 2;                                    
    exceptionStatus->cause += toSet - to_subtract; // deletes previous ExeCode and adds new one
}

int checkMode(unsigned int status_register)
{
    unsigned int statuscode = status_register & 8;
    statuscode >>= 3;
    return statuscode; //if the KUp bit is 0 then kernel mode is on
}

void setProcessorMode(int mode)
{
    GET_STATUS(current_status);                 // creates a pointer to the biosdatapage status 
    if (mode) current_status->status &= !(8);   // disables the KUp bit (kernel mode on)
    else current_status->status |= 8;           // enables the KUp bit (user mode on)
}

void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}