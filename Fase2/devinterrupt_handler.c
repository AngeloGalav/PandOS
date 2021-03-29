#include "../Libraries/devinterrupt_handler.h"

void InterruptPendingChecker(unsigned int cause_reg)
{
    
    unsigned int ip_reg = BitExtractor(cause_reg, 0xFF00, 8);

    int mask = 2;
    for (int i = 1; i < 8; i++)
    {
        if (ip_reg & mask) 
            InterruptLineDeviceCheck(i);
        mask *= 2;
    }
    
        
}

void InterruptLineDeviceCheck(int line)
{
    unsigned int * device = (memaddr) IDEVBITMAP + (line * 0x4);
    if((line>=3) && (line<=6))
    {
        int mask = 1;
        for (int i = 0; i<8; i++)
        {
            if(*device & mask)
                InterruptHandler(line, i);
            mask *=2;
        }
    }
}

void InterruptHandler(int line, int device)
{
    unsigned int devAddrBase = 0x10000054 + ((line - 3) * 0x80) + (device * 0x10);
}