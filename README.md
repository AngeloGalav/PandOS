## Progetto PandOS del corso di Sistemi Operativi 2020/2021

## Autori:

- Adriano Pace
- Angelo Galavotti
- Denis Pondini
- Antonin Avdei

## Introduzione

PandOs è un "sistema operativo" creato per essere rappresentabile tramite più layer astratti che implementano le funzioni chiave di un sistema operativo Unix.
L’ambiente di riferimento per l’esecuzione del software e’μMPS3. Si tratta di un emulatore per architettura MIPS con interfaccia grafica integrata per l’esecuzione, l’interazione e il debugging del software.Alcuni celebri esempi di dispositvi basati su MIPS: le consolePlaystation 2, Playstation Portable e Nintendo64; la sonda spazialeNew Horizons; l’autopilota della Tesla Model S.

## Tecnologie usate

* Ambiente di sviluppo Unix
* Linguaggio di programmazione C
* Emulatore per architettura MIPS μMPS3

## Fase 1

Il tema di questa fase è la gestione dei processi e dei semafori. I primi vengono gestiti attraverso i pcb (ovvero i descrittori dei processi) che possono essere organizzate in code o alberi. Inoltre, la lista monodirezionale pcbFree_h contiene i pcb liberi, che potranno essere inseriti nelle code o alberi prima citati e reinserite nella lista una volta che non vengono più utilizzate.

I semafori vengono organizzati tramite una lista di descrittori di semafori (semd) detta ASL, che contiene due nodi "dummy" agli estremi. Ogni descrittore della lista contiene una chiave di riconoscimento (semAdd) e una coda di processi pcb. Ogni semaforo diventa attivo una volta che questo contiene un processo, ad esclusione dei nodi dummy che hanno chiave 0 e MAXINT rispettivamente e che non contengono pcb. La lista in sé è ordinata in senso crescente in base a semAdd. Anche per i semafori abbiamo una lista monodirezionale di descrittori semd liberi, che ha la stessa esatta funzione di pcbFree_h, anche se nel contesto degli semd.

Le strutture dei semafori e dei pcb vengono inizializzate attraverso le funzioni initASL e iniPcbs rispettivamente.

## Fase 2

Il tema di questa fase è la creazione della parte principale del nostro kernel. Vengono implementati il codice dello scheduler, del gestore delle eccezioni, del gestore degli interrupt e delle syscall di cui si può fare uso.

Lo scheduler prende un processo corrente dalla readyQueue (ovvero la coda dei processi ready che possono essere avviati) e ne riprende
(o inizia) l'esecuzione.
Le syscall sono particolari funzioni usabili esclusivamente in kernel mode, che permettono di eseguire determinate funzioni come la terminazione e creazione di processi, esecuzione di comandi P e V sui semafori, ritorno delle informazioni sul tempo usato da un processo etc...
L'interrupt handler gestisce gli interrupt di device timer (come il PLT e l'interval timer) e non-timer (come dispositivi di networking, terminali e stampanti), permettendo di eseguire delle V nei semafori di questi device e liberando i processi bloccati nelle queue ad essi associati.
L'exception handler gestisce le eccezioni (tra cui anche syscall e interrupt) ridirezionando i processi nelle procedure richieste o che possono gestire tale eccezione. In caso di Program Trap o di TLB exception (eccezioni che riguardano l'organizzazione della memoria), si esegue una procedura "Pass Up or Die", che permette di passare la gestione dell'eccezione al layer di supporto in caso esso sia presente, o di terminare il processo che ha causato l'eccezione altrimenti. 

## Come compilare e avviare

La compilazione avviene attraverso il makefile presente nella directory del progetto. Per avviare il programma è necessario avviare μMPS3 e creare una "nuova macchina virtuale" salvando le configurazioni nella directory del progetto, a quel punto l'emulatore sarà in grado di leggere i file generati dal makefile.

N.B: se si usa una distro di linux basata su arch, bisogna modificare il makefile includendo anche il percorso commentato.

```
$ cd Progetto_Sistemi/
$ cd Fase1/
$ make
```
In caso si voglia eliminare i file creati, esistono due semplici comandi:
Per la fase 1 basta usare `$ make clean`, mentre per la fase 2 esiste `$ make clean_all`.
