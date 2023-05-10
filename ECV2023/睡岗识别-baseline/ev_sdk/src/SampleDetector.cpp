#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <glog/logging.h>
//jsoncpp 相关的头文件
#include "reader.h"
#include "writer.h"
#include "value.h"
#include "SampleDetector.hpp"

SampleDetector::SampleDetector()
{
    
}

SampleDetector::~SampleDetector()
{
    UnInit();
}

STATUS SampleDetector::Init(float thresh)
{        
    std::string labelPath = "/usr/local/ev_sdk/model/label.txt";
//     std::string labelPath = "/project/train/models/label.txt";
    std::ifstream ifs(labelPath);
    if (!ifs)
    {
        LOG(ERROR) << labelPath << " not found!";
        return ERROR_INIT;
    }

    std::string line;
    while (std::getline(ifs, line))
    {
        mClassNames.push_back(line);
    }
    this->mNumClass = mClassNames.size();

    string modelPath = "/usr/local/ev_sdk/model/best.onnx";
//     string modelPath = "/project/train/models/best.onnx";
    OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_CUDA(mSessionOptions, 0);
    mSessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_BASIC);
    mSession = new Session(mEnv, modelPath.c_str(), mSessionOptions);
    size_t numInputNodes = mSession->GetInputCount();
    size_t numOutputNodes = mSession->GetOutputCount();
    AllocatorWithDefaultOptions allocator;
    for (int i = 0; i < numInputNodes; i++)
    {
        mInputNames.push_back(mSession->GetInputName(i, allocator));
    }
    for (int i = 0; i < numOutputNodes; i++)
    {
        mOutputNames.push_back(mSession->GetOutputName(i, allocator));
    }
    mThresh = thresh;
    
    string model_path = "/usr/local/ev_sdk/model/dark_hrnet_w32_256x192.onnx";
    sessionOptions_person.SetGraphOptimizationLevel(ORT_ENABLE_BASIC);
    ort_session_person = new Session(env_person, model_path.c_str(), sessionOptions_person);
    numInputNodes = ort_session_person->GetInputCount();
    numOutputNodes = ort_session_person->GetOutputCount();
    AllocatorWithDefaultOptions allocator_person;
    for (int i = 0; i < numInputNodes; i++)
    {
        input_names_person.push_back(ort_session_person->GetInputName(i, allocator_person));
    }
    for (int i = 0; i < numOutputNodes; i++)
    {
        output_names_person.push_back(ort_session_person->GetOutputName(i, allocator_person));
        Ort::TypeInfo output_type_info_person = ort_session_person->GetOutputTypeInfo(i);
        auto output_tensor_info_person = output_type_info_person.GetTensorTypeAndShapeInfo();
        auto output_dims_person = output_tensor_info_person.GetShape();
        output_node_dims_person.push_back(output_dims_person);
    }
    output_node_dims_person[0][0] = 1;
    center[0] = (float)this->inpWidth_person*0.5;
    center[1] = (float)this->inpHeight_person*0.5;
    scale[0] = (float)this->inpWidth_person / 200;
    scale[1] = (float)this->inpHeight_person / 200;
    preds = new float[output_node_dims_person[0][1] * 2 + 2];
    
    SDKLOG(INFO) << "Init Done. model name is " << modelPath << ",thresh is " << mThresh;
    return 0;
}

STATUS SampleDetector::UnInit()
{
    if( mSession != nullptr)
    {
        delete mSession;
        mSession = nullptr;
    }
    
    if(ort_session_person != nullptr)
    {
        delete ort_session_person;
        ort_session_person = nullptr;
    }
    if(preds != nullptr)
    {
        delete preds;
        preds = nullptr;
    }
}

cv::Mat SampleDetector::ResizeImage(cv::Mat &srcimg, int *newh, int *neww, int *top, int *left)
{
    int srch = srcimg.rows, srcw = srcimg.cols;
    *newh = this->mInpHeight;
    *neww = this->mInpWidth;
    cv::Mat dstimg;
    if (this->mKeepRatio && srch != srcw)
    {
        float hw_scale = (float)srch / srcw;
        if (hw_scale > 1)
        {
            *newh = this->mInpHeight;
            *neww = int(this->mInpWidth / hw_scale);
            cv::resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
            *left = int((this->mInpWidth - *neww) * 0.5);
            cv::copyMakeBorder(dstimg, dstimg, 0, 0, *left, this->mInpWidth - *neww - *left, BORDER_CONSTANT, 0);
        }
        else
        {
            *newh = (int)this->mInpHeight * hw_scale;
            *neww = this->mInpWidth;
            cv::resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
            *top = (int)(this->mInpHeight - *newh) * 0.5;
            cv::copyMakeBorder(dstimg, dstimg, *top, this->mInpHeight - *newh - *top, 0, 0, BORDER_CONSTANT, 0);
        }
    }
    else
    {
        cv::resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
    }
    return dstimg;
}

