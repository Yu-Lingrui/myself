#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void getContours(Mat imgDil, Mat img) {

	vector<vector<Point>> contours; //轮廓数据
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //通过预处理的二值图像找到所有轮廓contours
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2); //绘制所有轮廓

	for (int i = 0; i < contours.size(); i++) 
	{
		double area = contourArea(contours[i]); //计算每个轮廓区域
		cout << area << endl;

		vector<vector<Point>> conPoly(contours.size()); 
		vector<Rect> boundRect(contours.size());
		string objectType;

		if (area > 1000) //过滤噪声
		{
			//找轮廓的近似多边形或曲线
			double peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			
			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]); //找每个近似曲线的最小上边界矩形
			
			int objCor = (int)conPoly[i].size();

			if (objCor == 3) { objectType = "Tri"; }
			if (objCor == 4) { 
				
				float aspRatio = (float)boundRect[i].width / boundRect[i].height; //宽高比
				cout << aspRatio << endl;
				if (aspRatio > 0.95 && aspRatio < 1.05) {
					objectType = "Square";
				}
				else {
					objectType = "Rect";
				}
			}
			if (objCor > 4) { objectType = "CirCle"; }

			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2); //绘制滤除噪声后的所有轮廓
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5); //绘制边界框
			putText(img, objectType, { boundRect[i].x, boundRect[i].y - 5 }, FONT_HERSHEY_PLAIN, 1, Scalar(0, 69, 255), 1);
		}
	} 
}

int main()
{
	string path = "/mnt/d/opencv/Fundamentals/resources/shapes.png";
	Mat img = imread(path);
	Mat imgGray, imgBlur, imgCanny, imgDil;

	// Preprocessing
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 25, 75);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);

	getContours(imgDil, img);

	imshow("Image", img);
	/*imshow("Image Gray", imgGray);
	imshow("Image Blur", imgBlur);
	imshow("Image Canny", imgCanny);
	imshow("Image Dil", imgDil);*/

	waitKey(0);

	return 0;
}
