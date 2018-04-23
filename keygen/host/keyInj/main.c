#include <err.h>
#include <stdio.h>
#include <string.h>
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
	uint8_t key_filename[256]={ 0 },stored_key_filename[256]={ 0 };
	uint8_t key_buffer[32]={0};
	okey o;
	size_t i,key_size;
	FILE *fp;
	uint32_t flags;

	if(argc>2){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		if(strlen(argv[2])>=sizeof(stored_key_filename))
			errx(1,"stored key filename is over the buffer limit(%zd)\n",sizeof(stored_key_filename));
		memcpy(key_filename,argv[1],strlen(argv[1]));
		memcpy(stored_key_filename,argv[2],strlen(argv[2]));
	}else{
		printf("tool to inject external AES key(in REE) into trustzone(secure storage)\n");
		printf("usage: keyinj <key filename in TEE> <key filename stored in REE>\n");
		return 1;
	}

	print("key filename in TEE:%s\n",key_filename);
	print("key filename stored in REE:%s\n",key_filename);

	if((fp=fopen(argv[2],"rb"))!=NULL){
		key_size = 8*fread(key_buffer,1,sizeof(key_buffer),fp);
		fclose(fp);
	}else{
		printf("fopen failure:%s\n",argv[2]);
	}
	printf("Obtained keySize=%zd\n",key_size);
	for(i=0;i<key_size/8;i++) printf("%x ",key_buffer[i]);
	printf("\n");

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
	res = keyInject(&o,TEE_STORAGE_PRIVATE,(char*)key_filename,key_buffer,key_size,flags);
	if(res!=TEEC_SUCCESS)
		errx(1,"keyInject failed with code 0x%x origin 0x%x flags:0x%x",res,o.error,flags);

	printf("key(%s) is injected into file(%s) in TEE flags:0x%x\n",stored_key_filename,key_filename,flags);

	print("finalizeContext...\n");
	finalizeContext(&o);
	print("finalizeContext ok\n");

	print("KeyInj end\n");
	return 0;
}
