#include "data.h"
#include <stdlib.h>

Instruction * createInstruction(MODE mode, unsigned long long int key1, unsigned long long int key2){
	Instruction* data = (Instruction*)malloc(sizeof(Instruction));
	data->key1 = key1;
	data->key2 = key2;
	data->mode = mode;
	return data;
}
Data * createData(unsigned long long int key, char * value){
	Data * data = (Data *)malloc(sizeof(Data));
	data->key = key;
	data->value = value;
	return data;
}

/*
Instruction * createInstruction(MODE mode, unsigned long long int key1, char * value){
	Instruction * data = (Instruction *)malloc(sizeof(Instruction));
	data->key1 = key1;
	data->mode = mode;
	data->value = value;
	return data;
}

Data * createData(unsigned long long int key, char * value){
	Data * data = (Data *)malloc(sizeof(Data));
	data->key = key;
	data->value = value;
	return data;
}
*/
