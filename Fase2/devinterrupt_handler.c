#include "../Libraries/devinterrupt_handler.h"

void InterruptPendingChecker(unsigned int cause_reg)
{
    //(memaddr) INTDEVBITMAP;
    unsigned int ip_reg = BitExtractor(cause_reg, 0xFF00, 8);

    int k = 2;
    for (int i = 1; i < 8; i++)
    {
        if (ip_reg & k) 
            InterruptLineDeviceCheck(i);
        k *= 2;
    }
    
        
}