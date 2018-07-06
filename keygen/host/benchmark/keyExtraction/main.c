#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <okey.h>
#include <util.h>

#define TEE_ATTR_SECRET_VALUE               0xC0000000

int keyExtraction(okey *o,uint8_t *key_filename)
{
	TEEC_Result res;
	uint32_t keyObj=0;
	uint32_t flags;
	size_t key_size;
	uint8_t key_buffer[1024]={ 0 };

        //Open persistent key object
        flags = TEE_DATA_FLAG_ACCESS_WRITE_META |
                TEE_DATA_FLAG_ACCESS_READ       |
                TEE_DATA_FLAG_SHARE_READ        |
                TEE_DATA_FLAG_ACCESS_WRITE      |
                TEE_DATA_FLAG_SHARE_WRITE;
        res = keyOpen(o,PRIVATE,(char*)key_filename,flags,&keyObj);
        if(res!=TEEC_SUCCESS)
		return -1;

	//retrieve key buffer
	key_size = sizeof(key_buffer);
        res = keyGetObjectBufferAttribute(o,keyObj,TEE_ATTR_SECRET_VALUE,key_buffer,&key_size);
        if(res!=TEEC_SUCCESS)
		return -1;
	
	printf("(%zd)",key_size); 
	fflush(stdout);

        //Close key
        res = keyClose(o,keyObj);
        if(res!=TEEC_SUCCESS)
		return -1;
	return 0;
}

int main(int argc, char *argv[])
{
	TEEC_Result res;
	uint8_t key_filename[256]={ 0 };
	okey o;
	struct timeval startTime,endTime;
	int i,loop=1;

	if(argc>1){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
		if(argc>2) loop = atoi(argv[2]); 
		printf("loop=%d\n",loop);
	}else{
		printf("Key Extraction benchmark test\n");
		printf("\nusage: kxtr <keyfile in Trustzone> <loop>\n");
		printf("\t loop: number of loop, if not specified, loop is 1\n");
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
		if(keyExtraction(&o,key_filename)!=0) {
               		printf("error found in keyExtraction\n");
			closeSession(&o);
			finalizeContext(&o);
			return -1;
		}
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
