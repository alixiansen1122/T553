/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include "yunit.h"
#include "iotsec.h"

extern void yunit_test_init(void);
extern void yunit_test_api(void);

void yunit_test_main(int count, char **name)
{
    int item_count = count;
    yunit_test_init();
    yunit_test_api();
    int ret = 0;

    if (item_count > 1) {
        int i;

        for (i = 1; i < item_count; i++) {
            yunit_test_suite_t *test_suite = yunit_get_test_suite(name[i]);

            if (test_suite != NULL) {
                ret = yunit_run_test_suite(test_suite);
                MyPrintf("suite %s completed with %d\n", name[i], ret);

                continue;
            }
	    else {
                MyPrintf("suite name err\n");
		return;
	    }

            const char *suite_case = name[i];
            char *test = strrchr(suite_case, ':');

            if (test != NULL) {
                *test++ = '\0';

                test_suite = yunit_get_test_suite(suite_case);

                if (test_suite != NULL) {
                    yunit_test_case_t *test_case = yunit_get_test_case(test_suite, test);

                    if (test_case != NULL) {
                        ret = yunit_run_test_case(test_suite, test_case);
                        MyPrintf("suite %s completed with %d\n", name[i], ret);
                    } else {
                        MyPrintf("test case %s not found\n", test);
                    }
                } else {
                    MyPrintf("suite %s not found\n", suite_case);
                }
            }
        }
    } else {
        ret = yunit_test_run();
        MyPrintf("\nTests completed with return value %d\n", ret);
    }

    yunit_test_print_result();

    yunit_test_deinit();
}

void yunit_sdk_test(void)
{
    char *name[15] = {"ALIPAY_HAL"};
    yunit_test_main(1, name);
}
