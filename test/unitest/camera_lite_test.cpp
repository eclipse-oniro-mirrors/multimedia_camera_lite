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

#include "camera_lite_test.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;
using namespace testing::ext;

/* *
 * get current dir
 * @return  string current file path of the test suits
 */
string GetCurDir()
{
    string filePath = "";
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) == NULL) {
        perror("get file path error");
    } else {
        printf("Current Dir: %s\r\n", buffer);
        filePath = buffer;
        free(buffer);
    }
    return filePath + "/";
}

// SetUpTestCase
void CameraLiteTest::SetUpTestCase(void)
{
    g_testPath = GetCurDir();
    cout << "SetUpTestCase" << endl;
}

// TearDownTestCase
void CameraLiteTest::TearDownTestCase(void)
{
    g_testPath = "";
    cout << "TearDownTestCase" << endl;
}

void CameraLiteTest::SetUp(void)
{
    // CameraSetUp
    g_onGetCameraAbilityFlag = false;
    g_onConfigureFlag = false;
    g_onGetSupportedSizesFlag = false;
    // CameraDeviceCallBack
    g_onCameraAvailableFlag = false;
    g_onCameraUnavailableFlag = false;
    // CameraStateCallback
    g_onCreatedFlag = false;
    g_onCreateFailedFlag = false;
    g_onConfiguredFlag = false;
    g_onConfigureFailedFlag = false;
    g_onReleasedFlag = false;
    // FrameStateCallback
    g_onCaptureTriggerAbortedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    g_onCaptureTriggerStartedFlag = false;
    g_onFrameFinishedFlag = false;
    g_onGetFrameConfigureType = false;
    g_onFrameErrorFlag = false;
    g_onFrameProgressedFlag = false;
    g_onFrameStartedFlag = false;
    cout << "SetUp" << endl;
}

// Tear down
void CameraLiteTest::TearDown(void)
{
    cout << "TearDown." << endl;
}

/* *
 * creat Recorder
 */
