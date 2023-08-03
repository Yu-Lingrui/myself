//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//int main(int argc, char** argv)
//{
//	Mat image = imread("face.jpg");
//	imshow("input", image);
//	namedWindow("result", WINDOW_AUTOSIZE);
//	//平移到(25, 25)
//	float warpMatValues[] = { 1.0,0.0,25.0,0.0,1.0,25.0 };// [1 0 25; 0 1 25]
//	Mat warpMat = Mat(2, 3, CV_32F, warpMatValues);
//	//Warp Image
//	Mat result;
//	Size outDim = image.size();
//	warpAffine(image, result, warpMat, outDim);
//	imshow("result", result);
//	waitKey(0);
//
//	//宽度放大两倍
//	float warpMatValues2[] = { 2.0,0.0,0.0,0.0,2.0,0.0 };
//	warpMat = Mat(2, 3, CV_32F, warpMatValues2);
//	warpAffine(image, result, warpMat, Size(outDim.width * 2, outDim.height));
//	imshow("result", result);
//	waitKey(0);
//
//	//旋转图像
//	float angleInRadians = 30;
//	angleInRadians = 30 * 3.14 / 180.0;
//
//	float cosTheta = cos(angleInRadians);
//	float sinTheta = sin(angleInRadians);
//
//	//定义旋转矩阵
//	float warpMatValues4[] = { cosTheta,sinTheta,0.0,-sinTheta,cosTheta,0.0 };
//	warpMat = Mat(2, 3, CV_32F, warpMatValues4);
//
//	//仿射变换
//	warpAffine(image, result, warpMat, outDim);
//	imshow("result", result);
//	waitKey(0);
//
//	float centerX = image.size().width / 2;
//	float centerY = image.size().height / 2;
//
//	float tx = (1 - cosTheta) * centerX - sinTheta * centerY;
//	float ty = sinTheta * centerX + (1 - cosTheta) * centerY;
//
//	float warpMatValues5[] = { cosTheta, sinTheta, tx, -sinTheta, cosTheta, ty };
//	warpMat = Mat(2, 3, CV_32F, warpMatValues5);
//	//基于中心的旋转
//	warpAffine(image, result, warpMat, outDim);
//	imshow("result", result);
//	waitKey(0);
//
//	//错切变换
//	float shearAmount = 0.2;
//	float warpMatValues6[] = { 1, shearAmount, 0, 0, 1.0, 0 };
//	warpMat = Mat(2, 3, CV_32F, warpMatValues6);
//
//
//	//Warp image
//	warpAffine(image, result, warpMat, outDim);
//	imshow("result", result);
//	waitKey(0);
//
//	//Read source image
//	Mat im_src = imread("1.jpg");
//	//Four corners of the book in source image
//	vector<Point2f> pts_src{ Point2f(141,131),Point2f(480,159),Point2f(493,630),Point2f(64,601) };
//	//Read destination image.
//	Mat im_dst = imread("2.jpg");
//	imshow("input2", im_dst);
//	//Four corners of the book in destination image.
//	vector<Point2f> pts_dst{ Point2f(318,256),Point2f(534,372),Point2f(316,670),Point2f(73,473) };
//
//	//Calculate Homography
//	Mat h = findHomography(pts_src, pts_dst);
//
//	//Warp source image to destination based on homography
//	Mat im_out;
//	warpPerspective(im_src, im_out, h, im_dst.size());
//	imshow("result", im_out);
//	waitKey(0);
//
//	destroyAllWindows();
//	return 0;
//}