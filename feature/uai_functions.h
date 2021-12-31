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
 * @file        uai_functions.h
 *
 * @brief       This file is based on the edgeimpulse\inferencing-sdk-cpp\dsp\sp
 *              eechpy\functions.hpp file, and we have made some optimizations a
 *              nd portings for OneOS. We also refer to the related code in spee
 *              chpy\functions.py.
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __UAI_FUNCTIONS_H__
#define __UAI_FUNCTIONS_H__

#include "uai_errno.h"
#include "uai_matrix.h"

#include <stdio.h>

namespace uai {
namespace feature {

class functions
{
public:
    static float frequency_to_mel(float f);
    static float mel_to_frequency(float mel);

    static void zero_handling(float *input, os_size_t size);
    static void zero_handling(uai_mat_t *input);

    static int triangle(float *x,
                        os_size_t x_size,
                        os_int32_t left,
                        os_int32_t middle,
                        os_int32_t right);
};
};      // namespace feature
};      // namespace uai
#endif  // __UAI_FUNCTIONS_H__