Recorder *SampleCreateRecorder()
{
    int ret = 0;
    int32_t sampleRate = 48000;
    int32_t channelCount = 1;
    AudioCodecFormat audioFormat = AAC_LC;
    AudioSourceType inputSource = AUDIO_MIC;
    int32_t audioEncodingBitRate = sampleRate;
    VideoSourceType source = VIDEO_SOURCE_SURFACE_ES;
    int32_t frameRate = 30;
    double fps = 30;
    int32_t rate = 4096;
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    int32_t width = 1920;
    int32_t height = 1080;
    VideoCodecFormat encoder;
    encoder = HEVC;
    Recorder *recorder = new Recorder();
    if ((ret = recorder->SetVideoSource(source, sourceId)) != SUCCESS) {
        cout << "SetVideoSource failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoEncoder(sourceId, encoder)) != SUCCESS) {
        cout << "SetVideoEncoder failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoSize(sourceId, width, height)) != SUCCESS) {
        cout << "SetVideoSize failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoFrameRate(sourceId, frameRate)) != SUCCESS) {
        cout << "SetVideoFrameRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoEncodingBitRate(sourceId, rate)) != SUCCESS) {
        cout << "SetVideoEncodingBitRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetCaptureRate(sourceId, fps)) != SUCCESS) {
        cout << "SetCaptureRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioSource(inputSource, audioSourceId)) != SUCCESS) {
        cout << "SetAudioSource failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioEncoder(audioSourceId, audioFormat)) != SUCCESS) {
        cout << "SetAudioEncoder failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate)) != SUCCESS) {
        cout << "SetAudioSampleRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioChannels(audioSourceId, channelCount)) != SUCCESS) {
        cout << "SetAudioChannels failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate)) != SUCCESS) {
        cout << "SetAudioEncodingBitRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    return recorder;
}

/* *
 * Create Frame StateCallback
 */
class SampleFrameStateCallback : public FrameStateCallback {
    /* *
     * check file exist
     * @param filename filename
     * @return  check result
     */
    int32_t FileCheck(const char *filename)
    {
        fstream fileTmp;
        fileTmp.open(filename);
        if (!fileTmp) {
            cout << "file is not exist!" << endl;
            return RET_ERR;
        } else {
            cout << "file is exist!" << endl;
            fileTmp.close();
            return RET_OK;
        }
    }

    /* *
    * Save Capture
    * @return
    */
    int32_t SampleSaveCapture(string testPath, const char *p, uint32_t size)
    {
        cout << "Start saving picture" << endl;
        string filePath = "";
        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm *ltm = localtime(&tv.tv_sec);
        if (ltm != nullptr) {
            ostringstream ss("Capture_");
            ss << "Capture" << ltm->tm_hour << "_" << ltm->tm_min << "_" << ltm->tm_sec << ".jpg";
            filePath = testPath + ss.str();
            ofstream pic(testPath + ss.str(), ofstream::out | ofstream::trunc);
            cout << "write " << size << " bytes" << endl;
            pic.write(p, size);
            cout << "Saving picture end" << endl;
        }
        const char *filename = filePath.data();
        int32_t ret = FileCheck(filename);
        return ret;
    }

    void OnFrameFinished(Camera &camera, FrameConfig &fc, FrameResult &result) override
    {
        g_onFrameStartedFlag = true;
        g_onFrameProgressedFlag = true;
        cout << "Receive frame complete inform." << endl;
        if (fc.GetFrameConfigType() == FRAME_CONFIG_CAPTURE) {
            g_onGetFrameConfigureType = true;
            cout << "Capture frame received." << endl;
            list<Surface *> surfaceList = fc.GetSurfaces();
            for (Surface *surface : surfaceList) {
                SurfaceBuffer *buffer = surface->AcquireBuffer();
                if (buffer != nullptr) {
                    char *virtAddr = static_cast<char *>(buffer->GetVirAddr());
                    if (virtAddr != nullptr) {
                        SampleSaveCapture(g_testPath, virtAddr, buffer->GetSize());
                    } else {
                        g_onFrameErrorFlag = true;
                    }
                    surface->ReleaseBuffer(buffer);
                } else {
                    g_onFrameErrorFlag = true;
                }
                delete surface;
            }
            delete &fc;
        } else {
            g_onFrameErrorFlag = true;
        }
        g_onFrameFinishedFlag = true;
    }
};

/* *
 * create CameraStateCallback
 */
class SampleCameraStateMng : public CameraStateCallback {
public:
    SampleCameraStateMng() = delete;

    explicit SampleCameraStateMng(EventHandler &eventHdlr) : eventHandler_(eventHdlr) {}

    ~SampleCameraStateMng()
    {
        if (recorder_ != nullptr) {
            recorder_->Release();
            delete recorder_;
        }
        if (cam_ != nullptr) {
            cam_->Release();
        }
    }

    void OnCreated(Camera &c) override
    {
        g_onCreatedFlag = true;
        cout << "Sample recv OnCreate camera." << endl;
        auto config = CameraConfig::CreateCameraConfig();
        config->SetFrameStateCallback(&fsCb_, &eventHandler_);
        c.Configure(*config);
        g_onConfigureFlag = true;
        cam_ = &c;
    }

    void OnCreateFailed(const std::string cameraId, int32_t errorCode) override
    {
        g_onCreateFailedFlag = true;
        cout << "Sample recv OnCreateFailed camera." << endl;
    }

    void OnReleased(Camera &c) override
    {
        g_onReleasedFlag = true;
        cout << "Sample recv OnReleased camera." << endl;
    }

    void StartRecord()
    {
        int ret;
        if (isRecording_) {
            cout << "Camera is already recording." << endl;
            return;
        }
        if (recorder_ == nullptr) {
            recorder_ = SampleCreateRecorder();
        }
        if (recorder_ == nullptr) {
            cout << "Recorder not available" << endl;
            return;
        }
        string path = GetCurDir();
        ret = recorder_->SetOutputPath(path);
        if (ret != SUCCESS) {
            cout << "SetOutputPath failed :" << ret << std::endl;
            return;
        }

        ret = recorder_->Prepare();
        if (ret != SUCCESS) {
            cout << "Prepare failed.=" << ret << endl;
            return;
        }
        Surface *surface = (recorder_->GetSurface(0)).get();
        surface->SetWidthAndHeight(WIDTH, HEIGHT);
        surface->SetQueueSize(QUEUE_SIZE);
        surface->SetSize(BUFFER_SIZE * BUFFER_SIZE);
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_RECORD);
        fc->AddSurface(*surface);
        ret = recorder_->Start();
        if (ret != SUCCESS) {
            delete fc;
            cout << "recorder start failed. ret=" << ret << endl;
            return;
        }
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }

        ret = cam_->TriggerLoopingCapture(*fc);
        if (ret != 0) {
            delete fc;
            cout << "camera start recording failed. ret=" << ret << endl;
            return;
        }
        isRecording_ = true;
        cout << "camera start recording succeed." << endl;
    }

    void StartPreview()
    {
        if (isPreviewing_) {
            cout << "Camera is already previewing." << endl;
            return;
        }
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_PREVIEW);
        Surface *surface = Surface::CreateSurface();
        if (surface == nullptr) {
            delete fc;
            cout << "CreateSurface failed" << endl;
            return;
        }
        surface->SetWidthAndHeight(WIDTH, HEIGHT);
        surface->SetUserData("region_position_x", "0");
        surface->SetUserData("region_position_y", "0");
        surface->SetUserData("region_width", "480");
        surface->SetUserData("region_height", "480");
        fc->AddSurface(*surface);
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }
        int32_t ret = cam_->TriggerLoopingCapture(*fc);
        if (ret != 0) {
            delete fc;
            cout << "camera start preview failed. ret=" << ret << endl;
            return;
        }
        g_onCaptureTriggerCompletedFlag = true;
        delete surface;
        isPreviewing_ = true;
        cout << "camera start preview succeed." << endl;
    }

    void Capture()
    {
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
        Surface *surface = Surface::CreateSurface();
        if (surface == nullptr) {
            delete fc;
            cout << "CreateSurface failed" << endl;
            return;
        }
        surface->SetWidthAndHeight(1920, 1080); /* 1920:width,1080:height */
        fc->AddSurface(*surface);
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }
        g_onCaptureTriggerStartedFlag = true;
        cam_->TriggerSingleCapture(*fc);
        g_onCaptureTriggerCompletedFlag = true;
    }

    void Stop()
    {
        if (recorder_ != nullptr) {
            recorder_->Stop(false);
        }

        while (cam_ == nullptr) {
            cout << "Camera is not ready." << endl;
            return;
        }
        cam_->StopLoopingCapture();
        isPreviewing_ = false;
        isRecording_ = false;
    }

    bool isPreviewing_ = false;
    bool isRecording_ = false;
    EventHandler &eventHandler_;
    Camera *cam_ = nullptr;
    Recorder *recorder_ = nullptr;
    SampleFrameStateCallback fsCb_;
};

