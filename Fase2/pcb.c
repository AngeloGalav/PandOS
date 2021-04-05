#include "../include/pcb.h"

// Puntatore alla lista monodirezionale dei pcb_t liberi e disponibili, quindi non utilizzati.
HIDDEN pcb_t* pcbFree_h;

// Array di pcb_t di lunghezza MAXPROC = 20. Contiene tutti i processi concorrenti.
HIDDEN pcb_t pcbFree_table[MAXPROC];

/** Initialization of the pcbFree_h list. Every element of the 
 *  pcbFree_table is added to it.
*/
void initPcbs()
{
    pcbFree_h = &pcbFree_table[0];
    pcb_t* hd = pcbFree_h;

    int i = 1;

    while (i < MAXPROC) // The list will contain a total of MAXPROC elements.
    {
        hd->p_next = &pcbFree_table[i];
        hd = hd->p_next;
        i = i + 1;
    }

    hd->p_next = NULL;
}

/** Initializes the pcb fields to null */
void initializePcb(pcb_t* node)
{
    if (node != NULL)
    {
        node->p_next = NULL;
        node->p_prev = NULL;
        

        node->p_child = NULL;
        node->p_next_sib = NULL;
        node->p_prnt = NULL;
        node->p_prev_sib = NULL;
    }
}

/**
*   Inserts the PCB which is pointed by p in the pcbFree_h 
*   list (the list of free pcbs).
*/
void freePcb(pcb_t* p)
{
    if (p != NULL)
    {
        p->p_prev = NULL;       // We perform an head insert, so that p is the first element of pcbFree
        p->p_next = pcbFree_h;
        pcbFree_h = p;
    }
}

/**
*   Returns NULL if the pcbFree_h is empty, otherwisr it 
*   removes an element from the pcbFree, initializes all the 
*   fields to NULL and returns the removed element.
*/
pcb_t *allocPcb()
{
    if (pcbFree_h == NULL)
    {
        return NULL;
    }
    else
    {
        pcb_t* temp = pcbFree_h;
        pcbFree_h = pcbFree_h->p_next;  // Takes a pcb from the pcbFree list

        initializePcb(temp);           // temp is initiliazed, and then returned

        return temp;
    }
}


/**
*   Creates a list of PCB, initializing it as an empty
*   list (i.e. returns NULL).
*/
pcb_t* mkEmptyProcQ()
{
    return NULL; 
}


/** 
*   Returns TRUE if the list which is pointed by tp is empty,
*   otherwise it returns FALSE.
*/
int emptyProcQ(pcb_t *tp)
{
    if (tp == NULL) return TRUE;
    else return FALSE;
}

/** 
*   Inserts the pcb which is pointed by p in the process queue tp.
*   The double pointer on tp lets us insert p as the last element of
*   the queue.
*/
void insertProcQ(pcb_t** tp, pcb_t* p)
{
    if ((*tp) != NULL && p != NULL)
    {
        pcb_t* head = (*tp)->p_next;
        p->p_prev = (*tp);
        p->p_next = head; // Inserting p at the tail of the queue...

        head->p_prev = p;

        (*tp)->p_next = p;

        (*tp) = p;      // ... and updating the sentinel afterwards.
    }
    else if ((*tp) == NULL && p != NULL)
    {
        (*tp) = p;
        (*tp)->p_next = (*tp);
        (*tp)->p_prev = (*tp);
    }
}


/** 
*   Returns the element at the start (or the end, depending on how you see it) of the
*   process queue tp (so the oldest element), without removing it.
*   Returns NULL if tp does not contain any elements.
*/
pcb_t *headProcQ(pcb_t *tp)
{
    if (tp == NULL) return NULL;

    return tp->p_next;        // Return the element located right after the tail (that is, the head)
}

/** Removes the oldest element in tp and returns its pointer.
*   Returns NULL if the queue is empty.
*/
pcb_t* removeProcQ(pcb_t **tp)
{
    if (*tp == NULL) 
        return NULL;
    else if (*tp == (*tp)->p_next) // In this situation, there's a single element in the queue.
    {
        pcb_t* head = *tp;

        *tp = NULL;     // Removes the only element of the queue, and so it becomes empty.
        return head;
    }
    else   // Branch for more elements in the queue.
    {
        pcb_t* head = (*tp)->p_next;    // Removes the heading element.
        (*tp)->p_next = head->p_next;

        pcb_t* tmp = head->p_next;
        tmp->p_prev = (*tp);

        return head;
    }
}

