#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 1000003

int main(int argc, const char * argv[]){
	unsigned int amount = 1000000 / 8;
	unsigned int* bitArray = (unsigned int*)malloc(sizeof(unsigned int)*amount);
	for(unsigned int i = 0; i < amount; ++i) bitArray[i] = 0;
	unsigned int which_int = atoi(argv[1]) >> 3;
	unsigned int index = atoi(argv[1]) - (which_int << 3);
	
	printf("which_int = %u index = %u\n",which_int,  index);
}
