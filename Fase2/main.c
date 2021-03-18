#include "../Libraries/pandos_const.h"
#include "../Libraries/pandos_types.h"
#include "umps3/umps/aout.h"
#include "umps3/umps/arch.h"
#include "umps3/umps/bios_defs.h"
#include "umps3/umps/cp0.h"
#include "umps3/umps/regdef.h"
#include "umps3/umps/libumps.h"
#include "../Libraries/pcb.h"
#include "../Libraries/asl.h"
#include "../Libraries/libraries.h"
#include "../Libraries/definitions.h"

/* Include the test function */
extern void test();

/* Number of started, but not yet terminated processes. */
HIDDEN unsigned int processCount = 0;

/* Number of started, but not terminated 
processes that in are the “blocked” state 
due to an I/O or timer request */
HIDDEN unsigned int softBlockCount = 0;

/* Tail pointer to a queue of pcbs that are in the “ready” state. */
HIDDEN pcb_PTR readyQueue;

/* Pointer to the current pcb that is in running state */ 
HIDDEN pcb_PTR currentProcess = NULL;

/* Int Array for semaphores*/
HIDDEN int semaphores[SEMAPHORE_QTY];

/* Inizialize pass-up-vector with the addressess needed */
HIDDEN passupvector_t* passupvector;

/*Placeholder function for TLB-Refill*/
void uTLB_RefillHandler ();

/*placeholder function for exception handling*/
void fooBar();

/* Draft scheduler */
void placeholder_scheduler();

int main()
{

    readyQueue = mkEmptyProcQ();
    initPcbs(); 
    initASL();

    /* Fill up pass-up-vector*/
    passupvector->tlb_refill_handler = (memaddr) uTLB_RefillHandler; //Codice già dato nel p2test.c
    passupvector->tlb_refill_stackPtr = (memaddr) 0x20001000;
    passupvector->exception_stackPtr = (memaddr) 0x20001000;
    passupvector->exception_handler = (memaddr) fooBar; // exception handling function callback

    for(int i = 0; i < SEMAPHORE_QTY; i++)
        semaphores[i] = 0;
    
    LDIT(PSECOND); // carichiamo il valore dell'interval timer con 100 millis

    /* Start the process initialization */
    pcb_PTR proc = allocPcb();
    
    insertProcQ(&(readyQueue), proc);
    
    processCount += 1;

    initializePcbt(proc);
    proc->p_time = 0;
    proc->p_semAdd = NULL;
    proc->p_supportStruct = NULL;
    
    // nel 26 esimo (che e' 26 causa umps3 che non conta 2 entry) registro del GPR c'è lo stack pointer 
    // del programma, dobbiamo scrivere li dentro il valore di RAMTOP
    // ((*((int *)RAMBASEADDR)) + (*((int *)RAMBASESIZE))))

    RAMTOP(proc->p_s.gpr[26]); // Equivalente di proc->p_s.gpr[26] = ((*((int *)RAMBASEADDR)) + (*((int *)RAMBASESIZE)));

    //PC
    proc->p_s.pc_epc = (memaddr) test; 

    // il primo 1 indica che attiviamo l'interval timer per il processo, e il secondo 
    // che stiamo attivando tutti gli interrupt.
    // 00001000000000000000000000000100 == 134217732
    proc->p_s.status = INIT_STATUS;    //se non va, mettiamo setSTATUS(134217732); 

    proc->p_s.gpr[24] = (memaddr) test; 

    //// CALL THE SCHEDULER ////
    placeholder_scheduler();
}

void placeholder_scheduler()
{
    if (!emptyProcQ(readyQueue))
    {
        currentProcess = removeProcQ(&readyQueue);
        int status = setTIMER(TIMERVALUE(5000));
        LDST ((state_t *) &(currentProcess->p_s)); // 

        //IF BLOCKING SYSCALL PUT THE PROCESS IN THE EVENT QUEUE


        //ELSE IF NON-BLOCKING SYSCALL PUT THE PROCESS IN THE READY QUEUE WITH ROUNDROBIN


    } else // IF  READY QUEUE IT'S EMPTY
    {
        if ( processCount == 0)
        {
            //TO-DO we should be in KERNEL MODE
            HALT();
        }
        else if ((processCount && softBlockCount) > 0)
        {
            setSTATUS(WAIT_STATUS);

            WAIT();
        }
        else if ((softBlockCount == 0) && (processCount > 0))
        {
            //DEADLOCK YOU ARE FCKD
            PANIC();

        }
    }
}



void uTLB_RefillHandler () {
		
	setENTRYHI(0x80000000);
	setENTRYLO(0x00000000);
	TLBWR();	
	
	LDST ((state_t *) 0x0FFFF000);
}



