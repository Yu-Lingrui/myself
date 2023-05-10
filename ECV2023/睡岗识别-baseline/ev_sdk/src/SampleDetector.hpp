/*
 * Copyright (c) 2021 ExtremeVision Co., Ltd.
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

#ifndef JI_SAMPLEDETECTOR_HPP
#define JI_SAMPLEDETECTOR_HPP
#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cuda_provider_factory.h>
#include <onnxruntime_cxx_api.h>

#include "ji.h"
#include "ji_utils.h"
#include "WKTParser.h"
#include "Configuration.hpp"
#include "utils.h"

#define STATUS int
using namespace std;
using namespace cv;
using namespace Ort;

typedef struct BoxInfo
{
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    int label;
    std::string name;
    float keypoints[17*3];
} BoxInfo;

typedef struct Box_pts_Info
{
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    int label;
    std::string name;
    float keypoints[17*3];
} Box_pts_Info;

class SampleDetector
{

public:
    /*
     * @breif 检测器构造函数     
    */ 
    SampleDetector();
    
    /*
     * @breif 检测器析构函数     
    */ 
    ~SampleDetector();
    
    /*
     * @breif 初始化检测器相关的资源
     * @param strModelName 检测器加载的模型名称     
     * @param thresh 检测阈值
     * @return 返回结果, STATUS_SUCCESS代表调用成功
    */ 
    STATUS Init(float thresh);

    /*
     * @breif 去初始化,释放模型检测器的资源     
     * @return 返回结果, STATUS_SUCCESS代表调用成功
    */ 
    STATUS UnInit();
    
    /*
     * @breif 根据送入的图片进行模型推理, 并返回检测结果
     * @param inFrame 输入图片
     * @param result 检测结果通过引用返回
     * @return 返回结果, STATUS_SUCCESS代表调用成功
    */
    STATUS ProcessImage(Mat &inFrame, std::vector<Box_pts_Info> &result, float thresh = 0.15);

public:
    // 接口的返回值定义
    static const int ERROR_BASE = 0x0200;
    static const int ERROR_INPUT = 0x0201;
    static const int ERROR_INIT = 0x0202;
    static const int ERROR_PROCESS = 0x0203;
    static const int STATUS_SUCCESS = 0x0000;   
private:
    vector<string> mClassNames;
    int mNumClass;
    vector<char *> mInputNames;
    vector<char *> mOutputNames;
    const float mAnchors[3][6] = {{10.0, 13.0, 16.0, 30.0, 33.0, 23.0}, {30.0, 61.0, 62.0, 45.0, 59.0, 119.0}, {116.0, 90.0, 156.0, 198.0, 373.0, 326.0}};
    const float mStride[3] = {8, 16, 32};
    const bool mKeepRatio = true;
    const int mInpWidth = 640;
    const int mInpHeight = 640;
    double mnmsThresh = 0.5;
    float mThresh = 0.25;
   
    Mat ResizeImage(Mat &srcImg, int *newh, int *neww, int *top, int *left);
    vector<float> input_image_;
    void Normalize(Mat &img);
    void NMS(vector<BoxInfo> &inputBoxes);

    Session *mSession = nullptr;
    Env mEnv = Env(ORT_LOGGING_LEVEL_ERROR, "yolov5");
    SessionOptions mSessionOptions = SessionOptions();
    
    vector<float> input_person;
    Env env_person = Env(ORT_LOGGING_LEVEL_ERROR, "hrnet");
    Ort::Session *ort_session_person = nullptr;
    SessionOptions sessionOptions_person = SessionOptions();
    vector<char*> input_names_person;
    vector<char*> output_names_person;
    vector<vector<int64_t>> output_node_dims_person;
    const int inpWidth_person = 192;
    const int inpHeight_person = 256;
    Mat resize_image(Mat srcimg, int *newh, int *neww, int *top, int *left);
    const bool keep_ratio = true;
    void normalize_person(Mat img);
    void key_points(Mat srcimg, Rect box);
    vector<float> center{ 0,0 };
    vector<float> scale{ 0,0 };
    float* preds = nullptr;
};

#endif //JI_SAMPLEDETECTOR_HPP
