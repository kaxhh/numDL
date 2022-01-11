/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on 
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 *
 * @file        nd_assert.h
 *
 * @brief       Support assert functions
 *
 * @revision
 * Date         Author          Notes
 * 2022-01-11   OneOS AI Team      First version
 ***********************************************************************************************************************
 */

#ifndef __NUMDL_ASSERT_H__
#define __NUMDL_ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NUMDL_USING_ASSERT
#define NUMDL_ASSERT(condition)                                                                                \
do                                                                                                          \
{                                                                                                           \
    if (!(condition))                                                                                       \
    {                                                                                                       \
        printf("Assert failed. Condition(%s). [%s][%d]\r\n", #condition, __FUNCTION__, __LINE__);       \
        while(1)                                                                                            \
        {                                                                                                   \
            ;                                                                                               \
        }                                                                                                   \
    }                                                                                                       \
} while (0)

#define NUMDL_ASSERT_EX(condition, fmt, ...)                                                                   \
    do                                                                                                      \
    {                                                                                                       \
        if (!(condition))                                                                                   \
        {                                                                                                   \
            printf("Assert failed. " fmt " [%s][%d]\r\n", ##__VA_ARGS__, __FUNCTION__, __LINE__);       \
            while (1)                                                                                       \
            {                                                                                               \
                ;                                                                                           \
            }                                                                                               \
        }                                                                                                   \
    } while (0)

#else
#define NUMDL_ASSERT(condition)
#define NUMDL_ASSERT_EX(condition, fmt, ...)
#endif /* NUMDL_USING_ASSERT */

#ifdef __cplusplus
}
#endif

#endif /* __NUMDL_ASSERT_H__ */

