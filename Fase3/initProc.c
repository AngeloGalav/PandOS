#include "../include/initProc.h"
#include "../include/definitions.h"

/* U-proc state and support structs */
HIDDEN support_t U_support_structure[UPROCMAX];
HIDDEN state_t U_state_structure[UPROCMAX];
int supstackTLB[500];
int supstackGen[500];

/* Support semaphores matrix */
int support_semaphores[SUPP_SEM_N][UPROCMAX]; // line 0 is for printer, 1 write_terminal, 2 read_terminal 

/* Swap pool table */
extern swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphores*/
extern int swap_semaphore; 

/* Init swap_pool sempahores */
extern void initSwapStructs();

extern int processCount;

void test()
{
    initSupportStructs();
    UProcInitiliazer();
    
    //se la lista dei figli di test è vuota si ammazza
    if (processCount <= 1)
        SYSCALL(TERMPROCESS, 0, 0, 0);

    
}

void UProcInitiliazer()
{
    for (int i = 0; i < UPROCMAX; i++)
        SYSCALL(1, (int) &U_state_structure[i], (int) &U_support_structure[i], 0);   

}


void initSupportStructs()
{   

    initSwapStructs();
    /* Set all semaphores to 1 cause of mutex */
    for (int i = 0; i < UPROCMAX; i++)
    {

        for (int j = 0; j < SUPP_SEM_N; j++) support_semaphores[j][i] = 1;
        
    }

    /* Initialize u'proc processor state */

    for (int i = 0; i < UPROCMAX; i++)
    {
        U_state_structure[i].pc_epc =  U_state_structure->reg_t9 =  (memaddr) UPROCSTARTADDR ;
        U_state_structure[i].reg_sp = (memaddr) USERSTACKTOP;
        //in order : all interrupts, user-mode and first bit, local timer enabled
        U_state_structure[i].status = IMON | 0X00000003 | TEBITON ;
        SET_ASID(U_state_structure[i].entry_hi,i +1);
    }

    /* Initialize u'proc support structure */
    //only  ASID , exceptContent  and privatePgTbl must be set before SYS1 call !

    for (int i = 0; i < UPROCMAX; i++)
    {
        U_support_structure[i].sup_asid = i + 1 ; 

        U_support_structure[i].sup_exceptContext[0].pc = PGFAULTEXCEPT;
        U_support_structure[i].sup_exceptContext[1].pc = GENERALEXCEPT;

        //in order, all interrupts, usermode - first bit (if 1 interrupts are counted as valid),
        //please check if this OR is right, page 9 Pops
        U_support_structure[i].sup_exceptContext[0].status = IMON | 0X00000001 | TEBITON ;
        U_support_structure[i].sup_exceptContext[1].status = IMON | 0X00000001 | TEBITON ;

        //Set the two SP fields to utilize the two stack spaces allocated in the Support Structure.
        U_support_structure[i].sup_exceptContext[0].stackPtr = &(supstackTLB[499]);
        U_support_structure[i].sup_exceptContext[1].stackPtr = &(supstackGen[499]);

        //pops 6.3.2 tells us how to set a page table entry, is like TLB entry so follow it
        //we must set VPN, ASID, V and D bits
        int j;

        for (j = 0;j < 31; j ++)
        {
            //set the VPN to [0x80000..0x8001E]
            SET_VPN(U_support_structure[i].sup_privatePgTbl[j].pte_entryHI, 0x80000 + j);
            SET_ASID(U_support_structure[i].sup_privatePgTbl[j].pte_entryHI, i + 1);
            SET_D_AND_G(U_support_structure[i].sup_privatePgTbl[j].pte_entryLO);
        }

        //the last is the stack page and it's apart from the others
        SET_VPN(U_support_structure[i].sup_privatePgTbl[31].pte_entryHI, 0xBFFFF);
        SET_ASID(U_support_structure[i].sup_privatePgTbl[31].pte_entryHI, i + 1);
        SET_D_AND_G(U_support_structure[i].sup_privatePgTbl[31].pte_entryLO);
        
    }
    
    
    /* If(tony arbano == coglione)
        kernel panic() */
}