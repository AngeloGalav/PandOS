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

void printList(pcb_t* lhead, int limit)
{
    printf("\n");
    pcb_t* hd = lhead;

    int i=0;
    do
    {
        printf("val: %d | memadrr: %d\n", hd->val, hd);
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




