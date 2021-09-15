/*
 * dsm_test.cpp
 * Copyright (C) 2019 Amlogic, Inc. All rights reserved.
 *
 *  Created on: 2021年9月14日
 *      Author: tao.guo
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include "libdsm.h"

#define MAX_SESSIONS 1024

class DSMSession {
public:
    DSMSession(): handle_(-1), token_(0) {
        handle_ = dsm_open();
    }
    ~DSMSession() {
        dsm_close(handle_);
    }

    int openSession(uint32_t prefix = 0, uint32_t mask = 0) {
        return dsm_open_session_prefix(handle_, &token_, prefix, mask);
    }

    int addKeyslot(struct dsm_keyslot *keyslot) {
        return dsm_add_keyslot(handle_, keyslot);
    }

    int delKeyslot(uint32_t id) {
        return dsm_del_keyslot(handle_, id);
    }

    int setToken(uint32_t token) {
        return dsm_set_token(handle_, token);
    }

    int getKeyslotList(struct dsm_keyslot_list *keyslots) {
        return dsm_get_keyslot_list(handle_, keyslots);
    }

    int setProperty(uint32_t type, uint32_t value) {
        return dsm_set_property(handle_, type, value);
    }

    int getProperty(uint32_t type, uint32_t *value) {
        return dsm_get_property(handle_, type, value);
    }

    uint32_t token() { return token_; }
    int handle() { return handle_; }

private:
    int handle_;
    uint32_t token_;
};
#define EXPECT_DESC(result, expect, desc) \
    if ((result) != (expect)) { \
        if (desc != nullptr) printf("%s\n", (char *)desc); \
        printf("TEST Failed @ %d %x\n", __LINE__, (int)(result)); \
        return __LINE__; \
    }

#define EXPECT_NOT_DESC(result, expect, desc) \
    if ((result) == (expect)) { \
        if (desc != nullptr) printf("%s\n", (char *)desc); \
        printf("TEST Failed @ %d %x\n", __LINE__, (int)(result)); \
        return __LINE__; \
    }

#define EXPECT(result, expect) EXPECT_DESC(result, expect, nullptr)
#define EXPECT_NOT(result, expect) EXPECT_NOT_DESC(result, expect, nullptr)

/**
 * @fn int DSM_test0(void)
 * @brief test dsm_open and dsm_open_session_prefix
 *
 * @return
 */
int DSM_test0(void)
{
    int r;
    DSMSession sess1;
    DSMSession *sess[MAX_SESSIONS];
    DSMSession *sess2;
    int i, j;

    /**
     * Test open 1 session
     */
    EXPECT_DESC(sess1.handle() >= 0, true, "Handle should >= 0");
    r = sess1.openSession();
    EXPECT_DESC(r, 0, "Open session should return 0");

    r = sess1.openSession();
    EXPECT_NOT_DESC(r, 0, "Open session twice should fail");

    /**
     * Test prefix and mask
     */
    sess2 = new DSMSession();
    EXPECT_DESC(sess2->handle() >= 0, true, "Handle should >= 0");
    r = sess2->openSession(0xA5A50000, 0xFFFF0000);
    EXPECT_DESC(r, 0, "Open session should return 0");
    delete sess2;

    sess2 = new DSMSession();
    EXPECT_DESC(sess2->handle() >= 0, true, "Handle should >= 0");
    r = sess2->openSession(0xA5A50001, 0xFFFF0000);
    EXPECT_NOT_DESC(r, 0, "Open session should fail if prefix has low 16 bits data");
    delete sess2;

    sess2 = new DSMSession();
    EXPECT_DESC(sess2->handle() >= 0, true, "Handle should >= 0");
    r = sess2->openSession(0xA5A50000, 0xFFFF0001);
    EXPECT_NOT_DESC(r, 0, "Open session should fail if mask has low 16 bits data");
    delete sess2;


    /**
     * Test open MAX_SESSIONS sessions
     */
    i = 0;
    do {
        sess[i] = new DSMSession();
        if (sess[i]->handle() < 0)
            break;
        r = sess[i]->openSession();
        if (r)
            break;
        if (i == MAX_SESSIONS - 1)
            break;
        i++;
    } while (1);

    for (j = 0; j <= i; j++) {
        delete sess[j];
    }

    EXPECT_DESC(i, MAX_SESSIONS - 1, "Open session count is less than MAX_SESSIONS");
    printf("%s Done\n", __FUNCTION__);
    return 0;
}

