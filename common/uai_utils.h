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
 * @file        uai_utils.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-07-14   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __UAI_UTILS_H__
#define __UAI_UTILS_H__

#include <stdlib.h>

int uai_base64_encode(const char *input,
                      size_t input_size,
                      char *output,
                      size_t output_size);

#endif /* __UAI_UTILS_H__ */
