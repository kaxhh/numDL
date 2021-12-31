/**
 *******************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *use this file except in compliance with the License. You may obtain a copy of
 *the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 *distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *License for the specific language governing permissions and limitations under
 *the License.
 *
 * @file        mo_wifi_tc.c
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#include <uai_functions.h>

#include <stdio.h>
#include <float.h>

#include <atest.h>
#include <os_errno.h>
#include <os_clock.h>
#include <os_stddef.h>
#include <os_memory.h>

#define REL_ERROR (1.0e-2)

namespace uai {
namespace feature {

static const float gs_freq_input[] = {20.0,
                                      160.0,
                                      394.0,
                                      670.0,
                                      1000.0,
                                      1420.0,
                                      1900.0,
                                      2450.0,
                                      3120.0,
                                      4000.0,
                                      5100.0,
                                      6600.0,
                                      9000.0,
                                      14000.0};

static const float gs_mel_expect[] = {31.748644,
                                      231.99524,
                                      503.22318,
                                      756.76434,
                                      999.9907,
                                      1248.8186,
                                      1478.834,
                                      1695.0952,
                                      1912.4349,
                                      2146.0757,
                                      2383.0786,
                                      2642.3071,
                                      2962.6587,
                                      3431.177};

static const float gs_mel_input[] = {0.0,
                                     250.0,
                                     500.0,
                                     750.0,
                                     1000.0,
                                     1250.0,
                                     1500.0,
                                     1750.0,
                                     2000.0,
                                     2250.0,
                                     2500.0,
                                     2750.0,
                                     3000.0,
                                     3250.0};

static const float gs_freq_expect[] = {0.0,
                                       173.84952,
                                       390.8756,
                                       661.80176,
                                       1000.01404,
                                       1422.2236,
                                       1949.2916,
                                       2607.2603,
                                       3428.6396,
                                       4454.0137,
                                       5734.0464,
                                       7331.983,
                                       9326.779,
                                       11816.993};

static void test_mel_to_frequency(void)
{
    float freq_output[OS_ARRAY_SIZE(gs_mel_input)] = {0.0};

    for (int i = 0; i < OS_ARRAY_SIZE(gs_mel_input); i++) {
        freq_output[i] = functions::mel_to_frequency(gs_mel_input[i]);
    }

    os_bool_t check = OS_TRUE;
    for (os_size_t i = 0; i < OS_ARRAY_SIZE(gs_mel_input); i++) {
        if ((freq_output[i] < gs_freq_expect[i] - REL_ERROR) ||
            (freq_output[i] > gs_freq_expect[i] + REL_ERROR)) {
            check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   freq_output[i],
                   gs_freq_expect[i]);
            break;
        }
    }
    tp_assert_true(check);
}

static void test_frequency_to_mel(void)
{
    float mel_output[OS_ARRAY_SIZE(gs_freq_input)] = {0.0};

    for (int i = 0; i < OS_ARRAY_SIZE(gs_freq_input); i++) {
        mel_output[i] = functions::frequency_to_mel(gs_freq_input[i]);
    }

    os_bool_t check = OS_TRUE;
    for (os_size_t i = 0; i < OS_ARRAY_SIZE(gs_mel_input); i++) {
        if ((mel_output[i] < gs_mel_expect[i] - REL_ERROR) ||
            (mel_output[i] > gs_mel_expect[i] + REL_ERROR)) {
            check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   mel_output[i],
                   gs_mel_expect[i]);
            break;
        }
    }
    tp_assert_true(check);
}

static void test_zero_handle(void)
{
    float test_data[] = {0.1, 0.01, 0.0, 0.01, 0.1, 0.1};
    float except_data[] = {0.1, 0.01, FLT_EPSILON, 0.01, 0.1, 0.1};

    functions::zero_handling(test_data, OS_ARRAY_SIZE(test_data));

    os_bool_t check = OS_TRUE;
    for (os_size_t i = 0; i < OS_ARRAY_SIZE(test_data); i++) {
        if ((test_data[i] < except_data[i] - REL_ERROR) ||
            (test_data[i] > except_data[i] + REL_ERROR)) {
            check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   test_data[i],
                   except_data[i]);
            break;
        }
    }
    tp_assert_true(check);

    test_data[2] = 0.0;
    uai_mat_t mat1 = {.rows = 3, .cols = 2, .data = test_data};

    functions::zero_handling(&mat1);

    check = OS_TRUE;
    for (os_size_t i = 0; i < OS_ARRAY_SIZE(test_data); i++) {
        if ((mat1.data[i] < except_data[i] - REL_ERROR) ||
            (mat1.data[i] > except_data[i] + REL_ERROR)) {
            check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   mat1.data[i],
                   except_data[i]);
            break;
        }
    }
    tp_assert_true(check);
}

static void test_case(void)
{
    ATEST_UNIT_RUN(test_mel_to_frequency);
    ATEST_UNIT_RUN(test_frequency_to_mel);
    ATEST_UNIT_RUN(test_zero_handle);

    return;
}

static os_err_t test_init(void)
{
    return OS_EOK;
}

static os_err_t test_cleanup(void)
{
    return OS_EOK;
}

ATEST_TC_EXPORT(uai_sdk.feature.functions.tc,
                test_case,
                test_init,
                test_cleanup,
                TC_PRIORITY_MIDDLE);

}  // namespace feature
}  // namespace uai