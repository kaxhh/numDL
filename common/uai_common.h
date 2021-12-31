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
 * @file        uai_common.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-07-14   OneOS AI Team   First version
 *******************************************************************************
 */

#ifndef __UAI_COMMON__
#define __UAI_COMMON__

#include <oneos_config.h>

#define UAI_DEV_SENSOR_MAX_NUM (3)
#define UAI_DEV_NAME_MAX_SIZE  (32)
#define UAI_DEV_UID_MAX_SIZE   (32)

#define UAI_FLASH_SAMPLE_PART_NAME "sample"

#define UAI_SENSOR_NAME_MAX_SIZE (32)
#define UAI_SENSOR_FREQ_MAX_NUM  (3)

#define UAI_MIC_SAMPLE_TASK_STASK (2048)
#define UAI_MIC_SAMPLE_TASK_PRIO  (OS_TASK_PRIORITY_MAX / 3 - 1)

#ifdef OS_AUDIO_RECORD_FIFO_SIZE
#define UAI_MIC_SAMPLE_CHUNK_SIZE (OS_AUDIO_RECORD_FIFO_SIZE)
#else
#define UAI_MIC_SAMPLE_CHUNK_SIZE (320)
#endif

#define UAI_SAMPLE_READ_BUFF_SIZE (768)

#endif /* __UAI_COMMON__ */
