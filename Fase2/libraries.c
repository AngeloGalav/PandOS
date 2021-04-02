#include "../Libraries/libraries.h"
#include "../Libraries/debugger.h"

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

void setProcessorMode(int mode)
{
    GET_BDP_STATUS(current_status);                 // creates a pointer to the biosdatapage status 
    if (mode) current_status->status &= !(8);   // disables the KUp bit (kernel mode on)
    else current_status->status |= 8;           // enables the KUp bit (user mode on)
}

void *memcpy(void *dest, const void *src, size_t n)
{
    memcpy_start();
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
    memcpy_end();
}

unsigned int BitExtractor(unsigned int reg, int mask, int shift)
{
    reg = reg & mask;
    reg >>= shift;
    return reg;
}

unsigned int GetStatusWord(int intlNo, int dnum, int isReadTerm)
{
    devreg_t* device_register;
    if (2 < intlNo && intlNo < 7)
    {
        device_register = (devreg_t*) (0x10000054 + ((intlNo - 3) * 0x80) + (dnum * 0x10));
        return device_register->dtp.status;
    }
    else
    {
        device_register = (devreg_t*) (0x10000054 + ((intlNo - 3) * 0x80) + (dnum * 0x10));
        if (isReadTerm) return device_register->term.recv_status;
        else return device_register->term.transm_status;
    }
}