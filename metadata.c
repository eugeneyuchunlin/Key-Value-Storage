#include "metadata.h"
#include "btree.h"
#include "data.h"
#include "bloom_filter.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

MetaData * createMetaData(unsigned int number, B_tree * tree){
	MetaData * metadata = (MetaData *)malloc(sizeof(MetaData));
	metadata->inCache = 0;
	metadata->number = number;
	metadata->dataSize = tree->size;
	metadata->lower = metadata->upper = 0;
	metadata->bitArray = tree->bitArray;
	metadata->data = NULL;
	metadata->bitArraySize = tree->bitArraySize;	
	char file_name[100];
	sprintf(file_name, "./storage/%u.data", number);
	FILE * file = fopen(file_name, "w");
	Output(tree, file);
	fclose(file);
	sprintf(file_name, "./storage/%u.meta", number);		
	file = fopen(file_name, "w");
	fprintf(file,"%u\n%llu\n%llu\n%u\n%u\n", metadata->number, metadata->lower, metadata->upper, metadata->bitArraySize, metadata->dataSize);
	for(unsigned int i = 0; i < metadata->bitArraySize; ++i){
		fprintf(file, "%u ", metadata->bitArray[i]);
	}	
	fclose(file);

	Clear_tree(tree);
	return metadata;
}

MetaData * loadMetaData(char * filename){
	FILE * file = fopen(filename, "r");
	assert(file != NULL);
	MetaData * metadata = (MetaData *)malloc(sizeof(MetaData));
	fscanf(file, "%u", &metadata->number);
	fscanf(file, "%llu",&metadata->lower);
	fscanf(file, "%llu", &metadata->upper);
	fscanf(file, "%u", &metadata->bitArraySize);
	fscanf(file, "%u", &metadata->dataSize);
	metadata->bitArray = (unsigned int *)malloc(sizeof(unsigned int)*metadata->bitArraySize);
	unsigned int index = 0;	
	while(fscanf(file,"%u", &(metadata->bitArray[index])) != EOF) ++index;
	fclose(file);
	metadata->data = NULL;
	metadata->inCache = 0;
	return metadata;	
}


MetaDataSys * createMetaDataSys(){
	MetaDataSys * sys = (MetaDataSys*)malloc(sizeof(MetaDataSys));
	
	// check if db.info exist
	FILE * file;
	file = fopen("./db.info", "r");
	char meta_file_name[100];

	if(file == NULL){
		// a new db.info
		sys->metadatas = (MetaData **)malloc(sizeof(MetaData*) * 100);
		sys->capacity = 100;
		sys->size = 0;
		return sys;
	}else{
		// load db.info
		fscanf(file, "%u %u %u", &(sys->size),&(sys->capacity), &(sys->sortedIndex));
		sys->metadatas = (MetaData **)malloc(sizeof(MetaData *) * sys->capacity);
		fclose(file);

		// load each metadata;
		for(unsigned int i = 0; i < sys->size; ++i){
			sprintf(meta_file_name, "./storage/%u.meta", i);
			sys->metadatas[i] = loadMetaData(meta_file_name);
		}	
		
	}	

	return sys;	
}

void outputMetaDataSys(MetaDataSys * sys){
	FILE * file = fopen("./db.info", "w");
	fprintf(file, "%u %u %u\n", sys->size, sys->capacity, sys->sortedIndex);
	fclose(file);
}

short outputTree(MetaDataSys * sys, B_tree * tree){
	MetaData * metadata = createMetaData(tree->number, tree);
	if(sys->size + 1 > sys->capacity){
		sys->metadatas = (MetaData **)realloc(sys->metadatas, sizeof(MetaData*) *  (sys->capacity + 100));
		sys->capacity += 100;
		assert(sys->metadatas != NULL);
	}
	sys->metadatas[sys->size] = metadata;
	++sys->size;
	return 1;
}


