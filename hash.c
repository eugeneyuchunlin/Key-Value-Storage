#include "hash.h"

unsigned long djb2_hash(unsigned char * str){
	unsigned long hash = 5381;
	int c;
	while ((c = *str++)){
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}


unsigned long sdbm_hash(unsigned char *str){
	unsigned long hash = 0;
	int c;
	while((c = *str++)){
		hash =  c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

unsigned long lose_lose_hash(unsigned char *str){
	unsigned long hash = 0;
	int c;
	while((c = *str++)){
		hash += c;
	}
	return hash;
}
