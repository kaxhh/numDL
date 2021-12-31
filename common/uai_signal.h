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
 * @file        uai_signal.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-08   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __UAI_SIGNAL_H__
#define __UAI_SIGNAL_H__

#include <os_stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct uai_signal
{
    /**
     * A function to retrieve part of the sensor signal
     *
     * No bytes will be requested outside of the `total_length`.
     *
     * @param offset   [in]  The offset in the signal
     * @param length   [in]  The length of the part signal
     * @param out_buff [out] An out buffer to set the signal data
     *
     * @return         The actually read size on successful, otherwise negative
     *                 returned.
     */
    int (*get_data)(os_size_t offset, os_size_t size, float *out_buff);

    os_size_t total_length; /** The total length of the signal */
} uai_signal_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UAI_SIGNAL_H__ */
