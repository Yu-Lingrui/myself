#include"yolox.h"

YOLOX::YOLOX(const utils::InitParameter& param) :yolo::YOLO(param)
{
}

YOLOX::~YOLOX()
{
    checkRuntime(cudaFree(m_input_resize_without_padding_device));
}

bool YOLOX::init(const std::vector<unsigned char>& trtFile)
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
    // ...
    //nvinfer1::Dims input_dims = this->m_context->getBindingDimensions(0);

    // 2. get output's dim
    m_output_dims = this->m_context->getBindingDimensions(1);
    m_total_objects = m_output_dims.d[1];
    assert(m_param.batch_size == m_output_dims.d[0] || 
           m_param.batch_size == 1 // batch_size = 1, but it will infer with "batch_size=m_output_dims.d[0]", only support static batch
            );
    m_output_area = 1; // 22500 * 85
    for (int i = 1; i < m_output_dims.nbDims; i++)
    {
        if (m_output_dims.d[i] != 0)
        {
            m_output_area *= m_output_dims.d[i];
        }
    }
    // 3. malloc
    checkRuntime(cudaMalloc(&m_output_src_device, m_param.batch_size * m_output_area * sizeof(float)));

    // 4. cal affine matrix
    float a = float(m_param.dst_h) / m_param.src_h;
    float b = float(m_param.dst_w) / m_param.src_w;
    float scale = a < b ? a : b;
    m_resized_h = roundf((float)m_param.src_h * scale);
    m_resized_w = roundf((float)m_param.src_w * scale);

    checkRuntime(cudaMalloc(&m_input_resize_without_padding_device,
        m_param.batch_size * 3 * m_resized_h * m_resized_w * sizeof(float)));
    cv::Mat src2dst = (cv::Mat_<float>(2, 3) << scale, 0.f, (scale - 1) * 0.5,
        0.f, scale, (scale - 1) * 0.5);
    cv::Mat dst2src = cv::Mat::zeros(2, 3, CV_32FC1);
    cv::invertAffineTransform(src2dst, dst2src);
    m_dst2src.v0 = dst2src.ptr<float>(0)[0];
    m_dst2src.v1 = dst2src.ptr<float>(0)[1];
    m_dst2src.v2 = dst2src.ptr<float>(0)[2];
    m_dst2src.v3 = dst2src.ptr<float>(1)[0];
    m_dst2src.v4 = dst2src.ptr<float>(1)[1];
    m_dst2src.v5 = dst2src.ptr<float>(1)[2];
    return true;
}

void YOLOX::preprocess(const std::vector<cv::Mat>& imgsBatch)
{
    // 1.resize
    resizeDevice(m_param.batch_size, m_input_src_device, m_param.src_w, m_param.src_h,
        m_input_resize_without_padding_device, m_resized_w, m_resized_h, 114, m_dst2src);
#if 0 // valid
    {
        float* phost = new float[3 * m_resized_h * m_resized_w];
        float* pdevice = m_input_resize_without_padding_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 3 * m_resized_h * m_resized_w,
                sizeof(float) * 3 * m_resized_h * m_resized_w, cudaMemcpyDeviceToHost));
            cv::Mat ret(m_resized_h, m_resized_w, CV_32FC3, phost);
            cv::Mat img_resized = cv::Mat::zeros(cv::Size(m_resized_w, m_resized_h), CV_8UC3);
            cv::resize(imgsBatch[j], img_resized, cv::Size(m_resized_w, m_resized_h));
            ret.convertTo(ret, CV_8UC3, 1.0, 0.0);
            cv::namedWindow("ret", cv::WINDOW_NORMAL);
            cv::imshow("ret", ret);
            cv::waitKey(1);
        }
        delete[] phost;
    }
#endif // 0
    // 2.copy with padding
    copyWithPaddingDevice(m_param.batch_size, m_input_resize_without_padding_device, m_resized_w, m_resized_h, 
        m_input_resize_device, m_param.dst_w, m_param.dst_h, 114.f);
