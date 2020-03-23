#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char** argv){
	if(argc < 4) return 0;
	
	int fd = open(argv[1],O_RDWR);
	int offset = atoi(argv[2]);
	lseek(fd,offset,SEEK_SET);
	write(fd,argv[3],10);

	return 0;
}
