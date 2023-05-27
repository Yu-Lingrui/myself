#include"pphunmanseg.h"
#include"decode_pphunmanseg.h"

PPHunmanSeg::PPHunmanSeg(const utils::InitParameter& param) : m_param(param)
{
    // input
    m_input_src_device = nullptr;
    m_input_resize_device = nullptr;
    m_input_rgb_device = nullptr;
    m_input_norm_device = nullptr;
    m_input_hwc_device = nullptr;
    checkRuntime(cudaMalloc(&m_input_src_device,    param.batch_size * 3 * param.src_h * param.src_w * sizeof(float)));
    checkRuntime(cudaMalloc(&m_input_resize_device, param.batch_size * 3 * param.dst_h * param.dst_w * sizeof(float)));
    checkRuntime(cudaMalloc(&m_input_rgb_device,    param.batch_size * 3 * param.dst_h * param.dst_w * sizeof(float)));
    checkRuntime(cudaMalloc(&m_input_norm_device,   param.batch_size * 3 * param.dst_h * param.dst_w * sizeof(float)));
    checkRuntime(cudaMalloc(&m_input_hwc_device,    param.batch_size * 3 * param.dst_h * param.dst_w * sizeof(float)));

    // output
    m_output_src_device = nullptr;
    m_output_mask_device = nullptr;
    m_output_resize_device = nullptr;
    m_output_resize_host = nullptr;
    checkRuntime(cudaMalloc(&m_output_mask_device,   m_param.batch_size * 1 * m_param.dst_h * m_param.dst_w * sizeof(float)));
    checkRuntime(cudaMalloc(&m_output_resize_device, m_param.batch_size * 1 * m_param.src_h * m_param.src_w * sizeof(float)));
    m_output_resize_host = new float[m_param.batch_size * 1 * m_param.src_h * m_param.src_w];
}

PPHunmanSeg::~PPHunmanSeg()
{
    // input
    checkRuntime(cudaFree(m_input_src_device));
    checkRuntime(cudaFree(m_input_resize_device));
    checkRuntime(cudaFree(m_input_rgb_device));
    checkRuntime(cudaFree(m_input_norm_device));
    checkRuntime(cudaFree(m_input_hwc_device));
    // output
    checkRuntime(cudaFree(m_output_mask_device));
    checkRuntime(cudaFree(m_output_resize_device));
    delete[] m_output_resize_host;
}

bool PPHunmanSeg::init(const std::vector<unsigned char>& trtFile)
{
    // 1. init engine & context
    if (trtFile.empty())
    {
        return false;
    }
    // runtime
    std::unique_ptr<nvinfer1::IRuntime> runtime =
        std::unique_ptr<nvinfer1::IRuntime>(nvinfer1::createInferRuntime(sample::gLogger.getTRTLogger()));
    if (runtime == nullptr)
    {
        return false;
    }
    // deserializeCudaEngine
    this->m_engine = std::unique_ptr<nvinfer1::ICudaEngine>(runtime->deserializeCudaEngine(trtFile.data(), trtFile.size()));

    if (this->m_engine == nullptr)
    {
        return false;
    }
    // context
    this->m_context = std::unique_ptr<nvinfer1::IExecutionContext>(this->m_engine->createExecutionContext());
    if (this->m_context == nullptr)
    {
        return false;
    }
    // binding dim
    if (m_param.dynamic_batch) // for some models only support static dynamic batch. eg: yolox
    {
        this->m_context->setBindingDimensions(0, nvinfer1::Dims4(m_param.batch_size, 3, m_param.dst_h, m_param.dst_w));
    }
    
    // 2. get output's dim
    m_output_src_dims = this->m_context->getBindingDimensions(1);
    assert(m_param.batch_size <= m_output_src_dims.d[0]);
    
    auto get_area = [](const nvinfer1::Dims& dims) {
        int area = 1;
        for (int i = 1; i < dims.nbDims; i++)
        {
            if (dims.d[i] != 0)
                area *= dims.d[i];
        }
        return area;
    };
    m_output_src_area  = get_area(m_output_src_dims); // b * 1 * 192 * 192

    // 3. malloc
    checkRuntime(cudaMalloc(&m_output_src_device, m_param.batch_size * m_output_src_area * sizeof(float)));

    // 4. cal affine matrix
    float scale_y = float(m_param.dst_h) / m_param.src_h;
    float scale_x = float(m_param.dst_w) / m_param.src_w;
    cv::Mat src2dst = (cv::Mat_<float>(2, 3) << scale_x, 0.f, (-scale_x * m_param.src_w + m_param.dst_w + scale_x - 1) * 0.5,
        0.f, scale_y, (-scale_y * m_param.src_h + m_param.dst_h + scale_y - 1) * 0.5);
    cv::Mat dst2src = cv::Mat::zeros(2, 3, CV_32FC1);
    cv::invertAffineTransform(src2dst, dst2src);

    m_dst2src.v0 = dst2src.ptr<float>(0)[0];
    m_dst2src.v1 = dst2src.ptr<float>(0)[1];
    m_dst2src.v2 = dst2src.ptr<float>(0)[2];
    m_dst2src.v3 = dst2src.ptr<float>(1)[0];
    m_dst2src.v4 = dst2src.ptr<float>(1)[1];
    m_dst2src.v5 = dst2src.ptr<float>(1)[2];

    m_src2dst.v0 = src2dst.ptr<float>(0)[0];
    m_src2dst.v1 = src2dst.ptr<float>(0)[1];
    m_src2dst.v2 = src2dst.ptr<float>(0)[2];
    m_src2dst.v3 = src2dst.ptr<float>(1)[0];
    m_src2dst.v4 = src2dst.ptr<float>(1)[1];
    m_src2dst.v5 = src2dst.ptr<float>(1)[2];

    return true;
}

