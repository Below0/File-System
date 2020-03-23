#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main(int argc, char** argv){

	if(argc<3) return 0;

	int fd = open(argv[1], O_WRONLY|O_APPEND);
	if(fd<0) return 0;

	write(fd, argv[2], 10);

	return 0;
}
