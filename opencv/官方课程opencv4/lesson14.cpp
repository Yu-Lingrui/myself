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
//	Canny(Image, edge, 100, 300); // ��ֵ��,��˹ģ��,�����ݶȺͷ���,����ֵ����,˫��ֵ�ͱ�Ե����
//	bitwise_and(Image, Image, dst, edge); // ��ɫ��Ե
//	imshow("Canny", edge);
//	waitKey(0);
//	destroyAllWindows();
//}