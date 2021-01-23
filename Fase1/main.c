#include "pcb_test.h"

#define MAXPROC 20
#define HIDDEN static
#define TRUE            1
#define FALSE           0


/*  TODO: Rework delle funzioni per permettere il supporto delle corretto delle queue
*   (devono essere rifatte considerando che una queue è definita da un puntatore alla coda (tail) della queue).

    queste funzioni sono:

    pcb_t* removeProcQ(pcb_t **tp);

    pcb_t* outProcQ(pcb_t **tp, pcb_t *p);
*/


/* TODO: Funzioni sui semafori:

    int insertBlocked(int *semAdd,pcb_t *p);

    pcb_t* removeBlocked(int *semAdd);

    pcb_t* outBlocked(pcb_t *p);

    pcb_t* headBlocked(int *semAdd);
*
*/

/*
    TODO: fare pcbFree monodirezionale.
*/

/*  TODO: Dopo che abbiamo finito i due moduli, sarà necessario metterli in due file separati detti
*   pcb.c e asl.c (con i rispettivi .h) in modo da rendere il progetto ordinato.
*
*/

//(PCB sta per Process Control Block)
//(ASL sta per Active semaphore List)
//(SEMD sta per Semaphore Descriptor)

//sentinella della lista pcbfree, punta a pcbfree_h
typedef struct sentinel{
    pcb_t* head;
} sentinel;


///STRUTTURE DATI///

///->PCB///
sentinel p_sentinel;

//puntatore alla lista dei pcb_t liberi e disponibili, quindi non utilizzati. Il puntatore in sé non è un elemento della lista, bensi punta alla testa.
HIDDEN pcb_t* pcbFree_h;

//array di pcb_t di lunghezza MAXPROC = 20. Contiene tutti i processi concorrenti. (il prof ha chiesto di renderlo statico)
HIDDEN pcb_t pcbFree_table[MAXPROC];

//array usato come memoria dummy per testing
pcb_t pcb_memory[300];

///->SEMD///

/*  Un semaforo è attivo quando ho almeno un pcb mella sua queue (da qui usiamo emptyProcQ).
 *
 *  Quello che dovremo fare è creare una lista (LISTA, NON QUEUE) di semd detta semd_h, dove semd_h punta alla testa della lista.
 *  Questa lista semd_h viene detta ASL, e deve essere ordinata in base al valore di s_semAdd in senso ascendente (dal più piccolo al più grande)
 *
 *  semdFree_h è la lista dei semdFree non utilizzati (initSemds?? lol)
 *  semdFree_h è una lista, NON una queue! (esattamente come pcbFree_h)
 *  semd_table è un array di lunghezza MAXPROC
*/

HIDDEN semd_t semd_table[MAXPROC];
HIDDEN semd_t* semdFree_h;
HIDDEN semd_t* semd_h;

///FUNZIONI////

/*  Questa è la initPcbs che funziona come si deve, ovvere pcbFree punta al primo elemento della lista e basta, senza essere anch'esso il primo
 *  elemento. Dunque, per stampare la lista intera ti basta usare pcbFree_h (e anche per lavorare su pcbFree_h ti basta usare pcbFree_h senza
 *  la &). Questa è la funzione che consiglio di usare.
*/
void initPcbs();

void initList(pcb_t* node);

void freePcb(pcb_t* p);

pcb_t *allocPcb();

///FUNZIONI DI MANIPOLAZIONE DELLE QUEUE///

/*  N.B.: Queste funzioni non sono usate per manipolare un insieme particolare di code o una coda di processi specifica, bensì sono
*   funzioni generiche per la manipolazione di una queue di processi.
*/

pcb_t* mkEmptyProcQ();

int emptyProcQ(pcb_t *tp);

void insertProcQ(pcb_t** tp, pcb_t* p);

pcb_t *headProcQ(pcb_t **tp);

pcb_t* removeProcQ(pcb_t **tp);

