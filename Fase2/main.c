#include "umps3/umps/aout.h"
#include "umps3/umps/arch.h"
#include "umps3/umps/bios_defs.h"
#include "umps3/umps/cp0.h"
#include "umps3/umps/regdef.h"
#include "umps3/umps/libumps.h"
#include "../Libraries/libraries.h"
#include "../Libraries/syscall.h"
#include "../Libraries/scheduler.h"
#include "../Libraries/interrupt_handler.h"

/* Scheduler related variables */
extern unsigned int processCount;
extern unsigned int softBlockCount;
extern pcb_PTR readyQueue;
extern pcb_PTR currentProcess;

/* Int Array for device semaphores*/
int device_semaphores[SEMAPHORE_QTY]; 

/* Inizialize pass-up-vector with the addressess needed */
HIDDEN passupvector_t* passupvector;

/*placeholder function for exception handling*/
void fooBar();

/* Handler of the syscall excp */
void SyscallExceptionHandler(state_t* exception_state);

/* Include the test function, in order to execute p2test.c */
extern void test();

/*Placeholder function for TLB-Refill*/
extern uTLB_RefillHandler ();

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
        device_semaphores[i] = 0;
    
    LDIT(PSECOND); // carichiamo il valore dell'interval timer con 100 millis

    /* Start the process initialization */
    pcb_PTR proc = allocPcb();
    
    insertProcQ(&(readyQueue), proc);
    
    processCount += 1;
    //we are not initialiazing p_s pointer , and even if we do that how can we accede to it later on ?
    initializePcbt(proc);
    proc->p_time = 0;
    proc->p_semAdd = NULL;
    proc->p_supportStruct = NULL;
    
    // nel 26 esimo (che e' 26 causa umps3 che non conta 2 entry) registro del GPR c'è lo stack pointer 
    // del programma, dobbiamo scrivere li dentro il valore di RAMTOP
    RAMTOP(proc->p_s.reg_sp); // Equivalente di proc->p_s.gpr[26] = ((*((int *)RAMBASEADDR)) + (*((int *)RAMBASESIZE)));

    //PC
    proc->p_s.pc_epc = (memaddr) test; 

    // il primo 1 indica che attiviamo l'interval timer per il processo, e il secondo 
    // che stiamo attivando tutti gli interrupt.
    // 00001000000000000000000000000100 == 134217732
    proc->p_s.status = INIT_STATUS;    //se non va, mettiamo setSTATUS(134217732); 

    proc->p_s.reg_t9 = (memaddr) test; 

    currentProcess = proc;

    //// CALL THE SCHEDULER ////
    scheduler();
}