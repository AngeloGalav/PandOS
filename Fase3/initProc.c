#include "../include/initProc.h"
#include "../include/definitions.h"
#include "../include/vmSupport.h"
#include "../include/sysSupport.h"

/* U-proc state and support structs */
HIDDEN support_t U_support_structure[UPROCMAX];
HIDDEN state_t U_state_structure[UPROCMAX];

/* Support semaphores matrix */
int support_devsemaphores[SUPP_SEM_N][UPROCMAX]; // line 0 is for printer, 1 write_terminal, 2 read_terminal 
int flash_device_semaphores[UPROCMAX]; // array of semaphores dedicated to the backing store/flash devices of each uproc

/* Swap pool table */
extern swap_t swap_table[POOLSIZE];

int masterSemaphore;

/* Swap pool devices semaphores*/
extern int swap_semaphore; 

extern void Support_Pager();

extern void GeneralException_Handler();

extern int processCount;

void test(){
    initSwapStructs();
    initSemaphores();

    for (int i = 0; i < UPROCMAX; i++)
        initProcess(i);
    
    //se la lista dei figli di test è vuota si ammazza
    // bp_PAGE_NOT_FOUND();

    for (int i = 0; i < UPROCMAX; i++) {
        SYSCALL(PASSEREN, (int) &masterSemaphore, 0 ,0); // in this way, the 'test' process will only wake up  
    }                                              // when every other UPROC is over. So it's sleeping 8 times.

    SYSCALL(TERMPROCESS, 0, 0, 0);
}

void initSemaphores()
{
    masterSemaphore = 0;

    /* Set all semaphores to 1 cause of mutex */
    for (int i = 0; i < UPROCMAX; i++){
        for (int j = 0; j < SUPP_SEM_N; j++) 
            support_devsemaphores[j][i] = 1;
        
        flash_device_semaphores[i] = 1;
    }
}

void initProcess(int id)
{       
    /* --- Initialization of the processor state structure --- */

    U_state_structure[id].pc_epc =  (U_state_structure[id].reg_t9 = UPROCSTARTADDR);
    U_state_structure[id].reg_sp = USERSTACKTOP;

    // interrupt enable, usermode, local timer enabled.
    // IMON == accepts all interrupts (only if the bit is on, then the interrupt is accepted).
    // Set IEp and KUp because when doing a LDST on the processor state, the stack is popped, and 
    // the bits become representative of the current status.
    
    U_state_structure[id].status = IMON | IEPON | USERPON | TEBITON ;
    U_state_structure[id].entry_hi = (id + 1) << ASIDSHIFT;

    /* --- Initialize u'proc support structure --- */

    // ASID are 1..8
    U_support_structure[id].sup_asid = id + 1; 

    // PC set to the pager (TLB handler) and General Exception Handler respectively
    U_support_structure[id].sup_exceptContext[PGFAULTEXCEPT].pc = (memaddr) Support_Pager;
    U_support_structure[id].sup_exceptContext[GENERALEXCEPT].pc = (memaddr) GeneralException_Handler;

    // kernel-mode on (KUp set to 0), Interrupt on (IEPON, IMON) and PLT on
    U_support_structure[id].sup_exceptContext[PGFAULTEXCEPT].status = IMON | IEPON | TEBITON ;
    U_support_structure[id].sup_exceptContext[GENERALEXCEPT].status = IMON | IEPON | TEBITON ;

    /* --- Initialize u'proc support structure --- */

    // Set the two SP fields to utilize the two stack spaces allocated in the Support Structure.
    memaddr ramTop;
    RAMTOP(ramTop);
    U_support_structure[id].sup_exceptContext[PGFAULTEXCEPT].stackPtr = ramTop - (id + 1) * PAGESIZE * 2;
    U_support_structure[id].sup_exceptContext[GENERALEXCEPT].stackPtr = ramTop - (id + 1) * PAGESIZE * 2 + PAGESIZE;

    // U_support_structure[id].sup_exceptContext[PGFAULTEXCEPT].stackPtr = (memaddr) &U_support_structure[id].sup_stackTLB[499];
    // U_support_structure[id].sup_exceptContext[GENERALEXCEPT].stackPtr =  (memaddr) &U_support_structure[id].sup_stackGen[499];

    // initialization of the process private PageTable

    for (int j = 0; j < MAXPAGES; j++)
    {
        if (j == MAXPAGES - 1) // the last is the stack page, so VPN is set to the starting address
            U_support_structure[id].sup_privatePgTbl[j].pte_entryHI = 0xBFFFF << VPNSHIFT;
        else
            // set the VPN to [0x80000..0x8001E]
            U_support_structure[id].sup_privatePgTbl[j].pte_entryHI = PAGETBLSTART + (j << VPNSHIFT); // pageNumbers go from 0 to 31 (31 is the stack page)
        
        SET_ASID(U_support_structure[id].sup_privatePgTbl[j].pte_entryHI, id + 1); // id + 1 = ASID
        U_support_structure[id].sup_privatePgTbl[j].pte_entryLO = DIRTYON;

        // valid bit is already set to off...
    }

    // starting the process
    int status = SYSCALL(CREATEPROCESS, (int) &U_state_structure[id], (int) &U_support_structure[id], 0);   
}