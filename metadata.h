#ifndef __META_DATA_H__
#define __META_DATA_H__

#include "btree.h"


struct MetaData{
	int *bitArray; // for bloom filter
	unsigned long long int lower;
	unsigned long long int upper;
	unsigned int number;
	char * file_name;
};
typedef struct MetaData MetaData;

struct MetaDataSys{
	MetaData ** metadatas;
	unsigned int size;
	unsigned int capacity;
};
typedef struct MetaDataSys MetaDataSys;

MetaData * createMetaData(unsigned int number, B_tree * tree);


#endif
