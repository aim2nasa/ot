#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;

	res = TEEC_InitializeContext(NULL,&ctx);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InitializeContext failed with code 0x%x",res);

	TEEC_FinalizeContext(&ctx);
	printf("Hello end\n");
	return 0;
}
