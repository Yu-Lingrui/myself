//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//void drawSquare(int action, int x, int y, int flags, void* userdata);
//
//// Points to store the center of the circle and a point on the circumference
//Point topleft, bottomright;
////Source image
//Mat source, dummy;
//
//int main(int argc, char** argv) {
//
//	source = imread("face.jpg",1);
//	dummy = source.clone();//deepcopy
//	namedWindow("frame", WINDOW_AUTOSIZE);
//	//注册后回调
//	setMouseCallback("frame", drawSquare);
//	char k = 0;
//	while (true)
//	{
//		
//		imshow("frame", source);
//		char key = waitKey();
//		if (key == 27) {
//			break;
//		}
//	}
//	destroyAllWindows();
//}
//
////鼠标事件响应回调函数实现
//void drawSquare(int action, int x, int y, int flags, void* userdata)
//{
//	//按下左键
//	if (action == EVENT_LBUTTONDOWN)
//	{
//		topleft = Point(x, y);
//		circle(source, topleft, 1, Scalar(255, 255, 0), 2, LINE_AA);
//	}
//	//抬起左键
//	else if (action == EVENT_LBUTTONUP)
//	{
//		bottomright = Point(x, y);
//		//绘制矩形
//		rectangle(source, topleft, bottomright, Scalar(255, 255, 0), 2, LINE_AA);
//		imwrite("roiface.jpg", dummy(Range(topleft.y, bottomright.y), Range(topleft.x, bottomright.x)));
//		imshow("frame", source);
//	}
//}