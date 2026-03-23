/*
 * Copyright (c) 2020-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbedtls_pkcs7.h"
#include <ctype.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "app_common.h"
#include "securec.h"
#include "mbedtls/platform.h"
#include <mbedtls/md.h>
#include <mbedtls/rsa.h>
#include <mbedtls/sha256.h>
#include <mbedtls/pk.h>
#include "mbedtls/x509_crt.h"
#include "mbedtls/oid.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <mbedtls/ecdh.h>
#include <mbedtls/cipher.h>
#include <mbedtls/error.h>
#include "log.h"
#include "nv.h"

#define VERIFY_BUF_LEN 512

#ifndef MBEDTLS_OID_PKCS7
#define MBEDTLS_OID_PKCS7 MBEDTLS_OID_PKCS "\x07"
#endif

#ifndef MBEDTLS_OID_PKCS7_DATA
#define MBEDTLS_OID_PKCS7_DATA MBEDTLS_OID_PKCS7 "\x01"
#endif

#ifndef MBEDTLS_OID_PKCS7_SIGNED_DATA
#define MBEDTLS_OID_PKCS7_SIGNED_DATA MBEDTLS_OID_PKCS7 "\x02"
#endif

#ifndef MBEDTLS_OID_PKCS9_MSG_DIGEST
#define MBEDTLS_OID_PKCS9_MSG_DIGEST MBEDTLS_OID_PKCS9 "\x04"
#endif

#define PKCS7_SIGNED_DATA_VERSION 1
#define PEM_FORMAT_SINGED_DATA 1
#define DER_FORMAT_SINGED_DATA 2

#define PKCS7_ERR_RETURN_WITH_LOG(rc) \
    do { \
        if ((rc) != PKCS7_SUCC) \
        { \
            HILOG_ERROR(HILOG_MODULE_AAFWK, "%s:%u, error occurred, ret:%d", __FUNCTION__, __LINE__, (rc)); \
            return rc; \
        } \
    } while (0)

#define OEM_CERT_PATH "/user/app/user/certcfg/"

/* hardcoded public key */
static const char ROOT_CA_OEM_PUBKEY_IN_PEM[] =
    "-----BEGIN PUBLIC KEY-----\r\n"
    "MIIBojANBgkqhkiG9w0BAQEFAAOCAY8AMIIBigKCAYEAvnjlpcsSvEOhMfYwrW/Y\r\n"
    "ExDmXL47SkbomiorGtdF63J2Vkgs2/HTxhUKBkIvSPbGRTR+io8rAo2J3oz1U48m\r\n"
    "+M/+yIszIYC2s+tW64v8NuxLwMFFeg7+cud07/FwfJYYIRvyDQVduwV0VwvkTQy+\r\n"
    "4/n5m+zJEHNheCakEEmFXn7vXUzxlSVSji5cHbGZ2ZzdmettLMc88tv/Kq8X3rgM\r\n"
    "KQkngVylO4p93UvzY9hIAh9heL5VWMVOj241p3RojavIofODIeF7Vti2DjJjvbPW\r\n"
    "igV0M9C0wLlZzMTQwoZbZT8mKsHkXW3XdTgc8ccD7RioeQa9MOlLKFbnJQLYNKr+\r\n"
    "WsGDTFO2iHQmRAIGGuvWnxZIGS5Ho+ZUntdIYAvH8ww+36eFOwNp5cb1/ZjZmQ4c\r\n"
    "p0zTw3zF8mSEieToMgIgz19wc9hvYgMPLlZpzFZklg8tJenJvcNpk5/3saEmpV6R\r\n"
    "gGZ0IrmT/5h3Z/49W86Dv3b2ro4zb/sqtLufDqGoF3IDAgMBAAE=\r\n"
    "-----END PUBLIC KEY-----\r\n";

static mbedtls_x509_crt g_rootCaG2Cert;
static bool g_rootCertLoaded;
static const unsigned char ROOT_CA_G2_CERT_IN_PEM[] =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIICGjCCAaGgAwIBAgIIShhpn519jNAwCgYIKoZIzj0EAwMwUzELMAkGA1UEBhMC\r\n"
    "Q04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UECwwKSHVhd2VpIENCRzEeMBwGA1UE\r\n"
    "AwwVSHVhd2VpIENCRyBSb290IENBIEcyMB4XDTIwMDMxNjAzMDQzOVoXDTQ5MDMx\r\n"
    "NjAzMDQzOVowUzELMAkGA1UEBhMCQ04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UE\r\n"
    "CwwKSHVhd2VpIENCRzEeMBwGA1UEAwwVSHVhd2VpIENCRyBSb290IENBIEcyMHYw\r\n"
    "EAYHKoZIzj0CAQYFK4EEACIDYgAEWidkGnDSOw3/HE2y2GHl+fpWBIa5S+IlnNrs\r\n"
    "GUvwC1I2QWvtqCHWmwFlFK95zKXiM8s9yV3VVXh7ivN8ZJO3SC5N1TCrvB2lpHMB\r\n"
    "wcz4DA0kgHCMm/wDec6kOHx1xvCRo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0T\r\n"
    "AQH/BAUwAwEB/zAdBgNVHQ4EFgQUo45a9Vq8cYwqaiVyfkiS4pLcIAAwCgYIKoZI\r\n"
    "zj0EAwMDZwAwZAIwMypeB7P0IbY7c6gpWcClhRznOJFj8uavrNu2PIoz9KIqr3jn\r\n"
    "BlBHJs0myI7ntYpEAjBbm8eDMZY5zq5iMZUC6H7UzYSix4Uy1YlsLVV738PtKP9h\r\n"
    "FTjgDHctXJlC5L7+ZDY=\r\n"
    "-----END CERTIFICATE-----\r\n";

static mbedtls_x509_crt g_debugModeRootCert;
static bool g_debugModeEnabled;
static const unsigned char DEBUG_MODE_ROOT_CERT_IN_PEM[] =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIICJTCCAaugAwIBAgIIb/9KnVieVTgwCgYIKoZIzj0EAwMwWDELMAkGA1UEBhMC\r\n"
    "Q04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UECwwKSHVhd2VpIENCRzEjMCEGA1UE\r\n"
    "AwwaSHVhd2VpIENCRyBSb290IENBIEcyIFRlc3QwHhcNMjAwMzEyMTI0NDAwWhcN\r\n"
    "NDkwMzEyMTI0NDAwWjBYMQswCQYDVQQGEwJDTjEPMA0GA1UECgwGSHVhd2VpMRMw\r\n"
    "EQYDVQQLDApIdWF3ZWkgQ0JHMSMwIQYDVQQDDBpIdWF3ZWkgQ0JHIFJvb3QgQ0Eg\r\n"
    "RzIgVGVzdDB2MBAGByqGSM49AgEGBSuBBAAiA2IABLS4fgvaYKKfyMZW/4nNTsSv\r\n"
    "xqVxqOEDfLySZK/fSEN0IDQj0sK/qK5hvnf0OxWhwI49P3dKGmQ+cSujXvy0me2D\r\n"
    "JTjY127XYZJrvJwwMkrT/vMrZC5kSOEJbt1qAgSmiaNCMEAwDgYDVR0PAQH/BAQD\r\n"
    "AgEGMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFGldwFjx9Tzm/QpA8R1gc9wc\r\n"
    "eMbFMAoGCCqGSM49BAMDA2gAMGUCMQCCUDRaglmycUGrHmF+L8owKJhbqOUqbwuX\r\n"
    "7XL/vJcp3HeHjiXu7XZmYQ+QAvHPhU0CMCiwWFbDl8ETw4VK25QbwhL/QiUfiRfC\r\n"
    "J6LzteOvjLTEV5iebQMz/nS1j7/oj3Rsqg==\r\n"
    "-----END CERTIFICATE-----\r\n";
static mbedtls_x509_crt g_ohosRootCert;
static const unsigned char OHOS_ROOT_CERT_IN_PEM[] =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIICRDCCAcmgAwIBAgIED+E4izAMBggqhkjOPQQDAwUAMGgxCzAJBgNVBAYTAkNO\r\n"
    "MRQwEgYDVQQKEwtPcGVuSGFybW9ueTEZMBcGA1UECxMQT3Blbkhhcm1vbnkgVGVh\r\n"
    "bTEoMCYGA1UEAxMfT3Blbkhhcm1vbnkgQXBwbGljYXRpb24gUm9vdCBDQTAeFw0y\r\n"
    "MTAyMDIxMjE0MThaFw00OTEyMzExMjE0MThaMGgxCzAJBgNVBAYTAkNOMRQwEgYD\r\n"
    "VQQKEwtPcGVuSGFybW9ueTEZMBcGA1UECxMQT3Blbkhhcm1vbnkgVGVhbTEoMCYG\r\n"
    "A1UEAxMfT3Blbkhhcm1vbnkgQXBwbGljYXRpb24gUm9vdCBDQTB2MBAGByqGSM49\r\n"
    "AgEGBSuBBAAiA2IABE023XmRaw2DnO8NSsb+KG/uY0FtS3u5LQucdr3qWVnRW5ui\r\n"
    "QIL6ttNZBEeLTUeYcJZCpayg9Llf+1SmDA7dY4iP2EcRo4UN3rilovtfFfsmH4ty\r\n"
    "3SApHVFzWUl+NwdH8KNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n"
    "AQYwHQYDVR0OBBYEFBc6EKGrGXzlAE+s0Zgnsphadw7NMAwGCCqGSM49BAMDBQAD\r\n"
    "ZwAwZAIwd1p3JzHN93eoPped1li0j64npgqNzwy4OrkehYAqNXpcpaEcLZ7UxW8E\r\n"
    "I2lZJ3SbAjAkqySHb12sIwdSFKSN9KCMMEo/eUT5dUXlcKR2nZz0MJdxT5F51qcX\r\n"
    "1CumzkcYhgU=\r\n"
    "-----END CERTIFICATE-----\r\n";

static CertVerifyCache g_certCache = {0};
static bool g_cacheValid = false;

static void *Pkcs7Calloc(size_t nmemb, size_t size)
{
#ifdef LITEWEARABLE_SUPPORT_ENABLED
    return APPV_CALLOC(nmemb, size);
#else
    return calloc(nmemb, size);
#endif
}

static void Pkcs7Free(void *ptr)
{
#ifdef LITEWEARABLE_SUPPORT_ENABLED
    APPV_FREE(ptr);
#else
    free(ptr);
#endif
}
#ifdef OH_MARKET_ENABLE
#define DIALMARKET_PUBKEY_PATH "/user/dialpubkey"
static const unsigned char g_dialRootCertInPem[] =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIEsTCCAxmgAwIBAgIUdJ4u/0ct0XJHZphCzmBQQddboC4wDQYJKoZIhvcNAQEL\r\n"
    "BQAwaDELMAkGA1UEBhMCQ04xETAPBgNVBAgMCFNoZW56aGVuMREwDwYDVQQHDAhT\r\n"
    "aGVuemhlbjEPMA0GA1UECgwGSFRMaW5rMQ8wDQYDVQQLDAZIVExpbmsxETAPBgNV\r\n"
    "BAMMCEhUTGlua0NBMB4XDTI1MDQyNzA2NDkwN1oXDTM1MDQyNTA2NDkwN1owaDEL\r\n"
    "MAkGA1UEBhMCQ04xETAPBgNVBAgMCFNoZW56aGVuMREwDwYDVQQHDAhTaGVuemhl\r\n"
    "bjEPMA0GA1UECgwGSFRMaW5rMQ8wDQYDVQQLDAZIVExpbmsxETAPBgNVBAMMCEhU\r\n"
    "TGlua0NBMIIBojANBgkqhkiG9w0BAQEFAAOCAY8AMIIBigKCAYEAytceDkYIIn2P\r\n"
    "340TG6+3yI4NhauO7IoXDI74w3CXpqFG40F2h42IKnDLsDznGKMyaRglsab09K1m\r\n"
    "/yrVomrH2/e1EpMaATiKIJFkEijSjo0/EGqYeT1q59tyXdkgl/VLV04Yh8ra5lU8\r\n"
    "nVog/Lk02GLhoTVttMQOA2dn3muaLzjuBHEjfAisfubP+mxE1e7/P6E2ZD5D8J2i\r\n"
    "XoMSDTO6FwZwNBtTnrTu11z1h5bRQ6jPU3fAWxcfThjR/InocTtS4OlpKNuPlnOt\r\n"
    "PpS+j0DVD+UJKR3wPEXSpxwBVveSJB7NsCMIuAM9urTmhzJzcSu9pKKzb6r22xBx\r\n"
    "HaNQ6g+rX6vCCHnT/vUlqscyrKxnC3DwEqzK2PByvAGaMDfB6co/nyncYmQ0eIz+\r\n"
    "8Px+xoLcFQ6qfyHBlj81FHvkM5R/TprruGUUvWkTySQJnLuRY7aEqmFWQAKXYppK\r\n"
    "G6ZmgnDYiV2vwpsdR6TbVomdmpIV7r0Vp8hs1vVRaZHOJoJdNrynAgMBAAGjUzBR\r\n"
    "MB0GA1UdDgQWBBR6KC9wRu9J6A1Fa86FoLNVgSvYMzAfBgNVHSMEGDAWgBR6KC9w\r\n"
    "Ru9J6A1Fa86FoLNVgSvYMzAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUA\r\n"
    "A4IBgQDB/QGEdrz/bGpmHBca32oWYmeI4OSGZSjoBdzg54tI0TzncrQTduzSYTLM\r\n"
    "8bl3HExDlZXR/wqMlfqftNctlHKRUpX3K0A78M3j2xdBu5AoeDI4Xis7AkWTeo4U\r\n"
    "6EQYtZU1rY6RgX95GsMPyMtbEwB+xiUM8w2gf8rhTLt6iotDU6ErUh6qLS7zqQOJ\r\n"
    "0ulmF9g9/bLcyfaVeXdxVL1+Ggw2jIpxkqO4XjeuUNj/1FrMmiZdKQAdny5aNEf4\r\n"
    "MZ9IWd1WNgSOtbg/Csf7m55yOCJjUr3QteE1pRJoMAw0mw9GXEZkndRX27Sl6wvW\r\n"
    "xeD9niKvD5dG7vg/dzSJNUBqL7dOyNezyWXJIlxXMG24MuWMe7GbBBbxdatEXs1E\r\n"
    "fT/P6O9ffghZbd7q5Cm3pC/xfMtpPl1sV4OeDzIzs2j5Fvc7LXTePT60bbFLe5WB\r\n"
    "yPLVWeOJrMxWjrppi8O7cVBo/uhWopV14vbawJ970flrgRH6/dm50P/VHQ7ktAZe\r\n"
    "ZVFOoMU=\r\n"
    "-----END CERTIFICATE-----\r\n";