void PPHunmanSeg::check()
{
    // print inputs and outputs' dims
    int idx;
    nvinfer1::Dims dims;

    sample::gLogInfo << "the engine's info:" << std::endl;
    for (auto layer_name : m_param.input_output_names)
    {
        idx = this->m_engine->getBindingIndex(layer_name.c_str());
        dims = this->m_engine->getBindingDimensions(idx);
        sample::gLogInfo << "idx = " << idx << ", " << layer_name << ": ";
        for (int i = 0; i < dims.nbDims; i++)
        {
            sample::gLogInfo << dims.d[i] << ", ";
        }
        sample::gLogInfo << std::endl;
    }

    sample::gLogInfo << "the context's info:" << std::endl;
    for (auto layer_name : m_param.input_output_names)
    {
        idx = this->m_engine->getBindingIndex(layer_name.c_str());
        dims = this->m_context->getBindingDimensions(idx);
        sample::gLogInfo << "idx = " << idx << ", " << layer_name << ": ";
        for (int i = 0; i < dims.nbDims; i++)
        {
            sample::gLogInfo << dims.d[i] << ", ";
        }
        sample::gLogInfo << std::endl;
    }
}

void PPHunmanSeg::copy(const std::vector<cv::Mat>& imgsBatch)
{
    cv::Mat img_fp32 = cv::Mat::zeros(imgsBatch[0].size(), CV_32FC3); // todo 
    cudaHostRegister(img_fp32.data, img_fp32.elemSize() * img_fp32.total(), cudaHostRegisterPortable);

    // copy to device
    float* pi = m_input_src_device;
    //for (size_t i = 0; i < m_param.batch_size; i++)
    for (size_t i = 0; i < imgsBatch.size(); i++)
    {
        //std::vector<float> img_vec = std::vector<float>(imgsBatch[i].reshape(1, 1));
        imgsBatch[i].convertTo(img_fp32, CV_32FC3);
        checkRuntime(cudaMemcpy(pi, img_fp32.data, sizeof(float) * 3 * m_param.src_h * m_param.src_w, cudaMemcpyHostToDevice));
        /*imgsBatch[i].convertTo(imgsBatch[i], CV_32FC3);
        checkRuntime(cudaMemcpy(pi, imgsBatch[i].data, sizeof(float) * 3 * m_param.src_h * m_param.src_w, cudaMemcpyHostToDevice));*/
        pi += 3 * m_param.src_h * m_param.src_w;
    }

    cudaHostUnregister(img_fp32.data);
}

void PPHunmanSeg::preprocess(const std::vector<cv::Mat>& imgsBatch)
{
    // 1.resize
    resizeDevice(m_param.batch_size, m_input_src_device, m_param.src_w, m_param.src_h,
        m_input_resize_device, m_param.dst_w, m_param.dst_h, utils::ColorMode::RGB, m_dst2src);

#if 0 // valid
    {
        float* phost = new float[3 * m_param.dst_h * m_param.dst_w];
        float* pdevice = m_input_resize_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 3 * m_param.dst_h * m_param.dst_w,
                sizeof(float) * 3 * m_param.dst_h * m_param.dst_w, cudaMemcpyDeviceToHost));
            cv::Mat ret(m_param.dst_h, m_param.dst_w, CV_32FC3, phost);
            ret.convertTo(ret, CV_8UC3, 1.0, 0.0);
            cv::namedWindow("ret", cv::WINDOW_NORMAL);
            cv::imshow("ret", ret);
            cv::waitKey(1);
        }
        delete[] phost;
    }
