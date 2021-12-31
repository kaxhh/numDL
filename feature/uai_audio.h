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
 *              eechpy\feature.hpp file, and we have made some optimizations and
 *              portings for OneOS. We also refer to the related code in speechp
 *              y\feature.py.
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __UAI_AUDIO_H__
#define __UAI_AUDIO_H__

#include "uai_errno.h"
#include "uai_signal.h"
#include "uai_matrix.h"

namespace uai {
namespace feature {

class audio_feature
{
public:
    static int filterbanks(uai_mat_t* filterbanks,
                           os_uint16_t num_filter,
                           os_uint16_t num_fft,
                           os_uint32_t sampling_freq,
                           os_uint32_t low_freq,
                           os_uint32_t high_freq,
                           os_bool_t output_transposed);

    static int mfe(uai_mat_t* features,
                   uai_mat_t* energies,
                   uai_signal_t* signal,
                   os_uint32_t sampling_freq,
                   float frame_length_s,
                   float frame_stride_s,
                   os_uint16_t num_filter,
                   os_uint16_t num_fft,
                   os_uint32_t low_freq,
                   os_uint32_t high_freq,
                   float preemph_cof);

    static int calc_mfe_buff_size(uai_mat_size_t* mat_size,
                                  os_size_t signal_length,
                                  os_uint32_t sampling_freq,
                                  float frame_length_s,
                                  float frame_stride_s,
                                  os_uint16_t num_filter);

    static int mfcc(uai_mat_t* features,
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
                    os_base_t dc_elimination);

    static int calc_mfcc_buff_size(uai_mat_size_t* mat_size,
                                   os_size_t signal_length,
                                   os_uint32_t sampling_freq,
                                   float frame_length_s,
                                   float frame_stride_s,
                                   os_uint16_t num_cepstral);
};
};      // namespace feature
};      // namespace uai
#endif  // __UAI_AUDIO_H__