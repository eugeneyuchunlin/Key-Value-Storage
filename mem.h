#ifndef __MEM_H__
#define __MEM_H__

typedef unsigned long size_t;
struct mem{
	void * memory;
	void ** currentPointer;
	size_t max_size;
};

#endif
