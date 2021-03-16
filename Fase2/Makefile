main.o : main.c
    gcc -c main.c ../Libraries/asl.h ../Libraries/libraries.h ../Libraries/pandos_const.h ../Libraries/pandos_types.h ../Libraries/pcb.h -o main.o -w
main : main.o
    gcc -o main main.o -w