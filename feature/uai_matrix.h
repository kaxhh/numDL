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
 * @file        uai_matrix.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __UAI_MATRIX_H__
#define __UAI_MATRIX_H__

#include <os_stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct uai_mat_size
{
    os_size_t rows;
    os_size_t cols;
} uai_mat_size_t;

typedef struct uai_mat
{
    os_size_t rows;
    os_size_t cols;

    float* data;
} uai_mat_t;

uai_mat_t* uai_mat_create(os_size_t rows, os_size_t cols);
void uai_mat_destroy(uai_mat_t* mat);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UAI_MATRIX_H__ */
