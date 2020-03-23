#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "record.h"
//필요하면 header file 추가 가능

//
// argv[1]: 레코드 파일명
// argv[2]: 레코드 파일에 저장되어 있는 전체 레코드의 수
//

int main(int argc, char **argv)
{
	if(argc != 3){
		printf("error!\n");
	
	}
	int fd = open(argv[1],O_RDONLY);
	if(fd<0){
		printf("open error!\n");
		return 0;

	}

	int i;
	char buff[250];
	int cnt = atoi(argv[2]);

	struct timeval  start, end;
	gettimeofday(&start, NULL);
	for(i = 0; i < cnt; i++){
		read(fd, buff, 250);
	}
	gettimeofday(&end, NULL);
	printf("%ld us\n",end.tv_usec - start.tv_usec);
	// 표준입력으로 받은 레코드 파일로부터 전체 레코드를 "순차적"으로 읽어들이고, 이때
	// 걸리는 시간을 측정하는 코드 구현함
	// 파일 i/o 시간 비용 측정 실험이므로 파일로부터 읽어 들인 레코드를 굳이 학생 구조체 변수에
    // 저장할 필요는 없음


	return 0;
}
