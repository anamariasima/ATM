build:
	gcc -c atm.c
	gcc atm.o -o atm
run: 
	./atm 
clean:
	rm atm.o atm