MetaData * loadTable(MetaData * metadata){
	metadata->data = (Data **)malloc(sizeof(Data*) * metadata->dataSize);
	metadata->accessTime = 0;
	char file_name[100];
	Data * temp;
	sprintf(file_name, "./storage/%u.data", metadata->number);
	FILE * file = fopen(file_name, "r");	
	unsigned long long int key;
	char * value = (char *)malloc(sizeof(char)*129);
	unsigned int i = 0;
	while(fscanf(file, "%llu %s", &key, value) != EOF){
		temp =	createData(key, (unsigned char *)value);
		metadata->data[i] = temp;
		++i;
		value = (char *)malloc(sizeof(char)*129);
	}
	return metadata;
}

TableCache * createTableCache(unsigned int CACHE_CAPACITY){
	TableCache * cache = (TableCache*)malloc(sizeof(TableCache));
	cache->capacity = CACHE_CAPACITY;
	cache->size = 0;
	cache->rows = (MetaData**)malloc(sizeof(MetaData*) * CACHE_CAPACITY);
	return cache;
}

void dropOutOfCache(MetaData * metadata){
	if(metadata && metadata->data){
		for(unsigned int i = 0; i < metadata->dataSize; ++i){
			free(metadata->data[i]->value);
			free(metadata->data[i]);
		}
	}
	metadata->inCache = 0;
	metadata->accessTime = 0;
}

int metaDataComp(const void *metadata1,const void *metadata2){
	return ((MetaData *)metadata1)->number > ((MetaData*)metadata2)->number;
}

void addToCache(TableCache * cache, MetaData * metadata){
	short minIndex = 0;
	unsigned char min = 255;
	metadata->inCache = 1;
	metadata->accessTime |= (1UL << 8);
	
	if(cache->size + 1 < cache->capacity){
		cache->rows[cache->size] = metadata;
		++cache->size;
	}else{
		for(unsigned int i = 0; i < cache->size; ++i){
			if(cache->rows[i]->accessTime < min){
				minIndex = i;
			}
		}
		dropOutOfCache(cache->rows[minIndex]);
		// metadata = loadTable(metadata);
		cache->rows[minIndex] = metadata;
		
	}
	qsort(cache->rows, cache->size, sizeof(MetaData *), metaDataComp);
}

Data * search(MetaData * metadata, Data * data){
	// should perform binary search
	for(unsigned int i = 0; i < metadata->dataSize; ++i){
		if(metadata->data[i]->key ==  data->key){
			data->value = metadata->data[i]->value;
		}
	}
	return data;
}

Data * searchData(TableCache * cache, MetaDataSys * sys, Data *data){
	// search Cache First!
	MetaData * founded = NULL;
	for(unsigned int i = 0; i < cache->size; ++i){
		// if(get_bloom_filter(cache->rows[i]->bitArray, data->key)){
			search(cache->rows[i], data);
			if(data->value){
				founded = cache->rows[i];
				break;
			}
	// 	}
	}	
	
	// update access time
	if(founded == NULL){
		for(unsigned int i = 0; i < cache->size; ++i){
			cache->rows[i]->accessTime = cache->rows[i]->accessTime >> 1;
			// cache->rows[i]->accessTime |= (1UL<<8);
		}	
	}else{
		for(unsigned int i = 0; i < cache->size; ++i){
			if(founded != cache->rows[i]){
				cache->rows[i]->accessTime = cache->rows[i]->accessTime >> 1;	
			}
		}
		return data; // search in the cache
	}	
	
	

	// search sys
	for(int i = sys->size - 1; i >= 0; --i){
		
		if(sys->metadatas[i]->inCache == 0 /*&& get_bloom_filter(sys->metadatas[i]->bitArray, data->key)*/){
			sys->metadatas[i] = loadTable(sys->metadatas[i]);
			search(sys->metadatas[i], data);
			if(data->value){
				// add to cache
				addToCache(cache, sys->metadatas[i]);
				return data;
			}else{
				dropOutOfCache(sys->metadatas[i]);
			}
		}
	}

	return data;
}

