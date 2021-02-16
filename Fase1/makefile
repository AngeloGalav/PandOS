HEADER = p2test.c testami.h pandos_types.h pandos_const.h

diocane: p2test.o
	gcc -o diocane p2test.o -w && rm *.o

p2test.o: $(HEADER)
	gcc -c p2test.c -w

clean: 
	rm *.o 