#define PUB_KEY_SIZE 65
#define SHARED_SECRET_SIZE 32
#define UT_NV_MAX_LENGTH 4060
#define CHUNK_SIZE (1024 * 4) // Each HASH update processes 4KB
#define SHA256_HASH_SIZE 32
#define RSA3072_SIG_SIZE 384
#define PUB_KEY_FILE_FULL_NAME_LEN 256
#define ECC_PEN_KEY_LEN 256
#define TARGET_PEM ".pem"

typedef struct {
    mbedtls_ecp_group grp; // Elliptic curve group (including curve parameters)
    mbedtls_mpi d; // ECC private key
    mbedtls_ecp_point Q; // ECC public key
    unsigned char pubKey[PUB_KEY_SIZE];
    size_t pubKeyLen;
    unsigned char workKey[SHARED_SECRET_SIZE];
    unsigned char noise[NOISE_SIZE];
} EcdhEntity;

typedef struct {
    mbedtls_cipher_context_t *ctx;
    const unsigned char *key;
    const unsigned char *iv;
    const unsigned char *input;
    size_t ilen;
    unsigned char *output;
    size_t *olen;
    int32_t paddingMode;
} AesParams;

const uint16_t NV_KEY_ID = NV_ID_SECURE_DIAL;

int32_t PKCS7_SaveKeyToNv(unsigned char *pem_buffer, uint32_t pem_bufferLen, uint16_t keyId)
{
    int32_t ret = 0;
    nv_key_attr_t attr = {0, 1, 0, 0};
    ret = uapi_nv_write_with_attr(keyId, pem_buffer, pem_bufferLen, &attr, NULL);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to write key to nv.");
        return PKCS7_SAVE_PUB_KEY_FAIL;
    }
    return ret;
}

int32_t PKCS7_GetKeyFromNv(uint16_t keyId, uint8_t *data)
{
    int32_t ret = 0;
    uint16_t k_length = 0;
    nv_key_attr_t attr = {0, 1, 0, 0};
    ret = uapi_nv_read_with_attr(keyId, UT_NV_MAX_LENGTH, &k_length, data, &attr);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to read key from nv.");
        return PKCS7_GET_PUB_KEY_FAIL;
    }
    return ret;
}

int32_t InitDrbg(mbedtls_ctr_drbg_context *drbg, mbedtls_entropy_context *entropy)
{
    const char *pers = "ECDH_Negotiation";
    mbedtls_entropy_init(entropy);
    mbedtls_ctr_drbg_init(drbg);
    if (mbedtls_ctr_drbg_seed(drbg, mbedtls_entropy_func, entropy, (const uint8_t *)pers, strlen(pers)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_ctr_drbg_seed failed");
        return PKCS7_INVALID_PARAM;
    }
    return 0;
}

int32_t InitEcdhEntity(EcdhEntity *entity, mbedtls_ctr_drbg_context *drbg)
{
    int32_t ret = 0;

    // Initialize all cryptographic objects
    mbedtls_ecp_group_init(&entity->grp);
    mbedtls_mpi_init(&entity->d);
    mbedtls_ecp_point_init(&entity->Q);
    entity->pubKeyLen = 0;

    // 1. Loading curve MBEDTLS-EPC_SP_BP256R1
    ret = mbedtls_ecp_group_load(&entity->grp, MBEDTLS_ECP_DP_BP256R1);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_ecp_group_load failed: -0x%04X\n", -ret);
        return ret;
    }

    // 2. Generate key pairs
    ret = mbedtls_ecdh_gen_public(
        &entity->grp, &entity->d, &entity->Q,
        mbedtls_ctr_drbg_random, drbg);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_ecdh_gen_public failed: -0x%04X\n", -ret);
        return ret;
    }
    return 0;
}

// Triple HKDF derived function (using synchronized salt values)
int32_t TripleHkdfDerive(const unsigned char *sharedSecret, size_t secretLen,
    const unsigned char sals[SALT_INFO_POOL_ROWS * NOISE_SIZE], unsigned char *finalKey, size_t keyLen)
{
    int32_t ret;
    const mbedtls_md_info_t *md = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    unsigned char intermediate_key[AES_KEY_LEN];

    // First round of HKDF: salt1
    if ((ret = mbedtls_hkdf(md, sals, NOISE_SIZE,  sharedSecret, secretLen,
        NULL, 0, intermediate_key, AES_KEY_LEN)) != 0) {
        return ret;
    }

    // Second round HKDF: salt2， Attach protocol identifier to info2
    const char *info2 = "HKDF-Round2";
    // IKM is the output of the previous round, Covering the intermediate key
    if ((ret = mbedtls_hkdf(md, sals + NOISE_SIZE, NOISE_SIZE, intermediate_key, AES_KEY_LEN,
        (const unsigned char *)info2, strlen(info2), intermediate_key, AES_KEY_LEN)) != 0) {
        return ret;
    }

    // Third round HKDF: salt3， Fixed protocol identification
    const char *info3 = "AES128-CBC-Key";
    if ((ret = mbedtls_hkdf(md, sals + NOISE_SIZE + NOISE_SIZE, NOISE_SIZE, intermediate_key, AES_KEY_LEN,
        (const unsigned char *)info3, strlen(info3), finalKey, keyLen)) != 0) {
        return ret;
    }
    memset_s(intermediate_key, AES_KEY_LEN, 0, AES_KEY_LEN);
    return 0;
}

int32_t ComputeSharedSecret(EcdhEntity *local, EcdhEntity *peer, mbedtls_ctr_drbg_context *drbg,
    unsigned char pubKeyFileName[PUBLIC_KEY_FILE_LEN], mbedtls_entropy_context *entropy)
{
    int32_t ret;
    mbedtls_mpi z;
    mbedtls_mpi_init(&z);
    unsigned char sharedSecret[SHARED_SECRET_SIZE] = {0};
    // 1. Analyze the public key of the opposite end
    mbedtls_ecp_point peer_Q;
    mbedtls_ecp_point_init(&peer_Q);
    ret = mbedtls_ecp_point_read_binary(&local->grp, &peer_Q, peer->pubKey, peer->pubKeyLen);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_ecp_point_read_binary failed: -0x%04X\n", -ret);
        goto cleanup;
    }
    // 2. Calculate shared key
    ret = mbedtls_ecdh_compute_shared(&local->grp, &z, &peer_Q, &local->d, mbedtls_ctr_drbg_random, drbg);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_ecdh_compute_shared failed: -0x%04X\n", -ret);
        goto cleanup;
    }
    unsigned char saltInfoPool[SALT_INFO_POOL_ROWS * NOISE_SIZE] = {0};
    mbedtls_ctr_drbg_random(drbg, saltInfoPool, NOISE_SIZE * SALT_INFO_POOL_ROWS);
    /*
     * Optimizing the performance of converting each byte into two hexadecimal characters
     *  using the lookup table method.
    */
    const char hexTable[] = "0123456789ABCDEF";
    for (int i = 0; i < SALT_INFO_POOL_ROWS * NOISE_SIZE; i++) {
        pubKeyFileName[i * 2] = hexTable[saltInfoPool[i] >> 4]; // Move 4 positions to the right
        pubKeyFileName[i * 2 + 1] = hexTable[saltInfoPool[i] & 0x0F];
    }
    ret = memcpy_s(pubKeyFileName + SALT_INFO_POOL_ROWS * NOISE_SIZE * 2, strlen(TARGET_PEM), TARGET_PEM,
        strlen(TARGET_PEM));
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "memcpy_s failed: -0x%04X\n", -ret);
        goto cleanup;
    }
    pubKeyFileName[PUBLIC_KEY_FILE_LEN - 1] = '\0';
    // 3. Convert to byte stream
    ret = mbedtls_mpi_write_binary(&z, sharedSecret, SHARED_SECRET_SIZE);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_mpi_write_binary failed: -0x%04X\n", -ret);
        goto cleanup;
    }
    // HKDF derived working key (parameters must be consistent with the counterpart)
    if ((ret = TripleHkdfDerive(sharedSecret, sizeof(sharedSecret), saltInfoPool,
        local->workKey, sizeof(local->workKey))) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Three failed HKDF attempts: -0x%04x\n", -ret);
        goto cleanup;
    }
cleanup:
    mbedtls_mpi_free(&z);
    mbedtls_ecp_point_free(&peer_Q);
    memset_s(sharedSecret, sizeof(sharedSecret), 0, sizeof(sharedSecret));
    return ret;
}

// Extract private key as byte array
int32_t GetPrivateKey(EcdhEntity *entity, unsigned char *eccPrivKey, size_t *eccPrivKeyLen)
{
    if (mbedtls_mpi_size(&entity->d) > *eccPrivKeyLen) {
        return MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL;
    }

    *eccPrivKeyLen = mbedtls_mpi_size(&entity->d);
    return mbedtls_mpi_write_binary(&entity->d, eccPrivKey, *eccPrivKeyLen);
}

int32_t CreateCubdirAndFile(const char *subdir, const char *filename, const void *data, size_t size)
{
    // 1. Create the client's public key directory and file for the first boot, and grant the file 0755 permission
    if (mkdir(subdir, 0755) != 0) {
        if (errno != EEXIST) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to create subdirectories!");
            return PKCS7_FILE_OPERATION_FAIL;
        }
    }

    // 2. Build a complete file path
    char fullpath[PUB_KEY_FILE_FULL_NAME_LEN];
    int32_t ret = snprintf_s(fullpath, sizeof(fullpath), sizeof(fullpath) - 1, "%s/%s", subdir, filename);
    if (ret < 0 || ret >= sizeof(fullpath)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Path truncated!");
        return PKCS7_INVALID_PARAM;
    }
    // 3. Create and write files
    FILE *fp = fopen(fullpath, "wb");
    if (!fp) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to create file");
        return PKCS7_FILE_OPERATION_FAIL;
    }

    if (data && size > 0) {
        if (fwrite(data, 1, size, fp) != size) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Fail to write to file!");
            fclose(fp);
            return PKCS7_FILE_OPERATION_FAIL;
        }
    }

    fclose(fp);
    return 0;
}

// Check if there is a. pem file in the directory
bool CheckPemExists(const char *dirPath)
{
    DIR *dir = opendir(dirPath);
    if (!dir) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Fail to open dir:%s!", dirPath);
        return false;
    }

    struct dirent *entry;
    bool hasPem = false;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // Check file extension
        const char *ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, TARGET_PEM) == 0) {
            hasPem = true;
            break;
        }
    }

    closedir(dir);
    return hasPem;
}

static int32_t GetClientECCPubKey(EcdhEntity *entity, unsigned char *clientPubKey)
{
    // Convert the public key to PEM format and write it to a file
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);

    // Bind ECC key context
    int32_t ret = mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_pk_setup failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    // Copy curve parameters and public key to pk context
    mbedtls_ecp_keypair *ec_key = mbedtls_pk_ec(pk);
    ret = mbedtls_ecp_group_copy(&ec_key->MBEDTLS_PRIVATE(grp), &entity->grp);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_ecp_group_copy failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    ret = mbedtls_ecp_copy(&ec_key->MBEDTLS_PRIVATE(Q), &entity->Q);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_ecp_copy failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    // Generate PEM format public key
    ret = mbedtls_pk_write_pubkey_pem(&pk, clientPubKey, ECC_PEN_KEY_LEN);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "mbedtls_pk_write_pubkey_pem failed: -0x%04X\n", -ret);
        goto cleanup;
    }
cleanup:
    mbedtls_pk_free(&pk);
    return ret;
}

