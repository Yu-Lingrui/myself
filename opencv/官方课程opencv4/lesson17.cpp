//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//int main(int argc, char** argv)
//{
//	Mat img = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/1.jpg");
//	// Convert to grayscale
//	Mat imgGray;
//	cvtColor(img, imgGray, COLOR_BGR2GRAY);
//
//	// Initiate ORB detector
//	Ptr<ORB>orb = ORB::create();
//	//Ptr<SIFT>detector = SIFT::create();
//
//	// find the keypoints with ORB
//	vector<KeyPoint>kp;
//	orb->detect(imgGray, kp, Mat());
//
//	// compute the descriptors with ORB
//	Mat des;
//	orb->compute(imgGray, kp, des);
//
//	// draw only keypoints location, not size and orientation
//	Mat img2;
//	drawKeypoints(img, kp, img2, Scalar(0, 255, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//	imshow("ORB keypoints", img2);
//	waitKey(0);
//	destroyAllWindows();
//
//	return 0;
//}