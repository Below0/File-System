#include <stdio.h>
#include <stdlib.h>
#include "student.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
typedef struct _table{
	int size;
	int offset;


}Table;

short readIndex(FILE* idx);
int writeIndex(FILE* idx, short n);
int table_size;
int search_check;
//
// �л� ���ڵ� ���Ͽ� ���ڵ带 �����ϱ� ���� ����ü STUDENT�� ����Ǿ� �ִ� �����͸� ���ڵ� ���·� �����.
// 
int pack(char *recordbuf, const STUDENT *s){
	int i = 0;
	int index = 0;
	while(s->id[i] != '\0'){
	recordbuf[index++] = s->id[i++];
	}
	recordbuf[index++] = '|';
	i = 0;
	while(s->name[i] !='\0'){
		recordbuf[index++] = s->name[i++];
	}
	recordbuf[index++] = '|';
	i = 0;
	while(s->addr[i] != '\0'){
		recordbuf[index++] = s->addr[i++];
	}
	recordbuf[index++] = '|';
	recordbuf[index++] = s->year[0];
	recordbuf[index++] = '|';
	i = 0;
	while(s->dept[i] != '\0'){
		recordbuf[index++] = s->dept[i++];
	}
	i = 0;
	recordbuf[index++] = '|';
	while(s->phone[i] != '\0'){
		recordbuf[index++] = s->phone[i++];
	}
	recordbuf[index++] = '|';
	i = 0;
	while(s->email[i] != '\0'){
		recordbuf[index++] = s->email[i++];
	}
	recordbuf[index++] = '|';
	recordbuf[index] = '\0';
	return index;
}


// 
// �л� ���ڵ� ���Ϸκ��� ���ڵ带 �о� �� �� ����ü STUDENT�� ��ȯ�Ѵ�.
//
void unpack(const char *recordbuf, STUDENT *s){
char *temp;
char copy[MAX_RECORD_SIZE];
strcpy(copy,recordbuf);
temp = strtok(copy,"|");
strcpy(s->id, temp);
temp = strtok(NULL,"|");
strcpy(s->name, temp);
temp = strtok(NULL,"|");
strcpy(s->addr, temp);
temp = strtok(NULL,"|");
strcpy(s->year, temp);
temp = strtok(NULL,"|");
strcpy(s->dept, temp);
temp = strtok(NULL,"|");
strcpy(s->phone, temp);
temp = strtok(NULL,"|");
strcpy(s->email, temp);

}

//
// �л� ���ڵ� ���Ϸκ���  ���ڵ� ��ȣ�� �ش��ϴ� ���ڵ带 �о ���ڵ� ���ۿ� �����Ѵ�.
//
void readRecord(FILE *fp, char *recordbuf, int rn){
	FILE* idx;
	char temp[MAX_RECORD_SIZE];
	if((idx = fopen(INDEX_FILE_NAME,"rb"))==NULL) return ;
	int offset,cnt = 0, index = 0;
	fseek(idx, 2+2*rn, SEEK_SET);
	offset = readIndex(idx);
	fseek(fp, 2+offset, SEEK_SET);
	while(cnt != 7){
	fread(&recordbuf[index], 1, 1, fp);
	if(recordbuf[0] == '*'){
	break;
	}
	else if(recordbuf[index] == '|') cnt++;
	index++;
	}
	recordbuf[index] = '\0';
	fclose(idx);


}

//
//�˰���: ����
//�л� ���ڵ� ���Ͽ��� ���� ���ڵ��� ���� ���θ� �˻��� �� ���� ���ڵ尡 �����ϸ� �� ������
//���ο� ���ڵ带 �����ϸ�, ���� ���� ���ڵ尡 �������� �ʰų� ���ǿ� �����ϴ� ���� ���ڵ尡
//�������� ������  ������ �Ǹ������� �����Ѵ�.
//
void add(FILE *fp, const STUDENT *s);

//
//�˰���: �˻�
//�л� ���ڵ� ���Ͽ��� sequential search ������� �־��� �й� Ű���� ��ġ�ϴ� ���ڵ带 ã�´�.
//����� ����� �־��� print �Լ��� ����Ѵ�.
//�˻� ���ڵ尡 �����ϸ� ���ڵ� ��ȣ rn��, �׷��� ������ -1�� �����Ѵ�.
//
int search(FILE *fp, const char *keyval);

//
//�˰���: ����
//�л� ���Ͽ��� �־��� �й� Ű���� ��ġ�ϴ� ���ڵ带 ã�� �� �ش� ���ڵ带 ���� ó���Ѵ�.
//
void delete(FILE *fp, const char *keyval);
void printRecord(const STUDENT *s, int n);
void init();
int main(int argc, char *argv[])
{
	//printf("student.c %s %d \n",argv[1], argc);
	FILE *fp;  // �л� ���ڵ� ������ ���� ������
	init();

	if(argc < 2) return 0;
	else{
		if(!strcmp(argv[1],"-a")){
		if(argc != 9){
		printf("input error!\n");
		return 0;
		}
		fp = fopen(RECORD_FILE_NAME, "r+b");
		STUDENT st;
		strcpy(st.id, argv[2]);
		strcpy(st.name,argv[3]);
		strcpy(st.dept, argv[4]);
		strcpy(st.year, argv[5]);
		strcpy(st.addr, argv[6]);
		strcpy(st.phone, argv[7]);
		strcpy(st.email, argv[8]);
		add(fp, &st);
		}
		else if(!strcmp(argv[1],"-d")){
		fp = fopen(RECORD_FILE_NAME, "r+b");
		delete(fp, argv[2]);
		
		}
		else if(!strcmp(argv[1],"-s")){
		int res;
		search_check = 1;
		fp = fopen(RECORD_FILE_NAME, "r+b");
		res = search(fp, argv[2]);
		//if(res == -1) printf("Search Failure!\n");
		}
	}
	fclose(fp);

	return 0;
	
}
int readSize(FILE* fp){
	int i = 0,cnt = 0;
	int size = 0;
	char temp[MAX_RECORD_SIZE];
	while(cnt != 7){
	size += fread(temp, 1, 1, fp);
	if(temp[0] == '|') cnt++;
	}
	return size;

}

