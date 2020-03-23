#include <stdio.h>
#include "record.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//�ʿ��ϸ� header file �߰� ����

//
// argv[1]: ���ڵ� ���� students.dat�� ����Ǵ� ��ü ���ڵ��� ��
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
    // 250����Ʈ �л� ���ڵ带 students.dat ���Ͽ� �ϳ��� �����ϴ� �ڵ带 ������
    // �л� ���ڵ��� �����ʹ� ���Ƿ� �����ص� ������
    
	return 0;
}
