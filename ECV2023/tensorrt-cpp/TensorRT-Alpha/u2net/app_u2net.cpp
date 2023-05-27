#include"u2net.h"

void setParameters(utils::InitParameter& initParameters)
{
	initParameters.class_names = utils::dataSets::coco80;
	//initParameters.num_class = 80; // for coco

	initParameters.batch_size = 1;
	initParameters.dst_h = 320;
	initParameters.dst_w = 320;

	initParameters.input_output_names = { "images",  "output" };
	
	initParameters.scale = 1.0; // div by max in u2net!

	initParameters.means[0] = 0.485;
	initParameters.means[1] = 0.456;
	initParameters.means[2] = 0.406;
	initParameters.stds[0] = 0.229;
	initParameters.stds[1] = 0.224;
	initParameters.stds[2] = 0.225;

	initParameters.save_path = "";
}

void task(u2net::U2NET& u2net, const utils::InitParameter& param, std::vector<cv::Mat>& imgsBatch, const int& delayTime, const int& batchi,
	const bool& isShow, const bool& isSave)
{
	u2net.copy(imgsBatch);
	utils::DeviceTimer d_t1; u2net.preprocess(imgsBatch);  float t1 = d_t1.getUsedTime();
	utils::DeviceTimer d_t2; u2net.infer();				   float t2 = d_t2.getUsedTime();
	utils::DeviceTimer d_t3; u2net.postprocess(imgsBatch); float t3 = d_t3.getUsedTime();
	sample::gLogInfo << "preprocess time = " << t1 / param.batch_size << "; "
		"infer time = " << t2 / param.batch_size << "; "
		"postprocess time = " << t3 / param.batch_size << std::endl;
	if(isShow)
		u2net.showMask(imgsBatch, delayTime);
	if(isSave)
		u2net.saveMask(imgsBatch, param.save_path, param.batch_size, batchi);
}

int main(int argc, char** argv)
{
	cv::CommandLineParser parser(argc, argv,
		{
			"{model 	|| tensorrt model file	}"
			"{size      || image (h, w), eg: 640}"
			"{batch_size|| batch size           }"
			"{video     || video's path			}"
			"{img       || image's path			}"
			"{cam_id    || camera's device id	}"
			"{show      || if show the result	}"
			"{savePath  || save path, can be ignore}"
		});

	/************************************************************************************************
	* init
	*************************************************************************************************/
	// parameters
	utils::InitParameter param;
	setParameters(param);
	// path
	std::string model_path = "../../data/u2net/u2net.trt";
	std::string video_path = "../../data/people.mp4";
	std::string image_path = "../../data/6406403.jpg";
	// camera' id
	int camera_id = 0;

	// get input
	utils::InputStream source;
	//source = utils::InputStream::IMAGE;
	//source = utils::InputStream::VIDEO;
	source = utils::InputStream::CAMERA;

	// update params from command line parser
	int size = -1; // w or h
	int batch_size = 8;
	bool is_show = false;
	bool is_save = false;
	if(parser.has("model"))
	{
		model_path = parser.get<std::string>("model");
		sample::gLogInfo << "model_path = " << model_path << std::endl;
	}
	if(parser.has("size"))
	{
		size = parser.get<int>("size");
		sample::gLogInfo << "size = " << size << std::endl;
		param.dst_h = param.dst_w = size;
	}
	if(parser.has("batch_size"))
	{
		batch_size = parser.get<int>("batch_size");
		sample::gLogInfo << "batch_size = " << batch_size << std::endl;
		param.batch_size = batch_size;
	}
	if(parser.has("video"))
	{
		source = utils::InputStream::VIDEO;
		video_path = parser.get<std::string>("video");
		sample::gLogInfo << "video_path = " << video_path << std::endl;
	}
	if(parser.has("img"))
	{
		source = utils::InputStream::IMAGE;
		image_path = parser.get<std::string>("img");
		sample::gLogInfo << "image_path = " << image_path << std::endl;
	}
	if(parser.has("cam_id"))
	{
		source = utils::InputStream::CAMERA;
		camera_id = parser.get<int>("cam_id");
		sample::gLogInfo << "camera_id = " << camera_id << std::endl;
	}
	if(parser.has("show"))
	{
		is_show = true;
		sample::gLogInfo << "is_show = " << is_show << std::endl;
	}
	if(parser.has("savePath"))
	{
		is_save = true;
		param.save_path = parser.get<std::string>("savePath");
		sample::gLogInfo << "save_path = " << param.save_path << std::endl;
	}

	int total_batches = 0;
	int delay_time = 1;
	cv::VideoCapture capture;
	if (!setInputStream(source, image_path, video_path, camera_id,
		capture, total_batches, delay_time, param))
	{
		sample::gLogError << "read the input data errors!" << std::endl;
		return -1;
	}

	u2net::U2NET u2net(param);

	// read model
	std::vector<unsigned char> trt_file = utils::loadModel(model_path);
	if (trt_file.empty())
	{
		sample::gLogError << "trt_file is empty!" << std::endl;
		return -1;
	}
	// init model
	if (!u2net.init(trt_file))
	{
		sample::gLogError << "initEngine() ocur errors!" << std::endl;
		return -1;
	}
	u2net.check();
	/************************************************************************************************
	* recycle
	*************************************************************************************************/
	cv::Mat frame;
	std::vector<cv::Mat> imgs_batch;
	imgs_batch.reserve(param.batch_size);
	sample::gLogInfo << imgs_batch.capacity() << std::endl;
	int i = 0; // debug
	int batchi = 0;
	while (capture.isOpened())
	{
		if (batchi >= total_batches && source != utils::InputStream::CAMERA)
		{
			break;
		}
		if (imgs_batch.size() < param.batch_size) // get input
		{
			if (source != utils::InputStream::IMAGE)
			{
				capture.read(frame);
			}
			else
			{
				frame = cv::imread(image_path);
			}

			if (frame.empty())
			{
				sample::gLogWarning << "no more video or camera frame" << std::endl;
				task(u2net, param, imgs_batch, delay_time, batchi, is_show, is_save);
				imgs_batch.clear(); // clear
				//sample::gLogInfo << imgs_batch.capacity() << std::endl;
				batchi++;
				break;
			}
			else
			{
				imgs_batch.emplace_back(frame.clone());
			}

		}
		else // infer
		{
			task(u2net, param, imgs_batch, delay_time, batchi, is_show, is_save);
			imgs_batch.clear(); // clear
			//sample::gLogInfo << imgs_batch.capacity() << std::endl;
			batchi++;
		}
	}
	return  -1;
}

