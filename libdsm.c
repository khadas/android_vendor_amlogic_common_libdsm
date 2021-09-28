
/*
 * libaml_dsm.c
 * Copyright (C) 2019 Amlogic, Inc. All rights reserved.
 *
 *  Created on: 2021年9月14日
 *      Author: tao.guo
 */
#define  LOG_TAG    "aml_dsm"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <log/log.h>

//#include "linux/aml_dsm_dev.h"
#include "aml_dsm.h"
#include "libdsm.h"

#define DSM_DEV "/dev/aml_dsm"


int DSM_OpenSession(uint32_t param)
{
	int fd = -1;

	(void) (param);
	fd = open(DSM_DEV, O_RDWR | O_TRUNC);
	if (fd < 0) {
		goto exit;
	}

exit:
	return fd;
}

void DSM_CloseSession(int handle)
{
	if (handle >= 0) {
		close(handle);
	}
}

int DSM_GenerateToken(int handle, uint32_t *token)
{
	return DSM_GenerateTokenWithPrefix(handle, 0, 0, token);
}

int DSM_GenerateTokenWithPrefix(int handle, uint32_t prefix, uint32_t mask, uint32_t *token)
{
	int r = -1;
	union dsm_para para = { 0 };

	if (!token || handle < 0)
		return -EINVAL;

	para.session.prefix = prefix;
	para.session.mask = mask;
	r = ioctl(handle, DSM_IOC_OPEN_SESSION, (unsigned long)&para);
	if (!r)
		*token = para.session.token;
	return r;
}

int DSM_AddKeySlot(int handle, struct dsm_keyslot *slot)
{
	int r = -1;
	union dsm_para para = { 0 };

	if (!slot || handle < 0)
		return -EINVAL;

	para.keyslot.id = slot->id;
	para.keyslot.parity = slot->parity;
	para.keyslot.algo = slot->algo;
	para.keyslot.is_iv = slot->is_iv;
	para.keyslot.is_enc = slot->is_enc;

	r = ioctl(handle, DSM_IOC_ADD_KEYSLOT, (unsigned long)&para);

	return r;
}

int DSM_RemoveKeySlot(int handle, uint32_t id)
{
	int r = -1;
	union dsm_para para = { 0 };

	if (handle < 0)
		return -EINVAL;

	para.keyslot.id = id;
	r = ioctl(handle, DSM_IOC_DEL_KEYSLOT, (unsigned long)&para);
	return r;
}

int DSM_BindToken(int handle, uint32_t token)
{
	int r = -1;
	union dsm_para para = { 0 };

	if (handle < 0)
		return -EINVAL;

	para.session.token = token;
	r = ioctl(handle, DSM_IOC_SET_TOKEN, (unsigned long)&para);
	return r;
}

int DSM_GetKeySlots(int handle, struct dsm_keyslot_list *slots)
{
	int r = -1;
	int i;
	union dsm_para para = { 0 };

	if (!slots || handle < 0)
		return -EINVAL;

	r = ioctl(handle, DSM_IOC_GET_KEYSLOT_LIST, (unsigned long)&para);
	if (!r) {
		slots->count = para.keyslot_list.count;
		for (i = 0; i < slots->count; i++) {
			slots->keyslots[i].id = para.keyslot_list.keyslots[i].id;
			slots->keyslots[i].parity = para.keyslot_list.keyslots[i].parity;
			slots->keyslots[i].algo = para.keyslot_list.keyslots[i].algo;
			slots->keyslots[i].is_iv = para.keyslot_list.keyslots[i].is_iv;
			slots->keyslots[i].is_enc = para.keyslot_list.keyslots[i].is_enc;
		}
	}

	return r;
}

int DSM_SetProperty(int handle, enum dsm_property_type type, uint32_t value)
{
	int r = -1;
	union dsm_para para = { 0 };

	if (type >= DSM_PROP_UNKNOWN)
		return -EINVAL;

	para.property.key = type;
	para.property.value = value;

	r = ioctl(handle, DSM_IOC_SET_PROPERTY, (unsigned long)&para);

	return r;
}

int DSM_GetProperty(int handle, enum dsm_property_type type, uint32_t *value)
{
	int r = -1;
	union dsm_para para = { 0 };

	if (!value || type >= DSM_PROP_UNKNOWN)
		return -EINVAL;

	para.property.key = type;
	r = ioctl(handle, DSM_IOC_GET_PROPERTY, (unsigned long)&para);
	if (!r)
		*value = para.property.value;
	return r;
}
