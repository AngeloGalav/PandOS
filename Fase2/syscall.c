#include "../Libraries/pcb.h"
#include "../Libraries/asl.h"
#include "../Libraries/libraries.h"
#include "../Libraries/definitions.h"

extern pcb_PTR readyQueue;

extern pcb_PTR currentProcess;

extern unsigned int processCount;
extern unsigned int softBlockedCount;
extern int device_semaphores[SEMAPHORE_QTY];

void SYS1(state_t arg1,support_t* arg2)
{
    pcb_PTR newproc = allocPcb(); 

    initializePcbt(newproc);
    insertProcQ(&(readyQueue), newproc);
    insertChild(currentProcess, newproc);

    processCount += 1;

    newproc->p_semAdd = NULL;
    newproc->p_time = 0; 
    newproc->p_s = arg1;
    if(arg2 != NULL)
        newproc->p_supportStruct = arg2;
    else    
        newproc->p_supportStruct = NULL;
}


void SYS2()
{
    /* Assuming the current process is the one that executes this functions */
    outChild(currentProcess);
    KillRec(currentProcess->p_child);
}


static void TerminateSingleProcess(pcb_t* to_terminate) // static because it can only be called in this file scope.
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
            softBlockedCount = softBlockedCount - 1;
    }
}

/** Kills a process recursively
 * 
 * @param: the child of the root process we want to kill.
 */
void KillRec(pcb_PTR root_child)
{
    if (root_child == NULL) return;

    TerminateSingleProcess(root_child);

    KillRec(root_child->p_next_sib);
    KillRec(root_child->p_child); // We repeat this recursevely for each child of the process
}


void SYS8(state_t* arg1) //Indecisione su quale registro salvare il dato, se nel current process oppure nell'exception state.
{
    arg1->reg_v0 = currentProcess->p_supportStruct;
}

