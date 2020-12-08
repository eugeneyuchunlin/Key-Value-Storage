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
	lseek(fd, 400+1, SEEK_SET);
	write(fd, "", 1);
	lseek(fd, 0, SEEK_SET);

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

	char * ptr = (char *)mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(ptr == MAP_FAILED){
		printf("fail to mapping");
		exit(-3);
	}
	int pc;
	for(int i = 0; i < 10; ++i){
		ptr += sprintf(ptr, "%d\n", array[i]);
	}

	munmap(ptr, statbuf.st_size);
	return 0;
}