#if 0 // valid
    {
        float* phost = new float[3 * m_param.dst_w * m_param.dst_h];
        float* pdevice = m_input_resize_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 3 * m_param.dst_w * m_param.dst_h,
                sizeof(float) * 3 * m_param.dst_w * m_param.dst_h, cudaMemcpyDeviceToHost));
            cv::Mat ret(m_param.dst_h, m_param.dst_w, CV_32FC3, phost);
            ret.convertTo(ret, CV_8UC3);
            cv::namedWindow("ret", cv::WINDOW_NORMAL);
            cv::imshow("ret", ret);
            cv::waitKey(1);
        }
        delete[] phost;
    }
#endif // 0

    // 3. hwc2chw
    hwc2chwDevice(m_param.batch_size, m_input_resize_device, m_param.dst_w, m_param.dst_h,
        m_input_hwc_device, m_param.dst_w, m_param.dst_h);
#if 0
    {

        float* phost = new float[3 * m_param.dst_h * m_param.dst_w];
        float* pdevice = m_input_hwc_device;
        for (size_t j = 0; j < imgsBatch.size(); j++)
        {
            checkRuntime(cudaMemcpy(phost, pdevice + j * 3 * m_param.dst_h * m_param.dst_w,
                sizeof(float) * 3 * m_param.dst_h * m_param.dst_w, cudaMemcpyDeviceToHost));

            cv::Mat tmp = imgsBatch[j].clone();

            cv::Mat b(m_param.dst_h, m_param.dst_w, CV_32FC1, phost);
            cv::Mat g(m_param.dst_h, m_param.dst_w, CV_32FC1, phost + 1 * m_param.dst_h * m_param.dst_w);
            cv::Mat r(m_param.dst_h, m_param.dst_w, CV_32FC1, phost + 2 * m_param.dst_h * m_param.dst_w);
            std::vector<cv::Mat> bgr{ b, g, r };
            cv::Mat ret;
            cv::merge(bgr, ret);
            ret.convertTo(ret, CV_8UC3);
            cv::imshow("ret", ret);
            cv::waitKey(1);

        }
        delete[] phost;

    }
#endif
}

__global__
void copy_with_padding_kernel_function(int batchSize, float* src, int srcWidth, int srcHeight, int srcArea, int srcVolume,
    float* dst, int dstWidth, int dstHeight, int dstArea, int dstVolume, float paddingValue)
{
    int dx = blockDim.x * blockIdx.x + threadIdx.x;
    int dy = blockDim.y * blockIdx.y + threadIdx.y;
    if (dx < dstArea && dy < batchSize)
    {
        int dst_y = dx / dstWidth; // dst row
        int dst_x = dx % dstWidth; // dst col
        float* pdst = dst + dy * dstVolume + dst_y * dstWidth * 3 + dst_x * 3;
       
        if (dst_y < srcHeight && dst_x < srcWidth)
        {
            float* psrc = src + dy * srcVolume + dst_y * srcWidth * 3 + dst_x * 3;
            pdst[0] = psrc[0];
            pdst[1] = psrc[1];
            pdst[2] = psrc[2];
        }
        else
        {
            pdst[0] = paddingValue;
            pdst[1] = paddingValue;
            pdst[2] = paddingValue;
        }
       

    }
}

void copyWithPaddingDevice(const int& batchSize, float* src, int srcWidth, int srcHeight,
    float* dst, int dstWidth, int dstHeight, float paddingValue)
{
    dim3 block_size(BLOCK_SIZE, BLOCK_SIZE);
    dim3 grid_size((dstWidth * dstHeight /** 3*/ + BLOCK_SIZE - 1) / BLOCK_SIZE,
        (batchSize + BLOCK_SIZE - 1) / BLOCK_SIZE);
    int src_area = srcHeight * srcWidth;
    int dst_area = dstHeight * dstWidth;

    int src_volume = 3 * srcHeight * srcWidth;
    int dst_volume = 3 * dstHeight * dstWidth;
    assert(srcWidth <= dstWidth);
    assert(srcHeight <= dstHeight);
    copy_with_padding_kernel_function <<< grid_size, block_size, 0, nullptr >>>(batchSize, src, srcWidth, srcHeight, src_area, src_volume,
        dst, dstWidth, dstHeight, dst_area, dst_volume, paddingValue);
}