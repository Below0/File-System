// 주의사항
// 1. blkmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blkmap.h에 정의되어 있지 않을 경우 본인이 만들어서 사용하면 됨
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "blkmap.h"
// 필요한 경우 헤더 파일을 추가하시오.

extern FILE *devicefp;
int FREE = 0;
int amt[DATABLKS_PER_DEVICE];
int FREE_BLK = DATABLKS_PER_DEVICE;
//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// main()함수에서 반드시 먼저 호출이 되어야 한다.
void ftl_open()
{
	int i;
	for(i = 0; i < DATABLKS_PER_DEVICE;i++)
		amt[i] = -1;
	//FREE = BLOCKS_PER_DEVICE;
	//
	// address mapping table 생성 및 초기화 등을 진행
    // mapping table에서 lbn과 pbn의 수는 blkmap.h에 정의되어 있는 DATABLKS_PER_DEVICE
    // 수와 같아야 하겠지요? 나머지 free block 하나는 overwrite 시에 사용하면 됩니다.
	// pbn 초기화의 경우, 첫 번째 write가 발생하기 전을 가정하므로 예를 들면, -1로 설정을
    // 하고, 그 이후 필요할 때마다 block을 하나씩 할당을 해 주면 됩니다. 어떤 순서대로 할당하는지는
    // 각자 알아서 판단하면 되는데, free block들을 어떻게 유지 관리할 지는 스스로 생각해 보기
    // 바랍니다.
	//
	return;
}

//
// file system이 ftl_write()를 호출하면 FTL은 flash memory에서 주어진 lsn과 관련있는
// 최신의 데이터(512B)를 읽어서 sectorbuf가 가리키는 곳에 저장한다.
// 이 함수를 호출하기 전에 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 한다.
// 즉, 이 함수에서 메모리를 할당받으면 안된다.
//

void write_num(int lsn,char buf[PAGE_SIZE]){	
int i = sprintf(&buf[SECTOR_SIZE],"%d", lsn);
buf[SECTOR_SIZE+i+1] = '\0';
}

int read_num(char buf[PAGE_SIZE]){
	char tmp[SPARE_SIZE];
	int res;
	memcpy(tmp, &buf[SECTOR_SIZE], SPARE_SIZE);
	tmp[SPARE_SIZE] = '\0';
	int check = (int)tmp[0];
	res = atoi(tmp);
	if(check < 48 || check > 57 || tmp[0] == NULL) return -1;
	return res;
}

void ftl_copy(int lbn, int offset){
int buf[PAGES_PER_BLOCK] = {0,};
int i,j, spare, copyset;
int copy, bug;
char buff[PAGE_SIZE];
char empty[PAGE_SIZE];
memset(empty, 0xFF, PAGE_SIZE);
for(i = 1; i < PAGES_PER_BLOCK; i++){
	bug = dd_read((amt[lbn]+1)*PAGES_PER_BLOCK-i, buff);
	spare = read_num(buff);
	copyset = spare%NONBUF_PAGES_PER_BLOCK;
	if(bug == -1|| spare == -1|| copyset==offset)
		dd_write(FREE_BLK*PAGES_PER_BLOCK+copyset, empty);
	else if(buf[copyset] == 1) continue;
	else{
		dd_write(FREE_BLK*PAGES_PER_BLOCK+copyset, buff);
		buf[copyset] = 1;
	}

}


}



void ftl_read(int lsn, char *sectorbuf)
{
	int lbn, offset, spare, bug;
	char buf[PAGE_SIZE];
	lbn = lsn/NONBUF_PAGES_PER_BLOCK;
	offset = lsn % NONBUF_PAGES_PER_BLOCK;
	if(amt[lbn] == -1){
		memcpy(sectorbuf, "BLOCK is empty\0", SECTOR_SIZE);
		return;
	}
	int i = 0;
	for(i = 1; i <= PAGES_PER_BLOCK; i++){
		bug = dd_read((amt[lbn]+1)*PAGES_PER_BLOCK-i,buf);
		spare = read_num(buf);
		if(bug == -1) continue;
		else if(spare == lsn){
			memcpy(sectorbuf, buf,SECTOR_SIZE);
			return;
		}
	}
	memcpy(sectorbuf, "PAGE is empty\0", SECTOR_SIZE);
	return;
}

//
// file system이 ftl_write()를 호출하면 FTL은 flash memory에 sectorbuf가 가리키는 512B
// 데이터를 저장한다. 당연히 flash memory의 어떤 주소에 저장할 것인지는 block mapping 기법을
// 따라야한다.
//
void ftl_write(int lsn, char *sectorbuf)
{
char buf[PAGE_SIZE];
char tmp[PAGE_SIZE];
char input[PAGE_SIZE];
char empty[PAGE_SIZE];
memset(empty, 0xff, PAGE_SIZE);
int lbn, offset, temp, i;
int spare, buf_offset = -1;
int bug;
lbn = lsn/NONBUF_PAGES_PER_BLOCK;
offset = lsn%NONBUF_PAGES_PER_BLOCK;
memcpy(input,sectorbuf, SECTOR_SIZE);
write_num(lsn, input);

if(amt[lbn] == -1){ // empty block
	amt[lbn] = FREE++;
}
else{
dd_read(amt[lbn]*PAGES_PER_BLOCK+offset,tmp);
spare = read_num(tmp);
if(spare != -1){
	for(i = 0; i < BUF_PAGES_PER_BLOCK; i++){
	bug = dd_read(amt[lbn]*PAGES_PER_BLOCK+NONBUF_PAGES_PER_BLOCK+i,tmp);
	if(bug == -1 || read_num(tmp) == -1){
		buf_offset = NONBUF_PAGES_PER_BLOCK+i;
		break;
	}
	}
	if(buf_offset != -1){
	dd_write(amt[lbn]*PAGES_PER_BLOCK+buf_offset, input);
		return;
	}
	/* BUF is FULL */

ftl_copy(lbn, offset);
temp = amt[lbn];
amt[lbn] = FREE_BLK;
FREE_BLK = temp;
dd_erase(FREE_BLK);
}
}
dd_write(amt[lbn]*PAGES_PER_BLOCK+offset, input);
return;
}
