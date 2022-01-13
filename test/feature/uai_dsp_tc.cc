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

#include "testdata/yes_30ms_testdata.h"

#include <uai_dsp.h>
#include <nd_errno.h>
#include <uai_matrix.h>

#include <stdio.h>

#include <atest.h>
#include <os_errno.h>
#include <os_clock.h>
#include <os_stddef.h>
#include <os_memory.h>
#include "numdl.h"

#define REL_ERROR (1.0e-3)

#define CONSTANT_E   (2.7182818284590452354)
#define CONSTANT_E_2 (7.3890560989306502274)
#define CONSTANT_E_3 (20.0855369231876677418)
#define CONSTANT_E_4 (54.5981500331442390813)


namespace uai {
namespace feature {

static os_size_t gs_size = YES_30MS_DATA_SIZE;
static float* gs_output_float = OS_NULL;

static void test_sum(void)
{
    float test_array[10] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0};
    float execpt_sum = 10.5;
    int sum=1000;
    
    sum = cube(sum);
    execpt_sum = cube(execpt_sum);

    float array_sum = dsp::sum(test_array, OS_ARRAY_SIZE(test_array));
    printf("------------------%lf--------\r\n",pi);
    printf("------------------%f--------\r\n",execpt_sum);
    printf("------------------%d--------\r\n",sum);
    printf("--------------------------\r\n");
    tp_assert_in_range(
        array_sum, execpt_sum - REL_ERROR, execpt_sum + REL_ERROR);
}

static void test_mat_dot(void)
{
    float mat1_data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    uai_mat_t mat1 = {
        .rows = 2,
        .cols = 3,
        .data = mat1_data,
    };

    float mat2_data[] = {7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    uai_mat_t mat2 = {
        .rows = 3,
        .cols = 2,
        .data = mat2_data,
    };

    float execpt_data[] = {58.0, 64.0, 139.0, 154.0};

    uai_mat_t* output = uai_mat_create(2, 2);

    int ret = dsp::dot(&mat1, &mat2, output);
    tp_assert_integer_equal(ret, NUMDL_EOK);

    os_bool_t dot_check = OS_TRUE;
    for (os_size_t i = 0; i < output->rows * output->cols; i++) {
        if ((output->data[i] < execpt_data[i] - REL_ERROR) ||
            (output->data[i] > execpt_data[i] + REL_ERROR)) {
            dot_check = OS_FALSE;
            break;
        }
    }
    tp_assert_true(dot_check);

    uai_mat_destroy(output);
}

static void test_linspace_float(void)
{
    float start_f = 0.0;
    float end_f = 1.0;
    os_size_t num = 5;
    float* out_f = (float*)os_calloc(1, sizeof(float) * num);
    float out_except_f[] = {0.0, 0.25, 0.5, 0.75, 1.0};

    int ret = dsp::linspace(start_f, end_f, num, out_f);
    tp_assert_integer_equal(ret, NUMDL_EOK);

    os_bool_t linspace_check = OS_TRUE;
    for (os_size_t i = 0; i < num; i++) {
        if ((out_f[i] < out_except_f[i] - REL_ERROR) ||
            (out_f[i] > out_except_f[i] + REL_ERROR)) {
            linspace_check = OS_FALSE;
            break;
        }
    }
    tp_assert_true(linspace_check);
}

static void test_linspace_i16(void)
{
    os_int16_t start_i16 = 0;
    os_int16_t end_i16 = 10;
    os_size_t num = 6;
    os_int16_t* out_i16 = (os_int16_t*)os_calloc(1, sizeof(os_int16_t) * num);
    os_int16_t out_except_i16[] = {0, 2, 4, 6, 8, 10};

    int ret = dsp::linspace(start_i16, end_i16, num, out_i16);
    tp_assert_integer_equal(ret, NUMDL_EOK);

    os_bool_t linspace_check = OS_TRUE;
    for (os_size_t i = 0; i < num; i++) {
        if (out_i16[i] != out_except_i16[i]) {
            linspace_check = OS_FALSE;
            break;
        }
    }
    tp_assert_true(linspace_check);
}

static void test_linspace_i32(void)
{
    os_int32_t start_i32 = 0;
    os_int32_t end_i32 = 100000;
    os_size_t num = 6;
    os_int32_t* out_i32 = (os_int32_t*)os_calloc(1, sizeof(os_int32_t) * num);
    os_int32_t out_except_i32[] = {0, 20000, 40000, 60000, 80000, 100000};

    int ret = dsp::linspace(start_i32, end_i32, num, out_i32);
    tp_assert_integer_equal(ret, NUMDL_EOK);

    os_bool_t linspace_check = OS_TRUE;
    for (os_size_t i = 0; i < num; i++) {
        if (out_i32[i] != out_except_i32[i]) {
            linspace_check = OS_FALSE;
            break;
        }
    }
    tp_assert_true(linspace_check);
}

static void test_linspace(void)
{
    test_linspace_float();
    test_linspace_i16();
    test_linspace_i32();
}

static void test_int16_to_float(void)
{
    int ret = dsp::int16_to_float(g_yes_30ms_int16, gs_output_float, gs_size);
    tp_assert_integer_equal(ret, NUMDL_EOK);

    os_bool_t convert_check = OS_TRUE;
    for (os_size_t i = 0; i < gs_size; i++) {
        if ((gs_output_float[i] < g_yes_30ms_float[i] - REL_ERROR) ||
            (gs_output_float[i] > g_yes_30ms_float[i] + REL_ERROR)) {
            convert_check = OS_FALSE;
            break;
        }
    }
    tp_assert_true(convert_check);
}

static void test_log_function(void)
{
    float log_input[5] = {
        1.0, CONSTANT_E, CONSTANT_E_2, CONSTANT_E_3, CONSTANT_E_4};

    float log_result[5] = {0.0, 1.0, 2.0, 3.0, 4.0};

    float log_output[5] = {0.0};

    os_bool_t log_check = OS_TRUE;
    for (os_size_t i = 0; i < 5; i++) {
        log_output[i] = dsp::log(log_input[i]);
        if ((log_output[i] < log_result[i] - REL_ERROR) ||
            (log_output[i] > log_result[i] + REL_ERROR)) {
            log_check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   log_output[i],
                   log_result[i]);
            break;
        }
    }
    tp_assert_true(log_check);
}

static void test_log2_funciton(void)
{
    float log2_input[5] = {1.0, 2.0, 4.0, 8.0, 16.0};

    float log2_result[5] = {0.0, 1.0, 2.0, 3.0, 4.0};

    float log2_output[5] = {0.0};

    os_bool_t log_check = OS_TRUE;
    for (os_size_t i = 0; i < 5; i++) {
        log2_output[i] = dsp::log2(log2_input[i]);
        if ((log2_output[i] < log2_result[i] - REL_ERROR) ||
            (log2_output[i] > log2_result[i] + REL_ERROR)) {
            log_check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   log2_output[i],
                   log2_result[i]);
            break;
        }
    }
    tp_assert_true(log_check);
}

