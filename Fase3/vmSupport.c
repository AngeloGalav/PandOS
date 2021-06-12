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

    //controllo se l'i-esima pagina scelta è valida controllandola dentro la spawpool_table
    if (swap_table[frame].sw_asid != NOPROC)
    {
        int x = swap_table[frame].sw_asid ; // da riscrivere meglio ma è per capire il flow
        //k è la pagina indicata all'indirizzo old_i delal swap table e appartiene al processo
        //con quell'asid e assumiamo  sia "dirty" cioè forse col dirtyu bit acceso
        UNSET_BIT(swap_table[frame].sw_pte->pte_entryLO, VALIDON) ; //bisogna mettere il V bit a 0
    }
    
}


int Get_PageTable_Index(support_t* sPtr)
{
    state_t* BadVADDR ;
    GET_BDP_STATUS(BadVADDR);
    unsigned int BadVaddr = BadVADDR->gpr[CP0_BadVAddr]; 
    for(int i = 0; i < MAXPAGES ; i++)
        if(sPtr->sup_privatePgTbl[i].pte_entryHI == BadVaddr)
            return i; //ritorniamo l'indice della pagina a cui corrisponde l'address nell'entryHi
}

int Round_Robin()
{
    static int frame = 0;
    int old_frame = frame; //scelgo il frame
    frame = (frame % POOLSIZE) + 1;
    return old_frame;
}