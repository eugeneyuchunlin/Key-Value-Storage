#ifndef __B_TREE_H__
#define __B_TREE_H__
#include "data.h"
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
};

typedef struct B_tree B_tree;

struct Level{
	struct Data ** datas;
	unsigned int index;
	unsigned int max;
};

typedef struct Level Level;

NODE * createNode();
B_tree * createTree();
Level * createLevel(unsigned int, Data **);

void insertNode(
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

void Breadth_first_search(
		B_tree * tree
);

void Output(B_tree * tree);

void Depth_first_search(NODE * node, FILE *);

void Test_parent_relationship(NODE *root);

#endif
