#ifndef __DATA_H__
#define __DATA_H__

enum MODE{
	PUT,
	GET,
	SCAN
};

typedef enum MODE MODE;

struct Instruction{
	MODE mode;
	unsigned long long int key1;
	unsigned long long int key2;
	char * value;
};

typedef struct Instruction Instruction;

struct Data{
	unsigned long long int key;
	char * value;
};

typedef struct Data Data;

Data * createData(unsigned long long int, char *);

// Instruction * createInstruction(MODE, unsigned long long int, unsigned long long int);

// Instruction * createInstruction(MODE, unsigned long long int, char *);




#endif
