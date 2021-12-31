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
 * @file        uai_dsp.cc
 *
 * @brief       This file is based on the edgeimpulse\dsp\numpy.hpp file, and we
 *              have made some optimizations and portings for OneOS.
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __UAI_DSP_H__
#define __UAI_DSP_H__

#include "uai_matrix.h"

#include <os_stddef.h>

namespace uai {
namespace feature {

typedef enum dct_norm
{
    DCT_NORMAL_NONE = 0,
    DCT_NORMAL_ORTHO
} dct_norm_t;

/** TODO: 重构参数名称 */

class dsp
{
public:
    static float sum(float* input, os_size_t size);

    static int dot(uai_mat_t* mat1, uai_mat_t* mat2, uai_mat_t* output);
    static int dot_by_row(os_size_t mat1_row,
                          float* mat1_row_data,
                          os_size_t mat1_cols,
                          uai_mat_t* mat2,
                          uai_mat_t* output);

    static int int16_to_float(const os_int16_t* src,
                              float* dst,
                              os_size_t size);

    static float log(float a);
    static float log2(float a);
    static float log10(float a);

    static int log(uai_mat_t* matrix);
    static int log10(uai_mat_t* matrix);

    static int linspace(float start, float stop, os_size_t num, float* out);
    static int linspace(os_int16_t start,
                        os_int16_t stop,
                        os_size_t num,
                        os_int16_t* out);
    static int linspace(os_int32_t start,
                        os_int32_t stop,
                        os_size_t num,
                        os_int32_t* out);

    static int rfft(const float* src,
                    os_size_t src_len,
                    float* out,
                    os_size_t out_len,
                    os_size_t fft_len);

    static int dct2(uai_mat_t* mat, dct_norm_t mode);
};

};  // namespace feature
};  // namespace uai

#endif /* __UAI_DSP_H__ */