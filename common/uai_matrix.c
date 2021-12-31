/**
 *******************************************************************************
 * Copyright (c) 2021, China Mobile Communications Group Co.,Ltd.
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
 * @file        uai_matrix.c
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#include "uai_matrix.h"

#define UAI_LOG_TAG "uai.mat"
#include "uai_log.h"

#include <os_memory.h>
#include <os_assert.h>

/**
 * Create a matrix of size [rows x cols]
 *
 * @param rows [in] Number of rows of the matrix
 * @param cols [in] Number of cols of the matrix
 *
 * @returns Pointer to matrix if OK
 */
uai_mat_t* uai_mat_create(os_size_t rows, os_size_t cols)
{
    OS_ASSERT(rows > 0 && cols > 0);

    uai_mat_t* mat = (uai_mat_t*)os_calloc(1, sizeof(uai_mat_t));
    if (OS_NULL == mat) {
        ERROR("Create matrix instance failed, no enough memory.");
        return OS_NULL;
    }

    os_size_t buff_size = sizeof(float) * rows * cols;

    float* buff = (float*)os_calloc(1, buff_size);
    if (OS_NULL == buff) {
        ERROR("Alloc matrix data buffer(%d bytes) failed, no enough memory.",
              buff_size);
        os_free(mat);
        return OS_NULL;
    }

    mat->rows = rows;
    mat->cols = cols;
    mat->data = buff;

    return mat;
}

/**
 * Destory a matrix
 *
 * @param mat [in] Pointer to matrix
 */
void uai_mat_destroy(uai_mat_t* mat)
{
    OS_ASSERT(mat != OS_NULL);

    if (mat->data != OS_NULL) {
        os_free(mat->data);
    }

    os_free(mat);
}
