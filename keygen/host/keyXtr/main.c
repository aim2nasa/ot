#include <err.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <okey.h>

#define TEE_ATTR_SECRET_VALUE               0xC0000000

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
	uint8_t key_filename[256]={ 0 },key_buffer[1024]={ 0 };
	okey o;
	uint32_t keyObj=0;
	size_t i,key_size;
	FILE *fp;
	uint32_t flags;

	if(argc>1){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
	}else{
		printf("tool to extract key in trustzone, can dump it in REE as file\n");
		printf("usage: keyxtr <key filename in TEE> <key dump file in REE (optional)>\n");
		return 1;
	}

	printf("key filename:%s\n",key_filename);
	if(argc>2) printf("key dump file:%s\n",argv[2]);

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

        flags = TEE_DATA_FLAG_ACCESS_WRITE_META |
                TEE_DATA_FLAG_ACCESS_READ       |
                TEE_DATA_FLAG_SHARE_READ        |
                TEE_DATA_FLAG_ACCESS_WRITE      |
                TEE_DATA_FLAG_SHARE_WRITE;
	res = keyOpen(&o,private,(char*)key_filename,flags,&keyObj);
	if(res!=TEEC_SUCCESS)
		errx(1,"keyOpen failed with code 0x%x origin 0x%x flags:0x%x",res,o.error,flags);
	
	printf("file open successful:%s,handle:0x%x flags:0x%x\n",key_filename,keyObj,flags);

	key_size = sizeof(key_buffer);
	res = keyGetObjectBufferAttribute(&o,keyObj,TEE_ATTR_SECRET_VALUE,key_buffer,&key_size);
	if(res!=TEEC_SUCCESS)
		errx(1,"keyGetObjectBufferAttribute failed with code 0x%x",res);

	if(argc>2){
		if((fp=fopen(argv[2],"wb"))!=NULL){
			int nWrite = fwrite(key_buffer,1,key_size,fp);
			assert(nWrite==key_size);
			fclose(fp);
		}else{
			printf("fopen failure:%s\n",argv[2]);
		}
	}
	printf("Obtained keySize=%zd bits(%zd bytes)\n",8*key_size,key_size);
	for(i=0;i<key_size;i++) printf("%x ",key_buffer[i]);
	printf("\n");
	
	res = keyClose(&o,keyObj);
	if(res!=TEEC_SUCCESS)
		errx(1,"keyClose failed with code 0x%x",res);

	printf("file close successful:%s,handle:0x%x\n",key_filename,keyObj);

	print("finalizeContext...\n");
	finalizeContext(&o);
	print("finalizeContext ok\n");

	print("KeyXtr end\n");
	return 0;
}