pcb_t* outProcQ(pcb_t **tp, pcb_t *p);

///FUNZIONI SUGLI ALBERI///

int emptyChild(pcb_t *p);

void insertChild(pcb_t *prnt, pcb_t *p);

pcb_t* removeChild(pcb_t *p);

pcb_t *outChild(pcb_t* p);

///FUNZIONI DEI SEMAFORI///

int insertBlocked(int *semAdd,pcb_t *p);

pcb_t* removeBlocked(int *semAdd);

pcb_t* outBlocked(pcb_t *p);

pcb_t* headBlocked(int *semAdd);

void initASL(); //(fatta)



int main()
{
    initPcbs();

    pcb_t* pcbQueue = mkEmptyProcQ();

    //pcb_t making
    pcb_t tests;
    pcb_t* test;
    test = &tests;
    test->val = 69;

    pcb_t* allocTest1;
    pcb_t test2;
    allocTest1 = &test2;
    allocTest1->val = 909;

    pcb_t* allocTest2;
    pcb_t test3;
    allocTest2 = &test3;
    allocTest2->val = 902;


    insertProcQ(&pcbQueue, test);
    printList(pcbQueue, 1);

    insertProcQ(&pcbQueue, allocTest1);
    printList(pcbQueue, 2);

    insertProcQ(&pcbQueue, allocTest2);
    printList(pcbQueue, 3);


    /*
    pcb_t* work = NULL;

    work = outProcQ(&pcbFree_h, test);

    printList(pcbFree_h, MAXPROC + 3);
    printf("ELEMENTO ELIMINATO: %d | addr: %d\n", work->val, work);
    */

    /*
    pcb_t* tree_source;
    tree_source = fillTree_UGLY(tree_source, pcb_memory, 5);
    printPcbTree(tree_source);

    //pcbFree_h->val = 69;

    //insertChild((tree_source->p_child)->p_next_sib, pcbFree_h);

    //printPcbTree(tree_source);

    pcbFree_h = removeChild(tree_source->p_child);

    //pcbFree_h = outChild((tree_source->p_child)->p_next_sib);

    if (pcbFree_h == NULL)
    {
        printf("CANNOT DELETE NULL ELEMENT!\n");

    }else{

        printf("DELETED ELEMENT IS %d, value of %d\n" , pcbFree_h, pcbFree_h->val);

        if (pcbFree_h->p_prnt == NULL && pcbFree_h->p_prev_sib == NULL && pcbFree_h->p_next_sib == NULL)
        {
            printf("COMPLETELY DELETED CLEAN\n");
        }
    }

    printPcbTree(tree_source);
    */

    return 0;
}


/** Inizializza la pcbFree in modo da contenere tutti gli elementi della
 *  pcbFree_table.
 *  Questo metodo deve essere chiamato una volta sola in fase
 *  di inizializzazione della struttura dati.
 *
*/
void initPcbs() //questa funzione mette quindi tutte gli elementi di initPcbs nella lista.
{
    pcbFree_h = &pcbFree_table[0];
    pcb_t* hd = pcbFree_h;

    hd->val = 0;    //diamo dei valori a ciascun elemento della lista UNICAMENTE perché riuscire a debuggare.
    hd->p_prev = &pcbFree_table[MAXPROC - 1];
    hd->p_next = &pcbFree_table[1];

    hd = hd->p_next;

    int i = 1;

    while (i < MAXPROC - 1){
        hd->val = i;
        hd->p_prev = &pcbFree_table[i-1];
        hd->p_next = &pcbFree_table[i+1];

        i = i + 1;
        hd = hd->p_next;
    }

    hd->val = i;
    hd->p_prev = &pcbFree_table[i - 1];
    hd->p_next = &pcbFree_table[0];

    p_sentinel.head = pcbFree_h;
}