static void test_log10_function(void)
{
    float log10_input[5] = {1.0, 10.0, 100.0, 1000.0, 10000.0};

    float log10_result[5] = {0.0, 1.0, 2.0, 3.0, 4.0};

    float log10_output[5] = {0.0};

    os_bool_t log_check = OS_TRUE;
    for (os_size_t i = 0; i < 5; i++) {
        log10_output[i] = dsp::log10(log10_input[i]);
        if ((log10_output[i] < log10_result[i] - REL_ERROR) ||
            (log10_output[i] > log10_result[i] + REL_ERROR)) {
            log_check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   log10_output[i],
                   log10_result[i]);
            break;
        }
    }
    tp_assert_true(log_check);
}

static void test_log_mat_function(void)
{
    float log_input[5] = {
        1.0, CONSTANT_E, CONSTANT_E_2, CONSTANT_E_3, CONSTANT_E_4};
    float log_result[5] = {0.0, 1.0, 2.0, 3.0, 4.0};

    uai_mat_t test = {.rows = 1, .cols = 5, .data = log_input};
    uai_mat_t result = {.rows = 1, .cols = 5, .data = log_result};

    int ret = dsp::log(&test);
    tp_assert_integer_equal(ret, NUMDL_EOK);

    os_bool_t log_check = OS_TRUE;
    for (os_size_t i = 0; i < test.cols * test.rows; i++) {
        if ((test.data[i] < result.data[i] - REL_ERROR) ||
            (test.data[i] > result.data[i] + REL_ERROR)) {
            log_check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   test.data[i],
                   result.data[i]);
            break;
        }
    }
    tp_assert_true(log_check);
}

static void test_rfft_function(void)
{
    os_size_t fft_len = 256;
    os_size_t out_len = fft_len / 2 + 1;

    float* out = (float*)os_calloc(1, sizeof(float) * out_len);

    int ret = dsp::rfft(g_yes_30ms_preemphasis, gs_size, out, out_len, fft_len);
    tp_assert_integer_equal(ret, NUMDL_EOK);

    os_bool_t rfft_check = OS_TRUE;
    for (os_size_t i = 0; i < out_len; i++) {
        if ((out[i] < g_yes_30ms_fft[i] - REL_ERROR) ||
            (out[i] > g_yes_30ms_fft[i] + REL_ERROR)) {
            rfft_check = OS_FALSE;
            printf("rfft output %ld value is %f, expected value is %f\r\n",
                   i,
                   out[i],
                   g_yes_30ms_fft[i]);
            // break;
        }
    }
    tp_assert_true(rfft_check);

    os_free(out);
}

static void test_case(void)
{
    ATEST_UNIT_RUN(test_sum);
#if 0
    ATEST_UNIT_RUN(test_mat_dot);
    ATEST_UNIT_RUN(test_int16_to_float);
    ATEST_UNIT_RUN(test_log_function);
    ATEST_UNIT_RUN(test_log2_funciton);
    ATEST_UNIT_RUN(test_log10_function);
    ATEST_UNIT_RUN(test_log_mat_function);
    ATEST_UNIT_RUN(test_linspace);
    ATEST_UNIT_RUN(test_rfft_function);
#endif
    return;
}

static os_err_t test_init(void)
{
    gs_output_float = (float*)os_calloc(1, sizeof(float) * gs_size);
    if (OS_NULL == gs_output_float) {
        return OS_ENOMEM;
    }

    return OS_EOK;
}

static os_err_t test_cleanup(void)
{
    if (gs_output_float != OS_NULL) {
        os_free(gs_output_float);
    }

    return OS_EOK;
}

ATEST_TC_EXPORT(uai_sdk.feature.dsp.tc,
                test_case,
                test_init,
                test_cleanup,
                TC_PRIORITY_MIDDLE);

}  // namespace feature
}  // namespace uai