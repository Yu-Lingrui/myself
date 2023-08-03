//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
////void drawSquare(int action, int x, int y, int flags, void* userdata);
//void scaleImage(int, void*);
//
//// Points to store the center of the circle and a point on the circumference
////Point topleft, bottomright;
////Source image
////Mat source, dummy;
//int maxscaleUp = 100;
//int scaleFactor = 1;
//int scaleType = 0;
//int maxType = 1;
//Mat im;
//string windowName = "Resize Image";
//string trackbarvalue = "Scale";
//string trackbarType = "Type:\n 0:Scale Up \n 1:Scale Down";
//
//int main(int argc, char** argv) {
//
//	im = imread("face.jpg", 1);
//	//im = imread(argv[1]);//face.jpg
//	//dummy = source.clone();//deepcopy
//	namedWindow(windowName, WINDOW_AUTOSIZE);
//	createTrackbar(trackbarType, windowName, &scaleFactor, maxscaleUp, scaleImage);
//	createTrackbar(trackbarType, windowName, &scaleType, maxType, scaleImage);
//	//×¢²áºó»Øµ÷
//	//setMouseCallback("frame", drawSquare);
//	//scaleImage(25,0);
//	char k = 0;
//	while (true)
//	{
//		//imshow("frame", im);
//		char key = waitKey();
//		if (key == 27) {
//			break;
//		}
//	}
//	//destroyAllWindows();
//}
//
//
//void scaleImage(int, void*) {
//	double scaleFactorDouble = 1;
//	if (scaleType == 0) {
//		scaleFactorDouble = 1 + scaleFactor / 100.0;
//	}
//	if (scaleType == 1) {
//		scaleFactorDouble = 1 - scaleFactor / 100.0;
//	}
//	if (scaleFactorDouble == 0) {
//		scaleFactorDouble = 1;
//	}
//	Mat scaledImage;
//	resize(im, scaledImage, Size(), scaleFactorDouble, scaleFactorDouble, INTER_LINEAR);
//	imshow(windowName, scaledImage);
//}