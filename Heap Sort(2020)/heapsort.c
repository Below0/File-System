#include <stdio.h>
#include "person.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int page_cnt;
int record_cnt;

void swap(char *a, char*b){
	char temp[RECORD_SIZE];

	strcpy(temp, a);
	strcpy(a, b);
	strcpy(b, temp);
}

Person parseRecord(char *recordbuf){
	Person p;
	char *temp;
	char copy[RECORD_SIZE];

	
	strcpy(copy, recordbuf);
	temp = strtok(copy,"#");
	strcpy(p.sn,temp);
	temp = strtok(NULL,"#");
	strcpy(p.name,temp);
	temp = strtok(NULL,"#");
	strcpy(p.age,temp);
	temp = strtok(NULL,"#");
	strcpy(p.addr,temp);
	temp = strtok(NULL,"#");
	strcpy(p.phone,temp);
	temp = strtok(NULL,"#");
	strcpy(p.email,temp);
	return p;
}

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp,pagenum*PAGE_SIZE,SEEK_SET);
	fread(pagebuf, PAGE_SIZE, 1, fp);
}


void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp,pagenum*PAGE_SIZE,SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

void heapify(char **heaparray, int n){//최소 힙 구성
	Person p, t;
	int temp;
	int j;


	for(int i = 1; i <= n; i++){
		p = parseRecord(heaparray[i]);
		temp = i/2;
		j = i;
		while(temp > 0){
			t = parseRecord(heaparray[temp]);
			if(strcmp(p.sn, t.sn)<0){
				swap(heaparray[j], heaparray[temp]);
				j = temp;
			}
			temp /= 2;
		}
	}

}

void printHeap(char **heaparray){
	printf("============\n");
	for(int i = 1; i <= record_cnt; i++){
		printf("%s\n",heaparray[i]);
	}
	printf("============\n");
}

void buildHeap(FILE *inputfp, char **heaparray)
{
	for(int i = record_cnt; i > 0; i--){
		heapify(heaparray, i);
		swap(heaparray[1], heaparray[i]);
	}
}


void makeSortedFile(FILE *outputfp, char **heaparray)
{
	char pagebuf[PAGE_SIZE];
	int buf_idx = 0;
	int page_idx = 1;
	for(int i = 1; i <= record_cnt; i++){
		while(buf_idx + RECORD_SIZE > PAGE_SIZE){
			writePage(outputfp, pagebuf,page_idx++);
			buf_idx = 0;
			memset(pagebuf, 0, sizeof(pagebuf));
		}
		strcpy(&pagebuf[buf_idx], heaparray[i]);
		buf_idx+=RECORD_SIZE;
	}

	if(buf_idx > 0) writePage(outputfp, pagebuf, page_idx);
}

int main(int argc, char *argv[])
{
	int recordCheck = 0;
	int pageIndex = 0;
	int heapIndex = 1;
	char pagebuf[PAGE_SIZE];
	char **heaparray;
	FILE *inputfp;
	FILE *outputfp;


	if(argc < 3){
		printf("argc error!\n");
		return -1;
	}

	if(!(inputfp = fopen(argv[1],"rb"))){
		printf("inputfp error!\n");
		return -1;
	}
	if(!(outputfp = fopen(argv[2],"wb"))){
		printf("outputfp error!\n");
		return -1;
	}

	readPage(inputfp, pagebuf, 0);
	strncpy((char*)&page_cnt, pagebuf, 4);
	strncpy((char*)&record_cnt, &pagebuf[4], 4);
	writePage(outputfp, pagebuf, 0);

	heaparray = malloc(sizeof(char *) * (record_cnt+1));
	for(int i = 0; i <= record_cnt; i++) heaparray[i] = malloc(sizeof(char) * RECORD_SIZE);

	for(int i = 1; i < page_cnt; i++){
		readPage(inputfp,pagebuf, i);
		pageIndex = 0;
	
		while(pageIndex+RECORD_SIZE < PAGE_SIZE && recordCheck < record_cnt){
			strncpy(heaparray[heapIndex++], &pagebuf[pageIndex], RECORD_SIZE);
			pageIndex+=100;
			recordCheck++;
		}
	}

	buildHeap(inputfp, heaparray);
//	printHeap(heaparray);
	makeSortedFile(outputfp, heaparray);

	for(int i = 0; i <= record_cnt; i++) free(heaparray[i]);
	free(heaparray);
	fclose(inputfp);
	fclose(outputfp);
	return 1;
}