#endif // 0

    // 2. bgr2rgb
    bgr2rgbDevice(m_param.batch_size, m_input_resize_device, m_param.dst_w, m_param.dst_h,
        m_input_rgb_device, m_param.dst_w, m_param.dst_h);

#if 0 // valid
    {
        float* phost = new float[3 * m_param.dst_h * m_param.dst_w];
        float* pdevice = m_input_rgb_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 3 * m_param.dst_h * m_param.dst_w,
                sizeof(float) * 3 * m_param.dst_h * m_param.dst_w, cudaMemcpyDeviceToHost));
            cv::Mat ret(m_param.dst_h, m_param.dst_w, CV_32FC3, phost);
            ret.convertTo(ret, CV_8UC3, 1.0, 0.0);
            cv::namedWindow("ret", cv::WINDOW_NORMAL);
            cv::imshow("ret", ret);
            cv::waitKey(1);
        }
        delete[] phost;
    }
#endif // 0

    // 3. norm:scale mean std
    normDevice(m_param.batch_size, m_input_rgb_device, m_param.dst_w, m_param.dst_h,
        m_input_norm_device, m_param.dst_w, m_param.dst_h, m_param);

#if 0 // valid
    {
        float* phost = new float[3 * m_param.dst_h * m_param.dst_w];
        float* pdevice = m_input_norm_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 3 * m_param.dst_h * m_param.dst_w,
                sizeof(float) * 3 * m_param.dst_h * m_param.dst_w, cudaMemcpyDeviceToHost));

            float* phost_ch1 = phost + m_param.dst_h * m_param.dst_w;
            float* phost_ch2 = phost_ch1 + m_param.dst_h * m_param.dst_w;
            for (int i = 0; i < 120; i++)
            {
                if(i % 3 == 0)
                    sample::gLogInfo << phost[i] << std::endl;
            }
            sample::gLogInfo << std::endl;
            cv::Mat ret(m_param.dst_h, m_param.dst_w, CV_32FC3, phost);
            for (size_t y = 0; y < ret.rows; y++)
            {
                for (size_t x = 0; x < ret.cols; x++)
                {
                    for (size_t c = 0; c < 3; c++)
                    {
                        //
                        ret.at<cv::Vec3f>(y, x)[c]
                            = m_param.scale * (ret.at<cv::Vec3f>(y, x)[c] * m_param.stds[c] + m_param.means[c]);
                    }

                }
            }
            ret.convertTo(ret, CV_8UC3, 1.0, 0.0);
            cv::imshow("ret", ret);
            cv::waitKey(1);
        }
        delete[] phost;
    }
#endif // 0

    // 4. hwc2chw
    hwc2chwDevice(m_param.batch_size, m_input_norm_device, m_param.dst_w, m_param.dst_h,
        m_input_hwc_device, m_param.dst_w, m_param.dst_h);
#if 0
    {

        float* phost = new float[3 * m_param.dst_h * m_param.dst_w];
        float* pdevice = m_input_hwc_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 3 * m_param.dst_h * m_param.dst_w,
                sizeof(float) * 3 * m_param.dst_h * m_param.dst_w, cudaMemcpyDeviceToHost));
            sample::gLogInfo << "-------------" << std::endl;
            for (int i = 0; i < 40; i++)
            {
                sample::gLogInfo << phost[i] << std::endl;
            }
            sample::gLogInfo << "-------------" << std::endl;

            cv::Mat b(m_param.dst_h, m_param.dst_w, CV_32FC1, phost);
            cv::Mat g(m_param.dst_h, m_param.dst_w, CV_32FC1, phost + 1 * m_param.dst_h * m_param.dst_w);
            cv::Mat r(m_param.dst_h, m_param.dst_w, CV_32FC1, phost + 2 * m_param.dst_h * m_param.dst_w);
            std::vector<cv::Mat> bgr{ b, g, r };
            cv::Mat ret;
            cv::merge(bgr, ret);
            ret.convertTo(ret, CV_8UC3, 255, 0.0);
            cv::imshow("ret", ret);

            /* SYSTEMTIME st = { 0 };
             GetLocalTime(&st);
             std::string t = std::to_string(st.wHour) + std::to_string(st.wMinute) + std::to_string(st.wMilliseconds);
             std::string save_path = "F:/Data/temp/";;
             cv::imwrite(save_path + t + ".jpg", ret);*/
            cv::waitKey(1);

            cv::Mat img_ = imgsBatch[j].clone();
        }
        delete[] phost;
    }