/**
 * @brief demo自定义辅助函数
 */
void SampleDetector::Normalize(cv::Mat &img)
{
    //    img.convertTo(img, CV_32F);
    int row = img.rows;
    int col = img.cols;
    this->input_image_.resize(row * col * img.channels());
    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                float pix = img.ptr<uchar>(i)[j * 3 + c];
                this->input_image_[c * row * col + i * col + j] = pix / 255.0;
//                 this->input_image_[c * row * col + i * col + j] = pix;
            }
        }
    }
}

cv::Mat SampleDetector::resize_image(Mat srcimg, int *newh, int *neww, int *top, int *left)
{
    int srch = srcimg.rows, srcw = srcimg.cols;
    *newh = this->inpHeight_person;
    *neww = this->inpWidth_person;
    Mat dstimg;
    if (this->keep_ratio && srch != srcw)
    {
        float hw_scale = (float)srch / srcw;
        if (hw_scale > 1) {
            *newh = this->inpHeight_person;
            *neww = int(this->inpWidth_person / hw_scale);
            resize(srcimg, dstimg, Size(*neww, *newh), INTER_LINEAR);
            *left = int((this->inpWidth_person - *neww) * 0.5);
            copyMakeBorder(dstimg, dstimg, 0, 0, *left, this->inpWidth_person - *neww - *left, BORDER_CONSTANT, 0);
        }
        else {
            *newh = (int)this->inpHeight_person * hw_scale;
            *neww = this->inpWidth_person;
            resize(srcimg, dstimg, Size(*neww, *newh), INTER_LINEAR);
            *top = (int)(this->inpHeight_person - *newh) * 0.5;
            copyMakeBorder(dstimg, dstimg, *top, this->inpHeight_person - *newh - *top, 0, 0, BORDER_CONSTANT, 0);
        }
    }
    else {
        resize(srcimg, dstimg, Size(*neww, *newh), INTER_LINEAR);
    }
    return dstimg;
}

void SampleDetector::normalize_person(Mat img)
{
    //img.convertTo(img, CV_32F);
    int row = img.rows;
    int col = img.cols;
    this->input_person.resize(row * col * img.channels());
    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                float pix = img.ptr<uchar>(i)[j * 3 + c];
                this->input_person[c * row * col + i * col + j] = pix;
            }
        }
    }
}

void SampleDetector::key_points(Mat srcimg, Rect box)
{
    Mat person_img = srcimg(box);
    int newh = 0, neww = 0, padh = 0, padw = 0;
    Mat dstimg = this->resize_image(person_img, &newh, &neww, &padh, &padw);
    this->normalize_person(dstimg);
    array<int64_t, 4> inputShape{ 1, 3, this->inpHeight_person, this->inpWidth_person };

    auto allocateInfo = MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Value inputTensor = Value::CreateTensor<float>(allocateInfo, input_person.data(), input_person.size(), inputShape.data(), inputShape.size());

    // 开始推理
    vector<Value> output_tensors_hrnet = ort_session_person->Run(RunOptions{ nullptr }, &input_names_person[0], &inputTensor, 1, output_names_person.data(), output_names_person.size());
    float* floatarr = output_tensors_hrnet[0].GetTensorMutableData<float>();
    get_final_preds(floatarr, output_node_dims_person[0], center, scale, preds);

    for (int i = 0; i < output_node_dims_person[0][1]; i++)
    {
        preds[i] = box.x + (preds[i] - padw)*box.width / neww;
        preds[i + 17] = box.y + (preds[i + 17] - padh)*box.height / newh;
    }
}