int32_t PKCS7_GenerateECCKeypair(void)
{
    unsigned char clientPubKey[ECC_PEN_KEY_LEN];
    EcdhEntity client = {0};
    EcdhEntity peer = {0};
    if (CheckPemExists(DIALMARKET_PUBKEY_PATH)) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "The public key already exists!");
        return 0;
    }
    unsigned char pubKeyFileName[PUBLIC_KEY_FILE_LEN] = {0};
    // ECDH negotiation
    mbedtls_ctr_drbg_context drbg;
    mbedtls_entropy_context entropy;
    int32_t ret = InitDrbg(&drbg, &entropy);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to InitDrbg!\n");
        return ret;
    }
    // 1. Generate client key pairs
    ret = InitEcdhEntity(&client, &drbg);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to init ecdh entity!\n");
        return ret;
    }
    // Server public key
    const uint8_t server_pub_key[PUB_KEY_SIZE] = {
        0x04, 0x67, 0x6E, 0xFF, 0x58, 0x0E, 0xB2, 0x64, 0x79, 0x40, 0x29, 0x9C, 0xBB, 0x1A, 0xE6,
        0x22, 0xA2, 0x62, 0x8F, 0x06, 0xDA, 0x74, 0x0C, 0xAC, 0x88, 0x1D, 0x8D, 0x40, 0xBE, 0xB8,
        0xC9, 0xE3, 0x79, 0x9F, 0x32, 0x66, 0x5D, 0xEC, 0xF1, 0xBA, 0xCF, 0x03, 0x10, 0x1B, 0x80,
        0xD0, 0x8B, 0xF8, 0xF0, 0x56, 0x01, 0x74, 0x30, 0xC3, 0x20, 0x18, 0x5B, 0x83, 0x5F, 0xA4,
        0xA0, 0x90, 0x6E, 0x53, 0x89
    };
    ret = memcpy_s(peer.pubKey, PUB_KEY_SIZE, server_pub_key, PUB_KEY_SIZE);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to init ecdh entity!\n");
        return ret;
    }
    peer.pubKeyLen = PUB_KEY_SIZE;
    ret = ComputeSharedSecret(&client, &peer, &drbg, pubKeyFileName, &entropy);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to compute shared secret!\n");
        goto cleanup;
    }
    ret = PKCS7_SaveKeyToNv(client.workKey, AES_KEY_LEN, NV_KEY_ID);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to PKCS7_SaveKeyToNv!\n");
        goto cleanup;
    }
    ret = GetClientECCPubKey(&client, clientPubKey);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to GetClientECCPubKey!\n");
        goto cleanup;
    }
    if (CreateCubdirAndFile(DIALMARKET_PUBKEY_PATH, (const char*)pubKeyFileName,
        clientPubKey, strlen((const char*)clientPubKey)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Public key write failed!");
        ret = MBEDTLS_ERR_PK_FILE_IO_ERROR;
        goto cleanup;
    }

cleanup:
    mbedtls_ecp_group_free(&client.grp);
    mbedtls_mpi_free(&client.d);
    mbedtls_ecp_point_free(&client.Q);
    memset_s(&client, sizeof(client), 0, sizeof(client));
    mbedtls_ctr_drbg_free(&drbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}

static int32_t verifyCertChain(FILE *fp, const MetadataStruct *meta, mbedtls_x509_crt *certChain)
{
    int32_t ret = 0;
    uint32_t flags = 0;
    unsigned char *appCert = NULL;
    mbedtls_x509_crt rootCert;
    mbedtls_x509_crt_init(&rootCert);
    if ((ret = mbedtls_x509_crt_parse(&rootCert, g_dialRootCertInPem, sizeof(g_dialRootCertInPem))) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Root certificate parsing failed: -0x%04X!", -ret);
        goto cleanup;
    }

    // Read and parse application certificates
    appCert = Pkcs7Calloc(meta->appCertLen + 1, sizeof(char));
    if (appCert == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc failed!");
        ret = PKCS7_MEMORY_EXHAUST;
        goto cleanup;
    }
    fseek(fp, meta->sigOffset + meta->sigLen, SEEK_SET);
    if (fread(appCert, 1, meta->appCertLen, fp) != meta->appCertLen) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Read app cert failed!");
        ret = PKCS7_FILE_OPERATION_FAIL;
        goto cleanup;
    }
    appCert[meta->appCertLen] = '\0';
    if ((ret = mbedtls_x509_crt_parse(certChain, appCert, meta->appCertLen + 1)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Application certificate parsing failed: -0x%04X!", -ret);
        goto cleanup;
    }
    // Read and parse intermediate certificates
    unsigned char *interCert = Pkcs7Calloc(meta->intermediateCertLen + 1, sizeof(char));
    if (interCert == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "malloc for intermediate cert failed!");
        ret = PKCS7_MEMORY_EXHAUST;
        goto cleanup;
    }
    fseek(fp, meta->sigOffset + meta->sigLen + meta->appCertLen, SEEK_SET);
    if (fread(interCert, 1, meta->intermediateCertLen, fp) != meta->intermediateCertLen) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Intermediate certificate read failed!");
        ret = PKCS7_FILE_OPERATION_FAIL;
        goto cleanup;
    }
    interCert[meta->intermediateCertLen] = '\0';
    if ((ret = mbedtls_x509_crt_parse(certChain, interCert, meta->intermediateCertLen + 1)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Intermediate certificate parsing failed: -0x%04X!", -ret);
        goto cleanup;
    }
    // Verify certificate chain
    if ((ret = mbedtls_x509_crt_verify(certChain, &rootCert, NULL, NULL, &flags, NULL, NULL)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Certificate chain verification failed: -0x%04X!", -ret);
    }

cleanup:
    if (appCert) {
        Pkcs7Free(appCert);
        appCert = NULL;
    }
    if (interCert) {
        Pkcs7Free(interCert);
        interCert = NULL;
    }
    mbedtls_x509_crt_free(&rootCert);
    return ret;
}

int32_t AesCrypt(const AesParams *params)
{
    if (params == NULL || params->ctx == NULL || params->key == NULL || params->iv == NULL || params->input == NULL ||
        params->output == NULL || params->olen == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Invalid AES parameters");
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    int32_t ret = 0;
    if ((ret = mbedtls_cipher_reset(params->ctx)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Cipher reset failed: -0x%04X", -ret);
        return ret;
    }
    // 128 is the length of key for AES128
    if ((ret = mbedtls_cipher_setkey(params->ctx, params->key, 128, MBEDTLS_DECRYPT)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Set key failed: -0x%04X", -ret);
        return ret;
    }

    if ((ret = mbedtls_cipher_set_padding_mode(params->ctx, params->paddingMode)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Set padding failed: -0x%04X", -ret);
        return ret;
    }

    if ((ret = mbedtls_cipher_crypt(params->ctx, params->iv, AES_KEY_LEN, params->input, params->ilen,
        params->output, params->olen)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Cryption failed: -0x%04X", -ret);
    }
    return ret;
}

static int32_t VerifySignature(FILE *fp, const MetadataStruct *meta, mbedtls_x509_crt *certChain, unsigned char *hash)
{
    int32_t ret = 0;
    unsigned char *sigBuf = Pkcs7Calloc(meta->sigLen, sizeof(char));
    if (sigBuf == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "memory allocation failed!");
        return PKCS7_MEMORY_EXHAUST;
    }

    // Read signature data
    fseek(fp, meta->sigOffset, SEEK_SET);
    if (fread(sigBuf, 1, meta->sigLen, fp) != meta->sigLen) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Signature reading failed!");
        ret = PKCS7_FILE_OPERATION_FAIL;
        goto exit;
    }
    // verify signature
    mbedtls_pk_rsassa_pss_options pss_options = {
        .mgf1_hash_id = MBEDTLS_MD_SHA256,
        .expected_salt_len = SHA256_HASH_SIZE // Must be consistent with the SaltLength on the Java side
    };
    // Pass PSS parameters when calling the verification interface
    ret = mbedtls_pk_verify_ext(MBEDTLS_PK_RSASSA_PSS, &pss_options, &certChain->pk, MBEDTLS_MD_SHA256,
        hash, SHA256_HASH_SIZE, sigBuf, meta->sigLen);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Verification of dial signature failed!, ret:%d", ret);
    }
exit:
    if (sigBuf != NULL) {
        Pkcs7Free(sigBuf);
        sigBuf = NULL;
    }
    return ret;
}

static int32_t VerifySignatureStream(FILE *fp, const MetadataStruct *meta, mbedtls_x509_crt *certChain,
    const unsigned char *restoredData, size_t restoredLen)
{
    int32_t ret = 0;
    unsigned char hash[SHA256_HASH_SIZE] = {0};
    mbedtls_md_context_t mdCtx;
    size_t unencryptedLen = 0;
    size_t processed = 0;
    unsigned char *buffer = Pkcs7Calloc(CHUNK_SIZE, sizeof(char));
    if (buffer == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "memory allocation failed!");
        return PKCS7_MEMORY_EXHAUST;
    }
    size_t baseSize = FRONT_FIXED_SIZE + meta->dialViewCount;
    unencryptedLen = meta->originalLen - baseSize;
    mbedtls_md_init(&mdCtx);
    mbedtls_md_setup(&mdCtx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    mbedtls_md_starts(&mdCtx);
    size_t processedRestored = 0;
    fseek(fp, meta->encryptedLen - meta->padLen, SEEK_SET);
    // 1. Process decrypted encrypted data blocks (including padding)
    while (processedRestored < restoredLen) {
        // Logic for Filling Buffer Zones
        size_t remainingRestored = restoredLen - processedRestored;
        size_t copySize = (remainingRestored > CHUNK_SIZE) ? CHUNK_SIZE : remainingRestored;
        // Copy decrypted data to the buffer
        ret = memcpy_s(buffer, copySize, restoredData + processedRestored, copySize);
        if (ret != 0) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Verification of dial signature failed!, ret:%d", ret);
            ret = PKCS7_MEMORY_EXHAUST;
            goto exit;
        }
        // If the buffer is not filled, try supplementing data from the file
        if (copySize < CHUNK_SIZE) {
            size_t need_read = CHUNK_SIZE - copySize;
            size_t actually_read = fread(buffer + copySize, 1, need_read, fp);
            copySize += actually_read;
        }
    
        // Update hash calculation
        mbedtls_md_update(&mdCtx, buffer, copySize);
        processedRestored += copySize;
    
        // If it exceeds the original data range, jump out
        if (processedRestored > restoredLen) {
            break;
        }
    }

    // 2. Process remaining unencrypted data
    unencryptedLen -= processedRestored - restoredLen;
    if (unencryptedLen > 0) {
        while (processed < unencryptedLen) {
            size_t remaining = unencryptedLen - processed;
            size_t chunk = (remaining > CHUNK_SIZE) ? CHUNK_SIZE : remaining;

            // Read unencrypted data blocks directly from files
            if (fread(buffer, 1, chunk, fp) != chunk) {
                HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to read unencrypted data!");
                ret = PKCS7_FILE_OPERATION_FAIL;
                goto exit;
            }
            mbedtls_md_update(&mdCtx, buffer, chunk);
            processed += chunk;
        }
        // Verify if it has been fully read
        if (processed != unencryptedLen) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Unencrypted data length mismatch!", processed, unencryptedLen);
            ret = PKCS7_INVALID_PARAM;
            goto exit;
        }
    }
    // Complete hash calculation
    mbedtls_md_finish(&mdCtx, hash);

    ret = VerifySignature(fp, meta, certChain, hash);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "VerifySignature failed!");
    }
exit:
    mbedtls_md_free(&mdCtx);
    if (buffer != NULL) {
        Pkcs7Free(buffer);
        buffer = NULL;
    }
    return ret;
}

static int32_t DecryptAesKey(mbedtls_cipher_context_t *ctx, const MetadataStruct *meta, const uint8_t *eccWorkKey,
    unsigned char *aesKey)
{
    size_t olen = AES_KEY_LEN;
    AesParams params = {
        .ctx = ctx,
        .key = eccWorkKey,
        .iv = meta->iv2,
        .input = meta->password,
        .ilen = sizeof(meta->password),
        .output = aesKey,
        .olen = &olen,
        .paddingMode = MBEDTLS_PADDING_NONE
    };
    int32_t ret = AesCrypt(&params);
    if (ret == 0 && olen != AES_KEY_LEN) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Invalid AES key length: %zu", olen);
        return PKCS7_INVALID_PARAM;
    }
    return ret;
}

static int32_t ReadAndPrepareData(FILE *fp, const MetadataStruct *meta, unsigned char **encryptedData, size_t *dataLen)
{
    *encryptedData = Pkcs7Calloc(meta->encryptedLen, 1);
    if (*encryptedData == NULL) {
        return PKCS7_MEMORY_EXHAUST;
    }

    fseek(fp, 0, SEEK_SET);
    size_t readLen = fread(*encryptedData, 1, meta->encryptedLen - meta->padLen, fp);
    if (readLen != meta->encryptedLen - meta->padLen) {
        Pkcs7Free(*encryptedData);
        return PKCS7_FILE_OPERATION_FAIL;
    }

    if (memcpy_s(*encryptedData + readLen, meta->padLen, meta->padData, meta->padLen) != 0) {
        Pkcs7Free(*encryptedData);
        return PKCS7_MEMORY_EXHAUST;
    }

    *dataLen = meta->encryptedLen;
    return 0;
}

int32_t PKCS7_VerifyAndDecrypt(FILE *fp, bool firstLoad, MetadataStruct *meta, unsigned char *restoredBuf,
    uint32_t originalLen)
{
    int32_t ret = 0;
    unsigned char aesKey[AES_KEY_LEN] = {0};
    uint8_t eccWorkKey[AES_KEY_LEN] = {0};
    mbedtls_x509_crt *certChain = NULL;
    mbedtls_cipher_context_t ctx;
    unsigned char *encryptedData = NULL;
    size_t dataLen = 0;

    // Parameter validation
    if (fp == NULL || meta == NULL || restoredBuf == NULL || originalLen < meta->encryptedLen) {
        return PKCS7_INVALID_PARAM;
    }

    mbedtls_cipher_init(&ctx);
    const mbedtls_cipher_info_t *info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_CBC);
    
    // Setup cipher context
    if ((ret = mbedtls_cipher_setup(&ctx, info)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Cipher setup failed: -0x%04X", -ret);
        goto cleanup;
    }

    // ECC key handling
    if (meta->pubKeyType == ENC_TYPE_ECC) {
        if ((ret = PKCS7_GetKeyFromNv(NV_KEY_ID, eccWorkKey)) != 0) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Get ECC key failed: %d", ret);
            goto cleanup;
        }
        if ((ret = DecryptAesKey(&ctx, meta, eccWorkKey, aesKey)) != 0) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Decrypt AES key failed: %d", ret);
            goto cleanup;
        }
    }

    // Read and decrypt data
    if ((ret = ReadAndPrepareData(fp, meta, &encryptedData, &dataLen)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Read data failed: %d", ret);
        goto cleanup;
    }

    size_t olen = meta->encryptedLen;
    AesParams decryptParams = {
        .ctx = &ctx,
        .key = aesKey,
        .iv = meta->iv1,
        .input = encryptedData,
        .ilen = dataLen,
        .output = restoredBuf,
        .olen = &olen,
        .paddingMode = MBEDTLS_PADDING_PKCS7
    };
    if ((ret = AesCrypt(&decryptParams)) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Decrypt failed: %d", ret);
        goto cleanup;
    }

    // Signature verification for first load
    if (firstLoad) {
        if ((certChain = Pkcs7Calloc(1, sizeof(mbedtls_x509_crt))) == NULL) {
            ret = PKCS7_MEMORY_EXHAUST;
            goto cleanup;
        }
        mbedtls_x509_crt_init(certChain);

        if ((ret = verifyCertChain(fp, meta, certChain)) != 0) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Cert chain failed: %d", ret);
            goto cleanup;
        }

        if ((ret = VerifySignatureStream(fp, meta, certChain, restoredBuf, olen)) != 0) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Signature failed: %d", ret);
            goto cleanup;
        }
    }

cleanup:
    if (certChain != NULL) {
        mbedtls_x509_crt_free(certChain);
        free(certChain);
    }
    if (encryptedData != NULL) {
        Pkcs7Free(encryptedData);
    }
    mbedtls_cipher_free(&ctx);
    memset_s(aesKey, AES_KEY_LEN, 0, AES_KEY_LEN);
    memset_s(eccWorkKey, AES_KEY_LEN, 0, AES_KEY_LEN);
    return ret;
}
#endif

static void FreeCertVerifyCache(CertVerifyCache *cache)
{
    if (cache == NULL) return;
    
    if (cache->hashListData != NULL) {
        Pkcs7Free(cache->hashListData);
        cache->hashListData = NULL;
    }
    cache->hashListSize = 0;
    cache->verifyTime = 0;
    g_cacheValid = false;
}

