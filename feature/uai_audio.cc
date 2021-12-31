/**
 *******************************************************************************
 * Copyright (c) 2021 China Mobile Communications Group Co.,Ltd.
 * Copyright (c) 2021 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file        uai_audio.cc
 *
 * @brief       This file is based on the edgeimpulse\inferencing-sdk-cpp\dsp\sp
 *              eechpy\feature.hpp file, and we have made some optimizations a
 *              nd portings for OneOS. We also refer to the related code in spee
 *              chpy\feature.py.
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#include "uai_audio.h"
#include "uai_dsp.h"
#include "uai_functions.h"
#include "uai_processing.h"

#define UAI_LOG_TAG "uai.feat.audio"
#include "uai_log.h"

#include <cmath>
#include <stdlib.h>
#include <string.h>

#include <os_assert.h>

namespace uai {
namespace feature {

/**
 * Compute the Mel-filterbanks. Each filter will be stored in one rows.
 * The columns correspond to fft bins.
 *
 * @param filterbanks       [out] Matrix of size num_filter * coefficients
 * @param num_filter        [in]  The number of filters in the filterbank
 * @param num_fft           [in]  The number of fft bins
 * @param sampling_freq     [in]  The samplerate of the signal we are working
 *                                with. It affects mel spacing.
 * @param low_freq          [in]  lowest band edge of mel filters, default 0 Hz
 * @param high_freq         [in]  highest band edge of mel filters, default
 *                                samplerate / 2
 * @param output_transposed [in]  If set to true this will transpose the matrix
 *                                (memory efficient). This is more efficient
 *                                than calling this function and then
 *                                transposing as the latter requires the
 *                                filterbank to be allocated twice (for a short
 *                                while).
 * @returns 0 if OK
 */
int audio_feature::filterbanks(uai_mat_t* filterbanks,
                               os_uint16_t num_filter,
                               os_uint16_t num_fft,
                               os_uint32_t sampling_freq,
                               os_uint32_t low_freq,
                               os_uint32_t high_freq,
                               os_bool_t output_transposed)
{
    OS_ASSERT(filterbanks != OS_NULL);

    os_uint16_t coefficients = num_fft / 2 + 1;

    if (filterbanks->rows != num_filter || filterbanks->cols != coefficients) {
        ERROR("The filterbanks matrix shape mismatch");
        return UAI_EINVAL;
    }

    memset(filterbanks->data, 0, num_filter * coefficients * sizeof(float));

    /* num_points = num_filter + 2 is because for num_filter filterbanks we need
     * num_filter + 2 point. */
    os_uint16_t num_points = num_filter + 2;

    float* mels = (float*)calloc(1, num_points * sizeof(float));
    if (OS_NULL == mels) {
        return UAI_ENOMEM;
    }
    float* freqs = (float*)calloc(1, num_points * sizeof(float));
    if (OS_NULL == freqs) {
        free(mels);
        return UAI_ENOMEM;
    }

    dsp::linspace(functions::frequency_to_mel(static_cast<float>(low_freq)),
                  functions::frequency_to_mel(static_cast<float>(high_freq)),
                  num_points,
                  mels);

    for (os_size_t i = 0; i < num_points; i++) {
        freqs[i] = functions::mel_to_frequency(mels[i]);
        freqs[i] = freqs[i] < low_freq ? low_freq : freqs[i];
        freqs[i] = freqs[i] > high_freq ? high_freq : freqs[i];
    }
    free(mels);

    os_size_t* freq_index =
        (os_size_t*)calloc(1, num_points * sizeof(os_size_t));
    if (OS_NULL == freq_index) {
        free(freqs);
        return UAI_ENOMEM;
    }
    for (os_size_t i = 0; i < num_points; i++) {
        freq_index[i] =
            static_cast<int>(floor((num_fft + 1) * freqs[i] / sampling_freq));
    }
    free(freqs);

    for (os_size_t i = 0; i < num_filter; i++) {
        os_int32_t left = freq_index[i];
        os_int32_t middle = freq_index[i + 1];
        os_int32_t right = freq_index[i + 2];

        os_int32_t filter_len = (right - left + 1);
        float* filter = (float*)malloc(filter_len * sizeof(float));
        if (OS_NULL == filter) {
            free(freq_index);
            return UAI_ENOMEM;
        }

        dsp::linspace(left, right, filter_len, filter);
        functions::triangle(filter, filter_len, left, middle, right);

        for (os_size_t j = 0; j < filter_len; j++) {
            os_size_t index = output_transposed
                                  ? ((left + j) * filterbanks->rows + i)
                                  : (i * filterbanks->cols + left + j);
            filterbanks->data[index] = filter[j];
        }

        free(filter);
    }

    if (output_transposed) {
        os_size_t temp = filterbanks->rows;
        filterbanks->rows = filterbanks->cols;
        filterbanks->cols = temp;
    }

    free(freq_index);

    return UAI_EOK;
}

