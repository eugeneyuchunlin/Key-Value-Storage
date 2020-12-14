#include "metadata.h"
#include "btree.h"
#include "data.h"
#include "def.h"
#include "bloom_filter.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

MetaData * createMetaData(MetaDataSys* sys, TableCache * cache, unsigned int number, B_tree * tree){
	MetaData * metadata = (MetaData *)malloc(sizeof(MetaData));
	metadata->inCache = 0;
	metadata->number = number;
	metadata->dataSize = tree->size;
	metadata->lower = metadata->upper = 0;
	metadata->bitArray = tree->bitArray;
	metadata->data = (Data **)malloc(sizeof(Data*)*tree->size);
	metadata->bitArraySize = tree->bitArraySize;	
	metadata->isUsed = 1;
	char file_name[100];
	sprintf(metadata->dataFilename, "./storage/%u.data", number);
	FILE * file = fopen(metadata->dataFilename, "w");
	

	unsigned int index = 0;
	Output(tree, file, metadata->data, &index);
	addToCache(cache,sys, metadata);	

	fclose(file);
	sprintf(file_name, "./storage/%u.meta", number);		
	file = fopen(file_name, "w");
	fprintf(file,"%u\n%llu\n%llu\n%u\n%u\n%s\n%d\n", metadata->number, metadata->lower, metadata->upper, metadata->bitArraySize, metadata->dataSize, metadata->dataFilename, metadata->isUsed);
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
	fscanf(file, "%s", metadata->dataFilename);
	fscanf(file, "%hd", &metadata->isUsed);
	metadata->bitArray = (unsigned char *)malloc(sizeof(unsigned char)*metadata->bitArraySize);
	unsigned int index = 0;	
	while(fscanf(file,"%hhu", &(metadata->bitArray[index])) != EOF) ++index;
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
		sys->capacity = 3; // default capacity;
		sys->metadatas = (MetaData **)malloc(sizeof(MetaData*) * sys->capacity);
		sys->size = 0;
		return sys;
	}else{
		// load db.info
		fscanf(file, "%u %u %u", &(sys->size),&(sys->capacity), &(sys->sortedIndex));
		sys->metadatas = (MetaData **)malloc(sizeof(MetaData *) * sys->capacity);

		// load each metadata;
		for(unsigned int i = 0; i < sys->size; ++i){
			fscanf(file, "%s", meta_file_name);	
			sys->metadatas[i] = loadMetaData(meta_file_name);
		}	

		fclose(file);
		
	}	

	return sys;	
}

void outputMetaDataSys(MetaDataSys * sys){
	FILE * file = fopen("./db.info", "w");
	fprintf(file, "%u %u %u\n", sys->size, sys->capacity, sys->sortedIndex);
	FILE * file2;	
	char file_name[100];
	MetaData *metadata;
	for(unsigned int i = 0; i < sys->size; ++i){
		metadata = sys->metadatas[i];
		sprintf(file_name, "./storage/%u.meta", metadata->number);		
		file2 = fopen(file_name, "w");
		fprintf(file2,"%u\n%llu\n%llu\n%u\n%u\n%s\n%d\n", metadata->number, metadata->lower, metadata->upper, metadata->bitArraySize, metadata->dataSize, metadata->dataFilename, metadata->isUsed);
		for(unsigned int i = 0; i < metadata->bitArraySize; ++i){
			fprintf(file2, "%u ", metadata->bitArray[i]);
		}	
		fclose(file2);

		if(sys->metadatas[i]->isUsed){
			fprintf(file, "./storage/%u.meta\n", sys->metadatas[i]->number);
		}
	}
	fclose(file);
}