/**
 * 计算文件的SHA256哈希值（与openssl dgst -sha256兼容）
 * @param filePath 文件路径
 * @param hash 输出二进制哈希值（32字节）
 * @param hexHash 输出十六进制哈希字符串（65字节，包含终止符）
 * @return 成功返回PKCS7_SUCC，失败返回错误码
 */
static int32_t CalculateFileSha256(const char *filePath, unsigned char *hash, char *hexHash)
{
    if (filePath == NULL || hash == NULL || hexHash == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Invalid parameters to CalculateFileSha256");
        return PKCS7_INVALID_PARAM;
    }

    FILE *file = NULL;
    mbedtls_sha256_context sha256Ctx;
    unsigned char *buffer = NULL;
    size_t bufferSize = 4096; // 4KB缓冲区
    size_t bytesRead;
    int32_t rc = PKCS7_SUCC;
    const char *hexChars = "0123456789abcdef";

    // 分配堆内存缓冲区
    buffer = Pkcs7Calloc(bufferSize, sizeof(unsigned char));
    if (buffer == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to allocate buffer for file reading");
        return PKCS7_MEMORY_EXHAUST;
    }

    file = fopen(filePath, "rb");
    if (!file) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to open file: %s", filePath);
        rc = PKCS7_FILE_OPERATION_FAIL;
        goto cleanup;
    }

    mbedtls_sha256_init(&sha256Ctx);
    
    // 初始化SHA256上下文（0表示SHA-256，1表示SHA-224）
    if (mbedtls_sha256_starts(&sha256Ctx, 0) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to initialize SHA256 context");
        rc = PKCS7_INVALID_VALUE;
        goto cleanup;
    }

    // 读取文件并更新哈希
    while ((bytesRead = fread(buffer, 1, bufferSize, file)) > 0) {
        if (mbedtls_sha256_update(&sha256Ctx, buffer, bytesRead) != 0) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to update SHA256 hash");
            rc = PKCS7_INVALID_VALUE;
            goto cleanup;
        }
    }

    // 检查文件读取错误
    if (ferror(file)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Error reading file: %s", filePath);
        rc = PKCS7_FILE_OPERATION_FAIL;
        goto cleanup;
    }

    // 完成哈希计算
    if (mbedtls_sha256_finish(&sha256Ctx, hash) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to finalize SHA256 hash");
        rc = PKCS7_INVALID_VALUE;
        goto cleanup;
    }

    // 转换为十六进制字符串（与openssl输出格式一致）
    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
        hexHash[i * 2] = hexChars[(hash[i] >> 4) & 0x0F];
        hexHash[i * 2 + 1] = hexChars[hash[i] & 0x0F];
    }
    hexHash[64] = '\0';

cleanup:
    if (file) fclose(file);
    if (buffer) Pkcs7Free(buffer);
    mbedtls_sha256_free(&sha256Ctx);
    return rc;
}

// 十六进制哈希转换为二进制（使用安全函数）
static int32_t HexHashToBinary(const char *hexHash, unsigned char *binaryHash)
{
    if (hexHash == NULL || binaryHash == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    for (size_t i = 0; i < SHA256_HASH_SIZE; i++) {
        unsigned int byte = 0;
        if (sscanf_s(hexHash + i * 2, "%2x", &byte) != 1) {
            return PKCS7_INVALID_VALUE;
        }
        binaryHash[i] = (unsigned char)byte;
    }
    return PKCS7_SUCC;
}

// 解析单行哈希条目
static int32_t ParseHashLine(const char *line, size_t lineLen, CertHashEntry *entry)
{
    if (line == NULL || entry == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    const char *colon = memchr(line, ':', lineLen);
    if (colon == NULL || colon <= line || colon >= line + lineLen - 1) {
        return PKCS7_INVALID_VALUE;
    }

    size_t nameLen = colon - line;
    size_t hashHexLen = line + lineLen - colon - 1;

    // 跳过可能的回车符
    if (hashHexLen > 0 && *(colon + hashHexLen) == '\r') {
        hashHexLen--;
    }

    if (hashHexLen != 64) {
        return PKCS7_INVALID_VALUE;
    }

    // 分配证书名内存
    entry->certName = Pkcs7Calloc(nameLen + 1, 1);
    if (entry->certName == NULL) {
        return PKCS7_MEMORY_EXHAUST;
    }

    // 复制证书名
    if (memcpy_s(entry->certName, nameLen + 1, line, nameLen) != EOK) {
        Pkcs7Free(entry->certName);
        entry->certName = NULL;
        return PKCS7_INVALID_VALUE;
    }
    entry->certName[nameLen] = '\0';

    // 复制哈希十六进制字符串
    if (memcpy_s(entry->hashHex, sizeof(entry->hashHex), colon + 1, hashHexLen) != EOK) {
        Pkcs7Free(entry->certName);
        entry->certName = NULL;
        return PKCS7_INVALID_VALUE;
    }
    entry->hashHex[hashHexLen] = '\0';
    entry->hashHexLen = hashHexLen;

    // 转换为二进制哈希
    return HexHashToBinary(entry->hashHex, entry->hash);
}

// 十六进制哈希转换为二进制
/**
 * 解析哈希列表文本格式（证书文件名:哈希值）
 * 格式示例：cert.pem:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
 */
int32_t ParseHashListTextFormat(const unsigned char *data, size_t dataLen, CertHashList *hashList)
{
    if (data == NULL || dataLen == 0 || hashList == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    const char *start = (const char *)data;
    const char *end = start + dataLen;
    const char *ptr = start;
    size_t lineCount = 0;

    // 计算行数
    while (ptr < end) {
        const char *lineEnd = memchr(ptr, '\n', end - ptr);
        if (lineEnd == NULL) {
            if (ptr < end) lineCount++;
            break;
        }
        if (lineEnd > ptr) lineCount++;
        ptr = lineEnd + 1;
    }

    if (lineCount == 0) {
        return PKCS7_INVALID_VALUE;
    }

    // 分配内存
    hashList->entries = Pkcs7Calloc(lineCount, sizeof(CertHashEntry));
    if (hashList->entries == NULL) {
        return PKCS7_MEMORY_EXHAUST;
    }
    
    hashList->count = 0;
    hashList->capacity = lineCount;

    // 解析每行
    ptr = start;
    size_t index = 0;
    
    while (ptr < end && index < lineCount) {
        const char *lineEnd = memchr(ptr, '\n', end - ptr);
        if (lineEnd == NULL) lineEnd = end;
        
        size_t lineLen = lineEnd - ptr;
        if (lineLen > 0) {
            int32_t rc = ParseHashLine(ptr, lineLen, &hashList->entries[index]);
            if (rc == PKCS7_SUCC) {
                hashList->count++;
                index++;
            }
        }
        ptr = lineEnd + 1;
    }

    return (hashList->count > 0) ? PKCS7_SUCC : PKCS7_INVALID_VALUE;
}

static void FreeCertHashList(CertHashList *hashList)
{
    if (hashList == NULL) return;
    
    if (hashList->entries != NULL) {
        for (size_t i = 0; i < hashList->count; i++) {
            if (hashList->entries[i].certName != NULL) {
                Pkcs7Free(hashList->entries[i].certName);
                hashList->entries[i].certName = NULL;
            }
        }
        Pkcs7Free(hashList->entries);
        hashList->entries = NULL;
    }
    hashList->count = 0;
    hashList->capacity = 0;
}

// 解析hash列表文件内容
static int32_t ParseHashListContent(const unsigned char *data, size_t dataLen, 
                                   char ***certNames, unsigned char **certHashes, 
                                   size_t *certCount)
{
    if (data == NULL || dataLen == 0 || certNames == NULL || 
        certHashes == NULL || certCount == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "ParseHashListContent: dataLen=%zu", dataLen);
    // 简单的行解析逻辑，实际应根据具体格式调整
    size_t count = 0;
    const char *start = (const char *)data;
    const char *end = start + dataLen;
    
    // 第一遍：计算行数
    const char *ptr = start;
    while (ptr < end) {
        const char *lineEnd = memchr(ptr, '\n', end - ptr);
        if (lineEnd == NULL) break;
        if (lineEnd > ptr) count++;
        ptr = lineEnd + 1;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "Found %zu lines in hash list", count);
    if (count == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "No valid lines found in hash list");
        return PKCS7_INVALID_VALUE;
    }
    
    *certNames = Pkcs7Calloc(count, sizeof(char *));
    *certHashes = Pkcs7Calloc(count, SHA256_HASH_SIZE);
    if (*certNames == NULL || *certHashes == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Memory allocation failed for certNames or certHashes");
        if (*certNames) Pkcs7Free(*certNames);
        if (*certHashes) Pkcs7Free(*certHashes);
        return PKCS7_MEMORY_EXHAUST;
    }
    
    // 第二遍：解析每行
    ptr = start;
    size_t index = 0;
    while (ptr < end && index < count) {
        const char *lineEnd = memchr(ptr, '\n', end - ptr);
        if (lineEnd == NULL) lineEnd = end;
        
        const char *colon = memchr(ptr, ':', lineEnd - ptr);
        if (colon != NULL && colon > ptr && (lineEnd - colon - 1) >= SHA256_HASH_SIZE) {
            // 解析证书文件名
            size_t nameLen = colon - ptr;
            (*certNames)[index] = Pkcs7Calloc(nameLen + 1, 1);
            if ((*certNames)[index] == NULL) {
                // 清理已分配的内存
                for (size_t i = 0; i < index; i++) {
                    Pkcs7Free((*certNames)[i]);
                }
                Pkcs7Free(*certNames);
                Pkcs7Free(*certHashes);
                return PKCS7_MEMORY_EXHAUST;
            }
            int ret = memcpy_s((*certNames)[index], nameLen + 1, ptr, nameLen);
            if (ret != 0) {
                HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to copy certificate name, error: %d", ret);
                // 清理已分配的内存
                Pkcs7Free((*certNames)[index]);
                for (size_t i = 0; i < index; i++) {
                    Pkcs7Free((*certNames)[i]);
                }
                Pkcs7Free(*certNames);
                Pkcs7Free(*certHashes);
                return PKCS7_FILE_OPERATION_FAIL;
            }
            (*certNames)[index][nameLen] = '\0';
            
            // 解析哈希值
            ret = memcpy_s(&(*certHashes)[index * SHA256_HASH_SIZE], SHA256_HASH_SIZE, colon + 1, SHA256_HASH_SIZE);
            if (ret != 0) {
                HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to copy certificate hash, error: %d", ret);
                // 清理已分配的内存
                Pkcs7Free((*certNames)[index]);
                for (size_t i = 0; i < index; i++) {
                    Pkcs7Free((*certNames)[i]);
                }
                Pkcs7Free(*certNames);
                Pkcs7Free(*certHashes);
                return PKCS7_FILE_OPERATION_FAIL;
            }
            index++;
        }
        
        ptr = lineEnd + 1;
    }
    
    *certCount = index;
    return PKCS7_SUCC;
}

// 读取哈希列表文件
static int32_t ReadHashListFile(const char *hashListFile, unsigned char **fileData, long *fileSize)
{
    FILE *file = NULL;
    int32_t rc = PKCS7_SUCC;

    if (hashListFile == NULL || fileData == NULL || fileSize == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    file = fopen(hashListFile, "rb");
    if (!file) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to open hash list file: %s", hashListFile);
        return PKCS7_FILE_OPERATION_FAIL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return PKCS7_FILE_OPERATION_FAIL;
    }
    
    *fileSize = ftell(file);
    if (fseek(file, 0, SEEK_SET) != 0 || *fileSize <= 0) {
        fclose(file);
        return PKCS7_FILE_OPERATION_FAIL;
    }
    
    *fileData = Pkcs7Calloc(*fileSize + 1, sizeof(char));
    if (*fileData == NULL) {
        fclose(file);
        return PKCS7_MEMORY_EXHAUST;
    }
    
    size_t bytesRead = fread(*fileData, 1, *fileSize, file);
    fclose(file);
    
    if (bytesRead != (size_t)*fileSize) {
        Pkcs7Free(*fileData);
        *fileData = NULL;
        return PKCS7_FILE_OPERATION_FAIL;
    }
    
    (*fileData)[*fileSize] = '\0';
    return PKCS7_SUCC;
}

// 读取签名文件
static int32_t ReadSignatureFile(const char *signatureFile, unsigned char **signature)
{
    FILE *sigFile = NULL;
    
    if (signatureFile == NULL || signature == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    sigFile = fopen(signatureFile, "rb");
    if (!sigFile) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to open signature file: %s", signatureFile);
        return PKCS7_FILE_OPERATION_FAIL;
    }

    *signature = Pkcs7Calloc(RSA3072_SIG_SIZE, sizeof(char));
    if (*signature == NULL) {
        fclose(sigFile);
        return PKCS7_MEMORY_EXHAUST;
    }

    size_t sigLen = fread(*signature, 1, RSA3072_SIG_SIZE, sigFile);
    fclose(sigFile);
    
    if (sigLen != RSA3072_SIG_SIZE) {
        Pkcs7Free(*signature);
        *signature = NULL;
        return PKCS7_FILE_OPERATION_FAIL;
    }
    
    return PKCS7_SUCC;
}

// 验证哈希列表签名
static int32_t VerifyHashListSignature(const unsigned char *fileData, long fileSize, 
                                      const unsigned char *signature)
{
    mbedtls_pk_context pk;
    unsigned char hash[SHA256_HASH_SIZE];
    int32_t rc;

    if (fileData == NULL || fileSize <= 0 || signature == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    mbedtls_pk_init(&pk);
    
    rc = mbedtls_pk_parse_public_key(&pk, (const unsigned char *)ROOT_CA_OEM_PUBKEY_IN_PEM,
        strlen(ROOT_CA_OEM_PUBKEY_IN_PEM) + 1);
    if (rc != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Public key parsing failed: -0x%04X", -rc);
        mbedtls_pk_free(&pk);
        return PKCS7_INVALID_PARAM;
    }

    // 设置RSA填充模式
    if (mbedtls_pk_get_type(&pk) == MBEDTLS_PK_RSA) {
        mbedtls_rsa_context *rsa = mbedtls_pk_rsa(pk);
        mbedtls_rsa_set_padding(rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
    }

    rc = mbedtls_sha256(fileData, fileSize, hash, 0);
    if (rc != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "SHA256 calculation failed: -0x%04X", -rc);
        mbedtls_pk_free(&pk);
        return rc;
    }

    rc = mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, hash, SHA256_HASH_SIZE, 
                          signature, RSA3072_SIG_SIZE);
    if (rc != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Signature verification failed: -0x%04X", -rc);
    }
    
    mbedtls_pk_free(&pk);
    return (rc == 0) ? PKCS7_SUCC : rc;
}

static int32_t VerifyAndCacheHashList(CertVerifyCache *cache)
{
    int32_t rc = 0;
    const char *hashListFile = OEM_CERT_PATH"root_cert_hash_list.bin";
    const char *signatureFile = OEM_CERT_PATH"hash_list_sign.bin";
    unsigned char *fileData = NULL;
    unsigned char *signature = NULL;
    unsigned char *cachedData = NULL;
    long fileSize = 0;

    if (cache == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Invalid cache parameter");
        return PKCS7_INVALID_PARAM;
    }

    // 检查缓存是否仍然有效
    if (g_cacheValid && cache->hashListData != NULL) {
        struct stat st;
        if (stat(hashListFile, &st) == 0 && st.st_mtime <= cache->verifyTime) {
            HILOG_INFO(HILOG_MODULE_AAFWK, "Using cached hash list verification");
            return PKCS7_SUCC;
        }
    }
    
    // 清除旧缓存
    FreeCertVerifyCache(cache);

    // 读取哈希列表文件
    rc = ReadHashListFile(hashListFile, &fileData, &fileSize);
    if (rc != PKCS7_SUCC) {
        goto cleanup;
    }

    // 读取签名文件
    rc = ReadSignatureFile(signatureFile, &signature);
    if (rc != PKCS7_SUCC) {
        goto cleanup;
    }

    // 验证签名
    rc = VerifyHashListSignature(fileData, fileSize, signature);
    if (rc != PKCS7_SUCC) {
        goto cleanup;
    }

    // 创建缓存数据的副本
    cachedData = Pkcs7Calloc(fileSize + 1, sizeof(char));
    if (cachedData == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Memory allocation failed for cache data");
        rc = PKCS7_MEMORY_EXHAUST;
        goto cleanup;
    }
    
    if (memcpy_s(cachedData, fileSize + 1, fileData, fileSize) != EOK) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to copy cache data");
        rc = PKCS7_INVALID_VALUE;
        goto cleanup;
    }
    cachedData[fileSize] = '\0';

    // 设置缓存
    cache->hashListData = cachedData;
    cache->hashListSize = fileSize;
    cache->verifyTime = time(NULL);
    
    if (strcpy_s(cache->certDirPath, sizeof(cache->certDirPath), OEM_CERT_PATH) != EOK) {
        HILOG_WARN(HILOG_MODULE_AAFWK, "Failed to copy certificate directory path");
    }
    
    g_cacheValid = true;
    cachedData = NULL; // 所有权转移给cache，避免重复释放

    HILOG_INFO(HILOG_MODULE_AAFWK, "Hash list verification successful");

cleanup:
    if (fileData) {
        Pkcs7Free(fileData);
        fileData = NULL;
    }
    if (signature) {
        Pkcs7Free(signature);
        signature = NULL;
    }
    if (cachedData) {
        Pkcs7Free(cachedData);
        cachedData = NULL;
    }
    return rc;
}

/* valid digest alg now: sha256 sha384 sha512 */
static bool InvalidDigestAlg(const mbedtls_asn1_buf *alg)
{
    return MBEDTLS_OID_CMP(MBEDTLS_OID_DIGEST_ALG_SHA256, alg) &&
        MBEDTLS_OID_CMP(MBEDTLS_OID_DIGEST_ALG_SHA384, alg) &&
        MBEDTLS_OID_CMP(MBEDTLS_OID_DIGEST_ALG_SHA512, alg);
}

static int32_t GetContentInfoType(unsigned char **p, const unsigned char *end,
                              mbedtls_asn1_buf *contentType, bool *hasContent)
{
    size_t seqLen = 0;
    size_t len = 0;
    int32_t rc;

    rc = mbedtls_asn1_get_tag(p, end, &seqLen, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
    if (rc) {
        return rc;
    }
    unsigned char *start = *p;
    end = start + seqLen;
    rc = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_OID);
    if (rc) {
        return rc;
    }
    contentType->tag = MBEDTLS_ASN1_OID;
    contentType->len = len;
    contentType->p = *p;
    *hasContent = (seqLen != len + (*p - start));
    *p += len; /* pass the oid info to the real content location. */

    return PKCS7_SUCC;
}

static int32_t GetContentLenOfContentInfo(unsigned char **p, const unsigned char *end, size_t *len)
{
    return mbedtls_asn1_get_tag(p, end, len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_CONTEXT_SPECIFIC);
}

static int32_t ParseSignerVersion(unsigned char **p, const unsigned char *end, SignerInfo *signer)
{
    return mbedtls_asn1_get_int(p, end, &signer->version);
}

static int32_t ParseSignerIssuerAndSerialNum(unsigned char **p, const unsigned char *end, SignerInfo *signer)
{
    int32_t rc;
    size_t len;

    rc = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
    if (rc) {
        return rc;
    }

    signer->issuerRaw.p = *p;
    rc = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
    if (rc) {
        return rc;
    }
    /* parse issuer name */
    rc = mbedtls_x509_get_name(p, *p + len, &signer->issuer);
    if (rc) {
        return rc;
    }
    signer->issuerRaw.len = *p - signer->issuerRaw.p; /* not include the serial. */

    rc = mbedtls_x509_get_serial(p, end, &signer->serial);

    return rc;
}

static int32_t ParseSignerDigestAlg(unsigned char **p, const unsigned char *end, SignerInfo *signer)
{
    int32_t rc = mbedtls_asn1_get_alg_null(p, end, &signer->digestAlgId);
    if (rc) {
        return rc;
    }
    if (InvalidDigestAlg(&signer->digestAlgId)) {
        return PKCS7_INVALID_DIGEST_ALG;
    }
    return PKCS7_SUCC;
}

static int32_t ParseSignerAuthAttr(unsigned char **p, const unsigned char *end, SignerInfo *signer)
{
    int32_t rc;
    size_t len = 0;
    unsigned char *raw = *p;

    rc = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_CONTEXT_SPECIFIC);
    if (rc) {
        return PKCS7_SUCC; /* because this is optional item */
    }
    signer->authAttr.tag = MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_CONTEXT_SPECIFIC;
    signer->authAttr.p = *p;
    signer->authAttr.len = len;
    size_t tlLen = *p - raw;
    *p += len;

    signer->authAttrRaw.p = raw;
    signer->authAttrRaw.len = len + tlLen;
    return PKCS7_SUCC;
}

