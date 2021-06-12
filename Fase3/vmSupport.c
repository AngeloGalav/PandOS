#include "../include/vmSupport.h"
#include "../include/definitions.h"


/* Swap pool table */
swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphores*/
int swap_semaphore; 


void initSwapStructs()
{
   
    swap_semaphore = 1;

    for (int i = 0 ; i < POOLSIZE ; i++)
    {
        swap_table[i].sw_asid = NOPROC ;
    }

}

void Support_Pager() // TLB_exception_Handler andrà richiamato immagino
{
     //We take the support info of the current process with the SYS8
    support_t *sPtr = SYSCALL(GETSPTPTR, 0, 0, 0);
    //extract the execCode to check which exception is happening dio caneporcodiomaiale
    int excCode = GET_EXEC_CODE(sPtr->sup_exceptState[0].cause);

    if (excCode == EXC_MOD) // se è una TLB modification exception la tratto come PROgram trap
        SYSCALL(TERMINATE,0 ,0, 0); 

    SYSCALL(VERHOGEN, (int) &swap_semaphore, 0, 0); // (cast a int inutile )?

    //Andare a prendere l'indirizz della page table address nel CP0 register nel BIOS
    //con quell'indirizzo dentro la page table del current process trovare l'iesima pagina
    //restituire l'indirizzo della pagina
    
    int p = Get_PageTable_Index(sPtr);

    // uso una variabile static che ogni volta andrà ad essere incrementata di uno % POOLSIZE
    // questo perchè quella che prendiamo poi viene "reinserita " quindi sarà la nuova pagina
    
    int frame = Round_Robin();

    int asid = swap_table[frame].sw_asid ;
    //controllo se l'i-esima pagina scelta è valida controllandola dentro la spawpool_table
    if (asid != NOPROC)
    {   
        // DISABLING INTERRUPTS HERE using getStatus -> setStatus
        unsigned int entry = getSTATUS();
        UNSET_BIT(entry, IECON);// gliolevo 
        setSTATUS(entry);//gliorimetto
        
        //risalgo al process x dal frame k
        UNSET_BIT(swap_table[frame].sw_pte->pte_entryLO, VALIDON) ; //bisogna mettere il V bit a 0
     
        //section 4.5.2 pandos
        TLBCLR(); // per il momento facciamo erase del TLB come consigliato, poi andrà cambiato

        devreg_t* devReg = DEV_REG_ADDR(4, asid); // prendo il flash/backing store del process x
        //in DATA0 the starting physical address of the 4k block to be read (or written);
        // the particular frame’s starting address.
        devReg->dtp.data0 = swap_table[frame].sw_pte->pte_entryLO >> PFNSHIFT;
        devReg->dtp.command |= FLASHWRITE  | ; // QUale cazzo è il BLOCKNUMBER ????
        SYSCALL(IOWAIT,0,0,0);
        //check the status and see if an error occurred, if yes generate a program trap
         // ENABLING INTERRUPTS HERE using setStatus
        setSTATUS(IECON);
        
    }
    
}


int Get_PageTable_Index(support_t* sPtr)
{
    //vado a vedere nella memoria logica del processo quale pagina corrisponde
    //così mi rendo conto quale era quella che cercava e che ha scatenato il fault
    state_t* BadVADDR ;
    GET_BDP_STATUS(BadVADDR);
    unsigned int BadVaddr = BadVADDR->gpr[CP0_BadVAddr]; 
    for(int i = 0; i < MAXPAGES ; i++)
        if((sPtr->sup_privatePgTbl[i].pte_entryHI >> VPNSHIFT) == BadVaddr)
            return i; //ritorniamo l'indice della pagina a cui corrisponde l'address nell'entryHi
}

int Round_Robin()
{
    static int frame = 0;
    int old_frame = frame; //scelgo il frame
    frame = (frame % POOLSIZE) + 1;
    return old_frame;
}