short outputTree(TableCache * cache, MetaDataSys * sys, B_tree * tree){
	MetaData * metadata = createMetaData(sys, cache, tree->number, tree);
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
	Data * temp;
	FILE * file = fopen(metadata->dataFilename, "r");
	assert(file != NULL);
	unsigned long long int key;
	char * value = (char *)malloc(sizeof(char)*129);
	unsigned int i = 0;
	// printf("Metadata NO.%d\n", metadata->number);
	while(fscanf(file, "%llu %s", &key, value) != EOF){
		temp =	createData(key, (unsigned char *)value);
		metadata->data[i] = temp;
		++i;
		value = (char *)malloc(sizeof(char)*129);
	}
	free(value);
	fclose(file);
	return metadata;
}

MetaData * loadTableAndSearch(MetaData * metadata, Data * data){
	metadata->data = (Data **)malloc(sizeof(Data*) * metadata->dataSize);
	metadata->accessTime = 0;
	Data * temp;
	FILE * file = fopen(metadata->dataFilename, "r");
	assert(file != NULL);
	unsigned long long int key;
	char * value = (char *)malloc(sizeof(char)*129);
	unsigned int i = 0;
	// printf("Metadata NO.%d\n", metadata->number);
	while(fscanf(file, "%llu %s", &key, value) != EOF){
		temp =	createData(key, (unsigned char *)value);
		metadata->data[i] = temp;
		++i;
		if(key == data->key)
			data->value = (unsigned char*)value;
		value = (char *)malloc(sizeof(char)*129);
	}
	free(value);
	fclose(file);
	return metadata;	
}

TableCache * createTableCache(unsigned int CACHE_CAPACITY){
	TableCache * cache = (TableCache*)malloc(sizeof(TableCache));
	cache->capacity = CACHE_CAPACITY;
	cache->size = 0;
	// cache->rows = (MetaData**)malloc(sizeof(MetaData*) * CACHE_CAPACITY);
	return cache;
}

void freeLoadedData(MetaData * metadata){
	if(metadata && metadata->data){
		for(unsigned int i = 0; i < metadata->dataSize; ++i){
			// printf("i = %d\n", i);
			// free(metadata->data[i]->value);
			freeData(metadata->data[i]);
		}
		free(metadata->data);
	}
}

void dropOutOfCache(MetaData * metadata){
	// printf("data number = %d\ndata size = %d\n",metadata->number,  metadata->dataSize);
	freeLoadedData(metadata);	
	// metadata->data = NULL;
	metadata->inCache = 0;
	metadata->accessTime = 0;
}



int metaDataComp(const void *metadata1,const void *metadata2){
	return ((MetaData *)metadata1)->number > ((MetaData*)metadata2)->number;
}

void addToCache(TableCache * cache, MetaDataSys *sys, MetaData * metadata){
	short minIndex = 0;
	unsigned char min = 255;
	metadata->inCache = 1;
	metadata->accessTime >>= 1;
	metadata->accessTime |= (1UL << 8);
	
	if(cache->size + 1 < cache->capacity){
		// cache->rows[cache->size] = metadata;
		++cache->size;
	}else{
		for(unsigned int i = 0; i < sys->size; ++i){
			if(sys->metadatas[i]->accessTime < min){
				minIndex = i;
			}
		}
		dropOutOfCache(sys->metadatas[minIndex]); // just free data
		
	}
	// qsort(cache->rows, cache->size, sizeof(MetaData *), metaDataComp);
}

Data * search(MetaData * metadata, Data * data){
	// should perform binary search
	binarySearch(metadata->data, 0, metadata->dataSize - 1, data);
	
	// for(unsigned int i = 0; i < metadata->dataSize; ++i){
	// 	if(metadata->data[i]->key ==  data->key){
	// 		data->value = metadata->data[i]->value;
	// 		return data;
	// 	}
	// }
	return data;
}

Data * binarySearch(Data ** dataArray,int start,int end, Data * target){
	int index = end - start;
	if(index < 0) 
		return target;

	index = start + (index >> 1);
	if(dataArray[index]->key > target->key) // search left
		return binarySearch(dataArray, start, index-1, target);
	else if(dataArray[index]->key < target->key){
		return binarySearch(dataArray, index + 1, end, target);
	}else{
		target->value = dataArray[index]->value;
		return target;
	}
}

