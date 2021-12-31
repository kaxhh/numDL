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
 * @file        uai_processing.cc
 *
 * @brief       This file is based on the edgeimpulse\inferencing-sdk-cpp\dsp\sp
 *              eechpy\processing.hpp file, and we have made some optimizations
 *              and portings for OneOS. We also refer to the related code in spe
 *              echpy\processing.py.
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#include "uai_processing.h"
#include "uai_errno.h"

#define UAI_LOG_TAG "uai.processing"
#include "uai_log.h"

#include <cmath>
#include <stdio.h>

#include <os_assert.h>
#include <os_memory.h>

namespace uai {

namespace feature {

/**
 * frame_length is a float and can thus be off by a little bit, e.g.
 * frame_length = 0.018f actually can yield 0.018000011f
 * thus screwing up our frame calculations here...
 */
static float ceil_unless_very_close_to_floor(float v)
{
    if (v > floor(v) && v - floor(v) < 0.001f) {
        v = (floor(v));
    } else {
        v = (ceil(v));
    }
    return v;
}

/**
 * Preemphasising on the signal. This modifies the signal in place
 *
 * @param signal [in,out] The input signal.
 * @param cof    [in]     The preemphasising coefficient. 0 equals to no
 * filtering.
 *
 * @returns 0 if OK
 */
int processing::pre_emphasise(float* signal, os_size_t signal_length, float cof)
{
    OS_ASSERT(signal != OS_NULL);

    float end = signal[signal_length - 1];

    for (int i = signal_length - 1; i >= 1; i--) {
        signal[i] -= signal[i - 1] * cof;
    }

    signal[0] -= end * cof;

    return UAI_EOK;
}

/**
 * Frame a signal into overlapping frames.
 *
 * @param frames_info  [out] The struct store our farming results.
 * @param signal       [in]  The Signal to be framed.
 * @param frequency    [in]  The sampling frequency of the signal.
 * @param frame_length [in]  The length of the frame in second.
 * @param frame_stride [in]  The stride between frames in second.
 * @param zero_padding [in]  If the samples is not a multiple of
 *        frame_length(number of frames sample), zero padding will
 *        be done for generating last frame.
 *
 * @returns 0 if OK
 */
int processing::stack_frames(frames_info_t* frames_info,
                             uai_signal_t* signal,
                             os_uint32_t frequency,
                             float frame_length_s,
                             float frame_stride_s,
                             os_bool_t zero_padding)
{
    OS_ASSERT(frames_info != OS_NULL);
    OS_ASSERT(signal != OS_NULL);

    int ret = calc_stack_frames_info(frames_info,
                                     signal->total_length,
                                     frequency,
                                     frame_length_s,
                                     frame_stride_s,
                                     zero_padding);
    if (ret != UAI_EOK) {
        ERROR("Calculate stack frame info failed.");
        return ret;
    }

    os_size_t f_num = frames_info->frame_nums;
    os_size_t* f_offsets = (os_size_t*)os_calloc(1, f_num * sizeof(os_size_t));
    if (OS_NULL == f_offsets) {
        ERROR("Alloc frames info instance (%d bytes) failed, no enough memory.",
              f_num * sizeof(os_size_t));
        return UAI_ENOMEM;
    }

    os_size_t f_stride = frames_info->frame_stride;
    os_size_t count = 0;
    for (os_size_t offset = 0; offset < signal->total_length;
         offset += f_stride) {
        if (count >= frames_info->frame_nums) {
            break;
        }

        f_offsets[count] = offset;
        count++;
    }

    frames_info->frame_offsets = f_offsets;

    return UAI_EOK;
}

void processing::stack_frames_info_free(frames_info_t* frames_info)
{
    return os_free(frames_info->frame_offsets);
}

/**
 * Calculate the number of stack frames for the settings provided.
 *
 * This is needed to allocate the right buffer size for the output of f.e. the
 * MFE blocks.
 *
 * @param frames_info    [out] The struct store our farming results.
 * @param signal_length  [in] The length of the signal
 * @param frequency      [in] The sampling frequency of the signal.
 * @param frame_length_s [in] The length of the frame.
 * @param frame_stride_s [in] The stride between frames.
 * @param zero_padding   [in] If the samples is not a multiple of
 *        frame_length(number of frames sample), zero padding will
 *        be done for generating last frame.
 *
 * @returns 0 if OK
 */
int processing::calc_stack_frames_info(frames_info_t* frames_info,
                                       os_size_t signal_length,
                                       os_uint32_t frequency,
                                       float frame_length_s,
                                       float frame_stride_s,
                                       os_bool_t zero_padding)
{
    OS_ASSERT(frames_info != OS_NULL);

    os_size_t f_length = static_cast<os_size_t>(ceil_unless_very_close_to_floor(
        static_cast<float>(frequency) * frame_length_s));
    os_size_t f_stride = static_cast<os_size_t>(ceil_unless_very_close_to_floor(
        static_cast<float>(frequency) * frame_stride_s));

    os_size_t over_length = f_length - f_stride;

    os_size_t f_nums = 0;

    if (zero_padding) {
        f_nums = static_cast<os_size_t>(
            ceil(static_cast<float>(signal_length - over_length) / f_stride));
    } else {
        f_nums = static_cast<os_size_t>(
            floor(static_cast<float>(signal_length - over_length) / f_stride));
    }

    frames_info->end_offset = f_nums * f_stride + over_length;
    frames_info->frame_nums = f_nums;
    frames_info->frame_stride = f_stride;
    frames_info->frame_length = f_length;

    return UAI_EOK;
}

int processing::power_spectrum(const float* frame,
                               os_size_t frame_len,
                               float* out,
                               os_size_t out_len,
                               os_size_t fft_len)
{
    OS_ASSERT(frame != OS_NULL);
    OS_ASSERT(out != OS_NULL);

    int ret = dsp::rfft(frame, frame_len, out, out_len, fft_len);
    if (ret != UAI_EOK) {
        return ret;
    }

    for (os_size_t i = 0; i < out_len; i++) {
        out[i] = (1.0 / static_cast<float>(fft_len)) * out[i] * out[i];
    }

    return UAI_EOK;
}

/**
 * Lazy preemphasising on the signal.
 *
 * @param signal [in] The pointer to input signal.
 * @param cof    [in] The preemphasising coefficient. 0 equals to no filtering.
 */
preemphasise::preemphasise(uai_signal_t* signal, float cof)
    : _sighal(signal),
      _cof(cof)
{
    OS_ASSERT(_sighal != OS_NULL);

    _prev_sample = 0.0;
    int ret = signal->get_data(signal->total_length - 1, 1, &_last_sample);
    if (ret <= 0) {
        ERROR("Get last sample failed.");
        return;
    }
}

preemphasise::~preemphasise()
{
    _sighal = OS_NULL;
}

/**
 * Get preemphasized data from the underlying audio buffer...
 * This retrieves data from the signal then preemphasizes it.
 *
 * @param offset [in]  The offset in the audio signal
 * @param length [in]  The length of the audio signal
 * @param out    [out] The output buffer
 *
 * @return The actually get data size on successful, otherwise negative
 *         returned.
 */
int preemphasise::get_data(os_size_t offset, os_size_t length, float* out)
{
    OS_ASSERT(out != OS_NULL);

    if (offset + length > _sighal->total_length) {
        ERROR("Offset (%d) + length (%d) is larger than signal length (%d).",
              offset,
              length,
              _sighal->total_length);
        return UAI_EINVAL;
    }

    int ret = _sighal->get_data(offset, length, out);
    if (ret <= 0) {
        ERROR("Get signal data failed.");
        return ret;
    }

    for (os_size_t i = length - 1; i >= 1; i--) {
        out[i] -= _cof * out[i - 1];
    }

    if (0 == offset) {
        out[0] -= _cof * _last_sample;
    } else {
        ret = _sighal->get_data(offset - 1, 1, &_prev_sample);
        if (ret <= 0) {
            ERROR("Get prev signal data failed.");
            return ret;
        }
        out[0] -= _cof * _prev_sample;
    }

    return length;
}

}  // namespace feature

}  // namespace uai
