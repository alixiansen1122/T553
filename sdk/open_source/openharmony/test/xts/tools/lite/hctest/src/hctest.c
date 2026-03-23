/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ohos_init.h>
#include <stdlib.h>
#include <securec.h>
#include <unistd.h>

#include "hctest.h"
#include "parameter.h"
#include "samgr_lite.h"

#ifdef INTER_ATTEST_MINI_MODULE
#include "devattest_interface.h"

#define DEVATTEST_SUCCESS 0
#endif

#define UDIDSIZE_LEN 64

void setUp(void) {}
void tearDown(void) {}
void suiteSetUp(void) { }
int suiteTearDown(int num_failures) { return num_failures; }


static TestSuiteManager g_testSuiteManager;
static BOOL CompareInputType(const char *source, const char *input);
static void RunSingleTestCase(CTestCase* cTestCase,
                              const char *caseName, const int32 flag);
static int16 g_totalSuitesNum = 0;
static int16 g_doneSuitesNum = 0;
static void RunSingleTestSuite(CTestSuite* testSuite)
{
    if (testSuite == NULL) {
        return;
    }
    int16 size = VECTOR_Size(&(testSuite->test_cases));
    if (size < 0) {
        return;
    }
    UnityBegin(testSuite->file);
    int16 i;
    CTestCase *testCase = (CTestCase *)(VECTOR_At(&(testSuite->test_cases), 0));
    // when setup failed， skip test suites
    if (testCase == NULL || !testCase->lite_setup()) {
        printf("Setup failed, skip this test suite!\n");
        UnityEnd();
        return;
    }
    for (i = size - 1; i >= 0; i--) {
        CTestCase *hcCase = (CTestCase *)(VECTOR_At(&(testSuite->test_cases), i));
        if (hcCase != NULL) {
            RunSingleTestCase(hcCase, hcCase->case_name, hcCase->flag);
        }
    }
    testCase->lite_teardown();
    UnityEnd();
}

static CTestSuite* GetTestSuite(const char *test_suite)
{
    CTestSuite* suite = NULL;
    TestSuiteManager* testMgr = GetTestMgrInstance();
    if (testMgr == NULL || test_suite == NULL) {
        return suite;
    }
    int16 size = VECTOR_Size(&(testMgr->test_suites));
    int16 i;
    for (i = 0; i < size; i++) {
        CTestSuite* curSuite = (CTestSuite *)(VECTOR_At(&(testMgr->test_suites), i));
        if (strcmp(curSuite->suite_name, test_suite) == 0) {
            suite = curSuite;
            break;
        }
    }

    return suite;
}

static BOOL RegisterTestSuite(CTestSuite *testSuite)
{
    VECTOR_Add(&(g_testSuiteManager.test_suites), testSuite);
    g_totalSuitesNum++;
    return TRUE;
}

static BOOL RemoveTestSuite(CTestSuite *testSuite)
{
    VECTOR_Swap(&(g_testSuiteManager.test_suites),
                VECTOR_Find(&(g_testSuiteManager.test_suites), testSuite),
                testSuite);
    return TRUE;
}

static void AddTestCase(CTestCase *testCase)
{
    if (testCase == NULL) {
        return;
    }
    CTestSuite* suite = GetTestSuite(testCase->suite_name);
    if (suite == NULL) {
        CTestSuite suite_object;
        suite_object.subsystem_name = NULL;
        suite_object.module_name = NULL;
        suite_object.suite_name = testCase->suite_name;
        suite_object.test_cases = VECTOR_Make(NULL, NULL);
        suite = &suite_object;
        VECTOR_Add(&(g_testSuiteManager.test_suites), suite);
    }
    VECTOR_Add(&(suite->test_cases), testCase);
    return;
}

static BOOL CompareInputType(const char *source, const char *input)
{
    if (strcmp(input, CONST_STRING_SPACE) != 0 && strcmp(input, source) != 0) {
        return TRUE;
    }
    return FALSE;
}

static BOOL g_isBreak = FALSE;
static void RunSingleTestCase(CTestCase* cTestCase,
                              const char *caseName, const int32 flag)
{
    if (cTestCase != NULL) {
        if (CompareInputType(cTestCase->case_name, caseName)
            || (cTestCase->flag != flag)) {
            g_isBreak = TRUE;
            return;
        }
        UnityDefaultTestRun(cTestCase->execute_func, cTestCase->case_name, cTestCase->line_num);
    }
}

static void RunSpecialTestSuite(const char *subSystemName,
                                const char *moduleName,
                                const char *suiteName,
                                const char *caseName,
                                int caseLevel)
{
    int16 i;
    int16 j;
    int16 size = VECTOR_Size(&(g_testSuiteManager.test_suites));
    UNITY_BEGIN();
    for (i = 0; i < size; i++) {
        if (g_isBreak) {
            break;
        }
        CTestSuite* curSuite = (CTestSuite *)(VECTOR_At(&(g_testSuiteManager.test_suites), i));
        if (curSuite != NULL) {
            if (CompareInputType(curSuite->subsystem_name, subSystemName)
                || CompareInputType(curSuite->module_name, moduleName)
                || CompareInputType(curSuite->suite_name, suiteName)) {
                continue;
            }
            for (j = 0; j < VECTOR_Size(&(curSuite->test_cases)); j++) {
                CTestCase* cTestCase = (CTestCase *)(VECTOR_At(
                    &(curSuite->test_cases), j));
                RunSingleTestCase(cTestCase, caseName, caseLevel);
            }
        }
    }
    UNITY_END();
}

