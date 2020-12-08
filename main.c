#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "data.h"
#include "hash.h"
#include "btree.h"

int main(int argc, const char * argv[]){
	B_tree * tree = createTree();
	srand(time(NULL));
	// unsigned long long int test_data[24] = {3150, 2997, 3538, 3264, 1750, 2528, 2505, 382, 844, 2837, 1200, 599, 2451, 1567, 1232, 327, 2266, 4870, 4028, 2583, 85, 3915, 1637, 1735};
	unsigned int times = (unsigned int)atoi(argv[1]);	
	for(unsigned int i = 0; i < times; ++i){
		insertNode(tree, createData(rand()%100000, NULL));	
		// printf("i = %u\n", i);
	}
	// insertNode(tree, createData(15, NULL));
	// insertNode(tree, createData(2, NULL));

	// insertNode(tree, createData(7, NULL));
	// insertNode(tree, createData(2, NULL));
	// insertNode(tree, createData(25, NULL));

	// insertNode(tree, createData(18, NULL));
	// insertNode(tree, createData(17, NULL));

	Output(tree);

	return 0;
}
