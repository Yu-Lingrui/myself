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
	imshow("Image", img);
	waitKey(0); //显示图片不会一闪而过

	return 0;
}
