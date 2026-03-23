/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Application core board init function for standard
 * Author:
 * Create:
 */
#ifndef _CARDBIND_H_
#define _CARDBIND_H_
#ifdef __cplusplus
extern "C"{
#endif
int Select(int logic, char* instruction, char* errtext);
int WritePPK(int logic, char* instruction, char* n, char* d, char* e, char* errtext);
int BindInit(int logic, char* instruction, char* errtext);
int Bind(int logic, char* cardECCPubkeyEnc, char* IMEI, char* n, char* d, char* e, char* instruction,
    char* shardkey, char* errtext);
int BindResponse(char* cardResponseText, char* shardkey, char* eid, char* errtext);
int VerifyInit(int logic, char* instruction, char* errtext);
int Verify(int logic, char* cardEncText, char* IMEI, char* BindeID, char* shardkey, char* instruction, char* errtext);
#ifdef __cplusplus
}
#endif
#endif