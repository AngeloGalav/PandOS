#include "../Libraries/asl.h"
#include "../Libraries/debugger.h"

// Array di semafori non attivi
HIDDEN semd_t semd_table[MAXPROC + 2];

// Puntatore alla lista di semafori liberi non in uso
HIDDEN semd_t* semdFree_h;

// Puntatore alla lista dei semafori attualmente in uso
HIDDEN semd_t* semd_h;


/**
 *  Viene inserito il PCB puntato da p nella coda dei processi bloccati associata al SEMD
 *  con chiave semAdd. Se il semaforo corrispondente non è presente nella ASL, alloca un
 *  nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce nella ASL,
 *  settando i campi in maniera opportuna (i.e.key e s_procQ). Se non è possibile
 *  allocare un nuovo SEMD perché la lista di quelli liberi è vuota restituisce TRUE.
 *  In tutti gli altri casi, restituisce FALSE.
 *
*/
int insertBlocked(int *semAdd, pcb_t *p)
{
    semd_t* cursor = semd_h;    // prendiamo il primo nodo dummy

    while (cursor->s_semAdd != (int*)MAXINT)
    {
        if (cursor->s_semAdd == semAdd)
        {
            insertProcQ(&(cursor->s_procQ), p);
            p->p_semAdd = semAdd;
            return FALSE;
        }
        // Viene effettuato l'inserimento rispettando un ordine non decrescente
        else if (cursor->s_next->s_semAdd > semAdd || cursor->s_next->s_semAdd == (int*)MAXINT)
        {
            if (semdFree_h == NULL)
                return TRUE;
            else
            {
                semd_t* toAdd = semdFree_h; // prendiamo dalla lista dei descrittori liberi
                semdFree_h = semdFree_h->s_next;
                toAdd->s_next = cursor->s_next;
                cursor->s_next = toAdd;
                toAdd->s_procQ = mkEmptyProcQ();
                insertProcQ(&(toAdd->s_procQ), p);
                
                // Aggiornamento l'indirizzo del descrittore
                toAdd->s_semAdd = semAdd;
                
                p->p_semAdd = semAdd;
                return FALSE;
            }
        }
        cursor = cursor->s_next;
    }
    return FALSE;
}


/** Ritorna il primo PCB dalla coda dei processi bloccati (s_procq) associata al SEMD della
 *  ASL con chiave semAdd. Se tale descrittore non esiste nella ASL, restituisce NULL.
 *  Altrimenti, restituisce l’elemento rimosso. Se la coda dei processi bloccati per il semaforo
 *  diventa vuota, rimuove il descrittore  corrispondente dalla ASL e lo inserisce nella coda
 *  dei descrittori liberi (semdFree_h).
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

            if (emptyProcQ(cursor->s_procQ))    // caso in cui il semaforo è stato svuotato, 
            {                                   // lo togliamo dalla ASL
                cursor_previous->s_next = cursor->s_next;  

                semd_t* toAdd = cursor;     // aggiungiamo il descrittore tolto alla lista dei descrittori liberi

                toAdd->s_next = semdFree_h;  // Inserimento in testa a semdFree_h
                semdFree_h = toAdd;
            }
            
            if (toReturn != NULL) toReturn->p_semAdd = NULL;
            return toReturn;
        }
        cursor_previous = cursor;   // aggiornamento dei cursori
        cursor = cursor->s_next;
    }
    return NULL;
}



/**
 *  Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato
 *  (indicato da p->p_semAdd). Se il PCB non compare in tale coda, allora restituisce NULL
 *  (condizione di errore). Altrimenti, restituisce p.
*/
pcb_t* outBlocked(pcb_t *p)
{
    semd_t* cursor = semd_h->s_next;
    semd_t* cursor_previous = semd_h;

    if (p == NULL || p->p_semAdd == NULL) return NULL;

    while (cursor->s_semAdd != (int*)MAXINT)
    {
        // Se il semAdd è quello cercato ...
        if(cursor->s_semAdd == p->p_semAdd)
        {
            pcb_t* toReturn = outProcQ(&(cursor->s_procQ), p);

            if (emptyProcQ(cursor->s_procQ))    // caso in cui il semaforo è stato svuotato, 
            {                                   // lo togliamo dalla ASL
                cursor_previous->s_next = cursor->s_next;  

                semd_t* toAdd = cursor;     // aggiungiamo il descrittore tolto alla lista dei descrittori liberi

                toAdd->s_next = semdFree_h;  // Inserimento in testa a semdFree_h
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




/**
 *  Restituisce (senza rimuovere) il puntatore al PCB che si trova in testa alla coda dei processi
 *  associata al SEMD con chiave semAdd. Ritorna NULL se il SEMD non compare nella ASL oppure
 *  se compare ma la sua coda dei processi è vuota.
*/
pcb_t* headBlocked(int *semAdd)
{
    semd_t* hd = semd_h;

    while (hd->s_semAdd != (int*)MAXINT)
    {
        // Corrisponde al semAdd cercato ...
        if(hd->s_semAdd == semAdd)
        {
            // ...se la sua s_procQ è vuota ...
            if (hd->s_procQ == NULL) return NULL;
            // ... altrimenti ritorna il pcb in testa senza rimuoverlo
            else return (hd->s_procQ)->p_next;

        }
        hd = hd->s_next;
    }
    return NULL;
}


/**
 *  Inizializza la lista dei semdFree in modo da contenere tutti gli elementi
 *  della semdTable. Questo metodo viene invocato una volta sola durante
 *  l’inizializzazione della struttura dati.
 *  Vengono utilizzati due nodi "dummies" per ottimizzare il controllo della lista.
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

    // Nodo dummy ad inizio lista
    semd_h = &semd_table[0];
    semd_h->s_semAdd = (int*)0x00000000;
    semd_h->s_procQ = NULL;

    // Nodo dummy a fine lista
    semd_h->s_next = &semd_table[MAXPROC +1];
    semd_h->s_next->s_semAdd = (int*)MAXINT;
    semd_h->s_next->s_procQ = NULL;

    semd_h->s_next->s_next = NULL;
}
