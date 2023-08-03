//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//Mat im;
//
//int main(int argc, char** argv) {
//	im = imread("D:\\Visual Studio2019(64bit)\\Projects\\Opencv\\coin.png");
//	imshow("1", im);
//	Mat img = im.clone();
//	/***********************************************/
//	//Set up detector with default parameters
//	Ptr<SimpleBlobDetector>detector = SimpleBlobDetector::create();
//	vector<KeyPoint> keypoints;
//	detector->detect(im, keypoints);
//	int x, y;
//	int radius;
//	double diameter;
//	//cvtColor(im, img, COLOR_GRAY2BGR);
//	for (int i = 0; i < keypoints.size(); i++) {
//		KeyPoint k = keypoints[i];
//		Point keyPt;
//		keyPt = k.pt;
//		x = (int)keyPt.x;
//		y = (int)keyPt.y;
//		//Mark center in BLACK
//		circle(img, Point(x, y), 5, Scalar(255, 0, 0), -1);
//		//Get radius of coin
//		diameter = k.size;
//		radius = (int)diameter / 2.0;
//		// Mark blob in Green
//		circle(img, Point(x, y), radius, Scalar(0, 255, 0), 2);
//	}
//	imshow("2", img);
//	//Setup SimpleBlobDetector parameters.
//	//SimpleBlobDetector::Params params;
//	////Change thresholds
//	//params.minThreshold = 10;
//	//params.maxThreshold = 200;
//	////Filter by Area.
//	//params.filterByArea = true;
//	//params.minArea = 50;
//	////Filter by circularity
//	//params.filterByCircularity = true;
//	//params.minCircularity = 0.1;
//	////Filter by Convexity
//	//params.filterByConvexity = true;
//	//params.minConvexity = 0.87;
//	////Filter by Inertia
//	//params.filterByInertia = true;
//	//params.minInertiaRatio = 0.01;
//	//Ptr<SimpleBlobDetector>detector = SimpleBlobDetector::create(params);
//	waitKey(0);
//	destroyAllWindows();
//}