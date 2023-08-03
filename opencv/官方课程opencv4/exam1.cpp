//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;

//int main(int argc, char** argv)
//{
//	vector<Point> Points;
//	string strDecoded;
//	// 读取人脸图像
//	Mat QR = imread("3.png");
//	// 实例化
//	QRCodeDetector qrcoder;
//	// qr检测并解码
//	strDecoded = qrcoder.detectAndDecode(QR, Points);
//	if (Points.size() == 4)
//	{
//		//画出ROI
//		rectangle(QR, Points[0], Points[2], Scalar(0, 0, 255), 2);
//	}
//	putText(QR, strDecoded, Points[0] + Point(-10, -5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);
//	imshow("QR", QR);
//	imwrite("qr.jpg",QR);
//	//此函数等待按键，按键盘任意键就返回
//	waitKey();
//
//}