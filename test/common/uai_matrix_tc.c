/**
 *******************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
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
 * @file        uai_metrix_tc.c
 *
 * @revision
 * Date         Author          Notes
 * 2020-11-18   OneOS Team      First Version
 *******************************************************************************
 */
#include "uai_matrix.h"

#include <atest.h>

#include <os_errno.h>

#define MAT_MAX_ROWS (10)
#define MAT_MAX_COLS (10)

static void do_mat_create_and_destory(os_size_t rows, os_size_t cols)
{
    uai_mat_t* mat = uai_mat_create(rows, cols);

    tp_assert_not_null(mat);
    tp_assert_not_null(mat->data);
    tp_assert_integer_equal(rows, mat->rows);
    tp_assert_integer_equal(cols, mat->cols);

    uai_mat_destroy(mat);
}

static void test_mat_create_and_destory(void)
{
    for (os_size_t rows = 1; rows <= MAT_MAX_ROWS; rows++) {
        for (os_size_t cols = 1; cols <= MAT_MAX_COLS; cols++) {
            do_mat_create_and_destory(rows, cols);
        }
    }
}

static void test_case(void)
{
    ATEST_UNIT_RUN(test_mat_create_and_destory);
    return;
}

static os_err_t test_init(void)
{
    return OS_EOK;
}

static os_err_t test_cleanup(void)
{
    return OS_EOK;
}

ATEST_TC_EXPORT(uai_sdk.matrix.tc,
                test_case,
                test_init,
                test_cleanup,
                TC_PRIORITY_MIDDLE);