/* *
 * create CameraStateCallback for state test
 */
class SampleCameraStateCallback : public CameraStateCallback {
public:
    SampleCameraStateCallback() = delete;

    explicit SampleCameraStateCallback(EventHandler &eventHdlr) : eventHandler_(eventHdlr) {}

    ~SampleCameraStateCallback()
    {
        if (cam_ != nullptr) {
            cam_->Release();
        }
    }

    void OnCreated(Camera &c) override
    {
        g_onCreatedFlag = true;
        cout << "camera Create success." << endl;
        auto config = CameraConfig::CreateCameraConfig();
        config->SetFrameStateCallback(&fsCb_, &eventHandler_);
        c.Configure(*config);
        cam_ = &c;
    }

    void OnCreateFailed(const std::string cameraId, int32_t errorCode) override
    {
        g_onCreateFailedFlag = true;
        cout << "Camera Create Failed." << endl;
    }

    void OnReleased(Camera &c) override
    {
        g_onReleasedFlag = true;
        cout << "camera Releasedsuccess." << endl;
    }

    void OnConfigured(Camera &c) override
    {
        g_onConfiguredFlag = true;
        cout << "Camera Configured success." << endl;
    }

    void OnConfigureFailed(const std::string cameraId, int32_t errorCode) override
    {
        g_onConfigureFailedFlag = true;
        cout << "Camera Configured failed." << endl;
    }

    EventHandler &eventHandler_;
    Camera *cam_ = nullptr;
    SampleFrameStateCallback fsCb_;
};

/* *
 * Creat camera device callback
 */
class SampleCameraDeviceCallback : public CameraDeviceCallback {
public:
    SampleCameraDeviceCallback() {}

    ~SampleCameraDeviceCallback() {}

    // camera device status changed
    void OnCameraStatus(std::string cameraId, int32_t status) override
    {
        cout << "SampleCameraDeviceCallback OnCameraStatus\n" << endl;
        if (status == CAMERA_DEVICE_STATE_AVAILABLE) {
            g_onCameraAvailableFlag = true;
            cout << "SampleCameraDeviceCallback onCameraAvailable\n" << endl;
        } else if (status == CAMERA_DEVICE_STATE_UNAVAILABLE) {
            g_onCameraUnavailableFlag = true;
            cout << "SampleCameraDeviceCallback onCameraUnavailable\n" << endl;
        }
    }
};

