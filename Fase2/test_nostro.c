#include "pandos_const.h"
#include "pandos_types.h"
#include "asl.h"
#include "pcb.h"

/**
 * Abbiamo bisogno di variabili globali per:
 * 
 * 1) Conteggio dei processi vivi
 * 2) Conteggio dei processi bloccati
 * 3) Coda dei processi ready [Potremo poi manipolarli con i nostri fidati metodi di pcb.h]
 * 4) Puntatore al processo attualmente attivo (puntatore al pcb?)
 * 5) Un semaforo per ogni dispositivo (?)
 * 6) e poi le strutture dati in fase uno...
 * 
 * 
 * COSE DA FARE :
 * 1) migliorare il codice dei pcb... possiamo gia farlo ora (in particolare c'e' una funzione snellibile e
 *  da applicare i miglioramenti che ci ha consigliato maldini)
 * 
 * (Cos'e' un pass up vector????)
 * 
 * Ci occuperemo della inizializzazione delle varie cose!!!! 
 * 
*/

 

int main ()
{
    initPcbs();
    initASL();

}