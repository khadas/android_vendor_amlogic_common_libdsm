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

enum dsm_prop_sc2_dsc_type {
	DSM_PROP_SC2_DSC_TYPE_INVALID = 0,
	DSM_PROP_SC2_DSC_TYPE_TSN,
	DSM_PROP_SC2_DSC_TYPE_TSD,
	DSM_PROP_SC2_DSC_TYPE_TSE
};


enum dsm_prop_slot_stat {
	DSM_PROP_SLOT_NOT_READY = 0,
	DSM_PROP_SLOT_IS_READY
};

enum dsm_property_type {
	DSM_PROP_SC2_DSC_TYPE,
	DSM_PROP_DEC_SLOT_READY,
	DSM_PROP_ENC_SLOT_READY,
	DSM_PROP_UNKNOWN
};

enum dsm_parity {
	DSM_PARITY_NONE,
	DSM_PARITY_EVEN,
	DSM_PARITY_ODD,
	DSM_PARITY_UNKNOWN
};

enum dsm_algo {
	DSM_ALGO_AES_ECB_CLR_END,
	DSM_ALGO_AES_ECB_CLR_FRONT,
	DSM_ALGO_AES_CBC_CLR_END,
	DSM_ALGO_AES_CBC_IDSA,
	DSM_ALGO_CSA2,
	DSM_ALGO_DES_SCTE41,
	DSM_ALGO_DES_SCTE52,
	DSM_ALGO_TDES_ECB_CLR_END,
	DSM_ALGO_CPCM_LSA_MDI_CBC,
	DSM_ALGO_CPCM_LSA_MDD_CBC,
	DSM_ALGO_CSA3,
	DSM_ALGO_ASA,
	DSM_ALGO_ASA_LIGHT,
	DSM_ALGO_S17_ECB_CLR_END,
	DSM_ALGO_S17_ECB_CTS,
	DSM_ALGO_UNKNOWN
};

struct dsm_keyslot {
	uint32_t id;
	enum dsm_parity parity;
	enum dsm_algo algo;
	uint32_t is_iv;
	uint32_t is_enc;
};

struct dsm_keyslot_list {
	uint32_t count;
	struct dsm_keyslot keyslots[DSM_MAX_KEYSLOTS];
};

int     DSM_OpenSession(uint32_t param);
void    DSM_CloseSession(int handle);
int DSM_GenerateToken(int handle, uint32_t *token);
int DSM_GenerateTokenWithPrefix(int handle, uint32_t prefix, uint32_t mask, uint32_t *token);
int DSM_BindToken(int handle, uint32_t token);
int DSM_AddKeySlot(int handle, struct dsm_keyslot *slot);
int DSM_RemoveKeySlot(int handle, uint32_t id);
int DSM_GetKeySlots(int handle, struct dsm_keyslot_list *slots);
int DSM_SetProperty(int handle, enum dsm_property_type type, uint32_t value);
int DSM_GetProperty(int handle, enum dsm_property_type type, uint32_t *value);

#ifdef  __cplusplus
}
#endif

#endif /* _LIBAML_DSM_H_ */
