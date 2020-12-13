#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "data.h"
// #include "database.h"
#include "bloom_filter.h"
#include "btree.h"
#include "def.h"
#include "metadata.h"

// #define DEBUG


char * outputFileName(const char * inputfileName){
	// find .
	unsigned int i, index;
	index = 0;
	char * outputname = (char *)malloc(sizeof(char)*(strlen(inputfileName)+2));
	
	char * suffix;
	for(i = strlen(inputfileName) - 1; i >= 0; --i){
		if(inputfileName[i] == '/'){
			index = i;
			break;
		}
	}

	suffix = (char *)inputfileName + index + 1;
	index = 0;	


	for(i = 0; i < strlen(suffix); ++i){
		if(suffix[i] == '.'){
			index = i;
			break;
		}
	}

	if(index == 0)
		return NULL;
	strcpy(outputname, suffix);
	strcpy(outputname+index+1, "output");

	return outputname;
}

int main(int argc, const char * argv[]){
	
	// B_tree * tree = createTree(MAX_TREE_SIZE);
	// putData(tree, createData(1, (unsigned char *)"apple"));
	// putData(tree, createData(5, NULL));
	// printf("check result = %d\n", get_bloom_filter(tree->bitArray, 5));

	// basicFunction((char*)argv[1]);
	FILE * file = fopen(argv[1], "r");
	assert(file != NULL);
	char * output_file_name = outputFileName(argv[1]);
	FILE * outputFIle = fopen(output_file_name, "w");
	FILE * log = fopen("./storage/db.log", "a+");

	unsigned char c_mode[100];
	unsigned long long int key1, key2;
	unsigned char * value = (unsigned char *)malloc(sizeof(unsigned char)*129);
	Data *data;
	MetaDataSys * sys = createMetaDataSys();
	TableCache * cache = createTableCache(8);

	B_tree * tree = createTree(MAX_TREE_SIZE, sys->size);
	
	Data * gData = createData(0, NULL);
	
	unsigned int dataAmount = 0;

	while(fscanf(file, "%s", c_mode) != EOF){
		if(c_mode[0] == 'P'){ // PUT
			fscanf(file, "%llu", &key1);
			value = (unsigned char *)malloc(sizeof(char)*129); 
			fscanf(file, "%s", value);
			fprintf(log, "%llu %s\n", key1, value);
			// free(value);
			// continue;
			data = createData(key1, value);
			if(!putData(tree, data)){
				// printf("OUTPUT\n");
				outputTree(sys, tree);
				tree = createTree(MAX_TREE_SIZE, sys->size);
				putData(tree, data);
			}
			++dataAmount;
		}else if(c_mode[0] == 'G'){ // GET
			// continue;
			fscanf(file, "%llu", &key1);
			gData->key = key1;
			gData->value = NULL;
			gData = getData(tree, gData);
			if(gData->value == NULL){
				// if memtable not found, use table cache
				searchData(cache, sys, gData);	
				if(gData->value == NULL)
#ifdef DEBUG
						printf("%llu EMPTY\n", gData->key);
#else 
						fprintf(outputFIle, "EMPTY\n");
#endif
				else
#ifdef DEBUG
					printf("%llu %s\n",gData->key, gData->value);
#else
					fprintf(outputFIle, "%s\n", gData->value);
#endif
			}else{
#ifdef DEBUG
					printf("%llu %s\n",gData->key, gData->value);
#else
					fprintf(outputFIle, "%s\n", gData->value);
#endif

			}
			gData->value = NULL;
		}else{ // SCAN
			// continue;	
			fscanf(file, "%llu %llu", &key1, &key2);
			// printf("SCAN !!!from %llu to %llu\n\n", key1, key2);
			for(unsigned long long int i = key1; i <= key2; ++i){
				gData->key = i;
				gData = getData(tree, gData);
				if(gData->value == NULL){
					searchData(cache, sys, gData);
					if(gData->value == NULL)
#ifdef DEBUG
						printf("%llu EMPTY\n", gData->key);
#else 
						fprintf(outputFIle, "EMPTY\n");
#endif
					else
#ifdef DEBUG
					printf("%llu %s\n",gData->key, gData->value);
#else
					fprintf(outputFIle, "%s\n", gData->value);
#endif

				}else{
#ifdef DEBUG
					printf("%llu %s\n",gData->key, gData->value);
#else
					fprintf(outputFIle, "%s\n", gData->value);
#endif
				}
				gData->value = NULL;
			}
		}
	}
	// printf("%s\n", tree->root->mid->left->l_data->value);
	// file = fopen("tmp", "w");
	// Depth_first_search(tree->root, file);
	outputTree(sys, tree);
	outputMetaDataSys(sys);
	// outputTree(
	// Output(tree);
	return 0;
}
