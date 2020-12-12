#include <stdlib.h>
#include <sys/signal.h>
#include <assert.h>
#include "bloom_filter.h"
#include "btree.h"

NODE * createNode(){
	NODE * node = (NODE *)malloc(sizeof(NODE));
	node->r_data = node->l_data = NULL;
	node->right = node->mid = node->left = node->parent = NULL;
	node->r_empty = node->l_empty = 1;
	return node;
}

B_tree * createTree(unsigned int MAX_SIZE){
	B_tree* tree = (B_tree *)malloc(sizeof(B_tree));
	tree->root = createNode();
	tree->size = 0;
	tree->max_size = MAX_SIZE;
	MAX_SIZE = MAX_SIZE >> 3;
	tree->bitArray = (unsigned int*)malloc(sizeof(unsigned int)*MAX_SIZE);
	return tree;
}


Data * getData(
		B_tree * tree,
		Data * data
){
	NODE * foundNode = searchNode(tree->root, data);
	
	if(!foundNode->l_empty && foundNode->l_data->key == data->key){
		data->value = foundNode->l_data->value;
	}else if(!foundNode->r_empty && foundNode->r_data->key == data->key){
		data->value = foundNode->r_data->value;
	}
	return data;
}

short putData(
		B_tree * tree,
		Data * data
){
	if(tree->size >= tree->max_size)
		return 0;

	// printf("insert -> %llu\n", data->key);
	// search the right node;
	NODE * foundNode = searchNode(tree->root, data);
	NODE * newNode;


	// FIND the KEY !!!
	if(!foundNode->l_empty && foundNode->l_data->key == data->key){
		free(foundNode->l_data);
		foundNode->l_data = data;
		return 1;
	}else if(!foundNode->r_empty && foundNode->r_data->key == data->key){
		free(foundNode->r_data);
		foundNode->r_data = data;
		return 1;
	}	
	
	
	if(foundNode->l_empty){
		foundNode->l_data = data;
		foundNode->l_empty = 0;
	}else if(data->key < foundNode->l_data->key && foundNode->r_empty){ //1 ->  [2, ] ===> exchange => [1, 2]
		foundNode->r_data = foundNode->l_data;
		foundNode->r_empty = 0;
		foundNode->l_data = data;
	}else if(data->key > foundNode->l_data->key && foundNode->r_empty){ // 2 -> [1, ] ===> [1, 2]
		foundNode->r_data = data;
		foundNode->r_empty = 0;
	}else if(!foundNode->l_empty && !foundNode->r_empty){// means that the founded node is full => 1 -> [2, 3]

		newNode = _insertNode(foundNode, data);
		if(newNode != NULL){
			assert(!newNode->l_empty);
			tree->root = newNode;	
		}
	}

	//put_bloom_filter(tree->bitArray	

	tree->size += 1;
	return 1;
}

NODE * searchNode(
		NODE * rootNode,
		Data * data
){
	if(rootNode->l_empty) // the tree is empty;
		return rootNode;
	
	if(rootNode->l_data->key == data->key){ // find it!!
		return rootNode;	
	}else if (!rootNode->r_empty && rootNode->r_data->key == data->key){
		return rootNode;
	}
	

	if(data->key < rootNode->l_data->key){
		if(rootNode->left == NULL) return rootNode; // means that the data need to be inserted in this node
		else return searchNode(rootNode->left, data); // search deeply
	}else if(data->key > rootNode->l_data->key && rootNode->r_empty){
		if(rootNode->mid == NULL) return rootNode;
		else return searchNode(rootNode->mid, data);
	}else if(data->key > rootNode->l_data->key && data->key < rootNode->r_data->key){
		if(rootNode->mid == NULL) return rootNode;
		else return searchNode(rootNode->mid, data);
	}else{
		if(rootNode->right == NULL) return rootNode;
		else return searchNode(rootNode->right, data);
	}
		
}

