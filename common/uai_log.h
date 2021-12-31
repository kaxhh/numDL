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
 * @file        uai_log.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __UAI_LOG_H__
#define __UAI_LOG_H__

#include <oneos_config.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(UAI_LOG_TAG)
#define UAI_LOG_TAG "uai"
#endif

#ifdef OS_USING_DLOG
#include <dlog.h>

/* Debug contorol by dlog config */
#define ERROR(fmt, ...) LOG_E(UAI_LOG_TAG, fmt, ##__VA_ARGS__)

#define WARN(fmt, ...) LOG_W(UAI_LOG_TAG, fmt, ##__VA_ARGS__)

#define INFO(fmt, ...) LOG_I(UAI_LOG_TAG, fmt, ##__VA_ARGS__)

#define DEBUG(fmt, ...) LOG_D(UAI_LOG_TAG, fmt, ##__VA_ARGS__)

#else /* Not define OS_USING_DLOG, using kernel printf */
#include <os_util.h>

#if !defined(UAI_LOG_LVL)
#define UAI_LOG_LVL UAI_LOG_INFO
#endif

#define UAI_LOG_ERROR   (3) /* Error conditions */
#define UAI_LOG_WARNING (4) /* Warning conditions */
#define UAI_LOG_INFO    (6) /* Informational */
#define UAI_LOG_DEBUG   (7) /* Debug-level messages */

#if (UAI_LOG_ERROR <= UAI_LOG_LVL)
#define ERROR(fmt, ...)                                                        \
    os_kprintf("[ERROR] [%s] " fmt "\r\n", UAI_LOG_TAG, ##__VA_ARGS__);
#else
#define ERROR(fmt, ...)
#endif

#if (UAI_LOG_WARNING <= UAI_LOG_LVL)
#define WARN(fmt, ...)                                                         \
    os_kprintf("[WARN] [%s] " fmt "\r\n", UAI_LOG_TAG, ##__VA_ARGS__);
#else
#define WARN(fmt, ...)
#endif

#if (UAI_LOG_INFO <= UAI_LOG_LVL)
#define INFO(fmt, ...)                                                         \
    os_kprintf("[INFO] [%s] " fmt "\r\n", UAI_LOG_TAG, ##__VA_ARGS__);
#else
#define INFO(fmt, ...)
#endif

#if (UAI_LOG_DEBUG <= UAI_LOG_LVL)
#define DEBUG(fmt, ...)                                                        \
    os_kprintf("[DEBUG] [%s] " fmt "\r\n", UAI_LOG_TAG, ##__VA_ARGS__);
#else
#define DEBUG(fmt, ...)
#endif

#endif /* OS_USING_DLOG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UAI_LOG_H__ */
