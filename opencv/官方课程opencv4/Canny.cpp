//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//Mat im;
//
//int main(int argc, char** argv) {
//	Mat thr, dstImage;
//	im = imread("D:\\Visual Studio2019(64bit)\\Projects\\Opencv\\face.jpg"); 
//	imshow("face", im);
//	cvtColor(im, thr,COLOR_BGR2GRAY);
//	//threshold(gray, thr, 127, 255, THRESH_BINARY | THRESH_OTSU);//ֻ֧�ֵ�ͨ��
//	//adaptiveThreshold(gray, thr, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 25, 10);//�ֲ�
//	Canny(thr, dstImage, 50, 100, 3); // Canny��Ե���
//	imshow("Canny", dstImage);
//	waitKey(0);
//	destroyAllWindows();
//}