//funzione ausiliaria per inizializzare i campi a NULL
void initList(pcb_t* node)
{
    if (node != NULL){
        node->p_next = NULL;
        node->p_prev = NULL;
        node->p_child = NULL;
        node->p_next_sib = NULL;
        //node->p_s = 0;
        node->p_prnt = NULL;
        node->p_prev_sib = NULL;
    }
}

/** Inserisce il PCB puntato da p nella listaù
*   dei PCB liberi (pcbFree_h)
*/
void freePcb(pcb_t* p) //questa funzione quindi libera un processo mettendolo nella lista dei processi liberi.
{                      //l'inserimento consiste in un semplice inserimento in coda.
    if (p != NULL && pcbFree_h != NULL)
    {
        tail_insert(pcbFree_h, p);
    }
}

/** Restituisce NULL se la pcbFree_h è vuota.
*   Altrimenti rimuove un elemento dalla
*   pcbFree, inizializza tutti i campi (NULL/0)
*   e restituisce l’elemento rimosso.
*/
pcb_t *allocPcb() //Alloca un pcb della lista pcbFree nell
{                 //TODO: Aggiungere supporto alla sentinella.
    if (pcbFree_h == NULL)
    {
        return NULL;
    }
    else
    {
        pcb_t* temp = pcbFree_h;

        pcb_t* tail = pcbFree_h->p_prev;
        pcb_t* nx = pcbFree_h->p_next;

        //p_sentinel->head = pcbFree_h->p_next;

        nx->p_prev = tail;
        tail->p_next = nx;

        pcbFree_h = nx;

        initList(temp); //funzione che inizializza i campi

        //tail->p_next = p_sentinel->head;
        //p_sentinel->head = NULL;

        return temp;
    }
}

/** Crea una lista di PCB, inizializzandola
*   come lista vuota (i.e. restituisce
*   NULL).
*/
pcb_t* mkEmptyProcQ()
{
    pcb_t* list_tail = NULL;
    return list_tail;
}


/** Restituisce TRUE se la lista puntata da
*   head è vuota, FALSE altrimenti
*/
int emptyProcQ(pcb_t *tp)

{
    if (tp == NULL) return TRUE;
    else return FALSE;
}

/**
 * Inserisce l’elemento puntato da p nella
 * coda dei processi tp. La doppia
 * indirezione su tp serve per poter inserire
 * p come ultimo elemento della coda.
 */

void insertProcQ(pcb_t** tp, pcb_t* p) //MANCA IL CASO tp == NULL !!!
{

    if ((*tp) != NULL && p != NULL)
    {
        p->p_prev = (*tp);
        p->p_next = (*tp)->p_next;

        (*tp)->p_next->p_prev = p;
        (*tp)->p_next = p;

        (*tp) = p;      //la sentinella tp ora punta all'indirizzo contenuto da p
    }
    else if (p != NULL && (*tp) == NULL)
    {
        (*tp) = p;
        (*tp)->p_next = (*tp);
        (*tp)->p_prev = (*tp);
    }
}


/**
 * Restituisce l’elemento in fondo alla coda dei processi tp, SENZA RIMUOVERLO.
 * Ritorna NULL se la coda non ha elementi.
 */
pcb_t *headProcQ(pcb_t **tp)
{
    if (tp == NULL)
    {
        return NULL;
    }

    return (*tp);
}

/**
 * Rimuove l’elemento piu’ vecchio dalla coda
 * tp. Ritorna NULL se la coda è vuota,
 * altrimenti ritorna il puntatore all’elemento
 * rimosso dalla lista.
 */
pcb_t* removeProcQ(pcb_t **tp) //con elemento più vecchio, il prof intende di rimuovere il primo processo (quello in testa)
{
    if(*tp == NULL)
    {
        return NULL;
    }
    else
    {
        pcb_t *tmp = *tp;
        *tp = (*tp)->p_next;
        (*tp)->p_prev = tmp->p_prev;
        tmp->p_prev->p_next = (*tp);
        return tmp;
    }
}