static int _do_mfe(uai_mat_t* features,
                   uai_mat_t* energies,
                   uai_signal_t* signal,
                   os_uint16_t num_fft,
                   uai_mat_t* filterbanks,
                   frames_info_t* frames_info,
                   float preemph_cof)
{
    int ret = UAI_EOK;

    uai_mat_t* signal_frame = uai_mat_create(1, frames_info->frame_length);
    if (OS_NULL == signal_frame) {
        ERROR("Create a matrix of signal frame failed.");
        return UAI_ENOMEM;
    }

    os_size_t spec_frame_len = num_fft / 2 + 1;
    uai_mat_t* spec_frame = uai_mat_create(1, spec_frame_len);
    if (OS_NULL == spec_frame) {
        ERROR("Create a matrix of power spectrum failed.");
        uai_mat_destroy(signal_frame);
        return UAI_ENOMEM;
    }

    preemphasise pre_emph(signal, preemph_cof);

    for (os_size_t f_index = 0; f_index < frames_info->frame_nums; f_index++) {
        /** Read one frame from the signal buffer */
        int get_size = pre_emph.get_data(frames_info->frame_offsets[f_index],
                                         frames_info->frame_length,
                                         signal_frame->data);
        if (get_size != frames_info->frame_length) {
            ERROR("Read one frame from the signal buffer failed.");
            ret = UAI_ERROR;
            break;
        }

        /** Compute the power spectrum of the frame */
        ret = processing::power_spectrum(signal_frame->data,
                                         frames_info->frame_length,
                                         spec_frame->data,
                                         spec_frame_len,
                                         num_fft);
        if (ret != UAI_EOK) {
            ERROR("Compute the power spectrum of the frame failed.");
            break;
        }

        /** Compute the energy of each frame */
        energies->data[f_index] = dsp::sum(spec_frame->data, spec_frame_len);

        /** Compute the mel filterbanks feature of each frame */
        ret = dsp::dot_by_row(
            f_index, spec_frame->data, spec_frame_len, filterbanks, features);
        if (ret != UAI_EOK) {
            ERROR("Compute the mel filterbanks feature of the frame failed.");
            break;
        }
    }

    uai_mat_destroy(signal_frame);
    uai_mat_destroy(spec_frame);

    return ret;
}

/**
 * Compute Mel-filterbank energy features from an audio signal.
 *
 * @param features       [out] The energy of fiterbanks of size num_frames x
 *                             num_filters.
 * @param energies       [out] The energy of each frame of size num_frames x 1
 * @param signal         [in] The audio signal
 * @param sampling_freq  [in] The sampling frequency of the signal
 * @param frame_length_s [in] The length of each frame in seconds.
 * @param frame_stride_s [in] The step between successive frames in seconds.
 * @param num_filters    [in] The number of filters in the filterbank.
 * @param num_fft        [in] The number of FFT points.
 * @param low_freq       [in] The lowest band edge of mel filters in Hz.
 * @param high_freq      [in] The highest band edge of mel filters in Hz.
 * @param preemph_cof    [in] The preemphasis coefficient.
 *
 * @returns 0 if OK
 */
