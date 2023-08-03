//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//int main(int argc, char** argv) 
//{
//	Mat image = imread("pic6.png");
//	Mat imageCopy = image.clone();
//	Mat imageGray, imThresh;
//	// Convert to grayscale
//	cvtColor(image, imageGray, COLOR_BGR2GRAY);
//	//threshold(imageGray, imThresh, 90, 255, THRESH_BINARY | THRESH_OTSU);
//	adaptiveThreshold(imageGray, imThresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 35, 5);//¾Ö²¿
//	imshow("image", imThresh);
//	// Find all contours in the image
//	vector <vector<Point> > contours;
//	vector<Vec4i> hierarchy; // ²ã´Î
//	findContours(imThresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
//	cout << "Number of contours found " << contours.size();
//	drawContours(image, contours, -1, Scalar(0, 255, 0));
//	for (size_t i = 0; i < contours.size(); i++)
//	{
//		Moments M = moments(contours[i]);
//		int x = int(M.m10 / double(M.m00));
//		int y = int(M.m01 / double(M.m00));
//		circle(image, Point(x, y), 5, Scalar(0, 0, 255), -1);
//		double area = contourArea(contours[i]);
//		printf("contours index %d, area: %.2f\n", i, area);
//		// rectangle
//		Rect box = boundingRect(contours[i]);
//		rectangle(image, box, Scalar(0, 255, 0), 2, 8);
//	}
//	imshow("image1", image);
//	// Find external contours in the image
//	//findContours(imageGray, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
//	//cout << "Number of contours found =" << contours.size();
//	//image = imageCopy.clone();
//	//// Draw all the contours
//	//drawContours(image, contours, -1, Scalar(0, 255, 0), 3);
//	//imshow("image2", image);
//	imageCopy = image.clone();
//	RotatedRect rotrect;
//	//Point2f rect_points[4];
//	Mat boxPoints2f, boxPointsCov;
//	for (size_t i = 0; i < contours.size(); i++)
//	{
//		//Rotated rectangle
//		rotrect = minAreaRect(contours[i]);
//		boxPoints(rotrect, boxPoints2f);
//		boxPoints2f.assignTo(boxPointsCov, CV_32S);
//		polylines(imageCopy, boxPointsCov, true, Scalar(0, 255, 255), 2);
//	}
//	//imageCopy = image.clone();
//	//Point2f center;
//	//float radius;
//	//for (size_t i = 0; i < contours.size(); i++) {
//	//	// Fit a circle
//	//	minEnclosingCircle(contours[i], center, radius);
//	//	circle(imageCopy, center, radius, Scalar(2, 125, 125), 2);
//	//}
//	imshow("image2", imageCopy);
//	waitKey(0);
//	destroyAllWindows();
//}