/**
 * @fn int DSM_test1(void)
 * @brief test dsm_add_keyslot, dsm_set_property, dsm_get_property
 *
 * @return
 */
int DSM_test1(void)
{
    int r;
    DSMSession sess;
    struct dsm_keyslot keyslot;
    uint32_t value;

    int i;

    /**
     * Open device
     */
    EXPECT(sess.handle() >= 0, true);

    keyslot.id = 1;
    r = sess.addKeyslot(&keyslot);
    EXPECT_NOT_DESC(r, 0, "Add keyslot should fail if session not open");

    /**
     * Open session
     */
    r = sess.openSession();
    EXPECT(r, 0);

    keyslot.id = 1;
    r = sess.addKeyslot(&keyslot);
    EXPECT_DESC(r, 0, "Add keyslot should success after open session");

    keyslot.id = 1;
    r = sess.addKeyslot(&keyslot);
    EXPECT_NOT_DESC(r, 0, "Add duplicate keyslot should fail");

    for (i = 2; i <= DSM_MAX_KEYSLOTS * 2; i++) {
        keyslot.id = i;
        r = sess.addKeyslot(&keyslot);
        if (r)
            break;
    }
    EXPECT_DESC(i, DSM_MAX_KEYSLOTS + 1, "The keyslot that id exceed DSM_MAX_KEYSLOTS should fail");

    r = sess.setProperty(DSM_PROP_ALGO, 0x1234);
    EXPECT_DESC(r, 0, "Set prop DSM_PROP_ALGO should success");

    r = sess.setProperty(DSM_PROP_INTENT, 0x5678);
    EXPECT_DESC(r, 0, "Set prop DSM_PROP_INTENT should success");

    r = sess.setProperty(DSM_PROP_MAX, 0);
    EXPECT_NOT_DESC(r, 0, "Set prop >=DSM_PROP_MAX should fail");

    r = sess.getProperty(DSM_PROP_ALGO, &value);
    EXPECT_DESC(r, 0, "Get prop DSM_PROP_ALGO should success");
    EXPECT_DESC(value, 0x1234, "Prop DSM_PROP_ALGO should match the value set")

    r = sess.getProperty(DSM_PROP_INTENT, &value);
    EXPECT_DESC(r, 0, "Get prop DSM_PROP_INTENT should success");
    EXPECT_DESC(value, 0x5678, "Prop DSM_PROP_INTENT should match the value set")

    r = sess.getProperty(DSM_PROP_MAX, 0);
    EXPECT_NOT_DESC(r, 0, "Get prop >=DSM_PROP_MAX should fail");

    printf("%s Done\n", __FUNCTION__);
    return 0;
}

/**
 * @fn int DSM_test2(void)
 * @brief test dsm_del_keyslot
 *
 * @return
 */
int DSM_test2(void)
{
    int r;
    DSMSession sess;
    struct dsm_keyslot keyslot;
    int i;

    /**
     * Open device
     */
    EXPECT(sess.handle() >= 0, true);

    r = sess.delKeyslot(1);
    EXPECT_NOT_DESC(r, 0, "Del keyslot should fail if session not open");

    /**
     * Open session
     */
    r = sess.openSession();
    EXPECT(r, 0);

    r = sess.delKeyslot(1);
    EXPECT_NOT_DESC(r, 0, "Del keyslot should fail if keyslot not exist");

    for (i = 0; i < DSM_MAX_KEYSLOTS; i++) {
        keyslot.id = 1 + i;
        r = sess.addKeyslot(&keyslot);
        EXPECT_DESC(r, 0, "All keyslot should add successful");
    }

    for (i = 0; i < DSM_MAX_KEYSLOTS; i++) {
        r = sess.delKeyslot(1 + i);
        EXPECT_DESC(r, 0, "All keyslot should del successful");
    }

    for (i = 0; i < DSM_MAX_KEYSLOTS; i++) {
        keyslot.id = 1 + i;
        r = sess.addKeyslot(&keyslot);
        EXPECT_DESC(r, 0, "All keyslot should add successful");
    }

    for (i = 0; i < DSM_MAX_KEYSLOTS; i++) {
        r = sess.delKeyslot(1 + i);
        EXPECT_DESC(r, 0, "All keyslot should del successful");
    }

    printf("%s Done\n", __FUNCTION__);
    return 0;
}

/**
 * @fn int DSM_test3(void)
 * @brief test dsm_set_token, dsm_get_session_info, dsm_get_keyslot_list
 *
 * @return
 */
