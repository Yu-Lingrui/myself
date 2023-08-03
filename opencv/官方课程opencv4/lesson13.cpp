//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//int main(int argc, char** argv) {
//	// Read the image
//	Mat dst;
//	Mat Image = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/face.jpg");
//	// Display the images
//	/*Mat k = Mat::ones(Size(11, 11), CV_32FC1);*/
//	/*k /= 121.0;*/
//	//Mat k = Mat::ones(Size(3, 3), CV_32FC1);
//	//k /= 9;
//	/*filter2D(Image, dst, -1, k, Point(-1, -1), 0.0, 4);*/
//	imshow("aaa", Image);
//	//imshow("a", dst);
//	// blur
//	/*int ks3x3 = 3;
//	int ks7x7 = 7;
//	Mat dst1, dst2;
//	blur(Image, dst1, Size(ks3x3, ks3x3));
//	blur(Image, dst2, Size(ks7x7, ks7x7));
//	imshow("b", dst1);
//	imshow("bb", dst2);*/
//	// gaussian blur
//	Mat dst3;
//	//GaussianBlur(Image, dst3, Size(5, 5), 0, 0, 4);
//	//GaussianBlur(Image, dst3, Size(0, 0), 15, 0, 4); // sigma：Size为0才起作用
//	//medianBlur(Image, dst3, 5); // 去除椒盐噪声
//	// 双边模糊
//	//bilateralFilter(Image, dst3, 15, 80, 80);// 去除高斯噪声
//	// imshow("bbb", dst3);
//	//Grad 1
//	Mat sobel_x, sobel_y;
//	/*Sobel(Image, sobel_x, CV_32F, 1, 0, 3);
//	Sobel(Image, sobel_y, CV_32F, 0, 1, 3);
//	normalize(sobel_x, sobel_x, 0, 1, NORM_MINMAX);
//	normalize(sobel_y, sobel_y, 0, 1, NORM_MINMAX);*/
//	//Grad 2
//	//Laplacian(Image, dst3, CV_32F, 3);
//	//normalize(dst3, dst3, 0, 1, NORM_MINMAX);
//	Laplacian(Image, dst3, CV_32F, 1);
//	dst3.convertTo(dst3, CV_8UC3);
//	GaussianBlur(Image, Image, Size(3, 3), 0);
//	//add(Image, dst3, dst3); // Laplacian + GaussianBlur = 锐化
//	subtract(Image, dst3, dst3); // Laplacian - GaussianBlur = 锐化
//	Mat sharpen = (Mat_<int>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0); // 锐化卷积
//	Mat sharpenOutput;
//	filter2D(Image, sharpenOutput, -1, sharpen);
//	imshow("USM", dst3);
//	imshow("d", sharpenOutput);
//	/*imshow("c", sobel_x);
//	imshow("cc", sobel_y);*/
//	waitKey(0);
//	destroyAllWindows();
//}
