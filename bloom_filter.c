#include "def.h"
#include "bloom_filter.h"
#include <stdio.h>
#include <string.h>

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

void set_bit(unsigned int index, unsigned char * bitArray){
	unsigned int whichInt, whichBit;
	whichInt = index >> 3;
	whichBit = index - (whichInt << 3);
	bitArray[whichInt] |= (1UL << whichBit);
}

unsigned int check_bit(unsigned int index, unsigned char * bitArray){
	unsigned int whichInt, whichBit, testValue;
	whichInt = index >> 3;
	whichBit = index - (whichInt << 3);
	testValue = bitArray[whichInt];
	testValue |= (1UL << whichBit);
	return (testValue ^ bitArray[whichInt]) == 0;
}

unsigned long knuth_hash(unsigned long long int number){
	return number * 2654435761 % MAX_PRIME_VALUE;
}

void put_bloom_filter(unsigned char * bitArray, unsigned long long int key){
	set_bit(knuth_hash(key), bitArray);
	unsigned char key_c_str[50];
	sprintf((char*)key_c_str, "%llu", key);
	set_bit(sdbm_hash(key_c_str), bitArray);
	set_bit(lose_lose_hash(key_c_str), bitArray);
}

short get_bloom_filter(unsigned char * bitArray, unsigned long long int key){
	unsigned char key_c_str[50];
	sprintf((char *)key_c_str, "%llu", key);
	return (check_bit(knuth_hash(key), bitArray) && check_bit(sdbm_hash(key_c_str), bitArray) && check_bit(lose_lose_hash(key_c_str), bitArray));
}


