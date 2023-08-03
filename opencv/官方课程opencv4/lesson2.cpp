//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//int main(int argc, char** argv) {
//	VideoCapture capture;
//	capture.open("D:/Visual Studio2019(64bit)/Projects/Opencv/vtest.avi");
//	Mat frame;
//	namedWindow("frame", WINDOW_AUTOSIZE);
//	while (capture.isOpened())
//	{
//		capture >> frame;
//		// capture.read(frame);
//		if (frame.empty())
//		{
//			break;
//		}
//		imshow("frame", frame);
//		char key = waitKey(50);
//		if (key == 27) {
//			break;
//		}
//	}
//	destroyAllWindows();
//}