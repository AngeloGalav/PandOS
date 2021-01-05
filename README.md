# Progetto_Sistemi

Cosa si è fatto fino ad ora 4 gennaio:
create e TESTATE le prime sei funzioni  richieste, fino ad ora abbiamo lavorato solo con  puntatori e liste (specifiche nei commenti).

Per ora la stampa del programma è questa:


val: 1 | prev: 1406239088 | actual: -1330568488 | next: -1330568432
val: 2 | prev: -1330568488 | actual: -1330568432 | next: -1330568376
val: 3 | prev: -1330568432 | actual: -1330568376 | next: -1330568320
val: 4 | prev: -1330568376 | actual: -1330568320 | next: -1330568264
val: 5 | prev: -1330568320 | actual: -1330568264 | next: -1330568208
val: 6 | prev: -1330568264 | actual: -1330568208 | next: -1330568152
val: 7 | prev: -1330568208 | actual: -1330568152 | next: -1330568096
val: 8 | prev: -1330568152 | actual: -1330568096 | next: -1330568040
val: 9 | prev: -1330568096 | actual: -1330568040 | next: -1330567984
val: 10 | prev: -1330568040 | actual: -1330567984 | next: -1330567928
val: 11 | prev: -1330567984 | actual: -1330567928 | next: -1330567872
val: 12 | prev: -1330567928 | actual: -1330567872 | next: -1330567816
val: 13 | prev: -1330567872 | actual: -1330567816 | next: -1330567760
val: 14 | prev: -1330567816 | actual: -1330567760 | next: -1330567704
val: 15 | prev: -1330567760 | actual: -1330567704 | next: -1330567648
val: 16 | prev: -1330567704 | actual: -1330567648 | next: -1330567592
val: 17 | prev: -1330567648 | actual: -1330567592 | next: -1330567536
val: 18 | prev: -1330567592 | actual: -1330567536 | next: -1330567480
val: 19 | prev: -1330567536 | actual: -1330567480 | next: 1406239088
val: 69 | prev: -1330567480 | actual: 1406239088 | next: -1330568488
val: 1 | prev: 1406239088 | actual: -1330568488 | next: -1330568432
val: 2 | prev: -1330568488 | actual: -1330568432 | next: -1330568376
val: 3 | prev: -1330568432 | actual: -1330568376 | next: -1330568320

Sono TUTTI gli elementi della nostra lista, il 69 è l'elemento test, 1 2 3 vengono ristampati per capire se la cosa funzionava.
Ovviamente ogni stampa che fate cambiano gli indirizzi ma è normale, basta che corrispondano.
Abbiamo dei problemi con la funzione mkEmptyProcQ() perchè praticamente non capiamo come creare un puntatore e farlo puntare a qualcosa effettivamente, per poi utilizzarlo.ABbiamo provato a creare dei pcb_t fittizzi a cui assegnarlo ma questo crea il problema che poi ogni volta che si richiama quella funzione il pcbt utilizzato dal compilatore è lo stesso, abbiamo perciò provato a creare un array di pcb_t ed assegnare con un contatore un pcb_t diverso ogni volta che viene chiamata la funzione, questo però ci porta al dubbio che questa cosa sia effettivamente legale perciò abbiamo chiesto al prof un pò di delucidazioni.
Comunque sia tutte le altre funzioni sembrano funzionare senza grandi problemi.
