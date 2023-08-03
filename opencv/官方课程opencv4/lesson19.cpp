//#include <iostream>
//#include <opencv2/opencv.hpp>  
//
//
//using namespace cv;
//using namespace std;
//
//int main() {
//	//‘ÿ»ÎÕº∆¨
//	std::vector<Rect> faces;
//	Mat frame = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/face.jpg");
//	Mat frameGray, frameclone;
//	cvtColor(frame, frameGray, COLOR_BGR2GRAY); 
//	int faceNeighborsMax = 5;
//	int neighborStep = 2;
//
//	//»À¡≥ºÏ≤‚
//	for (int neigh = 0; neigh < faceNeighborsMax; neigh = neigh + neighborStep) {
//		frameclone = frame.clone();
//		faceCascade.detectMultiScale(frameGray, faces, 1.2, neigh);
//
//		for (size_t i = 0; i < faces.size(); i++) {
//			int x = faces[i].x;
//			int y = faces[i].y;
//			int w = faces[i].width;
//			int h = faces[i].height;
//
//			rectangle(frameclone, Point(x, y), Point(x + w, y + h), Scalar(255, 0, 0), 2, 4);
//		}
//
//		//ªÊ÷∆œ‘ æ
//		putText(frameclone, format("# Neighbors =%d", neigh), Point(10, 50), FONT_HERSHEY_SIMPLEX, 1,
//			Scalar(0, 6, 255), 4);
//		imshow("Face Detection Demo", frameclone);
//		int k = waitKey(500);
//		if (k == 27) {
//			destroyAllWindows();
//			break;
//		}
//	}
//	return 0;
//}