static void RunTestSuite(const char* suite_name)
{
    printf("Start to run test suite:%s\n", suite_name);
    CTestSuite* curSuite = GetTestSuite(suite_name);
    if (curSuite != NULL) {
        g_doneSuitesNum++;
        int16 times = curSuite->times;
        int16 i;
        for (i = 0; i < times; i++) {
            sleep(1);
            printf("Run test suite %d times\n", i+1);
            RunSingleTestSuite(curSuite);
        }
        if (g_totalSuitesNum == g_doneSuitesNum) {
            printf("All the test suites finished!\n");
        }
    }
}

void LiteTestPrint(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
}

void ObtainProductParams(void)
{
    int sdkApiVersion = GetSdkApiVersion();
    if (sdkApiVersion != 0) {
        printf("SdkApiVersion = %d\n", sdkApiVersion);
    }

    int firstApiVersion = GetFirstApiVersion();
    if (firstApiVersion != 0) {
        printf("firstApiVersion = %d\n", firstApiVersion);
    }

    const char *bootloaderVersion = GetBootloaderVersion();
    if (bootloaderVersion != NULL) {
        printf("bootloaderVersion = %s\n", bootloaderVersion);
    }

    const char *incrementalVersion = GetIncrementalVersion();
    if (incrementalVersion != NULL) {
        printf("incrementalVersion = %s\n", incrementalVersion);
    }

    const char *buildType = GetBuildType();
    if (buildType != NULL) {
        printf("buildType = %s\n", buildType);
    }

    const char *buildUser = GetBuildUser();
    if (buildUser != NULL) {
        printf("buildUser = %s\n", buildUser);
    }

    const char *buildHost = GetBuildHost();
    if (buildHost != NULL) {
        printf("buildHost = %s\n", buildHost);
    }

    const char *buildTime = GetBuildTime();
    if (buildTime != NULL) {
        printf("buildTime = %s\n", buildTime);
    }

    const char *abiList = GetAbiList();
    if (abiList != NULL) {
        printf("AbiList = %s\n", abiList);
    }
    
}

#ifdef INTER_ATTEST_MINI_MODULE
void ObtainAttestResultParams(void)
{
    AttestResultInfo attestResultInfo = { 0 };
    attestResultInfo.ticket = NULL;
    int32_t retStatus = GetAttestStatus(&attestResultInfo);
    if (retStatus != DEVATTEST_SUCCESS) {
        printf("[CLIENT MAIN] wrong. retStatus:%d\n", retStatus);
    }
    printf("authResult = %d\n",attestResultInfo.authResult);
    printf("softwareResult = %d\n",attestResultInfo.softwareResult);
}
#endif

void ObtainSystemParams(void)
{
    printf("******To Obtain Product Params Start******\n");
    const char *productType = GetDeviceType();
    if (productType != NULL) {
        printf("Device Type = %s\n", productType);
    }

    const char *securityPatchTag = GetSecurityPatchTag();
    if (securityPatchTag != NULL) {
        printf("Security Patch = %s\n", securityPatchTag);
    }

    const char *osName = GetOSFullName();
    if (osName != NULL) {
        printf("OsFullName = %s\n", osName);
    }

    const char *displayVersion = GetDisplayVersion();
    if (displayVersion != NULL) {
        printf("DisplayVersion = %s\n", displayVersion);
    }

    const char *versionId = GetVersionId();
    if (versionId != NULL) {
        printf("VersionID = %s\n", versionId);
    }

#ifdef INTER_ATTEST_MINI_MODULE
    ObtainAttestResultParams();
#endif

    char udid[UDIDSIZE_LEN + 1] = { 0 };
    int retUdid = GetDevUdid(udid, UDIDSIZE_LEN + 1);
    if (retUdid == 0) {
        printf("DevUdid = %s\n", udid);
    }

    const char *manuFacture = GetManufacture();
    if (manuFacture != NULL) {
        printf("manuFacture = %s\n", manuFacture);
    }

    const char *productModel = GetProductModel();
    if (productModel != NULL) {
        printf("productModel = %s\n", productModel);
    }

    const char *serial = GetSerial();
    if (serial != NULL) {
        printf("serial = %s\n", serial);
    }

    const char *brand = GetBrand();
    if (brand != NULL) {
        printf("brand = %s\n", brand);
    }

    const char *productSeries = GetProductSeries();
    if (productSeries != NULL) {
        printf("productSeries = %s\n", productSeries);
    }

    const char *softwareModel = GetSoftwareModel();
    if (softwareModel != NULL) {
        printf("softwareModel = %s\n", softwareModel);
    }

    const char *hardWareModel = GetHardwareModel();
    if (hardWareModel != NULL) {
        printf("HardwareModel = %s\n", hardWareModel);
    }

    const char *buildRootHash = GetBuildRootHash();
    if (buildRootHash != NULL) {
        printf("BuildRootHash = %s\n", buildRootHash);
    }

    const char *marketName = GetMarketName();
    if (marketName != NULL) {
        printf("marketName = %s\n", marketName);
    }

    ObtainProductParams();
    
    printf("******To Obtain Product Params End  ******\n");
    return;
}

static void InitTestSuiteMgr(void)
{
    g_testSuiteManager.test_suites = VECTOR_Make(NULL, NULL);
    g_testSuiteManager.GetTestSuite = GetTestSuite;
    g_testSuiteManager.RegisterTestSuite = RegisterTestSuite;
    g_testSuiteManager.AddTestCase = AddTestCase;
    g_testSuiteManager.RemoveTestSuite = RemoveTestSuite;
    g_testSuiteManager.RunSpecialTestSuite = RunSpecialTestSuite;
    g_testSuiteManager.RunTestSuite = RunTestSuite;
    printf("[%10s] HCTest Framework inited.\n",  "HCtest Service");
    ObtainSystemParams();
}
CORE_INIT(InitTestSuiteMgr);
TestSuiteManager* GetTestMgrInstance(void)
{
    return &g_testSuiteManager;
}
