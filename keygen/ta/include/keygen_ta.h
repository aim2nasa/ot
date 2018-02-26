#ifndef TA_KEYGEN_H
#define TA_KEYGEN_H

//ab201cb2-177d-11e8-b642-0ed5f89f718b
#define TA_KEYGEN_UUID \
	{ 0xab201cb2, 0x177d, 0x11e8, \
		{ 0xb6, 0x42, 0x0e, 0xd5, 0xf8, 0x9f, 0x71, 0x8b} }

#define TA_KEY_GEN_CMD		0
#define TA_KEY_OPEN_CMD		1
#define TA_KEY_CLOSE_CMD	2
#define TA_KEY_UNLINK_CMD	3
#define TA_KEY_ALLOC_ENUM_CMD	4
#define TA_KEY_FREE_ENUM_CMD	5
#define TA_KEY_RESET_ENUM_CMD	6
#define TA_KEY_START_ENUM_CMD	7
#define TA_KEY_NEXT_ENUM_CMD	8

#endif
