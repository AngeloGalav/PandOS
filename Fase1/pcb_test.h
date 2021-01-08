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


pcb_t* fillList(pcb_t* lhead, pcb_t Memory[]);
pcb_t* fillTree(pcb_t* root, pcb_t Memory[], int nodenums);
pcb_t* fillTreeRecursive(pcb_t* parent_node, pcb_t Memory[], int nodenums, int i);
void tail_insert(pcb_t* blist, pcb_t* elem);
void head_insert(pcb_t* blist,pcb_t* elem);
void printList(pcb_t* lhead, int limit);
void reversePrintList(pcb_t* lhead, int limit);
void printPcbTree(pcb_t* p);



//funzione ausiliaria per stampare la lista
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

//self- explanatory
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

//funzione ausiliaria per creare una lista test
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

/** Stampa la lista dei figli di un albero.
*   Molto simile alla stampa di una lista di pcb_t.
*
*/

void printSiblingList(pcb_t* lhead)
{
    if (lhead == NULL)
    {
        printf("the node has no children\n");
        return;
    } else

    {
        printf("children list:\n");
        pcb_t* hd = lhead;

        while(hd != NULL){
            printf("val: %d | prev sib: %d | actual: %d | next sib: %d\n", hd->val, hd->p_prev_sib, hd , hd->p_next_sib);
            hd = hd->p_next_sib;
        }

    }
}

/** Stampa l'albero di pcb_t ricorsivamente.
*   (Per Adriano: eccoti un cazzo in culo ricorsivo :P)
*
*/
void printPcbTree(pcb_t* p)
{

    printf("\nval: %d | node address: %d\n", p->val , p);
    printSiblingList(p->p_child);
    pcb_t* sibs = p->p_child;
    while (sibs != NULL)
    {
        printPcbTree(sibs);
        sibs = sibs->p_next_sib;
    }
}

/** Riempie l'albero ricorsivamente.
*   Tuttavia NON FUNZIONA e in più non so quanto sia utile.
*
*/

pcb_t* fillTreeRecursive(pcb_t* parent_node, pcb_t Memory[], int nodenums, int i)
{
    pcb_t* list_pcb = &Memory[i];

    list_pcb->p_prnt = parent_node;
    parent_node->p_child = list_pcb;

    i = i + 1;

    int c = 0;

    do
    {
        if (i > nodenums)
        {
            return NULL;
        } else {
            list_pcb->val = i + 10;
            c = c + 1;
            list_pcb->p_next = fillTreeRecursive(list_pcb->p_next, Memory,nodenums, i + c + 10);
            list_pcb = list_pcb->p_next;
        }
    }
    while (c < 3);

    return parent_node;

}

/** Riempie l'albero partendo dal nodo e poi riempiendolo ricorsivamente.
*   Tuttavia NON FUNZIONA e in più non so quanto sia utile.
*
*/

pcb_t* fillTree(pcb_t* root, pcb_t Memory[], int nodenums)
{
    pcb_t* rt = &Memory[0];
    rt->val = 0;
    int i = 1;

    rt = fillTreeRecursive(rt, Memory, nodenums, i);

    return rt;
}


/** Funzione schifosa e scemissima per creare un albero alla cazzo di cane.
*   Volevo farlo con le altre funzioni ricorsive sopra ma le avevo fatte frettolosamente e
*   senza voglia di scervellarmi. Quindi ecco a voi sto schifo. Almeno funziona.
*/

pcb_t* fillTree_UGLY(pcb_t* root, pcb_t Memory[], int nodenums)
{
    pcb_t* rt = &Memory[0];
    rt->val = 0;

    pcb_t* list_head = &Memory[1];
    pcb_t* lhd = list_head;
    list_head->val = 1;
    list_head->p_child = NULL;

    rt->p_child = list_head;
    list_head->p_prnt = rt;


    list_head->p_next_sib = &Memory[2];
    list_head = list_head->p_next_sib;
    list_head->val = 2;
    list_head->p_prnt = rt;

    list_head->p_next_sib = &Memory[3];
    list_head = list_head->p_next_sib;
    list_head->val = 3;
    list_head->p_prnt = rt;

    list_head = &Memory[4];
    list_head->val = 4;
    lhd->p_child = list_head;
    list_head->p_prnt = lhd;


    list_head->p_next_sib = &Memory[5];
    list_head = list_head->p_next_sib;
    list_head->val = 5;
    list_head->p_prnt = lhd;


    return rt;
}




///LOST FUNCTIONS/LEGACY FUNCTIONS