/*
 * check if enc alg is rsa/ecdsa 256/384/512
 */
static bool InvalidDigestEncAlg(const mbedtls_x509_buf *alg)
{
    return MBEDTLS_OID_CMP(MBEDTLS_OID_PKCS1_SHA256, alg) &&
        MBEDTLS_OID_CMP(MBEDTLS_OID_PKCS1_SHA384, alg) &&
        MBEDTLS_OID_CMP(MBEDTLS_OID_PKCS1_SHA512, alg) &&
        MBEDTLS_OID_CMP(MBEDTLS_OID_ECDSA_SHA256, alg) &&
        MBEDTLS_OID_CMP(MBEDTLS_OID_ECDSA_SHA384, alg) &&
        MBEDTLS_OID_CMP(MBEDTLS_OID_ECDSA_SHA512, alg) &&
        MBEDTLS_OID_CMP(MBEDTLS_OID_RSASSA_PSS, alg);
}

static int32_t ParseSignerEncAlg(unsigned char **p, const unsigned char *end, SignerInfo *signer)
{
    int32_t rc;
    mbedtls_asn1_buf params = {0};
    /* params not be used now */
    rc = mbedtls_asn1_get_alg(p, end, &signer->digestEncAlgId, &params);
    if (rc) {
        return rc;
    }
    if (InvalidDigestEncAlg(&signer->digestEncAlgId)) {
        return PKCS7_INVALID_SIGNING_ALG;
    }
    return PKCS7_SUCC;
}

/*
 * encryptedDigest EncryptedDigest,
 *   EncryptedDigest ::= OCTET STRING
 */
static int32_t ParseSignerSignature(unsigned char **p, const unsigned char *end, SignerInfo *signer)
{
    int32_t rc;
    size_t len = 0;

    rc = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_OCTET_STRING);
    if (rc) {
        return rc;
    }
    signer->signature.tag = MBEDTLS_ASN1_OCTET_STRING;
    signer->signature.len = len;
    signer->signature.p = *p;
    *p += len;
    return PKCS7_SUCC;
}

static int32_t GetSignerSignature(const SignerInfo *signer, unsigned char **sig, size_t *sigLen)
{
    size_t len = signer->signature.len;
    unsigned char *buf = signer->signature.p;
    *sig = buf;
    *sigLen = len;
    return PKCS7_SUCC;
}

static int32_t ParseSignerUnAuthAttr(unsigned char **p, const unsigned char *end, SignerInfo *signer)
{
    int32_t rc;
    size_t len = 0;

    /* the optional unauth attr is not exist */
    if (end - *p < 1) {
        return PKCS7_SUCC;
    }
    rc = mbedtls_asn1_get_tag(p, end, &len, (MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_CONTEXT_SPECIFIC) + 1);
    if (rc) {
        return rc;
    }
    signer->unAuthAttr.tag = (MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_CONTEXT_SPECIFIC) + 1;
    signer->unAuthAttr.len = len;
    signer->unAuthAttr.p = *p;
    *p += len;
    return PKCS7_SUCC;
}

static int32_t SerialCmp(const mbedtls_x509_buf *a, const mbedtls_x509_buf *b)
{
    if (a->len == b->len && memcmp(a->p, b->p, a->len) == 0) {
        return 0;
    }
    return -1;
}

#define DIFF_NUM 32
static bool IsLegitString(int32_t tag)
{
    if (tag == MBEDTLS_ASN1_UTF8_STRING || tag == MBEDTLS_ASN1_PRINTABLE_STRING) {
        return true;
    }
    return false;
}

static int32_t CompareX509String(const mbedtls_x509_buf *first, const mbedtls_x509_buf *second)
{
    if (IsLegitString(first->tag) && IsLegitString(second->tag)) {
        for (int32_t i = 0; i < first->len; i++) {
            if (first->p[i] == second->p[i] ||
                ((islower(first->p[i]) != 0) && (first->p[i] - DIFF_NUM == second->p[i])) ||
                ((isupper(first->p[i]) != 0) && (first->p[i] + DIFF_NUM == second->p[i]))) {
                continue;
            }
            return -1;
        }
        return 0;
    }
    return -1;
}

static int32_t GetDeps(const mbedtls_x509_name *nameList)
{
    int32_t deps = 0;
    while (nameList != NULL) {
        nameList = nameList->next;
        deps++;
    }
    return deps;
}

static int32_t CompareX509NameList(const mbedtls_x509_name *first, const mbedtls_x509_name *second)
{
    if (first == NULL || second == NULL) {
        return -1;
    }
    int32_t firstDeps = GetDeps(first);
    int32_t secondDeps = GetDeps(second);
    if (firstDeps != secondDeps) {
        return -1;
    }
    for (int32_t i = 0; i < firstDeps; i++) {
        if (first->oid.tag != second->oid.tag ||
            first->oid.len != second->oid.len ||
            memcmp(first->oid.p, second->oid.p, second->oid.len) != 0 ||
            first->MBEDTLS_PRIVATE(next_merged) != second->MBEDTLS_PRIVATE(next_merged) ||
            first->val.len != second->val.len) {
            return -1;
        }
        if (CompareX509String(&first->val, &second->val) != 0) {
            return -1;
        }
        first = first->next;
        second = second->next;
    }
    return 0;
}

static int32_t ParseSignedDataSignerInfos(unsigned char **p, const unsigned char *end, SignerInfo *signers)
{
    int32_t rc;
    size_t len = 0;

    rc = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SET);
    if (rc || len == 0) {
        return PKCS7_HAS_NO_SIGNER_INFO;
    }
    end = *p + len; // update end to the SET end.

    while (*p < end) {
        size_t oneSignerLen;
        unsigned char *oneSignerEnd = NULL;
        /* parse one signer info */
        rc = mbedtls_asn1_get_tag(p, end, &oneSignerLen, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
        PKCS7_ERR_RETURN_WITH_LOG(rc);

        oneSignerEnd = *p + oneSignerLen;
        /* parse version */
        rc = ParseSignerVersion(p, oneSignerEnd, signers);
        PKCS7_ERR_RETURN_WITH_LOG(rc);

        /* parse issuerAndSerialNum */
        rc = ParseSignerIssuerAndSerialNum(p, oneSignerEnd, signers);
        PKCS7_ERR_RETURN_WITH_LOG(rc);

        /* parse digestAlgorithm */
        rc = ParseSignerDigestAlg(p, oneSignerEnd, signers);
        PKCS7_ERR_RETURN_WITH_LOG(rc);

        /* parse authenticatedAttributes */
        rc = ParseSignerAuthAttr(p, oneSignerEnd, signers);
        PKCS7_ERR_RETURN_WITH_LOG(rc);

        /* parse digestEncryptionAlgorithm */
        rc = ParseSignerEncAlg(p, oneSignerEnd, signers);
        PKCS7_ERR_RETURN_WITH_LOG(rc);

        /* parse encryptedDigest */
        rc = ParseSignerSignature(p, oneSignerEnd, signers);
        PKCS7_ERR_RETURN_WITH_LOG(rc);

        /* parse unauthenticatedAttributes */
        rc = ParseSignerUnAuthAttr(p, oneSignerEnd, signers);
        PKCS7_ERR_RETURN_WITH_LOG(rc);

        if (*p < end) {
            signers->next = Pkcs7Calloc(1, sizeof(*signers));
            if (signers->next == NULL) {
                /* release resource in main entry. */
                return PKCS7_MEMORY_EXHAUST;
            }
            signers = signers->next;
        }
    }
    return rc;
}

static int32_t ParseSignedDataVersion(unsigned char **p, const unsigned char *end, int32_t *ver)
{
    int32_t rc = mbedtls_asn1_get_int(p, end, ver);
    if (rc) {
        return rc;
    }

    if (*ver != PKCS7_SIGNED_DATA_VERSION) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Invalid version : %d\n", *ver);
        return PKCS7_INVALID_VERSION;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "Parse signed data version success\n");
    return PKCS7_SUCC;
}