short readIndex(FILE* idx){
	short target;
	fread(&target, sizeof(short), 1, idx);
	return target;
}
int writeIndex(FILE* idx, short n){
	return fwrite(&n, sizeof(short), 1, idx);
	}

void printRecord(const STUDENT *s, int n)
{
	int i;

	for(i=0; i<n; i++)
	{
		printf("%s|%s|%s|%s|%s|%s|%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}
Table* sizeTable(FILE *idx ){
	fseek(idx, 0, SEEK_SET);
	Table *table;
	table_size = readIndex(idx);
	table = (Table*)malloc(sizeof(Table)*table_size);
	int i = 0;
	for(i = 0; i < table_size; i++){
		table[i].offset = readIndex(idx);
	}
	for(i = 0; i < table_size-1; i++){
		table[i].size = table[i+1].offset - table[i].offset;
	}
	table[table_size-1].size = MAX_RECORD_SIZE+1;
	fseek(idx, 0, SEEK_SET);
	return table;
}
int searchTable(Table *table, int offset){
	int i =0;
	for(i = 0; i < table_size; i++){
		if(table[i].offset == offset) return table[i].size;
	}
	return -1;

}
void add(FILE *fp, const STUDENT *s){
	FILE *idx;
	idx = fopen(INDEX_FILE_NAME, "r+b");
	char idx_buf[3],rec_buf[3];
	int i, j, offset, size = 0;
	short target= 0;
	int del_size;
	int head, length, check = 0;
	Table *table;
	table = sizeTable(idx);

	char record[MAX_RECORD_SIZE];
	length = pack(record, s);
	fseek(fp, 0, SEEK_SET);
	target = readIndex(fp);
	int temp;
	short next = -1, prev = -1;
	if(target != -1){ //first fit
		while(target != -1){
			fseek(fp, 2+target, SEEK_SET);
			fread(idx_buf, 1, 1, fp);
			if(idx_buf[0] == '*'){
				next = readIndex(fp);
				del_size = searchTable(table, target);
				if(del_size >= length){
					check = 1;
					if(prev == -1){
						fseek(fp, 0, SEEK_SET); // header change
						writeIndex(fp, next);
					}
					else{
						fseek(fp, 3+prev, SEEK_SET);// linkedlist change prev -> next
						writeIndex(fp, next);
					}
					fseek(fp, 2+target, SEEK_SET);
					break;
				}
				prev = target;
				target = next;
			}
		}
	}
	if(check == 0){ // test
	 //no  first fit
	fseek(idx, 0, SEEK_SET);
	head = readIndex(idx);
	fseek(idx, 0, SEEK_SET);
	writeIndex(idx, head+1);
	if(head == 0){
	writeIndex(idx, 0);
	}
	else{
	fseek(idx, 2*head, SEEK_SET);
	offset = readIndex(idx);
	fseek(fp, 2+offset, SEEK_SET);
	size = readSize(fp);
	writeIndex(idx, offset+size);
	}
	}//test erase test
	fwrite(record,1,length,fp);
}
int search(FILE *fp, const char *keyval){
	FILE* idx;
	STUDENT s;
	if((idx = fopen(INDEX_FILE_NAME, "rb"))==NULL){
	//	printf("no index file!\n");
		return -1;
	}
	int head = 0;

	int i;
	head = readIndex(idx);
	char buf[MAX_RECORD_SIZE];
	for(i = 0; i < head; i++){
	readRecord(fp,buf, i);
	if(buf[0] == '\0') continue;
	unpack(buf, &s);
	if(!strcmp(s.id, keyval)){
	if(search_check == 1) printRecord(&s,1);
	fclose(idx);
	return i;
	}
	}
	fclose(idx);
	return -1;
}
void delete(FILE *fp, const char *keyval){
	FILE* idx = fopen(INDEX_FILE_NAME,"rb");
	int offset = search(fp, keyval);
	char buf[MAX_RECORD_SIZE];
	int head, target, size;
	fseek(idx, 2+2*offset, SEEK_SET);
	target = readIndex(idx);
	fseek(fp, 0, SEEK_SET);
	head = readIndex(fp);
	if(offset == -1) return;
	fseek(fp, target, SEEK_CUR);
	size = readSize(fp);
	fseek(fp, -size, SEEK_CUR);
	fwrite("*", 1, 1,fp);
	writeIndex(fp, head);
	fwrite(buf,1, size-3,fp);
	fseek(fp, 0, SEEK_SET);
	writeIndex(fp, target);
	
	fclose(idx);
}
void init(){
	FILE *fp;
	FILE *inx;
 // �л� ���ڵ� ������ ���� ������
if(!(inx=fopen(INDEX_FILE_NAME, "rb"))){

	inx = fopen(INDEX_FILE_NAME, "w+b"); 
	if(writeIndex(inx, 0));// printf("index create!\n");
	fclose(inx);
}	
if(!(fp=fopen(RECORD_FILE_NAME, "rb"))){
	fp = fopen(RECORD_FILE_NAME, "w+b");
	short init = -1;

	if(fwrite(&init, sizeof(short), 1, fp));// printf("record create!\n");
	fclose(fp);
}
}
