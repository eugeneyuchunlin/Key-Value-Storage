#ifndef __HASH_H__
#define __HASH_H__

#include "def.h"

unsigned long djb2_hash(unsigned char *str);

unsigned long sdbm_hash(unsigned char *str);

unsigned long lose_lose_hash(unsigned char * str);

void set_bit(unsigned int index, unsigned int * bitArray);

void put_bloom_filter(unsigned int * bitArray, unsigned char * str);
    

#endif
