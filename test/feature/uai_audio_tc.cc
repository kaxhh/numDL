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
 * @file        uai_audio_tc.c
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */
#include <uai_audio.h>
#include <uai_dsp.h>

#include <stdio.h>
#include <float.h>

#include "testdata/filterbanks_data.h"
#include "testdata/yes_1000ms_testdata.h"

#include <atest.h>
#include <os_assert.h>
#include <os_errno.h>
#include <os_clock.h>
#include <os_stddef.h>
#include <os_memory.h>

#define REL_ERROR (1.0e-3)

namespace uai {
namespace feature {

static void test_filterbanks(void)
{
    os_uint16_t num_filter = 32;
    os_uint16_t num_fft = 256;
    os_uint32_t sampling_freq = 16000;
    os_uint32_t low_freq = 0;
    os_uint32_t high_freq = 8000;

    os_uint16_t coefficients = num_fft / 2 + 1;
    uai_mat_t* filterbanks = uai_mat_create(num_filter, coefficients);
    tp_assert_not_null(filterbanks);

    int ret = audio_feature::filterbanks(filterbanks,
                                         num_filter,
                                         num_fft,
                                         sampling_freq,
                                         low_freq,
                                         high_freq,
                                         OS_FALSE);
    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(num_filter, filterbanks->rows);
    tp_assert_integer_equal(coefficients, filterbanks->cols);

    os_bool_t filterbanks_check = OS_TRUE;
    for (int i = 0; i < num_filter * coefficients; i++) {
        if ((filterbanks->data[i] < g_filterbank_data[i] - REL_ERROR) ||
            (filterbanks->data[i] > g_filterbank_data[i] + REL_ERROR)) {
            filterbanks_check = OS_FALSE;
            printf("filterbanks->data[%d]:%f g_filterbank_data[%d]:%f",
                   i,
                   filterbanks->data[i],
                   i,
                   g_filterbank_data[i]);
            break;
        }
    }
    tp_assert_true(filterbanks_check);

    uai_mat_destroy(filterbanks);

    uai_mat_t* filterbanks_T = uai_mat_create(num_filter, coefficients);
    tp_assert_not_null(filterbanks);

    ret = audio_feature::filterbanks(filterbanks,
                                     num_filter,
                                     num_fft,
                                     sampling_freq,
                                     low_freq,
                                     high_freq,
                                     OS_TRUE);
    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(coefficients, filterbanks->rows);
    tp_assert_integer_equal(num_filter, filterbanks->cols);

    filterbanks_check = OS_TRUE;
    for (int i = 0; i < num_filter * coefficients; i++) {
        if ((filterbanks->data[i] < g_filterbank_data_T[i] - REL_ERROR) ||
            (filterbanks->data[i] > g_filterbank_data_T[i] + REL_ERROR)) {
            filterbanks_check = OS_FALSE;
            printf("filterbanks->data[%d]:%f g_filterbank_data[%d]:%f",
                   i,
                   filterbanks->data[i],
                   i,
                   g_filterbank_data_T[i]);
            break;
        }
    }
    tp_assert_true(filterbanks_check);

    uai_mat_destroy(filterbanks_T);
}

static void test_calc_mfe_buff_size(void)
{
    uai_mat_size_t mfe_mat_size = {0};

    os_size_t signal_length = 16000;
    os_uint32_t sampling_freq = 16000;
    float frame_length_s = 0.02;
    float frame_stride_s = 0.02;
    os_uint16_t num_filter = 32;

    int ret = audio_feature::calc_mfe_buff_size(&mfe_mat_size,
                                                signal_length,
                                                sampling_freq,
                                                frame_length_s,
                                                frame_stride_s,
                                                num_filter);
    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(mfe_mat_size.cols, num_filter);
    tp_assert_integer_equal(mfe_mat_size.rows, 50);

    frame_stride_s = 0.01;
    num_filter = 20;

    ret = audio_feature::calc_mfe_buff_size(&mfe_mat_size,
                                            signal_length,
                                            sampling_freq,
                                            frame_length_s,
                                            frame_stride_s,
                                            num_filter);
    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(mfe_mat_size.cols, num_filter);
    tp_assert_integer_equal(mfe_mat_size.rows, 99);
}

static int get_yes_1000ms_data(os_size_t offset, os_size_t size, float* out)
{
    OS_ASSERT(out != OS_NULL);

    int ret = dsp::int16_to_float(&g_yes_1000ms_int16[offset], out, size);

    if (ret != UAI_EOK) {
        return UAI_ERROR;
    }

    return size;
}

static void test_mfe(void)
{
    os_size_t signal_length = 16000;
    os_uint32_t sampling_freq = 16000;
    float frame_length_s = 0.02;
    float frame_stride_s = 0.02;
    os_uint16_t num_filter = 32;
    os_uint16_t num_fft = 256;
    os_uint32_t low_freq = 0;
    os_uint32_t high_freq = 8000;
    float preemph_cof = 0.98;

    uai_mat_size_t mfe_mat_size = {0};
    int ret = audio_feature::calc_mfe_buff_size(&mfe_mat_size,
                                                signal_length,
                                                sampling_freq,
                                                frame_length_s,
                                                frame_stride_s,
                                                num_filter);
    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(mfe_mat_size.rows, 50);
    tp_assert_integer_equal(mfe_mat_size.cols, 32);

    uai_mat_t* feature = uai_mat_create(mfe_mat_size.rows, mfe_mat_size.cols);
    tp_assert_not_null(feature);
    uai_mat_t* energies = uai_mat_create(mfe_mat_size.rows, 1);
    tp_assert_not_null(energies);

    uai_signal_t signal = {0};
    signal.total_length = signal_length;
    signal.get_data = get_yes_1000ms_data;

    os_tick_t start = os_tick_get();

    ret = audio_feature::mfe(feature,
                             energies,
                             &signal,
                             sampling_freq,
                             frame_length_s,
                             frame_stride_s,
                             num_filter,
                             num_fft,
                             low_freq,
                             high_freq,
                             preemph_cof);

    os_tick_t end = os_tick_get();

    printf("mfe cost:%lums\r\n", end - start);

    tp_assert_integer_equal(ret, UAI_EOK);

    os_bool_t energies_check = OS_TRUE;
    for (os_size_t i = 0; i < energies->rows * energies->cols; i++) {
        if ((energies->data[i] < g_yes_1000ms_mfe_energy[i] - REL_ERROR) ||
            (energies->data[i] > g_yes_1000ms_mfe_energy[i] + REL_ERROR)) {
            energies_check = OS_FALSE;
            printf("energies output %ld value is %f, expected value is %f\r\n",
                   i,
                   energies->data[i],
                   g_yes_1000ms_mfe_energy[i]);
        }
    }
    tp_assert_true(energies_check);

    os_bool_t feature_check = OS_TRUE;
    for (os_size_t i = 0; i < feature->rows * feature->cols; i++) {
        if ((feature->data[i] < g_yes_1000ms_mfe_feature[i] - REL_ERROR) ||
            (feature->data[i] > g_yes_1000ms_mfe_feature[i] + REL_ERROR)) {
            feature_check = OS_FALSE;
            printf("feature output %ld value is %f, expected value is %f\r\n",
                   i,
                   feature->data[i],
                   g_yes_1000ms_mfe_feature[i]);
        }
    }
    tp_assert_true(feature_check);

    uai_mat_destroy(feature);
    uai_mat_destroy(energies);
}

static void test_calc_mfcc_buff_size(void)
{
    uai_mat_size_t mfcc_mat_size = {0};

    os_size_t signal_length = 16000;
    os_uint32_t sampling_freq = 16000;
    float frame_length_s = 0.02;
    float frame_stride_s = 0.02;
    os_uint16_t num_cepstral = 13;

    int ret = audio_feature::calc_mfcc_buff_size(&mfcc_mat_size,
                                                 signal_length,
                                                 sampling_freq,
                                                 frame_length_s,
                                                 frame_stride_s,
                                                 num_cepstral);
    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(mfcc_mat_size.cols, num_cepstral);
    tp_assert_integer_equal(mfcc_mat_size.rows, 50);

    frame_stride_s = 0.01;
    num_cepstral = 20;

    ret = audio_feature::calc_mfcc_buff_size(&mfcc_mat_size,
                                             signal_length,
                                             sampling_freq,
                                             frame_length_s,
                                             frame_stride_s,
                                             num_cepstral);
    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(mfcc_mat_size.cols, num_cepstral);
    tp_assert_integer_equal(mfcc_mat_size.rows, 99);
}

static void test_mfcc(void)
{
    os_size_t signal_length = 16000;
    os_uint32_t sampling_freq = 16000;
    float frame_length_s = 0.02;
    float frame_stride_s = 0.02;
    os_uint16_t num_filter = 32;
    os_uint16_t num_cepstral = 13;
    os_uint16_t num_fft = 256;
    os_uint32_t low_freq = 0;
    os_uint32_t high_freq = 8000;
    float preemph_cof = 0.98;
    os_bool_t dc_elimination = OS_TRUE;

    uai_mat_size_t mfcc_size = {0};
    int ret = audio_feature::calc_mfcc_buff_size(&mfcc_size,
                                                 signal_length,
                                                 sampling_freq,
                                                 frame_length_s,
                                                 frame_stride_s,
                                                 num_cepstral);
    tp_assert_integer_equal(ret, UAI_EOK);
    tp_assert_integer_equal(mfcc_size.cols, num_cepstral);
    tp_assert_integer_equal(mfcc_size.rows, 50);

    uai_mat_t* mfcc_features = uai_mat_create(mfcc_size.rows, mfcc_size.cols);
    tp_assert_not_null(mfcc_features);

    uai_signal_t signal = {0};
    signal.total_length = signal_length;
    signal.get_data = get_yes_1000ms_data;

    os_tick_t start = os_tick_get();

    ret = audio_feature::mfcc(mfcc_features,
                              &signal,
                              sampling_freq,
                              frame_length_s,
                              frame_stride_s,
                              num_cepstral,
                              num_filter,
                              num_fft,
                              low_freq,
                              high_freq,
                              preemph_cof,
                              dc_elimination);
    os_tick_t end = os_tick_get();
    printf("mfcc cost:%lums\r\n", end - start);

    tp_assert_integer_equal(ret, UAI_EOK);

    os_bool_t features_check = OS_TRUE;
    for (os_size_t i = 0; i < mfcc_features->rows * mfcc_features->cols; i++) {
        if ((mfcc_features->data[i] < g_yes_1000ms_mfcc[i] - REL_ERROR) ||
            (mfcc_features->data[i] > g_yes_1000ms_mfcc[i] + REL_ERROR)) {
            features_check = OS_FALSE;
            printf("features output %ld value is %f, expected value is %f\r\n",
                   i,
                   mfcc_features->data[i],
                   g_yes_1000ms_mfcc[i]);
            break;
        }
    }
    tp_assert_true(features_check);

    uai_mat_destroy(mfcc_features);
}

static void test_case(void)
{
    ATEST_UNIT_RUN(test_filterbanks);
    ATEST_UNIT_RUN(test_calc_mfe_buff_size);
    ATEST_UNIT_RUN(test_mfe);
    ATEST_UNIT_RUN(test_calc_mfcc_buff_size);
    ATEST_UNIT_RUN(test_mfcc);

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

ATEST_TC_EXPORT(uai_sdk.feature.audio.tc,
                test_case,
                test_init,
                test_cleanup,
                TC_PRIORITY_MIDDLE);

}  // namespace feature
}  // namespace uai