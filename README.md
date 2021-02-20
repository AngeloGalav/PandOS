##Fase 1 del progetto di Sistemi Operativi 2020/2021

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


## Come compilare e avviare

La compilazione avviene attraverso il makefile presente nella directory del progetto. Per avviare il programma è necessario avviare μMPS3 e creare una "nuova macchina virtuale" salvando le configurazioni nella directory del progetto, a quel punto l'emulatore sarà in grado di leggere i file generati dal makefile.

N.B: se si usa una distro di linux basata su arch, bisogna modificare il makefile includendo anche il percorso commentato.


* cd Progetto_Sistemi
* cd Fase1
* make