/**
*   Removes the PCB that is pointed by p from the process queue pointed by tp.
*   If p can't be found, NULL is returned (p could be located at any position of the queue this time).
*/
pcb_t* outProcQ(pcb_t **tp, pcb_t *p)
{
    if (tp == NULL || (*tp) == NULL || p == NULL) return NULL;
    else
    {
        if ((*tp) != p) // General scenario in which p isn't the first element
        {
            pcb_t* tmp = (*tp)->p_next; // tmp points the head of the queue

            while (tmp != (*tp))
            {
                if (tmp == p)
                {
                    tmp->p_prev->p_next = tmp->p_next;
                    tmp->p_next->p_prev = tmp->p_prev;  // Removes the element
                    return tmp;
                }
                tmp = tmp->p_next;
            }
            return NULL;
        }else if ((*tp) == (*tp)->p_next && (*tp) == p) // In this case, p is the only element of the queue
        {
            pcb_t* tmp = (*tp);
            *tp = NULL;

            return tmp;
        }else                 //In this case, the sentinel of the queue points to p and p isn't the only element.
        {
            pcb_t* tmp = (*tp);

            (*tp) = (*tp)->p_prev;

            (*tp)->p_next = tmp->p_next; // we link the new tail to the head of the queue
            tmp->p_next->p_prev = (*tp); // we link the head to the new tail

            return tmp;
        }
    }

    return NULL;
}


/**
*   Returns TRUE if the PCB which is pointed by p hasn't 
*   got any children. Otherwise, the function returns FALSE.
*/
int emptyChild(pcb_t *p)
{
    if ((p != NULL) && (p->p_child == NULL)) return TRUE;
    else return FALSE;
}

/**
*   Inserts p in the children list of prnt.
*/
void insertChild(pcb_t *prnt, pcb_t *p)
{
    if (prnt != NULL && p != NULL)
    {
        pcb_t* tmp = prnt->p_child;     // Adds the element at the head of the children list.
        prnt->p_child = p;
        p->p_next_sib = tmp;

        p->p_prnt = prnt;
                                                 // If prnt does not have any children, sets tmp->prev_sib to p, so
        if (tmp != NULL) tmp->p_prev_sib = p;    // we can handle the bidirectionality.
    }
}

/**
*   Removes the first child of the PCB which is pointed by p.
*   If p hasn't got any children, it returns NULL.
*/
pcb_t* removeChild(pcb_t *p)
{
    if (p != NULL && p->p_child != NULL) // In this case, p has a child
    {
        pcb_t* tmp = p->p_child;         // tmp points to the first child of the list

        if (tmp->p_next_sib != NULL)     // If tmp has a rightmost sibling, 
        {                                // then p has more than 1 child.
            p->p_child = tmp->p_next_sib;
            tmp->p_next_sib->p_prev_sib = NULL;
        } else
        {
            p->p_child = NULL;  // otherwise, set p->child to NULL
        }

        return tmp;
    }

    return NULL;
}

/**
*   Removes the PCB pointed by p from the children list of the parent. If the PCB hasn't 
*   got a parent, NULL is returned, otherwise the function returns the 
*   removed element (that is, p). Unlike removeChild, this method removes a descriptor
*   that could be located in any part of the list.
*/
pcb_t *outChild(pcb_t* p)
{
    if (p == NULL) return NULL;
    if (p->p_prnt == NULL) return NULL;

    if (p->p_prev_sib == NULL) return removeChild(p->p_prnt); // If p has no leftmost siblings, it's the first child
                                                              // and we can remove it with the previous method
    {
        pcb_t* tmp = p->p_prev_sib;
        tmp->p_next_sib = p->p_next_sib;

        if (p->p_next_sib != NULL)  // if there's a rightmost sibling, then we link it to the previous sibling
            p->p_next_sib->p_prev_sib = tmp;

        return p;
    }
}
