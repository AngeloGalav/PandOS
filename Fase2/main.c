#include "umps3/umps/aout.h"
#include "umps3/umps/arch.h"
#include "umps3/umps/bios_defs.h"
#include "umps3/umps/cp0.h"
#include "umps3/umps/regdef.h"
#include "umps3/umps/libumps.h"
#include "../include/libraries.h"
#include "../include/syscall.h"
#include "../include/scheduler.h"
#include "../include/exception_handler.h"
#include "../include/debugger.h"

/* Tail pointer to a queue of pcbs that are in the “ready” state. */
pcb_PTR readyQueue;

/** Number of started, but not terminated processes that in are the
*   “blocked” state  due to an I/O or timer request 
*/
int softBlockCount;

/* Number of started, but not yet terminated processes. */
int processCount;

/* Pointer to the current pcb that is in running state */ 
pcb_PTR currentProcess;

/* Int Array for device semaphores*/
int device_semaphores[SEMAPHORE_QTY]; 

/* The initial processor state */
HIDDEN state_t initialial_state;

/* Inizialize pass-up-vector with the addressess needed */
HIDDEN passupvector_t* passupvector;

/* Include the test function, in order to execute p2test.c */
extern void test();

/* Placeholder function for TLB-Refill */
extern void uTLB_RefillHandler();

int main()
{
    softBlockCount = 0;
    processCount = 0;

    readyQueue = mkEmptyProcQ();
    initPcbs(); 
    initASL();

    passupvector = (passupvector_t*) PASSUPVECTOR;
    
    /* Filling up the pass-up-vector*/
    passupvector->tlb_refill_handler = (memaddr) uTLB_RefillHandler; 
    passupvector->tlb_refill_stackPtr = (memaddr) KERNELSTACK;
    passupvector->exception_stackPtr = (memaddr) KERNELSTACK; // fresh new stack for the exception handler
    passupvector->exception_handler = (memaddr) exceptionHandler; // exception handling function callback

    for(int i = 0; i < SEMAPHORE_QTY; i++)
        device_semaphores[i] = 0;
    
    LDIT(PSECOND); // loading Interval Timer with 1000 millis

    /* Start the process initialization */
    pcb_PTR proc = allocPcb();
    initializePcb(proc);
    
    processCount += 1;

    proc->p_time = 0;
    proc->p_semAdd = NULL;
    proc->p_supportStruct = NULL;

    /* setting up the first process state */
	STST(&initialial_state);			/* create a state area */	
	
    /* Equivalent to proc->p_s.gpr[26] = ((*((int *)RAMBASEADDR)) + (*((int *)RAMBASESIZE))); */
    RAMTOP(initialial_state.reg_sp);

	initialial_state.pc_epc = initialial_state.reg_t9 = (memaddr) test; /* setting PC, in order to execute p2test functions */
	initialial_state.status =   IEPON | IMON | TEBITON; /* enabling PLT and interrupts */
    
    proc->p_s = initialial_state;

    currentProcess = NULL; 
    insertProcQ(&(readyQueue), proc); /* inserting the current process in the readyQueue so that it can be called later */

    Scheduler();

    return 0;
}