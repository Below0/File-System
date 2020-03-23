#include <stdio.h>
#include "student.h"
#include <string.h>
#include <stdlib.h>

FILE* hash;
int writeRecord(FILE*fp);
int recordCount;
//
// �л� ���ڵ� ���Ϸκ���  ���ڵ� ��ȣ�� �ش��ϴ� ���ڵ带 �о� ���ڵ� ���ۿ� �����Ѵ�.
//
void readStudentRec(FILE *fp, char *recordbuf, int rn)
{
	int cnt;
	fseek(fp, STUDENT_RECORD_SIZE*rn, SEEK_SET);
	cnt = fread(recordbuf, SID_FIELD_SIZE, 1, fp);
	cnt += fread(&recordbuf[10], 20, 1, fp);
	cnt += fread(&recordbuf[30], 30, 1, fp);
	cnt += fread(&recordbuf[60], 1, 1, fp);
	cnt += fread(&recordbuf[61], 19, 1, fp);
	cnt += fread(&recordbuf[80], 15, 1, fp);
	cnt += fread(&recordbuf[95], 25, 1, fp);
	if(cnt <= 0){
		memset(recordbuf, 0, STUDENT_RECORD_SIZE);
	}
}

//
// Hash file�κ��� rn�� ���ڵ� ��ȣ�� �ش��ϴ� ���ڵ带 �о� ���ڵ� ���ۿ� �����Ѵ�.
//
void readHashRec(FILE *fp, char *recordbuf, int rn)
{
	fseek(fp, 4+(HASH_RECORD_SIZE*rn), SEEK_SET);
	fread(recordbuf, HASH_RECORD_SIZE,1, fp);
}

//
// Hash file�� rn�� ���ڵ� ��ȣ�� �ش��ϴ� ��ġ�� ���ڵ� ������ ���ڵ带 �����Ѵ�.
//
void writeHashRec(FILE *fp, const char *recordbuf, int rn)
{
	char temp[HASH_RECORD_SIZE+1];
	int check;
	int table_size;
	fseek(fp, 0, SEEK_SET);
	fread(&table_size, 4, 1, fp);
	int loop = 1;
	int i;
	for(i = 0; i < table_size; i++){
	check = 0;
	temp[0] = '\0';
	fseek(fp, 4+HASH_RECORD_SIZE*rn++,SEEK_SET);
	check = fread(temp,HASH_RECORD_SIZE,1,fp);

	if(check <= 0 || temp[0] <= 0){
		break;
	}
	if(rn == table_size -1) rn = 0;
	}
	if(rn<=table_size-1){
	fseek(fp, 4+HASH_RECORD_SIZE*(rn-1),SEEK_SET);
	fwrite(recordbuf, SID_FIELD_SIZE, 1, fp);
	fwrite(&recordCount, sizeof(recordCount), 1, fp);
	}
	else{
		printf("offset is over!\n");
	}
}

//
// n�� ũ�⸦ ���� hash file���� �־��� �й� Ű���� hashing�Ͽ� �ּҰ�(���ڵ� ��ȣ)�� �����Ѵ�.
//
int hashFunction(const char *sid, int n){
	int temp = strlen(sid);
	return ((int)sid[temp-1]*(int)sid[temp-2])%n;

}

//
// n�� ũ�⸦ ���� hash file�� �����Ѵ�.
// Hash file�� fixed length record ������� ����Ǹ�, ���ڵ��� ũ��� 14 ����Ʈ�̴�.
// (student.h ����)
//
void makeHashfile(int n)
{
	if((hash = fopen(HASH_FILE_NAME, "w+b"))==NULL){
			printf("create hash error\n");
			return;
		}
	
	FILE *fp;
	if((fp = fopen(RECORD_FILE_NAME, "rb"))==NULL){
		printf("no record file in makeHashfile()\n");
		return;
	}
	int hash_num = 0;
	recordCount = 0;
	char recordbuf[STUDENT_RECORD_SIZE+1];
	char key[SID_FIELD_SIZE+1];
	fwrite(&n, sizeof(n), 1, hash);
	do{
		readStudentRec(fp,recordbuf,recordCount);
		strncpy(key,recordbuf,SID_FIELD_SIZE);
		if(!strcmp(key,"\0")) break;
		hash_num = hashFunction(key, n);
		writeHashRec(hash,key,hash_num);
		recordCount++;
	}while(strcmp(recordbuf,"\0"));
	// Hash file�� ������ �� �� ������ �Ǿպκп� 4����Ʈ ����� �д�. 
	// ���⿡�� hash file�� ũ�� n�� �����Ѵ�. �̰��� search()�� (�ʿ��ϸ�) delete()�� 	// ���� ���̴�.
	fclose(hash);
	fclose(fp);
}

