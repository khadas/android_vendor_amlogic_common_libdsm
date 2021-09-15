/*
 * libaml_dsm.h
 * Copyright (C) 2019 Amlogic, Inc. All rights reserved.
 *
 *  Created on: 2021年9月14日
 *      Author: tao.guo
 */

#ifndef _LIBAML_DSM_H_
#define _LIBAML_DSM_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define DSM_MAX_KEYSLOTS            32  /*Max Keyslot Per Session*/

enum {
    DSM_PROP_ALGO,
    DSM_PROP_INTENT,
    DSM_PROP_MAX
};

struct dsm_keyslot {
    uint32_t id;
    uint32_t type;
};

struct dsm_keyslot_list {
    uint32_t count;
    struct dsm_keyslot keyslots[DSM_MAX_KEYSLOTS];
};

int     dsm_open(void);
void    dsm_close(int handle);
int     dsm_open_session(int handle, uint32_t *token);
int     dsm_open_session_prefix(int handle, uint32_t *token,
                                uint32_t prefix, uint32_t mask);
int     dsm_add_keyslot(int handle, struct dsm_keyslot *keyslot);
int     dsm_del_keyslot(int handle, uint32_t id);
int     dsm_set_token(int handle, uint32_t token);
int     dsm_get_keyslot_list(int handle, struct dsm_keyslot_list *keyslot_list);
int     dsm_set_property(int handle, uint32_t type, uint32_t value);
int     dsm_get_property(int handle, uint32_t type, uint32_t *value);

#ifdef  __cplusplus
}
#endif

#endif /* _LIBAML_DSM_H_ */