/** Rimuove il PCB puntato da p dalla coda dei
*   processi puntata da tp. Se p non è presente
*   nella coda, restituisce NULL (p può trovarsi
*   in una posizione arbitraria della coda).
*/
pcb_t* outProcQ(pcb_t **tp, pcb_t *p)
{
    if( (tp != NULL) && (*tp != NULL) && (p != NULL)  && ((*tp) != p)) // p non è il primo elemento
    {
        pcb_t* tmp = (*tp)->p_next;
        while(tmp != (*tp) )
        {
            if(tmp == p)
            {
                tmp->p_prev->p_next = tmp->p_next;
                tmp->p_next->p_prev = tmp->p_prev;
                tmp->p_next = NULL;
                tmp->p_prev = NULL;
                return tmp;
            }
            tmp = tmp->p_next;
        }
        return NULL;
    }
    else if ( (*tp) == p &&(tp != NULL) &&(*tp) != NULL) // p è il primo elemento
    {
            pcb_t* tmp = (*tp);
            (*tp) = (*tp)->p_next;
            (*tp)->p_prev = tmp->p_prev;
            tmp->p_prev->p_next = (*tp);
            return tmp;
    }
    else
    {
        return NULL;
    }

}

///FUNZIONI ALBERI///

/** Restituisce TRUE se il PCB puntato da p
*   non ha figli, FALSE altrimenti.
*/
int emptyChild(pcb_t *p)    //restituisce true se è una foglia!
{
    if ( p != NULL && p->p_child == NULL)
    {
        return TRUE;
    } else {
        return FALSE;
    }
}

/** Inserisce il PCB puntato da p come figlio
*   del PCB puntato da prnt.
*/
void insertChild(pcb_t *prnt, pcb_t *p) //inserisce un figlio p a prnt.
{
    if (prnt != NULL && p != NULL)  //se il figlio non esiste, evito il segFault.
    {
        pcb_t* tmp = prnt->p_child;
        prnt->p_child = p;
        p->p_next_sib = tmp;

        p->p_prnt = prnt;

        if (tmp != NULL){
            tmp->p_prev_sib = p;    //se prnt non aveva figli inizialmente, mi basta mettere tmp->prev_sib a p;
        }
    }

}

/** Rimuove il primo figlio del PCB puntato
*   da p. Se p non ha figli, restituisce NULL.
*/
pcb_t* removeChild(pcb_t *p)
{
    if (p != NULL && p->p_child != NULL) //se p non ha un figlio, ritorno NULL
    {
        pcb_t* tmp = p->p_child;    //tmp è il figlio

        if (tmp->p_next_sib != NULL){ //se p non ha fratelli destri, evito il segFault
            p->p_child = tmp->p_next_sib;
            tmp->p_next_sib->p_prev_sib = NULL;
        } else {
            p->p_child = NULL;  //se p aveva un solo figlio, mi basta mettere p->child = null;
        }

        /*  Ho aggiunto queste due righe nel caso il prof volesse che quando potassimo l'albero non ci sia più traccia dell'albero
        *   d'origine. Ma ciò dipende tutto da cosa vuole lui. Nel dubbio, queste due righe si possono sempre togliere.
        */
        tmp->p_prnt = NULL;
        tmp->p_next_sib = NULL;

        return tmp;
    }

    return NULL;
}

