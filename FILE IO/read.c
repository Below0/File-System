#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>


int main(int argc, char** argv){
	int fd;
	if(argc <3 ) { return 0;}
	char str[11];
	fd = open(argv[1],O_RDONLY);
	if(fd<0) return 0;
	int inp = atoi(argv[2]);
	lseek(fd, inp, SEEK_SET);
	read(fd, str, 10);
	printf("%s\n", str);
	return 0;
}
