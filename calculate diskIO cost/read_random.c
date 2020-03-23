#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
//�ʿ��ϸ� header file �߰� ����


#define SUFFLE_NUM	10000	// �� ���� ������� ������ �� ����

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// �ʿ��� �Լ��� ������ �� �߰��� �� ����

//
// argv[1]: ���ڵ� ���ϸ�
// argv[2]: ���ڵ� ���Ͽ� ����Ǿ� �ִ� ��ü ���ڵ� ��
//
int main(int argc, char **argv){
	int *read_order_list;
	if(argc != 3){
	printf("argc error!\n");
	return 0;
	
	}


	int num_of_records = atoi(argv[2]);
	read_order_list = (int*)malloc(num_of_records * sizeof(int));
	// �� �Լ��� �����ϸ� 'read_order_list' �迭�� �о�� �� ���ڵ� ��ȣ���� ������� �����Ǿ� �����
    //'num_of_records'�� ���ڵ� ���Ͽ� ����Ǿ� �ִ� ��ü ���ڵ��� ���� �ǹ���
	GenRecordSequence(read_order_list, num_of_records);
	int fd = open(argv[1], O_RDONLY);
	if( fd < 0){
	printf("open error!\n");
	return 0;
	}
	int i;
	char buff[251];
	struct timeval start, end;
	gettimeofday(&start, NULL);
	for(i = 0; i < num_of_records; i++){
		lseek(fd, 250*(read_order_list[i]),SEEK_SET);
		read(fd, buff, 250);
	}
		gettimeofday(&end, NULL);
	printf("%ld us\n", end.tv_usec - start.tv_usec);

	// 'read_order_list'�� �̿��Ͽ� ǥ�� �Է����� ���� ���ڵ� ���Ϸκ��� ���ڵ带 random �ϰ� �о���̰�,
    // �̶� �ɸ��� �ð��� �����ϴ� �ڵ� ������

	return 0;
	}

void GenRecordSequence(int *list, int n){
	int i, j, k;
	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
