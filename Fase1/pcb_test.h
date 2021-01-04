#include <stdio.h>

typedef struct pcb_t {
	/* process queue fields */
    struct pcb_t   *p_next;							/* ptr to next entry			*/
    struct pcb_t   *p_prev; 						/* ptr to previous entry		*/

	/* process tree fields */
	struct pcb_t	*p_prnt, 						/* ptr to parent				*/
					*p_child,						/* ptr to 1st child				*/
					*p_next_sib,					/* ptr to next sibling 			*/
					*p_prev_sib;					/* ptr to prev. sibling			*/

	/* process status information */
	int     val;              /* processor state        */

	/* add more fields here */

}  pcb_t, *pcb_PTR;

void printList(pcb_t* lhead, int limit)
{
    printf("\n");
    pcb_t* hd = lhead;

    int i=0;
    do
    {

        printf("val: %d | prev: %d | actual: %d | next: %d\n", hd->val, (hd)->p_prev, hd , (hd)->p_next);
        hd = hd->p_next;
        i=i+1;

    }
    while (i < limit);

}

void reversePrintList(pcb_t* lhead, int limit)
{
    printf("\n");
    pcb_t* hd = lhead;

    int i=0;
    do
    {
        printf("val: %d | memadrr: %d\n", hd->val, hd);
        hd = hd->p_prev;
        i=i+1;

    }
    while (i < limit);

}

void tail_insert(pcb_t* blist, pcb_t* elem)
{
    if(blist != NULL)
    {
        if(blist->p_next == NULL)
        {
            blist->p_next == elem;
            blist->p_prev == elem;
            elem->p_next = blist;
            elem->p_prev = blist;
        }
        else
        {
            pcb_t* tail = blist->p_prev;
            tail->p_next = elem;
            elem->p_prev = tail;
            elem->p_next = blist;
            blist->p_prev = elem;
        }
    }

}

void head_insert(pcb_t* blist,pcb_t* elem)
{
    if(blist != NULL)
    {
        if(blist->p_next == NULL)
        {
            blist->p_next == elem;
            blist->p_prev == elem;
            elem->p_next = blist;
            elem->p_prev = blist;
        }
        else
        {
            elem->p_next = blist->p_next;
            elem->p_prev = blist;
            blist->p_next = elem;
            elem->p_next->p_prev = elem;
        }
    }
}

pcb_t* fillList(pcb_t* lhead, pcb_t Memory[])
{
    pcb_t* hd = &lhead;

    int i=0;

    do
    {
        hd->val = i ;
        i = i + 1;
        hd->p_next = &Memory[i];
        hd = hd->p_next;
    }
    while (i < 10);

    return lhead;
}


pcb_t* fillList_2(pcb_t* lhead, pcb_t Memory[])
{
    pcb_t* hd = &lhead;

    int i=0;

    do
    {
        hd->val = i ;
        i = i + 1;
        hd->p_next = &Memory[i];
        hd = hd->p_next;
    }
    while (i < 10);

    return lhead;
}


///LOST FUNCTIONS/LEGACY FUNCTIONS