static int32_t ParseSignedDataDigestAlgs(unsigned char **p, const unsigned char *end, DigestAlgId *algIds)
{
    int32_t rc;
    size_t len = 0;

    /* parse SET OF header */
    rc = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SET);
    if (rc) {
        return rc;
    }
    end = *p + len;

    /* parse SET OF 's digest alg content */
    DigestAlgId *id = algIds;
    while (*p < end) {
        mbedtls_asn1_buf params = {0};
        /* alg param is supported, but not be used now */
        rc = mbedtls_asn1_get_alg(p, end, &id->algBuf, &params);
        if (rc) {
            return rc;
        }
        if (InvalidDigestAlg(&id->algBuf)) {
            return PKCS7_INVALID_DIGEST_ALG;
        }
        if (*p < end) {
            id->next = Pkcs7Calloc(1, sizeof(DigestAlgId));
            if (id->next == NULL) {
                /* resource will be released in parse main entry */
                return PKCS7_MEMORY_EXHAUST;
            }
            id = id->next;
        }
    }
    return PKCS7_SUCC;
}

static void DlogContentInfo(const Content *content)
{
    int32_t len = content->data.len;
    if (len <= 0) {
        return;
    }
    char *info = Pkcs7Calloc(len + 1, sizeof(char));
    if (info == NULL) {
        return;
    }
    if (strncpy_s(info, len + 1, (char *)content->data.p, len) != EOK) {
        Pkcs7Free(info);
        return;
    }
    Pkcs7Free(info);
}

static int32_t ParseSignedDataContentInfo(unsigned char **p, const unsigned char *end, Content *content)
{
    int32_t rc;
    size_t len = 0;
    bool hasContent = false;

    rc = GetContentInfoType(p, end, &content->oid, &hasContent);
    if (rc) {
        return rc;
    }

    if (MBEDTLS_OID_CMP(MBEDTLS_OID_PKCS7_DATA, &content->oid) || !hasContent) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Invalid content type or has no real content");
        return PKCS7_INVALID_CONTENT_TYPE_OR_NO_CONTENT;
    }
    rc = GetContentLenOfContentInfo(p, end, &len);
    if (rc) {
        return rc;
    }
    content->data.tag = MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_CONTEXT_SPECIFIC; // has no use
    content->data.p = *p;
    content->data.len = len;
    DlogContentInfo(content);
    *p += len;
    return PKCS7_SUCC;
}

static int32_t ParseSignedDataCerts(unsigned char **p, const unsigned char *end, mbedtls_x509_crt **certs)
{
    int32_t rc;
    size_t len = 0;

    rc = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_CONTEXT_SPECIFIC);
    if (rc) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Has no certificates in signed data.");
        return PKCS7_SUCC;
    }
    *certs = mbedtls_calloc(1, sizeof(**certs));
    if (*certs == NULL) {
        return PKCS7_MEMORY_EXHAUST;
    }
    mbedtls_x509_crt_init(*certs);

    unsigned char *certsEnd = *p + len;
    int32_t cnt = 0;
    while (*p < certsEnd) {
        /* scan every cert */
        size_t oneCertLen;
        unsigned char *seqBegin = *p;
        rc = mbedtls_asn1_get_tag(p, end, &oneCertLen, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
        if (rc) {
            mbedtls_free(*certs);
            *certs = NULL;
            return rc;
        }
        if (oneCertLen + (*p - seqBegin) > (certsEnd - seqBegin)) {
            mbedtls_free(*certs);
            *certs = NULL;
            return PKCS7_PARSING_ERROR;
        }
        rc = mbedtls_x509_crt_parse(*certs, seqBegin, oneCertLen + (*p - seqBegin));
        if (rc) {
            mbedtls_free(*certs);
            *certs = NULL;
            return rc;
        }
        *p += oneCertLen;
        cnt++;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "Parse signed data certs success");
    return rc;
}

static int32_t ParseSignedDataCrl(unsigned char **p, const unsigned char *end, mbedtls_x509_crl *crl)
{
    int32_t rc;
    size_t len = 0;

    rc = mbedtls_asn1_get_tag(p, end, &len, (MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_CONTEXT_SPECIFIC) + 1);
    if (rc) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "Has no crl in signed data.");
        return PKCS7_SUCC;
    }
    mbedtls_x509_crl_init(crl);
    rc = mbedtls_x509_crl_parse(crl, *p, len);
    *p += len;
    return rc;
}

static mbedtls_x509_crt *FindSuperCert(mbedtls_x509_crt *cur, mbedtls_x509_crt *certsList)
{
    /* current level's subject is next level issuer */
    while (certsList != NULL) {
        if (CompareX509NameList(&cur->issuer, &certsList->subject) == 0) {
            break;
        }
        certsList = certsList->next;
    }
    return certsList;
}

// 验证单个证书文件
static int32_t VerifyCertificateFile(const char *certPath, const unsigned char *expectedHash, 
                                   mbedtls_x509_crt *topCert)
{
    unsigned char actualHash[SHA256_HASH_SIZE];
    char actualHashHex[65];
    int32_t rc;

    if (certPath == NULL || expectedHash == NULL || topCert == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    // 计算文件哈希
    rc = CalculateFileSha256(certPath, actualHash, actualHashHex);
    if (rc != PKCS7_SUCC) {
        return rc;
    }

    // 比较哈希值
    if (memcmp(actualHash, expectedHash, SHA256_HASH_SIZE) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Certificate hash verification failed for: %s", certPath);
        return PKCS7_INVALID_VALUE;
    }

    // 解析证书
    mbedtls_x509_crt tmpCert;
    mbedtls_x509_crt_init(&tmpCert);
    
    rc = mbedtls_x509_crt_parse_file(&tmpCert, certPath);
    if (rc != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to parse certificate: %s, error: -0x%04X", certPath, -rc);
        mbedtls_x509_crt_free(&tmpCert);
        return rc;
    }

    // 检查证书匹配
    int32_t match = CompareX509NameList(&tmpCert.subject, &topCert->issuer);
    mbedtls_x509_crt_free(&tmpCert);
    
    return (match == 0) ? PKCS7_SUCC : PKCS7_INVALID_VALUE;
}

// 加载根证书
static int32_t LoadRootCertificate(const char *certPath)
{
    if (certPath == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    mbedtls_x509_crt_free(&g_ohosRootCert);
    mbedtls_x509_crt_init(&g_ohosRootCert);
    
    int32_t rc = mbedtls_x509_crt_parse_file(&g_ohosRootCert, certPath);
    if (rc != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to load root certificate: %s, error: -0x%04X", certPath, -rc);
        return PKCS7_HAS_NO_SIGNER_CRT;
    }
    
    return PKCS7_SUCC;
}

// 辅助函数
static bool ShouldSkipFile(const struct dirent *entry)
{
    if (entry == NULL) {
        return true;
    }
    
    return (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0 ||
            strstr(entry->d_name, ".pem") == NULL);
}

static const CertHashEntry *FindHashEntry(const CertHashList *hashList, const char *fileName)
{
    if (hashList == NULL || fileName == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < hashList->count; i++) {
        if (hashList->entries[i].certName != NULL && 
            strcmp(fileName, hashList->entries[i].certName) == 0) {
            return &hashList->entries[i];
        }
    }
    return NULL;
}

// 扫描证书目录
static int32_t ScanCertificateDirectory(DIR *dir, const CertHashList *hashList, 
                                       mbedtls_x509_crt *topCert)
{
    struct dirent *entry;
    char certPath[MAX_CERTPATH_LEN];
    int32_t rc = PKCS7_PARSING_ERROR;

    if (dir == NULL || hashList == NULL || topCert == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (ShouldSkipFile(entry)) {
            continue;
        }

        // 构建证书路径
        if (snprintf_s(certPath, sizeof(certPath), sizeof(certPath) - 1, 
                      "%s%s", OEM_CERT_PATH, entry->d_name) < 0) {
            continue;
        }

        // 查找匹配的哈希条目
        const CertHashEntry *hashEntry = FindHashEntry(hashList, entry->d_name);
        if (hashEntry == NULL) {
            continue;
        }

        // 验证证书文件
        rc = VerifyCertificateFile(certPath, hashEntry->hash, topCert);
        if (rc == PKCS7_SUCC) {
            // 加载根证书
            rc = LoadRootCertificate(certPath);
            if (rc == PKCS7_SUCC) {
                break;
            }
        }
    }

    return rc;
}

// 检查证书路径是否可用
bool IsCertPathAvailable(void)
{
    struct stat st;
    if (stat(OEM_CERT_PATH, &st) != 0) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "Certificate path does not exist: %s", OEM_CERT_PATH);
        return false;
    }
    
    if (!S_ISDIR(st.st_mode)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Certificate path is not a directory: %s", OEM_CERT_PATH);
        return false;
    }
    
    // 检查目录是否可读
    if (access(OEM_CERT_PATH, 0) != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Certificate directory is not readable: %s", OEM_CERT_PATH);
        return false;
    }
    
    return true;
}

// 加载内置证书
static int32_t LoadSelfSignedCert(void)
{
    mbedtls_x509_crt_free(&g_ohosRootCert);
    mbedtls_x509_crt_init(&g_ohosRootCert);

    HILOG_INFO(HILOG_MODULE_AAFWK, "Loading built-in OpenHarmony root certificate");

    // 规范化证书格式：将\r\n转换为\n
    size_t certLen = strlen((const char*)OHOS_ROOT_CERT_IN_PEM);
    char *normalizedCert = Pkcs7Calloc(certLen + 1, 1);
    if (normalizedCert == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Memory allocation failed for certificate normalization");
        return PKCS7_MEMORY_EXHAUST;
    }

    // 复制并替换换行符
    const char *src = (const char*)OHOS_ROOT_CERT_IN_PEM;
    char *dst = normalizedCert;
    while (*src) {
        if (src[0] == '\r' && src[1] == '\n') {
            *dst++ = '\n';
            src += 2; // 跳过\r\n
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';

    int32_t rc = mbedtls_x509_crt_parse(&g_ohosRootCert, (const unsigned char*)normalizedCert, 
                                       strlen(normalizedCert) + 1);

    Pkcs7Free(normalizedCert);

    if (rc != 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to parse built-in certificate: -0x%04X", -rc);
        return PKCS7_HAS_NO_SIGNER_CRT;
    }
    
    HILOG_INFO(HILOG_MODULE_AAFWK, "Built-in certificate loaded successfully with normalized line endings");
    return PKCS7_SUCC;
}

static int32_t FindAndStoreRootCertWithCache(mbedtls_x509_crt *topCert, CertVerifyCache *cache)
{
    if (topCert == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    // 如果缓存无效，先验证哈希列表
    if (cache == NULL || cache->hashListData == NULL) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "Cache invalid, verifying hash list");
        int32_t rc = VerifyAndCacheHashList(cache);
        if (rc != PKCS7_SUCC) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Hash list verification failed: %d", rc);
            return rc;
        }
    }

    CertHashList hashList = {0};
    DIR *dir = NULL;
    int32_t rc = PKCS7_PARSING_ERROR;

    // 解析哈希列表
    rc = ParseHashListTextFormat(cache->hashListData, cache->hashListSize, &hashList);
    if (rc != PKCS7_SUCC || hashList.count == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to parse hash list or no entries found");
        FreeCertHashList(&hashList);
        return PKCS7_INVALID_VALUE;
    }

    // 打开证书目录
    dir = opendir(OEM_CERT_PATH);
    if (dir == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to open certificate directory: %s", OEM_CERT_PATH);
        FreeCertHashList(&hashList);
        return PKCS7_BUILD_CERT_PATH_FAIL;
    }

    // 扫描证书文件
    rc = ScanCertificateDirectory(dir, &hashList, topCert);

    closedir(dir);
    FreeCertHashList(&hashList);
    
    if (rc != PKCS7_SUCC) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to find matching root certificate");
    }
    
    return rc;
}

static int32_t LoadRootCert()
{
    int32_t rc = 0;
    if (!g_rootCertLoaded) {
        mbedtls_x509_crt_init(&g_rootCaG2Cert);
        rc = mbedtls_x509_crt_parse(&g_rootCaG2Cert, ROOT_CA_G2_CERT_IN_PEM, sizeof(ROOT_CA_G2_CERT_IN_PEM));
        if (rc) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "load root ca failed!");
            return rc;
        } else {
            HILOG_INFO(HILOG_MODULE_AAFWK, "load root ca success.");
        }
        g_rootCertLoaded = true;
    }
    return rc;
}

static void UnLoadRootCert(void)
{
    if (g_rootCertLoaded) {
        mbedtls_x509_crt_free(&g_rootCaG2Cert);
        g_rootCertLoaded = false;
    }
}

static int32_t LoadDebugModeRootCert(void)
{
    mbedtls_x509_crt_init(&g_debugModeRootCert);
    int32_t rc = mbedtls_x509_crt_parse(&g_debugModeRootCert, DEBUG_MODE_ROOT_CERT_IN_PEM,
        sizeof(DEBUG_MODE_ROOT_CERT_IN_PEM));
    if (rc) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "load debug mode root ca failed %d", rc);
        return rc;
    } else {
        HILOG_INFO(HILOG_MODULE_AAFWK, "load debug mode root ca success");
    }
    return rc;
}

static int32_t UnLoadDebugModeRootCert(void)
{
    mbedtls_x509_crt_free(&g_debugModeRootCert);
    return PKCS7_SUCC;
}

static void UnLoadSelfSignedCert(void)
{
    mbedtls_x509_crt_free(&g_ohosRootCert);
}

static mbedtls_x509_crt *GetLowLevelCerts(SignedData *signedData)
{
    SignerInfo *signer = &signedData->signers;
    mbedtls_x509_buf *signerSerial = &signer->serial;
    mbedtls_x509_name *signerIssuer = &signer->issuer;
    mbedtls_x509_crt *cert = signedData->certs;
    while (cert != NULL) {
        if (SerialCmp(signerSerial, &cert->serial) == 0 &&
            CompareX509NameList(signerIssuer, &cert->issuer) == 0) {
            HILOG_INFO(HILOG_MODULE_AAFWK, "Found signer's low level cert");
            return cert;
        }
        cert = cert->next;
    }
    return NULL;
}

