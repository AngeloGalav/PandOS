#include "umps3/umps/aout.h"
#include "umps3/umps/arch.h"
#include "umps3/umps/bios_defs.h"
#include "umps3/umps/cp0.h"
#include "umps3/umps/regdef.h"
#include "umps3/umps/libumps.h"
#include "../Libraries/libraries.h"
#include "../Libraries/syscall.h"
#include "../Libraries/scheduler.h"
#include "../Libraries/exception_handler.h"

/* Scheduler related variables */
extern unsigned int processCount;
extern unsigned int softBlockCount;
extern pcb_PTR readyQueue;
extern pcb_PTR currentProcess;

/* Int Array for semaphores*/
HIDDEN int semaphores[SEMAPHORE_QTY];

/* Inizialize pass-up-vector with the addressess needed */
HIDDEN passupvector_t* passupvector;

/* Include the test function, in order to execute p2test.c */
extern void test();

/*Placeholder function for TLB-Refill*/
extern uTLB_RefillHandler();

/* Draft scheduler */
void placeholder_scheduler();

int main()
{
    readyQueue = mkEmptyProcQ();
    initPcbs(); 
    initASL();

    /* Fill up pass-up-vector*/
    passupvector->tlb_refill_handler = (memaddr) uTLB_RefillHandler; 
    passupvector->tlb_refill_stackPtr = (memaddr) 0x20001000;
    passupvector->exception_stackPtr = (memaddr) 0x20001000; // fresh new stack for the exception handler
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
    
    /* Equivalent to proc->p_s.gpr[26] = ((*((int *)RAMBASEADDR)) + (*((int *)RAMBASESIZE))); */
    RAMTOP(proc->p_s.reg_sp);

    //setting PC, in order to execute p2test functions
    proc->p_s.pc_epc = (memaddr) test; 
    proc->p_s.reg_t9 = (memaddr) test;

    // bit 1 indicates that the PLT is enabled for this process, e il secondo 
    // che stiamo attivando tutti gli interrupt.
    // 00001000000000000000000000000100 == 134217732
    proc->p_s.status = INIT_STATUS;    //se non va, mettiamo setSTATUS(134217732); 

    currentProcess = proc; // now the first process is also the current process

    Scheduler();
}