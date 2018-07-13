#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <okey.h>
#include <util.h>

static long gCount = 0;

int keyGeneration(okey *o,size_t key_size,int perObj)
{
	TEEC_Result res;
	uint32_t flags;

        flags = TEE_DATA_FLAG_ACCESS_WRITE_META |
                TEE_DATA_FLAG_ACCESS_READ       |
                TEE_DATA_FLAG_SHARE_READ        |
                TEE_DATA_FLAG_ACCESS_WRITE      |
                TEE_DATA_FLAG_SHARE_WRITE;

	if(perObj==0)
        	res = keyGen(o,PRIVATE,(char*)"",flags,key_size);
	else{
		uint8_t key_filename[256]={0};
		sprintf((char*)key_filename,"%ld",gCount++);
        	res = keyGen(o,PRIVATE,(char*)key_filename,flags,key_size);
	}
        if(res!=TEEC_SUCCESS)
		return -1;

	return 0;
}

int main(int argc, char *argv[])
{
	TEEC_Result res;
	okey o;
	struct timeval startTime,endTime;
	int i,loop=1,perObj=1;
	size_t keySize;

	if(argc>1){
		keySize = atoi(argv[1]);
		if(keySize!=128 && keySize!=192 && keySize!=256) {
			printf("error, key size must be one of 128,192,256\n");
			return -1;
		}
		printf("keySize=%zd bits\n",keySize);
		if(argc>2) loop = atoi(argv[2]); 
		printf("loop=%d\n",loop);
		if(argc>3) perObj = (atoi(argv[3])==0?0:1);
		printf("perObj=%d\n",perObj);
	}else{
		printf("Key Generation benchmark test\n");
		printf("\nusage: kgen <key size> <loop> <perObj>\n");
		printf("\t key size: size in bit,must be one of 128,192 or 256\n");
		printf("\t loop: number of loop, if not specified, loop is 1\n");
		printf("\t perObj: 0 not write key to storage, default is 1(i.e.write) (optional)\n"); 
		return -1;
	}

	res = initializeContext(NULL,&o);
	if(res!=TEEC_SUCCESS){
		printf("initializeContext failed with code 0x%x\n",res);
		return -1;
	}

	res = openSession(&o,TEEC_LOGIN_PUBLIC,NULL,NULL);
	if(res!=TEEC_SUCCESS){
		printf("openSession failed with code 0x%x origin 0x%x\n",res,o.error);
		finalizeContext(&o);
		return -1;
	}

	gettimeofday(&startTime,NULL);
	print("start: %ld secs, %ld usecs\n",startTime.tv_sec,startTime.tv_usec);

	for(i=0;i<loop;i++) {
		if(keyGeneration(&o,keySize,perObj)!=0) {
               		printf("error found in keyGeneration\n");
			closeSession(&o);
			finalizeContext(&o);
			return -1;
		}
		printf(".");
		fflush(stdout);
	}
	printf("\n");

	gettimeofday(&endTime,NULL);
	print("end: %ld secs, %ld usecs\n",endTime.tv_sec,endTime.tv_usec);

	printf("elapsed time: %ld us\n",microSec(&startTime,&endTime));

	closeSession(&o);
	finalizeContext(&o);

	print("rk end\n");
	return 0;
}
