//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//Mat im;
//
//int main(int argc, char** argv) {
//	Mat gray, thr, thr3, autothr;
//	im = imread("face.jpg", 1); 
//	imshow("face", im);
//	cvtColor(im, gray,COLOR_BGR2GRAY);
//	//threshold(gray, thr, 127, 255, THRESH_BINARY);
//	threshold(im, thr3, 127, 255, THRESH_BINARY);
//	threshold(gray, autothr, 127, 255, THRESH_BINARY | THRESH_OTSU);//只支持单通道
//	adaptiveThreshold(gray, thr, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 25, 10);//局部
//	imshow("thr", thr);
//	imshow("thr3", thr3);
//	imshow("autothr", autothr);
//	waitKey(0);
//	destroyAllWindows();
//}