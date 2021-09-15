
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

#include "linux/aml_dsm_dev.h"
#include "libdsm.h"

#define DSM_DEV "/dev/aml_dsm"


int dsm_open(void)
{
    int fd = -1;

    fd = open(DSM_DEV, O_RDWR | O_TRUNC);
    if (fd < 0) {
        goto exit;
    }

exit:
    return fd;
}

void dsm_close(int handle)
{
    if (handle >= 0) {
        close(handle);
    }
}

int dsm_open_session(int handle, uint32_t *token)
{
    return dsm_open_session_prefix(handle, token, 0, 0);
}

int dsm_open_session_prefix(int handle, uint32_t *token, uint32_t prefix, uint32_t mask)
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

int dsm_add_keyslot(int handle, struct dsm_keyslot *keyslot)
{
    int r = -1;
    union dsm_para para = { 0 };

    if (!keyslot || handle < 0)
        return -EINVAL;

    para.keyslot.id = keyslot->id;
    para.keyslot.type = keyslot->type;

    r = ioctl(handle, DSM_IOC_ADD_KEYSLOT, (unsigned long)&para);

    return r;
}

int dsm_del_keyslot(int handle, uint32_t id)
{
    int r = -1;
    union dsm_para para = { 0 };

    if (handle < 0)
        return -EINVAL;

    para.keyslot.id = id;
    r = ioctl(handle, DSM_IOC_DEL_KEYSLOT, (unsigned long)&para);
    return r;
}

int dsm_set_token(int handle, uint32_t token)
{
    int r = -1;
    union dsm_para para = { 0 };

    if (handle < 0)
        return -EINVAL;

    para.session.token = token;
    r = ioctl(handle, DSM_IOC_SET_TOKEN, (unsigned long)&para);
    return r;
}

int dsm_get_keyslot_list(int handle, struct dsm_keyslot_list *keyslot_list)
{
    int r = -1;
    int i;
    union dsm_para para = { 0 };

    if (!keyslot_list || handle < 0)
        return -EINVAL;

    r = ioctl(handle, DSM_IOC_GET_KEYSLOT_LIST, (unsigned long)&para);
    if (!r) {
        keyslot_list->count = para.keyslot_list.count;
        for (i = 0; i < keyslot_list->count; i++) {
            keyslot_list->keyslots[i].id = para.keyslot_list.keyslots[i].id;
            keyslot_list->keyslots[i].type = para.keyslot_list.keyslots[i].type;
        }
    }

    return r;
}

int dsm_set_property(int handle, uint32_t type, uint32_t value)
{
    int r = -1;
    union dsm_para para = { 0 };

    para.property.key = type;
    para.property.value = value;

    r = ioctl(handle, DSM_IOC_SET_PROPERTY, (unsigned long)&para);

    return r;
}

int dsm_get_property(int handle, uint32_t type, uint32_t *value)
{
    int r = -1;
    union dsm_para para = { 0 };

    if (!value)
        return -EINVAL;
    para.property.key = type;
    r = ioctl(handle, DSM_IOC_GET_PROPERTY, (unsigned long)&para);
    if (!r)
        *value = para.property.value;
    return r;
}
