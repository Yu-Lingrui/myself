#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	string path = "/mnt/d/opencv/Fundamentals/resources/test.png";
	Mat img = imread(path);

	CascadeClassifier faceCascade;// 用于对象检测的级联分类器类
	faceCascade.load("/mnt/d/opencv/Fundamentals/resources/haarcascade_frontalface_default.xml");

	if (faceCascade.empty()) { cout << "XML file not loaded" << endl; }

	vector<Rect> faces;//继承子类，检测输入图像中不同大小的对象。检测到的对象作为矩形列表返回
	faceCascade.detectMultiScale(img, faces, 1.1, 10);

	for (int i = 0; i < faces.size(); i++) 
	{
		rectangle(img, faces[i].tl(), faces[i].br(), Scalar(255, 0, 255), 3);
	}

	imshow("Image", img);
	waitKey(0);

	return 0;
}