Data * searchData(TableCache * cache, MetaDataSys * sys, Data *data){
	// search Cache First!
	//
	// search sys
	int i;
	MetaData * founded = NULL;
	for(i = sys->size - 1; i >= 0; --i){
// atomic start
		if(get_bloom_filter(sys->metadatas[i]->bitArray, data->key)){
			if(sys->metadatas[i]->lower == sys->metadatas[i]->upper){
				if(!sys->metadatas[i]->inCache) // if not in cache, then perform load and search
					sys->metadatas[i] = loadTableAndSearch(sys->metadatas[i], data);
				else // else just perform search
					search(sys->metadatas[i], data);
				
				if(data->value ){ // if get the result
					if(!sys->metadatas[i]->inCache){ // if not in cache ===> add to cache
						addToCache(cache,sys, sys->metadatas[i]);
						founded = sys->metadatas[i];
					}else{
						founded = sys->metadatas[i];
					}
					break;
				}else{ // if result not founded 
					if(!sys->metadatas[i]->inCache){ // if not in cache then just free the data
						freeLoadedData(sys->metadatas[i]);	
					}
				}
			}else if(sys->metadatas[i]->lower <= data->key && data->key <= sys->metadatas[i]->upper){
				if(!sys->metadatas[i]->inCache) // if not in cache, then perform load and search
					sys->metadatas[i] = loadTableAndSearch(sys->metadatas[i], data);
				else // else just perform search
					search(sys->metadatas[i], data);
				
				if(data->value ){ // if get the result
					if(!sys->metadatas[i]->inCache){ // if not in cache ===> add to cache
						addToCache(cache,sys, sys->metadatas[i]);
						founded = sys->metadatas[i];
					}else{
						founded = sys->metadatas[i];
					}
					break;
				}else{ // if result not founded 
					if(!sys->metadatas[i]->inCache){ // if not in cache then just free the data
						freeLoadedData(sys->metadatas[i]);	
					}
				}
			}
		}
// atomic end
	}
	


	//
	// for(unsigned int i = 0; i < cache->size; ++i){
	// 	 if(get_bloom_filter(cache->rows[i]->bitArray, data->key)){
	// 		search(cache->rows[i], data);
	// 		if(data->value){
	// 			founded = cache->rows[i];
	// 			break;
	// 		}
	//  	}
	// }	
	
	// update access time
	if(founded != NULL){ // if founded
		for(unsigned int i = 0; i < sys->size; ++i){
			if(sys->metadatas[i] != founded){
				sys->metadatas[i]->accessTime >>= 1;
			}
		}
	}	
	

	return data;
}

void readBlockOfData(chunkFile * cf, unsigned int chunkSize){
	cf->current = 0;
	unsigned int i = 0;
	unsigned long long int key;
	char * value = (char *)malloc(sizeof(char)*129);
	short hasRead = 0;

	while(EOF != fscanf(cf->file,"%llu %s", &key, value)){
		cf->readData[i] = createData(key, (unsigned char*)value);
		value = (char *)malloc(sizeof(char)*129);
		++i;
		hasRead = 1;
		if(i >= chunkSize){
			cf->size = i;
			return;
		}
	} 
	if(!hasRead){
		cf->size = i;
		cf->end = 1;
		fclose(cf->file);
	}else{
		cf->size = i;
	}
}

void freeBlockOfData(chunkFile * cf){
	for(unsigned int i = 0; i < cf->size; ++i){
		freeData(cf->readData[i]);
	}
}


