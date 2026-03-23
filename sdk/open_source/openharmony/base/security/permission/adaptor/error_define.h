/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PERM_ERROR_CODE_H
#define PERM_ERROR_CODE_H

enum PmsErrorCode {
    PERM_ERRORCODE_SUCCESS = 0,
    PERM_ERRORCODE_INVALID_PARAMS = 10,
    PERM_ERRORCODE_INVALID_PERMNAME,
    PERM_ERRORCODE_MALLOC_FAIL,
    PERM_ERRORCODE_OPENFD_FAIL,
    PERM_ERRORCODE_READFD_FAIL,
    PERM_ERRORCODE_WRITEFD_FAIL,
    PERM_ERRORCODE_JSONPARSE_FAIL,
    PERM_ERRORCODE_COPY_ERROR,
    PERM_ERRORCODE_FIELD_TOO_LONG,
    PERM_ERRORCODE_PERM_NOT_EXIST,
    PERM_ERRORCODE_UNLINK_ERROR,
    PERM_ERRORCODE_FILE_NOT_EXIST,
    PERM_ERRORCODE_MEMSET_FAIL,
    PERM_ERRORCODE_STAT_FAIL,
    PERM_ERRORCODE_PATH_INVALID,
    PERM_ERRORCODE_TOO_MUCH_PERM,
    PERM_ERRORCODE_TASKID_NOT_EXIST,
    PERM_ERRORCODE_PERM_NUM_ERROR,
};

#endif // PERM_ERROR_CODE_H