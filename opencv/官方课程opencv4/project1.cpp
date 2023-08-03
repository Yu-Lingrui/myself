//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//void sketch_filter(Mat& image);
//void cartoonify(Mat& image);
//
//int main(int argc, char** argv) {
//	//Mat image = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/face.jpg");
//	//namedWindow("input", WINDOW_AUTOSIZE);
//	//imshow("input", image);
//	//sketch_filter(image);
//	//waitKey(0);
//
//	VideoCapture cap(0);
//	Mat frame;
//	while (true)
//	{
//		bool ret = cap.read(frame);
//		imshow("input", frame);
//		if (!ret) break;
//		sketch_filter(frame);
//		char c = waitKey(1);
//		if (c == 27)
//		{
//			break;
//		}
//	}
//	waitKey(0);
//	return 0;
//}
//
//void sketch_filter(Mat &img) {
//	double t1  = getTickCount();
//	//灰度去色
//	Mat gray;
//	cvtColor(img, gray, COLOR_BGR2GRAY);
//	//取反与高斯模糊
//	Mat invert;
//	bitwise_not(gray, invert);
//	GaussianBlur(invert, invert, Size(15, 15), 0);
//	//减淡公式：C = MIN(A + (A×B) / (255 - B), 255), // 去饱和
//	//其中C为混合结果，A为去色后的像素点，B为高斯模糊后的像素点。
//	Mat result(gray.size(), CV_8UC1);
//	for (size_t row = 0; row < gray.rows; row++) {
//		uchar* g_pixel = gray.data + row * gray.step;
//		uchar* in_pixel = invert.data + row * invert.step;
//		uchar* result_row = result.data + row * result.step;
//		for (size_t col = 0; col < gray.cols; col++)
//		{
//			int a = *g_pixel++;
//			int b = *in_pixel++;
//			int c = std::min(a + (a * b) / (256 - b), 255);
//			*result_row++ = c;
//		}
//	}
//	gray.release();
//	invert.release();
//	double t2 = getTickCount();
//	double fps  = getTickFrequency() / (t2 - t1);
//	putText(result, format("current FPS %.2f", fps), Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2, 8);
//	imshow("素描效果", result);
//}
//
////void cartoonify(Mat& image) {
////	Mat blur_tmp;
////	Mat tmp;
////	int kernelSize = 11;
////	double sigmaColor = 9;
////	double sigmaSpace = 7;
////	bilateralFilter(image, tmp, kernelSize, sigmaColor, sigmaSpace);
////	for (int i = 0; i < 14; i++)
////	{
////		bilateralFilter(tmp, blur_tmp, kernelSize, sigmaColor, sigmaSpace);
////		tmp = blur_tmp.clone();
////	}
////	Mat gray;
////	cvtColor(image, gray, COLOR_BGR2GRAY);
////	GaussianBlur(gray, gray, Size(3, 3), 3, 3);
////
////	Mat edge_tmp;
////	const int LAPLACIAN_FILTER_SIZE = 5;
////	Laplacian(gray, edge_tmp, CV_8U, LAPLACIAN_FILTER_SIZE);
////	threshold(edge_tmp, edge_tmp, 127, 255, THRESH_BINARY_INV);
////	for (int row = 0; row < edge_tmp.rows; row++)
////	{
////		for (int col = 0; col < edge_tmp.cols; col++)
////		{
////			if (edge_tmp.at<uchar>(row, col) == 0) {
////				blur_tmp.at<Vec3b>(row, col)[0] = 0;
////				blur_tmp.at<Vec3b>(row, col)[1] = 0;
////				blur_tmp.at<Vec3b>(row, col)[2] = 0;
////			}
////		}
////	}
////
////	//blur_tmp;
////	imshow("卡通", blur_tmp);
////}
