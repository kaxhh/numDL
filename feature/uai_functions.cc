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
 * @file        uai_functions.cc
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

#include "uai_functions.h"
#include "uai_dsp.h"

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

namespace uai {
namespace feature {

/**
 * Converting from frequency to Mel scale
 *
 * @param f [in] The frequency values(or a single frequency) in Hz.
 *
 * @returns The mel scale values(or a single mel).
 */
float functions::frequency_to_mel(float f)
{
    return 1127.0 * dsp::log(1 + f / 700.0f);
}

/**
 * Converting from Mel scale to frequency.
 *
 * @param mel [in] The mel scale values(or a single mel).
 *
 * @returns The frequency values(or a single frequency) in Hz.
 */
float functions::mel_to_frequency(float mel)
{
    return 700.0f * (exp(mel / 1127.0f) - 1.0f);
}

/**
 * This function handle the issue with zero values if the are exposed
 * to become an argument for any log function.
 *
 * @param input [in,out] Array
 * @param size  [in]     Size of array
 *
 * @returns void
 */
void functions::zero_handling(float *input, os_size_t size)
{
    for (os_size_t ix = 0; ix < size; ix++) {
        if (input[ix] == 0) {
            input[ix] = FLT_EPSILON;
        }
    }
}

/**
 * This function handle the issue with zero values if the are exposed
 * to become an argument for any log function.
 *
 * @param input [in] Matrix
 *
 * @returns void
 */
void functions::zero_handling(uai_mat_t *input)
{
    zero_handling(input->data, input->rows * input->cols);
}

/**
 * Triangle, I'm not really sure what this does, @see
 * https://github.com/astorfi/speechpy/blob/master/speechpy/functions.py
 *
 * @param x      [in,out] Linspace output, will be overwritten!
 * @param x_size [in]     Size of the linspace output
 * @param left   [in]
 * @param middle [in]
 * @param right  [in]
 *
 * @returns 0 if OK
 */
int functions::triangle(float *x,
                        os_size_t x_size,
                        os_int32_t left,
                        os_int32_t middle,
                        os_int32_t right)
{
    float *out = (float *)calloc(1, x_size * sizeof(float));

    for (os_size_t i = 0; i < x_size; i++) {
        if (x[i] > left && x[i] <= middle) {
            out[i] = (x[i] - left) / (middle - left);
        }

        if (x[i] < right && middle <= x[i]) {
            out[i] = (right - x[i]) / (right - middle);
        }
    }

    memcpy(x, out, x_size * sizeof(float));

    free(out);

    return UAI_EOK;
}

};  // namespace feature
};  // namespace uai