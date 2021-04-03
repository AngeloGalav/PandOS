#include "../Libraries/pcb.h"
#include "../Libraries/debugger.h"

// Puntatore alla lista monodirezionale dei pcb_t liberi e disponibili, quindi non utilizzati.
HIDDEN pcb_t* pcbFree_h;

// Array di pcb_t di lunghezza MAXPROC = 20. Contiene tutti i processi concorrenti.
HIDDEN pcb_t pcbFree_table[MAXPROC];

/** Inizializzazione della lista pcbFree_h.
    Per inizializzarla, aggiunge alla lista ogni elemento degli array dei processi.
*/
void initPcbs()
{
    pcbFree_h = &pcbFree_table[0];
    pcb_t* hd = pcbFree_h;

    int i = 1;

    while (i < MAXPROC) // Nella lista finale avremmo MAXPROC elementi in totale.
    {
        hd->p_next = &pcbFree_table[i];
        hd = hd->p_next;
        i = i + 1;
    }

    hd->p_next = NULL;
}

/** Inizializza i campi di un pcb_t a NULL*/
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

/** Inserisce il PCB puntato da p nella lista
*   dei PCB liberi (pcbFree_h)
*/
void freePcb(pcb_t* p)
{
    if (p != NULL)
    {
        p->p_prev = NULL;       // Faccio un inserimento in testa, mettendolo come primo elemento della lista.
        p->p_next = pcbFree_h;
        pcbFree_h = p;
    }
}

/** Restituisce NULL se la pcbFree_h è vuota.
*   Altrimenti rimuove un elemento dalla pcbFree, inizializza
*   tutti i campi (NULL/0) e restituisce l’elemento rimosso.
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
        pcbFree_h = pcbFree_h->p_next;  // Prende un pcb_t dalla lista dei pcbFree.

        initializePcb(temp);           // Lo inizializza, e poi lo ritorna.

        return temp;
    }
}


/** Crea una lista di PCB, inizializzandola come lista
*   vuota (i.e. restituisce NULL).
*/
pcb_t* mkEmptyProcQ()
{
    return NULL;    // Ritorna NULL, per inizializzare il puntatore alla lista.
}


/** Restituisce TRUE se la lista puntata da tp è
*   vuota, FALSE altrimenti.
*/
int emptyProcQ(pcb_t *tp)
{
    if (tp == NULL) return TRUE;
    else return FALSE;
}

/** Inserisce l’elemento puntato da p nella coda dei processi tp.
*   La doppia indirezione su tp serve per poter inserire p come ultimo
*   elemento della coda.
*/
void insertProcQ(pcb_t** tp, pcb_t* p)
{
    if ((*tp) != NULL && p != NULL)
    {
        pcb_t* head = (*tp)->p_next;
        p->p_prev = (*tp);
        p->p_next = head; // Inserisco l'elemento nelle coda della Queue...

        head->p_prev = p;

        (*tp)->p_next = p;

        (*tp) = p;      // ... e aggiorno la sentinella.
    }
    else if ((*tp) == NULL && p != NULL)
    {
        (*tp) = p;
        (*tp)->p_next = (*tp);
        (*tp)->p_prev = (*tp);
    }
}


/** Restituisce l’elemento in fondo alla coda dei processi tp, SENZA RIMUOVERLO.
*   Ritorna NULL se la coda non ha elementi.
*/
pcb_t *headProcQ(pcb_t *tp)
{
    if (tp == NULL) return NULL;

    return tp->p_next;        // Restituisce l'elemento in testa (ovvero quello precedente alla coda).
}

/** Rimuove l’elemento piu’ vecchio dalla coda tp. Ritorna NULL se
*   la coda è vuota, altrimenti ritorna il puntatore all’elemento
*   rimosso dalla lista.
*/
pcb_t* removeProcQ(pcb_t **tp)
{
    if (*tp == NULL) 
        return NULL;
    else if (*tp == (*tp)->p_next) // Caso in cui ho un singolo elemento nella coda.
    {
        pcb_t* head = *tp;

        *tp = NULL; // Rimuove l'unico elemento della coda, quindi la coda diventa vuota.
        return head;
    }
    else    // Caso in cui ho piu' di un elemento nella coda.
    {
        pcb_t* head = (*tp)->p_next;    // Rimuove l'elemento in testa.
        (*tp)->p_next = head->p_next;
        //head->p_next->p_prev = (*tp);

        pcb_t* tmp = head->p_next;
        tmp->p_prev = (*tp);

        return head;
    }
}

