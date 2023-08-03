//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//Mat im;
//
//int main(int argc, char** argv) {
//	Mat gray, thr, thr1;
//	im = imread("face.jpg", 1);
//	Mat se = getStructuringElement(MORPH_RECT, Size(3,3));
//	erode(im, thr, se);
//	dilate(im, thr1, se);
//	morphologyEx(im, thr, MORPH_OPEN, se, Point(-1, -1), 3);
//	imshow("face", im);
//	imshow("thr", thr);
//	imshow("thr1", thr1);
//	waitKey(0);
//	destroyAllWindows();
//}