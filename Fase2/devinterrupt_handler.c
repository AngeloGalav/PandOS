#include "../Libraries/devinterrupt_handler.h"

void InterruptPendingChecker(unsigned int cause_reg)
{
    //(memaddr) INTDEVBITMAP;
    unsigned int ip_reg = BitExtractor(cause_reg, 0xFF00, 8);
        
}