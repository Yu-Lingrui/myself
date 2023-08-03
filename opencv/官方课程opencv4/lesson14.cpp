//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//int main(int argc, char** argv) {
//	// Read the image
//	Mat dst, edge;
//	Mat Image = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/face.jpg");
//	// Canny
//	Canny(Image, edge, 100, 300); // 二值化,高斯模糊,计算梯度和方向,极大值抑制,双阈值和边缘连结
//	bitwise_and(Image, Image, dst, edge); // 彩色边缘
//	imshow("Canny", edge);
//	waitKey(0);
//	destroyAllWindows();
//}