//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//void method_one();
//void method_two();
//
//int main(int argc, char** argv) {
//	// Read the video
//	method_one();
//	return 0;
//}
//
//void method_one() {
//	VideoCapture capture;
//	capture.open("D:/Visual Studio2019(64bit)/Projects/Opencv/test.mp4");
//	if (!capture.isOpened()) {
//		printf("could not load video...\n");
//		return;
//	}
//	int h = capture.get(CAP_PROP_FRAME_HEIGHT);
//	int w = capture.get(CAP_PROP_FRAME_WIDTH);
//	Mat frame, gray;
//	int max = -1;
//	int sum = 0;
//	while (true)
//	{
//		bool ret = capture.read(frame);
//		if (!ret) {
//			break;
//		}
//		imshow("frame", frame);
//		cvtColor(frame, gray, COLOR_BGR2GRAY);
//		for (int row = 1; row <= h - 1; row++) {
//			for (int col = 1; col <= w - 1; col++) {
//				int dx = gray.at<uchar>(row, col) * 2 - gray.at<uchar>(row, col + 1) - gray.at<uchar>(row, col - 1);
//				int dy = gray.at<uchar>(row, col) * 2 - gray.at<uchar>(row + 1, col) - gray.at<uchar>(row - 1, col);
//				sum += (abs(dx) + abs(dy));
//			}
//		}
//		if (max < sum) {
//			max = sum;
//			imwrite("../sharpen.png", frame);
//		}
//		sum = 0;
//
//		char c = waitKey(1);
//		if (c == 27) {
//			break;
//		}
//	}
//	Mat result = imread("../sharpen.png");
//	imshow("最佳清晰度", result);
//	waitKey(0);
//	destroyAllWindows;
//}
//
//void method_two() {
//	VideoCapture capture;
//	capture.open("D:/Visual Studio2019(64bit)/Projects/Opencv/test.mp4");
//	if (!capture.isOpened()) {
//		printf("could not load video...\n");
//		return;
//	}
//	int h = capture.get(CAP_PROP_FRAME_HEIGHT);
//	int w = capture.get(CAP_PROP_FRAME_WIDTH);
//	Mat frame, gray;
//	float dev = 0;
//	float max = -1;
//	Mat temp = Mat::zeros(Size(w, h), CV_32FC1);
//	Mat mean, stddev;
//	while (true)
//	{
//		bool ret = capture.read(frame);
//		if (!ret) {
//			break;
//		}
//		imshow("frame", frame);
//		cvtColor(frame, gray, COLOR_BGR2GRAY);
//		for (int row = 1; row <= h - 1; row++) {
//			for (int col = 1; col <= w - 1; col++) {
//				int dx = gray.at<uchar>(row, col) * 2 - gray.at<uchar>(row, col + 1) - gray.at<uchar>(row, col - 1);
//				int dy = gray.at<uchar>(row, col) * 2 - gray.at<uchar>(row + 1, col) - gray.at<uchar>(row - 1, col);
//				temp.at<float>(row, col) = abs(dx + dy);
//			}
//		}
//		meanStdDev(temp, mean, stddev);
//		dev = stddev.at<double>(0, 0); // gray (0, 0) 存放；
//		printf("stddev %.2f n", dev);
//		if (max < dev) {
//			max = dev;
//			imwrite("../sharpen.png", frame);
//		}
//		char c = waitKey(1);
//		if (c == 27) {
//			break;
//		}
//	}
//	Mat result = imread("../sharpen.png");
//	imshow("最佳清晰度", result);
//	waitKey(0);
//	destroyAllWindows;
//}
