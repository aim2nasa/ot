#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <keygen_ta.h>
#include <string.h>
#include "enumHelper.h"
#include <stdlib.h>

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
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_KEYGEN_UUID;
	uint32_t err_origin;
	uint32_t storage_id = TEE_STORAGE_PRIVATE;
	size_t infoSize;
	uint32_t idSize;
	int i,j;
	uint8_t info[256];
	uint8_t id[256+1];	//+1 to include null at the end of name
	char *fileName = 0;

	print("TEEC_InitializeContext...\n");
	res = TEEC_InitializeContext(NULL,&ctx);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InitializeContext failed with code 0x%x",res);
	print("TEEC_InitializeContext ok\n");

	print("TEEC_OpenSession...\n");
	res = TEEC_OpenSession(&ctx,&sess,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_OpenSession failed with code 0x%x origin 0x%x",res,err_origin);
	print("TEEC_OpenSession ok\n");

	res = fs_alloc_enum(&sess,&err_origin);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_alloc_enum failed with code 0x%x",res);
	printf("enum handle 0x%x\n",err_origin);

	res = fs_start_enum(&sess,err_origin,storage_id);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_start_enum failed with code 0x%x",res);

	i=0;
	memset((void*)info,0,sizeof(info));
	memset((void*)id,0,sizeof(id));
	infoSize = sizeof(info);
	idSize = sizeof(id);
	while(TEEC_SUCCESS==fs_next_enum(&sess,err_origin,info,&infoSize,id,&idSize)){
		printf("enum loop:%d, infoSize:%zd,idSize:%u \n",i++,infoSize,idSize);
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

	res = fs_free_enum(&sess,err_origin);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_free_enum failed with code 0x%x",res);

	print("TEEC_FinalizeContext...\n");
	TEEC_FinalizeContext(&ctx);
	print("TEEC_FinalizeContext ok\n");

	print("KeyOpen end\n");
	return 0;
}
