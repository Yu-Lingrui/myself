//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//void eqhst_demo();
//Mat img;
//
//int main(int argc, char** argv) {
//	eqhst_demo();
//}
//
//void eqhst_demo()
//{
//	img = imread("face.jpg");
//	//imshow("1", img);
//	/***********************************************/
//	//Convert to HSV color space
//	Mat hsv;
//	cvtColor(img, hsv, COLOR_BGR2HSV);
//	//hsv.convertTo(hsv, CV_32FC3);
//	//Split the channels
//	vector<Mat> hsvChannels;//Mat hsvChannels[3];
//	vector<Mat> chsv;//adpat;
//	vector<Mat> rgb;
//	split(hsv, hsvChannels);
//	split(hsv, chsv);
//	split(img, rgb);
//	// --CLAEH--
//	auto claeh = createCLAHE(2.0, Size(8, 8));
//	claeh->apply(chsv[2], chsv[2]);
//	for (size_t i = 0; i < rgb.size(); i++)
//	{
//		equalizeHist(rgb[i], rgb[i]);
//	}
//	equalizeHist(hsvChannels[2], hsvChannels[2]);
//	Mat dst_v;
//	Mat dst;
//	Mat dstc;
//	merge(hsvChannels, hsv);
//	merge(rgb, dst);
//	merge(chsv, dstc);
//	cvtColor(hsv, dst_v, COLOR_HSV2BGR);
//	cvtColor(dstc, dstc, COLOR_HSV2BGR);
//	imshow("hsv_v", dst_v);
//	imshow("hsv_c", dstc);
//	imshow("rgb", dst);
//	waitKey(0);
//	destroyAllWindows();
//}