#ifndef __HASH_H__
#define __HASH_H__

#include "def.h"

unsigned long djb2_hash(unsigned char *str);

unsigned long sdbm_hash(unsigned char *str);

unsigned long lose_lose_hash(unsigned char * str);

unsigned long knuth_hash(unsigned long long int number);

void set_bit(unsigned int index, unsigned char * bitArray);

void put_bloom_filter(unsigned char * bitArray, unsigned long long int key);

short get_bloom_filter(unsigned char * bitArray, unsigned long long int key);

#endif
