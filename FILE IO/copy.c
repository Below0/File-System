#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv){
	size_t n_size;
	char buff[100];
	if(argc<3) return 0;

	int fd1 = open(argv[1],O_RDONLY);
	int fd2 = open(argv[2],O_WRONLY|O_CREAT,0644);
	int	check;
	if(!(fd1&&fd2)) return 0;
	while((check= read(fd1,buff,100))>0){
		write(fd2, buff,check);
	}

	close(fd1);
	close(fd2);
	
	



	return 0;
}