int DSM_test3(void)
{
    int r;
    DSMSession mediacas, tuner;
    struct dsm_keyslot_list list;
    struct dsm_keyslot keyslot;
    uint32_t value;

    EXPECT(mediacas.handle() >= 0, true);
    EXPECT(tuner.handle() >= 0, true);

    r = tuner.setToken(mediacas.token());
    EXPECT_NOT_DESC(r, 0, "Set invalid token should fail");

    r = mediacas.openSession();
    EXPECT(r, 0);

    r = mediacas.setProperty(DSM_PROP_ALGO, 0x1234);
    EXPECT(r, 0);

    r = mediacas.setProperty(DSM_PROP_INTENT, 0x5678);
    EXPECT(r, 0);

    keyslot.id = 0x4567;
    r = mediacas.addKeyslot(&keyslot);
    EXPECT(r, 0);

    r = tuner.setToken(mediacas.token());
    EXPECT_DESC(r, 0, "Set valid token should success");


    r = tuner.getProperty(DSM_PROP_ALGO, &value);
    EXPECT(r, 0);
    EXPECT_DESC(value, 0x1234, "Prop DSM_PROP_ALGO should match the value set")

    r = tuner.getProperty(DSM_PROP_INTENT, &value);
    EXPECT(r, 0);
    EXPECT_DESC(value, 0x5678, "Prop DSM_PROP_INTENT should match the value set")


    r = tuner.getKeyslotList(&list);
    EXPECT_DESC(r, 0, "Get session keyslot list should success");
    EXPECT(list.count, 1);

    r = tuner.delKeyslot(list.keyslots[0].id);
    EXPECT_NOT_DESC(r, 0, "Client session can not del keyslot");

    keyslot.id = 0x5678;
    r = tuner.addKeyslot(&keyslot);
    EXPECT_NOT_DESC(r, 0, "Client session can not add keyslot");

    printf("%s Done\n", __FUNCTION__);
    return 0;
}

/**
 * @fn int DSM_test4(void)
 * @brief test 2 paired sessions dsm_get_keyslot_list and dsm_get_keyslot
 *
 * @return
 */
int DSM_test4(void)
{
    int r;
    DSMSession mediacas[2], tuner[2];
    struct dsm_keyslot_list list_in[2];
    struct dsm_keyslot_list list_out[2];
    uint32_t base_type[2];
    int i, j;
    struct dsm_keyslot keyslot;


    list_in[0].count = 10;
    list_in[1].count = 20;
    base_type[0] = 0x12345678U;
    base_type[1] = 0x90abcdefU;


    /**
     * Open MediaCas and add keyslots
     * Open Tuner and set token
     */
    for (i = 0; i < 2; i++) {
        EXPECT(mediacas[i].handle() >= 0, true);
        EXPECT(tuner[i].handle() >= 0, true);
        r = mediacas[i].openSession();
        EXPECT(r, 0);
        r = tuner[i].setToken(mediacas[i].token());
        EXPECT(r, 0);
        for (j = 0; j < list_in[i].count; j++) {
            keyslot.id = j + 1;
            keyslot.type = base_type[i] + j;
            r = mediacas[i].addKeyslot(&keyslot);
            EXPECT_DESC(r, 0, "All keyslot should add successful");
        }
    }

    /**
     * Tuner try to get keyslot and verify
     */
    for (i = 0; i < 2; i++) {
        r = tuner[i].getKeyslotList(&list_out[i]);
        EXPECT_DESC(r, 0, "Get keyslot list should successful");
        EXPECT_DESC(list_out[i].count, list_in[i].count, "Keyslot count should match add count");
        for (j = 0; j < list_out[i].count; j ++) {
            EXPECT_DESC(list_out[i].keyslots[j].id, j + 1, "Keyslot id should match add id");
            EXPECT_DESC(list_out[i].keyslots[j].type, base_type[i] + j, "Keyslot type should match add type");
        }
    }
    printf("%s Done\n", __FUNCTION__);
    return 0;
}

int main(int argc, char *argv[])
{
    int r;

    (void)&argc;
    (void)&argv;

    r = DSM_test0();
    EXPECT(r, 0);
    r = DSM_test1();
    EXPECT(r, 0);
    r = DSM_test2();
    EXPECT(r, 0);
    r = DSM_test3();
    EXPECT(r, 0);
    r = DSM_test4();
    EXPECT(r, 0);
    return 0;
}
