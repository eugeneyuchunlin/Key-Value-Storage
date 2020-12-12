#include "def.h"
#include "bloom_filter.h"

unsigned long djb2_hash(unsigned char * str){
	unsigned long hash = 5381;
	int c;
	while ((c = *str++)){
		hash = ((hash << 5) + hash) + c;
	}
	return hash % MAX_PRIME_VALUE;
}


unsigned long sdbm_hash(unsigned char *str){
	unsigned long hash = 0;
	int c;
	while((c = *str++)){
		hash =  c + (hash << 6) + (hash << 16) - hash;
	}

	return hash % MAX_PRIME_VALUE;
}

unsigned long lose_lose_hash(unsigned char *str){
	unsigned long hash = 0;
	int c;
	while((c = *str++)){
		hash += c;
	}
	return hash % MAX_PRIME_VALUE;
}

void set_bit(unsigned int index, unsigned int * bitArray){
	unsigned int whichInt, whichBit;
	whichInt = index >> 3;
	whichBit = index - (whichInt << 3);
	bitArray[whichInt] |= (1UL << whichBit);
}

void put_bloom_filter(unsigned int * bitArray, unsigned char * str){
	set_bit(djb2_hash(str), bitArray);
	set_bit(sdbm_hash(str), bitArray);
	set_bit(lose_lose_hash(str), bitArray);
}