/* *
 * Get camera Id
 */
void GetCameraId(CameraKit *cameraKit, list<string> &camList, string &camId)
{
    cameraKit = CameraKit::GetInstance();
    camList = cameraKit->GetCameraIds();
    for (auto &cam : camList) {
        cout << "camera name:" << cam << endl;
        const CameraAbility *ability = cameraKit->GetCameraAbility(cam);
        EXPECT_NE(ability, nullptr);
        g_onGetCameraAbilityFlag = true;
        /* find camera which fits user's ability */
        list<CameraPicSize> sizeList = ability->GetSupportedSizes(0);
        if (sizeList.size() != 0) {
            g_onGetSupportedSizesFlag = true;
        }
        for (auto &pic : sizeList) {
            cout << "Pic size: " << pic.width << "x" << pic.height << endl;
            if (pic.width == WIDTH && pic.height == HEIGHT) {
                /* 1920:width,1080:height */
                camId = cam;
                break;
            }
        }
    }
    if (camId.empty()) {
        cout << "No available camera.(1080p wanted)" << endl;
        return;
    }
}

using namespace OHOS;
using namespace std;

/*
 * Feature: CamerLite
 * Function: GetCameraID
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription:  Get camera ID Test.
 */
HWTEST_F(CameraLiteTest, Test_GetCameraIDs, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    EXPECT_FALSE(camId.empty());
    EXPECT_EQ("main", camId);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: GetCameraAbility
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription:  Get camera ability Test.
 */
HWTEST_F(CameraLiteTest, Test_GetCameraAbility, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    EXPECT_EQ(g_onGetCameraAbilityFlag, true);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: CreatCamerakit
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription:  create camerakit instance Test.
 */
HWTEST_F(CameraLiteTest, TestCreatCamerakit, Level1)
{
    CameraKit *cameraKit = nullptr;
    cameraKit = CameraKit::GetInstance();
    EXPECT_NE(nullptr, cameraKit);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: SampleCameraDeviceCallback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Get cameraKit deviceCallback
 */
HWTEST_F(CameraLiteTest, TestNewDeviceCallback, Level1)
{
    SampleCameraDeviceCallback *deviceCallback = nullptr;
    deviceCallback = new SampleCameraDeviceCallback();
    EXPECT_NE(nullptr, deviceCallback);
    delete deviceCallback;
    deviceCallback = nullptr;
}

/*
 * Feature: CamerLite
 * Function: GetSupportedSizes
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Get cameraKit supported Size
 */
HWTEST_F(CameraLiteTest, TestGetSupportedSizes, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    EXPECT_EQ(g_onGetSupportedSizesFlag, true);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: RegisterCameraDeviceCallback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Register Camera device callback
 */
HWTEST_F(CameraLiteTest, Test_RegisterCameraDeviceCallback, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    SampleCameraDeviceCallback *deviceCallback = nullptr;
    deviceCallback = new SampleCameraDeviceCallback();
    EXPECT_NE(nullptr, deviceCallback);
    cameraKit->RegisterCameraDeviceCallback(*deviceCallback, eventHdlr);
    sleep(1);
    EXPECT_EQ(g_onCameraAvailableFlag, true);
    delete deviceCallback;
    deviceCallback = nullptr;
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: RegisterCameraDeviceCallback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Unregister Camera Device Callback
 */
HWTEST_F(CameraLiteTest, Test_UnregisterCameraDeviceCallback, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    SampleCameraDeviceCallback *deviceCallback = nullptr;
    deviceCallback = new SampleCameraDeviceCallback();
    EXPECT_NE(nullptr, deviceCallback);
    cameraKit->RegisterCameraDeviceCallback(*deviceCallback, eventHdlr);
    sleep(1);
    EXPECT_EQ(g_onCameraAvailableFlag, true);
    cameraKit->UnregisterCameraDeviceCallback(*deviceCallback);
    sleep(1);
    EXPECT_EQ(g_onCameraUnavailableFlag, false);
    delete deviceCallback;
    deviceCallback = nullptr;
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: CreateCamera
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Capture On Configure
 */
HWTEST_F(CameraLiteTest, TestCaptureOnConfigure, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(1);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(1);
    EXPECT_EQ(g_onConfigureFlag, true);
    EXPECT_NE(camStateMng.cam_, nullptr);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: CreateCamera
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Create Camera
 */
HWTEST_F(CameraLiteTest, TestCreateCamera, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    EXPECT_EQ(g_onCreatedFlag, true);
    EXPECT_NE(g_onCreateFailedFlag, true);
    EXPECT_NE(camStateMng.cam_, nullptr);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: CreateCamera
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Create Camera with error camera id
 */
HWTEST_F(CameraLiteTest, TestCapture_Create_Camera_ERROR_cameraId, Level1)
{
    CameraKit *cameraKit = nullptr;
    cameraKit = CameraKit::GetInstance();
    string camId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    EXPECT_EQ(g_onCreatedFlag, false);
    EXPECT_EQ(g_onCreateFailedFlag, true);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: FrameConfig
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Test Capture Frame config
 */
HWTEST_F(CameraLiteTest, TestCapture_Frame_config, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
    EXPECT_NE(fc, nullptr);
    delete fc;
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: CreateSurface
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Test Create Surface
 */
HWTEST_F(CameraLiteTest, TestCapture_Create_Surface, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete fc;
        cout << "CreateSurface failed" << endl;
    }
    EXPECT_NE(surface, nullptr);
    delete surface;
    delete fc;
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: CreateSurface
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Get Surface Size
 */
HWTEST_F(CameraLiteTest, TestCapture_GetSurfaceSize, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete fc;
        cout << "CreateSurface failed" << endl;
    }
    surface->SetWidthAndHeight(1920, 1080); /* 1920:width,1080:height */
    EXPECT_EQ(1920, surface->GetWidth());
    EXPECT_EQ(1080, surface->GetHeight());
    delete surface;
    delete fc;
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: GetSurfaces
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Get FrameConfig size
 */
HWTEST_F(CameraLiteTest, TestCapture_frameConfig_getsize, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    if (surface == nullptr) {
        delete fc;
        cout << "CreateSurface failed" << endl;
    }
    surface->SetWidthAndHeight(1920, 1080); /* 1920:width,1080:height */
    fc->AddSurface(*surface);
    EXPECT_NE(0, fc->GetSurfaces().size());
    delete surface;
    delete fc;
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: Capture
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Test Get Frame
 */
HWTEST_F(CameraLiteTest, TestOnFrameProgressed, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_EQ(g_onFrameStartedFlag, true);
    EXPECT_EQ(g_onFrameProgressedFlag, true);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: Capture
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Get FrameConfig onConfig
 */
HWTEST_F(CameraLiteTest, TestGetFrameConfigureType, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_EQ(g_onGetFrameConfigureType, true);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: Capture
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Test Get Frame finished
 */
HWTEST_F(CameraLiteTest, TestFrameCompletedFlag, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_EQ(g_onFrameFinishedFlag, true);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: Capture
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Test GetFrame Error
 */
HWTEST_F(CameraLiteTest, TestonFrameErrorFlag, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_NE(g_onFrameErrorFlag, true);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: Capture
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Test Capture Success
 */
HWTEST_F(CameraLiteTest, TestCapture01, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    camStateMng.Capture();
    sleep(1);
    EXPECT_EQ(g_onCaptureTriggerStartedFlag, true);
    EXPECT_EQ(g_onCaptureTriggerCompletedFlag, true);
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: Capture,StartRecord
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Test capture and record
 */
HWTEST_F(CameraLiteTest, TestRecord01, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    EXPECT_EQ(g_onCreatedFlag, true);
    EXPECT_EQ(g_onConfigureFlag, true);
    camStateMng.Capture();
    sleep(3);
    camStateMng.StartRecord();
    sleep(3);
    camStateMng.Stop();
    cameraKit = NULL;
}

/*
 * Feature: CamerLite
 * Function: Capture
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Test get event handler
 */
HWTEST_F(CameraLiteTest, TestGetEventHandler, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> camList;
    string camId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, camList, camId);
    SampleCameraStateMng camStateMng(eventHdlr);
    sleep(3);
    cameraKit->CreateCamera(camId, camStateMng, eventHdlr);
    sleep(3);
    EXPECT_EQ(g_onCreatedFlag, true);
    EXPECT_EQ(g_onConfigureFlag, true);
    camStateMng.Capture();
    sleep(3);
    camStateMng.StartRecord();
    sleep(3);
    camStateMng.Stop();
    cameraKit = NULL;
}
