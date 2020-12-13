GCC := gcc --std=c99 -g

all: bloom_filter.o main.o btree.o data.o metadata.o
	$(GCC) -o main main.o bloom_filter.o btree.o data.o metadata.o

bloom_filter.o : bloom_filter.c bloom_filter.h
	$(GCC) -c bloom_filter.c

btree.o: btree.c btree.h
	$(GCC) -c btree.c

data.o: data.c data.h
	$(GCC) -c data.c

database.o: database.c database.h
	$(GCC) -c database.c

metadata.o: metadata.c metadata.h
	$(GCC) -c metadata.c

main.o: main.c def.h
	$(GCC) -c main.c

verify: verify.cpp
	g++ --std=c++11 -o verify verify.cpp

clear:
	rm *.o
	rm db.info
	rm -r storage
	mkdir -p storage
