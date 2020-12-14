#ifndef __META_DATA_H__
#define __META_DATA_H__

#include "btree.h"


struct MetaData{
	unsigned char *bitArray; // for bloom filter
	unsigned long long int lower;
	unsigned long long int upper;
	unsigned int number;
	unsigned int dataSize;
	unsigned int bitArraySize;
	unsigned char accessTime;
	char dataFilename[70];
	Data ** data;
	short inCache;
	short isUsed;
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
	// MetaData ** rows;	
	unsigned int size;
	unsigned int capacity;
};
typedef struct TableCache TableCache;

struct chunkFile{
	FILE * file;
	char filename[40];
	Data **readData;
	unsigned int size;
	short end;
	unsigned int current;
};
typedef struct chunkFile chunkFile;


MetaData * loadTable(MetaData * metadata);
TableCache * createTableCache(unsigned int CACHE_CAPACITY);
MetaData * createMetaData(MetaDataSys * sys, TableCache *, unsigned int number, B_tree * tree);
MetaDataSys * createMetaDataSys();
void addToCache(TableCache * cache,MetaDataSys * sys,  MetaData * metadata);
void outputMetaDataSys(MetaDataSys * sys);
short outputTree(TableCache * cache, MetaDataSys * sys, B_tree * tree);
void dropOutOfCache(MetaData * metadata);
int metaDataComp(const void *,const void *);
void addToCache(TableCache *,MetaDataSys * sys,  MetaData*);
Data * search(MetaData *, Data *);
Data * searchData(TableCache *, MetaDataSys *, Data *);
Data * binarySearch(Data **,int,int, Data *);
void Compaction(MetaDataSys * sys, unsigned int metadataSize, unsigned int chunkSize, unsigned int round);
void freeBlockOfData(chunkFile*cf);
void readBlockOfData(chunkFile*cf, unsigned int chunkSize);

#endif
