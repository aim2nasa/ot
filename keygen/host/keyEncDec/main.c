#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <tee_api_defines.h>
#include <tee_api_types.h>
#include <keygen_ta.h>
#include <string.h>
#include <common.h>
#include <sys/stat.h>
#include <okey.h>

#define TEEC_OPERATION_INITIALIZER	{ 0 }
#define TEE_STORAGE_PRIVATE		0x00000001
#define TEE_AES_BLOCK_SIZE             16UL

int print(const char *format,...)
{
#ifdef DEBUG
	return printf(format);
#else
	return 0;
#endif
}

static TEEC_SharedMemory in_shm={
	.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT
};
static TEEC_SharedMemory out_shm={
	.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT
};

static TEEC_Result allocate_shm(TEEC_Context *ctx,TEEC_SharedMemory *shm, size_t sz)
{
	shm->buffer = NULL;
	shm->size = sz;
	return TEEC_AllocateSharedMemory(ctx,shm);
}

static void free_shm(TEEC_SharedMemory *shm)
{
	TEEC_ReleaseSharedMemory(shm);
}

static void copy_shm(TEEC_SharedMemory *shm,void *src,size_t n)
{
	memcpy(shm->buffer,src,n);
}

static void set_shm(TEEC_SharedMemory *shm,size_t n)
{
	memset(shm->buffer,0,n);
}

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint8_t key_filename[256]={ 0 },inp_filename[256]={ 0 },out_filename[256]={ 0 };
	uint8_t buffer[TEE_AES_BLOCK_SIZE]={ 0 };
	uint32_t keyObj=0;
	FILE *fp,*out_fp;
	size_t nSize,keySize=256;
	size_t size=TEE_AES_BLOCK_SIZE; 		//shared memory buffer size
	TEE_OperationHandle encOp;
	bool bEnc = true;
	struct stat inpFileStat;
	okey o;

	if(argc>4){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
		memcpy(inp_filename,argv[2],strlen(argv[2]));
		memcpy(out_filename,argv[3],strlen(argv[3]));
		if(*argv[4]=='e') 
			bEnc=true;
		else if(*argv[4]=='d') 
			bEnc = false;
		else{
			printf("mode must be either e(encoding) or d(decoding)\n");
			goto cleanup1;
		}
	}else{
		printf("AES encoding/decoding tool using key stored in trustzone\n");
		printf("usage: ked <keyfile> <inpfile> <outfile> <mode:e/d>:\n");
		printf("       mode e:encoding, d:decoding\n");

		goto cleanup1;
	}
	printf("mode:");
	if(bEnc) printf("encoding\n"); else printf("decoding\n");

	res = initializeContext(NULL,&o);
	if(res!=TEEC_SUCCESS){
		printf("initializeContext failed with code 0x%x\n",res);
		goto cleanup1;
	}

	res = openSession(&o,TEEC_LOGIN_PUBLIC,NULL,NULL);
	if(res!=TEEC_SUCCESS){
		printf("openSession failed with code 0x%x origin 0x%x\n",res,o.error);
		goto cleanup2;
	}

	//Open persistent key object
	res = keyOpen(&o,TEE_STORAGE_PRIVATE,(char*)key_filename,&keyObj);
	if(res!=TEEC_SUCCESS){
		printf("keyOpen failed with code 0x%x origin 0x%x\n",res,o.error);
		goto cleanup3;
	}
	printf("key obtained:%s,handle:0x%x\n",key_filename,keyObj);

	//Allocate operation
	res = keyAllocOper(&o,bEnc,keySize,&encOp);
	if(res!=TEEC_SUCCESS){
		printf("keyAllocOper failed with code 0x%x origin 0x%x\n",res,o.error);
		goto cleanup3;
	}
	printf("allocateOperation handle:%p\n",encOp);

	//inject key for the allocated operation
	res = keySetkeyOper(&o,encOp,keyObj);
	if(res!=TEEC_SUCCESS){
		printf("keySetkeyOper failed with code 0x%x origin 0x%x\n",res,o.error);
		goto cleanup3;
	}
	printf("setkey(0x%x) for operation(%p)\n",keyObj,encOp);
	
	//Initialize symmetric cipher operation
	if((res=allocate_shm(o.ctx,&in_shm,size))!=TEEC_SUCCESS) {
		printf("allocate_shm faild with code 0x%x\n",res);
		goto cleanup3;
	}
	printf("shared memory buffer:%p,size:%zd\n",in_shm.buffer,in_shm.size);
	if((res=allocate_shm(o.ctx,&out_shm,size))!=TEEC_SUCCESS) {
		printf("allocate_shm faild with code 0x%x\n",res);
		goto cleanup3;
	}
	printf("shared memory buffer:%p,size:%zd\n",out_shm.buffer,out_shm.size);
	op.params[0].value.a = (uintptr_t)encOp;
	op.params[1].tmpref.buffer = 0;
	op.params[1].tmpref.size = 0;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_NONE,TEEC_NONE);
	res = TEEC_InvokeCommand(o.session,TA_CIPHER_INIT_CMD,&op,&o.error);
	if(res!=TEEC_SUCCESS){
		printf("TA_CIPHER_INIT_CMD TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n",res,o.error);
		goto cleanup3;
	}
	printf("Cipher operation Initialized with %p\n",encOp);

	//Read input file
	fp = fopen(argv[2],"r");
	out_fp = fopen(argv[3],"w");
	if(fp==0||out_fp==0) {
		printf("fopen failure ");
		if(fp==0) printf("<inpfile>:%s\n",argv[2]);
		if(out_fp==0) printf("<outfile>:%s\n",argv[2]);
		goto cleanup3;
	}

	if(stat(argv[2],&inpFileStat)<0){
		printf("stat failure\n");
		goto cleanup3;
	}
	printf("inpfile:%s size:%ld\n",argv[2],inpFileStat.st_size);

	while((nSize=fread(buffer,1,sizeof(buffer),fp))>0) { //read as much as TEE_AES_BLOCK_SIZE
		//Cipher update
		set_shm(&in_shm,size);
		set_shm(&out_shm,size);
		copy_shm(&in_shm,buffer,nSize);
		op.params[0].value.a = (uintptr_t)encOp;
		op.params[1].memref.parent = &in_shm;
		op.params[1].memref.size = sizeof(buffer);  //even though nSize less than sizeof(buffer)
		op.params[2].memref.parent = &out_shm;
		op.params[2].memref.size = size;
		op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
						 TEEC_MEMREF_PARTIAL_INPUT,
						 TEEC_MEMREF_PARTIAL_OUTPUT,
						 TEEC_NONE);
		res = TEEC_InvokeCommand(o.session,TA_CIPHER_UPDATE_CMD,&op,&o.error);
		if(res!=TEEC_SUCCESS){
			printf("TA_CIPHER_UPDATE_CMD TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n",res,o.error);
			goto cleanup4;
		}
		if((nSize=fwrite(out_shm.buffer,1,op.params[2].memref.size,out_fp))!=op.params[2].memref.size) {
			printf("error, fwrite nSize:%zd != op.params[2].memref.size:%zd\n",nSize,op.params[2].memref.size);

			goto cleanup4;
		}
		printf("[%zd] ",op.params[2].memref.size);
	}
	printf("\n");

	//Free Allocated operation
	res = keyFreeOper(&o,encOp);
	if(res!=TEEC_SUCCESS){
		printf("keyFreeOper failed with code 0x%x origin 0x%x\n",res,o.error);
		goto cleanup3;
	}
	printf("allocateOperation handle:%p freed\n",encOp);
	
	//Close key
	res = keyClose(&o,keyObj);
	if(res!=TEEC_SUCCESS){
		printf("keyClose failed with code 0x%x origin 0x%x\n",res,o.error);
		goto cleanup3;
	}

cleanup4:
	fclose(out_fp);
	fclose(fp);
	free_shm(&out_shm);
	free_shm(&in_shm);
cleanup3:
	closeSession(&o);
cleanup2:
	finalizeContext(&o);
cleanup1:
	print("KeyEnc end\n");
	return 0;
}