/** Rimuove il PCB puntato da p dalla coda dei processi puntata da tp. Se p non è presente
*   nella coda, restituisce NULL (p può trovarsi in una posizione arbitraria della coda).
*/
pcb_t* outProcQ(pcb_t **tp, pcb_t *p)
{
    if (tp == NULL || (*tp) == NULL || p == NULL) return NULL;
    else
    {
        if ((*tp) != p) // Caso generale (in cui p non e' il primo elemento)
        {
            pcb_t* tmp = (*tp)->p_next; // Iniziamo prima prendendo il puntatore alla

            while (tmp != (*tp))
            {
                if (tmp == p)
                {
                    tmp->p_prev->p_next = tmp->p_next;
                    tmp->p_next->p_prev = tmp->p_prev;  // Rimuovo l'elemento (se lo trovo)
                    initializePcb(tmp);
                    return tmp;
                }
                tmp = tmp->p_next;
            }
            return NULL;
        }else if ((*tp) == (*tp)->p_next && (*tp) == p) // Caso in cui tp ha un solo elemento, ed e' p
        {
            pcb_t* tmp = (*tp);
            *tp = NULL;

            initializePcb(tmp);
            return tmp;
        }else                       // Caso in cui la sentinella punta a p e p non è l'unico elemento
        {
            pcb_t* tmp = (*tp);

            (*tp) = (*tp)->p_prev;

            (*tp)->p_next = tmp->p_next; // puntiamo la nuova coda a head
            tmp->p_next->p_prev = (*tp); // dico a head qual'e' la nuova coda

            initializePcb(tmp);
            return tmp;
        }
    }

    return NULL;
}


/** Restituisce TRUE se il PCB puntato da p
*   non ha figli, FALSE altrimenti.
*/
int emptyChild(pcb_t *p)
{
    if ((p != NULL) && (p->p_child == NULL)) return TRUE;
    else return FALSE;
}

/** Inserisce il PCB puntato da p come figlio
*   del PCB puntato da prnt.
*/
void insertChild(pcb_t *prnt, pcb_t *p)
{
    if (prnt != NULL && p != NULL)
    {
        pcb_t* tmp = prnt->p_child;     // Aggiunge in testa il nuovo elemento alla lista dei figli.
        prnt->p_child = p;
        p->p_next_sib = tmp;

        p->p_prnt = prnt;

        if (tmp != NULL) tmp->p_prev_sib = p;    // Se prnt non aveva figli inizialmente, setta tmp->prev_sib a p,
                                                 // in modo da gestire la bidirezionalita'
    }
}

/** Rimuove il primo figlio del PCB puntato
*   da p. Se p non ha figli, restituisce NULL.
*/
pcb_t* removeChild(pcb_t *p)
{
    if (p != NULL && p->p_child != NULL) // Caso in cui p ha un figlio
    {
        pcb_t* tmp = p->p_child;         // tmp è un puntatore che punta al primo figlio

        if (tmp->p_next_sib != NULL)     // Se p ha fratelli destri, vuol dire che non e' un figlio singolo.
        {
            p->p_child = tmp->p_next_sib;
            tmp->p_next_sib->p_prev_sib = NULL;
        } else
        {
            p->p_child = NULL;  // Se p aveva un solo figlio, setta p->child = null
        }

        // Ri-inizializzazione dei campi, in modo che non ci sia piu' traccia dell'albero di partenza
        //initializePcb(tmp);

        return tmp;
    }

    return NULL;
}

/** Rimuove il PCB puntato da p dalla lista  dei figli del padre. Se il PCB puntato da
*   p non ha un padre, restituisce NULL, altrimenti restituisce l’elemento
*   rimosso (cioè p). A differenza della removeChild, p può trovarsi in una
*   posizione arbitraria (ossia non è necessariamente il primo figlio del padre).
*/
pcb_t *outChild(pcb_t* p)
{
    if (p->p_prnt == NULL) return NULL;

    if (p->p_prev_sib == NULL) return removeChild(p->p_prnt); // Se p non ha un fratello sinistro, e' il primo figlio.
                                                              // Si puo' quindi applicare la funzione precedente.
    {
        pcb_t* tmp = p->p_prev_sib;
        tmp->p_next_sib = p->p_next_sib;

        if (p->p_next_sib != NULL)  // Se ha un fratello destro, collego quest'ultimo con il fratello precedente
            p->p_next_sib->p_prev_sib = tmp;

        //initializePcb(p);

        return p;
    }
}
