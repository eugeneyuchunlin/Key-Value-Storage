GCC := gcc --std=c99 -g
all: hash.o main.o btree.o data.o
	$(GCC) -o main main.o hash.o btree.o data.o

hash.o : hash.c hash.h
	$(GCC) -c hash.c

btree.o: btree.c btree.h
	$(GCC) -c btree.c

data.o: data.c data.h
	$(GCC) -c data.c

main.o: main.c
	$(GCC) -c main.c

verify: verify.cpp
	g++ --std=c++11 -o verify verify.cpp

clear:
	rm *.o
