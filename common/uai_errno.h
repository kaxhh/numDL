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
 * @file        uai_errno.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-07   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __UAI_ERRNO_H__
#define __UAI_ERRNO_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UAI_EOK      (0)  /* There is no error. */
#define UAI_ERROR    (1)  /* A generic error happens. */
#define UAI_ETIMEOUT (2)  /* Timed out. */
#define UAI_EFULL    (3)  /* The resource is full. */
#define UAI_EEMPTY   (4)  /* The resource is empty. */
#define UAI_ENOMEM   (5)  /* No memory. */
#define UAI_ENOSYS   (6)  /* Function not implemented. */
#define UAI_EBUSY    (7)  /* Busy. */
#define UAI_EIO      (8)  /* IO error. */
#define UAI_EINTR    (9)  /* Interrupted system call. */
#define UAI_EINVAL   (10) /* Invalid argument. */
#define UAI_ENODEV   (11) /* No such device */
#define UAI_EPERM    (12) /* Operation not permitted */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UAI_ERRNO_H__ */
