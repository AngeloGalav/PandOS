#include "../include/initProc.h"

/* U-proc state and support structs */
HIDDEN support_t U_support_structure[UPROCMAX];
HIDDEN state_t U_state_structure[UPROCMAX];
int supstackTLB[500];
int supstackGen[500];


/* Swap pool table */
swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphores*/
int swap_semaphores[UPROCMAX]; // 16 semaphores cause every process has 2 of them ? like in phase 2?
int mastersemaphore = 1;

void test()
{
    initSupportStructs();
    UProcInitiliazer();

    ///TODO: [initialize create ]
}

void UProcInitiliazer()
{


    for (int i = 0; i < UPROCMAX; i++)
    // e vaffanculo porcodio
        SYSCALL(1, (int) &U_state_structure[i], (int) &U_support_structure[i], 0);
        

}


void initSupportStructs()
{
    /* Set all semaphores to 1 cause of mutex */
    for(int i = 0; i < UPROCMAX; i++)
    {
        swap_semaphores[i] = 1;
    }

    /* Initialize u'proc processor state */
    unsigned int asid;
    for(int i = 0; i < UPROCMAX; i++)
    {
        U_state_structure[i].pc_epc =  U_state_structure->reg_t9 =  (memaddr) UPROCSTARTADDR ;
        U_state_structure[i].reg_sp = (memaddr) USERSTACKTOP;
        //in order : all interrupts, user-mode and first bit, local timer enabled
        U_state_structure[i].status = IMON | 0X00000003 | TEBITON ;
        asid = i + 1;
        asid <<= ASIDSHIFT;
        U_state_structure[i].entry_hi =  U_state_structure[i].entry_hi | asid;
       
    }

    /* Initialize u'proc support structure */

    for(int i = 0; i < UPROCMAX; i++)
    {
        //only  ASID , exceptContent  and privatePgTbl must be set before SYS1 call !
        U_support_structure[i].sup_asid = i + 1 ; 

        U_support_structure[i].sup_exceptContext[0].pc =  PGFAULTEXCEPT;
        U_support_structure[i].sup_exceptContext[1].pc = GENERALEXCEPT ;

        //in order, all interrupts, usermode - first bit (if 1 interrupts are counted as valid),
        //please check if this OR is right, page 9 Pops
        U_support_structure[i].sup_exceptContext[0].status = IMON | 0X00000001 | TEBITON ;
        U_support_structure[i].sup_exceptContext[1].status = IMON | 0X00000001 | TEBITON ;

        //Set the two SP fields to utilize the two stack spaces allocated in the Support Structure.
        //it seems those are two int arrays that describe two memory's areas
        U_support_structure[i].sup_exceptContext[0].stackPtr = &(supstackTLB[499]) ;
        U_support_structure[i].sup_exceptContext[1].stackPtr = &(supstackGen[499]);

        //pops 6.3.2 tells us how to set a page table entry, is like TLB entry so follow it
        //we must set VON, ASID, V and D bits
        int j;
        unsigned int VPN;

        for(j = 0;j < 31; j ++)
        {
            //set the VPN to [0x80000..0x8001E]
            //PLEASE CREATE A MACRO OR A FUNCTION THAT DOES THIS !!!!!!!!
            VPN = 0x80000 + j;
            VPN <<= VPNSHIFT ;
            U_support_structure[i].sup_privatePgTbl[j].pte_entryHI = 
            U_support_structure[i].sup_privatePgTbl[j].pte_entryHI | VPN ;
            //TO-DO set the ASID and all the bits for the 31 pages

        }
        //the last is the stack page and it's apart from the others
        //PLEASE CREATE A MACRO OR A FUNCTION THAT DOES THIS !!!!!!!!
        VPN &= 0;
        VPN = 0xBFFFF;
        VPN <<= VPNSHIFT;
        U_support_structure[i].sup_privatePgTbl[31].pte_entryHI = 
        U_support_structure[i].sup_privatePgTbl[31].pte_entryHI | VPN ;
        


    }
    
    
    /*if(tony arbano == coglione)
        kernel panic() */
}