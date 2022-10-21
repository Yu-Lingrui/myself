#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	string path = "/mnt/d/opencv/Fundamentals/resources/test.png";
	Mat img = imread(path);
	Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

	cvtColor(img, imgGray, COLOR_BGR2GRAY); //灰度化
	GaussianBlur(img, imgBlur, Size(3, 3), 3, 0); //高斯模糊
	Canny(imgBlur, imgCanny, 25, 75); //边缘检测
    //形态学
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);
	erode(imgDil, imgErode, kernel);

	imshow("Image", img);
	imshow("ImageGray", imgGray);
	imshow("ImageBlur", imgBlur);
	imshow("ImageCanny", imgCanny);
	imshow("ImageDilation", imgDil);
	imshow("ImageErode", imgErode);
	waitKey(0);

	return 0;
}
