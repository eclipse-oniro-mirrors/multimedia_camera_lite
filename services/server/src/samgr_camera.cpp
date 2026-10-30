/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include <ohos_errno.h>
#include <ohos_init.h>
#include <cstdio>

#include "feature.h"
#include "media_log.h"
#include "iproxy_client.h"
#include "iproxy_server.h"
#include "iunknown.h"
#include "samgr_lite.h"
#include "service.h"

#include "camera_type.h"
#include "camera_server.h"

namespace OHOS {
struct DefaultFeatureApi {
    INHERIT_SERVER_IPROXY;
};

struct CameraService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(DefaultFeatureApi);
    Identity identity;
};

static const char* GetName(Service* service)
{
    (void)service;
    return Media::SERVICE_NAME;
}

static BOOL Initialize(Service* service, Identity identity)
{
    if (service == nullptr) {
        return FALSE;
    }
    CameraService* example = (CameraService*)service;
    example->identity = identity;
    MEDIA_INFO_LOG("Initialize(%s)! Identity<%d, %d>", Media::SERVICE_NAME,
        identity.serviceId, identity.featureId);
    return TRUE;
}

static BOOL MessageHandle(Service* service, Request* msg)
{
    if (service == nullptr || msg == nullptr) {
        return FALSE;
    }
    MEDIA_INFO_LOG("MessageHandle(%s)! Request<%d, %d>",
        service->GetName(service), msg->msgId, msg->msgValue);
    return FALSE;
}

static TaskConfig GetTaskConfig(Service* service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_BELOW_NORMAL, 0x800, 20, SHARED_TASK};
    return config;
}

static int32 Invoke(IServerProxy* iProxy, int funcId, void* origin, IpcIo* req, IpcIo* reply)
{
    Media::CameraServer::CameraServerRequestHandle(funcId, origin, req, reply);
    return EC_SUCCESS;
}

// C++11 兼容：原本使用 C++20 指定初始化器（designated initializer），
// 现改为默认构造 + 逐字段赋值，字段顺序与 CameraService 声明顺序一致。
static CameraService g_example = {};

static void InitCameraService(void)
{
    g_example.GetName = GetName;
    g_example.Initialize = Initialize;
    g_example.MessageHandle = MessageHandle;
    g_example.GetTaskConfig = GetTaskConfig;
    g_example.ver = SERVER_IMPL_PROXY_VER;
    g_example.ref = 1;
    g_example.iUnknown.QueryInterface = IUNKNOWN_QueryInterface;
    g_example.iUnknown.AddRef = IUNKNOWN_AddRef;
    g_example.iUnknown.Release = IUNKNOWN_Release;
    g_example.iUnknown.Invoke = Invoke;
}

static void Init(void)
{
    InitCameraService();
    SAMGR_GetInstance()->RegisterService((Service*)&g_example);
    SAMGR_GetInstance()->RegisterDefaultFeatureApi(Media::SERVICE_NAME, GET_IUNKNOWN(g_example));
}

SYSEX_SERVICE_INIT(Init);
} // namespace OHOS