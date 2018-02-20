#include <tee_client_api.h>
#define DEBUG_FS

TEEC_Result fs_open(TEEC_Session *sess, void *id, uint32_t id_size,
		    uint32_t flags, uint32_t *obj, uint32_t storage_id);

TEEC_Result fs_create(TEEC_Session *sess, void *id, uint32_t id_size,
	  	      uint32_t flags, uint32_t attr, void *data,
		      uint32_t data_size, uint32_t *obj,
		      uint32_t storage_id);

TEEC_Result fs_create_overwrite(TEEC_Session *sess, void *id,
		                uint32_t id_size, uint32_t storage_id);

TEEC_Result fs_close(TEEC_Session *sess, uint32_t obj);

TEEC_Result fs_read(TEEC_Session *sess, uint32_t obj, void *data,
		    uint32_t data_size, uint32_t *count);

TEEC_Result fs_write(TEEC_Session *sess, uint32_t obj, void *data,
		     uint32_t data_size);

TEEC_Result fs_seek(TEEC_Session *sess, uint32_t obj, int32_t offset,
		    int32_t whence);

TEEC_Result fs_unlink(TEEC_Session *sess, uint32_t obj);

TEEC_Result fs_trunc(TEEC_Session *sess, uint32_t obj, uint32_t len);

TEEC_Result fs_rename(TEEC_Session *sess, uint32_t obj, void *id,
		      uint32_t id_size);

TEEC_Result fs_alloc_enum(TEEC_Session *sess, uint32_t *e);

TEEC_Result fs_reset_enum(TEEC_Session *sess, uint32_t e);

TEEC_Result fs_free_enum(TEEC_Session *sess, uint32_t e);

TEEC_Result fs_start_enum(TEEC_Session *sess, uint32_t e,
			  uint32_t storage_id);

TEEC_Result fs_next_enum(TEEC_Session *sess, uint32_t e, void *obj_info,
			 size_t *info_size, void *id, uint32_t *id_size);

TEEC_Result fs_restrict_usage(TEEC_Session *sess, uint32_t obj,
			      uint32_t obj_usage);

TEEC_Result fs_alloc_obj(TEEC_Session *sess, uint32_t obj_type,
			 uint32_t max_key_size, uint32_t *obj);

TEEC_Result fs_free_obj(TEEC_Session *sess, uint32_t obj);

TEEC_Result fs_reset_obj(TEEC_Session *sess, uint32_t obj);

TEEC_Result fs_get_obj_info(TEEC_Session *sess, uint32_t obj,
			    void *obj_info, size_t info_size);
