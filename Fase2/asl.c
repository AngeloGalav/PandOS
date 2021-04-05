#include "../include/asl.h"

// Inactive semaphore array
HIDDEN semd_t semd_table[MAXPROC + 2];

// Pointer to the list of inactive semaphore, which are not currently in use
HIDDEN semd_t* semdFree_h;

// Active semaphore list pointer
HIDDEN semd_t* semd_h;


/** Inserts the pcb "p" in the queue of the semd associated with the semAdd key. If the corresponding
 *  semaphore decriptor is not present in the ASL,  a new SEMD gets allocated
 *  from the list of free SEMDs (semdFree).
 *  It returns TRUE if a new SEMD can't be allocated from the free SEMD list.
 *  In any other case, it returns FALSE returned.
 */
int insertBlocked(int *semAdd, pcb_t *p)
{
    semd_t* cursor = semd_h;    // we store a reference to the first dummy node

    while (cursor->s_semAdd != (int*)MAXINT)
    {
        if (cursor->s_semAdd == semAdd)
        {
            insertProcQ(&(cursor->s_procQ), p);
            p->p_semAdd = semAdd;
            return FALSE;
        }
        // inserting the new semd in non-ascending order.
        else if (cursor->s_next->s_semAdd > semAdd || cursor->s_next->s_semAdd == (int*)MAXINT)
        {
            if (semdFree_h == NULL)
                return TRUE;
            else
            {
                semd_t* toAdd = semdFree_h; // We take a descriptor from the free descriptors' list (semdFree)
                semdFree_h = semdFree_h->s_next;
                toAdd->s_next = cursor->s_next;
                cursor->s_next = toAdd;
                toAdd->s_procQ = mkEmptyProcQ();
                insertProcQ(&(toAdd->s_procQ), p);
                
                // We update the address of blocked the process
                toAdd->s_semAdd = semAdd;
                
                p->p_semAdd = semAdd;
                return FALSE;
            }
        }
        cursor = cursor->s_next;
    }
    return FALSE;
}


/** Returns the first PCB from the queue (s_procq) of the SEMD of semAdd key. If this SEMD doesn't exist in the ASL, 
 *  the function returns NULL. Otherwise, it returns the element that was removed. 
 *  If the semaphore's blocked process queue is emptied, the corresponding descriptor is 
 *  removed and placed from the ASL to the free descriptor queue (semdFree_h).
 *
*/
pcb_t* removeBlocked(int *semAdd)
{
    semd_t* cursor = semd_h->s_next;
    semd_t* cursor_previous = semd_h;

    while (cursor->s_semAdd != (int*)MAXINT)
    {
        if (cursor->s_semAdd == semAdd)
        {
            pcb_t* toReturn = removeProcQ(&(cursor->s_procQ));

            if (emptyProcQ(cursor->s_procQ))    // in this case, the semaphore's queue is empty, 
            {                                   // so we remove it from the ASL
                cursor_previous->s_next = cursor->s_next;  

                semd_t* toAdd = cursor;     // we add the freed descriptor to semdFree

                toAdd->s_next = semdFree_h;  // head insertion in semdFree_h
                semdFree_h = toAdd;
            }
            
            if (toReturn != NULL) toReturn->p_semAdd = NULL;
            return toReturn;
        }
        cursor_previous = cursor;   // updating the cursors
        cursor = cursor->s_next;
    }
    return NULL;
}



/** Removes the PCB pointed by p in the semaphore in which it (p) is blocked.
 *  If the PCB does not appear in the queue, it returns NULL. Otherwise, the 
 *  function returns p. 
*/
pcb_t* outBlocked(pcb_t *p)
{
    semd_t* cursor = semd_h->s_next;
    semd_t* cursor_previous = semd_h;

    if (p == NULL || p->p_semAdd == NULL) return NULL;

    while (cursor->s_semAdd != (int*)MAXINT)
    {
        // We look for the corrisponding semaAdd
        if(cursor->s_semAdd == p->p_semAdd)
        {
            pcb_t* toReturn = outProcQ(&(cursor->s_procQ), p);

            if (emptyProcQ(cursor->s_procQ))   // in this case, the semaphore's queue is empty, 
            {                                  // so we remove it from the ASL
                cursor_previous->s_next = cursor->s_next;  

                semd_t* toAdd = cursor;     // we add the freed descriptor to semdFree

                toAdd->s_next = semdFree_h;  // head insertion in semdFree_h
                semdFree_h = toAdd;
            }

            if (toReturn != NULL) toReturn->p_semAdd = NULL;
            return toReturn;
        }
        cursor_previous = cursor;
        cursor = cursor->s_next;
    }
    return NULL;
}




/** Returns (without removing it) the pointer to the PCB that is found at the head of the process
 *  queue of the SEMD with semAdd key associated to it. Returns NULL if the SEMD does not appear in 
 *  the ASL or if its process queue is empty.
*/
pcb_t* headBlocked(int *semAdd)
{
    semd_t* hd = semd_h;

    while (hd->s_semAdd != (int*)MAXINT)
    {
        // We look for the corresponding semAdd
        if(hd->s_semAdd == semAdd)
        {
            // if its procq is empty, return NULL
            if (hd->s_procQ == NULL) return NULL;
            // ... otherwise, return the head of the queue without removing it.
            else return (hd->s_procQ)->p_next;

        }
        hd = hd->s_next;
    }
    return NULL;
}


/**
 *  Initializes the semdFree list so that it contains all the elements from
 *  semdTable. This function is called only once during the initialization of 
 *  the data structures. 
 *  Two dummy nodes are used to optimize the control of the list.
 * 
 */
void initASL()
{
    semdFree_h = &semd_table[1];

    semd_t* hd = semdFree_h;

    int i = 2;

    while (i < MAXPROC + 1)
    {
        hd->s_next = &semd_table[i];
        hd = hd->s_next;
        i = i + 1;
    }

    hd->s_next = NULL;

    // Dummy node at the start of the list
    semd_h = &semd_table[0];
    semd_h->s_semAdd = (int*)0x00000000;
    semd_h->s_procQ = NULL;

     // Dummy node at the end of the list
    semd_h->s_next = &semd_table[MAXPROC +1];
    semd_h->s_next->s_semAdd = (int*)MAXINT;
    semd_h->s_next->s_procQ = NULL;

    semd_h->s_next->s_next = NULL;
}
