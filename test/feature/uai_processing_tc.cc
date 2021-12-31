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

#include <uai_errno.h>
#include <uai_processing.h>

#include <stdio.h>
#include <string.h>

#include <atest.h>
#include <os_assert.h>
#include <os_errno.h>
#include <os_clock.h>
#include <os_stddef.h>
#include <os_memory.h>

#define REL_ERROR   (1.0e-3)
#define COEFFICIENT (0.98)

namespace uai {
namespace feature {

static os_size_t gs_size = YES_30MS_DATA_SIZE;
static float *gs_output_float = OS_NULL;

static void test_pre_emphasise_function(void)
{
    memcpy(gs_output_float, g_yes_30ms_float, sizeof(float) * gs_size);

    int ret = processing::pre_emphasise(gs_output_float, gs_size, COEFFICIENT);
    tp_assert_integer_equal(ret, UAI_EOK);

    os_bool_t emphasise_check = OS_TRUE;
    for (os_size_t i = 0; i < gs_size; i++) {
        if ((gs_output_float[i] < g_yes_30ms_preemphasis[i] - REL_ERROR) ||
            (gs_output_float[i] > g_yes_30ms_preemphasis[i] + REL_ERROR)) {
            emphasise_check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   gs_output_float[i],
                   g_yes_30ms_preemphasis[i]);
            break;
        }
    }
    tp_assert_true(emphasise_check);
}

static int test_get_data(os_size_t offset, os_size_t size, float *out_buff)
{
    OS_ASSERT(out_buff != OS_NULL);

    memcpy(out_buff, g_yes_30ms_float + offset, sizeof(float) * size);
    return size;
}

static void test_pre_emphasise_class(void)
{
    uai_signal_t signal = {0};

    signal.total_length = gs_size;
    signal.get_data = &test_get_data;

    memset(gs_output_float, 0, sizeof(float) * gs_size);

    preemphasise pre_emphasise(&signal, COEFFICIENT);

    os_size_t frame_length = 10;
    for (os_size_t offset = 0; offset <= gs_size - frame_length;
         offset += frame_length) {
        pre_emphasise.get_data(offset, frame_length, &gs_output_float[offset]);
    }

    os_bool_t emphasise_check = OS_TRUE;
    for (os_size_t i = 0; i < gs_size; i++) {
        if ((gs_output_float[i] < g_yes_30ms_preemphasis[i] - REL_ERROR) ||
            (gs_output_float[i] > g_yes_30ms_preemphasis[i] + REL_ERROR)) {
            emphasise_check = OS_FALSE;
            printf("output %ld value is %f, expected value is %f\r\n",
                   i,
                   gs_output_float[i],
                   g_yes_30ms_preemphasis[i]);
            break;
        }
    }
    tp_assert_true(emphasise_check);
}

static void do_test_stack_frame(os_uint32_t test_freq,
                                os_size_t test_signal_length,
                                float test_frame_length_s,
                                float test_frame_stride_s,
                                os_bool_t test_zero_padding,
                                os_size_t expect_frame_length,
                                os_size_t expect_frame_stride,
                                os_size_t expect_frame_nums,
                                os_size_t expect_end_offset)
{
    frames_info_t test_frames_info = {0};
    uai_signal_t test_signal = {0};
    test_signal.total_length = test_signal_length;

    int ret = processing::stack_frames(&test_frames_info,
                                       &test_signal,
                                       test_freq,
                                       test_frame_length_s,
                                       test_frame_stride_s,
                                       test_zero_padding);

    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(test_frames_info.frame_length, expect_frame_length);
    tp_assert_integer_equal(test_frames_info.frame_stride, expect_frame_stride);
    tp_assert_integer_equal(test_frames_info.frame_nums, expect_frame_nums);
    tp_assert_integer_equal(test_frames_info.end_offset, expect_end_offset);

    os_bool_t frames_check = OS_TRUE;
    os_size_t last_offset = test_frames_info.frame_offsets[0];
    os_size_t i = 1;
    for (i = 1; i < test_frames_info.frame_nums; i++) {
        os_size_t curr_offset = test_frames_info.frame_offsets[i];
        if (curr_offset - last_offset != expect_frame_stride) {
            frames_check = OS_FALSE;
            printf("frame %ld offset is %ld, expect offset %ld\r\n",
                   i,
                   curr_offset,
                   last_offset + expect_frame_stride);
            break;
        }

        last_offset = curr_offset;
    }

    tp_assert_true(frames_check);
    tp_assert_integer_equal(
        test_frames_info.frame_offsets[test_frames_info.frame_nums - 1] +
            expect_frame_length,
        expect_end_offset);

    processing::stack_frames_info_free(&test_frames_info);
}

static void test_stack_frame(void)
{
    /* clang-format off */
    do_test_stack_frame(16000, 16000, 0.02, 0.02, OS_FALSE, 320, 320, 50, 16000);
    do_test_stack_frame(16000, 16000, 0.02, 0.02, OS_TRUE, 320, 320, 50, 16000);
    do_test_stack_frame(16000, 16000, 0.02, 0.01, OS_FALSE, 320, 160, 99, 16000);
    do_test_stack_frame(16000, 16000, 0.02, 0.01, OS_TRUE, 320, 160, 99, 16000);
    do_test_stack_frame(16000, 16000, 0.03, 0.01, OS_FALSE, 480, 160, 98, 16000);
    do_test_stack_frame(16000, 16000, 0.03, 0.01, OS_TRUE, 480, 160, 98, 16000);
    do_test_stack_frame(16000, 16000, 0.03, 0.02, OS_FALSE, 480, 320, 49, 15840);
    do_test_stack_frame(16000, 16000, 0.03, 0.02, OS_TRUE, 480, 320, 50, 16160);
    /* clang-format on */
}

static void test_power_spectrum(void)
{
    os_size_t fft_len = 256;
    os_size_t out_len = fft_len / 2 + 1;

    float *out = (float *)os_calloc(1, sizeof(float) * out_len);

    int ret = processing::power_spectrum(
        g_yes_30ms_preemphasis, gs_size, out, out_len, fft_len);
    tp_assert_integer_equal(ret, UAI_EOK);

    os_bool_t power_spectrum_check = OS_TRUE;
    for (os_size_t i = 0; i < out_len; i++) {
        if ((out[i] < g_yes_30ms_power_fft[i] - REL_ERROR) ||
            (out[i] > g_yes_30ms_power_fft[i] + REL_ERROR)) {
            power_spectrum_check = OS_FALSE;
            break;
        }
    }
    tp_assert_true(power_spectrum_check);

    os_free(out);
}

static void test_case(void)
{
    ATEST_UNIT_RUN(test_pre_emphasise_function);
    ATEST_UNIT_RUN(test_pre_emphasise_class);
    ATEST_UNIT_RUN(test_stack_frame);
    ATEST_UNIT_RUN(test_power_spectrum);
    return;
}

static os_err_t test_init(void)
{
    gs_output_float = (float *)os_calloc(1, sizeof(float) * gs_size);
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

ATEST_TC_EXPORT(uai_sdk.feature.processing.tc,
                test_case,
                test_init,
                test_cleanup,
                TC_PRIORITY_MIDDLE);

}  // namespace feature
}  // namespace uai