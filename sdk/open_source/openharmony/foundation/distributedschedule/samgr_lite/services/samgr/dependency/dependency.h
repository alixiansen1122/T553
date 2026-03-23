/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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
#ifndef OHOS_DEPENDENCY_H
#define OHOS_DEPENDENCY_H

#include "common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
	const char *service;
	uint8 priority;
}ServicePriority;

/*
 * Get high Priority Services from services in Vector
 * return the amount of remain services in service list.
 */
uint32 GetHighPriorityServices(Vector *vec);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // OHOS_DEPENDENCY_H
