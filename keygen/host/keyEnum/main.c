#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <keygen_ta.h>
#include <string.h>
#include "enumHelper.h"
#include <stdlib.h>
#include <okey.h>

#define TEE_STORAGE_PRIVATE		0x00000001

int print(const char *format,...)
{
#ifdef DEBUG
	return printf(format);
#else
	return 0;
#endif
}

int main(int argc, char *argv[])
{
	TEEC_Result res;
	uint32_t enumHandle;
	uint32_t storage_id = TEE_STORAGE_PRIVATE;
	size_t infoSize;
	uint32_t idSize;
	int i,j;
	uint8_t info[256];
	uint8_t id[256+1];	//+1 to include null at the end of name
	char *fileName = 0;
	okey o;

	print("initializeContext...\n");
	res = initializeContext(NULL,&o);
	if(res!=TEEC_SUCCESS)
		errx(1,"initializeContext failed with code 0x%x",res);
	print("initializeContext ok\n");

	print("openSession...\n");
	res = openSession(&o,TEEC_LOGIN_PUBLIC,NULL,NULL);
	if(res!=TEEC_SUCCESS)
		errx(1,"openSession failed with code 0x%x origin 0x%x",res,o.error);
	print("openSession ok\n");

	res = fs_alloc_enum(o.session,&enumHandle);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_alloc_enum failed with code 0x%x",res);
	print("enum handle 0x%x\n",enumHandle);

	res = fs_start_enum(o.session,enumHandle,storage_id);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_start_enum failed with code 0x%x",res);

	i=0;
	memset((void*)info,0,sizeof(info));
	memset((void*)id,0,sizeof(id));
	infoSize = sizeof(info);
	idSize = sizeof(id);

	printf("tool to list up(enumerate) keys stored in trustzone\n");
	while(TEEC_SUCCESS==fs_next_enum(o.session,enumHandle,info,&infoSize,id,&idSize)){
		printf("[%d] infoSize:%zd,idSize:%u \n",i++,infoSize,idSize);
		printf(" id:"); 
		fileName = malloc(idSize);
		memcpy(fileName,id,idSize);
		fileName[idSize]=0;
		printf("%s",fileName); 
		printf("\n info:"); 
		for(j=0;j<infoSize;j++) {
			printf("%x",info[j]); 
		}
		printf("\n"); 

		infoSize = sizeof(info);
		idSize = sizeof(id);
		free(fileName);
	}

	res = fs_free_enum(o.session,enumHandle);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_free_enum failed with code 0x%x",res);

	print("finalizeContext...\n");
	finalizeContext(&o);
	print("finalizeContext ok\n");

	print("KeyEnum end\n");
	return 0;
}
