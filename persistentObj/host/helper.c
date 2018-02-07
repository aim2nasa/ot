#include "helper.h"
#include <persistentObj_ta.h>

#define TEEC_OPERATION_INITIALIZER { 0 }

TEEC_Result fs_create(TEEC_Session *sess,void *id,uint32_t id_size,
   		      uint32_t flags, uint32_t attr, void *data,
		      uint32_t data_size, uint32_t *obj,
		      uint32_t storage_id)
{
        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
        TEEC_Result res;
        uint32_t org;

        op.params[0].tmpref.buffer = id;
        op.params[0].tmpref.size = id_size;
        op.params[1].value.a = flags;
        op.params[1].value.b = 0;
        op.params[2].value.a = attr;
        op.params[2].value.b = storage_id;
        op.params[3].tmpref.buffer = data;
        op.params[3].tmpref.size = data_size;

        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                         TEEC_VALUE_INOUT, TEEC_VALUE_INPUT,
                                         TEEC_MEMREF_TEMP_INPUT);

        res = TEEC_InvokeCommand(sess, TA_PERSISTENTOBJ_CMD_CREATE, &op, &org);

        if (res == TEEC_SUCCESS)
                *obj = op.params[1].value.b;

        return res;
}

TEEC_Result fs_unlink(TEEC_Session *sess, uint32_t obj)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t org;

	op.params[0].value.a = obj;

        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
                                         TEEC_NONE, TEEC_NONE);
	return TEEC_InvokeCommand(sess,TA_PERSISTENTOBJ_CMD_UNLINK, &op, &org);
}

TEEC_Result fs_open(TEEC_Session *sess, void *id, uint32_t id_size,
	 	    uint32_t flags, uint32_t *obj, uint32_t storage_id)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	TEEC_Result res;
	uint32_t org;

	op.params[0].tmpref.buffer = id;
	op.params[0].tmpref.size = id_size;
	op.params[1].value.a = flags;
	op.params[1].value.b = 0;
	op.params[2].value.a = storage_id;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_VALUE_INOUT, TEEC_VALUE_INPUT,
					 TEEC_NONE);

	res = TEEC_InvokeCommand(sess, TA_PERSISTENTOBJ_CMD_OPEN, &op, &org);

	if (res == TEEC_SUCCESS)
		*obj = op.params[1].value.b;

	return res;
}

TEEC_Result fs_close(TEEC_Session *sess, uint32_t obj)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t org;

	op.params[0].value.a = obj;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);

	return TEEC_InvokeCommand(sess, TA_PERSISTENTOBJ_CMD_CLOSE, &op, &org);
}
