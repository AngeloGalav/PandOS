## Progetto PandOS del corso di Sistemi Operativi 2020/2021

## Autori:

- Adriano Pace
- Angelo Galavotti
- Denis Pondini
- Antonin Avdei

## Introduzione

Questa è la Fase 1 del progetto PandOs, un "sistema operativo" creato per essere rappresentabile tramite più layer astratti che implementano le funzioni chiave di un sistema operativo Unix.
Questa fase, chiamata "Queues Manager" è pensata per creare un appoggio a tutti i layer di livello più alto fornendo le strutture dati necessarie alla gestione dei processi.
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

[work in progress]

## Come compilare e avviare

La compilazione avviene attraverso il makefile presente nella directory del progetto. Per avviare il programma è necessario avviare μMPS3 e creare una "nuova macchina virtuale" salvando le configurazioni nella directory del progetto, a quel punto l'emulatore sarà in grado di leggere i file generati dal makefile.

N.B: se si usa una distro di linux basata su arch, bisogna modificare il makefile includendo anche il percorso commentato.

```
$ cd Progetto_Sistemi/
$ cd Fase1/
$ make
```
