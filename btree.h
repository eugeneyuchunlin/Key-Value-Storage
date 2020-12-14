#ifndef __B_TREE_H__
#define __B_TREE_H__
#include "data.h"
#include "bloom_filter.h"
#include <stdio.h>

/** In this header file, I'd like to define the 2-3 B tree
 *
 */


struct NODE;

struct NODE{
	Data * l_data;
	short l_empty;

	Data * r_data;
	short r_empty;

	
	struct NODE * parent;
	struct NODE * left;
	struct NODE * mid;
	struct NODE * right;
};
typedef struct NODE  NODE;

struct B_tree{
	struct NODE * root;
	unsigned int size;
	unsigned int number;
	unsigned int max_size;
	unsigned char* bitArray;
	unsigned int bitArraySize;
};
typedef struct B_tree B_tree;


NODE * createNode();
B_tree * createTree(unsigned int MAX_SIZE, unsigned int number);

short putData(
		B_tree * tree,
		Data * data
);

NODE * _insertNode(
		NODE * node,
		Data * data
);

NODE * searchNode(
		NODE * rootNode,
		Data * data
);

Data * getData(B_tree * tree, Data * );

void Breadth_first_search(
		B_tree * tree
);

void Clear_tree(B_tree * tree);
void Clear_children(NODE* node);

void Output(B_tree * tree, FILE * file, Data ** collectedData, unsigned int*size);

void Depth_first_search(NODE * node, FILE *, Data ** collectedData, unsigned int *size);

void Test_parent_relationship(NODE *root);

#endif
