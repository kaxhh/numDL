/**
 *******************************************************************************
 * Copyright (c) 2021, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *use this file except in compliance with the License. You may obtain a copy of
 *the License at
 *
 *     http://www.apache.org/licenses/LICENSE2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 *distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *License for the specific language governing permissions and limitations under
 *the License.
 *
 * @file        yes_30ms_testdata.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */
#ifndef __YES_30MS_TESTDATA_H__
#define __YES_30MS_TESTDATA_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <os_stddef.h>

#define YES_30MS_DATA_SIZE (480)

extern const os_int16_t g_yes_30ms_int16[];
extern const float g_yes_30ms_float[];
extern const float g_yes_30ms_preemphasis[];
extern const float g_yes_30ms_fft[];
extern const float g_yes_30ms_power_fft[];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __YES_30MS_TESTDATA_H__ */