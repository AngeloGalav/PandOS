#include "../Libraries/pcb.h"
#include "../Libraries/asl.h"
#include "../Libraries/libraries.h"
#include "../Libraries/definitions.h"
#include "../Libraries/syscall.h"

extern pcb_PTR readyQueue;

extern pcb_PTR currentProcess;

extern unsigned int processCount;
extern unsigned int softBlockCount;
extern int device_semaphores[SEMAPHORE_QTY];

void SYS1(state_t arg1, support_t* arg2)
{
    pcb_PTR newproc = allocPcb(); 

    initializePcbt(newproc);
    insertProcQ(&(readyQueue), newproc);
    insertChild(currentProcess, newproc);

    processCount += 1;

    newproc->p_semAdd = NULL;
    newproc->p_time = 0; 
    //newproc->p_s = arg1; //this causes a memcpy error
    if(arg2 != NULL)
        newproc->p_supportStruct = arg2;
    else    
        newproc->p_supportStruct = NULL;
}


void SYS2()
{
    /* Assuming the current process is the one that executes this functions */
    outChild(currentProcess);
    TerminateSingleProcess(currentProcess);
    KillRec(currentProcess->p_child);
}


HIDDEN void TerminateSingleProcess(pcb_t* to_terminate) // static because it can only be called in this file scope.
{
    // Se il valore del semaforo e' negativo, allora il numero dei processi bloccati corrisponde in quel semaforo corrisponde
    // al valore assoluto del valore del semaforo stesso. Questo valore, se un processo bloccato viene terminato, va aggiustato.
    processCount = processCount - 1;
    freePcb(to_terminate);

    for (int i = 0; i < SEMAPHORE_QTY; i++)
    {
        if (to_terminate->p_semAdd == device_semaphores[i]) return; // Se e' un device semaphore, ho gia' fatto le mie operazioni
    }

    if (removeBlocked(to_terminate->p_semAdd) != NULL) // controllare se e' un device semaphore... (Come si fa?) 
    {
            *(to_terminate->p_semAdd) = *(to_terminate->p_semAdd) + 1;
            softBlockCount = softBlockCount - 1;
    }
}

/** Kills a process recursively
 * 
 * @param: the child of the root process we want to kill.
 */
HIDDEN void KillRec(pcb_PTR root_child)
{
    if (root_child == NULL) return;

    KillRec(root_child->p_next_sib);
    KillRec(root_child->p_child); // We repeat this recursevely for each child of the process
    TerminateSingleProcess(root_child);
}

void SYS3(int** semAddr) 
{
    if (*semAddr > 0) *semAddr = *semAddr - 1;
    else 
    {
        insertBlocked(semAddr, currentProcess);
        softBlockCount = softBlockCount + 1;     
    }
}

void SYS4(int** semAddr) 
{
    *semAddr = *semAddr + 1;
    removeBlocked(semAddr);
}

// DOBBIAMO CONTROLLARE IL BIOSDATAPAGE, li' ci sono i valori

void SYS5(state_t * exceptionState) // sbagliata 
{

    //retrieve a1 & a2 from BIOS DATA PAGE

    // 

    //In caso di problemi testare a1 e a2 come unsigned int e passare il puntatore alla funzione
    int * a1 = currentProcess->p_s.reg_a1;
    int * a2 = currentProcess->p_s.reg_a2;
    int index = (*a1-3)*8 + *a2;
    // devAddrBase = 0x1000.0054 + ((IntlineNo - 3) * 0x80) + (DevNo * 0x10) //Location of status word
    // devStatusWord = devAddrBase + 
    // mask with 0x000F
    
    insertBlocked(&device_semaphores[index],currentProcess);
    while(currentProcess->p_s.reg_a3);  //da verificare
    exceptionState->reg_v0 = currentProcess->p_s.status;    //this is wrong
}

void SYS6()
{
    currentProcess->p_s.reg_v0 = currentProcess->p_time;
}

void SYS7()
{
    SYS3(&device_semaphores[48]);
}

void SYS8(state_t* exceptionState) //Indecisione su quale registro salvare il dato, se nel current process oppure nell'exception state.
{
    exceptionState->reg_v0 = currentProcess->p_supportStruct;
}