int audio_feature::mfe(uai_mat_t* features,
                       uai_mat_t* energies,
                       uai_signal_t* signal,
                       os_uint32_t sampling_freq,
                       float frame_length_s,
                       float frame_stride_s,
                       os_uint16_t num_filter,
                       os_uint16_t num_fft,
                       os_uint32_t low_freq,
                       os_uint32_t high_freq,
                       float preemph_cof)
{
    OS_ASSERT(features != OS_NULL);
    OS_ASSERT(energies != OS_NULL);
    OS_ASSERT(signal != OS_NULL);

    /** Step1: Frame a signal into overlapping frames. */
    frames_info_t frames_info = {0};

    int ret = processing::stack_frames(&frames_info,
                                       signal,
                                       sampling_freq,
                                       frame_length_s,
                                       frame_stride_s,
                                       OS_FALSE);
    if (ret != UAI_EOK) {
        ERROR("Frame a signal into overlapping frames failed.");
        return ret;
    }

    if (frames_info.frame_nums != features->rows ||
        num_filter != features->cols) {
        ERROR("The matirx of features size mismatch.");
        processing::stack_frames_info_free(&frames_info);
        return UAI_EINVAL;
    }

    if (frames_info.frame_nums != energies->rows || 1 != energies->cols) {
        ERROR("The matirx of energies size mismatch.");
        processing::stack_frames_info_free(&frames_info);
        return UAI_EINVAL;
    }

    /** Step2: Compute the Mel-filterbanks. */
    uai_mat_t* filterbanks = uai_mat_create(num_filter, num_fft / 2 + 1);
    if (OS_NULL == filterbanks) {
        ERROR("Create a matrix of filterbanks failed.");
        processing::stack_frames_info_free(&frames_info);
        return UAI_ENOMEM;
    }

    /** Transpose the filterbanks matrix by default */
    ret = audio_feature::filterbanks(filterbanks,
                                     num_filter,
                                     num_fft,
                                     sampling_freq,
                                     low_freq,
                                     high_freq,
                                     OS_TRUE);
    if (ret != UAI_EOK) {
        ERROR("Compute the Mel-filterbanks failed.");
        processing::stack_frames_info_free(&frames_info);
        uai_mat_destroy(filterbanks);
        return ret;
    }

    /** Step3: Compute the mfe features of each frame. */
    ret = _do_mfe(features,
                  energies,
                  signal,
                  num_fft,
                  filterbanks,
                  &frames_info,
                  preemph_cof);

    if (UAI_EOK == ret) {
        functions::zero_handling(energies);
        functions::zero_handling(features);
    }

    processing::stack_frames_info_free(&frames_info);
    uai_mat_destroy(filterbanks);

    return ret;
}

/**
 * Calculate the buffer size for MFE
 *
 * @param mat_size           [out] The buffer size for MFE.
 * @param signal_length      [in] Length of the signal.
 * @param sampling_frequency [in] The sampling frequency of the signal.
 * @param frame_length_s     [in] The length of the frame in second.
 * @param frame_stride_s     [in] The stride between frames in second.
 * @param num_filters        [in] The number of filters in the mel
 * filterbank.
 *
 * @returns 0 if OK
 */
int audio_feature::calc_mfe_buff_size(uai_mat_size_t* mat_size,
                                      os_size_t signal_length,
                                      os_uint32_t sampling_freq,
                                      float frame_length_s,
                                      float frame_stride_s,
                                      os_uint16_t num_filter)
{
    OS_ASSERT(mat_size != OS_NULL);

    frames_info_t frames_info = {0};

    /** Zero padding is disabled by default */
    int ret = processing::calc_stack_frames_info(&frames_info,
                                                 signal_length,
                                                 sampling_freq,
                                                 frame_length_s,
                                                 frame_stride_s,
                                                 OS_FALSE);
    if (ret != UAI_EOK) {
        ERROR("Calculate stack frame info failed.");
        return ret;
    }

    mat_size->rows = frames_info.frame_nums;
    mat_size->cols = num_filter;

    return UAI_EOK;
}

static int _do_mfcc(uai_mat_t* mfcc_features,
                    uai_mat_t* mfe_features,
                    uai_mat_t* mfe_energies,
                    uai_signal_t* signal,
                    os_uint32_t sampling_freq,
                    float frame_length_s,
                    float frame_stride_s,
                    os_uint16_t num_cepstral,
                    os_uint16_t num_filter,
                    os_uint16_t num_fft,
                    os_uint32_t low_freq,
                    os_uint32_t high_freq,
                    float preemph_cof,
                    os_base_t dc_elimination)
{
    OS_ASSERT(mfcc_features != OS_NULL);
    OS_ASSERT(mfe_features != OS_NULL);
    OS_ASSERT(mfe_energies != OS_NULL);
    OS_ASSERT(signal != OS_NULL);

    /** Step1: Compute Mel-filterbank energy features from an audio signal. */
    int ret = audio_feature::mfe(mfe_features,
                                 mfe_energies,
                                 signal,
                                 sampling_freq,
                                 frame_length_s,
                                 frame_stride_s,
                                 num_filter,
                                 num_fft,
                                 low_freq,
                                 high_freq,
                                 preemph_cof);
    if (ret != UAI_EOK) {
        ERROR("Compute Mel-filterbank energy features failed.");
        return ret;
    }

    /** Step2:  Get the log filterbank energies. */
    ret = dsp::log(mfe_features);
    if (ret != UAI_EOK) {
        ERROR("Get the log filterbank energies failed.");
        return ret;
    }

    /** Step3:  Discrete Cosine Transform. */
    ret = dsp::dct2(mfe_features, DCT_NORMAL_ORTHO);
    if (ret != UAI_EOK) {
        ERROR("Discrete Cosine Transform failed.");
        return ret;
    }

    /** Step4: Replace first cepstral coefficient with log of frame energy for
     * DC elimination */
    if (dc_elimination) {
        for (os_size_t row = 0; row < mfe_features->rows; row++) {
            os_size_t data_index = row * mfe_features->cols;
            mfe_features->data[data_index] = dsp::log(mfe_energies->data[row]);
        }
    }

    /** Step5: Copy to the mfcc features output */
    for (os_size_t row = 0; row < mfe_features->rows; row++) {
        for (os_size_t col = 0; col < num_cepstral; col++) {
            mfcc_features->data[row * num_cepstral + col] =
                mfe_features->data[row * mfe_features->cols + col];
        }
    }

    return UAI_EOK;
}

