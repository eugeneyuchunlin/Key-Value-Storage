#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "data.h"
// #include "database.h"
#include "bloom_filter.h"
#include "btree.h"
#include "def.h"
#include "metadata.h"

// #define DEBUG

int main(int argc, const char * argv[]){
	
	// B_tree * tree = createTree(MAX_TREE_SIZE);
	// putData(tree, createData(1, (unsigned char *)"apple"));
	// putData(tree, createData(5, NULL));
	// printf("check result = %d\n", get_bloom_filter(tree->bitArray, 5));

	// basicFunction((char*)argv[1]);
	FILE * file = fopen(argv[1], "r");
	assert(file != NULL);
	FILE * outputFIle = fopen("result.out", "w");

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
			data = createData(key1, value);
			if(!putData(tree, data)){
				// printf("OUTPUT\n");
				outputTree(sys, tree);
				tree = createTree(MAX_TREE_SIZE, sys->size);
				putData(tree, data);
			}
			++dataAmount;
		}else if(c_mode[0] == 'G'){ // GET
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
