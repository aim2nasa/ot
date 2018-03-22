#include "enumHelper.h"
#include <keygen_ta.h>

#define TEEC_OPERATION_INITIALIZER { 0 }

TEEC_Result fs_alloc_enum(TEEC_Session *sess, uint32_t *e)
{
	TEEC_Result res;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t org;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);

	res = TEEC_InvokeCommand(sess, TA_KEY_ALLOC_ENUM_CMD, &op, &org);

	if (res == TEEC_SUCCESS)
		*e = op.params[0].value.a;

	return res;
}

TEEC_Result fs_reset_enum(TEEC_Session *sess, uint32_t e)
{
	TEEC_Result res;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t org;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);

	op.params[0].value.a = e;
	res = TEEC_InvokeCommand(sess, TA_KEY_RESET_ENUM_CMD, &op, &org);

	return res;
}

TEEC_Result fs_free_enum(TEEC_Session *sess, uint32_t e)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t org;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE,
					 TEEC_NONE);

	op.params[0].value.a = e;

	return TEEC_InvokeCommand(sess, TA_KEY_FREE_ENUM_CMD, &op, &org);
}

TEEC_Result fs_start_enum(TEEC_Session *sess, uint32_t e,
			  uint32_t storage_id)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t org;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);

	op.params[0].value.a = e;
	op.params[0].value.b = storage_id;

	return TEEC_InvokeCommand(sess, TA_KEY_START_ENUM_CMD, &op, &org);
}

TEEC_Result fs_next_enum(TEEC_Session *sess, uint32_t e, void *obj_info,
			 size_t *info_size, void *id, uint32_t *id_size)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t org;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
					 TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
	if (obj_info && info_size)
		op.paramTypes |= (TEEC_MEMREF_TEMP_OUTPUT << 4);

	op.params[0].value.a = e;
	op.params[1].tmpref.buffer = obj_info;
	op.params[1].tmpref.size = *info_size;
	op.params[2].tmpref.buffer = id;
	op.params[2].tmpref.size = *id_size;

	TEEC_Result res = TEEC_InvokeCommand(sess, TA_KEY_NEXT_ENUM_CMD, &op, &org);
	*info_size = op.params[1].tmpref.size;
	*id_size = op.params[2].tmpref.size;
	return res;
}
