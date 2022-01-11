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
 * @file        nd_log.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __NUMDL_LOG_H__
#define __NUMDL_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(NUMDL_LOG_TAG)
#define NUMDL_LOG_TAG "numDL"
#endif

#if !defined(NUMDL_LOG_LVL)
#define NUMDL_LOG_LVL NUMDL_LOG_INFO
#endif

#define NUMDL_LOG_ERROR   (3) /* Error conditions */
#define NUMDL_LOG_WARNING (4) /* Warning conditions */
#define NUMDL_LOG_INFO    (6) /* Informational */
#define NUMDL_LOG_DEBUG   (7) /* Debug-level messages */

#if (NUMDL_LOG_ERROR <= NUMDL_LOG_LVL)
#define ERROR(fmt, ...)                                                        \
    printf("[ERROR] [%s] " fmt "\r\n", NUMDL_LOG_TAG, ##__VA_ARGS__);
#else
#define ERROR(fmt, ...)
#endif

#if (NUMDL_LOG_WARNING <= NUMDL_LOG_LVL)
#define WARN(fmt, ...)                                                         \
    printf("[WARN] [%s] " fmt "\r\n", NUMDL_LOG_TAG, ##__VA_ARGS__);
#else
#define WARN(fmt, ...)
#endif

#if (NUMDL_LOG_INFO <= NUMDL_LOG_LVL)
#define INFO(fmt, ...)                                                         \
    printf("[INFO] [%s] " fmt "\r\n", NUMDL_LOG_TAG, ##__VA_ARGS__);
#else
#define INFO(fmt, ...)
#endif

#if (NUMDL_LOG_DEBUG <= NUMDL_LOG_LVL)
#define DEBUG(fmt, ...)                                                        \
    printf("[DEBUG] [%s] " fmt "\r\n", NUMDL_LOG_TAG, ##__VA_ARGS__);
#else
#define DEBUG(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NUMDL_LOG_H__ */
