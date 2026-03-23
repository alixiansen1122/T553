/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Application core board init function for standard
 * Author:
 * Create:
 */

#include "stdbool.h"
#include "esim_lpa.h"
#include "esim_bind.h"

__attribute__((weak)) char* api_version(void)
{
    return "ap_stub";
}
__attribute__((weak)) int api_esimProductEdition(char *productEdition, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_getEid(char *eid, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_getEuicc(struct API_EuiccInfo *euiccInfo, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_seInterfaceInit(ApduTransmitRecv apduTransmitRecv, WaitForReady waitForReady)
{
    return -1;
}
__attribute__((weak)) int api_httpInterfaceInit(HttpPostReq httpPostReq)
{
    return -1;
}
__attribute__((weak)) int api_setupIMEI(char *imei)
{
    return -1;
}
__attribute__((weak)) int api_matedataRegisterCallback(MetadataCallback metadataCallback)
{
    return -1;
}
__attribute__((weak)) int api_getProfileClassList(struct API_Profile pList[], int pageSize, int pageNum, int *count, int profileClass, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_getProfileList(struct API_Profile pList[], int pageSize, int pageNum, int *count, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_getProfile(struct API_Profile *profile, char *ICCID, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_editProfile(char *ICCID, char* NickName, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_deleteProfile(char *ICCID, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_enableProfile(char *ICCID, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_enableProfileFlag(char *ICCID, int refreshflag, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_disableProfile(char *ICCID, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_disableProfileFlag(char *ICCID, int refreshflag, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_rerunNotification(char *msg)
{
    return -1;
}
__attribute__((weak)) int api_profileDownloadAC(char *AC, char *confirmCode, char *msg)
{
    return -1;
}
__attribute__((weak)) int api_memoryReset(char *msg)
{
    return -1;
}
__attribute__((weak)) int WritePPK(int logic, char* instruction, char* n, char* d, char* e, char* errtext)
{
    return -1;
}
__attribute__((weak)) int BindInit(int logic, char* instruction, char* errtext)
{
    return -1;
}
__attribute__((weak)) int Bind(int logic, char* cardECCPubkeyEnc, char* IMEI, char* n, char* d, char* e, char* instruction,
    char* shardkey, char* errtext)
{
    return -1;
}
__attribute__((weak)) int BindResponse(char* cardResponseText, char* shardkey, char* eid, char* errtext)
{
    return -1;
}
__attribute__((weak)) int VerifyInit(int logic, char* instruction, char* errtext)
{
    return -1;
}
__attribute__((weak)) int Verify(int logic, char* cardEncText, char* IMEI, char* BindeID, char* shardkey, char* instruction, char* errtext)
{
    return -1;
}