static int32_t ParseSignedData(unsigned char *buf, size_t bufLen, SignedData *signedData)
{
    unsigned char *p = buf;
    unsigned char *end = buf + bufLen;
    size_t len = 0;
    int32_t rc;

    /* parse SignedData sequence header */
    rc = mbedtls_asn1_get_tag(&p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
    if (rc) {
        return rc;
    }

    /* parse version of signed data */
    rc = ParseSignedDataVersion(&p, end, &signedData->version);
    if (rc) {
        return rc;
    }

    /* parse digestAlgorithms */
    rc = ParseSignedDataDigestAlgs(&p, end, &signedData->digestAlgIds);
    if (rc) {
        return rc;
    }

    /* parse contentInfo */
    rc = ParseSignedDataContentInfo(&p, end, &signedData->content);
    if (rc) {
        return rc;
    }

    if (p >= end) {
        return PKCS7_PARSING_ERROR;
    }
    /* parse certificates (optional) */
    rc = ParseSignedDataCerts(&p, end, &signedData->certs);
    if (rc) {
        return rc;
    }

    /* parse crls (optional) */
    rc = ParseSignedDataCrl(&p, end, &signedData->crl);
    if (rc) {
        return rc;
    }

    /* parse signerInfos */
    rc = ParseSignedDataSignerInfos(&p, end, &signedData->signers);
    HILOG_INFO(HILOG_MODULE_AAFWK, "ParseSignedDataSignerInfos %d", rc);
    return rc;
}

static bool IsSigedDataOid(const Pkcs7 *pkcs7)
{
    return !MBEDTLS_OID_CMP(MBEDTLS_OID_PKCS7_SIGNED_DATA, &pkcs7->contentTypeOid);
}

static void FreeSignedDataDigestAlgs(Pkcs7 *pkcs7)
{
    DigestAlgId *alg = pkcs7->signedData.digestAlgIds.next;
    DigestAlgId *next = NULL;

    while (alg != NULL) {
        next = alg->next;
        Pkcs7Free(alg);
        alg = next;
    }
    pkcs7->signedData.digestAlgIds.next = NULL;
}

static void FreeSignerCerts(SignerInfo *signer)
{
    if (signer->certPath.crt != NULL) {
        mbedtls_x509_crt_free(signer->certPath.crt);
        mbedtls_free(signer->certPath.crt);
        signer->certPath.crt = NULL;
    }
}

static void FreeSignerIssuer(SignerInfo *signer)
{
    mbedtls_x509_name *name_cur = NULL;
    mbedtls_x509_name *name_prv = NULL;
    name_cur = signer->issuer.next;
    while (name_cur != NULL) {
        name_prv = name_cur;
        name_cur = name_cur->next;
        mbedtls_free(name_prv);
    }
    signer->issuer.next = NULL;
}

static void FreeSignersInfo(Pkcs7 *pkcs7)
{
    SignerInfo *signer = pkcs7->signedData.signers.next;
    SignerInfo *next = NULL;

    while (signer != NULL) {
        next = signer->next;
        FreeSignerCerts(signer);
        FreeSignerIssuer(signer);
        Pkcs7Free(signer);
        signer = next;
    }
    pkcs7->signedData.signers.next = NULL;
    FreeSignerCerts(&pkcs7->signedData.signers);
    FreeSignerIssuer(&pkcs7->signedData.signers);
}

static void FreeSignedDataCerts(Pkcs7 *pkcs7)
{
    if (pkcs7->signedData.certs != NULL) {
        mbedtls_x509_crt_free(pkcs7->signedData.certs);
        mbedtls_free(pkcs7->signedData.certs);
        pkcs7->signedData.certs = NULL;
    }
}

static void FreeSignedDataCrl(Pkcs7 *pkcs7)
{
    mbedtls_x509_crl_free(&pkcs7->signedData.crl);
    return;
}

static int32_t GetCertsNumOfSignedData(const mbedtls_x509_crt *crts)
{
    int32_t cnt = 0;
    while (crts != NULL) {
        crts = crts->next;
        cnt++;
    }
    return cnt;
}

static void DelCertOfSignedData(SignedData *signedData, mbedtls_x509_crt *crt)
{
    mbedtls_x509_crt *head = signedData->certs;
    if (crt == head) {
        signedData->certs = crt->next;
        crt->next = NULL;
    } else {
        mbedtls_x509_crt *prev = head;
        while (head != NULL) {
            if (head == crt) {
                prev->next = crt->next;
                crt->next = NULL;
                break;
            }
            prev = head;
            head = head->next;
        }
    }
}

static void AddCertToSignerCertPath(SignerInfo *signer, mbedtls_x509_crt *crt)
{
    mbedtls_x509_crt *prev = signer->certPath.crt;
    mbedtls_x509_crt *cur = prev;
    if (prev == NULL) {
        signer->certPath.crt = crt;
        crt->next = NULL;
    } else {
        while (cur != NULL) {
            prev = cur;
            cur = cur->next;
        }
        prev->next = crt;
        crt->next = NULL;
    }

    signer->certPath.depth++;
}

static int32_t BuildSignerCertPath(SignerInfo *signer, mbedtls_x509_crt *lowerCrt, SignedData *signedData)
{
    if (signer == NULL || lowerCrt == NULL || signedData == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    
    HILOG_INFO(HILOG_MODULE_AAFWK, "Building certificate path");
    
    int32_t scanCnt = 0;
    int32_t rc = PKCS7_SUCC;
    
    // 确保根证书已加载
    if (!g_rootCertLoaded) {
        rc = LoadRootCert();
        if (rc != PKCS7_SUCC) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Root certificate not loaded: %d", rc);
            return PKCS7_ROOT_CA_NOT_VALID;
        }
    }
    
    // 设置默认根证书
    signer->rootCert = &g_rootCaG2Cert;

    mbedtls_x509_crt *certs = signedData->certs;
    if (certs == NULL) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "No certificates in signed data");
        return PKCS7_INVALID_VALUE;
    }
    
    mbedtls_x509_crt *cur = lowerCrt;
    mbedtls_x509_crt *next = NULL;
    int32_t certsCnt = GetCertsNumOfSignedData(certs);
    
    HILOG_INFO(HILOG_MODULE_AAFWK, "Certificate chain has %d certificates", certsCnt);
    
    DelCertOfSignedData(signedData, cur);
    AddCertToSignerCertPath(signer, cur);
    
    while (true) {
        next = FindSuperCert(cur, signedData->certs);
        if (next == NULL) {
            HILOG_INFO(HILOG_MODULE_AAFWK, "Reached top of certificate chain");
            
            // 根据路径可用性选择证书源
            if (IsCertPathAvailable()) {
                HILOG_INFO(HILOG_MODULE_AAFWK, "Using local certificate path for root certificate");
                rc = FindAndStoreRootCertWithCache(cur, &g_certCache);
            } else {
                HILOG_INFO(HILOG_MODULE_AAFWK, "Using built-in OpenHarmony root certificate");
                rc = LoadSelfSignedCert();
                if (rc == PKCS7_SUCC) {
                    // 设置签名者的根证书为内置证书
                    signer->rootCert = &g_ohosRootCert;
                }
            }
            
            if (rc != PKCS7_SUCC) {
                HILOG_ERROR(HILOG_MODULE_AAFWK, "Failed to load root certificate: %d", rc);
            }
            break;
        } else {
            DelCertOfSignedData(signedData, next);
            AddCertToSignerCertPath(signer, next);
        }
        
        scanCnt++;
        if (scanCnt > certsCnt) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Certificate chain scanning exceeded limit");
            rc = PKCS7_BUILD_CERT_PATH_FAIL;
            break;
        }
        cur = next;
    }
    
    HILOG_INFO(HILOG_MODULE_AAFWK, "Certificate path building completed: %d", rc);
    return rc;
}

static int32_t ConstructSignerCerts(SignedData *signedData)
{
    if (signedData == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    
    SignerInfo *signer = &signedData->signers;
    while (signer != NULL) {
        mbedtls_x509_crt *cert = GetLowLevelCerts(signedData);
        if (cert == NULL) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Could not found signer's lowest cert");
            return PKCS7_INVALID_VALUE;
        }
        
        // 根据路径可用性构建证书路径
        int32_t rc = BuildSignerCertPath(signer, cert, signedData);
        if (rc != PKCS7_SUCC) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Build signer certificate path failed: %d", rc);
            return rc;
        }

        signer = signer->next;
    }
    return PKCS7_SUCC;
}

/* get signer digest alg */
static int32_t GetSignerDigestAlg(const SignerInfo *signer, mbedtls_md_type_t *algType)
{
    const mbedtls_x509_buf *alg = &signer->digestAlgId;
    if (!MBEDTLS_OID_CMP(MBEDTLS_OID_DIGEST_ALG_SHA256, alg)) {
        *algType = MBEDTLS_MD_SHA256;
        return PKCS7_SUCC;
    }
    if (!MBEDTLS_OID_CMP(MBEDTLS_OID_DIGEST_ALG_SHA384, alg)) {
        *algType = MBEDTLS_MD_SHA384;
        return PKCS7_SUCC;
    }
    if (!MBEDTLS_OID_CMP(MBEDTLS_OID_DIGEST_ALG_SHA512, alg)) {
        *algType = MBEDTLS_MD_SHA512;
        return PKCS7_SUCC;
    }
    return PKCS7_INVALID_DIGEST_ALG;
}

/* get signer pubkey of sign from signer cert */
static int32_t GetSignerPubKeyOfSignature(const SignerInfo *signer, mbedtls_pk_context **pk)
{
    /* signer cert_path first cert is the lowest cert. yet is the signature cert */
    if (signer == NULL || pk == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    if (signer->certPath.crt != NULL) {
        *pk = &signer->certPath.crt->pk;
        return PKCS7_SUCC;
    }
    return PKCS7_INVALID_VALUE;
}

int32_t PKCS7_VerifySignerSignature(const Pkcs7 *pkcs7, PKCS7_CalcDigest calcDigest)
{
    int32_t rc;
    if (pkcs7 == NULL || calcDigest == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    const SignerInfo *signer = &pkcs7->signedData.signers;
    unsigned char *sig = NULL;
    size_t sigLen;
    while (signer != NULL) {
        rc = GetSignerSignature(signer, &sig, &sigLen);
        if (rc) {
            return rc;
        }
        HILOG_INFO(HILOG_MODULE_AAFWK, "get signer signature len : %zu", sigLen);

        mbedtls_pk_context *pk = NULL;
        rc = GetSignerPubKeyOfSignature(signer, &pk);
        if (rc) {
            return rc;
        }
        mbedtls_md_type_t digAlg;
        rc = GetSignerDigestAlg(signer, &digAlg);
        if (rc) {
            return rc;
        }
        unsigned char hash[MAX_HASH_SIZE];
        (void)memset_s(hash, MAX_HASH_SIZE, 0, MAX_HASH_SIZE);
        size_t hashLen = 0;
        rc = calcDigest(pkcs7, signer, digAlg, hash, &hashLen);
        if (rc) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Calculate content hash failed by calling callback");
            return rc;
        }
        /* if is rsassa-pss, need to set padding version to V21, RFC3447 */
        if (!MBEDTLS_OID_CMP(MBEDTLS_OID_RSASSA_PSS, &signer->digestEncAlgId)) {
            mbedtls_rsa_set_padding(pk->MBEDTLS_PRIVATE(pk_ctx), MBEDTLS_RSA_PKCS_V21, (mbedtls_md_type_t)0);
        }
        rc = mbedtls_pk_verify(pk, digAlg, hash, hashLen, sig, sigLen);
        (void)memset_s(hash, MAX_HASH_SIZE, 0, MAX_HASH_SIZE);
        if (rc) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Verify signature failed, returned -0x%04x", rc);
            return rc;
        } else {
            HILOG_INFO(HILOG_MODULE_AAFWK, "Verify signer signature success\n");
        }
        signer = signer->next;
    }
    return rc;
}

static void DLogCrtVerifyInfo(uint32_t flags)
{
    char vrfyBuf[VERIFY_BUF_LEN];
    (void)memset_s(vrfyBuf, VERIFY_BUF_LEN, 0, VERIFY_BUF_LEN);
    mbedtls_x509_crt_verify_info(vrfyBuf, sizeof(vrfyBuf), " ! ", flags);
    HILOG_DEBUG(HILOG_MODULE_AAFWK, "%s", vrfyBuf);
}

static int32_t IsRevoked(const mbedtls_x509_crt *crt, const mbedtls_x509_crl *crl)
{
    mbedtls_x509_crl_entry *cur = (mbedtls_x509_crl_entry *)(&crl->entry);
    while (cur != NULL) {
        if (cur->serial.len == 0) {
            return PKCS7_SUCC;
        }
        if (crt->serial.len != cur->serial.len) {
            cur = cur->next;
            continue;
        }
        if (memcmp(crt->serial.p, cur->serial.p, cur->serial.len) == 0) {
            return PKCS7_IS_REVOKED;
        }
        cur = cur->next;
    }
    return PKCS7_SUCC;
}

static int32_t VerifyCrl(const mbedtls_x509_crt *crt, const mbedtls_x509_crl *crl)
{
    const mbedtls_x509_crl *crlList = crl;
    while (crlList != NULL) {
        if (crlList->version == 0 ||
            CompareX509NameList(&crlList->issuer, &crt->issuer) != 0) {
            crlList = crlList->next;
            continue;
        }
        HILOG_INFO(HILOG_MODULE_AAFWK, "find crl");
        if (IsRevoked(crt, crlList)) {
            return PKCS7_IS_REVOKED;
        }
        crlList = crlList->next;
    }
    return PKCS7_SUCC;
}

static int32_t VerifyClicert(mbedtls_x509_crt *clicert, mbedtls_x509_crt *rootCert, const Pkcs7 *pkcs7)
{
    uint32_t flags = 0;
    int32_t rc = mbedtls_x509_crt_verify(clicert, rootCert,
        (mbedtls_x509_crl *)&pkcs7->signedData.crl, NULL, &flags, NULL, NULL);
    if (rc) {
        DLogCrtVerifyInfo(flags);
    } else {
        HILOG_INFO(HILOG_MODULE_AAFWK, "Verify signers cert chain root cert success");
        if (VerifyCrl(clicert, (mbedtls_x509_crl *)&pkcs7->signedData.crl) != PKCS7_SUCC) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "cert crl verify failed");
            return PKCS7_IS_REVOKED;
        }
        return PKCS7_SUCC;
    }
    return rc;
}

