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

#include "uai_dsp.h"
#include "nd_errno.h"

#define NUMDL_LOG_TAG "uai.dsp"
#include "nd_log.h"

#include <kiss_fftr.h>

#include <math.h>
#include <string.h>

#include <nd_assert.h>

namespace uai {
namespace feature {

/**
 * Calculate the sum of the array
 *
 * @param input [in] Pointer to array
 * @param size  [in] Size of the array
 *
 * @returns The sum of the array
 */
float dsp::sum(float* input, os_size_t size)
{
    NUMDL_ASSERT(input != NULL);

    float sum = 0;

    for (os_size_t i = 0; i < size; i++) {
        sum += input[i];
    }

    return sum;
}

/**
 * Multiply two matrices (MxN * NxK matrix)
 *
 * @param mat1   [in]  Pointer to matrix1 (MxN)
 * @param mat2   [in]  Pointer to matrix2 (NxK)
 * @param output [out] Pointer to out matrix (MxK)
 *
 * @returns 0 if OK
 */
int dsp::dot(uai_mat_t* mat1, uai_mat_t* mat2, uai_mat_t* output)
{
    NUMDL_ASSERT(mat1 != OS_NULL);
    NUMDL_ASSERT(mat2 != OS_NULL);
    NUMDL_ASSERT(output != OS_NULL);
    NUMDL_ASSERT(mat1->cols == mat2->rows);
    NUMDL_ASSERT(mat1->rows == output->rows);
    NUMDL_ASSERT(mat2->cols == output->cols);

    memset(output->data, 0, output->rows * output->cols * sizeof(float));

    for (os_size_t i = 0; i < mat1->rows; i++) {
        dsp::dot_by_row(
            i, mat1->data + (i * mat1->cols), mat1->cols, mat2, output);
    }

    return NUMDL_EOK;
}

/**
 * Multiply two matrices lazily per row in matrix 1 (MxN * NxK matrix)
 *
 * @param mat1_row      [in]  Matrix1 row index
 * @param mat1_row_data [in]  Matrix1 row
 * @param mat1_cols     [in]  Matrix1 row size (1xN)
 * @param mat2          [in]  Pointer to matrix2 (NxK)
 * @param output        [out] Pointer to out matrix (MxK)
 *
 * @returns 0 if OK
 */
int dsp::dot_by_row(os_size_t mat1_row,
                    float* mat1_row_data,
                    os_size_t mat1_cols,
                    uai_mat_t* mat2,
                    uai_mat_t* output)
{
    NUMDL_ASSERT(mat1_row_data != OS_NULL);
    NUMDL_ASSERT(mat2 != OS_NULL);
    NUMDL_ASSERT(output != OS_NULL);
    NUMDL_ASSERT(mat1_cols == mat2->rows);
    NUMDL_ASSERT(mat2->cols == output->cols);

    for (os_size_t i = 0; i < mat2->cols; i++) {
        float tmp = 0.0;
        for (os_size_t j = 0; j < mat1_cols; j++) {
            tmp += mat1_row_data[j] * mat2->data[j * mat2->cols + i];
        }
        output->data[mat1_row * mat2->cols + i] = tmp;
    }

    return NUMDL_EOK;
}

/**
 * Convert an int16_t buffer into a float buffer, maps to -1..1
 *
 * @param input  [in]  points to the input vector
 * @param output [out] points to the floating-point output vector
 * @param length [in]  Number of samples in each vector
 *
 * @returns 0 if OK
 */
int dsp::int16_to_float(const os_int16_t* input,
                        float* output,
                        os_size_t length)

{
    NUMDL_ASSERT(input != OS_NULL);
    NUMDL_ASSERT(output != OS_NULL);

    for (os_size_t i = 0; i < length; i++) {
        output[i] = (float)input[i] / 32768.f;
    }


    return NUMDL_EOK;
}

/**
 * Fast log() function
 *
 * @param a [in] Input number
 *
 * @returns Natural log value of a
 */
float dsp::log(float a)
{
    float r = 0.0;

    r = logf(a);

    return r;
}

/**
 * Fast log2 functions
 *
 * @param a [in] Input number
 *
 * @returns Log2 value of a
 */
float dsp::log2(float a)
{
    float r = 0.0;

    r = log2f(a);

    return r;
}

/**
 * Fast log10 functions
 *
 * @param a [in] Input number
 *
 * @returns Log10 value of a
 */
float dsp::log10(float a)
{
    return dsp::log2(a) * 0.3010299956639812f;
}

/**
 * Calculate the natural log value of a matrix. Does an in-place replacement.
 *
 * @param matrix [in,out] Matrix (MxN)
 *
 * @returns 0 if OK
 */
int dsp::log(uai_mat_t* matrix)
{
    NUMDL_ASSERT(matrix != OS_NULL);
    NUMDL_ASSERT(matrix->data != OS_NULL);

    for (os_size_t i = 0; i < matrix->cols * matrix->rows; i++) {
        matrix->data[i] = dsp::log(matrix->data[i]);
    }

    return NUMDL_EOK;
}

/**
 * Calculate the log10 of a matrix. Does an in-place replacement.
 * @param matrix [in,out] Matrix (MxN)
 *
 * @returns 0 if OK
 */
int dsp::log10(uai_mat_t* matrix)
{
    NUMDL_ASSERT(matrix != OS_NULL);
    NUMDL_ASSERT(matrix->data != OS_NULL);

    for (os_size_t i = 0; i < matrix->cols * matrix->rows; i++) {
        matrix->data[i] = dsp::log10(matrix->data[i]);
    }

    return NUMDL_EOK;
}

/**
 * Return evenly spaced numbers over a specified interval.
 * Returns num evenly spaced samples, calculated over the interval [start,
 * stop]. The endpoint of the interval can optionally be excluded.
 *
 * Based on https://github.com/ntessore/algo/blob/master/linspace.c
 * Licensed in public domain (see LICENSE in repository above)
 *
 * @param start  [in] The starting value of the sequence.
 * @param stop   [in] The end value of the sequence.
 * @param number [in] Number of samples to generate.
 * @param out    [in] Out array, with size `num`
 *
 * @returns 0 if OK
 */
int dsp::linspace(float start, float stop, os_size_t num, float* out)
{
    NUMDL_ASSERT(out != OS_NULL);

    if (1 == num) {
        out[0] = start;
        return NUMDL_EOK;
    }

    // step size
    float step = (stop - start) / (num - 1);

    // do steps
    for (os_size_t ix = 0; ix < num - 1; ix++) {
        out[ix] = start + ix * step;
    }

    // last entry always stop
    out[num - 1] = stop;

    return NUMDL_EOK;
}

/**
 * Return evenly spaced q15 numbers over a specified interval.
 * Returns num evenly spaced samples, calculated over the interval [start,
 * stop]. The endpoint of the interval can optionally be excluded.
 *
 * Based on https://github.com/ntessore/algo/blob/master/linspace.c
 * Licensed in public domain (see LICENSE in repository above)
 *
 * @param start  [in] The starting value of the sequence.
 * @param stop   [in] The end value of the sequence.
 * @param number [in] Number of samples to generate.
 * @param out    [in] Out array, with size `num`
 *
 * @returns 0 if OK
 */
int dsp::linspace(os_int16_t start,
                  os_int16_t stop,
                  os_size_t num,
                  os_int16_t* out)
{
    NUMDL_ASSERT(out != OS_NULL);

    if (1 == num) {
        out[0] = start;
        return NUMDL_EOK;
    }

    // step size
    os_int16_t step = (stop - start) / (num - 1);

    // do steps
    for (os_size_t ix = 0; ix < num - 1; ix++) {
        out[ix] = start + ix * step;
    }

    // last entry always stop
    out[num - 1] = stop;

    return NUMDL_EOK;
}

/**
 * Return evenly spaced q31 numbers over a specified interval.
 * Returns num evenly spaced samples, calculated over the interval [start,
 * stop]. The endpoint of the interval can optionally be excluded.
 *
 * Based on https://github.com/ntessore/algo/blob/master/linspace.c
 * Licensed in public domain (see LICENSE in repository above)
 *
 * @param start  [in]  The starting value of the sequence.
 * @param stop   [in]  The end value of the sequence.
 * @param number [in]  Number of samples to generate.
 * @param out    [out] Out array, with size `num`
 *
 * @returns 0 if OK
 */
int dsp::linspace(os_int32_t start,
                  os_int32_t stop,
                  os_size_t num,
                  os_int32_t* out)
{
    NUMDL_ASSERT(out != OS_NULL);

    if (1 == num) {
        out[0] = start;
        return NUMDL_EOK;
    }

    // step size
    os_int32_t step = (stop - start) / (num - 1);

    // do steps
    for (os_size_t ix = 0; ix < num - 1; ix++) {
        out[ix] = start + ix * step;
    }

    // last entry always stop
    out[num - 1] = stop;

    return NUMDL_EOK;
}

static int kiss_rfft_cpx(const float* src,
                         kiss_fft_cpx* cpx_out,
                         os_size_t fft_len)
{
    NUMDL_ASSERT(src != NULL);
    NUMDL_ASSERT(cpx_out != OS_NULL);

    kiss_fftr_cfg cfg = kiss_fftr_alloc(fft_len, 0, OS_NULL, OS_NULL);
    if (OS_NULL == cfg) {
        ERROR("Allocate fft cfg error.");
        return NUMDL_ENOMEM;
    }

    kiss_fftr(cfg, src, cpx_out);

    free(cfg);

    return NUMDL_EOK;
}

static int kiss_rfft(const float* src,
                     float* out,
                     os_size_t fft_len,
                     os_size_t feat_len)
{
    NUMDL_ASSERT(src != OS_NULL);
    NUMDL_ASSERT(out != OS_NULL);

    kiss_fft_cpx* cpx_out =
        (kiss_fft_cpx*)malloc(feat_len * sizeof(kiss_fft_cpx));
    if (OS_NULL == cpx_out) {
        ERROR("Allocate output cpx buffer error.");
        return NUMDL_ENOMEM;
    }

    int ret = kiss_rfft_cpx(src, cpx_out, fft_len);
    if (NUMDL_EOK != ret) {
        ERROR("Kiss rfft error.");
        free(cpx_out);
        return ret;
    }

    for (os_size_t i = 0; i < feat_len; i++) {
        out[i] = sqrt(pow(cpx_out[i].r, 2) + pow(cpx_out[i].i, 2));
    }

    free(cpx_out);
    return NUMDL_EOK;
}

/**
 * Compute the one-dimensional discrete Fourier Transform for real input.
 * This function computes the one-dimensional n-point discrete Fourier
 * Transform (DFT) of a real-valued array by means of an efficient algorithm
 * called the Fast Fourier Transform (FFT).
 *
 * @param src     [in]  Source buffer
 * @param src_len [in]  Size of the source buffer
 * @param out     [out] Output buffer
 * @param out_len [in]  Size of the output buffer, should be n_fft / 2 + 1
 * @param fft_len [in]  Length of the FFT
 *
 * @returns 0 if OK
 */
int dsp::rfft(const float* src,
              os_size_t src_len,
              float* out,
              os_size_t out_len,
              os_size_t fft_len)
{
    NUMDL_ASSERT(src != OS_NULL);
    NUMDL_ASSERT(out != OS_NULL);

    os_size_t feat_len = fft_len / 2 + 1;
    if (feat_len != out_len) {
        ERROR("The output buffer length(%d) mismatch, should be %d",
              out_len,
              feat_len);
        return NUMDL_EINVAL;
    }

    /* If fft_len is smaller than the length of the source, the input is
     * cropped. If it is larger, the source is padded with zeros. */
    if (src_len > fft_len) {
        src_len = fft_len;
    }

    uai_mat_t* src_mat = uai_mat_create(1, fft_len);
    if (OS_NULL == src_mat) {
        return NUMDL_ENOMEM;
    }

    memcpy(src_mat->data, src, src_len * sizeof(float));

    int ret = NUMDL_EOK;

    ret = kiss_rfft(src_mat->data, out, fft_len, feat_len);

    uai_mat_destroy(src_mat);

    return ret;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif  // M_PI

/**
 * DCT type II, unscaled
 *
 * Based on https://www.nayuki.io/page/fast-discrete-cosine-transform-algorithms
 * fast-dct-fft.c
 *
 * Copyright (c) 2021, China Mobile Communications Group Co.,Ltd.
 * Copyright (c) 2017 Project Nayuki. (MIT License)
 *
 * @returns 0 if OK
 */
static int dct2_transform(float* data, os_size_t data_len)
{
    NUMDL_ASSERT(data != OS_NULL);

    float* fft_data_in = (float*)calloc(1, data_len * sizeof(float));
    if (fft_data_in == OS_NULL) {
        ERROR("Allocate fft_data_in buffer error.");
        return NUMDL_ENOMEM;
    }

    os_size_t fft_data_out_size = (data_len / 2 + 1) * sizeof(kiss_fft_cpx);
    kiss_fft_cpx* fft_data_out = (kiss_fft_cpx*)calloc(fft_data_out_size, 1);
    if (fft_data_out == OS_NULL) {
        ERROR("Allocate fft_data_out buffer error.");
        free(fft_data_in);
        return NUMDL_ENOMEM;
    }

    /** Preprocess the input buffer with the data */
    os_size_t half_len = data_len / 2;
    for (os_size_t i = 0; i < half_len; i++) {
        fft_data_in[i] = data[i * 2];
        fft_data_in[data_len - 1 - i] = data[i * 2 + 1];
    }
    if (data_len % 2 == 1) {
        fft_data_in[half_len] = data[data_len - 1];
    }

    int ret = NUMDL_EOK;

    ret = kiss_rfft_cpx(fft_data_in, fft_data_out, data_len);


    for (os_size_t i = 0; i < data_len / 2 + 1; i++) {
        float temp = i * M_PI / (data_len * 2);
        data[i] = fft_data_out[i].r * cos(temp) + fft_data_out[i].i * sin(temp);
    }

    free(fft_data_in);
    free(fft_data_out);

    return ret;
}

static int dct2_by_row(float* data, os_size_t length, dct_norm_t mode)
{
    NUMDL_ASSERT(data != OS_NULL);

    int ret = dct2_transform(data, length);
    if (ret != NUMDL_EOK) {
        return ret;
    }

    /** for some reason the output is 2x too low... */
    for (os_size_t i = 0; i < length; i++) {
        data[i] *= 2;
    }

    if (mode == DCT_NORMAL_ORTHO) {
        data[0] *= sqrt(1.0f / static_cast<float>(4 * length));
        for (os_size_t i = 1; i < length; i++) {
            data[i] *= sqrt(1.0f / static_cast<float>(2 * length));
        }
    }

    return NUMDL_EOK;
}

/**
 * Discrete Cosine Transform of arbitrary type sequence 2 on a matrix.
 *
 * @param mat  [in,out] input matrix
 * @param mode [in] DCT_NORMAL_ORTHO or DCT_NORMAL_NONE
 *
 * @returns 0 if OK
 */
int dsp::dct2(uai_mat_t* mat, dct_norm_t mode)
{
    NUMDL_ASSERT(mat != OS_NULL);

    for (os_size_t row = 0; row < mat->rows; row++) {
        int ret = dct2_by_row(&mat->data[row * mat->cols], mat->cols, mode);
        if (ret != NUMDL_EOK) {
            ERROR("DCT2 row %d failed.", row);
            return ret;
        }
    }

    return NUMDL_EOK;
}
};  // namespace feature
};  // namespace uai