//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//// Piecewise Linear interpolation implemented on a particular Channel
//void interpolation(uchar * lut, float* fullRange, float* Curve, float* originalValue)
//{
//	int i;
//	for (i = 0; i < 256; i++)
//	{
//		int j = 0;
//		float a = fullRange[i];
//		while (a > originalValue[j]) {
//			j++;
//		}
//		if (a == originalValue[j])
//		{
//			lut[i] = Curve[j];
//			continue;
//			float slope = ((float)(Curve[j] - Curve[j - 1])) /
//				(originalValue[j] - originalValue[j - 1]);
//			float constant = Curve[j] - slope * originalValue[j];
//			lut[i] = slope * fullRange[i] + constant;
//		}
//	}
//}
//int main(int argc, char** argv) {
//	// Read the image
//	Mat Image = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/face.jpg");
//	// Pivot points for X - Coordinates
//	float originalVa1ue[] = { 0,50,100,150,200,255 };
//	//Changed points on Y - axis for each channel
//	float rCurve[] = { 0,80,150,190,220,255 };
//	float bCurve[] = { 0,20,40,75,150,255 };
//	//Splitting the channels
//	vector<Mat> channels(3);
//	split(Image, channels);
//	//Create a LookUp Table
//	float fullRange[256];
//	int i;
//	for (i = 0; i < 256; i++)
//	{
//		fullRange[i] = (float)i;
//		Mat lookUpTable(1, 256, CV_8U);
//		uchar* lut = lookUpTable.ptr();
//		// Apply interpolation and create look up table
//		interpolation(lut, fullRange, rCurve, originalVa1ue);
//
//		// Apply mapping and check for underflow / overflow in Red Channel
//		LUT(channels[2], lookUpTable, channels[2]);
//		min(channels[2], 255, channels[2]);
//		max(channels[2], 0, channels[2]);
//		//Apply interpolation and create look up table
//		interpolation(lut, fullRange, bCurve, originalVa1ue);
//		// Apply mappirig and check for underflow / overflow in Blue Channel
//		LUT(channels[0], lookUpTable, channels[0]);
//		min(channels[0], 255, channels[0]);
//		max(channels[0], 0, channels[0]);
//	}
//	Mat output;
//	// Merge the channels
//	merge(channels, output);
//
//	// Display the images
//	Mat combined;
//	hconcat(Image, output, combined);
//	imshow("aaa", combined);
//	waitKey(0);
//	destroyAllWindows();
//}
