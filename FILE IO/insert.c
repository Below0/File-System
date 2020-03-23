#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
int main(int argc, char** argv){
	int fd;

	int str[100];
	if(argc<4) return 0;

	fd = open(argv[1], O_RDWR);
	int tmp = atoi(argv[2]);
	lseek(fd,tmp,SEEK_SET);
	int check;
	check=read(fd,str,1000);
		
	lseek(fd,tmp,SEEK_SET);

	write(fd,argv[3],10);

	write(fd,str,check);



	return 0;
}
