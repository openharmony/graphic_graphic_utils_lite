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

#ifndef GRAPHIC_LITE_GRAPHIC_MUTEX_H
#define GRAPHIC_LITE_GRAPHIC_MUTEX_H
#include "stdbool.h"
#include "stdint.h"
#ifdef _WIN32
#include <windows.h>
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
#include <pthread.h>
#else
#include "cmsis_os2.h"   // LiteOS‑M 使用CMSIS‑RTOS2
#endif // WIN32
#include "gfx_utils/heap_base.h"

namespace OHOS {
/** @brief graphic mutex adapter for different platform. */
class GraphicMutex : public HeapBase {
public:
    /** Default constructor */
    GraphicMutex()
    {
#ifdef _WIN32
        mutex_ = CreateMutex(NULL, FALSE, NULL);
        initFlag_ = (mutex_ != NULL);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        initFlag_ = (pthread_mutex_init(&mutex_, NULL) == 0);
#else
        mutex_ = osMutexNew(NULL);
        initFlag_ = (mutex_ != NULL);
#endif // WIN32
    }

    /** Default destructor */
    ~GraphicMutex()
    {
        if (!initFlag_) {
            return;
        }
#ifdef _WIN32
        CloseHandle(mutex_);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        pthread_mutex_destroy(&mutex_);
#else
        osMutexDelete(mutex_);
#endif // WIN32
    }

    inline bool Lock()
    {
        if (!initFlag_) {
            return false;
        }
#ifdef _WIN32
        return (WaitForSingleObject(mutex_, INFINITE) == WAIT_OBJECT_0);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        return (pthread_mutex_lock(&mutex_) == 0);
#else
        return (osMutexAcquire(mutex_, osWaitForever) == osOK);
#endif // WIN32
    }

    inline bool Unlock()
    {
        if (!initFlag_) {
            return false;
        }
#ifdef _WIN32
        return ReleaseMutex(mutex_);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        return (pthread_mutex_unlock(&mutex_) == 0);
#else
        return (osMutexRelease(mutex_) == osOK);
#endif // WIN32
    }

private:
    bool initFlag_;
#ifdef _WIN32
    HANDLE mutex_;
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_t mutex_;
#else
     osMutexId_t mutex_;
#endif // WIN32
};
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_MUTEX_H