NODE * merge(
		NODE * node1,
		NODE * node2
){
	if(node2 == NULL) // means that node1 is root node, so node1->parent == NULL
		return node1;
	
	unsigned int empty_count = 0;
	NODE * newNode;
	if(node2->r_empty)
		++empty_count;

	if(empty_count){
		if(node1->l_data->key > node2->l_data->key){
			node2->r_data = node1->l_data;
			node2->r_empty = 0;

			node2->mid = node1->left;
			node2->mid->parent = node2;

			node2->right = node1->mid;
			node2->right->parent = node2;

		}else{
			node2->r_data = node2->l_data;
			node2->l_data = node1->l_data;
			node2->r_empty = 0;

			node2->right = node2->mid;
			// node2->right->parent = node2;

			node2->mid = node1->mid;
			node2->mid->parent = node2;

			node2->left = node1->left;
			node2->left->parent = node2;
		}
		free(node1);
		return NULL;
	}else{ // empty count == 0
		if(node2->left == NULL){ // left
			newNode = createNode();
			newNode->l_data = node2->l_data;
			newNode->l_empty = 0;

			node2->l_data = node2->r_data;
			node2->r_empty = 1;

			node2->left = node2->mid;
			// node2->left->parent = node2; // stupid pointer,  point to itself

			node2->mid = node2->right;
			// node2->mid->parent = node2;
			node2->right = NULL;

			newNode->left = node1;

			newNode->mid = node2;

			newNode->parent = node2->parent;
			if(node2->parent){
				if(node2->parent->left == node2) node2->parent->left = NULL;
				else if(node2->parent->mid == node2) node2->parent->mid = NULL;
				else node2->parent->right = NULL;
			}
			
			newNode->mid->parent = newNode;
			newNode->left->parent = newNode;
			// node2->parent = node1->parent = newNode;
		}else if (node2->mid == NULL){ // mid
			newNode = createNode();
			newNode->l_data = node1->l_data;
			newNode->l_empty = 0;
			
			NODE * lNode = createNode();
			lNode->l_data = node2->l_data;
			lNode->l_empty = 0;

			lNode->left = node2->left;
			lNode->left->parent = lNode;
			lNode->mid = node1->left;
			lNode->mid->parent = lNode;

			node2->l_data = node2->r_data;
			node2->r_empty = 1;

			node2->left = node1->mid;
			node2->left->parent = node2;
			node2->mid = node2->right;
			// node2->mid->parent = node2;
			node2->right = NULL;
			
			if(node2->parent){
				if(node2->parent->left == node2) node2->parent->left = NULL;
				else if(node2->parent->mid == node2) node2->parent->mid = NULL;
				else node2->parent->right = NULL;
			}
			

			newNode->parent = node2->parent;

			newNode->left = lNode;
			newNode->left->parent = newNode;
			newNode->mid = node2;
			newNode->mid->parent = newNode;

			free(node1);
		}else{ // right
			newNode = createNode();
			newNode->l_data = node2->r_data;
			newNode->l_empty = 0;
			
			node2->r_empty = 1;
			newNode->left = node2;
			newNode->mid = node1;

			if(node2->parent){
				if(node2->parent->left == node2) node2->parent->left = NULL;
				else if(node2->parent->mid == node2) node2->parent->mid = NULL;
				else node2->parent->right = NULL;
			}
			
			newNode->parent = node2->parent;
			newNode->left->parent = newNode;
			newNode->mid->parent = newNode;
			// node2->parent = newNode;
			// node1->parent = newNode;
		}

		return merge(newNode, newNode->parent);
	}

	
}


NODE * _insertNode( 
		NODE * node,
		Data * data
){
	// determined which data is middle value
	
	Data *midData;
	Data *smallData;
	Data *bigData;

	smallData = node->l_data;
	bigData = node->r_data;

	if(data->key < node->l_data->key){
		midData = node->l_data;
		smallData = data;
	}else if (node->l_data->key < data->key && data->key < node->r_data->key){
		midData = data;
	}else{
		midData = node->r_data;	
		bigData = data;
	}


	// create the subtree
	NODE * rNode = createNode();
	rNode->l_data = bigData;
	rNode->l_empty = 0;

	NODE * lNode = createNode();
	lNode->l_data = smallData;
	lNode->l_empty = 0;

	node->r_empty = 1;
	node->l_data = midData;
	node->left = lNode;
	node->mid = rNode;

	node->left->parent = node;
	node->mid->parent = node;
	

	if(node->parent){
		if(node->parent->left == node) node->parent->left = NULL;
		else if(node->parent->mid == node) node->parent->mid = NULL;
		else node->parent->right = NULL;
	}

	return merge(node, node->parent);
}



void Breadth_first_search(B_tree * tree){
	// Data ** queue = (Data **)malloc(sizeof(Data *)*tree->size);
	// unsigned int tail_index = 1;
	// unsigned int head_index = 0;

	// Level ** levels = (Level **)malloc(sizeof(Level *) * 100);
	// levels[0] = createLevel(2, &queue[head_index]);
	// head_index += 2;
	// tail_index += 2;

	// // while(){
	// // 	if(queue[head_index]->
	// // 	++head;	
	// // }

	// free(queue);	
}

void Output(B_tree * tree){
	FILE * file;
	file = fopen("test.txt", "w");
	Depth_first_search(tree->root, file);	
}


void Depth_first_search(NODE * node, FILE * file){
	if(node->l_empty) return;
	if(node->left) Depth_first_search(node->left, file);	
	fprintf(file, "%llu %s\n", node->l_data->key, node->l_data->value);
	if(node->mid) Depth_first_search(node->mid, file);
	if(!node->r_empty) fprintf(file, "%llu %s\n", node->r_data->key, node->r_data->value);
	
	if(node->right) Depth_first_search(node->right, file);
}

void Clear_tree(B_tree * tree){
	Clear_children(tree->root);	
	free(tree);
}


void Clear_children(NODE * node){
	if (node->left) Clear_children(node->left);
	if (node->mid) Clear_children(node->mid);
	if (node->right) Clear_children(node->right);
	if(!node->l_empty) free(node->l_data);	
	if(!node->r_empty) free(node->r_data);
	free(node);
	return;

}

