#include <stdio.h>
#include "student.h"
#include <string.h>
#include <stdlib.h>

FILE* hash;
int writeRecord(FILE*fp);
int recordCount;
//
// 학생 레코드 파일로부터  레코드 번호에 해당하는 레코드를 읽어 레코드 버퍼에 저장한다.
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
// Hash file로부터 rn의 레코드 번호에 해당하는 레코드를 읽어 레코드 버퍼에 저장한다.
//
void readHashRec(FILE *fp, char *recordbuf, int rn)
{
	fseek(fp, 4+(HASH_RECORD_SIZE*rn), SEEK_SET);
	fread(recordbuf, HASH_RECORD_SIZE,1, fp);
}

//
// Hash file에 rn의 레코드 번호에 해당하는 위치에 레코드 버퍼의 레코드를 저장한다.
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
// n의 크기를 갖는 hash file에서 주어진 학번 키값을 hashing하여 주소값(레코드 번호)를 리턴한다.
//
int hashFunction(const char *sid, int n){
	int temp = strlen(sid);
	return ((int)sid[temp-1]*(int)sid[temp-2])%n;

}

//
// n의 크기를 갖는 hash file을 생성한다.
// Hash file은 fixed length record 방식으로 저장되며, 레코드의 크기는 14 바이트이다.
// (student.h 참조)
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
	// Hash file을 생성할 때 이 파일의 맨앞부분에 4바이트 헤더를 둔다. 
	// 여기에는 hash file의 크기 n을 저장한다. 이것은 search()와 (필요하면) delete()를 	// 위한 것이다.
	fclose(hash);
	fclose(fp);
}

//
// 주어진 학번 키값을 hash file에서 검색한다.
// 그 결과는 주어진 학번 키값이 존재하는 hash file에서의 주소(레코드 번호)와 search length이다.
// 검색한 hash file에서의 주소는 rn에 저장하며, 이때 hash file에 주어진 학번 키값이
// 존재하지 않으면 rn에 -1을 저장한다. (search()는 delete()에서도 활용할 수 있음)
// search length는 함수의 리턴값이며, 검색 결과에 상관없이 search length는 항상 계산되어야 한다.
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
// Hash file에서 주어진 학번 키값과 일치하는 레코드를 찾은 후 해당 레코드를 삭제 처리한다.
// 이때 학생 레코드 파일에서 레코드 삭제는 필요하지 않다.
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
// rn은 hash file에서의 레코드 번호를, sl은 search length를 의미한다.
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
	// 학생레코드파일은 student.h에 정의되어 있는 STUDENT_FILE_NAME을, 
	// hash file은 HASH_FILE_NAME을 사용한다.

	// 검색 기능을 수행할 때 출력은 반드시 주어진 printSearchResult() 함수를 사용한다.
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
