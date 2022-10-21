#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	string path = "/mnt/d/opencv/Fundamentals/resources/test_video.mp4";
	VideoCapture cap(path); //视频捕捉对象
	Mat img;
	while (true) {

		cap.read(img);

		imshow("Image", img);
		waitKey(1);
	}
	return 0;
}
