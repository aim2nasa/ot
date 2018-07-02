#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <okey.h>

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
	uint8_t key_filename[256]={ 0 };
	uint32_t keyObj=0;
	OperationHandle encOp;
	bool bEnc = true;
	okey o;
	uint32_t flags;

	if(argc>1){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
	}else{
		printf("Key Provision benchmark test\n");
		printf("Measure how fast we can repeatedly load key and be ready for symmetric cipher operation\n");
		printf("Above process is defined as key provision in this test\n");
		printf("In a loop, stored key is opened and used for the init of cipher operation\n");
		printf("After then opened key is closed(return its resource back to OS)\n");
		printf("\nusage: keyprov <keyfile in Trustzone>\n");
		goto cleanup1;
	}

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
        flags = TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_SHARE_READ; 
	res = keyOpen(&o,PRIVATE,(char*)key_filename,flags,&keyObj);
	if(res!=TEEC_SUCCESS){
		printf("keyOpen failed with code 0x%x origin 0x%x flags:0x%x\n",res,o.error,flags);
		goto cleanup3;
	}
	printf("key obtained:%s,handle:0x%x flags:0x%x\n",key_filename,keyObj,flags);

	//Allocate operation
	res = keyAllocOper(&o,bEnc,keyObj,&encOp);
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
	
	res = cipherInit(&o,encOp,1);
	if(res!=TEEC_SUCCESS){
		printf("cipherInit failed with code 0x%x origin 0x%x\n",res,o.error);
		goto cleanup3;
	}
	printf("Cipher operation Initialized with %p\n",encOp);

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

cleanup3:
	closeSession(&o);
cleanup2:
	finalizeContext(&o);
cleanup1:
	print("rk end\n");
	return 0;
}
