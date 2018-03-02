#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <tee_api_defines.h>
#include <tee_api_types.h>
#include <keygen_ta.h>
#include <string.h>
#include <common.h>

#define TEEC_OPERATION_INITIALIZER	{ 0 }
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
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint8_t key_filename[256]={ 0 },inp_filename[256]={ 0 },out_filename[256]={ 0 };
	uint8_t buffer[32]={ 0 };
	uint32_t keyObj=0;
	FILE *fp,*out_fp;
	size_t nSize,keySize=256;
	TEE_OperationHandle encOp;

	if(argc>3){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
		memcpy(inp_filename,argv[2],strlen(argv[2]));
		memcpy(out_filename,argv[3],strlen(argv[3]));
	}else{
		printf("usage: kenc <keyfile> <inpfile> <outfile>\n");
		goto cleanup1;
	}

	res = TEEC_InitializeContext(NULL,&ctx);
	if(res!=TEEC_SUCCESS){
		printf("TEEC_InitializeContext failed with code 0x%x\n",res);
		goto cleanup1;
	}

	res = TEEC_OpenSession(&ctx,&sess,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL,&err_origin);
	if(res!=TEEC_SUCCESS){
		printf("TEEC_OpenSession failed with code 0x%x origin 0x%x\n",res,err_origin);
		goto cleanup2;
	}

	//Open persistent key object
	op.params[0].value.a = TEE_STORAGE_PRIVATE;
	op.params[1].tmpref.buffer = key_filename;
	op.params[1].tmpref.size = strlen((const char*)key_filename);
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_VALUE_OUTPUT,TEEC_NONE);
	res = TEEC_InvokeCommand(&sess,TA_KEY_OPEN_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS){
		printf("TA_KEY_OPEN_CMD TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n",res,err_origin);
		goto cleanup3;
	}
	keyObj = op.params[2].value.a;	
	printf("key obtained:%s,handle:0x%x\n",key_filename,keyObj);

	//Allocate operation
	op.params[1].value.a = TEE_ALG_AES_ECB_NOPAD;
	op.params[2].value.a = TEE_MODE_ENCRYPT;
	op.params[3].value.a = keySize;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT,TEEC_VALUE_INPUT,TEEC_VALUE_INPUT,TEEC_VALUE_INPUT);
	res = TEEC_InvokeCommand(&sess,TA_KEY_ALLOC_OPER_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS){
		printf("TA_KEY_ALLOC_OPER_CMD TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n",res,err_origin);
		goto cleanup3;
	}
	encOp = VAL2HANDLE(op.params[0].value.a);
	printf("allocateOperation handle:%p\n",encOp);

	//inject key for the allocated operation
	op.params[0].value.a = (uintptr_t)encOp;
	op.params[1].value.a = keyObj;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE);
	res = TEEC_InvokeCommand(&sess,TA_KEY_SETKEY_OPER_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS){
		printf("TA_KEY_ALLOC_OPER_CMD TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n",res,err_origin);
		goto cleanup3;
	}
	printf("setkey(0x%x) for operation(%p)\n",keyObj,encOp);

	//Read input file
	fp = fopen(argv[2],"r");
	out_fp = fopen(argv[3],"w");
	if(fp==0) errx(1,"fopen failure:%s",argv[2]);
	while((nSize=fread(buffer,1,sizeof(buffer),fp))>0) {
		printf(".");
	}
	printf("\n");
	fclose(out_fp);
	fclose(fp);

	//Free Allocated operation
	op.params[0].value.a = (uintptr_t)encOp;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);
	res = TEEC_InvokeCommand(&sess,TA_KEY_FREE_OPER_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS){
		printf("TA_KEY_FREE_OPER_CMD TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n",res,err_origin);
		goto cleanup3;
	}
	printf("allocateOperation handle:%p freed\n",encOp);
	
	//Close key
	op.params[0].value.a = keyObj;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);
	res = TEEC_InvokeCommand(&sess,TA_KEY_CLOSE_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS){
		printf("TA_KEY_CLOSE_CMD TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n",res,err_origin);
		goto cleanup3;
	}

cleanup3:
	TEEC_CloseSession(&sess);
cleanup2:
	TEEC_FinalizeContext(&ctx);
cleanup1:
	print("KeyEnc end\n");
	return 0;
}