/** Rimuove il PCB puntato da p dalla lista
*   dei figli del padre. Se il PCB puntato da
*   p non ha un padre, restituisce NULL,
*   altrimenti restituisce l’elemento
*   rimosso (cioè p). A differenza della
*   removeChild, p può trovarsi in una
*   posizione arbitraria (ossia non è
*   necessariamente il primo figlio del padre).
*/
pcb_t *outChild(pcb_t* p)   //ho interpretato questa funzione nel caso si intendesse che p sia un elemento della lista dei figlio di p->prnt
{

    if (p->p_prnt == NULL)
    {
        return NULL;
    }


    if (p->p_prev_sib == NULL)
    {
        return removeChild(p->p_prnt); //se non ha un fratello sinistro, p è sicuramente il primo figlio. posso quindi usare la funzione vecchia
    } else
    {
        pcb_t* tmp = p->p_prev_sib; //se è un elemento in mezzo o alla fine, mi basta eseguire queste due righe.
        tmp->p_next_sib = p->p_next_sib;

        if (p->p_next_sib != NULL) //se non ha un fratello destro, evito il segFault;
        {
            p->p_next_sib->p_prev_sib = tmp;
        }

        /*  Ho aggiunto queste tre righe nel caso il prof volesse che quando potassimo l'albero non ci sia più traccia dell'albero
        *   d'origine. Ma ciò dipende tutto da cosa vuole lui. Nel dubbio, queste tre righe si possono sempre togliere.
        */
        p->p_prnt = NULL;
        p->p_next_sib = NULL;
        p->p_prev_sib = NULL;

        return p;
    }
}


///FUNZIONI SUI SEMAFORI///

/**
 *  Viene inserito il PCB puntato da p nella coda dei processi bloccati associata al SEMD con chiave
 *  semAdd. Se il semaforo corrispondente non è presente nella ASL, alloca un nuovo SEMD dalla
 *  lista di quelli liberi (semdFree) e lo inserisce nella ASL, settando I campi in maniera opportuna (i.e.
 *  key e s_procQ). Se non è possibile allocare un nuovo SEMD perché la lista di quelli liberi è vuota,
 *  restituisce TRUE. In tutti gli altri casi, restituisce FALSE.
*/
int insertBlocked(int *semAdd,pcb_t *p)
{
    return 0;
}


/** Ritorna il primo PCB dalla coda dei processi
 *  bloccati (s_procq) associata al SEMD della ASL con chiave semAdd. Se tale descrittore
 *  non esiste nella ASL, restituisce NULL. Altrimenti, restituisce l’elemento rimosso. Se
 *  la coda dei processi bloccati per il semaforo diventa vuota, rimuove il descrittore
 *  corrispondente dalla ASL e lo inserisce nella coda dei descrittori liberi (semdFree_h).
 *
*/
pcb_t* removeBlocked(int *semAdd)
{
    return NULL;
}

/**
 *  Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (indicato da p-
 *  >p_semAdd). Se il PCB non compare in tale coda, allora restituisce NULL (condizione di errore).
 *  Altrimenti, restituisce p.
*/
pcb_t* outBlocked(pcb_t *p)
{
    return NULL;
}

/**
 *  Restituisce (senza rimuovere) il puntatore al PCB che si trova in testa alla coda dei processi
 *  associata al SEMD con chiave semAdd. Ritorna NULL se il SEMD non compare nella ASL oppure
 *  se compare ma la sua coda dei processi è vuota.
*/
pcb_t* headBlocked(int *semAdd)
{
    return NULL;
}

/**
 *  Inizializza la lista dei semdFree in modo da contenere tutti gli elementi
 *  della semdTable. Questo metodo viene invocato una volta sola durante
 *  l’inizializzazione della struttura dati.
*/
void initASL()  //da vedere: come mantenere l'ordine. (creare una funzione di checkOrder?)
{
    semdFree_h = &semd_table[0];
    pcb_t* hd = semdFree_h;

    hd->p_prev = &semd_table[MAXPROC - 1];
    hd->p_next = &semd_table[1];

    hd = hd->p_next;

    int i = 1;

    while (i < MAXPROC - 1){
        hd->p_prev = &semd_table[i-1];
        hd->p_next = &semd_table[i+1];

        i = i + 1;
        hd = hd->p_next;
    }

    hd->val = i;
    hd->p_prev = &semd_table[i - 1];
    hd->p_next = &semd_table[0];

}



