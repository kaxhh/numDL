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
 * @file        nd_errno.h
 *
 * @revision
 * Date         Author          Notes
 * 2021-09-07   OneOS AI Team   First Version
 *******************************************************************************
 */

#ifndef __NUMDL_ERRNO_H__
#define __NUMDL_ERRNO_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NUMDL_EOK      (0)  /* There is no error. */
#define NUMDL_ERROR    (1)  /* A generic error happens. */
#define NUMDL_ETIMEOUT (2)  /* Timed out. */
#define NUMDL_EFULL    (3)  /* The resource is full. */
#define NUMDL_EEMPTY   (4)  /* The resource is empty. */
#define NUMDL_ENOMEM   (5)  /* No memory. */
#define NUMDL_ENOSYS   (6)  /* Function not implemented. */
#define NUMDL_EBUSY    (7)  /* Busy. */
#define NUMDL_EIO      (8)  /* IO error. */
#define NUMDL_EINTR    (9)  /* Interrupted system call. */
#define NUMDL_EINVAL   (10) /* Invalid argument. */
#define NUMDL_ENODEV   (11) /* No such device */
#define NUMDL_EPERM    (12) /* Operation not permitted */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NUMDL_ERRNO_H__ */
