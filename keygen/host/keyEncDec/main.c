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
	uint8_t key_filename[256]={ 0 },inp_filename[256]={ 0 },out_filename[256]={ 0 };
	uint8_t buffer[TEE_AES_BLOCK_SIZE]={ 0 };
	uint32_t keyObj=0;
	FILE *fp,*out_fp;
	size_t nSize;
	OperationHandle encOp;
	bool bEnc = true;
	struct stat inpFileStat;
	okey o;
	uint32_t flags;

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
		res = cipherUpdate(&o,encOp,buffer,nSize);
		if(res!=TEEC_SUCCESS){
			printf("cipherUpdate failed with code 0x%x origin 0x%x\n",res,o.error);
			goto cleanup4;
		}
		if((nSize=fwrite(outSharedMemory()->buffer,1,outSharedMemory()->size,out_fp))!=outSharedMemory()->size) {
			printf("error, fwrite nSize:%zd != outSharedMemory()->size:%zd\n",nSize,outSharedMemory()->size);
			goto cleanup4;
		}
		printf("[%zd] ",outSharedMemory()->size);
	}
	cipherClose();
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
cleanup3:
	closeSession(&o);
cleanup2:
	finalizeContext(&o);
cleanup1:
	print("KeyEnc end\n");
	return 0;
}
