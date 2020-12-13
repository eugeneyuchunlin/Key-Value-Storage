#ifndef __META_DATA_H__
#define __META_DATA_H__

#include "btree.h"


struct MetaData{
	unsigned int *bitArray; // for bloom filter
	unsigned long long int lower;
	unsigned long long int upper;
	unsigned int number;
	unsigned int dataSize;
	unsigned int bitArraySize;
	unsigned char accessTime;
	Data ** data;
	short inCache;
	// char * file_name;
};
typedef struct MetaData MetaData;

struct MetaDataSys{
	MetaData ** metadatas;
	unsigned int sortedIndex;
	unsigned int size;
	unsigned int capacity;
};
typedef struct MetaDataSys MetaDataSys;

struct TableCache{
	MetaData ** rows;	
	unsigned int size;
	unsigned int capacity;
};
typedef struct TableCache TableCache;


MetaData * loadTable(MetaData * metadata);
TableCache * createTableCache(unsigned int CACHE_CAPACITY);
MetaData * createMetaData(unsigned int number, B_tree * tree);
MetaDataSys * createMetaDataSys();
void addToCache(TableCache * cache, MetaData * metadata);
void outputMetaDataSys(MetaDataSys * sys);
short outputTree(MetaDataSys * sys, B_tree * tree);
void dropOutOfCache(MetaData * metadata);
int metaDataComp(const void *,const void *);
void addToCache(TableCache *, MetaData*);
Data * search(MetaData *, Data *);
Data * searchData(TableCache *, MetaDataSys *, Data *);


#endif