int32_t PKCS7_VerifyCertsChain(const Pkcs7 *pkcs7)
{
    if (pkcs7 == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    
    int32_t cnt = 0;
    const SignerInfo *signer = &pkcs7->signedData.signers;
    
    while (signer != NULL) {
        mbedtls_x509_crt *clicert = signer->certPath.crt;
        if (clicert == NULL) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Signer has no certificates");
            return PKCS7_HAS_NO_SIGNER_CRT;
        }

        int32_t rc;
        cnt++;

        if (g_debugModeEnabled) {
            rc = VerifyClicert(clicert, &g_debugModeRootCert, pkcs7);
            if (rc == PKCS7_SUCC) {
                signer = signer->next;
                continue;
            }
            HILOG_WARN(HILOG_MODULE_AAFWK, "Debug mode verification failed");
        }
#ifndef OHOS_SIGN_HAPS_BY_SERVER
        // 首先尝试使用内置OpenHarmony根证书
        rc = VerifyClicert(clicert, &g_ohosRootCert, pkcs7);
        if (rc == PKCS7_SUCC) {
            signer = signer->next;
            continue;
        }
#endif
        // 如果内置证书验证失败，尝试系统根证书
        HILOG_WARN(HILOG_MODULE_AAFWK, "Built-in certificate failed, trying system root certificate");
        rc = VerifyClicert(clicert, &g_rootCaG2Cert, pkcs7);
        if (rc == PKCS7_SUCC) {
            HILOG_INFO(HILOG_MODULE_AAFWK, "System root certificate verification successful");
            signer = signer->next;
            continue;
        }
        HILOG_ERROR(HILOG_MODULE_AAFWK, "All certificate verification attempts failed: %d", rc);
        return rc;
    }
    
    HILOG_INFO(HILOG_MODULE_AAFWK, "All certificate chains verified successfully");
    return PKCS7_SUCC;
}

int32_t PKCS7_GetSignerSignningCertSubject(const SignerInfo *signer, char *subject, size_t subjectLen)
{
    int32_t rc;
    if (signer == NULL || subject == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    const mbedtls_x509_crt *crt = signer->certPath.crt;
    rc = mbedtls_x509_dn_gets(subject, subjectLen, &crt->subject);
    if (rc < 0) {
        return rc;
    }
    return PKCS7_SUCC;
}

int32_t PKCS7_GetSignerSignningCertIssuer(const SignerInfo *signer, char *issuer, size_t issuerLen)
{
    int32_t rc;
    if (signer == NULL || issuer == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    const mbedtls_x509_crt *crt = signer->certPath.crt;
    rc = mbedtls_x509_dn_gets(issuer, issuerLen, &crt->issuer);
    if (rc < 0) {
        return rc;
    }
    return PKCS7_SUCC;
}

static size_t GetSignersCnt(const SignerInfo *signers)
{
    size_t cnt = 0;
    while (signers != NULL) {
        cnt++;
        signers = signers->next;
    }
    return cnt;
}

static bool IsIncludeRoot(const SignerInfo *signer)
{
    mbedtls_x509_crt *pre = signer->certPath.crt;
    mbedtls_x509_crt *cur = pre;
    int32_t i = 0;
    while (i < signer->certPath.depth && cur != NULL) {
        pre = cur;
        cur = cur->next;
        i++;
    }

    if (pre == NULL) {
        return false;
    }

    /* root cert is a self-sign cert */
    if (CompareX509NameList(&pre->issuer, &pre->subject) == 0) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "Include root cert");
        return true;
    }
    HILOG_INFO(HILOG_MODULE_AAFWK, "Not include root cert");
    return false;
}

static int32_t GetSignerSignningCertDepth(const SignerInfo *signer)
{
    if (IsIncludeRoot(signer)) {
        return signer->certPath.depth;
    }

    /* root cert is not included in signer->certPath, add 1 for root cert */
    return signer->certPath.depth + 1;
}

void PKCS7_FreeAllSignersResolvedInfo(SignersResovedInfo *sri)
{
    if (sri == NULL) {
        return;
    }
    if (sri->signers != NULL) {
        Pkcs7Free(sri->signers);
        sri->signers = NULL;
    }
    Pkcs7Free(sri);
}

SignersResovedInfo *PKCS7_GetAllSignersResolvedInfo(const Pkcs7 *pkcs7)
{
    SignersResovedInfo *sri = NULL;
    if (pkcs7 == NULL) {
        return NULL;
    }
    size_t signersCnt = GetSignersCnt(&pkcs7->signedData.signers);
    if (signersCnt == 0) {
        return NULL;
    }
    sri = Pkcs7Calloc(1, sizeof(*sri));
    if (sri == NULL) {
        return NULL;
    }
    sri->nrOfSigners = signersCnt;
    sri->signers = Pkcs7Calloc(signersCnt, sizeof(SignerResovledInfo));
    if (sri->signers == NULL) {
        Pkcs7Free(sri);
        return NULL;
    }
    int32_t rc;
    const SignerInfo *signer = &pkcs7->signedData.signers;
    int32_t idx = 0;
    while (signer != NULL && idx < signersCnt) {
        rc = PKCS7_GetSignerSignningCertSubject(signer, sri->signers[idx].subject, sizeof(sri->signers[idx].subject));
        if (rc) {
            goto OUT;
        }
        rc = PKCS7_GetSignerSignningCertIssuer(signer, sri->signers[idx].issuer, sizeof(sri->signers[idx].issuer));
        if (rc) {
            goto OUT;
        }
        sri->signers[idx].depth = GetSignerSignningCertDepth(signer);

        signer = signer->next;
        idx++;
    }
    return sri;
OUT:
    PKCS7_FreeAllSignersResolvedInfo(sri);
    return NULL;
}

int32_t PKCS7_GetDigestInSignerAuthAttr(const SignerInfo *signer, unsigned char **dig, size_t *digLen)
{
    if (signer == NULL || dig == NULL || digLen == NULL) {
        return PKCS7_INVALID_VALUE;
    }
    unsigned char *p = signer->authAttr.p;
    if (p == NULL) {
        return PKCS7_HAS_NO_AUTH_ATTR_IN_SIGNER;
    }
    unsigned char *end = p + signer->authAttr.len;
    size_t tmpLen = 0;

    /* SET OF SEQUENCE */
    while (p < end) {
        size_t seqLen;
        unsigned char *seqEnd = NULL;
        int32_t rc = mbedtls_asn1_get_tag(&p, end, &seqLen, MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED);
        if (rc) {
            return rc;
        }
        seqEnd = p + seqLen;
        /* SEQUENCE : OID and SET */
        size_t oidLen;
        rc = mbedtls_asn1_get_tag(&p, seqEnd, &oidLen, MBEDTLS_ASN1_OID);
        if (rc) {
            return rc;
        }
        if (oidLen == MBEDTLS_OID_SIZE(MBEDTLS_OID_PKCS9_MSG_DIGEST) &&
            memcmp(p, MBEDTLS_OID_PKCS9_MSG_DIGEST, MBEDTLS_OID_SIZE(MBEDTLS_OID_PKCS9_MSG_DIGEST)) == 0) {
            p += oidLen;
            rc = mbedtls_asn1_get_tag(&p, seqEnd, &tmpLen, MBEDTLS_ASN1_SET | MBEDTLS_ASN1_CONSTRUCTED);
            if (rc) {
                return rc;
            }
            /* we just resolve one now. */
            rc = mbedtls_asn1_get_tag(&p, seqEnd, &tmpLen, MBEDTLS_ASN1_OCTET_STRING);
            if (rc) {
                return rc;
            }
            *dig = p;
            *digLen = tmpLen;
            return PKCS7_SUCC;
        } else {
            p = seqEnd;
        }
    }
    return PKCS7_INVALID_VALUE;
}

int32_t PKCS7_GetSignerAuthAttr(const SignerInfo *signer, unsigned char **data, size_t *dataLen)
{
    if (signer == NULL || signer->authAttrRaw.p == NULL || data == NULL || dataLen == NULL) {
        return PKCS7_INVALID_VALUE;
    }
    *dataLen = signer->authAttrRaw.len;
    *data = signer->authAttrRaw.p;
    *(unsigned char *)signer->authAttrRaw.p = MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SET;
    return PKCS7_SUCC;
}

int32_t PKCS7_GetContentData(const Pkcs7 *pkcs7, unsigned char **data, size_t *dataLen)
{
    if (pkcs7 == NULL || data == NULL || dataLen == NULL) {
        return PKCS7_INVALID_PARAM;
    }

    unsigned char *p = pkcs7->signedData.content.data.p;
    size_t len = pkcs7->signedData.content.data.len;
    unsigned char *end = p + len;
    size_t octetLen;
    int32_t rc = mbedtls_asn1_get_tag(&p, end, &octetLen, MBEDTLS_ASN1_OCTET_STRING);
    if (rc != 0) {
        return rc;
    }
    *data = p;
    *dataLen = octetLen;
    return PKCS7_SUCC;
}

int32_t PKCS7_EnableDebugMode(bool mode)
{
    if (g_debugModeEnabled == mode) {
        return PKCS7_SUCC;
    }
    int32_t rc = ((mode == true) ? LoadDebugModeRootCert() : UnLoadDebugModeRootCert());
    if (rc) {
        return rc;
    }
    g_debugModeEnabled = mode;
    return PKCS7_SUCC;
}

#ifdef PARSE_PEM_FORMAT_SIGNED_DATA
static int32_t ParsePemFormatSignedData(const unsigned char *buf, size_t bufLen, mbedtls_pem_context *pem, char *format)
{
    if (bufLen != 0 && strstr((const char *)buf, "-----BEGIN PKCS7-----") != NULL) {
        int32_t rc;
        size_t useLen = 0;
        mbedtls_pem_init(pem);
        rc = mbedtls_pem_read_buffer(pem, "-----BEGIN PKCS7-----", "-----END PKCS7-----",
                                      buf, NULL, 0, &useLen);
        if (rc == 0 && useLen == bufLen) {
            *format = PEM_FORMAT_SINGED_DATA;
            return PKCS7_SUCC;
        }
        mbedtls_pem_free(pem);
    } else {
        *format = DER_FORMAT_SINGED_DATA;
        return PKCS7_SUCC; // DER format
    }
    return PKCS7_INVALID_PARAM;
}
#endif

// 统一的证书加载入口
static int32_t CheckAndLoadCertificates(void)
{
    static bool certificatesLoaded = false;
    static int32_t lastLoadResult = PKCS7_SUCC;
    
    // 如果已经加载过且成功，直接返回缓存结果
    if (certificatesLoaded) {
        return lastLoadResult;
    }
    
    int32_t rc = PKCS7_SUCC;
    
    if (IsCertPathAvailable()) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "Certificate path available, verifying hash list");
        rc = VerifyAndCacheHashList(&g_certCache);
        if (rc != PKCS7_SUCC) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "Hash list verification failed, falling back to built-in certificate");
            // 验证失败时回退到内置证书
            rc = LoadSelfSignedCert();
        }
    } else {
        HILOG_INFO(HILOG_MODULE_AAFWK, "Certificate path not available, using built-in certificate");
        // 清理可能存在的旧缓存
        FreeCertVerifyCache(&g_certCache);
        rc = LoadSelfSignedCert();
    }
    
    certificatesLoaded = true;
    lastLoadResult = rc;
    return rc;
}

int32_t PKCS7_ParseSignedData(const unsigned char *buf, size_t bufLen, Pkcs7 *pkcs7)
{
    int32_t rc;
    size_t len = 0;
    bool hasContent = false;
    unsigned char *start = NULL;
    unsigned char *end = NULL;
    
    if (buf == NULL || bufLen == 0 || pkcs7 == NULL) {
        return PKCS7_INVALID_PARAM;
    }
    
    (void)memset_s(pkcs7, sizeof(*pkcs7), 0, sizeof(*pkcs7));

    HILOG_INFO(HILOG_MODULE_AAFWK, "Starting PKCS7 parsing, bufLen=%zu", bufLen);

    start = (unsigned char *)buf;
    end = start + bufLen;

    // 先解析PKCS7数据，再处理证书
    HILOG_INFO(HILOG_MODULE_AAFWK, "Begin to parse pkcs#7 signed data");

    // 解析ContentInfo头部
    rc = GetContentInfoType(&start, end, &(pkcs7->contentTypeOid), &hasContent);
    if (rc != PKCS7_SUCC) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Get content info type failed: %d", rc);
        goto exit;
    }
    
    if (!IsSigedDataOid(pkcs7) || !hasContent) {
        rc = PKCS7_INVALID_CONTENT_TYPE_OR_NO_CONTENT;
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Input data is not pkcs#7 signed data format or has no content info");
        goto exit;
    }
    
    rc = GetContentLenOfContentInfo(&start, end, &len);
    if (rc != PKCS7_SUCC) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Get content length failed: %d", rc);
        goto exit;
    }
    
    if (start + len > end) {
        rc = PKCS7_INVALID_CONTENT_TYPE_OR_NO_CONTENT;
        HILOG_ERROR(HILOG_MODULE_AAFWK, "The length of input data is invalid");
        goto exit;
    }
    
    rc = ParseSignedData(start, len, &(pkcs7->signedData));
    if (rc != PKCS7_SUCC) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Parse signed data failed: %d", rc);
        goto exit;
    }
    
    HILOG_INFO(HILOG_MODULE_AAFWK, "Parse pkcs#7 signed data success");

    // 现在构建证书路径（这里会根据路径可用性选择证书）
    rc = ConstructSignerCerts(&pkcs7->signedData);
    if (rc != PKCS7_SUCC) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "Construct signer certificates failed: %d", rc);
        goto exit;
    }
    
    HILOG_INFO(HILOG_MODULE_AAFWK, "PKCS7 parsing completed successfully");
    return rc;

exit:
    HILOG_ERROR(HILOG_MODULE_AAFWK, "PKCS7 parsing failed: %d", rc);
    PKCS7_FreeRes(pkcs7);
    return rc;
}

void PKCS7_FreeRes(Pkcs7 *pkcs7)
{
    if (pkcs7 == NULL) {
        return;
    }
    FreeSignedDataDigestAlgs(pkcs7);
    FreeSignersInfo(pkcs7);
    FreeSignedDataCerts(pkcs7);
    FreeSignedDataCrl(pkcs7);
    UnLoadRootCert();
#ifndef OHOS_SIGN_HAPS_BY_SERVER
    UnLoadSelfSignedCert();
#endif
    FreeCertVerifyCache(&g_certCache);
}