/**
 * Compute MFCC features from an audio signal.
 *
 * @param features       [out] The mfcc features of size num_frames x
 *                             num_cepstral.
 * @param signal         [in] The audio signal
 * @param sampling_freq  [in] The sampling frequency of the signal
 * @param frame_length_s [in] The length of each frame in seconds.
 * @param frame_stride_s [in] The step between successive frames in seconds.
 * @param num_cepstral   [in] The number of cepstral coefficients.
 * @param num_filters    [in] The number of filters in the filterbank.
 * @param num_fft        [in] The number of FFT points.
 * @param low_freq       [in] The lowest band edge of mel filters in Hz.
 * @param high_freq      [in] The highest band edge of mel filters in Hz.
 * @param preemph_cof    [in] The preemphasis coefficient.
 * @param dc_elimination [in] Whether to eliminate the DC component.
 *
 * @returns 0 if OK
 */
int audio_feature::mfcc(uai_mat_t* features,
                        uai_signal_t* signal,
                        os_uint32_t sampling_freq,
                        float frame_length_s,
                        float frame_stride_s,
                        os_uint16_t num_cepstral,
                        os_uint16_t num_filter,
                        os_uint16_t num_fft,
                        os_uint32_t low_freq,
                        os_uint32_t high_freq,
                        float preemph_cof,
                        os_base_t dc_elimination)
{
    OS_ASSERT(features != OS_NULL);
    OS_ASSERT(signal != OS_NULL);

    uai_mat_size_t mfe_size = {0};
    int ret = calc_mfe_buff_size(&mfe_size,
                                 signal->total_length,
                                 sampling_freq,
                                 frame_length_s,
                                 frame_stride_s,
                                 num_filter);
    if (ret != UAI_EOK) {
        ERROR("Calculate the buffer size for MFE failed.");
        return ret;
    }

    if (features->rows != mfe_size.rows || features->cols != num_cepstral) {
        ERROR("The matrix of features size mismatch.");
        return UAI_EINVAL;
    }

    uai_mat_t* mfe_energies = uai_mat_create(mfe_size.rows, 1);
    if (OS_NULL == mfe_energies) {
        ERROR("Create a matrix of energies failed.");
        return UAI_ENOMEM;
    }

    uai_mat_t* mfe_features = uai_mat_create(mfe_size.rows, mfe_size.cols);
    if (OS_NULL == mfe_features) {
        ERROR("Create a matrix of features failed.");
        uai_mat_destroy(mfe_energies);
        return UAI_ENOMEM;
    }

    ret = _do_mfcc(features,
                   mfe_features,
                   mfe_energies,
                   signal,
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
    if (ret != UAI_EOK) {
        ERROR("Compute MFCC features failed.");
    }

    uai_mat_destroy(mfe_features);
    uai_mat_destroy(mfe_energies);

    return ret;
}

int audio_feature::calc_mfcc_buff_size(uai_mat_size_t* mat_size,
                                       os_size_t signal_length,
                                       os_uint32_t sampling_freq,
                                       float frame_length_s,
                                       float frame_stride_s,
                                       os_uint16_t num_cepstral)
{
    OS_ASSERT(mat_size != OS_NULL);

    frames_info_t frames_info = {0};

    /** Zero padding is disabled by default */
    int ret = processing::calc_stack_frames_info(&frames_info,
                                                 signal_length,
                                                 sampling_freq,
                                                 frame_length_s,
                                                 frame_stride_s,
                                                 OS_FALSE);
    if (ret != UAI_EOK) {
        ERROR("Calculate stack frame info failed.");
        return ret;
    }

    mat_size->rows = frames_info.frame_nums;
    mat_size->cols = num_cepstral;

    return UAI_EOK;
}

};  // namespace feature
};  // namespace uai