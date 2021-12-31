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
 * @file        uai_processing.h
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

#ifndef __UAI_PROCESSING_H__
#define __UAI_PROCESSING_H__

#include "uai_dsp.h"
#include "uai_signal.h"

#include <os_stddef.h>

namespace uai {
namespace feature {

typedef struct frames_info
{
    os_size_t frame_length;
    os_size_t frame_stride;
    os_size_t frame_nums;

    os_size_t end_offset;

    os_size_t* frame_offsets;
} frames_info_t;

class processing
{
public:
    static int pre_emphasise(float* signal, os_size_t signal_length, float cof);

    static int stack_frames(frames_info_t* frames_info,
                            uai_signal_t* signal,
                            os_uint32_t frequency,
                            float frame_length_s,
                            float frame_stride_s,
                            os_bool_t zero_padding);

    static void stack_frames_info_free(frames_info_t* frames_info);

    static int calc_stack_frames_info(frames_info_t* frames_info,
                                      os_size_t signal_length,
                                      os_uint32_t frequency,
                                      float frame_length_s,
                                      float frame_stride_s,
                                      os_bool_t zero_padding);

    static int power_spectrum(const float* frame,
                              os_size_t frame_len,
                              float* out,
                              os_size_t out_len,
                              os_size_t fft_len);
};

class preemphasise
{
public:
    preemphasise(uai_signal_t* signal, float cof);
    ~preemphasise();

    int get_data(os_size_t offset, os_size_t length, float* out);

private:
    uai_signal_t* _sighal;
    float _cof;
    float _prev_sample;
    float _last_sample;
};

};  // namespace feature
};  // namespace uai

#endif /* __UAI_PROCESSING_H__ */