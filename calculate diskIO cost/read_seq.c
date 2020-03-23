#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "record.h"
//�ʿ��ϸ� header file �߰� ����

//
// argv[1]: ���ڵ� ���ϸ�
// argv[2]: ���ڵ� ���Ͽ� ����Ǿ� �ִ� ��ü ���ڵ��� ��
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
	// ǥ���Է����� ���� ���ڵ� ���Ϸκ��� ��ü ���ڵ带 "������"���� �о���̰�, �̶�
	// �ɸ��� �ð��� �����ϴ� �ڵ� ������
	// ���� i/o �ð� ��� ���� �����̹Ƿ� ���Ϸκ��� �о� ���� ���ڵ带 ���� �л� ����ü ������
    // ������ �ʿ�� ����


	return 0;
}
