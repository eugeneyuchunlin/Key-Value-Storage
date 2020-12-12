#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "btree.h"

void basicFunction(char * file_name);

void outputTree(unsigned int number, B_tree * tree);

#endif