void Compaction(MetaDataSys *sys, unsigned int metadataSize,  unsigned int chunkSize, unsigned int round){
	short FILE_END[metadataSize];
	char filename[metadataSize][100];
	for(unsigned int i = 0; i < metadataSize; ++i){
		FILE_END[metadataSize] = 0;
	}

	chunkFile * files = (chunkFile*)malloc(sizeof(chunkFile)*metadataSize);
	for(unsigned int i = 0; i < metadataSize; ++i){
		// sprintf(files[i].filename, "storage/%u.data", sys->metadatas[i]->number);
		strcpy(files[i].filename, sys->metadatas[i]->dataFilename);
		files[i].file = fopen(files[i].filename, "r");
		files[i].readData = (Data **)malloc(sizeof(Data*)*chunkSize);
		files[i].end = 0;
		readBlockOfData(&files[i], chunkSize);
		FILE_END[i] = 0;
	}
		
	short CONTINUE_READ = 1;
	Data * min_data;
	unsigned int min_position;
	short exist;
	unsigned int maxSize = MAX_TREE_SIZE;
	unsigned int counter, fileIndex;
	unsigned int dataSize[metadataSize];
	unsigned long long int lowers[metadataSize];
	unsigned long long int uppers[metadataSize];
	unsigned int sizes[metadataSize];
		
	counter = fileIndex = 0;
	sprintf(filename[fileIndex], "./storage/%u-%u.data", round, fileIndex);
	FILE * file = fopen(filename[0], "w");

	while(CONTINUE_READ){
		exist = 0;
		for(unsigned int i = 0; i < metadataSize; ++i){
			if(!files[i].end){
				exist = 1;
				min_data = files[i].readData[files[i].current];
				break;
			}
		}
		
		if(!exist)
			break;

		for(int i = metadataSize - 1; i >= 0; --i){
			if(!files[i].end && files[i].readData[files[i].current]->key < min_data->key){
				min_data = files[i].readData[files[i].current];
				min_position = i;
			}
		}

		// output the data;
		if(counter < maxSize){
			if(counter == 0){
				lowers[fileIndex] = min_data->key;
			}
			fprintf(file, "%llu %s\n",min_data->key, min_data->value);
			++counter;
		
		}else{
			fclose(file);
			dataSize[fileIndex] = counter;
			uppers[fileIndex] = min_data->key;
			sizes[fileIndex] = counter;
			++fileIndex;
			sprintf(filename[fileIndex], "./storage/%u-%u.data", round, fileIndex);
			file = fopen(filename[fileIndex], "w");
			counter = 1;
			lowers[fileIndex] = min_data->key+1;
			fprintf(file, "%llu %s\n", min_data->key, min_data->value);
		}

		for(int i = metadataSize - 1; i >= 0; --i){
			if(!files[i].end && files[i].readData[files[i].current]->key == min_data->key){
				files[i].current ++;
				if(files[i].current >= files[i].size){
					readBlockOfData(&files[i], chunkSize);
				}
			}
		}	
	}
	fclose(file);
	uppers[fileIndex] = min_data->key;
	sizes[fileIndex] = counter;
	++fileIndex;

	// update information to metadata;
	// atomic !!!
	// update content : 
	
	for(unsigned int i = 0; i < fileIndex; ++i){
		sys->metadatas[i]->lower = lowers[i];
		sys->metadatas[i]->upper = uppers[i];
		strcpy(sys->metadatas[i]->dataFilename, filename[i]);
		if(sys->metadatas[i]->inCache)
			dropOutOfCache(sys->metadatas[i]);
		sys->metadatas[i]->isUsed = 1;
		sys->metadatas[i]->dataSize = sizes[i];
	}
	for(unsigned int i = fileIndex; i < metadataSize; ++i){
		if(sys->metadatas[i]->inCache)
			dropOutOfCache(sys->metadatas[i]);
		sys->metadatas[i]->isUsed = 0;
	}

	unsigned int gap = metadataSize - fileIndex;
	for(unsigned int i = metadataSize; i < sys->size; ++i){
		sys->metadatas[i - gap] = sys->metadatas[i];
	}

	sys->size -= gap;

	

	
}