STATUS SampleDetector::ProcessImage(cv::Mat &inFrame, std::vector<Box_pts_Info> &result, float thresh)
{
    mThresh = thresh;
    if (inFrame.empty())
    {
        SDKLOG(ERROR) << "Invalid input!";
        return -1;
    }
    result.clear();
    int newh = 0, neww = 0, top = 0, left = 0;
    Mat dst = this->ResizeImage(inFrame, &newh, &neww, &top, &left);

    this->Normalize(dst);
    array<int64_t, 4> inputShape{1, 3, this->mInpHeight, this->mInpWidth};

    auto allocateInfo = MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Value inputTensor = Value::CreateTensor<float>(allocateInfo, input_image_.data(), input_image_.size(), inputShape.data(), inputShape.size());

    // 开始推理
    vector<Value> ortOutputs = mSession->Run(RunOptions{nullptr}, &mInputNames[0], &inputTensor, 1, mOutputNames.data(), mOutputNames.size());
    vector<BoxInfo> generateBoxes;
    const float *outs = ortOutputs[0].GetTensorMutableData<float>();

    /////generate proposals
    float ratioh = (float)inFrame.rows / newh, ratiow = (float)inFrame.cols / neww;
    int n = 0, q = 0, i = 0, j = 0, nout = mNumClass + 5, row_ind = 0;
    for (n = 0; n < 3; n++) ///
    {
        int num_grid_x = (int)(this->mInpWidth / this->mStride[n]);
        int num_grid_y = (int)(this->mInpHeight / this->mStride[n]);
        for (q = 0; q < 3; q++) ///
        {
            const float anchor_w = this->mAnchors[n][q * 2];
            const float anchor_h = this->mAnchors[n][q * 2 + 1];
            for (i = 0; i < num_grid_y; i++)
            {
                for (j = 0; j < num_grid_x; j++)
                {
                    const float *pdata = outs + row_ind * nout;
                    float box_score = pdata[4];
                    if (box_score > this->mThresh)
                    {
                        int max_ind = 0;
                        float max_class_score = 0;
                        for (int k = 0; k < mNumClass; k++)
                        {
                            if (pdata[5 + k] > max_class_score)
                            {
                                max_class_score = pdata[5 + k];
                                max_ind = k;
                            }
                        }
                        
                        string box_name = mClassNames[max_ind];
                        if(box_name=="prostrate_sleep" || box_name=="sit_sleep" || box_name=="lie_sleep")
                        {
                            float cx = (pdata[0] * 2.f - 0.5f + j) * this->mStride[n]; ///cx
                            float cy = (pdata[1] * 2.f - 0.5f + i) * this->mStride[n]; ///cy
                            float w = powf(pdata[2] * 2.f, 2.f) * anchor_w;           ///w
                            float h = powf(pdata[3] * 2.f, 2.f) * anchor_h;           ///h
                            float x0 = max<float>((cx - 0.5 * w - left) * ratiow, 0.f);
                            float y0 = max<float>((cy - 0.5 * h - top) * ratioh, 0.f);
                            float x1 = min<float>((cx + 0.5 * w - left) * ratiow, (float)inFrame.cols);
                            float y1 = min<float>((cy + 0.5 * h - top) * ratioh, (float)inFrame.rows);

                            generateBoxes.push_back(BoxInfo{x0, y0, x1, y1, box_score, max_ind, box_name});
                        }  
                    }
                    row_ind++;
                }
            }
        }
    }
    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    NMS(generateBoxes);
    for (auto &obj : generateBoxes)
    {
        Rect rect;
        rect.x = int(obj.x1);
        rect.y = int(obj.y1);
        rect.width = int(obj.x2 - obj.x1);
        rect.height = int(obj.y2 - obj.y1);
        this->key_points(inFrame, rect);
        Box_pts_Info box;
        box.x1 = obj.x1;
        box.y1 = obj.y1;
        box.x2 = obj.x2;
        box.y2 = obj.y2;
        box.score = obj.score;
        box.label = obj.label;
        box.name = obj.name;
        for (int j = 0; j < 17; j++)
        {
            box.keypoints[j*3] = preds[j];
            box.keypoints[j*3+1] = preds[j+17];
            box.keypoints[j*3+2] = 0.95;
        }
        result.push_back(box);
    }
    return 0;
}

void SampleDetector::NMS(std::vector<BoxInfo> &inputBoxes)
{
    std::sort(inputBoxes.begin(), inputBoxes.end(), [](BoxInfo a, BoxInfo b) { return a.score > b.score; });
    std::vector<float> vArea(inputBoxes.size());
    for (int i = 0; i < int(inputBoxes.size()); ++i)
    {
        vArea[i] = (inputBoxes.at(i).x2 - inputBoxes.at(i).x1 + 1) * (inputBoxes.at(i).y2 - inputBoxes.at(i).y1 + 1);
    }

    std::vector<bool> isSuppressed(inputBoxes.size(), false);
    for (int i = 0; i < int(inputBoxes.size()); ++i)
    {
        if (isSuppressed[i])
        {
            continue;
        }
        for (int j = i + 1; j < int(inputBoxes.size()); ++j)
        {
            if (isSuppressed[j])
            {
                continue;
            }
            float xx1 = (max)(inputBoxes[i].x1, inputBoxes[j].x1);
            float yy1 = (max)(inputBoxes[i].y1, inputBoxes[j].y1);
            float xx2 = (min)(inputBoxes[i].x2, inputBoxes[j].x2);
            float yy2 = (min)(inputBoxes[i].y2, inputBoxes[j].y2);

            float w = (max)(float(0), xx2 - xx1 + 1);
            float h = (max)(float(0), yy2 - yy1 + 1);
            float inter = w * h;
            float ovr = inter / (vArea[i] + vArea[j] - inter);

            if (ovr >= this->mnmsThresh)
            {
                isSuppressed[j] = true;
            }
        }
    }
    int idx_t = 0;
    inputBoxes.erase(remove_if(inputBoxes.begin(), inputBoxes.end(), [&idx_t, &isSuppressed](const BoxInfo &f) { return isSuppressed[idx_t++]; }), inputBoxes.end());
}
