#include "database.h"
#include "btree.h"
#include "data.h"
#include "def.h"

void basicFunction(char * file_name){
	FILE * file = fopen(file_name, "r");
	assert(file != NULL);
	unsigned char c_mode[100];
	unsigned long long int key1, key2;
	unsigned char * value = (unsigned char *)malloc(sizeof(char)*129);
	Data *data;

	B_tree * tree = createTree(MAX_TREE_SIZE);
	
	Data * gData = createData(0, NULL);
	
	unsigned int dataAmount = 0;

	while(fscanf(file, "%s", c_mode) != EOF){

		if(c_mode[0] == 'P'){ // PUT
			fscanf(file, "%llu", &key1);
			value = (unsigned char *)malloc(sizeof(char)*129); 
			fscanf(file, "%s", value);
			data = createData(key1, value);
			if(!putData(tree, data)){
				printf("OUTPUT\n");
				outputTree(0, tree); // should pass for metadata engin
				tree = createTree(MAX_TREE_SIZE);
				putData(tree, data);
			}
			++dataAmount;
		}else if(c_mode[0] == 'G'){ // GET
			fscanf(file, "%llu", &key1);
			gData->key = key1;
			gData = getData(tree, gData);
			if(data->value == NULL){
				printf("EMPTY\n");
			}else{
				printf("%s\n", data->value);
			}
			gData->value = NULL;
		}
	}
	
}


void outputTree(unsigned int number, B_tree * tree){

}
