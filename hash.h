#ifndef __HASH_H__
#define __HASH_H__

unsigned long djb2_hash(unsigned char *str);

unsigned long sdbm_hash(unsigned char *str);

unsigned long lose_lose_hash(unsigned char * str);
    

#endif
