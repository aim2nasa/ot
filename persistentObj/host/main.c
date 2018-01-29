#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;

        printf("TEEC_InitializeContext...\n");
        res = TEEC_InitializeContext(NULL,&ctx);
        if(res!=TEEC_SUCCESS)
                errx(1,"TEEC_InitializeContext failed with code 0x%x",res);
        printf("TEEC_InitializeContext ok\n");

        printf("TEEC_FinalizeContext...\n");
        TEEC_FinalizeContext(&ctx);
        printf("TEEC_FinalizeContext ok\n");

	printf("PersitentObj end\n");
	return 0;
}