//
// �־��� �й� Ű���� hash file���� �˻��Ѵ�.
// �� ����� �־��� �й� Ű���� �����ϴ� hash file������ �ּ�(���ڵ� ��ȣ)�� search length�̴�.
// �˻��� hash file������ �ּҴ� rn�� �����ϸ�, �̶� hash file�� �־��� �й� Ű����
// �������� ������ rn�� -1�� �����Ѵ�. (search()�� delete()������ Ȱ���� �� ����)
// search length�� �Լ��� ���ϰ��̸�, �˻� ����� ������� search length�� �׻� ���Ǿ�� �Ѵ�.
//
int search(const char *sid, int *rn)
{

	int table_size;
	int hash_f;
	fseek(hash, 0, SEEK_SET);
	fread(&table_size, sizeof(int), 1, hash);
	hash_f = hashFunction(sid, table_size);
	fseek(hash, HASH_RECORD_SIZE*hash_f, SEEK_CUR);
	int i, cnt = 0, recordnum = 0;
	char temp[HASH_RECORD_SIZE+1];
	char key[SID_FIELD_SIZE+1];
	for(i = 0; i < table_size; i++){
		cnt++;
		temp[0] = '\0';
		key[0] = '\0';
		fread(temp, HASH_RECORD_SIZE,1, hash);
		strncpy(key, temp, SID_FIELD_SIZE);

		if(!(strcmp(key,sid))){
			*rn = hash_f+recordnum;
			return cnt;
		}
		if(hash_f + i == table_size -1){
			fseek(hash, 4, SEEK_SET);
			hash_f = 0;
			recordnum = 0;
		}
		recordnum++;
	}
	*rn = -1;
	return cnt;
}

//
// Hash file���� �־��� �й� Ű���� ��ġ�ϴ� ���ڵ带 ã�� �� �ش� ���ڵ带 ���� ó���Ѵ�.
// �̶� �л� ���ڵ� ���Ͽ��� ���ڵ� ������ �ʿ����� �ʴ�.
//
void delete(const char *sid){
	int target;
	char erase[HASH_RECORD_SIZE+1];
	memset(erase,0,HASH_RECORD_SIZE);
	search(sid, &target);
	fseek(hash, 4+(target*HASH_RECORD_SIZE),SEEK_SET);
//	fwrite(erase, HASH_RECORD_SIZE, 1, hash);
	fwrite("*", 1,1,hash);
}

//
// rn�� hash file������ ���ڵ� ��ȣ��, sl�� search length�� �ǹ��Ѵ�.
//
void printSearchResult(int rn, int sl)
{
	printf("%d %d\n", rn, sl);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	if(argc<2){
		printf("argc error\n");
		return 0;
	}
	if(!strcmp(argv[1],"-c")){
		if(argc != 3){
			printf("argc != 3\n");
			return 0;
		}
			makeHashfile(atoi(argv[2]));
	}
	else if(!strcmp(argv[1],"-a")){
		printf("add record\n");
		if((fp = fopen(RECORD_FILE_NAME,"r+b"))==NULL){
			fp = fopen(RECORD_FILE_NAME,"w+b");
		}
		writeRecord(fp);
		fclose(fp);
	}
	else if(!strcmp(argv[1],"-s")){
	if((hash = fopen(HASH_FILE_NAME, "rb"))==NULL){
		printf("hash open error!\n");
		return 0;
	}

	int searchlength, rn;
	searchlength = search(argv[2], &rn);
	printSearchResult(rn, searchlength);
	fclose(hash);
	}
	else if(!strcmp(argv[1],"-d")){
	if((hash = fopen(HASH_FILE_NAME, "r+b"))==NULL){
		printf("hash open error!\n");
		return 0;
	}
	delete(argv[2]);


	
	fclose(hash);
	}
	// �л����ڵ������� student.h�� ���ǵǾ� �ִ� STUDENT_FILE_NAME��, 
	// hash file�� HASH_FILE_NAME�� ����Ѵ�.

	// �˻� ����� ������ �� ����� �ݵ�� �־��� printSearchResult() �Լ��� ����Ѵ�.
}

int writeRecord(FILE* fp){
	int cnt = 0;
	STUDENT s;
	char buf[STUDENT_RECORD_SIZE];
	printf("ID10 : ");
	scanf("%s", s.id);
	printf("name20 : ");
		scanf("%s", s.name);
	printf("addr30 : ");
	scanf("%s", s.addr);
	printf("year1 : ");
	scanf("%s", s.year);
	printf("dept19 : ");
		scanf("%s", s.dept);
	printf("phone15 : ");
		scanf("%s", s.phone);
	printf("email25 : ");
	scanf("%s", s.email);
	while(fread(buf,STUDENT_RECORD_SIZE,1,fp) > 0);
	fwrite(s.id,10,1,fp);
	fwrite(s.name,20,1,fp);
	fwrite(s.addr,30,1,fp);
	fwrite(s.year,1,1,fp);
	fwrite(s.dept,19,1,fp);
	fwrite(s.phone, 15, 1, fp);
	fwrite(s.email,25, 1, fp);

return 0;
}
