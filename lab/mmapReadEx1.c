#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
	int N = 5;
	int array[10] = {1, 2, 3 ,4,5};
	int fd = open("OS", O_RDWR | O_CREAT);

	if(fd < 0){
		printf("fail to open file");
		exit(-1);
	}

	struct stat statbuf;
	int err = fstat(fd, &statbuf);
	if(err < 0){
		printf("fail to open file");
		exit(-2);
	}

	int * ptr = (int *)mmap(NULL, 1024, PROT_READ, MAP_SHARED, fd, 0);
	if(ptr == MAP_FAILED){
		printf("fail to mapping");
		exit(-3);
	}
	for(int i = 0; i < 10; ++i){
		printf("%d\n", ptr[i]);
	}

	munmap(ptr, statbuf.st_size);
	return 0;
}
