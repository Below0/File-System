#include <stdio.h>
#include "record.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//필요하면 header file 추가 가능

//
// argv[1]: 레코드 파일 students.dat에 저장되는 전체 레코드의 수
//

void Stu_Write(int fd, Student a){

	write(fd, a.id, 10);
	write(fd, a.name, 30);
	write(fd, a.address, 70);
	write(fd, a.univ, 30);
	write(fd, a.dept, 40);
	write(fd, a.others, 70);

}


int main(int argc, char **argv)
{
	if(argc != 2){
		printf("error\n");
		return 0;
	}
	int cnt = atoi(argv[1]);
	char tmp = 'a';
	int fd = open("students.dat",O_WRONLY|O_CREAT|O_TRUNC,0644);
	while(cnt-- != 0){
		Student tar;
		tar.id[0] = tmp;
		tar.name[0] = tmp;
		tar.address[0] = tmp;
		tar.univ[0] = tmp;
		tar.dept[0] = tmp;
		tar.others[0] = tmp++;
		Stu_Write(fd, tar);
	}
    // 250바이트 학생 레코드를 students.dat 파일에 하나씩 저장하는 코드를 구현함
    // 학생 레코드의 데이터는 임의로 생성해도 무방함
    
	return 0;
}