#endif
}

bool PPHunmanSeg::infer()
{
    float* bindings[] = { m_input_hwc_device, m_output_src_device };
    bool context = m_context->executeV2((void**)bindings);
    return context;
}

void PPHunmanSeg::postprocess(const std::vector<cv::Mat>& imgsBatch)
{
#if 0 // valid
    {
        float* phost = new float[m_param.batch_size * 2 * 192 * 192];
        float* pdevice = m_output_src_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 2 * 192 * 192, sizeof(float) * 2 * 192 * 192, cudaMemcpyDeviceToHost));
            cv::Mat img_ch0(m_param.dst_h, m_param.dst_w, CV_32FC1, phost);
            cv::Mat img_ch1(m_param.dst_h, m_param.dst_w, CV_32FC1, phost + 1 * 192 * 192);

            // test on opencv
            cv::Mat img_mask = cv::Mat::zeros(192, 192, CV_8UC1);
            // python code:result = np.argmax(output[0], axis=1).astype(np.uint8)
            for (int y = 0; y < 192; y++)
            {
                for (int x = 0; x < 192; x++)
                {
                    img_mask.ptr<uchar>(y)[x] = static_cast<uchar>(
                        255.f * (img_ch1.ptr<float>(y)[x] > img_ch0.ptr<float>(y)[x] ? 1 : 0));
                }
            }
        }
        delete[] phost;
    }
#endif // 0

    // 1. decode
    pphunmanseg::decodeDevice(m_param.batch_size, m_output_src_device, m_param.dst_w, m_param.dst_h, 
        m_output_mask_device, m_param.dst_w, m_param.dst_h);
#if 0 // valid
    {
        float* phost = new float[1 * 192 * 192];
        float* pdevice = m_output_mask_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 1 * 192 * 192,
                sizeof(float) * 1 * 192 * 192, cudaMemcpyDeviceToHost));
            cv::Mat prediction(192, 192, CV_32FC1, phost);
            //save to binary
            //utils::saveBinaryFile(phost, (1 + m_output_objects_width * m_param.topK), "yolov7.bin");
        }
        delete[] phost;
    }
#endif // 0
    // 2. resize
    resizeDevice(m_param.batch_size, m_output_mask_device, m_param.dst_w, m_param.dst_h,
        m_output_resize_device, m_param.src_w, m_param.src_h, utils::ColorMode::GRAY, m_src2dst);
#if 0 // valid
    {
        float* phost = new float[1 * m_param.src_h * m_param.src_w];
        float* pdevice = m_output_resize_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 1 * m_param.src_h * m_param.src_w,
                sizeof(float) * 1 * m_param.src_h * m_param.src_w, cudaMemcpyDeviceToHost));
            cv::Mat prediction(m_param.src_h, m_param.src_w, CV_32FC1, phost);
          
        }
        delete[] phost;
    }
#endif // 0
    // 3. copy result
    checkRuntime(cudaMemcpy(m_output_resize_host, m_output_resize_device, m_param.batch_size * sizeof(float) * m_param.src_w * m_param.src_h, cudaMemcpyDeviceToHost));
}

void PPHunmanSeg::reset()
{
}

void PPHunmanSeg::showMask(const std::vector<cv::Mat>& imgsBatch, const int& cvDelayTime)
{
    for (size_t bi = 0; bi < imgsBatch.size(); bi++)
    {
        cv::Mat img_mask(m_param.src_h, m_param.src_w, CV_32FC1, m_output_resize_host + bi * m_param.src_w * m_param.src_h);
        img_mask.convertTo(img_mask, CV_8UC1, 255.0, 0.); // * 255
        cv::imshow("img_mask", img_mask);
        cv::waitKey(cvDelayTime);
    }
}

void PPHunmanSeg::saveMask(const std::vector<cv::Mat>& imgsBatch, const std::string& savePath, const int& batchSize, const int& batchi)
{
    for (size_t bi = 0; bi < imgsBatch.size(); bi++)
    {
        cv::Mat img_mask(m_param.src_h, m_param.src_w, CV_32FC1, m_output_resize_host + bi * m_param.src_w * m_param.src_h);
        img_mask.convertTo(img_mask, CV_8UC1, 255.0, 0.); // * 255
        cv::imshow("img_mask", img_mask);
        int imgi = batchi * batchSize + bi;
        cv::imwrite(savePath + "_" + std::to_string(imgi) + ".bmp", img_mask);
        cv::waitKey(1); 
    }
}



