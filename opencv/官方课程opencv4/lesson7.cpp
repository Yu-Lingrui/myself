//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//Mat im;
//
//int main(int argc, char** argv) {
//	im = imread("LinuxLogo.jpg", IMREAD_GRAYSCALE); // CV_8U
//	imshow("input", im);
//	/****************************************/
//	Mat imThresh;//CV_32S
//	threshold(im, imThresh, 127, 255, THRESH_BINARY);
//	imshow("input1", imThresh);
//	//Find connected components
//	//Convert image to 8 - bits
//	imThresh.convertTo(imThresh, CV_8U);
//
//	Mat imLabels;
//	int nComponents = connectedComponents(imThresh, imLabels);
//	Mat imLabelsCopy = imLabels.clone();//CV_32S
//
//	//First let's find the min and max values in imLabels
//	Point minLoc, maxLoc;
//	double minVal, maxVal;
//	//The following line finds the min and max pixel values
//	// and their locations in an image.
//	minMaxLoc(imLabels, &minVal, &maxVal, &minLoc, &maxLoc);
//	//Normalize the image so the min value is 0 and max value is 255.
//	imLabels = 255 * (imLabels - minVal) / (maxVal - minVal);
//	//Convert image to 8 - bits
//	imLabels.convertTo(imLabels, CV_8U);
//	imshow("input2", imLabels);
//	// Display the labels
//	cout << "Mumber of Components = " << nComponents;
//	for (size_t i = 0; i < nComponents; i++)
//	{
//		imshow("input3",imLabels == i);
//		waitKey(0);
//	}
//	// Make a copy of the image
//	Mat imLabels1;
//	imLabels1 = imLabelsCopy.clone();
//	double minValue, maxValue;
//	minMaxLoc(imLabels1, &minValue, &maxValue, &minLoc, &maxLoc);
//	//Normalize the image so the min value is 0 and max value is 255.
//	imLabels1 = 255 * (imLabels1 - minValue) / (maxValue - minValue);
//	//Convert image to 8 - bits
//	imLabels1.convertTo(imLabels1, CV_8U);
//	//Apply a color map
//	Mat imColorMap;
//	applyColorMap(imLabels1, imColorMap, COLORMAP_JET);
//	imshow("input4", imColorMap);
//	
//	waitKey(0);
//	destroyAllWindows();
//}