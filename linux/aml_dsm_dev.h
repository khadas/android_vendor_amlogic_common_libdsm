/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2021 Amlogic, Inc. All rights reserved.
 */

#ifndef _AML_DSM_DEV_H_
#define _AML_DSM_DEV_H_

#include <linux/ioctl.h>
#include <linux/types.h>

union dsm_para;

#define DSM_IOC_MAGIC  'C'

#define DSM_IOC_OPEN_SESSION        _IOWR(DSM_IOC_MAGIC, 1, union dsm_para)
#define DSM_IOC_ADD_KEYSLOT         _IOWR(DSM_IOC_MAGIC, 2, union dsm_para)
#define DSM_IOC_DEL_KEYSLOT         _IOWR(DSM_IOC_MAGIC, 3, union dsm_para)
#define DSM_IOC_SET_TOKEN           _IOWR(DSM_IOC_MAGIC, 4, union dsm_para)
#define DSM_IOC_GET_KEYSLOT_LIST    _IOWR(DSM_IOC_MAGIC, 5, union dsm_para)
#define DSM_IOC_SET_PROPERTY        _IOWR(DSM_IOC_MAGIC, 6, union dsm_para)
#define DSM_IOC_GET_PROPERTY        _IOWR(DSM_IOC_MAGIC, 7, union dsm_para)

#define MAX_KEYSLOTS                32  /*Max Keyslot Per Session*/

enum {
    DSM_DRV_PROP_ALGO,
    DSM_DRV_PROP_INTENT,
    DSM_DRV_PROP_MAX
};

struct keyslot_s {
    __u32 id;
    __u32 type;
};

struct keyslot_list_s {
    __u32 count;
    struct keyslot_s keyslots[MAX_KEYSLOTS];
};

struct property_s {
    __u32 key;
    __u32 value;
};

union dsm_para {
    struct keyslot_s keyslot;
    struct {
        __u32 token;
        __u32 prefix;
        __u32 mask;
    } session;
    struct keyslot_list_s keyslot_list;
    struct property_s property;
};

#endif/*_AML_DSM_DEV_H_*/