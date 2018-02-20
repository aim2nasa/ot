#include <err.h>
#include <stdio.h>
#include <tee_api_defines.h>
#include <ta_storage.h>
#include "helper.h"
#include <string.h>

#define MAX_FILES 3

static uint8_t files[][2]={ { 0x01,0x02 },{ 0x03,0x04 },{ 0x05,0x06 } };
static uint8_t data[] = { 0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x1a,0x1b };

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_STORAGE_UUID;
	uint32_t err_origin,e=0;
	uint32_t obj[MAX_FILES];
	uint32_t storage_id = TEE_STORAGE_PRIVATE;
	size_t infoSize;
	uint32_t idSize;
	int i,j;
	uint8_t info[200];
	uint8_t id[200];

        printf("TEEC_InitializeContext...\n");
        res = TEEC_InitializeContext(NULL,&ctx);
        if(res!=TEEC_SUCCESS)
                errx(1,"TEEC_InitializeContext failed with code 0x%x",res);
        printf("TEEC_InitializeContext ok\n");

        printf("TEEC_OpenSession...\n");
        res = TEEC_OpenSession(&ctx,&sess,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL,&err_origin);
        if(res!=TEEC_SUCCESS)
                errx(1,"TEEC_OpenSession failed with code 0x%x origin 0x%x",res,err_origin);
        printf("TEEC_OpenSession ok\n");

        printf("Creating in TA...\n");
	for(i=0;i<MAX_FILES;i++) {
		res = fs_create(&sess,files[i],sizeof(files[i]),
			TEE_DATA_FLAG_ACCESS_WRITE|
			TEE_DATA_FLAG_ACCESS_WRITE_META,
			0,data,sizeof(data),&obj[i],storage_id);
        	if(res!=TEEC_SUCCESS)
                	errx(1,"fs_create failed with code 0x%x",res);

        	printf("Created in TA, obj[%d]=0x%x, files[%d]=",i,obj[i],i);
		for(j=0;j<sizeof(files[i]);j++) printf("%x",files[i][j]); 
		printf("\n");
	}

	res = fs_alloc_enum(&sess,&e);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_alloc_enum failed with code 0x%x",res);
	printf("enum handle 0x%x\n",e);

	res = fs_start_enum(&sess,e,storage_id);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_start_enum failed with code 0x%x",res);

	i=0;
	memset((void*)info,0,sizeof(info));
	memset((void*)id,0,sizeof(id));
	infoSize = sizeof(info);
	idSize = sizeof(id);
	while(TEEC_SUCCESS==fs_next_enum(&sess,e,info,&infoSize,id,&idSize)){
		printf("enum loop:%d, infoSize:%zd,idSize:%u \n",i++,infoSize,idSize);
		printf(" id:"); 
		for(j=0;j<idSize;j++) {
			printf("%x",id[j]); 
		}
		printf("\n info:"); 
		for(j=0;j<infoSize;j++) {
			printf("%x",info[j]); 
		}
		printf("\n"); 

		infoSize = sizeof(info);
		idSize = sizeof(id);
	}

	res = fs_free_enum(&sess,e);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_free_enum failed with code 0x%x",res);

        printf("Unlinking...\n");
	for(i=0;i<MAX_FILES;i++) {
		res = fs_unlink(&sess,obj[i]);
        	if(res!=TEEC_SUCCESS)
                	errx(1,"fs_unlink[%d] failed with code 0x%x",i,res);
        	printf("Unlink[%d]\n",i);
	}

        printf("TEEC_FinalizeContext...\n");
        TEEC_FinalizeContext(&ctx);
        printf("TEEC_FinalizeContext ok\n");

	printf("PersitentObj end\n");
	return 0;
}
